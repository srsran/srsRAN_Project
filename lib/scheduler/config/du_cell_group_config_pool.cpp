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
  init_dl_bwp(cell_cfg.dl_cfg_common.init_dl_bwp), init_ul_bwp(cell_cfg.ul_cfg_common.init_ul_bwp)
{
}

ue_cell_config_ptr du_cell_config_pool::update_ue(const serving_cell_config& ue_cell)
{
  ue_cell_res_config ret;
  ret.cell_index = ue_cell.cell_index;
  if (ue_cell.ul_config.has_value()) {
    ret.ul_config.emplace(ul_cfg_pool.create(ue_cell.ul_config.value()));
  }
  if (ue_cell.pdsch_serv_cell_cfg.has_value()) {
    ret.pdsch_serv_cell_cfg = pdsch_serv_cell_pool.create(ue_cell.pdsch_serv_cell_cfg.value());
  }
  if (ue_cell.csi_meas_cfg.has_value()) {
    ret.csi_meas_cfg = csi_meas_config_pool.create(ue_cell.csi_meas_cfg.value());
  }
  ret.tag_id = ue_cell.tag_id;

  add_bwp(ret,
          to_bwp_id(0),
          init_dl_bwp,
          &ue_cell.init_dl_bwp,
          &init_ul_bwp,
          ue_cell.ul_config.has_value() ? &ue_cell.ul_config->init_ul_bwp : nullptr);
  for (const auto& bwp : ue_cell.dl_bwps) {
    add_bwp(ret,
            bwp.bwp_id,
            bwp.bwp_dl_common.value(),
            bwp.bwp_dl_ded.has_value() ? &bwp.bwp_dl_ded.value() : nullptr,
            nullptr,
            nullptr);
  }

  return cell_cfg_pool.create(ret);
}

void du_cell_config_pool::add_bwp(ue_cell_res_config&           out,
                                  bwp_id_t                      bwp_id,
                                  const bwp_downlink_common&    dl_bwp_common,
                                  const bwp_downlink_dedicated* dl_bwp_ded,
                                  const bwp_uplink_common*      ul_bwp_common,
                                  const bwp_uplink_dedicated*   ul_bwp_ded)
{
  // Create BWP config.
  bwp_config bwp_cfg;
  bwp_cfg.bwp_id = bwp_id;
  // BWP DL Common
  bwp_cfg.dl_common = dl_bwp_common;
  if (dl_bwp_common.pdcch_common.coreset0.has_value()) {
    auto& coreset0 = dl_bwp_common.pdcch_common.coreset0.value();
    bwp_cfg.coresets.emplace(coreset0.id, coreset_config_pool.create(coreset0));
    out.coresets.emplace(coreset0.id, bwp_cfg.coresets[coreset0.id]);
  }
  if (dl_bwp_common.pdcch_common.common_coreset.has_value()) {
    auto& common_coreset = dl_bwp_common.pdcch_common.common_coreset.value();
    bwp_cfg.coresets.emplace(common_coreset.id, coreset_config_pool.create(common_coreset));
    out.coresets.emplace(common_coreset.id, bwp_cfg.coresets[common_coreset.id]);
  }
  for (const auto& ss : dl_bwp_common.pdcch_common.search_spaces) {
    bwp_cfg.search_spaces.emplace(ss.get_id(), ss);
  }

  // BWP DL Dedicated
  if (dl_bwp_ded != nullptr) {
    bwp_cfg.dl_ded = *dl_bwp_ded;
    for (const coreset_configuration& cs : dl_bwp_ded->pdcch_cfg->coresets) {
      // TS 38.331, "PDCCH-Config" - In case network reconfigures control resource set with the same
      // ControlResourceSetId as used for commonControlResourceSet configured via PDCCH-ConfigCommon,
      // the configuration from PDCCH-Config always takes precedence and should not be updated by the UE based on
      // servingCellConfigCommon.
      bwp_cfg.coresets.emplace(cs.id, coreset_config_pool.create(cs));
      out.coresets.emplace(cs.id, bwp_cfg.coresets[cs.id]);
    }
    for (const auto& ss : dl_bwp_ded->pdcch_cfg->search_spaces) {
      bwp_cfg.search_spaces.emplace(ss.get_id(), ss);
      out.search_spaces.emplace(ss.get_id(), ss);
    }
  }

  // BWP UL Common
  if (ul_bwp_common != nullptr) {
    bwp_cfg.ul_common = *ul_bwp_common;
  }

  // BWP UL Dedicated
  if (ul_bwp_ded != nullptr) {
    bwp_cfg.ul_ded = *ul_bwp_ded;
  }
  out.bwps.emplace(bwp_id, bwp_config_pool.create(bwp_cfg));
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
  slotted_id_vector<du_cell_index_t, ue_cell_config_ptr> cell_cfgs;
  if (cfg_req.cfg.cells.has_value()) {
    for (const auto& cell : cfg_req.cfg.cells.value()) {
      cell_cfgs.emplace(cell.serv_cell_cfg.cell_index,
                        cells.at(cell.serv_cell_cfg.cell_index).update_ue(cell.serv_cell_cfg));
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
  slotted_id_vector<du_cell_index_t, ue_cell_config_ptr> cell_cfgs;
  if (cfg_req.cfg.cells.has_value()) {
    for (const auto& cell : cfg_req.cfg.cells.value()) {
      cell_cfgs.emplace(cell.serv_cell_cfg.cell_index,
                        cells.at(cell.serv_cell_cfg.cell_index).update_ue(cell.serv_cell_cfg));
    }
  }

  return ue_reconfig_params{cfg_req.cfg, lc_ch_list, cell_cfgs};
}
