
#include "stdafx.h"
#include "FaceTracker.h"
#include <comdef.h>

#include <SFML\System.hpp>

using namespace std;

ft_error::ft_error(string message, HRESULT hr) : runtime_error(NULL)
{
    string error_message;

    switch (hr) {
    case FT_ERROR_INVALID_MODELS:
        error_message = "Face tracking models have incorrect format";
        break;
    case FT_ERROR_INVALID_INPUT_IMAGE:
        error_message = "Input image is invalid";
        break;
    case FT_ERROR_FACE_DETECTOR_FAILED:
        error_message = "Tracking failed due to face detection errors";
        break;
    case FT_ERROR_AAM_FAILED:
        error_message = "Tracking failed due to errors in tracking individual face parts";
        break;
    case FT_ERROR_NN_FAILED:
        error_message = "Tracking failed due to Neural Network failure";  //inability of the Neural Network to find nose, mouth corners, and eyes
        break;
    case FT_ERROR_UNINITIALIZED:
        error_message = "Face tracker is not initialized";
        break;
    case FT_ERROR_INVALID_MODEL_PATH:
        error_message = "Model files could not be located";
        break;
    case FT_ERROR_EVAL_FAILED:
        error_message = "Face is tracked, but the results are poor";
        break;
    case FT_ERROR_INVALID_CAMERA_CONFIG:
        error_message = "Camera configuration is invalid";
        break;
    case FT_ERROR_INVALID_3DHINT:
        error_message = "The 3D hint vectors contain invalid values (could be out of range)";
        break;
    case FT_ERROR_HEAD_SEARCH_FAILED:
        error_message = "Cannot find the head area based on the 3D hint vectors";
        break;
    case FT_ERROR_USER_LOST:
        error_message = "The user being tracked has been lost";
        break;
    case FT_ERROR_KINECT_DLL_FAILED:
        error_message = "Kinect DLL failed to load";
        break;
    case FT_ERROR_KINECT_NOT_CONNECTED:
        error_message = "Kinect sensor is not connected or is already in use";
        break;

        // Get the COM error message
    default:
        wstring com_error_message = _com_error(hr).ErrorMessage();
        error_message = string(com_error_message.begin(), com_error_message.end()); // wstring to string (don't care about unicode)
    }

    this->msg = message + error_message;
}


FaceTracker::FaceTracker()
{

}

void FaceTracker::Initialize() {
    isTracked = false;
    hasFace = false;

    faceRect = { 0, 0, 0, 0 };

    HRESULT hr;
    videoConfig = { 640, 480, 531.15f };   // TODO: Don't hard-code these values
    depthConfig = { 640, 480, 285.63f*2.0f };

    pFaceTracker = FTCreateFaceTracker(NULL);
    if (pFaceTracker == nullptr)
        throw std::exception("Could not create the face tracker interface");

    hr = pFaceTracker->Initialize(&videoConfig, &depthConfig, NULL, NULL);
    if (FAILED(hr))
        throw ft_error("Could not initialize the face tracker: ", hr);

    this->pFTResult = NULL;
    hr = pFaceTracker->CreateFTResult(&this->pFTResult);
    if (FAILED(hr) || this->pFTResult == nullptr)
        throw ft_error("Could not initialize the face tracker result: ", hr);

    // RGB Image
    pColorImage = FTCreateImage();
    if (pColorImage == nullptr || FAILED(hr = pColorImage->Allocate(videoConfig.Width, videoConfig.Height, FTIMAGEFORMAT_UINT8_B8G8R8X8)))
        throw runtime_error("Could not allocate colour image for face tracker");

    pDepthImage = FTCreateImage();
    if (pDepthImage == nullptr || FAILED(hr = pDepthImage->Allocate(depthConfig.Width, depthConfig.Height, FTIMAGEFORMAT_UINT16_D13P3)))
        throw runtime_error("Could not allocate depth image for face tracker");

    model.Initialize(pFaceTracker);

    sensorData.pVideoFrame = pColorImage;
    sensorData.pDepthFrame = pDepthImage;
    sensorData.ZoomFactor = 1.0f;
    sensorData.ViewOffset = { 0, 0 };
}

void FaceTracker::Track(cv::Mat colorImage, cv::Mat depthImage)
{
    HRESULT hr;

    FT_SENSOR_DATA sd(pColorImage, pDepthImage, 1.0f);

    cv::Mat colorimg = colorImage;
    cv::Mat depthimg = depthImage;
    cv::cvtColor(colorImage, colorimg, cv::COLOR_BGR2RGBA);

    // The library expects D13P3 format, ie. the last 3 bits are the player index. Therefore we need to <<3 (or multiply by 8) to get it to work.
    depthimg = depthimg * 8;      
    
    // Get camera frame buffer
    hr = pColorImage->Attach(colorimg.cols, colorimg.rows, colorimg.data, FTIMAGEFORMAT_UINT8_B8G8R8X8, colorimg.cols*colorimg.channels());
    if (FAILED(hr))
        throw ft_error("Error attaching color image buffer: ", hr);

    hr = pDepthImage->Attach(depthimg.cols, depthimg.rows, depthimg.data, FTIMAGEFORMAT_UINT16_D13P3, depthimg.cols*2);
    if (FAILED(hr))
        throw ft_error("Error attaching depth image buffer: ", hr);


    if (!isTracked) {
        hr = pFaceTracker->StartTracking(&sd, NULL, NULL, pFTResult);
    }
    else {
        hr = pFaceTracker->ContinueTracking(&sd, NULL, pFTResult);
    }

    //printTrackingState(hr);

    if (SUCCEEDED(hr) && SUCCEEDED(pFTResult->GetStatus())) {
        isTracked = true;
        hasFace = true;

        pFTResult->GetFaceRect(&this->faceRect);

        float scale, rotation[3], translation[3];
        pFTResult->Get3DPose(&scale, rotation, translation);

        this->scale = scale;
        this->rotation = sf::Vector3f(rotation[0], rotation[1], rotation[2]);
        this->translation = sf::Vector3f(translation[0], translation[1], translation[2]);

        // Get 3D face model
        model.UpdateModel(pFTResult, &videoConfig);

        pFTResult->GetStatus();
    }
    else {
        isTracked = false;
        pFTResult->Reset();
    }
}

void FaceTracker::printTrackingState(string message, HRESULT hr) {
    if (hr != last_exc) {
        if (FAILED(hr))
            cout << ft_error(message, hr).what() << endl;
        else
            cout << "Tracking successful" << endl;
    }
    last_exc = hr;
}

FaceTracker::~FaceTracker() {
    Uninitialize();
}

#define ReleaseAndNull(v) if (v != nullptr) {v->Release(); v=nullptr;}

void FaceTracker::Uninitialize() {
    ReleaseAndNull(pFaceTracker);
    ReleaseAndNull(pColorImage);
    ReleaseAndNull(pDepthImage);
    ReleaseAndNull(pFTResult);
}
