#include "FluidFlowCalculator.hpp"

FluidFlowCalculator::FluidFlowCalculator(size_t elementSize, Mesh& mesh)
	: QObject(), elementSize(elementSize), mesh(mesh), currentElementIndex(0),
	stiffnessMatrix(mesh.GetPointsCount(), mesh.GetPointsCount()),
	massMatrix(mesh.GetPointsCount(), 1),
	resultMatrix(mesh.GetPointsCount(), 1) { }

FluidFlowCalculator::~FluidFlowCalculator()
{
	currentElementIndex = 0;
}

void FluidFlowCalculator::calculate()
{
	emit started();

	const size_t pointsCount = mesh.GetPointsCount();
	//const size_t elementsCount = mesh.GetElementsCount();

	stiffnessMatrix = boost::numeric::ublas::matrix<double>(pointsCount, pointsCount);
	for(size_t x = 0; x < pointsCount; ++x)
		for(size_t y = 0; y < pointsCount; ++y)
			stiffnessMatrix(x, y) = 0.0;

	massMatrix = boost::numeric::ublas::matrix<double>(pointsCount, 1);
	for(size_t x = 0; x < pointsCount; ++x)
		massMatrix(x, 0) = 0.0;
}
