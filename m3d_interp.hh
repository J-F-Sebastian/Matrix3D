#ifndef M3D_INTERP_HH_INCLUDED
#define M3D_INTERP_HH_INCLUDED

#include <cmath>
#include "m3d_math_vector.hh"
#include "m3d_color.hh"

/*
 * Interpolation classes
 *
 */

/*
 * Linear interpolation for single values.
 *
 * val(n) = val1*(1 - n) + val2*n
 *
 * n goes from 0 to 1, real values.
 *
 * Discrete formula follows
 *
 * val(N) = val1*(steps - N)/steps + val2*N/steps
 * val(N) = val1 - val1*N/steps + val2*N/steps
 * val(N) = val1 + (val2 - val1)*N/steps
 *
 * N goes from 0 to steps, integer values.
 * N is increased by calling step().
 */

class m3d_interpolation
{
public:
	m3d_interpolation(const int steps) : steps(steps)
	{
		if (this->steps < 1)
			this->steps = 1;
	}

	virtual void step(void) = 0;

	inline bool finished(void) const
	{
		return (steps) ? false : true;
	}

	inline int stepsvalue(void) const { return steps; }

protected:
	int steps;
};

/*
 * Linear interpolation of float values.
 */
class m3d_interpolation_float : public m3d_interpolation
{
public:
	m3d_interpolation_float() : m3d_interpolation(0), start(0.0f), val(0.0f), delta(0.0f) {}
	explicit m3d_interpolation_float(const int steps);
	explicit m3d_interpolation_float(const int steps, const float val1, const float val2);

	void init(const int step, const float val1, const float val2);

	virtual void step(void);

	void valuearray(float *out);

	inline float value(void) { return val; }

	inline float deltavalue(void) { return delta; }

	inline float compute(int stepn) { return start + (float)stepn * delta; }

private:
	float start, val, delta;
};

/*
 * Linear interpolation of short integer values.
 */
class m3d_interpolation_short : public m3d_interpolation
{
public:
	m3d_interpolation_short() : m3d_interpolation(0), start(0), val(0), delta(0) {}
	explicit m3d_interpolation_short(const int steps);
	explicit m3d_interpolation_short(const int steps, const short val1, const short val2);

	void init(const int step, const short val1, const short val2);

	virtual void step(void);

	void valuearray(short *out);

	inline short value(void) { return (short)((val + 0x8000) >> 16); }

	inline int deltavalue(void) { return delta; }

	inline short compute(int stepn) { return (short)((start + stepn * delta + 0x8000) >> 16); }

private:
	int start, val, delta;
};

/*
 * Linear interpolation of colors.
 */
class m3d_interpolation_color : public m3d_interpolation
{
public:
	m3d_interpolation_color() : m3d_interpolation(0), start(0), val(0), r(0), g(0), b(0) {}
	explicit m3d_interpolation_color(const int steps);
	explicit m3d_interpolation_color(const int steps, m3d_color &val1, m3d_color &val2);

	void init(const int steps, m3d_color &val1, m3d_color &val2);

	virtual void step(void);

	void valuearray(uint32_t *out);

	inline uint32_t value(void) { return val.color; }

private:
	union m3d_color::m3d_color_channels start, val;
	int r, g, b;
	int ra, ga, ba;
};

/*
 * Linear interpolation of single values with perspective correction.
 *
 * 1/z3 = 1/z1 + (1/z2 - 1/z1)*n
 *
 * value = z3*(val1/z1 + (val2/z2 - val1/z1)*n)
 *
 * n goes from 0 to 1, real values.
 * Discrete formula is way too complicated to be written in ASCII...
 */

class m3d_interpolation_float_perspective : public m3d_interpolation
{
public:
	m3d_interpolation_float_perspective(int steps, float z1, float z2, float val1, float val2);

	virtual void step(void);

	inline float value(void) { return val; }

private:
	float val1, z1inv, deltav, deltazinv, val;
};

/*
 * Reciprocal linear interpolation of vectors.
 *
 * vector(N)/z(N) = vector1/z1 + N*vector2/z2/steps
 *
 * vector(N) = z(N) * (vector1/z1 + N*vector1/z2/steps)
 */
class m3d_reciprocal_z_interpv_step
{
public:
	m3d_reciprocal_z_interpv_step(int steps, float z1, float z2, m3d_vector &v1, m3d_vector &v2) : steps(steps)
	{
		vector = v1;
		vector.myvector[X_C] /= z1;
		vector.myvector[Y_C] /= z1;
		vector.myvector[Z_C] /= z1;

		if (steps)
		{
			deltavector = v2;
			deltavector.myvector[X_C] /= z2;
			deltavector.myvector[Y_C] /= z2;
			deltavector.myvector[Z_C] /= z2;
			// (val2/z2 - val1/z1)
			deltavector.subtract(vector);
			// (val2/z2 - val1/z1)/steps
			deltavector.myvector[X_C] /= (float)steps;
			deltavector.myvector[Y_C] /= (float)steps;
			deltavector.myvector[Z_C] /= (float)steps;
		}
		else
		{
			deltavector.myvector[X_C] = 0.0f;
			deltavector.myvector[Y_C] = 0.0f;
			deltavector.myvector[Z_C] = 0.0f;
		}

		stepvector = vector;
		stepvector.myvector[X_C] *= z1;
		stepvector.myvector[Y_C] *= z1;
		stepvector.myvector[Z_C] *= z1;

		recipz = 1.0f / z1;
		if (steps)
		{
			deltarecipz = (1.0f / z2 - 1.0f / z1) / (float)steps;
		}
		else
		{
			deltarecipz = 0.0;
		}
	}

	float get_z(void)
	{
		return 1.0f / recipz;
	}

	int get_int_z(void)
	{
		return (int)(1.0f / recipz);
	}

	float get_recipz(void)
	{
		return recipz;
	}

	m3d_vector &get_vectorvalue(void)
	{
		return stepvector;
	}

	void step(void)
	{
		if (steps)
		{
			vector.add(deltavector);
			recipz += deltarecipz;
			stepvector = vector;
			stepvector.myvector[X_C] /= recipz;
			stepvector.myvector[Y_C] /= recipz;
			stepvector.myvector[Z_C] /= recipz;
			steps--;
		}
	}

	bool last_step(void)
	{
		return (steps) ? false : true;
	}

private:
	int steps;
	/* reciprocal of Z */
	float recipz;
	m3d_vector vector;
	m3d_vector deltavector;
	m3d_vector stepvector;
	float deltarecipz;
};

#endif
