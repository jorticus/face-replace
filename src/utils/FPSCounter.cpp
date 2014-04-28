#include "utils\FPSCounter.h"


FPSCounter::~FPSCounter() {
}

void FPSCounter::BeginPeriod() {
    //clock.restart();
}

void FPSCounter::EndPeriod() {
    AddSample(clock.getElapsedTime().asSeconds());
    clock.restart();
}

float FPSCounter::GetAverageFps() {
    return 1.0f / GetAverageInterval();
}

float FPSCounter::GetCurrentFps() {
    return 1.0f / GetCurrentInterval();
}

float FPSCounter::GetAverageInterval() {
    if (count == 0)
        return 0.0f;

    float sum = 0.0f;
    for (int i = 0; i < count; i++)
        sum += hist[i];

    return sum / static_cast<float>(count);
}

float FPSCounter::GetCurrentInterval() {
    return hist[idx];
}

void FPSCounter::AddSample(float interval) {
    hist[idx] = interval;
    if (++idx == c_samples)
        idx = 0;

    if (count < c_samples)
        count++;
}