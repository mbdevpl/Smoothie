#pragma once

#include "FluidFlowCalculator.hpp"

/*!
 * \brief The thread object that keeps reference to FluidFlowCalculator,
 *  and allows to run the calculator from Qt GUI.
 */
class CalculatorThread : public QThread
{
	Q_OBJECT
private:
	FluidFlowCalculator& calculator;
public:
	CalculatorThread(FluidFlowCalculator& calculator);
private slots:
	void calculate();
};
