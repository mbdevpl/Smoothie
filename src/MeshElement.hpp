#pragma once

#include "common.view.h"
#include "MeshPoint.hpp"

/*!
 * \brief Mesh element keeps references to its vertices.
 */
class MeshElement
{
public:
	std::vector<MeshPoint*> points;
	Point3D centre;

public:
	MeshElement();
	MeshElement(size_t capacity);
	void recalculate();
};
