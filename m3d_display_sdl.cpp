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

#include <iostream>

using namespace std;

#include "m3d_display.hh"

m3d_display::m3d_display(int xres, int yres)
{
	SDL_version linked;
	SDL_DisplayMode mode;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
	}

	SDL_GetVersion(&linked);

	cout << "SDL version is " << (int)linked.major << "." << (int)linked.minor << "." << (int)linked.patch << endl;

	// create a new window
	window = SDL_CreateWindow("Matrix3D",
				  SDL_WINDOWPOS_UNDEFINED,
				  SDL_WINDOWPOS_UNDEFINED,
				  xres,
				  yres,
				  SDL_WINDOW_SHOWN);

	if (!window)
	{
		cout << "Unable to create " << xres << "x" << yres << " window: " << SDL_GetError() << endl;
	}

	screenSurface = SDL_GetWindowSurface(window);

	if (!screenSurface)
	{
		cout << "Unable to get buffer : " << SDL_GetError() << endl;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (!renderer)
	{
		cout << "Unable to create accelerated renderer :" << SDL_GetError() << endl;
		renderer = SDL_CreateSoftwareRenderer(screenSurface);
	}

	if (!renderer)
	{
		cout << "Unable to create software renderer :" << SDL_GetError() << endl;
	}

	if (SDL_GetCurrentDisplayMode(0, &mode))
	{
		cout << "Unable to get display mode :" << SDL_GetError() << endl;
	}
	else
	{
		cout << "Display Resolution " << mode.w << " x " << mode.h << endl
		     << "format: " << endl
		     << "pixel type     : " << SDL_PIXELTYPE(mode.format) << endl
		     << "pixel order    : " << SDL_PIXELORDER(mode.format) << endl
		     << "pixel layout   : " << SDL_PIXELLAYOUT(mode.format) << endl
		     << "bits per pixel : " << SDL_BITSPERPIXEL(mode.format) << endl
		     << "bytes per pixel: " << SDL_BYTESPERPIXEL(mode.format) << endl
		     << "alpha channel  : " << SDL_ISPIXELFORMAT_ALPHA(mode.format) << endl
		     << "format         : " << SDL_GetPixelFormatName(mode.format) << endl
		     << "---------------------------------------" << endl;
	}

	if (SDL_GetWindowDisplayMode(window, &mode))
	{
		cout << "Unable to get window mode :" << SDL_GetError() << endl;
	}
	else
	{
		cout << "Window Resolution " << mode.w << " x " << mode.h << endl
		     << "format: " << endl
		     << "pixel type     : " << SDL_PIXELTYPE(mode.format) << endl
		     << "pixel order    : " << SDL_PIXELORDER(mode.format) << endl
		     << "pixel layout   : " << SDL_PIXELLAYOUT(mode.format) << endl
		     << "bits per pixel : " << SDL_BITSPERPIXEL(mode.format) << endl
		     << "bytes per pixel: " << SDL_BYTESPERPIXEL(mode.format) << endl
		     << "alpha channel  : " << SDL_ISPIXELFORMAT_ALPHA(mode.format) << endl
		     << "format         : " << SDL_GetPixelFormatName(mode.format) << endl
		     << "---------------------------------------" << endl;
	}

	// This stuff must be reviewed for proper failure handling
	xmax = mode.w;
	ymax = mode.h;

	// Fill the surface black
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
}

/** Default destructor */
m3d_display::~m3d_display()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

uint32_t *m3d_display::get_video_buffer(int16_t x0, int16_t y0)
{
	return (uint32_t *)(screenSurface->pixels) + (y0 * screenSurface->w) + x0;
}

void m3d_display::set_color(uint8_t red, uint8_t green, uint8_t blue)
{
	SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE);
}

void m3d_display::fill_buffer()
{
	SDL_RenderClear(renderer);
}

void m3d_display::show_buffer()
{
	// Present the buffer
	SDL_UpdateWindowSurface(window);
}

void m3d_display::clear_buffer()
{
	SDL_FillRect(screenSurface, &screenSurface->clip_rect, 0);
}

void m3d_display::clear_renderer()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
}

void m3d_display::draw_lines(SDL_Point pts[], unsigned ptsnum)
{
	if (SDL_RenderDrawLines(renderer, pts, ptsnum) < 0)
	{
		cout << "SDL error: " << SDL_GetError() << endl;
	}
}

void m3d_display::show_renderer()
{
	// Present the rendered lines
	SDL_RenderPresent(renderer);
}