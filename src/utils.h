#ifndef UTILS_H
#define UTILS_H

#include <boost/program_options.hpp>
#include <iostream>
#include <spot/tl/formula.hh>
#include <string>
#include <vector>
#include <spot/twa/fwd.hh>

bool parse_cli(int argc, const char *argv[], std::string &formula,
               std::vector<std::string> &input_vars,
               std::vector<std::string> &output_vars, bool &verbose_output);

struct ReactiveSyntInstance {
    std::vector<std::string> input_vars;
    std::vector<std::string> output_vars;
    spot::formula formula;
    spot::twa_graph_ptr automaton;
};

std::ostream &operator<<(std::ostream &out,
                         const std::vector<std::string> &vec);
std::ostream &operator<<(std::ostream &out,
                         const ReactiveSyntInstance &instance);

#endif
