#ifndef _M3D_ILLUM_HH_
#define _M3D_ILLUM_HH_

#include "m3d_world.hh"
#include "m3d_render_color.hh"

class m3d_illumination
{
public:
	m3d_illumination();

	void ambient_lighting(m3d_vertex &vtx, m3d_render_object &obj, m3d_world &world, struct m3d_render_color &out);

	void diffuse_lighting(m3d_vertex &vtx, m3d_render_object &obj, m3d_world &world, struct m3d_render_color &out);

	void specular_lighting(m3d_vertex &vtx, m3d_render_object &obj, m3d_world &world, struct m3d_render_color &out);
};

class m3d_illum : public m3d_illumination
{
public:
	static m3d_illumination &inst(void);
};

#endif