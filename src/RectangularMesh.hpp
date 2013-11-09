#pragma once

#include "Mesh.hpp"

class RectangularMesh : public Mesh
{
public:
	RectangularMesh();
	RectangularMesh(size_t width, size_t height, Point3D domainStart, Point3D domainEnd);
	RectangularMesh(size_t width, size_t height, Point3D domainStart, Point3D domainEnd, bool borderIsBoundary);

protected:
	virtual void initialize();

public:
	virtual void setBorderToBoundary();

};
