
#include <vector>
#include <boost\format.hpp>
#include "FaceTracker.h"
#include "models\CustomFaceModel.h"

#include <SFML\OpenGL.hpp>

using namespace std;

static const int su_map[] = {
    0, // Head height
    1, // Eyebrows vertical position
    2, // Eyes vertical position
    3, // Eyes width
    4, // Eyes height
    5, // Eye separation distance
    8, // Nose vertical position
    10, // Mouth vertical position
    11, // Mouth width
    -1, // Eyes vertical difference
    -1, // Chin width
};

static const int au_map[] = {
    0, // Upper lip raiser
    1, // Jaw lowerer
    2, // Lip stretcher
    3, // Brow lowerer
    4, // Lip corner depressor
    5, // Outer brow raiser
};

CustomFaceModel::CustomFaceModel() : FaceModel()
{
}


CustomFaceModel::~CustomFaceModel()
{
}

bool CustomFaceModel::LoadMesh(std::string filename) {
    // Load the face mesh from a .wfm file (eg. candide3.wfm)
    if (!mesh.read(filename))
        return false;

    // Load the texture if defined
    if (!mesh._texFilename.empty()) {
        if (!texture.loadFromFile(mesh._texFilename)) {
            throw runtime_error((boost::format("Error loading face mesh texture '%s'") % mesh._texFilename).str());
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

    // Get face shape units (SUs)
    float headScale;
    FLOAT *pSUCoefs;
    UINT suCount;
    BOOL haveConverged;
    if (FAILED(hr = pFaceTracker->GetShapeUnits(&headScale, &pSUCoefs, &suCount, &haveConverged)))
        throw ft_error("Error getting head SUs", hr);
    shapeUnits = vector<float>(pSUCoefs, pSUCoefs + suCount);

    // Use the AUs and SUs to deform the original mesh
    int nSD = mesh.nStaticDeformations();
    if (nSD > 0) {
        for (int i = 0; i < suCount; i++) {
            // Map kinect shape units to candide-3 shape units
            int su_idx = su_map[i];
            if (su_idx >= 0) {
                mesh.setStaticParam(su_idx, static_cast<double>(shapeUnits[i]));
            }
        }
        mesh.updateStatic();
    }

    // Get face Action Units (AUs)
    float *pAUs;
    UINT auCount;
    if (FAILED(hr = pFTResult->GetAUCoefficients(&pAUs, &auCount)))
        throw ft_error("Error getting head AUs", hr);
    actionUnits = vector<float>(pAUs, pAUs + auCount);

    int nDD = mesh.nDynamicDeformations();
    if (nDD > 0) {
        for (int i = 0; i < auCount; i++) {
            // Map kinect shape units to candide-3 action units
            int au_idx = au_map[i];
            if (au_idx >= 0) {
                mesh.setDynamicParam(au_idx, static_cast<double>(actionUnits[i]));
            }
        }
    }

    // Update the mesh
    mesh.updateGlobal();

    hasModel = true;
}

void CustomFaceModel::DrawGL() {
    if (hasModel) {
        bool hasTexcoords = mesh.hasTexCoords();

        glPushMatrix();

        glBegin(GL_TRIANGLES);
        for (int f = 0; f < mesh.nFaces(); f++) {
            auto face = mesh.face(f);

            for (int v = 0; v < (int)face.nDim(); v++) {
                int i = face[v];

                if (hasTexcoords) {
                    auto uv = mesh.texCoord(i);
                    glTexCoord2d(uv[0], uv[1]);
                }

                auto vertex = mesh.vertex(i);
                glVertex3d(vertex[0], vertex[1], vertex[2]);
            }
        }
        glEnd();

        glPopMatrix();
    }
}

