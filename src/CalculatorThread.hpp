#pragma once

#include "FluidFlowCalculator.hpp"

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
