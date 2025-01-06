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
	m3d_interpolation_float_perspective run(runlen, val1, val2, z1, z2);
	run.valuearray(iscanline + start);
}

void m3d_renderer_shaded::triangle_fill_shaded(m3d_render_object &obj, m3d_vertex *vtx[], m3d_world &world)
{
	uint32_t *output;
	float *outz;
	float p0 = vtx[0]->prjposition[Z_C];
	float p1 = vtx[1]->prjposition[Z_C];
	float p2 = vtx[2]->prjposition[Z_C];
	float p3 = (float)vtx[0]->scrposition.x + 0.5f;
	float p4 = (float)vtx[1]->scrposition.x + 0.5f;
	float p5 = (float)vtx[2]->scrposition.x + 0.5f;
	float i0 = colors[0].ambint + colors[0].diffint;
	float i1 = colors[1].ambint + colors[1].diffint;
	float i2 = colors[2].ambint + colors[2].diffint;
	unsigned runlen0 = vtx[2]->scrposition.y - vtx[0]->scrposition.y + 1;
	unsigned runlen1 = vtx[1]->scrposition.y - vtx[0]->scrposition.y + 1;
	unsigned runlen2 = vtx[2]->scrposition.y - vtx[1]->scrposition.y + 1;
	int fillrunlen;
	int16_t y = (int16_t)vtx[0]->scrposition.y;
	float *lscanline, *rscanline;
	float *lzscanline, *rzscanline;
	float *liscanline, *riscanline;
	float lgradient = (p5 - p3) / (float)runlen0;
	float rgradient = (p4 - p3) / (float)runlen1;

	/*
	 * check x values to understand who's the left half and who's the right
	 */
	if (lgradient <= rgradient)
	{
		store_fscanlines(runlen0, p3, p5);
		store_fscanlines(runlen1, p3, p4, runlen0);
		store_fscanlines(runlen2, p4, p5, runlen0 + runlen1 - 1);
		store_zscanlines(runlen0, p0, p2);
		store_zscanlines(runlen1, p0, p1, runlen0);
		store_zscanlines(runlen2, p1, p2, runlen0 + runlen1 - 1);
		store_iscanlines(runlen0, p0, p2, i0, i2);
		store_iscanlines(runlen1, p0, p1, i0, i1, runlen0);
		store_iscanlines(runlen2, p1, p2, i0, i2, runlen0 + runlen1 - 1);
	}
	else
	{
		store_fscanlines(runlen1, p3, p4);
		store_fscanlines(runlen2, p4, p5, runlen1 - 1);
		store_fscanlines(runlen0, p3, p5, runlen1 + runlen2 - 1);
		store_zscanlines(runlen1, p0, p1);
		store_zscanlines(runlen2, p1, p2, runlen1 - 1);
		store_zscanlines(runlen0, p0, p2, runlen1 + runlen2 - 1);
		store_iscanlines(runlen1, p0, p1, i0, i1);
		store_iscanlines(runlen2, p1, p2, i1, i2, runlen1 - 1);
		store_iscanlines(runlen0, p0, p2, i0, i2, runlen1 + runlen2 - 1);
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
			m3d_interpolation_float_perspective sl(fillrunlen, *lzscanline, *rzscanline, *liscanline, *riscanline);
			output = display->get_video_buffer((int16_t)truncf(*lscanline), y);
			outz = zbuffer.get_zbuffer((int16_t)truncf(*lscanline), y);
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
