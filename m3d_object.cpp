#include <stddef.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <errno.h>

#include "m3d_object.hh"

using namespace std;

/********************************/
/****** class m3d_triangle ******/
/********************************/

void m3d_triangle::rotate(m3d_matrix &rotation)
{
	rotation.rotate(normal, normal);
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
		vertex_visible.reset();
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
			/*
			 * compute the surface normal.
			 * vertices will be used this way:
			 *    0 -> 1   vector 1
			 *    0 -> 2   vector 2
			 *
			 * normal is vector1 X vector2
			 */
			a = vertices.at(_mesh->index[1]).position;
			a.subtract(vertices.at(_mesh->index[0]).position);
			b = vertices.at(_mesh->index[2]).position;
			b.subtract(vertices.at(_mesh->index[0]).position);
			a.cross_product(b);
			a.normalize();
			it.normal = a;
			_mesh++;
		}
		triangle_visible.reset();
	}
	else
	{
		return (EINVAL);
	}

	compute_center();
	// DIRECTION

	return (0);
}

/*
 * perform rolling of the object, rotating around z angle in degrees
 */
void m3d_object::roll(float angle)
{
	m3d_matrix temp = m3d_matrix_roll(angle);
	update_object(temp);
	visibility_uptodate = false;
}

/*
 * perform yawing of the object, rotating around y angle in degrees
 */
void m3d_object::yaw(float angle)
{
	m3d_matrix temp = m3d_matrix_yaw(angle);
	update_object(temp);
	visibility_uptodate = false;
}

/*
 * perform pitching of the object, rotating around x angle in degrees
 */
void m3d_object::pitch(float angle)
{
	m3d_matrix temp = m3d_matrix_pitch(angle);
	update_object(temp);
	visibility_uptodate = false;
}

// move the object to a new location
void m3d_object::move(const m3d_vector &newposition)
{
	center.add(newposition);
	visibility_uptodate = false;
}

void m3d_object::print()
{
#ifdef DEBUG
	ostringstream tmp;
	unsigned i;

	cout << "Object" << endl;
	i = 0;
	for (auto &it : vertices)
	{
		if (vertex_visible[i++])
		{
			cout << " V, ";
		}
		else
		{
			cout << "    ";
		}
		it.print();
	}
	cout << "Center ---> ";
	center.print();
	cout << "Triangle Mesh" << endl;
	i = 0;
	for (auto &it : mesh)
	{
		tmp << "(" << it.index[0] << "," << it.index[1] << "," << it.index[2] << ") ,";
		if (triangle_visible[i++])
		{
			tmp << " V,";
		}
		tmp << " Normal ---> ";
		cout << tmp.str();
		tmp.str("");
		it.normal.print();
	}
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

	center.myvector[X_C] *= part;
	center.myvector[Y_C] *= part;
	center.myvector[Z_C] *= part;
	center.myvector[T_C] = 1.0f;
}

void m3d_object::update_object(m3d_matrix &transform)
{
	for (auto &it : vertices)
	{
		transform.rotate(it.position, it.position);
		transform.rotate(it.normal, it.normal);
	}

	/* update triangle surfaces' normals */
	for (auto &it : mesh)
	{
		it.rotate(transform);
	}

	/* update object's direction */
	/* MISSING */
}

void m3d_object::compute_visibility(m3d_camera &viewpoint)
{
	m3d_point temp;
	unsigned i = 0;

	if (visibility_uptodate)
	{
		return;
	}

	triangle_visible.reset();
	vertex_visible.reset();
	/*
	 * Compute visibility by adding center to the first triangle vertex,
	 * then subtracting the viewpoint, then invoking is_visible.
	 */
	for (auto &it : mesh)
	{
		temp = vertices.at(it.index[0]).position;
		// temp is now in world coordinates
		temp.add(center);
		triangle_visible[i] = viewpoint.is_visible(temp, it.normal);
		if (triangle_visible[i])
		{
			vertex_visible[it.index[0]] = true;
			vertex_visible[it.index[1]] = true;
			vertex_visible[it.index[2]] = true;
		}
		++i;
	}

	visibility_uptodate = true;
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

	retcode = m3d_object::create(_vertices, vertnum, _mesh, meshnum);

	if (retcode)
	{
		return (retcode);
	}

	// Set vertices normals by summing triangles' normals to every
	// vertices.
	// Triangles' normals are orthogonal to the surface, Vertices' normals
	// are the normalized sum of surfaces' normals
	//
	for (auto &it : mesh)
	{
		vertices.at(it.index[0]).normal.add(it.normal);
		vertices.at(it.index[1]).normal.add(it.normal);
		vertices.at(it.index[2]).normal.add(it.normal);
	}

	// Normalize
	for (auto &it : vertices)
	{
		it.normal.print();
		it.normal.normalize();
	}

	color = _color;
	return (0);
};

void m3d_render_object::print()
{
#ifdef DEBUG
	cout << "Render Object" << endl;
	m3d_object::print();
	color.print();
	cout << endl;
#endif
}
