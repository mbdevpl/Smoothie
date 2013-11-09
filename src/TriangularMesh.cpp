#include "TriangularMesh.hpp"

TriangularMesh::TriangularMesh()
{
	initialize();
}

TriangularMesh::TriangularMesh(size_t width, size_t height, Point3D domainStart, Point3D domainEnd)
	: Mesh(width, height, domainStart, domainEnd)
{
	initialize();
}

TriangularMesh::TriangularMesh(size_t width, size_t height, Point3D domainStart, Point3D domainEnd, bool borderIsBoundary)
	: Mesh(width, height, domainStart, domainEnd)
{
	initialize();
	if(borderIsBoundary)
		setBorderToBoundary();
}

void TriangularMesh::initialize()
{
	float xDiff = domainEnd.x - domainStart.x;
	float yDiff = domainEnd.y - domainStart.y;

	xDiff /= width - 1;
	yDiff /= height - 1;
	float xDiffHalf = xDiff / 2;

	size_t index = 0;
	bool toggle = false;
	for(size_t y = 0; y < height; ++y)
	{
		float currY = domainStart.y + yDiff * y;
		toggle = !toggle;
		size_t currWidth = width - (toggle ? 1 : 0);
		for(size_t x = 0; x < currWidth; ++x)
		{
			points.push_back(new MeshPoint(domainStart.x + xDiff * x + (toggle ? xDiffHalf : 0), currY, 0.0, index, false));
			++index;
			if((toggle || x >= 1) && y >=1)
			{
				size_t ptsCount = GetPointsCount();

				MeshElement* elem1 = new MeshElement(3);
				elems.push_back(elem1);

				if(toggle && (x == 0))
				{
					// first triangle in the 'shorter' row
					// only one triangle
					elem1->points[0] = points[ptsCount-1-width];
					elem1->points[1] = points[ptsCount-width];
					elem1->points[2] = points[ptsCount-1];
				}
				else if(!toggle && (x == width - 1))
				{
					// last triangle in the 'longer' row
					// only one triangle
					elem1->points[0] = points[ptsCount-1-width];
					elem1->points[1] = points[ptsCount-1];
					elem1->points[2] = points[ptsCount-2];
				}
				else
				{
					// all other cases
					// two triangles
					elem1->points[0] = points[ptsCount-1-width];
					elem1->points[1] = points[ptsCount-1];
					elem1->points[2] = points[ptsCount-2];

					MeshElement* elem2 = new MeshElement();
					elems.push_back(elem2);
					elem2->points.push_back(points[ptsCount-1-width]);
					elem2->points.push_back(points[ptsCount-width]);
					elem2->points.push_back(points[ptsCount-1]);
				}
			}
		}
	}

	recalculate();
}

void TriangularMesh::setBorderToBoundary()
{
	bool toggle = false;
	size_t i = 0;
	for(size_t y = 0; y < height; ++y)
	{
		toggle = !toggle;
		for(size_t x = 0; x < width - (toggle ? 1 : 0); ++x)
		{
			if(x == 0 || x == width-1 || (toggle && x == width - 2) || y == 0 || y == height-1)
			{
				points[i]->boundary = true;
			}
			++i;
		}
	}
}
