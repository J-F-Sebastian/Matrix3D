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

#include "m3d_math_point.hh"

/**********************************************************************************************
 **********************************************************************************************
 **********************************************************************************************
                                     M3D_POINT
 **********************************************************************************************
 **********************************************************************************************
 **********************************************************************************************/

m3d_point::m3d_point()
{
        myvector[X_C] = myvector[Y_C] = myvector[Z_C] = 0.0f;
        myvector[T_C] = 1.0f;
}

m3d_point::m3d_point(const float values[])
{
        myvector[X_C] = values[X_C];
        myvector[Y_C] = values[Y_C];
        myvector[Z_C] = values[Z_C];
        myvector[T_C] = 1.0f;
}

m3d_point::m3d_point(const m3d_point &other) : m3d_vector(other.myvector)
{
        myvector[T_C] = 1.0f;
}

m3d_point::~m3d_point()
{
}