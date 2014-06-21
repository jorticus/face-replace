#ifndef ERUFACE_MODEL_H
#define ERUFACE_MODEL_H

#include "eruFace/VertexSet.h"
#include "eruFace/Deformation.h"

namespace eruFace
{

//////////////////////////////////////////////////////////////////////
//
//  Model
//
/// A class for 3D triangle models (wireframes, facet models).
//
//////////////////////////////////////////////////////////////////////

    class Model
    {
        public:
            // Construction/destruction/copying
            Model();
            Model(double, double);
            ~Model();
            void init(double, double);
            void clear() { init(0, 0); }

	    private:
	        Model( const Model& );
	        Model& operator=( const Model& );

	    public:
            // Primitives
            inline bool valid() const { return nVertices() > 0; }

            inline eruFace::Vertex& vertex( int v ) { return _transformedCoords[v]; }
            inline eruFace::Vertex vertex( int v ) const { return _transformedCoords[v]; }
            inline eruFace::Vertex& imageCoord( int v ) { return _imageCoords[v]; }
            inline eruFace::Vertex imageCoords( int v ) const { return _imageCoords[v]; }
            inline int nVertices() const { return _baseCoords.nVertices(); }

            inline eruFace::Face& face( int f ) { return _faces[f]; }
            inline eruFace::Face  face( int f ) const { return _faces[f]; }
            inline int nFaces() const { return _faces.size(); }

            // Deformations/parameters

            inline const Deformation& dynamicDeformation( int n ) const { return _dynamicDeformations[n]; }
            inline       Deformation& dynamicDeformation( int n ) { return _dynamicDeformations[n]; }
            inline const Deformation& staticDeformation( int n ) const { return _staticDeformations[n]; }
            inline       Deformation& staticDeformation( int n ) { return _staticDeformations[n]; }

            void       setDynamicParam( int, double );
            double     getDynamicParam( int ) const;
            int        nDynamicDeformations() const;
            void       setStaticParam( int, double );
            double     getStaticParam( int ) const;
            int        nStaticDeformations() const;
            bool       updateStatic();
            bool       updateDynamic();

            // Global transform (pose)
            void       setGlobal          (const eruMath::Vector3d& r, double s, const eruMath::Vector3d& t)          { _rotation = r; _scale = s; _translation = t;         }
            void       setGlobal          (const eruMath::Vector3d& r, const eruMath::Vector3d& s, const eruMath::Vector3d& t)	{ _rotation = r; _scale = s; _translation = t;         }

            void       getGlobal          (eruMath::Vector3d& r, eruMath::Vector3d& s, eruMath::Vector3d& t) const             { r = _rotation; s = _scale; t = _translation;         }
            void       getGlobal          (eruMath::Vector3d& r, double& s, eruMath::Vector3d& t)   const             { r = _rotation; s = _scale[0]; t = _translation;       }

            void       getGlobalRxyzSTxy  (double* g) const;
            void       setGlobalRxyzSTxy  (double* g);
            void       setRotation        (const eruMath::Vector3d& r);
            void       setScale           (const eruMath::Vector3d& s);
            void       setTranslation     (const eruMath::Vector3d& t);
            void       setRotation        (double x, double y, double z);
            void       setScale           (double x, double y, double z);
            void       setTranslation     (double x, double y, double z);

            void       rotate             (const eruMath::Vector3d& r);
            void       scale              (const eruMath::Vector3d& s);
            void       translate          (const eruMath::Vector3d& t);
            void       rotate             (double x, double y, double z);
            void       scale              (double x, double y, double z);
            void       translate          (double x, double y, double z);
            void       scale              (double s);

            void       updateGlobal();
            void       updateImageCoords( int viewPortSize, int imageWidth, int imageHeight, Projection projection = ortographic );

            void       setAllParams( const std::vector<double>& );
            void       getAllParams( std::vector<double>& );

            void       setSmdFilename     (const char*);

            // Copying
            void       copyTexCoords      (const Model&);
            void       copyTexCoordsFromVertices (const Model&);
            void       copyStaticParams   (const Model&);
            void       copyDynamicParams  (const Model&);
            void       copyGlobal         (const Model&);

	        // Graphics
            //void       draw                   (eruImg::Image& img, BYTE r, BYTE g, BYTE b);

	        // Texture handling
            //virtual void copyTexture      (const eruImg::Image& newTex, const char* fname);
            //bool       fixTexCoords       (double viewPortWidth, double viewPortHeight);
            //bool       hasTexture         ()               { return (_texture.Valid() && hasTexCoords()); }
            bool       hasTexCoords       ()               { return _texCoords.size()>0; }
            //int        getTexWidth        ()               { return _texture.Width(); }
            //int        getTexHeight       ()               { return _texture.Height(); }
            //void       initTexture        ( int w, int h, int t ) { _texture.Init( w, h, t ); }

            TexCoord&  texCoord           (int i)          { return _texCoords[i];  }
            const TexCoord texCoord      (int i) const    { return _texCoords[i];  }

            // File & stream I/O
            friend std::istream& operator >>   (std::istream&, Model&);
            friend std::ostream& operator <<   (std::ostream&, const Model&);

            bool       read               ( const std::string& );
            bool       read               ( std::istream& );

            virtual bool readTexture      ( const std::string& );
            bool       write              ( const std::string& );
            bool       writeVRML          ( const std::string& );

        protected:
            VertexSet _baseCoords;
            VertexSet _staticCoords;
            VertexSet _dynamicCoords;
            VertexSet _transformedCoords;
            VertexSet _imageCoords;

            std::vector<Face> _faces;

            std::vector<Deformation> _dynamicDeformations;
            std::vector<double>      _dynamicParams;
            
            std::vector<Deformation> _staticDeformations;
            std::vector<double>      _staticParams;
	  
            eruMath::Vector3d _rotation;
            eruMath::Vector3d _scale; 
            eruMath::Vector3d _translation;
            eruMath::Matrix   _transform;

            bool _staticParamsModified;
            bool _dynamicParamsModified;
            bool _transformModified;
            bool _newTexture;

            std::vector<TexCoord> _texCoords;

        public:
            //eruImg::Image _texture;
            std::string   _texFilename;
            std::string   _wfmFilename;
            std::string   _smdFilename;
            std::string   _amdFilename;
            double        _vportWidth;
            double        _vportHeight;

        protected:
            void createTransform();

	        // Internal File & stream I/O
            bool       readAMD            ( std::istream& );
            bool       readSMD            ( std::istream& );

            void       readFaces          ( std::istream& );
            void       readDynamicDeformations( std::istream& );
            void       readStaticDeformations( std::istream& );
            void       readDeformations   ( std::vector<Deformation>&, const std::string&, std::istream& );
            void       readParams         ( std::vector<double>&, std::istream& );
            void       readTexCoords      ( std::istream& );
            void       readGlobal         ( std::istream& );

            bool       writeAMD           ( std::ostream& ) const;
            bool       writeSMD           ( std::ostream& ) const;

            bool       writeBaseCoords    ( std::ostream& os ) const { os << _baseCoords; return os.good(); }
            bool       writeFaces         ( std::ostream& os ) const;
            bool       writeDynamicDeformations( std::ostream& ) const;
            bool       writeStaticDeformations( std::ostream& ) const;
            bool       writeDeformations  ( const std::vector<Deformation>&, std::ostream& ) const;
            bool       writeDynamicParams ( std::ostream& ) const;
            bool       writeStaticParams  ( std::ostream& ) const;
            bool       writeParams        ( const std::vector<double>&, std::ostream& ) const;
            bool       writeTexCoords     ( std::ostream& ) const;
            bool       writeGlobal        ( std::ostream& ) const;

            bool       writeVRMLHeader    ( std::ostream&, const std::string& );

            double     getFAPU            (int fapu)   const;
            int        getFAP             (int fap)    const;
            void       getFAPs            (int* faps)  const;
            bool       validFAP           (int fap)    const;
            void       validFAPs          (bool* faps) const;
    };

} // namespace eruFace

#endif // __ERUFACE_MODEL_H__