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
