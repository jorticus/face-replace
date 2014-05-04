
#include "FaceTracker.h"
#include "models\CustomFaceModel.h"

#include <SFML\OpenGL.hpp>

CustomFaceModel::CustomFaceModel() : FaceModel()
{
}


CustomFaceModel::~CustomFaceModel()
{
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

/*void CustomFaceModel::DrawGL() {
    if (hasModel) {
        glBegin(GL_TRIANGLES);
        for each (auto tri in faces) {
            glTexCoord2fv(reinterpret_cast<const GLfloat*>(&uvcoords[tri.i]));
            glVertex3fv(reinterpret_cast<const GLfloat*>(&vertices[tri.i]));

            glTexCoord2fv(reinterpret_cast<const GLfloat*>(&uvcoords[tri.j]));
            glVertex3fv(reinterpret_cast<const GLfloat*>(&vertices[tri.j]));

            glTexCoord2fv(reinterpret_cast<const GLfloat*>(&uvcoords[tri.k]));
            glVertex3fv(reinterpret_cast<const GLfloat*>(&vertices[tri.k]));
        }
        glEnd();
    }
}*/

