#ifndef REACTIVE_SYNTHESIS_BFSS_VARIABLE_DEPENDENCY_H
#define REACTIVE_SYNTHESIS_BFSS_VARIABLE_DEPENDENCY_H

#include <string>
#include <vector>

using Variables = std::vector<std::string>;
using Formula = std::string;

// We make a prime variable by adding the suffix "_pp" to variable's name
inline std::string get_prime_variable(std::string &var) {
    return var + "_pp";
}

class Specification {
private:
    Formula *m_formula; // In the format of LTL2BA / LTL3BA
    Variables *m_input_vars;
    Variables *m_output_vars;
public:
    Specification(Formula *formula,
                  Variables *input_vars,
                  Variables *output_vars) :
            m_formula(formula),
            m_input_vars(input_vars),
            m_output_vars(output_vars) {}

    ~Specification() {
        delete m_formula;
        delete m_input_vars;
        delete m_output_vars;
    }

    Specification *get_prime();
};

#endif //REACTIVE_SYNTHESIS_BFSS_VARIABLE_DEPENDENCY_H
