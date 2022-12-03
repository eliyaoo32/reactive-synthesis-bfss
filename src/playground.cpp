#include <iostream>
#include <spot/tl/formula.hh>
#include <spot/tl/parse.hh>
#include <spot/twa/fwd.hh>
#include <spot/twaalgos/postproc.hh>
#include <spot/twaalgos/translate.hh>
#include <string>
#include <vector>
using namespace std;

int main() {
    string formula =
        "((Fa & Fb & GFp0) | (FG!p0 & (G!a | G!b))) & G((p0 & !p1) | (!p0 & p1))";
    vector<string> inputs = {"a", "b", "c"};
    vector<string> outputs = {"p0", "p1"};

    spot::parsed_formula parsed_formula = spot::parse_infix_psl(formula);
    spot::translator trans;
    trans.set_type(spot::postprocessor::Buchi);
    trans.set_pref(spot::postprocessor::SBAcc);
    auto automaton = trans.run(parsed_formula.f);

    auto outs = automaton->out(4);

    for (auto out : outs) {
        if (out.dst == 4) {
            // bdd_printdot(out.cond);

            int p1_num = automaton->register_ap("p1");
            bdd z1 = bdd_restrict(out.cond, bdd_ithvar(p1_num));
            bdd z2 = bdd_restrict(out.cond, bdd_nithvar(p1_num));
            // bdd z = bdd_restrict(out.cond, bdd_ithvar(p1_num)) &
            //         bdd_restrict(out.cond, bdd_nithvar(p1_num));
            bdd_printdot(z2);
        }
    }

    // for (auto& var : inputs) {
    //     cout << var << " : " << automaton->register_ap(var) << endl;
    // }
    // for (auto& var : outputs) {
    //     cout << var << " : " << automaton->register_ap(var) << endl;
    // }

    return EXIT_SUCCESS;
}