#include <iostream>
#include <boost/algorithm/string.hpp>

#include "reactive_specification.h"
#include "variable_dependency.h"
#include "find_dependencies.h"

using namespace std;

int main(int argc, char** argv) {
    // Extract arguments
    Variables input_vars, output_vars;
    string formula;
    extract_arguments(argc, argv, formula, input_vars, output_vars);

    // All the LTL variables, with the order: input_vars, output_vars
    Variables all_variables;
    all_variables.insert(all_variables.begin(), output_vars.begin(), output_vars.end());
    all_variables.insert(all_variables.begin(), input_vars.begin(), input_vars.end());

    // Check for dependency
    BenchmarkMetrics metrics;
    ReactiveSpecification spec = ReactiveSpecification(formula, input_vars, output_vars);
    search_for_dependencies(std::cout, metrics, spec, all_variables);

    return EXIT_SUCCESS;
}


ostream& operator<<(ostream& out, BenchmarkMetrics& benchmarkMetrics) {
    out << "To print this bench mark" << endl;

    return out;
}

void extract_arguments(int argc, char** argv, string& formula, Variables& input_vars, Variables& output_vars) {
    if(argc != 4) {
        std::cerr << "Usage: find_dependencies <ltl_formula> <input_vars> <output_vars>" << endl;
        std::cerr << "The <input_vars> <output_vars> are seperated by comma (,). For example: x_0,x_1,y_3" << endl;
        exit(EXIT_FAILURE);
    }

    formula = argv[1];
    boost::split(input_vars, argv[2], boost::is_any_of(","));
    boost::split(output_vars, argv[3], boost::is_any_of(","));

    if(input_vars.empty()) {
        std::cerr << "Error: Input variables are missing" << endl;
        exit(EXIT_FAILURE);
    }
    if(output_vars.empty()) {
        std::cerr << "Error: Output variables are missing" << endl;
        exit(EXIT_FAILURE);
    }
}

void search_for_dependencies(ostream& out, BenchmarkMetrics& metrics, ReactiveSpecification& spec, Variables& all_variables) {
    metrics.start_spec_construction();
    // TODO: construct spec in spec
    metrics.end_spec_construction();

    for (string& var : all_variables) {
        metrics.start_testing_variable(var);

        Variables dependent = { var };
        Variables dependency;
        copy_if(
                all_variables.begin(),
                all_variables.end(),
                back_inserter(dependency),
                [var](string& v) { return v != var; }
        );
        bool is_dependent = are_variables_dependent(spec, dependency, dependent);

        // Update metrics
        if(is_dependent) {
            metrics.add_dependent(var);
        }

        metrics.done_testing_variable();
    }

    metrics.complete();
}
