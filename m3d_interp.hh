#ifndef M3D_INTERP_HH_INCLUDED
#define M3D_INTERP_HH_INCLUDED

#include "m3d_math.hh"

/*
 * Interpolation classes
 *
 */

/*
 * Linear interpolation for single values.
 *
 * val(N) = val1 + N*val2/steps
 *
 * N goes from 0 to steps-1 for step rounds.
 * N is increased by calling step().
 */

class m3d_interp_step
{
public:
	m3d_interp_step(int steps, float val1, float val2);

	void step(void);

	inline float get_val(void)
	{
		return val;
	}

	inline int get_int_val(void)
	{
		return (int)val;
	}

	inline float get_delta(void)
	{
		return delta;
	}

	inline bool finished(void)
	{
		return (steps) ? false : true;
	}

private:
	int steps;
	float val, delta;
};

/*
 * Reciprocal linear interpolation of single values.
 *
 * val(N)/z(N) = val1/z1 + N*val2/z2/steps
 *
 * val(N) = z(N) * (val1/z1 + N*val2/z2/steps)
 */

class m3d_reciprocal_z_interp_step
{
public:
	m3d_reciprocal_z_interp_step(int steps, float z1, float z2, float val1 = 1.0f, float val2 = 1.0f);

	void step(void);

	inline float get_z(void)
	{
		return 1.0f / recipz;
	}

	inline int get_int_z(void)
	{
		return (int)(1.0f / recipz);
	}

	inline float get_recipz(void)
	{
		return recipz;
	}

	inline float get_paramvalue(void)
	{
		return paramvalue * get_z();
	}

	inline bool finished(void)
	{
		return (steps) ? false : true;
	}

private:
	int steps;
	/* reciprocal of Z */
	float recipz;
	float paramvalue;
	float deltaparamvalue;
	float deltarecipz;
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
