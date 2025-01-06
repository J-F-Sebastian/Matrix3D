/*
 * Matrix3D
 *
 * Copyright (C) 1995 - 2025 Diego Gallizioli
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef M3D_MATH_DATA_HH
#define M3D_MATH_DATA_HH

/*
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