#include "variable_dependency.h"

using namespace std;

int main() {
    Formula formula = "G (! g_0 && true || (true && (! g_1))) && G (r_0 -> F g_0) && G (r_1 -> F g_1)";
    Variables input_vars = {"r_0", "r_1"};
    Variables output_vars = {"g_0", "g_1"};

    Specification spec = Specification(&formula, &input_vars, &output_vars);
    Specification* prime_spec = spec.get_prime();

    return 0;
}
