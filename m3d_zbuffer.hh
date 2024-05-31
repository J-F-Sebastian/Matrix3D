#ifndef M3D_ZBUFFER_HH_INCLUDED
#define M3D_ZBUFFER_HH_INCLUDED

#include <cstdint>

class m3d_zbuffer
{
public:
	m3d_zbuffer() : zbuffer(nullptr), size(0), xres(0), yres(0) {}
	m3d_zbuffer(int16_t xres, int16_t yres) : size(xres * yres), xres(xres), yres(yres)
	{
		//    size = xres * yres;
		//        zbuffer = new float[size];
		zbuffer = new int16_t[size];
	}

	~m3d_zbuffer()
	{
		if (zbuffer)
			delete zbuffer;
	}

	void reset(void)
	{
		for (int n = 0; n < size; n++)
			zbuffer[n] = INT16_MAX;
	}

	inline bool test(int16_t x0, int16_t y0, int16_t z)
	{
		int16_t *zb = get_zbuffer(x0, y0);
		if (z < *zb)
		{
			*zb = z;
			return true;
		}
		return false;
	}

	inline bool test(int16_t *zbuf, int16_t z)
	{
		if (z < *zbuf)
		{
			*zbuf = z;
			return true;
		}
		return false;
	}

	inline int16_t *get_zbuffer(int16_t x0, int16_t y0)
	{
		return zbuffer + (y0 * xres) + x0;
	}

private:
	int16_t *zbuffer;
	int size;
	int16_t xres, yres;
};

#endif