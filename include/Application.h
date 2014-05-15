#pragma once

#include <opencv2\opencv.hpp>

#include <SFML\Graphics.hpp>
#include <SFML\OpenGL.hpp>

#include <OpenNI.h>

#include <atomic>
#include <thread>

//#include <kinect\nui\Kinect.h>
//#include <kinect\nui\ImageFrame.h>

#include "FaceTracker.h"
//#include "HeadTracker.h"

#include "utils\FPSCounter.h"
#include "utils\RunningAverage.h"

//#include "wfm\WireframeFile.h"
#include "eru\Model.h"

#include "Capture.h"


class Application
{
    // Configuration
    const std::string title = "Virtual Mirror";
    //const int width = 640*2;
    //const int height = 480+128;
    const int fullscreen = false;    // NOTE: if fullscreen is true, width/height are ignored and the native screen resolution is used instead.

    const std::string resources_dir = "resources\\";
    const std::string capture_dir = "cap\\";

    //const std::string default_font_file = "fonts\\TitilliumWeb-Bold.ttf";
    const std::string default_font_file = "fonts\\Exo-Bold.ttf";

    const int depth_threshold = 2400; //mm

    const bool ssfx_enabled = false;    // Screen-space effects

    const bool advanced_view = false;   // If true, show depth video and other information
    const bool show_status = true;     // If true, show status information such as FPS
    const bool draw_face_wireframe = false;

public:
    std::vector<std::wstring> args;
    sf::RenderWindow *window;

    Application(int argc, _TCHAR* argv[]);
    ~Application();

    int Main();

protected:
    void InitializeResources();
    void InitializeWindow();
    void Initialize3D();

    //void Capture();
    void Process();
    void Draw();
    void DrawVideo(sf::RenderTarget* target);
    void Draw3D(sf::RenderTarget* target);
    void DrawOverlay(sf::RenderTarget* target);
    void DrawStatus(sf::RenderTarget* target);

    void OnKeyPress(sf::Event e);

    // Captured image frames
    cv::Mat colorImage;     // 8UC3 (RGB)
    cv::Mat depthImage;     // 32F, not normalized
    cv::Mat depthRaw;       // 16U

    cv::Mat faceImage;

    bool newFrame;
    bool colorReady;
    bool depthReady;

    Capture capture;

    FaceTracker faceTracker;
    //eruFace::Model faceMesh;

private:
    sf::Vector2f AnalyzeLevels(cv::Mat image);
    sf::Vector2f levelCorrection;

    sf::Vector2u initialSize;
    GLfloat aspectRatio;

    FPSCounter fpsCounter;
    RunningAverage<unsigned int> trackReliability;

    sf::Shader outlineShader;
    sf::Shader blendShader;

    sf::Font fps_font;
    sf::Font font;

    sf::Texture faceTexture;
    sf::Sprite faceSprite;

    sf::Texture depthTexture;
    sf::Texture colorTexture;

    std::string GetTrackingStatus();

    float raw_depth;

    cv::Size face_size;
    cv::Point face_offset;
    cv::Point face_center;

};

