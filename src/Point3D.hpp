#pragma once

#include "Point2D.hpp"

struct Point3D : public Point2D
{
public:
	float z;

public:
	Point3D();
	Point3D(float x, float y, float z);
	virtual void reset();

};
