#pragma once

#include "FluidFlowCalculator.hpp"
#include "RectangularMesh.hpp"

class RectangularFluidFlowCalculator : public FluidFlowCalculator
{
public:
	RectangularFluidFlowCalculator(RectangularMesh& mesh);
	virtual void calculate();

protected:
	/*!
	 * \brief 6-point square integral
	 * \param pointIndexX
	 * \param pointIndexY
	 * \return result of the integration
	 */
	virtual double localIntegral(size_t pointIndexX, size_t pointIndexY);
	virtual double localFunction(size_t pointIndexX, size_t pointIndexY, double x, double y);
	virtual double localPartialDerivative(size_t pointIndex, char pointCoordinate, double x, double y);

};
