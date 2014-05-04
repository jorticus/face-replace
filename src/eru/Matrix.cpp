//////////////////////////////////////////////////////////////////////
/**

\file    Matrix.cpp  

\brief   Implementation of the Matrix class.

*/
//////////////////////////////////////////////////////////////////////

#include "eru/StringStreamUtils.h"
#include "eruMath/Matrix.h"

//////////////////////////////////////////////////////////////////////
//
// From CLAPACK
//
//////////////////////////////////////////////////////////////////////

/*#include "clapack\f2clibs\f2c.h"

extern "C" {
int sgesv_(integer* n, integer* nrhs, real* a, integer* lda, 
	integer* ipiv, real* b, integer* ldb, integer* info);
int dgesv_(integer* n, integer* nrhs, doublereal* a, integer 
	*lda, integer* ipiv, doublereal* b, integer* ldb, integer* info);
}*/

//////////////////////////////////////////////////////////////////////

using namespace eruMath;

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////

Matrix::~Matrix()
{
	delete[] _data;
}

//////////////////////////////////////////////////////////////////////

Matrix::Matrix()
{
	_nRows = 0;
	_nCols = 0;
	_data  = 0;
}

//////////////////////////////////////////////////////////////////////

Matrix::Matrix( int nRows, int nCols )
{
	_nRows = nRows;
	_nCols = nCols;

	if ( !(_data = new(std::nothrow) double[ _nRows*  _nCols ]) )
    {
        //eruThrow( "Couldn't allocate memory for matrix!" );
	}
}

//////////////////////////////////////////////////////////////////////

Matrix::Matrix( const Matrix& m, CopyMode copyMode )
{
	if ( this != &m )
    {
		_nRows = m.nRows();
		_nCols = m.nCols();

		if ( !(_data = new(std::nothrow) double[ _nRows*  _nCols ]) )
        {
			return; //eruThrow( "Couldn't allocate memory for matrix!" );
		}

		if ( copyMode == deep ) {
			for ( int i = 0; i < _nRows*_nCols; i++ ) {
				_data[i] = m[i];
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////

void
Matrix::resize( int nRows, int nCols )
{

	if ( _nRows == nRows && _nCols == nCols )
    {
		return;
	}

	_nRows = nRows;
	_nCols = nCols;

	delete[] _data;

	if ( !(_data = new(std::nothrow) double[ _nRows*  _nCols ]) )
    {
        //eruThrow( "Couldn't allocate memory for matrix!" );
	}
}

//////////////////////////////////////////////////////////////////////

Matrix&
Matrix::operator=(const Matrix& m)
{
	if ( this != &m )
    {
		resize( m.nRows(), m.nCols() );
		for ( int i = 0; i < _nRows*_nCols; i++ )
        {
			_data[i] = m[i];
		}
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////

Matrix&
Matrix::operator=( double t )
{
	for ( int i = 0; i < _nRows*_nCols; i++ )
    {
		_data[i] = t;
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////

Matrix&
Matrix::operator=( const double* t )
{
	for (int i = 0; i < _nRows*_nCols; i++)
    {
		_data[i] = t[i];
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////

double&
Matrix::operator[]( int d )
{
	if ( d > nElements() )
    {
		//FCNNAME( "double& Matrix::operator [] (int)" );
		//eruThrow( "Index exceeds matrix dimensions!" );
	}
	return _data[d];
}

//////////////////////////////////////////////////////////////////////

double
Matrix::operator[]( int d ) const
{
	if ( d > nElements() )
    {
		//FCNNAME( "double Matrix::operator [] (int) const" );
		//eruThrow( "Index exceeds matrix dimensions!" );
	}
	return _data[d];
}

//////////////////////////////////////////////////////////////////////

double& Matrix::operator()( int row, int col )
{
	if ( row >= nRows() || col >= nCols() )
    {
		//FCNNAME( "double Matrix::operator () (int, int)" );
		//eruThrow( "Index exceeds matrix dimensions!" );
	}
	return _data[ row*_nCols + col ];
}

//////////////////////////////////////////////////////////////////////

double Matrix::operator()( int row, int col ) const
{
	if ( row >= nRows() || col >= nCols() )
    {
		//FCNNAME( "double& Matrix::operator () (int, int) const" );
		//eruThrow( "Index exceeds matrix dimensions!" );
	}
	return _data[ row*_nCols + col ];
}

//////////////////////////////////////////////////////////////////////

Matrix& Matrix::operator/=( const double t ) {
	if ( t == 0 ) {
		//FCNNAME( "Matrix& Matrix::operator/= ( const double )" );
		//eruThrow( "Division by zero!" );
	}
	for ( int i = 0; i < _nRows*_nCols; i++ ) {
		_data[i] /= t;
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////

Matrix& Matrix::operator+=( const double t ) {
	for ( int i = 0; i < _nRows*_nCols; i++ ) {
		_data[i] += t;
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////

Matrix& Matrix::operator*=( const double t ) {
	for ( int i = 0; i < _nRows*_nCols; i++ ) {
		_data[i] *= t;
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////

Matrix& Matrix::operator-=( const double t ) {
	for ( int i = 0; i < _nRows*_nCols; i++ ) {
		_data[i] -= t;
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////

Vector2d& Matrix::operator*( const Vector2d& v ) const {
	Vector2d* w = new(std::nothrow) Vector2d;
	if ( !w ) {
		//eruThrow( "Out of memory!" );
	}

	if ( _nRows == 1 && _nCols == 1 ) {
		*w = v*  elem(0,0);
		return* w;
	}

	if ( _nRows == 2 && _nCols == 2 ) {
		w->set(elem(0,0)*v[0]+elem(0,1)*v[1], elem(1,0)*v[0]+elem(1,1)*v[1]);
		return* w;
	}

	//eruThrow( "Dimensionality error! ");
	//return* w;
}


//////////////////////////////////////////////////////////////////////

Vector3d& Matrix::operator*( const Vector3d& v ) const {
	Vector3d* w = new(std::nothrow) Vector3d;
	if ( !w ) {
		//eruThrow( "Out of memory!" );
	}

	if ( _nRows == 1 && _nCols == 1 ) {
		*w = v*  elem(0,0);
		return* w;
	}

	if ( _nRows == 2 && _nCols == 2 ) {
		w->set(elem(0,0)*v[0]+elem(0,1)*v[1], elem(1,0)*v[0]+elem(1,1)*v[1], v[2]);
		return* w;
	}

	if ( _nRows == 3 && _nCols == 3 ) {
		w->set(
			elem(0,0)*v[0] + elem(0,1)*v[1] + elem(0,2)*v[2],
			elem(1,0)*v[0] + elem(1,1)*v[1] + elem(1,2)*v[2],
			elem(2,0)*v[0] + elem(2,1)*v[1] + elem(2,2)*v[2]
			);
		return* w;
	}

	//eruThrow( "Dimensionality error! ");
	return* w;
}

//////////////////////////////////////////////////////////////////////

Matrix& Matrix::operator*( const Matrix& m ) const {
	Matrix* dest = new(std::nothrow) Matrix;
	if ( !dest) {
		//FCNNAME( "Matrix& Matrix::operator*  ( const Matrix& ) const" );
		//eruThrow( "Out of memory! ");
	}
	dest->multiply( *this, m );
	return* dest;
}

//////////////////////////////////////////////////////////////////////

void Matrix::multiply( const Matrix& A, const Matrix& B ) {
	if ( A.nCols() != B.nRows() ) {
		//eruThrow( "Dimensionality error!" );
	}

	resize( A.nRows(), B.nCols() );

	for ( int row = 0; row < _nRows; row++ )	{
		for ( int col = 0; col < _nCols; col++ ) {
			double tmp = 0.0;
			for ( int i = 0; i < B.nRows(); i++ ) {
				tmp += A.elem( row, i )*  B.elem( i, col );
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////

void Matrix::fillCol( int col, float* data ) {
	if ( col >= nCols() ) {
		//FCNNAME( "void Matrix::fillCol( int, float* )" );
		//eruThrow( "Index exceeds matrix dimensions!" );
	}
	for ( int row = 0; row < _nRows; row++ ) {
		elem( row, col ) = (double)data[ row ];
	}
}

//////////////////////////////////////////////////////////////////////

void Matrix::fillRow( int row, float* data ) {
	if ( row >= nRows() ) {
		//FCNNAME( "void Matrix::fillRow( int, float* )" );
		//eruThrow( "Index exceeds matrix dimensions!" );
	}
	for ( int col = 0; col < _nCols; col++ )	{
		elem( row, col ) = (double)data[ col ];
	}
}

//////////////////////////////////////////////////////////////////////

Matrix& Matrix::transpose() {
	Matrix tmp(*this, deep );
	return transpose( tmp );
}

//////////////////////////////////////////////////////////////////////

Matrix& Matrix::transpose( const Matrix& m ) {
	if ( this == &m ) {
		return transpose();
	}

	resize( m.nCols(), m.nRows() );
	for (int row = 0; row < nRows(); row++ ) {
		for ( int col = 0; col < nCols(); col++ ) {
			elem( row, col ) = m[ col, row ];
		}
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////

double Matrix::det() const {

	if ( nRows() != nCols() )
        {
		return 0.0;//eruThrow( "Only a square matrix has a determinant!" );
	}

	switch( nRows() ) {
		case 0:
			return 0.0;
		case 1:
			return _data[0];
		case 2:
			return( _data[0]*_data[3] - _data[1]*_data[2]);
		case 3: 
			return(
				_data[0]*  _data[4]*  _data[8] +
				_data[1]*  _data[5]*  _data[6] +
				_data[2]*  _data[3]*  _data[7] -
				_data[0]*  _data[5]*  _data[7] -
				_data[1]*  _data[3]*  _data[8] -
				_data[2]*  _data[4]*  _data[6]
			);
		default:
			return 0.0;//eruThrow( "Determinants on larger matrices than 3x3 not implemented!" );
	}
}

//////////////////////////////////////////////////////////////////////

Matrix& Matrix::invert( const Matrix& m ){

	if ( nRows() != nCols() ) {
		//eruThrow( "Only a square matrix can be inverted!" );
	}

	if ( this == &m ) {
		invert(); 
	}

	resize( m.nRows(), m.nCols() );
	double _det;

	switch( m.nRows() )	{
		case 0:
			break;

		case 1:
            if ( m[0] == 0.0 ) {
                _data[0] = 0.0;
                //eruWarning( "Inverting singular 1x1 matrix! (Setting matrix to zero)" );
            }else {
  			    _data[0] = 1.0 / m[0];
			}
			break;
		case 2:
	  	    _det = m.det();
            if (_det == 0) {
                *this = 0.0;
                //eruWarning("Inverting singular 2x2 matrix! (Setting matrix to zero)");
            }else {
  			    elem(0,0) =  m[1,1] / _det;
	  		    elem(1,1) =  m[0,0] / _det;
		  	    elem(0,1) = -m[0,1] / _det;
			    elem(1,0) = -m[1,0] / _det;
            }
			break;

		default:
			double* srcdata = new(std::nothrow) double[ _nRows*  _nCols ];
			if ( !srcdata ) {
				//eruThrow( "Out of memory!" );
			}
			for ( int i = 0; i < _nRows*  _nCols; i++ ) {
				srcdata[i] = m[i];
			}
			for ( int row = 0; row < _nRows; row++ ) {
				for ( int col = 0; col < _nCols; col++ ) {
					elem( row, col ) = ( ( row == col ) ? 1 : 0 );
				}
			}

			// Solve the system src*  x == I, i.e. calculate the inverse of src.
			/*integer
				N     = _nRows,
				NRHS  = _nRows,
				LDA   = _nRows,
				*IPIV = new(std::nothrow) integer[ _nRows ],
				LDB   = _nRows,
				INFO;
			if ( !IPIV )
			{
				delete[] srcdata;
				eruThrow( "Out of memory! ");
			}*/
			//dgesv_(&N, &NRHS, srcdata, &LDA, IPIV, _data, &LDB, &INFO);
			delete[] srcdata;
//			delete[] IPIV;
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////

Matrix& Matrix::invert() {

	if ( nRows() != nCols() ) {
		//eruThrow( "Only a square matrix can be inverted!" );
	}

 	double _det;

	switch( _nRows ) {

		case 0:
			break;

		case 1:
            if ( _data[0] == 0.0 ) {
                //eruWarning("Inverting singular 1x1 matrix! (Setting matrix to zero)");
            }else {
  			     _data[0] = 1.0 / _data[0];
            }
			break;

		case 2: 
	  	    _det = det();
            if ( _det == 0 ) {
                *this = 0.0;
                //eruWarning("Inverting singular 2x2 matrix! (Setting matrix to zero)");
            }else {
  				double tmpd =  elem(0,0) / _det;
	  			elem(0,0)   =  elem(1,1) / _det;
		  		elem(1,1)   =  tmpd;
		  		elem(0,1)   = -elem(0,1) / _det;
		  		elem(1,0)   = -elem(1,0) / _det;
            }
			break;

		default:
			double* srcdata = new(std::nothrow) double[ _nRows*  _nRows ];
			if ( !srcdata ) {
				//eruThrow( "Out of memory!" );
			}

			for ( int i = 0; i < _nRows*  _nCols; i++ ) {
				srcdata[i] = _data[i];
			}
			for ( int row = 0; row < _nRows; row++ ) {
				for ( int col = 0; col < _nCols; col++ ) {
					elem( row, col ) = ( ( row == col ) ? 1 : 0 );
				}
			}
			// Solve the system src*  x == I, i.e. calculate the inverse of src.
			/*integer
				N     = _nRows,
				NRHS  = _nRows,
				LDA   = _nRows,
				*IPIV = new(std::nothrow) integer[ _nRows ],
				LDB   = _nRows,
				INFO;
			if ( !IPIV )
			{
				delete[] srcdata;
				eruThrow( "Out of memory!" );
			}*/
			//dgesv_(&N, &NRHS, srcdata, &LDA, IPIV, _data, &LDB, &INFO);
			delete[] srcdata;
			//delete[] IPIV;
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////

std::istream& operator >> ( std::istream& s, Matrix& m ) {
	int nCols, nRows;
	s >> nRows >> nCols;
	m.resize( nRows, nCols );
	for ( int i = 0; i < m.nRows()*m.nCols(); i++ ) {
		double d;
		s >> d;
		m[i] = d;
	}
	return s;
}

//////////////////////////////////////////////////////////////////////

std::ostream& operator << (std::ostream& s, const Matrix& m) {
	s << m.nRows() << " " << m.nCols() << std::endl;
	for ( int r = 0; r < m.nRows(); r++ ) {
		s << eruMath::startv;
		for (int c = 0; c < m.nCols(); c++) {
			s << m[r,c] << eruMath::fillv;
		}
		s << eruMath::endv << std::endl;
	}
	return s;
}

