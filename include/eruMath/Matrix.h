#ifndef MATRIX_H
#define MATRIX_H

#include "eruMath/eruMath.h"
#include "eruMath/Vector.h"

namespace eruMath {

//////////////////////////////////////////////////////////////////////
//
//
//  Matrix
//
/// A class for matrices storing elements of type double.
//
//
//////////////////////////////////////////////////////////////////////

	class Matrix {

	public:

		/// \brief Default constructor.
		Matrix() throw();

		/// \brief Initialize to nRows rows and nCols columns.
		///
		/// \throws eru::Exception if memory could not be allocated.
		Matrix( int nRows, int nCols ) throw( /*eru::Exception*/ );

		/// \brief Copy-constructor.
    ///
		/// Depending on copyOp, only the dimensions of the source matrix
		/// are copied (shallow copy), or the data as well (deep copy).
		/// \throws eru::Exception if memory could not be allocated.
		Matrix( const Matrix& m, CopyMode copyMode = shallow ) throw(/*eru::Exception*/);

		/// \brief Desctructor.
		~Matrix() throw();

		/// \brief Resize the matrix. All data is lost.
		///
		/// \throws eru::Exception if memory could not be allocated.
		void resize( int nRows, int nCols ) throw(/*eru::Exception*/);

		/// \brief Number of rows.
		int nRows() const throw()  {	return _nRows; }

		/// \brief Number of columns.
		int nCols() const throw()  {	return _nCols; }

		/// \brief Number of data elements.
		int nElements() const throw()  {	return _nCols*  _nRows; }

		/// \brief Copy data from another matrix.
		///
		/// \throws eru::Exception if memory could not be allocated.
		Matrix& operator = ( const Matrix& m ) throw(/*eru::Exception*/);

		/// \brief Copy data from memory pointed to by \c ptr.
		Matrix& operator = ( const double* ptr ) throw();

		/// \brief Fill matrix with value of \c t.
		Matrix& operator = ( double t ) throw();

		/// \brief Get pointer to data.
		operator double*() throw() { return _data; }

		/// \brief Get data element.
		///
		/// \throws eru::Exception if \c d exceeds matrix dimensions.
		double& operator [] ( int d ) throw( /*eru::Exception*/ );

		/// \brief Get const data element.
		///
		/// \throws eru::Exception if \c d exceeds matrix dimensions.
		double  operator [] ( int d ) const throw( /*eru::Exception*/ );

		/// \brief Get data element.
		///
		/// \throws eru::Exception if (\c row, \c col) exceeds matrix dimensions.
		double& operator () ( int row, int col ) throw( /*eru::Exception*/ );

		/// \brief Get const data element.
		///
		/// \throws eru::Exception if (\c row, \c col) exceeds matrix dimensions.
		double  operator () ( int row, int col ) const throw( /*eru::Exception*/ );

		/// \brief Divide all elements with \c t.
		///
		/// \throws eru::Exception if \c t equals zero.
		Matrix& operator /= ( double t ) throw( /*eru::Exception*/ );

		/// \brief Add \c t to all elements.
		Matrix& operator += ( double t ) throw();

		/// \brief Subtract \c t from all elements.
		Matrix& operator -= ( double t ) throw();

		/// \brief Multiply all elements with \c t.
		Matrix& operator*=( double t ) throw();

		/// \brief Multiply a matrix with a 2D vector.
		///
		/// \throws eru::Exception if memory could not be allocated or
		///         if the matrix has the wrong size (the sizes 1x1 or 2x2
		///         are allowed).
		Vector2d& operator*( const Vector2d& ) const throw(/*eru::Exception*/ );

		/// \brief Multiply a matrix with a 3D vector.
		///
		/// \throws eru::Exception if memory could not be allocated or
		///         if the matrix has the wrong size (the sizes 1x1, 2x2,
		///         and 3x3 are allowed).
		Vector3d& operator*  ( const Vector3d& ) const throw(/*eru::Exception*/ );

		/// \brief Multiply with another matrix.
		///
		/// \throws eru::Exception if memory could not be allocated or
		///         the matrices do not have compatible sizes.
		Matrix& operator*  ( const Matrix& ) const throw( /*eru::Exception*/);

		/// \brief Multiply two matrices and store result in current matrix.
		///
		/// \throws eru::Exception if memory could not be allocated or
		///         the matrices do not have compatible sizes.
		void multiply( const Matrix&, const Matrix& );

		/// \brief Fill a row of the matrix with data pointed to by fptr.
		///
		/// \throws eru::Exception if \c row exceeds matrix dimensions.
		void fillRow( int row, float* fptr ) throw(/*eru::Exception*/);

		/// \brief Fill a column of the matrix with data pointed to by fptr.
		///
		/// \throws eru::Exception if \c col exceeds matrix dimensions.
		void fillCol( int col, float* fptr ) throw(/*eru::Exception*/);

		/// \brief Transpose the matrix.
		///
		/// \throws eru::Exception if temporary memory could not be allocated.
		Matrix& transpose() throw(/*eru::Exception*/);

		/// \brief Transpose another matrix and store the result in the current matrix.
		///
		/// \throws eru::Exception if temporary memory could not be allocated.
		Matrix& transpose( const Matrix& ) throw(/*eru::Exception*/);

		/// \brief Compute determinant of a square matrix.
		///
		/// \throws eru::Exception if matrix is non-square or larger than 3x3.
		/// \todo Matrices larger than 3x3 should be handled.
		double det() const throw(/*eru::Exception*/ );

		/// \brief Invert a square matrix.
		///
		/// The inverted matrix is stored in the calling matrix AND returned.
		/// If the matrix is singular, all elements are set to zero.
		/// This routine uses LAPACK.
		/// \throws eru::Exception if the matrix is non-square or
		///         if memory could not be allocated.
		Matrix& invert( const Matrix& ) throw(/*eru::Exception*/);

		/// \brief Invert a square matrix.
		///
		/// The inverted matrix is stored in the calling matrix AND returned.
		/// If the matrix is singular, all elements are set to zero.
		/// This routine uses LAPACK.
		/// \throws eru::Exception if the matrix is non-square or
		///         if memory could not be allocated.
		Matrix& invert() throw(/*eru::Exception*/);

		/// \brief Returns true if the matrix is singular (with a given tolerance).
		///
		/// \throws eru::Exception if matrix is non-square or larger than 3x3.
		/// \todo Matrices larger than 3x3 should be handled.
		bool singular( double t = 0.0 ) const throw(/*eru::Exception*/) { return ( abs(det()) <= t ); }

		/// \brief Read a matrix from a stream.
		///
		/// \throws eru::Exception if memory could not be allocated.
		friend std::istream& operator >> (std::istream&, Matrix&) throw(/*eru::Exception*/);

		/// \brief Write a matrix to a stream.
		friend std::ostream& operator << (std::ostream&, const Matrix&);

	private:

	  /// Number of rows.
		int _nRows;

	  /// Number of columns.
		int _nCols;

		/// Pointer to data.
		double* _data;

		/// Get an element.
		double& elem( int row, int col )       { return _data[ row*_nCols + col ]; }

		/// Get a const element.
		double  elem( int row, int col )  const { return _data[ row*_nCols + col ]; }

	};

} // namespace

#endif //#ifndef MATRIX_H