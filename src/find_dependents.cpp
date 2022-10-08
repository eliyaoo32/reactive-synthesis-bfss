#include <iostream>

#include "variable_dependency.h"

using namespace std;

int main() {
    // lilydemo01
//    Formula formula = "G (req -> X (grant && X (grant && X grant)) && (grant -> X (! grant)) && (cancel -> X (! grant U go)))";
//    Variables input_vars = {"go", "cancel", "req"};
//    Variables output_vars = {"grant"};

    // tsl_paper/button
    Formula formula = "G (! (u0count0count && ! u0count0f1dincrement0count1b <-> (u0count0f1dincrement0count1b && ! u0count0count)) && ! (u0pic0pic && ! u0pic0f1drender2button0count1b <-> (u0pic0f1drender2button0count1b && ! u0pic0pic))) && G (p0p0event0click <-> u0count0f1dincrement0count1b) && G u0pic0f1drender2button0count1b";
    Variables input_vars = {"p0p0event0click"};
    Variables output_vars = {"u0pic0f1drender2button0count1b", "u0pic0pic", "u0count0f1dincrement0count1b", "u0count0count"};

    Specification spec = Specification(&formula, &input_vars, &output_vars);

    Variables dependency = {"u0pic0pic", "u0count0f1dincrement0count1b", "u0count0count", "p0p0event0click"};
    Variables dependent = {"u0pic0f1drender2button0count1b"};

    cout << std::boolalpha << are_variables_dependent(spec, dependency, dependent) << endl;

    return 0;
}
