#ifndef AUTOMATON_DEPENDENCIES_H
#define AUTOMATON_DEPENDENCIES_H

#include <vector>
#include <string>

#include <spot/tl/parse.hh>
#include <spot/twaalgos/translate.hh>

#include "synt_instance.h"

using PairState = std::pair<unsigned, unsigned>;

using PairEdges = std::pair<spot::twa_graph::edge_storage_t, spot::twa_graph::edge_storage_t>;

void getAllCompatibleStates(std::vector<PairState> &pairStates, const spot::twa_graph_ptr& aut);

bool areEdgesShareCommonVariable(spot::twa_graph::edge_storage_t& e1, spot::twa_graph::edge_storage_t& e2);

bool isDependentByConditions(int dependent_var, std::vector<int>& dependency_vars, const bdd& cond1, const bdd& cond2, spot::twa_graph_ptr& aut);

class AutomatonDependencies {
private:
    SyntInstance &m_synt_instance;

    static bool is_variable_dependent(std::string dependent_var, std::vector<std::string> &dependency_vars,
                               std::vector<PairState> &pairStates, spot::twa_graph_ptr aut);

    static bool isVariableDependentByPairEdge(std::string& dependent_var, std::vector<std::string>& dependency_vars,
                                       const PairEdges& edges, spot::twa_graph_ptr& aut);
public:
    explicit AutomatonDependencies(SyntInstance &synt_instance)
        : m_synt_instance(synt_instance) {};

    void find_dependencies(std::vector<std::string> &dependent_variables,
                           std::vector<std::string> &independent_variables);
};


#endif
