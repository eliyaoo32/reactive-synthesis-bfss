/*
This file takes a Buchi automaton and synthesis it
*/

#include <iostream>
#include <spot/twaalgos/mealy_machine.hh>
#include <vector>

#include "automaton_algorithm.h"
#include "synt_instance.h"
#include "synthesis_utils.h"
#include "utils.h"

using namespace std;
using namespace spot;

int main(int argc, const char* argv[]) {
    /**
     * Process the command line arguments
     */
    CLIOptions options;
    int parsed_cli_status = parse_cli(argc, argv, options);
    if (!parsed_cli_status) {
        return EXIT_FAILURE;
    }
    ostream nullout(nullptr);
    ostream& verbose = options.verbose ? std::cout : nullout;

    verbose << "=> Loaded Options: " << endl;
    verbose << options << endl;

    SyntInstance synt_instance(options.inputs, options.outputs, options.formula);
    AutomatonSyntMeasure synt_measures(synt_instance);

    synthesis_info gi;
    gi.s = synthesis_info::algo::SPLIT_DET;
    gi.minimize_lvl = 2;  // I.e, simplication level

    // Synthesis the input
    spot::mealy_like ml;
    bool should_find_deps = !options.skip_dependencies;
    bool is_realizable = synthesis_formula(synt_instance, gi, synt_measures, verbose,
                                           should_find_deps, ml);

    // Output results
    cout << "/* Synthesis Measures: " << endl;
    cout << synt_measures << endl;
    cout << "*/" << endl;

    if (is_realizable) {
        cout << "REALIZABLE" << endl;
        spot::print_hoa(cout, ml.mealy_like);
    } else {
        cout << "UNREALIZABLE" << endl;
    }
}
