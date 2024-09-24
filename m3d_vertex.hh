#ifndef M3D_VERTEX_H
#define M3D_VERTEX_H

#include <SDL.h>

#include "m3d_math.hh"

class m3d_vertex
{
public:
	m3d_vertex() : position(), normal() {};
	~m3d_vertex() {};
	m3d_vertex(const float coords[]) : position(coords) {};
	m3d_vertex(const m3d_vertex &other);
	void print(void);

	/*
	 * Vertex position in world coordinates
	 */
	m3d_point position;
	/*
	 * Vertex normal in world coordinates
	 */
	m3d_vector normal;
	/*
	 * Vertex position in projected homogeneous coordinates
	 */
	m3d_point prjposition;
	/*
	 * Vertex normal in camera coordinates
	 */
	m3d_vector camnormal;
	/*
	 * Screen coordinates for prjposition
	 */
	SDL_Point scrposition;
};

#endif // M3D_VERTEX_H
