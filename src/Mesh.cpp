#include "Mesh.hpp"

Mesh::Mesh()
	: width(0), height(0), domainStart(), domainEnd(),
	points(), elems(), name(), type("unknown") { }

Mesh::Mesh(const Mesh& mesh)
	: width(mesh.width), height(mesh.height), domainStart(mesh.domainStart), domainEnd(mesh.domainEnd),
	points(), elems(), name(mesh.name), type(mesh.type)
{
	for(auto it = mesh.points.begin(); it != mesh.points.end(); ++it)
		points.push_back(new MeshPoint(**it));
	for(auto it = mesh.elems.begin(); it != mesh.elems.end(); ++it)
	{
		MeshElement* e = new MeshElement();
		for(auto pit = (*it)->points.begin(); pit != (*it)->points.end(); ++pit)
			e->points.push_back(points[(*pit)->index]);
		e->recalculate();
		elems.push_back(e);
	}
}

Mesh::Mesh(const size_t width, const size_t height, Point3D domainStart, Point3D domainEnd,
		const std::string& type)
	: width(width), height(height), domainStart(domainStart), domainEnd(domainEnd),
		points(0), elems(0), name(), type(type) { }

Mesh::Mesh(const std::string& filepath)
	: width(0), height(0), domainStart(), domainEnd(),
	points(), elems()
{
	std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
	if(ext.compare("xml") == 0)
	{
		xmlKeepBlanksDefault(0); // ignore XML indentation
		xmlDocPtr doc = xmlParseFile(filepath.c_str());
		fromXml(doc);
		xmlFreeDoc(doc);
	}
	else if(ext.compare("csv") == 0)
	{
		throw new std::runtime_error("not yet implemented");
	}
	else if(ext.compare("mdt") == 0 || ext.compare("mdr") == 0)
	{
		std::ifstream in(filepath);
		std::string str((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
		fromMdrOrMdt(str);
	}
	else
	{
		throw new std::runtime_error("unsupported file extension");
	}
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

std::string Mesh::detectType() const
{
	std::string meshTypeValue;

	if(elems.size() == 0)
		meshTypeValue = "unknown";
	else
	{
		MeshElement* first = elems.at(0);
		size_t elemSize = first->points.size();
		for(auto it = elems.begin(); it != elems.end(); ++it)
			if((**it).points.size() != elemSize)
			{
				meshTypeValue = "irregular";
				break;
			}
		if(elemSize == 3)
			meshTypeValue = "triangular";
		else if(elemSize == 4)
			meshTypeValue = "rectangular";
		else
			meshTypeValue = "regular";
	}

	return meshTypeValue;
}

void Mesh::saveToFile(const std::string& filepath) const
{
	std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
	if(ext.compare("xml") == 0)
	{
		xmlDocPtr doc = toXml();
		xmlKeepBlanksDefault(0); // turn on XML indentation
		xmlSaveFormatFile(filepath.c_str(), doc, true);
		xmlFreeDoc(doc);
	}
	else if(ext.compare("csv") == 0)
	{
		std::vector<std::vector<std::string> > csv = toCsv();
		std::ofstream out(filepath);
		for(auto it = csv.begin(); it != csv.end(); ++it)
		{
			for(auto eit = it->begin(); eit != it->end(); ++eit)
			{
				std::string& entry = *eit;
				if(eit != it->begin())
					out << ';';
				out << entry;
			}
			out << std::endl;
		}
		out.close();
	}
	else if(ext.compare("mdt") == 0 || ext.compare("mdr") == 0)
	{
		std::string detectedType = detectType();
		if(detectedType.compare("triangular") != 0 && detectedType.compare("rectangular") != 0)
			throw new std::runtime_error("cannot save current type of mesh to this type of file");

		std::string filepath_corrected;
		if(ext.compare("mdt") == 0 && detectedType.compare("rectangular") == 0)
			filepath_corrected = filepath.substr(0, filepath.length() - 1).append("r");
		else if(ext.compare("mdr") == 0 && detectedType.compare("triangular") == 0)
			filepath_corrected = filepath.substr(0, filepath.length() - 1).append("t");
		else
			filepath_corrected = filepath;

		std::string mdtOrMdr = toMdrOrMdt();
		std::ofstream out(filepath_corrected);
		out << mdtOrMdr;
		out.close();
	}
	else
	{
		throw new std::runtime_error("unsupported file extension");
	}
}

void Mesh::fromXml(const xmlDocPtr& doc)
{

	xmlNodePtr root = xmlDocGetRootElement(doc);
	std::string rootName(FROM_XML root->name);
#ifdef DEBUG
	{
		std::stringstream ss;
		ss << rootName << std::endl;
		qDebug() << ss.str().c_str();
	}
#endif
	if(rootName.compare("mesh"))
		throw std::runtime_error("cannot load data, invalid file format");

	for(xmlAttrPtr attr = root->properties; attr != nullptr; attr = attr->next)
	{
		std::string name(FROM_XML attr->name);
		xmlNodePtr attrValue = attr->children;
		std::string value(FROM_XML xmlNodeGetContent(attrValue));
#ifdef DEBUG
		{
			std::stringstream ss;
			ss << name << " -> " << value << std::endl;
			qDebug() << ss.str().c_str();
		}
#endif
		if(!name.compare("width"))
			this->width = (size_t)std::stoi(value);
		else if(!name.compare("height"))
			this->height = (size_t)std::stoi(value);
		else if(!name.compare("domain_start_x"))
			this->domainStart.x = std::stof(value);
		else if(!name.compare("domain_start_y"))
			this->domainStart.y = std::stof(value);
		else if(!name.compare("domain_start_z"))
			this->domainStart.z = std::stof(value);
		else if(!name.compare("domain_end_x"))
			this->domainEnd.x = std::stof(value);
		else if(!name.compare("domain_end_y"))
			this->domainEnd.y = std::stof(value);
		else if(!name.compare("domain_end_z"))
			this->domainEnd.z = std::stof(value);
		else if(!name.compare("name"))
			this->name = value;
		else if(!name.compare("type"))
			this->type = value;
	}
	xmlNodePtr vertices_node = nullptr;
	xmlNodePtr elements_node = nullptr;
	for(xmlNodePtr node = root->children; node != nullptr; node = node->next)
	{
		std::string name(FROM_XML node->name);
		if(!name.compare("vertices"))
		{
			vertices_node = node;
		}
		if(!name.compare("elements"))
		{
			elements_node = node;
		}
	}

	std::map<size_t,MeshPoint*> points_temporary;
	for(xmlNodePtr node = vertices_node->children; node != nullptr; node = node->next)
	{
		std::string name(FROM_XML node->name);
		if(name.compare("vertex"))
			throw std::runtime_error("cannot load data, invalid file format");
		MeshPoint* p = new MeshPoint();
		for(xmlAttrPtr attr = node->properties; attr != nullptr; attr = attr->next)
		{
			std::string name(FROM_XML attr->name);
			xmlNodePtr attrValue = attr->children;
			std::string value(FROM_XML xmlNodeGetContent(attrValue));
			if(!name.compare("id"))
				p->index = (size_t)std::stoi(value);
			else if(!name.compare("x"))
				p->x = std::stof(value);
			else if(!name.compare("y"))
				p->y = std::stof(value);
			else if(!name.compare("z"))
				p->z = std::stof(value);
			else if(!name.compare("boundary"))
				p->boundary = std::stoi(value) == 0 ? false : true;
		}
		points_temporary[p->index] = p;
	}
	for(size_t i = 0; i < points_temporary.size(); ++i)
		points.push_back(points_temporary[i]);

	for(xmlNodePtr node = elements_node->children; node != nullptr; node = node->next)
	{
		std::string name(FROM_XML node->name);
		if(name.compare("element"))
			throw std::runtime_error("cannot load data, invalid file format");
		MeshElement* e = new MeshElement();
		for(xmlAttrPtr attr = node->properties; attr != nullptr; attr = attr->next)
		{
			std::string name(FROM_XML attr->name);
			xmlNodePtr attrValue = attr->children;
			std::string value(FROM_XML xmlNodeGetContent(attrValue));
			if(!name.compare("vertices"))
			{
				std::stringstream temp_value_part;
				for(auto it = value.begin(); it != value.end(); ++it)
				{
					if(*it == ';')
					{
						size_t index = (size_t)std::stoi(temp_value_part.str());
						e->points.push_back(points[index]);
						temp_value_part.str(std::string());
						temp_value_part.clear();
					}
					else
						temp_value_part << *it;
				}
				size_t index = (size_t)std::stoi(temp_value_part.str());
				e->points.push_back(points[index]);
			}
			//else if(!name.compare(""))
			//	e-> = ;
		}
		e->recalculate();
		elems.push_back(e);
	}
}

xmlDocPtr Mesh::toXml() const
{
	xmlDocPtr doc = xmlNewDoc(TO_XML "1.0");
	if(doc == nullptr)
		throw new std::runtime_error("error while creating xml code");

	xmlNodePtr root = xmlNewNode(nullptr, TO_XML "mesh");
	if(root == nullptr)
		throw new std::runtime_error("error while creating xml code");
	xmlDocSetRootElement(doc, root);

	xmlNsPtr xmlns = xmlNewNs(root, TO_XML "http://schemas.mbdev.pl/smoothie/1.0", nullptr);
	if(xmlns == nullptr)
		throw new std::runtime_error("error while creating xml code");

	xmlNodePtr comment = xmlNewDocComment(doc, TO_XML "file created by Smoothie, see http://mbdev.pl/projects/smoothie");
	if(comment == nullptr)
		throw new std::runtime_error("error while creating xml comment");

	xmlAttrPtr mesh_width = xmlNewProp(root, TO_XML "width", TO_XML std::to_string(width).c_str());
	if(mesh_width == nullptr)
		throw new std::runtime_error("error while creating xml code");

	xmlAttrPtr mesh_height = xmlNewProp(root, TO_XML "height", TO_XML std::to_string(height).c_str());
	if(mesh_height == nullptr)
		throw new std::runtime_error("error while creating xml code");

	xmlAttrPtr domain_start_x = xmlNewProp(root, TO_XML "domain_start_x", TO_XML std::to_string(domainStart.x).c_str());
	if(domain_start_x == nullptr)
		throw new std::runtime_error("error while creating xml code");

	xmlAttrPtr domain_start_y = xmlNewProp(root, TO_XML "domain_start_y", TO_XML std::to_string(domainStart.y).c_str());
	if(domain_start_y == nullptr)
		throw new std::runtime_error("error while creating xml code");

	xmlAttrPtr domain_start_z = xmlNewProp(root, TO_XML "domain_start_z", TO_XML std::to_string(domainStart.z).c_str());
	if(domain_start_z == nullptr)
		throw new std::runtime_error("error while creating xml code");

	xmlAttrPtr domain_end_x = xmlNewProp(root, TO_XML "domain_end_x", TO_XML std::to_string(domainEnd.x).c_str());
	if(domain_end_x == nullptr)
		throw new std::runtime_error("error while creating xml code");

	xmlAttrPtr domain_end_y = xmlNewProp(root, TO_XML "domain_end_y", TO_XML std::to_string(domainEnd.y).c_str());
	if(domain_end_y == nullptr)
		throw new std::runtime_error("error while creating xml code");

	xmlAttrPtr domain_end_z = xmlNewProp(root, TO_XML "domain_end_z", TO_XML std::to_string(domainEnd.z).c_str());
	if(domain_end_z == nullptr)
		throw new std::runtime_error("error while creating xml code");

	xmlAttrPtr mesh_name = xmlNewProp(root, TO_XML "name", TO_XML name.c_str());
	if(mesh_name == nullptr)
		throw new std::runtime_error("error while creating xml code");

	xmlAttrPtr mesh_type = xmlNewProp(root, TO_XML "type", TO_XML detectType().c_str());
	if(mesh_type == nullptr)
		throw new std::runtime_error("error while creating xml code");

	xmlNodePtr verticesNode = xmlNewNode(nullptr, TO_XML "vertices");
	xmlAddChild(root, verticesNode);
	for(auto it = points.begin(); it != points.end(); ++it)
	{
		MeshPoint& point = **it;
		xmlNodePtr vertexNode = xmlNewNode(nullptr, TO_XML "vertex");
		xmlAttrPtr id = xmlNewProp(vertexNode, TO_XML "id", TO_XML std::to_string(point.index).c_str());
		if(id == nullptr)
			throw new std::runtime_error("error while creating xml code");
		xmlAttrPtr x = xmlNewProp(vertexNode, TO_XML "x", TO_XML std::to_string(point.x).c_str());
		if(x == nullptr)
			throw new std::runtime_error("error while creating xml code");
		xmlAttrPtr y = xmlNewProp(vertexNode, TO_XML "y", TO_XML std::to_string(point.y).c_str());
		if(y == nullptr)
			throw new std::runtime_error("error while creating xml code");
		xmlAttrPtr z = xmlNewProp(vertexNode, TO_XML "z", TO_XML std::to_string(point.z).c_str());
		if(z == nullptr)
			throw new std::runtime_error("error while creating xml code");
		xmlAttrPtr boundary = xmlNewProp(vertexNode, TO_XML "boundary", TO_XML std::to_string(point.boundary).c_str());
		if(boundary == nullptr)
			throw new std::runtime_error("error while creating xml code");
		xmlAddChild(verticesNode, vertexNode);
	}

	xmlNodePtr elementsNode = xmlNewNode(nullptr, TO_XML "elements");
	xmlAddChild(root, elementsNode);
	for(auto it = elems.begin(); it != elems.end(); ++it)
	{
		MeshElement& elem = **it;
		xmlNodePtr elementNode = xmlNewNode(nullptr, TO_XML "element");
		//xmlAttrPtr size = xmlNewProp(elementNode, TO_XML "size", TO_XML std::to_string(elem.points.size()).c_str());
		//if(size == nullptr)
		//	throw new std::runtime_error("error while creating xml code");
		std::stringstream ss;
		for(auto pit = elem.points.begin(); pit != elem.points.end(); ++pit)
		{
			MeshPoint& point = **pit;
			if(pit != elem.points.begin())
				ss << ';';
			ss << std::to_string(point.index).c_str();
		}
		xmlAttrPtr vertices = xmlNewProp(elementNode, TO_XML "vertices", TO_XML ss.str().c_str());
		if(vertices == nullptr)
			throw new std::runtime_error("error while creating xml code");
		xmlAddChild(elementsNode, elementNode);
	}

	return doc;
}

void Mesh::fromCsv(const std::vector<std::vector<std::string> >& csv)
{
	throw new std::runtime_error("not yet implemented");
}

std::vector<std::vector<std::string> > Mesh::toCsv() const
{
	std::vector<std::vector<std::string> > result;

	throw new std::runtime_error("not yet implemented");
	return result;
}

void Mesh::fromMdrOrMdt(const std::string& mdrOrMdt)
{
	throw new std::runtime_error("not yet implemented");
}

std::string Mesh::toMdrOrMdt() const
{
	std::string result;

	throw new std::runtime_error("not yet implemented");
	return result;
}
