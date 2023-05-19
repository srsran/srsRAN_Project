/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/scheduler/config/sched_cell_config_helpers.h"
#include "srsran/scheduler/config/serving_cell_config_factory.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace srsran::config_helpers;

std::vector<sched_grid_resource> srsran::config_helpers::build_pucch_guardbands_list(const uplink_config& ul_cfg)
{
  std::vector<sched_grid_resource> pucch_guardbands{};

  auto list_contains_resource = [&pucch_guardbands](const sched_grid_resource& res) {
    return std::find(pucch_guardbands.begin(), pucch_guardbands.end(), res) != pucch_guardbands.end();
  };

  for (const auto& pucch_res : ul_cfg.init_ul_bwp.pucch_cfg->pucch_res_list) {
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
