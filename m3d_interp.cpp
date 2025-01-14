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
#include <climits>
#include <cstdint>
#include "m3d_interp.hh"

m3d_interpolation_float::m3d_interpolation_float(const unsigned int steps) : m3d_interpolation(steps), start(0.0f), val(0.0f), delta(0.0f) {}

m3d_interpolation_float::m3d_interpolation_float(const unsigned int steps, const float val1, const float val2) : m3d_interpolation(steps), start(val1), val(val1)
{
	if (steps == 1)
	{
		delta = 0.0f;
	}
	else
	{
		delta = (val2 - val1) / (float)(this->steps - 1);
	}
}

void m3d_interpolation_float::init(const unsigned int step, const float val1, const float val2)
{
	steps = (step > 0) ? step : 1;
	val = start = val1;

	if (step == 1)
	{
		delta = 0.0f;
	}
	else
	{
		delta = (val2 - val1) / (float)(steps - 1);
	}
}

void m3d_interpolation_float::step()
{
	if (steps)
	{
		val += delta;
		steps--;
	}
}

void m3d_interpolation_float::valuearray(float *out)
{
	while (steps--)
	{
		*out++ = val;
		val += delta;
	}
}

m3d_interpolation_short::m3d_interpolation_short(const unsigned int steps) : m3d_interpolation(steps), start(0), val(0), delta(0) {}

m3d_interpolation_short::m3d_interpolation_short(const unsigned int steps, const short val1, const short val2) : m3d_interpolation(steps), start(val1 << 16), val(val1 << 16)
{
	if (steps == 1)
	{
		delta = 0;
	}
	else
	{
		delta = (int)(val2 - val1);
		delta <<= 16;
		delta /= (int)(this->steps - 1);
	}
}

void m3d_interpolation_short::init(const unsigned int step, const short val1, const short val2)
{
	steps = (step > 0) ? step : 1;
	val = start = val1 << 16;

	if (steps == 1)
	{
		delta = 0;
	}
	else
	{
		delta = (int)(val2 - val1);
		delta <<= 16;
		delta /= (int)(this->steps - 1);
	}
}

void m3d_interpolation_short::step()
{
	if (steps)
	{
		val += delta;
		steps--;
	}
}

void m3d_interpolation_short::valuearray(short *out)
{
	while (steps--)
	{
		*out++ = (short)((val + 0x8000) >> 16);
		val += delta;
	}
}

m3d_interpolation_color::m3d_interpolation_color(const unsigned int steps) : m3d_interpolation(steps)
{
	start.color = val.color = 0;
	for (unsigned i = m3d_color::B_CHANNEL; i < m3d_color::A_CHANNEL; i++)
	{
		delta[i] = acc[i] = 0;
	}
}

m3d_interpolation_color::m3d_interpolation_color(const unsigned int steps, m3d_color &val1, m3d_color &val2) : m3d_interpolation(steps)
{
	start.color = val.color = val1.getColor();

	if (steps == 1)
	{
		for (unsigned i = m3d_color::B_CHANNEL; i < m3d_color::A_CHANNEL; i++)
		{
			delta[i] = acc[i] = 0;
		}
	}
	else
	{
		for (unsigned i = m3d_color::B_CHANNEL; i < m3d_color::A_CHANNEL; i++)
		{
			if (val2.getChannel(i) >= val1.getChannel(i))
			{
				delta[i] = (unsigned)(val2.getChannel(i) - val1.getChannel(i));
				delta[i] <<= 24;
				delta[i] /= (this->steps - 1);
			}
			else
			{
				delta[i] = (unsigned)(val1.getChannel(i) - val2.getChannel(i));
				delta[i] <<= 24;
				delta[i] /= (this->steps - 1);
				delta[i] = UINT_MAX - delta[i];
			}
			acc[i] = (unsigned)val.channels[i] << 24;
			acc[i] += 1U << 23;
		}
	}
}

void m3d_interpolation_color::init(const unsigned int step, m3d_color &val1, m3d_color &val2)
{
	steps = (step > 0) ? step : 1;

	start.color = val.color = val1.getColor();
	if (steps == 1)
	{
		for (unsigned i = m3d_color::B_CHANNEL; i < m3d_color::A_CHANNEL; i++)
		{
			delta[i] = acc[i] = 0;
		}
	}
	else
	{
		for (unsigned i = m3d_color::B_CHANNEL; i < m3d_color::A_CHANNEL; i++)
		{
			if (val2.getChannel(i) >= val1.getChannel(i))
			{
				delta[i] = (unsigned)(val2.getChannel(i) - val1.getChannel(i));
				delta[i] <<= 24;
				delta[i] /= (this->steps - 1);
			}
			else
			{
				delta[i] = (unsigned)(val1.getChannel(i) - val2.getChannel(i));
				delta[i] <<= 24;
				delta[i] /= (this->steps - 1);
				delta[i] = UINT_MAX - delta[i];
			}
			acc[i] = (unsigned)val.channels[i] << 24;
			acc[i] += 1U << 23;
		}
	}
}

void m3d_interpolation_color::step()
{
	if (steps)
	{
		for (unsigned i = m3d_color::B_CHANNEL; i < m3d_color::A_CHANNEL; i++)
		{
			acc[i] += delta[i];
			val.channels[i] = acc[i] >> 24;
		}
		steps--;
	}
}

void m3d_interpolation_color::valuearray(uint32_t *out)
{
	while (steps--)
	{
		*out++ = val.color;
		for (unsigned i = m3d_color::B_CHANNEL; i < m3d_color::A_CHANNEL; i++)
		{
			acc[i] += delta[i];
			val.channels[i] = (acc[i] >> 24) & UCHAR_MAX;
		}
	}
}

m3d_interpolation_float_perspective::m3d_interpolation_float_perspective(const unsigned int steps, float z1, float z2, float val1, float val2) : m3d_interpolation(steps), val1(val1), val(val1)
{
	if (steps == 1)
	{
		deltav = deltazinv = z1inv = 0.0f;
	}
	else
	{
		deltav = (val2 / z2 - val1 / z1) / (float)(this->steps - 1);
		deltazinv = (1.0f / z2 - 1.0f / z1) / (float)(this->steps - 1);
		z1inv = 1.0f / z1;
	}
}

void m3d_interpolation_float_perspective::step()
{
	if (steps)
	{
		z1inv += deltazinv;
		val1 += deltav;
		val = val1 / z1inv;
		steps--;
	}
}

void m3d_interpolation_float_perspective::valuearray(float *out)
{
	while (steps--)
	{
		*out++ = val;
		z1inv += deltazinv;
		val1 += deltav;
		val = val1 / z1inv;
	}
}

m3d_interpolation_vector::m3d_interpolation_vector(const unsigned int steps, m3d_vector &v1, m3d_vector &v2) : m3d_interpolation(steps), vector1(v1), val(v1)
{
	if (steps == 1)
	{
		deltavector.myvector[X_C] = deltavector.myvector[Y_C] = deltavector.myvector[Z_C] = deltavector.myvector[T_C] = 0.0f;
	}
	else
	{
		deltavector.myvector[X_C] = (v2.myvector[X_C] - v1.myvector[X_C]) / (float)(this->steps - 1);
		deltavector.myvector[Y_C] = (v2.myvector[Y_C] - v1.myvector[Y_C]) / (float)(this->steps - 1);
		deltavector.myvector[Z_C] = (v2.myvector[Z_C] - v1.myvector[Z_C]) / (float)(this->steps - 1);
	}
}

void m3d_interpolation_vector::step()
{
	if (steps)
	{
		vector1.add(deltavector);
		val = vector1;
		steps--;
	}
}

void m3d_interpolation_vector::valuearray(m3d_vector *out)
{
	while (steps--)
	{
		*out++ = val;
		vector1.add(deltavector);
		val = vector1;
	}
}

m3d_interpolation_vector_perspective::m3d_interpolation_vector_perspective(const unsigned int steps, float z1, float z2, m3d_vector &v1, m3d_vector &v2) : m3d_interpolation(steps), vector1(v1), val(v1)
{
	if (steps == 1)
	{
		deltavector.myvector[X_C] = deltavector.myvector[Y_C] = deltavector.myvector[Z_C] = deltavector.myvector[T_C] = 0.0f;
		deltazinv = z1inv = 0.0f;
	}
	else
	{
		deltavector.myvector[X_C] = (v2.myvector[X_C] / z2 - v1.myvector[X_C] / z1) / (float)(this->steps - 1);
		deltavector.myvector[Y_C] = (v2.myvector[Y_C] / z2 - v1.myvector[Y_C] / z1) / (float)(this->steps - 1);
		deltavector.myvector[Z_C] = (v2.myvector[Z_C] / z2 - v1.myvector[Z_C] / z1) / (float)(this->steps - 1);
		deltazinv = (1.0f / z2 - 1.0f / z1) / (float)(this->steps - 1);
		z1inv = 1.0f / z1;
	}
}

void m3d_interpolation_vector_perspective::step()
{
	if (steps)
	{
		z1inv += deltazinv;
		vector1.add(deltavector);
		val = vector1;
		val.scale(1.0f / z1inv);
		steps--;
	}
}

void m3d_interpolation_vector_perspective::valuearray(m3d_vector *out)
{
	while (steps--)
	{
		*out++ = val;
		z1inv += deltazinv;
		vector1.add(deltavector);
		val = vector1;
		val.scale(1.0f / z1inv);
	}
}