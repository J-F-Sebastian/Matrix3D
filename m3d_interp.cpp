#include "m3d_interp.hh"

m3d_interpolation_float::m3d_interpolation_float(const int steps) : m3d_interpolation(steps), start(0.0f), val(0.0f), delta(0.0f) {}

m3d_interpolation_float::m3d_interpolation_float(const int steps, const float val1, const float val2) : m3d_interpolation(steps), start(val1), val(val1)
{
	delta = (val2 - val1) / (float)steps;
}

void m3d_interpolation_float::init(const int step, const float val1, const float val2)
{
	steps = step;
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
	delta /= steps;
}

void m3d_interpolation_short::init(const int step, const short val1, const short val2)
{
	steps = step;
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

m3d_interpolation_color::m3d_interpolation_color(const int steps) : m3d_interpolation(steps), r(0), g(0), b(0), ra(0), ga(0), ba(0) { start.color = val.color = 0; }

m3d_interpolation_color::m3d_interpolation_color(const int steps, m3d_color &val1, m3d_color &val2) : m3d_interpolation(steps)
{
	start.color = val.color = val1.getColor();
	r = (int)val2.getChannel(m3d_color::R_CHANNEL) - (int)val1.getChannel(m3d_color::R_CHANNEL);
	g = (int)val2.getChannel(m3d_color::G_CHANNEL) - (int)val1.getChannel(m3d_color::G_CHANNEL);
	b = (int)val2.getChannel(m3d_color::B_CHANNEL) - (int)val1.getChannel(m3d_color::B_CHANNEL);
	r <<= 16;
	g <<= 16;
	b <<= 16;
	r /= steps;
	g /= steps;
	b /= steps;
	ra = ga = ba = 0;
}

void m3d_interpolation_color::init(const int step, m3d_color &val1, m3d_color &val2)
{
	steps = step;
	start.color = val.color = val1.getColor();
	r = (int)val2.getChannel(m3d_color::R_CHANNEL) - (int)val1.getChannel(m3d_color::R_CHANNEL);
	g = (int)val2.getChannel(m3d_color::G_CHANNEL) - (int)val1.getChannel(m3d_color::G_CHANNEL);
	b = (int)val2.getChannel(m3d_color::B_CHANNEL) - (int)val1.getChannel(m3d_color::B_CHANNEL);
	r <<= 16;
	g <<= 16;
	b <<= 16;
	r /= steps;
	g /= steps;
	b /= steps;
	ra = ga = ba = 0;
}

void m3d_interpolation_color::step()
{
	if (steps)
	{
		ra += r;
		ga += g;
		ba += b;
		val.channels[m3d_color::R_CHANNEL] += (uint8_t)(ra >> 16);
		val.channels[m3d_color::G_CHANNEL] += (uint8_t)(ga >> 16);
		val.channels[m3d_color::B_CHANNEL] += (uint8_t)(ba >> 16);
		steps--;
	}
}

void m3d_interpolation_color::valuearray(uint32_t *out)
{
	while (steps--)
	{
		*out++ = val.color;
		ra += r;
		ga += g;
		ba += b;
		val.channels[m3d_color::R_CHANNEL] += (uint8_t)(ra >> 16);
		val.channels[m3d_color::G_CHANNEL] += (uint8_t)(ga >> 16);
		val.channels[m3d_color::B_CHANNEL] += (uint8_t)(ba >> 16);
	}
}

m3d_interpolation_float_perspective::m3d_interpolation_float_perspective(int steps, float z1, float z2, float val1, float val2) : m3d_interpolation(steps), val1(val1)
{
	deltav = (val2 / z2 - val1 / z1) / (float)steps;
	deltazinv = (1.0f / z2 - 1.0f / z1) / (float)steps;
	z1inv = 1.0f / z1;
	val = z1 * val1;
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