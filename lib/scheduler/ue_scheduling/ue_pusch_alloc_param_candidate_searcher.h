/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "../support/pusch/pusch_td_resource_indices.h"
#include "ue.h"

namespace srsran {

/// \brief This class assists with the search of PUSCH allocation parameters given an UE config that ensures a valid UE
/// PUSCH resource allocation in the Cell Resource Grid. The search iterates through different SearchSpace and PUSCH
/// time-domain resource candidates, returning only the valid ones. This class operates as a range with begin()
/// and end() iterators, and only searches for valid candidates in a lazy fashion. That means that we only compute
/// all valid candidates if we iterate from begin() to end().
///
/// To be a valid candidate, the following conditions must be met:
/// - SearchSpace must be part of UE dedicated configuration only.
/// - The slot must have sufficient UL symbols, given the PUSCH time-domain resource.
/// - For \c is_retx equal to true, we ensure the candidates will lead to allocations with the same number of UL
/// symbols and RNTI type as the previous HARQ allocation.
class ue_pusch_alloc_param_candidate_searcher
{
  using search_space_candidate_list = static_vector<const search_space_info*, MAX_NOF_SEARCH_SPACE_PER_BWP>;
  using ss_iter = static_vector<const search_space_info*, MAX_NOF_SEARCH_SPACE_PER_BWP>::const_iterator;

public:
  struct iterator;

  /// Parameters for a given PUSCH candidate.
  struct candidate {
    /// Chosen Search Space.
    const search_space_info& ss() const { return **ss_it; }

    /// Chosen PUSCH Time Domain Resource.
    const pusch_time_domain_resource_allocation& pusch_td_res() const
    {
      return (*ss_it)->pusch_time_domain_list[time_res];
    }

    /// Chosen PUSCH Time Domain Resource Index.
    unsigned pusch_td_res_index() const { return time_res; }

    /// Chosen DCI UL RNTI Config Type.
    dci_ul_rnti_config_type dci_ul_rnti_cfg_type() const
    {
      if (rnti_type.has_value()) {
        return rnti_type.value();
      }
      return ss().get_ul_dci_format() == dci_ul_format::f0_0 ? dci_ul_rnti_config_type::c_rnti_f0_0
                                                             : dci_ul_rnti_config_type::c_rnti_f0_1;
    }

    bool operator==(const candidate& other) const
    {
      return ss_it == other.ss_it and time_res == other.time_res and rnti_type == other.rnti_type;
    }
    bool operator!=(const candidate& other) const { return not(*this == other); }
    /// A candidate being less than another means that the first has higher priority than the second.
    bool operator<(const candidate& other) const
    {
      return (ss_it < other.ss_it) or (ss_it == other.ss_it and time_res < other.time_res);
    }
    bool operator>=(const candidate& other) const { return not(*this < other); }
    bool operator<=(const candidate& other) const { return *this < other or *this == other; }
    bool operator>(const candidate& other) const { return not(*this < other) and *this != other; }

  private:
    friend struct iterator;
    friend class ue_pusch_alloc_param_candidate_searcher;

    candidate(ss_iter ss, unsigned time_res_, std::optional<dci_ul_rnti_config_type> rnti_type_) :
      ss_it(ss), time_res(time_res_), rnti_type(rnti_type_)
    {
    }

    ss_iter                                ss_it;
    unsigned                               time_res;
    std::optional<dci_ul_rnti_config_type> rnti_type;
  };

  /// Iterator for list of candidates.
  struct iterator {
    using value_type = candidate;
    using reference  = value_type&;
    using pointer    = value_type*;

    iterator(ue_pusch_alloc_param_candidate_searcher& parent_, ss_iter ss_it, unsigned time_res_) :
      parent(&parent_), current(ss_it, time_res_, parent->preferred_rnti_type)
    {
      // Cell is not part of UE configured cells.
      if (parent->ue_cc == nullptr) {
        return;
      }

      parent->iterate_until_valid_candidate_found(current);
      if (not parent->is_candidate_valid(current)) {
        // No valid candidates found and iteration finished.
        current.ss_it    = parent->ss_candidate_list.end();
        current.time_res = 0;
      }
    }

    candidate&       operator*() { return current; }
    const candidate& operator*() const { return current; }
    candidate*       operator->() { return &current; }
    const candidate* operator->() const { return &current; }

    /// Incrementing the iterator means moving up one position in the list of candidates.
    iterator& operator++()
    {
      // Cell is not part of UE configured cells.
      if (parent->ue_cc == nullptr) {
        return *this;
      }

      ++current.time_res;
      // When we increment the time_res, it may be pointing to the end of the PUSCH time domain resource list.
      // So, we need to move to the next SearchSpace and reset time resource to 0.
      if (current.time_res >= (*current.ss_it)->pusch_time_domain_list.size()) {
        current.time_res = 0;
        ++current.ss_it;
        if (current.ss_it != parent->ss_candidate_list.end()) {
          parent->valid_ss_pusch_td_res_indices =
              get_pusch_td_resource_indices(parent->ue_cc->cfg().cell_cfg_common, parent->pdcch_slot, *current.ss_it);
        } else {
          // Iteration finished.
          return *this;
        }
      }
      parent->iterate_until_valid_candidate_found(current);
      if (not parent->is_candidate_valid(current)) {
        // No valid candidates found and iteration finished.
        current.ss_it    = parent->ss_candidate_list.end();
        current.time_res = 0;
      }
      return *this;
    }

    bool operator==(const iterator& other) const { return current == other.current; }
    bool operator!=(const iterator& other) const { return not(*this == other); }

  private:
    ue_pusch_alloc_param_candidate_searcher* parent;
    candidate                                current;
  };

  /// Create a searcher for UE PUSCH parameters.
  ue_pusch_alloc_param_candidate_searcher(const ue&              ue_ref_,
                                          du_cell_index_t        cell_index,
                                          ul_harq_process&       ul_harq_,
                                          slot_point             pdcch_slot_,
                                          span<const slot_point> slots_with_no_pusch_space_) :
    ue_ref(ue_ref_),
    ue_cc(ue_ref.find_cell(cell_index)),
    slots_with_no_pusch_space(slots_with_no_pusch_space_),
    ul_harq(ul_harq_),
    is_retx(not ul_harq.empty()),
    pdcch_slot(pdcch_slot_)
  {
    // Cell is not part of UE configured cells.
    if (ue_cc == nullptr) {
      return;
    }

    if (not ul_harq.empty()) {
      preferred_rnti_type = ul_harq.last_tx_params().dci_cfg_type;
    }

    // Generate list of Search Spaces.
    generate_ss_candidates();

    // Generate valid PUSCH Time Domain Resource Indices for first SearchSpace in the SS candidate list.
    if (not ss_candidate_list.empty()) {
      valid_ss_pusch_td_res_indices =
          get_pusch_td_resource_indices(ue_cc->cfg().cell_cfg_common, pdcch_slot, *ss_candidate_list.begin());
    }
  }

  /// Get begin to the list of candidates.
  iterator begin() { return iterator{*this, ss_candidate_list.begin(), 0}; }
  iterator end() { return iterator{*this, ss_candidate_list.end(), 0}; }

  /// Returns whether there are candidates or not.
  bool is_empty() { return ss_candidate_list.empty() or valid_ss_pusch_td_res_indices.empty() or begin() == end(); }

private:
  // Generate Search Space candidates for a given HARQ.
  void generate_ss_candidates()
  {
    // Update alloc_params list.
    ss_candidate_list = ue_cc->get_active_ul_search_spaces(pdcch_slot, preferred_rnti_type);
    // Consider SearchSpaces only in UE dedicated configuration.
    span<const search_space_configuration> ue_ded_cfg_ss =
        ue_cc->cfg().cfg_dedicated().init_dl_bwp.pdcch_cfg->search_spaces;
    bool is_css_in_ue_dedicated_cfg =
        std::any_of(ue_ded_cfg_ss.begin(), ue_ded_cfg_ss.end(), [](const search_space_configuration& ss) {
          return ss.is_common_search_space();
        });
    // Filter out any SearchSpace configured which belong to common configuration only if there is no Common
    // SearchSpace (CSS) configured in UE dedicated configuration. If there is at least one CSS configured in UE
    // dedicated configuration then we need to consider also the SearchSpaces configured in common configuration as they
    // have higher priority due to lower SearchSpace index.
    if (not is_css_in_ue_dedicated_cfg) {
      const search_space_info** ss_iterator = ss_candidate_list.begin();
      while (ss_iterator != ss_candidate_list.end()) {
        const auto* ue_ded_cfg_ss_it = std::find_if(
            ue_ded_cfg_ss.begin(),
            ue_ded_cfg_ss.end(),
            [id = (*ss_iterator)->cfg->get_id()](const search_space_configuration& ss) { return ss.get_id() == id; });
        if (ue_ded_cfg_ss_it == ue_ded_cfg_ss.end()) {
          ss_iterator = ss_candidate_list.erase(ss_iterator);
        } else {
          ++ss_iterator;
        }
      }
    }
  }

  // Check if a candidate has valid parameters for an allocation.
  bool is_candidate_valid(const candidate& current) const
  {
    // Check whether SearchSpace is valid.
    if (current.ss_it == nullptr or current.ss_it == ss_candidate_list.end()) {
      return false;
    }

    // Check whether PUSCH Time Domain resource index is valid.
    if (current.time_res >= (*current.ss_it)->pusch_time_domain_list.size()) {
      return false;
    }

    const slot_point pusch_slot = pdcch_slot + current.pusch_td_res().k2;

    // Check whether PUSCH slot is UL enabled.
    if (not ue_cc->cfg().cell_cfg_common.is_ul_enabled(pusch_slot)) {
      return false;
    }

    // Check whether PUSCH time domain resource fits in UL symbols of the slot.
    if (ue_cc->cfg().cell_cfg_common.get_nof_ul_symbol_per_slot(pusch_slot) !=
        current.pusch_td_res().symbols.length()) {
      return false;
    }

    // Check whether there is any space left in PUSCH slot for allocation.
    if (slots_with_no_pusch_space.end() !=
        std::find(slots_with_no_pusch_space.begin(), slots_with_no_pusch_space.end(), pusch_slot)) {
      return false;
    }

    // If it is a retx, we need to ensure we use a time_domain_resource with the same number of symbols as used for
    // the first transmission.
    if (is_retx and current.pusch_td_res().symbols.length() != ul_harq.last_tx_params().nof_symbols) {
      return false;
    }

    return true;
  }

  // Iterate over the list of candidates until a valid one is found.
  void iterate_until_valid_candidate_found(candidate& current)
  {
    for (; current.ss_it != ss_candidate_list.end(); ++current.ss_it) {
      // NOTE: At this point UE is no longer in fallback mode.
      // Skip SearchSpaces without PDCCH candidates to be monitored in this slot.
      if (not(*current.ss_it)
                 ->get_pdcch_candidates(
                     ue_cc->get_aggregation_level(
                         ue_cc->link_adaptation_controller().get_effective_cqi(), **current.ss_it, false),
                     pdcch_slot)
                 .empty()) {
        for (; current.time_res < (*current.ss_it)->pusch_time_domain_list.size(); ++current.time_res) {
          if (std::find(valid_ss_pusch_td_res_indices.begin(), valid_ss_pusch_td_res_indices.end(), current.time_res) ==
              valid_ss_pusch_td_res_indices.end()) {
            continue;
          }
          if (is_candidate_valid(current)) {
            // Valid candidate found.
            return;
          }
        }
      }
    }
  }

  // UE being allocated.
  const ue& ue_ref;
  // UE cell being allocated.
  const ue_cell* ue_cc;

  // Slots with no RBs left for PUSCH allocation.
  span<const slot_point> slots_with_no_pusch_space;

  // UL HARQ considered for allocation.
  const ul_harq_process& ul_harq;
  // Whether the current search is for a newTx or a reTx.
  const bool is_retx;
  // List of SearchSpace candidates for the UL HARQ considered for allocation.
  search_space_candidate_list ss_candidate_list;
  // Valid PUSCH time domain resource indices for current SearchSpace.
  static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS> valid_ss_pusch_td_res_indices;
  // RNTI type used to generate ss_candidate_list.
  std::optional<dci_ul_rnti_config_type> preferred_rnti_type;

  // PDCCH slot point used to verify if the PUSCH fits a UL slot.
  slot_point pdcch_slot;
};

} // namespace srsran
