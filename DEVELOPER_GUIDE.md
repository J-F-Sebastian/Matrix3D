# Developer Guide

<span style="color: gray;">This project is written as a compact 3D rendering library with a demo application. In the repository, main.cpp is not the library itself; it is a showcase that wires library objects together to produce a minimal interactive scene.</span>

## 1. Project purpose

<span style="color: green; font-weight: bold;">Matrix3D</span> is a software-rendering codebase intended to teach and demonstrate the pipeline behind a 3D engine without the weight of a full engine framework. The library includes:

- geometry and matrix math
- mesh and object modeling
- scene, camera, and light abstractions
- a collection of renderers
- a platform display backend

The public intent is to allow a developer to create a custom mesh, add it to a world, project it through a camera, and render it with different shading styles.

## 2. Build the project

The repository is configured with CMake. The default project target is the demo binary `matrix3d`.

```bash
cmake -S . -B build
cmake --build build --config Release
```

On Windows, the default `CMakeLists.txt` builds a Win32 executable using the native display backend. This matches the demo in <span style="color: purple; font-weight: bold;">main.cpp</span>.

## 3. Library usage model

The engine is structured as a library in the conceptual sense, even though it is not packaged as a static library target. The intended usage is:

1. define a mesh as `m3d_input_point[]` and `m3d_input_trimesh[]`
2. construct a <span style="color: green; font-weight: bold;">m3d_render_object</span>
3. create a <span style="color: green; font-weight: bold;">m3d_camera</span>
4. create a <span style="color: green; font-weight: bold;">m3d_world</span>
5. add the object and lights to the world
6. choose a renderer such as <span style="color: green; font-weight: bold;">m3d_renderer_wireframe</span> or <span style="color: green; font-weight: bold;">m3d_renderer_shaded_phong</span>
7. call <span style="color: purple; font-weight: bold;">render()</span> on the renderer

## 4. Typical setup sequence

```cpp
m3d_color color(255, 64, 64, 0);
m3d_render_object object;
m3d_ambient_light ambient(m3d_color(255, 255, 255, 0), 0.4f);
m3d_point_light_source light(lightpos, m3d_color(255, 255, 255, 0), 100.0f, 1.0f, 0.1f, 20000.0f);
m3d_camera camera(viewpoint, viewpointat, 1024, 768);
m3d_world world(ambient, camera);
m3d_display_wingdi display(hwnd, 1024, 768);
m3d_renderer_shaded_phong renderer(&display);

object.create(vertices, vertex_count, mesh, mesh_count, color);
world.add_light_source(light);
world.add_object(object);

renderer.render(world);
```

## 5. Building a mesh

A mesh is described by the following primitives:

- `m3d_input_point` — a 4-element homogeneous point/vertex
- `m3d_input_trimesh` — a triangle with three indices into the vertex list

For example, your object should follow this pattern:

```cpp
static struct m3d_input_point cube[] = {
    {{ 100.0f, 100.0f, 100.0f, 1.0f }},
    {{ 100.0f, 100.0f, -100.0f, 1.0f }},
    {{ -100.0f, 100.0f, -100.0f, 1.0f }},
    // ...
};

static struct m3d_input_trimesh cubemesh[] = {
    {{0, 1, 2}},
    {{0, 2, 3}},
    // ...
};
```

Then call:

```cpp
object.create(cube, NELEMENTS(cube), cubemesh, NELEMENTS(cubemesh), color);
```

## 6. Object transforms

The object model supports movement and rotation through the following methods:

- <span style="color: purple; font-weight: bold;">roll(float angle)</span>
- <span style="color: purple; font-weight: bold;">yaw(float angle)</span>
- <span style="color: purple; font-weight: bold;">pitch(float angle)</span>
- <span style="color: purple; font-weight: bold;">move(const m3d_vector &newposition)</span>
- <span style="color: purple; font-weight: bold;">set(const m3d_point &newposition)</span>

These update the mesh orientation and the center transform without requiring a separate transform manager. The actual matrix math is handled internally using a transformation matrix built from pitch, yaw, and roll.

## 7. Camera usage

The camera is initialized with:

```cpp
m3d_camera camera(viewpoint, viewpointat, xres, yres);
```

where:

- `viewpoint` is the camera position in world coordinates
- `viewpointat` is the point the camera looks at
- `xres` and `yres` are the framebuffer resolution

The camera computes view and projection transforms using a frustum matrix. Use the camera as a long-lived scene dependency rather than re-creating it for every draw call.

## 8. Lighting guidance

Lights are added to the world via:

- <span style="color: purple; font-weight: bold;">m3d_world::add_light_source()</span>
- <span style="color: purple; font-weight: bold;">m3d_world::set_ambient_light()</span>

A minimal setup uses:

```cpp
m3d_ambient_light ambient(color, 0.4f);
m3d_point_light_source light(lightpos, color, 100.0f, 1.0f, 0.1f, 20000.0f);
world.set_ambient_light(ambient);
world.add_light_source(light);
```

The renderer then calls the illumination functions that compute ambient, diffuse, and specular contributions. If you are implementing a custom shading pass, the safe extension point is the `m3d_illumination` hierarchy.

## 9. Selecting a renderer

The demo application cycles through these modes:

- `m3d_renderer_wireframe`
- `m3d_renderer_flat`
- `m3d_renderer_shaded`
- `m3d_renderer_shaded_gouraud`
- `m3d_renderer_shaded_phong`

Choose the renderer based on the visual effect you want:

- <span style="color: purple; font-weight: bold;">wireframe</span> — best for geometry debugging
- <span style="color: purple; font-weight: bold;">flat</span> — simplest shading, faster
- <span style="color: purple; font-weight: bold;">shaded</span> — basic diffuse/ambient interpolation
- <span style="color: purple; font-weight: bold;">Gouraud</span> — per-vertex color interpolation
- <span style="color: purple; font-weight: bold;">Phong</span> — per-pixel normal interpolation for more realistic light response

## 10. Display backends

The repository contains a Win32 display implementation but also includes a SDL option in the source tree.

- active by default: <span style="color: green; font-weight: bold;">m3d_display_wingdi</span>
- alternative source present: <span style="color: green; font-weight: bold;">m3d_display_sdl</span>

The CMake target currently uses the Win32 backend, which is what the demo expects. If you want to use another backend, you will likely need to update the build system and the platform-specific window creation logic.

## 11. Recommended extension points

If you want to build on this library, the most natural extensions are:

- add a new renderer: implement a subclass of <span style="color: green; font-weight: bold;">m3d_renderer</span>
- add a new primitive mesh generator: create a reusable mesh builder helper
- add texture coordinates: augment `m3d_vertex` and the renderers
- add a scene graph or a model importer: wrap the world/object infrastructure
- add a software clipping pass: extension of the camera/projection stage

## 12. Debugging tips

- use `print()` methods in the math and object classes to inspect normals and transforms
- enable DEBUG if needed in the source to obtain more verbose object output
- validate the mesh orientation carefully; back-face culling depends on the normal direction
- watch for negative or near-zero `T` values in projection math, since they affect perspective divide and visibility

## 13. Demo application workflow

The application in <span style="color: purple; font-weight: bold;">main.cpp</span> is intentionally simple:

- creates a few colored objects
- positions them in world space
- creates a camera and light source
- builds a world
- adds the objects to the world
- lets the user switch between renderers and rotate objects with keyboard controls

This makes it a useful reference implementation for how the library is intended to be used in practice.

## 14. Best practices

- keep mesh data in static arrays or generated assets rather than dynamic but unbounded containers
- avoid modifying object data while a render pass is in progress
- treat the renderer as the owner of the z-buffer state for a frame
- keep core math and scene logic decoupled from windowing code as much as possible

## 15. Summary

The project is a clear example of a minimal software rasterizer: object transforms, camera projection, illumination, z-buffering, and scanline rasterization are all present and separable. The code is best used as a learning library and as a starting point for a custom renderer, while the demo demonstrates how the engine is intended to be used in a real windowed application.
