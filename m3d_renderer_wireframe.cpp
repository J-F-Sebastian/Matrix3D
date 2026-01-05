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

#include "m3d_renderer_wireframe.hh"

/*
 * WIREFRAME RENDERING
 */

void m3d_renderer_wireframe::render(m3d_world &world)
{
	m3d_point temp;
	m3d_color ctemp;
	m3d_display_point toscreen[M3D_MAX_TRIANGLES * 3];
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
			if (itro->trivisible[i++])
			{
				for (j = 0; j < 3; j++)
				{
					toscreen[k++] = itro->vertices[triangle.index[j]].scrposition;
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
