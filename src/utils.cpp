#include <boost/algorithm/string/join.hpp>
#include <iostream>

#include "utils.h"

Duration TimeMeasure::end() {
    TimePoint end = high_resolution_clock::now();
    m_duration = static_cast<Duration>(duration_cast<milliseconds>(end - m_start_time).count());

    return m_duration;
}

Duration TimeMeasure::get_duration() {
    if(m_duration == TIME_MEASURE_DEFAULT_DURATION) {
        this->end();
    }

    return m_duration;
}

std::ostream& operator<<(std::ostream& out, TimeMeasure& timeMeasure) {
    out << timeMeasure.get_duration() << " ms";
    return out;
}

std::ostream& operator<<(std::ostream& out, const std::vector<std::string>& vec_str) {
    out << boost::algorithm::join(vec_str, ", ");
    return out;
}