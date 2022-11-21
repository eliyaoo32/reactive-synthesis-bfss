#ifndef REACTIVE_SYNTHESIS_BFSS_SYNT_INSTANCE_H
#define REACTIVE_SYNTHESIS_BFSS_SYNT_INSTANCE_H

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


class SyntInstance {
private:
    std::vector<std::string> m_input_vars;
    std::vector<std::string> m_output_vars;
    spot::formula* m_formula;
public:
    SyntInstance(const std::string& input_str, const std::string& output_str);
    SyntInstance(std::vector<std::string>  inputs, std::vector<std::string>  outputs)
            : m_input_vars(std::move(inputs)), m_output_vars(std::move(outputs)), m_formula(nullptr) {};

    ~SyntInstance() {
        delete m_formula;
    }

    [[nodiscard]] spot::formula* get_formula() const { return m_formula; }

    [[nodiscard]] const std::vector<std::string>& get_input_vars() const { return m_input_vars; }

    [[nodiscard]] const std::vector<std::string>& get_output_vars() const { return m_output_vars; }

    void build_formula(const std::string& formula);

    spot::twa_graph_ptr build_buchi_automaton();

    [[nodiscard]] std::string get_formula_str() const;

    friend std::ostream& operator<<(std::ostream& out, const SyntInstance& instance);
};

std::ostream &operator<<(std::ostream &out,
                         const SyntInstance &instance);


#endif //REACTIVE_SYNTHESIS_BFSS_SYNT_INSTANCE_H
