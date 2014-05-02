#pragma once

#include <Windows.h>
#include <FaceTrackLib.h> // Part of the Microsoft Kinect Developer Toolkit

#include <SFML\Graphics.hpp>

#include <vector>

class FaceModel : virtual public sf::Drawable//, public sf::Transformable
{
public:
    FaceModel();
    virtual ~FaceModel();

    void Initialize(IFTFaceTracker* pFaceTracker);
    void UpdateModel(IFTResult* pFTResult, FT_CAMERA_CONFIG* pCameraConfig);

    std::vector<FT_VECTOR3D> vertices;
    std::vector<FT_TRIANGLE> faces;

protected:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    bool                hasModel;

    IFTModel*           pModel;
    IFTFaceTracker*     pFaceTracker;
};

