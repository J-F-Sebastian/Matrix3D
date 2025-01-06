#ifndef M3D_RENDERER_WIREFRAME_H
#define M3D_RENDERER_WIREFRAME_H

#include "m3d_renderer.hh"

class m3d_renderer_wireframe : public m3d_renderer
{
public:
	/** Default constructor */
	m3d_renderer_wireframe() : m3d_renderer() {};
	m3d_renderer_wireframe(m3d_display *disp) : m3d_renderer(disp) {};

	/** Default destructor */
	virtual ~m3d_renderer_wireframe() {};

	virtual void render(m3d_world &world);
};

#endif
