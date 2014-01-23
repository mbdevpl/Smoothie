#pragma once

#include "Mesh.hpp"

/*!
 * \brief Specification of Mesh, for which all elements are rectangular (have 4 vertices).
 */
class RectangularMesh : public Mesh
{
public:
	RectangularMesh();
	RectangularMesh(const Mesh& mesh);
	RectangularMesh(size_t width, size_t height, Point3D domainStart, Point3D domainEnd);
	RectangularMesh(size_t width, size_t height, Point3D domainStart, Point3D domainEnd, bool borderIsBoundary);

protected:
	virtual void initialize();

public:
	virtual void setBorderToBoundary();

};
