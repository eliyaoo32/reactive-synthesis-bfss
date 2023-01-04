#ifndef REACTIVE_SYNTHESIS_BFSS_SYNTHESIS_UTILS_H
#define REACTIVE_SYNTHESIS_BFSS_SYNTHESIS_UTILS_H

#include <iostream>
#include <spot/misc/optionmap.hh>
#include <spot/twaalgos/degen.hh>
#include <spot/twaalgos/determinize.hh>
#include <spot/twaalgos/game.hh>
#include <spot/twaalgos/hoa.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/mealy_machine.hh>
#include <spot/twaalgos/sbacc.hh>
#include <spot/twaalgos/sccfilter.hh>
#include <spot/twaalgos/synthesis.hh>
#include <spot/twaalgos/translate.hh>
#include <vector>

#include "find_deps_by_automaton.h"
#include "synt_instance.h"
#include "utils.h"

spot::twa_graph_ptr ntgba2dpa(const spot::twa_graph_ptr& aut, bool force_sbacc);

void remove_ap_from_automaton(const spot::twa_graph_ptr& automaton,
                              vector<string>& variables);

spot::twa_graph_ptr get_dpa_from_nba(spot::twa_graph_ptr nba, spot::synthesis_info& gi,
                                     AutomatonSyntMeasure& synt_measures,
                                     SyntInstance& synt_instance);

spot::twa_graph_ptr get_nba_for_synthesis(SyntInstance& synt_instance,
                                          spot::synthesis_info& gi,
                                          AutomatonSyntMeasure& synt_measures,
                                          std::ostream& verbose);

// Return if realizable
bool synthesis_formula(SyntInstance& synt_instance, spot::synthesis_info& gi,
                       AutomatonSyntMeasure& synt_measures, std::ostream& verbose,
                       bool find_deps, spot::mealy_like& ml);

#endif