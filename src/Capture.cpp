#include "Capture.h"

#include <iostream>

using namespace std;
using namespace openni;

Capture::Capture():
fpsCounter(8)
{

}


Capture::~Capture()
{
    OpenNI::shutdown();

    device.close();
    depthStream.destroy();
    colorStream.destroy();
}

void Capture::Initialize() {
    // Initialize the Kinect camera
    cout << "Initializing Kinect Camera" << endl;
    openni::Status rc = openni::STATUS_OK;

    rc = OpenNI::initialize();
    if (rc != openni::STATUS_OK)
        throw runtime_error(string("Could not initialize OpenNI library: ") + string(OpenNI::getExtendedError()));

    rc = device.open(openni::ANY_DEVICE);
    if (rc != openni::STATUS_OK)
        throw runtime_error(string("Device open failed: ") + string(OpenNI::getExtendedError()));

    rc = depthStream.create(device, openni::SENSOR_DEPTH);
    if (rc != openni::STATUS_OK)
        throw runtime_error(string("Couldn't find depth stream: ") + string(OpenNI::getExtendedError()));

    rc = depthStream.start();
    if (rc != openni::STATUS_OK)
        throw runtime_error(string("Couldn't start depth stream: ") + string(OpenNI::getExtendedError()));

    rc = colorStream.create(device, openni::SENSOR_COLOR);
    if (rc != openni::STATUS_OK)
        throw runtime_error(string("Couldn't find color stream: ") + string(OpenNI::getExtendedError()));

    rc = colorStream.start();
    if (rc != openni::STATUS_OK)
        throw runtime_error(string("Couldn't start color stream: ") + string(OpenNI::getExtendedError()));

    if (!depthStream.isValid() || !colorStream.isValid())
        throw runtime_error("No valid streams");

    //device.setImageRegistrationMode(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR);
}

void Capture::Process() {
    //if (!depthStream.isValid() || !colorStream.isValid())
    //    throw runtime_error("Error reading depth/color stream");

    fpsCounter.BeginPeriod();

    int changedIndex;
    auto streams = new VideoStream*[2] {&colorStream, &depthStream};
    //auto frames = new VideoFrameRef[2];

    openni::Status rc = OpenNI::waitForAnyStream(streams, 2, &changedIndex);
    if (rc != openni::STATUS_OK)
        throw runtime_error("Could not read depth sensor");


    /*mutex.lock();
    switch (changedIndex) {
    case 0: {
                rc = colorStream.readFrame(&colorFrame);
                if (rc != openni::STATUS_OK || !colorFrame.isValid())
                    throw runtime_error("Error reading color stream");
                break;
    }
    case 1: {
                rc = depthStream.readFrame(&depthFrame);
                if (rc != openni::STATUS_OK || !depthFrame.isValid())
                    throw runtime_error("Error reading depth stream");
                break;
    }
    default:
        throw runtime_error("Invalid stream index");
    }
    mutex.unlock();*/
    
    mutex.lock();

    rc = colorStream.readFrame(&colorFrame);
    if (rc != openni::STATUS_OK || !colorFrame.isValid())
        throw runtime_error("Error reading color stream");

    rc = depthStream.readFrame(&depthFrame);
    if (rc != openni::STATUS_OK || !depthFrame.isValid())
        throw runtime_error("Error reading depth stream");

    mutex.unlock();

    fpsCounter.EndPeriod();

    
}

//void Capture::GetFrame(cv::OutputArray color, cv::OutputArray depth) {
void Capture::GetFrame(cv::Mat *color, cv::Mat *depth) {

    // Wait for capture
    //TODO: Are mutexes a good way to do this? Are there other ways?
    mutex.lock();

    if (colorFrame.isValid()) {
        *color = cv::Mat(colorFrame.getHeight(), colorFrame.getWidth(), CV_8UC3, (void*)colorFrame.getData());
    }

    if (depthFrame.isValid()) {
        *depth = cv::Mat(depthFrame.getHeight(), depthFrame.getWidth(), CV_16U, (void*)depthFrame.getData());

    }

    mutex.unlock();

}

void Capture::Run() {
    cout << "Thread started" << endl;

    while (!m_stop)
        Process();

    cout << "Thread stopped" << endl;
}
