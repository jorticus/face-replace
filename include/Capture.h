#pragma once

#include <opencv2\core.hpp>
#include <OpenNI.h>
#include "utils\Runnable.h"

#include "utils\FPSCounter.h"

#include <mutex>

class Capture : public Runnable
{
public:
    Capture();
    ~Capture();

    void Initialize();
    void Process();

    //void GetFrame(cv::OutputArray color, cv::OutputArray depth);
    void GetFrame(cv::Mat *color, cv::Mat *depth);
    FPSCounter fpsCounter;
private:
    void Run();

    

    // OpenNI device members
    openni::Device device;
    openni::VideoStream colorStream;
    openni::VideoStream depthStream;
    openni::VideoFrameRef colorFrame;
    openni::VideoFrameRef depthFrame;

    std::mutex mutex;


};

