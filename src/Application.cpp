//
// Virtual Mirror Demo App
// Author: Jared Sanson (jared@jared.geek.nz)
//


#include "stdafx.h"
#include "Application.h"

#include <boost\format.hpp>

#include <NuiApi.h>

using namespace std;
using namespace sf;
using namespace openni;

/*class ScreenspaceShader : public sf::Drawable {
public:
    ScreenspaceShader(sf::Shader shader): m_shader(shader){}
    virtual ~ScreenspaceShader() {}

private:
    sf::Shader m_shader;
};*/

Application::Application(int argc, _TCHAR* argv[]) :
fpsCounter(8),
trackReliability(128),
window(nullptr),
levelCorrection(0.0, 1.0)
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

    faceTracker.Uninitialize();

    capture.Stop();
}

void Application::InitializeResources() {
    cout << "Loading resources" << endl;

    // Load default font
    if (!font.loadFromFile(resources_dir + this->default_font_file))
        throw runtime_error("Could not load font \"" + default_font_file + "\"");


    // Load shaders
    if (!outlineShader.loadFromFile(resources_dir + "shaders\\outline-shader.frag", Shader::Type::Fragment))
        throw runtime_error("Could not load shader \"outline-shader.frag\"");

    outlineShader.setParameter("outlineColor", Color::Black);
    outlineShader.setParameter("outlineWidth", 0.008f);

    if (!blendShader.loadFromFile(resources_dir + "shaders\\face-blend.frag", Shader::Type::Fragment))
        throw runtime_error("Could not laod shader \"face-blend.frag\"");

    
    cout << "Loading face model" << endl;
    faceTexture.loadFromFile(resources_dir + "faces\\gaben.png");
    //faceTexture.setSmooth(true);
    faceSprite.setTexture(faceTexture);

    if (!faceTracker.model.LoadMesh(resources_dir + "faces\\candide3_textured.wfm"))
        throw runtime_error("Error loading mesh 'candide3_textured.wfm'");
    
    // You can use this to save the candide model as a VRML mesh file
    //if (!faceMesh.write("candide3.wrl"))
    //    throw runtime_error("Error writing mesh");
}

void Application::InitializeWindow() {
    // OpenGL-specific settings
    ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 8;
    settings.majorVersion = 3;
    settings.minorVersion = 0;

    if (this->fullscreen) {
        // Return all allowable full-screen modes, sorted from best to worst.
        auto modes = sf::VideoMode::getFullscreenModes();

        // Use the best mode to set the window to be full-screen
        this->window = new RenderWindow(modes[0], this->title, Style::Fullscreen, settings);
    }
    else {
        int width = (advanced_view) ? 1280.f : 640.f;
        int height = (advanced_view) ? (480.f + 128.f) : 480.f;

        // Otherwise use the specified width/height to create a windowed window
        window = new RenderWindow(sf::VideoMode(width, height), this->title, Style::Default, settings);
    }

    window->setVerticalSyncEnabled(true);
    window->setActive(true);

    initialSize = window->getSize();

    cout << "Started" << endl;
}

void Application::Initialize3D() {
    // Enable Z-buffer
    //glEnable(GL_DEPTH_TEST);
    //glDepthMask(GL_TRUE);
    //glClearDepth(1.0f);


    // Configure viewport (clipped to within the color video stream)
    if (advanced_view) {
        Vector2f scale(
            static_cast<float>(window->getSize().x) / static_cast<float>(initialSize.x),
            static_cast<float>(window->getSize().y) / static_cast<float>(initialSize.y)
            );

        glViewport(
            0, static_cast<GLint>(scale.y * (initialSize.y - 480.0f)),
            static_cast<GLsizei>(scale.x * 640.0f), static_cast<GLsizei>(scale.y * 480.0f));
    }
    else {
        glViewport(0.f, 0.f, window->getSize().x, window->getSize().y);
    }

    //aspectRatio = static_cast<float>(window->getSize().x) / window->getSize().y;
    aspectRatio = 640.0f / 480.0f;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-aspectRatio, aspectRatio, -1.f, 1.f, 1.f, 500.f);
}

int Application::Main()
{
    InitializeResources();

    capture.Initialize();
    faceTracker.Initialize();
    
    InitializeWindow();
    
    Initialize3D();

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
                //glViewport(0, 0, event.size.width, event.size.height);
                Initialize3D();
                break;

            case Event::KeyPressed:
                OnKeyPress(event);
                break;
            }
        }

        fpsCounter.BeginPeriod();

        // Drawing loop
        Draw();

        fpsCounter.EndPeriod();
    }

    return 0;
}

void Application::OnKeyPress(sf::Event e) {
    switch (e.key.code) {
    case Keyboard::Escape:
        window->close();
        break;

    case Keyboard::F11:
        // Save current color/depth streams to disk
        cv::Mat colorTemp;
        cv::cvtColor(colorImage, colorTemp, cv::COLOR_BGR2RGB);
        cv::imwrite(capture_dir + "color.png", colorTemp);

        cv::Mat depthTemp;
        cv::normalize(depthImage, depthTemp, 0.0, 255.0, cv::NORM_MINMAX, CV_8U);
        cv::applyColorMap(depthTemp, depthTemp, cv::COLORMAP_JET);
        cv::imwrite(capture_dir + "depthraw.png", depthRaw);
        cv::imwrite(capture_dir + "depth.png", depthTemp);

        // Capture screenshot of window
        sf::Image screenshot = window->capture();
        screenshot.saveToFile(capture_dir + "screenshot.png");
    }
}

void cvApplyAlpha(cv::Mat rgb_in, cv::Mat alpha_in, cv::Mat &rgba_out) {
    cv::cvtColor(rgb_in, rgba_out, cv::COLOR_RGB2RGBA);

    int from_to[] = { 0,0, 1,1, 2,2, 3,3 };
    cv::Mat in[] = { rgb_in, alpha_in };
    cv::Mat out[] = { rgba_out };
    cv::mixChannels(in, 2, out, 1, from_to, 4);
}

void Application::Draw() {
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    window->clear(Color::White);

    // Retrieve captured video color/depth frames
    capture.GetFrame(&colorImage, &depthRaw);
    depthRaw.convertTo(depthImage, CV_32F);  // Most OpenCV functions only support 8U or 32F

    // Try track the face in the current frame
    faceTracker.Track(colorImage, depthRaw);

    // Custom processing on frame
    Process();

    // If screen-space shaders are enabled, create a render texture target to draw to.
    RenderTexture tex;
    if (ssfx_enabled) {
        //TODO: Instead of creating a texture every frame, only create when initialized & resized
        tex.create(window->getSize().x, window->getSize().y, true);
        tex.clear(Color::White);
        tex.setView(window->getView());

        // Set up OpenGL settings for the current texture
        Initialize3D();
    }
    RenderTarget* target = (ssfx_enabled) ? reinterpret_cast<RenderTarget*>(&tex) : window;

    // Draw video stream
    target->pushGLStates();
    DrawVideo(target);

    // Draw 2D overlays
    DrawOverlay(target);

    // Draw 3D geometry
    target->popGLStates();
    Draw3D(target);
    target->pushGLStates();


    
    // Draw the captured screen texture with any applied shaders
    if (ssfx_enabled) {
        tex.display();  // Required

        // Apply screen-space shaders here if required

        //pixelateShader.setParameter("pixel_threshold", 0.001f);
        //window->draw(sf::Sprite(tex.getTexture()), &pixelateShader);
        window->draw(sf::Sprite(tex.getTexture()));
    }

    // Draw status information on top (not affected by the shader)
    if (show_status) {
        DrawStatus(window);
    }

    target->popGLStates();

    // Finally update the screen
    window->display();
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

    //cv::Mat kernel(3, 3, CV_8U, cv::Scalar(1));
    //cv::morphologyEx(depth_mask, depth_mask, cv::MORPH_OPEN, kernel);


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


    // Get face bounds
    // NOTE: rect is guaranteed to be within image bounds
    RECT rect = faceTracker.faceRect;
    face_size = cv::Size(rect.right - rect.left, rect.bottom - rect.top);
    face_offset = cv::Point(rect.left, rect.top);
    face_center = cv::Point(face_offset.x + face_size.width / 2, face_offset.y + face_size.height / 2);
}

void Application::DrawVideo(RenderTarget* target) {
    // Draw rgb texture to the window
    Vector2f rgbLocation(0, 0);
    Sprite rgbSprite(colorTexture);
    rgbSprite.move(rgbLocation);
    target->draw(rgbSprite);

    // In advanced view, draw the depth stream too
    if (advanced_view) {
        Vector2f depthLocation(
            static_cast<float>(colorImage.cols),
            static_cast<float>((colorImage.rows - depthImage.rows) / 2));
        Sprite depthSprite(depthTexture);
        depthSprite.move(depthLocation);
        target->draw(depthSprite);
    }
}

void Application::Draw3D(RenderTarget* target) {
    //glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    blendShader.setParameter("iResolution", sf::Vector2f(window->getSize()));

    //// Draw XYZ marker ////

    /*glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-aspectRatio, aspectRatio, -1.f, 1.f, 1.f, 500.f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.f, 0.f, -10.0f);
    glRotatef(faceTracker.rotation.x, -1.f,  0.f,  0.f);
    glRotatef(faceTracker.rotation.y,  0.f, -1.f,  0.f);
    glRotatef(faceTracker.rotation.z,  0.f,  0.f,  1.f);
    
    glBegin(GL_LINES);
        glColor3f(1.f, 0.f, 0.f);
        glVertex3f(0.f, 0.f, 0.f);
        glVertex3f(1.f, 0.f, 0.f);

        glColor3f(0.f, 1.f, 0.f);
        glVertex3f(0.f, 0.f, 0.f);
        glVertex3f(0.f, 1.f, 0.f);

        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(0.f, 0.f, 0.f);
        glVertex3f(0.f, 0.f, 1.f);
    glEnd();*/
    

    //// Draw face mesh ////

    if (faceTracker.isTracked) {
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // Set up perspective projection
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        // Set up the correct perspective projection matrix for the kinect
        gluPerspective(NUI_CAMERA_COLOR_NOMINAL_VERTICAL_FOV, 4.f / 3.f, 0.1f, 10.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(
            0.f, 0.f, 0.f,
            0.f, 0.f, 1.f,
            0.f, 1.f, 0.f);
        glScalef(-1.f, 1.f, 1.f);

        glTranslatef(faceTracker.translation.x, faceTracker.translation.y, faceTracker.translation.z);

        glRotatef(faceTracker.rotation.x, 1.f, 0.f, 0.f);
        glRotatef(faceTracker.rotation.y, 0.f, 1.f, 0.f);
        glRotatef(faceTracker.rotation.z, 0.f, 0.f, 1.f);

        // Draw textured face
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor3f(1.f, 1.f, 1.f);

        // Capture face texture and analyze luminance levels
        if (face_size.width > 0 && face_size.height > 0) {
            faceImage = colorImage(cv::Rect(face_offset, face_size)).clone();
            levelCorrection = AnalyzeLevels(faceImage);

            //TODO: Limit histogram analysis to face-coloured pixels
        }

        blendShader.setParameter("overlayTexture", faceTexture);
        blendShader.setParameter("backgroundTexture", colorTexture);
        blendShader.setParameter("lumaCorrect", levelCorrection);
       
        //sf::Texture::bind(&faceTexture);
        sf::Shader::bind(&blendShader);

        faceTracker.model.DrawGL();

        sf::Texture::bind(NULL);
        sf::Shader::bind(NULL);

        // Draw wireframe face mesh
        if (draw_face_wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_TEXTURE_2D);
            glColor3f(1.f, 1.f, 1.f);
            faceTracker.model.DrawGL();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
}

void Application::DrawOverlay(RenderTarget* target) {
    raw_depth = NAN;

    // Draw face bounds
    /*RectangleShape face_bounds(Vector2f((float)face_size.width, (float)face_size.height));
    face_bounds.move((float)face_offset.x, (float)face_offset.y);
    face_bounds.setFillColor(Color::Transparent);
    face_bounds.setOutlineColor((faceTracker.isTracked) ? Color::Red : Color(255, 0, 0, 40));
    face_bounds.setOutlineThickness(2.5f);

    window->draw(face_bounds);*/

    // Draw face center
    /*CircleShape dot(3, 8);
    dot.setFillColor(Color::Red);
    dot.move((float)face_center.x, (float)face_center.y);

    target->draw(dot);*/

    if (faceTracker.isTracked) {

        // Calculate distance at face center
        raw_depth = depthImage.at<float>(face_center.y, face_center.x);

        // Capture face texture
        /*if (face_size.width > 0 && face_size.height > 0) {
            faceImage = colorImage(cv::Rect(face_offset, face_size)).clone();

            Texture boxedFaceTexture;
            cv::cvtColor(faceImage, faceImage, cv::COLOR_BGR2BGRA);
            boxedFaceTexture.create(faceImage.cols, faceImage.rows);
            boxedFaceTexture.update(faceImage.data, faceImage.cols, faceImage.rows, 0, 0);

            // Draw captured face
            Sprite boxedFaceSprite(boxedFaceTexture);
            boxedFaceSprite.move(8.f, 480.f + 64.f - static_cast<float>(face_size.height) / 2.f);
            target->draw(boxedFaceSprite);
        }*/

        //cout <<
        //    faceTracker->translation.x << ", " <<
        //    faceTracker->translation.y << ", " <<
        //    faceTracker->translation.z << endl;

        //cout <<
        //    faceTracker.rotation.x << ", " <<
        //    faceTracker.rotation.y << ", " <<
        //    faceTracker.rotation.z << endl;


        // Update face overlay
        /*auto src_size = faceTexture.getSize();
        auto dest_size = sf::Vector2f((float)face_size.width, (float)face_size.height);
        sf::Vector2f face_scale(dest_size.x / src_size.x, dest_size.y / src_size.y);
        faceSprite.setScale(face_scale);
        faceSprite.setPosition((float)face_offset.x, (float)face_offset.y);

        target->draw(faceSprite);*/
    }
}

void Application::DrawStatus(RenderTarget* target) {
    // Draw status text
    boost::format fps_fmt("%.1f (%.1f) FPS");
    fps_fmt % fpsCounter.GetAverageFps();
    fps_fmt % capture.fpsCounter.GetAverageFps();
    //Text text_fps((boost::format("%.1f FPS") % fpsCounter.GetAverageFps()).str(), font, 16);
    Text text_fps(fps_fmt.str(), font, 16);
    text_fps.move(colorImage.cols - text_fps.getLocalBounds().width, 0);
    text_fps.setColor(Color::White);
    target->draw(text_fps, &outlineShader);

    Text text_status(GetTrackingStatus(), font, 16);
    text_status.move(8, 20);
    text_status.setColor(Color::White);
    target->draw(text_status, &outlineShader);

    /*Text text_track((boost::format("Reliability %.2f%%") % (trackReliability.GetAverage() * 100.0f)).str(), font, 16);
    text_track.move(8, 0);
    text_track.setColor(Color::White);
    window->draw(text_track, &outlineShader);*/

    Text text_dist(
        (!isnan(raw_depth)) ? (boost::format("Distance %.1fmm") % raw_depth).str() : "Distance --", 
        font, 16);
    text_dist.move(8, 0);
    text_dist.setColor(Color::White);
    target->draw(text_dist, &outlineShader);
}

string Application::GetTrackingStatus() {
    if (faceTracker.isTracked) {
        HRESULT hr = faceTracker.GetTrackStatus();

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

Vector2f Application::AnalyzeLevels(cv::Mat image) {
    // Convert to luminance. Do not use HSB/HSV, as B/V doesn't correspond to actual luminance!
    // Y' = 0.299*R + 0.587*G + 0.144*B
    cv::cvtColor(faceImage, faceImage, cv::COLOR_BGR2GRAY);

    // Calculate luminance histogram
    cv::MatND hist;
    int histSize = 256;
    float range[] = { 0, 255 };
    const float* ranges = { range };
    cv::calcHist(&faceImage, 1, 0, cv::Mat(), hist, 1, &histSize, &ranges, true, false);

    // Calculate total sum of pixel counts
    double sum = 0.0;
    for (int i = 0; i < histSize; i++) {
        sum += hist.at<float>(i);
    }

    // Find the maximum and minimum luminance of the video
    double csum = 0.0;
    double sum_a = sum * 0.01;  // 1% and 99% thresholds are used to find the minimum/maximum luminance,
    double sum_b = sum * 0.99;  // while providing some allowance for a few completely white/black pixels (which don't really contribute to the min/max brightness).
    int p_a = 0;
    int p_b = histSize - 1;
    for (int i = 0; i < histSize; i++) {
        csum += hist.at<float>(i);
        if (csum < sum_a)
            p_a = i;
        else if (csum < sum_b)
            p_b = i;
    }
    // 0 <= p_a < p_b <= 255 guaranteed.

    // Convert to values suitable for the shader
    // NOTE: Shader assumes the provided overlay face texture is normalized
    double l_a = 0.0;
    double l_b = 1.0;
    l_a = (static_cast<float>(p_a) / 255.0);
    l_b = 1.0 / (static_cast<float>(p_b) / 255.0);

    return Vector2f(l_a, l_b);
}