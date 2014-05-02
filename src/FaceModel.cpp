
#include "FaceTracker.h"
#include "FaceModel.h"

#include <SFML\OpenGL.hpp>

using namespace sf;
using namespace std;

FaceModel::FaceModel() :
pModel(nullptr),
pFaceTracker(nullptr),
hasModel(false)
{

}

FaceModel::~FaceModel()
{
}

void FaceModel::Initialize(IFTFaceTracker* pFaceTracker) {
    this->pFaceTracker = pFaceTracker;

    HRESULT hr;
    if (FAILED(hr = pFaceTracker->GetFaceModel(&pModel)))
        throw ft_error("Error initializing face model", hr);
}

void FaceModel::UpdateModel(IFTResult* pFTResult, FT_CAMERA_CONFIG* pCameraConfig) {
    HRESULT hr;

    if (pModel == nullptr)
        throw std::runtime_error("Face model not initialized");

    UINT vertexCount = pModel->GetVertexCount();

    // Get 3D pose
    float scale, rotation[3], translation[3];
    pFTResult->Get3DPose(&scale, rotation, translation);

    // Get face Action Units (AUs)
    float *pAUs;
    UINT auCount;
    if (FAILED(hr = pFTResult->GetAUCoefficients(&pAUs, &auCount)))
        throw ft_error("Error getting head AUs", hr);

    // Get face shape units (SUs)
    float headScale;
    FLOAT *pSUCoefs;
    UINT SUCount;
    BOOL haveConverged;
    if (FAILED(hr = pFaceTracker->GetShapeUnits(&headScale, &pSUCoefs, &SUCount, &haveConverged)))
        throw ft_error("Error getting head SUs", hr);

    // Allocate 2D points
    /*vertexCount = pModel->GetVertexCount();
    FT_VECTOR2D* pPts2D = reinterpret_cast<FT_VECTOR2D*>
        (_malloca(sizeof(FT_VECTOR2D)* vertexCount));
    if (pPts2D == nullptr)
        throw runtime_error("Error allocating memory");

    // Get mesh vertices, deformed by AUs, SUs, and 3D pose.
    POINT p = { 0, 0 };
    hr = pModel->GetProjectedShape(
        pCameraConfig, 1.0f, p,
        pSUCoefs, pModel->GetSUCount(), pAUs, auCount,
        scale, rotation, translation,
        pPts2D, vertexCount
    );
    if (FAILED(hr))
    throw ft_error("Error projecting 3D face model", hr);*/


    // Allocate 3D points (Note that this assumes Vector3f and FT_VECTOR3D 
    // are the same type and size, which seems to be the case)
    vertices = vector<FT_VECTOR3D>(vertexCount);

    hr = pModel->Get3DShape(
        pSUCoefs, pModel->GetSUCount(), pAUs, auCount,
        scale, rotation, translation,
        &vertices[0], vertexCount
    );
    if (FAILED(hr))
        throw ft_error("Error projecting 3D face model", hr);


    FT_TRIANGLE* pTris;
    UINT triCount;
    if (FAILED(hr = pModel->GetTriangles(&pTris, &triCount)))
        throw ft_error("Error getting model triangles", hr);

    faces = vector<FT_TRIANGLE>(pTris, pTris + triCount);

    hasModel = true;
}


void FaceModel::draw(RenderTarget& target, RenderStates states) const {
    if (hasModel) {
        //states.transform *= getTransform();
        //target.draw(vertices, vertexCount, sf::Points, states);

        glColor3f(1.f, 1.f, 1.f);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_TRIANGLES);
        for each (auto tri in faces) {
            glVertex3fv(reinterpret_cast<const GLfloat*>(&vertices[tri.i]));
            glVertex3fv(reinterpret_cast<const GLfloat*>(&vertices[tri.j]));
            glVertex3fv(reinterpret_cast<const GLfloat*>(&vertices[tri.k]));
        }
        glEnd();
    }
}