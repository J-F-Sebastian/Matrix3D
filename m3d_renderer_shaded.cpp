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

#include "m3d_renderer_shaded.hh"
#include "m3d_interp.hh"

/*
 * SHADED RENDERER, WITH 1/z INTERPOLATION
 */

void m3d_renderer_shaded::render(m3d_world &world)
{
	unsigned i, j;
	m3d_vertex *vtx[3];

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

				sort_triangle(vtx, colors);

				triangle_fill_shaded(*itro, vtx, world);
			}
		}
	}

	// Present the rendered lines
	display->show_buffer();
}

void m3d_renderer_shaded::store_iscanlines(unsigned runlen, float z1, float z2, float val1, float val2, unsigned start)
{
	m3d_interpolation_float_perspective run(runlen, z1, z2, val1, val2);
	run.valuearray(iscanline + start);
}

void m3d_renderer_shaded::triangle_fill_shaded(m3d_render_object &obj, m3d_vertex *vtx[], m3d_world &world)
{
	uint32_t *output;
	float *outz;
	float p0 = vtx[0]->prjposition[Z_C];
	float p1 = vtx[1]->prjposition[Z_C];
	float p2 = vtx[2]->prjposition[Z_C];
	int16_t p3 = (int16_t)vtx[0]->scrposition.x;
	int16_t p4 = (int16_t)vtx[1]->scrposition.x;
	int16_t p5 = (int16_t)vtx[2]->scrposition.x;
	float i0 = colors[0].ambint + colors[0].diffint;
	float i1 = colors[1].ambint + colors[1].diffint;
	float i2 = colors[2].ambint + colors[2].diffint;
	unsigned runlen0 = (unsigned)(vtx[2]->scrposition.y - vtx[0]->scrposition.y + 1);
	unsigned runlen1 = (unsigned)(vtx[1]->scrposition.y - vtx[0]->scrposition.y + 1);
	unsigned runlen2 = (unsigned)(vtx[2]->scrposition.y - vtx[1]->scrposition.y + 1);
	int fillrunlen;
	int16_t y = (int16_t)vtx[0]->scrposition.y;
	int16_t *lscanline, *rscanline;
	float *lzscanline, *rzscanline;
	float *liscanline, *riscanline;

	store_scanlines(runlen0, p3, p5);
	store_scanlines(runlen1, p3, p4, runlen0);
	store_scanlines(runlen2, p4, p5, runlen0 + runlen1 - 1);
	store_zscanlines(runlen0, p0, p2);
	store_zscanlines(runlen1, p0, p1, runlen0);
	store_zscanlines(runlen2, p1, p2, runlen0 + runlen1 - 1);
	store_iscanlines(runlen0, p0, p2, i0, i2);
	store_iscanlines(runlen1, p0, p1, i0, i1, runlen0);
	store_iscanlines(runlen2, p1, p2, i0, i2, runlen0 + runlen1 - 1);

	lscanline = rscanline = scanline;
	lzscanline = rzscanline = zscanline;
	liscanline = riscanline = iscanline;
	/*
	 * check x values to understand who's the left side and who's the right side.
	 * runlen1 - 1 is the position of the scanline passing through point 1, the middle
	 * point of the triangle projected to screen.
	 */
	if (scanline[runlen1 - 1] <= scanline[runlen0 + runlen1 - 1])
	{
		rscanline += runlen0;
		rzscanline += runlen0;
		riscanline += runlen0;
	}
	else
	{
		lscanline += runlen0;
		lzscanline += runlen0;
		liscanline += runlen0;
	}

	while (runlen0--)
	{
		fillrunlen = *rscanline - *lscanline + 1;
		m3d_interpolation_float_perspective sl(fillrunlen, *lzscanline, *rzscanline, *liscanline, *riscanline);
		output = display->get_video_buffer(*lscanline, y);
		outz = zbuffer.get_zbuffer(*lscanline, y);
		while (sl.finished() == false)
		{
			if (zbuffer.test_update(outz, sl.value()))
			{
				*output = colors[0].Kdiff.brighten2(sl.value());
			}
			++output;
			++outz;
			sl.step();
		}
		lscanline++;
		rscanline++;
		lzscanline++;
		rzscanline++;
		liscanline++;
		riscanline++;
		y++;
	}
}
