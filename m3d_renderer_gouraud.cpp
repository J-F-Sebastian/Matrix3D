#include "m3d_renderer_gouraud.hh"
#include "m3d_interp.hh"

/*
 * GOURAUD SHADING RENDERER
 */

void m3d_renderer_shaded_gouraud::store_iscanlines(unsigned runlen, m3d_color &val1, m3d_color &val2, unsigned start)
{
	m3d_interpolation_color run(runlen, val1, val2);
	run.valuearray(iscanline + start);
}

void m3d_renderer_shaded_gouraud::triangle_fill_shaded(m3d_render_object &obj, m3d_vertex *vtx[], m3d_world &world)
{
	uint32_t *output;
	float *outz;
	float p0 = vtx[0]->prjposition[Z_C];
	float p1 = vtx[1]->prjposition[Z_C];
	float p2 = vtx[2]->prjposition[Z_C];
	float p3 = (float)vtx[0]->scrposition.x + 0.5f;
	float p4 = (float)vtx[1]->scrposition.x + 0.5f;
	float p5 = (float)vtx[2]->scrposition.x + 0.5f;
	unsigned runlen0 = vtx[2]->scrposition.y - vtx[0]->scrposition.y + 1;
	unsigned runlen1 = vtx[1]->scrposition.y - vtx[0]->scrposition.y + 1;
	unsigned runlen2 = vtx[2]->scrposition.y - vtx[1]->scrposition.y + 1;
	m3d_color c0 = colors[0].Kamb + colors[0].Kdiff;
	m3d_color c1 = colors[1].Kamb + colors[1].Kdiff;
	m3d_color c2 = colors[2].Kamb + colors[2].Kdiff;
	int fillrunlen;
	int16_t y = (int16_t)vtx[0]->scrposition.y;
	float *lscanline, *rscanline;
	float *lzscanline, *rzscanline;
	uint32_t *liscanline, *riscanline;
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
		store_iscanlines(runlen0, c0, c2);
		store_iscanlines(runlen1, c0, c1, runlen0);
		store_iscanlines(runlen2, c1, c2, runlen0 + runlen1 - 1);
	}
	else
	{
		store_fscanlines(runlen1, p3, p4);
		store_fscanlines(runlen2, p4, p5, runlen1 - 1);
		store_fscanlines(runlen0, p3, p5, runlen1 + runlen2 - 1);
		store_zscanlines(runlen1, p0, p1);
		store_zscanlines(runlen2, p1, p2, runlen1 - 1);
		store_zscanlines(runlen0, p0, p2, runlen1 + runlen2 - 1);
		store_iscanlines(runlen1, c0, c1);
		store_iscanlines(runlen2, c1, c2, runlen1 - 1);
		store_iscanlines(runlen0, c0, c2, runlen1 + runlen2 - 1);
	}

	lscanline = fscanline;
	rscanline = fscanline + runlen0;
	lzscanline = zscanline;
	rzscanline = zscanline + runlen0;
	liscanline = iscanline;
	riscanline = iscanline + runlen0;

	while (runlen0--)
	{
		fillrunlen = lroundf(*rscanline - *lscanline) + 1;
		if (fillrunlen)
		{
			m3d_interpolation_float sl(fillrunlen, *lzscanline, *rzscanline);
			m3d_color ca(*liscanline), cb(*riscanline);
			m3d_interpolation_color lint(fillrunlen, ca, cb);
			output = display->get_video_buffer((int16_t)truncf(*lscanline), y);
			outz = zbuffer.get_zbuffer((int16_t)truncf(*lscanline), y);
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