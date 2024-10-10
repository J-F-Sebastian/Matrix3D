#ifndef M3D_MATH_VECTOR_HH
#define M3D_MATH_VECTOR_HH

/*
 * Copyright 2014-2024 Diego Gallizioli
 * Source file name:
 * Description: float math for 3d vectors / matrices computation
 * Notes: ALL VECTORS MUST BE 4 ITEM LONG
 * ALL MATRIXES MUST BE 4x4 ITEM LONG
 * ALL VALUES ARE FLOAT (single precision)
 *
 * vector format: {x - y - z - t}
 * matrix format: /x - y - z - t\
 *                |x - y - z - t|
 *                |x - y - z - t|
 *                \x - y - z - t/
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
 * Rotations are clockwise around the referred axis.
 *
 * Class m3d_vector implements a translation-invariant vector.
 * All basic computations and operations with vectors are implemented
 * as methods.
 * Module, rotations, basic operations (addition, subtraction, etc.) plus
 * dot and vector products.
 */

#include "m3d_math_data.hh"

class m3d_vector
{
public:
        m3d_vector();
        m3d_vector(const float values[]);
        m3d_vector(const m3d_vector &other);
        ~m3d_vector();

        m3d_vector(const struct m3d_input_point &point)
        {
                myvector[X_C] = point.vector[X_C];
                myvector[Y_C] = point.vector[Y_C];
                myvector[Z_C] = point.vector[Z_C];
                myvector[T_C] = 0.0f;
        }

        m3d_vector(const float x, const float y, const float z)
        {
                myvector[X_C] = x;
                myvector[Y_C] = y;
                myvector[Z_C] = z;
                myvector[T_C] = 0.0f;
        }

        void operator=(const m3d_vector &other)
        {
                myvector[X_C] = other.myvector[X_C];
                myvector[Y_C] = other.myvector[Y_C];
                myvector[Z_C] = other.myvector[Z_C];
                myvector[T_C] = other.myvector[T_C];
        }

        void operator=(const float other[])
        {
                myvector[X_C] = other[X_C];
                myvector[Y_C] = other[Y_C];
                myvector[Z_C] = other[Z_C];
                myvector[T_C] = 0.0f;
        }

        float &operator[](int idx)
        {
                return myvector[idx];
        }

        const float &operator[](int idx) const
        {
                return myvector[idx];
        }

        friend m3d_vector operator+(const m3d_vector &veca, const m3d_vector &vecb)
        {
                m3d_vector ret(veca);
                ret.add(vecb);
                return ret; // return the result by value (uses move constructor)
        }

        /*
         * perform cross product myvector X veca and store the result in myvector.
         * NOTE: the value of myvector is altered.
         */
        void cross_product(const m3d_vector &veca);

        /*
         * perform dot product myvector * veca and return the scalar result.
         * NOTE: the value of myvector is NOT altered.
         */
        float dot_product(const m3d_vector &veca);

        /*
         * perform subtraction
         */
        void subtract(const m3d_vector &veca);

        /*
         * perform addition
         */
        void add(const m3d_vector &veca);

        /*
         * compute the module
         */
        float module(void);

        /*
         * compute the square module
         */
        float module2(void);

        /*
         * perform normalization
         */
        void normalize(void);

        /*
         * perform scaling, multiply X,Y,Z by a scalar value.
         * T is unchanged.
         */
        void scale(float val);

        /*
         * perform mirroring of a vector, same as negate in algebra.
         * The resulting vector is a simmetry through the origin (0,0,0).
         */
        void mirror(void);

        /*
         * perform rolling of a vector, rotating around z angle in degrees
         */
        void roll(float angle);

        /*
         * perform yawing of a vector, rotating around y angle in degrees
         */
        void yaw(float angle);

        /*
         * perform pitching of a vector, rotating around x angle in degrees
         */
        void pitch(float angle);

        /*
         * stream the vector to cout
         */
        void print();

        static void print(const float vector[]);

#if defined(_MSC_VER)
        __declspec(align(16)) float myvector[m3d_vector_size];
#else
        float myvector[m3d_vector_size] __attribute__((aligned(16)));
#endif
};

#endif