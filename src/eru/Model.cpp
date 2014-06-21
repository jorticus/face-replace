// Model.cpp: implementation of the Model class.
//
//////////////////////////////////////////////////////////////////////

#include <direct.h>
#include <fstream>
#include <exception>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include "eru/Model.h"
#include "eru/StringStreamUtils.h"

using namespace eruFace;
using namespace eruMath;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Model::Model()
{
    // Settings

    // Values
    _rotation	             = 0.0;
    _scale                   = 1.0;
    _translation             = 0.0;
    _dynamicParamsModified   = false;
    _staticParamsModified    = false;
    _vportWidth              = 4;
    _vportHeight             = 4;

    // Compounds
    _texFilename = "";
    _wfmFilename = "";
    _smdFilename = "";
    _amdFilename = "";
}

Model::Model(double viewPortWidth, double viewPortHeight)
{
    Model();
    _vportWidth  = viewPortWidth;
    _vportHeight = viewPortHeight;
}
//////////////////////////////////////////////////////////////////////

void
Model::init(double viewPortWidth, double viewPortHeight)
{
    // Arrays
    _dynamicDeformations.clear();
    _dynamicParams.clear();
    _staticDeformations.clear();
    _staticParams.clear();
    _texCoords.clear();
    _faces.clear();
    _baseCoords.clear();
    _staticCoords.clear();
    _dynamicCoords.clear();
    _transformedCoords.clear();

    // Settings

    // Values
    _rotation                = 0.0;
    _scale                   = 1.0;
    _translation             = 0.0;
    _dynamicParamsModified   = false;
    _staticParamsModified    = false;
    _vportWidth              = viewPortWidth;
    _vportHeight             = viewPortHeight;

    // Compounds
    _texFilename = "";
    _wfmFilename = "";
    _smdFilename = "";
    _amdFilename = "";

    //_texture.Clear();
}

//////////////////////////////////////////////////////////////////////

Model::~Model()
{
}

//////////////////////////////////////////////////////////////////////
// Parameter setting
//////////////////////////////////////////////////////////////////////

int
Model::nDynamicDeformations() const
{
    return _dynamicDeformations.size();
}

void
Model::setDynamicParam(int paramNo, double paramVal) 
{
    if (paramNo >= nDynamicDeformations() || paramNo < 0)
    {
        throw std::runtime_error("Dynamic parameter number out of range!");
    }
    _dynamicParams[paramNo] = paramVal;
    _dynamicParamsModified = true;
}

double
Model::getDynamicParam(int paramNo) const
{
    if (paramNo >= nDynamicDeformations() || paramNo < 0)
    {
        throw std::runtime_error("Dynamic parameter number out of range!");
    }
    return _dynamicParams[paramNo];
}

//////////////////////////////////////////////////////////////////////

int Model::nStaticDeformations() const
{
    return _staticDeformations.size();
}

void
Model::setStaticParam(int paramNo, double paramVal)
{
    if (paramNo >= nStaticDeformations() || paramNo < 0)
    {
        throw std::runtime_error("Static parameter number out of range!");
    }
    _staticParams[paramNo] = paramVal;
    _staticParamsModified = true;
}

double
Model::getStaticParam(int paramNo) const
{
    if (paramNo >= nStaticDeformations() || paramNo < 0)
    {
        throw std::runtime_error("Static parameter number out of range!");
    }
    return _staticParams[paramNo];
}

//////////////////////////////////////////////////////////////////////

void
Model::getGlobalRxyzSTxy(double* g) const 
{
    g[0]=_rotation[0]; 
    g[1]=_rotation[1]; 
    g[2]=_rotation[2];
    g[3]=_scale[0];
    g[4]=_translation[0]; 
    g[5]=_translation[1];
}

void
Model::setGlobalRxyzSTxy(double* g)
{
    _rotation[0]=g[0]; 
    _rotation[1]=g[1]; 
    _rotation[2]=g[2];
    _scale=g[3];
    _translation[0]=g[4]; 
    _translation[1]=g[5];
}

//////////////////////////////////////////////////////////////////////

void Model::setRotation    (const Vector3d& r)            { _rotation = r; }
void Model::setScale       (const Vector3d& s)            { _scale = s; }
void Model::setTranslation (const Vector3d& t)            { _translation = t; }
void Model::setRotation    (double x, double y, double z) { _rotation.set(x, y, z); }
void Model::setScale       (double x, double y, double z) { _scale.set(x, y, z); }
void Model::setTranslation (double x, double y, double z) { _translation.set(x, y, z); }

//////////////////////////////////////////////////////////////////////

void Model::rotate         (const Vector3d& r)            { _rotation += r; }
void Model::scale          (const Vector3d& s)            { _scale *= s; }
void Model::translate      (const Vector3d& t)            { _translation += t; }
void Model::rotate         (double x, double y, double z) { _rotation[0] += x; _rotation[1] += y; _rotation[2] += z; }
void Model::scale          (double x, double y, double z) { _scale[0] *= x; _scale[1] *= y; _scale[2] *= z; }
void Model::translate      (double x, double y, double z) { _translation[0] += x; _translation[1] += y; _translation[2] += z; }
void Model::scale          (double s)                     { _scale[0] *= s; _scale[1] *= s; _scale[2] *= s; }

void
Model::setSmdFilename(const char* smd)
{ 
	_smdFilename = smd;
}

void
Model::setAllParams(const std::vector<double>& params)
{
	if (params.size() != 9 + nStaticDeformations() + nDynamicDeformations())
    {
        throw std::runtime_error("Bad argument: Wrong input vector size!");
	}
	_rotation[0]    = params[0];
	_rotation[1]    = params[1];
	_rotation[2]    = params[2];
	_scale[0]       = params[3];
	_scale[1]       = params[4];
	_scale[2]       = params[5];
	_translation[0] = params[6];
	_translation[1] = params[7];
	_translation[2] = params[8];
	for (int p = 0; p < nStaticDeformations(); p++)
    {
		setStaticParam(p, params[p+9]);
	}
	for (int p = 0; p < nDynamicDeformations(); p++)
    {
		setDynamicParam(p, params[p+9+nStaticDeformations()]);
	}
}

void
Model::getAllParams(std::vector<double>& params)
{
	params.resize(9 + nStaticDeformations() + nDynamicDeformations());
	params[0] = _rotation[0];
	params[1] = _rotation[1];
	params[2] = _rotation[2];
	params[3] = _scale[0];
	params[4] = _scale[1];
	params[5] = _scale[2];
	params[6] = _translation[0];
	params[7] = _translation[1];
	params[8] = _translation[2];

	for (int p = 0; p < nStaticDeformations(); p++)
    {
		params[p+9] = getStaticParam(p);
	}
	for (int p = 0; p < nDynamicDeformations(); p++)
    {
		params[p+9+nStaticDeformations()] = getDynamicParam(p);
	}
}

//////////////////////////////////////////////////////////////////////
// Update functions
//////////////////////////////////////////////////////////////////////


bool
Model::updateStatic()
{
	if (!_staticParamsModified)
    {
		return false;
    }
    _staticCoords.applyDeformations(_baseCoords, _staticDeformations, _staticParams);
	_staticParamsModified = false;
	_dynamicParamsModified = true;
	return true;
}

//////////////////////////////////////////////////////////////////////

bool
Model::updateDynamic()
{
	if (!updateStatic() && !_dynamicParamsModified)
    {
		return false;
    }
    _dynamicCoords.applyDeformations(_staticCoords, _dynamicDeformations, _dynamicParams);
	_dynamicParamsModified = false;
	return true;
}

//////////////////////////////////////////////////////////////////////

void
Model::updateGlobal()
{
	updateDynamic();
	_transformedCoords = _dynamicCoords;
    _transformedCoords.scale(_scale);
    _transformedCoords.rotate(_rotation);
    _transformedCoords.translate(_translation);
}

void
Model::updateImageCoords(int viewPortSize, int imageWidth, int imageHeight, Projection projection)
{
    updateGlobal();
    _imageCoords = _transformedCoords;
    double f = double(max(imageWidth, imageHeight)) / double(viewPortSize);
    switch(projection)
    {
        case ortographic:
        {
            _imageCoords.scale(f, -f);
		    break;
        }
	    case weakPerspective:
        {
            f /= (1.0 + _translation[2]);
		    _imageCoords.scale(f, -f);
		    break;
        }
	    case perspective:
        {
            for (int i = 0; i < nVertices(); i++)
            {
		        _imageCoords[i] *= (f / (1.0 + _translation[2] + _transformedCoords[i][2]));
		    }
		    break;
        }
    }
    _imageCoords.translate(double(imageWidth) / 2.0, double(imageHeight) / 2.0);
}


//////////////////////////////////////////////////////////////////////
// Copying
//////////////////////////////////////////////////////////////////////

void
Model::copyTexCoords(const Model& srcModel)
{
	_texCoords.assign(srcModel._texCoords.begin(), srcModel._texCoords.end());
}

//////////////////////////////////////////////////////////////////////

void
Model::copyTexCoordsFromVertices(const Model& srcModel)
{
    _texCoords.resize(srcModel.nVertices());
    for (int v = 0; v < srcModel.nVertices(); v++)
    {
        _texCoords[v][0] = srcModel.vertex(v)[0];
        _texCoords[v][1] = srcModel.vertex(v)[1];
    }
}

//////////////////////////////////////////////////////////////////////

void
Model::copyStaticParams(const Model& srcModel)
{
	_staticParams.assign(srcModel._staticParams.begin(), srcModel._staticParams.end());
}

//////////////////////////////////////////////////////////////////////

void
Model::copyDynamicParams(const Model& srcModel)
{
	_dynamicParams.assign(srcModel._dynamicParams.begin(), srcModel._dynamicParams.end());
}

//////////////////////////////////////////////////////////////////////

void
Model::copyGlobal(const Model& srcModel)
{
	_rotation = srcModel._rotation;
	_scale = srcModel._scale;
	_translation = srcModel._translation;
}

//////////////////////////////////////////////////////////////////////
// Graphics and Texture handling
//////////////////////////////////////////////////////////////////////

/*
void
Model::draw(Image& img, BYTE r, BYTE g, BYTE b)
{
    int x0;
    int y0;
    int x1; 
    int y1; 
    int x2; 
    int y2; 
    int px;
	
	if (img.Type(Image::inv_order))
    {
		px = (b<<16)|(g<<8)|r; //RGB(b,g,r);
    }
    else
    {
		px = (r<<16)|(g<<8)|b; //RGB(r,g,b);
    }

	for (int i = 0; i < nFaces(); i++)
    {
        Face f = face(i);
		x0 = (int)imageCoord(f[0])[0];
		y0 = (int)imageCoord(f[0])[1];
		x1 = (int)imageCoord(f[1])[0];
		y1 = (int)imageCoord(f[1])[1];
		x2 = (int)imageCoord(f[2])[0];
		y2 = (int)imageCoord(f[2])[1];

		img.DrawLine(x0, y0, x1, y1, px);
		img.DrawLine(x2, y2, x1, y1, px);
		img.DrawLine(x0, y0, x2, y2, px);
	}
}*/

//////////////////////////////////////////////////////////////////////

/*void
Model::copyTexture(const Image& newTex, const char* fname)
{
    _texFilename = fname;
	_texture.Init(newTex);
	_texture.Copy(newTex);
	_newTexture = true;
}

//////////////////////////////////////////////////////////////////////

bool
Model::fixTexCoords(double viewPortWidth, double viewPortHeight)
{
	if (nVertices() == 0) 
		return false;
	if (!_texture.Valid()) 
		return false;

    _texCoords.resize(nVertices());

	for (int v = 0; v < nVertices(); v++)
	{
		_texCoords[v][0] = (0.5 + vertex(v)[0] / viewPortWidth);
		_texCoords[v][1] = (0.5 - vertex(v)[1] / viewPortHeight);
	}
	return true;
}*/


//////////////////////////////////////////////////////////////////////
// File/stream input
//////////////////////////////////////////////////////////////////////

/*

FILE FORMAT

# Commented line

# VERTEX LIST:
int nVertices
for (i = 0:(nVertices-1)) {
	float x y z
}

# FACE LIST:
int nFaces
for (i = 1:nFaces) {
	int vertex0 vertex1 vertex2
}

# ANIMATION UNITS LIST:
int nAnimationUnits
for (i = 1:nAnimationUnits) {
# Action Unit name
	int nAffectedVertices
	for (j = 1:nAffectedVertices) {
		int AffectedVertex
		float x y z
	}
}

# SHAPE UNITS LIST:
int nShapeUnits
for (i = 1:nShapeUnits) {
	int nAffectedVertices
	for (j = 1:nAffectedVertices)) {
		int AffectedVertex
		float x y z
	}
}

# ANIMATION PARAMETERS:
int nAnimParams
for (i = 1:nAnimParams) {
	int AnimUnit
	float AnimParam
}

# SHAPE PARAMETERS:
int nShapeParams
for (i = 1:nShapeParams) {
	int ShapeUnit
	float ShapeParam
}

# TEXTURE:
int nTexCoords
if (nTexCoords > 0) {
	for (i = 0:(nTexCoords-1)) {
		int vertex
		float x y
	}
	string TextureFileName
}

# GLOBAL MOTION:
float R[0] R[1] R[2]
float S[0] S[1] S[2]
float _translation[0] T[1] T[2]

# END OF FILE
*/


bool
Model::read(const std::string& fname)
{ 
	bool success;

	//char oldPath[_MAX_PATH];
    std::string newPath = fname;// eru::getFilePath(fname);
	//_getcwd(oldPath, _MAX_PATH);
	//_chdir(newPath.c_str());
	std::string simplefilename = eru::getSimpleFileName(fname);
    std::string ext = eru::getLowerCaseFileExtension(fname);
	//std::ifstream is(simplefilename.c_str());
    std::ifstream is(fname);

	if (!is.is_open())
    {
		return false;
	}

	if (ext == "wfm")
    {
		if (success = read(is))
        {
			if (fname.length() <= _MAX_PATH)
            {
				_wfmFilename = fname;
				_amdFilename = fname;
				_smdFilename = fname;
			}
            else
            {
				_wfmFilename = "";
				_amdFilename = "";
				_smdFilename = "";
			}
		}
		return success;
	}

	if (ext == "amd")
    {
		if (success = readAMD(is))
        {
			if (fname.length() <= _MAX_PATH)
            {
				_amdFilename = fname;
			}
            else
            {
				_amdFilename = "";
			}
		}
		return success;
	}

	if (ext == "smd")
    {
		if (success = readSMD(is))
        {
			if (fname.length() <= _MAX_PATH)
            {
				_amdFilename = fname;
				_smdFilename = fname;
			}
            else
            {
				_amdFilename = "";
				_smdFilename = "";
			}
		}
		return success;
	}
 
    throw std::runtime_error((boost::format("Unknown file type: '%s'") % ext).str());
}

//////////////////////////////////////////////////////////////////////

bool
Model::readAMD(std::istream& is)
{
    eru::skipComments(is);
    std::string fname = eru::getNextLine(is);
    std::string ext = eru::getLowerCaseFileExtension(fname);
	if (ext != "wfm" && ext != "smd")
    {
        throw std::runtime_error((boost::format("Unknown file type: '%s'") % ext).str());
	}

	char oldPath[_MAX_PATH];
    std::string newPath = eru::getFilePath(fname);
	_getcwd(oldPath, _MAX_PATH);
	_chdir(newPath.c_str());
    bool success = read(eru::getSimpleFileName(fname));
	_chdir(oldPath);
    if (success)
    {
	    _smdFilename = fname;
        readParams(_dynamicParams, is);
	    readGlobal(is);
	    _dynamicParamsModified = true;
	    return true;
	}
    else
    {
        return false;
    }
}

//////////////////////////////////////////////////////////////////////

bool
Model::readSMD(std::istream& is)
{
	eru::skipComments(is);
    std::string fname = eru::getNextLine(is);
    std::string ext = eru::getLowerCaseFileExtension(fname);
	if (ext != "wfm")
    {
        throw std::runtime_error((boost::format("Unknown file type: '%s'") % ext).str());
	}
	if (!read(fname))
    {
        throw std::runtime_error((boost::format("Couldn't read wireframe file: %s (pointed to by static model file)!") % fname).str());
	}
	readParams(_staticParams, is);
	_staticParamsModified = true;
	return true;
}

//////////////////////////////////////////////////////////////////////

bool
Model::read(std::istream& is)
{
	_texFilename = "";
	is >> _baseCoords;
	if (!is) 
		return false;

	if (!is.eof())
    {
        readFaces(is);
        if (!is) 
			return false;
    }  
	if (!is.eof())
    {
        readDynamicDeformations(is);
        if (!is) 
			return false;
    }
	if (!is.eof())
    {
        readStaticDeformations(is);
        if (!is) 
			return false;
    }
	if (!is.eof())
    {
        readParams(_dynamicParams, is);
        if (!is)
			return false;
    }
	if (!is.eof())
    {
        readParams(_staticParams, is);
        if (!is)
			return false;
    }
	if (!is.eof())
    {
        readTexCoords(is);
        if (!is)
			return false;
    }
	if (!is.eof())
    {
        readGlobal(is);
        if (!is) 
			return false;
    }
	_staticParamsModified  = true;
	_dynamicParamsModified = true;
	updateGlobal();
	return true;
}

//////////////////////////////////////////////////////////////////////

void
Model::readFaces(std::istream& is)
{
    eru::skipComments(is);
    int nFaces, a, b, c;
    is >> nFaces;
    _faces.resize(nFaces);
    for (int i = 0; i < nFaces; i++)
    {
        is >> a >> b >> c;
        _faces[i].set(a, b, c);
    }
}

//////////////////////////////////////////////////////////////////////

void
Model::readDynamicDeformations(std::istream& is)
{
    readDeformations(_dynamicDeformations, "Action Unit", is);
    _dynamicParams.assign(nDynamicDeformations(), 0.0);

    _dynamicIndices.clear();
    for (int i = 0; i < _dynamicDeformations.size(); i++) {
        auto name = _dynamicDeformations[i].getName();
        boost::algorithm::to_lower(name);
        _dynamicIndices[name] = i;
    }
}

void
Model::readStaticDeformations(std::istream& is)
{
    readDeformations(_staticDeformations, "Shape Unit", is);
    _staticParams.assign(nStaticDeformations(), 0.0);

    _staticIndices.clear();
    for (int i = 0; i < _staticDeformations.size(); i++) {
        auto name = _staticDeformations[i].getName();
        boost::algorithm::to_lower(name);
        _staticIndices[name] = i;
    }
}

//////////////////////////////////////////////////////////////////////

void
Model::readDeformations(std::vector<Deformation>& dv, const std::string& defaultName, std::istream& is)
{
	eru::skipComments(is);
    int n;
	is >> n;
	if (0 < n)
    {
	    dv.resize(n);
		for (int i = 0; i < n; i++)
        {
			dv[i].read(is, defaultName, i);
		}
	}
}

//////////////////////////////////////////////////////////////////////

void
Model::readParams(std::vector<double>& dv, std::istream& is)
{
	eru::skipComments(is);
	int nParamsInFile;
	is >> nParamsInFile;
    int paramNo;
	double paramVal;
	for (int i = 0; i < nParamsInFile; i++)
    {
		is >> paramNo >> paramVal;
		dv[paramNo] = paramVal;
	}
}

//////////////////////////////////////////////////////////////////////

void
Model::readTexCoords(std::istream& is)
{
	eru::skipComments(is);
	int nCoordsInFile;
	is >> nCoordsInFile;

	if (nCoordsInFile > 0)
    {
        _texCoords.resize(nCoordsInFile);
		int coordNo;
        double x;
        double y;
		for (int i = 0; i < nCoordsInFile; i++)
        {
			is >> coordNo >> x >> y;
			_texCoords[coordNo].set(x,y);
		}
        _texFilename = eru::getNextLine(is);
	}
}

//////////////////////////////////////////////////////////////////////

void
Model::readGlobal(std::istream& is)
{
	eru::skipComments(is);
	is >> _rotation >> _scale >> _translation;
}

//////////////////////////////////////////////////////////////////////

bool
Model::readTexture(const std::string& fname)
{
	/*if (!_texture.Read(fname.c_str()))
    {
        return false;
    }*/
    _texFilename = fname;
	_newTexture = true;
	return true;
}

//////////////////////////////////////////////////////////////////////

std::istream& operator>>(std::istream& is, Model& md)
{
    md.read(is);
	return is;
}


//////////////////////////////////////////////////////////////////////
//
// File/stream output
//
//////////////////////////////////////////////////////////////////////


bool
Model::write(const std::string& fname)
{
	if (fname.length() >= _MAX_PATH)
    {
        throw std::runtime_error("Filename too long!");
	}
    std::string ext = eru::getLowerCaseFileExtension(fname);
  
	if (ext == "wrl")
    {
	    return writeVRML(fname);
	}
	std::ofstream os(fname.c_str());

	if (!os.is_open()) return false;

	if (ext == "wfm")
    {
		os << *this;
		os.flush();
		if (os.good())
        {
			_wfmFilename = fname;
			_smdFilename = fname;
			_amdFilename = fname;
            return true;
		}
		return false;
	}

	if (ext == "smd")
    {
		if (writeSMD(os))
        {
			_smdFilename = fname;
			_amdFilename = fname;
            return true;
		}
		return false;
	}

	if (ext == "amd")
    {
		if (writeAMD(os))
        {
			_amdFilename = fname;
            return true;
		}
		return false;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////

bool
Model::writeAMD(std::ostream& os) const
{
	const char* oldfillv = eruMath::fillv;
	const char* oldendv  = eruMath::endv;
	eruMath::fillv = "    ";
	eruMath::endv = "\n";
	os << std::fixed;

	os << "# STATIC FACE MODEL:\n" << _smdFilename << std::endl;
	writeDynamicParams(os);
	writeGlobal(os);
	os << "\n# END OF FILE\n";

	eruMath::fillv = oldfillv;
	eruMath::endv = oldendv;
	return os.good();
}

//////////////////////////////////////////////////////////////////////

bool
Model::writeSMD(std::ostream& os) const
{
	const char* oldfillv = eruMath::fillv;
	const char* oldendv  = eruMath::endv;
	eruMath::fillv = "    ";
	eruMath::endv = "\n";
	os << std::fixed;

	os << "# FACE MODEL:\n" << _wfmFilename << std::endl;
	writeDynamicParams(os);
	os << "\n# END OF FILE\n";

	eruMath::fillv = oldfillv;
	eruMath::endv = oldendv;
	return os.good();
}

//////////////////////////////////////////////////////////////////////

bool
Model::writeFaces(std::ostream& os) const
{
    os << "\n# FACES LIST:\n"
    << nFaces() << "\n";
	for (std::vector<Face>::const_iterator f = _faces.begin(); f != _faces.end(); f++)
    {
        os <<* f;
    }
    return os.good();
}

bool
Model::writeDynamicDeformations(std::ostream& os) const
{
	os << "\n# ANIMATION UNITS LIST:\n" << nDynamicDeformations() << std::endl;
	for (int i = 0; i < nDynamicDeformations(); i++)
    {
		os << std::endl << _dynamicDeformations[i];
	}
	return os.good();
}

//////////////////////////////////////////////////////////////////////

bool
Model::writeStaticDeformations(std::ostream& os) const
{
	os << "\n# SHAPE UNITS LIST:\n" << nStaticDeformations() << std::endl;
	for (int i = 0; i < nStaticDeformations(); i++)
    {
		os << std::endl << _staticDeformations[i];
	}
	return os.good();
}

//////////////////////////////////////////////////////////////////////

bool
Model::writeDynamicParams(std::ostream& os) const
{
	os << "\n# ANIMATION PARAMETERS:\n";
	return writeParams(_dynamicParams, os);
}

bool
Model::writeStaticParams(std::ostream& os) const
{
	os << "\n# SHAPE PARAMETERS:\n";
	return writeParams(_staticParams, os);
}

bool
Model::writeParams(const std::vector<double>& p, std::ostream& os) const
{
    os << p.size() << std::endl;
    for (unsigned int i = 0; i < p.size(); i++)
    {
      os << i << "\t" << p[i] << std::endl;
    }
    return os.good();
}


//////////////////////////////////////////////////////////////////////

bool
Model::writeTexCoords(std::ostream& os) const
{
	int nTexCoords = _texCoords.size();

	os << "\n# TEXTURE:\n" << nTexCoords << std::endl;
	if (nTexCoords > 0)
    {
		for (int i = 0; i < nTexCoords; i++)
        {
			os << i << "\t" << _texCoords[i];
		}
		os << _texFilename << std::endl;
	}
	return os.good();
}

//////////////////////////////////////////////////////////////////////

bool
Model::writeGlobal(std::ostream& os) const
{
	os << "\n# GLOBAL MOTION:\n" << _rotation << _scale << _translation;
	return os.good();
}

//////////////////////////////////////////////////////////////////////

std::ostream&
eruFace::operator<<(std::ostream& os, const Model& md)
{
	const char* oldfillv = eruMath::fillv;
	const char* oldendv  = eruMath::endv;
	eruMath::fillv = "    ";
	eruMath::endv = "\n";
	os << std::fixed;

	os
		<< "# Wireframe file written by the " //<< versionInfo._product << " "
		/*<< eruFace::versionInfo._version */<< std::endl
		<< "# from the Image Coding Group, Linköping University, Sweden\n\n"
		<< "# Fileversion: 2.1" << std::endl << std::endl;

    md.writeBaseCoords(os);
    md.writeFaces(os);
	md.writeDynamicDeformations(os);
	md.writeStaticDeformations(os);
	md.writeDynamicParams(os);
	md.writeStaticParams(os);
	md.writeTexCoords(os);
	md.writeGlobal(os);

	os << "\n# END OF FILE\n";

	eruMath::fillv = oldfillv;
	eruMath::endv = oldendv;
    return os;
}

//////////////////////////////////////////////////////////////////////

bool
Model::writeVRML(const std::string& fname)
{
	std::ofstream o(fname.c_str());
	if (!o.is_open()) return false;

	int i;
	bool txifile;
	const char* oldStartv = eruMath::startv;
	const char* oldFillv  = eruMath::fillv;
	const char* oldEndv   = eruMath::endv;

	txifile = (hasTexCoords() && _texFilename.length()>0);

	// Write File Header

	writeVRMLHeader(o, fname);
	o
		<< "        Group {\n"
		<< "          children [\n"
		<< "            Shape {\n\n"
		<< "              appearance Appearance {\n";

	if (txifile) {
		o
			<< "                texture ImageTexture {\n"
			<< "                  url \"" << _texFilename << "\"\n"
			<< "                }\n";
	}

	// Material definition
	o
		<< "                material Material {\n"
		<< "                  diffuseColor 0.8, 0.6, 0.6\n"
		<< "                  ambientIntensity 0.4\n"
		<< "                  specularColor 0.1 0.1 0.1\n"
		<< "                  emissiveColor 0 0 0\n"
		<< "                  shininess 0.0\n"
		<< "                  transparency 0\n"
		<< "                } # --- End of Material\n" 
		<< "              } # --- End of Appearance\n\n";

	o
		<< "              geometry IndexedFaceSet {\n";
	if (txifile)
		o << "                creaseAngle 1\n"; //with texture we want smooth shading
	else
		o << "                creaseAngle 0\n"; //without texture sharp edges look better
	o
		<< "                solid FALSE\n\n"
		<< "                coord Coordinate {\n"
		<< "                  point [\n";

	// Vertex coordinates

	eruMath::startv = "                    ";
	eruMath::fillv  = "\t\t";
	eruMath::endv   = ",\n";
	for (i = 0; i < nVertices()-1; i++)
    {
		o << _dynamicCoords[i];
	}
	eruMath::endv = "\n";
	o
		<< _dynamicCoords[i]
		<< "                  ] # --- End of Point\n"
		<< "                } # --- End of Coordinate\n\n";

	// Coordinate index (faces)
 
	o << "                coordIndex [\n";
	eruMath::startv = "                  ";
	eruMath::fillv  = ",\t\t";
	eruMath::endv   = ",\t\t-1,\n";
	for (i = 0; i < nFaces()-1; i++)
    {
		o << face(i);
	}
	eruMath::endv = ",\t\t-1\n";
	o
		<< face(i)
		<< "                ] # --- End of coordIndex\n\n";

	// Texture coordinates

	if (txifile)
    {
		o
			<< "                texCoord TextureCoordinate {\n"
			<< "                  point [\n";
		eruMath::startv = "                    ";
		eruMath::fillv  = "\t\t";
		eruMath::endv   = ",\n";
		for (i = 0; i < nVertices()-1; i++)
        {
			o << texCoord(i);
		}
		eruMath::endv = "\n";
		o
			<< texCoord(i)
			<< "                  ] # --- End of Point\n"
			<< "                } # --- End of TextureCoordinate\n\n";

	// Texture coordinate index	(faces)

		o << "                texCoordIndex [\n";
	eruMath::startv = "                  ";
	eruMath::fillv  = ",\t\t";
	eruMath::endv   = ",\t\t-1,\n";
		for (i = 0; i < nFaces()-1; i++)
        {
			o << face(i);
		}
		eruMath::endv = ",\t\t-1\n";
		o
			<< face(i)
			<< "                ] # --- End of texCoordIndex\n\n";
	}

	o
		<< "              } # --- End Of IndexedFaceSet\n\n"
		<< "            } # --- End Of Shape\n"
		<< "          ] # --- End Of Children\n"
		<< "        } # --- End Of Group\n"
		<< "      ] # --- End Of children \n"
		<< "    } # --- End Of Transform\n"
		<< "  ] # --- End Of children \n"
		<< "} # --- End Of Transform\n";

	o.flush();
	o.close();

	eruMath::startv = oldStartv;
	eruMath::fillv  = oldFillv;
	eruMath::endv   = oldEndv;

	return true;
}

//////////////////////////////////////////////////////////////////////

bool
Model::writeVRMLHeader(std::ostream& o, const std::string& fname)
{
    std::string name = eru::getStrippedName(fname);
	o
		<< "#VRML V2.0 utf8\n\n";

	o
		<< "NavigationInfo {\n"
		<< "  speed  5.0\n"
		<< "  type  \"EXAMINE\"\n"
		<< "}\n\n";

	o
		<< "Viewpoint { \n"
		<< "  fieldOfView  0.8\n"
		<< "  jump  FALSE\n"
		<< "  position     0 0 4\n"
		<< "  orientation     0 0 1 0 \n"
		<< "  description \"front\" \n"
		<< "}\n\n";

	o
		<< "Transform { # --- Shape : " << name << " ---\n"
		<< "  translation 0 0 0\n"
		<< "  rotation 0 0 1 0\n"
		<< "  children [\n";

	eruMath::startv = "";
	eruMath::fillv  = " ";
	eruMath::endv   = "";
	o
		<< "    Transform { # --- Shape : " << name << " # ---\n"
		<< "      translation " << _translation << std::endl
		<< "      rotation " << _rotation << " 0\n"
		<< "      children [\n";
	return true;
}

//////////////////////////////////////////////////////////////////////

// MPEG-4 FAP definitions
//                          0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59  60  61  62  63  64  65  66  67  68
const int FapDim[69]    = {-1, -1, -1,  1,  1,  1,  0,  0,  1,  1,  1,  1,  1,  1,  2,  0,  2,  2,  1,  1,  1,  1,  1,  0,  0,  1,  1,  2,  2, -1, -1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  1,  1,  0,  1,  2,  1,  1,  0,  1,  2,  1,  1,  0,  0,  1,  1,  1,  1,  1,  1,  0,  0,  1,  1,  1,  1,  0,  0};
const int FapFapu[69]   = { 0,  0,  0,  1,  1,  1,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  4,  4,  4,  4,  5,  5,  5,  5,  4,  4,  4,  4,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  3,  3,  0,  0,  0,  0,  5,  5,  5,  5,  1,  1,  0,  0,  1,  1,  1,  1,  1,  1,  3,  3,  3,  3,  3,  3,  3,  3};
const int FapPosMot[69] = { 0,  0,  0, -1, -1,  1,  1, -1, -1, -1,  1,  1,  1,  1,  1, -1,  1,  1,  1, -1, -1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1, -1,  1,  1, -1,  1,  1,  0,  0,  0,  0,  0,  1, -1,  1, -1,  1,  1, -1, -1, -1,  1,  1,  1,  1,  1, -1,  1, -1,  0,  0,  0,  0};
// down/right = -1, up/left/forward = 1

const int FapuDim[5]     = { 0,  1,  0,  1,  1};

// CANDIDE-specific FAP information
//                           0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59  60  61  62  63  64  65  66  67  68

const int FapVertex[69]  = {-1, -1, -1, 10, 87, 40, 88, 89, 81, 82, 83, 84, 88, 89, 10, 10, 40, 87,  9, 21, 54, 22, 55, -1, -1, -1, -1,  1,  1, -1, -1, 17, 50, 16, 49, 15, 48, 17, 50, 90, 91, 27, 60, -1, -1,  1, -1, -1, -2, -3, -4,  7,  8, 31, 64, 79, 80, 85, 86, 31, 64, 26, 59,  5,  5, -1, -1, -1, -1};
const int FapuVertex1[5] = {31,  6, 24,  6, 67};
const int FapuVertex2[5] = {64, 87, 57, 56, 68};

//////////////////////////////////////////////////////////////////////

double
Model::getFAPU(int fapu) const
{
	if (fapu == FapuAU)
    {
        return 1e-5;
    }
	return abs(_staticCoords[FapuVertex1[fapu]][FapuDim[fapu]]
		- _staticCoords[FapuVertex2[fapu]][FapuDim[fapu]]) / 1024.0;
}

//////////////////////////////////////////////////////////////////////

int
Model::getFAP(int fap) const
{
	int vert = FapVertex[fap];
	int fpm  = FapPosMot[fap];
	int fapu = FapFapu[fap];
	int dim  = FapDim[fap];

	if (vert == -1) return 0;
	if (vert <  -1) return fpm*  int(_rotation[dim] / getFAPU(fapu));
	return fpm*  int((_dynamicCoords[vert][dim] - _staticCoords[vert][dim]) / getFAPU(fapu));
}

//////////////////////////////////////////////////////////////////////

void
Model::getFAPs(int* faps) const
{
    for (int fap = 1; fap <= 68; fap++)
    {
        faps[fap-1] = getFAP(fap);
    }
}

//////////////////////////////////////////////////////////////////////

bool
Model::validFAP(int fap) const
{
    if (FapVertex[fap] == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

//////////////////////////////////////////////////////////////////////

void
Model::validFAPs(bool* faps) const
{
    for (int fap = 1; fap <= 68; fap++)
    {
        faps[fap-1] = validFAP(fap);
    }
}













/*
//////////////////////////////////////////////////////////////////////
	
int Model::Write(FILE* file)  {

	fprintf(file, "# Wireframe file written by the %s %s\n", FaceModel::versionInfo.productname, FaceModel::versionInfo.versionstring);
	fprintf(file, "# from the Image Coding Group, Linköping University, Sweden\n");

	if (!WriteWfm        (file)) return false;
	if (!WriteAnimUnits  (file)) return false;
	if (!WriteShapeUnits (file)) return false;
	if (!WriteAnimParams (file)) return false;
	if (!WriteShapeParams(file)) return false;
	if (!WriteTexCoords  (file)) return false;
	if (!WriteGlobal     (file)) return false;

	fprintf(file, "\n# END OF FILE\n");

	return true;
}

//////////////////////////////////////////////////////////////////////

int Model::WriteWfm(FILE* file) {
	return wfmBase.Write(file);
}

//////////////////////////////////////////////////////////////////////

int Model::WriteAnimUnits(FILE* file) {
	fprintf(file, "\n# ANIMATION UNITS LIST:\n%d\n", nAnimUnits);
	for (int i = 0; i < nAnimUnits; i++) {
		_dynamicDeformations[i].Write(file);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

int Model::WriteShapeUnits(FILE* file) {
	fprintf(file, "\n# SHAPE UNITS LIST:\n%d\n", nShapeUnits);
	for (int i = 0; i < nShapeUnits; i++) {
		_staticDeformations[i].Write(file);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

int Model::WriteTexCoords(FILE* file) {
	int nTexCoords = (texCoords != 0) ? nVertices() : 0;
	fprintf(file, "\n# TEXTURE:\n%d\n", nTexCoords);
	if (nTexCoords > 0) {
		for (int i = 0; i < nTexCoords; i++) {
			fprintf(file, "%d\t%1.5f  %1.5f\n", i, (float)texCoords[i][0], (float)texCoords[i][1]);
		}
		fprintf(file, "%s\n", texFilename);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

int Model::WriteAmd(FILE* file)
{
	fprintf(file, "# STATIC FACE MODEL:\n%s\n", smdFilename);
	if (!WriteAnimParams (file)) return false;
	if (!WriteGlobal     (file)) return false;
	fprintf(file, "\n# END OF FILE\n");

	return true;
}

//////////////////////////////////////////////////////////////////////

int Model::WriteSmd(FILE* file)
{
	fprintf(file, "# FACE MODEL:\n%s\n", wfmFilename);
	if (!WriteShapeParams (file)) return false;
	fprintf(file, "\n# END OF FILE\n");
	return true;
}

//////////////////////////////////////////////////////////////////////

int Model::WriteAnimParams(FILE* file) {
	fprintf(file,"\n# ANIMATION PARAMETERS:\n%d\n", nAnimUnits);
	for (int i = 0; i < nAnimUnits; i++) {
		fprintf(file, "%d\t%1.5f\n", i, (float)GetAnimParam(i));
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

int Model::WriteShapeParams(FILE* file)
{
	fprintf(file,"\n# SHAPE PARAMETERS:\n%d\n", nShapeUnits);
	for (int i = 0; i < nShapeUnits; i++) {
		fprintf(file, "%d\t%1.5f\n", i, (float)GetShapeParam(i));
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

int Model::WriteGlobal(FILE* file)
{
	fprintf(file, "\n# GLOBAL MOTION:\n");
	fprintf(file, "%1.5f  %1.5f  %1.5f\n", float(_rotation[0]), float(_rotation[1]), float(_rotation[2]));
	fprintf(file, "%1.5f  %1.5f  %1.5f\n", float(_scale[0]), float(_scale[1]), float(_scale[2]));
	fprintf(file, "%1.5f  %1.5f  %1.5f\n", float(_translation[0]), float(_translation[1]), float(_translation[2]));
	return true;
}

//////////////////////////////////////////////////////////////////////

int Model::Read(const char* fname) { 

  FILE* fp;
	int success;

	char oldPath[USEFUL_STRLEN];
	_getcwd(oldPath, USEFUL_STRLEN);
	_chdir(eru::getpath(fname));

	std::string ext(&fname[strlen(fname)-4]);

  if (!(fp = fopen(eru::getfilename(fname), "r"))){
		_chdir(oldPath);
		return false;
	}

	if (ext == ".wfm") {
	  success = Read(fp);
	  fclose(fp);
		if (strlen(fname) <= USEFUL_STRLEN) {
			strcpy(wfmFilename, fname);
			strcpy(amdFilename, fname);
			strcpy(smdFilename, fname);
		} else {
			wfmFilename[0] = 0;
			amdFilename[0] = 0;
			smdFilename[0] = 0;
		}
		_chdir(oldPath);
		return success;
	}

	if (ext == ".amd") {
	  success = ReadAmd(fp);
	  fclose(fp);
		if (strlen(fname) <= USEFUL_STRLEN) {
			strcpy(amdFilename, fname);
		} else {
			amdFilename[0] = 0;
		}
		_chdir(oldPath);
		return success;
	}

	if (ext == ".smd") {
	  success = ReadSmd(fp);
	  fclose(fp);
		if (strlen(fname) <= USEFUL_STRLEN) {
			strcpy(amdFilename, fname);
			strcpy(smdFilename, fname);
		} else {
			amdFilename[0] = 0;
			smdFilename[0] = 0;
		}
		_chdir(oldPath);
		return success;
	}

	FaceModel::Error("Unknown file type.");
	_chdir(oldPath);
	return false;
}

//////////////////////////////////////////////////////////////////////

int Model::ReadAmd(FILE* file) {

	char fname[USEFUL_STRLEN];

	eru::getnextstring(file, fname);
	std::string ext(&fname[strlen(fname)-4]);
	if (ext != ".wfm" && ext != ".smd") {
		FaceModel::Error(std::string("Unknown file type \"") + ext + "\"!");
		return false;
	}

	char oldPath[USEFUL_STRLEN];
	_getcwd(oldPath, USEFUL_STRLEN);
	_chdir(eru::getpath(fname));
	if (!Read(eru::getfilename(fname))) return false;
	_chdir(oldPath);
	strcpy(smdFilename, fname);

	ReadAnimParams(file);
	ReadGlobal(file);
	animParamsModified = true;
	return true;
}

//////////////////////////////////////////////////////////////////////

int Model::ReadSmd(FILE* file) {

	char fname[USEFUL_STRLEN];

	eru::getnextstring(file, fname);
	std::string ext(&fname[strlen(fname)-4]);
	if (ext != ".wfm") {
		FaceModel::Error(std::string("Unknown file type \"") + ext + "\"!");
		return false;
	}
	if (!Read(fname)) return false;

	ReadShapeParams(file);
	shapeParamsModified = true;
	return true;
}

//////////////////////////////////////////////////////////////////////

int Model::Read(FILE* file)	{

	texFilename[0] = 0;
	ReadWfm(file);
	//WfmVertex mean = wfmBase.Mean();
	//wfmBase.Translate(-mean[0], -mean[1], -mean[2]);
	
	if (!feof(file)) ReadAnimUnits(file);
	if (!feof(file)) ReadShapeUnits(file);
	if (!feof(file)) ReadAnimParams(file);
	if (!feof(file)) ReadShapeParams(file);
	if (!feof(file)) ReadTexCoords(file);
	if (!feof(file)) ReadGlobal(file);

	shapeParamsModified = true;
	animParamsModified  = true;

	UpdateGlobal();
	
	return true;
}

//////////////////////////////////////////////////////////////////////

int Model::ReadWfm(FILE* file) {
	return wfmBase.Read(file);
}

//////////////////////////////////////////////////////////////////////

int Model::ReadAnimUnits(FILE* file)
{
  nAnimUnits = eru::getnextint(file);
	if (nAnimUnits > 0) {
	  _dynamicDeformations = new DefUnit[nAnimUnits];
		_dynamicParams = new double[nAnimUnits];
		if (!_dynamicDeformations || !_dynamicParams) {
			FCNNAME("Model::ReadAnimUnits(FILE*)");
			eruError("Memory error: Couldn't allocate memory for animation units!");
		}
		for (int i = 0; i < nAnimUnits; i++) {
		  _dynamicDeformations[i].Read(file, "Action Unit ", i);
			_dynamicParams[i] = 0.0;
		}
	}
	return nAnimUnits;
}

//////////////////////////////////////////////////////////////////////

int Model::ReadShapeUnits(FILE* file)
{

  nShapeUnits = eru::getnextint(file);

	if (nShapeUnits > 0) {
	  _staticDeformations = new DefUnit[nShapeUnits];
		_staticParams = new double[nShapeUnits];
		if (!_staticDeformations || !_staticParams) {
			FCNNAME("Model::ReadShapeUnits()");
			eruError("Memory error: Couldn't allocate memory for shape units!");
		}
		for (int i = 0; i < nShapeUnits; i++) {
		  _staticDeformations[i].Read(file, "Shape Unit ", i);
			_staticParams[i] = 0.0;
		}
	}
	return nShapeUnits;
}

//////////////////////////////////////////////////////////////////////

int Model::ReadAnimParams(FILE* file) {
	int nParamsInFile = eru::getnextint(file);
	int i, p;
	float f;
	for (i = 0; i < nParamsInFile; i++) {
		fscanf(file, "%d %f", &p, &f);
		SetAnimParam(p, f);
	}
	return nParamsInFile;
}

//////////////////////////////////////////////////////////////////////

int Model::ReadShapeParams(FILE* file) {
	int nParamsInFile = eru::getnextint(file);
	int i, p;
	float f;
	for (i = 0; i < nParamsInFile; i++) {
		fscanf(file, "%d %f", &p, &f);
		SetShapeParam(p, f);
	}
	return nParamsInFile;
}

//////////////////////////////////////////////////////////////////////

int Model::ReadTexCoords(FILE* file) {

	int nCoordsInFile = eru::getnextint(file);

	if (nCoordsInFile > 0) {
		if (texCoords) delete[] texCoords;
		texCoords = new TexCoord[nVertices()];
		if (!texCoords) {
			FCNNAME("Model::ReadTexCoords()");
			eruError("Memory error: Couldn't allocate memory for texture coordinates!");
		}

		int i, v;
		float x, y;

		for (i = 0; i < nCoordsInFile; i++) {
			fscanf(file, "%d %f %f", &v, &x, &y);
			texCoords[v][0] = x;
			texCoords[v][1] = y;
		}
		eru::getnextline(file, texFilename);
	}
	return nCoordsInFile;
}

//////////////////////////////////////////////////////////////////////

int Model::ReadGlobal(FILE* file) {
	_rotation[0] = eru::getnextdouble(file);
	_rotation[1] = eru::getnextdouble(file);
	_rotation[2] = eru::getnextdouble(file);
	_scale[0] = eru::getnextdouble(file);
	_scale[1] = eru::getnextdouble(file);
	_scale[2] = eru::getnextdouble(file);
	_translation[0] = eru::getnextdouble(file);
	_translation[1] = eru::getnextdouble(file);
	_translation[2] = eru::getnextdouble(file);
	return true;
}

//////////////////////////////////////////////////////////////////////

*/