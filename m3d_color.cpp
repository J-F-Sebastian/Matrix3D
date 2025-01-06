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
#include <cmath>

#include "m3d_color.hh"

using namespace std;

void m3d_color::change_channel_int(uint8_t &chan, float intensity)
{
	int temp;

	temp = (int)((float)chan * intensity);

	temp = (temp < 0) ? 0 : (temp > 255) ? 255
					     : temp;

	chan = (uint8_t)temp;
}

void m3d_color::setColor(uint8_t red,
			 uint8_t green,
			 uint8_t blue,
			 uint8_t alpha)
{
	mycolor.channels[R_CHANNEL] = red;
	mycolor.channels[G_CHANNEL] = green;
	mycolor.channels[B_CHANNEL] = blue;
	mycolor.channels[A_CHANNEL] = alpha;
}

void m3d_color::brighten(float intensity)
{
	if (std::signbit(intensity))
	{
		intensity = 0.0f;
	}

	change_channel_int(mycolor.channels[R_CHANNEL], intensity);
	change_channel_int(mycolor.channels[G_CHANNEL], intensity);
	change_channel_int(mycolor.channels[B_CHANNEL], intensity);
}

uint32_t m3d_color::brighten2(float intensity)
{
	m3d_color_channels myunion;

	if (std::signbit(intensity))
	{
		intensity = 0.0f;
	}

	myunion.color = mycolor.color;
	change_channel_int(myunion.channels[R_CHANNEL], intensity);
	change_channel_int(myunion.channels[G_CHANNEL], intensity);
	change_channel_int(myunion.channels[B_CHANNEL], intensity);

	return (myunion.color);
}

void m3d_color::brighten_channels(float chansint[])
{
	for (unsigned i = 0; i < A_CHANNEL; i++)
	{
		if (std::signbit(chansint[i]))
		{
			chansint[i] = 0.0f;
		}
	}

	change_channel_int(mycolor.channels[R_CHANNEL], chansint[R_CHANNEL]);
	change_channel_int(mycolor.channels[G_CHANNEL], chansint[G_CHANNEL]);
	change_channel_int(mycolor.channels[B_CHANNEL], chansint[B_CHANNEL]);
}

uint32_t m3d_color::brighten_channels2(float chansint[])
{
	m3d_color_channels myunion;

	for (unsigned i = B_CHANNEL; i < A_CHANNEL; i++)
	{
		if (std::signbit(chansint[i]))
		{
			chansint[i] = 0.0f;
		}
	}

	myunion.color = mycolor.color;
	change_channel_int(myunion.channels[R_CHANNEL], chansint[R_CHANNEL]);
	change_channel_int(myunion.channels[G_CHANNEL], chansint[G_CHANNEL]);
	change_channel_int(myunion.channels[B_CHANNEL], chansint[B_CHANNEL]);
	return (myunion.color);
}

void m3d_color::average_colors(m3d_color array[], unsigned num, m3d_color &out)
{
	unsigned sumr = 0, sumg = 0, sumb = 0;
	unsigned count = num;

	while (count--)
	{
		sumr += array[count].mycolor.channels[R_CHANNEL];
		sumg += array[count].mycolor.channels[G_CHANNEL];
		sumb += array[count].mycolor.channels[B_CHANNEL];
	}

	sumr /= num;
	sumg /= num;
	sumb /= num;

	if (sumr > 255)
		sumr = 255;

	if (sumg > 255)
		sumg = 255;

	if (sumb > 255)
		sumb = 255;

	out.setColor((uint8_t)sumr, (uint8_t)sumg, (uint8_t)sumb, 0);
}

/*
 * add_colors compute the sum for num items in array and store the result
 * in out.
 * Sums for R, G, B channels are clamped to 255.
 */
void m3d_color::add_colors(m3d_color array[], unsigned num, m3d_color &out)
{
	unsigned sumr = 0, sumg = 0, sumb = 0;

	while (num--)
	{
		sumr += array[num].mycolor.channels[R_CHANNEL];
		sumg += array[num].mycolor.channels[G_CHANNEL];
		sumb += array[num].mycolor.channels[B_CHANNEL];
	}

	if (sumr > 255)
		sumr = 255;

	if (sumg > 255)
		sumg = 255;

	if (sumb > 255)
		sumb = 255;

	out.setColor((uint8_t)sumr, (uint8_t)sumg, (uint8_t)sumb, 0);
}

void m3d_color::print()
{
#ifdef DEBUG
	cout << "Color RGBA ("
	     << (int)mycolor.channels[R_CHANNEL]
	     << ","
	     << (int)mycolor.channels[G_CHANNEL]
	     << ","
	     << (int)mycolor.channels[B_CHANNEL]
	     << ","
	     << (int)mycolor.channels[A_CHANNEL]
	     << ")"
	     << endl;
#endif
}
