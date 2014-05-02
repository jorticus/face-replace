
#include "FaceTracker.h"
#include "FaceModel.h"

#include <SFML\OpenGL.hpp>
#include <iostream>
#include <fstream>

const FT_VECTOR2D uvcoords[] = {
    { 0.623815f, 0.985407f },
    { 0.376034f, 0.987269f },
    { 0.191910f, 0.886327f },
    { 0.407905f, 0.646534f },
    { 0.0f, 0.0f },
    { 0.574700f, 0.280408f },
    { 0.490542f, 0.292013f },
    { 0.616170f, 0.021357f },
    { 0.491083f, 0.253926f },
    { 0.490207f, -0.012211f },
    { 0.368391f, 0.023221f },
    { 0.510297f, 1.166478f },
    { 0.148994f, 1.116832f },
    { 0.496000f, 0.819472f },
    { 0.864134f, 0.613336f },
    { 0.770591f, 0.668875f },
    { 0.580815f, 0.645234f },
    { 0.494430f, 0.657459f },
    { 0.713634f, 0.629262f },
    { 0.589405f, 0.582646f },
    { 0.770398f, 0.636840f },
    { 0.713196f, 0.565699f },
    { 0.651899f, 0.615556f },
    { 0.511043f, 0.619753f },
    { 0.572311f, 0.392803f },
    { 0.417094f, 0.393969f },
    { 0.713129f, 0.554609f },
    { 0.618267f, 0.338859f },
    { 0.772684f, 0.431003f },
    { 0.834494f, 0.587018f },
    { 0.745396f, 0.219774f },
    { 0.559327f, 0.252788f },
    { 0.490605f, 0.173855f },
    { 0.548368f, 0.347845f },
    { 0.355707f, 1.154085f },
    { 0.0f, 0.0f },
    { 0.0f, 0.0f },
    { 0.0f, 0.0f },
    { 0.0f, 0.0f },
    { 0.0f, 0.0f },
    { 0.426138f, 0.253788f },
    { 0.0f, 0.0f },
    { 0.0f, 0.0f },
    { 0.0f, 0.0f },
    { 0.860810f, 1.099735f },
    { 0.811357f, 0.881672f },
    { 0.941511f, 0.839244f },
    { 0.939339f, 0.598378f },
    { 0.072860f, 0.845771f },
    { 0.135339f, 0.618813f },
    { 0.221943f, 0.672997f },
    { 0.165204f, 0.592048f },
    { 0.221748f, 0.640963f },
    { 0.081878f, 0.514562f },
    { 0.280293f, 0.568951f },
    { 0.223006f, 0.621186f },
    { 0.280735f, 0.632515f },
    { 0.370490f, 0.340722f },
    { 0.280224f, 0.557863f },
    { 0.221588f, 0.435145f },
    { 0.112634f, 0.359395f },
    { 0.578034f, 0.322698f },
    { 0.895845f, 0.353510f },
    { 0.541321f, 0.506280f },
    { 0.628242f, 0.254052f },
    { 0.489775f, 0.343694f },
    { 0.491944f, 0.322001f },
    { 0.686214f, 0.563268f },
    { 0.745623f, 0.614130f },
    { 0.745311f, 0.562824f },
    { 0.250064f, 0.617855f },
    { -0.030491f, 0.243585f },
    { 0.686525f, 0.614575f },
    { 0.249750f, 0.566548f },
    { 0.309159f, 0.617410f },
    { 0.434446f, 0.348701f },
    { 0.407152f, 0.323982f },
    { 0.479715f, 0.619989f },
    { 0.404280f, 0.584037f },
    { 0.559049f, 0.247841f },
    { 0.425860f, 0.248842f },
    { 0.490798f, 0.247809f },
    { 0.361864f, 0.256053f },
    { 0.574602f, 0.223461f },
    { 0.409810f, 0.224699f },
    { 0.490041f, 0.209037f },
    { 0.837578f, 0.138324f },
    { 0.409910f, 0.281646f },
    { 0.659078f, 0.244540f },
    { 0.329497f, 0.247016f },
    { 0.246237f, 0.223525f },
    { 0.168288f, 0.143353f },
    { 0.448759f, 0.506976f },
    { 0.493836f, 0.528481f },
    { 0.774104f, 0.617045f },
    { 0.774049f, 0.607804f },
    { 0.222952f, 0.611945f },
    { 0.774090f, 0.565911f },
    { 0.222728f, 0.574983f },
    { 0.713494f, 0.614805f },
    { 0.222993f, 0.570052f },
    { 0.773824f, 0.570842f },
    { 0.308849f, 0.566104f },
    { 0.651843f, 0.606315f },
    { 0.342828f, 0.608637f },
    { 0.651883f, 0.564423f },
    { 0.342870f, 0.566744f },
    { 0.342887f, 0.617878f },
    { 0.280589f, 0.618057f },
    { 0.651618f, 0.569353f },
    { 0.342605f, 0.571675f },
    { 0.520856f, 0.288133f },


};

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

    min = { -1000.f, -1000.f };
    max = { 1000.f, 1000.f };
    
    for each (auto vert in vertices) {
        if (vert.x > min.x) min.x = vert.x;
        if (vert.y > min.y) min.y = vert.y;
        if (vert.x < max.x) max.x = vert.x;
        if (vert.y < max.y) max.y = vert.y;
    }

    hasModel = true;
}


void FaceModel::draw(RenderTarget& target, RenderStates states) const {
    if (hasModel) {
        //states.transform *= getTransform();
        //target.draw(vertices, vertexCount, sf::Points, states);

        glColor3f(1.f, 1.f, 1.f);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        bool auto_uv = true;


        if (auto_uv) {
            FT_VECTOR2D size = { max.x - min.x, max.y - min.y };

            glBegin(GL_TRIANGLES);
            for each (auto tri in faces) {
                glTexCoord2f((vertices[tri.i].x - min.x) / size.x, (vertices[tri.i].y - min.y) / size.y);
                glVertex3fv(reinterpret_cast<const GLfloat*>(&vertices[tri.i]));

                glTexCoord2f((vertices[tri.j].x - min.x) / size.x, (vertices[tri.j].y - min.y) / size.y);
                glVertex3fv(reinterpret_cast<const GLfloat*>(&vertices[tri.j]));

                glTexCoord2f((vertices[tri.k].x - min.x) / size.x, (vertices[tri.k].y - min.y) / size.y);
                glVertex3fv(reinterpret_cast<const GLfloat*>(&vertices[tri.k]));
            }
            glEnd();

        }
        else {
            // TODO: Find a way to load UVs that map to the correct vertices
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

        this->SaveToObjFile(std::string("face.obj"));
    }
}


void FaceModel::SaveToObjFile(std::string filename) const {
    ofstream file;
    file.open(filename);
    file << "o Kinect_Canidae" << endl;
    for each (auto vert in vertices) {
        file << "v " << vert.x << " " << vert.y << " " << vert.z << endl;
    }
    for each (auto face in faces) {
        file << "f " << face.i+1 << " " << face.j+1 << " " << face.k+1 << endl;
    }
    file.close();
}