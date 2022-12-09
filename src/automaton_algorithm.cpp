#include "automaton_algorithm.h"

#include <boost/range/join.hpp>
#include <cmath>
#include <spot/twaalgos/sccfilter.hh>
#include <string>

#include "utils.h"

using namespace std;

void AutomatonAlgorithm::init_automaton() {
    // Build Automaton & BDD Cacher
    m_measures.start_automaton_construct();
    m_automaton = m_synt_instance.build_buchi_automaton();
    m_measures.end_automaton_construct(m_automaton);

    m_measures.start_prune_automaton();
    m_automaton = spot::scc_filter_states(m_automaton);  // Prune m_automaton
    m_measures.end_prune_automaton(m_automaton);

    m_bdd_cacher = new BDDVarsCacher(m_automaton);
}

void AutomatonAlgorithm::find_dependencies(vector<string>& dependent_variables,
                                           vector<string>& independent_variables) {
    init_automaton();

    // Find PairStates
    m_measures.start_search_pair_states();
    vector<PairState> compatibleStates;
    get_all_compatible_states(compatibleStates, m_automaton);
    m_measures.end_search_pair_states(static_cast<int>(compatibleStates.size()));

    // Find Dependencies
    std::vector<std::string> candidates(m_synt_instance.get_output_vars());
    while (!candidates.empty()) {
        std::string dependent_var = candidates.back();
        candidates.pop_back();
        m_measures.start_testing_variable(dependent_var);

        // Dependency Set = Input Vars + Candidates Vars + Independent Vars
        auto target_deps_set =
            boost::join(m_synt_instance.get_input_vars(),
                        boost::join(candidates, independent_variables));
        vector<string> dependency_set;
        copy(target_deps_set.begin(), target_deps_set.end(),
             back_inserter(dependency_set));

        // Check if candidates variable is dependent
        if (AutomatonAlgorithm::is_variable_dependent(dependent_var, dependency_set,
                                                      compatibleStates)) {
            dependent_variables.push_back(dependent_var);
            m_measures.end_testing_variable(true, dependency_set);
        } else {
            independent_variables.push_back(dependent_var);
            m_measures.end_testing_variable(false, dependency_set);
        }
    }
}

bool AutomatonAlgorithm::is_variable_dependent(std::string dependent_var,
                                               vector<std::string>& dependency_vars,
                                               vector<PairState>& pairStates) {
    // Extract variables indexes
    vector<VarIndexer> reset_vars_nums;
    vector<int> dependency_vars_nums;
    int dependent_var_num = m_bdd_cacher->get_variable_index(dependent_var);

    for (auto& var : dependency_vars) {
        dependency_vars_nums.push_back(m_bdd_cacher->get_variable_index(var));
    }
    vector<string> reset_vars;
    vector<string> excluded_vars(dependency_vars);
    excluded_vars.emplace_back(dependent_var);
    m_synt_instance.all_vars_excluded(reset_vars, excluded_vars);
    for (auto& var : reset_vars) {
        reset_vars_nums.push_back({m_bdd_cacher->get_variable_index(var),
                                   m_bdd_cacher->get_prime_variable_index(var)});
    }

    // For each pair-state, Can we move to an accepting state with different
    // value of dependent_var? If yes, then dependent_var is not dependent
    for (auto pairState : pairStates) {
        for (auto& t1 : m_automaton->out(pairState.first)) {
            for (auto& t2 : m_automaton->out(pairState.second)) {
                PairEdges pair_edges = PairEdges(t1, t2);

                if (!AutomatonAlgorithm::is_dependent_by_pair_edges(
                        dependent_var_num, dependency_vars_nums, reset_vars_nums,
                        pair_edges)) {
                    return false;
                }
            }
        }
    }

    return true;
}

/**
 * A Variable X is dependent on the set Y if for all pair-states (s1, s2), not exists an
 * assignment 𝜋 of Y such that both s1(𝜋, X=True, ...), s2(𝜋, X=False, ...) are satified
 * (And vice-versa).
 *
 * Mathematcially, [∃Y : s1(Y, X=True, Z) & s2(Y, X=False, Z')] is not satisfiable.
 * and [∃Y : s1(Y, X=False, Z) & s2(Y, X=True, Z')] is not satisfiable as well.
 * Where Z are rest of variables.
 */
bool AutomatonAlgorithm::is_dependent_by_pair_edges(int dependent_var,
                                                    vector<int>& dependency_vars,
                                                    vector<VarIndexer>& reset_vars,
                                                    const PairEdges& edges) {
    bdd z1(edges.first.cond);
    bdd z2(edges.second.cond);

    // Replace in z2 the reset_vars with their prime vars
    bddPair* pairs = bdd_newpair();
    for (auto& var : reset_vars) {
        bdd_setpair(pairs, var.var_index, var.prime_var_index);
    }
    z2 = bdd_replace(z2, pairs);

    bdd vars_exists = bddtrue;
    for (auto& var : dependency_vars) {
        vars_exists = vars_exists & bdd_ithvar(var);
    }

    // Case 1: [∃Y : s1(Y, X=True, Z) & s2(Y, X=False, Z')] is unsat
    bdd cond1 =
        bdd_appex(bdd_restrict(z1, bdd_ithvar(dependent_var)),
                  bdd_restrict(z2, bdd_nithvar(dependent_var)), bddop_and, vars_exists);
    bool cond1_sat = cond1 != bddfalse;
    if (cond1_sat) {
        bdd_freepair(pairs);
        return false;
    }

    // Case 2: [∃Y : s1(Y, X=False, Z) & s2(Y, X=True, Z')] is unsat
    bdd cond2 =
        bdd_appex(bdd_restrict(z1, bdd_nithvar(dependent_var)),
                  bdd_restrict(z2, bdd_ithvar(dependent_var)), bddop_and, vars_exists);
    bool cond2_sat = cond2 != bddfalse;
    if (cond2_sat) {
        bdd_freepair(pairs);
        return false;
    }

    // No condition is satified
    bdd_freepair(pairs);
    return true;
}

// Return a list of pair-states, where each pair-state are states which can be
// arrived in the automaton by the same prefix.
void get_all_compatible_states(std::vector<PairState>& pairStates,
                               const spot::twa_graph_ptr& aut) {
    unsigned init_state = aut->get_init_state_number();
    pairStates.emplace_back(init_state, init_state);

    // Storing all neighbours of pair-states that need to be tested
    unordered_set<string> testedPairs;
    std::vector<PairState> untestedPairStates = {PairState(init_state, init_state)};

    // Testing all neighbours of pair-states that not tested yet.
    while (!untestedPairStates.empty()) {
        PairState pairState = untestedPairStates.back();
        untestedPairStates.pop_back();

        for (auto& t1 : aut->out(pairState.first)) {
            for (auto& t2 : aut->out(pairState.second)) {
                string key1 = std::to_string(t1.dst) + "#" + std::to_string(t2.dst);
                string key2 = std::to_string(t2.dst) + "#" + std::to_string(t1.dst);

                bool isPairTested = testedPairs.find(key1) != testedPairs.end() ||
                                    testedPairs.find(key2) != testedPairs.end();

                if (isPairTested) {
                    continue;
                }
                testedPairs.insert(key1);

                if (are_edges_shares_variable(t1, t2)) {
                    pairStates.emplace_back(t1.dst, t2.dst);
                    untestedPairStates.emplace_back(t1.dst, t2.dst);
                }
            }
        }
    }
}

// Are the variables used in the edges are the same?
bool are_edges_shares_variable(spot::twa_graph::edge_storage_t& e1,
                               spot::twa_graph::edge_storage_t& e2) {
    auto x = bdd_support(e1.cond);
    auto y = bdd_support(e2.cond);

    return x == y;
}

bool can_restrict_variable(bdd& bd, int variable, bool restriction_value) {
    bdd var_bdd = restriction_value ? bdd_ithvar(variable) : bdd_nithvar(variable);

    return bdd_and(bd, var_bdd) != bddfalse;
}