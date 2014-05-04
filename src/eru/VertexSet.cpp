// VertexSet.cpp: implementation of the VertexSet class.
//
//////////////////////////////////////////////////////////////////////


#include <fstream>
//#include <FL/math.h>
#include "eruFace/VertexSet.h"
#include "eru/StringStreamUtils.h"

using namespace eruMath;
using namespace eruFace;

// ==========================================
// Contructors & destructor
// ==========================================

VertexSet::VertexSet()
{
}

// ==========================================

VertexSet::VertexSet( int size )
{
    _vertices.resize( size );
}

// ==========================================

VertexSet::VertexSet( const VertexSet& vs )
{
    _vertices.resize( vs.nVertices() );
    for (int i = 0; i < vs.nVertices(); i++)
    {
        _vertices[i] = vs[i];
    }
}

// ==========================================

VertexSet::~VertexSet()
{
}

// ==========================================

VertexSet&
VertexSet::operator=( const VertexSet& vs )
{
    if ( this == &vs )
    {
	return *this;
    }
    _vertices.resize( vs.nVertices() );

    for ( int i = 0; i < vs.nVertices(); i++ )
    {
        _vertices[i] = vs[i];
    }
    return *this;
}

// ==========================================
// Primitives
// ==========================================

Vertex&
VertexSet::operator[](int n)
{
    if ( n >= 0 || n < nVertices() )
    {
        return _vertices[n]; //std::stringstream ss; ss << "Index exceeds vertex set size! (size==" << _vertices.size() << ", index==" << n << ")";
    }
}

const Vertex&
VertexSet::operator[]( int n ) const
{
    if ( n >= 0 || n < nVertices() )
    {
        return _vertices[n]; //std::stringstream ss; ss << "Index exceeds vertex set size! (size==" << _vertices.size() << ", index==" << n << ")";	
    }
}

// ==========================================

void
VertexSet::init( int n )
{
    if ( n < 0 )
    {
        return;
    }
    _vertices.resize( n );
}  

void
VertexSet::clear()
{
    _vertices.clear();
}  

// ==========================================

Vertex
VertexSet::mean() const
{
    Vertex m;
    for (int i = 0; i < nVertices(); i++)
    {
        m += _vertices[i];
    }
    m /= _vertices.size();
    return m;
}

// ==========================================
// Basic wireframe transformations
// ==========================================

// ==========================================
// Transformation matrix

//#define CANDIDE_SHOULDERS

void
VertexSet::transform( const Matrix& m )
{
    if ( m.nCols() != 3 )
	{
        return; //eruThrow( "Vertices can be transformed by 3x3 matrices only!" );
    };

    #ifdef CANDIDE_SHOULDERS
        for (int i = 0; i < nVertices()-4; i++)
    #else
        for (int i = 0; i < nVertices(); i++)
    #endif
        {
            _vertices[i].set(
            m(0,0)*_vertices[i][0] + m(0,1)*_vertices[i][1] + m(0,2)*_vertices[i][2],
            m(1,0)*_vertices[i][0] + m(1,1)*_vertices[i][1] + m(1,2)*_vertices[i][2],
            m(2,0)*_vertices[i][0] + m(2,1)*_vertices[i][1] + m(2,2)*_vertices[i][2]);
        }
}

// ==========================================
// Rotation


void
VertexSet::rotate( const eruMath::Vector3d& v )
{
    rotate( v[0], v[1], v[2] );
}

void
VertexSet::rotate( double x, double y, double z )
{
    Matrix m(3,3);
    m(0,0) =  cos(z)*cos(y);
    m(0,1) = -sin(z)*cos(x)-cos(z)*sin(y)*sin(x);
    m(0,2) =  sin(z)*sin(x)-cos(z)*sin(y)*cos(x);

    m(1,0) =  sin(z)*cos(y);
    m(1,1) =  cos(z)*cos(x)-sin(z)*sin(y)*sin(x);
    m(1,2) = -cos(z)*sin(x)-sin(z)*sin(y)*cos(x);
 
    m(2,0) =  sin(y);
    m(2,1) =  cos(y)*sin(x);
    m(2,2) =  cos(y)*cos(x);

    transform( m );
}

// ==========================================

void
VertexSet::rotateAround(double x, double y, double z, int n)
{
    Vertex origin = _vertices[n];
    translate(-origin[0], -origin[1], -origin[2]);
    rotate(x, y, z);
    translate(origin);
}

// ==========================================
// Translation

void
VertexSet::translate( const Vertex& v )
{
    for (int i = 0; i < nVertices(); i++)
    {
        _vertices[i] += v;
    }
}

// ==========================================
// Scaling

void
VertexSet::scale( const Vertex& v )
{
    for (int i = 0; i < nVertices(); i++)
    {
        _vertices[i] *= v;
    }
}

// ==========================================

void
VertexSet::scaleTo2D( double width, double height )
{
    double min_x, max_x, min_y, max_y;
    min_x = max_x = _vertices[0][0];
    min_y = max_y = _vertices[0][1];

    for ( int i = 1; i < nVertices(); i++ )
    {
        min_x = min( _vertices[i][0], min_x );
        max_x = max( _vertices[i][0], max_x );
        min_y = min( _vertices[i][1], min_y );
        max_y = max( _vertices[i][1], max_y );
    }
    scale( width/(max_x-min_x), height/(max_y-min_y), 1 );
}

// =====================================================

void
VertexSet::applyDeformation( const Deformation& deformation, double coeff )
{
    for (int disp = 0; disp < deformation.nDisplacements(); disp++)
    {
        int vertexNo = deformation.vertexNo( disp );
        _vertices[vertexNo][0] += coeff*  deformation[disp][0];
        _vertices[vertexNo][1] += coeff*  deformation[disp][1];
        _vertices[vertexNo][2] += coeff*  deformation[disp][2];
    }
}

void
VertexSet::applyDeformations( const VertexSet& v, const std::vector<Deformation>& dv, const std::vector<double>& p )
{
    *this = v;
    for ( std::vector<Deformation>::size_type i = 0; i < dv.size(); i++ )  //?changed from i<=0
    { 
        applyDeformation( dv[i], p[i] );
    }
};


// ==========================================
// IO operators
// ==========================================

std::ostream&
eruFace::operator<<( std::ostream& s, const VertexSet& v )
{
    s << "# VERTEX LIST:\n" << v.nVertices() << "\n";
    for ( int i = 0; i < v.nVertices(); i++ )
    {
        s << v[i];
    }
    return s;
}

// ==========================================

std::istream&
eruFace::operator>>( std::istream& is, VertexSet& vs )
{
    int n;
    double x, y, z;
    eru::skipComments( is );
    is >> n;
    vs.init( n );

    for ( int i = 0; i < n; i++ )
    {
        is >> x >> y >> z;
	vs[i].set( x, y, z );
    }
    return is;
}

// ==========================================

bool
VertexSet::read( const std::string& fname )
{
    std::ifstream is( fname.c_str() );
    if (is.is_open())
    {
        is >> *this;
        return is.good();
    }
    return false;
}

// ==========================================

bool
VertexSet::write( const std::string& fname ) const
{
    std::ofstream os( fname.c_str() );
    if (os.is_open())
    {
        os << *this;
        return os.good();
    }
    return false;
}

