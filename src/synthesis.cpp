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
#include <spot/twaalgos/synthesis.hh>
#include <spot/twaalgos/translate.hh>
#include <vector>

#include "synt_instance.h"
#include "utils.h"

using namespace std;
using namespace spot;

twa_graph_ptr ntgba2dpa(const twa_graph_ptr& aut, bool force_sbacc);
void synthesis_buchi(const twa_graph_ptr& buchi, const bdd& outputs, bool force_sbacc);

int main(int argc, const char* argv[]) {
    string synt_formula, input_str, output_str;
    bool is_verbose;
    Algorithm selected_algorithm;  // TODO: remove the selected algorithm

    int parsed_cli_status = parse_cli(argc, argv, synt_formula, input_str, output_str,
                                      is_verbose, selected_algorithm);
    if (!parsed_cli_status) {
        return EXIT_FAILURE;
    }
    SyntInstance synt_instance(input_str, output_str);

    // Building Buchi Automaton
    spot::parsed_formula pf = spot::parse_infix_psl(synt_formula);
    if (pf.format_errors(std::cerr)) {
        throw std::runtime_error("Error parsing formula_str: " + synt_formula);
    }
    if (pf.f == nullptr) {
        throw std::runtime_error("Formula is not built yet");
    }

    // option_map& extra_options = gi.opt;
    // extra_options.operator[]("simul", 0);
    // extra_options.set_str("tls-impl", 1);
    // extra_options.set_str("wdba-minimize", 2);
    // const bdd_dict_ptr& dict = gi.dict;

    // spot::translator trans(extra_options);
    option_map opts;
    trans.set_type(spot::postprocessor::Buchi);
    trans.set_pref(spot::postprocessor::SBAcc);
    auto aut = trans.run(pf.f);

    // Synthesis the buchi automaton
    bool force_sbacc = false;
    bdd bdd_outputs = bddtrue;
    for (auto& out_var : synt_instance.get_output_vars()) {
        bdd_outputs &= bdd_ithvar(aut->register_ap(out_var));
    }

    synthesis_buchi(aut, bdd_outputs, force_sbacc);
}

void synthesis_buchi(const twa_graph_ptr& buchi, const bdd& outputs, bool force_sbacc) {
    // TODO: check when we can remove the Buchi automaton of specification
    // TODO: check what suppose to be the value of force sbacc

    // Generate a DPA to be solved
    auto splitted = split_2step(buchi, outputs, true);
    twa_graph_ptr dpa = ntgba2dpa(splitted, force_sbacc);
    alternate_players(dpa);
    dpa->merge_states();
    assert(dpa->get_named_prop<std::vector<bool>>("state-player") &&
           "DPA has no \"state-player\"");
    set_synthesis_outputs(dpa, outputs);

    // Solve the DPA
    bool is_game_solved = solve_game(dpa);
    if (!is_game_solved) {
        // TODO: define as UNREALIZABLE
        cout << "UNREALIZABLE" << endl;
        return;
    }

    auto mealy = solved_game_to_separated_mealy(dpa);
    print_hoa(cout, mealy);
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