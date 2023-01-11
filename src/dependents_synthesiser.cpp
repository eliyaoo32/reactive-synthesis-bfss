#include "dependents_synthesiser.h"

#include <algorithm>
#include <iostream>
#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>
#include <spot/twaalgos/aiger.hh>
#include <spot/twaalgos/translate.hh>

using namespace std;
using namespace spot;

aig_ptr DependentsSynthesiser::synthesis_next_states_aig() {
    /**
     * Step 1: Create an AIGER with required input, output and latches
     */
    vector<string> outputs = {};
    uint num_latches = m_nba_without_deps->num_states();

    // Inputs = Independets Vars + Dependent Vars
    vector<string> inputs = {};
    std::copy(m_input_vars.begin(), m_input_vars.end(), std::back_inserter(inputs));
    std::copy(m_indep_vars.begin(), m_indep_vars.end(), std::back_inserter(inputs));

    aig_ptr res_ptr = std::make_shared<aig>(inputs, outputs, num_latches,
                                            m_nba_without_deps->get_dict());
    aig& circ = *res_ptr;

    /**
     * Step 2: Find for each latch (state), between what gates it suppose to apply OR
     * operator
     */
    unordered_map<uint, vector<uint>> latch_to_bdds_gate;
    unordered_map<int, uint> bdd_to_aig_var_num;  // Maps BDD id to AIGER var num

    for (uint state = 0; state < num_latches; state++) {
        for (auto& trans : m_nba_without_deps->out(state)) {
            int bdd_id = trans.cond.id();

            // Create a var num to bdd if not exists
            if (bdd_to_aig_var_num.find(bdd_id) == bdd_to_aig_var_num.end()) {
                bdd_to_aig_var_num[bdd_id] = circ.bdd2INFvar(trans.cond);
            }

            // Add the gate to the latch
            if (latch_to_bdds_gate.find(trans.dst) == latch_to_bdds_gate.end()) {
                latch_to_bdds_gate[trans.dst] = {};
            }
            latch_to_bdds_gate[trans.dst].push_back(bdd_to_aig_var_num[bdd_id]);
        }
    }

    /**
     * Step 3: each latch, next state value is OR between all its BDD gates
     */
    for (auto& [latch, bdds_gates] : latch_to_bdds_gate) {
        circ.set_next_latch(latch, circ.aig_or(bdds_gates));
    }

    return res_ptr;
}
