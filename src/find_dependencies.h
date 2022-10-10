#ifndef REACTIVE_SYNTHESIS_BFSS_FIND_DEPENDENCIES_H
#define REACTIVE_SYNTHESIS_BFSS_FIND_DEPENDENCIES_H

#include <iostream>
#include <string>
#include <boost/algorithm/string/join.hpp>

#include "reactive_specification.h"
#include "utils.h"

void extract_arguments(int argc, char** argv, std::string& formula, Variables& input_vars, Variables& output_vars);

struct TestedVariable {
    std::string variable_name;
    long duration;
};

class BenchmarkMetrics {
private:
    // Spec construction data
    bool m_is_spec_constructed;
    TimeMeasure m_spec_constructed_measure;

    // Variables results
    Variables m_dependent_variables;
    std::vector<TestedVariable> m_tested_variables;

    // Current variable tested
    TimeMeasure* m_current_testing_var_measure;
    std::string m_current_testing_var;

    // Complete process
    bool m_is_completed;
    TimeMeasure m_total_measure;
public:
    BenchmarkMetrics() : m_is_spec_constructed(false), m_current_testing_var(""), m_is_completed(false) {
        m_total_measure.start();
    }

    void start_spec_construction() {
        m_spec_constructed_measure.start();
    }

    void end_spec_construction() {
        m_spec_constructed_measure.end();
        m_is_spec_constructed = true;
    }

    void start_testing_variable(std::string& var) {
        m_current_testing_var = var;

        m_current_testing_var_measure = new TimeMeasure();
        m_current_testing_var_measure->start();
    }

    void done_testing_variable() {
        long duration = m_current_testing_var_measure->end();
        m_tested_variables.push_back({ m_current_testing_var, duration });

        delete m_current_testing_var_measure;
        m_current_testing_var = "";
    }

    void add_dependent(std::string& var) {
        m_dependent_variables.push_back(std::string(var));
    }

    void complete() {
        m_is_completed = true;
        m_total_measure.end();
    }

    void summary(std::ostream& out) {
        out << std::boolalpha;

        out << "Is completed successfully: \t" << m_is_completed << std::endl;
        out << "Total Duration: \t" << m_total_measure << std::endl;

        out << "Was spec constructed: \t " << m_is_spec_constructed << std::endl;
        out << "Spec construction duration: \t " << m_spec_constructed_measure << std::endl;

        out << "Dependent Variables: \t" << boost::algorithm::join(m_dependent_variables, ", ") << std::endl;
        out << "Tested Variables: " << std::endl;

        for(auto& tested_var : m_tested_variables) {
            out << "\t - Variable: " << tested_var.variable_name << "\t Duration: " << tested_var.duration << " ms" << std::endl;
        }
    }
};

std::ostream& operator<<(std::ostream& out, BenchmarkMetrics& benchmarkMetrics);

void search_for_dependencies(std::ostream* out, BenchmarkMetrics& metrics, ReactiveSpecification& spec, Variables& all_variables);

#endif //REACTIVE_SYNTHESIS_BFSS_FIND_DEPENDENCIES_H
