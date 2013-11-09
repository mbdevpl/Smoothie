#include "CalculatorThread.hpp"

CalculatorThread::CalculatorThread(FluidFlowCalculator& calculator)
	: QThread(), calculator(calculator)
{
	QObject::connect(this, SIGNAL(started()), this, SLOT(calculate()));
	//setPriority();
}
void CalculatorThread::calculate()
{
	calculator.calculate();
}
