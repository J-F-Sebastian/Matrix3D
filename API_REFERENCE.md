# API Reference

<span style="color: gray;">This document describes the main public API of the project. It is intentionally focused on the classes and methods that define the rendering engine and scene model.</span>

## 1. Core data types

### <span style="color: green; font-weight: bold;">m3d_input_point</span>

A homogeneous 3D point represented as four float values: `{x, y, z, w}`.

- `float vector[4]`
- used as input for mesh definitions and camera transforms

### <span style="color: green; font-weight: bold;">m3d_display_point</span>

A 2D integer pixel coordinate.

- `int x`
- `int y`

### <span style="color: green; font-weight: bold;">m3d_input_trimesh</span>

A triangle primitive storing three vertex indices.

- `uint32_t index[3]`

### <span style="color: green; font-weight: bold;">m3d_render_color</span>

Stores per-vertex illumination data for the rasterized triangle pipeline.

- `m3d_color Kamb`
- `m3d_color Kspec`
- `m3d_color Kdiff`
- `float ambint`
- `float specint`
- `float diffint`

## 2. Math subsystem

### <span style="color: green; font-weight: bold;">m3d_vector</span>

The base geometric vector type used across the engine.

Key methods:

- <span style="color: purple; font-weight: bold;">cross_product(const m3d_vector &veca)</span>
- <span style="color: purple; font-weight: bold;">dot_product(const m3d_vector &veca)</span>
- <span style="color: purple; font-weight: bold;">subtract(const m3d_vector &veca)</span>
- <span style="color: purple; font-weight: bold;">add(const m3d_vector &veca)</span>
- <span style="color: purple; font-weight: bold;">module()</span>
- <span style="color: purple; font-weight: bold;">module2()</span>
- <span style="color: purple; font-weight: bold;">normalize()</span>
- <span style="color: purple; font-weight: bold;">scale(float val)</span>
- <span style="color: purple; font-weight: bold;">mirror()</span>
- <span style="color: purple; font-weight: bold;">roll(float angle)</span>
- <span style="color: purple; font-weight: bold;">yaw(float angle)</span>
- <span style="color: purple; font-weight: bold;">pitch(float angle)</span>
- <span style="color: purple; font-weight: bold;">print()</span>

Public data:

- `float myvector[4]`

### <span style="color: green; font-weight: bold;">m3d_point</span>

A 3D position object derived from `m3d_vector`.

Key constructors and operators:

- `m3d_point()`
- `m3d_point(const float values[])`
- `m3d_point(const m3d_point &other)`
- `m3d_point(const struct m3d_input_point &point)`
- `m3d_point(const float x, const float y, const float z)`
- `operator=(const m3d_point &other)`

### <span style="color: green; font-weight: bold;">m3d_matrix</span>

Represents a 4x4 matrix in raw column order and is the engine’s primary transform container.

Key methods:

- <span style="color: purple; font-weight: bold;">m3d_matrix()</span>
- <span style="color: purple; font-weight: bold;">m3d_matrix(const float values[][4])</span>
- <span style="color: purple; font-weight: bold;">insert(const m3d_vector &vector, unsigned row)</span>
- <span style="color: purple; font-weight: bold;">multiply(m3d_matrix &mat)</span>
- <span style="color: purple; font-weight: bold;">multiply(m3d_vector &vector)</span>
- <span style="color: purple; font-weight: bold;">transpose()</span>
- <span style="color: purple; font-weight: bold;">translate(m3d_vector &vector)</span>
- <span style="color: purple; font-weight: bold;">rotation_matrix_vect(m3d_vector &veca, float angle)</span>
- <span style="color: purple; font-weight: bold;">reflect_matrix_vect(m3d_vector &veca)</span>
- <span style="color: purple; font-weight: bold;">orientation_matrix(m3d_vector &veca, m3d_vector &vecb)</span>
- <span style="color: purple; font-weight: bold;">orientation_matrix_vect_y(m3d_vector &veca)</span>
- <span style="color: purple; font-weight: bold;">rotate(m3d_vector &veca, m3d_vector &out)</span>
- <span style="color: purple; font-weight: bold;">transform(m3d_vector &veca, m3d_vector &out)</span>
- <span style="color: purple; font-weight: bold;">print()</span>

Public data:

- `float mymatrix[4][4]`

Subclasses:

- <span style="color: green; font-weight: bold;">m3d_matrix_roll</span>
- <span style="color: green; font-weight: bold;">m3d_matrix_pitch</span>
- <span style="color: green; font-weight: bold;">m3d_matrix_yaw</span>
- <span style="color: green; font-weight: bold;">m3d_matrix_rotation</span>
- <span style="color: green; font-weight: bold;">m3d_matrix_transform</span>
- <span style="color: green; font-weight: bold;">m3d_matrix_identity</span>
- <span style="color: green; font-weight: bold;">m3d_matrix_camera</span>
- <span style="color: green; font-weight: bold;">m3d_frustum</span>

### <span style="color: green; font-weight: bold;">m3d_frustum</span>

Builds the perspective projection matrix from camera view space to normalized clip space.

Constructors:

- `m3d_frustum(const float fowangle, const int xres, const int yres, const float near, const float far)`
- `m3d_frustum(const float fowangle, const int xres, const int yres, const float near)`

## 3. Scene and geometry objects

### <span style="color: green; font-weight: bold;">m3d_vertex</span>

Represents a vertex with position and normal data in object and projected space.

Public data:

- `m3d_point position`
- `m3d_vector normal`
- `m3d_point tposition`
- `m3d_vector tnormal`
- `m3d_point prjposition`
- `m3d_point prjnormal`
- `m3d_display_point scrposition`

Key methods:

- `m3d_vertex()`
- `m3d_vertex(const float coords[])`
- `m3d_vertex(const m3d_vertex &other)`
- <span style="color: purple; font-weight: bold;">print()</span>

### <span style="color: green; font-weight: bold;">m3d_triangle</span>

A triangular face storing the three vertex indices and the associated face normal in several coordinate systems.

Public data:

- `uint32_t index[3]`
- `m3d_vector normal`
- `m3d_vector tnormal`
- `m3d_vector prjnormal`

Key method:

- <span style="color: purple; font-weight: bold;">project(m3d_camera &camera)</span>

### <span style="color: green; font-weight: bold;">m3d_object</span>

Base object model for a mesh.

Public data:

- `std::vector<m3d_vertex> vertices`
- `std::vector<m3d_triangle> mesh`
- `m3d_vector direction`
- `m3d_point center`
- `m3d_point tcenter`

Key methods:

- <span style="color: purple; font-weight: bold;">create(struct m3d_input_point *_vertices, const uint32_t vertnum, struct m3d_input_trimesh *mesh, const uint32_t meshnum)</span>
- <span style="color: purple; font-weight: bold;">roll(float angle)</span>
- <span style="color: purple; font-weight: bold;">yaw(float angle)</span>
- <span style="color: purple; font-weight: bold;">pitch(float angle)</span>
- <span style="color: purple; font-weight: bold;">move(const m3d_vector &newposition)</span>
- <span style="color: purple; font-weight: bold;">set(const m3d_point &newposition)</span>
- <span style="color: purple; font-weight: bold;">print()</span>

Protected/private helpers:

- <span style="color: purple; font-weight: bold;">update_object()</span>
- <span style="color: purple; font-weight: bold;">compute_center()</span>

### <span style="color: green; font-weight: bold;">m3d_render_object</span>

Render-specific extension of `m3d_object` with depth, color, and visibility state.

Public data:

- `float z_sorting`
- `std::bitset<M3D_MAX_TRIANGLES> trivisible`
- `std::bitset<M3D_MAX_VERTICES> vtxvisible`
- `m3d_color color`
- `unsigned flags`

Key methods:

- <span style="color: purple; font-weight: bold;">create(struct m3d_input_point *_vertices, const uint32_t vertnum, struct m3d_input_trimesh *_mesh, const uint32_t meshnum, m3d_color &_color)</span>
- <span style="color: purple; font-weight: bold;">project(m3d_camera &camera)</span>
- <span style="color: purple; font-weight: bold;">print()</span>

Flags:

- `OBJ_VISIBLE`
- `OBJ_CHANGED`

## 4. Cameras and worlds

### <span style="color: green; font-weight: bold;">m3d_camera</span>

Defines the viewing position and the projection transform.

Key methods:

- `m3d_camera()`
- `m3d_camera(const struct m3d_input_point &position, const struct m3d_input_point &at, int16_t xres, int16_t yres)`
- <span style="color: purple; font-weight: bold;">to_camera(m3d_point &pointsrc, m3d_point &pointdst)</span>
- <span style="color: purple; font-weight: bold;">to_camera(m3d_vector &vecsrc, m3d_vector &vecdst)</span>
- <span style="color: purple; font-weight: bold;">projection(m3d_point &pointsrc, m3d_point &pointdst)</span>
- <span style="color: purple; font-weight: bold;">projection(m3d_vector &vecsrc, m3d_vector &vectdst)</span>
- <span style="color: purple; font-weight: bold;">to_screen(m3d_point &point, m3d_display_point &pix)</span>
- <span style="color: purple; font-weight: bold;">projection_to_screen(m3d_point &pointsrc, m3d_point &pointdst, m3d_display_point &pix)</span>
- <span style="color: purple; font-weight: bold;">is_visible(m3d_point &point, m3d_vector &normal)</span>
- <span style="color: purple; font-weight: bold;">get_position(m3d_point &pos)</span>
- <span style="color: purple; font-weight: bold;">get_tposition(m3d_point &tposition)</span>

Public data:

- `m3d_point position`
- `m3d_matrix_camera transform`
- `m3d_frustum frustum`
- `m3d_display_point screen_resolution`

### <span style="color: green; font-weight: bold;">m3d_world</span>

The rendering scene container.

Public data:

- `std::list<m3d_render_object *> objects_list`
- `std::list<m3d_point_light_source *> lights_list`
- `m3d_ambient_light ambient_light`
- `m3d_camera camera`

Key methods:

- <span style="color: purple; font-weight: bold;">m3d_world(const m3d_ambient_light &ambient, const m3d_camera &camera)</span>
- <span style="color: purple; font-weight: bold;">~m3d_world()</span>
- <span style="color: purple; font-weight: bold;">add_object(m3d_render_object &object)</span>
- <span style="color: purple; font-weight: bold;">add_light_source(m3d_point_light_source &light_source)</span>
- <span style="color: purple; font-weight: bold;">set_ambient_light(m3d_ambient_light &_ambient_light)</span>
- <span style="color: purple; font-weight: bold;">set_ambient_light_intensity(float intensity)</span>
- <span style="color: purple; font-weight: bold;">sort(std::list<m3d_render_object *> &_objects_list)</span>
- <span style="color: purple; font-weight: bold;">print()</span>

## 5. Lighting API

### <span style="color: green; font-weight: bold;">m3d_light_source</span>

Abstract base type for all light sources.

Key methods:

- `virtual float get_intensity(const m3d_point &objpos) = 0`
- `m3d_color get_color(void)`
- `void set_color(const m3d_color &clr)`
- `void set_src_intensity(float intensity)`
- `virtual void print(void) = 0`

### <span style="color: green; font-weight: bold;">m3d_ambient_light</span>

Represents constant environment light.

Key methods:

- `m3d_ambient_light(const m3d_color &color, const float src_intensity)`
- `virtual float get_intensity(const m3d_point &objpos)`
- `virtual void print(void)`

### <span style="color: green; font-weight: bold;">m3d_point_light_source</span>

Represents a positional light source with attenuation.

Public data:

- `m3d_point position`
- `float Kc, Kl, Kq`

Key methods:

- `m3d_point_light_source(const struct m3d_input_point &position, const m3d_color &color, const float Kc, const float Kl, const float Kq, const float intensity)`
- `virtual float get_intensity(const m3d_point &objpos)`
- `virtual void print(void)`

### <span style="color: green; font-weight: bold;">m3d_spot_light_source</span>

Directional variant of point lighting.

Key methods:

- `m3d_spot_light_source(const struct m3d_input_point &lookat, const struct m3d_input_point &position, const m3d_color &color, const float Kc, const float Kl, const float Kq, const float intensity)`
- `virtual float get_intensity(const m3d_point &objpos)`
- `virtual void print(void)`

### <span style="color: green; font-weight: bold;">m3d_illumination</span>

Computes ambient, diffuse, and specular lighting.

Key methods:

- <span style="color: purple; font-weight: bold;">ambient_lighting(m3d_vertex &vtx, m3d_render_object &obj, m3d_world &world, struct m3d_render_color &out)</span>
- <span style="color: purple; font-weight: bold;">diffuse_lighting(m3d_vertex &vtx, m3d_render_object &obj, m3d_world &world, struct m3d_render_color &out)</span>
- <span style="color: purple; font-weight: bold;">specular_lighting(m3d_vertex &vtx, m3d_render_object &obj, m3d_world &world, struct m3d_render_color &out)</span>

### <span style="color: green; font-weight: bold;">m3d_illum</span>

Singleton entry point for the illumination system.

Key method:

- <span style="color: purple; font-weight: bold;">inst()</span>

## 6. Color API

### <span style="color: green; font-weight: bold;">m3d_color</span>

Stores an RGBA color value in a single 32-bit integer and provides arithmetic helpers.

Key methods:

- `m3d_color()`
- `m3d_color(uint32_t color)`
- `m3d_color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)`
- <span style="color: purple; font-weight: bold;">setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)</span>
- <span style="color: purple; font-weight: bold;">getColor()</span>
- <span style="color: purple; font-weight: bold;">getChannel(unsigned chanNum)</span>
- <span style="color: purple; font-weight: bold;">brighten(float intensity)</span>
- <span style="color: purple; font-weight: bold;">brighten2(float intensity)</span>
- <span style="color: purple; font-weight: bold;">brighten_channels(float *chansint)</span>
- <span style="color: purple; font-weight: bold;">brighten_channels2(float *chansint)</span>
- <span style="color: purple; font-weight: bold;">average_colors(m3d_color array[], unsigned num, m3d_color &out)</span>
- <span style="color: purple; font-weight: bold;">add_colors(m3d_color array[], unsigned num, m3d_color &out)</span>
- <span style="color: purple; font-weight: bold;">print()</span>

Operators:

- `operator*`
- `operator+`

## 7. Bitmap API

### <span style="color: green; font-weight: bold;">m3d_bmp</span>

Loads Windows BMP files with uncompressed 1-, 4-, 8-, 16-, 24-, or 32-bit pixels. Indexed BMPs use their color table; 16-bit true-color pixels use the standard 5-5-5 RGB layout. Compressed BMPs and unsupported headers are rejected.

- `m3d_bmp(const std::string &filename)` — inspects the file header and palette; check `is_valid()` and `get_error()`
- `int load()` — decodes pixels into owned memory; returns `0` on success or an errno-style error code
- `m3d_color read(int x, int y) const` — returns a pixel using top-left image coordinates; throws `std::logic_error` if not loaded and `std::out_of_range` for invalid coordinates
- `void unload()` — releases decoded pixels while retaining file metadata
- `void print() const` — prints diagnostic information when `DEBUG` is enabled
- `bool is_valid() const`, `bool is_loaded() const`, `int get_error() const`
- `int get_width() const`, `int get_height() const`, `uint16_t get_bits_per_pixel() const`, `std::size_t get_memory_size() const`

## 8. Renderer API

### <span style="color: green; font-weight: bold;">m3d_renderer</span>

Base renderer class. Owns the display context, z-buffer, and scanline arrays.

Key methods:

- `m3d_renderer(m3d_display *disp)`
- `virtual ~m3d_renderer()`
- <span style="color: purple; font-weight: bold;">virtual void render(m3d_world &world)</span>
- <span style="color: purple; font-weight: bold;">void sort_triangle(m3d_vertex *vtx[3])</span>
- <span style="color: purple; font-weight: bold;">void sort_triangle(m3d_vertex *vtx[3], struct m3d_render_color *colors)</span>
- <span style="color: purple; font-weight: bold;">void store_scanlines(unsigned runlen, int16_t val1, int16_t val2, unsigned start = 0)</span>
- <span style="color: purple; font-weight: bold;">void store_zscanlines(unsigned runlen, float val1, float val2, unsigned start = 0)</span>
- <span style="color: purple; font-weight: bold;">uint32_t *get_video_buffer(int16_t x0, int16_t y0)</span>
- <span style="color: purple; font-weight: bold;">void compute_visible_list_and_sort(m3d_world &world)</span>

Protected members:

- `m3d_display *display`
- `int16_t *scanline`
- `float *zscanline`
- `m3d_zbuffer zbuffer`
- `std::list<m3d_render_object *> vislist`

### <span style="color: green; font-weight: bold;">m3d_renderer_wireframe</span>

Renders each visible triangle as wire edges.

Key method:

- <span style="color: purple; font-weight: bold;">virtual void render(m3d_world &world)</span>

### <span style="color: green; font-weight: bold;">m3d_renderer_flat</span>

Per-triangle flat shading.

Key method:

- <span style="color: purple; font-weight: bold;">virtual void render(m3d_world &world)</span>

Private helper:

- <span style="color: purple; font-weight: bold;">triangle_fill_flat(m3d_vertex *vtx[], m3d_color &color)</span>

### <span style="color: green; font-weight: bold;">m3d_renderer_shaded</span>

Base class for interpolated shading.

Key methods:

- <span style="color: purple; font-weight: bold;">virtual void render(m3d_world &world)</span>
- <span style="color: purple; font-weight: bold;">void store_iscanlines(unsigned runlen, float z1, float z2, float val1, float val2, unsigned start = 0)</span>
- <span style="color: purple; font-weight: bold;">virtual void triangle_fill_shaded(m3d_render_object &obj, m3d_vertex *vtx[], m3d_world &world)</span>

Protected data:

- `float *iscanline`
- `struct m3d_render_color colors[3]`

### <span style="color: green; font-weight: bold;">m3d_renderer_shaded_gouraud</span>

Interpolates per-vertex colors across the triangle.

Key methods:

- <span style="color: purple; font-weight: bold;">void store_cscanlines(unsigned runlen, m3d_color &val1, m3d_color &val2, unsigned start = 0)</span>
- <span style="color: purple; font-weight: bold;">virtual void triangle_fill_shaded(m3d_render_object &obj, m3d_vertex *vtx[], m3d_world &world)</span>

Protected data:

- `uint32_t *cscanline`

### <span style="color: green; font-weight: bold;">m3d_renderer_shaded_phong</span>

Interpolates normals and world positions for per-pixel Phong shading.

Key methods:

- <span style="color: purple; font-weight: bold;">void store_vscanlines(unsigned runlen, m3d_vertex &val1, m3d_vertex &val2, unsigned start = 0)</span>
- <span style="color: purple; font-weight: bold;">void store_wscanlines(unsigned runlen, m3d_vertex &val1, m3d_vertex &val2, unsigned start = 0)</span>
- <span style="color: purple; font-weight: bold;">virtual void triangle_fill_shaded(m3d_render_object &obj, m3d_vertex *vtx[], m3d_world &world)</span>

Protected data:

- `m3d_vector *vscanline`
- `m3d_point *wscanline`

## 9. Display API

### <span style="color: green; font-weight: bold;">m3d_display</span>

Platform-independent display abstraction.

Key methods:

- `virtual uint32_t *get_video_buffer(int x0, int y0) = 0`
- `int get_xmax(void)`
- `int get_ymax(void)`
- `virtual void fill_buffer(void) = 0`
- `virtual void show_buffer(void) = 0`
- `virtual void clear_buffer(void) = 0`
- `virtual void set_color(uint8_t red, uint8_t green, uint8_t blue) = 0`
- `virtual void draw_lines(m3d_display_point pts[], unsigned ptsnum) = 0`
- `virtual void clear_renderer(void) = 0`
- `virtual void show_renderer(void) = 0`

### <span style="color: green; font-weight: bold;">m3d_display_wingdi</span>

Windows-specific DIB implementation used by the demo application.

Key methods:

- `m3d_display_wingdi(HWND m_hwnd, int xres, int yres)`
- `virtual ~m3d_display_wingdi()`
- `virtual uint32_t *get_video_buffer(int x0, int y0)`
- `virtual void fill_buffer(void)`
- `virtual void show_buffer(void)`
- `virtual void clear_buffer(void)`
- `virtual void set_color(uint8_t red, uint8_t green, uint8_t blue)`
- `virtual void draw_lines(m3d_display_point pts[], unsigned ptsnum)`
- `virtual void clear_renderer(void)`
- `virtual void show_renderer(void)`

## 10. Z-buffer API

### <span style="color: green; font-weight: bold;">m3d_zbuffer</span>

Stores a per-pixel depth map for visibility testing.

Key methods:

- `m3d_zbuffer(int16_t xres, int16_t yres)`
- `~m3d_zbuffer()`
- `void reset(void)`
- `bool test_update(int16_t x0, int16_t y0, float z)`
- `bool test_update(float *zbuf, float z)`
- `inline float *get_zbuffer(int16_t x0, int16_t y0)`

## 11. Summary of public usage

The most important types to know for application-level integration are:

- <span style="color: green; font-weight: bold;">m3d_world</span>
- <span style="color: green; font-weight: bold;">m3d_camera</span>
- <span style="color: green; font-weight: bold;">m3d_render_object</span>
- <span style="color: green; font-weight: bold;">m3d_color</span>
- <span style="color: green; font-weight: bold;">m3d_point_light_source</span>
- <span style="color: green; font-weight: bold;">m3d_renderer</span>
- <span style="color: green; font-weight: bold;">m3d_display</span>

Everything else in the engine is either a low-level math utility or a specialized implementation detail used by the renderers.
