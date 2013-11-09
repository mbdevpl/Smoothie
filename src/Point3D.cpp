#include "Point3D.hpp"

Point3D::Point3D()
	: Point2D(), z(0) { }

Point3D::Point3D(float x, float y, float z)
	: Point2D(x, y), z(z) { }

void Point3D::reset()
{
	Point2D::reset();
	z = 0;
}
