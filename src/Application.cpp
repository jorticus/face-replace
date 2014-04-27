//
// Virtual Mirror Demo App
// Author: Jared Sanson (jared@jared.geek.nz)
//


#include "stdafx.h"
#include "Application.h"

#include <boost\format.hpp>

using namespace std;
using namespace sf;

Application::Application(int argc, _TCHAR* argv[]) :
    fpsCounter(32),
    trackReliability(128)
{
    // Convert command-line arguments to std::vector
    for (int i = 0; i < argc; i++)
        this->args.push_back(wstring(argv[i]));
}


Application::~Application()
{
    if (this->headTracker != nullptr)
        delete this->headTracker;

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
        Event event;
        while (window->pollEvent(event)) {
            switch (event.type) {
            case Event::Closed:
                this->window->close();
                break;

            case Event::Resized:
                glViewport(0, 0, event.size.width, event.size.height);
                break;

            case Event::KeyPressed:
                if (event.key.code == Keyboard::Escape)
                    window->close();
                break;
            }
        }

        fpsCounter.BeginPeriod();

        // Drawing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Draw();
        window->display();

        fpsCounter.EndPeriod();
    }

    return 0;
}


void Application::Initialize() {
    // Load textures
    cout << "Loading textures" << endl;

    /*if (!wolf_tex.loadFromFile("wolf.png"))
        throw std::exception("Couldn't load texture");*/

    // Initialize the Kinect camera
    cout << "Initializing Kinect" << endl;
    kinect = new kinect::nui::Kinect();
    kinect->Initialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH);

    cout << "Opening kinect video streams" << endl;
    videoStream = &kinect->VideoStream();
    depthStream = &kinect->DepthStream();

    videoStream->Open(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480);
    depthStream->Open(NUI_IMAGE_TYPE_DEPTH, NUI_IMAGE_RESOLUTION_640x480);
    //depthStream->Open(NUI_IMAGE_TYPE_DEPTH, NUI_IMAGE_RESOLUTION_320x240);

//    kinect->WaitAndUpdateAll(); // Force connect
   
    cout << "Initializing Kinect Face Tracker Library" << endl;

    faceTracker = new FaceTracker();

    headTracker = new HeadTracker();


    cout << "Loading resources" << endl;

    // Load default font
    if (!font.loadFromFile(this->default_font_file))
        throw runtime_error("Could not load font \"" + default_font_file + "\"");

    // Load shaders
    if (!outlineShader.loadFromFile("data\\outline-shader.frag", Shader::Type::Fragment))
        throw runtime_error("Could not load shader \"outline-shader.frag\"");

    outlineShader.setParameter("outlineColor", Color::Black);
    outlineShader.setParameter("outlineWidth", 0.008f);

    // OpenGL-specific settings
    ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 0;

    if (this->fullscreen) {
        // Return all allowable full-screen modes, sorted from best to worst.
        auto modes = VideoMode::getFullscreenModes();

        // Use the best mode to set the window to be full-screen
        this->window = new RenderWindow(modes[0], this->title, Style::Fullscreen, settings);
    }
    else {
        // Otherwise use the specified width/height to create a windowed window
        window = new RenderWindow(VideoMode(this->width, this->height), this->title, Style::Default, settings);
    }
    this->window->setVerticalSyncEnabled(true);

    cout << "Started" << endl;
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
        // Capture

        sf::Vector2u rgbSize(imageMD.Width(), imageMD.Height());
        sf::Vector2u depthSize(depthMD.Width(), depthMD.Height());

        rgbImage = cv::Mat(rgbSize.y, rgbSize.x, CV_8UC4, imageMD.Bits());
        cv::Mat depthData = cv::Mat(depthSize.y, depthSize.x, CV_16UC2, depthMD.Bits());
        
        cv::cvtColor(rgbImage, rgbImage, cv::COLOR_RGBA2RGB); // The 4th byte is just padding, so discard it
        vector<cv::Mat> depthOut(2);
        cv::split(depthData, depthOut);     // Split the depth and player index
        depthRaw = depthOut[1];
        depthRaw.convertTo(depthImage, CV_32F);


        // Segment background
        /*for (int row = 0; row < depthImage.rows; row++) {
            uint16_t* p = reinterpret_cast<uint16_t*>(depthImage.ptr(row));
            for (int x = 0; x < depthImage.cols; x++) {
                *p = (*p < this->depth_threshold) ? *p : this->depth_threshold;
                p++;
            }
        }*/

        cv::threshold(depthImage, depthImage, 2000.0, 0.0, cv::THRESH_TOZERO_INV);


        //cv::InputArray kernel(new int[] = { 1, 2 });
        //cv::dilate(depthImage, depthImage, kernel);
        //cv::GaussianBlur(depthImage, depthImage, cv::Size(15, 15), 0, 0);

        //cv::Sobel(depthImage, depthImage, CV_32F, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
        
        

        // Display

        cv::Mat depthImageDisplay = depthImage;

        cv::normalize(depthImageDisplay, depthImageDisplay, 0.0, 255.0, cv::NORM_MINMAX, CV_8U);

        depthImageDisplay = 255 - depthImageDisplay;

        // Create a mask for depth pixels with an invalid value
        //cv::Mat depthErrorMask;
        //cv::threshold(depthImageDisplay, depthErrorMask, 254, 0, cv::THRESH_TOZERO);

        //cv::medianBlur(depthImage, depthImage, 13);

        // Map depth to JET color map, and mask any invalid pixels to 0
        cv::applyColorMap(depthImageDisplay, depthImageDisplay, cv::COLORMAP_JET);
        //cv::cvtColor(depthErrorMask, depthErrorMask, cv::COLOR_GRAY2RGB);
        //cv::bitwise_and(depthImageDisplay, ~depthErrorMask, depthImageDisplay);

        // Convert images to OpenGL textures
        cv::cvtColor(rgbImage, rgbImage, cv::COLOR_RGB2BGRA); // OpenGL texture must be in BGRA format
        rgbTexture.create(rgbImage.cols, rgbImage.rows);
        rgbTexture.update(rgbImage.data, rgbImage.cols, rgbImage.rows, 0, 0);

        //cv::cvtColor(depthImageDisplay, depthImageDisplay, cv::COLOR_GRAY2BGRA);
        cv::cvtColor(depthImageDisplay, depthImageDisplay, cv::COLOR_RGB2BGRA);
        depthTexture.create(depthImageDisplay.cols, depthImageDisplay.rows);
        depthTexture.update(depthImageDisplay.data, depthImageDisplay.cols, depthImageDisplay.rows, 0, 0);
    }
}

void Application::TrackFace() {
   faceTracker->Track(rgbImage, depthImage);

   trackReliability.AddSample(faceTracker->isTracked);
}

void Application::Draw() {

    Capture();

    // Draw rgb and depth textures to the window
    Vector2f rgbLocation(0, 0);
    Vector2f depthLocation(rgbImage.cols, (rgbImage.rows - depthImage.rows) / 2);

    Sprite rgbSprite(rgbTexture);
    Sprite depthSprite(depthTexture);

    rgbSprite.move(rgbLocation);
    depthSprite.move(depthLocation);

    window->draw(rgbSprite);
    window->draw(depthSprite);





    //TrackFace();

    //cv::MSER mser();
    cv::SimpleBlobDetector::Params params;
    params.minThreshold = 40;
    params.maxThreshold = 60;
    params.thresholdStep = 5;
    params.minArea = 100;
    params.minConvexity = 0.3;
    params.minInertiaRatio = 0.01;

    params.maxArea = 8000;
    params.maxConvexity = 10;

    params.filterByColor = false;
    params.filterByCircularity = false;

    cv::Mat img;
    cv::normalize(depthImage, img, 0.0, 255.0, cv::NORM_MINMAX, CV_8U);
    //depthImage.convertTo(img, CV_8U);


    cv::SimpleBlobDetector detector(params);
    detector.create("SimpleBlob");

    std::vector<cv::KeyPoint> keypoints;
    detector.detect(img, keypoints);

    /*for (int i = 0; i < keypoints.size(); i++) {
        auto keypoint = keypoints[i];
        CircleShape dot(keypoint.size, 6);

        dot.setFillColor(Color::Green);
        dot.move(keypoint.pt.x + depthLocation.x, keypoint.pt.y + depthLocation.y);


        
        window->draw(dot);
    }*/



    // Draw face bounds
    RECT rect = faceTracker->faceRect;
    RectangleShape face_bounds(Vector2f(rect.right - rect.left, rect.bottom - rect.top));
    face_bounds.move(rect.left, rect.top);
    face_bounds.setFillColor(Color::Transparent);
    face_bounds.setOutlineColor((faceTracker->isTracked) ? Color::Red : Color(255, 0, 0, 40));
    face_bounds.setOutlineThickness(2.5f);

    window->draw(face_bounds);


    // Draw face center
    Vector2f face_center(rect.left + (rect.right - rect.left) / 2.0f, rect.top + (rect.bottom - rect.top) / 2.0f);

    CircleShape dot(3, 8);
    dot.setFillColor(Color::Red);
    dot.move(face_center);

    window->draw(dot);


    // Calculate distance at face center
    uint16_t raw_depth = depthRaw.at<uint16_t>(cv::Point(static_cast<int>(face_center.x), static_cast<int>(face_center.y)));



    // Draw status text
    Text text_status(GetTrackingStatus(), font, 16);
    Text text_fps((boost::format("%.1f FPS") % fpsCounter.GetAverageFps()).str(), font, 16);
    Text text_track((boost::format("Reliability %.2f%%") % (trackReliability.GetAverage() * 100.0f)).str(), font, 16);
    Text text_dist((boost::format("Distance %.1fmm") % raw_depth).str(), font, 16);
    //TODO: Distance from head, for quantifying my results

    text_fps.move(8, 0);
    text_fps.setColor(Color::White);
    window->draw(text_fps, &outlineShader);

    text_track.move(100, 0);
    text_track.setColor(Color::White);
    window->draw(text_track, &outlineShader);

    text_dist.move(270, 0);
    text_dist.setColor(Color::White);
    window->draw(text_dist, &outlineShader);

    text_status.move(8, 20);
    text_status.setColor(Color::White);
    window->draw(text_status, &outlineShader);
}

string Application::GetTrackingStatus() {
    if (faceTracker != nullptr) {
        HRESULT hr = faceTracker->GetTrackStatus();

        if (FAILED(hr)) {
            return ft_error("", hr).what();
        }
        else {
            return "Tracking";
        }
    }
    else {
        return "Uninitialized";
    }
}