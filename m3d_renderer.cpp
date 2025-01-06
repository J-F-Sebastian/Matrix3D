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
	delete fscanline;
	delete zscanline;
}

m3d_renderer::m3d_renderer(m3d_display *disp) : display(disp), zbuffer((int16_t)disp->get_xmax(), (int16_t)disp->get_ymax())
{
	scanline = new int16_t[display->get_ymax() * 2];
	fscanline = new float[display->get_ymax() * 2];
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

/*
 * run-length scanline computation
 * This is the same algorithm for run-length lines, but will collect endpoint
 * values only.
 * Some keynotes: y0 is always lesser than or equal to y1.
 * Thanks to Michael Abrash for his invaluable books....
 */
int16_t m3d_renderer::store_scanlines(int16_t x0,
				      int16_t y0,
				      int16_t x1,
				      int16_t y1,
				      unsigned start)
{
	int16_t advancex, deltax, deltay;
	int16_t wholestep, adjup, adjdown;
	int16_t errorterm, inipixcount, finpixcount;
	int16_t runlen;
	uint32_t cnt, fill;
	int16_t val = x0;
	int16_t *stor = scanline + start;

	if (x0 > x1)
	{
		deltax = x0 - x1;
		advancex = -1;
	}
	else
	{
		deltax = x1 - x0;
		advancex = 1;
	}

	deltay = y1 - y0;

	if (!deltay)
	{
		/* horizontal line, one value only */
		*stor++ = val;
		return 1;
	}

	if (!deltax)
	{
		/* vertical line, all x values are the same... */
		for (cnt = 0; cnt <= (uint32_t)deltay; cnt++)
		{
			*stor++ = val;
		}
		return (deltay + 1);
	}

	if (deltax == deltay)
	{
		/* diagonal line, x advances at every step with the same value */
		*stor++ = val;
		for (cnt = 0; cnt < (uint32_t)deltay; cnt++)
		{
			val += advancex;
			*stor++ = val;
		}
		return (deltay + 1);
	}

	if (deltax < deltay)
	{
		/* Y major line */

		/* minimum # of pixels in a run */
		wholestep = deltay / deltax;

		adjup = (deltay % deltax) * 2;
		adjdown = deltax * 2;
		errorterm = (deltay % deltax) - adjdown;

		inipixcount = (wholestep / 2) + 1;
		finpixcount = inipixcount;

		if (!adjup && !(wholestep & 1))
		{
			inipixcount--;
		}

		if (wholestep & 1)
		{
			errorterm += deltax;
		}

		/* set first run of x0 */
		for (fill = 0; fill < (uint32_t)inipixcount; fill++)
		{
			*stor++ = val;
		}
		val += advancex;

		/* set all full runs, x0 is advanced at every outer loop */
		for (cnt = 0; cnt < (uint32_t)(deltax - 1); cnt++)
		{
			runlen = wholestep;
			errorterm += adjup;

			if (errorterm > 0)
			{
				runlen++;
				errorterm -= adjdown;
			}

			for (fill = 0; fill < (uint32_t)runlen; fill++)
			{
				*stor++ = val;
			}
			val += advancex;
		}

		/* set the final run of pixels, x0 does not need to be updated */
		for (fill = 0; fill < (uint32_t)finpixcount; fill++)
		{
			*stor++ = val;
		}
	}
	else
	{
		/* X major */

		/* minimum # of pixels in a run */
		wholestep = deltax / deltay;

		adjup = (deltax % deltay) * 2;
		adjdown = deltay * 2;
		errorterm = (deltax % deltay) - adjdown;

		inipixcount = (wholestep / 2) + 1;
		finpixcount = inipixcount;

		if (!adjup && !(wholestep & 1))
		{
			inipixcount--;
		}

		if (wholestep & 1)
		{
			errorterm += deltay;
		}

		/*
		 * When X is major axis, we should draw lines along x, which means,
		 * we have multiple x values with the same y value.
		 * If advancex is negative, we are computing and ending edge, i.e. we
		 * need to store the final x0 values, not the initial, or the line
		 * drawing algorithm will fail filling in the line pixels.
		 */
		if (advancex > 0)
		{
			*stor++ = val;
			val += inipixcount;

			/* set all full runs */
			for (cnt = 0; cnt < (uint32_t)(deltay - 1); cnt++)
			{
				runlen = wholestep;
				errorterm += adjup;

				if (errorterm > 0)
				{
					runlen++;
					errorterm -= adjdown;
				}
				*stor++ = val;
				val += runlen;
			}

			/* set the final run of pixels */
			*stor = val;
		}
		else
		{
			*stor++ = val;
			val -= inipixcount;

			/* set all full runs */
			for (cnt = 0; cnt < (uint32_t)(deltay - 1); cnt++)
			{
				runlen = wholestep;
				errorterm += adjup;

				if (errorterm > 0)
				{
					runlen++;
					errorterm -= adjdown;
				}
				*stor++ = val;
				val -= runlen;
			}

			/* set the final run of pixels */
			*stor = val;
		}
	}
	return (deltay + 1);
}

void m3d_renderer::store_fscanlines(unsigned runlen, float val1, float val2, unsigned start)
{
	m3d_interpolation_float run(runlen, val1, val2);
	run.valuearray(fscanline + start);
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
			vislist.push_front(itro);
	}

	if (vislist.size())
	{
		world.sort(vislist);
		zbuffer.reset();
	}
}
