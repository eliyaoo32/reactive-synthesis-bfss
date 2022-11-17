#ifndef UTILS_H
#define UTILS_H

#include <boost/program_options.hpp>
#include <iostream>
#include <spot/tl/formula.hh>
#include <string>
#include <vector>
#include <spot/twa/fwd.hh>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/postproc.hh>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

bool parse_cli(int argc, const char *argv[], std::string &formula,
               std::string &input, std::string &output);

extern std::ostream* verbose_out;

class SyntInstance {
private:
    std::vector<std::string> m_input_vars;
    std::vector<std::string> m_output_vars;
    spot::formula* m_formula;
    spot::twa_graph_ptr m_automaton;
public:
    SyntInstance(const std::string& input_str, const std::string& output_str) {
        boost::split(m_input_vars, input_str, boost::is_any_of(","));
        boost::split(m_output_vars, output_str, boost::is_any_of(","));
    }

    void build_formula(const std::string& formula) {
        spot::parsed_formula pf = spot::parse_infix_psl(formula);
        if (pf.format_errors(std::cerr)) {
            throw std::runtime_error("Error parsing formula_str: " + formula);
        }

        m_formula = new spot::formula(pf.f);
    }

    void build_automaton() {
        if(m_formula == nullptr) {
            throw std::runtime_error("Formula is not built yet");
        }

        spot::translator trans;
        trans.set_type(spot::postprocessor::Buchi);
        trans.set_pref(spot::postprocessor::SBAcc);  // Maybe postprocessor::Small?
        m_automaton = trans.run(m_formula);
    }

    spot::twa_graph_ptr get_automaton() const {
        return m_automaton;
    }

    spot::formula* get_formula() const {
        return m_formula;
    }

    std::string get_formula_str() const {
        std::stringstream formula_stream;
        formula_stream << m_formula;
        return formula_stream.str();
    }

    ~SyntInstance() {
        delete m_formula;
        delete spot::twa_graph_ptr;
    }

    friend std::ostream& operator<<(std::ostream& out, const SyntInstance& instance);
};

std::ostream &operator<<(std::ostream &out,
                         const std::vector<std::string> &vec);
std::ostream &operator<<(std::ostream &out,
                         const SyntInstance &instance);

#endif
