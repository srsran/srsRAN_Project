/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pusch_td_resource_indices.h"

using namespace srsran;

static static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>
compute_pusch_td_resource_indices(span<const pusch_time_domain_resource_allocation> pusch_time_domain_list,
                                  const cell_configuration&                         cell_cfg,
                                  slot_point                                        pdcch_slot,
                                  unsigned                                          min_k1)
{
  // Compute list of PUSCH time domain resource index list relevant for the PUSCH slot.
  static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS> pusch_td_res_index_list;

  std::optional<unsigned> nof_full_ul_slots = std::nullopt;
  std::optional<unsigned> nof_full_dl_slots = std::nullopt;
  if (cell_cfg.is_tdd()) {
    nof_full_ul_slots = nof_full_ul_slots_per_tdd_period(cell_cfg.tdd_cfg_common.value());
    nof_full_dl_slots = nof_dl_slots_per_tdd_period(cell_cfg.tdd_cfg_common.value());
  }

  for (const pusch_time_domain_resource_allocation& pusch_td_res : pusch_time_domain_list) {
    if (not cell_cfg.is_tdd() or
        pusch_td_res.symbols.length() == get_active_tdd_ul_symbols(cell_cfg.tdd_cfg_common.value(),
                                                                   (pdcch_slot + pusch_td_res.k2).slot_index(),
                                                                   cyclic_prefix::NORMAL)
                                             .length()) {
      if ((not cell_cfg.is_tdd() or (*nof_full_dl_slots >= *nof_full_ul_slots)) and pusch_td_res.k2 <= min_k1) {
        // NOTE: Generated PUSCH time domain resources are sorted based on ascending order of k2 values and
        // descending order of nof. UL symbols for PUSCH.
        // [Implementation-defined] For DL heavy TDD configuration, only one entry in the PUSCH time domain
        // resources list with k2 value less than or equal to minimum value of k1(s) and, which matches nof. active
        // UL symbols in a slot is used.
        pusch_td_res_index_list.push_back(std::distance(pusch_time_domain_list.begin(), &pusch_td_res));
        break;
      }
      if (cell_cfg.is_tdd() and (*nof_full_ul_slots > *nof_full_dl_slots)) {
        // [Implementation-defined] For UL heavy TDD configuration multiple k2 values are considered for scheduling
        // since it allows multiple UL PDCCH allocations in the same slot for same UE but with different k2 values.
        pusch_td_res_index_list.push_back(std::distance(pusch_time_domain_list.begin(), &pusch_td_res));
      }
    }
  }

  return pusch_td_res_index_list;
}

span<const pusch_time_domain_resource_allocation>
srsran::get_pusch_time_domain_resource_table(const cell_configuration& cell_cfg, const search_space_info* ss_info)
{
  return ss_info != nullptr ? ss_info->pusch_time_domain_list
                            : cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().pusch_td_alloc_list;
}

static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>
srsran::get_pusch_td_resource_indices(const cell_configuration& cell_cfg,
                                      slot_point                pdcch_slot,
                                      const search_space_info*  ss_info)
{
  unsigned min_k1 = *std::min(cell_cfg.dl_data_to_ul_ack.begin(), cell_cfg.dl_data_to_ul_ack.end());
  if (ss_info != nullptr) {
    min_k1 = *std::min(ss_info->get_k1_candidates().begin(), ss_info->get_k1_candidates().end());
  }
  return compute_pusch_td_resource_indices(
      get_pusch_time_domain_resource_table(cell_cfg, ss_info), cell_cfg, pdcch_slot, min_k1);
}

std::vector<static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>>
srsran::get_pusch_td_resource_indices_per_slot(const cell_configuration& cell_cfg, const search_space_info* ss_info)
{
  // NOTE: [Implementation-defined] In case of FDD, we consider only one slot as all slots are similar unlike in TDD
  // where there can be DL/UL full or partial slots.
  const unsigned nof_slots = cell_cfg.is_tdd() ? nof_slots_per_tdd_period(*cell_cfg.tdd_cfg_common) : 1;

  // List circularly indexed by slot with the list of applicable PUSCH Time Domain resource indexes per slot.
  // NOTE: The list would be empty for UL slots.
  std::vector<static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>> pusch_td_list_per_slot;
  pusch_td_list_per_slot.resize(nof_slots);
  // Populate the initial list of applicable PUSCH time domain resources per slot.
  for (unsigned slot_idx = 0, e = nof_slots; slot_idx != e; ++slot_idx) {
    slot_point pdcch_slot{to_numerology_value(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs), slot_idx};
    if (cell_cfg.is_dl_enabled(pdcch_slot)) {
      pusch_td_list_per_slot[slot_idx] = get_pusch_td_resource_indices(cell_cfg, pdcch_slot);
    }
  }
  return pusch_td_list_per_slot;
}

std::vector<static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>>
srsran::get_fairly_distributed_pusch_td_resource_indices(const cell_configuration& cell_cfg,
                                                         const search_space_info*  ss_info)
{
  // NOTE: [Implementation-defined] In case of FDD, we consider only one slot as all slots are similar unlike in TDD
  // where there can be DL/UL full or partial slots.
  const unsigned nof_slots = cell_cfg.is_tdd() ? nof_slots_per_tdd_period(*cell_cfg.tdd_cfg_common) : 1;

  // List circularly indexed by slot with the list of applicable PUSCH Time Domain resource indexes per slot.
  // NOTE: The list would be empty for UL slots.
  std::vector<static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>> initial_pusch_td_list_per_slot =
      get_pusch_td_resource_indices_per_slot(cell_cfg, ss_info);

  // Fetch the relevant PUSCH time domain resource list.
  span<const pusch_time_domain_resource_allocation> pusch_time_domain_list =
      get_pusch_time_domain_resource_table(cell_cfg, ss_info);

  unsigned nof_dl_slots      = 0;
  unsigned nof_full_ul_slots = 0;
  if (cell_cfg.is_tdd()) {
    nof_dl_slots      = nof_dl_slots_per_tdd_period(cell_cfg.tdd_cfg_common.value());
    nof_full_ul_slots = nof_full_ul_slots_per_tdd_period(cell_cfg.tdd_cfg_common.value());
  }

  // Case of FDD or DL heavy TDD pattern.
  if (not cell_cfg.is_tdd() or (nof_dl_slots >= nof_full_ul_slots)) {
    return initial_pusch_td_list_per_slot;
  }

  // [Implementation-defined] Fairness is achieved by computing nof. UL PDCCHs to be scheduled per each PDCCH slot.
  // Then, iterating over UL slots finding the nearest PDCCH slot to it such that nof. UL PDCCHs at each PDCCH slot more
  // or less satisfies the earlier computed value.

  // Estimate the nof. UL PDCCHs that can be scheduled in each PDCCH slot.
  const auto nof_ul_pdcchs_per_dl_slot =
      static_cast<unsigned>(std::round(static_cast<double>(nof_full_ul_slots) / static_cast<double>(nof_dl_slots)));

  // List circularly indexed by slot with the list of applicable PUSCH Time Domain resource indexes per slot fairly
  // distributed among all the PDCCH slots.
  // NOTE: The list would be empty for UL slots.
  std::vector<static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>> final_pusch_td_list_per_slot;
  final_pusch_td_list_per_slot.resize(nof_slots);

  unsigned last_pdcch_slot_index_for_ul_slot = nof_slots;
  // Iterate from latest UL slot to earliest and find the closest PDCCH slot to that UL slot.
  // NOTE: There can be scenarios where the closest PDCCH slot may not be able to schedule PUSCH in the chosen UL slot.
  // In this case we move on next closest PDCCH slot, so on and so forth.
  for (int ul_slot_idx = nof_slots; ul_slot_idx >= 0; --ul_slot_idx) {
    // Skip if it's not a UL slot.
    // TODO: Revisit when scheduling of PUSCH over partial UL slots is supported.
    if (not is_tdd_full_ul_slot(cell_cfg.tdd_cfg_common.value(), ul_slot_idx)) {
      continue;
    }
    // Flag indicating whether a valid PDCCH slot for a given UL slot is found or not.
    bool no_pdcch_slot_found = true;
    for (int dl_slot_idx = ul_slot_idx; dl_slot_idx >= 0; --dl_slot_idx) {
      // Skip if it's not a DL slot.
      if (not has_active_tdd_dl_symbols(cell_cfg.tdd_cfg_common.value(), dl_slot_idx)) {
        continue;
      }
      // Check whether there is a PUSCH time domain resource with required k2 value for the PDCCH slot.
      unsigned required_k2 = ul_slot_idx - dl_slot_idx;
      auto*    it          = std::find_if(initial_pusch_td_list_per_slot[dl_slot_idx].begin(),
                              initial_pusch_td_list_per_slot[dl_slot_idx].end(),
                              [&pusch_time_domain_list, required_k2](unsigned pusch_td_res_idx) {
                                return pusch_time_domain_list[pusch_td_res_idx].k2 == required_k2;
                              });
      if (it == initial_pusch_td_list_per_slot[dl_slot_idx].end()) {
        continue;
      }
      // Store PDCCH slot index at which a valid PUSCH time domain resource was found to schedule PUSCH at given UL
      // slot.
      last_pdcch_slot_index_for_ul_slot = dl_slot_idx;
      // Skip if nof. PUSCH time domain resource indexes for this PDCCH slot exceed nof. UL PDCCHs that can be scheduled
      // in each PDCCH slot.
      if (final_pusch_td_list_per_slot[dl_slot_idx].size() >= nof_ul_pdcchs_per_dl_slot) {
        // Search for next PDCCH slot.
        continue;
      }
      // Store the nof. PUSCH time domain resource index for this PDCCH slot.
      final_pusch_td_list_per_slot[dl_slot_idx].push_back(*it);
      no_pdcch_slot_found = false;
      break;
    }
    // [Implementation-defined] If no PDCCH slot is found we pick the last valid PDCCH slot for this UL slot, regardless
    // of the restriction to not allow more than \c nof_ul_pdcchs_per_dl_slot UL PDCCHs per PDCCH slot.
    if (no_pdcch_slot_found) {
      unsigned required_k2 = ul_slot_idx - last_pdcch_slot_index_for_ul_slot;
      auto*    it          = std::find_if(initial_pusch_td_list_per_slot[last_pdcch_slot_index_for_ul_slot].begin(),
                              initial_pusch_td_list_per_slot[last_pdcch_slot_index_for_ul_slot].end(),
                              [&pusch_time_domain_list, required_k2](unsigned pusch_td_res_idx) {
                                return pusch_time_domain_list[pusch_td_res_idx].k2 == required_k2;
                              });
      // TODO: fix and compute the proper k2 value for the PUSCH time domain resource.
      if (it != initial_pusch_td_list_per_slot[last_pdcch_slot_index_for_ul_slot].end()) {
        final_pusch_td_list_per_slot[last_pdcch_slot_index_for_ul_slot].push_back(*it);
      }
    }
  }

  // Sort PUSCH time domain resource indexes (ascending order) in the final list of PUSCH time domain resources
  // maintained per slot.
  for (unsigned slot_idx = 0, e = nof_slots; slot_idx != e; ++slot_idx) {
    if (has_active_tdd_dl_symbols(cell_cfg.tdd_cfg_common.value(), slot_idx)) {
      std::sort(final_pusch_td_list_per_slot[slot_idx].begin(), final_pusch_td_list_per_slot[slot_idx].end());
    }
  }

  return final_pusch_td_list_per_slot;
}
