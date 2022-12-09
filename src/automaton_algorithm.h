#ifndef AUTOMATON_DEPENDENCIES_H
#define AUTOMATON_DEPENDENCIES_H

#include <map>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/translate.hh>
#include <string>
#include <vector>

#include "synt_instance.h"
#include "synt_measure.h"

using PairState = std::pair<unsigned, unsigned>;

using PairEdges =
    std::pair<spot::twa_graph::edge_storage_t, spot::twa_graph::edge_storage_t>;

/* Due to bug in BuDDY where restricting variable is not working if this is the only
 * variable left, this workaround is used. */
bool can_restrict_variable(bdd& bd, int variable, bool restriction_value);

void getAllCompatibleStates(std::vector<PairState>& pairStates,
                            const spot::twa_graph_ptr& aut);

bool areEdgesShareCommonVariable(spot::twa_graph::edge_storage_t& e1,
                                 spot::twa_graph::edge_storage_t& e2);

class BDDCacher {
   private:
    int m_total_vars;
    int m_prime_var_start;  // At what index of variable, the prime var starts
    int m_prime_var_coefficient;
    unordered_map<string, int> m_variable_index;
    unordered_map<string, int> m_prime_variable_index;
    spot::twa_graph_ptr m_automaton;

   public:
    BDDCacher(spot::twa_graph_ptr automaton)
        : m_automaton(automaton), m_prime_var_coefficient(0) {
        m_total_vars = automaton->ap().size();

        // Register prime variables
        m_prime_var_start =
            automaton->get_dict()->register_anonymous_variables(m_total_vars, automaton);
    }

    int get_variable_index(std::string& variable_name) {
        if (m_variable_index.find(variable_name) == m_variable_index.end()) {
            m_variable_index[variable_name] = m_automaton->register_ap(variable_name);
        }

        return m_variable_index[variable_name];
    }

    int get_prime_variable_index(std::string& variable_name) {
        if (m_prime_variable_index.find(variable_name) == m_prime_variable_index.end()) {
            m_prime_variable_index[variable_name] =
                m_prime_var_start + m_prime_var_coefficient;

            m_prime_var_coefficient++;
        }

        return m_prime_variable_index[variable_name];
    }
};

class AutomatonAlgorithm {
   private:
    SyntInstance& m_synt_instance;
    AutomatonSyntMeasure& m_measures;
    spot::twa_graph_ptr m_automaton;
    BDDCacher* m_bdd_cacher;

    bool is_variable_dependent(std::string dependent_var,
                               std::vector<std::string>& dependency_vars,
                               std::vector<PairState>& pairStates);

    bool is_dependent_by_pair_edges(std::string& dependent_var,
                                    std::vector<std::string>& dependency_vars,
                                    const PairEdges& edges);

   public:
    explicit AutomatonAlgorithm(SyntInstance& synt_instance,
                                AutomatonSyntMeasure& measure)
        : m_synt_instance(synt_instance), m_measures(measure), m_bdd_cacher(nullptr) {}

    ~AutomatonAlgorithm() { delete m_bdd_cacher; }

    void find_dependencies(std::vector<std::string>& dependent_variables,
                           std::vector<std::string>& independent_variables);
};

#endif
