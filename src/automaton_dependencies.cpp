#include <string>

#include "automaton_dependencies.h"

using namespace std;


void AutomatonDependencies::find_dependencies(std::vector<std::string> &dependent_variables,
                                              std::vector<std::string> &independent_variables) {
    // Find PairStates
    // TODO: call scc_filter here (instead of general builder)
    vector<PairState> compatibleStates;
    getAllCompatibleStates(compatibleStates, m_synt_instance.automaton);

    // Find Dependencies
    std::vector<std::string> candidates(m_synt_instance.output_vars);
    while (!candidates.empty()) {
        std::string dependent_var = candidates.back();
        candidates.pop_back();

        // Build dependency set
        vector<string> dependency_set = m_synt_instance.input_vars;
        std::copy(candidates.begin(), candidates.end(), std::back_inserter(dependency_set));
        std::copy(independent_variables.begin(), independent_variables.end(), std::back_inserter(dependency_set));

        // Check if candidates variable is dependent
        if (is_variable_dependent(dependent_var, dependency_set)) {
            dependent_variables.push_back(dependent_var);
        } else {
            independent_variables.push_back(dependent_var);
        }
    }
}

void getAllCompatibleStates(std::vector<PairState> &pairStates, spot::twa_graph_ptr aut) {
    unsigned init_state = aut->get_init_state_number();
    pairStates.push_back(PairState(init_state, init_state));

    // Storing all neighbours of pair-states that need to be tested
    unordered_set<string> testedPairs;
    std::vector<PairState> untestedPairStates = { PairState(init_state, init_state) };

    // Testing all neighbours of pair-states that not tested yet.
    while(!untestedPairStates.empty()) {
        PairState pairState = untestedPairStates.back();
        untestedPairStates.pop_back();

        // TODO: test only if both dst leads to
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
                    pairStates.push_back(PairState(t1.dst, t2.dst));
                    untestedPairStates.push_back(PairState(t1.dst, t2.dst));
                }
            }
        }
    }
}


bool AutomatonDependencies::is_variable_dependent(std::string dependent_var, std::vector<std::string>& dependency_vars,
                                                  std::vector<PairState>& pairStates, spot::twa_graph_ptr aut) {
    for(auto pairState : pairStates) {
        for(auto& t1 : aut->out(pairState.first)) {
            for(auto& t2 : aut->out(pairState.second)) {
                PairEdges pair_edges = PairEdges(t1, t2);
                if(!isVariableDependentByPairEdge(dependent_var, dependency_vars, pair_edges, aut)) {
                    return false;
                }
            }
        }
    }

    return true;
}

bool AutomatonDependencies::isVariableDependentByPairEdge(std::string dependent_var, std::vector<std::string>& dependency_vars,
                                                          PairEdges edges, spot::twa_graph_ptr aut) {
    int dependent_var_num = aut->register_ap(dependent_var);

    // TODO: handle the inversed case, where first gets nith and second ith
    bdd x = bdd_restrict( edges.first.cond, bdd_ithvar(dependent_var_num) );
    bdd y = bdd_restrict( edges.second.cond, bdd_nithvar(dependent_var_num) );
    bdd z = x & y;
    for(auto& var : dependency_vars) {
        int var_num = aut->register_ap(var);
        z = bdd_exist(z, bdd_ithvar(var_num));
    }

    // TODO: check BDD cleaning
    bool is_dependent = z == bddfalse;
    return is_dependent;
}


// TODO: Replace this condition by if they share any common Boolean variable
bool areEdgesShareCommonVariable(spot::twa_graph::edge_storage_t& e1, spot::twa_graph::edge_storage_t& e2) {
    auto x= bdd_support(e1.cond);
    auto y = bdd_support(e2.cond);

    return x == y;
    // return e1.cond == e2.cond;
}


