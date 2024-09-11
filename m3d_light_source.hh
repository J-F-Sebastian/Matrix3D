#ifndef M3D_LIGHT_SOURCE_H
#define M3D_LIGHT_SOURCE_H

#include <cmath>

#include "m3d_math.hh"
#include "m3d_vertex.hh"
#include "m3d_color.hh"

/*
 * Base abstract class for light sources
 */
class m3d_light_source
{
public:
	m3d_light_source() : color(), sintensity(0.0f) {};
	virtual ~m3d_light_source() {};
	m3d_light_source(const m3d_light_source &other) : color(other.color), sintensity(other.sintensity) {};
	m3d_light_source(const m3d_color &color, float src_intensity) : color(color), sintensity(src_intensity) {};

	virtual float get_intensity(const m3d_point &objpos) = 0;
	m3d_color get_color(void) { return color; }

	void set_color(const m3d_color &clr) { color = clr; }
	void set_src_intensity(float intensity) { sintensity = intensity; };

	virtual void print(void) = 0;

protected:
	m3d_color color;
	float sintensity;
};

/*
 * Ambient light.
 * The light baseline, diffuse illumination of the ambient of the scene.
 * This light source is constant everywhere and does not attenuate with distance
 * direction or other parameters.
 */
class m3d_ambient_light : public m3d_light_source
{
public:
	m3d_ambient_light() : m3d_light_source() {};
	virtual ~m3d_ambient_light() {};

	m3d_ambient_light(const m3d_ambient_light &other) : m3d_light_source(other) {};

	m3d_ambient_light(const m3d_color &color,
			  const float src_intensity) : m3d_light_source(color, src_intensity) {}

	virtual float get_intensity(const m3d_point &objpos) { return sintensity; }

	virtual void print(void);
};

/*
 * Point light.
 * The point light source is spherical and it emits a light intensity which is
 * inversely proportional to the distance of an object.
 * The intensity fall-off does not depend upon position or incidence angles, just distance.
 */
class m3d_point_light_source : public m3d_light_source
{
public:
	m3d_point_light_source() : m3d_light_source(), position(), Kc(0.0f), Kl(0.0f), Kq(0.0f) {}
	virtual ~m3d_point_light_source() {};
	m3d_point_light_source(const m3d_point_light_source &other) : m3d_light_source(other),
								      position(other.position),
								      Kc(other.Kc),
								      Kl(other.Kl),
								      Kq(other.Kq) {}
	m3d_point_light_source(const struct m3d_input_point &position,
			       const m3d_color &color,
			       const float Kc,
			       const float Kl,
			       const float Kq,
			       const float intensity) : m3d_light_source(color, intensity),
							position(position),
							Kc(Kc),
							Kl(Kl),
							Kq(Kq) {}

	const m3d_point &get_position(void) const { return position; }

	virtual float get_intensity(const m3d_point &objpos);

	virtual void print(void);

protected:
	m3d_point position;
	// Constant, Linear and Quadratic attenuation constants
	float Kc, Kl, Kq;
};

/*
 * Spot light.
 * The spot light source is conical and it emits a light intensity which is
 * inversely proportional to the distance of an object.
 * The intensity fall-off does not depend upon position or incidence angles, just distance.
 */
class m3d_spot_light_source : public m3d_point_light_source
{
public:
	/** Default constructor */
	m3d_spot_light_source() : m3d_point_light_source() {}

	/** Default destructor */
	virtual ~m3d_spot_light_source() {};
	/** Copy constructor
	 *  \param other Object to copy from
	 */
	m3d_spot_light_source(const m3d_spot_light_source &other) : m3d_point_light_source(other) {}

	m3d_spot_light_source(const struct m3d_input_point &lookat,
			      const struct m3d_input_point &position,
			      const m3d_color &color,
			      const float Kc,
			      const float Kl,
			      const float Kq,
			      const float intensity) : m3d_point_light_source(position, color, Kc, Kl, Kq, intensity), direction(lookat)
	{
		direction.subtract(get_position());
		direction.normalize();
	}

	// World coordinates not rotated
	virtual float get_intensity(const m3d_point &objpos);

	virtual void print(void);

protected:
	m3d_vector direction;
};

#endif // M3D_LIGHT_SOURCE_H
