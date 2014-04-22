
#include "stdafx.h"
#include "FaceTracker.h"
#include <comdef.h>


class ft_error : public std::runtime_error
{
public:
    ft_error(std::string message, HRESULT hr) : runtime_error(NULL)
    {
        std::string error_message;

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
            error_message = "Tracking failed due to inability of the Neural Network to find nose, mouth corners, and eyes";
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
            error_message = "Cannot find the head area based on the 3D hint vectors or region of interest rectangle";
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
            std::wstring com_error_message = _com_error(hr).ErrorMessage();
            error_message = std::string(com_error_message.begin(), com_error_message.end()); // wstring to string (don't care about unicode)
        }

        this->msg = message + error_message;
    }
    
    virtual const char* what() const throw() {
        return msg.c_str();
    }

private:
    std::string msg;
};


FaceTracker::FaceTracker()
{
    HRESULT hr;
    FT_CAMERA_CONFIG videoConfig;
    FT_CAMERA_CONFIG depthConfig;
    FT_CAMERA_CONFIG* pDepthConfig = NULL;

    videoConfig.Width = 640;
    videoConfig.Height = 480;
    videoConfig.FocalLength = 0;

    depthConfig.Width = 640;
    depthConfig.Height = 480;
    depthConfig.FocalLength = 0;

    pFaceTracker = FTCreateFaceTracker(NULL);
    if (pFaceTracker == nullptr)
        throw std::exception("Could not create the face tracker interface");

    //0x8fac000e
    hr = pFaceTracker->Initialize(&videoConfig, &depthConfig, NULL, NULL);
    if (FAILED(hr))
        throw ft_error("Could not initialize the face tracker: ", hr);

    //TODO: Why does this fail???
    hr = pFaceTracker->CreateFTResult(&this->pFTResult);
    if (FAILED(hr) || this->pFTResult == nullptr)
        throw ft_error("Could not initialize the face tracker result: ", hr);

    // RGB Image
    colorImage = FTCreateImage();
    if (colorImage == nullptr || FAILED(hr = colorImage->Allocate(videoConfig.Width, videoConfig.Height, FTIMAGEFORMAT_UINT8_B8G8R8X8)))
        throw std::exception("Could not allocate colour image for face tracker");

    depthImage = FTCreateImage();
    if (depthImage == nullptr || FAILED(hr = depthImage->Allocate(depthConfig.Width, depthConfig.Height, FTIMAGEFORMAT_UINT16_D13P3)))
        throw std::exception("Could not allocate depth image for face tracker");


}


FaceTracker::~FaceTracker()
{
    if (pFaceTracker != nullptr)
        pFaceTracker->Release();
}
