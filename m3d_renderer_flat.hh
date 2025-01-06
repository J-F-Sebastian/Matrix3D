#ifndef M3D_RENDERER_FLAT_H
#define M3D_RENDERER_FLAT_H

#include "m3d_renderer.hh"

class m3d_renderer_flat : public m3d_renderer
{
public:
	/** Default constructor */
	m3d_renderer_flat() : m3d_renderer() {};
	m3d_renderer_flat(m3d_display *disp) : m3d_renderer(disp) {};

	/** Default destructor */
	virtual ~m3d_renderer_flat() {};

	virtual void render(m3d_world &world);

private:
	void triangle_fill_flat(m3d_vertex *vtx[], int16_t *runlen, m3d_color &color);
};

class m3d_renderer_flatf : public m3d_renderer
{
public:
	/** Default constructor */
	m3d_renderer_flatf() : m3d_renderer() {};
	m3d_renderer_flatf(m3d_display *disp) : m3d_renderer(disp) {};

	/** Default destructor */
	virtual ~m3d_renderer_flatf() {};

	virtual void render(m3d_world &world);

private:
	void triangle_fill_flat(m3d_vertex *vtx[], m3d_color &color);
};

#endif
