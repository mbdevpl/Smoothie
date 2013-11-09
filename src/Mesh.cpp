#include "Mesh.hpp"

Mesh::Mesh()
//	: width(20), height(20), set(20), val(20), name("test")
{
}

Mesh::Mesh(size_t width, size_t height, Point3D domainStart, Point3D domainEnd)
	: width(width), height(height), domainStart(domainStart), domainEnd(domainEnd),
	points(0), elems(0) //set(width), val(height), name("test")
{
}

Mesh::~Mesh()
{
	//set.clear();
	//val.clear();
	width = 0;
	height = 0;
	// TODO: free vectors
	points.clear();
	elems.clear();
}

void Mesh::recalculate()
{
	for(auto it = elems.begin(); it != elems.end(); ++it)
	{
		MeshElement& elem = *(*it);
		elem.recalculate();
	}
}
