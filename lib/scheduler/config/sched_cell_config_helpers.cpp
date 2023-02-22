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
#include "srsran/scheduler/config/serving_cell_config_factory.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace srsran::config_helpers;

std::vector<sched_grid_resource> srsran::config_helpers::build_pucch_guardbands_list(const uplink_config& ul_cfg)
{
  std::vector<sched_grid_resource> pucch_guardbands{};

  auto list_contain_resource = [&pucch_guardbands](const sched_grid_resource& res) {
    return std::find(pucch_guardbands.begin(), pucch_guardbands.end(), res) != pucch_guardbands.end();
  };

  for (const auto& pucch_res : ul_cfg.init_ul_bwp.pucch_cfg->pucch_res_list) {
    sched_grid_resource res_0, res_1;
    res_0.prbs.set(pucch_res.starting_prb, pucch_res.starting_prb + 1);

    if (pucch_res.intraslot_freq_hopping) {
      res_1.prbs.set(pucch_res.second_hop_prb, pucch_res.second_hop_prb + 1);
      switch (pucch_res.format) {
        case pucch_format::FORMAT_1: {
          res_0.symbols.set(pucch_res.format_1.starting_sym_idx,
                            pucch_res.format_1.starting_sym_idx + pucch_res.format_1.nof_symbols / 2);
          res_1.symbols.set(pucch_res.format_1.starting_sym_idx + pucch_res.format_1.nof_symbols / 2,
                            pucch_res.format_1.starting_sym_idx + pucch_res.format_1.nof_symbols);
          break;
        }
        case pucch_format::FORMAT_2: {
          res_0.symbols.set(pucch_res.format_2.starting_sym_idx,
                            pucch_res.format_2.starting_sym_idx + pucch_res.format_2.nof_symbols / 2);
          res_1.symbols.set(pucch_res.format_2.starting_sym_idx + pucch_res.format_2.nof_symbols / 2,
                            pucch_res.format_2.starting_sym_idx + pucch_res.format_2.nof_symbols);
          break;
        }
        default:
          report_error("Only PUCCH format 1 currently supported");
      }
    } else {
      switch (pucch_res.format) {
        case pucch_format::FORMAT_1: {
          res_0.symbols.set(pucch_res.format_1.starting_sym_idx,
                            pucch_res.format_1.starting_sym_idx + pucch_res.format_1.nof_symbols);
          break;
        }
        case pucch_format::FORMAT_2: {
          res_0.symbols.set(pucch_res.format_2.starting_sym_idx,
                            pucch_res.format_2.starting_sym_idx + pucch_res.format_2.nof_symbols);
          break;
        }
        default:
          report_error("Only PUCCH format 1 currently supported");
      }
    }

    if (not res_0.is_empty() and not list_contain_resource(res_0)) {
      pucch_guardbands.emplace_back(res_0);
    }
    if (not res_1.is_empty() and not list_contain_resource(res_1)) {
      pucch_guardbands.emplace_back(res_1);
    }
  }
  return pucch_guardbands;
}
