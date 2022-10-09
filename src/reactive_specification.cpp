#include <boost/algorithm/string/replace.hpp>
#include <spot/tl/formula.hh>
#include <spot/twaalgos/contains.hh>
#include <spot/twaalgos/translate.hh>

#include "reactive_specification.h"

ReactiveSpecification* ReactiveSpecification::get_prime() {
    // Generate input/output vars
    Variables prime_inputs, prime_outputs;
    prime_inputs.resize( m_input_vars.size() );
    prime_outputs.resize( m_output_vars.size() );
    std::transform(m_input_vars.begin(), m_input_vars.end(), prime_inputs.begin(), get_prime_variable);
    std::transform(m_output_vars.begin(), m_output_vars.end(), prime_outputs.begin(), get_prime_variable);

    // Create prime formula
    std::stringstream formula_stream;
    formula_stream << m_formula;
    std::string prime_formula = formula_stream.str();
    for(auto input_var : m_input_vars) {
        boost::replace_all(prime_formula, input_var, get_prime_variable(input_var));
    }
    for(auto output_var : m_output_vars) {
        boost::replace_all(prime_formula, output_var, get_prime_variable(output_var));
    }

    // Create prime specification
    auto* prime_spec = new ReactiveSpecification(prime_formula, prime_inputs, prime_outputs);
    return prime_spec;
}

spot::twa_graph_ptr construct_formula(const spot::formula& formula) {
    spot::translator trans;
    spot::twa_graph_ptr automaton = trans.run(formula);

    return automaton;
}
