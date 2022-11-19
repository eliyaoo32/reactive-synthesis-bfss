#ifndef UTILS_H
#define UTILS_H

#include <boost/program_options.hpp>
#include <iostream>
#include <spot/tl/formula.hh>
#include <string>
#include <chrono>
#include <vector>
#include <spot/twa/fwd.hh>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/postproc.hh>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>


enum Algorithm {
    UNKNOWN = 0,
    FORMULA = 1,
    AUTOMATON = 2
};

Algorithm string_to_algorithm(const std::string& str);

bool parse_cli(int argc, const char *argv[], std::string &formula,
               std::string &input, std::string &output, bool& should_verbose, Algorithm& selected_algorithm);

std::ostream &operator<<(std::ostream &out,
                         const std::vector<std::string> &vec);

using Duration = long;

class TimeMeasure {
private:
    std::chrono::steady_clock::time_point m_start;
    double m_total_duration;
public:
    TimeMeasure() : m_total_duration(-1) {}

    void start() {
        m_start = std::chrono::steady_clock::now();
    }

    Duration end() {
        m_total_duration = time_elapsed();
        return m_total_duration;
    }

    [[nodiscard]] Duration time_elapsed() const {
        auto end = std::chrono::steady_clock::now();
        return static_cast<Duration>(std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start).count());
    }

    [[nodiscard]] Duration get_duration() const {
        if(m_total_duration == -1) {
            throw std::runtime_error("TimeMeasure::get_total_duration() called before TimeMeasure::end()");
        }

        return m_total_duration;
    }
};


#endif
