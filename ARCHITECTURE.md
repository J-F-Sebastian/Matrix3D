# Architecture

<span style="color: green; font-weight: bold;">Matrix3D</span> is organized as a layered rendering system. At the lowest level there is a math kernel, then a scene/object model, then a camera/world layer, then renderer implementations, and finally a platform display backend.

<span style="color: gray;">The architecture is intentionally direct: data structures are plain C++ classes and arrays, the rendering logic is explicit, and the “scene graph” is reduced to a list of render objects plus one camera and a collection of lights.</span>

## High-level system layout

```mermaid
%%{init:{'theme':'base','themeVariables':{ 'primaryColor':'#dfe9ff','primaryTextColor':'#102a43','primaryBorderColor':'#5b8def','lineColor':'#2d6cdf','secondaryColor':'#edf3ff','tertiaryColor':'#eef7ff'}}}%%
flowchart TB
    A[Application / Demo] --> B[World]
    B --> C[Camera]
    B --> D[Render Objects]
    B --> E[Light Sources]
    D --> F[Mesh and Vertices]
    F --> G[Math Layer]
    C --> G
    E --> H[Illumination]
    D --> I[Renderer]
    I --> J[Z-buffer + Rasterizer]
    J --> K[Display Backend]
```

## Object hierarchy

The object layout is simple and linear:

```mermaid
%%{init:{'theme':'base','themeVariables':{ 'primaryColor':'#dfe9ff','primaryTextColor':'#102a43','primaryBorderColor':'#5b8def','lineColor':'#2d6cdf','secondaryColor':'#edf3ff','tertiaryColor':'#eef7ff'}}}%%
classDiagram
    class m3d_vector
    class m3d_point
    class m3d_vertex
    class m3d_triangle
    class m3d_object
    class m3d_render_object
    class m3d_world
    class m3d_camera
    class m3d_renderer
    class m3d_renderer_wireframe
    class m3d_renderer_flat
    class m3d_renderer_shaded
    class m3d_renderer_shaded_gouraud
    class m3d_renderer_shaded_phong
    class m3d_display
    class m3d_display_wingdi

    m3d_point --|> m3d_vector
    m3d_vertex --> m3d_point
    m3d_vertex --> m3d_vector
    m3d_triangle --> m3d_vector
    m3d_object --> m3d_vertex
    m3d_object --> m3d_triangle
    m3d_render_object --|> m3d_object
    m3d_world --> m3d_render_object
    m3d_world --> m3d_camera
    m3d_renderer --> m3d_world
    m3d_renderer --> m3d_display
    m3d_renderer_wireframe --|> m3d_renderer
    m3d_renderer_flat --|> m3d_renderer
    m3d_renderer_shaded --|> m3d_renderer
    m3d_renderer_shaded_gouraud --|> m3d_renderer_shaded
    m3d_renderer_shaded_phong --|> m3d_renderer_shaded
    m3d_display_wingdi --|> m3d_display
```

### Core object model

- <span style="color: green; font-weight: bold;">m3d_object</span>
  - stores a mesh of vertices and a triangle index list
  - tracks center, direction, orientation, and update state
  - can rotate by pitch, yaw, and roll with <span style="color: purple; font-weight: bold;">pitch()</span>, <span style="color: purple; font-weight: bold;">yaw()</span>, and <span style="color: purple; font-weight: bold;">roll()</span>
- <span style="color: green; font-weight: bold;">m3d_render_object</span>
  - extends the base object with render metadata
  - stores color, visibility bitsets, and projected depth values
  - contains the per-face visibility state used by the rasterizers
- <span style="color: green; font-weight: bold;">m3d_triangle</span>
  - describes a triangular face as three indices into the vertex array
  - stores face normal data in object and projected space

## Transformation and math layer

The math subsystem provides the primitive operations used by the renderer:

- <span style="color: green; font-weight: bold;">m3d_vector</span>
  - carries four components in homogeneous form, usually `{x, y, z, t}`
  - exposes operations for dot and cross products, normalization, scaling, mirroring, and rotation
- <span style="color: green; font-weight: bold;">m3d_point</span>
  - is a position-like vector with translation semantics
- <span style="color: green; font-weight: bold;">m3d_matrix</span>
  - holds a 4x4 transformation matrix and supports multiply, transpose, rotation, translation, and orientation transforms
- <span style="color: green; font-weight: bold;">m3d_matrix_camera</span>
  - builds the camera-to-world viewing transform
- <span style="color: green; font-weight: bold;">m3d_frustum</span>
  - builds the perspective projection matrix used to convert camera space to normalized projected space

This is a low-level linear algebra API, not a general-purpose simulation framework. The matrix model is explicitly designed around 4D vectors, which matches the homogeneous coordinate conventions used in the renderer.

## Camera and projection model

The camera is defined by a position, a look-at target, screen size, and a frustum.

```mermaid
%%{init:{'theme':'base','themeVariables':{ 'primaryColor':'#dfe9ff','primaryTextColor':'#102a43','primaryBorderColor':'#5b8def','lineColor':'#2d6cdf','secondaryColor':'#edf3ff','tertiaryColor':'#eef7ff'}}}%%
sequenceDiagram
    participant Obj as Object
    participant Cam as m3d_camera
    participant M as Transform Matrix
    participant F as Frustum
    participant Screen as Display

    Obj->>Cam: position / normal in world coordinates
    Cam->>M: apply world-to-camera transform
    M-->>Cam: camera-space coordinates
    Cam->>F: perspective projection
    F-->>Cam: homogeneous clip-space points
    Cam->>Screen: convert to screen coordinates
```

The code path is:

1. object vertices are transformed into world space via object-local transform
2. camera applies world-to-camera transform
3. frustum converts camera space to clip space
4. screen conversion maps normalized coordinates to pixel coordinates

The important detail is the engine keeps the original depth in the `T` component and normalizes x/y/z by `T`, which is the usual perspective divide used in software rendering.

## World and scene graph

<span style="color: green; font-weight: bold;">m3d_world</span> is the scene container. It stores:

- a list of render objects
- a list of point lights and ambient light
- a camera used for the current rendering pass

The world also provides sorting logic in <span style="color: purple; font-weight: bold;">sort()</span>. It computes each object's projected center depth and orders the visible list from farthest to nearest in the fragment stage. This is essential for painter's algorithm behavior and z-buffer correctness.

## Lighting model

The light system is intentionally simple but representative:

- <span style="color: green; font-weight: bold;">m3d_light_source</span> is an abstract base
- <span style="color: green; font-weight: bold;">m3d_ambient_light</span> adds constant global illumination
- <span style="color: green; font-weight: bold;">m3d_point_light_source</span> attenuates by distance
- <span style="color: green; font-weight: bold;">m3d_spot_light_source</span> is a directional variant in the same family

All lighting contributions are computed in <span style="color: green; font-weight: bold;">m3d_illumination</span> and its singleton <span style="color: green; font-weight: bold;">m3d_illum</span>.

The typical shading pipeline is:

- ambient light: adds a base intensity
- diffuse light: uses Lambertian term from light direction and face normal
- specular light: uses a half-vector approximation

## Rasterization pipeline

The base renderer manages the common responsibilities:

- clear the screen buffer
- compute visible objects by projecting the mesh
- sort visible objects by depth
- reset and manage z-buffer
- expose scanline interpolation helpers

The concrete renderers are selected by the demo application:

- <span style="color: green; font-weight: bold;">m3d_renderer_wireframe</span>
  - draws triangle edges only
- <span style="color: green; font-weight: bold;">m3d_renderer_flat</span>
  - fills triangles with a single object color averaged from vertices
- <span style="color: green; font-weight: bold;">m3d_renderer_shaded</span>
  - performs intensity interpolation over a triangle surface
- <span style="color: green; font-weight: bold;">m3d_renderer_shaded_gouraud</span>
  - interpolates vertex colors across the triangle
- <span style="color: green; font-weight: bold;">m3d_renderer_shaded_phong</span>
  - interpolates normals and world positions to compute per-pixel lighting

## Z-buffer and fill strategy

The rasterizer is scanline-based rather than modern tile-based. It follows a classic software-rendering pattern:

1. sort triangle vertices by screen y
2. build left/right edge scanline buffers
3. interpolate x, z, and shading data across each scanline
4. for each pixel, call the z-buffer test
5. update the pixel only when the depth is closer

This appears in the shared scaffolding and in each concrete renderer. The z-buffer is implemented by <span style="color: green; font-weight: bold;">m3d_zbuffer</span>, which stores a float depth for every screen pixel and tests whether a new pixel is closer than the current one.

## Display backend

The display abstraction is intentionally thin:

- <span style="color: green; font-weight: bold;">m3d_display</span> defines the platform-independent interface
- <span style="color: green; font-weight: bold;">m3d_display_wingdi</span> uses a DIB-backed Win32 surface for the windowed demo

A second backend exists in <span style="color: purple; font-weight: bold;">m3d_display_sdl.cpp</span>, but the default project configuration in <span style="color: purple; font-weight: bold;">CMakeLists.txt</span> compiles the Windows backend instead.

## Detailed pipeline

```mermaid
%%{init:{'theme':'base','themeVariables':{ 'primaryColor':'#dfe9ff','primaryTextColor':'#102a43','primaryBorderColor':'#5b8def','lineColor':'#2d6cdf','secondaryColor':'#edf3ff','tertiaryColor':'#eef7ff'}}}%%
flowchart LR
    A[Create mesh] --> B[Build object]
    B --> C[Compute center + normals]
    C --> D[Apply object transforms]
    D --> E[Camera transform]
    E --> F[Perspective projection]
    F --> G[Back-face culling]
    G --> H[Sort visible objects]
    H --> I[Rasterize triangles]
    I --> J[Z-buffer test]
    J --> K[Shading]
    K --> L[Display buffer]
    L --> M[Window output]
```

## Design strengths and trade-offs

### Strengths

- Clear separation between math, scene, and renderer
- Small and understandable core; easy to trace in one session
- Explicit pipeline makes it suitable as a teaching renderer
- Multiple shading modes show the progression from wireframe to Phong

### Trade-offs

- There is no full scene graph, no asset system, and no modern ECS or component architecture
- The object model is tightly coupled to triangle meshes and fixed-size buffers
- The Win32 backend makes the demo Windows-centric despite the code being mostly portable in concept
- The code contains some legacy patterns and older-style C++ conventions

## Architectural summary

The architecture is a classic software rasterizer, implemented as a compact engine library plus a Windows demo. It uses homogeneous math and matrix-based transforms, a custom camera frustum, a z-buffer, and direct scanline interpolation for fill operations. In terms of design, it is intentionally less abstract than a modern engine but much easier to follow and modify.
