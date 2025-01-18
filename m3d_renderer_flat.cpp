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

#include "m3d_renderer_flat.hh"
#include "m3d_interp.hh"

static inline m3d_color m3d_average_light(m3d_render_color n[])
{
	m3d_color temp[3];
	m3d_color out;

	temp[0] = n[0].Kamb + n[0].Kdiff;
	temp[1] = n[1].Kamb + n[1].Kdiff;
	temp[2] = n[2].Kamb + n[2].Kdiff;

	m3d_color::average_colors(temp, 3, out);
	return out;
}

/*
 * FLAT RENDERING
 */

void m3d_renderer_flat::render(m3d_world &world)
{
	unsigned i, j;
	m3d_vertex *vtx[3];
	m3d_render_color colors[3];
	m3d_color color;

	// Compute visible objects
	compute_visible_list_and_sort(world);

	// Fill the surface black
	display->clear_buffer();

	for (auto itro : vislist)
	{
		i = 0;
		for (auto &triangle : itro->mesh)
		{
			if (itro->trivisible[i++])
			{
				for (j = 0; j < 3; j++)
				{
					vtx[j] = &itro->vertices.at(triangle.index[j]);
					m3d_illum::inst().ambient_lighting(*vtx[j], *itro, world, colors[j]);
					m3d_illum::inst().diffuse_lighting(*vtx[j], *itro, world, colors[j]);
				}

				sort_triangle(vtx);

				color = m3d_average_light(colors);

				triangle_fill_flat(vtx, color);
			}
		}
	}

	// Present the rendered lines
	display->show_buffer();
}

void m3d_renderer_flat::triangle_fill_flat(m3d_vertex *vtx[], m3d_color &color)
{
	uint32_t *output;
	float *outz;
	float p0 = vtx[0]->prjposition[Z_C];
	float p1 = vtx[1]->prjposition[Z_C];
	float p2 = vtx[2]->prjposition[Z_C];
	float p3 = (float)vtx[0]->scrposition.x;
	float p4 = (float)vtx[1]->scrposition.x;
	float p5 = (float)vtx[2]->scrposition.x;
	unsigned runlen0 = (unsigned)(vtx[2]->scrposition.y - vtx[0]->scrposition.y + 1);
	unsigned runlen1 = (unsigned)(vtx[1]->scrposition.y - vtx[0]->scrposition.y + 1);
	unsigned runlen2 = (unsigned)(vtx[2]->scrposition.y - vtx[1]->scrposition.y + 1);
	int fillrunlen;
	int16_t y = (int16_t)vtx[0]->scrposition.y;
	float *lscanline, *rscanline;
	float *lzscanline, *rzscanline;

	store_fscanlines(runlen0, p3, p5);
	store_fscanlines(runlen1, p3, p4, runlen0);
	store_fscanlines(runlen2, p4, p5, runlen0 + runlen1 - 1);
	store_zscanlines(runlen0, p0, p2);
	store_zscanlines(runlen1, p0, p1, runlen0);
	store_zscanlines(runlen2, p1, p2, runlen0 + runlen1 - 1);

	lscanline = rscanline = fscanline;
	lzscanline = rzscanline = zscanline;
	/*
	 * check x values to understand who's the left side and who's the right side.
	 * runlen1 - 1 is the position of the scanline passing through point 1, the middle
	 * point of the triangle projected to screen.
	 */
	if (fscanline[runlen1 - 1] <= fscanline[runlen0 + runlen1 - 1])
	{
		rscanline += runlen0;
		rzscanline += runlen0;
	}
	else
	{
		lscanline += runlen0;
		lzscanline += runlen0;
	}

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
				*output = color.getColor();
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
