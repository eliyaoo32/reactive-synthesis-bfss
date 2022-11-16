#include <iostream>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/sccfilter.hh>
#include <spot/twaalgos/translate.hh>
#include <string>
#include <vector>

#include "formula_dependencies.h"
#include "utils.h"

namespace Options = boost::program_options;
using namespace std;

int main(int argc, const char* argv[]) {
    ReactiveSyntInstance reactive_synthesis_instance;
    bool verbose_output = false;
    bool dependencies_by_formula = true;    // TODO: extract this from CLI
    bool dependencies_by_automaton = true;  // TODO: extract this from CLI

    int parsed_cli_status =
        parse_cli(argc, argv, reactive_synthesis_instance.formula_str,
                  reactive_synthesis_instance.input_vars,
                  reactive_synthesis_instance.output_vars, verbose_output);
    if (!parsed_cli_status) {
        return EXIT_FAILURE;
    }

    if (verbose_output) {
        cout << "Reactive Synthesis Problem: " << endl
             << reactive_synthesis_instance << endl;
    }

    // Construct automaton
    // TODO: measure times
    spot::parsed_formula pf =
        spot::parse_infix_psl(reactive_synthesis_instance.formula_str);
    if (pf.format_errors(std::cerr)) {
        throw std::runtime_error("Error parsing formula_str: " +
                                 reactive_synthesis_instance.formula_str);
    }
    reactive_synthesis_instance.formula = pf.f;

    spot::translator trans;
    trans.set_type(spot::postprocessor::Buchi);
    trans.set_pref(spot::postprocessor::SBAcc);  // Maybe postprocessor::Small?
    spot::twa_graph_ptr automaton =
        trans.run(reactive_synthesis_instance.formula);
    automaton = spot::scc_filter(automaton);  // Removes redundent SCC

    // Find Dependencies by formula method
    // TODO: measure time
    if (dependencies_by_formula) {
        vector<string> dependent_variables, independent_variables;
        FormulaDependencies formula_dependencies(reactive_synthesis_instance);
        formula_dependencies.find_dependencies(dependent_variables,
                                               independent_variables);

        // OUTPUT for test
        cout << "Dependent variables: " << endl;
        for (auto& var : dependent_variables) {
            cout << var << endl;
        }
        cout << "Independent variables: " << endl;
        for (auto& var : independent_variables) {
            cout << var << endl;
        }
    }

    // Step 3: Find dependencies by automaton (if needed) - report events
    // Step 4: Print the results in JSON format

    return EXIT_SUCCESS;
}
