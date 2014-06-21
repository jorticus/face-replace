
#include <vector>
#include <boost\format.hpp>
#include "FaceTracker.h"
#include "models\CustomFaceModel.h"

#include <SFML\OpenGL.hpp>

using namespace std;

#define NUM_KINECT_SU 11
static const string kinect_su_map[NUM_KINECT_SU] = {
    "head height",
    "eyebrows vertical position",
    "eyes vertical position",
    "eyes, width",
    "eyes, height",
    "eye separation distance",
    "nose vertical position",
    "mouth vertical position",
    "mouth width",
    "", // Eyes vertical distance // Not specified in candide-3
    "", // Chin width // Not specified in candide-3
};

#define NUM_KINECT_AU 6
static const string kinect_au_map[NUM_KINECT_AU] = {
    "auv0   upper lip raiser (au10)",
    "auv11 jaw drop (au26/27)",
    "auv2   lip stretcher (au20)",
    "auv3   brow lowerer (au4)",
    "auv14 lip corner depressor (au13/15)",
    "auv5   outer brow raiser (au2)",
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

    // Look up kinect to wfm parameter mappings and store for later use
    su_map.clear();
    for (int i = 0; i < NUM_KINECT_SU; i++) {
        const string name = kinect_su_map[i];
        try {
            int map_idx = (!name.empty()) ? mesh.staticDeformationIndex(name) : -1;
            su_map.push_back(map_idx);
        }
        catch (exception& e) {
            throw runtime_error((boost::format("Kinect SU '%s' not found in the provided model") % name).str());
        }
    }
    au_map.clear();
    for (int i = 0; i < NUM_KINECT_AU; i++) {
        const string name = kinect_au_map[i];
        try {
            int map_idx = (!name.empty()) ? mesh.dynamicDeformationIndex(name) : -1;
            au_map.push_back(map_idx);
        }
        catch (exception& e) {
            throw runtime_error((boost::format("Kinect AU '%s' not found in the provided model") % name).str());
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
            int idx = su_map[i];
            if (idx >= 0) {
                mesh.setStaticParam(i, shapeUnits[i]);
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
            int idx = au_map[i];
            if (idx >= 0) {
                mesh.setDynamicParam(i, actionUnits[i]);
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

