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

#include "ue_cell.h"
#include "../support/dmrs_helpers.h"
#include "../support/mcs_calculator.h"
#include "../support/pdcch_aggregation_level_calculator.h"
#include "../support/prbs_calculator.h"
#include "../support/sch_pdu_builder.h"
#include "srsran/scheduler/scheduler_feedback_handler.h"

using namespace srsran;

/// Number of UL HARQs reserved per UE (Implementation-defined)
constexpr unsigned NOF_UL_HARQS = 16;

ue_cell::ue_cell(du_ue_index_t                ue_index_,
                 rnti_t                       crnti_val,
                 const ue_cell_configuration& ue_cell_cfg_,
                 ue_harq_timeout_notifier     harq_timeout_notifier) :
  ue_index(ue_index_),
  cell_index(ue_cell_cfg_.cell_cfg_common.cell_index),
  harqs(crnti_val,
        (unsigned)ue_cell_cfg_.cfg_dedicated().pdsch_serv_cell_cfg->nof_harq_proc,
        NOF_UL_HARQS,
        harq_timeout_notifier,
        ue_cell_cfg_.cell_cfg_common.ntn_cs_koffset),
  crnti_(crnti_val),
  cell_cfg(ue_cell_cfg_.cell_cfg_common),
  ue_cfg(&ue_cell_cfg_),
  expert_cfg(cell_cfg.expert_cfg.ue),
  logger(srslog::fetch_basic_logger("SCHED")),
  channel_state(cell_cfg.expert_cfg.ue, ue_cfg->get_nof_dl_ports()),
  ue_mcs_calculator(ue_cell_cfg_.cell_cfg_common, channel_state)
{
}
void ue_cell::deactivate()
{
  // Stop UL HARQ retransmissions.
  // Note: We do no stop DL retransmissions because we are still relying on DL to send a potential RRC Release.
  for (unsigned hid = 0; hid != harqs.nof_ul_harqs(); ++hid) {
    harqs.ul_harq(hid).cancel_harq_retxs();
  }

  active = false;
}

void ue_cell::handle_reconfiguration_request(const ue_cell_configuration& ue_cell_cfg)
{
  ue_cfg = &ue_cell_cfg;
}

dl_harq_process::dl_ack_info_result ue_cell::handle_dl_ack_info(slot_point                 uci_slot,
                                                                mac_harq_ack_report_status ack_value,
                                                                unsigned                   harq_bit_idx,
                                                                optional<float>            pucch_snr)
{
  dl_harq_process::dl_ack_info_result result = harqs.dl_ack_info(uci_slot, ack_value, harq_bit_idx, pucch_snr);

  if (result.update == dl_harq_process::status_update::acked or
      result.update == dl_harq_process::status_update::nacked) {
    // HARQ is not expecting more ACK bits. Consider the feedback in the link adaptation controller.
    ue_mcs_calculator.handle_dl_ack_info(
        result.update == dl_harq_process::status_update::acked, result.mcs, result.mcs_table);
  }

  return result;
}

grant_prbs_mcs ue_cell::required_dl_prbs(const pdsch_time_domain_resource_allocation& pdsch_td_cfg,
                                         unsigned                                     pending_bytes,
                                         dci_dl_rnti_config_type                      dci_type) const
{
  pdsch_config_params pdsch_cfg;
  switch (dci_type) {
    case dci_dl_rnti_config_type::tc_rnti_f1_0:
      pdsch_cfg = get_pdsch_config_f1_0_tc_rnti(cell_cfg, pdsch_td_cfg);
      break;
    case dci_dl_rnti_config_type::c_rnti_f1_0:
      pdsch_cfg = get_pdsch_config_f1_0_c_rnti(cell_cfg, &cfg(), pdsch_td_cfg);
      break;
    case dci_dl_rnti_config_type::c_rnti_f1_1:
      pdsch_cfg = get_pdsch_config_f1_1_c_rnti(cfg(), pdsch_td_cfg, channel_state_manager().get_nof_dl_layers());
      break;
    default:
      report_fatal_error("Unsupported PDCCH DCI UL format");
  }

  optional<sch_mcs_index> mcs = ue_mcs_calculator.calculate_dl_mcs(pdsch_cfg.mcs_table);
  if (not mcs.has_value()) {
    // Return a grant with no PRBs if the MCS is invalid (CQI is either 0, for UE out of range, or > 15).
    return grant_prbs_mcs{.n_prbs = 0};
  }
  sch_mcs_description mcs_config = pdsch_mcs_get_config(pdsch_cfg.mcs_table, mcs.value());

  sch_prbs_tbs prbs_tbs = get_nof_prbs(prbs_calculator_sch_config{pending_bytes,
                                                                  (unsigned)pdsch_cfg.symbols.length(),
                                                                  calculate_nof_dmrs_per_rb(pdsch_cfg.dmrs),
                                                                  pdsch_cfg.nof_oh_prb,
                                                                  mcs_config,
                                                                  pdsch_cfg.nof_layers});
  if (prbs_tbs.nof_prbs == 0) {
    return grant_prbs_mcs{.n_prbs = 0};
  }

  // Bound Nof PRBs by the number of PRBs in the BWP and the limits defined in the scheduler config.
  const bwp_downlink_common& bwp_dl_cmn = *ue_cfg->bwp(active_bwp_id()).dl_common;
  unsigned                   nof_prbs   = std::min(prbs_tbs.nof_prbs, bwp_dl_cmn.generic_params.crbs.length());

  // Apply grant size limits specified in the config.
  nof_prbs = std::max(std::min(nof_prbs, cell_cfg.expert_cfg.ue.pdsch_nof_rbs.stop()),
                      cell_cfg.expert_cfg.ue.pdsch_nof_rbs.start());
  nof_prbs = std::max(std::min(nof_prbs, ue_cfg->rrm_cfg().pdsch_grant_size_limits.stop()),
                      ue_cfg->rrm_cfg().pdsch_grant_size_limits.start());

  return grant_prbs_mcs{mcs.value(), nof_prbs};
}

grant_prbs_mcs ue_cell::required_ul_prbs(const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                         unsigned                                     pending_bytes,
                                         dci_ul_rnti_config_type                      dci_type) const
{
  const bwp_uplink_common& bwp_ul_cmn = *ue_cfg->bwp(active_bwp_id()).ul_common;

  // In the following, we allocate extra bits to account for the possible UCI overhead. At this point, we don't
  // differentiate between HARQ-ACK bits and CSI bits, which would be necessary to compute the beta-offset values.
  // Here, we only need to allocate some extra space.
  const unsigned uci_bits_overallocation = 20U;
  const bool     is_csi_report_slot      = false;

  pusch_config_params pusch_cfg;
  switch (dci_type) {
    case dci_ul_rnti_config_type::tc_rnti_f0_0:
      pusch_cfg = get_pusch_config_f0_0_tc_rnti(cell_cfg, pusch_td_cfg);
      break;
    case dci_ul_rnti_config_type::c_rnti_f0_0:
      pusch_cfg =
          get_pusch_config_f0_0_c_rnti(*ue_cfg, bwp_ul_cmn, pusch_td_cfg, uci_bits_overallocation, is_csi_report_slot);
      break;
    case dci_ul_rnti_config_type::c_rnti_f0_1:
      pusch_cfg = get_pusch_config_f0_1_c_rnti(
          *ue_cfg, pusch_td_cfg, channel_state.get_nof_ul_layers(), uci_bits_overallocation, is_csi_report_slot);
      break;
    default:
      report_fatal_error("Unsupported PDCCH DCI UL format");
  }

  sch_mcs_index       mcs        = ue_mcs_calculator.calculate_ul_mcs(pusch_cfg.mcs_table);
  sch_mcs_description mcs_config = pusch_mcs_get_config(pusch_cfg.mcs_table, mcs, false);

  const unsigned nof_symbols = static_cast<unsigned>(pusch_td_cfg.symbols.length());

  sch_prbs_tbs prbs_tbs = get_nof_prbs(prbs_calculator_sch_config{pending_bytes,
                                                                  nof_symbols,
                                                                  calculate_nof_dmrs_per_rb(pusch_cfg.dmrs),
                                                                  pusch_cfg.nof_oh_prb,
                                                                  mcs_config,
                                                                  pusch_cfg.nof_layers});

  unsigned nof_prbs = std::min(prbs_tbs.nof_prbs, bwp_ul_cmn.generic_params.crbs.length());

  // Apply grant size limits specified in the config.
  nof_prbs = std::max(std::min(nof_prbs, ue_cfg->rrm_cfg().pusch_grant_size_limits.stop()),
                      ue_cfg->rrm_cfg().pusch_grant_size_limits.start());

  return grant_prbs_mcs{mcs, nof_prbs};
}

int ue_cell::handle_crc_pdu(slot_point pusch_slot, const ul_crc_pdu_indication& crc_pdu)
{
  // Update UL HARQ state.
  int tbs = harqs.ul_crc_info(crc_pdu.harq_id, crc_pdu.tb_crc_success, pusch_slot);
  if (tbs >= 0) {
    // HARQ with matching ID and UCI slot was found.

    // Update link adaptation controller.
    const ul_harq_process& h_ul = harqs.ul_harq(crc_pdu.harq_id);
    ue_mcs_calculator.handle_ul_crc_info(
        crc_pdu.tb_crc_success, h_ul.last_tx_params().mcs, h_ul.last_tx_params().mcs_table);

    // Update PUSCH KO count metrics.
    ue_metrics.consecutive_pusch_kos = (crc_pdu.tb_crc_success) ? 0 : ue_metrics.consecutive_pusch_kos + 1;

    // Update PUSCH SNR reported from PHY.
    if (crc_pdu.ul_sinr_metric.has_value()) {
      channel_state.update_pusch_snr(crc_pdu.ul_sinr_metric.value());
    }
  }

  return tbs;
}

void ue_cell::handle_csi_report(const csi_report_data& csi_report)
{
  set_fallback_state(false);
  apply_link_adaptation_procedures(csi_report);
  if (not channel_state.handle_csi_report(csi_report)) {
    logger.warning("ue={} rnti={}: Invalid CSI report received", ue_index, rnti());
  }
}

template <typename FilterSearchSpace>
static static_vector<const search_space_info*, MAX_NOF_SEARCH_SPACE_PER_BWP>
get_prioritized_search_spaces(const ue_cell& ue_cc, FilterSearchSpace filter, bool is_dl)
{
  static_vector<const search_space_info*, MAX_NOF_SEARCH_SPACE_PER_BWP> active_search_spaces;

  // Get all Search Spaces configured in PDCCH-Config for active BWP.
  const auto& bwp_ss_lst = ue_cc.cfg().bwp(ue_cc.active_bwp_id()).search_spaces;
  for (const search_space_info* search_space : bwp_ss_lst) {
    if (filter(*search_space)) {
      active_search_spaces.push_back(search_space);
    }
  }

  // Sort search spaces by priority.
  auto sort_ss = [&ue_cc, is_dl](const search_space_info* lhs, const search_space_info* rhs) {
    // NOTE: It does not matter whether we use lhs or rhs SearchSpace to get the aggregation level as we are sorting not
    // filtering. Filtering is already done in previous step.
    const unsigned aggr_lvl_idx = to_aggregation_level_index(
        ue_cc.get_aggregation_level(ue_cc.channel_state_manager().get_wideband_cqi(), *lhs, is_dl));
    return lhs->cfg->get_nof_candidates()[aggr_lvl_idx] > rhs->cfg->get_nof_candidates()[aggr_lvl_idx];
  };
  std::sort(active_search_spaces.begin(), active_search_spaces.end(), sort_ss);

  return active_search_spaces;
}

static_vector<const search_space_info*, MAX_NOF_SEARCH_SPACE_PER_BWP>
ue_cell::get_active_dl_search_spaces(slot_point                        pdcch_slot,
                                     optional<dci_dl_rnti_config_type> required_dci_rnti_type) const
{
  static_vector<const search_space_info*, MAX_NOF_SEARCH_SPACE_PER_BWP> active_search_spaces;

  if (required_dci_rnti_type == dci_dl_rnti_config_type::tc_rnti_f1_0) {
    // In case of TC-RNTI, use Type-1 PDCCH CSS for a UE.
    active_search_spaces.push_back(
        &cfg().search_space(cfg().cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id));
    return active_search_spaces;
  }

  // In fallback mode state, only use search spaces configured in CellConfigCommon.
  if (is_fallback_mode) {
    srsran_assert(not required_dci_rnti_type.has_value() or
                      required_dci_rnti_type == dci_dl_rnti_config_type::c_rnti_f1_0,
                  "Invalid required dci-rnti parameter");
    for (const search_space_configuration& ss :
         ue_cfg->cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces) {
      if (pdcch_helper::is_pdcch_monitoring_active(pdcch_slot, ss)) {
        active_search_spaces.push_back(&ue_cfg->search_space(ss.get_id()));
      }
    }
    return active_search_spaces;
  }

  auto filter_ss = [this, pdcch_slot, required_dci_rnti_type](const search_space_info& ss) {
    // See TS 38.213, A UE monitors PDCCH candidates in one or more of the following search spaces sets:
    // - a Type3-PDCCH CSS set configured by SearchSpace in PDCCH-Config with searchSpaceType = common for DCI formats
    //   with CRC scrambled by INT-RNTI, SFI-RNTI, TPC-PUSCH-RNTI, TPC-PUCCH-RNTI, or TPC-SRS-RNTI and, only for the
    //   primary cell, C-RNTI, MCS-C-RNTI, or CS-RNTI(s).
    // - a USS set configured by SearchSpace in PDCCH-Config with searchSpaceType = ue-Specific for DCI formats
    //   with CRC scrambled by C-RNTI, MCS-C-RNTI, SP-CSI-RNTI, or CS-RNTI(s).
    //
    // As per TS 38.213, the UE monitors PDCCH candidates for DCI format 0_0 and DCI format 1_0 with CRC scrambled by
    // the C-RNTI, the MCS-C-RNTI, or the CS-RNTI in the one or more search space sets in a slot where the UE monitors
    // PDCCH candidates for at least a DCI format 0_0 or a DCI format 1_0 with CRC scrambled by SI-RNTI, RA-RNTI or
    // P-RNTI.
    if (ss.cfg->is_common_search_space()) {
      const auto& pdcch_config_ss_lst = cfg().bwp(active_bwp_id()).dl_ded->pdcch_cfg->search_spaces;
      const bool  is_type3_css        = std::find_if(pdcch_config_ss_lst.begin(),
                                             pdcch_config_ss_lst.end(),
                                             [&ss](const search_space_configuration& ss_cfg) {
                                               return ss.cfg->get_id() == ss_cfg.get_id();
                                             }) != pdcch_config_ss_lst.end();

      const bool is_ss_for_ra =
          ss.cfg->get_id() == cfg().cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id;
      const bool is_ss_for_paging =
          not cfg().cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.has_value() or
          ss.cfg->get_id() ==
              cfg().cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.value();

      // [Implementation-defined] We exclude SearchSpace#0 to avoid the complexity of computing the SearchSpace#0 PDCCH
      // candidates monitoring occasions associated with a SS/PBCH block as mentioned in TS 38.213, clause 10.1.
      if (ss.cfg->get_id() == to_search_space_id(0) or
          (not is_ss_for_ra and not is_ss_for_paging and not is_type3_css)) {
        return false;
      }
    }

    if (not pdcch_helper::is_pdcch_monitoring_active(pdcch_slot, *ss.cfg)) {
      return false;
    }

    if (ss.get_pdcch_candidates(get_aggregation_level(channel_state_manager().get_wideband_cqi(), ss, true), pdcch_slot)
            .empty()) {
      return false;
    }
    if (required_dci_rnti_type.has_value() and
        not pdcch_helper::search_space_supports_dci_dl_format(*ss.cfg, get_dci_dl_format(*required_dci_rnti_type))) {
      return false;
    }
    return true;
  };
  return get_prioritized_search_spaces(*this, filter_ss, true);
}

static_vector<const search_space_info*, MAX_NOF_SEARCH_SPACE_PER_BWP>
ue_cell::get_active_ul_search_spaces(slot_point                        pdcch_slot,
                                     optional<dci_ul_rnti_config_type> required_dci_rnti_type) const
{
  // In fallback mode state, only use search spaces configured in CellConfigCommon.
  if (is_fallback_mode) {
    static_vector<const search_space_info*, MAX_NOF_SEARCH_SPACE_PER_BWP> active_search_spaces;
    srsran_assert(not required_dci_rnti_type.has_value() or
                      required_dci_rnti_type == dci_ul_rnti_config_type::c_rnti_f0_0,
                  "Invalid required dci-rnti parameter");
    for (const search_space_configuration& ss :
         ue_cfg->cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces) {
      if (pdcch_helper::is_pdcch_monitoring_active(pdcch_slot, ss)) {
        active_search_spaces.push_back(&ue_cfg->search_space(ss.get_id()));
      }
    }
    return active_search_spaces;
  }

  auto filter_ss = [this, pdcch_slot, required_dci_rnti_type](const search_space_info& ss) {
    // See TS 38.213, A UE monitors PDCCH candidates in one or more of the following search spaces sets:
    // - a Type3-PDCCH CSS set configured by SearchSpace in PDCCH-Config with searchSpaceType = common for DCI formats
    //   with CRC scrambled by INT-RNTI, SFI-RNTI, TPC-PUSCH-RNTI, TPC-PUCCH-RNTI, or TPC-SRS-RNTI and, only for the
    //   primary cell, C-RNTI, MCS-C-RNTI, or CS-RNTI(s).
    // - a USS set configured by SearchSpace in PDCCH-Config with searchSpaceType = ue-Specific for DCI formats
    //   with CRC scrambled by C-RNTI, MCS-C-RNTI, SP-CSI-RNTI, or CS-RNTI(s).
    //
    // As per TS 38.213, the UE monitors PDCCH candidates for DCI format 0_0 and DCI format 1_0 with CRC scrambled by
    // the C-RNTI, the MCS-C-RNTI, or the CS-RNTI in the one or more search space sets in a slot where the UE monitors
    // PDCCH candidates for at least a DCI format 0_0 or a DCI format 1_0 with CRC scrambled by SI-RNTI, RA-RNTI or
    // P-RNTI.
    if (ss.cfg->is_common_search_space()) {
      const auto& pdcch_config_ss_lst = cfg().bwp(active_bwp_id()).dl_ded->pdcch_cfg->search_spaces;
      const bool  is_type3_css        = std::find_if(pdcch_config_ss_lst.begin(),
                                             pdcch_config_ss_lst.end(),
                                             [&ss](const search_space_configuration& ss_cfg) {
                                               return ss.cfg->get_id() == ss_cfg.get_id();
                                             }) != pdcch_config_ss_lst.end();

      const bool is_ss_for_ra =
          ss.cfg->get_id() == cfg().cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id;
      const bool is_ss_for_paging =
          not cfg().cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.has_value() or
          ss.cfg->get_id() ==
              cfg().cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.value();

      // [Implementation-defined] We exclude SearchSpace#0 to avoid the complexity of computing the SearchSpace#0 PDCCH
      // candidates monitoring occasions associated with a SS/PBCH block as mentioned in TS 38.213, clause 10.1.
      if (ss.cfg->get_id() == to_search_space_id(0) or
          (not is_ss_for_ra and not is_ss_for_paging and not is_type3_css)) {
        return false;
      }
    }

    if (not pdcch_helper::is_pdcch_monitoring_active(pdcch_slot, *ss.cfg)) {
      return false;
    }

    if (ss.get_pdcch_candidates(get_aggregation_level(channel_state_manager().get_wideband_cqi(), ss, false),
                                pdcch_slot)
            .empty()) {
      return false;
    }
    return (not required_dci_rnti_type.has_value() or
            *required_dci_rnti_type == (ss.get_ul_dci_format() == dci_ul_format::f0_0
                                            ? dci_ul_rnti_config_type::c_rnti_f0_0
                                            : dci_ul_rnti_config_type::c_rnti_f0_1));
  };
  return get_prioritized_search_spaces(*this, filter_ss, false);
}

/// \brief Get recommended aggregation level for PDCCH given reported CQI.
aggregation_level ue_cell::get_aggregation_level(cqi_value cqi, const search_space_info& ss_info, bool is_dl) const
{
  cqi_table_t cqi_table = cqi_table_t::table1;
  unsigned    dci_size;

  if (ss_info.cfg->is_common_search_space()) {
    if (is_dl) {
      dci_size = ss_info.dci_sz.format1_0_common_size.total.value();
    } else {
      dci_size = ss_info.dci_sz.format0_0_common_size.total.value();
    }
  } else {
    if (is_dl) {
      dci_size = ss_info.get_dl_dci_format() == dci_dl_format::f1_1 ? ss_info.dci_sz.format1_1_ue_size->total.value()
                                                                    : ss_info.dci_sz.format1_0_ue_size->total.value();
    } else {
      dci_size = ss_info.get_ul_dci_format() == dci_ul_format::f0_1 ? ss_info.dci_sz.format0_1_ue_size->total.value()
                                                                    : ss_info.dci_sz.format0_0_ue_size->total.value();
    }
  }

  if (not ss_info.cfg->is_common_search_space() and cfg().cfg_dedicated().csi_meas_cfg.has_value()) {
    // NOTE: It is assumed there is atleast one CSI report configured for UE.
    cqi_table = cfg().cfg_dedicated().csi_meas_cfg->csi_report_cfg_list.back().cqi_table.value();
  }

  return map_cqi_to_aggregation_level(cqi, cqi_table, ss_info.cfg->get_nof_candidates(), dci_size);
}

void ue_cell::apply_link_adaptation_procedures(const csi_report_data& csi_report)
{
  // Early return if no decrease in CQI and RI.
  const bool cqi_decreased = csi_report.first_tb_wideband_cqi.has_value() and
                             csi_report.first_tb_wideband_cqi.value() < channel_state.get_wideband_cqi();
  const bool ri_decreased = csi_report.ri.has_value() and csi_report.ri.value() < channel_state.get_nof_dl_layers();
  if (not cqi_decreased and not ri_decreased) {
    return;
  }

  const csi_report_wideband_cqi_type wideband_cqi = csi_report.first_tb_wideband_cqi.has_value()
                                                        ? csi_report.first_tb_wideband_cqi.value()
                                                        : channel_state.get_wideband_cqi();
  const unsigned                     recommended_dl_layers =
      csi_report.ri.has_value() and csi_report.ri.value() <= ue_cfg->get_nof_dl_ports()
                              ? csi_report.ri->value()
                              : channel_state.get_nof_dl_layers();

  static const uint8_t tb_index = 0;
  // Link adaptation for HARQs.
  // [Implementation-defined] If the drop in RI or CQI when compared to the RI or CQI at the time of new HARQ
  // transmission is above threshold then HARQ re-transmissions are cancelled.
  for (unsigned hid = 0; hid != harqs.nof_dl_harqs(); ++hid) {
    dl_harq_process& h_dl = harqs.dl_harq(hid);
    if (h_dl.empty()) {
      continue;
    }
    const bool is_ri_diff_above_threshold =
        expert_cfg.dl_harq_la_ri_drop_threshold != 0 and
        recommended_dl_layers + expert_cfg.dl_harq_la_ri_drop_threshold <= h_dl.last_alloc_params().nof_layers;
    const bool is_cqi_diff_above_threshold =
        expert_cfg.dl_harq_la_cqi_drop_threshold != 0 and
        wideband_cqi.to_uint() + expert_cfg.dl_harq_la_cqi_drop_threshold <= h_dl.last_alloc_params().cqi.to_uint();
    if (is_ri_diff_above_threshold or is_cqi_diff_above_threshold) {
      h_dl.cancel_harq_retxs(tb_index);
    }
  }
}
