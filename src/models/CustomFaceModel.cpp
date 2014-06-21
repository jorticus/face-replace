
#include <vector>
#include <boost\format.hpp>
#include "FaceTracker.h"
#include "models\CustomFaceModel.h"

#include <SFML\OpenGL.hpp>

using namespace std;

CustomFaceModel::CustomFaceModel() : FaceModel()
{
}


CustomFaceModel::~CustomFaceModel()
{
}

bool CustomFaceModel::LoadMesh(std::string filename) {
    // Load the face mesh from a .wfm file (eg. candide3.wfm)
    if (!faceMesh.read(filename))
        return false;

    // Load the texture if defined
    if (!faceMesh._texFilename.empty()) {
        if (!texture.loadFromFile(faceMesh._texFilename)) {
            throw runtime_error((boost::format("Error loading face mesh texture '%s'") % faceMesh._texFilename).str());
        }
    }

    return true;
}

void CustomFaceModel::Initialize(IFTFaceTracker* pFaceTracker) {
    this->pFaceTracker = pFaceTracker;

    HRESULT hr;
    if (FAILED(hr = pFaceTracker->GetFaceModel(&pModel)))
        throw ft_error("Error initializing face model", hr);
}

void CustomFaceModel::UpdateModel(IFTResult* pFTResult, FT_CAMERA_CONFIG* pCameraConfig) {
    HRESULT hr;
    hasModel = false;

    if (pModel == nullptr)
        throw std::runtime_error("Face model not initialized");

    // Get face Action Units (AUs)
    float *pAUs;
    UINT auCount;
    if (FAILED(hr = pFTResult->GetAUCoefficients(&pAUs, &auCount)))
        throw ft_error("Error getting head AUs", hr);
    actionUnits = vector<float>(pAUs, pAUs + auCount);

    // Get face shape units (SUs)
    float headScale;
    FLOAT *pSUCoefs;
    UINT SUCount;
    BOOL haveConverged;
    if (FAILED(hr = pFaceTracker->GetShapeUnits(&headScale, &pSUCoefs, &SUCount, &haveConverged)))
        throw ft_error("Error getting head SUs", hr);
    shapeUnits = vector<float>(pSUCoefs, pSUCoefs + SUCount);

    // Use the AUs and SUs to deform the original mesh


    hasModel = true;
}

void CustomFaceModel::DrawGL() {
    if (hasModel) {
        bool hasTexcoords = faceMesh.hasTexCoords();

        glPushMatrix();

        glBegin(GL_TRIANGLES);
        for (int f = 0; f < faceMesh.nFaces(); f++) {
            auto face = faceMesh.face(f);

            for (int v = 0; v < (int)face.nDim(); v++) {
                int i = face[v];

                if (hasTexcoords) {
                    auto uv = faceMesh.texCoord(i);
                    glTexCoord2d(uv[0], uv[1]);
                }

                auto vertex = faceMesh.vertex(i);
                glVertex3d(vertex[0], vertex[1], vertex[2]);
            }
        }
        glEnd();

        glPopMatrix();
    }
}

