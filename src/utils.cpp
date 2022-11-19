#include "utils.h"

#include <boost/program_options.hpp>
#include <iostream>
#include <vector>

namespace Options = boost::program_options;
using namespace std;

bool parse_cli(int argc, const char *argv[], std::string &formula, std::string &input_vars,
               std::string &output_vars, bool& should_verbose, Algorithm& selected_algorithm) {
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
        )
        (
            "algo,algorithm", Options::value<string>()->required(),
            "Which algorithm to use: formula, automaton"
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

        if(vm.count("algo")) {
            selected_algorithm = string_to_algorithm(vm["algo"].as<string>());

            if(selected_algorithm == Algorithm::UNKNOWN) {
                cerr << "Invalid algorithm: " << vm["algo"].as<string>() << endl;
                return false;
            }
        } else {
            std::cerr << "No algorithm was selected " << std::endl;
            return false;
        }

        return true;
    } catch (const Options::error &ex) {
        cerr << ex.what() << '\n';
        cout << desc << endl;
        return false;
    }
}


Algorithm string_to_algorithm(const std::string& str) {
    if (str == "formula") {
        return Algorithm::FORMULA;
    } else if (str == "automaton") {
        return Algorithm::AUTOMATON;
    } else {
        return Algorithm::UNKNOWN;
    }
}

ostream &operator<<(ostream &out, const vector<string> &vec) {
    for (const string &s : vec) {
        out << s << ", ";
    }
    return out;
}
