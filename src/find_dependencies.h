#ifndef REACTIVE_SYNTHESIS_BFSS_FIND_DEPENDENCIES_H
#define REACTIVE_SYNTHESIS_BFSS_FIND_DEPENDENCIES_H

#include <iostream>
#include <string>
#include "reactive_specification.h"

void extract_arguments(int argc, char** argv, std::string& formula, Variables& input_vars, Variables& output_vars);

struct CheckedVariable {
    std::string variable_name;
    int duration;
};

class BenchmarkMetrics {
private:
    bool m_is_spot_constructed;
    int m_spot_construction_duration;
    bool m_is_completed;
    int m_total_duration;
    Variables m_dependent_variables;
    std::vector<CheckedVariable> m_checked_variables;
    std::string m_current_testing_variable;
public:
    BenchmarkMetrics() :
            m_is_spot_constructed(false),
            m_spot_construction_duration(-1),
            m_is_completed(false),
            m_current_testing_variable(""),
            m_total_duration(-1) {}

    void add_dependent(std::string var) {
        m_dependent_variables.push_back(var);
    }

    void add_checked_var(std::string var, int duration) {
        m_checked_variables.push_back({ var, duration });
    }

    void constructed_by_spot(int duration) {
        m_is_spot_constructed = true;
        m_spot_construction_duration = duration;
    }

    void finish(bool is_completed, int total_duration) {
        m_is_completed = is_completed;
        m_total_duration = total_duration;
    }

    void set_current_testing_variable(std::string var) {
        m_current_testing_variable = var;
    }
};

std::ostream& operator<<(std::ostream& out, BenchmarkMetrics& benchmarkMetrics);

#endif //REACTIVE_SYNTHESIS_BFSS_FIND_DEPENDENCIES_H
