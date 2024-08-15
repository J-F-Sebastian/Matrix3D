#ifndef M3D_MATH_HH_INCLUDED
#define M3D_MATH_HH_INCLUDED

/*
 * Copyright 2014 Diego Gallizioli
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

/*
 * A translation-invariant vector.
 */
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
		// myvector[T_C] = 0.0f;
	}

	void operator=(const float other[])
	{
		myvector[X_C] = other[X_C];
		myvector[Y_C] = other[Y_C];
		myvector[Z_C] = other[Z_C];
		// myvector[T_C] = other[T_C];
		myvector[T_C] = 0.0f;
	}

	friend m3d_vector operator+(const m3d_vector &veca, const m3d_vector &vecb)
	{
		m3d_vector ret(veca);
		ret.add(vecb);
		return ret; // return the result by value (uses move constructor)
	}

	/*
	 * perform cross product myvector X veca and store the result in myvector
	 */
	void cross_product(const m3d_vector &veca);

	/*
	 * perform dot product myvector * veca and return the scalar result
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
};

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

	void operator=(const m3d_matrix &other)
	{
		unsigned i, j;

		for (i = 0; i < m3d_vector_size; i++)
		{
			for (j = 0; j < m3d_vector_size; j++)
			{
				mymatrix[i][j] = other.mymatrix[i][j];
			}
		}
	}

	void operator=(const float other[][m3d_vector_size])
	{
		unsigned i, j;

		for (i = 0; i < m3d_vector_size; i++)
		{
			for (j = 0; j < m3d_vector_size; j++)
			{
				mymatrix[i][j] = other[i][j];
			}
		}
	}

	/*
	 * Insert a vector at the specified row; existing values are overwritten
	 */
	void insert(const m3d_vector &vector, unsigned row);

	/*
	 * perform multiplication of 2 matrices, rows by columns. Store the result in mymatrix
	 */
	void multiply(m3d_matrix &mat);

	/*
	 * perform multiplication of mymatrix and a vector. Store the result in vector
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

private:
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

class m3d_frustum : public m3d_matrix
{
public:
	/*
	 * angle is the viewpoint width in degrees.
	 */
	m3d_frustum(const float angle, const int xres, const int yres);
};

#endif // M3D_MATH_HH_INCLUDED
