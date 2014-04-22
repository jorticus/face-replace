#pragma once

#include <Windows.h>
#include <FaceTrackLib.h> // Part of the Microsoft Kinect Developer Toolkit

class FaceTracker
{
public:
    FaceTracker();
    ~FaceTracker();

private:
    IFTFaceTracker* pFaceTracker;
    
    IFTResult*      pFTResult;

    IFTImage*       colorImage;
    IFTImage*       depthImage;


};


