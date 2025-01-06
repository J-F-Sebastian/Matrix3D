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

#ifndef M3D_ZBUFFER_HH_INCLUDED
#define M3D_ZBUFFER_HH_INCLUDED

#include <cstdint>

class m3d_zbuffer
{
public:
	m3d_zbuffer() : zbuffer(nullptr), size(0), xres(0), yres(0) {}
	m3d_zbuffer(int16_t xres, int16_t yres) : size(xres * yres), xres(xres), yres(yres)
	{
		zbuffer = new float[size];
		reset();
	}

	~m3d_zbuffer()
	{
		if (zbuffer)
			delete zbuffer;
	}

	void reset(void)
	{
		for (int i = 0; i < size; i++)
			zbuffer[i] = 1.0f;
	}

	bool test_update(int16_t x0, int16_t y0, float z)
	{
		float *zb = get_zbuffer(x0, y0);
		if (z <= *zb)
		{
			*zb = z;
			return true;
		}
		return false;
	}

	bool test_update(float *zbuf, float z)
	{
		if (z <= *zbuf)
		{
			*zbuf = z;
			return true;
		}
		return false;
	}

	inline float *get_zbuffer(int16_t x0, int16_t y0)
	{
		return zbuffer + (y0 * xres) + x0;
	}

private:
	float *zbuffer;
	int size;
	int16_t xres, yres;
};

#endif