//===================================================================
/**

\file    eruMath.h  

\brief   The Eru Mathematics & Statistics Library
    
This is the main header file for the Eru Mathematics
& Statistics Library (eruMath.dll). For more information,
see the eruMath namespace.

*/
//===================================================================
/**

\namespace eruMath

\brief Eru Mathematics & Statistics Library 

 The eruMath namespace contains classes, functions, and variables
 for vectors, matrices, Gaussian probabiliy distributions etc.
 implemented by the Eru Mathematics
 & Statistics Library (eruMath.dll).

 The Eru Mathematics & Statistics Library uses the Standard Eru API
 for user messages, error handling, log files, version info, ...

 The eruMath.dll uses CLAPACK for linear algebra.

\version 1.4.7

\date January 9, 2003

\todo Add throw declarations to VectorSet.
*/


//////////////////////////////////////////////////////////////////////
//  History
//////////////////////////////////////////////////////////////////////
//
// Created: 2000-09-20
//
// 1.4.5  (2002-12-31):*  operator>>(istream,Gaussian) added.
//                     *  std::vector reimplemented since the implementation
//                        in VC++6 is buggy. See NO_STD_VECTOR and Vector.h.
//                     *  VectorSet and Vector operator[] const returns reference.
// 1.4.6  (2003-01-03):*  Vector(T*) added.
//                     *  Vector::operator=(T*) added.
// 1.4.7  (2003-01-09): Bugfix: A few std::vector::reserve() changed to resize().
//
//////////////////////////////////////////////////////////////////////

#ifndef ERUMATH_H
#define ERUMATH_H

// Define NO_STD_VECTOR if you do not want to use STL's std::vector
// (buggy in Visual C++ 6).

#ifdef _MSC_VER
#if _MSC_VER < 1300
#define NO_STD_VECTOR
#endif
#endif

//////////////////////////////////////////////////////////////////////
// Includes
//////////////////////////////////////////////////////////////////////

//#include "eru/eru.h"
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////

/// Number of bits in look-up tables.
#define LUTBITS 7
#define IPLP(img) ((IplImage*)((img)._iplImage))

namespace eruMath {

	/// Random uniform variable in the range [0, 1].
	double inline rect01() {
		return double(rand()) / 32768.0;
	}

	/// Random uniform variable with zero mean.
	double inline rect() {
		return rect01() - 0.5;
	}

	/// Round to nearest integer.
	template <class T> int nint(T t) {
		return int(t > 0 ? t+0.5 : t-0.5);
	}

	/// Absolute value.
	template <class T> T abs(T t) {
		return (t<0 ? -t : t);
	}

	/// Sign
	template <class T> int sign(T t) {
		return (t==0 ? 0 : (t>0 ? 1 : -1));
	}

	/// Starting delimiter of vector.
	extern const char* startv;

	/// Fill delimiter of vector.
	extern const char* fillv;

	/// Ending delimiter of vector.
	extern const char* endv;

  /// Copy modes for matrix data.
	enum CopyMode {
		shallow, deep
	};

   #ifndef _WINDOWS_
   template<class T> T min(const T& a, const T& b) {
	   return(a<b?a:b);
   }
   template<class T> T max(const T& a, const T& b) {
	   return(a>b?a:b);
   }
   #endif


} // namespace eruMath

#endif //#ifndef ERUMATH_H