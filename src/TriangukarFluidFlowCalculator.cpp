#include "TriangukarFluidFlowCalculator.hpp"

TriangukarFluidFlowCalculator::TriangukarFluidFlowCalculator(TriangularMesh& mesh)
	: FluidFlowCalculator(3, mesh) { }

void TriangukarFluidFlowCalculator::calculate()
{
	FluidFlowCalculator::calculate();

	const size_t pointsCount = mesh.GetPointsCount();
	const size_t elementsCount = mesh.GetElementsCount();

	using namespace boost::numeric::ublas;

	matrix<double> localStiffnessMatrix(elementSize, elementSize);
	for(size_t x = 0; x < elementSize; ++x)
		for(size_t y = 0; y < elementSize; ++y)
			localStiffnessMatrix(x, y) = 0.0;

	for(size_t x = 0; x < elementSize; ++x)
		for(size_t y = 0; y < elementSize; ++y)
		{
			localStiffnessMatrix(x, y) = localIntegral(x, y);
		}

#ifdef DEBUG
	{
		std::stringstream ss;
		ss << "localStiffnessMatrix = " << localStiffnessMatrix << std::endl;
		qDebug() << ss.str().c_str();
	}
#endif

	for(size_t i = 0; i < elementsCount; ++i)
	{
		currentElementIndex = i;
		MeshElement& element = mesh.el(i);
		std::vector<MeshPoint*>& pts = element.points;

		const double determinant =
				(pts[1]->x - pts[2]->x) * (pts[0]->y - pts[2]->y)
				- (pts[1]->y - pts[2]->y) * (pts[0]->x - pts[2]->x);
		const double area = 0.5 * std::abs(determinant);
		const double multiplier = 0.5 * area;

		if(multiplier == 0)
			throw new std::runtime_error("multiplier (the jacobian) should not be zero");

		for(size_t x = 0; x < elementSize; ++x)
		{
			MeshPoint* ptx = element.points.at(x);
			for(size_t y = 0; y < elementSize; ++y)
			{
				MeshPoint* pty = element.points.at(y);
				stiffnessMatrix(ptx->index, pty->index) += localStiffnessMatrix(x, y) * multiplier;
			}
		}

#ifdef DEBUG
		{
			std::stringstream ss;
			ss << "stiffnessMatrix = " << stiffnessMatrix;
			qDebug() << ss.str().c_str();
		}
#endif
	}

	size_t boundaryElementsCount = 0;
	for(size_t i = 0; i < pointsCount; ++i)
		if(mesh.GetPoint(i).boundary)
			++boundaryElementsCount;

	bool conditions_present = false;
	for(size_t i = 0; i < pointsCount; ++i)
	{
		const MeshPoint& pt = mesh.GetPoint(i);

		if(pt.boundary)
		{
			conditions_present = true;
			// overwrite stiffness matrix
			matrix<double> identity(pointsCount, 1);
			identity.assign(zero_matrix<double>(pointsCount, 1));
			identity(i, 0) = 1;
			for(size_t x = 0; x < pointsCount; ++x)
				stiffnessMatrix(i, x) = identity(x, 0);

			// overwrite mass matrix
			massMatrix(i, 0) = pt.z;
		}
	}

	if(conditions_present)
	{
		// multiply inverse of stiffness matrix by mass matrix
		resultMatrix = left_divide(stiffnessMatrix, massMatrix);
	}

#ifdef DEBUG
	{
		std::stringstream ss;
		ss << "final result = " << resultMatrix << std::endl;
		qDebug() << ss.str().c_str();
	}
#endif

	emit finished();
}

double TriangukarFluidFlowCalculator::localIntegral(size_t pointIndexX, size_t pointIndexY)
{
	std::vector<double> xk(3);
	xk[0] = -0.666666666666667;
	xk[1] = -0.666666666666667;
	xk[2] = 0.333333333333333;

	std::vector<double> yk(3);
	yk[0] = -0.666666666666667;
	yk[1] = 0.333333333333333;
	yk[2] = -0.666666666666667;

	std::vector<double> wk(3);
	wk[0] = -0.666666666666667;
	wk[1] = -0.666666666666667;
	wk[2] = -0.666666666666667;

	double integrationResult = 0.0;
	for(size_t i = 0; i < 3; ++i)
		integrationResult += wk[i] * localFunction(pointIndexX, pointIndexY, xk[i], yk[i]);
	return integrationResult;
}

double TriangukarFluidFlowCalculator::localFunction(size_t pointIndexX, size_t pointIndexY, double x, double y)
{
	float result =
		localPartialDerivative(pointIndexX, 'x', x, y) * localPartialDerivative(pointIndexY, 'x', x, y)
		+ localPartialDerivative(pointIndexX, 'y', x, y) * localPartialDerivative(pointIndexY, 'y', x, y);
	return result;
}

double TriangukarFluidFlowCalculator::localPartialDerivative(size_t pointIndex, char pointCoordinate, double x, double y)
{
	MeshElement& element = mesh.el(currentElementIndex);
	std::vector<MeshPoint*>& pts = element.points;

	x = 0.0;
	y = 0.0;

	const double determinant =
		(pts.at(1)->x - pts.at(2)->x) * (pts.at(0)->y - pts.at(2)->y)
		- (pts.at(1)->y - pts.at(2)->y) * (pts.at(0)->x - pts.at(2)->x);

	double diff = 0.0;
	switch(pointCoordinate)
	{
	case 'x':
		switch(pointIndex)
		{
		case 0:
			diff = pts.at(2)->y - pts.at(1)->y;
			break;
		case 1:
			diff = pts.at(0)->y - pts.at(2)->y;
			break;
		case 2:
			diff = pts.at(1)->y - pts.at(0)->y;
			break;
		}
		break;
	case 'y':
		switch(pointIndex)
		{
		case 0:
			diff = pts.at(1)->x - pts.at(2)->x;
			break;
		case 1:
			diff = pts.at(2)->x - pts.at(0)->x;
			break;
		case 2:
			diff = pts.at(0)->x - pts.at(1)->x;
			break;
		}
		break;
	}

	return diff / determinant;
}
