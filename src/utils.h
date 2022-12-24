#ifndef UTILS_H
#define UTILS_H

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <iostream>
#include <spot/tl/formula.hh>
#include <spot/tl/parse.hh>
#include <spot/twa/fwd.hh>
#include <spot/twaalgos/postproc.hh>
#include <spot/twaalgos/translate.hh>
#include <string>
#include <vector>

enum Algorithm { UNKNOWN = 0, FORMULA = 1, AUTOMATON = 2 };

Algorithm string_to_algorithm(const std::string &str);

bool parse_cli(int argc, const char *argv[], std::string &formula, std::string &input,
               std::string &output, bool &should_verbose, Algorithm &selected_algorithm);

std::ostream &operator<<(std::ostream &out, const std::vector<std::string> &vec);

using Duration = long;

class TimeMeasure {
   private:
    std::chrono::steady_clock::time_point m_start;
    Duration m_total_duration;
    bool m_has_started;

   public:
    TimeMeasure() : m_total_duration(-1), m_has_started(false) {}

    void start();

    bool has_started() const { return m_has_started; }

    Duration end();

    [[nodiscard]] Duration time_elapsed() const;

    [[nodiscard]] Duration get_duration(bool validate_is_ended = true) const;
};

#endif
