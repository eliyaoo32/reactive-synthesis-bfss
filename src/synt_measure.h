#ifndef REACTIVE_SYNTHESIS_BFSS_SYNT_MEASURE_H
#define REACTIVE_SYNTHESIS_BFSS_SYNT_MEASURE_H

#include <boost/json.hpp>
#include <iostream>
#include <spot/twa/twa.hh>
#include <vector>

#include "synt_instance.h"
#include "utils.h"
using namespace std;

namespace json = boost::json;

struct TestedVariable {
    string name;
    Duration duration;
    bool is_dependent;
};

class SyntMeasures {
   private:
    // Automaton data
    bool m_is_automaton_built;
    uint m_total_automaton_states;
    string m_automaton_state_based_status;
    TimeMeasure m_aut_construct_time;

    // Variables data
    TimeMeasure m_variable_test_time;
    string* currently_testing_var;
    vector<TestedVariable> m_tested_variables;

    // Generic data
    TimeMeasure m_total_time;
    SyntInstance& m_synt_instance;

   protected:
    virtual void get_json_object(json::object& obj) const;

   public:
    explicit SyntMeasures(SyntInstance& m_synt_instance)
        : m_is_automaton_built(false),
          currently_testing_var(nullptr),
          m_synt_instance(m_synt_instance),
          m_total_automaton_states(-1) {
        m_total_time.start();
    }

    ~SyntMeasures() { delete currently_testing_var; }

    void start_automaton_construct() { m_aut_construct_time.start(); }

    void end_automaton_construct(spot::twa_graph_ptr& automaton) {
        m_aut_construct_time.end();
        m_is_automaton_built = true;

        m_total_automaton_states = automaton->num_states();
        m_automaton_state_based_status =
            automaton->prop_state_acc().is_true()
                ? "true"
                : (automaton->prop_state_acc().is_false() ? "false" : "maybe");
    }

    void start_testing_variable(string& var) {
        m_variable_test_time.start();
        currently_testing_var = new string(var);
    }

    void end_testing_variable(bool is_dependent) {
        m_variable_test_time.end();

        m_tested_variables.push_back({*currently_testing_var,
                                      m_variable_test_time.get_duration(),
                                      is_dependent});
        delete currently_testing_var;
        currently_testing_var = nullptr;
    }

    friend ostream& operator<<(ostream& os, const SyntMeasures& sm);
};

class AutomatonSyntMeasure : public SyntMeasures {
   private:
    TimeMeasure m_prune_automaton_time;
    string m_prune_automaton_state_based_status;
    uint m_total_prune_automaton_states;
    TimeMeasure m_search_pair_states_time;
    int m_total_pair_states;

   protected:
    void get_json_object(json::object& obj) const override;

   public:
    explicit AutomatonSyntMeasure(SyntInstance& m_synt_instance)
        : SyntMeasures(m_synt_instance),
          m_total_pair_states(-1),
          m_total_prune_automaton_states(-1) {}

    void start_search_pair_states() { m_search_pair_states_time.start(); }

    void end_search_pair_states(int total_pair_states) {
        m_search_pair_states_time.end();
        m_total_pair_states = total_pair_states;
    }

    void start_prune_automaton() { m_prune_automaton_time.start(); }

    void end_prune_automaton(spot::twa_graph_ptr& pruned_automaton) {
        m_prune_automaton_time.end();

        m_total_prune_automaton_states = pruned_automaton->num_states();
        m_prune_automaton_state_based_status =
            pruned_automaton->prop_state_acc().is_true()
                ? "true"
                : (pruned_automaton->prop_state_acc().is_false() ? "false"
                                                                 : "maybe");
    }
};

#endif  // REACTIVE_SYNTHESIS_BFSS_SYNT_MEASURE_H
