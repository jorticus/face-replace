#pragma once

#include <SFML\System.hpp>
#include <vector>

class FPSCounter
{
public:
    FPSCounter(int samples=128): c_samples(samples), hist(samples), count(0), idx(0) {}
    ~FPSCounter();

    void BeginPeriod();
    void EndPeriod();

    float GetAverageFps();
    float GetCurrentFps();

    float GetAverageInterval();
    float GetCurrentInterval();

private:
    int c_samples;

    sf::Clock clock;

    std::vector<float> hist;
    int idx;
    int count;

    void AddSample(float interval);
};

