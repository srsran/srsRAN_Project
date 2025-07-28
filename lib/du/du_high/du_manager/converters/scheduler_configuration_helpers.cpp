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

#include "scheduler_configuration_helpers.h"
#include "../du_ue/du_ue.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/scheduler/config/logical_channel_config_factory.h"
#include "srsran/scheduler/config/sched_cell_config_helpers.h"

using namespace srsran;
using namespace srs_du;

std::optional<si_scheduling_config>
srsran::srs_du::make_si_scheduling_info_config(const du_cell_config& du_cfg, span<const units::bytes> si_message_lens)
{
  srsran_assert(si_message_lens.size() == (du_cfg.si_config.has_value() ? du_cfg.si_config->si_sched_info.size() : 0),
                "Number of SI messages does not match the number of SI payload sizes");

  std::optional<si_scheduling_config> sched_req;

  if (du_cfg.si_config.has_value()) {
    sched_req.emplace();
    sched_req->si_window_len_slots = du_cfg.si_config->si_window_len_slots;
    sched_req->si_messages.resize(du_cfg.si_config->si_sched_info.size());
    for (unsigned i = 0, sz = du_cfg.si_config->si_sched_info.size(); i != sz; ++i) {
      sched_req->si_messages[i].period_radio_frames = du_cfg.si_config->si_sched_info[i].si_period_radio_frames;
      sched_req->si_messages[i].msg_len             = si_message_lens[i];
      sched_req->si_messages[i].si_window_position  = du_cfg.si_config->si_sched_info[i].si_window_position;
    }
  }

  return sched_req;
}

/// Derives Scheduler Cell Configuration from DU Cell Configuration.
sched_cell_configuration_request_message
srsran::srs_du::make_sched_cell_config_req(du_cell_index_t                            cell_index,
                                           const srs_du::du_cell_config&              du_cfg,
                                           units::bytes                               sib1_len,
                                           const std::optional<si_scheduling_config>& si_sched_cfg)
{
  srsran_assert(sib1_len.value() > 0, "SIB1 payload size needs to be set");
  srsran_assert(si_sched_cfg.has_value()
                    ? si_sched_cfg->si_messages.size()
                    : 0 == (du_cfg.si_config.has_value() ? du_cfg.si_config->si_sched_info.size() : 0),
                "Number of SI messages does not match the number of SI payload sizes");

  sched_cell_configuration_request_message sched_req{};
  sched_req.cell_index           = cell_index;
  sched_req.cell_group_index     = (du_cell_group_index_t)cell_index; // No CA by default.
  sched_req.pci                  = du_cfg.pci;
  sched_req.dl_carrier           = du_cfg.dl_carrier;
  sched_req.ul_carrier           = du_cfg.ul_carrier;
  sched_req.dl_cfg_common        = du_cfg.dl_cfg_common;
  sched_req.ul_cfg_common        = du_cfg.ul_cfg_common;
  sched_req.scs_common           = du_cfg.scs_common;
  sched_req.ssb_config           = du_cfg.ssb_cfg;
  sched_req.dmrs_typeA_pos       = du_cfg.dmrs_typeA_pos;
  sched_req.tdd_ul_dl_cfg_common = du_cfg.tdd_ul_dl_cfg_common;
  sched_req.ntn_cs_koffset       = du_cfg.ntn_cs_koffset;
  sched_req.nof_beams            = 1;

  sched_req.coreset0     = du_cfg.coreset0_idx;
  sched_req.searchspace0 = du_cfg.searchspace0_idx;

  // Convert SIB1 and SI message info scheduling config.
  sched_req.sib1_payload_size = sib1_len;
  sched_req.si_scheduling     = si_sched_cfg;

  sched_req.pucch_guardbands = config_helpers::build_pucch_guardbands_list(
      du_cfg.pucch_cfg, du_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length());

  sched_req.zp_csi_rs_list = du_cfg.ue_ded_serv_cell_cfg.init_dl_bwp.pdsch_cfg->zp_csi_rs_res_list;

  if (du_cfg.ue_ded_serv_cell_cfg.csi_meas_cfg.has_value()) {
    sched_req.nzp_csi_rs_res_list = du_cfg.ue_ded_serv_cell_cfg.csi_meas_cfg->nzp_csi_rs_res_list;
  }

  if (du_cfg.ue_ded_serv_cell_cfg.ul_config.has_value() and
      du_cfg.ue_ded_serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg.has_value()) {
    sched_req.dl_data_to_ul_ack = du_cfg.ue_ded_serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->dl_data_to_ul_ack;
  }

  sched_req.rrm_policy_members = du_cfg.rrm_policy_members;

  sched_req.cfra_enabled = du_cfg.cfra_enabled;

  return sched_req;
}

sched_ue_config_request srsran::srs_du::create_scheduler_ue_config_request(const du_ue_context&         ue_ctx,
                                                                           const du_ue_resource_config& ue_res_cfg)
{
  sched_ue_config_request sched_cfg;

  sched_cfg.cells.emplace();
  sched_cfg.cells->resize(1);
  (*sched_cfg.cells)[0]               = ue_res_cfg.cell_group.cells[0];
  sched_cfg.sched_request_config_list = ue_res_cfg.cell_group.mcg_cfg.scheduling_request_config;
  // Add SRB and DRB logical channels.
  sched_cfg.lc_config_list.emplace();
  for (const auto& srb : ue_res_cfg.srbs) {
    auto& sched_lc_ch = sched_cfg.lc_config_list->emplace_back(
        config_helpers::create_default_logical_channel_config(srb_id_to_lcid(srb.srb_id)));
    sched_lc_ch.lc_group                  = srb.mac_cfg.lcg_id;
    sched_lc_ch.lc_sr_mask                = srb.mac_cfg.lc_sr_mask;
    sched_lc_ch.lc_sr_delay_timer_applied = srb.mac_cfg.lc_sr_delay_applied;
    sched_lc_ch.sr_id.emplace(srb.mac_cfg.sr_id);
  }
  for (const auto& drb : ue_res_cfg.drbs) {
    auto& sched_lc_ch =
        sched_cfg.lc_config_list->emplace_back(config_helpers::create_default_logical_channel_config(drb.lcid));
    sched_lc_ch.lc_group                  = drb.mac_cfg.lcg_id;
    sched_lc_ch.lc_sr_mask                = drb.mac_cfg.lc_sr_mask;
    sched_lc_ch.lc_sr_delay_timer_applied = drb.mac_cfg.lc_sr_delay_applied;
    sched_lc_ch.sr_id.emplace(drb.mac_cfg.sr_id);
    sched_lc_ch.rrm_policy.s_nssai = drb.s_nssai;
    sched_lc_ch.rrm_policy.plmn_id = ue_ctx.nr_cgi.plmn_id;
    sched_lc_ch.qos.emplace();
    sched_lc_ch.qos->qos          = *get_5qi_to_qos_characteristics_mapping(drb.qos.qos_desc.get_5qi());
    sched_lc_ch.qos->arp_priority = drb.qos.alloc_retention_prio.prio_level_arp;
    sched_lc_ch.qos->gbr_qos_info = drb.qos.gbr_qos_info;
  }
  sched_cfg.drx_cfg      = ue_res_cfg.cell_group.mcg_cfg.drx_cfg;
  sched_cfg.meas_gap_cfg = ue_res_cfg.meas_gap;

  return sched_cfg;
}
