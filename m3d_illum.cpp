#include "m3d_illum.hh"
#include <iostream>
static inline float m3d_max(float a, float b)
{
	return (a > b) ? a : b;
}

m3d_illumination::m3d_illumination()
{
}

// TBD: consider all lights as white RGB(255,255,255) to reduce computation
void m3d_illumination::ambient_lighting(m3d_vertex &vtx, m3d_render_object &obj, m3d_world &world, struct m3d_render_color &out)
{
	// Ambient light does not depend on position anyway...
	float lightint = world.ambient_light.get_intensity(vtx.tposition);
	out.Kamb = obj.color;
	out.Kamb.brighten(lightint);
	out.ambint = lightint;
}

// TBD: consider all lights as white RGB(255,255,255) to reduce computation
void m3d_illumination::diffuse_lighting(m3d_vertex &vtx, m3d_render_object &obj, m3d_world &world, struct m3d_render_color &out)
{
	float lightint = 0.0f;
	m3d_vector vtxworld(vtx.tposition);
	//  Now we sum all diffuse contributions considering light position w.r.t. vtx position and the surface normal.
	for (auto lights : world.lights_list)
	{
		m3d_vector L(lights->tposition);
		L.subtract(vtxworld);
		L.normalize();
		float dot = L.dot_product(vtx.tnormal);
		lightint += lights->get_intensity(vtx.tposition) * m3d_max(dot, 0.0f);
	}
	out.Kdiff = obj.color;
	out.Kdiff.brighten(lightint);
	out.diffint = lightint;
}

// Using halfway vector
void m3d_illumination::specular_lighting(m3d_vertex &vtx, m3d_render_object &obj, m3d_world &world, struct m3d_render_color &out)
{
	float lightint = 0.0f;
	m3d_vector vtxworld(vtx.tposition);
	m3d_point cam;
	world.camera.get_tposition(cam);
	m3d_vector V(vtx.tposition);
	V.subtract(cam);
	// Now we sum all specular contributions considering light position w.r.t. vtx position and the surface normal.
	for (auto lights : world.lights_list)
	{
		m3d_vector L(lights->tposition);
		L.subtract(vtxworld);
		// Halfway vector H
		m3d_vector H(L + V);
		H.normalize();
		L.normalize();
		if (vtx.normal.dot_product(L) > 0.0f)
		{
			float dot = L.dot_product(vtx.tnormal);
			// We hardcoded a value of p = 8 but this should be dynamic....
			dot *= dot;
			dot *= dot;
			dot *= dot;
			lightint += lights->get_intensity(vtx.tposition) * m3d_max(dot, 0.0f);
		}
	}
	out.Kspec = obj.color;
	out.Kspec.brighten(lightint);
	out.specint = lightint;
}

m3d_illumination &m3d_illum::inst()
{
	static m3d_illumination instance;

	return instance;
}