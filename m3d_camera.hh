#ifndef M3D_CAMERA_H
#define M3D_CAMERA_H

#include <stdint.h>
#include <SDL.h>

#include "m3d_math.hh"
#include "m3d_vertex.hh"

struct m3d_screen_point
{
	int16_t x;
	int16_t y;
};

class m3d_camera
{
public:
	/** Default constructor */
	m3d_camera();
	/** Default destructor */
	~m3d_camera();

	m3d_camera(const m3d_camera &other) : position(other.position),
					      transform(other.transform),
					      frustum(other.frustum),
					      screen_resolution(other.screen_resolution) {};

	m3d_camera(const struct m3d_input_point &position,
		   const struct m3d_input_point &at,
		   int16_t xres,
		   int16_t yres);

	// Project a world coordinates point inside the view frustum to homogeneous clip space coordinates
	void projection(m3d_point &point);
	// Transform point from world coordinates to camera coordinates
	void transform_to_viewpoint(m3d_point &pointsrc, m3d_point &pointdst);
	// Transform vertex from world coordinates to camera coordinates
	void transform_to_viewpoint(m3d_vertex &vertexsrc);
	// Project point from homogeneous clip space coordinates to screen
	void project_to_screen(m3d_point &point, SDL_Point &pix);
	// Project point from world coordinates to screen
	void transform_and_project_to_screen(m3d_point &pointsrc, m3d_point &pointdst, SDL_Point &pix);
	// Compute visibility using normal as a surface normal vector and point to compute
	// the vector going from camera to point itself in world coordinates.
	bool is_visible(m3d_point &point, m3d_vector &normal);

	const m3d_point &get_position(void) { return position; }

private:
	m3d_point position;
	m3d_matrix_camera transform;
	m3d_frustum frustum;
	struct m3d_screen_point screen_resolution;
};

#endif // M3D_CAMERA_H
