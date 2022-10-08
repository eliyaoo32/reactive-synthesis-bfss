#ifndef REACTIVE_SYNTHESIS_BFSS_VARIABLE_DEPENDENCY_H
#define REACTIVE_SYNTHESIS_BFSS_VARIABLE_DEPENDENCY_H

#include "reactive_specification.h"

spot::formula* get_dependency_formula(ReactiveSpecification& spec, Variables& dependency, Variables& dependent);

bool are_variables_dependent(ReactiveSpecification& spec, Variables& dependency, Variables& dependent);

#endif //REACTIVE_SYNTHESIS_BFSS_VARIABLE_DEPENDENCY_H
