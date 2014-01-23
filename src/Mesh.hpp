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

private:
	std::string name;
	std::string type;

public:
	Mesh();
	Mesh(const Mesh& mesh);
	Mesh(const size_t width, const size_t height, Point3D domainStart, Point3D domainEnd,
			const std::string& type);
	Mesh(const std::string& filepath);
	virtual ~Mesh();

protected:
	virtual void initialize() = 0;

private:
	void initialize_abstract() { initialize(); }

public:
	virtual void setBorderToBoundary() = 0;
	virtual void recalculate();

private:
	std::string detectType() const;

public:
	/*!
	 * \brief What type of file should be created is deduced from the file extension.
	 * \param filename full path to the target file
	 */
	void saveToFile(const std::string& filename) const;

private:
	void fromXml(const xmlDocPtr& doc);

	/*!
	 * \brief Converts this mesh to XML document type used by libxml2
	 * \return resulting xml document
	 */
	xmlDocPtr toXml() const;

	void fromCsv(const std::vector<std::vector<std::string> >& csv);
	std::vector<std::vector<std::string> > toCsv() const;
	void fromMdrOrMdt(const std::string& mdrOrMdt);
	std::string toMdrOrMdt() const;

public:
	void SetWidth(const size_t width) { this->width = width; }
	void SetHeight(const size_t height) { this->height = height; }
	void SetName(const std::string& name) { this->name = name; }
	//void SetType(const std::string& type) { this->type = type; }
	const std::string& GetType() const { return type; }
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
