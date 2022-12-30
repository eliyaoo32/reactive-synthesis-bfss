#include "utils.h"

#include <boost/program_options.hpp>
#include <iostream>
#include <vector>

namespace Options = boost::program_options;
using namespace std;

bool parse_cli(int argc, const char *argv[], CLIOptions &options) {
    Options::options_description desc("Tool to find dependencies in LTL formula");
    desc.add_options()("formula,f", Options::value<string>(&options.formula)->required(),
                       "LTL formula")(
        "output,o", Options::value<string>(&options.outputs)->required(),
        "Output variables")("input,i",
                            Options::value<string>(&options.inputs)->required(),
                            "Input variables")(
        "verbose,v", Options::bool_switch(&options.verbose), "Verbose messages")(
        "algo,algorithm", Options::value<string>(),
        "Which algorithm to use: formula, automaton")(
        "skip-deps",
        Options::bool_switch(&options.skip_dependencies)->default_value(false),
        "Should skip finding dependent variables and synthesis them separately");

    try {
        Options::command_line_parser parser{argc, argv};
        parser.options(desc).allow_unregistered().style(
            Options::command_line_style::default_style |
            Options::command_line_style::allow_slash_for_short);
        Options::parsed_options parsed_options = parser.run();

        Options::variables_map vm;
        Options::store(parsed_options, vm);
        Options::notify(vm);

        if (vm.count("algo")) {
            options.algorithm = string_to_algorithm(vm["algo"].as<string>());
        } else {
            options.algorithm = Algorithm::UNKNOWN;
        }

        return true;
    } catch (const Options::error &ex) {
        cerr << ex.what() << '\n';
        cout << desc << endl;
        return false;
    }
}

Algorithm string_to_algorithm(const std::string &str) {
    if (str == "formula") {
        return Algorithm::FORMULA;
    } else if (str == "automaton") {
        return Algorithm::AUTOMATON;
    } else {
        return Algorithm::UNKNOWN;
    }
}

std::string algorithm_to_string(const Algorithm &algo) {
    switch (algo) {
        case Algorithm::FORMULA:
            return "formula";
        case Algorithm::AUTOMATON:
            return "automaton";
        default:
            return "unknown";
    }
}

ostream &operator<<(ostream &out, const vector<string> &vec) {
    for (const string &s : vec) {
        out << s << ", ";
    }
    return out;
}

std::ostream &operator<<(std::ostream &out, const CLIOptions &options) {
    out << boolalpha;
    out << " - Formula: " << options.formula << endl;
    out << " - Inputs: " << options.inputs << endl;
    out << " - Outputs: " << options.outputs << endl;
    out << " - Verbose: " << options.verbose << endl;
    out << " - Algorithm: " << algorithm_to_string(options.algorithm) << endl;
    out << " - Skip dependencies: " << options.skip_dependencies << endl;

    return out;
}

/// TimeMeasure implementation
void TimeMeasure::start() {
    m_start = std::chrono::steady_clock::now();
    m_has_started = true;
}

Duration TimeMeasure::end() {
    m_total_duration = time_elapsed();
    return m_total_duration;
}

Duration TimeMeasure::time_elapsed() const {
    auto end = std::chrono::steady_clock::now();
    return static_cast<Duration>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start).count());
}

Duration TimeMeasure::get_duration(bool validate_is_ended) const {
    if (validate_is_ended && m_total_duration == -1) {
        throw std::runtime_error(
            "TimeMeasure::get_total_duration() called before "
            "TimeMeasure::end()");
    }

    return m_total_duration;
}