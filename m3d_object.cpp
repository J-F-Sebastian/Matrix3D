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

int m3d_object::create(struct m3d_input_point *vertices,
		       const uint32_t vertnum,
		       struct m3d_input_trimesh *mesh,
		       const uint32_t meshnum)
{
	vector<m3d_vertex>::iterator it;
	vector<m3d_triangle>::iterator it2;
	m3d_vector a, b;

	if (vertices && vertnum && (vertnum < M3D_MAX_VERTICES) && this->vertices.empty())
	{
		try
		{
			this->vertices.resize(vertnum);
		}
		catch (const bad_alloc &e)
		{
			cerr << "Out of memory " << e.what() << endl;
			return (ENOMEM);
		}
		for (it = this->vertices.begin(); it != this->vertices.end(); it++)
		{
			it->position = vertices->vector;
			vertices++;
		}
		vertex_visible.reset();
	}
	else
	{
		return (EINVAL);
	}

	if (mesh && meshnum && (meshnum < M3D_MAX_TRIANGLES) && this->mesh.empty())
	{
		try
		{
			this->mesh.resize(meshnum);
		}
		catch (const bad_alloc &e)
		{
			cerr << "Out of memory " << e.what() << endl;
			return (ENOMEM);
		}

		for (it2 = this->mesh.begin(); it2 != this->mesh.end(); it2++, mesh++)
		{
			it2->index[0] = mesh->index[0];
			it2->index[1] = mesh->index[1];
			it2->index[2] = mesh->index[2];
			/*
			 * compute the surface normal.
			 * vertices will be used this way:
			 *    0 -> 1   vector 1
			 *    0 -> 2   vector 2
			 *
			 * normal is vector1 X vector2
			 */
			a = this->vertices.at(mesh->index[1]).position;
			a.subtract(this->vertices.at(mesh->index[0]).position);
			b = this->vertices.at(mesh->index[2]).position;
			b.subtract(this->vertices.at(mesh->index[0]).position);
			a.cross_product(b);
			a.normalize();
			it2->normal = a;
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
#ifdef NDEBUG
	vector<m3d_vertex>::iterator it;
	vector<m3d_triangle>::iterator it2;
	ostringstream tmp;
	unsigned i;

	cout << "Object" << endl;
	for (it = vertices.begin(), i = 0; it != vertices.end(); it++, i++)
	{
		if (vertex_visible[i])
		{
			cout << " V, ";
		}
		else
		{
			cout << "    ";
		}
		it->print();
	}
	cout << "Center ---> ";
	center.print();
	cout << "Triangle Mesh" << endl;
	for (it2 = mesh.begin(), i = 0; it2 != mesh.end(); it2++, i++)
	{
		tmp << "(" << it2->index[0] << "," << it2->index[1] << "," << it2->index[2] << ") ,";
		if (triangle_visible[i])
		{
			tmp << " V,";
		}
		tmp << " Normal ---> ";
		cout << tmp.str();
		tmp.str("");
		it2->normal.print();
	}
	cout << endl;
#endif
}

void m3d_object::compute_center()
{
	float part = 1.0f / (float)vertices.size();
	vector<m3d_vertex>::iterator it;

	for (it = vertices.begin(); it != vertices.end(); it++)
	{
		center.add(it->position);
	}

	center.myvector[X_C] *= part;
	center.myvector[Y_C] *= part;
	center.myvector[Z_C] *= part;
	center.myvector[T_C] = 1.0f;
}

void m3d_object::update_object(m3d_matrix &transform)
{
	vector<m3d_vertex>::iterator it = vertices.begin();
	vector<m3d_triangle>::iterator it2 = mesh.begin();

	while (it != vertices.end())
	{
		transform.rotate(it->position, it->position);
		transform.rotate(it->normal, it->normal);
		++it;
	}

	/* update triangle surfaces' normals */
	while (it2 != mesh.end())
	{
		it2->rotate(transform);
		++it2;
	}

	/* update object's direction */
	/* MISSING */
}

void m3d_object::compute_visibility(m3d_camera &viewpoint)
{
	vector<m3d_triangle>::iterator it;
	m3d_point temp;
	unsigned i;

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
	for (it = mesh.begin(), i = 0; it != mesh.end(); it++, i++)
	{
		temp = vertices.at(it->index[0]).position;
		// temp is now in world coordinates
		temp.add(center);
		triangle_visible[i] = viewpoint.is_visible(temp, it->normal);
		if (triangle_visible[i])
		{
			vertex_visible[it->index[0]] = true;
			vertex_visible[it->index[1]] = true;
			vertex_visible[it->index[2]] = true;
		}
	}

	visibility_uptodate = true;
}

/*************************************/
/****** class m3d_render_object ******/
/*************************************/

int m3d_render_object::create(struct m3d_input_point *vertices,
			      const uint32_t vertnum,
			      struct m3d_input_trimesh *mesh,
			      const uint32_t meshnum,
			      m3d_color &color)
{
	vector<m3d_triangle>::iterator it;
	vector<m3d_vertex>::iterator it2;
	int retcode;

	retcode = m3d_object::create(vertices, vertnum, mesh, meshnum);

	if (retcode)
	{
		return (retcode);
	}

	// Set vertices normals by summing triangles' normals to every
	// vertices.
	// Triangles' normals are orthogonal to the surface, Vertices' normals
	// are the normalized sum of surfaces' normals
	//
	for (it = this->mesh.begin(); it != this->mesh.end(); it++)
	{
		this->vertices.at(it->index[0]).normal.add(it->normal);
		this->vertices.at(it->index[1]).normal.add(it->normal);
		this->vertices.at(it->index[2]).normal.add(it->normal);
	}

	// Normalize
	for (it2 = this->vertices.begin(); it2 != this->vertices.end(); it2++)
	{
		it2->normal.print();
		it2->normal.normalize();
	}

	this->color = color;
	return (0);
};

void m3d_render_object::print()
{
#ifdef NDEBUG
	cout << "Render Object" << endl;
	m3d_object::print();
	color.print();
	cout << endl;
#endif
}
