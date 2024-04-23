/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pucch_res_test_builder_helper.h"

using namespace srsran;

static du_cell_config generate_du_cell_config(const bwp_uplink_common&          init_ul_bwp,
                                              optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common,
                                              const serving_cell_config&        base_ue_cfg,
                                              const pucch_builder_params&       pucch_cfg)
{
  du_cell_config cell_cfg;
  cell_cfg.ul_cfg_common.init_ul_bwp = init_ul_bwp;
  cell_cfg.tdd_ul_dl_cfg_common      = tdd_ul_dl_cfg_common;
  cell_cfg.ue_ded_serv_cell_cfg      = base_ue_cfg;
  cell_cfg.pucch_cfg                 = pucch_cfg;
  return cell_cfg;
}

pucch_res_builder_test_helper::pucch_res_builder_test_helper() : pucch_res_mgr(nullopt) {}

pucch_res_builder_test_helper::pucch_res_builder_test_helper(const bwp_uplink_common&          init_ul_bwp,
                                                             optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common,
                                                             const serving_cell_config&        base_ue_cfg,
                                                             const pucch_builder_params&       pucch_cfg) :
  pucch_res_mgr(srs_du::du_pucch_resource_manager(
      static_vector<du_cell_config, 1>{
          generate_du_cell_config(init_ul_bwp, tdd_ul_dl_cfg_common, base_ue_cfg, pucch_cfg)},
      max_pucch_grants_per_slot))
{
}

void pucch_res_builder_test_helper::setup(const bwp_uplink_common&          init_ul_bwp,
                                          optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common,
                                          const serving_cell_config&        base_ue_cfg,
                                          const pucch_builder_params&       pucch_cfg)
{
  srsran_assert(not pucch_res_mgr.has_value(), "The object has been already set-up by the constructor");
  pucch_res_mgr.emplace(
      srs_du::du_pucch_resource_manager(static_vector<du_cell_config, 1>{generate_du_cell_config(
                                            init_ul_bwp, tdd_ul_dl_cfg_common, base_ue_cfg, pucch_cfg)},
                                        max_pucch_grants_per_slot));
}

bool pucch_res_builder_test_helper::add_build_new_ue_pucch_cfg(serving_cell_config& serv_cell_cfg)
{
  if (not pucch_res_mgr.has_value()) {
    return false;
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
