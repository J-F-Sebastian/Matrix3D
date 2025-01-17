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

#ifndef M3D_RENDERER_WIREFRAME_H
#define M3D_RENDERER_WIREFRAME_H

#include "m3d_renderer.hh"

class m3d_renderer_wireframe : public m3d_renderer
{
public:
	/** Default constructor */
	m3d_renderer_wireframe() : m3d_renderer() {};
	m3d_renderer_wireframe(m3d_display *disp) : m3d_renderer(disp) {};

	/** Default destructor */
	virtual ~m3d_renderer_wireframe() {};

	virtual void render(m3d_world &world);
};

#endif
