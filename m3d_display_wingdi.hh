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

#ifndef M3D_DISPLAY_WINGDI_H
#define M3D_DISPLAY_WINGDI_H

#include <windows.h>
#include <wingdi.h>
#include <cstdint>

#include "m3d_display.hh"

class m3d_display_wingdi : public m3d_display
{
public:
	/** Default constructor */
	m3d_display_wingdi() : m3d_display(), hdc(0), hdcDIB(0), hDIB(0), pixels(0), color(0) {};
	m3d_display_wingdi(HWND m_hwnd, int xres, int yres);
	/** Default destructor */
	virtual ~m3d_display_wingdi();

	virtual uint32_t *get_video_buffer(int x0, int y0);

	/** Act directly on the surface buffer */
	virtual void fill_buffer(void);
	virtual void show_buffer(void);
	virtual void clear_buffer(void);

	/** Act on the renderer */
	virtual void set_color(uint8_t red, uint8_t green, uint8_t blue);
	virtual void draw_lines(m3d_display_point pts[], unsigned ptsnum);
	virtual void clear_renderer(void);
	virtual void show_renderer(void);

private:
	// The window we draw to!
	HWND hwnd;
	// The window context
	HDC hdc;
	// The backbuffer context
	HDC hdcDIB;
	// The DIB Object used to draw into the backbuffer
	HBITMAP hDIB;
	// Backbuffer memory pointer
	void *pixels;
	// Renderer color
	uint32_t color;
};

#endif