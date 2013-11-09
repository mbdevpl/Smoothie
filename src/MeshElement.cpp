#include "MeshElement.hpp"

MeshElement::MeshElement()
	: points(), centre() { }

MeshElement::MeshElement(size_t capacity)
	: points(capacity), centre()  { }

void MeshElement::recalculate()
{
	centre.reset();
	for(auto it = points.begin(); it != points.end(); ++it)
	{
		MeshPoint& pt = *(*it);
		centre.x += pt.x / points.size();
		centre.y += pt.y / points.size();
		centre.z += pt.z / points.size();
	}
}
