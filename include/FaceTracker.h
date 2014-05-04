#pragma once

#include <opencv2\opencv.hpp>

#include <Windows.h>
#include <FaceTrackLib.h> // Part of the Microsoft Kinect Developer Toolkit
#include "models\FaceModel.h"
#include "models\CustomFaceModel.h"

#include <SFML\Graphics.hpp>

class ft_error : public std::runtime_error
{
public:
    ft_error(std::string message, HRESULT hr);

    virtual const char* what() const throw() {
        return msg.c_str();
    }

private:
    std::string msg;
};

class FaceTracker
{
public:
    FaceTracker();
    ~FaceTracker();

    void Initialize();
    void Uninitialize();

    void Track(cv::Mat colorImage, cv::Mat depthImage);
    HRESULT GetTrackStatus() { return (pFTResult != nullptr) ? pFTResult->GetStatus() : -1; }

    // Read-only!!
    bool            isTracked;
    bool            hasFace;
    RECT            faceRect;

    float           scale;
    sf::Vector3f    rotation;
    sf::Vector3f    translation;

    CustomFaceModel model;
    //FaceModel       model;

private:
    FT_CAMERA_CONFIG videoConfig, depthConfig;

    IFTFaceTracker* pFaceTracker = NULL;
    
    IFTResult*      pFTResult = NULL;
    IFTModel*       pModel = NULL;

    IFTImage*       pColorImage = NULL;
    IFTImage*       pDepthImage = NULL;
    FT_SENSOR_DATA  sensorData;

    
    HRESULT         last_exc = S_OK;

    void            printTrackingState(std::string message, HRESULT hr);
};


