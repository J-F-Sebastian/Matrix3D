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
	int16_t runlen[3];

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

				/*
				 * Store x in this order: A to C, A to B, B to C
				 *
				 *           A
				 *
				 *       B
				 *
				 *
				 *              C
				 */

				// runlen[0] = runlen[1] + runlen[2] - 1
				// due to the overlapping value when computing runlen 1 and 2
				runlen[0] = store_scanlines((int16_t)vtx[0]->scrposition.x,
							    (int16_t)vtx[0]->scrposition.y,
							    (int16_t)vtx[2]->scrposition.x,
							    (int16_t)vtx[2]->scrposition.y);

				runlen[1] = store_scanlines((int16_t)vtx[0]->scrposition.x,
							    (int16_t)vtx[0]->scrposition.y,
							    (int16_t)vtx[1]->scrposition.x,
							    (int16_t)vtx[1]->scrposition.y,
							    runlen[0]);

				runlen[2] = store_scanlines((int16_t)vtx[1]->scrposition.x,
							    (int16_t)vtx[1]->scrposition.y,
							    (int16_t)vtx[2]->scrposition.x,
							    (int16_t)vtx[2]->scrposition.y,
							    runlen[0] + runlen[1] - 1);

				triangle_fill_flat(vtx, runlen, color);
			}
		}
	}

	// Present the rendered lines
	display->show_buffer();
}

void m3d_renderer_flat::triangle_fill_flat(m3d_vertex *vtx[], int16_t *runlen, m3d_color &color)
{
	uint32_t *output;
	float *outz;
	int16_t y = (int16_t)vtx[0]->scrposition.y;
	int steps;
	int16_t *leftx, *rightx;
	m3d_interpolation_float leftz(runlen[1]), rightz(runlen[1]);
	m3d_interpolation_float leftz2(runlen[2]), rightz2(runlen[2]);

	/*
	 * check x values to understand who's the left half and who's the right.
	 * First run length is ALWAYS the longest run.
	 */
	if (scanline[runlen[0] / 2] <= scanline[runlen[0] + runlen[1] - 1])
	{
		leftx = scanline;
		rightx = scanline + runlen[0];
		leftz.init(runlen[1], vtx[0]->prjposition.myvector[Z_C], vtx[0]->prjposition.myvector[Z_C] + (vtx[2]->prjposition.myvector[Z_C] - vtx[0]->prjposition.myvector[Z_C]) / 2);
		rightz.init(runlen[1], vtx[0]->prjposition.myvector[Z_C], vtx[1]->prjposition.myvector[Z_C]);
		leftz2.init(runlen[2], vtx[0]->prjposition.myvector[Z_C] + (vtx[2]->prjposition.myvector[Z_C] - vtx[0]->prjposition.myvector[Z_C]) / 2, vtx[2]->prjposition.myvector[Z_C]);
		rightz2.init(runlen[2], vtx[1]->prjposition.myvector[Z_C], vtx[2]->prjposition.myvector[Z_C]);
	}
	else
	{
		leftx = scanline + runlen[0];
		rightx = scanline;
		leftz.init(runlen[1], vtx[0]->prjposition.myvector[Z_C], vtx[1]->prjposition.myvector[Z_C]);
		rightz.init(runlen[1], vtx[0]->prjposition.myvector[Z_C], vtx[0]->prjposition.myvector[Z_C] + (vtx[2]->prjposition.myvector[Z_C] - vtx[0]->prjposition.myvector[Z_C]) / 2);
		leftz2.init(runlen[2], vtx[1]->prjposition.myvector[Z_C], vtx[2]->prjposition.myvector[Z_C]);
		rightz2.init(runlen[2], vtx[0]->prjposition.myvector[Z_C] + (vtx[2]->prjposition.myvector[Z_C] - vtx[0]->prjposition.myvector[Z_C]) / 2, vtx[2]->prjposition.myvector[Z_C]);
	}

	while (runlen[1]--)
	{
		steps = *rightx - *leftx + 1;
		m3d_interpolation_float sl(steps, leftz.value(), rightz.value());
		output = display->get_video_buffer(*leftx, y);
		outz = zbuffer.get_zbuffer(*leftx, y);
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
		leftx++;
		rightx++;
		leftz.step();
		rightz.step();
		y++;
	}

	// Skip first run, it overlaps
	leftz2.step();
	rightz2.step();
	runlen[2]--;

	while (runlen[2]--)
	{
		steps = *rightx - *leftx + 1;
		m3d_interpolation_float sl(steps, leftz2.value(), rightz2.value());
		output = display->get_video_buffer(*leftx, y);
		outz = zbuffer.get_zbuffer(*leftx, y);
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
		leftx++;
		rightx++;
		leftz2.step();
		rightz2.step();
		y++;
	}
}

/*
 * FLAT RENDERING WITH FLOATING POINT IMPLEMENTATION
 */

void m3d_renderer_flatf::render(m3d_world &world)
{
	unsigned i, j;
	m3d_vertex *vtx[3];
	m3d_render_color colors[3];
	m3d_color color;

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

// If this thing prove to be faster, refactor and remove integer scanlines
void m3d_renderer_flatf::triangle_fill_flat(m3d_vertex *vtx[], m3d_color &color)
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
	int fillrunlen;
	int16_t y = (int16_t)vtx[0]->scrposition.y;
	float *lscanline, *rscanline;
	float *lzscanline, *rzscanline;
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
	}
	else
	{
		store_fscanlines(runlen1, p3, p4);
		store_fscanlines(runlen2, p4, p5, runlen1 - 1);
		store_fscanlines(runlen0, p3, p5, runlen1 + runlen2 - 1);
		store_zscanlines(runlen1, p0, p1);
		store_zscanlines(runlen2, p1, p2, runlen1 - 1);
		store_zscanlines(runlen0, p0, p2, runlen1 + runlen2 - 1);
	}

	lscanline = fscanline;
	rscanline = fscanline + runlen0;
	lzscanline = zscanline;
	rzscanline = zscanline + runlen0;

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