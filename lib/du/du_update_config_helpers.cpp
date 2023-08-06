/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "../du_manager/ran_resource_management/pucch_resource_generator.h"

using namespace srsran;

// Helper that computes the greatest RB index used by the PUCCH resources on the BWP's left side. Note that the PUCCH
// resources are located at in 2 separate blocks, at both sides of the BWP, i.e., 1 block on the left side (where
// indices is 0, 1, 2, ...) and 1 block on the right side (where indices are ..., N_BWP_RBs -3, N_BWP_RBs-2,
// N_BWP_RBs-1) of the BWP. This function considers only the block of PUCCH resources on the BWP's left side and returns
// the RB with the greatest index of this block.
static unsigned greatest_used_rb_on_bwp_left_side(const pucch_resource& res, unsigned bwp_size)
{
  // Return true if the given PRB is on the BWP's left side (i.e., if the PRB index is less than the BWP's size measured
  // in PRBs).
  auto is_on_bwp_left_side = [bwp_size](unsigned prb) { return prb < bwp_size / 2; };

  srsran_assert((res.format == srsran::pucch_format::FORMAT_1 and
                 variant_holds_alternative<pucch_format_1_cfg>(res.format_params)) or
                    (res.format == srsran::pucch_format::FORMAT_2 or
                     variant_holds_alternative<pucch_format_2_3_cfg>(res.format_params)),
                "Only PUCCH Format 1 and 2 currently supported.");

  unsigned max_rb_idx_on_left_side = 0;

  if (res.format == srsran::pucch_format::FORMAT_1) {
    const unsigned nof_prbs_f1 = 1U;

    // Check if first hop and second hop separately.
    if (is_on_bwp_left_side(res.starting_prb + nof_prbs_f1)) {
      max_rb_idx_on_left_side = std::max(res.starting_prb + nof_prbs_f1, max_rb_idx_on_left_side);
    }
    if (res.second_hop_prb.has_value() and is_on_bwp_left_side(res.second_hop_prb.value() + nof_prbs_f1)) {
      max_rb_idx_on_left_side = std::max(res.second_hop_prb.value() + nof_prbs_f1, max_rb_idx_on_left_side);
    }
  }
  if (res.format == srsran::pucch_format::FORMAT_2) {
    // Check if first hop and second hop separately.
    const unsigned nof_prbs = variant_get<pucch_format_2_3_cfg>(res.format_params).nof_prbs;

    if (is_on_bwp_left_side(res.starting_prb + nof_prbs)) {
      max_rb_idx_on_left_side = std::max(res.starting_prb + nof_prbs, max_rb_idx_on_left_side);
    }
    if (res.second_hop_prb.has_value() and is_on_bwp_left_side(res.second_hop_prb.value() + nof_prbs)) {
      max_rb_idx_on_left_side = std::max(res.second_hop_prb.value() + nof_prbs, max_rb_idx_on_left_side);
    }
  }
  return max_rb_idx_on_left_side;
}

unsigned srsran::config_helpers::compute_prach_frequency_start(const pucch_builder_params& user_params,
                                                               unsigned                    bwp_size,
                                                               bool                        is_long_prach)
{
  // This is to preserve a guardband between the PUCCH and PRACH.
  const unsigned pucch_to_prach_guardband = is_long_prach ? 0U : 3U;

  // Compute the cell PUCCH resource list, depending on which parameter that has been passed.
  const std::vector<pucch_resource> res_list = srs_du::generate_cell_pucch_res_list(
      user_params.nof_ue_pucch_f1_res_harq.to_uint() * user_params.nof_cell_harq_pucch_res_sets +
          user_params.nof_sr_resources,
      user_params.nof_ue_pucch_f2_res_harq.to_uint() * user_params.nof_cell_harq_pucch_res_sets +
          user_params.nof_csi_resources,
      user_params.f1_params,
      user_params.f2_params,
      bwp_size);

  srsran_assert(not res_list.empty(), "The PUCCH resource list cannot be empty");

  unsigned prach_f_start = 0;

  for (const auto& pucch_res : res_list) {
    prach_f_start = std::max(greatest_used_rb_on_bwp_left_side(pucch_res, bwp_size), prach_f_start);
  }

  return prach_f_start + pucch_to_prach_guardband;
}
