#ifndef M3D_OBJECT_H
#define M3D_OBJECT_H

#include <vector>
#include <bitset>

#include "m3d_vertex.hh"
#include "m3d_color.hh"
#include "m3d_camera.hh"

#define M3D_MAX_VERTICES (10240)
#define M3D_MAX_TRIANGLES (10240 * 4)

/*
 * This data structure is used to create a mesh of points,
 * describing triangles, composing a surface (object).
 * Index values refers to m3d_point data stored in an
 * array.
 *
 * Example:
 *
 *  m3d_point triangle_array[] = {{1,2,3,0},    --> Point 0
 *                                {4,5,6,0},    --> Point 1
 *                                {7,6,9,0}};   --> Point 2
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

	uint32_t index[3];
	m3d_vector normal;
};

class m3d_object
{
public:
	/** Default constructor */
	m3d_object() : vertices(),
		       mesh(),
		       vertex_visible(),
		       triangle_visible(),
		       direction(),
		       center(),
		       z_sorting(0.0f),
		       visibility_uptodate(false) {};
	/** Default destructor */
	~m3d_object() {};
	/** Copy constructor
	 *  \param other Object to copy from
	 */
	m3d_object(const m3d_object &other) : vertices(other.vertices),
					      mesh(other.mesh),
					      vertex_visible(other.vertex_visible),
					      triangle_visible(other.triangle_visible),
					      direction(other.direction),
					      center(other.center),
					      z_sorting(0.0f),
					      visibility_uptodate(false) {};

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

	// Compute triangles visibility
	void compute_visibility(m3d_camera &viewpoint);

	void print(void);

	// vertices are always in object coordinates, referring to a center in [0,0,0]
	// vertices stores vectors to vertices and their normals
	std::vector<m3d_vertex> vertices;
	// mesh stores triangles and their normals
	std::vector<m3d_triangle> mesh;
	// visible vertices, visible triangles
	std::bitset<M3D_MAX_VERTICES> vertex_visible;
	std::bitset<M3D_MAX_TRIANGLES> triangle_visible;
	// Object direction, used for orientation and texturing
	m3d_vector direction;
	// center is always in world coordinates
	m3d_point center;
	// This is a convenience for sorting
	float z_sorting;

protected:
	void update_object(m3d_matrix &transform);

private:
	/** Compute the object center */
	void compute_center(void);
	bool visibility_uptodate;
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
	m3d_render_object() : m3d_object(), color() {};
	/** Default destructor */
	virtual ~m3d_render_object() {};
	/** Copy constructor
	 *  \param other Object to copy from
	 */
	m3d_render_object(const m3d_render_object &other) : m3d_object(other), color(other.color) {};

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

	void print(void);

	m3d_color color;

	unsigned flags;
};

#endif // M3D_OBJECT_H
