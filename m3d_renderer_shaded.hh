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

#ifndef M3D_RENDERER_SHADED_H
#define M3D_RENDERER_SHADED_H

#include "m3d_renderer.hh"

// Perspective-correct shading renderer
class m3d_renderer_shaded : public m3d_renderer
{
public:
	/** Default constructor */
	m3d_renderer_shaded() : m3d_renderer() {};
	explicit m3d_renderer_shaded(m3d_display *disp) : m3d_renderer(disp) { iscanline = new float[(unsigned)display->get_ymax() * 2]; };

	/** Default destructor */
	virtual ~m3d_renderer_shaded() { delete iscanline; };

	virtual void render(m3d_world &world);

protected:
	// The scanline light intensity buffer
	float *iscanline;
	struct m3d_render_color colors[3];

	void store_iscanlines(unsigned runlen, float z1, float z2, float val1, float val2, unsigned start = 0);
	virtual void triangle_fill_shaded(m3d_render_object &obj, m3d_vertex *vtx[], m3d_world &world);
};

#endif
