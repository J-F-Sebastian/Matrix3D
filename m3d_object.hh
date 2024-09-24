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

	void rotate(m3d_matrix &rotation);

	/*
	 * Indexes inside a mesh of the 3 vertexes composing the triangle
	 */
	uint32_t index[3];
	/*
	 * The normal to the triangle surface in world coordinates
	 */
	m3d_vector normal;
	/*
	 * The normal to the triangle surface in projected homogeneous coordinates
	 */
	m3d_vector prjnormal;
};

class m3d_object
{
public:
	/** Default constructor */
	m3d_object() : vertices(),
		       mesh(),
		       direction(),
		       center(),
		       uptodate(false) {};
	/** Default destructor */
	~m3d_object() {};
	/** Copy constructor
	 *  \param other Object to copy from
	 */
	m3d_object(const m3d_object &other) : vertices(other.vertices),
					      mesh(other.mesh),
					      direction(other.direction),
					      center(other.center),
					      uptodate(false) {};

	// Set vertices, mesh, their normals and cleanup bitmaps
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

	// move the object to a new location along the passed-in vector
	void move(const m3d_vector &newposition);

	void print(void);

	/*
	 * Vertices refer to the world center in [0,0,0] when built.
	 * Vertices stores vectors to vertices and their normals
	 * in both world and projected coordinates.
	 */
	std::vector<m3d_vertex> vertices;
	// mesh stores triangles and their normals
	std::vector<m3d_triangle> mesh;
	// Object direction, used for orientation and texturing
	m3d_vector direction;
	// center is always in world coordinates
	m3d_point center;

protected:
	void update_object(m3d_matrix &transform);

private:
	/** Compute the object center */
	void compute_center(void);
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

	/** Default constructor */
	m3d_render_object() : m3d_object(), color(), flags(0), z_sorting(0.0f) {};
	/** Default destructor */
	virtual ~m3d_render_object() {};
	/** Copy constructor
	 *  \param other Object to copy from
	 */
	m3d_render_object(const m3d_render_object &other) : m3d_object(other), z_sorting(0.0f), color(other.color), flags(0) {};

	// creates an object starting from a m3d_trimesh array.
	// vertices points to an array of m3d_point structs, describing the geometry of all
	// vertexes (points) of the object.
	// mesh must point to an array of m3d_input_trimesh structs, describing the geometry of
	// the object as a list of triangles, whose vertexes are stored in vertices.
	// Direction is a vector starting at (0,0,0,0) and pointed towards face number 0.
	// vertnum is the number of items in vertices.
	// meshnum is the number of items in mesh.
	int create(struct m3d_input_point *_vertices,
		   const uint32_t vertnum,
		   struct m3d_input_trimesh *_mesh,
		   const uint32_t meshnum,
		   m3d_color &_color);

	void project(m3d_camera &camera);

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
	// Object color
	m3d_color color;
	// visible or changed flags
	unsigned flags;
};

#endif // M3D_OBJECT_H
