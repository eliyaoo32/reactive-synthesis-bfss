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

    // Check for dependency
    Variables dependency = { input_vars[0] };
    Variables dependent = { output_vars[0] };
    ReactiveSpecification spec = ReactiveSpecification(formula, input_vars, output_vars);
    cout << std::boolalpha << are_variables_dependent(spec, dependency, dependent) << endl;

    return EXIT_SUCCESS;
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

ostream& operator<<(ostream& out, BenchmarkMetrics& benchmarkMetrics) {
    out << "To print this bench mark" << endl;

    return out;
}
