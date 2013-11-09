#pragma once

#include "common.view.h"
#include "MeshPoint.hpp"

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
