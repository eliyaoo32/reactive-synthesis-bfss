#include "synt_measure.h"

void SyntMeasures::end_testing_variable(bool is_dependent) {
    m_variable_test_time.end();

    m_tested_variables.push_back({
         *currently_testing_var,
         m_variable_test_time.get_duration(),
         is_dependent
     });
    delete currently_testing_var;
}

void SyntMeasures::start_testing_variable(string& var) {
    delete currently_testing_var;

    m_variable_test_time.start();
    currently_testing_var = new string(var);
}

ostream& operator<<(ostream& os, const SyntMeasures& sm) {
    json::object obj;
    obj["total_time"] = sm.m_total_time.time_elapsed();
    obj["is_automaton_built"] = sm.m_is_automaton_built;
    if(sm.m_is_automaton_built) {
        obj["automaton_build_duration"] = sm.m_aut_construct_time.get_duration();
    }

    json::array tested_vars;
    for (const auto& var : sm.m_tested_variables) {
        json::object var_obj;
        var_obj["name"] = var.name;
        var_obj["duration"] = var.duration;
        var_obj["is_dependent"] = var.is_dependent;

        tested_vars.emplace_back(var_obj);
    }
    obj["tested_variables"] = tested_vars;

    os << json::serialize(obj); // TODO: maybe os << obj is enough
    return os;
}
