#ifndef VECTOR_H
#define VECTOR_H

#include "eruMath/eruMath.h"

#ifndef NO_STD_VECTOR
#include <vector>
#else

namespace std {
  template <class T> class vector {
    private:
      T* _data;
      unsigned int _size;
    public:
			typedef T* iterator;
			typedef const T* const_iterator;
			typedef unsigned int size_type;
      vector() {_size=0;_data=0;}
      ~vector() {delete[] _data;}
      unsigned int size() const { return _size; }
      void resize(unsigned int n) {
        if (n==_size)return;
        T* tmpdata=_data;
        if (!(_data=new(std::nothrow) T[n])) {
          throw eru::Exception("eruMath","bad_alloc","std::vector::resize();",
            __FILE__,__LINE__,true);
        }
        for (unsigned int i=0;i<min(n,_size);i++)_data[i]=tmpdata[i];
        delete[] tmpdata;
        _size=n;
      }
      void reserve(int n) {resize(n);};
      T& operator [] (int n) {return _data[n];}
      const T& operator [] (int n) const {return _data[n];}
      T& back() {return _data[_size-1];}
      T back() const{return _data[_size-1];}
      void clear() {delete[] _data;_size=0;}
      bool empty() {return(_size!=0);}
			void push_back(const T& t) {resize(_size+1);back()=t;}
			iterator begin() {return _data;};
			iterator end() {return (&_data[_size]);}
			const_iterator begin() const {return _data;};
			const_iterator end() const {return (&_data[_size]);}
			void assign(const_iterator a,const_iterator b) {clear();resize((int(b)-int(a))/sizeof(T));T* p=_data;while(a<b)*p++=*a++;}
			void assign(int c, const T& t) {clear();resize(c);for (int i=0;i<c;i++)_data[i]=t;}
			const_iterator erase(const_iterator t){T* tmp=new T[_size-1];T* n=tmp;T* c=_data;while(c<t)*n++=*c++;T* r=n;c++;while(c<end())*n++=*c++;delete[] _data;_data=tmp;_size--;return r;}
  };
}

#endif

namespace eruMath
{

//////////////////////////////////////////////////////////////////////
//
//  Vector<dim, T>
//
/// A class for dim-dimensional vectors of T's.
//
/// T must support:
/// - T operator =  (const T)
/// - T operator += (const T)
/// - T operator -= (const T)
/// - T operator* = (const T)
/// - T operator /= (const T)
/// - ostream operator << (ostream&, const T);
/// - istream operator >> (istream&, T);
//
//////////////////////////////////////////////////////////////////////

	template <unsigned int _size, class _T> class Vector {

#define _noth throw()
#define _thex throw( /*eru::Exception*/ )
#define _cnoth const throw()
#define _cthex const throw( /*eru::Exception*/ )

	public:

		typedef _T* _Tptr;
		typedef _T& _Tref;
		typedef const _T _Ct;
		typedef const _T& _Ctref;
		typedef const _T* _Ctptr;

		typedef Vector<_size,_T> _V;
		typedef _V* _Vptr;
		typedef _V& _Vref;
		typedef const _V _Cv;
		typedef const _V& _Cvref;
		typedef const _V* _Cvptr;

		typedef unsigned int size_type;

		/// Macro for iterating Vector<dim,T> elements.
#define _iter( v, k ) \
	for ( size_type k = 0; k < v.size(); k++ )
#define _Ak \
	for ( size_type k = 0; k < size(); k++ )

		size_type nDim()                              _cnoth { return _size;                         }
		size_type size()                              _cnoth { return _size;                         }

		void    set( _Cvref v )                       _noth  { _Ak _data[k] = v[k];                  }
		void    set( _Ctref x )                       _noth  { _Ak _data[k] = x;                     }
		void    set( _Ctref x, _Ctref y )             _noth  { _data[0] = x; if (_size>1) _data[1]=y; }
		void    set( _Ctref x, _Ctref y, _Ctref z )   _noth  { set(x,y); if (_size>2) _data[2]=z;     }
		void    set( _Ctptr p )                       _noth  { _Ak _data[k] = p[k];                  }

		Vector()                                      _noth  { _Ak _data[k] = _T(0);                 }
		Vector( _Cvref v )                            _noth  { set( v );                             }
		Vector( _Ctref x )                            _noth  { set( x );                             }
		Vector( _Ctref x, _Ctref y )                  _noth  { set( x, y );                          }
		Vector( _Ctref x, _Ctref y, _Ctref z )        _noth  { set( x, y, z );                       }
		Vector( _Ctptr p )                            _noth  { set( p );                             }

		_Vref  operator =  ( _Cvref v )               _noth  { set( v ); return *this; }
		_Vref  operator =  ( _Ctref t )               _noth  { set( t ); return *this; }
		_Vref  operator =  ( _Ctptr p )               _noth  { set( p ); return *this; }

		_Tref  operator [] ( size_type d )            _noth  { return _data[d]; }
		_Ctref operator [] ( size_type d )            _cnoth { return _data[d]; }

		_Tptr   ptr()                                 _noth  { return _data; }
		_Ctptr  ptr()                                 _cnoth { return _data; }

#define _CopCvref( _op, _th ) \
		_V operator _op ( _Cvref v ) const _th { _V w; _Ak w[k] = _data[k] _op v[k]; return w; }
#define _CopCtref( _op, _th ) \
		_V operator _op ( _Ctref t ) const _th { _V w; _Ak w[k] = _data[k] _op t; return w; }
#define _opCvref( _op ) \
		_Vref operator _op ( _Cvref v ) _noth { _Ak _data[k] _op v[k]; return *this; }
#define _opCtref( _op ) \
		_Vref operator _op ( _Ctref t ) _noth { _Ak _data[k] _op t; return *this; }

		_CopCvref( +, _noth )
		_CopCvref( -, _noth )
		_CopCvref( *, _noth )
		_CopCvref( /, _thex )

		_CopCtref( +, _noth )
		_CopCtref( -, _noth )
		_CopCtref( *, _noth )
		_CopCtref( /, _thex )

		_opCvref( += )
		_opCvref( -= )
		_opCvref(*= )

		_opCtref( += )
		_opCtref( -= )
		_opCtref(*= )

		_Vref operator /= ( _Cvref v ) _thex {
			if (v[k]!=0)
                        {
                            _Ak _data[k] /= v[k];
                            return *this;
                        }
		}

		_Vref  operator /= ( _Ctref t ) _thex {
			if ( t != 0 )
                        {
			    _Ak _data[k] /= t;
			    return *this;
                        }
		}


		friend _Vref operator*( _Ctref t, _Cvref v ) _noth { _V w(v); return(w* = t); }

		_T     operator &  ( _Cvref v )               _cnoth { _T t(0); _Ak t += _data[k]*  v[k]; return t; }

		bool   operator == ( _Cvref v )               _cnoth { _Ak if (_data[k] != v[k]) return false; return true; }
		bool   operator == ( _Ctref t )               _cnoth { _Ak if (_data[k] != t) return false; return true; }
		bool   operator != ( _Cvref v )               _cnoth { return(!(*this == v)); }
		bool   operator != ( _Ctref t )               _cnoth { return(!(*this == t)); }

		/// \brief Output a vector to a stream.
		/// 
		/// The output will start with eruMath::startv, the elements will
		/// be separated by eruMath::fillv, and finally eruMath::endv will
		/// be written to the stream.
		friend std::ostream& operator << ( std::ostream& s, _Cvref v ) _noth 
		{
			s << eruMath::startv;
			_iter( v, k ) s << v[k] << ( k+1 == _size ? eruMath::endv : eruMath::fillv );
			return s;
		}

		/// \brief Input a vector from a stream.
		friend std::istream& operator >> ( std::istream& s, _Vref v ) _noth
		{
			_T t; _iter( v, k ) { s >> t ; v[k] = t; } return s;
		}

	protected:
		_T _data[_size]; // The vector contents

	};


//////////////////////////////////////////////////////////////////////

	typedef Vector<3,int>    Vector3i;
	typedef Vector<3,float>  Vector3f;
	typedef Vector<3,double> Vector3d;
	typedef Vector<2,int>    Vector2i;
	typedef Vector<2,float>  Vector2f;
	typedef Vector<2,double> Vector2d;

} // namespace

#endif //#ifndef VECTOR_H
