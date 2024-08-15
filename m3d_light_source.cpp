#include <iostream>

#include "m3d_light_source.hh"

using namespace std;

static inline float m3d_min(float a, float b)
{
	return (a < b) ? a : b;
}

void m3d_ambient_light::print(void)
{
#ifdef DEBUG
	cout << "Ambient light "
	     << "Color " << color.getColor() << "Intensity " << sintensity << endl;
#endif
}

// Intensity of the light color is computed with the formula
//
//  C' = 1/(Kc + Kl*d + Kq*d^2) * C
//
//  So the light color is modulated by a quadratic formula for its intensity.
//  Light intensity depends on distance between the light source and
//  the object position
float m3d_point_light_source::get_intensity(const m3d_point &objpos, const m3d_point & /*viewer*/)
{
	// compute vector from position to light source
	m3d_vector L(objpos);
	L.subtract(position);
	float d = L.module();
	// cout << "Point light intensity " << sintensity/(Kc + Kl*d + Kq*d*d) << endl;
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

// Intensity of the light color is computed with the formula
//
//  C' = 1/(Kc + Kl*d + Kq*d^2) * C
//
//  So the light color is modulated by a quadratic formula for its intensity.
//  Light intensity depends on distance between the light source and
//  the object position
float m3d_spot_light_source::get_intensity(const m3d_point &objpos, const m3d_point &viewer)
{
	// compute vector from position to light source
	m3d_vector L(objpos);
	m3d_vector temppos(get_position());
	L.subtract(temppos);
	// compute RdotL
	float RdotL = m3d_min(L.dot_product(direction), 0.0f);
	if (RdotL != 0.0f)
	{
		RdotL *= m3d_point_light_source::get_intensity(objpos, viewer);
	}

	return RdotL;
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