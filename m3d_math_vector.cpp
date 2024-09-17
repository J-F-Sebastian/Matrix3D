
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "m3d_math_vector.hh"

using namespace std;

#define INV_RAD ((float)M_PI / 180.0f)

/**********************************************************************************************
 **********************************************************************************************
 **********************************************************************************************
                                     M3D_VECTOR
 **********************************************************************************************
 **********************************************************************************************
 **********************************************************************************************/

m3d_vector::m3d_vector()
{
        myvector[X_C] = myvector[Y_C] = myvector[Z_C] = myvector[T_C] = 0.0f;
}

m3d_vector::m3d_vector(const float values[])
{
        myvector[X_C] = values[X_C];
        myvector[Y_C] = values[Y_C];
        myvector[Z_C] = values[Z_C];
        myvector[T_C] = 0.0f;
}

m3d_vector::m3d_vector(const m3d_vector &other)
{
        (*this) = other;
}

m3d_vector::~m3d_vector()
{
}

/*
 * perform cross product
 */
void m3d_vector::cross_product(const m3d_vector &veca)
{
        float temp[m3d_vector_size];

        temp[X_C] = myvector[Y_C] * veca.myvector[Z_C] - myvector[Z_C] * veca.myvector[Y_C];
        temp[Y_C] = myvector[Z_C] * veca.myvector[X_C] - myvector[X_C] * veca.myvector[Z_C];
        temp[Z_C] = myvector[X_C] * veca.myvector[Y_C] - myvector[Y_C] * veca.myvector[X_C];
        temp[T_C] = 0.0f;

        (*this) = temp;
}

/*
 * perform dot product
 */
float m3d_vector::dot_product(const m3d_vector &veca)
{
        return (veca.myvector[X_C] * myvector[X_C] +
                veca.myvector[Y_C] * myvector[Y_C] +
                veca.myvector[Z_C] * myvector[Z_C]);
}

/*
 * perform subtraction
 */
void m3d_vector::subtract(const m3d_vector &veca)
{
        myvector[X_C] -= veca.myvector[X_C];
        myvector[Y_C] -= veca.myvector[Y_C];
        myvector[Z_C] -= veca.myvector[Z_C];
}

/*
 * perform addition
 */
void m3d_vector::add(const m3d_vector &veca)
{
        myvector[X_C] += veca.myvector[X_C];
        myvector[Y_C] += veca.myvector[Y_C];
        myvector[Z_C] += veca.myvector[Z_C];
}

/*
 * compute the module
 */
float m3d_vector::module()
{
        return (sqrtf(myvector[X_C] * myvector[X_C] +
                      myvector[Y_C] * myvector[Y_C] +
                      myvector[Z_C] * myvector[Z_C]));
}

/*
 * compute the square module
 */
float m3d_vector::module2()
{
        return (myvector[X_C] * myvector[X_C] +
                myvector[Y_C] * myvector[Y_C] +
                myvector[Z_C] * myvector[Z_C]);
}

/*
 * perform normalization
 */
void m3d_vector::normalize()
{
        float temp;

        temp = module();

        if (temp != 0.0f)
        {
                temp = 1.0f / temp;
                myvector[X_C] *= temp;
                myvector[Y_C] *= temp;
                myvector[Z_C] *= temp;
        }
}

void m3d_vector::scale(float val)
{
        myvector[X_C] *= val;
        myvector[Y_C] *= val;
        myvector[Z_C] *= val;
}

/*
 * perform mirroring of a vector, same as negate in algebra.
 * The resulting vector is a simmetry through the origin (0,0,0).
 */
void m3d_vector::mirror(void)
{
        myvector[X_C] = -myvector[X_C];
        myvector[Y_C] = -myvector[Y_C];
        myvector[Z_C] = -myvector[Z_C];
}

/*
 * perform rolling of a vector, rotating counterclockwise around z angle in degrees,
 * x axis to y axis
 */
void m3d_vector::roll(float angle)
{
        float cosz = cos(angle * INV_RAD);
        float sinz = sin(angle * INV_RAD);
        float temp[m3d_vector_size];

        temp[X_C] = myvector[X_C] * cosz - myvector[Y_C] * sinz;
        temp[Y_C] = myvector[X_C] * sinz + myvector[Y_C] * cosz;
        temp[Z_C] = myvector[Z_C];
        temp[T_C] = myvector[T_C];

        (*this) = temp;
}

/*
 * perform yawing of a vector, rotating counterclockwise around y angle in degrees,
 * z axis to x axis
 */
void m3d_vector::yaw(float angle)
{
        float cosy = cos(angle * INV_RAD);
        float siny = sin(angle * INV_RAD);
        float temp[m3d_vector_size];

        temp[X_C] = myvector[X_C] * cosy + myvector[Z_C] * siny;
        temp[Y_C] = myvector[Y_C];
        temp[Z_C] = -myvector[X_C] * siny + myvector[Z_C] * cosy;
        temp[T_C] = myvector[T_C];

        (*this) = temp;
}

/*
 * perform pitching of a vector, rotating counterclockwise around x angle in degrees,
 * y axis to z axis
 */
void m3d_vector::pitch(float angle)
{
        float cosx = cos(angle * INV_RAD);
        float sinx = sin(angle * INV_RAD);
        float temp[m3d_vector_size];

        temp[X_C] = myvector[X_C];
        temp[Y_C] = myvector[Y_C] * cosx - myvector[Z_C] * sinx;
        temp[Z_C] = myvector[Y_C] * sinx + myvector[Z_C] * cosx;
        temp[T_C] = myvector[T_C];

        (*this) = temp;
}

void m3d_vector::print()
{
#ifdef DEBUG
        m3d_vector::print(myvector);
#endif
}

void m3d_vector::print(const float vector[])
{
#ifdef DEBUG
        ostringstream temp;

        temp.setf(ios_base::showpoint | ios_base::showpos);
        temp.precision(6);
        temp << "[X " << vector[X_C] << " Y " << vector[Y_C] << " Z " << vector[Z_C] << " T " << vector[T_C] << "]" << endl;
        cout << temp.str();
#else
        (void)vector;
#endif
}