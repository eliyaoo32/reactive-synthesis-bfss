#include <iostream>
#include <vector>

#include "formula_algorithm.h"
#include "automaton_algorithm.h"
#include "utils.h"
#include "synt_instance.h"
#include "synt_measure.h"

namespace Options = boost::program_options;
using namespace std;

int main(int argc, const char* argv[]) {
    string synt_formula, input_str, output_str;
    bool is_verbose;
    Algorithm selected_algorithm;

    int parsed_cli_status = parse_cli(argc, argv, synt_formula, input_str, output_str, is_verbose, selected_algorithm);
    if (!parsed_cli_status) {
        return EXIT_FAILURE;
    }

    std::ostream nullout(nullptr);
    ostream& verbose_out = is_verbose ? std::cout : nullout;

    // Build Synthesis synt_instance
    verbose_out << "Initialize Synthesis Instance..." << endl;
    SyntInstance synt_instance(input_str, output_str);
    verbose_out << "Building Synthesis Formula..." << endl;
    synt_instance.build_formula(synt_formula);
    verbose_out << "Synthesis Problem: " << endl;
    verbose_out << synt_instance << endl;
    verbose_out << "================================" << endl;

    // Find Dependencies by formula method
    if(selected_algorithm == Algorithm::FORMULA) {
        SyntMeasures synt_measures(synt_instance);

        verbose_out << "Building Synthesis Automaton..." << endl;
        synt_measures.start_automaton_construct();
        auto automaton = synt_instance.build_buchi_automaton();
        string state_based_status = automaton->prop_state_acc().is_true() ? "true" : (automaton->prop_state_acc().is_false() ? "false" : "maybe");
        synt_measures.end_automaton_construct(automaton);

        verbose_out << "Searching Dependencies By Formula Definition..." << endl;

        vector<string> formula_dependent_variables, formula_independent_variables;
        FormulaAlgorithm formula_dependencies(synt_instance, synt_measures);
        formula_dependencies.find_dependencies(formula_dependent_variables, formula_independent_variables);

        verbose_out << "Formula Dependent Variables: " << formula_dependent_variables << endl;
        verbose_out << "Formula Dependency Variables: " << formula_independent_variables << endl;

        cout << synt_measures << endl;
    }

    // Find Dependencies by automaton method
    if(selected_algorithm == Algorithm::AUTOMATON) {
        AutomatonSyntMeasure synt_measures(synt_instance);

        verbose_out << "Searching Dependencies By Automaton Definition..." << endl;

        vector<string> automaton_dependent_variables, automaton_independent_variables;
        AutomatonAlgorithm automaton_dependencies(synt_instance, synt_measures);
        automaton_dependencies.find_dependencies(automaton_dependent_variables, automaton_independent_variables);

        verbose_out << "Automaton Dependent Variables: " << automaton_dependent_variables << endl;
        verbose_out << "Automaton Dependency Variables: " << automaton_independent_variables << endl;

        cout << synt_measures << endl;
    }

    return EXIT_SUCCESS;
}
