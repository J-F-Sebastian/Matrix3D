#include "m3d_interp.hh"

m3d_interp_step::m3d_interp_step(int steps, float val1, float val2) : steps(steps), val(val1)
{
	delta = (steps) ? (val2 - val1) / (float)steps : 0.0f;
}

void m3d_interp_step::step()
{
	if (steps)
	{
		val += delta;
		steps--;
	}
}

m3d_reciprocal_z_interp_step::m3d_reciprocal_z_interp_step(int steps, float z1, float z2, float val1, float val2)
    : steps(steps), recipz(1.0f / z1), paramvalue(val1 / z1)
{
	if (steps)
	{
		deltaparamvalue = (val2 / z2 - val1 / z1) / (float)steps;
		deltarecipz = (1.0f / z2 - 1.0f / z1) / (float)steps;
	}
	else
	{
		deltaparamvalue = deltarecipz = 0.0f;
	}
}

void m3d_reciprocal_z_interp_step::step()
{
	if (steps)
	{
		paramvalue += deltaparamvalue;
		recipz += deltarecipz;
		steps--;
	}
}