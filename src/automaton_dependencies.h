#ifndef AUTOMATON_DEPENDENCIES_H
#define AUTOMATON_DEPENDENCIES_H

#include <vector>
#include <string>

#include <spot/tl/parse.hh>
#include <spot/twaalgos/translate.hh>

#include "utils.h"

using PairState = std::pair<unsigned, unsigned>;

using PairEdges = std::pair<spot::twa_graph::edge_storage_t, spot::twa_graph::edge_storage_t>;

void getAllCompatibleStates(std::vector<PairState> &pairStates, spot::twa_graph_ptr aut);

bool areEdgesShareCommonVariable(spot::twa_graph::edge_storage_t& e1, spot::twa_graph::edge_storage_t& e2);

class AutomatonDependencies {
private:
    ReactiveSyntInstance &m_synt_instance;

    bool is_variable_dependent(std::string dependent_var, std::vector<std::string>& dependency_vars,
                               std::vector<PairState>& pairStates, spot::twa_graph_ptr aut);

    bool isVariableDependentByPairEdge(std::string dependent_var, std::vector<std::string>& dependency_vars,
                                       PairEdges edges, spot::twa_graph_ptr aut)
public:
    AutomatonDependencies(ReactiveSyntInstance &synt_instance)
        : m_synt_instance(synt_instance) {};

    void find_dependencies(std::vector<std::string> &dependent_variables,
                           std::vector<std::string> &independent_variables);
};


#endif
