#ifndef M3D_MATH_DATA_HH
#define M3D_MATH_DATA_HH

/*
 * Copyright 2014-2024 Diego Gallizioli
 * Source file name:
 * Description: float math for 3d vectors / matrices computation
 * Notes: ALL VECTORS MUST BE 4 ITEM LONG
 * ALL MATRIXES MUST BE 4x4 ITEM LONG
 * ALL VALUES ARE FLOAT (single precision)
 *
 * vector format: {x - y - z - t}
 * matrix format: /x - x - x - x\
 *                |y - y - y - y|
 *                |z - z - z - z|
 *                \t - t - t - t/
 *
 * Roll is a rotation around z axis
 * Yaw is a rotation around y axis
 * Pitch is a rotation around x axis
 * Rotations follow the 3-fingers rule for computation: the rotation axis has the
 * same versus and direction of the cross product of the remaining 2 axis, rotating
 * in the direction of the rotation itself.
 * z axis - from the monitor to you
 * y axis - from floor to ceiling
 * x axis - from left to right
 *
 * Rotations are clockwise around the referred axis
 */

#define m3d_trig_table_size (4 * 1024)
#define m3d_vector_size (4)
#define m3d_matrix_size (4 * 4)

#define X_C (0)
#define Y_C (1)
#define Z_C (2)
#define T_C (3)

struct m3d_input_point
{
#if defined(_MSC_VER)
        __declspec(align(16)) float vector[m3d_vector_size];
#else
        float vector[m3d_vector_size] __attribute__((aligned(16)));
#endif
};

#endif