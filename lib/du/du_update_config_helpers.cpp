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

#include "srsran/du/du_update_config_helpers.h"
#include "srsran/ran/pusch/pusch_constants.h"
#include "srsran/scheduler/config/pucch_resource_generator.h"
#include "srsran/scheduler/config/srs_builder_params.h"

using namespace srsran;
using namespace config_helpers;

/// Helper function that, for a given PUCCH resource, returns the (inner) interval of PRBs that are not used for the
/// PUCCH resource. We define "inner interval" as the interval of PRBs spanning through the center of the BWP. This
/// function assumes that (i) the PUCCH resources are located in 2 separate blocks, at both external sides of the BWP,
/// and that (ii) a PUCCH resource can be at the same time on both of these sides (i.e., inter-slot frequency hopping).
static prb_interval find_pucch_inner_prbs(const pucch_resource& res, unsigned bwp_size)
{
  // Return true if the given PRB is on the BWP's left side (i.e., if the PRB index is less than the BWP's size measured
  // in PRBs).
  auto is_on_bwp_left_side = [bwp_size](unsigned prb) { return prb < bwp_size / 2; };
  // Return true if the given PRB is on the BWP's right side (approx., if the PRB index is more than half the BWP's size
  // measured in PRBs).
  // NOTE: for odd bwp_size and the for central PRB, both is_on_bwp_right_side() and is_on_bwp_left_side() are false.
  auto is_on_bwp_right_side = [bwp_size](unsigned prb) { return prb >= bwp_size - bwp_size / 2; };

  constexpr unsigned nof_prbs_f0_f1_f4 = 1U;

  const unsigned nof_prbs = res.format == pucch_format::FORMAT_2 or res.format == pucch_format::FORMAT_3
                                ? std::get<pucch_format_2_3_cfg>(res.format_params).nof_prbs
                                : nof_prbs_f0_f1_f4;

  unsigned max_rb_idx_on_left_side  = 0;
  unsigned min_rb_idx_on_right_side = bwp_size;

  if (is_on_bwp_left_side(res.starting_prb + nof_prbs)) {
    max_rb_idx_on_left_side = std::max(res.starting_prb + nof_prbs, max_rb_idx_on_left_side);
  }
  if (res.second_hop_prb.has_value() and is_on_bwp_left_side(res.second_hop_prb.value() + nof_prbs)) {
    max_rb_idx_on_left_side = std::max(res.second_hop_prb.value() + nof_prbs, max_rb_idx_on_left_side);
  }
  if (is_on_bwp_right_side(res.starting_prb)) {
    min_rb_idx_on_right_side = std::min(res.starting_prb, min_rb_idx_on_right_side);
  }
  if (res.second_hop_prb.has_value() and is_on_bwp_right_side(res.second_hop_prb.value())) {
    min_rb_idx_on_right_side = std::min(res.second_hop_prb.value(), min_rb_idx_on_right_side);
  }

  return prb_interval{max_rb_idx_on_left_side, min_rb_idx_on_right_side};
}

prb_interval config_helpers::find_largest_prb_interval_without_pucch(const pucch_builder_params& user_params,
                                                                     unsigned                    bwp_size)
{
  // Compute the cell PUCCH resource list, depending on which parameter that has been passed.
  const std::vector<pucch_resource>& res_list = config_helpers::generate_cell_pucch_res_list(
      user_params.nof_ue_pucch_f0_or_f1_res_harq.to_uint() * user_params.nof_cell_harq_pucch_res_sets +
          user_params.nof_sr_resources,
      user_params.nof_ue_pucch_f2_or_f3_or_f4_res_harq.to_uint() * user_params.nof_cell_harq_pucch_res_sets +
          user_params.nof_csi_resources,
      user_params.f0_or_f1_params,
      user_params.f2_or_f3_or_f4_params,
      bwp_size,
      user_params.max_nof_symbols);
  srsran_assert(not res_list.empty(), "The PUCCH resource list cannot be empty");

  prb_interval prb_without_pucch = {0, bwp_size};

  for (const auto& pucch_res : res_list) {
    prb_interval inner_prbs = find_pucch_inner_prbs(pucch_res, bwp_size);
    prb_without_pucch.set(std::max(prb_without_pucch.start(), inner_prbs.start()),
                          std::min(prb_without_pucch.stop(), inner_prbs.stop()));
  }
  return prb_without_pucch;
}

unsigned config_helpers::compute_prach_frequency_start(const pucch_builder_params& user_params,
                                                       unsigned                    bwp_size,
                                                       bool                        is_long_prach)
{
  // This is to preserve a guardband between the PUCCH and PRACH.
  const unsigned pucch_to_prach_guardband = is_long_prach ? 0U : 3U;
  return find_largest_prb_interval_without_pucch(user_params, bwp_size).start() + pucch_to_prach_guardband;
}

void config_helpers::compute_nof_sr_csi_pucch_res(pucch_builder_params&   user_params,
                                                  unsigned                max_pucch_grants_per_slot,
                                                  float                   sr_period_msec,
                                                  std::optional<unsigned> csi_period_msec)
{
  // [Implementation-defined] In the following, we compute the estimated number of PUCCH resources that are needed for
  // SR and CSI; we assume we cannot allocate more than max_pucch_grants_per_slot - 1U (1 is reserved for HARQ-ACK)
  // overall SR and CSI per slot, and the required resources are weighted based on CSI and SR period, respectively
  // (i.e., if the SR period is half of the CSI's, we allocate twice the resources to SR).
  // If the CSI is not enabled, we only allocate resources for SR.

  const unsigned max_pucch_grants_per_sr_csi = max_pucch_grants_per_slot - 1U;

  if (csi_period_msec.has_value()) {
    const unsigned required_nof_sr_resources =
        std::ceil(static_cast<double>(max_pucch_grants_per_sr_csi * csi_period_msec.value()) /
                  (static_cast<double>(sr_period_msec) + static_cast<double>(csi_period_msec.value())));

    user_params.nof_sr_resources = std::min(required_nof_sr_resources, user_params.nof_sr_resources);

    const unsigned required_nof_csi_resources =
        std::ceil(static_cast<double>(max_pucch_grants_per_sr_csi * sr_period_msec) /
                  (static_cast<double>(sr_period_msec) + static_cast<double>(csi_period_msec.value())));

    user_params.nof_csi_resources = std::min(required_nof_csi_resources, user_params.nof_csi_resources);
  } else {
    user_params.nof_sr_resources  = std::min(max_pucch_grants_per_sr_csi, user_params.nof_sr_resources);
    user_params.nof_csi_resources = 0;
  }
}

bounded_integer<unsigned, 1, 14>
config_helpers::compute_max_nof_pucch_symbols(const srs_builder_params& user_srs_params)
{
  // [Implementation-defined] In the following, we compute the maximum number of PUCCH symbols that can be used in a
  // slot based on the PUCCH and SRS configurations. The maximum number of PUCCH symbols is computed so that PUCCH and
  // SRS resources occupy all symbols in a slot and in such a way that they do not overlap each other.
  return user_srs_params.srs_period.has_value()
             ? NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - user_srs_params.max_nof_symbols.to_uint()
             : NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
}

void config_helpers::recompute_pusch_time_domain_resources(
    std::vector<pusch_time_domain_resource_allocation>& td_alloc_list,
    const srs_builder_params&                           user_srs_params,
    const std::optional<tdd_ul_dl_config_common>&       tdd_cfg)
{
  const unsigned td_alloc_size = td_alloc_list.size();
  const unsigned max_srs_symb  = user_srs_params.max_nof_symbols.value();
  const auto     symb_per_srs  = static_cast<unsigned>(user_srs_params.nof_symbols);

  td_alloc_list.reserve(pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS);

  if (tdd_cfg.has_value()) {
    const unsigned nof_dl_slots      = nof_dl_slots_per_tdd_period(tdd_cfg.value());
    const unsigned nof_full_ul_slots = nof_full_ul_slots_per_tdd_period(tdd_cfg.value());

    // DL-heavy case, we need to add extra resources.
    if (nof_dl_slots >= nof_full_ul_slots) {
      // For TDD and DL-heavy case, duplicate the PUSCH resources by adding extra resources with the same k2 value but
      // different symbols. We add resource in this order, until we reached capacity the full of the vector,
      // - Resources with ofdm_symbol_range{0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - max_srs_symb}, placed at the end of the
      // vector. This is to give priority to the SRS resources with more symbols.
      // - Resources with ofdm_symbol_range{0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - symb_per_srs}, placed after the first
      // resources with full symbols.

      // Number of resources over symbols {0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - max_srs_symb}. Given that we copy the
      // existing resources of the vector, cap this to the current vector size.
      const unsigned nof_res_sym_0_to_srs_max =
          pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS > td_alloc_size
              ? std::min(td_alloc_size, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS - td_alloc_size)
              : 0U;
      // Number of resources over symbols {0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - symb_per_srs}. Given that we copy the
      // existing resources of the vector, cap this to the current vector size.
      const unsigned nof_res_sym_0_to_symb_per_srs =
          nof_res_sym_0_to_srs_max != 0 and max_srs_symb != symb_per_srs
              ? std::min(td_alloc_size,
                         pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS - td_alloc_size - nof_res_sym_0_to_srs_max)
              : 0U;

      // If any, add resources with ofdm_symbol_range{0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - symb_per_srs} first.
      for (unsigned res_cnt = 0U; res_cnt != nof_res_sym_0_to_symb_per_srs and
                                  td_alloc_list.size() != pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS;
           ++res_cnt) {
        const auto& res_full_symbols = td_alloc_list[res_cnt];
        td_alloc_list.push_back(pusch_time_domain_resource_allocation{
            res_full_symbols.k2,
            res_full_symbols.map_type,
            ofdm_symbol_range{0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - symb_per_srs}});
      }

      // If any, add resources with ofdm_symbol_range{0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - max_srs_symb} at the end.
      for (unsigned res_cnt = 0U;
           res_cnt != nof_res_sym_0_to_srs_max and td_alloc_list.size() != pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS;
           ++res_cnt) {
        const auto& res_full_symbols = td_alloc_list[res_cnt];
        td_alloc_list.push_back(pusch_time_domain_resource_allocation{
            res_full_symbols.k2,
            res_full_symbols.map_type,
            ofdm_symbol_range{0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - max_srs_symb}});
      }
    }
    // UL-heavy case, we do not add extra resources, as the scheduler cannot currently handle (correctly) multiple OFDM
    // symbols for the same k2 value.
    else {
      for (auto& td_res : td_alloc_list) {
        td_res.symbols = ofdm_symbol_range{0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - max_srs_symb};
      }
    }
  }
  // For FDD, we duplicate the only resource by adding extra ones with the same k2 value but different symbols,
  // at symb_per_srs steps.
  else {
    for (unsigned srs_sym = symb_per_srs;
         srs_sym <= max_srs_symb and td_alloc_list.size() != pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS;
         srs_sym += symb_per_srs) {
      const auto& res_full_symbols = td_alloc_list.front();
      td_alloc_list.push_back(
          pusch_time_domain_resource_allocation{res_full_symbols.k2,
                                                res_full_symbols.map_type,
                                                ofdm_symbol_range{0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - srs_sym}});
    }
  }

  // Sorting in ascending order is performed to reduce the latency with which PUSCH is scheduled.
  std::sort(td_alloc_list.begin(), td_alloc_list.end(), [](const auto& lhs, const auto& rhs) {
    return lhs.k2 < rhs.k2 or (lhs.k2 == rhs.k2 and lhs.symbols.length() > rhs.symbols.length());
  });
}
