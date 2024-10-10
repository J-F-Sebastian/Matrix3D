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
	 * Vertex position in object coordinates.
	 * Values are computed referring to the object's center.
	 * World coordinates can be computed by adding the object's center.
	 */
	m3d_point position;
	/*
	 * Vertex normal in world coordinates.
	 * Normalized vector (not positional).
	 */
	m3d_vector normal;
	/*
	 * Vertex position in world coordinates, transformed.
	 */
	m3d_point tposition;
	/*
	 * Vertex normal in world coordinates, transformed.
	 */
	m3d_vector tnormal;
	/*
	 * Vertex position in projected homogeneous coordinates.
	 */
	m3d_point prjposition;
	/*
	 * Vertex normal in projected homogeneous coordinates.
	 */
	m3d_point prjnormal;
	/*
	 * Screen coordinates for prjposition.
	 */
	SDL_Point scrposition;
};

#endif // M3D_VERTEX_H
