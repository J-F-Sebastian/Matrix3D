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
		       int16_t yres) : position(position), transform(position, at), frustum(45.0f, xres, yres, 10.0f, 1000.0f)
{
	screen_resolution.x = xres;
	screen_resolution.y = yres;
}

void m3d_camera::projection(m3d_point &point)
{
	transform.transform(point, point);
	frustum.transform(point, point);
	point.myvector[X_C] /= point.myvector[T_C];
	point.myvector[Y_C] /= point.myvector[T_C];
	point.myvector[Z_C] /= point.myvector[T_C];
}

void m3d_camera::transform_to_viewpoint(m3d_point &pointsrc, m3d_point &pointdst)
{
	transform.transform(pointsrc, pointdst);
}

void m3d_camera::transform_to_viewpoint(m3d_vertex &vertex)
{
	transform.transform(vertex.position, vertex.position);
	transform.rotate(vertex.normal, vertex.normal);
}

void m3d_camera::project_to_screen(m3d_point &point, SDL_Point &pix)
{
	pix.x = (int)(point.myvector[X_C] * screen_resolution.x + screen_resolution.x) / 2;
	pix.y = (int)(-point.myvector[Y_C] * screen_resolution.y + screen_resolution.y) / 2;
}

void m3d_camera::transform_and_project_to_screen(m3d_point &pointsrc, m3d_point &pointdst, SDL_Point &pix)
{
	transform.transform(pointsrc, pointdst);
	frustum.transform(pointdst, pointdst);
	pointdst.myvector[X_C] /= pointdst.myvector[T_C];
	pointdst.myvector[Y_C] /= pointdst.myvector[T_C];
	pointdst.myvector[Z_C] /= pointdst.myvector[T_C];
	project_to_screen(pointdst, pix);
}

bool m3d_camera::is_visible(m3d_point &point, m3d_vector &normal)
{
	m3d_point temp(point);

	// temp vector goes from viewpoint to point
	temp.subtract(position);
	float cp = temp.dot_product(normal);
	return (std::signbit(cp));
}
