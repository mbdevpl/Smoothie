#pragma once

#include "Mesh.hpp"

/*!
 * \brief Using finite element method, the calculator computes the values of "free" vertices
 *  using values of "boundary" (i.e. "fixed" or "constant") vertices.
 */
class FluidFlowCalculator : public QObject
{
	Q_OBJECT
protected:
	/*!
	 * \brief Number of points in the element. Assumed constant across all elements.
	 */
	size_t elementSize;
	Mesh& mesh;
	size_t currentElementIndex;
	/*!
	 * \brief The stiffness matrix is guaranteed to be correct only after
	 *  the calculate() method finishes.
	 */
	boost::numeric::ublas::matrix<double> stiffnessMatrix;
	/*!
	 * \brief The mass matrix is guaranteed to be correct only after
	 *  the calculate() method finishes.
	 */
	boost::numeric::ublas::matrix<double> massMatrix;
	/*!
	 * \brief The result matrix is guaranteed to be correct only after
	 *  the calculate() method finishes.
	 */
	boost::numeric::ublas::matrix<double> resultMatrix;

protected:
	FluidFlowCalculator(size_t elementSize, Mesh& mesh);

public:
	virtual ~FluidFlowCalculator();
	/*!
	 * \brief Performs the finite element method calculation on a mesh given in the constructor.
	 *  This method changes all of the matrices (stiffness, mass, result).
	 */
	virtual void calculate();
	double stiffness(size_t indexX, size_t indexY) const { return stiffnessMatrix(indexX, indexY); }
	double mass(size_t index) const { return massMatrix(index, 0); }
	double result(size_t index) const { return resultMatrix(index, 0); }
	inline const Mesh& GetMesh() const { return mesh; }

signals:
	void started();
	void progress(size_t progress);
	void finished();

protected:
	virtual double localIntegral(size_t pointIndexX, size_t pointIndexY) = 0;
	virtual double localFunction(size_t pointIndexX, size_t pointIndexY, double x, double y) = 0;
	virtual double localPartialDerivative(size_t pointIndex, char pointCoordinate, double x, double y) = 0;

};

namespace boost
{
namespace numeric
{
namespace ublas
{

//#define DEBUG_BOOST_UBLAS_CUSTOM

/*!
 * \brief Inverts given matrix. Uses lu_factorize and lu_substitute.
 * \param input an invertible matrix (will throw std::runtime_error if not invertible)
 * \return the inverse matrix
 */
template<class T>
boost::numeric::ublas::matrix<T> invert(const boost::numeric::ublas::matrix<T>& input)
{
	// create a working copy of the input
	matrix<T> A(input);
	// create a permutation matrix for the LU-factorization
	permutation_matrix<std::size_t> pm(A.size1());

	// perform LU-factorization
	int res = lu_factorize(A, pm);
	if(res != 0)
		throw new std::runtime_error("unable to perform LU factorization");

	matrix<T> inverse(input.size1(), input.size2());
	// create identity matrix of "inverse"
	inverse.assign(identity_matrix<T>(A.size1()));

	// backsubstitute to get the inverse
	lu_substitute(A, pm, inverse);

	return inverse;
}

template<class T>
boost::numeric::ublas::matrix<T> divide(const boost::numeric::ublas::matrix<T>& numerator,
		const boost::numeric::ublas::matrix<T>& denominator)
{
	if(numerator.size2() != denominator.size1())
		throw new std::runtime_error("unable to divide matrices of incompatible sizes");
#ifdef DEBUG_BOOST_UBLAS_CUSTOM
	{
		std::stringstream ss;
		ss << " * matrix division... " << std::endl;
		ss << " * numerator = " << numerator << std::endl;
		ss << " * denominator = " << denominator;
		qDebug() << ss.str().c_str();
	}
#endif
	matrix<T> inverse = invert(denominator);
#ifdef DEBUG_BOOST_UBLAS_CUSTOM
	{
		matrix<T> identity = prod(denominator, inverse);
		std::stringstream ss;
		ss << " * denominator inverse = " << inverse << std::endl;
		ss << " * identity ?= " << identity;
		qDebug() << ss.str().c_str();
	}
#endif
	matrix<T> quotient = prod(numerator, inverse);
#ifdef DEBUG_BOOST_UBLAS_CUSTOM
	{
		std::stringstream ss;
		ss << " * numerator / denominator = " << quotient << std::endl;
		qDebug() << ss.str().c_str();
	}
#endif
	return quotient;
}

template<class T>
boost::numeric::ublas::matrix<T> left_divide(const boost::numeric::ublas::matrix<T>& denominator,
		const boost::numeric::ublas::matrix<T>& numerator)
{
	if(denominator.size2() != numerator.size1())
		throw new std::runtime_error("unable to left-divide matrices of incompatible sizes");
#ifdef DEBUG_BOOST_UBLAS_CUSTOM
	{
		std::stringstream ss;
		ss << " * matrix division... " << std::endl;
		ss << " * numerator = " << numerator << std::endl;
		ss << " * denominator = " << denominator;
		qDebug() << ss.str().c_str();
	}
#endif
	matrix<T> inverse = invert(denominator);
#ifdef DEBUG_BOOST_UBLAS_CUSTOM
	{
		matrix<T> identity = prod(denominator, inverse);
		std::stringstream ss;
		ss << " * denominator inverse = " << inverse << std::endl;
		ss << " * identity ?= " << identity;
		qDebug() << ss.str().c_str();
	}
#endif
	matrix<T> quotient = prod(inverse, numerator);
#ifdef DEBUG_BOOST_UBLAS_CUSTOM
	{
		std::stringstream ss;
		ss << " * denominator \\ numerator = " << quotient << std::endl;
		qDebug() << ss.str().c_str();
	}
#endif
	return quotient;
}

}
}
}

namespace std
{

/*!
 * \brief Converts gievn value to string, with a certain precision.
 * \param value the value to be converted to string
 * \param precision the precision of the conversion
 * \return the resulting string
 */
template <typename T>
std::string to_string(const T value, const streamsize precision)
{
		std::ostringstream out;
		out << std::setprecision(precision) << value;
		return out.str();
}

}
