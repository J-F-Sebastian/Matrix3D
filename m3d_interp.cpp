#include <iostream>
#include <climits>
#include <cstdint>
#include "m3d_interp.hh"

m3d_interpolation_float::m3d_interpolation_float(const int steps) : m3d_interpolation(steps), start(0.0f), val(0.0f), delta(0.0f) {}

m3d_interpolation_float::m3d_interpolation_float(const int steps, const float val1, const float val2) : m3d_interpolation(steps), start(val1), val(val1)
{
	delta = (val2 - val1) / (float)this->steps;
}

void m3d_interpolation_float::init(const int step, const float val1, const float val2)
{
	steps = (step > 0) ? step : 1;
	start = val1;
	val = val1;
	delta = (val2 - val1) / (float)steps;
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

m3d_interpolation_short::m3d_interpolation_short(const int steps) : m3d_interpolation(steps), start(0), val(0), delta(0) {}

m3d_interpolation_short::m3d_interpolation_short(const int steps, const short val1, const short val2) : m3d_interpolation(steps), start(val1 << 16), val(val1 << 16)
{
	delta = (int)(val2 - val1);
	delta <<= 16;
	delta /= this->steps;
}

void m3d_interpolation_short::init(const int step, const short val1, const short val2)
{
	steps = (step > 0) ? step : 1;
	start = val1 << 16;
	val = val1 << 16;

	delta = (int)(val2 - val1);
	delta <<= 16;
	delta /= steps;
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

m3d_interpolation_color::m3d_interpolation_color(const int steps) : m3d_interpolation(steps)
{
	start.color = val.color = 0;
	delta[0] = delta[1] = delta[2] = 0;
	acc[0] = acc[1] = acc[2] = 0;
}

m3d_interpolation_color::m3d_interpolation_color(const int steps, m3d_color &val1, m3d_color &val2) : m3d_interpolation(steps)
{
	start.color = val.color = val1.getColor();
	for (unsigned i = m3d_color::B_CHANNEL; i < m3d_color::A_CHANNEL; i++)
	{
		if (val2.getChannel(i) >= val1.getChannel(i))
		{
			delta[i] = val2.getChannel(i) - val1.getChannel(i);
			delta[i] <<= 24;
			delta[i] /= this->steps;
		}
		else
		{
			delta[i] = val1.getChannel(i) - val2.getChannel(i);
			delta[i] <<= 24;
			delta[i] /= this->steps;
			delta[i] = UINT_MAX - delta[i];
		}
		acc[i] = val.channels[i] << 24;
		acc[i] += 1 << 23;
	}
}

void m3d_interpolation_color::init(const int step, m3d_color &val1, m3d_color &val2)
{
	steps = (step > 0) ? step : 1;
	start.color = val.color = val1.getColor();
	for (unsigned i = m3d_color::B_CHANNEL; i < m3d_color::A_CHANNEL; i++)
	{
		if (val2.getChannel(i) >= val1.getChannel(i))
		{
			delta[i] = val2.getChannel(i) - val1.getChannel(i);
			delta[i] <<= 24;
			delta[i] /= steps;
		}
		else
		{
			delta[i] = val1.getChannel(i) - val2.getChannel(i);
			delta[i] <<= 24;
			delta[i] /= steps;
			delta[i] = UINT_MAX - delta[i];
		}
		acc[i] = val.channels[i] << 24;
		acc[i] += 1 << 23;
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

m3d_interpolation_float_perspective::m3d_interpolation_float_perspective(int steps, float z1, float z2, float val1, float val2) : m3d_interpolation(steps), val1(val1)
{
	deltav = (val2 / z2 - val1 / z1) / (float)this->steps;
	deltazinv = (1.0f / z2 - 1.0f / z1) / (float)this->steps;
	z1inv = 1.0f / z1;
	val = val1;
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