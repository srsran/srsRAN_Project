/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_cell_group_config_pool.h"
#include "srsran/scheduler/scheduler_configurator.h"

using namespace srsran;

du_cell_config_pool::du_cell_config_pool(const sched_cell_configuration_request_message& cell_cfg) :
  init_dl_bwp(cell_cfg.dl_cfg_common.init_dl_bwp)
{
  add_bwp(cell_cfg.dl_cfg_common.init_dl_bwp, nullptr);
}

ue_cell_creation_params du_cell_config_pool::update_ue(const cell_config_dedicated& ue_cell)
{
  ue_cell_creation_params ret;

  ret.dl_bwps.emplace(to_bwp_id(0), add_bwp(init_dl_bwp, &ue_cell.serv_cell_cfg.init_dl_bwp));
  for (const auto& bwp : ue_cell.serv_cell_cfg.dl_bwps) {
    ret.dl_bwps.emplace(
        to_bwp_id(bwp.bwp_id),
        add_bwp(bwp.bwp_dl_common.value(), bwp.bwp_dl_ded.has_value() ? &bwp.bwp_dl_ded.value() : nullptr));
  }
  return ret;
}

dl_bwp_config_ptr du_cell_config_pool::add_bwp(const bwp_downlink_common&    bwp_common,
                                               const bwp_downlink_dedicated* bwp_ded)
{
  dl_bwp_config out;

  // PDCCH config.
  bwp_pdcch_config pdcch_cfg;
  const auto&      pdcch_common = bwp_common.pdcch_common;
  // Coresets
  if (pdcch_common.coreset0.has_value()) {
    pdcch_cfg.coresets.emplace(to_coreset_id(0), pdcch_common.coreset0.value());
  }
  if (pdcch_common.common_coreset.has_value()) {
    pdcch_cfg.coresets.emplace(pdcch_common.common_coreset.value().id, pdcch_common.common_coreset.value());
  }
  if (bwp_ded != nullptr and bwp_ded->pdcch_cfg.has_value()) {
    // Dedicated coresets.
    auto& ded_coresets = bwp_ded->pdcch_cfg->coresets;
    for (auto& coreset : ded_coresets) {
      pdcch_cfg.coresets.emplace(coreset.id, coreset);
    }
  }
  // Search Spaces
  for (const auto& ss : pdcch_common.search_spaces) {
    pdcch_cfg.search_spaces.emplace(ss.get_id(), ss);
  }
  if (bwp_ded != nullptr and bwp_ded->pdcch_cfg.has_value()) {
    for (const auto& ss : bwp_ded->pdcch_cfg->search_spaces) {
      pdcch_cfg.search_spaces.emplace(ss.get_id(), ss);
    }
  }
  out.pdcch = pdcch_config_pool.create(pdcch_cfg);

  return dl_bwp_config_pool.create(out);
}

// class du_cell_group_config_pool

void du_cell_group_config_pool::add_cell(const sched_cell_configuration_request_message& cell_cfg)
{
  srsran_assert(cells.count(cell_cfg.cell_index) == 0, "Cell already exists");
  cells.emplace(cell_cfg.cell_index, cell_cfg);
}

ue_creation_params du_cell_group_config_pool::add_ue(const sched_ue_creation_request_message& cfg_req)
{
  // Create logical channel config.
  auto lc_ch_list = lc_ch_pool.create(cfg_req.cfg.lc_config_list.has_value() ? cfg_req.cfg.lc_config_list.value()
                                                                             : std::vector<logical_channel_config>{});

  // Create UE dedicated cell configs.
  slotted_id_vector<du_cell_index_t, ue_cell_creation_params> cell_cfgs;
  if (cfg_req.cfg.cells.has_value()) {
    for (const auto& cell : cfg_req.cfg.cells.value()) {
      cell_cfgs.emplace(cell.serv_cell_cfg.cell_index, cells.at(cell.serv_cell_cfg.cell_index).update_ue(cell));
    }
  }

  return ue_creation_params{cfg_req.cfg, lc_ch_list, cell_cfgs};
}

ue_reconfig_params du_cell_group_config_pool::reconf_ue(const sched_ue_reconfiguration_message& cfg_req)
{
  std::optional<logical_channel_config_list_ptr> lc_ch_list;
  if (cfg_req.cfg.lc_config_list.has_value()) {
    lc_ch_list = lc_ch_pool.create(cfg_req.cfg.lc_config_list.value());
  }

  // Create UE dedicated cell configs.
  slotted_id_vector<du_cell_index_t, ue_cell_creation_params> cell_cfgs;
  if (cfg_req.cfg.cells.has_value()) {
    for (const auto& cell : cfg_req.cfg.cells.value()) {
      cell_cfgs.emplace(cell.serv_cell_cfg.cell_index, cells.at(cell.serv_cell_cfg.cell_index).update_ue(cell));
    }
  }

  return ue_reconfig_params{cfg_req.cfg, lc_ch_list, cell_cfgs};
}
