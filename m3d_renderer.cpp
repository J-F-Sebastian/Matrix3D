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

m3d_renderer::~m3d_renderer()
{
	delete scanline;
	delete zscanline;
}

m3d_renderer::m3d_renderer(m3d_display *disp) : display(disp), zbuffer((int16_t)disp->get_xmax(), (int16_t)disp->get_ymax())
{
	scanline = new int16_t[display->get_ymax() * 2];
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
 * Sort triangle vertices by y coordinate
 */
void m3d_renderer::sort_triangle(m3d_vertex *vtx[3])
{
	if (vtx[1]->scrposition.y < vtx[0]->scrposition.y)
	{
		std::swap(vtx[0], vtx[1]);
	}

	if (vtx[1]->scrposition.y > vtx[2]->scrposition.y)
	{
		std::swap(vtx[1], vtx[2]);
	}

	if (vtx[1]->scrposition.y < vtx[0]->scrposition.y)
	{
		std::swap(vtx[0], vtx[1]);
	}
}

/*
 * Sort triangle vertices and attributes by y coordinate
 */
void m3d_renderer::sort_triangle(m3d_vertex *vtx[3], struct m3d_render_color *colors)
{
	if (vtx[1]->scrposition.y < vtx[0]->scrposition.y)
	{
		std::swap(vtx[0], vtx[1]);
		std::swap(colors[0], colors[1]);
	}

	if (vtx[1]->scrposition.y > vtx[2]->scrposition.y)
	{
		std::swap(vtx[1], vtx[2]);
		std::swap(colors[1], colors[2]);
	}

	if (vtx[1]->scrposition.y < vtx[0]->scrposition.y)
	{
		std::swap(vtx[0], vtx[1]);
		std::swap(colors[0], colors[1]);
	}
}

void m3d_renderer::store_scanlines(unsigned runlen, int16_t val1, int16_t val2, unsigned start)
{
	m3d_interpolation_short run(runlen, val1, val2);
	run.valuearray(scanline + start);
}

void m3d_renderer::store_zscanlines(unsigned runlen, float val1, float val2, unsigned start)
{
	m3d_interpolation_float run(runlen, val1, val2);
	run.valuearray(zscanline + start);
}

void m3d_renderer::compute_visible_list_and_sort(m3d_world &world)
{
	vislist.clear();

	for (auto itro : world.objects_list)
	{
		itro->project(world.camera);
		if (itro->trivisible.any())
			vislist.push_back(itro);
	}

	if (vislist.size())
	{
		world.sort(vislist);
		zbuffer.reset();
	}
}
