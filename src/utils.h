#ifndef UTILS_H
#define UTILS_H

#include <boost/program_options.hpp>
#include <iostream>
#include <spot/tl/formula.hh>
#include <string>
#include <vector>

bool parse_cli(int argc, const char *argv[], std::string &formula,
               std::vector<std::string> &input_vars,
               std::vector<std::string> &output_vars, bool &verbose_output);

struct ReactiveSyntInstance {
    std::string formula_str;  // TODO: check if I can remove this field
    std::vector<std::string> input_vars;
    std::vector<std::string> output_vars;
    spot::formula formula;
};

std::ostream &operator<<(std::ostream &out,
                         const std::vector<std::string> &vec);
std::ostream &operator<<(std::ostream &out,
                         const ReactiveSyntInstance &instance);

#endif
