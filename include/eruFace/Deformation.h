#ifndef ERUFACE_DEFORMATION_H
#define ERUFACE_DEFORMATION_H

#include "eruFace/eruFace.h"
//#include <vector>

namespace eruFace {

//////////////////////////////////////////////////////////////////////
//
//
//  Deformation
//
/// A class for deformations of 3D wireframes.
//
//
//////////////////////////////////////////////////////////////////////

  class Deformation
  {
  public:

     // =======================================
    // Initialization

    Deformation                   () throw();
    Deformation                   ( int n, const std::string& name = "", int FAPNo = 0 ) throw(/*eru::Exception*/);
    Deformation                   ( const Deformation& ) throw(/*eru::Exception*/);
    void init                     ( int n, const std::string& name = "", int FAPNo = 0 ) throw(/*eru::Exception*/);
	void init                     ( const std::string&, int, const std::vector<int>&, const std::vector<Vertex>& ) throw(/*eru::Exception*/);
    Deformation& operator =       ( const Deformation& ) throw(/*eru::Exception*/);

     // =======================================
    // Primitives

    Vertex& operator[]            ( int i )              throw(/*eru::Exception*/);
    const Vertex& operator[]      ( int i ) const        throw(/*eru::Exception*/);

    int inline  nDisplacements    ()        const        throw() { return _vertexNumbers.size(); }
    int         vertexNo          ( int i ) const        throw(/*eru::Exception*/);
    int         findVertex        ( int )   const        throw();
    void inline set               ( int i, int v, double x, double y, double z ) throw() { _vertexNumbers[i] = v; _vertexDisplacements[i].set(x, y, z); }
    void inline set               ( int i, int v, const Vertex& x ) throw() { _vertexNumbers[i] = v; _vertexDisplacements[i] = x; }
	  inline const std::string& getName()     const        throw() { return _name; }

     // =======================================
    // File & stream I/O

    friend std::istream& operator>>(std::istream&, Deformation&) throw(/*eru::Exception*/);
    friend std::ostream& operator<<(std::ostream&, const Deformation&) throw();

    void       read               ( std::istream&, const std::string&, int ) throw(/*eru::Exception*/);
    bool       read               ( const std::string& ) throw(/*eru::Exception*/);
    bool       write              ( const std::string& ) const throw();

     // =======================================
    // Attributes

  private:
	  int                 _FAPNo;
      std::vector<int>    _vertexNumbers;
      std::vector<Vertex> _vertexDisplacements;
	  std::string         _name;
  };

} // namespace eruFace

#endif //#ifndef ERUFACE_DEFORMATION_H