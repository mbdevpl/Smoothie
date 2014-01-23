#include "MeshPoint.hpp"

MeshPoint::MeshPoint()
	: Point3D(), index(0), boundary(false) { }

MeshPoint::MeshPoint(const MeshPoint& point)
	: Point3D(point), index(point.index), boundary(point.boundary) { }

MeshPoint::MeshPoint(float x, float y, float z, size_t index, bool boundary)
	: Point3D(x, y, z), index(index), boundary(boundary) { }
