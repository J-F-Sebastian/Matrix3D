#ifndef M3D_RENDERER_H
#define M3D_RENDERER_H

#include <SDL.h>

#include "m3d_display.hh"
#include "m3d_world.hh"
#include "m3d_zbuffer.hh"

class m3d_renderer
{
public:
	struct m3d_renderer_data
	{
		m3d_point vertex;
		m3d_vector normal;
		SDL_Point toscreen;
		float lightint;
		m3d_color color;
	};

	/** Default constructor */
	m3d_renderer() : display(nullptr) {};
	m3d_renderer(m3d_display *disp);

	/** Default destructor */
	virtual ~m3d_renderer();

	virtual void render(m3d_world &world);

protected:
	// The window we are rendering to
	m3d_display *display;
	// The scanline buffer
	int16_t *scanline;
	// Z buffer
	m3d_zbuffer zbuffer;
	// The list of visible objects
	std::list<m3d_render_object *> vislist;

	/*
	 * Sorts an array of 3 points composing a triangle.
	 * Y is the Y on-screen coordinate of the 3 vertices of the triangle.
	 * The triangle is geometrically unchanged, its vertices are sorted in descending order
	 * (top of screen to bottom of screen).
	 * */
	void sort_triangle(struct m3d_renderer_data trianglep[]);

	/*
	 * Store scanlines into the scanline buffer, starting at position start inside the buffer.
	 * The scanline buffer carries the X coordinates of every linecomposing the polygon
	 * to be filled with horizontal lines (scan lines!).
	 * The Y coordinates are implicitely starting at the lowest y0, i.e. the caller MUST know
	 * the coordinates of the points composing the polygon and how they are sorted.
	 */
	void store_scanlines(int16_t x0, int16_t y0, int16_t x1, int16_t y1, unsigned start = 0);

	/*
	 * Compute the light intensity for vertex vtx using the world description.
	 * All coordinates are in the WORLD reference system, i. e. the origin is (0,0,0).
	 */
	void illuminate(struct m3d_renderer_data &vtx, m3d_world &world);

	uint32_t *get_video_buffer(int16_t x0, int16_t y0);

	void compute_visible_list_and_sort(m3d_world &world);
};

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
	void triangle_fill_flat(struct m3d_renderer_data vtx[],
				unsigned runlen[]);
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
	void triangle_fill_flat(struct m3d_renderer_data vtx[]);
};

// Perspective-correct shading renderer
class m3d_renderer_shaded : public m3d_renderer
{
public:
	/** Default constructor */
	m3d_renderer_shaded() : m3d_renderer() {};
	m3d_renderer_shaded(m3d_display *disp) : m3d_renderer(disp) {};

	/** Default destructor */
	virtual ~m3d_renderer_shaded() {};

	virtual void render(m3d_world &world);

protected:
	virtual void triangle_fill_shaded(struct m3d_renderer_data vtx[],
					  unsigned runlen[],
					  m3d_world &world);
};

// Gouraud shading renderer
class m3d_renderer_shaded_gouraud : public m3d_renderer_shaded
{
public:
	/** Default constructor */
	m3d_renderer_shaded_gouraud() : m3d_renderer_shaded() {};
	m3d_renderer_shaded_gouraud(m3d_display *disp) : m3d_renderer_shaded(disp) {};

	/** Default destructor */
	virtual ~m3d_renderer_shaded_gouraud() {};

protected:
	virtual void triangle_fill_shaded(struct m3d_renderer_data vtx[],
					  unsigned runlen[],
					  m3d_world &world);
};

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
	virtual void triangle_fill_shaded(struct m3d_renderer_data vtx[],
					  unsigned runlen[],
					  m3d_world &world);
};

#endif // M3D_RENDERER_H
