#pragma once

#include "FaceModel.h"
#include <Windows.h>
#include <FaceTrackLib.h> // Part of the Microsoft Kinect Developer Toolkit

#include <vector>

#include "eru\Model.h"
#include <SFML\Graphics.hpp>

class CustomFaceModel : public FaceModel
{
public:
    CustomFaceModel();
    ~CustomFaceModel();

    bool LoadMesh(std::string filename);

    void Initialize(IFTFaceTracker* pFaceTracker);
    void UpdateModel(IFTResult* pFTResult, FT_CAMERA_CONFIG* pCameraConfig);

    virtual void DrawGL();

    eruFace::Model      faceMesh;
    sf::Texture         texture;

private:
    bool                hasModel;

    IFTModel*           pModel;
    IFTFaceTracker*     pFaceTracker;

    FT_VECTOR2D         min, max;
};

