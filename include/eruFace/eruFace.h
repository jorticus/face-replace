//===================================================================
/**

\file    eruFace.h  

\brief   Eru Face Model Library
    
This is the main header file for the Eru Face
Model Library (eruFace.dll). For more information,
see the eruFace namespace.

*/
//===================================================================
/**

\namespace eruFace

\brief Eru Face Model Library 

	The Eru Face Model Library provides
	functionality for wireframe-based face models.

	The main class is Model which uses the classes
	VertexSet and Deformation. The VertexSet contains the basic wireframe
	information (vertex coordinates) and the Deformation is a
	deformation to a VertexSet (like an Action Unit or a shape change).

	Model contains information on global changes (rotation, scale
	and translation), the faces (triangles) connecting the vertices,
	and four vertex sets:
	* One base set, preserving the original geometry.
	* One static set, which is the base set modified by
	  the static deformations.
	* One animated set, which is the static set modified by
	  the dynamic deformations.
	* One transformed set, which is the animated set transformed by
	  the global changes.

	Additionally, Model contains a texture and texture coordinates.
	Rendering is not included in the class, though. Suitably, that
	is covered by OpenGL or similar.
 
	The Eru Face Model Library uses the Standard Eru API for
	user messages, error handling, log files, version info, ...

\version 2.0.3

\date January 9, 2003
*/


//////////////////////////////////////////////////////////////////////
//  History
//////////////////////////////////////////////////////////////////////
//
// 
// 2.0.3 (2003-01-09):  Bugfixes: Reads amd and smd files. A few std::vector::reserve() changed to resize().
//
// 3.0.0 :Total rebuild
//
//////////////////////////////////////////////////////////////////////

#ifndef ERUFACE_H
#define ERUFACE_H

//////////////////////////////////////////////////////////////////////
// Includes
//////////////////////////////////////////////////////////////////////

#include "eruMath/Vector.h"

//////////////////////////////////////////////////////////////////////
// Typedefs and constants
//////////////////////////////////////////////////////////////////////

namespace eruFace {

	typedef eruMath::Vector3d Vertex;
	typedef eruMath::Vector2d TexCoord;
	typedef eruMath::Vector3i Face;

	const int FapuMW    = 0;
	const int FapuMNS   = 1;
	const int FapuES    = 2;
	const int FapuENS   = 3;
	const int FapuIRISD = 4;
	const int FapuAU    = 5;

	const double defaultViewPortSize  = 512;

    enum Projection { ortographic, weakPerspective, perspective };
}

#endif //#ifndef ERUFACE_H