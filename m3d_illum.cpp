#include "m3d_illum.hh"
#include <iostream>
static inline float m3d_max(float a, float b)
{
        return (a > b) ? a : b;
}

m3d_illumination::m3d_illumination()
{
}

// TBD: consider all lights as white RGB(255,255,255) to reduce computation
void m3d_illumination::diffuse_lighting(m3d_vertex &vtx, m3d_render_object &obj, m3d_world &world, struct m3d_render_color &out)
{
        m3d_color temp[2];
        // Ambient light does not depend on position anyway...
        float lightint = world.ambient_light.get_intensity(vtx.position);
        temp[0] = world.ambient_light.get_color();
        temp[0].brighten(lightint);
        out.diffint = lightint;
        lightint = 0.0f;
        m3d_vector vtxworld(vtx.position);
        vtxworld.add(obj.center);
        // Now we sum all diffuse contributions considering light position w.r.t. vtx position and the surface normal.
        for (auto lights : world.lights_list)
        {
                m3d_vector L(lights->get_position());
                L.subtract(vtxworld);
                L.normalize();
                float dot = L.dot_product(vtx.normal);
                lightint += lights->get_intensity(vtx.position) * m3d_max(dot, 0.0f);
        }
        temp[1] = obj.color;
        temp[1].brighten(lightint);
        out.diffint += lightint;
        m3d_color::add_colors(temp, 2, out.Kdiff);
        temp[1].print();
}

void m3d_illumination::specular_lighting(m3d_vertex &vtx, m3d_render_object &obj, m3d_world &world, struct m3d_render_color &out)
{
}

m3d_illumination &m3d_illum::inst()
{
        static m3d_illumination instance;

        return instance;
}