#pragma once

#include "Mesh.hpp"

class IrregularMesh : public Mesh
{
public:
	IrregularMesh()
		: Mesh() { }

	IrregularMesh(const Mesh& mesh)
		: Mesh(mesh) { }

	IrregularMesh(const std::string& filepath)
		: Mesh(filepath) { }

protected:
	virtual void initialize() { throw new std::runtime_error("unsupported operation"); }

public:
	virtual void setBorderToBoundary() { throw new std::runtime_error("unsupported operation"); }

};
