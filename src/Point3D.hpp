#pragma once

#include "Point2D.hpp"

/*!
 * \brief 3-dimensional point, derived from the 2-dimensional.
 */
struct Point3D : public Point2D
{
public:
	float z;

public:
	Point3D();
	Point3D(float x, float y, float z);
	virtual void reset();

};
