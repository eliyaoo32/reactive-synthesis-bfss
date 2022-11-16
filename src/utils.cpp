#include "utils.h"

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <string>
#include <vector>

namespace Options = boost::program_options;
using namespace std;

bool parse_cli(int argc, const char *argv[], string &formula,
               vector<string> &input_vars, vector<string> &output_vars,
               bool &verbose_output) {
    Options::options_description desc(
        "Tool to find dependencies in LTL formula");

    desc.add_options()("formula,f",
                       Options::value<string>(&formula)->required(),
                       "LTL formula")(
        "output,o",
        Options::value<string>()->required(),
        "Output variables")(
        "input,i",
        Options::value<string>()->required(),
        "Input variables")("verbose,v", Options::bool_switch(&verbose_output),
                           "Verbose messages");

    try {
        Options::command_line_parser parser{argc, argv};
        parser.options(desc).allow_unregistered().style(
            Options::command_line_style::default_style |
            Options::command_line_style::allow_slash_for_short);
        Options::parsed_options parsed_options = parser.run();

        Options::variables_map vm;
        Options::store(parsed_options, vm);
        Options::notify(vm);

        boost::split(input_vars, vm["input"].as<string>(), boost::is_any_of(","));
        boost::split(output_vars, vm["output"].as<string>(), boost::is_any_of(","));

        return true;
    } catch (const Options::error &ex) {
        cerr << ex.what() << '\n';
        cout << desc << endl;
        return false;
    }
}

ostream &operator<<(ostream &out, const vector<string> &vec) {
    for (const string &s : vec) {
        out << s << ", ";
    }
    return out;
};

ostream &operator<<(ostream &out, const ReactiveSyntInstance &instance) {
    out << "formula: " << instance.formula_str << endl;
    out << "input vars: " << instance.input_vars << endl;
    out << "output vars: " << instance.output_vars << endl;
    return out;
};
