/*
 * Matrix3D
 *
 * Copyright (C) 1995 - 2025 Diego Gallizioli
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef M3D_DISPLAY_H
#define M3D_DISPLAY_H

#include <cstdint>

#include "m3d_math_data.hh"

class m3d_display
{
public:
        /** Default constructor */
        m3d_display() : xmax(0), ymax(0) {};
        m3d_display(int xres, int yres) : xmax(xres), ymax(yres) {};
        /** Default destructor */
        virtual ~m3d_display() {};

        virtual uint32_t *get_video_buffer(int x0, int y0) = 0;

        int get_xmax(void) { return xmax; }
        int get_ymax(void) { return ymax; }

        /** Act directly on the surface buffer */
        virtual void fill_buffer(void) = 0;
        virtual void show_buffer(void) = 0;
        virtual void clear_buffer(void) = 0;

        /** Act on the renderer */
        virtual void set_color(uint8_t red, uint8_t green, uint8_t blue) = 0;
        virtual void draw_lines(m3d_display_point pts[], unsigned ptsnum) = 0;
        virtual void clear_renderer(void) = 0;
        virtual void show_renderer(void) = 0;

protected:
        // The window resolution
        int xmax, ymax;
};

#endif