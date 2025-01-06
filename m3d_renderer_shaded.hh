#ifndef M3D_RENDERER_SHADED_H
#define M3D_RENDERER_SHADED_H

#include "m3d_renderer.hh"

// Perspective-correct shading renderer
class m3d_renderer_shaded : public m3d_renderer
{
public:
	/** Default constructor */
	m3d_renderer_shaded() : m3d_renderer() {};
	explicit m3d_renderer_shaded(m3d_display *disp) : m3d_renderer(disp) { iscanline = new float[display->get_ymax() * 2]; };

	/** Default destructor */
	virtual ~m3d_renderer_shaded() { delete iscanline; };

	virtual void render(m3d_world &world);

protected:
	// The scanline light intensity buffer
	float *iscanline;
	struct m3d_render_color colors[3];

	void store_iscanlines(unsigned runlen, float z1, float z2, float val1, float val2, unsigned start = 0);
	virtual void triangle_fill_shaded(m3d_render_object &obj, m3d_vertex *vtx[], m3d_world &world);
};

#endif
