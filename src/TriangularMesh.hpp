#pragma once

#include "Mesh.hpp"

/*!
 * \brief Specification of Mesh, for which all elements are triangular (have 3 vertices).
 */
class TriangularMesh : public Mesh
{
public:
	TriangularMesh();
	TriangularMesh(const Mesh& mesh);
	TriangularMesh(size_t width, size_t height, Point3D domainStart, Point3D domainEnd);
	TriangularMesh(size_t width, size_t height, Point3D domainStart, Point3D domainEnd, bool borderIsBoundary);

protected:
	virtual void initialize();

public:
	virtual void setBorderToBoundary();

};
