#pragma once

#include "FaceModel.h"
#include <Windows.h>
#include <FaceTrackLib.h> // Part of the Microsoft Kinect Developer Toolkit

#include <vector>

class CustomFaceModel : public FaceModel
{
public:
    CustomFaceModel();
    ~CustomFaceModel();

    void Initialize(IFTFaceTracker* pFaceTracker);
    void UpdateModel(IFTResult* pFTResult, FT_CAMERA_CONFIG* pCameraConfig);

    //virtual void DrawGL();

private:
    bool                hasModel;

    IFTModel*           pModel;
    IFTFaceTracker*     pFaceTracker;

    FT_VECTOR2D         min, max;
};

