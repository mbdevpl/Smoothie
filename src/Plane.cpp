#include "Plane.hpp"

Plane::Plane()
	: xAngle(0.0), yAngle(0.0), zAngle(0.0), unit(AngleUnit::Radian) { }

Plane::Plane(float xAngle, float yAngle, float zAngle)
	: xAngle(xAngle), yAngle(yAngle), zAngle(zAngle), unit(AngleUnit::Radian) { }

Plane::Plane(float xAngle, float yAngle, float zAngle, AngleUnit::AngleUnit unit)
	: xAngle(xAngle), yAngle(yAngle), zAngle(zAngle), unit(unit) { }

void Plane::Change(float xAngleChange, float yAngleChange, float zAngleChange)
{
	xAngle += xAngleChange;
	yAngle += yAngleChange;
	zAngle += zAngleChange;
}
