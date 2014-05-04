#pragma once

#include <Windows.h>
#include <FaceTrackLib.h> // Part of the Microsoft Kinect Developer Toolkit

#include <vector>

class FaceModel
{
public:
    FaceModel();
    ~FaceModel();

    void Initialize(IFTFaceTracker* pFaceTracker);
    void UpdateModel(IFTResult* pFTResult, FT_CAMERA_CONFIG* pCameraConfig);

    std::vector<FT_VECTOR3D> vertices;
    std::vector<FT_VECTOR2D> uvcoords;
    std::vector<FT_TRIANGLE> faces;

    std::vector<float> actionUnits;
    std::vector<float> shapeUnits;

    void SaveToObjFile(std::string filename);
    virtual void DrawGL();

protected:
    bool                hasModel;

    IFTModel*           pModel;
    IFTFaceTracker*     pFaceTracker;
};

