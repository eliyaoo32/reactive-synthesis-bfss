#include <vector>
#include <spot/tl/formula.hh>
#include <boost/algorithm/string/replace.hpp>

#include "variable_dependency.h"

Specification* Specification::get_prime() {
    // Create prime vars
    auto* prime_inputs = new std::vector<std::string>();
    auto* prime_outputs = new std::vector<std::string>();

    prime_inputs->resize( m_input_vars->size() );
    prime_outputs->resize( m_output_vars->size() );

    std::transform(m_input_vars->begin(), m_input_vars->end(), prime_inputs->begin(), get_prime_variable);
    std::transform(m_output_vars->begin(), m_output_vars->end(), prime_outputs->begin(), get_prime_variable);

    // Create prime formula
    auto* prime_formula = new std::string(*m_formula);
    for(auto input_var : *m_input_vars) {
        boost::replace_all(*prime_formula, input_var, get_prime_variable(input_var));
    }
    for(auto output_var : *m_output_vars) {
        boost::replace_all(*prime_formula, output_var, get_prime_variable(output_var));
    }

    // Create Prime specification
    auto* prime_spec = new Specification(prime_formula, prime_inputs, prime_outputs);
    return prime_spec;
}
