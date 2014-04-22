//
// Virtual Mirror Demo App
// Author: Jared Sanson (jared@jared.geek.nz)
//


#include "stdafx.h"
#include "Application.h"

Application::Application(int argc, _TCHAR* argv[])
{
    // Convert command-line arguments to std::vector
    for (int i = 0; i < argc; i++)
        this->args.push_back(std::wstring(argv[i]));
}


Application::~Application()
{
    if (this->faceTracker != nullptr)
        delete this->faceTracker;

    if (this->kinect != nullptr)
        delete this->kinect;

    if (this->window != nullptr)
        delete this->window;
}

int Application::Main()
{
    this->Initialize();

    while (this->window->isOpen()) {

        // Handle screen events
        sf::Event event;
        while (this->window->pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                this->window->close();
                break;

            case sf::Event::Resized:
                glViewport(0, 0, event.size.width, event.size.height);
                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                    this->window->close();
                break;
            }
        }

        // Drawing
        //this->window->clear();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Draw();

        this->window->display();
    }

    return 0;
}


void Application::Initialize() {
    // Load textures
    std::cout << "Loading textures" << std::endl;

    /*if (!wolf_tex.loadFromFile("wolf.png"))
        throw std::exception("Couldn't load texture");*/

    // Initialize the Kinect camera
    std::cout << "Initializing Kinect" << std::endl;
    kinect = new kinect::nui::Kinect();
    kinect->Initialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH);

    std::cout << "Opening kinect video streams" << std::endl;
    videoStream = &kinect->VideoStream();
    depthStream = &kinect->DepthStream();

    videoStream->Open(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480);
    depthStream->Open(NUI_IMAGE_TYPE_DEPTH, NUI_IMAGE_RESOLUTION_640x480);

//    kinect->WaitAndUpdateAll(); // Force connect
   
    std::cout << "Initializing Kinect Face Tracker Library" << std::endl;

    faceTracker = new FaceTracker();


    //std::cout << "Initializing Window" << std::endl;

    // OpenGL-specific settings
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 0;

    if (this->fullscreen) {
        // Return all allowable full-screen modes, sorted from best to worst.
        auto modes = sf::VideoMode::getFullscreenModes();

        // Use the best mode to set the window to be full-screen
        this->window = new sf::RenderWindow(modes[0], this->title, sf::Style::Fullscreen, settings);
    }
    else {
        // Otherwise use the specified width/height to create a windowed window
        this->window = new sf::RenderWindow(sf::VideoMode(this->width, this->height), this->title, sf::Style::Default, settings);
    }
    this->window->setVerticalSyncEnabled(true);

    std::cout << "Started" << std::endl;
}

void Application::Capture() {

    // Capture frame from kinect
    kinect->WaitAndUpdateAll();
    kinect::nui::VideoFrame imageMD(*videoStream);
    kinect::nui::DepthFrame depthMD(*depthStream);

    // Check the kinect has an image
    // NOTE: If it's connected and you're not getting an image, make sure the powerpack is plugged in,
    //       and it's not already being used by something else.
    if (imageMD.Pitch() > 0 && depthMD.Pitch() > 0)
    {
        sf::Vector2u rgbSize(imageMD.Width(), imageMD.Height());
        sf::Vector2u depthSize(depthMD.Width(), depthMD.Height());

        rgbImage = cv::Mat(rgbSize.y, rgbSize.x, CV_8UC4, imageMD.Bits());
        cv::Mat depthData = cv::Mat(depthSize.y, depthSize.x, CV_16UC2, depthMD.Bits());

        cv::cvtColor(rgbImage, rgbImage, cv::COLOR_RGBA2RGB); // The 4th byte is just padding, so discard it

        std::vector<cv::Mat> depthOut(2);
        cv::split(depthData, depthOut);     // Split the depth and player index
        depthImage = depthOut[1];

        cv::normalize(depthImage, depthImage, 0.0, 255.0, cv::NORM_MINMAX, CV_8U);

        depthImage = 255 - depthImage;
        cv::threshold(depthImage, depthImage, 254, 0, cv::THRESH_TOZERO_INV);

        // Convert images to OpenGL textures
        cv::cvtColor(rgbImage, rgbImage, cv::COLOR_RGB2BGRA); // OpenGL texture must be in BGRA format
        rgbTexture.create(rgbImage.cols, rgbImage.rows);
        rgbTexture.update(rgbImage.data, rgbImage.cols, rgbImage.rows, 0, 0);

        cv::cvtColor(depthImage, depthImage, cv::COLOR_GRAY2BGRA);
        depthTexture.create(depthImage.cols, depthImage.rows);
        depthTexture.update(depthImage.data, depthImage.cols, depthImage.rows, 0, 0);
    }
}

void Application::TrackFace() {
    // TODO
}

void Application::Draw() {

    Capture();

    TrackFace();

    // Draw rgb and depth textures to the window
    sf::Sprite rgbSprite(rgbTexture);
    sf::Sprite depthSprite(depthTexture);

    depthSprite.move(rgbImage.cols, (rgbImage.rows - depthImage.rows) / 2);

    window->draw(rgbSprite);
    window->draw(depthSprite);

    //sf::Sprite wolf_sprite(wolf_tex);
    //window->draw(wolf_sprite);
}

