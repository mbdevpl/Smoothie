#pragma once

#include "TriangularMesh.hpp"
#include "RectangularMesh.hpp"

class MeshExamples
{
private:
	static bool generated;
	static std::vector<Mesh*> meshes;

public:
	static Mesh& get(size_t index);

private:
	static void generate(size_t index);

private:
	MeshExamples();
};
