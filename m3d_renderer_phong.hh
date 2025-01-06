#ifndef M3D_RENDERER_PHONG_H
#define M3D_RENDERER_PHONG_H

#include "m3d_renderer_shaded.hh"

// Phong shading renderer
class m3d_renderer_shaded_phong : public m3d_renderer_shaded
{
public:
	/** Default constructor */
	m3d_renderer_shaded_phong() : m3d_renderer_shaded() {};
	m3d_renderer_shaded_phong(m3d_display *disp) : m3d_renderer_shaded(disp) {};

	/** Default destructor */
	virtual ~m3d_renderer_shaded_phong() {};

protected:
	virtual void triangle_fill_shaded(m3d_render_object &obj, m3d_vertex *vtx[], m3d_world &world);
};

#endif
