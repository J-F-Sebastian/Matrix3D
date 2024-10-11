#ifndef _M3D_RENDER_COLOR_HH_
#define _M3D_RENDER_COLOR_HH_

#include "m3d_color.hh"

struct m3d_render_color
{
        /*
         * The ambient, specular and diffuse colors computed for this vertex
         */
        m3d_color Kamb, Kspec, Kdiff;
        /*
         * The intensity of ambient, specular and diffuse lights
         */
        float ambint, specint, diffint;
};

#endif