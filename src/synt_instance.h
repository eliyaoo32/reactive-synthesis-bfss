#ifndef REACTIVE_SYNTHESIS_BFSS_SYNT_INSTANCE_H
#define REACTIVE_SYNTHESIS_BFSS_SYNT_INSTANCE_H

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/range/join.hpp>
#include <iostream>
#include <spot/tl/formula.hh>
#include <spot/tl/parse.hh>
#include <spot/twa/fwd.hh>
#include <spot/twaalgos/postproc.hh>
#include <spot/twaalgos/translate.hh>
#include <string>
#include <vector>

class SyntInstance {
   private:
    std::vector<std::string> m_all_vars;
    std::vector<std::string> m_input_vars;
    std::vector<std::string> m_output_vars;
    std::string m_formula;

    void build_all_vars();

   public:
    SyntInstance(const std::string& input_str, const std::string& output_str,
                 const std::string& formula_str);

    SyntInstance(std::vector<std::string> inputs, std::vector<std::string> outputs,
                 std::string& formula);

    [[nodiscard]] const std::vector<std::string>& get_input_vars() const {
        return m_input_vars;
    }

    [[nodiscard]] const std::vector<std::string>& get_output_vars() const {
        return m_output_vars;
    }

    // Return all the variables exclude requested in "excluded"
    void all_vars_excluded(std::vector<std::string>& dst,
                           const std::vector<std::string>& excluded);

    std::string& get_formula_str() { return m_formula; }

    friend std::ostream& operator<<(std::ostream& out, SyntInstance& instance);

    friend spot::twa_graph_ptr construct_automaton(SyntInstance& synt_instance);

    friend spot::formula construct_formula(SyntInstance& synt_instance);
};

spot::twa_graph_ptr construct_automaton(SyntInstance& synt_instance);

spot::formula construct_formula(SyntInstance& synt_instance);

std::ostream& operator<<(std::ostream& out, SyntInstance& instance);

#endif  // REACTIVE_SYNTHESIS_BFSS_SYNT_INSTANCE_H
