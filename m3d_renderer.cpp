#include <algorithm>
#include <iostream>
#include <sstream>
#include <cfloat>

#include "m3d_renderer.hh"

using namespace std;

static inline float m3d_max(float a, float b)
{
    return (a > b) ? a : b;
}

static inline float m3d_average_light(struct m3d_renderer::m3d_renderer_data n[])
{
    return (n[0].lightint + n[1].lightint + n[2].lightint) / 3.0f;
}

m3d_renderer::~m3d_renderer()
{
    delete scanline;
}

m3d_renderer::m3d_renderer(m3d_display *disp) : display(disp), zbuffer(disp->get_xmax(), disp->get_ymax())
{
    scanline = new int16_t[display->get_xmax() * display->get_ymax()];
}

/*
 * Default renderer is void
 */
void m3d_renderer::render(m3d_world & /*world*/)
{
    //Update the surface
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
void m3d_renderer::store_scanlines(int16_t x0,
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
        return;
    }

    if (!deltax)
    {
        /* vertical line, all x values are the same... */
        for (cnt = 0; cnt <= (uint32_t)deltay; cnt++)
        {
            *stor++ = val;
        }
        return;
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
        return;
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
}

void m3d_renderer::illuminate(struct m3d_renderer_data &vtx, m3d_world &world)
{
    list<m3d_point_light_source *>::iterator lights = world.lights_list.begin();
    m3d_point L;
    float NdotL;

    //Compute Kdiffuse for Diffuse Lightning
    //
    // Kdiffuse = SurfaceColor x (AmbientIntensity + Sum(LightSourceIntensity x Max(NdotL, 0))
    //
    // Start assigning the ambient intensity
    vtx.lightint = world.ambient_light.get_intensity(vtx.vertex, world.camera.get_position());
    //Sum(LightSourceIntensity x Max(NdotL, 0) is added to ambient intensity
    while (lights != world.lights_list.end())
    {
        L = (*lights)->get_position();
        L.subtract(vtx.vertex);
        L.normalize();
        NdotL = m3d_max(L.dot_product(vtx.normal), 0.0f);
        if (NdotL != 0.0f)
        {
            NdotL *= (*lights)->get_intensity(vtx.vertex, world.camera.get_position());
            vtx.lightint += NdotL;
        }
        ++lights;
    }
}

/*
 * WIREFRAME RENDERING
 */

void m3d_renderer_wireframe::render(m3d_world &world)
{
    list<m3d_render_object *>::iterator itro;
    vector<m3d_triangle>::iterator triangle;
    m3d_point temp;
    m3d_color ctemp;
    SDL_Point toscreen[4 * M3D_MAX_TRIANGLES];
    unsigned i, j, k;

    //Fill the surface black
    display->clear_renderer();
    for (itro = world.objects_list.begin(); itro != world.objects_list.end(); itro++)
    {
        (*itro)->compute_visibility(world.camera);
        if ((*itro)->triangle_visible.none())
            continue;

        ctemp = (*itro)->color;
        display->set_color(ctemp.getChannel(m3d_color::R_CHANNEL),
                           ctemp.getChannel(m3d_color::G_CHANNEL),
                           ctemp.getChannel(m3d_color::B_CHANNEL));

        for (i = 0, k = 0, triangle = (*itro)->mesh.begin(); triangle != (*itro)->mesh.end(); i++, triangle++)
        {
            if ((*itro)->triangle_visible[i])
            {
                for (j = 0; j < 3; j++)
                {
                    temp = (*itro)->vertices[triangle->index[j]].position;
                    temp.add((*itro)->center);
                    world.camera.transform_and_project_to_screen(temp, toscreen[k++]);
                }
                toscreen[k] = toscreen[k - 3];
                k++;
            }
        }
        display->draw_lines(toscreen, k);
    }

    //Present the rendered lines
    display->show_renderer();
}

/*
 * FLAT RENDERING
 */
void m3d_renderer_flat::render(m3d_world &world)
{
    list<m3d_render_object *>::iterator itro = world.objects_list.begin();
    list<m3d_render_object *>::reverse_iterator ritro;
    list<m3d_render_object *> vislist;
    list<m3d_point_light_source *>::iterator lights;
    vector<m3d_triangle>::iterator triangle;
    unsigned runlen[3];
    unsigned i, j;
    m3d_point L;
    struct m3d_renderer_data vtx[3];

    while (itro != world.objects_list.end())
    {
        (*itro)->compute_visibility(world.camera);
        if ((*itro)->triangle_visible.any())
            vislist.push_back((*itro));
        ++itro;
    }
    if (vislist.empty())
        return;

    zbuffer.reset();
    world.sort(vislist);
    //Fill the surface black
    display->clear_buffer();

    for (ritro = vislist.rbegin(); ritro != vislist.rend(); ritro++)
    {
        for (i = 0, triangle = (*ritro)->mesh.begin(); triangle != (*ritro)->mesh.end(); i++, triangle++)
        {
            if ((*ritro)->triangle_visible[i])
            {
                for (j = 0; j < 3; j++)
                {
                    vtx[j].vertex = (*ritro)->vertices[triangle->index[j]].position;
                    vtx[j].vertex.add((*ritro)->center);
                    vtx[j].normal = (*ritro)->vertices[triangle->index[j]].normal;
                    illuminate(vtx[j], world);
                    world.camera.transform_and_project_to_screen(vtx[j].vertex, vtx[j].toscreen);
                }

                sort_triangle(vtx);

                // color is blended, brighten is a multiplication
                vtx[0].color = (*ritro)->color;
                vtx[0].color.brighten(m3d_average_light(vtx));
                /*
                * alloc a vector of memory pointers for any y.
                * In other words store all x computed along the lines composing
                * the triangle.
                * Obviously the number of pointers is twice the distance from smallest
                * to biggest y values.
                * run0 = run1 + run2 - 1
                *
                * run1 and run2 have 1 overlapping point
                *
                */

                runlen[0] = vtx[2].toscreen.y - vtx[0].toscreen.y + 1;
                runlen[1] = vtx[1].toscreen.y - vtx[0].toscreen.y + 1;
                runlen[2] = vtx[2].toscreen.y - vtx[1].toscreen.y + 1;

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

                /*
                * run0 cannot be 1, as this is avoided by the initial check on x and y
                */
                store_scanlines(vtx[0].toscreen.x, vtx[0].toscreen.y,
                                vtx[2].toscreen.x, vtx[2].toscreen.y);

                /*
                * run1 and run2 can be 1, but not both of them, as run1 cannot be 1
                * (nor 0!) and run0 = run1 + run2 - 1.
                * When a run equals 1, the line is horizontal.
                */
                store_scanlines(vtx[0].toscreen.x, vtx[0].toscreen.y,
                                vtx[1].toscreen.x, vtx[1].toscreen.y,
                                runlen[0]);

                store_scanlines(vtx[1].toscreen.x, vtx[1].toscreen.y,
                                vtx[2].toscreen.x, vtx[2].toscreen.y,
                                runlen[0] + runlen[1] - 1);

                triangle_fill_flat(vtx, runlen);
            }
        }
    }

    //Present the rendered lines
    display->show_buffer();
}

void m3d_renderer_flat::triangle_fill_flat(struct m3d_renderer_data vtx[],
                                           unsigned runlen[])
{
    unsigned a = 0;
    int16_t *ptscursora, *ptscursorb;
    uint32_t *output, *outputend;
    int16_t *outz;
    float p0 = vtx[0].vertex.myvector[Z_C];
    float p1 = vtx[1].vertex.myvector[Z_C];
    float p2 = vtx[2].vertex.myvector[Z_C];
    m3d_interp_step run0((float)runlen[0], p0, p2);
    m3d_interp_step run1((float)runlen[1], p0, p1);
    m3d_interp_step run2((float)runlen[2], p1, p2);
    m3d_interp_step *left, *right;
    int16_t y = vtx[0].toscreen.y;

    /*
     * check x values to understand who's the left half and who's the right
     */
    /* Draws a horizontal line from first half of points to second half */
    if (scanline[runlen[0] / 2] <= scanline[runlen[0] + runlen[0] / 2])
    {
        ptscursora = scanline;
        ptscursorb = scanline + runlen[0];
        left = &run0;
        right = &run1;
    }
    else
    {
        ptscursora = scanline + runlen[0];
        ptscursorb = scanline;
        left = &run1;
        right = &run0;
    }

    while (a++ < runlen[1])
    {
        m3d_interp_step sl((float)(*ptscursorb - *ptscursora + 1), left->get_val(), right->get_val());
        output = display->get_video_buffer(*ptscursora, y);
        outputend = display->get_video_buffer(*ptscursorb++, y);
        outz = zbuffer.get_zbuffer(*ptscursora++, y);
        while (output <= outputend)
        {
            if (zbuffer.test(outz, sl.get_int_val()))
            {
                *output = vtx[0].color.getColor();
            }
            ++output;
            ++outz;
            sl.step();
        }
        left->step();
        right->step();
        y++;
    }

    if (left == &run1)
    {
        left = &run2;
    }
    else
    {
        right = &run2;
    }

    a = 1;
    while (a++ < runlen[2])
    {
        m3d_interp_step sl((float)(*ptscursorb - *ptscursora + 1), left->get_val(), right->get_val());
        output = display->get_video_buffer(*ptscursora, y);
        outputend = display->get_video_buffer(*ptscursorb++, y);
        outz = zbuffer.get_zbuffer(*ptscursora++, y);
        while (output <= outputend)
        {
            if (zbuffer.test(outz, sl.get_int_val()))
            {
                *output = vtx[0].color.getColor();
            }
            ++output;
            ++outz;
            sl.step();
        }
        left->step();
        right->step();
        y++;
    }
}

/*
 * FLAT RENDERING WITH FLOATING POINT IMPLEMENTATION
 */

void m3d_renderer_flatf::render(m3d_world &world)
{
    list<m3d_render_object *>::iterator itro = world.objects_list.begin();
    list<m3d_render_object *>::reverse_iterator ritro;
    list<m3d_render_object *> vislist;
    list<m3d_point_light_source *>::iterator lights;
    vector<m3d_triangle>::iterator triangle;
    unsigned i, j;
    m3d_point L;
    struct m3d_renderer_data vtx[3];

    while (itro != world.objects_list.end())
    {
        (*itro)->compute_visibility(world.camera);
        if ((*itro)->triangle_visible.any())
            vislist.push_back((*itro));
        ++itro;
    }
    if (vislist.empty())
        return;

    zbuffer.reset();
    world.sort(vislist);
    //Fill the surface black
    display->clear_buffer();

    for (ritro = vislist.rbegin(); ritro != vislist.rend(); ritro++)
    {
        for (i = 0, triangle = (*ritro)->mesh.begin(); triangle != (*ritro)->mesh.end(); i++, triangle++)
        {
            if ((*ritro)->triangle_visible[i])
            {
                for (j = 0; j < 3; j++)
                {
                    vtx[j].vertex = (*ritro)->vertices[triangle->index[j]].position;
                    vtx[j].vertex.add((*ritro)->center);
                    vtx[j].normal = (*ritro)->vertices[triangle->index[j]].normal;
                    illuminate(vtx[j], world);
                    world.camera.transform_and_project_to_screen(vtx[j].vertex, vtx[j].toscreen);
                }

                sort_triangle(vtx);

                // color is blended, brighten is a multiplication
                vtx[0].color = (*ritro)->color;
                vtx[0].color.brighten(m3d_average_light(vtx));

                triangle_fill_flat(vtx);
            }
        }
    }

    //Present the rendered lines
    display->show_buffer();
}

void m3d_renderer_flatf::triangle_fill_flat(struct m3d_renderer_data vtx[])
{
    unsigned a = 0;
    uint32_t *output, *outputend;
    int16_t *outz;
    float p0 = vtx[0].vertex.myvector[Z_C];
    float p1 = vtx[1].vertex.myvector[Z_C];
    float p2 = vtx[2].vertex.myvector[Z_C];
    float p3 = vtx[0].toscreen.x;
    float p4 = vtx[1].toscreen.x;
    float p5 = vtx[2].toscreen.x;
    unsigned runlen0 = vtx[2].toscreen.y - vtx[0].toscreen.y + 1;
    unsigned runlen1 = vtx[1].toscreen.y - vtx[0].toscreen.y + 1;
    unsigned runlen2 = vtx[2].toscreen.y - vtx[1].toscreen.y;
    unsigned fillrunlen;
    m3d_interp_step zrun0((float)(runlen0), p0, p2);
    m3d_interp_step zrun1((float)(runlen1), p0, p1);
    m3d_interp_step zrun2((float)(runlen2), p1, p2);
    m3d_interp_step xrun0((float)(runlen0), p3, p5);
    m3d_interp_step xrun1((float)(runlen1), p3, p4);
    m3d_interp_step xrun2((float)(runlen2), p4, p5);
    m3d_interp_step *zrleft, *zrright, *xrleft, *xrright;
    int16_t y = vtx[0].toscreen.y;

    /*
     * check x values to understand who's the left half and who's the right
     */
    /* Draws a horizontal line from first half of points to second half */
    if (xrun0.get_delta() <= xrun1.get_delta())
    {
        zrleft = &zrun0;
        xrleft = &xrun0;
        zrright = &zrun1;
        xrright = &xrun1;
    }
    else
    {
        zrleft = &zrun1;
        xrleft = &xrun1;
        zrright = &zrun0;
        xrright = &xrun0;
    }

    while (a++ < runlen1)
    {
        fillrunlen = (unsigned)round(xrright->get_val() - xrleft->get_val() + 1.0f);
        m3d_interp_step sl((float)fillrunlen, zrleft->get_val(), zrright->get_val());
        output = display->get_video_buffer((unsigned)round(xrleft->get_val()), y);
        outputend = output + fillrunlen;
        outz = zbuffer.get_zbuffer((unsigned)round(xrleft->get_val()), y);
        while (output < outputend)
        {
            if (zbuffer.test(outz, sl.get_int_val()))
            {
                *output = vtx[0].color.getColor();
            }
            ++output;
            ++outz;
            sl.step();
        }
        xrleft->step();
        xrright->step();
        zrleft->step();
        zrright->step();
        y++;
    }

    if (xrleft == &xrun1)
    {
        xrleft = &xrun2;
        zrleft = &zrun2;
    }
    else
    {
        xrright = &xrun2;
        zrright = &zrun2;
    }

    a = 1;
    while (a++ < runlen2)
    {
        fillrunlen = (unsigned)round(xrright->get_val() - xrleft->get_val() + 1.0f);
        m3d_interp_step sl((float)fillrunlen, zrleft->get_val(), zrright->get_val());
        output = display->get_video_buffer((unsigned)round(xrleft->get_val()), y);
        outputend = output + fillrunlen;
        outz = zbuffer.get_zbuffer((unsigned)round(xrleft->get_val()), y);
        while (output < outputend)
        {
            if (zbuffer.test(outz, sl.get_int_val()))
            {
                *output = vtx[0].color.getColor();
            }
            ++output;
            ++outz;
            sl.step();
        }
        xrleft->step();
        xrright->step();
        zrleft->step();
        zrright->step();
        y++;
    }
}

/*
 * SHADED RENDERER, WITH 1/z INTERPOLATION
 */

void m3d_renderer_shaded::render(m3d_world &world)
{
    list<m3d_render_object *>::iterator itro = world.objects_list.begin();
    list<m3d_render_object *>::reverse_iterator ritro;
    list<m3d_render_object *> vislist;
    list<m3d_point_light_source *>::iterator lights;
    vector<m3d_triangle>::iterator triangle;
    unsigned runlen[3];
    unsigned i, j;
    m3d_point L;
    struct m3d_renderer_data vtx[3];

    while (itro != world.objects_list.end())
    {
        (*itro)->compute_visibility(world.camera);
        if ((*itro)->triangle_visible.any())
            vislist.push_back((*itro));
        ++itro;
    }
    if (vislist.empty())
        return;

    zbuffer.reset();
    world.sort(vislist);
    //Fill the surface black
    display->clear_buffer();

    for (ritro = vislist.rbegin(); ritro != vislist.rend(); ritro++)
    {
        for (i = 0, triangle = (*ritro)->mesh.begin(); triangle != (*ritro)->mesh.end(); i++, triangle++)
        {
            if ((*ritro)->triangle_visible[i])
            {
                for (j = 0; j < 3; j++)
                {
                    vtx[j].vertex = (*ritro)->vertices[triangle->index[j]].position;
                    vtx[j].vertex.add((*ritro)->center);
                    vtx[j].normal = (*ritro)->vertices[triangle->index[j]].position;
                    illuminate(vtx[j], world);
                    world.camera.transform_and_project_to_screen(vtx[j].vertex, vtx[j].toscreen);
                    vtx[j].color = (*ritro)->color;
                }

                sort_triangle(vtx);

                /*
                * alloc a vector of memory pointers for any y.
                * In other words store all x computed along the lines composing
                * the triangle.
                * Obviously the number of pointers is twice the distance from smallest
                * to biggest y values.
                * run0 = run1 + run2 - 1
                *
                * run1 and run2 have 1 overlapping point
                *
                */

                runlen[0] = vtx[2].toscreen.y - vtx[0].toscreen.y + 1;
                runlen[1] = vtx[1].toscreen.y - vtx[0].toscreen.y + 1;
                runlen[2] = vtx[2].toscreen.y - vtx[1].toscreen.y + 1;

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

                /*
                * run0 cannot be 1, as this is avoided by the initial check on x and y
                */
                store_scanlines(vtx[0].toscreen.x, vtx[0].toscreen.y,
                                vtx[2].toscreen.x, vtx[2].toscreen.y);

                /*
                * run1 and run2 can be 1, but not both of them, as run1 cannot be 1
                * (nor 0!) and run0 = run1 + run2 - 1.
                * When a run equals 1, the line is horizontal.
                */
                store_scanlines(vtx[0].toscreen.x, vtx[0].toscreen.y,
                                vtx[1].toscreen.x, vtx[1].toscreen.y,
                                runlen[0]);

                store_scanlines(vtx[1].toscreen.x, vtx[1].toscreen.y,
                                vtx[2].toscreen.x, vtx[2].toscreen.y,
                                runlen[0] + runlen[1] - 1);

                triangle_fill_shaded(vtx, runlen, world);
            }
        }
    }

    //Present the rendered lines
    display->show_buffer();
}

void m3d_renderer_shaded::triangle_fill_shaded(struct m3d_renderer_data vtx[],
                                               unsigned runlen[],
                                               m3d_world &world)
{
    unsigned a = 0;
    int16_t *ptscursora, *ptscursorb;
    uint32_t *output, *outputend;
    int16_t *outz;
    float zp0 = vtx[0].vertex.myvector[Z_C];
    float zp1 = vtx[1].vertex.myvector[Z_C];
    float zp2 = vtx[2].vertex.myvector[Z_C];
    m3d_reciprocal_z_interp_step run0(zp0,
                                      zp2,
                                      (float)runlen[0],
                                      vtx[0].lightint,
                                      vtx[2].lightint);
    m3d_reciprocal_z_interp_step run1(zp0,
                                      zp1,
                                      (float)runlen[1],
                                      vtx[0].lightint,
                                      vtx[1].lightint);
    m3d_reciprocal_z_interp_step run2(zp1,
                                      zp2,
                                      (float)runlen[2],
                                      vtx[1].lightint,
                                      vtx[2].lightint);
    m3d_reciprocal_z_interp_step *left, *right;

    m3d_interp_step zrun0((float)runlen[0], zp0, zp2);
    m3d_interp_step zrun1((float)runlen[1], zp0, zp1);
    m3d_interp_step zrun2((float)runlen[2], zp1, zp2);
    m3d_interp_step *zleft, *zright;
    int16_t y = vtx[0].toscreen.y;

    /*
     * check x values to understand who's the left half and who's the right
     */
    /* Draws a horizontal line from first half of points to second half */
    if (scanline[runlen[0] / 2] <= scanline[runlen[0] + runlen[0] / 2])
    {
        ptscursora = scanline;
        ptscursorb = scanline + runlen[0];
        left = &run0;
        right = &run1;
        zleft = &zrun0;
        zright = &zrun1;
    }
    else
    {
        ptscursora = scanline + runlen[0];
        ptscursorb = scanline;
        left = &run1;
        right = &run0;
        zleft = &zrun1;
        zright = &zrun0;
    }

    while (a++ < runlen[1])
    {
        m3d_reciprocal_z_interp_step sl(left->get_z(),
                                        right->get_z(),
                                        (float)(*ptscursorb - *ptscursora + 1),
                                        left->get_paramvalue(),
                                        right->get_paramvalue());
        m3d_interp_step zsl((float)(*ptscursorb - *ptscursora + 1), zleft->get_val(), zright->get_val());
        output = display->get_video_buffer(*ptscursora, y);
        outputend = display->get_video_buffer(*ptscursorb++, y);
        outz = zbuffer.get_zbuffer(*ptscursora++, y);

        while (output <= outputend)
        {
            if (zbuffer.test(outz, zsl.get_int_val()))
            {
                *output = vtx[0].color.brighten2(sl.get_paramvalue());
            }
            ++output;
            ++outz;
            sl.step();
            zsl.step();
        }
        left->step();
        right->step();
        y++;
    }

    if (left == &run1)
    {
        left = &run2;
        zleft = &zrun2;
    }
    else
    {
        right = &run2;
        zright = &zrun2;
    }

    a = 1;
    while (a++ < runlen[2])
    {
        m3d_reciprocal_z_interp_step sl(left->get_z(),
                                        right->get_z(),
                                        (float)(*ptscursorb - *ptscursora + 1),
                                        left->get_paramvalue(),
                                        right->get_paramvalue());
        m3d_interp_step zsl((float)(*ptscursorb - *ptscursora + 1), zleft->get_val(), zright->get_val());
        output = display->get_video_buffer(*ptscursora, y);
        outputend = display->get_video_buffer(*ptscursorb++, y);
        outz = zbuffer.get_zbuffer(*ptscursora++, y);
        while (output <= outputend)
        {
            if (zbuffer.test(outz, zsl.get_int_val()))
            {
                *output = vtx[0].color.brighten2(sl.get_paramvalue());
            }
            ++output;
            ++outz;
            sl.step();
            zsl.step();
        }
        left->step();
        right->step();
        y++;
    }
}

/*
 * GOURAUD SHADING RENDERER
 */

void m3d_renderer_shaded_gouraud::triangle_fill_shaded(struct m3d_renderer_data vtx[],
                                                       unsigned runlen[],
                                                       m3d_world &world)
{
    unsigned a = 0;
    int16_t *ptscursora, *ptscursorb;
    uint32_t *output, *outputend;
    int16_t *outz;
    m3d_interp_step *left, *right, *leftz, *rightz;
    m3d_interp_step lightint0((float)runlen[0], vtx[0].lightint, vtx[2].lightint);
    m3d_interp_step lightint1((float)runlen[1], vtx[0].lightint, vtx[1].lightint);
    m3d_interp_step lightint2((float)runlen[2], vtx[1].lightint, vtx[2].lightint);
    m3d_interp_step interpz0((float)runlen[0], vtx[0].vertex.myvector[Z_C], vtx[2].vertex.myvector[Z_C]);
    m3d_interp_step interpz1((float)runlen[1], vtx[0].vertex.myvector[Z_C], vtx[1].vertex.myvector[Z_C]);
    m3d_interp_step interpz2((float)runlen[2], vtx[1].vertex.myvector[Z_C], vtx[2].vertex.myvector[Z_C]);

    int16_t y = vtx[0].toscreen.y;

    /*
     * check x values to understand who's the left half and who's the right
     */
    /* Draws a horizontal line from first half of points to second half */
    if (scanline[runlen[0] / 2] <= scanline[runlen[0] + runlen[0] / 2])
    {
        ptscursora = scanline;
        ptscursorb = scanline + runlen[0];
        left = &lightint0;
        leftz = &interpz0;
        right = &lightint1;
        rightz = &interpz1;
    }
    else
    {
        ptscursora = scanline + runlen[0];
        ptscursorb = scanline;
        left = &lightint1;
        leftz = &interpz1;
        right = &lightint0;
        rightz = &interpz0;
    }

    while (a++ < runlen[1])
    {
        m3d_interp_step sl((float)(*ptscursorb - *ptscursora + 1), left->get_val(), right->get_val());
        m3d_interp_step z((float)(*ptscursorb - *ptscursora + 1), leftz->get_val(), rightz->get_val());
        output = display->get_video_buffer(*ptscursora, y);
        outputend = display->get_video_buffer(*ptscursorb++, y);
        outz = zbuffer.get_zbuffer(*ptscursora++, y);
        while (output <= outputend)
        {
            if (zbuffer.test(outz, z.get_int_val()))
            {
                *output = vtx[0].color.brighten2(sl.get_val());
            }
            ++output;
            ++outz;
            sl.step();
            z.step();
        }
        left->step();
        right->step();
        y++;
    }

    if (left == &lightint1)
    {
        left = &lightint2;
        leftz = &interpz2;
    }
    else
    {
        right = &lightint2;
        rightz = &interpz2;
    }

    a = 1;
    while (a++ < runlen[2])
    {
        m3d_interp_step sl((float)(*ptscursorb - *ptscursora + 1), left->get_val(), right->get_val());
        m3d_interp_step z((float)(*ptscursorb - *ptscursora + 1), leftz->get_val(), rightz->get_val());
        output = display->get_video_buffer(*ptscursora, y);
        outputend = display->get_video_buffer(*ptscursorb++, y);
        outz = zbuffer.get_zbuffer(*ptscursora++, y);
        while (output <= outputend)
        {
            if (zbuffer.test(outz, z.get_int_val()))
            {
                *output = vtx[0].color.brighten2(sl.get_val());
            }
            ++output;
            ++outz;
            sl.step();
            z.step();
        }
        left->step();
        right->step();
        y++;
    }
}

/*
 * PHONG SHADING RENDERER
 */

void m3d_renderer_shaded_phong::triangle_fill_shaded(struct m3d_renderer_data vtx[],
                                                     unsigned runlen[],
                                                     m3d_world &world)
{
    unsigned a = 0;
    int16_t *ptscursora, *ptscursorb;
    uint32_t *output, *outputend;
    int16_t *outz;
    struct m3d_renderer_data temp;

    m3d_reciprocal_z_interpv_step nrun0(vtx[0].vertex.myvector[Z_C],
                                        vtx[2].vertex.myvector[Z_C],
                                        (float)runlen[0],
                                        vtx[0].normal,
                                        vtx[2].normal);
    m3d_reciprocal_z_interpv_step nrun1(vtx[0].vertex.myvector[Z_C],
                                        vtx[1].vertex.myvector[Z_C],
                                        (float)runlen[1],
                                        vtx[0].normal,
                                        vtx[1].normal);
    m3d_reciprocal_z_interpv_step nrun2(vtx[1].vertex.myvector[Z_C],
                                        vtx[2].vertex.myvector[Z_C],
                                        (float)runlen[2],
                                        vtx[1].normal,
                                        vtx[2].normal);
    m3d_reciprocal_z_interpv_step vrun0(vtx[0].vertex.myvector[Z_C],
                                        vtx[2].vertex.myvector[Z_C],
                                        (float)runlen[0],
                                        vtx[0].vertex,
                                        vtx[2].vertex);
    m3d_reciprocal_z_interpv_step vrun1(vtx[0].vertex.myvector[Z_C],
                                        vtx[1].vertex.myvector[Z_C],
                                        (float)runlen[1],
                                        vtx[0].vertex,
                                        vtx[1].vertex);
    m3d_reciprocal_z_interpv_step vrun2(vtx[1].vertex.myvector[Z_C],
                                        vtx[2].vertex.myvector[Z_C],
                                        (float)runlen[2],
                                        vtx[1].vertex,
                                        vtx[2].vertex);

    m3d_reciprocal_z_interpv_step *nleft, *nright, *vleft, *vright;
    int16_t y = vtx[0].toscreen.y;

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
        m3d_reciprocal_z_interpv_step norm(nleft->get_z(),
                                           nright->get_z(),
                                           (float)(*ptscursorb - *ptscursora + 1),
                                           nleft->get_vectorvalue(),
                                           nright->get_vectorvalue());
        m3d_reciprocal_z_interpv_step pos(vleft->get_z(),
                                          vright->get_z(),
                                          (float)(*ptscursorb - *ptscursora + 1),
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
        m3d_reciprocal_z_interpv_step norm(nleft->get_z(),
                                           nright->get_z(),
                                           (float)(*ptscursorb - *ptscursora + 1),
                                           nleft->get_vectorvalue(),
                                           nright->get_vectorvalue());
        m3d_reciprocal_z_interpv_step pos(vleft->get_z(),
                                          vright->get_z(),
                                          (float)(*ptscursorb - *ptscursora + 1),
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
}