
#include "synt_instance.h"

#include <iostream>
#include <string>
#include <vector>

#include "utils.h"

SyntInstance::SyntInstance(const std::string& input_str, const std::string& output_str) {
    boost::split(m_input_vars, input_str, boost::is_any_of(","));
    boost::split(m_output_vars, output_str, boost::is_any_of(","));
    m_formula = nullptr;
    build_all_vars();
}

void SyntInstance::build_all_vars() {
    m_all_vars.resize(m_input_vars.size() + m_output_vars.size());
    for (std::string& var : boost::join(m_input_vars, m_output_vars)) {
        m_all_vars.push_back(var);
    }
}

void SyntInstance::all_vars_exclude(std::vector<std::string>& dst,
                                    const std::vector<std::string>& exclude) {
    dst.clear();

    for (const std::string& var : m_all_vars) {
        if (std::find(exclude.begin(), exclude.end(), var) == exclude.end()) {
            dst.push_back(var);
        }
    }
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
    if (m_formula == nullptr) {
        throw std::runtime_error("Formula is not built yet");
    }

    spot::translator trans;
    trans.set_type(spot::postprocessor::Buchi);
    trans.set_pref(spot::postprocessor::SBAcc);
    auto automaton = trans.run(m_formula);

    return automaton;
}

std::ostream& operator<<(std::ostream& out, const SyntInstance& instance) {
    out << "formula: " << instance.get_formula_str() << std::endl;
    out << "input vars: " << instance.m_input_vars << std::endl;
    out << "output vars: " << instance.m_output_vars << std::endl;
    return out;
}
