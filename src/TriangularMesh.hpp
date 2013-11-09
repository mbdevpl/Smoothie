#pragma once

#include "Mesh.hpp"

class TriangularMesh : public Mesh
{
public:
	TriangularMesh();
	TriangularMesh(size_t width, size_t height, Point3D domainStart, Point3D domainEnd);
	TriangularMesh(size_t width, size_t height, Point3D domainStart, Point3D domainEnd, bool borderIsBoundary);

protected:
	virtual void initialize();

public:
	virtual void setBorderToBoundary();

};
