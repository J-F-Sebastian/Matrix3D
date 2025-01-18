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

#include "m3d_renderer_gouraud.hh"
#include "m3d_interp.hh"

/*
 * GOURAUD SHADING RENDERER
 */

void m3d_renderer_shaded_gouraud::store_cscanlines(unsigned runlen, m3d_color &val1, m3d_color &val2, unsigned start)
{
	m3d_interpolation_color run(runlen, val1, val2);
	run.valuearray(cscanline + start);
}

void m3d_renderer_shaded_gouraud::triangle_fill_shaded(m3d_render_object &obj, m3d_vertex *vtx[], m3d_world &world)
{
	uint32_t *output;
	float *outz;
	float p0 = vtx[0]->prjposition[Z_C];
	float p1 = vtx[1]->prjposition[Z_C];
	float p2 = vtx[2]->prjposition[Z_C];
	int16_t p3 = (int16_t)vtx[0]->scrposition.x;
	int16_t p4 = (int16_t)vtx[1]->scrposition.x;
	int16_t p5 = (int16_t)vtx[2]->scrposition.x;
	unsigned runlen0 = (unsigned)(vtx[2]->scrposition.y - vtx[0]->scrposition.y + 1);
	unsigned runlen1 = (unsigned)(vtx[1]->scrposition.y - vtx[0]->scrposition.y + 1);
	unsigned runlen2 = (unsigned)(vtx[2]->scrposition.y - vtx[1]->scrposition.y + 1);
	m3d_color c0 = colors[0].Kamb + colors[0].Kdiff;
	m3d_color c1 = colors[1].Kamb + colors[1].Kdiff;
	m3d_color c2 = colors[2].Kamb + colors[2].Kdiff;
	unsigned int fillrunlen;
	int16_t y = (int16_t)vtx[0]->scrposition.y;
	int16_t *lscanline, *rscanline;
	float *lzscanline, *rzscanline;
	uint32_t *lcscanline, *rcscanline;

	store_scanlines(runlen0, p3, p5);
	store_scanlines(runlen1, p3, p4, runlen0);
	store_scanlines(runlen2, p4, p5, runlen0 + runlen1 - 1);
	store_zscanlines(runlen0, p0, p2);
	store_zscanlines(runlen1, p0, p1, runlen0);
	store_zscanlines(runlen2, p1, p2, runlen0 + runlen1 - 1);
	store_cscanlines(runlen0, c0, c2);
	store_cscanlines(runlen1, c0, c1, runlen0);
	store_cscanlines(runlen2, c1, c2, runlen0 + runlen1 - 1);

	lscanline = rscanline = scanline;
	lzscanline = rzscanline = zscanline;
	lcscanline = rcscanline = cscanline;
	/*
	 * check x values to understand who's the left side and who's the right side.
	 * runlen1 - 1 is the position of the scanline passing through point 1, the middle
	 * point of the triangle projected to screen.
	 */
	if (scanline[runlen1 - 1] <= scanline[runlen0 + runlen1 - 1])
	{
		rscanline += runlen0;
		rzscanline += runlen0;
		rcscanline += runlen0;
	}
	else
	{
		lscanline += runlen0;
		lzscanline += runlen0;
		lcscanline += runlen0;
	}

	while (runlen0--)
	{
		fillrunlen = *rscanline - *lscanline + 1;
		m3d_interpolation_float sl(fillrunlen, *lzscanline, *rzscanline);
		m3d_color ca(*lcscanline), cb(*rcscanline);
		m3d_interpolation_color lint(fillrunlen, ca, cb);
		output = display->get_video_buffer(*lscanline, y);
		outz = zbuffer.get_zbuffer(*lscanline, y);
		while (sl.finished() == false)
		{
			if (zbuffer.test_update(outz, sl.value()))
			{
				*output = lint.value();
			}
			++output;
			++outz;
			sl.step();
			lint.step();
		}
		lscanline++;
		rscanline++;
		lzscanline++;
		rzscanline++;
		lcscanline++;
		rcscanline++;
		y++;
	}
}
