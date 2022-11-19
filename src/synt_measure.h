#ifndef REACTIVE_SYNTHESIS_BFSS_SYNT_MEASURE_H
#define REACTIVE_SYNTHESIS_BFSS_SYNT_MEASURE_H

#include <iostream>
#include <vector>
#include <boost/json.hpp>

#include "utils.h"
#include "synt_instance.h"
using namespace std;

namespace json = boost::json;

struct TestedVariable {
    string name;
    Duration duration;
    bool is_dependent;
};

class SyntMeasures {
private:
    bool m_is_automaton_built;

    TimeMeasure m_total_time;
    TimeMeasure m_aut_construct_time;
    TimeMeasure m_variable_test_time;

    string* currently_testing_var;
    vector<TestedVariable> m_tested_variables;

    SyntInstance& m_synt_instance;
public:
    explicit SyntMeasures(SyntInstance& m_synt_instance) : m_is_automaton_built(false), currently_testing_var(nullptr), m_synt_instance(m_synt_instance) {
        m_total_time.start();
    }

    ~SyntMeasures() {
        delete currently_testing_var;
    }

    void start_automaton_construct() { m_aut_construct_time.start(); }

    void finish_automaton_construct() {
        m_is_automaton_built = true;
        m_aut_construct_time.end();
    }

    void start_testing_variable(string& var);

    void end_testing_variable(bool is_dependent);

    friend ostream& operator<<(ostream& os, const SyntMeasures& sm);
};

class AutomatonSyntMeasure : public SyntMeasures {
public:
    void start_search_pair_states();
    void end_search_pair_states(int total_states);
    void set_total_automaton_states(int total_states);
};


#endif //REACTIVE_SYNTHESIS_BFSS_SYNT_MEASURE_H
