#ifndef REACTIVE_SYNTHESIS_BFSS_UTILS_H
#define REACTIVE_SYNTHESIS_BFSS_UTILS_H

#define TIME_MEASURE_DEFAULT_DURATION (-1)

#include <chrono>
using namespace std::chrono;

using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
using Duration = long;

class TimeMeasure {
private:
    TimePoint m_start_time;
    Duration m_duration;
public:
    TimeMeasure() : m_duration(TIME_MEASURE_DEFAULT_DURATION) {}

    void start() {
        m_start_time = high_resolution_clock::now();
    }

    Duration end() {
        TimePoint end = high_resolution_clock::now();
        m_duration = static_cast<Duration>(duration_cast<milliseconds>(end - m_start_time).count());

        return m_duration;
    }

    Duration get_duration() {
        if(m_duration == TIME_MEASURE_DEFAULT_DURATION) {
            this->end();
        }

        return m_duration;
    }
};

std::ostream& operator<<(std::ostream& out, TimeMeasure& timeMeasure) {
    out << timeMeasure.get_duration() << " ms";
    return out;
}

#endif //REACTIVE_SYNTHESIS_BFSS_UTILS_H
