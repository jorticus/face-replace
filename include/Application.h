#pragma once

#include <opencv2\opencv.hpp>

#include <SFML\Graphics.hpp>
#include <SFML\OpenGL.hpp>

#include <OpenNI.h>

//#include <kinect\nui\Kinect.h>
//#include <kinect\nui\ImageFrame.h>

//#include "FaceTracker.h"
//#include "HeadTracker.h"

#include "utils\FPSCounter.h"
#include "utils\RunningAverage.h"


class Application
{
    // Configuration
    const std::string title = "Virtual Mirror";
    const int width = 640*2;
    const int height = 480;
    const int fullscreen = false;    // NOTE: if fullscreen is true, width/height are ignored and the native screen resolution is used instead.

    //const std::string default_font_file = "data\\TitilliumWeb-Bold.ttf";
    const std::string default_font_file = "data\\Exo-Bold.ttf";

    const int depth_threshold = 2400; //mm

public:
    std::vector<std::wstring> args;
    sf::RenderWindow *window = nullptr;

    Application(int argc, _TCHAR* argv[]);
    ~Application();

    int Main();

protected:
    void InitializeResources();
    void InitializeOpenNI();
    void InitializeWindow();

    void Capture();
    void Process();
    void Draw();

    // OpenNI device members
    openni::Device device;
    openni::VideoStream colorStream;
    openni::VideoStream depthStream;
    openni::VideoFrameRef colorFrame;
    openni::VideoFrameRef depthFrame;

    // Captured image frames
    cv::Mat colorImage;     // 8UC3 (RGB)
    cv::Mat depthImage;     // 32F, not normalized
    cv::Mat depthRaw;       // 16U
    //cv::Mat depthMask;      // Binary mask, marking valid/invalid depth pixels

    bool newFrame;
    bool colorReady;
    bool depthReady;

private:
    FPSCounter fpsCounter;
    RunningAverage<unsigned int> trackReliability;

    sf::Shader outlineShader;

    sf::Font fps_font;
    sf::Font font;

    sf::Texture wolf_tex;

    sf::Texture depthTexture;
    sf::Texture colorTexture;


    
    //void SegmentBackground();
    void TrackFace();

    std::string GetTrackingStatus();

    openni::VideoStream* oniStreams[2];


};

