#pragma once

#include <opencv2\opencv.hpp>

#include <SFML\Graphics.hpp>
#include <SFML\OpenGL.hpp>

#include <kinect\nui\Kinect.h>
#include <kinect\nui\ImageFrame.h>


class Application
{
    // Configuration
    const std::string title = "Virtual Mirror";
    const int width = 640*2;
    const int height = 480;
    const int fullscreen = false;    // NOTE: if fullscreen is true, width/height are ignored and the native screen resolution is used instead.

public:
    std::vector<std::wstring> args;
    sf::RenderWindow *window;

    kinect::nui::Kinect *kinect;
    kinect::nui::ImageStream *videoStream;
    kinect::nui::ImageStream *depthStream;

    Application(int argc, _TCHAR* argv[]);
    ~Application();

    int Main();

protected:
    void Initialize();
    void Draw();

private:
    sf::Texture wolf_tex;
    
};

