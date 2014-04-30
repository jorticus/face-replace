//
// Virtual Mirror Demo App
// Author: Jared Sanson (jared@jared.geek.nz)
//


#include "stdafx.h"
#include "Application.h"

#include <boost\format.hpp>

using namespace std;
using namespace sf;
using namespace openni;

Application::Application(int argc, _TCHAR* argv[]) :
fpsCounter(32),
trackReliability(128)
{
    // Convert command-line arguments to std::vector
    for (int i = 0; i < argc; i++)
        this->args.push_back(wstring(argv[i]));

    newFrame = false;
}

Application::~Application()
{

    if (this->window != nullptr)
        delete this->window;

    if (faceTracker != nullptr)
        delete faceTracker;

    capture.Stop();
}

int Application::Main()
{
    InitializeResources();

    capture.Initialize();
    
    faceTracker = new FaceTracker();
    
    InitializeWindow();
    

    colorImage = cv::Mat(480, 640, CV_8UC3);
    depthImage = cv::Mat(480, 640, CV_32F);
    depthRaw = cv::Mat(480, 640, CV_16U);

    capture.Start();

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
                OnKeyPress(event);
                break;
            }
        }

        //fpsCounter.BeginPeriod();

        // Drawing loop
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Draw();
        window->display();

        fpsCounter.EndPeriod();
        fpsCounter.BeginPeriod();
    }

    return 0;
}


void Application::OnKeyPress(sf::Event e) {
    switch (e.key.code) {
    case Keyboard::Escape:
        window->close();
        break;

    case Keyboard::F12:
        // Save current color/depth streams to disk
        cv::Mat colorTemp;
        cv::cvtColor(colorImage, colorTemp, cv::COLOR_BGR2RGB);
        cv::imwrite("cap\\color.png", colorTemp);

        cv::Mat depthTemp;
        cv::normalize(depthImage, depthTemp, 0.0, 255.0, cv::NORM_MINMAX, CV_8U);
        cv::applyColorMap(depthTemp, depthTemp, cv::COLORMAP_JET);
        cv::imwrite("cap\\depthraw.png", depthRaw);
        cv::imwrite("cap\\depth.png", depthTemp);

        // Capture screenshot of window
        sf::Image screenshot = window->capture();
        screenshot.saveToFile("cap\\screenshot.png");
    }
}


void Application::InitializeResources() {
    cout << "Loading resources" << endl;

    // Load default font
    if (!font.loadFromFile(this->default_font_file))
        throw runtime_error("Could not load font \"" + default_font_file + "\"");

    // Load shaders
    if (!outlineShader.loadFromFile("data\\outline-shader.frag", Shader::Type::Fragment))
        throw runtime_error("Could not load shader \"outline-shader.frag\"");

    outlineShader.setParameter("outlineColor", Color::Black);
    outlineShader.setParameter("outlineWidth", 0.008f);
}


void Application::InitializeWindow() {
    // OpenGL-specific settings
    ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 0;

    if (this->fullscreen) {
        // Return all allowable full-screen modes, sorted from best to worst.
        auto modes = sf::VideoMode::getFullscreenModes();

        // Use the best mode to set the window to be full-screen
        this->window = new RenderWindow(modes[0], this->title, Style::Fullscreen, settings);
    }
    else {
        // Otherwise use the specified width/height to create a windowed window
        window = new RenderWindow(sf::VideoMode(this->width, this->height), this->title, Style::Default, settings);
    }
    this->window->setVerticalSyncEnabled(true);

    cout << "Started" << endl;
}

void cvApplyAlpha(cv::Mat rgb_in, cv::Mat alpha_in, cv::Mat &rgba_out) {
    cv::cvtColor(rgb_in, rgba_out, cv::COLOR_RGB2RGBA);

    int from_to[] = { 0,0, 1,1, 2,2, 3,3 };
    cv::Mat in[] = { rgb_in, alpha_in };
    cv::Mat out[] = { rgba_out };
    cv::mixChannels(in, 2, out, 1, from_to, 4);
}

void Application::Process() {

    bool removeBackground = false;


    // Segment background
    cv::Mat mask_segment;
    cv::Mat mask_valid;
    cv::threshold(depthImage, mask_segment, this->depth_threshold, 255.0, cv::THRESH_BINARY_INV);
    cv::threshold(depthImage, mask_valid, 1, 255.0, cv::THRESH_BINARY);

    cv::Mat depth_mask;
    cv::bitwise_and(mask_segment, mask_valid, depth_mask);
    depth_mask.convertTo(depth_mask, CV_8U);

    cv::Mat kernel(3, 3, CV_8U, cv::Scalar(1));
    cv::morphologyEx(depth_mask, depth_mask, cv::MORPH_OPEN, kernel);


    //cv::InputArray kernel(new int[] = { 1, 2 });
    //cv::dilate(depthImage, depthImage, kernel);
    //cv::GaussianBlur(depthImage, depthImage, cv::Size(15, 15), 0, 0);

    //cv::Sobel(depthImage, depthImage, CV_32F, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);


    // Display

    cv::Mat depthImageDisplay = depthImage;

    cv::normalize(depthImageDisplay, depthImageDisplay, 0.0, 255.0, cv::NORM_MINMAX, CV_8U);

    //if (removeBackground)
        //cv::bitwise_and(depthImageDisplay, depthImageDisplay, depthImageDisplay);
        //cv::threshold(depthImageDisplay, depthImageDisplay, this->depth_threshold, 0.0, cv::THRESH_TOZERO_INV);

    //cv::normalize(depthImageDisplay, depthImageDisplay, 0.0, 255.0, cv::NORM_MINMAX, CV_8U);

    //depthImageDisplay = 255 - depthImageDisplay;

    // Create a mask for depth pixels with an invalid value
    //cv::Mat depthErrorMask;
    //cv::threshold(depthImageDisplay, depthErrorMask, 254, 0, cv::THRESH_TOZERO);

    //cv::medianBlur(depthImage, depthImage, 13);

    // Map depth to JET color ma6p, and mask any invalid pixels to 0
    cv::applyColorMap(depthImageDisplay, depthImageDisplay, cv::COLORMAP_JET);
    //cv::cvtColor(depthErrorMask, depthErrorMask, cv::COLOR_GRAY2RGB);
    //cv::bitwise_and(depthImageDisplay, ~depthErrorMask, depthImageDisplay);

    // Convert images to OpenGL textures
    cv::Mat image1;
    cv::cvtColor(colorImage, image1, cv::COLOR_BGR2BGRA); // OpenGL texture must be in BGRA format

    if (removeBackground)
        cvApplyAlpha(colorImage, depth_mask, image1);

    colorTexture.create(image1.cols, image1.rows);
    colorTexture.update(image1.data, image1.cols, image1.rows, 0, 0);

    cv::Mat image2;
    //cv::cvtColor(depthImageDisplay, depthImageDisplay, cv::COLOR_GRAY2BGRA);
    cv::cvtColor(depthImageDisplay, image2, cv::COLOR_RGB2BGRA);
    
    if (removeBackground)
        cvApplyAlpha(depthImageDisplay, depth_mask, image2);

    depthTexture.create(image2.cols, image2.rows);
    depthTexture.update(image2.data, image2.cols, image2.rows, 0, 0);


}

void Application::TrackFace() {
   //faceTracker->Track(rgbImage, depthImage);

   //trackReliability.AddSample(faceTracker->isTracked);
}

void Application::Draw() {
    float raw_depth = NAN;

    window->clear(Color::White);

    depthReady = false;
    colorReady = false;

    //while (/*!depthReady && */!colorReady)
        //Capture();

    capture.GetFrame(&colorImage, &depthRaw);
    depthRaw.convertTo(depthImage, CV_32F);  // Most OpenCV functions only support 8U or 32F

    faceTracker->Track(colorImage, depthRaw);

    Process();

    // Draw rgb and depth textures to the window
    Vector2f rgbLocation(0, 0);
    Vector2f depthLocation(colorImage.cols, (colorImage.rows - depthImage.rows) / 2);

    Sprite rgbSprite(colorTexture);
    Sprite depthSprite(depthTexture);

    rgbSprite.move(rgbLocation);
    depthSprite.move(depthLocation);

    window->draw(rgbSprite);
    window->draw(depthSprite);

    //raw_depth = depthImage.at<float>(cv::Point(static_cast<int>(100), static_cast<int>(100)));

    

    /*

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

    for (int i = 0; i < keypoints.size(); i++) {
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


    if (faceTracker->isTracked) {
        cout << 
            faceTracker->translation.x << ", " <<
            faceTracker->translation.y << ", " <<
            faceTracker->translation.z << endl;
    }



    // Draw status text
    boost::format fps_fmt("%.1f (%.1f) FPS");
    fps_fmt % fpsCounter.GetAverageFps();
    fps_fmt % capture.fpsCounter.GetAverageFps();
    //Text text_fps((boost::format("%.1f FPS") % fpsCounter.GetAverageFps()).str(), font, 16);
    Text text_fps(fps_fmt.str(), font, 16);
    text_fps.move(colorImage.cols - text_fps.getLocalBounds().width, 0);
    text_fps.setColor(Color::White);
    window->draw(text_fps, &outlineShader);

    Text text_status(GetTrackingStatus(), font, 16);
    text_status.move(8, 20);
    text_status.setColor(Color::White);
    window->draw(text_status, &outlineShader);

    Text text_track((boost::format("Reliability %.2f%%") % (trackReliability.GetAverage() * 100.0f)).str(), font, 16);
    text_track.move(8, 0);
    text_track.setColor(Color::White);
    window->draw(text_track, &outlineShader);

    Text text_dist(
        (!isnan(raw_depth)) ? (boost::format("Distance %.1fmm") % raw_depth).str() : "Distance --", 
        font, 16);
    text_dist.move(270, 0);
    text_dist.setColor(Color::White);
    window->draw(text_dist, &outlineShader);
}

string Application::GetTrackingStatus() {
    if (faceTracker != nullptr) {
        if (faceTracker->isTracked) {
            HRESULT hr = faceTracker->GetTrackStatus();

            if (FAILED(hr)) {
                return ft_error("", hr).what();
            }
            else {
                return "Tracking";
            }
        }
        else {
            return "No Face Detected";
        }
    }
    else {
        return "Uninitialized";
    }
}