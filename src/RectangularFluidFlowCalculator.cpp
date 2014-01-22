#include "RectangularFluidFlowCalculator.hpp"

RectangularFluidFlowCalculator::RectangularFluidFlowCalculator(RectangularMesh& mesh)
	: FluidFlowCalculator(4, mesh) { }

void RectangularFluidFlowCalculator::calculate()
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

		// TODO: fix the formula for determinant
		const double determinant = (pts[2]->y - pts[1]->y) * (pts[1]->x - pts[0]->x);
		const double area = std::abs(determinant);
		const double multiplier = 0.25 * area;

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

	for(size_t i = 0; i < pointsCount; ++i)
	{
		const MeshPoint& pt = mesh.GetPoint(i);

		if(pt.boundary)
		{
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

	// multiply inverse of stiffness matrix by mass matrix
	resultMatrix = left_divide(stiffnessMatrix, massMatrix);

#ifdef DEBUG
	{
		std::stringstream ss;
		ss << "final result = " << resultMatrix << std::endl;
		qDebug() << ss.str().c_str();
	}
#endif

	emit finished();

	/*
	//double b = 0;
	//double f = 0;
	//double u = ?;

	//std::vector<size_t> bound_indices;
	//std::vector<float> bound_val;

	//for(size_t i = 0; i < pointsCount; ++i)
	//	if(mesh.pt(i).boundary)
	//	{
	//		bound_indices.push_back(i);
	//		bound_val.push_back(mesh.pt(i).z);
	//	}

	//float* massMatrix = new float[pointsCount];
	//for(size_t y = 0; y < pointsCount; ++y)
	//	massMatrix[y] = 0.0;

	//std::vector<std::vector<float> > localStiffnessMatrix(elementSize);
	//for(size_t i = 0; i < elementSize; ++i)
	//	localStiffnessMatrix[i] = std::vector<float>(elementSize);
	//for(size_t x = 0; x < elementSize; ++x)
	//	for(size_t y = 0; y < elementSize; ++y)
	//		localStiffnessMatrix[x][y] = 0.0;
	*/

	/*
	//for(size_t i = 0; i < elementsCount; ++i)
	//{
	//	MeshElement& element = mesh.el(i);
	//	MeshPoint* pt1 = element.points.at(0);
	//	MeshPoint* pt2 = element.points.at(1);
	//	MeshPoint* pt3 = element.points.at(2);
	//	MeshPoint* pt4 = element.points.at(3);

	//	// flipped 1 and 3
	//	float multiplier = std::abs(pt3->y - pt2->y) * std::abs(pt2->x - pt1->x) * 0.25;

	//	for(size_t x = 0; x < 4; ++x)
	//		for(size_t y = 0; y < 4; ++y)
	//		{
	//			localStiffnessMatrix[x][y] = integral(i, x, y) * multiplier;
	//		}

	//	// needed only if f != 0
	//	//float* aaaa = new float[4];
	//	//for(size_t y = 0; y < 4; ++y)
	//	//	aaaa[y] = 0.0;
	//	//for(size_t y = 0; y < 4; ++y)
	//	//{
	//	//		// ...
	//	//}

	//	for(size_t x = 0; x < 4; ++x)
	//	{
	//		MeshPoint* ptx = element.points.at(x);
	//		for(size_t y = 0; y < 4; ++y)
	//		{
	//			MeshPoint* pty = element.points.at(y);

	//			stiffnessMatrix[ptx->index][pty->index] += localStiffnessMatrix[x][y];
	//		}
	//		//massMatrix;
	//	}
	//}
	*/
}

double RectangularFluidFlowCalculator::localIntegral(size_t pointIndexX, size_t pointIndexY)
{
	std::vector<double> xk(6);
	xk[0] = -0.666666666666667;
	xk[1] = -0.666666666666667;
	xk[2] = 0.333333333333333;
	xk[3] = 0.666666666666667;
	xk[4] = -0.333333333333333;
	xk[5] = 0.666666666666667;

	std::vector<double> yk(6);
	yk[0] = -0.666666666666667;
	yk[1] = 0.333333333333333;
	yk[2] = -0.666666666666667;
	yk[3] = 0.666666666666667;
	yk[4] = 0.666666666666667;
	yk[5] = -0.333333333333333;

	std::vector<double> wk(6);
	wk[0] = -0.666666666666667;
	wk[1] = -0.666666666666667;
	wk[2] = -0.666666666666667;
	wk[3] = -0.666666666666667;
	wk[4] = -0.666666666666667;
	wk[5] = -0.666666666666667;

	double total = 0.0;

	for(size_t i = 0; i < 6; ++i)
		total += wk[i] * localFunction(pointIndexX, pointIndexY, xk[i], yk[i]);

	return -total;
}

double RectangularFluidFlowCalculator::localFunction(size_t pointIndexX, size_t pointIndexY,
		double x, double y)
{
	double result =
			localPartialDerivative(pointIndexX, 'x', x, y) * localPartialDerivative(pointIndexY, 'x', x, y)
			+ localPartialDerivative(pointIndexX, 'y', x, y) * localPartialDerivative(pointIndexY, 'y', x, y);

	return result;
}

double RectangularFluidFlowCalculator::localPartialDerivative(size_t pointIndex,
		char pointCoordinate, double x, double y)
{
	std::vector<MeshPoint*>& pts = mesh.el(currentElementIndex).points;

	// TODO: fix formulas for determinants
	const double determinantX = pts.at(0)->x - pts.at(2)->x;
	const double determinantY = pts.at(0)->y - pts.at(2)->y;

	float result = 0.0;
	switch(pointIndex)
	{
	case 0:
		if(pointCoordinate == 'x')
			result = (1 + y) / (2 * determinantX);
		else if(pointCoordinate == 'y')
			result = (1 + x) / (2 * determinantY);
		break;
	case 1:
		if(pointCoordinate == 'x')
			result = (1 - y) / (2 * determinantX);
		else if(pointCoordinate == 'y')
			result = (-1 - x) / (2 * determinantY);
		break;
	case 2:
		if(pointCoordinate == 'x')
			result = (-1 + y) / (2 * determinantX);
		else if(pointCoordinate == 'y')
			result = (-1 + x) / (2 * determinantY);
		break;
	case 3:
		if(pointCoordinate == 'x')
			result = (-1 - y) / (2 * determinantX);
		else if(pointCoordinate == 'y')
			result = (1 - x) / (2 * determinantY);
		break;
	}

	return result;
}
