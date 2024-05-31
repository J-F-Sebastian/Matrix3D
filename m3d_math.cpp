#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "m3d_math.hh"

using namespace std;

static const float INV_RAD = M_PI / 180.0f;

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
#ifdef NDEBUG
    m3d_vector::print(myvector);
#endif
}

void m3d_vector::print(const float vector[])
{
#ifdef NDEBUG
    ostringstream temp;

    temp.setf(ios_base::showpoint | ios_base::showpos);
    temp.precision(6);
    temp << "[X " << vector[X_C] << " Y " << vector[Y_C] << " Z " << vector[Z_C] << " T " << vector[T_C] << "]" << endl;
    cout << temp.str();
#endif
}

/**********************************************************************************************
 **********************************************************************************************
 **********************************************************************************************
                                     M3D_AXIS
 **********************************************************************************************
 **********************************************************************************************
 **********************************************************************************************/

/* normal vectors*/
static constexpr float x_axis[] = {1.0f, 0.0f, 0.0f, 0.0f};
static constexpr float y_axis[] = {0.0f, 1.0f, 0.0f, 0.0f};
static constexpr float z_axis[] = {0.0f, 0.0f, 1.0f, 0.0f};

m3d_axis_x::m3d_axis_x() : m3d_axis(x_axis)
{
}

m3d_axis_y::m3d_axis_y() : m3d_axis(y_axis)
{
}

m3d_axis_z::m3d_axis_z() : m3d_axis(z_axis)
{
}

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

/**********************************************************************************************
 **********************************************************************************************
 **********************************************************************************************
                                     M3D_MATRIX
 **********************************************************************************************
 **********************************************************************************************
 **********************************************************************************************/

m3d_matrix::m3d_matrix()
{
    unsigned i, j;

    for (i = 0; i < m3d_vector_size; i++)
    {
        for (j = 0; j < m3d_vector_size; j++)
        {
            mymatrix[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

m3d_matrix::m3d_matrix(const float values[][m3d_vector_size])
{
    (*this) = values;
}

m3d_matrix::m3d_matrix(const m3d_matrix &mat)
{
    (*this) = mat;
}

void m3d_matrix::insert(const m3d_vector &vector, unsigned row)
{
    if ((X_C == row) || (Y_C == row) || (Z_C == row) || (T_C == row))
    {
        mymatrix[X_C][row] = vector.myvector[X_C];
        mymatrix[Y_C][row] = vector.myvector[Y_C];
        mymatrix[Z_C][row] = vector.myvector[Z_C];
        mymatrix[T_C][row] = vector.myvector[T_C];
    }
}

void m3d_matrix::multiply(m3d_matrix &mat)
{
    unsigned i, j;
    float temp[m3d_vector_size][m3d_vector_size];

    for (i = 0; i < m3d_vector_size; i++)
    {
        for (j = 0; j < m3d_vector_size; j++)
        {
            temp[i][j] = row_by_col(mymatrix, mat.mymatrix, i, j);
        }
    }

    (*this) = temp;
}

void m3d_matrix::multiply(m3d_vector &vector)
{
    unsigned i, j;
    float temp[m3d_vector_size];

    for (i = 0; i < m3d_vector_size; i++)
    {
        temp[i] = 0.0f;
        for (j = 0; j < m3d_vector_size; j++)
        {
            temp[i] += mymatrix[j][i] * vector.myvector[j];
        }
    }

    vector = temp;
}

void m3d_matrix::transpose()
{
    unsigned i, j;
    float temp;

    for (i = 0; i < m3d_vector_size - 1; i++)
    {
        for (j = i + 1; j < m3d_vector_size; j++)
        {
            temp = mymatrix[i][j];
            mymatrix[i][j] = mymatrix[j][i];
            mymatrix[j][i] = temp;
        }
    }
}

void m3d_matrix::translate(m3d_vector &vector)
{
    for (unsigned i = 0; i < m3d_vector_size; i++)
        mymatrix[T_C][i] += vector.myvector[i];
}

/*
 * perform rotation of a vector, out = mymatrix*veca
 */
void m3d_matrix::rotate(m3d_vector &veca, m3d_vector &out)
{
    m3d_vector vecb;

    for (unsigned i = 0; i < T_C; i++)
    {
        vecb.myvector[i] = veca.myvector[X_C] * mymatrix[X_C][i] +
                           veca.myvector[Y_C] * mymatrix[Y_C][i] +
                           veca.myvector[Z_C] * mymatrix[Z_C][i];
    }
    vecb.myvector[T_C] = veca.myvector[T_C];
    out = vecb;
}

/*
 * perform rotation and motion of a vector, the matrix need to be build
 * this way:
 *
 *   Xr1 Xr2 Xr3 0
 *   Yr1 Yr2 Yr3 0
 *   Zr1 Zr2 Zr3 0
 *   Xt   Yt  Zt 1
 *
 */
void m3d_matrix::transform(m3d_vector &veca, m3d_vector &out)
{
    m3d_vector temp;

    for (unsigned i = 0; i < m3d_vector_size; i++)
    {
        temp.myvector[i] = veca.myvector[X_C] * mymatrix[X_C][i] +
                           veca.myvector[Y_C] * mymatrix[Y_C][i] +
                           veca.myvector[Z_C] * mymatrix[Z_C][i] +
                           veca.myvector[T_C] * mymatrix[T_C][i];
    }
    out = temp;
}

void m3d_matrix::print()
{
#ifdef NDEBUG
    m3d_matrix::print(mymatrix);
#endif
}

void m3d_matrix::print(const float matrix[][m3d_vector_size])
{
#ifdef NDEBUG
    ostringstream temp;

    temp.setf(ios_base::showpoint | ios_base::showpos);
    temp.precision(6);

    temp << "+-" << setw(51) << "-+" << endl;
    for (unsigned i = 0; i < m3d_vector_size; i++)
        temp << "|X " << setw(10) << matrix[X_C][i] << " Y " << setw(10) << matrix[Y_C][i] << " Z " << setw(10) << matrix[Z_C][i] << " T " << setw(10) << matrix[T_C][i] << "|" << endl;

    temp << "+-" << setw(51) << "-+" << endl;
    cout << temp.str();
#endif
}

/**********************************************************************************************/
m3d_matrix_roll::m3d_matrix_roll(float angle)
{
    float cosz = cos(angle * INV_RAD);
    float sinz = sin(angle * INV_RAD);

    mymatrix[X_C][X_C] = cosz;
    mymatrix[X_C][Y_C] = -sinz;
    mymatrix[X_C][Z_C] = 0.0f;
    mymatrix[X_C][T_C] = 0.0f;

    mymatrix[Y_C][X_C] = sinz;
    mymatrix[Y_C][Y_C] = cosz;
    mymatrix[Y_C][Z_C] = 0.0f;
    mymatrix[Y_C][T_C] = 0.0f;

    mymatrix[Z_C][X_C] = 0.0f;
    mymatrix[Z_C][Y_C] = 0.0f;
    mymatrix[Z_C][Z_C] = 1.0f;
    mymatrix[Z_C][T_C] = 0.0f;

    mymatrix[T_C][X_C] = 0.0f;
    mymatrix[T_C][Y_C] = 0.0f;
    mymatrix[T_C][Z_C] = 0.0f;
    mymatrix[T_C][T_C] = 1.0f;

    transpose();
}

/**********************************************************************************************/
m3d_matrix_pitch::m3d_matrix_pitch(float angle)
{
    float cosx = cos(angle * INV_RAD);
    float sinx = sin(angle * INV_RAD);

    mymatrix[X_C][X_C] = 1.0f;
    mymatrix[X_C][Y_C] = 0.0f;
    mymatrix[X_C][Z_C] = 0.0f;
    mymatrix[X_C][T_C] = 0.0f;

    mymatrix[Y_C][X_C] = 0.0f;
    mymatrix[Y_C][Y_C] = cosx;
    mymatrix[Y_C][Z_C] = -sinx;
    mymatrix[Y_C][T_C] = 0.0f;

    mymatrix[Z_C][X_C] = 0.0f;
    mymatrix[Z_C][Y_C] = sinx;
    mymatrix[Z_C][Z_C] = cosx;
    mymatrix[Z_C][T_C] = 0.0f;

    mymatrix[T_C][X_C] = 0.0f;
    mymatrix[T_C][Y_C] = 0.0f;
    mymatrix[T_C][Z_C] = 0.0f;
    mymatrix[T_C][T_C] = 1.0f;

    transpose();
}

/**********************************************************************************************/
m3d_matrix_yaw::m3d_matrix_yaw(float angle)
{
    float cosy = cos(angle * INV_RAD);
    float siny = sin(angle * INV_RAD);

    mymatrix[X_C][X_C] = cosy;
    mymatrix[X_C][Y_C] = 0.0f;
    mymatrix[X_C][Z_C] = siny;
    mymatrix[X_C][T_C] = 0.0f;

    mymatrix[Y_C][X_C] = 0.0f;
    mymatrix[Y_C][Y_C] = 1.0f;
    mymatrix[Y_C][Z_C] = 0.0f;
    mymatrix[Y_C][T_C] = 0.0f;

    mymatrix[Z_C][X_C] = -siny;
    mymatrix[Z_C][Y_C] = 0.0f;
    mymatrix[Z_C][Z_C] = cosy;
    mymatrix[Z_C][T_C] = 0.0f;

    mymatrix[T_C][X_C] = 0.0f;
    mymatrix[T_C][Y_C] = 0.0f;
    mymatrix[T_C][Z_C] = 0.0f;
    mymatrix[T_C][T_C] = 1.0f;

    transpose();
}

/**********************************************************************************************/
m3d_matrix_rotation::m3d_matrix_rotation(float pitch, float yaw, float roll) : m3d_matrix()
{
    m3d_matrix_roll rollm(roll);
    m3d_matrix_pitch pitchm(pitch);
    m3d_matrix_yaw yawm(yaw);

    multiply(yawm);
    multiply(pitchm);
    multiply(rollm);
}

/**********************************************************************************************/
m3d_matrix_identity::m3d_matrix_identity() : m3d_matrix()
{
}

/**********************************************************************************************/
m3d_matrix_camera::m3d_matrix_camera(const m3d_input_point &campos, const m3d_input_point &lookat)
{
    m3d_vector pos(campos);
    m3d_vector tempz(lookat);

    /*
     * Compute tempz as the vector running from the camera position
     * towards the looking point <at>.
     * The vector's direction is from <position> to <at>.
     */
    tempz.subtract(pos);
    tempz.normalize();

    /*
     * Now compute the cross product between tempz and
     * the Y axis vector; since the final rotation is equivalent to
     * a rotation around Y and another around X, the Y axis in the world
     * coordinates belongs to the YZ plane in the camera coordinates.
     * The result of the cross product is tempx.
     */
    m3d_axis_y tempy_axis;
    m3d_vector tempx(tempz);
    tempx.cross_product(tempy_axis);
    tempx.normalize();

    /*
     * Finally compute tempy.
     */
    m3d_vector tempy(tempx);
    tempy.cross_product(tempz);
    tempy.normalize();

    /*
     * tempz is the resulting new z axis but directed towards the looking point.
     * We need the same axis reference (right-handed) used for other matrixes so
     * tempz is mirrored (camera z axis points from monitor to the viewer)
     * */
    tempz.mirror();

    mymatrix[X_C][X_C] = tempx.myvector[X_C];
    mymatrix[X_C][Y_C] = tempx.myvector[Y_C];
    mymatrix[X_C][Z_C] = tempx.myvector[Z_C];
    mymatrix[X_C][T_C] = tempx.myvector[T_C];

    mymatrix[Y_C][X_C] = tempy.myvector[X_C];
    mymatrix[Y_C][Y_C] = tempy.myvector[Y_C];
    mymatrix[Y_C][Z_C] = tempy.myvector[Z_C];
    mymatrix[Y_C][T_C] = tempy.myvector[T_C];

    mymatrix[Z_C][X_C] = tempz.myvector[X_C];
    mymatrix[Z_C][Y_C] = tempz.myvector[Y_C];
    mymatrix[Z_C][Z_C] = tempz.myvector[Z_C];
    mymatrix[Z_C][T_C] = tempz.myvector[T_C];

    mymatrix[T_C][X_C] = 0.0f;
    mymatrix[T_C][Y_C] = 0.0f;
    mymatrix[T_C][Z_C] = 0.0f;
    mymatrix[T_C][T_C] = 1.0f;

    transpose();
    rotate(pos, pos);
    pos.mirror();

    mymatrix[T_C][X_C] = pos.myvector[X_C];
    mymatrix[T_C][Y_C] = pos.myvector[Y_C];
    mymatrix[T_C][Z_C] = pos.myvector[Z_C];
    mymatrix[T_C][T_C] = pos.myvector[T_C];
}

/**********************************************************************************************/
m3d_frustum::m3d_frustum(const float angle, const int xres, const int yres)
{
    /* 
     * It is assumed that the far end is infinite.
     * e is the distance from camera to the projection plane, considering
     * a view angle of <angle> degrees.
     * left, right, top and bottom intersections with the projection planes are computed
     * to match the screen or window boundaries, provided by xres and yres.
     * Aspect ration <a> is computed using the horizontal and vertical resolution - this should be configurable.
     * 
     * Intersections
     * Left     x = -n/e
     * Right    x = n/e
     * Top      y = a*n/e
     * Bottom   y = -a*n/e
     * 
     * The transformation from frustum to perspective-corrected projection maps the frustum to a cube
     * with edges on X[-xres, +xres] Y[-yres, +yres] Z[e, 5*e].
     * Depth of view f is considered infinite but it can be set to a finite value too.
     */
    float e = 1.0f / tan(angle * INV_RAD / 2.0f);
    float n = e;
    float r = 1;
    float l = -1;
    float t = (float)yres / (float)xres;
    float b = -(float)yres / (float)xres;
    float f = -1000.0f;

#if 1
    mymatrix[X_C][X_C] = 2.0f * n / (r - l);
    mymatrix[X_C][Y_C] = 0.0f;
    mymatrix[X_C][Z_C] = (r + l) / (r - l);
    mymatrix[X_C][T_C] = 0.0f;

    mymatrix[Y_C][X_C] = 0.0f;
    mymatrix[Y_C][Y_C] = 2.0f * n / (t - b);
    mymatrix[Y_C][Z_C] = (t + b) / (t - b);
    mymatrix[Y_C][T_C] = 0.0f;

    mymatrix[Z_C][X_C] = 0.0f;
    mymatrix[Z_C][Y_C] = 0.0f;
    mymatrix[Z_C][Z_C] = -(f + n) / (f - n);
    mymatrix[Z_C][T_C] = -2.0f * n * f / (f - n);

    mymatrix[T_C][X_C] = 0.0f;
    mymatrix[T_C][Y_C] = 0.0f;
    mymatrix[T_C][Z_C] = -1.0f;
    mymatrix[T_C][T_C] = 0.0f;
#else
    mymatrix[X_C][X_C] = 2.0f * n / (r - l);
    mymatrix[X_C][Y_C] = 0.0f;
    mymatrix[X_C][Z_C] = (r + l) / (r - l);
    mymatrix[X_C][T_C] = 0.0f;

    mymatrix[Y_C][X_C] = 0.0f;
    mymatrix[Y_C][Y_C] = 2.0f * n / (t - b);
    mymatrix[Y_C][Z_C] = (t + b) / (t - b);
    mymatrix[Y_C][T_C] = 0.0f;

    mymatrix[Z_C][X_C] = 0.0f;
    mymatrix[Z_C][Y_C] = 0.0f;
    mymatrix[Z_C][Z_C] = -1.0f;
    mymatrix[Z_C][T_C] = -2.0f * n;

    mymatrix[T_C][X_C] = 0.0f;
    mymatrix[T_C][Y_C] = 0.0f;
    mymatrix[T_C][Z_C] = -1.0f;
    mymatrix[T_C][T_C] = 0.0f;
#endif
    transpose();
    print();
}
