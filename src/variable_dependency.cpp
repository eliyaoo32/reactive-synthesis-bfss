#include <stdexcept>
#include <vector>
#include <spot/tl/formula.hh>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/contains.hh>
#include <spot/twaalgos/translate.hh>
#include <boost/algorithm/string/join.hpp>

#include "variable_dependency.h"

spot::formula* equal_to_prime_formula(Variables& vars) {
    Variables var_equal_to_prime;
    var_equal_to_prime.resize(vars.size());
    std::transform(vars.begin(), vars.end(), var_equal_to_prime.begin(), [](std::string& var) {
        return "(" + var + " <-> " + get_prime_variable(var) + ")";
    });

    std::string equal_to_prime_str = boost::algorithm::join(var_equal_to_prime, " & ");
    spot::parsed_formula pf = spot::parse_infix_psl(equal_to_prime_str); // Need one of parse_infix_psl(), parse_prefix_ltl().
    if(pf.format_errors(std::cerr)) {
        throw std::runtime_error("Error parsing formula: " + equal_to_prime_str);
    }

    auto* equal_to_prime_formula = new spot::formula(pf.f);
    return equal_to_prime_formula;
}

bool are_variables_dependent(ReactiveSpecification& spec, Variables& dependency, Variables& dependent) {
    spot::translator trans;
    auto* dependency_formula = get_dependency_formula(spec, dependency, dependent);

    spot::twa_graph_ptr automaton = trans.run(dependency_formula);
    bool is_empty = automaton->is_empty();

    delete dependency_formula;

    return is_empty;
}

spot::formula* get_dependency_formula(ReactiveSpecification& spec, Variables& dependency, Variables& dependent) {
    if(dependency.empty() || dependent.empty()) {
        throw std::invalid_argument("Dependent and dependency are required to have at least 1 item to check for dependency");
    }

    // Get formulas strings
    ReactiveSpecification* prime_spec = spec.get_prime();
    spot::formula* dependencies_equals_formula = equal_to_prime_formula(dependency);
    spot::formula* dependents_equals_formula = equal_to_prime_formula(dependent);

    // Dependency formula constructing (Where X is dependent on Y): (f & f' & (Y=Y')U(Y=Y' & X!=X')) = (f & f' & (X!=X') M (Y=Y'))
    auto* dependency_formula = new spot::formula(
        spot::formula::And({
            spec.get_formula(),
            prime_spec->get_formula(),
            spot::formula::M(
                spot::formula::Not(*dependents_equals_formula),
                *dependencies_equals_formula
            )
        })
    );

    delete dependencies_equals_formula;
    delete dependents_equals_formula;
    delete prime_spec;

    return dependency_formula;
}
