#pragma once

#include "MeshElement.hpp"

/**
 * @brief A two-dimensional mesh consisting of multi-point convex segments.
 */
class Mesh
{
protected:
	size_t width;
	size_t height;
	Point3D domainStart;
	Point3D domainEnd;
	std::vector<MeshPoint*> points;
	std::vector<MeshElement*> elems;

public:
	//std::vector<std::vector<bool> > set;
	//std::vector<std::vector<float> > val;
	std::string name;

public:
	Mesh();
	Mesh(size_t width, size_t height, Point3D domainStart, Point3D domainEnd);
	virtual ~Mesh();

protected:
	virtual void initialize() = 0;

public:
	virtual void setBorderToBoundary() = 0;
	virtual void recalculate();

public:
	void SetWidth(const size_t width) { this->width = width; }
	void SetHeight(const size_t height) { this->height = height; }
	void SetName(const std::string& name) { this->name = name; }
	size_t GetWidth() const { return width; }
	size_t GetHeight() const { return height; }
	Point3D GetDomainStart() const { return domainStart; }
	Point3D GetDomainEnd() const { return domainEnd; }
	const MeshPoint& GetPoint(size_t index) const { return *(points[index]); }
	size_t GetPointsCount() const { return points.size(); }
	const MeshElement& GetElement(size_t index) const { return *(elems[index]); }
	size_t GetElementsCount() const { return elems.size(); }
	const std::string& GetName() const { return name; }

	MeshPoint& pt(size_t index) { return *(points[index]); }
	MeshElement& el(size_t index) { return *(elems[index]); }

	float GetDomainWidth() const { return domainEnd.x - domainStart.x; }
	float GetDomainHeight() const { return domainEnd.y - domainStart.y; }
	float GetDomainDepth() const { return domainEnd.z - domainStart.z; }

};
