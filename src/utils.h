#ifndef UTILS_H
#define UTILS_H

#include <boost/program_options.hpp>
#include <iostream>
#include <spot/tl/formula.hh>
#include <string>
#include <utility>
#include <vector>
#include <spot/twa/fwd.hh>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/postproc.hh>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>


enum Algorithms {
    FORMULA = 1,
    AUTOMATON = 2
};

bool parse_cli(int argc, const char *argv[], std::string &formula,
               std::string &input, std::string &output, bool& should_verbose, int& algorithms);

std::ostream &operator<<(std::ostream &out,
                         const std::vector<std::string> &vec);

#endif
