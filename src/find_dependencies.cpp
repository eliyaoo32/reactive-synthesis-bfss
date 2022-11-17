#include <iostream>
#include <vector>

#include "formula_dependencies.h"
#include "automaton_dependencies.h"
#include "utils.h"

namespace Options = boost::program_options;
using namespace std;

int main(int argc, const char* argv[]) {
    string synt_formula, input_str, output_str;
    int parsed_cli_status = parse_cli(argc, argv, synt_formula, input_str, output_str);
    if (!parsed_cli_status) {
        return EXIT_FAILURE;
    }

    // Build Synthesis synt_instance
    *verbose_out << "Initialize Synthesis Instance..." << endl;
    SyntInstance synt_instance(input_str, output_str);

    *verbose_out << "Building Synthesis Formula..." << endl;
    synt_instance.build_formula(synt_formula);

    *verbose_out << "Building Synthesis Automaton..." << endl;
    synt_instance.build_automaton();

    *verbose_out << "Synthesis Problem: " << endl;
    *verbose_out << synt_instance << endl;

    // Find Dependencies by formula method
    *verbose_out << "Searching Dependencies By Formula Definition..." << endl;
    vector<string> formula_dependent_variables, formula_independent_variables;
    FormulaDependencies formula_dependencies(synt_instance);
    formula_dependencies.find_dependencies(formula_dependent_variables,
                                           formula_independent_variables);

    *verbose_out << "Searching Dependencies By Automaton Definition..." << endl;
    vector<string> automaton_dependent_variables, automaton_independent_variables;
    AutomatonDependencies automaton_dependencies(synt_instance);
    automaton_dependencies.find_dependencies(automaton_dependent_variables,
                                             automaton_independent_variables);

    // Step 4: Print the results in JSON format

    return EXIT_SUCCESS;
}
