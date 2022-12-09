#ifndef AUTOMATON_DEPENDENCIES_H
#define AUTOMATON_DEPENDENCIES_H

#include <map>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/translate.hh>
#include <string>
#include <vector>

#include "bdd_var_cacher.h"
#include "synt_instance.h"
#include "synt_measure.h"

using PairState = std::pair<unsigned, unsigned>;

using PairEdges =
    std::pair<spot::twa_graph::edge_storage_t, spot::twa_graph::edge_storage_t>;

/* Due to bug in BuDDY where restricting variable is not working if this is the only
 * variable left, this workaround is used. */
bool can_restrict_variable(bdd& bd, int variable, bool restriction_value);

/*
Return all the states in automaton which are reachable by the same prefix.
*/
void get_all_compatible_states(std::vector<PairState>& pairStates,
                               const spot::twa_graph_ptr& aut);

bool are_edges_shares_variable(spot::twa_graph::edge_storage_t& e1,
                               spot::twa_graph::edge_storage_t& e2);

struct VarIndexer {
    int var_index{};
    int prime_var_index{};
};

class AutomatonAlgorithm {
   private:
    SyntInstance& m_synt_instance;
    AutomatonSyntMeasure& m_measures;
    spot::twa_graph_ptr m_automaton;
    BDDVarsCacher* m_bdd_cacher;

    bool is_variable_dependent(std::string dependent_var,
                               std::vector<std::string>& dependency_vars,
                               std::vector<PairState>& pairStates);

    bool is_dependent_by_pair_edges(int dependent_var, std::vector<int>& dependency_vars,
                                    vector<VarIndexer>& reset_vars,
                                    const PairEdges& edges);

    void init_automaton();

   public:
    explicit AutomatonAlgorithm(SyntInstance& synt_instance,
                                AutomatonSyntMeasure& measure)
        : m_synt_instance(synt_instance), m_measures(measure), m_bdd_cacher(nullptr) {}

    ~AutomatonAlgorithm() { delete m_bdd_cacher; }

    void find_dependencies(std::vector<std::string>& dependent_variables,
                           std::vector<std::string>& independent_variables);
};

#endif
