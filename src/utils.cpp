#include "utils.h"

#include <boost/program_options.hpp>
#include <iostream>
#include <vector>

namespace Options = boost::program_options;
using namespace std;

bool parse_cli(int argc, const char *argv[], std::string &formula, std::string &input_vars,
               std::string &output_vars) {
    bool should_verbose = false;

    Options::options_description desc(
        "Tool to find dependencies in LTL formula");
    desc.add_options()
        (
            "formula,f", Options::value<string>(&formula)->required(), "LTL formula"
        )
        (
            "output,o", Options::value<string>(&output_vars)->required(), "Output variables"
        )
        (
            "input,i", Options::value<string>(&input_vars)->required(), "Input variables"
        )
        (
            "verbose,v", Options::bool_switch(&should_verbose), "Verbose messages"
        );


    try {
        Options::command_line_parser parser{argc, argv};
        parser.options(desc).allow_unregistered().style(
            Options::command_line_style::default_style | Options::command_line_style::allow_slash_for_short
        );
        Options::parsed_options parsed_options = parser.run();

        Options::variables_map vm;
        Options::store(parsed_options, vm);
        Options::notify(vm);

        if(should_verbose) {
            verbose_out = &std::cout;
        }

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

ostream &operator<<(ostream &out, const SyntInstance &instance) {
    out << "formula: " << instance.get_formula_str() << endl;
    out << "input vars: " << instance.m_input_vars << endl;
    out << "output vars: " << instance.m_output_vars << endl;
    return out;
};
