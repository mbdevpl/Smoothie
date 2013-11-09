#include "Point2D.hpp"

Point2D::Point2D()
	: x(0), y(0) { }

Point2D::Point2D(float x, float y)
	: x(x), y(y) { }

void Point2D::reset()
{
	x = 0;
	y = 0;
}
