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

#include "pucch_res_test_builder_helper.h"
#include "../../../lib/scheduler/config/cell_configuration.h"
#include "srsran/scheduler/config/serving_cell_config_factory.h"

using namespace srsran;

static du_cell_config generate_du_cell_config(const bwp_uplink_common&               init_ul_bwp,
                                              std::optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common,
                                              const serving_cell_config&             base_ue_cfg,
                                              const pucch_builder_params&            pucch_cfg)
{
  du_cell_config cell_cfg;
  cell_cfg.ul_cfg_common.init_ul_bwp = init_ul_bwp;
  cell_cfg.tdd_ul_dl_cfg_common      = tdd_ul_dl_cfg_common;
  cell_cfg.ue_ded_serv_cell_cfg      = base_ue_cfg;
  cell_cfg.pucch_cfg                 = pucch_cfg;
  return cell_cfg;
}

pucch_res_builder_test_helper::pucch_res_builder_test_helper() : pucch_res_mgr(std::nullopt) {}

pucch_res_builder_test_helper::pucch_res_builder_test_helper(
    const bwp_uplink_common&               init_ul_bwp,
    std::optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common,
    const pucch_builder_params&            pucch_cfg) :
  required_info(pucch_res_builder_info{.init_ul_bwp          = init_ul_bwp,
                                       .tdd_ul_dl_cfg_common = tdd_ul_dl_cfg_common,
                                       .pucch_cfg            = pucch_cfg})
{
}

pucch_res_builder_test_helper::pucch_res_builder_test_helper(const cell_configuration&   cell_cfg,
                                                             const pucch_builder_params& pucch_cfg) :
  required_info(pucch_res_builder_info{.init_ul_bwp          = cell_cfg.ul_cfg_common.init_ul_bwp,
                                       .tdd_ul_dl_cfg_common = cell_cfg.tdd_cfg_common,
                                       .pucch_cfg            = pucch_cfg})
{
}

void pucch_res_builder_test_helper::setup(const bwp_uplink_common&               init_ul_bwp_,
                                          std::optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common_,
                                          const pucch_builder_params&            pucch_cfg)
{
  if (required_info.has_value()) {
    return;
  }
  required_info.emplace(pucch_res_builder_info{
      .init_ul_bwp = init_ul_bwp_, .tdd_ul_dl_cfg_common = tdd_ul_dl_cfg_common_, .pucch_cfg = pucch_cfg});
}

void pucch_res_builder_test_helper::setup(const cell_configuration& cell_cfg, const pucch_builder_params& pucch_cfg)
{
  setup(cell_cfg.ul_cfg_common.init_ul_bwp, cell_cfg.tdd_cfg_common, pucch_cfg);
}

bool pucch_res_builder_test_helper::add_build_new_ue_pucch_cfg(serving_cell_config& serv_cell_cfg)
{
  if (not required_info.has_value()) {
    return false;
  }

  if (not pucch_res_mgr.has_value()) {
    init_pucch_res_mgr(serv_cell_cfg);
  }

  // Create a temporary struct that will be fed to the function alloc_resources().
  srs_du::cell_group_config cell_group_cfg;
  cell_group_cfg.cells.emplace(0, cell_config_dedicated{.serv_cell_cfg = serv_cell_cfg});
  const bool alloc_outcome = pucch_res_mgr.value().alloc_resources(cell_group_cfg);
  if (not alloc_outcome) {
    return false;
  }
  // Copy the serv_cell_cfg configuration in cell_group_cfg the input serv_cell_cfg.
  serv_cell_cfg = cell_group_cfg.cells[0].serv_cell_cfg;
  return true;
}

void pucch_res_builder_test_helper::init_pucch_res_mgr(const serving_cell_config& base_ue_cfg)
{
  if (pucch_res_mgr.has_value()) {
    return;
  }
  pucch_res_mgr.emplace(srs_du::du_pucch_resource_manager(
      static_vector<du_cell_config, 1>{generate_du_cell_config(required_info.value().init_ul_bwp,
                                                               required_info.value().tdd_ul_dl_cfg_common,
                                                               base_ue_cfg,
                                                               required_info.value().pucch_cfg)},
      max_pucch_grants_per_slot));
}
