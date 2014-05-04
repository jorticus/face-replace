#ifndef ERUFACE_VERTEXSET_H
#define ERUFACE_VERTEXSET_H

#include "eruFace/eruFace.h"
#include "eruFace/Deformation.h"
#include "eruMath/Matrix.h"
#include "eruMath/Vector.h"
#include <istream>
#include <ostream>

namespace eruFace {

//////////////////////////////////////////////////////////////////////
//
//
//  VertexSet
//
/// A class for ...
//
//
//////////////////////////////////////////////////////////////////////

  class VertexSet
  {
  public:
    VertexSet                      ()                                  throw();
    VertexSet                      ( int size )                        throw(/*eru::Exception*/);
    VertexSet                      ( const VertexSet& )                throw(/*eru::Exception*/);
    VertexSet& operator=           ( const VertexSet& )                throw(/*eru::Exception*/);
    ~VertexSet                     ()                                  throw();

    Vertex& operator[]             ( int )                              throw(/*eru::Exception*/);
    const Vertex& operator[]       ( int ) const                        throw(/*eru::Exception*/);

    void        init               ( int n )                            throw(/*eru::Exception*/);
    void        clear              ()                                   throw(/*eru::Exception*/);
    int  inline nVertices          () const                             throw() { return _vertices.size(); }
	  Vertex      mean               () const                             throw();
  
    void        transform          ( const eruMath::Matrix& )          throw(/*eru::Exception*/);
    void        rotate             ( const eruMath::Vector3d& )        throw(/*eru::Exception*/);
	  void        rotate             ( double, double, double )           throw(/*eru::Exception*/);
	  void        rotateAround       ( double, double, double, int )      throw(/*eru::Exception*/);

	  void        translate          ( const Vertex& )                    throw();
	  void inline translate          ( double x, double y, double z = 0 ) throw() { Vertex v(x,y,z); translate(v); }

	  void        scale              ( const Vertex& )                    throw();
	  void inline scale              ( double f )                         throw() { Vertex v(f,f,f); scale(v); }
	  void inline scale              ( double x, double y, double z = 1 ) throw() { Vertex v(x,y,z); scale(v); }
	  void        scaleTo2D          ( double width, double height )      throw();

	  void        applyDeformation   ( const Deformation&, double );
    void        applyDeformations  ( const VertexSet&, const std::vector<Deformation>&, const std::vector<double>& ) throw(/*eru::Exception*/);


    friend std::istream& operator>>( std::istream&, VertexSet& )        throw(/*eru::Exception*/);
    friend std::ostream& operator<<( std::ostream&, const VertexSet& )  throw();
    bool        read               ( const std::string& )               throw();
    bool        write              ( const std::string& ) const          throw();

  protected:
  
    std::vector<Vertex> _vertices;
  };


} // namespace eruFace

#endif //#ifndef ERUFACE_VERTEXSET_H