#include <string>
#include <spot/twaalgos/sccfilter.hh>

#include "automaton_algorithm.h"

using namespace std;

void AutomatonAlgorithm::find_dependencies(std::vector<std::string> &dependent_variables,
                                           std::vector<std::string> &independent_variables) {

    m_measures.start_automaton_construct();
    auto automaton = m_synt_instance.build_buchi_automaton();
    m_measures.end_automaton_construct(automaton);

    m_measures.start_prune_automaton();
    automaton = spot::scc_filter_states(automaton); // Prune automaton
    m_measures.end_prune_automaton(automaton);

    // Find PairStates
    m_measures.start_search_pair_states();
    vector<PairState> compatibleStates;
    getAllCompatibleStates(compatibleStates, automaton);
    m_measures.end_search_pair_states(static_cast<int>(compatibleStates.size()));

    // Find Dependencies
    std::vector<std::string> candidates(m_synt_instance.get_output_vars());
    while (!candidates.empty()) {
        std::string dependent_var = candidates.back();
        candidates.pop_back();
        m_measures.start_testing_variable(dependent_var);

        // Build dependency set
        vector<string> dependency_set(m_synt_instance.get_input_vars());
        std::copy(candidates.begin(), candidates.end(), std::back_inserter(dependency_set));
        std::copy(independent_variables.begin(), independent_variables.end(), std::back_inserter(dependency_set));

        // Check if candidates variable is dependent
        if (AutomatonAlgorithm::is_variable_dependent(dependent_var, dependency_set, compatibleStates, automaton)) {
            dependent_variables.push_back(dependent_var);
            m_measures.end_testing_variable(true);
        } else {
            independent_variables.push_back(dependent_var);
            m_measures.end_testing_variable(false);
        }
    }
}

bool AutomatonAlgorithm::is_variable_dependent(std::string dependent_var, vector<std::string> &dependency_vars,
                                               vector<PairState> &pairStates, spot::twa_graph_ptr aut) {
    // For each pair-state, Can we move to an accepting state with different value of dependent_var? If yes, then dependent_var is not dependent
    for(auto pairState : pairStates) {
        for(auto& t1 : aut->out(pairState.first)) {
            for(auto& t2 : aut->out(pairState.second)) {
                PairEdges pair_edges = PairEdges(t1, t2);
                if(!AutomatonAlgorithm::isVariableDependentByPairEdge(dependent_var, dependency_vars, pair_edges, aut)) {
                    return false;
                }
            }
        }
    }

    return true;
}



bool AutomatonAlgorithm::isVariableDependentByPairEdge(std::string& dependent_var, std::vector<std::string>& dependency_vars,
                                                       const PairEdges& edges, spot::twa_graph_ptr& aut) {
    auto get_var_index = [&aut](const std::string& var) {
        // TODO: Check - should I cache the operator register_ap so I don't have to call it every time?
        return aut->register_ap(var);
    };
    int dependent_var_num = get_var_index(dependent_var);

    vector<int> dependency_vars_num;
    std::transform(dependency_vars.begin(), dependency_vars.end(), std::back_inserter(dependency_vars_num), get_var_index);

    // Can the 1st edge be assigned to true and 2nd to false?
    if(!isDependentByConditions(dependent_var_num, dependency_vars_num, edges.first.cond, edges.second.cond, aut)) {
        return false;
    }

    // Can the 1st edge be assigned to false and 2nd to true?
    if(!isDependentByConditions(dependent_var_num, dependency_vars_num, edges.second.cond, edges.first.cond, aut)) {
        return false;
    }

    return true;
}

// If exists values to dependency variables such that with different values of dependent variable the condition is satisfiable, then dependent variable is not dependent
bool isDependentByConditions(int dependent_var, std::vector<int>& dependency_vars, const bdd& cond1, const bdd& cond2, spot::twa_graph_ptr& aut) {
    bdd z = bdd_restrict(cond1, bdd_ithvar(dependent_var)) & bdd_restrict(cond2, bdd_nithvar(dependent_var));

    for(auto& var : dependency_vars) {
        z = bdd_exist(z, bdd_ithvar(var));
    }

    bool is_dependent = z == bddfalse;
    return is_dependent;
}

// Return a list of pair-states, where each pair-state are states which can be arrived in the automaton by the same prefix.
void getAllCompatibleStates(std::vector<PairState> &pairStates, const spot::twa_graph_ptr& aut) {
    unsigned init_state = aut->get_init_state_number();
    pairStates.emplace_back(init_state, init_state);

    // Storing all neighbours of pair-states that need to be tested
    unordered_set<string> testedPairs;
    std::vector<PairState> untestedPairStates = { PairState(init_state, init_state) };

    // Testing all neighbours of pair-states that not tested yet.
    while(!untestedPairStates.empty()) {
        PairState pairState = untestedPairStates.back();
        untestedPairStates.pop_back();

        for(auto& t1: aut->out(pairState.first)) {
            for(auto& t2: aut->out(pairState.second)) {
                string key1 = std::to_string(t1.dst) + "#" + std::to_string(t2.dst);
                string key2 = std::to_string(t2.dst) + "#" + std::to_string(t1.dst);
                bool isPairTested = testedPairs.find(key1) != testedPairs.end() || testedPairs.find(key2) != testedPairs.end();

                if(isPairTested) {
                    continue;
                }
                testedPairs.insert(key1);

                if(areEdgesShareCommonVariable(t1, t2)) {
                    pairStates.emplace_back(t1.dst, t2.dst);
                    untestedPairStates.emplace_back(t1.dst, t2.dst);
                }
            }
        }
    }
}


// Are the variables used in the edges are the same?
bool areEdgesShareCommonVariable(spot::twa_graph::edge_storage_t& e1, spot::twa_graph::edge_storage_t& e2) {
    auto x= bdd_support(e1.cond);
    auto y = bdd_support(e2.cond);

    return x == y;
}


