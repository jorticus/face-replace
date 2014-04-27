#pragma once

#include <opencv2\opencv.hpp>

class HeadTracker
{
public:
    HeadTracker();
    ~HeadTracker();

    void Track(cv::Mat &colorImage, cv::Mat &depthImage);
};

