/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/du/du_update_config_helpers.h"
#include "../du_manager/ran_resource_management/pucch_resource_generator.h"

using namespace srsran;

static unsigned get_max_rb_on_bwp_left_side(const pucch_resource& res, unsigned bwp_size)
{
  // Return true if the given PRB is on the first half of the PRB of a BWP.
  auto is_on_bwp_left_side = [bwp_size](unsigned prb) { return prb < bwp_size / 2; };

  srsran_assert((res.format == srsran::pucch_format::FORMAT_1 and
                 variant_holds_alternative<pucch_format_1_cfg>(res.format_params)) or
                    (res.format == srsran::pucch_format::FORMAT_2 or
                     variant_holds_alternative<pucch_format_2_3_cfg>(res.format_params)),
                "Only PUCCH Format 1 and 2 currently supported.");

  unsigned prach_f_start = 0;

  if (res.format == srsran::pucch_format::FORMAT_1) {
    const unsigned nof_prbs_f1 = 1U;

    // Check if first hop and second hop separately.
    if (is_on_bwp_left_side(res.starting_prb + nof_prbs_f1)) {
      prach_f_start = std::max(res.starting_prb + nof_prbs_f1, prach_f_start);
    }
    if (res.second_hop_prb.has_value() and is_on_bwp_left_side(res.second_hop_prb.value() + nof_prbs_f1)) {
      prach_f_start = std::max(res.second_hop_prb.value() + nof_prbs_f1, prach_f_start);
    }
  }
  if (res.format == srsran::pucch_format::FORMAT_2) {
    // Check if first hop and second hop separately.
    const unsigned nof_prbs = variant_get<pucch_format_2_3_cfg>(res.format_params).nof_prbs;

    if (is_on_bwp_left_side(res.starting_prb + nof_prbs)) {
      prach_f_start = std::max(res.starting_prb + nof_prbs, prach_f_start);
    }
    if (res.second_hop_prb.has_value() and is_on_bwp_left_side(res.second_hop_prb.value() + nof_prbs)) {
      prach_f_start = std::max(res.second_hop_prb.value() + nof_prbs, prach_f_start);
    }
  }
  return prach_f_start;
}

unsigned srsran::config_helpers::compute_prach_frequency_start(const pucch_builder_params& user_params,
                                                               unsigned                    bwp_size)
{
  // This is to preserve a guardband between the PUCCH and PRACH.
  const unsigned pucch_to_prach_guardband = 3;

  // Compute the cell PUCCH resource list, depending on which parameter that has been passed.
  const unsigned              nof_pucch_f2_res_f1 = 1U;
  std::vector<pucch_resource> res_list            = srs_du::generate_pucch_res_list_given_number(
      user_params.nof_ue_pucch_f1_res_harq.to_uint() + user_params.nof_sr_resources.to_uint(),
      user_params.nof_ue_pucch_f2_res_harq.to_uint() + nof_pucch_f2_res_f1,
      user_params.f1_params,
      user_params.f2_params,
      bwp_size);

  srsran_assert(not res_list.empty(), "The PUCCH resource list cannot be empty");

  unsigned prach_f_start = 0;

  for (const auto& pucch_res : res_list) {
    prach_f_start = std::max(get_max_rb_on_bwp_left_side(pucch_res, bwp_size), prach_f_start);
  }

  return prach_f_start + pucch_to_prach_guardband;
}
