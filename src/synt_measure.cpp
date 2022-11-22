#include "synt_measure.h"

void SyntMeasures::get_json_object(json::object& obj) const {
    // General information
    json::array output_vars;
    std::transform(m_synt_instance.get_output_vars().begin(),
                   m_synt_instance.get_output_vars().end(),
                   std::back_inserter(output_vars),
                   [](const std::string& var) { return json::string(var); });
    json::array input_vars;
    std::transform(m_synt_instance.get_input_vars().begin(),
                   m_synt_instance.get_input_vars().end(),
                   std::back_inserter(input_vars),
                   [](const std::string& var) { return json::string(var); });

    obj.emplace("output_vars", output_vars);
    obj.emplace("input_vars", input_vars);
    obj.emplace("formula", this->m_synt_instance.get_formula_str());
    obj["total_time"] = this->m_total_time.time_elapsed();

    // Automaton information
    json::object automaton;
    automaton["is_built"] = this->m_is_automaton_built;
    if (this->m_is_automaton_built) {
        automaton["build_duration"] = this->m_aut_construct_time.get_duration();
        automaton["total_states"] =
            static_cast<int>(this->m_total_automaton_states);
        automaton["state_based_status"] = this->m_automaton_state_based_status;
    }
    obj.emplace("automaton", automaton);

    // Dependency information
    json::array tested_vars;
    for (const auto& var : this->m_tested_variables) {
        json::object var_obj;
        var_obj["name"] = var.name;
        var_obj["duration"] = var.duration;
        var_obj["is_dependent"] = var.is_dependent;

        tested_vars.emplace_back(var_obj);
    }
    obj.emplace("tested_variables", tested_vars);
}

void AutomatonSyntMeasure::get_json_object(json::object& obj) const {
    SyntMeasures::get_json_object(obj);

    json::object automaton_algo_obj;
    automaton_algo_obj["type"] = "automaton";
    automaton_algo_obj["total_pair_state"] = this->m_total_pair_states;
    automaton_algo_obj["search_pair_state_duration"] =
        this->m_search_pair_states_time.get_duration();
    automaton_algo_obj["pruned_state_based_status"] =
        this->m_prune_automaton_state_based_status;
    automaton_algo_obj["prune_automaton_duration"] =
        this->m_prune_automaton_time.get_duration();
    automaton_algo_obj["prune_total_states"] =
        static_cast<int>(this->m_total_prune_automaton_states);

    obj.emplace("algorithm", automaton_algo_obj);
}

ostream& operator<<(ostream& os, const SyntMeasures& sm) {
    json::object obj;
    sm.get_json_object(obj);
    os << json::serialize(obj);

    return os;
}
