#pragma once

#include <opencv2\opencv.hpp>

#include <Windows.h>
#include <FaceTrackLib.h> // Part of the Microsoft Kinect Developer Toolkit
#include "FaceTrackerBase.h"

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

    void Track(cv::Mat &colorImage, cv::Mat &depthImage);
    HRESULT GetTrackStatus() { return (pFTResult != nullptr) ? pFTResult->GetStatus() : -1; }

private:
    IFTFaceTracker* pFaceTracker = NULL;
    
    IFTResult*      pFTResult = NULL;

    IFTImage*       pColorImage = NULL;
    IFTImage*       pDepthImage = NULL;
    FT_SENSOR_DATA  sensorData;

    bool            isTracked;
    HRESULT         last_exc = S_OK;

    void            printTrackingState(std::string message, HRESULT hr);
};


