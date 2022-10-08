#include <stdexcept>
#include <vector>
#include <spot/tl/formula.hh>
#include <spot/tl/parse.hh>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/join.hpp>

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

std::string*  equal_to_primes_formula(Variables& vars) {
    // Create a list of formulas: X <-> X_pp
    Variables var_equal_to_prime;
    var_equal_to_prime.resize(vars.size());
    std::transform(vars.begin(), vars.end(), var_equal_to_prime.begin(), [](std::string& var) {
        return var + " <-> " + get_prime_variable(var);
    });

    // Join the equiv formulas by &&
    auto* equal_formula = new std::string(boost::algorithm::join(var_equal_to_prime, " && "));

    return equal_formula;
}


bool are_dependent(Specification& spec, Variables& dependency, Variables& dependent) {
    if(dependency.empty() || dependent.empty()) {
        throw std::invalid_argument("Dependent and dependency are required to have at least 1 item to check for dependency");
    }

    Specification* prime_spec = spec.get_prime();
    std::string* dependency_equals_formula = equal_to_primes_formula(dependency);

    spot::formula spec_formula = spot::parse_formula(*(spec.get_formula()));
    spot::formula spec_prime_formula =  spot::parse_formula(*(prime_spec->get_formula()));

    delete dependency_equals_formula;
    delete prime_spec;
}
