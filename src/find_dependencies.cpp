#include <signal.h>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <spot/twaalgos/contains.hh>

#include "utils.h"
#include "reactive_specification.h"
#include "variable_dependency.h"
#include "find_dependencies.h"

using namespace std;

static BenchmarkMetrics benchmark_metrics;
static ostream* verbose = &null_ostream;
static ostream& result_out = std::cout;

void on_sighup(int args) {
    result_out << benchmark_metrics << endl;
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) {
    // Extract arguments
    Variables input_vars, output_vars;
    string formula;
    extract_arguments(argc, argv, formula, input_vars, output_vars);

    // Init data for searching dependencies
    ReactiveSpecification spec = ReactiveSpecification(formula, input_vars, output_vars);
    Variables all_variables;
    all_variables.insert(all_variables.begin(), output_vars.begin(), output_vars.end());
    all_variables.insert(all_variables.begin(), input_vars.begin(), input_vars.end());

    *verbose << "=> Extracted The Specification from arguments." << endl;
    *verbose << spec << "All Variables: " << all_variables << endl;

    signal(SIGHUP, on_sighup);

    search_for_dependencies(verbose, benchmark_metrics, spec, all_variables);

    // Output results
    result_out << benchmark_metrics;

    return EXIT_SUCCESS;
}

ostream& operator<<(ostream& out, BenchmarkMetrics& benchmarkMetrics) {
    benchmarkMetrics.summary(out);
    return out;
}

void extract_arguments(int argc, char** argv, string& formula, Variables& input_vars, Variables& output_vars) {
    if(argc < 4) {
        std::cerr << "Usage: find_dependencies <ltl_formula> <input_vars> <output_vars> <verbose_level>" << endl;
        std::cerr << "The <input_vars> <output_vars> are seperated by comma (,). For example: x_0,x_1,y_3" << endl;
        std::cerr << "" << endl;
        exit(EXIT_FAILURE);
    }
    if(argc >= 5 && strcmp(argv[4], "--verbose") == 0) {
        verbose = &std::cout;
    }

    formula = argv[1];
    boost::split(input_vars, argv[2], boost::is_any_of(","));
    boost::split(output_vars, argv[3], boost::is_any_of(","));

    if(input_vars.empty()) {
        std::cerr << "Error: Input variables are missing" << endl;
        exit(EXIT_FAILURE);
    }
    if(output_vars.empty()) {
        std::cerr << "Error: Output variables are missing" << endl;
        exit(EXIT_FAILURE);
    }
}

void search_for_dependencies(ostream* out_stream, BenchmarkMetrics& metrics, ReactiveSpecification& spec, Variables& all_variables) {
    ostream& out = *out_stream;
    out << "=> Start constructing TWA of the spec" << endl;

    metrics.start_spec_construction();
    spot::twa_graph_ptr formula_automata = construct_formula(spec.get_formula());
    metrics.end_spec_construction();

    out << "==> Constructed Successfully the TWA of the spec" << endl;

    out << "=> Start search for dependent variables" << endl;
    for (string& var : all_variables) {
        out << "==> Checking variable " << var << endl;
        metrics.start_testing_variable(var);

        Variables dependent = { var };
        Variables dependency;
        copy_if(
                all_variables.begin(),
                all_variables.end(),
                back_inserter(dependency),
                [var](string& v) { return v != var; }
        );
        bool is_dependent = are_variables_dependent(spec, dependency, dependent);

        // Update metrics
        if(is_dependent) {
            metrics.add_dependent(var);
            out << "===> The Variable \"" << var << "\" is dependent" << endl;
        } else {
            out << "===> The Variable \"" << var << "\" is not dependent" << endl;
        }

        metrics.done_testing_variable();
    }

    metrics.complete();
    out << "=> Done Analysing the benchmark" << endl;
}
