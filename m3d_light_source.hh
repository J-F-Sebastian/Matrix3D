#ifndef M3D_LIGHT_SOURCE_H
#define M3D_LIGHT_SOURCE_H

#include <cmath>

#include "m3d_math.hh"
#include "m3d_vertex.hh"
#include "m3d_color.hh"

// base class, abstract
class m3d_light_source
{
public:
	/** Default constructor */
	m3d_light_source() : color(), sintensity(0.0f){};

	/** Default destructor */
	virtual ~m3d_light_source(){};
	/** Copy constructor
	 *  \param other Object to copy from
	 */
	m3d_light_source(const m3d_light_source &other) : color(other.color), sintensity(other.sintensity){};

	m3d_light_source(const m3d_color &color, float src_intensity) : color(color), sintensity(src_intensity){};

	float get_intensity(const m3d_point & /*objpos*/, const m3d_point & /*viewer*/) { return sintensity; }

	virtual m3d_color get_color(void) = 0;
	void set_color(const m3d_color &clr) { color = clr; }
	void set_src_intensity(float intensity) { sintensity = (std::signbit(intensity)) ? 0.0f : intensity; };

	virtual void print(void) = 0;

protected:
	m3d_color color;
	float sintensity;
};

// Ambient light
class m3d_ambient_light : public m3d_light_source
{
public:
	m3d_ambient_light() : m3d_light_source(){};
	virtual ~m3d_ambient_light(){};

	m3d_ambient_light(const m3d_ambient_light &other) : m3d_light_source(other){};

	m3d_ambient_light(const m3d_color &color,
			  const float src_intensity) : m3d_light_source(color, src_intensity) {}

	// Ambient light is constant everywhere, objects and viewer position are not involved
	// get_intensity is inherited
	virtual m3d_color get_color(void) { return color; }

	virtual void print(void);
};

// point light source
class m3d_point_light_source : public m3d_light_source
{
public:
	/** Default constructor */
	m3d_point_light_source() : m3d_light_source(), position(), Kc(0.0f), Kl(0.0f), Kq(0.0f) {}

	/** Default destructor */
	virtual ~m3d_point_light_source(){};
	/** Copy constructor
	 *  \param other Object to copy from
	 */
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

	inline const m3d_point &get_position(void) { return position; }

	float get_intensity(const m3d_point &objpos, const m3d_point &viewer);

	virtual m3d_color get_color(void) { return color; }

	virtual void print(void);

protected:
	m3d_point position;
	// Constant, Linear and Quadratic attenuation constants
	float Kc, Kl, Kq;
};

// spot light source
class m3d_spot_light_source : public m3d_point_light_source
{
public:
	/** Default constructor */
	m3d_spot_light_source() : m3d_point_light_source() {}

	/** Default destructor */
	virtual ~m3d_spot_light_source(){};
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
		m3d_vector temppos(get_position());
		direction.subtract(temppos);
		direction.normalize();
	}

	// World coordinates not rotated
	float get_intensity(const m3d_point &objpos, const m3d_point &viewer);

	virtual void print(void);

protected:
	m3d_vector direction;
};

#endif // M3D_LIGHT_SOURCE_H
