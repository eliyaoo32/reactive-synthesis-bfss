#ifndef REACTIVE_SYNTHESIS_BFSS_UTILS_H
#define REACTIVE_SYNTHESIS_BFSS_UTILS_H

#define TIME_MEASURE_DEFAULT_DURATION (-1)

#include <chrono>
#include <vector>
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

    Duration end();

    Duration get_duration();
};

std::ostream& operator<<(std::ostream& out, TimeMeasure& timeMeasure);

std::ostream& operator<<(std::ostream& out, const std::vector<std::string>& vec_str);

extern std::ostream null_ostream;

#endif //REACTIVE_SYNTHESIS_BFSS_UTILS_H
