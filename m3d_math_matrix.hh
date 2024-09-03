#ifndef _M3D_MATH_MATRIX_HH_
#define _M3D_MATH_MATRIX_HH_

#include "m3d_math_vector.hh"

/*
 * A matrix in column,raw order
 * The default matrix is the Identity matrix
 */
class m3d_matrix
{
public:
        m3d_matrix();
        m3d_matrix(const float values[][m3d_vector_size]);
        m3d_matrix(const m3d_matrix &mat);

        void operator=(const m3d_matrix &other);

        void operator=(const float other[][m3d_vector_size]);

        /*
         * Insert a vector at the specified row; existing values are overwritten
         */
        void insert(const m3d_vector &vector, unsigned row);

        /*
         * perform multiplication of 2 matrices, rows by columns.
         * NOTE: Store the result in mymatrix.
         */
        void multiply(m3d_matrix &mat);

        /*
         * perform multiplication of mymatrix and a vector.
         * NOTE: Store the result in vector. mymatrix is not altered.
         */
        void multiply(m3d_vector &vector);

        /*
         * transpose matrix.
         */
        void transpose(void);

        /*
         * add vector to the T elements of mymatrix
         */
        void translate(m3d_vector &vector);

        /*
         * compute the rotation matrix for an arbitrary vector.
         * Rotation is around the passed in vector.
         * Vector components need to be normalized.
         */
        void rotation_matrix_vect(m3d_vector &veca, float angle);

        /*
         * compute the reflection matrix for an arbitrary vector.
         * Reflection is around the passed in vector.
         * This is the same as a rotation of 180 degrees around the passed in vector.
         * Vector components need to be normalized.
         */
        void reflect_matrix_vect(m3d_vector &veca);

        /*
         * compute the orientation matrix from an arbitrary vector to another
         * arbitrary vector.
         * Vector components need to be normalized.
         */
        void orientation_matrix(m3d_vector &veca, m3d_vector &vecb);

        /*
         * compute the orientation matrix from an arbitrary vector to y axis.
         * Vector components need to be normalized.
         * The rotation will be computed as the composition of 2 rotations:
         * one from the vector to his projection on xy plane
         * one from the projection on xy plane to the y axis vector.
         *
         */
        void orientation_matrix_vect_y(m3d_vector &veca);

        /*
         * perform rotation of a vector, out = mymatrix*veca
         */
        void rotate(m3d_vector &veca, m3d_vector &out);

        /*
         * perform rotation and motion of a vector, the matrix need to be build
         * this way:
         *
         *   Xr1 Xr2 Xr3 Xt
         *   Yr1 Yr2 Yr3 Yt
         *   Zr1 Zr2 Zr3 Zt
         *     0   0   0  1
         *
         */
        void transform(m3d_vector &veca, m3d_vector &out);

        /*
         * stream to cout the matrix
         */
        void print();

        static void print(const float matrix[][m3d_vector_size]);

#if defined(_MSC_VER)
        __declspec(align(16)) float mymatrix[m3d_vector_size][m3d_vector_size];
#else
        float mymatrix[m3d_vector_size][m3d_vector_size] __attribute__((aligned(16)));
#endif
};

class m3d_matrix_roll : public m3d_matrix
{
public:
        explicit m3d_matrix_roll(float angle);
};

class m3d_matrix_pitch : public m3d_matrix
{
public:
        explicit m3d_matrix_pitch(float angle);
};

class m3d_matrix_yaw : public m3d_matrix
{
public:
        explicit m3d_matrix_yaw(float angle);
};

class m3d_matrix_rotation : public m3d_matrix
{
public:
        explicit m3d_matrix_rotation(float pitch, float yaw, float roll);
};

class m3d_matrix_identity : public m3d_matrix
{
public:
        m3d_matrix_identity();
};

class m3d_matrix_camera : public m3d_matrix
{
public:
        m3d_matrix_camera() : m3d_matrix() {}
        m3d_matrix_camera(const m3d_input_point &campos, const m3d_input_point &lookat);
};

/*
 * Class m3d_frustum build a projection matrix transforming the camera view inside a frustum
 * to a cube.
 * The transformation implements the perspective projection of 3D points from the camera (world) system
 * to the screen system.
 * Magnitude of the vector of the projected point is normalized, while original depth Z is retained in T field.
 * The original depth has a changed sign, i.e. points have positive depth instead of negative as in the camera system.
 */
class m3d_frustum : public m3d_matrix
{
public:
        /*
         * fowangle is the viewpoint width in degrees.
         * xres and yres are the screen resolutions in pixels.
         * near and far are the distance of the near and far planes from the viewpoint.
         */
        m3d_frustum(const float fowangle, const int xres, const int yres, const float near, const float far);
        /*
         * fowangle is the viewpoint width in degrees.
         * xres and yres are the screen resolutions in pixels.
         * near is the distance of the near plane from the viewpoint.
         * far plane is set ad infinitum.
         */
        m3d_frustum(const float fowangle, const int xres, const int yres, const float near);
};

#endif