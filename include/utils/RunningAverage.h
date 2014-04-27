#pragma once

#include <SFML\System.hpp>
#include <vector>

template<class T>
class RunningAverage
{
public:
    RunningAverage(int samples = 128) : 
        max_samples(samples), history(samples), 
        count(0), 
        idx(0),
        current(0),
        average(0) {}

    ~RunningAverage() {}

    void AddSample(T sample) {
        history[idx] = sample;

        if (++idx == max_samples)
            idx = 0;

        if (count < max_samples)
            count++;

        T sum = 0;
        //T max = INFINITY;
        //T min = -INFINITY;
        for (int i = 0; i < count; i++) {
            sum += history[i];
        }

        this->average = static_cast<double>(sum) / static_cast<double>(count);
        this->current = sample;
    }

    double GetAverage() { return average; }
    T GetCurrent() { return current; }
    //T GetMaximum() { return maximum; }
    //T GetMinimum() { return minimum; }

    int GetSampleCount() { return count; }

private:
    int             max_samples;
    int             idx;
    int             count;

    std::vector<T>  history;
    
    double          average;
    //T               minimum;
    //T               maximum;
    T               current;
};

