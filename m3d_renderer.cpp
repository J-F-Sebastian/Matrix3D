#include <algorithm>
#include <iostream>
#include <sstream>
#include <cfloat>

#include "m3d_renderer.hh"
#include "m3d_interp.hh"

using namespace std;

static inline float m3d_max(float a, float b)
{
    return (a > b) ? a : b;
}

static inline m3d_color m3d_average_light(struct m3d_renderer::m3d_renderer_data n[])
{
    m3d_color temp[3] = {n[0].Kdiff, n[1].Kdiff, n[2].Kdiff};
    m3d_color out;
    m3d_color::average_colors(temp, 3, out);
    return out;
}

m3d_renderer::~m3d_renderer()
{
    delete scanline;
    delete fscanline;
    delete zscanline;
}

m3d_renderer::m3d_renderer(m3d_display *disp) : display(disp), zbuffer((int16_t)disp->get_xmax(), (int16_t)disp->get_ymax())
{
    scanline = new int16_t[display->get_ymax() * 2];
    fscanline = new float[display->get_ymax() * 2];
    zscanline = new float[display->get_ymax() * 2];
}

/*
 * Default renderer is void
 */
void m3d_renderer::render(m3d_world & /*world*/)
{
    // Fill the surface black
    display->clear_renderer();
    // Update the surface
    display->show_buffer();
}

/*
 * Sort triangle vertices and attributes by y coordinate
 */
void m3d_renderer::sort_triangle(struct m3d_renderer_data trianglep[])
{
    if (trianglep[1].toscreen.y < trianglep[0].toscreen.y)
    {
        std::swap(trianglep[0], trianglep[1]);
    }

    if (trianglep[1].toscreen.y > trianglep[2].toscreen.y)
    {
        std::swap(trianglep[1], trianglep[2]);
    }

    if (trianglep[1].toscreen.y < trianglep[0].toscreen.y)
    {
        std::swap(trianglep[0], trianglep[1]);
    }
}

/*
 * run-length scanline computation
 * This is the same algorithm for run-length lines, but will collect endpoint
 * values only.
 * Some keynotes: y0 is always lesser than or equal to y1.
 * Thanks to Michael Abrash for his invaluable books....
 */
int16_t m3d_renderer::store_scanlines(int16_t x0,
                                      int16_t y0,
                                      int16_t x1,
                                      int16_t y1,
                                      unsigned start)
{
    int16_t advancex, deltax, deltay;
    int16_t wholestep, adjup, adjdown;
    int16_t errorterm, inipixcount, finpixcount;
    int16_t runlen;
    uint32_t cnt, fill;
    int16_t val = x0;
    int16_t *stor = scanline + start;

    if (x0 > x1)
    {
        deltax = x0 - x1;
        advancex = -1;
    }
    else
    {
        deltax = x1 - x0;
        advancex = 1;
    }

    deltay = y1 - y0;

    if (!deltay)
    {
        /* horizontal line, one value only */
        *stor++ = val;
        return 1;
    }

    if (!deltax)
    {
        /* vertical line, all x values are the same... */
        for (cnt = 0; cnt <= (uint32_t)deltay; cnt++)
        {
            *stor++ = val;
        }
        return (deltay + 1);
    }

    if (deltax == deltay)
    {
        /* diagonal line, x advances at every step with the same value */
        *stor++ = val;
        for (cnt = 0; cnt < (uint32_t)deltay; cnt++)
        {
            val += advancex;
            *stor++ = val;
        }
        return (deltay + 1);
    }

    if (deltax < deltay)
    {
        /* Y major line */

        /* minimum # of pixels in a run */
        wholestep = deltay / deltax;

        adjup = (deltay % deltax) * 2;
        adjdown = deltax * 2;
        errorterm = (deltay % deltax) - adjdown;

        inipixcount = (wholestep / 2) + 1;
        finpixcount = inipixcount;

        if (!adjup && !(wholestep & 1))
        {
            inipixcount--;
        }

        if (wholestep & 1)
        {
            errorterm += deltax;
        }

        /* set first run of x0 */
        for (fill = 0; fill < (uint32_t)inipixcount; fill++)
        {
            *stor++ = val;
        }
        val += advancex;

        /* set all full runs, x0 is advanced at every outer loop */
        for (cnt = 0; cnt < (uint32_t)(deltax - 1); cnt++)
        {
            runlen = wholestep;
            errorterm += adjup;

            if (errorterm > 0)
            {
                runlen++;
                errorterm -= adjdown;
            }

            for (fill = 0; fill < (uint32_t)runlen; fill++)
            {
                *stor++ = val;
            }
            val += advancex;
        }

        /* set the final run of pixels, x0 does not need to be updated */
        for (fill = 0; fill < (uint32_t)finpixcount; fill++)
        {
            *stor++ = val;
        }
    }
    else
    {
        /* X major */

        /* minimum # of pixels in a run */
        wholestep = deltax / deltay;

        adjup = (deltax % deltay) * 2;
        adjdown = deltay * 2;
        errorterm = (deltax % deltay) - adjdown;

        inipixcount = (wholestep / 2) + 1;
        finpixcount = inipixcount;

        if (!adjup && !(wholestep & 1))
        {
            inipixcount--;
        }

        if (wholestep & 1)
        {
            errorterm += deltay;
        }

        /*
         * When X is major axis, we should draw lines along x, which means,
         * we have multiple x values with the same y value.
         * If advancex is negative, we are computing and ending edge, i.e. we
         * need to store the final x0 values, not the initial, or the line
         * drawing algorithm will fail filling in the line pixels.
         */
        if (advancex > 0)
        {
            *stor++ = val;
            val += inipixcount;

            /* set all full runs */
            for (cnt = 0; cnt < (uint32_t)(deltay - 1); cnt++)
            {
                runlen = wholestep;
                errorterm += adjup;

                if (errorterm > 0)
                {
                    runlen++;
                    errorterm -= adjdown;
                }
                *stor++ = val;
                val += runlen;
            }

            /* set the final run of pixels */
            *stor = val;
        }
        else
        {
            *stor++ = val;
            val -= inipixcount;

            /* set all full runs */
            for (cnt = 0; cnt < (uint32_t)(deltay - 1); cnt++)
            {
                runlen = wholestep;
                errorterm += adjup;

                if (errorterm > 0)
                {
                    runlen++;
                    errorterm -= adjdown;
                }
                *stor++ = val;
                val -= runlen;
            }

            /* set the final run of pixels */
            *stor = val;
        }
    }
    return (deltay + 1);
}

void m3d_renderer::store_fscanlines(unsigned runlen, float val1, float val2, unsigned start)
{
    m3d_interpolation_float run(runlen, val1, val2);
    run.valuearray(fscanline + start);
}

void m3d_renderer::store_zscanlines(unsigned runlen, float val1, float val2, unsigned start)
{
    m3d_interpolation_float run(runlen, val1, val2);
    run.valuearray(zscanline + start);
}

// TBD: consider all lights as white RGB(255,255,255) to reduce computation
void m3d_renderer::diffuse_lighting(struct m3d_renderer_data &vtx, m3d_render_object &obj, m3d_world &world)
{
    // Ambient light does not depend on position anyway...
    float lightint = world.ambient_light.get_intensity(vtx.vertex);
    // Now we sum all diffuse contributions considering light position w.r.t. vtx position and the surface normal.
    for (auto lights : world.lights_list)
    {
        m3d_vector L(lights->get_position());
        L.subtract(vtx.vertex);
        L.normalize();
        float dot = L.dot_product(vtx.normal);
        lightint += lights->get_intensity(vtx.vertex) * m3d_max(dot, 0);
    }
    vtx.diffint = lightint;
    vtx.Kdiff = obj.color;
    vtx.Kdiff.brighten(lightint);
}

void m3d_renderer::specular_lighting(struct m3d_renderer_data &vtx, m3d_render_object &obj, m3d_world &world)
{
}

void m3d_renderer::compute_visible_list_and_sort(m3d_world &world)
{
    vislist.clear();

    for (auto itro : world.objects_list)
    {
        itro->compute_visibility(world.camera);
        if (itro->triangle_visible.any())
            vislist.push_front(itro);
    }

    if (vislist.size())
    {
        world.sort(vislist);
        zbuffer.reset();
    }
}

/*
 * WIREFRAME RENDERING
 */

void m3d_renderer_wireframe::render(m3d_world &world)
{
    m3d_point temp;
    m3d_color ctemp;
    SDL_Point toscreen[M3D_MAX_TRIANGLES * 3];
    unsigned i, j, k;

    // Compute visible objects
    compute_visible_list_and_sort(world);

    // Fill the surface black
    display->clear_renderer();
    for (auto itro : vislist)
    {
        ctemp = itro->color;
        display->set_color(ctemp.getChannel(m3d_color::R_CHANNEL),
                           ctemp.getChannel(m3d_color::G_CHANNEL),
                           ctemp.getChannel(m3d_color::B_CHANNEL));

        i = k = 0;
        for (auto &triangle : itro->mesh)
        {
            if (itro->triangle_visible[i++])
            {
                for (j = 0; j < 3; j++)
                {
                    temp = itro->vertices[triangle.index[j]].position;
                    temp.add(itro->center);
                    world.camera.transform_and_project_to_screen(temp, temp, toscreen[k++]);
                }
                toscreen[k] = toscreen[k - 3];
                k++;
            }
        }
        display->draw_lines(toscreen, k);
    }

    // Present the rendered lines
    display->show_renderer();
}

/*
 * FLAT RENDERING
 */
void m3d_renderer_flat::render(m3d_world &world)
{
    unsigned i, j;
    m3d_point L;
    struct m3d_renderer_data vtx[3];
    int16_t runlen[3];

    // Compute visible objects
    compute_visible_list_and_sort(world);

    // Fill the surface black
    display->clear_buffer();

    for (auto itro : vislist)
    {
        i = 0;
        for (auto &triangle : itro->mesh)
        {
            if (itro->triangle_visible[i++])
            {
                for (j = 0; j < 3; j++)
                {
                    vtx[j].vertex = itro->vertices[triangle.index[j]].position;
                    vtx[j].vertex.add(itro->center);
                    vtx[j].normal = itro->vertices[triangle.index[j]].normal;
                    diffuse_lighting(vtx[j], *itro, world);
                    world.camera.transform_and_project_to_screen(vtx[j].vertex, vtx[j].vertex, vtx[j].toscreen);
                }

                sort_triangle(vtx);

                vtx[0].Kdiff = m3d_average_light(vtx);

                /*
                 * Store x in this order: A to C, A to B, B to C
                 *
                 *           A
                 *
                 *       B
                 *
                 *
                 *              C
                 */

                // runlen[0] = runlen[1] + runlen[2] - 1
                // due to the overlapping value when computing runlen 1 and 2
                runlen[0] = store_scanlines(vtx[0].toscreen.x, vtx[0].toscreen.y, vtx[2].toscreen.x, vtx[2].toscreen.y);
                runlen[1] = store_scanlines(vtx[0].toscreen.x, vtx[0].toscreen.y, vtx[1].toscreen.x, vtx[1].toscreen.y, runlen[0]);
                runlen[2] = store_scanlines(vtx[1].toscreen.x, vtx[1].toscreen.y, vtx[2].toscreen.x, vtx[2].toscreen.y, runlen[0] + runlen[1] - 1);

                triangle_fill_flat(vtx, runlen);
            }
        }
    }

    // Present the rendered lines
    display->show_buffer();
}

void m3d_renderer_flat::triangle_fill_flat(struct m3d_renderer_data vtx[], int16_t *runlen)
{
    uint32_t *output;
    float *outz;
    int16_t y = (int16_t)vtx[0].toscreen.y;
    int steps;
    int16_t *leftx, *rightx;
    m3d_interpolation_float leftz(runlen[1]), rightz(runlen[1]);
    m3d_interpolation_float leftz2(runlen[2]), rightz2(runlen[2]);

    /*
     * check x values to understand who's the left half and who's the right.
     * First run length is ALWAYS the longest run.
     */
    if (scanline[runlen[0] / 2] <= scanline[runlen[0] + runlen[1] - 1])
    {
        leftx = scanline;
        rightx = scanline + runlen[0];
        leftz.init(runlen[1], vtx[0].vertex.myvector[Z_C], vtx[0].vertex.myvector[Z_C] + (vtx[2].vertex.myvector[Z_C] - vtx[0].vertex.myvector[Z_C]) / 2);
        rightz.init(runlen[1], vtx[0].vertex.myvector[Z_C], vtx[1].vertex.myvector[Z_C]);
        leftz2.init(runlen[2], vtx[0].vertex.myvector[Z_C] + (vtx[2].vertex.myvector[Z_C] - vtx[0].vertex.myvector[Z_C]) / 2, vtx[2].vertex.myvector[Z_C]);
        rightz2.init(runlen[2], vtx[1].vertex.myvector[Z_C], vtx[2].vertex.myvector[Z_C]);
    }
    else
    {
        leftx = scanline + runlen[0];
        rightx = scanline;
        leftz.init(runlen[1], vtx[0].vertex.myvector[Z_C], vtx[1].vertex.myvector[Z_C]);
        rightz.init(runlen[1], vtx[0].vertex.myvector[Z_C], vtx[0].vertex.myvector[Z_C] + (vtx[2].vertex.myvector[Z_C] - vtx[0].vertex.myvector[Z_C]) / 2);
        leftz2.init(runlen[2], vtx[1].vertex.myvector[Z_C], vtx[2].vertex.myvector[Z_C]);
        rightz2.init(runlen[2], vtx[0].vertex.myvector[Z_C] + (vtx[2].vertex.myvector[Z_C] - vtx[0].vertex.myvector[Z_C]) / 2, vtx[2].vertex.myvector[Z_C]);
    }

    while (runlen[1]--)
    {
        steps = *rightx - *leftx + 1;
        m3d_interpolation_float sl(steps, leftz.value(), rightz.value());
        output = display->get_video_buffer(*leftx, y);
        outz = zbuffer.get_zbuffer(*leftx, y);
        while (sl.finished() == false)
        {
            if (zbuffer.test_update(outz, sl.value()))
            {
                *output = vtx[0].Kdiff.getColor();
            }
            ++output;
            ++outz;
            sl.step();
        }
        leftx++;
        rightx++;
        leftz.step();
        rightz.step();
        y++;
    }

    // Skip first run, it overlaps
    leftz2.step();
    rightz2.step();
    runlen[2]--;

    while (runlen[2]--)
    {
        steps = *rightx - *leftx + 1;
        m3d_interpolation_float sl(steps, leftz2.value(), rightz2.value());
        output = display->get_video_buffer(*leftx, y);
        outz = zbuffer.get_zbuffer(*leftx, y);
        while (sl.finished() == false)
        {
            if (zbuffer.test_update(outz, sl.value()))
            {
                *output = vtx[0].Kdiff.getColor();
            }
            ++output;
            ++outz;
            sl.step();
        }
        leftx++;
        rightx++;
        leftz2.step();
        rightz2.step();
        y++;
    }
}

/*
 * FLAT RENDERING WITH FLOATING POINT IMPLEMENTATION
 */

void m3d_renderer_flatf::render(m3d_world &world)
{
    unsigned i, j;
    m3d_point L;
    struct m3d_renderer_data vtx[3];

    compute_visible_list_and_sort(world);

    // Fill the surface black
    display->clear_buffer();

    for (auto itro : vislist)
    {
        i = 0;
        for (auto &triangle : itro->mesh)
        {
            if (itro->triangle_visible[i++])
            {
                for (j = 0; j < 3; j++)
                {
                    vtx[j].vertex = itro->vertices[triangle.index[j]].position;
                    vtx[j].vertex.add(itro->center);
                    vtx[j].normal = itro->vertices[triangle.index[j]].normal;
                    diffuse_lighting(vtx[j], *itro, world);
                    world.camera.transform_and_project_to_screen(vtx[j].vertex, vtx[j].vertex, vtx[j].toscreen);
                }

                sort_triangle(vtx);

                vtx[0].Kdiff = m3d_average_light(vtx);

                triangle_fill_flat(vtx);
            }
        }
    }

    // Present the rendered lines
    display->show_buffer();
}

// If this thing prove to be faster, refactor and remove integer scanlines
void m3d_renderer_flatf::triangle_fill_flat(struct m3d_renderer_data vtx[])
{
    uint32_t *output;
    float *outz;
    float p0 = vtx[0].vertex.myvector[Z_C];
    float p1 = vtx[1].vertex.myvector[Z_C];
    float p2 = vtx[2].vertex.myvector[Z_C];
    float p3 = (float)vtx[0].toscreen.x + 0.5f;
    float p4 = (float)vtx[1].toscreen.x + 0.5f;
    float p5 = (float)vtx[2].toscreen.x + 0.5f;
    unsigned runlen0 = vtx[2].toscreen.y - vtx[0].toscreen.y + 1;
    unsigned runlen1 = vtx[1].toscreen.y - vtx[0].toscreen.y + 1;
    unsigned runlen2 = vtx[2].toscreen.y - vtx[1].toscreen.y + 1;
    int fillrunlen;
    int16_t y = (int16_t)vtx[0].toscreen.y;
    float *lscanline, *rscanline;
    float *lzscanline, *rzscanline;
    float lgradient = (p5 - p3) / (float)runlen0;
    float rgradient = (p4 - p3) / (float)runlen1;

    /*
     * check x values to understand who's the left half and who's the right
     */
    /*
     * Draws a horizontal line from first half of points to second half.
     * If lgradient is less than or equal to rgradient then the left side
     * is longest.
     */
    if (lgradient <= rgradient)
    {
        store_fscanlines(runlen0, p3, p5);
        store_fscanlines(runlen1, p3, p4, runlen0);
        store_fscanlines(runlen2, p4, p5, runlen0 + runlen1 - 1);
        store_zscanlines(runlen0, p0, p2);
        store_zscanlines(runlen1, p0, p1, runlen0);
        store_zscanlines(runlen2, p1, p2, runlen0 + runlen1 - 1);
    }
    else
    {
        store_fscanlines(runlen1, p3, p4);
        store_fscanlines(runlen2, p4, p5, runlen1 - 1);
        store_fscanlines(runlen0, p3, p5, runlen1 + runlen2 - 1);
        store_zscanlines(runlen1, p0, p1);
        store_zscanlines(runlen2, p1, p2, runlen1 - 1);
        store_zscanlines(runlen0, p0, p2, runlen1 + runlen2 - 1);
    }

    lscanline = fscanline;
    rscanline = fscanline + runlen0;
    lzscanline = zscanline;
    rzscanline = zscanline + runlen0;

    while (runlen0--)
    {
        fillrunlen = lroundf(*rscanline - *lscanline) + 1;
        m3d_interpolation_float sl(fillrunlen, *lzscanline, *rzscanline);
        output = display->get_video_buffer((int16_t)truncf(*lscanline), y);
        outz = zbuffer.get_zbuffer((int16_t)truncf(*lscanline), y);
        while (sl.finished() == false)
        {
            if (zbuffer.test_update(outz, sl.value()))
            {
                *output = vtx[0].Kdiff.getColor();
            }
            ++output;
            ++outz;
            sl.step();
        }
        lscanline++;
        rscanline++;
        lzscanline++;
        rzscanline++;
        y++;
    }
}

/*
 * SHADED RENDERER, WITH 1/z INTERPOLATION
 */

void m3d_renderer_shaded::render(m3d_world &world)
{
    unsigned runlen[3];
    unsigned i, j;
    m3d_point L;
    struct m3d_renderer_data vtx[3];

    compute_visible_list_and_sort(world);

    // Fill the surface black
    display->clear_buffer();

    for (auto itro : vislist)
    {
        i = 0;
        for (auto &triangle : itro->mesh)
        {
            if (itro->triangle_visible[i++])
            {
                for (j = 0; j < 3; j++)
                {
                    vtx[j].vertex = itro->vertices[triangle.index[j]].position;
                    vtx[j].vertex.add(itro->center);
                    vtx[j].normal = itro->vertices[triangle.index[j]].normal;
                    diffuse_lighting(vtx[j], *itro, world);
                    world.camera.transform_and_project_to_screen(vtx[j].vertex, vtx[j].vertex, vtx[j].toscreen);
                }

                sort_triangle(vtx);

                triangle_fill_shaded(vtx, world);
            }
        }
    }

    // Present the rendered lines
    display->show_buffer();
}

void m3d_renderer_shaded::store_iscanlines(unsigned runlen, float z1, float z2, float val1, float val2, unsigned start)
{
    m3d_interpolation_float_perspective run(runlen, val1, val2, z1, z2);
    run.valuearray(iscanline + start);
}

void m3d_renderer_shaded::triangle_fill_shaded(struct m3d_renderer_data vtx[], m3d_world &world)
{
    uint32_t *output;
    float *outz;
    float p0 = vtx[0].vertex.myvector[Z_C];
    float p1 = vtx[1].vertex.myvector[Z_C];
    float p2 = vtx[2].vertex.myvector[Z_C];
    float p3 = (float)vtx[0].toscreen.x + 0.5f;
    float p4 = (float)vtx[1].toscreen.x + 0.5f;
    float p5 = (float)vtx[2].toscreen.x + 0.5f;
    float i0 = vtx[0].diffint;
    float i1 = vtx[1].diffint;
    float i2 = vtx[2].diffint;
    unsigned runlen0 = vtx[2].toscreen.y - vtx[0].toscreen.y + 1;
    unsigned runlen1 = vtx[1].toscreen.y - vtx[0].toscreen.y + 1;
    unsigned runlen2 = vtx[2].toscreen.y - vtx[1].toscreen.y + 1;
    int fillrunlen;
    int16_t y = (int16_t)vtx[0].toscreen.y;
    float *lscanline, *rscanline;
    float *lzscanline, *rzscanline;
    float *liscanline, *riscanline;
    float lgradient = (p5 - p3) / (float)runlen0;
    float rgradient = (p4 - p3) / (float)runlen1;

    /*
     * check x values to understand who's the left half and who's the right
     */
    /*
     * Draws a horizontal line from first half of points to second half.
     * If lgradient is less than or equal to rgradient then the left side
     * is longest.
     */
    if (lgradient <= rgradient)
    {
        store_fscanlines(runlen0, p3, p5);
        store_fscanlines(runlen1, p3, p4, runlen0);
        store_fscanlines(runlen2, p4, p5, runlen0 + runlen1 - 1);
        store_zscanlines(runlen0, p0, p2);
        store_zscanlines(runlen1, p0, p1, runlen0);
        store_zscanlines(runlen2, p1, p2, runlen0 + runlen1 - 1);
        store_iscanlines(runlen0, p0, p2, i0, i2);
        store_iscanlines(runlen1, p0, p1, i0, i1, runlen0);
        store_iscanlines(runlen2, p1, p2, i0, i2, runlen0 + runlen1 - 1);
    }
    else
    {
        store_fscanlines(runlen1, p3, p4);
        store_fscanlines(runlen2, p4, p5, runlen1 - 1);
        store_fscanlines(runlen0, p3, p5, runlen1 + runlen2 - 1);
        store_zscanlines(runlen1, p0, p1);
        store_zscanlines(runlen2, p1, p2, runlen1 - 1);
        store_zscanlines(runlen0, p0, p2, runlen1 + runlen2 - 1);
        store_iscanlines(runlen1, p0, p1, i0, i1);
        store_iscanlines(runlen2, p1, p2, i1, i2, runlen1 - 1);
        store_iscanlines(runlen0, p0, p2, i0, i2, runlen1 + runlen2 - 1);
    }

    lscanline = fscanline;
    rscanline = fscanline + runlen0;
    lzscanline = zscanline;
    rzscanline = zscanline + runlen0;
    liscanline = iscanline;
    riscanline = iscanline + runlen0;

    while (runlen0--)
    {
        fillrunlen = lroundf(*rscanline - *lscanline) + 1;
        if (fillrunlen)
        {
            m3d_interpolation_float_perspective sl(fillrunlen, *lzscanline, *rzscanline, *liscanline, *riscanline);
            output = display->get_video_buffer((int16_t)truncf(*lscanline), y);
            outz = zbuffer.get_zbuffer((int16_t)truncf(*lscanline), y);
            while (sl.finished() == false)
            {
                if (zbuffer.test_update(outz, sl.value()))
                {
                    *output = vtx[0].Kdiff.brighten2(sl.value());
                }
                ++output;
                ++outz;
                sl.step();
            }
        }
        lscanline++;
        rscanline++;
        lzscanline++;
        rzscanline++;
        liscanline++;
        riscanline++;
        y++;
    }
}

/*
 * GOURAUD SHADING RENDERER
 */

void m3d_renderer_shaded_gouraud::store_iscanlines(unsigned runlen, m3d_color &val1, m3d_color &val2, unsigned start)
{
    m3d_interpolation_color run(runlen, val1, val2);
    run.valuearray(iscanline + start);
}

void m3d_renderer_shaded_gouraud::triangle_fill_shaded(struct m3d_renderer_data vtx[], m3d_world &world)
{
    uint32_t *output;
    float *outz;
    float p0 = vtx[0].vertex.myvector[Z_C];
    float p1 = vtx[1].vertex.myvector[Z_C];
    float p2 = vtx[2].vertex.myvector[Z_C];
    float p3 = (float)vtx[0].toscreen.x + 0.5f;
    float p4 = (float)vtx[1].toscreen.x + 0.5f;
    float p5 = (float)vtx[2].toscreen.x + 0.5f;
    unsigned runlen0 = vtx[2].toscreen.y - vtx[0].toscreen.y + 1;
    unsigned runlen1 = vtx[1].toscreen.y - vtx[0].toscreen.y + 1;
    unsigned runlen2 = vtx[2].toscreen.y - vtx[1].toscreen.y + 1;
    int fillrunlen;
    int16_t y = (int16_t)vtx[0].toscreen.y;
    float *lscanline, *rscanline;
    float *lzscanline, *rzscanline;
    uint32_t *liscanline, *riscanline;
    float lgradient = (p5 - p3) / (float)runlen0;
    float rgradient = (p4 - p3) / (float)runlen1;

    /*
     * check x values to understand who's the left half and who's the right
     */
    /*
     * Draws a horizontal line from first half of points to second half.
     * If lgradient is less than or equal to rgradient then the left side
     * is longest.
     */
    if (lgradient <= rgradient)
    {
        store_fscanlines(runlen0, p3, p5);
        store_fscanlines(runlen1, p3, p4, runlen0);
        store_fscanlines(runlen2, p4, p5, runlen0 + runlen1 - 1);
        store_zscanlines(runlen0, p0, p2);
        store_zscanlines(runlen1, p0, p1, runlen0);
        store_zscanlines(runlen2, p1, p2, runlen0 + runlen1 - 1);
        store_iscanlines(runlen0, vtx[0].Kdiff, vtx[2].Kdiff);
        store_iscanlines(runlen1, vtx[0].Kdiff, vtx[1].Kdiff, runlen0);
        store_iscanlines(runlen2, vtx[1].Kdiff, vtx[2].Kdiff, runlen0 + runlen1 - 1);
    }
    else
    {
        store_fscanlines(runlen1, p3, p4);
        store_fscanlines(runlen2, p4, p5, runlen1 - 1);
        store_fscanlines(runlen0, p3, p5, runlen1 + runlen2 - 1);
        store_zscanlines(runlen1, p0, p1);
        store_zscanlines(runlen2, p1, p2, runlen1 - 1);
        store_zscanlines(runlen0, p0, p2, runlen1 + runlen2 - 1);
        store_iscanlines(runlen1, vtx[0].Kdiff, vtx[1].Kdiff);
        store_iscanlines(runlen2, vtx[1].Kdiff, vtx[2].Kdiff, runlen1 - 1);
        store_iscanlines(runlen0, vtx[0].Kdiff, vtx[2].Kdiff, runlen1 + runlen2 - 1);
    }

    lscanline = fscanline;
    rscanline = fscanline + runlen0;
    lzscanline = zscanline;
    rzscanline = zscanline + runlen0;
    liscanline = iscanline;
    riscanline = iscanline + runlen0;

    while (runlen0--)
    {
        fillrunlen = lroundf(*rscanline - *lscanline) + 1;
        if (fillrunlen)
        {
            m3d_interpolation_float sl(fillrunlen, *lzscanline, *rzscanline);
            m3d_color ca(*liscanline), cb(*riscanline);
            m3d_interpolation_color lint(fillrunlen, ca, cb);
            output = display->get_video_buffer((int16_t)truncf(*lscanline), y);
            outz = zbuffer.get_zbuffer((int16_t)truncf(*lscanline), y);
            while (sl.finished() == false)
            {
                if (zbuffer.test_update(outz, sl.value()))
                {
                    *output = lint.value();
                }
                ++output;
                ++outz;
                sl.step();
                lint.step();
            }
        }
        lscanline++;
        rscanline++;
        lzscanline++;
        rzscanline++;
        liscanline++;
        riscanline++;
        y++;
    }
}

/*
 * PHONG SHADING RENDERER
 */

void m3d_renderer_shaded_phong::triangle_fill_shaded(struct m3d_renderer_data vtx[],
                                                     m3d_world &world)
{
#if 0
    unsigned a = 1;
    int16_t *ptscursora, *ptscursorb;
    uint32_t *output, *outputend;
    int16_t *outz;
    struct m3d_renderer_data temp;

    m3d_reciprocal_z_interpv_step nrun0(runlen[0],
                                        vtx[0].vertex.myvector[Z_C],
                                        vtx[2].vertex.myvector[Z_C],
                                        vtx[0].normal,
                                        vtx[2].normal);
    m3d_reciprocal_z_interpv_step nrun1(runlen[1],
                                        vtx[0].vertex.myvector[Z_C],
                                        vtx[1].vertex.myvector[Z_C],
                                        vtx[0].normal,
                                        vtx[1].normal);
    m3d_reciprocal_z_interpv_step nrun2(runlen[2],
                                        vtx[1].vertex.myvector[Z_C],
                                        vtx[2].vertex.myvector[Z_C],
                                        vtx[1].normal,
                                        vtx[2].normal);
    m3d_reciprocal_z_interpv_step vrun0(runlen[0],
                                        vtx[0].vertex.myvector[Z_C],
                                        vtx[2].vertex.myvector[Z_C],
                                        vtx[0].vertex,
                                        vtx[2].vertex);
    m3d_reciprocal_z_interpv_step vrun1(runlen[1],
                                        vtx[0].vertex.myvector[Z_C],
                                        vtx[1].vertex.myvector[Z_C],
                                        vtx[0].vertex,
                                        vtx[1].vertex);
    m3d_reciprocal_z_interpv_step vrun2(runlen[2],
                                        vtx[1].vertex.myvector[Z_C],
                                        vtx[2].vertex.myvector[Z_C],
                                        vtx[1].vertex,
                                        vtx[2].vertex);

    m3d_reciprocal_z_interpv_step *nleft, *nright, *vleft, *vright;
    int16_t y = (int16_t)vtx[0].toscreen.y;

    /*
     * check x values to understand who's the left half and who's the right
     */
    /* Draws a horizontal line from first half of points to second half */
    if (scanline[runlen[0] / 2] <= scanline[runlen[0] + runlen[0] / 2])
    {
        ptscursora = scanline;
        ptscursorb = scanline + runlen[0];
        nleft = &nrun0;
        nright = &nrun1;
        vleft = &vrun0;
        vright = &vrun1;
    }
    else
    {
        ptscursora = scanline + runlen[0];
        ptscursorb = scanline;
        nleft = &nrun1;
        nright = &nrun0;
        vleft = &vrun1;
        vright = &vrun0;
    }

    while (a++ < runlen[1])
    {
        m3d_reciprocal_z_interpv_step norm((*ptscursorb - *ptscursora + 1),
                                           nleft->get_z(),
                                           nright->get_z(),
                                           nleft->get_vectorvalue(),
                                           nright->get_vectorvalue());
        m3d_reciprocal_z_interpv_step pos((*ptscursorb - *ptscursora + 1),
                                          vleft->get_z(),
                                          vright->get_z(),
                                          vleft->get_vectorvalue(),
                                          vright->get_vectorvalue());
        output = display->get_video_buffer(*ptscursora, y);
        outputend = display->get_video_buffer(*ptscursorb++, y);
        outz = zbuffer.get_zbuffer(*ptscursora++, y);
        while (output <= outputend)
        {
            if (zbuffer.test(outz, pos.get_int_z()))
            {
                temp.normal = norm.get_vectorvalue();
                memcpy(temp.vertex.myvector, pos.get_vectorvalue().myvector, sizeof(pos.get_vectorvalue().myvector));
                temp.color = vtx[0].color;
                illuminate(temp, world);
                *output = temp.color.brighten2(temp.lightint);
            }
            ++output;
            ++outz;
            norm.step();
            pos.step();
        }
        nleft->step();
        nright->step();
        vleft->step();
        vright->step();
        y++;
    }

    if (nleft == &nrun1)
    {
        nleft = &nrun2;
        vleft = &vrun2;
    }
    else
    {
        nright = &nrun2;
        vright = &vrun2;
    }

    a = 1;
    while (a++ < runlen[2])
    {
        m3d_reciprocal_z_interpv_step norm((*ptscursorb - *ptscursora + 1),
                                           nleft->get_z(),
                                           nright->get_z(),
                                           nleft->get_vectorvalue(),
                                           nright->get_vectorvalue());
        m3d_reciprocal_z_interpv_step pos((*ptscursorb - *ptscursora + 1),
                                          vleft->get_z(),
                                          vright->get_z(),
                                          vleft->get_vectorvalue(),
                                          vright->get_vectorvalue());
        output = display->get_video_buffer(*ptscursora, y);
        outputend = display->get_video_buffer(*ptscursorb++, y);
        outz = zbuffer.get_zbuffer(*ptscursora++, y);
        while (output <= outputend)
        {
            if (zbuffer.test(outz, pos.get_int_z()))
            {
                temp.normal = norm.get_vectorvalue();
                memcpy(temp.vertex.myvector, pos.get_vectorvalue().myvector, sizeof(pos.get_vectorvalue().myvector));
                temp.color = vtx[0].color;
                illuminate(temp, world);
                *output = temp.color.brighten2(temp.lightint);
            }
            ++output;
            ++outz;
            norm.step();
            pos.step();
        }
        nleft->step();
        nright->step();
        vleft->step();
        vright->step();
        y++;
    }
#endif
}