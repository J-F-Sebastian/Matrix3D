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

#include "m3d_renderer_phong.hh"
#include "m3d_interp.hh"

/*
 * PHONG SHADING RENDERER
 */

void m3d_renderer_shaded_phong::store_vscanlines(unsigned runlen, m3d_vertex &val1, m3d_vertex &val2, unsigned start)
{
	m3d_interpolation_vector run(runlen, val1.tnormal, val2.tnormal);
	run.valuearray(vscanline + start);
}

void m3d_renderer_shaded_phong::store_wscanlines(unsigned runlen, m3d_vertex &val1, m3d_vertex &val2, unsigned start)
{
	m3d_interpolation_vector run(runlen, val1.tposition, val2.tposition);
	run.valuearray(wscanline + start);
}

void m3d_renderer_shaded_phong::triangle_fill_shaded(m3d_render_object &obj, m3d_vertex *vtx[], m3d_world &world)
{
	uint32_t *output;
	float *outz;
	float p0 = vtx[0]->prjposition[Z_C];
	float p1 = vtx[1]->prjposition[Z_C];
	float p2 = vtx[2]->prjposition[Z_C];
	float p3 = (float)vtx[0]->scrposition.x + 0.5f;
	float p4 = (float)vtx[1]->scrposition.x + 0.5f;
	float p5 = (float)vtx[2]->scrposition.x + 0.5f;
	unsigned runlen0 = (unsigned)(vtx[2]->scrposition.y - vtx[0]->scrposition.y + 1);
	unsigned runlen1 = (unsigned)(vtx[1]->scrposition.y - vtx[0]->scrposition.y + 1);
	unsigned runlen2 = (unsigned)(vtx[2]->scrposition.y - vtx[1]->scrposition.y + 1);
	unsigned int fillrunlen;
	int16_t y = (int16_t)vtx[0]->scrposition.y;
	float *lscanline, *rscanline;
	float *lzscanline, *rzscanline;
	m3d_vector *lvscanline, *rvscanline;
	m3d_point *lwscanline, *rwscanline;
	float lgradient = (p5 - p3) / (float)runlen0;
	float rgradient = (p4 - p3) / (float)runlen1;

	/*
	 * check x values to understand who's the left half and who's the right
	 */
	/*
	 * Draws a horizontal line from first half of points to second half.
	 * If lgradient is less than or equal to rgradient then the left side
	 * is longest.
	 */
	if (lgradient <= rgradient)
	{
		store_fscanlines(runlen0, p3, p5);
		store_fscanlines(runlen1, p3, p4, runlen0);
		store_fscanlines(runlen2, p4, p5, runlen0 + runlen1 - 1);
		store_zscanlines(runlen0, p0, p2);
		store_zscanlines(runlen1, p0, p1, runlen0);
		store_zscanlines(runlen2, p1, p2, runlen0 + runlen1 - 1);
		store_vscanlines(runlen0, *vtx[0], *vtx[2]);
		store_vscanlines(runlen1, *vtx[0], *vtx[1], runlen0);
		store_vscanlines(runlen2, *vtx[1], *vtx[2], runlen0 + runlen1 - 1);
		store_wscanlines(runlen0, *vtx[0], *vtx[2]);
		store_wscanlines(runlen1, *vtx[0], *vtx[1], runlen0);
		store_wscanlines(runlen2, *vtx[1], *vtx[2], runlen0 + runlen1 - 1);
	}
	else
	{
		store_fscanlines(runlen1, p3, p4);
		store_fscanlines(runlen2, p4, p5, runlen1 - 1);
		store_fscanlines(runlen0, p3, p5, runlen1 + runlen2 - 1);
		store_zscanlines(runlen1, p0, p1);
		store_zscanlines(runlen2, p1, p2, runlen1 - 1);
		store_zscanlines(runlen0, p0, p2, runlen1 + runlen2 - 1);
		store_vscanlines(runlen1, *vtx[0], *vtx[1]);
		store_vscanlines(runlen2, *vtx[1], *vtx[2], runlen1 - 1);
		store_vscanlines(runlen0, *vtx[0], *vtx[2], runlen1 + runlen2 - 1);
		store_wscanlines(runlen1, *vtx[0], *vtx[1]);
		store_wscanlines(runlen2, *vtx[1], *vtx[2], runlen1 - 1);
		store_wscanlines(runlen0, *vtx[0], *vtx[2], runlen1 + runlen2 - 1);
	}

	lscanline = fscanline;
	rscanline = fscanline + runlen0;
	lzscanline = zscanline;
	rzscanline = zscanline + runlen0;
	lvscanline = vscanline;
	rvscanline = vscanline + runlen0;
	lwscanline = wscanline;
	rwscanline = wscanline + runlen0;
	while (runlen0--)
	{
		fillrunlen = (unsigned)lroundf(*rscanline - *lscanline) + 1;
		if (fillrunlen)
		{
			m3d_interpolation_float sl(fillrunlen, *lzscanline, *rzscanline);
			m3d_interpolation_vector norm(fillrunlen, *lvscanline, *rvscanline);
			m3d_interpolation_vector pts(fillrunlen, *lwscanline, *rwscanline);
			output = display->get_video_buffer((int16_t)truncf(*lscanline), y);
			outz = zbuffer.get_zbuffer((int16_t)truncf(*lscanline), y);
			while (sl.finished() == false)
			{
				if (zbuffer.test_update(outz, sl.value()))
				{
					m3d_vertex tmp;
					tmp.tposition = (m3d_point &)pts.value();
					tmp.tnormal = norm.value();
					m3d_illum::inst().ambient_lighting(tmp, obj, world, colors[0]);
					m3d_illum::inst().diffuse_lighting(tmp, obj, world, colors[0]);
					m3d_illum::inst().specular_lighting(tmp, obj, world, colors[0]);
					m3d_color total = colors[0].Kamb + colors[0].Kdiff + colors[0].Kspec;
					*output = total.getColor();
				}
				++output;
				++outz;
				sl.step();
				norm.step();
			}
		}
		lscanline++;
		rscanline++;
		lzscanline++;
		rzscanline++;
		lvscanline++;
		rvscanline++;
		lwscanline++;
		rwscanline++;
		y++;
	}
}