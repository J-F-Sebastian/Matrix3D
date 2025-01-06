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

#include "m3d_light_source.hh"

using namespace std;

static inline float m3d_max(float a, float b)
{
	return (a > b) ? a : b;
}

void m3d_ambient_light::print(void)
{
#ifdef DEBUG
	cout << "Ambient light "
	     << "Color " << color.getColor() << "Intensity " << sintensity << endl;
#endif
}

/*
 * Intensity of the light color is computed with the formula
 *
 *   C' = 1/(Kc + Kl*d + Kq*d^2) * C
 *
 * So the light color is modulated by a quadratic formula for its intensity.
 * Light intensity depends on the distance between the light source and the object position.
 * It is a spherical source of light irradiating in all directions.
 */
float m3d_point_light_source::get_intensity(const m3d_point &objpos)
{
	// compute vector from object position to light source to compute distance
	m3d_vector L(objpos);
	L.subtract(position);
	// The distance from the object (scalar)
	float d = L.module();
	return (sintensity / (Kc + Kl * d + Kq * d * d));
}

void m3d_point_light_source::print()
{
#ifdef DEBUG
	cout.setf(ios_base::showpoint);
	cout << "Light source" << endl
	     << "Kc " << Kc << ", Kl " << Kl << ", Kq " << Kq << endl;
	position.print();
#endif
}

/* Intensity of the light color is computed with the formula
 *
 *  C' = (max{-R*L,0}^p)/(Kc + Kl*d + Kq*d^2) * C
 *
 *  So the light color is modulated by a quadratic formula for its intensity.
 *  Light intensity depends upon the distance between the light source and
 *  the object position.
 *  The light spot projected by this light has a fall-off dictated by the
 *  position of the object w.r.t. the direction of the spotlight.
 *  Practically speaking, the intensity of the light diminish with the distance
 *  from light to obect but also with the distance from the spotlight center and
 *  the object.
 */
float m3d_spot_light_source::get_intensity(const m3d_point &objpos)
{
	// compute vector from object position to light source to compute distance
	m3d_vector L(objpos);
	L.subtract(position);
	// distance (scalar)
	float d = L.module();
	L.normalize();
	// compute -RdotL which is the dot product of the normalized position vector
	// of the object and the spotlight direction.
	float minusRdotL = m3d_max(L.dot_product(direction), 0.0f);
	if (minusRdotL != 0.0f)
	{
		// We hardcoded a value of p = 8 but this should be dynamic....
		minusRdotL *= minusRdotL;
		minusRdotL *= minusRdotL;
		minusRdotL *= minusRdotL;
		minusRdotL *= (sintensity / (Kc + Kl * d + Kq * d * d));
	}

	return minusRdotL;
}

void m3d_spot_light_source::print()
{
#ifdef DEBUG
	cout.setf(ios_base::showpoint);
	cout << "Light source" << endl
	     << "Kc " << Kc << ", Kl " << Kl << ", Kq " << Kq << endl;
	position.print();
	direction.print();
#endif
}