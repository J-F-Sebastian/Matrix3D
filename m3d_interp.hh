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

#ifndef M3D_INTERP_HH_INCLUDED
#define M3D_INTERP_HH_INCLUDED

#include <cmath>
#include <cstdint>
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
	explicit m3d_interpolation(const int steps) : steps(steps)
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
	m3d_interpolation_color() : m3d_interpolation(0)
	{
		start.color = val.color = 0;
		delta[0] = delta[1] = delta[2] = 0;
		acc[0] = acc[1] = acc[2] = 0;
	}

	explicit m3d_interpolation_color(const int steps);
	explicit m3d_interpolation_color(const int steps, m3d_color &val1, m3d_color &val2);

	void init(const int steps, m3d_color &val1, m3d_color &val2);

	virtual void step(void);

	void valuearray(uint32_t *out);

	inline uint32_t value(void) { return val.color; }

private:
	union m3d_color::m3d_color_channels start, val;
	unsigned delta[3];
	unsigned acc[3];
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

	void valuearray(float *out);

	inline float value(void) { return val; }

private:
	float val1, z1inv, deltav, deltazinv, val;
};

/*
 * Linear interpolation of vectors
 *
 * vector(N) = vector(1)*(1 - n) + vector(2)*n
 *
 * n goes from 0 to 1, real values.
 *
 */
class m3d_interpolation_vector : public m3d_interpolation
{
public:
	m3d_interpolation_vector(int steps, m3d_vector &v1, m3d_vector &v2);

	virtual void step(void);

	void valuearray(m3d_vector *out);

	inline m3d_vector &value(void) { return val; }

private:
	float z1inv, deltazinv;
	m3d_vector vector1, deltavector, val;
};

/*
 * Linear interpolation of vectors with perspective correction.
 *
 * 1/z3 = 1/z1 + (1/z2 - 1/z1)*n
 *
 * vector(N) = z3 * (vector1/z1 + (vector2/z2 - vector1/z1)*n)
 */
class m3d_interpolation_vector_perspective : public m3d_interpolation
{
public:
	m3d_interpolation_vector_perspective(int steps, float z1, float z2, m3d_vector &v1, m3d_vector &v2);

	virtual void step(void);

	void valuearray(m3d_vector *out);

	inline m3d_vector &value(void) { return val; }

private:
	float z1inv, deltazinv;
	m3d_vector vector1, deltavector, val;
};

#endif
