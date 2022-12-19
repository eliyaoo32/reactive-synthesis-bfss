/*
This file takes a Buchi automaton and synthesis it
*/

#include <iostream>
#include <spot/misc/optionmap.hh>
#include <spot/twaalgos/degen.hh>
#include <spot/twaalgos/determinize.hh>
#include <spot/twaalgos/game.hh>
#include <spot/twaalgos/hoa.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/mealy_machine.hh>
#include <spot/twaalgos/sbacc.hh>
#include <spot/twaalgos/sccfilter.hh>
#include <spot/twaalgos/synthesis.hh>
#include <spot/twaalgos/translate.hh>
#include <vector>

#include "automaton_algorithm.h"
#include "synt_instance.h"
#include "utils.h"

using namespace std;
using namespace spot;

twa_graph_ptr ntgba2dpa(const twa_graph_ptr& aut, bool force_sbacc);

int main(int argc, const char* argv[]) {
    /**
     * =================== Step 1: Processing synthesis problem
     */
    string synt_formula, input_str, output_str;
    bool is_verbose;
    Algorithm selected_algorithm;  // TODO: remove the selected algorithm

    int parsed_cli_status = parse_cli(argc, argv, synt_formula, input_str, output_str,
                                      is_verbose, selected_algorithm);
    if (!parsed_cli_status) {
        return EXIT_FAILURE;
    }
    SyntInstance synt_instance(input_str, output_str);
    AutomatonSyntMeasure synt_measures(synt_instance);
    vector<string> output_vars(synt_instance.get_output_vars());

    synt_instance.build_formula(synt_formula);

    /**
     * =================== Step 2: Creates NBA of the specification
     *                             (And prune redundent)
     */
    synthesis_info gi;
    gi.s = synthesis_info::algo::SPLIT_DET;

    option_map& extra_options = gi.opt;
    const bdd_dict_ptr& dict = gi.dict;

    extra_options.set_if_unset("simul", 0);
    extra_options.set_if_unset("tls-impl", 1);
    extra_options.set_if_unset("wdba-minimize", 2);

    synt_measures.start_automaton_construct();
    translator trans(dict, &extra_options);
    trans.set_type(spot::postprocessor::Buchi);
    // TODO: Check why SBAacc adds more states to the mealy machine, consider removing it
    trans.set_pref(spot::postprocessor::SBAcc);

    auto automaton = trans.run(*synt_instance.get_formula());
    synt_measures.end_automaton_construct(automaton);

    synt_measures.start_prune_automaton();
    automaton = spot::scc_filter_states(automaton);  // Prune automaton
    synt_measures.end_prune_automaton(automaton);

    auto tobdd = [&automaton](const std::string& ap_name) {
        return bdd_ithvar(automaton->register_ap(ap_name));
    };

    /**
     * =================== Step 3: Find Dependent Variables
     */
    vector<string> dependent_variables, independent_variables;
    AutomatonAlgorithm automaton_dependencies(synt_instance, synt_measures, automaton,
                                              false);
    automaton_dependencies.find_dependencies(dependent_variables, independent_variables);

    // =================== Step 4: Remove dependent variables from the NBA
    synt_measures.start_remove_dependent_ap();
    bdd dependents = bddtrue;
    for (string& dep_var : dependent_variables) {
        dependents &= tobdd(dep_var);
    }
    // Apply exists operator on all edges
    for (int i = 0; i < automaton->num_states(); i++) {
        for (auto& edge : automaton->out(i)) {
            edge.cond = bdd_exist(edge.cond, dependents);
        }
    }
    // Unregister dependent variables
    for (string& dep_var : dependent_variables) {
        int ap = automaton->register_ap(dep_var);
        automaton->unregister_ap(ap);
    }
    synt_measures.end_remove_dependent_ap();

    /**
     * =================== Step 5: Build a game from the NBA
     */
    synt_measures.start_split_2step();
    auto is_out = [&output_vars](const std::string& ao) -> bool {
        return std::find(output_vars.begin(), output_vars.end(), ao) != output_vars.end();
    };
    bdd outs = bddtrue;
    for (auto&& aap : automaton->ap()) {
        if (is_out(aap.ap_name())) {
            outs &= tobdd(aap.ap_name());
        }
    }
    auto splitted = split_2step(automaton, outs, true);
    synt_measures.end_split_2step();

    synt_measures.start_nba_to_dpa();
    // TODO: understand what's the function ntgba2dpa and how it works internally
    auto dpa = ntgba2dpa(splitted, gi.force_sbacc);
    // Transform an automaton into a parity game by propagating players.
    alternate_players(dpa);
    // Merge states knows about players
    dpa->merge_states();
    set_synthesis_outputs(dpa, outs);
    synt_measures.end_nba_to_dpa();

    /**
     * =================== Step 6: Solve the game and print the strategy
     */
    synt_measures.start_solve_game();
    auto& arena = dpa;
    bool is_solved = spot::solve_game(arena, gi);
    if (!is_solved) {
        cout << "UNREALIZABLE" << endl;
        return EXIT_FAILURE;
    }
    synt_measures.end_solve_game();

    synt_measures.start_dpa_to_mealy();
    spot::mealy_like ml;
    ml.success = spot::mealy_like::realizability_code::REALIZABLE_REGULAR;
    ml.mealy_like = spot::solved_game_to_mealy(arena, gi);
    // TODO: check what is the should_split
    bool should_split = false;
    // TODO: Check if the minimize lvl should be from synthesis_info
    simplify_mealy_here(ml.mealy_like, 5, should_split);
    synt_measures.end_dpa_to_mealy();

    synt_measures.completed();

    cout << "/* Synthesis Measures: " << endl;
    cout << synt_measures << endl;
    cout << "*/" << endl;
    spot::print_hoa(cout, ml.mealy_like);
}

// This code is taken from: spot/twaalgos/synthesis.cc
twa_graph_ptr ntgba2dpa(const twa_graph_ptr& aut, bool force_sbacc) {
    // if the input automaton is deterministic, degeneralize it to be sure to
    // end up with a parity automaton
    auto dpa = tgba_determinize(degeneralize_tba(aut), false, true, true, false);
    dpa->merge_edges();
    if (force_sbacc) dpa = sbacc(dpa);
    reduce_parity_here(dpa, true);
    assert(([&dpa]() -> bool {
        bool max, odd;
        return dpa->acc().is_parity(max, odd);
    }()));
    assert(is_deterministic(dpa));
    return dpa;
}
