#ifndef M3D_MATH_AXIS_HH_INCLUDED
#define M3D_MATH_AXIS_HH_INCLUDED

#include "m3d_math_vector.hh"

/*
 * A normalized vector describing X,Y,or Z axis
 */
class m3d_axis : public m3d_vector
{
public:
        m3d_axis() : m3d_vector() { normalize(); };
        explicit m3d_axis(const float values[]) : m3d_vector(values) { normalize(); };
        m3d_axis(const m3d_axis &other) : m3d_vector(other) { normalize(); };
        ~m3d_axis() {};

        float x(void)
        {
                return myvector[X_C];
        }

        float y(void)
        {
                return myvector[Y_C];
        }

        float z(void)
        {
                return myvector[Z_C];
        }

        float t(void)
        {
                return myvector[T_C];
        }
};

class m3d_axis_x : public m3d_axis
{
public:
        m3d_axis_x();
};

class m3d_axis_y : public m3d_axis
{
public:
        m3d_axis_y();
};

class m3d_axis_z : public m3d_axis
{
public:
        m3d_axis_z();
};

#endif