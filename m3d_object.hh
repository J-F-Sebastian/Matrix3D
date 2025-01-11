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

#ifndef M3D_OBJECT_H
#define M3D_OBJECT_H

#include <vector>
#include <bitset>
#include <SDL.h>

#include "m3d_vertex.hh"
#include "m3d_color.hh"
#include "m3d_camera.hh"

#define M3D_MAX_VERTICES (10240)
#define M3D_MAX_TRIANGLES (10240)

/*
 * This data structure is used to create a mesh of points,
 * describing triangles, composing a surface (object).
 * Index values refers to m3d_point data stored in an
 * array.
 *
 * Example:
 *
 *  m3d_point triangle_array[] = {{1,2,3,1},    --> Point 0
 *                                {4,5,6,1},    --> Point 1
 *                                {7,6,9,1}};   --> Point 2
 *
 *  m3d_input_trimesh triangle_triangle = {{0,1,2}};
 */
struct m3d_input_trimesh
{
	uint32_t index[3];
};

class m3d_triangle
{
public:
	m3d_triangle() : normal()
	{
		index[0] = index[1] = index[2] = 0;
	}

	m3d_triangle(const m3d_triangle &other) : normal(other.normal)
	{
		index[0] = other.index[0];
		index[1] = other.index[1];
		index[2] = other.index[2];
	}

	~m3d_triangle() {};

	/*
	 * Apply a rotation to the normal
	 */
	void project(m3d_camera &camera);

	/*
	 * Indexes inside a mesh of the 3 vertexes composing the triangle
	 */
	uint32_t index[3];
	/*
	 * The normal to the triangle surface in world coordinates
	 */
	m3d_vector normal;
	/*
	 * The normal to the triangle surface in world coordinates, transformed
	 */
	m3d_vector tnormal;
	/*
	 * The normal to the triangle surface in projected homogeneous coordinates
	 */
	m3d_vector prjnormal;
};

class m3d_object
{
public:
	m3d_object() : vertices(),
		       mesh(),
		       direction(),
		       center(),
		       uptodate(false) {};

	~m3d_object() {};

	m3d_object(const m3d_object &other) : vertices(other.vertices),
					      mesh(other.mesh),
					      direction(other.direction),
					      center(other.center),
					      pitchangle(0.0f),
					      yawangle(0.0f),
					      rollangle(0.0f),
					      uptodate(false) {};

	/*
	 * Set vertices, vertices normals, build the triangles mesh
	 * and the normals to triangles.
	 * Update the center, the direction, and finally set up the
	 * transformation matrix.
	 */
	int create(struct m3d_input_point *_vertices,
		   const uint32_t vertnum,
		   struct m3d_input_trimesh *mesh,
		   const uint32_t meshnum);

	/*
	 * perform rolling of the object, rotating around z angle in degrees
	 */
	void roll(float angle);

	/*
	 * perform yawing of the object, rotating around y angle in degrees
	 */
	void yaw(float angle);

	/*
	 * perform pitching of the object, rotating around x angle in degrees
	 */
	void pitch(float angle);

	/*
	 * move the object to a new location by adding newposition to center,
	 * i.e. move along the vector.
	 */
	void move(const m3d_vector &newposition);

	/*
	 * move the object to a new location by setting center to newposition,
	 * i.e. move to newposition.
	 */
	void set(const m3d_point &newposition);

	/*
	 * Dump debug data
	 */
	void print(void);

	/*
	 * Vertices refer to the object center in [0,0,0] when built.
	 * Vertices stores vectors to vertices and their normals
	 * in both world and projected coordinates.
	 */
	std::vector<m3d_vertex> vertices;
	/*
	 * mesh stores triangles and their normals
	 */
	std::vector<m3d_triangle> mesh;
	/*
	 * Object direction, used for orientation and texturing
	 */
	m3d_vector direction;
	/*
	 * center in world coordinates
	 */
	m3d_point center;
	/*
	 * center in world coordinates, transformed
	 */
	m3d_point tcenter;

protected:
	void update_object(void);

private:
	/*
	 * Compute the object center
	 */
	void compute_center(void);

	float pitchangle, yawangle, rollangle;
	/*
	 * The object need or need not an update for its transformation
	 */
	bool uptodate;
};

class m3d_render_object : public m3d_object
{
public:
	enum
	{
		OBJ_VISIBLE = 1 << 0,
		OBJ_CHANGED = 1 << 7
	};

	m3d_render_object() : m3d_object(), z_sorting(0.0f), color(), flags(0) {};

	~m3d_render_object() {};

	m3d_render_object(const m3d_render_object &other) : m3d_object(other), z_sorting(0.0f), color(other.color), flags(0) {};

	/*
	 * Call object create method, setup object color.
	 */
	int create(struct m3d_input_point *_vertices,
		   const uint32_t vertnum,
		   struct m3d_input_trimesh *_mesh,
		   const uint32_t meshnum,
		   m3d_color &_color);

	/*
	 * Perform projection to camera by applying a tranformation
	 * to all vertices and normals.
	 * The transformation is stored in transform matrix in object class.
	 */
	void project(m3d_camera &camera);

	/*
	 * Dump debug data
	 */
	void print(void);

	/*
	 * This is a convenience for sorting, it is a copy of the Z value of the object center
	 * in homogeneous clip space.
	 */
	float z_sorting;
	/*
	 * Visible triangles as a bitset
	 */
	std::bitset<M3D_MAX_TRIANGLES> trivisible;
	/*
	 * Visible vertices as a bitset
	 */
	std::bitset<M3D_MAX_VERTICES> vtxvisible;
	/*
	 * Object color
	 */
	m3d_color color;
	/*
	 * Visible or changed flags
	 */
	unsigned flags;
};

#endif // M3D_OBJECT_H
