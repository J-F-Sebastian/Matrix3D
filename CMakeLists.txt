cmake_minimum_required(VERSION 3.10)
project(Matrix3D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set Release as default build type for NMake
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(matrix3d WIN32
    main.cpp
    m3d_bmp.cpp
    m3d_camera.cpp
    m3d_color.cpp
    #m3d_display_sdl.cpp
    m3d_display_wingdi.cpp
    m3d_illum.cpp
    m3d_interp.cpp
    m3d_light_source.cpp
    m3d_math_axis.cpp
    m3d_math_matrix.cpp
    m3d_math_point.cpp
    m3d_math_vector.cpp
    m3d_math.cpp
    m3d_object.cpp
    m3d_renderer_flat.cpp
    m3d_renderer_gouraud.cpp
    m3d_renderer_phong.cpp
    m3d_renderer_shaded.cpp
    m3d_renderer_wireframe.cpp
    m3d_renderer.cpp
    m3d_vertex.cpp
    m3d_world.cpp
    m3d_zbuffer.cpp
)

if(MSVC)
  #target_compile_options(matrix3d PRIVATE /MT /W4 /EHcs /O2 /I${CMAKE_SOURCE_DIR}/../SDL2-32/include/SDL2 /I${CMAKE_SOURCE_DIR}/../SDL2_ttf/include)
  #target_link_libraries(matrix3d PRIVATE ${CMAKE_SOURCE_DIR}/../SDL2-32/lib/SDL2.lib ${CMAKE_SOURCE_DIR}/../SDL2_ttf/lib/x86/SDL2_ttf.lib)
  #target_link_options(matrix3d PRIVATE /subsystem:console)
  target_compile_options(matrix3d PRIVATE /W4 /EHcs /O2)
endif()

include(CTest)
if(BUILD_TESTING)
  add_executable(m3d_bmp_test
      tests/m3d_bmp_test.cpp
      m3d_bmp.cpp
      m3d_color.cpp
  )
  target_include_directories(m3d_bmp_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  add_test(NAME m3d_bmp_test COMMAND m3d_bmp_test)
endif()
