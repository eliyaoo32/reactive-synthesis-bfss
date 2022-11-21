
#include "synt_instance.h"
#include "utils.h"

#include <iostream>
#include <vector>
#include <string>


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

spot::twa_graph_ptr SyntInstance::build_buchi_automaton() {
    if(m_formula == nullptr) {
        throw std::runtime_error("Formula is not built yet");
    }

    spot::translator trans;
    trans.set_type(spot::postprocessor::Buchi);
    trans.set_pref(spot::postprocessor::SBAcc);
    auto automaton = trans.run(m_formula);

    return automaton;
}


std::ostream &operator<<(std::ostream &out, const SyntInstance &instance) {
    out << "formula: " << instance.get_formula_str() << std::endl;
    out << "input vars: " << instance.m_input_vars << std::endl;
    out << "output vars: " << instance.m_output_vars << std::endl;
    return out;
}
