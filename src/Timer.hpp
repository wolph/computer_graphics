#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <vector>
#include <numeric>

using namespace std::chrono;

class Timer{
private:
    std::vector<duration<double>> measurements;
    unsigned int index = 0;
    const float displayInterval;
    time_point<high_resolution_clock> last;
    time_point<high_resolution_clock> lastDisplay;

public:
    inline Timer(const unsigned int maxItems = 10, const float displayInterval =
            0.03) :
            measurements(maxItems), displayInterval(displayInterval),
                    last(high_resolution_clock::now()){
    }
    ;

    inline duration<double> next(){
        time_point<high_resolution_clock> now(high_resolution_clock::now());
        measurements[index] = now - last;
        last = now;
        return measurements[index];
    }

    inline bool needsDisplay(){
        next();
        duration<double> diff = last - lastDisplay;
        return diff.count() > displayInterval;
    }

    double avg(){
        double total = 0.;
        double count;
        for(auto measurement: measurements){
            if(measurement.count() > 0){
                total += measurement.count();
                count++;
            }
        }
        return total / count;
    }

    time_point<high_resolution_clock> updateLastDisplay(){
        return lastDisplay = last;
    }
};

#endif
