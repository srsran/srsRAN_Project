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

#include "srsran/scheduler/config/sched_cell_config_helpers.h"
#include "../../du_manager/ran_resource_management/pucch_resource_generator.h"
#include "srsran/scheduler/config/serving_cell_config_factory.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace srsran::config_helpers;

std::vector<sched_grid_resource>
srsran::config_helpers::build_pucch_guardbands_list(const pucch_builder_params& user_params, unsigned bwp_size)
{
  // Compute the cell PUCCH resource list, depending on which parameter that has been passed.
  std::vector<pucch_resource> res_list = srs_du::generate_cell_pucch_res_list(
      user_params.nof_ue_pucch_f1_res_harq.to_uint() * user_params.nof_cell_harq_pucch_res_sets +
          user_params.nof_sr_resources,
      user_params.nof_ue_pucch_f2_res_harq.to_uint() * user_params.nof_cell_harq_pucch_res_sets +
          user_params.nof_csi_resources,
      user_params.f1_params,
      user_params.f2_params,
      bwp_size);

  srsran_assert(not res_list.empty(), "The PUCCH resource list cannot be empty");

  std::vector<sched_grid_resource> pucch_guardbands{};

  auto list_contains_resource = [&pucch_guardbands](const sched_grid_resource& res) {
    return std::find(pucch_guardbands.begin(), pucch_guardbands.end(), res) != pucch_guardbands.end();
  };

  for (const auto& pucch_res : res_list) {
    sched_grid_resource res_0, res_1;
    res_0.prbs.set(pucch_res.starting_prb, pucch_res.starting_prb + 1);

    srsran_assert(variant_holds_alternative<pucch_format_1_cfg>(pucch_res.format_params) or
                      variant_holds_alternative<pucch_format_2_3_cfg>(pucch_res.format_params),
                  "Only PUCCH format 1 and 2 are currently supported");
    const unsigned starting_sym = variant_holds_alternative<pucch_format_1_cfg>(pucch_res.format_params)
                                      ? variant_get<pucch_format_1_cfg>(pucch_res.format_params).starting_sym_idx
                                      : variant_get<pucch_format_2_3_cfg>(pucch_res.format_params).starting_sym_idx;
    const unsigned nof_symbols  = variant_holds_alternative<pucch_format_1_cfg>(pucch_res.format_params)
                                      ? variant_get<pucch_format_1_cfg>(pucch_res.format_params).nof_symbols
                                      : variant_get<pucch_format_2_3_cfg>(pucch_res.format_params).nof_symbols;

    if (pucch_res.second_hop_prb.has_value()) {
      res_1.prbs.set(pucch_res.second_hop_prb.value(), pucch_res.second_hop_prb.value() + 1);
      res_0.symbols.set(starting_sym, starting_sym + nof_symbols / 2);
      res_1.symbols.set(starting_sym + nof_symbols / 2, starting_sym + nof_symbols);
    } else {
      res_0.symbols.set(starting_sym, starting_sym + nof_symbols);
    }

    if (not res_0.is_empty() and not list_contains_resource(res_0)) {
      pucch_guardbands.emplace_back(res_0);
    }
    if (not res_1.is_empty() and not list_contains_resource(res_1)) {
      pucch_guardbands.emplace_back(res_1);
    }
  }
  return pucch_guardbands;
}
