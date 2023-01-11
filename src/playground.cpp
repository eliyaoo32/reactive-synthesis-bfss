#include <iostream>
#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>
#include <spot/twaalgos/aiger.hh>
#include <spot/twaalgos/translate.hh>

using namespace std;
using namespace spot;

int main() {
    // Create an ltl formula
    string ltl_forula = "G(i1 -> o1 && i2 -> o2)";
    spot::parsed_formula pf = spot::parse_infix_psl(ltl_forula);
    if (pf.format_errors(std::cerr)) return 1;

    translator trans;
    twa_graph_ptr automaton = trans.run(pf.f);

    // Create aiger
    vector<string> inputs = {"i1", "i2", "i3"};
    vector<string> outputs = {};
    uint num_latches = 4;
    bdd_dict_ptr bdd_dict = automaton->get_dict();

    aig_ptr res_ptr = std::make_shared<aig>(inputs, outputs, num_latches, bdd_dict);
    aig& circ = *res_ptr;

    bdd i1 = bdd_ithvar(automaton->register_ap(inputs.at(0)));
    bdd i2 = bdd_ithvar(automaton->register_ap(inputs.at(1)));
    bdd i3 = bdd_ithvar(automaton->register_ap(inputs.at(2)));

    bdd some_for = (i1 | i2);
    some_for &= bdd_imp(i1, i3);

    uint some_for_aiger = circ.bdd2INFvar(some_for);
    cout << "some_for_aiger : " << some_for_aiger << endl;
    circ.set_next_latch(0, some_for_aiger);

    print_aiger(cout, res_ptr) << '\n';
}

/**
 * Aiger Details:
 * - Lateches = for each state a corresponding latch
 * - AIGER Inputs = Input APs + Independent Outs
 * - AIGER Output = Next States
 *
 * AIGER:
 * Aiger Input = AP Inputs + AP Independent Output
 * Aiger Latches = For each state a corresponding latch
 * Aiger Output = None
 *
 * Create an AIGER for each transation BDD
 * NBA: (s_i, B_ij, s_j) => Aig_ij
 *
 * s_j' = Or(
 *  AND(Aig_ij, s_i),
 *  for all (s_i, Aig_ij, s_j) in transitions
 * )
 */
// Automaton without dependent variables
aig_ptr next_states_aig(twa_graph_ptr automaton) {
    aig_ptr res_ptr = std::shared_ptr<aig>();
}
