#include "utils.h"

#include <boost/program_options.hpp>
#include <spot/twaalgos/sccfilter.hh>
#include <iostream>
#include <vector>

namespace Options = boost::program_options;
using namespace std;

bool parse_cli(int argc, const char *argv[], std::string &formula, std::string &input_vars,
               std::string &output_vars, bool& should_verbose) {
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

        return true;
    } catch (const Options::error &ex) {
        cerr << ex.what() << '\n';
        cout << desc << endl;
        return false;
    }
}

SyntInstance::SyntInstance(const std::string& input_str, const std::string& output_str) {
    boost::split(m_input_vars, input_str, boost::is_any_of(","));
    boost::split(m_output_vars, output_str, boost::is_any_of(","));
    m_formula = nullptr;
}

void SyntInstance::build_formula(const std::string& formula) {
    spot::parsed_formula pf = spot::parse_infix_psl(formula);
    if (pf.format_errors(std::cerr)) {
        throw std::runtime_error("Error parsing formula_str: " + formula);
    }

    m_formula = new spot::formula(pf.f);
}

std::string SyntInstance::get_formula_str() const {
    std::stringstream formula_stream;
    formula_stream << *m_formula;
    return formula_stream.str();
}

spot::twa_graph_ptr SyntInstance::build_automaton(bool prune) {
    if(m_formula == nullptr) {
        throw std::runtime_error("Formula is not built yet");
    }

    spot::translator trans;
    trans.set_type(spot::postprocessor::Buchi);
    trans.set_pref(spot::postprocessor::SBAcc);
    auto automaton = trans.run(m_formula);

    if(prune) {
        // Prune unreachable states
        automaton = spot::scc_filter_states(automaton);
    }

    return automaton;
}

ostream &operator<<(ostream &out, const vector<string> &vec) {
    for (const string &s : vec) {
        out << s << ", ";
    }
    return out;
}

ostream &operator<<(ostream &out, const SyntInstance &instance) {
    out << "formula: " << instance.get_formula_str() << endl;
    out << "input vars: " << instance.m_input_vars << endl;
    out << "output vars: " << instance.m_output_vars << endl;
    return out;
}
