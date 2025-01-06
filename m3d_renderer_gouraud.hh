#ifndef M3D_RENDERER_GOURAUD_H
#define M3D_RENDERER_GOURAUD_H

#include "m3d_renderer_shaded.hh"

// Gouraud shading renderer
class m3d_renderer_shaded_gouraud : public m3d_renderer_shaded
{
public:
	/** Default constructor */
	m3d_renderer_shaded_gouraud() : m3d_renderer_shaded() {};
	explicit m3d_renderer_shaded_gouraud(m3d_display *disp) : m3d_renderer_shaded(disp) { cscanline = new uint32_t[display->get_ymax() * 2]; };

	/** Default destructor */
	virtual ~m3d_renderer_shaded_gouraud() { delete cscanline; };

protected:
	// The scanline light color buffer
	uint32_t *cscanline;

	void store_cscanlines(unsigned runlen, m3d_color &val1, m3d_color &val2, unsigned start = 0);
	virtual void triangle_fill_shaded(m3d_render_object &obj, m3d_vertex *vtx[], m3d_world &world);
};

#endif
