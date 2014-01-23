#include "RectangularMesh.hpp"

RectangularMesh::RectangularMesh()
	: Mesh()
{
	initialize();
}

RectangularMesh::RectangularMesh(const Mesh& mesh)
	: Mesh(mesh)
{
	if(points.size() == 0 || elems.size() == 0)
		initialize();
}

RectangularMesh::RectangularMesh(size_t width, size_t height, Point3D domainStart, Point3D domainEnd)
	: Mesh(width, height, domainStart, domainEnd, "rectangular")
{
	initialize();
}

RectangularMesh::RectangularMesh(size_t width, size_t height, Point3D domainStart, Point3D domainEnd, bool borderIsBoundary)
	: Mesh(width, height, domainStart, domainEnd, "rectangular")
{
	initialize();
	if(borderIsBoundary)
		setBorderToBoundary();
}

void RectangularMesh::initialize()
{
	float xDiff = GetDomainWidth();
	float yDiff = GetDomainHeight();

	xDiff /= width - 1;
	yDiff /= height - 1;

	size_t index = 0;
	for(size_t y = 0; y < height; ++y)
	{
		float currY = domainStart.y + yDiff * y;
		for(size_t x = 0; x < width; ++x)
		{
			points.push_back(new MeshPoint(domainStart.x + xDiff * x, currY, 0.0, index, false));
			++index;
			if(x >= 1 && y >=1)
			{
				size_t ptsCount = GetPointsCount();
				MeshElement* elem = new MeshElement();
				elems.push_back(elem);
				elem->points.push_back(points[ptsCount-2-width]);
				elem->points.push_back(points[ptsCount-1-width]);
				elem->points.push_back(points[ptsCount-1]);
				elem->points.push_back(points[ptsCount-2]);
			}
		}
	}

	recalculate();
}

void RectangularMesh::setBorderToBoundary()
{
	size_t i = 0;
	for(size_t y = 0; y < height; ++y)
		for(size_t x = 0; x < width; ++x)
		{
			if(x == 0 || x == width-1 || y == 0 || y == height-1)
			{
				points[i]->boundary = true;
			}
			++i;
		}
}
