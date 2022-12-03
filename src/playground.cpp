#include <iostream>
#include <spot/tl/formula.hh>
#include <spot/tl/parse.hh>
#include <spot/twa/fwd.hh>
#include <spot/twaalgos/postproc.hh>
#include <spot/twaalgos/translate.hh>
#include <string>
#include <vector>
using namespace std;

bool can_restrict_variable(bdd& bd, int variable, bool restriction_value) {
    bdd var_bdd = restriction_value ? bdd_ithvar(variable) : bdd_nithvar(variable);

    return bdd_and(bd, var_bdd) != bddfalse;
}

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

    ////////////////////// Playing with BDD
    int a_num = automaton->register_ap("a");
    int p0_num = automaton->register_ap("p0");
    int p1_num = automaton->register_ap("p1");
    bdd formu = (bdd_ithvar(p0_num) & bdd_nithvar(p1_num)) |
                (bdd_nithvar(p0_num) & bdd_ithvar(p1_num));

    bdd z1 = bdd_restrict(formu, bdd_ithvar(p0_num));
    cout << "Can restrict p1 to true? " << boolalpha
         << can_restrict_variable(z1, p1_num, true) << endl;
    cout << "Can restrict p0 to false? " << boolalpha
         << can_restrict_variable(z1, p0_num, false) << endl;

    return EXIT_SUCCESS;
}
