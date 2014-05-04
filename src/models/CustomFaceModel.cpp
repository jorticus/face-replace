
#include "FaceTracker.h"
#include "models\CustomFaceModel.h"

#include <SFML\OpenGL.hpp>

CustomFaceModel::CustomFaceModel() : FaceModel()
{
}


CustomFaceModel::~CustomFaceModel()
{
}

bool CustomFaceModel::LoadMesh(std::string filename) {
    // Load the face mesh from a .wfm file (eg. candide3.wfm)
    return faceMesh.read(filename);
}

void CustomFaceModel::Initialize(IFTFaceTracker* pFaceTracker) {
    this->pFaceTracker = pFaceTracker;

    HRESULT hr;
    if (FAILED(hr = pFaceTracker->GetFaceModel(&pModel)))
        throw ft_error("Error initializing face model", hr);
}

void CustomFaceModel::UpdateModel(IFTResult* pFTResult, FT_CAMERA_CONFIG* pCameraConfig) {
    hasModel = true;
}

void CustomFaceModel::DrawGL() {
    bool has_texcoords = faceMesh.hasTexCoords();

    glPushMatrix();

    glBegin(GL_TRIANGLES);
    for (int f = 0; f < faceMesh.nFaces(); f++) {
        auto face = faceMesh.face(f);
        
        for (int v = 0; v < face.nDim(); v++) {
            int i = face[v];

            if (has_texcoords) {
                auto uv = faceMesh.texCoord(i);
                glTexCoord2f(uv[0], uv[1]);
            }

            auto vertex = faceMesh.vertex(i);
            glVertex3f(vertex[0], vertex[1], vertex[2]);
        }
    }
    glEnd();

    glPopMatrix();
}

