#ifndef REACTIVE_SYNTHESIS_BFSS_REACTIVE_SPECIFICATION_H
#define REACTIVE_SYNTHESIS_BFSS_REACTIVE_SPECIFICATION_H

#include <string>
#include <vector>
#include <spot/tl/formula.hh>
#include <spot/tl/parse.hh>
#include <boost/algorithm/string/join.hpp>

using Variables = std::vector<std::string>;

// We make a prime variable by adding the suffix "_pp" to variable's name
inline std::string get_prime_variable(std::string &var) {
    return var + "_pp";
}

spot::twa_graph_ptr construct_formula(const spot::formula& formula);

class ReactiveSpecification {
private:
    spot::formula m_formula; // In the format of LTL2BA / LTL3BA
    Variables m_input_vars;
    Variables m_output_vars;
public:
    ReactiveSpecification(std::string& formula,
                          Variables input_vars,
                          Variables output_vars) :
            m_formula(spot::parse_formula(formula)),
            m_input_vars(std::vector<std::string>(input_vars)),
            m_output_vars(std::vector<std::string>(output_vars))
        {}

    ReactiveSpecification* get_prime();

    const Variables& get_output_vars() { return m_output_vars; }

    const Variables& get_input_vars() { return m_input_vars; }

    const spot::formula& get_formula() { return m_formula; }
};

std::ostream& operator<<(std::ostream& out, ReactiveSpecification& spec);

#endif //REACTIVE_SYNTHESIS_BFSS_REACTIVE_SPECIFICATION_H
