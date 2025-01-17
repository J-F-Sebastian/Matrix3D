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

#include <stddef.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <errno.h>

#include "m3d_object.hh"
#include "m3d_illum.hh"

using namespace std;

/********************************/
/****** class m3d_triangle ******/
/********************************/

void m3d_triangle::project(m3d_camera &camera)
{
	camera.projection(normal, prjnormal);
}

/******************************/
/****** class m3d_object ******/
/******************************/

int m3d_object::create(struct m3d_input_point *_vertices,
		       const uint32_t vertnum,
		       struct m3d_input_trimesh *_mesh,
		       const uint32_t meshnum)
{
	m3d_vector a, b;

	if (_vertices && vertnum && (vertnum < M3D_MAX_VERTICES) && vertices.empty())
	{
		try
		{
			vertices.resize(vertnum);
		}
		catch (const bad_alloc &e)
		{
			cerr << "Out of memory " << e.what() << endl;
			return (ENOMEM);
		}
		for (auto &it : vertices)
		{
			it.position = _vertices->vector;
			_vertices++;
		}
	}
	else
	{
		return (EINVAL);
	}

	if (_mesh && meshnum && (meshnum < M3D_MAX_TRIANGLES) && mesh.empty())
	{
		try
		{
			mesh.resize(meshnum);
		}
		catch (const bad_alloc &e)
		{
			cerr << "Out of memory " << e.what() << endl;
			return (ENOMEM);
		}

		for (auto &it : mesh)
		{
			it.index[0] = _mesh->index[0];
			it.index[1] = _mesh->index[1];
			it.index[2] = _mesh->index[2];
			_mesh++;
		}
	}
	else
	{
		return (EINVAL);
	}

	compute_center();
	// DIRECTION

	return (0);
}

// Z
void m3d_object::roll(float angle)
{
	rollangle += angle;
	uptodate = false;
}

// Y
void m3d_object::yaw(float angle)
{
	yawangle += angle;
	uptodate = false;
}

// X
void m3d_object::pitch(float angle)
{
	pitchangle += angle;
	uptodate = false;
}

void m3d_object::set(const m3d_point &newposition)
{
	center = newposition;
	uptodate = false;
}

// move the object to a new location
void m3d_object::move(const m3d_vector &newposition)
{
	center.add(newposition);
	uptodate = false;
}

void m3d_object::print()
{
#ifdef DEBUG
	ostringstream tmp;
	unsigned i;

	cout << "* Object" << endl
	     << "  Vertices" << endl;
	i = 0;
	for (auto &it : vertices)
	{
		it.print();
	}
	cout << "  Triangle Mesh" << endl;
	i = 0;
	for (auto &it : mesh)
	{
		tmp << "(" << it.index[0] << "," << it.index[1] << "," << it.index[2] << ") ,";
		tmp << "  Normal ---> ";
		cout << tmp.str();
		tmp.str("");
		it.normal.print();
	}
	cout << endl;
	cout << "  Center    ---> ";
	center.print();
	cout << "  Direction ---> ";
	direction.print();
	cout << endl;
#endif
}

void m3d_object::compute_center()
{
	float part = 1.0f / (float)vertices.size();

	for (auto &it : vertices)
	{
		center.add(it.position);
	}

	center.scale(part);
	center.myvector[T_C] = 1.0f;
}

void m3d_object::update_object()
{
	if (uptodate == false)
	{
		m3d_matrix_transform t(pitchangle, yawangle, rollangle, center);

		for (auto &it : vertices)
		{
			// m3d_point truepos = center + it.position;
			t.transform(it.position, it.tposition);
			t.rotate(it.normal, it.tnormal);
		}

		/* update triangle surfaces' normals */
		for (auto &it : mesh)
		{
			t.rotate(it.normal, it.tnormal);
		}
		/* update object's direction */

		/* MISSING */
		uptodate = true;
	}
}

/*************************************/
/****** class m3d_render_object ******/
/*************************************/

int m3d_render_object::create(struct m3d_input_point *_vertices,
			      const uint32_t vertnum,
			      struct m3d_input_trimesh *_mesh,
			      const uint32_t meshnum,
			      m3d_color &_color)
{
	int retcode;
	m3d_vector a, b;

	retcode = m3d_object::create(_vertices, vertnum, _mesh, meshnum);

	if (retcode)
	{
		return (retcode);
	}

	/*
	 * Set vertices normals by summing triangles' normals to every
	 * vertices.
	 * Triangles' normals are orthogonal to the surface, Vertices' normals
	 * are the normalized sum of surfaces' normals.
	 * Surfaces' normals are normalized after building the vertices' normals,
	 * so that the area of a surface weights the normals of the vertices
	 * composing the surface.
	 */
	for (auto &it : mesh)
	{
		/*
		 * compute surface's normal.
		 * vertices will be used this way:
		 *    0 -> 1   vector 1
		 *    0 -> 2   vector 2
		 *
		 * normal is vector1 X vector2
		 */
		a = vertices.at(it.index[1]).position;
		a.subtract(vertices.at(it.index[0]).position);
		b = vertices.at(it.index[2]).position;
		b.subtract(vertices.at(it.index[0]).position);
		a.cross_product(b);

		/*
		 * Add surface normal to vertices' normals
		 */
		vertices.at(it.index[0]).normal.add(a);
		vertices.at(it.index[1]).normal.add(a);
		vertices.at(it.index[2]).normal.add(a);

		a.normalize();
		it.normal = a;
	}

	// Normalize
	for (auto &it : vertices)
	{
		it.normal.normalize();
		it.normal.print();
	}

	color = _color;
	return (0);
};

void m3d_render_object::project(m3d_camera &camera)
{
	m3d_point temp;

	update_object();

	for (auto &it : vertices)
	{
		camera.projection_to_screen(it.tposition, it.prjposition, it.scrposition);
		// FIXME you sure? Not homogeneous?
		camera.to_camera(it.tnormal, it.prjnormal);
	}

	unsigned i = 0;
	int xa, ya, xb, yb;
	vtxvisible.reset();
	for (auto &it : mesh)
	{
		camera.projection(it.tnormal, it.prjnormal);
		/*
		 * Compute the integer vectors as in create() but using the projected
		 * discrete coordinates.
		 * We avoid multiplication with camera vector, and compute the Z component
		 * of the vector product of the normal to the projected discrete surface.
		 * The result use homogeneus coordinates, so positive Z means the surface
		 * is facing away from us, i.e. it is invisible.
		 */
		xa = vertices.at(it.index[1]).scrposition.x - vertices.at(it.index[0]).scrposition.x;
		ya = vertices.at(it.index[1]).scrposition.y - vertices.at(it.index[0]).scrposition.y;
		xb = vertices.at(it.index[2]).scrposition.x - vertices.at(it.index[0]).scrposition.x;
		yb = vertices.at(it.index[2]).scrposition.y - vertices.at(it.index[0]).scrposition.y;

		trivisible[i] = (xa * yb - ya * xb > 0) ? false : true;
		if (trivisible[i++])
		{
			vtxvisible[it.index[0]] = true;
			vtxvisible[it.index[1]] = true;
			vtxvisible[it.index[2]] = true;
		}
	}

	camera.projection(center, temp);
	z_sorting = temp.myvector[Z_C];
}

void m3d_render_object::print()
{
#ifdef DEBUG
	ostringstream tmp;
	unsigned i;

	cout << "* Render Object" << endl;
	i = 0;
	for (auto &it : vertices)
	{
		it.print();
	}
	cout << "  Triangle Mesh" << endl;
	i = 0;
	for (auto &it : mesh)
	{
		tmp << "(" << it.index[0] << "," << it.index[1] << "," << it.index[2] << ") ,";
		if (trivisible[i++])
		{
			tmp << " V,";
		}
		tmp << " Normal ---> ";
		cout << tmp.str();
		tmp.str("");
		it.normal.print();
	}
	cout << endl;
	cout << "  Center    ---> ";
	center.print();
	cout << "  Direction ---> ";
	direction.print();
	cout << "  Color     ---> ";
	color.print();
	cout << endl;
#endif
}
