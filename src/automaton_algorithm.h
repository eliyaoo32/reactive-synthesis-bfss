#ifndef AUTOMATON_DEPENDENCIES_H
#define AUTOMATON_DEPENDENCIES_H

#include <spot/tl/parse.hh>
#include <spot/twaalgos/translate.hh>
#include <string>
#include <vector>

#include "synt_instance.h"
#include "synt_measure.h"

using PairState = std::pair<unsigned, unsigned>;

using PairEdges =
    std::pair<spot::twa_graph::edge_storage_t, spot::twa_graph::edge_storage_t>;

/* Due to bug in BuDDY where restricting variable is not allowed if this is the only
 * variable left, this workaround is used. */
bool can_restrict_variable(bdd& bd, int variable, bool restriction_value);

void getAllCompatibleStates(std::vector<PairState>& pairStates,
                            const spot::twa_graph_ptr& aut);

bool areEdgesShareCommonVariable(spot::twa_graph::edge_storage_t& e1,
                                 spot::twa_graph::edge_storage_t& e2);

bool isDependentByConditions(int dependent_var, std::vector<int>& dependency_vars,
                             const bdd& cond1, const bdd& cond2);

class AutomatonAlgorithm {
   private:
    SyntInstance& m_synt_instance;
    AutomatonSyntMeasure& m_measures;
    spot::twa_graph_ptr m_automaton;

    bool is_variable_dependent(std::string dependent_var,
                               std::vector<std::string>& dependency_vars,
                               std::vector<PairState>& pairStates);

    bool isVariableDependentByPairEdge(std::string& dependent_var,
                                       std::vector<std::string>& dependency_vars,
                                       const PairEdges& edges);

    int get_variable_index(std::string& variable_name) {
        return m_automaton->register_ap(variable_name);
    }

   public:
    explicit AutomatonAlgorithm(SyntInstance& synt_instance,
                                AutomatonSyntMeasure& measure)
        : m_synt_instance(synt_instance), m_measures(measure) {}

    void find_dependencies(std::vector<std::string>& dependent_variables,
                           std::vector<std::string>& independent_variables);
};

#endif
