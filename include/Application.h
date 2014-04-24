#pragma once

#include <opencv2\opencv.hpp>

#include <SFML\Graphics.hpp>
#include <SFML\OpenGL.hpp>

#include <kinect\nui\Kinect.h>
#include <kinect\nui\ImageFrame.h>

#include "FaceTracker.h"
#include "utils\FPSCounter.h"


class Application
{
    // Configuration
    const std::string title = "Virtual Mirror";
    const int width = 640*2;
    const int height = 480;
    const int fullscreen = false;    // NOTE: if fullscreen is true, width/height are ignored and the native screen resolution is used instead.

    //const std::string default_font_file = "data\\TitilliumWeb-Bold.ttf";
    const std::string default_font_file = "data\\Exo-Bold.ttf";

public:
    std::vector<std::wstring> args;
    sf::RenderWindow *window = nullptr;

    kinect::nui::Kinect *kinect = nullptr;
    kinect::nui::ImageStream *videoStream;
    kinect::nui::ImageStream *depthStream;

    FaceTracker* faceTracker = nullptr;

    Application(int argc, _TCHAR* argv[]);
    ~Application();

    int Main();

protected:
    void Initialize();
    void Draw();

private:
    FPSCounter fpsCounter;

    sf::Font fps_font;
    sf::Font font;

    sf::Texture wolf_tex;

    sf::Texture depthTexture;
    sf::Texture rgbTexture;

    cv::Mat rgbImage;
    cv::Mat depthImage;

    void Capture();
    void TrackFace();

    std::string GetTrackingStatus();

    sf::Shader outlineShader;
    
};

