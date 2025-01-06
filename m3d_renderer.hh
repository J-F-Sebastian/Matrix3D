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

#ifndef M3D_RENDERER_H
#define M3D_RENDERER_H

#include <SDL.h>

#include "m3d_display.hh"
#include "m3d_world.hh"
#include "m3d_zbuffer.hh"
#include "m3d_illum.hh"

class m3d_renderer
{
public:
	/** Default constructor */
	m3d_renderer() : display(nullptr) {};
	m3d_renderer(m3d_display *disp);

	/** Default destructor */
	virtual ~m3d_renderer();

	virtual void render(m3d_world &world);

protected:
	// The window we are rendering to
	m3d_display *display;
	// The scanline buffer
	int16_t *scanline;
	float *fscanline;
	// The scanline depth buffer
	float *zscanline;
	// Z buffer
	m3d_zbuffer zbuffer;
	// The list of visible objects
	std::list<m3d_render_object *> vislist;

	/*
	 * Sorts an array of 3 points composing a triangle.
	 * Y is the Y on-screen coordinate of the 3 vertices of the triangle.
	 * The triangle is geometrically unchanged, its vertices are sorted in descending order
	 * (top of screen to bottom of screen).
	 * */
	void sort_triangle(m3d_vertex *vtx[3]);
	void sort_triangle(m3d_vertex *vtx[3], struct m3d_render_color *colors);

	/*
	 * Store scanlines into the scanline buffer, starting at position start inside the buffer.
	 * The scanline buffer carries the X coordinates of every linecomposing the polygon
	 * to be filled with horizontal lines (scan lines!).
	 * The Y coordinates are implicitely starting at the lowest y0, i.e. the caller MUST know
	 * the coordinates of the points composing the polygon and how they are sorted.
	 */
	int16_t store_scanlines(int16_t x0, int16_t y0, int16_t x1, int16_t y1, unsigned start = 0);

	/*
	 * Store scanlines into the float fscanline buffer, starting at position start inside the buffer.
	 * The fscanline buffer carries the X coordinates of every linecomposing the polygon
	 * to be filled with horizontal lines (scan lines!).
	 * The Y coordinates are implicitely starting at the lowest y0, i.e. the caller MUST know
	 * the coordinates of the points composing the polygon and how they are sorted.
	 */
	void store_fscanlines(unsigned runlen, float val1, float val2, unsigned start = 0);

	/*
	 * Store scanlines into the float zscanline buffer, starting at position start inside the buffer.
	 * The zscanline buffer carries the Z homogeneous coordinates of every linecomposing the polygon
	 * to be filled with horizontal lines (scan lines!).
	 * The Y coordinates are implicitely starting at the lowest y0, i.e. the caller MUST know
	 * the coordinates of the points composing the polygon and how they are sorted.
	 */
	void store_zscanlines(unsigned runlen, float val1, float val2, unsigned start = 0);

	uint32_t *get_video_buffer(int16_t x0, int16_t y0);

	void compute_visible_list_and_sort(m3d_world &world);
};

#endif // M3D_RENDERER_H
