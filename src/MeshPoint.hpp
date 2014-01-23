#pragma once

#include "Point3D.hpp"

/*!
 * \brief A vertex of a mesh, derives from 3-dimensional point, but contains additional characteristics.
 */
class MeshPoint : public Point3D
{
public:
	/*!
	 * \brief Index of this point in the global mesh list. Needed to speed up operations
	 *retlated to the stiffness matrix.
	 */
	size_t index;
	/*!
	 * \brief True if this point is considered a part of boundary condition.
	 */
	bool boundary;

public:
	MeshPoint();
	MeshPoint(const MeshPoint& point);
	MeshPoint(float x, float y, float z, size_t index, bool boundary);

};
