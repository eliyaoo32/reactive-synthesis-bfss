#ifndef FORMULA_DEPENDENCIES_H
#define FORMULA_DEPENDENCIES_H

#include <spot/twa/twa.hh>
#include <string>
#include <vector>

#include "utils.h"

using namespace std;

// We make a prime variable by adding the suffix "_pp" to variable's name
inline std::string get_prime_variable(std::string &var) { return var + "_pp"; }

void synt_instance_prime(ReactiveSyntInstance &src, ReactiveSyntInstance &dst);

void equal_to_primes_formula(spot::formula &formula, vector<string> &vars);

class FormulaDependencies {
   private:
    ReactiveSyntInstance &m_synt_instance;
    ReactiveSyntInstance m_prime_synt_instance;

    bool is_variable_dependent(string &dependent_var,
                               vector<string> &dependency_vars);

    spot::formula *get_dependency_formula(string &dependent_var,
                                          vector<string> &dependency_vars);

   public:
    FormulaDependencies(ReactiveSyntInstance &synt_instance)
        : m_synt_instance(synt_instance) {
        synt_instance_prime(m_synt_instance, m_prime_synt_instance);
    };

    void find_dependencies(std::vector<std::string> &dependent_variables,
                           std::vector<std::string> &independent_variables);
};

#endif
