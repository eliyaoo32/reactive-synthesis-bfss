#include <signal.h>

#include <iostream>
#include <vector>

#include "automaton_algorithm.h"
#include "formula_algorithm.h"
#include "synt_instance.h"
#include "synt_measure.h"
#include "utils.h"

namespace Options = boost::program_options;
using namespace std;

static SyntMeasures* synt_measures = nullptr;

// TODO: throw cerr and exit instead of cout
void on_sighup(int args) {
    try {
        cout << *synt_measures << endl;
    } catch (const std::runtime_error& re) {
        std::cout << "Runtime error: " << re.what() << std::endl;
    } catch (const std::exception& ex) {
        std::cout << "Error occurred: " << ex.what() << std::endl;
    } catch (...) {
        std::cout << "Unknown failure occurred. Possible memory corruption" << std::endl;
    }

    exit(EXIT_SUCCESS);
}

int main(int argc, const char* argv[]) {
    string synt_formula, input_str, output_str;
    bool is_verbose;
    Algorithm selected_algorithm;

    int parsed_cli_status = parse_cli(argc, argv, synt_formula, input_str, output_str,
                                      is_verbose, selected_algorithm);
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

    signal(SIGINT, on_sighup);
    signal(SIGHUP, on_sighup);

    try {
        if (selected_algorithm == Algorithm::FORMULA) {
            auto* formula_measures = new SyntMeasures(synt_instance);
            synt_measures = formula_measures;

            verbose_out << "Building Synthesis Automaton..." << endl;
            formula_measures->start_automaton_construct();
            auto automaton = synt_instance.build_buchi_automaton();
            string state_based_status =
                automaton->prop_state_acc().is_true()
                    ? "true"
                    : (automaton->prop_state_acc().is_false() ? "false" : "maybe");
            formula_measures->end_automaton_construct(automaton);

            verbose_out << "Searching Dependencies By Formula Definition..." << endl;

            vector<string> formula_dependent_variables, formula_independent_variables;
            FormulaAlgorithm formula_dependencies(synt_instance, *formula_measures);
            formula_dependencies.find_dependencies(formula_dependent_variables,
                                                   formula_independent_variables);

            verbose_out << "Formula Dependent Variables: " << formula_dependent_variables
                        << endl;
            verbose_out << "Formula Dependency Variables: "
                        << formula_independent_variables << endl;
        }

        // Find Dependencies by automaton method
        if (selected_algorithm == Algorithm::AUTOMATON) {
            auto* automaton_measures = new AutomatonSyntMeasure(synt_instance);
            synt_measures = automaton_measures;

            verbose_out << "Searching Dependencies By Automaton Definition..." << endl;

            vector<string> automaton_dependent_variables, automaton_independent_variables;
            AutomatonAlgorithm automaton_dependencies(synt_instance, *automaton_measures);
            automaton_dependencies.find_dependencies(automaton_dependent_variables,
                                                     automaton_independent_variables);

            verbose_out << "Automaton Dependent Variables: "
                        << automaton_dependent_variables << endl;
            verbose_out << "Automaton Dependency Variables: "
                        << automaton_independent_variables << endl;
        }

        synt_measures->completed();
        cout << *synt_measures << endl;
        delete synt_measures;
    } catch (const std::runtime_error& re) {
        std::cout << "Runtime error: " << re.what() << std::endl;
    } catch (const std::exception& ex) {
        std::cout << "Error occurred: " << ex.what() << std::endl;
    } catch (...) {
        std::cout << "Unknown failure occurred. Possible memory corruption" << std::endl;
    }

    return EXIT_SUCCESS;
}
