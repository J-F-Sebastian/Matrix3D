#ifndef M3D_VERTEX_H
#define M3D_VERTEX_H

#include "m3d_math.hh"

class m3d_vertex
{
public:
	/** Default constructor */
	m3d_vertex() : position(), normal(){};
	/** Default destructor */
	~m3d_vertex(){};
	/** Non-default constructor */
	m3d_vertex(const float coords[]) : position(coords), normal(){};
	/** Copy constructor
	 *  \param other Object to copy from
	 */
	m3d_vertex(const m3d_vertex &other);
	/** Access position
	 * \return The current value of position
	 */
	void print(void);

	m3d_point position; //!< Member variable "position"
	m3d_vector normal;  //!< Member variable "normal"
};

#endif // M3D_VERTEX_H
