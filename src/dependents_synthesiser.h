#ifndef DEPENDENTS_SYNTHESISER_H
#define DEPENDENTS_SYNTHESISER_H

#include <spot/tl/parse.hh>
#include <spot/twaalgos/aiger.hh>
#include <string>
#include <vector>

class DependentsSynthesiser {
   private:
    spot::twa_graph_ptr m_nba_without_deps;
    std::vector<std::string> m_input_vars;
    std::vector<std::string> m_indep_vars;

   public:
    DependentsSynthesiser(spot::twa_graph_ptr nba_without_deps,
                          std::vector<std::string>& input_vars,
                          std::vector<std::string>& indep_vars)
        : m_nba_without_deps(nba_without_deps),
          m_input_vars(input_vars),
          m_indep_vars(indep_vars){};

    /**
     * Synthesis an aiger which find the next subsets of states given assignments to
     * Input and Independents variables
     */
    spot::aig_ptr synthesis_next_states_aig();
};

#endif