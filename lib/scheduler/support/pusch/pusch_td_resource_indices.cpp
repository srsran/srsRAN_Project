/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "pusch_td_resource_indices.h"
#include "../../config/ue_configuration.h"
#include "pusch_default_time_allocation.h"
#include "srsran/srslog/logger.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

using pusch_index_list = static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>;

/// Get minimum value for k1 given the common and dedicated configurations.
static unsigned get_min_k1(const cell_configuration& cell_cfg, const search_space_info* ss_info)
{
  unsigned min_k1 = *std::min(cell_cfg.dl_data_to_ul_ack.begin(), cell_cfg.dl_data_to_ul_ack.end());
  if (ss_info != nullptr) {
    min_k1 = *std::min(ss_info->get_k1_candidates().begin(), ss_info->get_k1_candidates().end());
  }
  return min_k1;
}

static span<const pusch_time_domain_resource_allocation>
get_pusch_time_domain_resource_table(const cell_configuration& cell_cfg, const search_space_info* ss_info)
{
  return ss_info != nullptr ? ss_info->pusch_time_domain_list
                            : cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().pusch_td_alloc_list;
}

/// Determine PUSCH TD resources for the FDD mode.
static pusch_index_list get_fdd_pusch_td_resource_indices(const cell_configuration& cell_cfg,
                                                          const search_space_info*  ss_info)
{
  srsran_sanity_check(not cell_cfg.is_tdd(), "Function expects FDD config");
  const unsigned min_k1                 = get_min_k1(cell_cfg, ss_info);
  auto           pusch_time_domain_list = get_pusch_time_domain_resource_table(cell_cfg, ss_info);

  pusch_index_list result;
  for (unsigned i = 0; i != pusch_time_domain_list.size(); ++i) {
    if (pusch_time_domain_list[i].k2 <= min_k1) {
      result.push_back(i);
    }
  }
  return result;
}

pusch_index_list srsran::get_pusch_td_resource_indices(const cell_configuration& cell_cfg,
                                                       slot_point                pdcch_slot,
                                                       const search_space_info*  ss_info)
{
  if (not cell_cfg.is_tdd()) {
    // FDD case.
    return get_fdd_pusch_td_resource_indices(cell_cfg, ss_info);
  }

  // TDD case.
  const unsigned min_k1                 = get_min_k1(cell_cfg, ss_info);
  auto           pusch_time_domain_list = get_pusch_time_domain_resource_table(cell_cfg, ss_info);
  const unsigned nof_full_ul_slots      = nof_full_ul_slots_per_tdd_period(cell_cfg.tdd_cfg_common.value());
  const unsigned nof_full_dl_slots      = nof_dl_slots_per_tdd_period(cell_cfg.tdd_cfg_common.value());
  const bool     is_dl_heavy            = nof_full_dl_slots >= nof_full_ul_slots;

  pusch_index_list result;
  for (unsigned td_idx = 0; td_idx != pusch_time_domain_list.size(); ++td_idx) {
    const pusch_time_domain_resource_allocation& pusch_td_res = pusch_time_domain_list[td_idx];
    // [Implementation-defined] PUSCH on partial UL slots is not supported.
    if (is_tdd_partial_ul_slot(cell_cfg.tdd_cfg_common.value(), (pdcch_slot + pusch_td_res.k2).slot_index())) {
      continue;
    }

    if (is_dl_heavy and pusch_td_res.k2 <= min_k1) {
      // DL-heavy case.
      // [Implementation-defined] For DL heavy TDD configuration, in the PUSCH time domain resources list, we allow only
      // entries with the same k2 value that less than or equal to minimum value of k1(s); these multiple entries can
      // have different symbols.
      if (not result.empty() and
          std::any_of(result.begin(),
                      result.end(),
                      [candidate_k2 = pusch_td_res.k2, pusch_time_domain_list](unsigned td_idx_it) {
                        return candidate_k2 != pusch_time_domain_list[td_idx_it].k2 ? true : false;
                      })) {
        break;
      }
      result.push_back(td_idx);
    }
    if (not is_dl_heavy) {
      // UL-heavy case.
      // [Implementation-defined] For UL heavy TDD configuration multiple k2 values are considered for scheduling
      // since it allows multiple UL PDCCH allocations in the same slot for same UE but with different k2 values.
      result.push_back(td_idx);
    }
  }
  return result;
}

std::vector<pusch_index_list> srsran::get_pusch_td_resource_indices_per_slot(const cell_configuration& cell_cfg,
                                                                             const search_space_info*  ss_info)
{
  // Note: [Implementation-defined] In case of FDD, we only consider one slot.
  if (not cell_cfg.is_tdd()) {
    return {get_fdd_pusch_td_resource_indices(cell_cfg, ss_info)};
  }

  // NOTE: [Implementation-defined] In case of FDD, we consider only one slot as all slots are similar unlike in TDD
  // where there can be DL/UL full or partial slots.
  const unsigned           nof_slots = nof_slots_per_tdd_period(*cell_cfg.tdd_cfg_common);
  const subcarrier_spacing scs       = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs;

  // List circularly indexed by slot with the list of applicable PUSCH Time Domain resource indexes per slot.
  // NOTE: The list would be empty for UL slots.
  std::vector<pusch_index_list> pusch_td_list_per_slot(nof_slots);
  // Populate the initial list of applicable PUSCH time domain resources per slot.
  for (unsigned slot_idx = 0, e = nof_slots; slot_idx != e; ++slot_idx) {
    slot_point pdcch_slot{to_numerology_value(scs), slot_idx};
    if (cell_cfg.is_dl_enabled(pdcch_slot)) {
      pusch_td_list_per_slot[slot_idx] = get_pusch_td_resource_indices(cell_cfg, pdcch_slot);
    }
  }
  return pusch_td_list_per_slot;
}

static std::optional<unsigned> find_td_index_with_k2(span<const pusch_time_domain_resource_allocation> pusch_res_list,
                                                     span<const unsigned>                              valid_indexes,
                                                     unsigned                                          k2)
{
  auto* it = std::find_if(valid_indexes.begin(), valid_indexes.end(), [&pusch_res_list, k2](unsigned pusch_td_res_idx) {
    return pusch_res_list[pusch_td_res_idx].k2 == k2;
  });
  if (it == valid_indexes.end()) {
    return std::nullopt;
  }
  return *it;
}

std::vector<pusch_index_list>
srsran::get_fairly_distributed_pusch_td_resource_indices(const cell_configuration& cell_cfg,
                                                         const search_space_info*  ss_info)
{
  // List circularly indexed by slot with the list of applicable PUSCH Time Domain resource indexes per slot.
  // NOTE: The list would be empty for UL slots.
  std::vector<pusch_index_list> initial_pusch_td_list_per_slot =
      get_pusch_td_resource_indices_per_slot(cell_cfg, ss_info);

  // In FDD case, we do not need to proceed further.
  if (not cell_cfg.is_tdd()) {
    return initial_pusch_td_list_per_slot;
  }

  const unsigned nof_dl_slots      = nof_dl_slots_per_tdd_period(cell_cfg.tdd_cfg_common.value());
  const unsigned nof_full_ul_slots = nof_full_ul_slots_per_tdd_period(cell_cfg.tdd_cfg_common.value());

  // In DL-heavy case, we do not need to proceed further.
  if (nof_dl_slots >= nof_full_ul_slots) {
    return initial_pusch_td_list_per_slot;
  }

  const unsigned nof_slots = nof_slots_per_tdd_period(*cell_cfg.tdd_cfg_common);

  // Fetch the relevant PUSCH time domain resource list.
  span<const pusch_time_domain_resource_allocation> pusch_time_domain_list =
      get_pusch_time_domain_resource_table(cell_cfg, ss_info);
  const unsigned max_k2 = pusch_time_domain_list.back().k2;

  // [Implementation-defined] Fairness is achieved by computing nof. UL PDCCHs to be scheduled per each PDCCH slot.
  // Then, iterating over UL slots finding the nearest PDCCH slot to it such that nof. UL PDCCHs at each PDCCH slot more
  // or less satisfies the earlier computed value.

  // Estimate the nof. UL PDCCHs that can be scheduled in each PDCCH slot.
  const auto nof_ul_pdcchs_per_dl_slot =
      static_cast<unsigned>(std::round(static_cast<double>(nof_full_ul_slots) / static_cast<double>(nof_dl_slots)));

  // List circularly indexed by slot with the list of applicable PUSCH Time Domain resource indexes per slot fairly
  // distributed among all the PDCCH slots.
  // NOTE: The list would be empty for UL slots.
  std::vector<pusch_index_list> final_pusch_td_list_per_slot(nof_slots);

  // Iterate from latest UL slot to earliest and find the closest PDCCH slot to that UL slot.
  // NOTE: There can be scenarios where the closest PDCCH slot may not be able to schedule PUSCH in the chosen UL slot.
  // In this case we move on next closest PDCCH slot, so on and so forth.
  for (unsigned candidate_idx = 0; candidate_idx != nof_slots; ++candidate_idx) {
    unsigned ul_slot_idx = nof_slots - candidate_idx;
    // Skip if it's not a UL slot.
    // TODO: Revisit when scheduling of PUSCH over partial UL slots is supported.
    if (not is_tdd_full_ul_slot(cell_cfg.tdd_cfg_common.value(), ul_slot_idx)) {
      continue;
    }
    // Flag indicating whether a valid PDCCH slot for a given UL slot is found or not.
    bool                    no_pdcch_slot_found = true;
    std::optional<unsigned> last_valid_k2;
    for (unsigned k2 = 0; k2 <= max_k2; ++k2) {
      unsigned dl_slot_idx = (nof_slots + ul_slot_idx - k2) % nof_slots;
      // Skip if it's not a DL slot.
      if (not has_active_tdd_dl_symbols(cell_cfg.tdd_cfg_common.value(), dl_slot_idx)) {
        continue;
      }
      // Check whether there is a PUSCH time domain resource with required k2 value for the PDCCH slot.
      std::optional<unsigned> idx =
          find_td_index_with_k2(pusch_time_domain_list, initial_pusch_td_list_per_slot[dl_slot_idx], k2);
      if (not idx.has_value()) {
        continue;
      }
      // Store PDCCH slot index at which a valid PUSCH time domain resource was found to schedule PUSCH at given UL
      // slot.
      last_valid_k2 = k2;
      // Skip if nof. PUSCH time domain resource indexes for this PDCCH slot exceed nof. UL PDCCHs that can be scheduled
      // in each PDCCH slot.
      if (final_pusch_td_list_per_slot[dl_slot_idx].size() >= nof_ul_pdcchs_per_dl_slot) {
        // Search for next PDCCH slot.
        continue;
      }
      // Store the nof. PUSCH time domain resource index for this PDCCH slot.
      final_pusch_td_list_per_slot[dl_slot_idx].push_back(*idx);
      no_pdcch_slot_found = false;
      break;
    }

    // Note: This should not happen if the config passed the validation.
    srsran_sanity_check(
        last_valid_k2.has_value(), "Invalid TDD pattern which leads to UL slot index={} with no valid k2", ul_slot_idx);

    // [Implementation-defined] If no PDCCH slot is found we pick the last valid PDCCH slot for this UL slot, regardless
    // of the restriction to not allow more than \c nof_ul_pdcchs_per_dl_slot UL PDCCHs per PDCCH slot.
    if (no_pdcch_slot_found) {
      std::optional<unsigned> min_k2;
      for (const auto& pusch_time_domain : pusch_time_domain_list) {
        min_k2 = std::min(min_k2.value_or(pusch_time_domain.k2), pusch_time_domain.k2);
      }
      const unsigned required_k2      = last_valid_k2.value();
      const unsigned pdcch_slot_index = (ul_slot_idx + nof_slots - required_k2) % nof_slots;

      // If the required k2 value is less than the minimum k2 value in the PUSCH time domain resource list, then we look
      // for the minimum k2 value that is greater than the DL-UL transmission period, as this is the PDCCH slot closest
      // to the PUSCH slot.
      std::optional<unsigned> candidate_required_k2;
      if (required_k2 < min_k2.value()) {
        for (const auto& pusch_time_domain : pusch_time_domain_list) {
          if (pusch_time_domain.k2 > cell_cfg.tdd_cfg_common.value().pattern1.dl_ul_tx_period_nof_slots) {
            candidate_required_k2 =
                std::min(candidate_required_k2.value_or(pusch_time_domain.k2), pusch_time_domain.k2);
          }
        }
      } else {
        candidate_required_k2 = required_k2;
      }
      // If a valid PUSCH time domain resource is found for the required k2 value, then we store it.
      std::optional<unsigned> pusch_td_res_idx_for_required_k2 = std::nullopt;
      if (candidate_required_k2.has_value()) {
        auto& init_push_list = initial_pusch_td_list_per_slot[pdcch_slot_index];
        auto* it             = std::find_if(init_push_list.begin(),
                                init_push_list.end(),
                                [&pusch_time_domain_list, candidate_required_k2](unsigned pusch_td_res_idx) {
                                  return pusch_time_domain_list[pusch_td_res_idx].k2 == candidate_required_k2.value();
                                });
        if (it != init_push_list.end()) {
          pusch_td_res_idx_for_required_k2.emplace(*it);
        }
      }
      if (pusch_td_res_idx_for_required_k2.has_value()) {
        final_pusch_td_list_per_slot[pdcch_slot_index].push_back(*pusch_td_res_idx_for_required_k2);
      } else {
        srslog::basic_logger& logger = srslog::fetch_basic_logger("SCHED", false);
        logger.warning("No valid PUSCH time domain resource found for UL slot dx={}", ul_slot_idx);
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
