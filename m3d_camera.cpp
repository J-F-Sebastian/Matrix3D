#include <cmath>
#include <iostream>

#include "m3d_camera.hh"

using namespace std;

m3d_camera::m3d_camera() : position(), transform(), frustum(0.0f, 0, 0, 1.0f, INFINITY)
{
	screen_resolution.x = screen_resolution.y = 0;
}

m3d_camera::~m3d_camera()
{
	// dtor
}

m3d_camera::m3d_camera(const struct m3d_input_point &position,
		       const struct m3d_input_point &at,
		       int16_t xres,
		       int16_t yres) : position(position), transform(position, at), frustum(45.0f, xres, yres, 100.0f)
{
	screen_resolution.x = xres;
	screen_resolution.y = yres;
}

void m3d_camera::to_camera(m3d_point &pointsrc, m3d_point &pointdst)
{
	transform.transform(pointsrc, pointdst);
}

void m3d_camera::to_camera(m3d_vector &vecsrc, m3d_vector &vectdst)
{
	transform.transform(vecsrc, vectdst);
}

void m3d_camera::projection(m3d_point &pointsrc, m3d_point &pointdst)
{
	transform.transform(pointsrc, pointdst);
	frustum.transform(pointdst, pointdst);
	pointdst[X_C] /= pointdst[T_C];
	pointdst[Y_C] /= pointdst[T_C];
	pointdst[Z_C] /= pointdst[T_C];
}

void m3d_camera::projection(m3d_vector &vecsrc, m3d_vector &vectdst)
{
	transform.transform(vecsrc, vectdst);
	frustum.transform(vectdst, vectdst);
}

void m3d_camera::to_screen(m3d_point &point, SDL_Point &pix)
{
	pix.x = lroundf((point[X_C] + 1.0f) * screen_resolution.x) / 2;
	pix.y = lroundf((-point[Y_C] + 1.0f) * screen_resolution.y) / 2;
}

void m3d_camera::projection_to_screen(m3d_point &pointsrc, m3d_point &pointdst, SDL_Point &pix)
{
	transform.transform(pointsrc, pointdst);
	frustum.transform(pointdst, pointdst);
	pointdst[X_C] /= pointdst[T_C];
	pointdst[Y_C] /= pointdst[T_C];
	pointdst[Z_C] /= pointdst[T_C];
	to_screen(pointdst, pix);
}

bool m3d_camera::is_visible(m3d_point &point, m3d_vector &normal)
{
	m3d_point temp(point);
	// temp vector goes from viewpoint to point
	temp.subtract(position);
	transform.transform(temp, temp);
	float cp = temp.dot_product(normal);
	return (std::signbit(cp));
}
