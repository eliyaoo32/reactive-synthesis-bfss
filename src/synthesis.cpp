/*
This file takes a Buchi automaton and synthesis it
*/

#include <iostream>
#include <spot/twaalgos/mealy_machine.hh>
#include <string>
#include <vector>

#include "find_deps_by_automaton.h"
#include "synt_instance.h"
#include "synthesis_utils.h"
#include "utils.h"

using namespace std;
using namespace spot;

void print_decomposed_headers(std::ostream& out,
                              std::vector<AutomatonSyntMeasure>& synthesis_measures_list,
                              int sub_formulas);

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

    synthesis_info gi;
    gi.s = synthesis_info::algo::SPLIT_DET;
    gi.minimize_lvl = 2;  // I.e, simplication level

    if (options.decompose_formula) {
        vector<string> output_vars = {};
        vector<string> input_vars = {};

        extract_variables(options.outputs, output_vars);
        extract_variables(options.inputs, input_vars);

        // Decompose the formula into sub-formulas
        auto splitted_formulas =
            spot::split_independant_formulas(options.formula, output_vars);
        auto sub_formulas = splitted_formulas.first;
        auto sub_out_aps = splitted_formulas.second;
        assert(sub_formulas.size() == sub_out_aps.size());

        verbose << "=> Found " << sub_formulas.size() << " sub-formulas" << endl;

        // Synthesis each subformula
        std::vector<spot::mealy_like> mealy_machines;
        std::vector<AutomatonSyntMeasure> synthesis_measures_list;
        auto sub_formula_iter = sub_formulas.begin();
        auto sub_out_aps_iter = sub_out_aps.begin();

        for (; sub_formula_iter != sub_formulas.end();
             sub_formula_iter++, sub_out_aps_iter++) {
            vector<string> sub_output_vars;
            for (auto& ap : *sub_out_aps_iter) {
                sub_output_vars.push_back(ap.ap_name());
            }

            /* TODO: remove those instances, currently those instances need to be alive
             * until end of the program*/
            SyntInstance* synt_instance =
                new SyntInstance(input_vars, sub_output_vars, *sub_formula_iter);
            synt_instance->order_output_vars(output_vars);

            AutomatonSyntMeasure synt_measures(*synt_instance, options.skip_dependencies);

            spot::mealy_like ml;
            bool should_find_deps = !options.skip_dependencies;
            bool is_realizable = synthesis_formula(*synt_instance, gi, synt_measures,
                                                   verbose, should_find_deps, ml);
            synthesis_measures_list.push_back(synt_measures);

            if (!is_realizable) {
                print_decomposed_headers(cout, synthesis_measures_list,
                                         sub_formulas.size());

                cout << "UNREALIZABLE" << endl;
                return 0;
            }

            mealy_machines.push_back(ml);
        }

        // Merge sub-formulas
        assert(std::all_of(
                   mealy_machines.begin(), mealy_machines.end(),
                   [](const auto& ml) {
                       return ml.success ==
                              spot::mealy_like::realizability_code::REALIZABLE_REGULAR;
                   }) &&
               "Cannot handle TGBA as strategy.");

        auto tot_strat = mealy_machines.front().mealy_like;
        for (size_t i = 1; i < mealy_machines.size(); i++)
            tot_strat = spot::mealy_product(tot_strat, mealy_machines[i].mealy_like);

        // Print mealy machine
        print_decomposed_headers(cout, synthesis_measures_list, sub_formulas.size());
        if (!tot_strat) {
            cout << "UNREALIZABLE" << endl;
        } else {
            cout << "REALIZABLE" << endl;
            spot::print_hoa(cout, tot_strat);
        }
    } else {
        SyntInstance synt_instance(options.inputs, options.outputs, options.formula);
        AutomatonSyntMeasure synt_measures(synt_instance, options.skip_dependencies);

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
}

void print_decomposed_headers(std::ostream& out,
                              std::vector<AutomatonSyntMeasure>& synthesis_measures_list,
                              int sub_formulas) {
    out << "/* Sub-Formulas: " << sub_formulas << " */" << endl;

    for (int i = 0; i < synthesis_measures_list.size(); i++) {
        out << "/* Synthesis Measures [" << (i + 1) << "]: " << endl;
        out << synthesis_measures_list[i] << endl;
        out << "*/" << endl;
    }
}
