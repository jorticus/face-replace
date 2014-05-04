// Deformation.cpp: implementation of the Deformation class.
//
//////////////////////////////////////////////////////////////////////

#include <fstream>
#include "eruFace/Deformation.h"
#include "eru/StringStreamUtils.h"

using namespace eruFace;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Deformation::Deformation( void )
{
    _FAPNo = 0;
    _name  = "";
}

//////////////////////////////////////////////////////////////////////

void
Deformation::init(const std::string& name, int FAPNo,
											 const std::vector<int>& vertexNumbers,
											 const std::vector<Vertex>& vertexDisplacements )
{
    _FAPNo = FAPNo;
    _name = name;
    _vertexNumbers.assign( vertexNumbers.begin(), vertexNumbers.end() );
    _vertexDisplacements.assign( vertexDisplacements.begin(), vertexDisplacements.end() );
}

//////////////////////////////////////////////////////////////////////

void
Deformation::init( int n, const std::string& name, int FAPNo )
{
    _vertexNumbers.resize( n );
    _vertexDisplacements.resize( n );
    _FAPNo = FAPNo;
    _name = name;
}

//////////////////////////////////////////////////////////////////////

Deformation::Deformation( const Deformation& d )
{
    init( d._name, d._FAPNo, d._vertexNumbers, d._vertexDisplacements );
}

//////////////////////////////////////////////////////////////////////

Deformation::Deformation( int n, const std::string& name, int FAPNo )
{
	init( n, name, FAPNo );
}

//////////////////////////////////////////////////////////////////////

Deformation& Deformation::operator = ( const Deformation& d )
{
	if ( this != &d )
	{
		init( d._name, d._FAPNo, d._vertexNumbers, d._vertexDisplacements );
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////
// Primitives
//////////////////////////////////////////////////////////////////////

Vertex&
Deformation::operator[] ( int i )
{ 
  /*FCNNAME( "Deformation::operator[] ( int i )" );
  if ( i < 0 || i > nDisplacements() )
  {
    eruThrow( "Index out of range!" );
  }*/
  return _vertexDisplacements[i];
}

const Vertex&
Deformation::operator[] ( int i ) const
{
  /*FCNNAME( "Deformation::operator[] ( int i )" );
  if ( i < 0 || i > nDisplacements() )
  {
    eruThrow( "Index out of range!" );
  }*/
  return _vertexDisplacements[i];
}

int
Deformation::vertexNo( int i ) const
{
  /*FCNNAME( "Deformation::vertexNo( int i )" );
  if ( i < 0 || i > nDisplacements() )
  {
    eruThrow( "Index out of range!" );
  }*/
  return _vertexNumbers[i];
}

int
Deformation::findVertex( int v ) const
{
  for ( int j = 0; j < nDisplacements(); j++ )
  {
    if ( vertexNo(j) == v ) return j;
  }
  return -1;
}

//////////////////////////////////////////////////////////////////////
// File & stream I/O
//////////////////////////////////////////////////////////////////////

void
Deformation::read( std::istream& is, const std::string& defaultName, int no )
{
  int n;
	std::string name;
	int FAPNo = 0;
  std::stringstream ss;
  if ( no < 0 )
  {
    ss << defaultName;
  }
  else
  {
    ss << defaultName << " " << no;
  }
  name = ss.str();

  std::string line = eru::getNextLine( is );
  if ( line[0] == '#' ) // If there is a name in the file
  {
    int i = 1;
		while( line[i] == ' ' ) i++;
    name = std::string( line.begin()+i, line.end() );
    eru::skipComments( is );
    is >> n;
  }
  else
  {
    n = atoi( line.c_str() );
  }

  if ( std::string( _name, 0, 2 ) == "FAP" )
  {
    FAPNo = atoi( std::string( _name, 3, 4 ).c_str() );
  }

  init( n, name, FAPNo );
  for ( int i = 0; i < n; i++ )
  {
		int v;
		double x, y, z;
		is >> v >> x >> y >> z;
    set( i, v, x, y, z );
  }
}

//////////////////////////////////////////////////////////////////////

std::ostream&
eruFace::operator<<( std::ostream& s, const Deformation& d )
{  
	s << "# " << d.getName().c_str() << std::endl;
  s << d.nDisplacements() << std::endl;
  for (int i = 0; i < d.nDisplacements(); i++)
  {
    s << d.vertexNo(i) << "\t" << d[i];
  }
  return s;
}

//////////////////////////////////////////////////////////////////////

std::istream&
eruFace::operator>>( std::istream& is, Deformation& d )
{
  d.read( is, "Deformation", -1 );
  return is;
}

//////////////////////////////////////////////////////////////////////

bool
Deformation::read( const std::string& fname )
{
  std::ifstream is( fname.c_str() );
  if ( !is.is_open() ) return false;
  is >> *this;
	return is.good();
}

//////////////////////////////////////////////////////////////////////

bool
Deformation::write( const std::string& fname ) const
{
  std::ofstream os( fname.c_str() );
  if ( !os.is_open() ) return false;
  os << *this;
	return os.good();
}



