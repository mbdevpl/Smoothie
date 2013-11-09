#pragma once

#include "FluidFlowCalculator.hpp"
#include "TriangularMesh.hpp"

class TriangukarFluidFlowCalculator : public FluidFlowCalculator
{
public:
	TriangukarFluidFlowCalculator(TriangularMesh& mesh);
	virtual void calculate();

protected:
	virtual double localIntegral(size_t pointIndexX, size_t pointIndexY);
	virtual double localFunction(size_t pointIndexX, size_t pointIndexY, double x, double y);
	virtual double localPartialDerivative(size_t pointIndex, char pointCoordinate, double x, double y);

};
