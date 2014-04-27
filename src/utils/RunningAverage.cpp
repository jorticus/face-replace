#include "utils\RunningAverage.h"


/*RunningAverage::RunningAverage()
{
}


RunningAverage::~RunningAverage()
{
}
*/

/*template<class T>
void RunningAverage<T>::AddSample(T sample) {
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
}*/