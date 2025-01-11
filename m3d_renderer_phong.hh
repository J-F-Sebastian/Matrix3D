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

#ifndef M3D_RENDERER_PHONG_H
#define M3D_RENDERER_PHONG_H

#include "m3d_renderer_shaded.hh"

// Phong shading renderer
class m3d_renderer_shaded_phong : public m3d_renderer_shaded
{
public:
	/** Default constructor */
	m3d_renderer_shaded_phong() : m3d_renderer_shaded() {};
	m3d_renderer_shaded_phong(m3d_display *disp) : m3d_renderer_shaded(disp)
	{
		vscanline = new m3d_vector[(unsigned)display->get_ymax() * 2];
		wscanline = new m3d_point[(unsigned)display->get_ymax() * 2];
	};

	/** Default destructor */
	virtual ~m3d_renderer_shaded_phong() {};

protected:
	// The scanline normals buffer
	m3d_vector *vscanline;
	// The scanline world-coordinates buffer
	m3d_point *wscanline;

	void store_vscanlines(unsigned runlen, m3d_vertex &val1, m3d_vertex &val2, unsigned start = 0);
	void store_wscanlines(unsigned runlen, m3d_vertex &val1, m3d_vertex &val2, unsigned start = 0);
	virtual void triangle_fill_shaded(m3d_render_object &obj, m3d_vertex *vtx[], m3d_world &world);
};

#endif
