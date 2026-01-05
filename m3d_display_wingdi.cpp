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

#include "m3d_display_wingdi.hh"

using namespace std;

m3d_display_wingdi::m3d_display_wingdi(HWND hwnd, int xres, int yres) : m3d_display(xres, yres), hwnd(hwnd)
{
	LPBITMAPINFO info;

	cout << "Ciao" << endl;

	hdc = GetDC(hwnd);
	if (!hdc)
	{
		cerr << "Could not get the main window DC!" << endl;
	}

	hdcDIB = CreateCompatibleDC(hdc);
	if (!hdcDIB)
	{
		cerr << "Could not create the backbuffer DC!" << endl;
	}

	hDIB = CreateCompatibleBitmap(hdc, 1, 1);
	if (!hDIB)
	{
		cerr << "Could not create the bitmap!" << endl;
	}

	info = (LPBITMAPINFO)calloc(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD), 1);
	info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	if (!GetDIBits(hdc, hDIB, 0, 0, NULL, info, DIB_RGB_COLORS))
	{
		cerr << "Could not get the bitmap color table!" << endl;
	}

	if (!GetDIBits(hdc, hDIB, 0, 0, NULL, info, DIB_RGB_COLORS))
	{
		cerr << "Could not get the bitmap pixel format!" << endl;
	}

	if (!DeleteObject(hDIB))
	{
		cerr << "Could not delete the bitmap!" << endl;
	}

	// Just a check ...
	if (info->bmiHeader.biCompression == BI_BITFIELDS)
	{
		int bpp;
		uint32_t *masks;

		bpp = info->bmiHeader.biPlanes * info->bmiHeader.biBitCount;
		masks = (uint32_t *)((uint8_t *)info + info->bmiHeader.biSize);

		if ((bpp != 32) ||
		    (masks[0] != 0x00FF0000) ||
		    (masks[1] != 0x0000FF00) ||
		    (masks[2] != 0x000000FF))
		{
			// We are not fine
			cerr << "Format is not ARGB 32 Bits!" << endl;
		}
	}

	info->bmiHeader.biWidth = xres;
	info->bmiHeader.biHeight = -yres; // negative for topdown bitmap
	info->bmiHeader.biSizeImage = xres * yres * sizeof(uint32_t);

	hDIB = CreateDIBSection(hdc, info, DIB_RGB_COLORS, &pixels, NULL, 0);
	free(info);

	if (!SelectObject(hdcDIB, hDIB))
	{
		cerr << "Could not select the DIB for the backbuffer!" << endl;
	}

	// Fill the surface black
	memset(pixels, 0, xres * yres * sizeof(uint32_t));
	BitBlt(hdc, 0, 0, xres, yres, hdcDIB, 0, 0, SRCCOPY);
}

/** Default destructor */
m3d_display_wingdi::~m3d_display_wingdi()
{
	if (!DeleteObject(hDIB))
	{
		cerr << "Could not delete the bitmap!" << endl;
	}

	if (!DeleteDC(hdcDIB))
	{
		cerr << "Could not delete the backbuffer DC!" << endl;
	}

	if (!ReleaseDC(hwnd, hdc))
	{
		cerr << "Could not release the main window DC!" << endl;
	}
}

uint32_t *m3d_display_wingdi::get_video_buffer(int x0, int y0)
{
	return (uint32_t *)(pixels) + (y0 * xmax) + x0;
}

void m3d_display_wingdi::set_color(uint8_t red, uint8_t green, uint8_t blue)
{
	color = RGB(red, green, blue);
}

void m3d_display_wingdi::fill_buffer()
{
	uint32_t *pix = (uint32_t *)pixels;
	while (pix < ((uint32_t *)pixels) + (xmax * ymax))
	{
		*pix++ = color;
	}
}

void m3d_display_wingdi::show_buffer()
{
	PAINTSTRUCT ps;
	// Present the buffer
	BeginPaint(hwnd, &ps);
	BitBlt(hdc, 0, 0, xmax, ymax, hdcDIB, 0, 0, SRCCOPY);
	EndPaint(hwnd, &ps);
}

void m3d_display_wingdi::clear_buffer()
{
	memset(pixels, 0, xmax * ymax * sizeof(uint32_t));
}

void m3d_display_wingdi::clear_renderer()
{
	clear_buffer();
	show_buffer();
}

void m3d_display_wingdi::draw_lines(struct m3d_display_point pts[], unsigned ptsnum)
{
	HPEN pen = CreatePen(PS_SOLID, 0, color);

	if (SelectObject(hdcDIB, pen))
	{
		MoveToEx(hdcDIB, pts[0].x, pts[0].y, NULL);

		for (unsigned i = 1; i < ptsnum; i++)
		{
			LineTo(hdcDIB, pts[i].x, pts[i].y);
		}

		if (!DeleteObject(pen))
		{
			cerr << "Could not delete the pen!" << endl;
		}
	}
}

void m3d_display_wingdi::show_renderer()
{
	PAINTSTRUCT ps;
	// Present the rendered lines
	BeginPaint(hwnd, &ps);
	BitBlt(hdc, 0, 0, xmax, ymax, hdcDIB, 0, 0, SRCCOPY);
	EndPaint(hwnd, &ps);
}