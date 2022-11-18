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

class SyntInstance {
private:
    std::vector<std::string> m_input_vars;
    std::vector<std::string> m_output_vars;
    spot::formula* m_formula;
    spot::twa_graph_ptr m_automaton;
public:
    SyntInstance(const std::string& input_str, const std::string& output_str);
    SyntInstance(const std::vector<std::string> inputs, const std::vector<std::string> outputs)
                : m_input_vars(inputs), m_output_vars(outputs) {};

    ~SyntInstance() {
        delete m_formula;
    }

    spot::twa_graph_ptr get_automaton() const { return m_automaton; }

    spot::formula* get_formula() const { return m_formula; }

    const std::vector<std::string>& get_input_vars() const { return m_input_vars; }

    const std::vector<std::string>& get_output_vars() const { return m_output_vars; }

    void build_formula(const std::string& formula);

    spot::twa_graph_ptr& build_automaton(bool prune=false);

    std::string get_formula_str() const;

    friend std::ostream& operator<<(std::ostream& out, const SyntInstance& instance);
};

std::ostream &operator<<(std::ostream &out,
                         const std::vector<std::string> &vec);
std::ostream &operator<<(std::ostream &out,
                         const SyntInstance &instance);

#endif
