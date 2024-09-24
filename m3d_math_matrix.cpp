#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "m3d_math_data.hh"
#include "m3d_math_axis.hh"
#include "m3d_math_matrix.hh"

using namespace std;

#define INV_RAD ((float)M_PI / 180.0f)

/*
 * Utility function to multiply a row and a column in a matrix.
 * NOTE: Remember the order used in this code, rows carry homogeneus
 * coordinates (all x, all y, all z, all t) while columns carry
 * the vectors composing the matrix.
 */
static inline float row_by_col(const float a[][m3d_vector_size],
			       const float b[][m3d_vector_size],
			       unsigned i,
			       unsigned j)
{
	float ret = 0.0f;
	unsigned k;

	for (k = 0; k < m3d_vector_size; k++)
	{
		ret += a[i][k] * b[k][j];
	}

	return ret;
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

void m3d_matrix::operator=(const m3d_matrix &other)
{
	memcpy(mymatrix, (void *)other.mymatrix, sizeof(mymatrix));
}

void m3d_matrix::operator=(const float other[][m3d_vector_size])
{
	memcpy(mymatrix, other, sizeof(mymatrix));
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
 * performs
 *
 * 1) rotation and motion of a point
 * 2) rotation of a vector
 *
 * the matrix need to be column first (columns carry the vectors)
 *
 *   Xr1 Xr2 Xr3 0
 *   Yr1 Yr2 Yr3 0
 *   Zr1 Zr2 Zr3 0
 *   Xt   Yt  Zt 1
 *
 * A vector has the T value set to 0.0f, so the last row of values does not
 * affect the transformation, i.e. a vector cannot be translated but can only be rotated.
 * A point has the T value set to 1.0f, so the last row of values is effectively
 * added to the rotation, while the output of the T value of the point is 1.0f.
 *
 * Examples
 *
 * Point [Xp, Yp, Zp, 1]
 *
 * Output Point Xp' = Xp*Xr1 + Yp*Yr1 + Zp*Zr1 + 1.0*Xt -> Xt is added as-is
 * Output Point Tp' = Xp*0.0 + Yp*0.0 + Zp*0.0 + 1.0*1.0 -> Still 1.0
 *
 * Vector [Xv, Yv, Zv, 0]
 *
 * Output Vector Xv' = Xv*Xr1 + Yv*Yr1 + Zv*Zr1 + 0.0*Xt -> Xt is ignored
 * Output Vector Tv' = Xv*0.0 + Yv*0.0 + Zv*0.0 + 0.0*1.0 -> Still 0.0
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
#ifdef DEBUG
	m3d_matrix::print(mymatrix);
#endif
}

void m3d_matrix::print(const float matrix[][m3d_vector_size])
{
#ifdef DEBUG
	ostringstream temp;

	temp.setf(ios_base::showpoint | ios_base::showpos);
	temp.precision(6);

	temp << "+-" << setw(51) << "-+" << endl;
	temp << "|X " << setw(10) << matrix[X_C][X_C] << " X " << setw(10) << matrix[X_C][Y_C] << " X " << setw(10) << matrix[X_C][Z_C] << " X " << setw(10) << matrix[X_C][T_C] << "|" << endl;
	temp << "|Y " << setw(10) << matrix[Y_C][X_C] << " Y " << setw(10) << matrix[Y_C][Y_C] << " Y " << setw(10) << matrix[Y_C][Z_C] << " Y " << setw(10) << matrix[Y_C][T_C] << "|" << endl;
	temp << "|Z " << setw(10) << matrix[Z_C][X_C] << " Z " << setw(10) << matrix[X_C][Y_C] << " Z " << setw(10) << matrix[Z_C][Z_C] << " Z " << setw(10) << matrix[Z_C][T_C] << "|" << endl;
	temp << "|T " << setw(10) << matrix[T_C][X_C] << " T " << setw(10) << matrix[T_C][Y_C] << " T " << setw(10) << matrix[T_C][Z_C] << " T " << setw(10) << matrix[T_C][T_C] << "|" << endl;

	temp << "+-" << setw(51) << "-+" << endl;
	cout << temp.str();
#else
	(void)matrix;
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
m3d_frustum::m3d_frustum(const float fowangle, const int xres, const int yres, const float near, const float far)
{
	/*
	 * Build a transform matrix for perspective projections.
	 * The reference is the glFrustum() function.
	 * The reference coordinates system is assumed to be right-handed with X axis left to right,
	 * Y axis bottom to top, Z axis front to back (towards the viewer, or exiting the screen).
	 *
	 *         ^ Y
	 *         |
	 *         |
	 *         |
	 *         |
	 *         O---------->
	 *        /           X
	 *       /
	 *      /
	 *     Z
	 *
	 * The frustum is a pyramid delimited by near and far view planes.
	 * The near plane is the projection plane, the plane where 3D points becomes 2D visible points
	 * by projection.
	 * The far plane is the visibility limit, the maximum depth at which a point is visible.
	 * The frustum is then the camera coordinate space, the space of visible points.
	 * The distance from the viewpoint to the near field is computed as
	 *
	 *  e = 1/tan(fowangle/2)
	 *
	 * where the field of view angle (fowangle) is the width of the view angle form left to right;
	 * bigger angles give closer near planes, hence wide views implies zooming out (objects get closer
	 * to the viewpoint as the distance of the near plane becomes smaller).
	 * The near plane intersections have coordinates of x[-1, 1] and y[-a , a], where a is the aspect ratio of the screen.
	 *
	 * In order to have the near plane at a specific distance n from the point of view we need to scale by a factor of n/e
	 * x and y intersections; so we get x[-n/e, n/e] and y [-a*n/e, a*n/e].
	 *
	 * The projection matrix transforms the contents of the frustum (camera-space) into homogeneus coordinates of
	 * a cube which extends in the range [-1, +1] for x, y and z values.
	 * The cube (clipping) is left-handed, i.e. the Z coordinates are negative from screen to viewpoint.
	 * The clipped/projected point
	 */
	float e = 1.0f / tan(fowangle * INV_RAD / 2.0f);
	float a = (float)yres / (float)xres;

	mymatrix[X_C][X_C] = (2.0f * near) / (2.0f * (near / e));
	mymatrix[X_C][Y_C] = 0.0f;
	mymatrix[X_C][Z_C] = 0.0f;
	mymatrix[X_C][T_C] = 0.0f;

	mymatrix[Y_C][X_C] = 0.0f;
	mymatrix[Y_C][Y_C] = (2.0f * near) / (2.0f * a * (near / e));
	mymatrix[Y_C][Z_C] = 0.0f;
	mymatrix[Y_C][T_C] = 0.0f;

	mymatrix[Z_C][X_C] = 0.0f;
	mymatrix[Z_C][Y_C] = 0.0f;
	mymatrix[Z_C][Z_C] = -(far + near) / (far - near);
	mymatrix[Z_C][T_C] = -2.0f * near * far / (far - near);

	mymatrix[T_C][X_C] = 0.0f;
	mymatrix[T_C][Y_C] = 0.0f;
	mymatrix[T_C][Z_C] = -1.0f;
	mymatrix[T_C][T_C] = 0.0f;

	transpose();
}

m3d_frustum::m3d_frustum(const float fowangle, const int xres, const int yres, const float near)
{
	/*
	 * Build a transform matrix for perspective projections.
	 * The reference is the glFrustum() function.
	 * The reference coordinates system is assumed to be right-handed with X axis left to right,
	 * Y axis bottom to top, Z axis front to back (towards the viewer, or exiting the screen).
	 *
	 *         ^ Y
	 *         |
	 *         |
	 *         |
	 *         |
	 *         O---------->
	 *        /           X
	 *       /
	 *      /
	 *     Z
	 *
	 * The frustum is a pyramid delimited by near and far view planes.
	 * The near plane is the projection plane, the plane where 3D points becomes 2D visible points
	 * by projection.
	 * The far plane is the visibility limit, the maximum depth at which a point is visible.
	 * The frustum is then the camera coordinate space, the space of visible points.
	 * The distance from the viewpoint to the near field is computed as
	 *
	 *  e = 1/tan(fowangle/2)
	 *
	 * where the field of view angle (fowangle) is the width of the view angle form left to right;
	 * bigger angles give closer near planes, hence wide views implies zooming out (objects get closer
	 * to the viewpoint as the distance of the near plane becomes smaller).
	 * The near plane intersections have coordinates of x[-1, 1] and y[-a , a], where a is the aspect ratio of the screen.
	 *
	 * In order to have the near plane at a specific distance n from the point of view we need to scale by a factor of n/e
	 * x and y intersections; so we get x[-n/e, n/e] and y [-a*n/e, a*n/e].
	 *
	 * The projection matrix transforms the contents of the frustum (camera-space) into homogeneus coordinates of
	 * a cube which extends in the range [-1, +1] for x, y and z values.
	 * The cube (clipping) is left-handed, i.e. the Z coordinates are negative from screen to viewpoint.
	 * The clipped/projected point
	 */
	float e = 1.0f / tan(fowangle * INV_RAD / 2.0f);
	float a = (float)yres / (float)xres;

	mymatrix[X_C][X_C] = (2.0f * near) / (2.0f * (near / e));
	mymatrix[X_C][Y_C] = 0.0f;
	mymatrix[X_C][Z_C] = 0.0f;
	mymatrix[X_C][T_C] = 0.0f;

	mymatrix[Y_C][X_C] = 0.0f;
	mymatrix[Y_C][Y_C] = (2.0f * near) / (2.0f * a * (near / e));
	mymatrix[Y_C][Z_C] = 0.0f;
	mymatrix[Y_C][T_C] = 0.0f;

	mymatrix[Z_C][X_C] = 0.0f;
	mymatrix[Z_C][Y_C] = 0.0f;
	mymatrix[Z_C][Z_C] = -1.0f;
	mymatrix[Z_C][T_C] = -2.0f * near;

	mymatrix[T_C][X_C] = 0.0f;
	mymatrix[T_C][Y_C] = 0.0f;
	mymatrix[T_C][Z_C] = -1.0f;
	mymatrix[T_C][T_C] = 0.0f;

	transpose();
}