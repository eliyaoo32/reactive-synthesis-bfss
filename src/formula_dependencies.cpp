#include "formula_dependencies.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <spot/tl/formula.hh>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/contains.hh>
#include <spot/twaalgos/translate.hh>

void FormulaDependencies::find_dependencies(
    std::vector<std::string>& dependent_variables,
    std::vector<std::string>& independent_variables) {
    std::vector<std::string> candidates(m_synt_instance.output_vars);

    while (!candidates.empty()) {
        std::string dependent_var = candidates.back();
        candidates.pop_back();

        // Build dependency set
        vector<string> dependency_set = m_synt_instance.input_vars;
        std::copy(candidates.begin(), candidates.end(), std::back_inserter(dependency_set));
        std::copy(independent_variables.begin(), independent_variables.end(), std::back_inserter(dependency_set));

        // Check if candidates variable is dependent
        if (is_variable_dependent(dependent_var, dependency_set)) {
            dependent_variables.push_back(dependent_var);
        } else {
            independent_variables.push_back(dependent_var);
        }
    }
}

bool FormulaDependencies::is_variable_dependent(
    string& dependent_var, vector<string>& dependency_vars) {
    spot::formula* dependency_formula =
        FormulaDependencies::get_dependency_formula(dependent_var,
                                                    dependency_vars);

    spot::translator trans;  // TODO: find best options for this translator
    spot::twa_graph_ptr automaton = trans.run(dependency_formula);
    bool is_empty = automaton->is_empty();

    delete dependency_formula;
    return is_empty;
}

spot::formula* FormulaDependencies::get_dependency_formula(
    string& dependent_var, vector<string>& dependency_vars) {
    if (dependent_var.empty() || dependency_vars.empty()) {
        throw std::invalid_argument(
            "Dependent and dependency are required to have at least 1 item to "
            "check for dependency");
    }

    vector<string> dependent_vars = {dependent_var};

    spot::formula dependencies_equals_formula, dependents_equals_formula;
    equal_to_primes_formula(dependencies_equals_formula, dependency_vars);
    equal_to_primes_formula(dependents_equals_formula, dependent_vars);

    auto* dependency_formula = new spot::formula(spot::formula::And(
        {m_synt_instance.formula, m_prime_synt_instance.formula,
         spot::formula::M(spot::formula::Not(dependents_equals_formula),
                          dependencies_equals_formula)}));

    return dependency_formula;
}

void equal_to_primes_formula(spot::formula& dst, vector<string>& vars) {
    vector<string> var_equal_to_prime;
    var_equal_to_prime.resize(vars.size());

    auto var_equal_to_prime_op = [](std::string& var) {
        return "(" + var + " <-> " + get_prime_variable(var) + ")";
    };
    std::transform(vars.begin(), vars.end(), var_equal_to_prime.begin(),
                   var_equal_to_prime_op);

    std::string equal_to_prime_str =
        boost::algorithm::join(var_equal_to_prime, " & ");
    spot::parsed_formula pf = spot::parse_infix_psl(equal_to_prime_str);
    if (pf.format_errors(std::cerr)) {
        throw std::runtime_error("Error parsing formula: " +
                                 equal_to_prime_str);
    }

    dst = pf.f;
}

void synt_instance_prime(ReactiveSyntInstance& src, ReactiveSyntInstance& dst) {
    // Build variables in dest
    dst.input_vars.resize(src.input_vars.size());
    dst.output_vars.resize(src.output_vars.size());

    std::transform(src.input_vars.begin(), src.input_vars.end(),
                   dst.input_vars.begin(), get_prime_variable);
    std::transform(src.output_vars.begin(), src.output_vars.end(),
                   dst.output_vars.begin(), get_prime_variable);

    // Build formula string in dest
    dst.formula_str = std::string(src.formula_str);
    for (auto& var : src.input_vars) {
        boost::replace_all(dst.formula_str, var, get_prime_variable(var));
    }
    for (auto& var : src.output_vars) {
        boost::replace_all(dst.formula_str, var, get_prime_variable(var));
    }

    // Build formula in dest
    spot::parsed_formula pf = spot::parse_infix_psl(dst.formula_str);
    if (pf.format_errors(std::cerr)) {
        throw std::runtime_error("Error parsing the prime formula: " +
                                 dst.formula_str);
    }
    dst.formula = pf.f;
}
