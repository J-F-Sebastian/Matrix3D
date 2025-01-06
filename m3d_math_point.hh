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

#ifndef M3D_MATH_POINT_HH_INCLUDED
#define M3D_MATH_POINT_HH_INCLUDED

#include "m3d_math_vector.hh"

/*
 * A point in space described as a vector, affected by translation
 */
class m3d_point : public m3d_vector
{
public:
        m3d_point();
        m3d_point(const float values[]);
        m3d_point(const m3d_point &other);
        ~m3d_point();

        m3d_point(const struct m3d_input_point &point)
        {
                myvector[X_C] = point.vector[X_C];
                myvector[Y_C] = point.vector[Y_C];
                myvector[Z_C] = point.vector[Z_C];
                myvector[T_C] = point.vector[T_C];
        }

        m3d_point(const float x, const float y, const float z)
        {
                myvector[X_C] = x;
                myvector[Y_C] = y;
                myvector[Z_C] = z;
                myvector[T_C] = 1.0f;
        }

        void operator=(const m3d_point &other)
        {
                myvector[X_C] = other.myvector[X_C];
                myvector[Y_C] = other.myvector[Y_C];
                myvector[Z_C] = other.myvector[Z_C];
                myvector[T_C] = other.myvector[T_C];
        }

        friend m3d_point operator+(const m3d_point &veca, const m3d_point &vecb)
        {
                m3d_point ret(veca);
                ret.add(vecb);
                return ret; // return the result by value (uses move constructor)
        }
};

#endif