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

#include "gnb_appconfig_translators.h"
#include "gnb_appconfig.h"
#include "srsran/cu_cp/cu_cp_configuration_helpers.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/du/du_cell_config_validation.h"
#include "srsran/du/du_update_config_helpers.h"
#include "srsran/e2/e2ap_configuration.h"
#include "srsran/e2/e2ap_configuration_helpers.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/ran/prach/prach_helper.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/scheduler/config/cell_config_builder_params.h"
#include "srsran/scheduler/config/csi_helper.h"
#include "srsran/scheduler/config/scheduler_expert_config_validator.h"
#include "srsran/scheduler/config/serving_cell_config_factory.h"
#include <algorithm>
#include <map>

using namespace srsran;
using namespace std::chrono_literals;

/// Static configuration that the gnb supports.
static constexpr cyclic_prefix cp = cyclic_prefix::NORMAL;

srs_cu_cp::rrc_ssb_mtc srsran::generate_rrc_ssb_mtc(unsigned period, unsigned offset, unsigned duration)
{
  srs_cu_cp::rrc_ssb_mtc ssb_mtc;
  switch (period) {
    case 5:
      ssb_mtc.periodicity_and_offset.sf5.emplace() = offset;
      break;
    case 10:
      ssb_mtc.periodicity_and_offset.sf10.emplace() = offset;
      break;
    case 20:
      ssb_mtc.periodicity_and_offset.sf20.emplace() = offset;
      break;
    case 40:
      ssb_mtc.periodicity_and_offset.sf40.emplace() = offset;
      break;
    case 80:
      ssb_mtc.periodicity_and_offset.sf80.emplace() = offset;
      break;
    case 160:
      ssb_mtc.periodicity_and_offset.sf160.emplace() = offset;
      break;
    default:
      report_error("Invalid SSB periodicity {}\n", period);
  }
  ssb_mtc.dur = duration;

  return ssb_mtc;
}

srsran::sctp_network_gateway_config srsran::generate_ngap_nw_config(const gnb_appconfig& config)
{
  srsran::sctp_network_gateway_config out_cfg;
  out_cfg.connection_name = "AMF";
  out_cfg.connect_address = config.amf_cfg.ip_addr;
  out_cfg.connect_port    = config.amf_cfg.port;
  out_cfg.bind_address    = config.amf_cfg.bind_addr;
  out_cfg.ppid            = NGAP_PPID;

  if (config.amf_cfg.sctp_rto_initial >= 0) {
    out_cfg.rto_initial = config.amf_cfg.sctp_rto_initial;
  }
  if (config.amf_cfg.sctp_rto_min >= 0) {
    out_cfg.rto_min = config.amf_cfg.sctp_rto_min;
  }
  if (config.amf_cfg.sctp_rto_max >= 0) {
    out_cfg.rto_max = config.amf_cfg.sctp_rto_max;
  }
  if (config.amf_cfg.sctp_init_max_attempts >= 0) {
    out_cfg.init_max_attempts = config.amf_cfg.sctp_init_max_attempts;
  }
  if (config.amf_cfg.sctp_max_init_timeo >= 0) {
    out_cfg.max_init_timeo = config.amf_cfg.sctp_max_init_timeo;
  }

  return out_cfg;
}

srs_cu_cp::cu_cp_configuration srsran::generate_cu_cp_config(const gnb_appconfig& config)
{
  srs_cu_cp::cu_cp_configuration out_cfg   = config_helpers::make_default_cu_cp_config();
  out_cfg.ngap_config.gnb_id               = config.gnb_id;
  out_cfg.ngap_config.ran_node_name        = config.ran_node_name;
  out_cfg.ngap_config.plmn                 = config.common_cell_cfg.plmn;
  out_cfg.ngap_config.tac                  = config.common_cell_cfg.tac;
  out_cfg.ngap_config.slice_configurations = config.slice_cfg;

  out_cfg.rrc_config.force_reestablishment_fallback = config.cu_cp_cfg.rrc_config.force_reestablishment_fallback;
  out_cfg.rrc_config.rrc_procedure_timeout_ms       = config.cu_cp_cfg.rrc_config.rrc_procedure_timeout_ms;
  out_cfg.rrc_config.drb_config                     = generate_cu_cp_qos_config(config);

  if (!from_string(out_cfg.default_security_indication.integrity_protection_ind,
                   config.cu_cp_cfg.security_config.integrity_protection)) {
    report_error("Invalid value for integrity_protection={}\n", config.cu_cp_cfg.security_config.integrity_protection);
  }
  if (!from_string(out_cfg.default_security_indication.confidentiality_protection_ind,
                   config.cu_cp_cfg.security_config.confidentiality_protection)) {
    report_error("Invalid value for confidentiality_protection={}\n",
                 config.cu_cp_cfg.security_config.confidentiality_protection);
  }

  out_cfg.ue_config.inactivity_timer             = std::chrono::seconds{config.cu_cp_cfg.inactivity_timer};
  out_cfg.ngap_config.ue_context_setup_timeout_s = std::chrono::seconds{config.cu_cp_cfg.ue_context_setup_timeout_s};
  out_cfg.statistics_report_period = std::chrono::seconds{config.metrics_cfg.cu_cp_statistics_report_period};

  out_cfg.mobility_config.mobility_manager_config.trigger_handover_from_measurements =
      config.cu_cp_cfg.mobility_config.trigger_handover_from_measurements;

  // Convert appconfig's cell list into cell manager type.
  for (const auto& app_cfg_item : config.cu_cp_cfg.mobility_config.cells) {
    srs_cu_cp::cell_meas_config meas_cfg_item;
    meas_cfg_item.serving_cell_cfg.nci       = app_cfg_item.nr_cell_id;
    meas_cfg_item.serving_cell_cfg.gnb_id    = app_cfg_item.gnb_id;
    meas_cfg_item.serving_cell_cfg.band      = app_cfg_item.band;
    meas_cfg_item.serving_cell_cfg.ssb_arfcn = app_cfg_item.ssb_arfcn;
    if (app_cfg_item.ssb_scs.has_value()) {
      meas_cfg_item.serving_cell_cfg.ssb_scs.emplace() =
          to_subcarrier_spacing(std::to_string(app_cfg_item.ssb_scs.value()));
    }
    if (app_cfg_item.periodic_report_cfg_id.has_value()) {
      meas_cfg_item.periodic_report_cfg_id =
          srs_cu_cp::uint_to_report_cfg_id(app_cfg_item.periodic_report_cfg_id.value());
    }
    for (const auto& ncell : app_cfg_item.ncells) {
      srs_cu_cp::neighbor_cell_meas_config ncell_meas_cfg;
      ncell_meas_cfg.nci = ncell.nr_cell_id;
      for (const auto& report_id : ncell.report_cfg_ids) {
        ncell_meas_cfg.report_cfg_ids.push_back(srs_cu_cp::uint_to_report_cfg_id(report_id));
      }

      meas_cfg_item.ncells.push_back(ncell_meas_cfg);
    }
    if (app_cfg_item.ssb_duration.has_value() && app_cfg_item.ssb_offset.has_value() &&
        app_cfg_item.ssb_period.has_value()) {
      // Add MTC config.
      meas_cfg_item.serving_cell_cfg.ssb_mtc.emplace() = generate_rrc_ssb_mtc(
          app_cfg_item.ssb_period.value(), app_cfg_item.ssb_offset.value(), app_cfg_item.ssb_duration.value());
    }

    // Store config.
    out_cfg.mobility_config.meas_manager_config.cells[meas_cfg_item.serving_cell_cfg.nci] = meas_cfg_item;
  }

  // Convert report config.
  for (const auto& report_cfg_item : config.cu_cp_cfg.mobility_config.report_configs) {
    srs_cu_cp::rrc_report_cfg_nr report_cfg;

    if (report_cfg_item.report_type == "periodical") {
      srs_cu_cp::rrc_periodical_report_cfg periodical;

      periodical.rs_type = srs_cu_cp::rrc_nr_rs_type::ssb;
      if (report_cfg_item.report_interval_ms.has_value()) {
        periodical.report_interv = report_cfg_item.report_interval_ms.value();
      } else {
        periodical.report_interv = 1024;
      }
      periodical.report_amount          = -1;
      periodical.report_quant_cell.rsrp = true;
      periodical.report_quant_cell.rsrq = true;
      periodical.report_quant_cell.sinr = true;
      periodical.max_report_cells       = 4;

      srs_cu_cp::rrc_meas_report_quant report_quant_rs_idxes;
      report_quant_rs_idxes.rsrp       = true;
      report_quant_rs_idxes.rsrq       = true;
      report_quant_rs_idxes.sinr       = true;
      periodical.report_quant_rs_idxes = report_quant_rs_idxes;

      periodical.max_nrof_rs_idxes_to_report = 4;
      periodical.include_beam_meass          = true;
      periodical.use_allowed_cell_list       = false;

      report_cfg.periodical = periodical;
    } else {
      srs_cu_cp::rrc_event_trigger_cfg event_trigger_cfg;

      // event id
      // A3 event config is currently the only supported event.
      auto& event_a3 = event_trigger_cfg.event_id.event_a3.emplace();

      if (report_cfg_item.a3_report_type.empty() or !report_cfg_item.a3_offset_db.has_value() or
          !report_cfg_item.a3_hysteresis_db.has_value()) {
        report_error("Invalid measurement report configuration.\n");
      }

      if (report_cfg_item.a3_report_type == "rsrp") {
        event_a3.a3_offset.rsrp = report_cfg_item.a3_offset_db.value();
      } else if (report_cfg_item.a3_report_type == "rsrq") {
        event_a3.a3_offset.rsrq = report_cfg_item.a3_offset_db.value();
      } else if (report_cfg_item.a3_report_type == "sinr") {
        event_a3.a3_offset.sinr = report_cfg_item.a3_offset_db.value();
      }

      event_a3.report_on_leave = false;

      event_a3.hysteresis      = report_cfg_item.a3_hysteresis_db.value();
      event_a3.time_to_trigger = report_cfg_item.a3_time_to_trigger_ms.value();

      event_a3.use_allowed_cell_list = false;

      event_trigger_cfg.rs_type = srs_cu_cp::rrc_nr_rs_type::ssb;
      if (report_cfg_item.report_interval_ms.has_value()) {
        event_trigger_cfg.report_interv = report_cfg_item.report_interval_ms.value();
      } else {
        event_trigger_cfg.report_interv = 1024;
      }
      event_trigger_cfg.report_amount          = -1;
      event_trigger_cfg.report_quant_cell.rsrp = true;
      event_trigger_cfg.report_quant_cell.rsrq = true;
      event_trigger_cfg.report_quant_cell.sinr = true;
      event_trigger_cfg.max_report_cells       = 4;

      srs_cu_cp::rrc_meas_report_quant report_quant_rs_idxes;
      report_quant_rs_idxes.rsrp              = true;
      report_quant_rs_idxes.rsrq              = true;
      report_quant_rs_idxes.sinr              = true;
      event_trigger_cfg.report_quant_rs_idxes = report_quant_rs_idxes;

      report_cfg.event_triggered = event_trigger_cfg;
    }

    // Store config.
    out_cfg.mobility_config.meas_manager_config
        .report_config_ids[srs_cu_cp::uint_to_report_cfg_id(report_cfg_item.report_cfg_id)] = report_cfg;
  }

  if (!config_helpers::is_valid_configuration(out_cfg)) {
    report_error("Invalid CU-CP configuration.\n");
  }

  return out_cfg;
}

srs_cu_up::cu_up_configuration srsran::generate_cu_up_config(const gnb_appconfig& config)
{
  srs_cu_up::cu_up_configuration out_cfg;
  out_cfg.statistics_report_period = std::chrono::seconds{config.metrics_cfg.cu_up_statistics_report_period};

  return out_cfg;
}

static pcch_config generate_pcch_config(const base_cell_appconfig& cell)
{
  pcch_config cfg{};
  switch (cell.paging_cfg.default_paging_cycle) {
    case 32:
      cfg.default_paging_cycle = paging_cycle::rf32;
      break;
    case 64:
      cfg.default_paging_cycle = paging_cycle::rf64;
      break;
    case 128:
      cfg.default_paging_cycle = paging_cycle::rf128;
      break;
    case 256:
      cfg.default_paging_cycle = paging_cycle::rf256;
      break;
    default:
      report_error(
          "Invalid default paging cycle={} for cell with pci={}\n", cell.paging_cfg.default_paging_cycle, cell.pci);
  }
  cfg.nof_pf = cell.paging_cfg.nof_pf;
  if (cell.paging_cfg.pf_offset > (static_cast<unsigned>(cfg.nof_pf) - 1)) {
    report_error("Invalid paging frame offset value={} for cell with pci={}. Value must be less than or equal to {}\n",
                 cell.paging_cfg.pf_offset,
                 cell.pci,
                 (static_cast<unsigned>(cfg.nof_pf) - 1));
  }
  cfg.paging_frame_offset = cell.paging_cfg.pf_offset;
  switch (cell.paging_cfg.nof_po_per_pf) {
    case 1:
      cfg.ns = pcch_config::nof_po_per_pf::one;
      break;
    case 2:
      cfg.ns = pcch_config::nof_po_per_pf::two;
      break;
    case 4:
      cfg.ns = pcch_config::nof_po_per_pf::four;
      break;
    default:
      report_error("Invalid number of paging occasions per paging frame={} for cell with pci={}\n",
                   cell.paging_cfg.nof_po_per_pf,
                   cell.pci);
  }

  return cfg;
}

static unsigned get_nof_rbs(const base_cell_appconfig& cell_cfg)
{
  return band_helper::get_n_rbs_from_bw(
      cell_cfg.channel_bw_mhz, cell_cfg.common_scs, band_helper::get_freq_range(*cell_cfg.band));
}

static tdd_ul_dl_config_common generate_tdd_pattern(subcarrier_spacing scs, const tdd_ul_dl_appconfig& config)
{
  tdd_ul_dl_config_common out;
  out.ref_scs = scs;

  out.pattern1.dl_ul_tx_period_nof_slots = config.pattern1.dl_ul_period_slots;
  out.pattern1.nof_dl_slots              = config.pattern1.nof_dl_slots;
  out.pattern1.nof_dl_symbols            = config.pattern1.nof_dl_symbols;
  out.pattern1.nof_ul_slots              = config.pattern1.nof_ul_slots;
  out.pattern1.nof_ul_symbols            = config.pattern1.nof_ul_symbols;

  if (config.pattern2.has_value()) {
    out.pattern2.emplace();
    out.pattern2->dl_ul_tx_period_nof_slots = config.pattern2->dl_ul_period_slots;
    out.pattern2->nof_dl_slots              = config.pattern2->nof_dl_slots;
    out.pattern2->nof_dl_symbols            = config.pattern2->nof_dl_symbols;
    out.pattern2->nof_ul_slots              = config.pattern2->nof_ul_slots;
    out.pattern2->nof_ul_symbols            = config.pattern2->nof_ul_symbols;
  }
  return out;
}

static void fill_csi_resources(serving_cell_config& out_cell, const base_cell_appconfig& cell_cfg)
{
  const auto& csi_cfg = cell_cfg.csi_cfg;

  csi_helper::csi_builder_params csi_params{};
  csi_params.pci           = cell_cfg.pci;
  csi_params.nof_rbs       = get_nof_rbs(cell_cfg);
  csi_params.nof_ports     = cell_cfg.nof_antennas_dl;
  csi_params.csi_rs_period = static_cast<csi_resource_periodicity>(csi_cfg.csi_rs_period_msec *
                                                                   get_nof_slots_per_subframe(cell_cfg.common_scs));
  if (cell_cfg.tdd_ul_dl_cfg.has_value()) {
    if (not csi_helper::derive_valid_csi_rs_slot_offsets(
            csi_params,
            csi_cfg.meas_csi_slot_offset,
            csi_cfg.tracking_csi_slot_offset,
            csi_cfg.zp_csi_slot_offset,
            generate_tdd_pattern(cell_cfg.common_scs, *cell_cfg.tdd_ul_dl_cfg))) {
      report_error("Unable to derive valid CSI-RS slot offsets and period for cell with pci={}\n", cell_cfg.pci);
    }
  } else {
    csi_params.meas_csi_slot_offset = csi_cfg.meas_csi_slot_offset.has_value() ? *csi_cfg.meas_csi_slot_offset : 2;
    csi_params.zp_csi_slot_offset   = csi_cfg.zp_csi_slot_offset.has_value() ? *csi_cfg.zp_csi_slot_offset : 2;
    csi_params.tracking_csi_slot_offset =
        csi_cfg.tracking_csi_slot_offset.has_value() ? *csi_cfg.tracking_csi_slot_offset : 12;
  }

  // Generate basic csiMeasConfig.
  out_cell.csi_meas_cfg = csi_helper::make_csi_meas_config(csi_params);

  // Set power control offset for all nzp-CSI-RS resources.
  for (auto& nzp_csi_res : out_cell.csi_meas_cfg->nzp_csi_rs_res_list) {
    nzp_csi_res.pwr_ctrl_offset = cell_cfg.csi_cfg.pwr_ctrl_offset;
  }

  // Set CQI table according to the MCS table used for PDSCH.
  switch (cell_cfg.pdsch_cfg.mcs_table) {
    case pdsch_mcs_table::qam64:
      out_cell.csi_meas_cfg->csi_report_cfg_list[0].cqi_table = cqi_table_t::table1;
      break;
    case pdsch_mcs_table::qam256:
      out_cell.csi_meas_cfg->csi_report_cfg_list[0].cqi_table = cqi_table_t::table2;
      break;
    case pdsch_mcs_table::qam64LowSe:
      out_cell.csi_meas_cfg->csi_report_cfg_list[0].cqi_table = cqi_table_t::table3;
      break;
    default:
      report_error("Invalid MCS table={} for cell with pci={}\n", cell_cfg.pdsch_cfg.mcs_table, cell_cfg.pci);
  }

  // Generate zp-CSI-RS resources.
  out_cell.init_dl_bwp.pdsch_cfg->zp_csi_rs_res_list = csi_helper::make_periodic_zp_csi_rs_resource_list(csi_params);
  out_cell.init_dl_bwp.pdsch_cfg->p_zp_csi_rs_res    = csi_helper::make_periodic_zp_csi_rs_resource_set(csi_params);
}

std::vector<du_cell_config> srsran::generate_du_cell_config(const gnb_appconfig& config)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("GNB", false);

  std::vector<du_cell_config> out_cfg;
  out_cfg.reserve(config.cells_cfg.size());

  unsigned cell_id = 0;
  for (const auto& cell : config.cells_cfg) {
    cell_config_builder_params param;
    const base_cell_appconfig& base_cell = cell.cell;
    param.pci                            = base_cell.pci;
    param.scs_common                     = base_cell.common_scs;
    param.channel_bw_mhz                 = base_cell.channel_bw_mhz;
    param.dl_arfcn                       = base_cell.dl_arfcn;
    param.band                           = *base_cell.band;
    // Enable CSI-RS if the PDSCH mcs is dynamic (min_ue_mcs != max_ue_mcs).
    param.csi_rs_enabled      = cell.cell.pdsch_cfg.min_ue_mcs != cell.cell.pdsch_cfg.max_ue_mcs;
    param.nof_dl_ports        = base_cell.nof_antennas_dl;
    param.search_space0_index = base_cell.pdcch_cfg.common.ss0_index;
    param.min_k1              = base_cell.pucch_cfg.min_k1;
    param.min_k2              = base_cell.pusch_cfg.min_k2;
    param.coreset0_index      = base_cell.pdcch_cfg.common.coreset0_index;

    const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
        base_cell.channel_bw_mhz, param.scs_common, band_helper::get_freq_range(*param.band));

    optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc;
    if (base_cell.pdcch_cfg.common.coreset0_index.has_value()) {
      ssb_freq_loc = band_helper::get_ssb_coreset0_freq_location(base_cell.dl_arfcn,
                                                                 *param.band,
                                                                 nof_crbs,
                                                                 base_cell.common_scs,
                                                                 base_cell.common_scs,
                                                                 param.search_space0_index,
                                                                 base_cell.pdcch_cfg.common.coreset0_index.value());
    } else {
      ssb_freq_loc = band_helper::get_ssb_coreset0_freq_location(base_cell.dl_arfcn,
                                                                 *param.band,
                                                                 nof_crbs,
                                                                 base_cell.common_scs,
                                                                 base_cell.common_scs,
                                                                 param.search_space0_index);
    }

    if (!ssb_freq_loc.has_value()) {
      report_error("Unable to derive a valid SSB pointA and k_SSB for cell id ({}).\n", base_cell.pci);
    }

    param.offset_to_point_a = (*ssb_freq_loc).offset_to_point_A;
    param.k_ssb             = (*ssb_freq_loc).k_ssb;
    param.coreset0_index    = (*ssb_freq_loc).coreset0_idx;

    // Set TDD pattern.
    if (band_helper::get_duplex_mode(*param.band) == duplex_mode::TDD) {
      param.tdd_ul_dl_cfg_common.emplace(generate_tdd_pattern(param.scs_common, cell.cell.tdd_ul_dl_cfg.value()));
    }

    // Create the configuration.
    out_cfg.push_back(config_helpers::make_default_du_cell_config(param));
    du_cell_config& out_cell = out_cfg.back();

    // Set the rest of the parameters.
    out_cell.nr_cgi.plmn      = base_cell.plmn;
    out_cell.nr_cgi.nci       = config_helpers::make_nr_cell_identity(config.gnb_id, config.gnb_id_bit_length, cell_id);
    out_cell.tac              = base_cell.tac;
    out_cell.searchspace0_idx = param.search_space0_index;

    // Cell selection parameters.
    out_cell.cell_sel_info.q_rx_lev_min = base_cell.q_rx_lev_min;
    out_cell.cell_sel_info.q_qual_min   = base_cell.q_qual_min;

    // SSB config.
    out_cell.ssb_cfg.ssb_period      = (ssb_periodicity)base_cell.ssb_cfg.ssb_period_msec;
    out_cell.ssb_cfg.ssb_block_power = base_cell.ssb_cfg.ssb_block_power;
    out_cell.ssb_cfg.pss_to_sss_epre = base_cell.ssb_cfg.pss_to_sss_epre;

    // SI message config.
    if (not base_cell.sib_cfg.si_sched_info.empty()) {
      out_cell.si_config.emplace();
      out_cell.si_config->si_window_len_slots = base_cell.sib_cfg.si_window_len_slots;
      out_cell.si_config->si_sched_info.resize(base_cell.sib_cfg.si_sched_info.size());
      std::vector<uint8_t> sibs_included;
      for (unsigned i = 0; i != base_cell.sib_cfg.si_sched_info.size(); ++i) {
        auto& out_si                  = out_cell.si_config->si_sched_info[i];
        out_si.si_period_radio_frames = base_cell.sib_cfg.si_sched_info[i].si_period_rf;
        out_si.sib_mapping_info.resize(base_cell.sib_cfg.si_sched_info[i].sib_mapping_info.size());
        for (unsigned j = 0; j != base_cell.sib_cfg.si_sched_info[i].sib_mapping_info.size(); ++j) {
          sibs_included.push_back(base_cell.sib_cfg.si_sched_info[i].sib_mapping_info[j]);
          out_si.sib_mapping_info[j] = static_cast<sib_type>(sibs_included.back());
        }
      }
      for (const uint8_t sib_id : sibs_included) {
        sib_info item;
        switch (sib_id) {
          case 19: {
            item = base_cell.sib_cfg.sib19;
          } break;
          default:
            report_error("SIB{} not supported\n", sib_id);
        }
        out_cell.si_config->sibs.push_back(item);
      }
    }

    // UE timers and constants config.
    out_cell.ue_timers_and_constants.t300 = std::chrono::milliseconds(base_cell.sib_cfg.ue_timers_and_constants.t300);
    out_cell.ue_timers_and_constants.t301 = std::chrono::milliseconds(base_cell.sib_cfg.ue_timers_and_constants.t301);
    out_cell.ue_timers_and_constants.t310 = std::chrono::milliseconds(base_cell.sib_cfg.ue_timers_and_constants.t310);
    out_cell.ue_timers_and_constants.n310 = base_cell.sib_cfg.ue_timers_and_constants.n310;
    out_cell.ue_timers_and_constants.t311 = std::chrono::milliseconds(base_cell.sib_cfg.ue_timers_and_constants.t311);
    out_cell.ue_timers_and_constants.n311 = base_cell.sib_cfg.ue_timers_and_constants.n311;
    out_cell.ue_timers_and_constants.t319 = std::chrono::milliseconds(base_cell.sib_cfg.ue_timers_and_constants.t319);

    // Carrier config.
    out_cell.dl_carrier.nof_ant = base_cell.nof_antennas_dl;
    out_cell.ul_carrier.nof_ant = base_cell.nof_antennas_ul;

    // UL common config.
    if (base_cell.ul_common_cfg.p_max.has_value()) {
      out_cell.ul_cfg_common.freq_info_ul.p_max = base_cell.ul_common_cfg.p_max.value();
    }

    // DL common config.
    if (base_cell.pdsch_cfg.dc_offset.has_value()) {
      out_cell.dl_cfg_common.freq_info_dl.scs_carrier_list.back().tx_direct_current_location =
          dc_offset_helper::pack(base_cell.pdsch_cfg.dc_offset.value(),
                                 out_cell.dl_cfg_common.freq_info_dl.scs_carrier_list.back().carrier_bandwidth);
    }

    // PRACH config.
    rach_config_common& rach_cfg                    = *out_cell.ul_cfg_common.init_ul_bwp.rach_cfg_common;
    rach_cfg.rach_cfg_generic.prach_config_index    = base_cell.prach_cfg.prach_config_index.value();
    rach_cfg.rach_cfg_generic.preamble_trans_max    = base_cell.prach_cfg.preamble_trans_max;
    rach_cfg.rach_cfg_generic.power_ramping_step_db = base_cell.prach_cfg.power_ramping_step_db;
    const bool is_long_prach =
        is_long_preamble(prach_configuration_get(band_helper::get_freq_range(param.band.value()),
                                                 band_helper::get_duplex_mode(param.band.value()),
                                                 rach_cfg.rach_cfg_generic.prach_config_index)
                             .format);
    // \c is_prach_root_seq_index_l839 and msg1_scs are derived parameters, that depend on the PRACH format. They are
    // originally computed in the base_cell struct, but since we overwrite the PRACH prach_config_index (which
    // determines the PRACH format), we need to recompute both \c is_prach_root_seq_index_l839 and \c msg1_scs.
    rach_cfg.is_prach_root_seq_index_l839 = is_long_prach;
    rach_cfg.msg1_scs                     = is_long_prach ? subcarrier_spacing::invalid : base_cell.common_scs;
    rach_cfg.prach_root_seq_index         = base_cell.prach_cfg.prach_root_sequence_index;
    rach_cfg.rach_cfg_generic.zero_correlation_zone_config = base_cell.prach_cfg.zero_correlation_zone;
    rach_cfg.rach_cfg_generic.preamble_rx_target_pw        = base_cell.prach_cfg.preamble_rx_target_pw;
    // \c msg1_frequency_start for RACH is one of the parameters that can either be set manually, or need to be
    // recomputed at the end of the manual configuration, as a results of other user parameters passed by the user.
    bool update_msg1_frequency_start = not base_cell.prach_cfg.prach_frequency_start.has_value();
    if (not update_msg1_frequency_start) {
      // Set manually.
      rach_cfg.rach_cfg_generic.msg1_frequency_start = base_cell.prach_cfg.prach_frequency_start.value();
    }
    rach_cfg.total_nof_ra_preambles = base_cell.prach_cfg.total_nof_ra_preambles;

    // PhysicalCellGroup Config parameters.
    if (base_cell.pcg_cfg.p_nr_fr1.has_value()) {
      out_cell.pcg_params.p_nr_fr1 = base_cell.pcg_cfg.p_nr_fr1.value();
    }

    // MAC Cell Group Config parameters.
    out_cell.mcg_params.periodic_timer = to_periodic_bsr_timer(base_cell.mcg_cfg.bsr_cfg.periodic_bsr_timer);
    out_cell.mcg_params.retx_timer     = to_retx_bsr_timer(base_cell.mcg_cfg.bsr_cfg.retx_bsr_timer);
    if (base_cell.mcg_cfg.bsr_cfg.lc_sr_delay_timer.has_value()) {
      out_cell.mcg_params.lc_sr_delay_timer = to_lc_sr_delay_timer(base_cell.mcg_cfg.bsr_cfg.lc_sr_delay_timer.value());
    }
    out_cell.mcg_params.max_tx           = to_sr_max_tx(base_cell.mcg_cfg.sr_cfg.sr_trans_max);
    out_cell.mcg_params.phr_prohib_timer = to_phr_prohibit_timer(base_cell.mcg_cfg.phr_cfg.phr_prohib_timer);
    if (base_cell.mcg_cfg.sr_cfg.sr_prohibit_timer.has_value()) {
      out_cell.mcg_params.sr_prohibit_timer.emplace(
          to_sr_prohib_timer(base_cell.mcg_cfg.sr_cfg.sr_prohibit_timer.value()));
    }

    // PCCH-Config.
    out_cell.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id =
        to_search_space_id(base_cell.paging_cfg.paging_search_space_id);
    out_cell.dl_cfg_common.pcch_cfg = generate_pcch_config(base_cell);

    // Parameters for PUSCH-ConfigCommon.
    if (not out_cell.ul_cfg_common.init_ul_bwp.pusch_cfg_common.has_value()) {
      out_cell.ul_cfg_common.init_ul_bwp.pusch_cfg_common.emplace();
    }
    out_cell.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().msg3_delta_preamble =
        base_cell.pusch_cfg.msg3_delta_preamble;
    out_cell.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().p0_nominal_with_grant =
        base_cell.pusch_cfg.p0_nominal_with_grant;
    out_cell.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().msg3_delta_power = base_cell.pusch_cfg.msg3_delta_power;

    // Parameters for PUCCH-ConfigCommon.
    if (not out_cell.ul_cfg_common.init_ul_bwp.pucch_cfg_common.has_value()) {
      out_cell.ul_cfg_common.init_ul_bwp.pucch_cfg_common.emplace();
    }
    out_cell.ul_cfg_common.init_ul_bwp.pucch_cfg_common.value().p0_nominal = base_cell.pucch_cfg.p0_nominal;

    // Common PDCCH config.
    search_space_configuration& ss1_cfg = out_cell.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces.back();
    ss1_cfg.set_non_ss0_nof_candidates(base_cell.pdcch_cfg.common.ss1_n_candidates);

    // UE-dedicated PDCCH config.
    freq_resource_bitmap freq_resources(pdcch_constants::MAX_NOF_FREQ_RESOURCES);
    unsigned             cset1_start_crb = 0;
    if (base_cell.pdcch_cfg.dedicated.coreset1_rb_start.has_value()) {
      cset1_start_crb = base_cell.pdcch_cfg.dedicated.coreset1_rb_start.value();
    } else if (not base_cell.pdcch_cfg.dedicated.dci_format_0_1_and_1_1) {
      // [Implementation-defined] Reason for starting from frequency resource 1 (i.e. CRB6) to remove the ambiguity of
      // UE decoding the DCI in CSS rather than USS when using fallback DCI formats (DCI format 1_0 and 0_0).
      cset1_start_crb = 6;
    }
    unsigned cset1_l_crb = nof_crbs - cset1_start_crb;
    if (base_cell.pdcch_cfg.dedicated.coreset1_l_crb.has_value()) {
      cset1_l_crb = base_cell.pdcch_cfg.dedicated.coreset1_l_crb.value();
    }
    const unsigned coreset1_nof_resources = cset1_l_crb / pdcch_constants::NOF_RB_PER_FREQ_RESOURCE;
    freq_resources.fill(cset1_start_crb / pdcch_constants::NOF_RB_PER_FREQ_RESOURCE,
                        cset1_start_crb / pdcch_constants::NOF_RB_PER_FREQ_RESOURCE + coreset1_nof_resources,
                        true);

    search_space_configuration& ss2_cfg   = out_cell.ue_ded_serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces[0];
    coreset_configuration&      cset1_cfg = out_cell.ue_ded_serv_cell_cfg.init_dl_bwp.pdcch_cfg->coresets[0];
    cset1_cfg.set_freq_domain_resources(freq_resources);
    if (base_cell.pdcch_cfg.dedicated.coreset1_duration.has_value()) {
      cset1_cfg.duration = base_cell.pdcch_cfg.dedicated.coreset1_duration.value();
    } else {
      cset1_cfg.duration =
          std::max(2U, static_cast<unsigned>(out_cell.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->duration));
    }
    const std::array<uint8_t, 5> auto_compute_ss2_n_candidates_cfg = {0, 0, 0, 0, 0};
    if (base_cell.pdcch_cfg.dedicated.ss2_n_candidates != auto_compute_ss2_n_candidates_cfg) {
      ss2_cfg.set_non_ss0_nof_candidates(base_cell.pdcch_cfg.dedicated.ss2_n_candidates);
    } else if (base_cell.pdcch_cfg.dedicated.ss2_type != search_space_configuration::type_t::common) {
      ss2_cfg.set_non_ss0_nof_candidates(
          {0, 0, config_helpers::compute_max_nof_candidates(aggregation_level::n4, cset1_cfg), 0, 0});
    }

    if (base_cell.pdcch_cfg.dedicated.ss2_type == search_space_configuration::type_t::common) {
      ss2_cfg.set_non_ss0_monitored_dci_formats(search_space_configuration::common_dci_format{.f0_0_and_f1_0 = true});
      // TODO: Handle this implementation defined restriction of max. 4 PDCCH candidates in validator.
      if (base_cell.pdcch_cfg.dedicated.ss2_n_candidates == auto_compute_ss2_n_candidates_cfg) {
        ss2_cfg.set_non_ss0_nof_candidates(
            {0,
             0,
             std::min(static_cast<uint8_t>(4U),
                      config_helpers::compute_max_nof_candidates(aggregation_level::n4, cset1_cfg)),
             0,
             0});
      }
    } else if (not base_cell.pdcch_cfg.dedicated.dci_format_0_1_and_1_1) {
      search_space_configuration& ss_cfg = out_cell.ue_ded_serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces[0];
      ss_cfg.set_non_ss0_monitored_dci_formats(search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0);
    }

    // PDSCH-Config - Update PDSCH time domain resource allocations based on partial slot.
    if (band_helper::get_duplex_mode(param.band.value()) == duplex_mode::TDD) {
      const auto& tdd_cfg = out_cell.tdd_ul_dl_cfg_common.value();
      out_cell.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list =
          config_helpers::make_pdsch_time_domain_resource(param.search_space0_index,
                                                          out_cell.dl_cfg_common.init_dl_bwp.pdcch_common,
                                                          out_cell.ue_ded_serv_cell_cfg.init_dl_bwp.pdcch_cfg,
                                                          tdd_cfg);
    }

    out_cell.ue_ded_serv_cell_cfg.pdsch_serv_cell_cfg->nof_harq_proc =
        (pdsch_serving_cell_config::nof_harq_proc_for_pdsch)config.common_cell_cfg.pdsch_cfg.nof_harqs;
    // Set DL MCS table.
    out_cell.ue_ded_serv_cell_cfg.init_dl_bwp.pdsch_cfg->mcs_table = base_cell.pdsch_cfg.mcs_table;

    // Parameters for csiMeasConfig.
    if (param.csi_rs_enabled) {
      fill_csi_resources(out_cell.ue_ded_serv_cell_cfg, base_cell);
    }

    // Parameters for PUCCH-Config builder (these parameters will be used later on to generate the PUCCH resources).
    pucch_builder_params&  du_pucch_cfg           = out_cell.pucch_cfg;
    const pucch_appconfig& user_pucch_cfg         = base_cell.pucch_cfg;
    du_pucch_cfg.nof_ue_pucch_f1_res_harq         = user_pucch_cfg.nof_ue_pucch_f1_res_harq;
    du_pucch_cfg.nof_ue_pucch_f2_res_harq         = user_pucch_cfg.nof_ue_pucch_f2_res_harq;
    du_pucch_cfg.nof_cell_harq_pucch_res_sets     = user_pucch_cfg.nof_cell_harq_pucch_sets;
    du_pucch_cfg.nof_sr_resources                 = user_pucch_cfg.nof_cell_sr_resources;
    du_pucch_cfg.nof_csi_resources                = user_pucch_cfg.nof_cell_csi_resources;
    du_pucch_cfg.f1_params.nof_symbols            = user_pucch_cfg.f1_nof_symbols;
    du_pucch_cfg.f1_params.occ_supported          = user_pucch_cfg.f1_enable_occ;
    du_pucch_cfg.f1_params.nof_cyc_shifts         = static_cast<nof_cyclic_shifts>(user_pucch_cfg.nof_cyclic_shift);
    du_pucch_cfg.f1_params.intraslot_freq_hopping = user_pucch_cfg.f1_intraslot_freq_hopping;
    du_pucch_cfg.f2_params.nof_symbols            = user_pucch_cfg.f2_nof_symbols;
    du_pucch_cfg.f2_params.max_code_rate          = user_pucch_cfg.max_code_rate;
    du_pucch_cfg.f2_params.max_nof_rbs            = user_pucch_cfg.f2_max_nof_rbs;
    du_pucch_cfg.f2_params.intraslot_freq_hopping = user_pucch_cfg.f2_intraslot_freq_hopping;
    du_pucch_cfg.f2_params.max_payload_bits       = user_pucch_cfg.max_payload_bits;

    // Parameters for PUSCH-Config.
    if (not out_cell.ue_ded_serv_cell_cfg.ul_config.has_value()) {
      out_cell.ue_ded_serv_cell_cfg.ul_config.emplace();
    }
    if (not out_cell.ue_ded_serv_cell_cfg.ul_config.value().init_ul_bwp.pusch_cfg.has_value()) {
      out_cell.ue_ded_serv_cell_cfg.ul_config.value().init_ul_bwp.pusch_cfg.emplace();
    }
    // Set UL MCS table.
    out_cell.ue_ded_serv_cell_cfg.ul_config->init_ul_bwp.pusch_cfg->mcs_table = base_cell.pusch_cfg.mcs_table;
    if (not out_cell.ue_ded_serv_cell_cfg.ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.has_value()) {
      out_cell.ue_ded_serv_cell_cfg.ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.emplace();
    }
    if (not out_cell.ue_ded_serv_cell_cfg.ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.has_value()) {
      out_cell.ue_ded_serv_cell_cfg.ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.emplace();
    }
    if (not out_cell.ue_ded_serv_cell_cfg.ul_config.value()
                .init_ul_bwp.pusch_cfg.value()
                .uci_cfg.value()
                .beta_offsets_cfg.has_value()) {
      out_cell.ue_ded_serv_cell_cfg.ul_config.value()
          .init_ul_bwp.pusch_cfg.value()
          .uci_cfg.value()
          .beta_offsets_cfg->emplace<uci_on_pusch::beta_offsets_semi_static>();
    }
    if (not variant_holds_alternative<uci_on_pusch::beta_offsets_semi_static>(
            out_cell.ue_ded_serv_cell_cfg.ul_config.value()
                .init_ul_bwp.pusch_cfg.value()
                .uci_cfg.value()
                .beta_offsets_cfg.value())) {
      out_cell.ue_ded_serv_cell_cfg.ul_config.value()
          .init_ul_bwp.pusch_cfg.value()
          .uci_cfg.value()
          .beta_offsets_cfg.reset();
      out_cell.ue_ded_serv_cell_cfg.ul_config.value()
          .init_ul_bwp.pusch_cfg.value()
          .uci_cfg.value()
          .beta_offsets_cfg->emplace<uci_on_pusch::beta_offsets_semi_static>();
    }
    beta_offsets& b_offsets =
        variant_get<uci_on_pusch::beta_offsets_semi_static>(out_cell.ue_ded_serv_cell_cfg.ul_config.value()
                                                                .init_ul_bwp.pusch_cfg.value()
                                                                .uci_cfg.value()
                                                                .beta_offsets_cfg.value());
    b_offsets.beta_offset_ack_idx_1    = base_cell.pusch_cfg.b_offset_ack_idx_1;
    b_offsets.beta_offset_ack_idx_2    = base_cell.pusch_cfg.b_offset_ack_idx_2;
    b_offsets.beta_offset_ack_idx_3    = base_cell.pusch_cfg.b_offset_ack_idx_3;
    b_offsets.beta_offset_csi_p1_idx_1 = base_cell.pusch_cfg.b_offset_csi_p1_idx_1;
    b_offsets.beta_offset_csi_p1_idx_2 = base_cell.pusch_cfg.b_offset_csi_p1_idx_2;
    b_offsets.beta_offset_csi_p2_idx_1 = base_cell.pusch_cfg.b_offset_csi_p2_idx_1;
    b_offsets.beta_offset_csi_p2_idx_2 = base_cell.pusch_cfg.b_offset_csi_p2_idx_2;

    // Parameters for PUCCH-Config.
    if (not out_cell.ue_ded_serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.has_value()) {
      out_cell.ue_ded_serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.emplace();
    }
    auto& sr_cng = out_cell.ue_ded_serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.value().sr_res_list;
    if (sr_cng.empty()) {
      sr_cng.emplace_back(scheduling_request_resource_config{});
    }
    sr_cng.front().period = static_cast<sr_periodicity>(get_nof_slots_per_subframe(base_cell.common_scs) *
                                                        base_cell.pucch_cfg.sr_period_msec);

    // If any dependent parameter needs to be updated, this is the place.
    if (update_msg1_frequency_start) {
      rach_cfg.rach_cfg_generic.msg1_frequency_start = config_helpers::compute_prach_frequency_start(
          du_pucch_cfg, out_cell.ul_cfg_common.init_ul_bwp.generic_params.crbs.length(), is_long_prach);
    }

    logger.info(
        "SSB derived parameters for cell: {}, band: {}, dl_arfcn:{}, crbs: {} scs:{}, ssb_scs:{}:\n\t - SSB offset "
        "pointA:{} \n\t - k_SSB:{} \n\t - SSB arfcn:{} \n\t - Coreset index:{} \n\t - Searchspace index:{}",
        base_cell.pci,
        *param.band,
        base_cell.dl_arfcn,
        nof_crbs,
        to_string(base_cell.common_scs),
        to_string(out_cfg.back().ssb_cfg.scs),
        (*ssb_freq_loc).offset_to_point_A.to_uint(),
        (*ssb_freq_loc).k_ssb.to_uint(),
        (*ssb_freq_loc).ssb_arfcn,
        (*ssb_freq_loc).coreset0_idx,
        (*ssb_freq_loc).searchspace0_idx);

    error_type<std::string> error = is_du_cell_config_valid(out_cfg.back());
    if (!error) {
      report_error("Invalid configuration DU cell detected.\n> {}\n", error.error());
    }
    ++cell_id;
  }

  return out_cfg;
}

std::map<five_qi_t, srs_cu_cp::cu_cp_qos_config> srsran::generate_cu_cp_qos_config(const gnb_appconfig& config)
{
  std::map<five_qi_t, srs_cu_cp::cu_cp_qos_config> out_cfg = {};
  if (config.qos_cfg.empty()) {
    out_cfg = config_helpers::make_default_cu_cp_qos_config_list();
    return out_cfg;
  }

  for (const qos_appconfig& qos : config.qos_cfg) {
    if (out_cfg.find(qos.five_qi) != out_cfg.end()) {
      report_error("Duplicate 5QI configuration: 5QI={}\n", qos.five_qi);
    }
    // Convert PDCP config
    pdcp_config& out_pdcp = out_cfg[qos.five_qi].pdcp;

    // RB type
    out_pdcp.rb_type = pdcp_rb_type::drb;

    // RLC mode
    rlc_mode mode = {};
    if (!from_string(mode, qos.rlc.mode)) {
      report_error("Invalid RLC mode: 5QI={}, mode={}\n", qos.five_qi, qos.rlc.mode);
    }
    if (mode == rlc_mode::um_bidir || mode == rlc_mode::um_unidir_ul || mode == rlc_mode::um_unidir_dl) {
      out_pdcp.rlc_mode = pdcp_rlc_mode::um;
    } else if (mode == rlc_mode::am) {
      out_pdcp.rlc_mode = pdcp_rlc_mode::am;
    } else {
      report_error("Invalid RLC mode: 5QI={}, mode={}\n", qos.five_qi, qos.rlc.mode);
    }

    // Integrity Protection required
    out_pdcp.integrity_protection_required = qos.pdcp.integrity_protection_required;

    // Ciphering required
    out_pdcp.ciphering_required = true;

    // > Tx
    // >> SN size
    if (!pdcp_sn_size_from_uint(out_pdcp.tx.sn_size, qos.pdcp.tx.sn_field_length)) {
      report_error("Invalid PDCP TX SN: 5QI={}, SN={}\n", qos.five_qi, qos.pdcp.tx.sn_field_length);
    }

    // >> discard timer
    out_pdcp.tx.discard_timer = pdcp_discard_timer{};
    if (!pdcp_discard_timer_from_int(out_pdcp.tx.discard_timer.value(), qos.pdcp.tx.discard_timer)) {
      report_error("Invalid PDCP discard timer. 5QI {} discard_timer {}\n", qos.five_qi, qos.pdcp.tx.discard_timer);
    }

    // >> status report required
    out_pdcp.tx.status_report_required = qos.pdcp.tx.status_report_required;

    // > Rx
    // >> SN size
    if (!pdcp_sn_size_from_uint(out_pdcp.rx.sn_size, qos.pdcp.rx.sn_field_length)) {
      report_error("Invalid PDCP RX SN: 5QI={}, SN={}\n", qos.five_qi, qos.pdcp.rx.sn_field_length);
    }

    // >> out of order delivery
    out_pdcp.rx.out_of_order_delivery = qos.pdcp.rx.out_of_order_delivery;

    // >> t-Reordering
    if (!pdcp_t_reordering_from_int(out_pdcp.rx.t_reordering, qos.pdcp.rx.t_reordering)) {
      report_error("Invalid PDCP t-Reordering. 5QI {} t-Reordering {}\n", qos.five_qi, qos.pdcp.rx.t_reordering);
    }
  }
  return out_cfg;
}

std::map<five_qi_t, du_qos_config> srsran::generate_du_qos_config(const gnb_appconfig& config)
{
  std::map<five_qi_t, du_qos_config> out_cfg = {};
  if (config.qos_cfg.empty()) {
    out_cfg = config_helpers::make_default_du_qos_config_list(config.metrics_cfg.rlc_report_period);
    return out_cfg;
  }

  for (const qos_appconfig& qos : config.qos_cfg) {
    if (out_cfg.find(qos.five_qi) != out_cfg.end()) {
      report_error("Duplicate 5QI configuration: 5QI={}\n", qos.five_qi);
    }
    // Convert RLC config
    auto& out_rlc = out_cfg[qos.five_qi].rlc;
    if (!from_string(out_rlc.mode, qos.rlc.mode)) {
      report_error("Invalid RLC mode: 5QI={}, mode={}\n", qos.five_qi, qos.rlc.mode);
    }

    if (out_rlc.mode == rlc_mode::um_bidir) {
      // UM Config
      //< RX SN
      if (!from_number(out_rlc.um.rx.sn_field_length, qos.rlc.um.rx.sn_field_length)) {
        report_error("Invalid RLC UM RX SN: 5QI={}, SN={}\n", qos.five_qi, qos.rlc.um.rx.sn_field_length);
      }
      //< RX t-reassembly
      out_rlc.um.rx.t_reassembly = qos.rlc.um.rx.t_reassembly;
      //< TX SN
      if (!from_number(out_rlc.um.tx.sn_field_length, qos.rlc.um.tx.sn_field_length)) {
        report_error("Invalid RLC UM TX SN: 5QI={}, SN={}\n", qos.five_qi, qos.rlc.um.tx.sn_field_length);
      }
      out_rlc.um.tx.queue_size = qos.rlc.um.tx.queue_size;
    } else if (out_rlc.mode == rlc_mode::am) {
      // AM Config
      //<  TX SN
      if (!from_number(out_rlc.am.tx.sn_field_length, qos.rlc.am.tx.sn_field_length)) {
        report_error("Invalid RLC AM TX SN: 5QI={}, SN={}\n", qos.five_qi, qos.rlc.am.tx.sn_field_length);
      }
      out_rlc.am.tx.t_poll_retx     = qos.rlc.am.tx.t_poll_retx;
      out_rlc.am.tx.max_retx_thresh = qos.rlc.am.tx.max_retx_thresh;
      out_rlc.am.tx.poll_pdu        = qos.rlc.am.tx.poll_pdu;
      out_rlc.am.tx.poll_byte       = qos.rlc.am.tx.poll_byte;
      out_rlc.am.tx.max_window      = qos.rlc.am.tx.max_window;
      out_rlc.am.tx.queue_size      = qos.rlc.am.tx.queue_size;
      //< RX SN
      if (!from_number(out_rlc.am.rx.sn_field_length, qos.rlc.am.rx.sn_field_length)) {
        report_error("Invalid RLC AM RX SN: 5QI={}, SN={}\n", qos.five_qi, qos.rlc.am.rx.sn_field_length);
      }
      out_rlc.am.rx.t_reassembly      = qos.rlc.am.rx.t_reassembly;
      out_rlc.am.rx.t_status_prohibit = qos.rlc.am.rx.t_status_prohibit;
    }
    out_rlc.metrics_period = std::chrono::milliseconds(config.metrics_cfg.rlc_report_period);

    // Convert F1-U config
    auto& out_f1u = out_cfg[qos.five_qi].f1u;
    //< t-Notify
    out_f1u.t_notify = qos.f1u_du.t_notify;
  }
  return out_cfg;
}

/// Fills the given low PHY configuration from the given gnb configuration.
static void generate_low_phy_config(lower_phy_configuration&           out_cfg,
                                    const cell_appconfig&              config,
                                    const ru_sdr_appconfig&            ru_cfg,
                                    const ru_sdr_cell_appconfig&       ru_cell_cfg,
                                    const lower_phy_threads_appconfig& low_phy_threads_cfg,
                                    unsigned                           max_processing_delay_slot)
{
  const base_cell_appconfig& cell_cfg = config.cell;
  out_cfg.scs                         = cell_cfg.common_scs;
  out_cfg.cp                          = cp;
  out_cfg.dft_window_offset           = 0.5F;
  out_cfg.max_processing_delay_slots  = max_processing_delay_slot;

  out_cfg.srate = sampling_rate::from_MHz(ru_cfg.srate_MHz);

  out_cfg.ta_offset = band_helper::get_ta_offset(*cell_cfg.band);
  if (ru_cfg.time_alignment_calibration.has_value()) {
    // Selects the user specific value.
    out_cfg.time_alignment_calibration = ru_cfg.time_alignment_calibration.value();
  } else {
    // Selects a default parameter that ensures a valid time alignment in the MSG1 (PRACH).
    out_cfg.time_alignment_calibration = 0;
  }

  // Select buffer size policy.
  if (ru_cfg.device_driver == "zmq") {
    out_cfg.baseband_tx_buffer_size_policy = lower_phy_baseband_buffer_size_policy::half_slot;
    out_cfg.baseband_rx_buffer_size_policy = lower_phy_baseband_buffer_size_policy::half_slot;
  } else if (low_phy_threads_cfg.execution_profile == lower_phy_thread_profile::single) {
    // For single executor, the same executor processes uplink and downlink. In this case, the processing is blocked
    // by the signal reception. The buffers must be smaller than a slot duration considering the downlink baseband
    // samples must arrive to the baseband device before the transmission time passes.
    out_cfg.baseband_tx_buffer_size_policy = lower_phy_baseband_buffer_size_policy::single_packet;
    out_cfg.baseband_rx_buffer_size_policy = lower_phy_baseband_buffer_size_policy::single_packet;
  } else {
    out_cfg.baseband_tx_buffer_size_policy = lower_phy_baseband_buffer_size_policy::slot;
    out_cfg.baseband_rx_buffer_size_policy = lower_phy_baseband_buffer_size_policy::single_packet;
  }

  // Get lower PHY system time throttling.
  out_cfg.system_time_throttling = ru_cfg.expert_cfg.lphy_dl_throttling;

  const unsigned bandwidth_sc =
      NOF_SUBCARRIERS_PER_RB *
      band_helper::get_n_rbs_from_bw(cell_cfg.channel_bw_mhz, cell_cfg.common_scs, frequency_range::FR1);

  // Apply gain back-off to account for the PAPR of the signal and the DFT power normalization.
  out_cfg.amplitude_config.input_gain_dB =
      -convert_power_to_dB(static_cast<float>(bandwidth_sc)) - ru_cfg.cells.back().amplitude_cfg.gain_backoff_dB;

  // If clipping is enabled, the amplitude controller will clip the IQ components when their amplitude comes within
  // 0.1 dB of the radio full scale value.
  out_cfg.amplitude_config.ceiling_dBFS = ru_cell_cfg.amplitude_cfg.power_ceiling_dBFS;

  out_cfg.amplitude_config.enable_clipping = ru_cell_cfg.amplitude_cfg.enable_clipping;

  // Set the full scale amplitude reference to 1.
  out_cfg.amplitude_config.full_scale_lin = 1.0F;

  lower_phy_sector_description sector_config;
  sector_config.bandwidth_rb =
      band_helper::get_n_rbs_from_bw(cell_cfg.channel_bw_mhz, cell_cfg.common_scs, frequency_range::FR1);
  sector_config.dl_freq_hz = band_helper::nr_arfcn_to_freq(cell_cfg.dl_arfcn);
  sector_config.ul_freq_hz =
      band_helper::nr_arfcn_to_freq(band_helper::get_ul_arfcn_from_dl_arfcn(cell_cfg.dl_arfcn, cell_cfg.band));
  sector_config.nof_rx_ports = cell_cfg.nof_antennas_ul;
  sector_config.nof_tx_ports = cell_cfg.nof_antennas_dl;
  out_cfg.sectors.push_back(sector_config);

  if (!is_valid_lower_phy_config(out_cfg)) {
    report_error("Invalid lower PHY configuration.\n");
  }
}

/// Slice the given string by the ',' limiter, and returns a vector with each position containing one slice of the
/// string.
static std::vector<std::string> split_rf_driver_args(const std::string& driver_args)
{
  std::stringstream        ss(driver_args);
  std::vector<std::string> result;

  while (ss.good()) {
    std::string str;
    getline(ss, str, ',');
    if (!str.empty()) {
      result.push_back(str);
    }
  }

  return result;
}

/// Finds the ZMQ ports within the given driver arguments. Returns a vector that contains with the ZMQ transmission or
/// reception ports.
static std::vector<std::string> extract_zmq_ports(const std::string& driver_args, const std::string& port_id)
{
  std::vector<std::string> ports;

  const std::vector<std::string>& splitted_args = split_rf_driver_args(driver_args);
  for (const auto& arg : splitted_args) {
    auto I = arg.find(port_id);

    if (I == std::string::npos) {
      continue;
    }

    I = arg.find("=");
    ports.push_back(arg.substr(++I));
  }

  return ports;
}

static double calibrate_center_freq_Hz(double center_freq_Hz, double freq_offset_Hz, double calibration_ppm)
{
  return (center_freq_Hz + freq_offset_Hz) * (1.0 + calibration_ppm * 1e-6);
}

static void generate_radio_config(radio_configuration::radio& out_cfg, const gnb_appconfig& config)
{
  const ru_sdr_appconfig& ru_cfg = variant_get<ru_sdr_appconfig>(config.ru_cfg);

  out_cfg.args             = ru_cfg.device_arguments;
  out_cfg.log_level        = config.log_cfg.radio_level;
  out_cfg.sampling_rate_hz = ru_cfg.srate_MHz * 1e6;
  out_cfg.otw_format       = radio_configuration::to_otw_format(ru_cfg.otw_format);
  out_cfg.clock.clock      = radio_configuration::to_clock_source(ru_cfg.clock_source);
  out_cfg.clock.sync       = radio_configuration::to_clock_source(ru_cfg.synch_source);

  const std::vector<std::string>& zmq_tx_addr = extract_zmq_ports(ru_cfg.device_arguments, "tx_port");
  const std::vector<std::string>& zmq_rx_addr = extract_zmq_ports(ru_cfg.device_arguments, "rx_port");

  // For each sector...
  for (unsigned sector_id = 0; sector_id != config.cells_cfg.size(); ++sector_id) {
    // Select cell configuration.
    const base_cell_appconfig& cell = config.cells_cfg[sector_id].cell;

    // Each cell is mapped to a different stream.
    radio_configuration::stream tx_stream_config;
    radio_configuration::stream rx_stream_config;

    // Deduce center frequencies.
    const double cell_tx_freq_Hz = band_helper::nr_arfcn_to_freq(cell.dl_arfcn);
    const double cell_rx_freq_Hz =
        band_helper::nr_arfcn_to_freq(band_helper::get_ul_arfcn_from_dl_arfcn(cell.dl_arfcn, cell.band));

    // Correct actual RF center frequencies considering offset and PPM calibration.
    double center_tx_freq_cal_Hz =
        calibrate_center_freq_Hz(cell_tx_freq_Hz, ru_cfg.center_freq_offset_Hz, ru_cfg.calibrate_clock_ppm);
    double center_rx_freq_cal_Hz =
        calibrate_center_freq_Hz(cell_rx_freq_Hz, ru_cfg.center_freq_offset_Hz, ru_cfg.calibrate_clock_ppm);

    // Calculate actual LO frequencies considering LO frequency offset and the frequency correction.
    double lo_tx_freq_cal_Hz = calibrate_center_freq_Hz(
        cell_tx_freq_Hz + ru_cfg.lo_offset_MHz * 1e6, ru_cfg.center_freq_offset_Hz, ru_cfg.calibrate_clock_ppm);
    double lo_rx_freq_cal_Hz = calibrate_center_freq_Hz(
        cell_rx_freq_Hz + ru_cfg.lo_offset_MHz * 1e6, ru_cfg.center_freq_offset_Hz, ru_cfg.calibrate_clock_ppm);

    // For each DL antenna port in the cell...
    for (unsigned port_id = 0; port_id != cell.nof_antennas_dl; ++port_id) {
      // Create channel configuration and append it to the previous ones.
      radio_configuration::channel tx_ch_config = {};
      tx_ch_config.freq.center_frequency_hz     = center_tx_freq_cal_Hz;
      if (std::isnormal(ru_cfg.lo_offset_MHz)) {
        tx_ch_config.freq.lo_frequency_hz = lo_tx_freq_cal_Hz;
      } else {
        tx_ch_config.freq.lo_frequency_hz = 0.0;
      }
      tx_ch_config.gain_dB = ru_cfg.tx_gain_dB;

      // Add the TX ports.
      if (ru_cfg.device_driver == "zmq") {
        if (sector_id * cell.nof_antennas_dl + port_id >= zmq_tx_addr.size()) {
          report_error("ZMQ transmission channel arguments out of bounds\n");
        }

        tx_ch_config.args = zmq_tx_addr[sector_id * cell.nof_antennas_dl + port_id];
      }
      tx_stream_config.channels.emplace_back(tx_ch_config);
    }
    out_cfg.tx_streams.emplace_back(tx_stream_config);

    // For each UL antenna port in the cell...
    for (unsigned port_id = 0; port_id != cell.nof_antennas_ul; ++port_id) {
      // Create channel configuration and append it to the previous ones.
      radio_configuration::channel rx_ch_config = {};
      rx_ch_config.freq.center_frequency_hz     = center_rx_freq_cal_Hz;
      if (std::isnormal(ru_cfg.lo_offset_MHz)) {
        rx_ch_config.freq.lo_frequency_hz = lo_rx_freq_cal_Hz;
      } else {
        rx_ch_config.freq.lo_frequency_hz = 0.0;
      }
      rx_ch_config.gain_dB = ru_cfg.rx_gain_dB;

      // Add the RX ports.
      if (ru_cfg.device_driver == "zmq") {
        if (sector_id * cell.nof_antennas_dl + port_id >= zmq_rx_addr.size()) {
          report_error("ZMQ reception channel arguments out of bounds\n");
        }

        rx_ch_config.args = zmq_rx_addr[sector_id * cell.nof_antennas_dl + port_id];
      }
      rx_stream_config.channels.emplace_back(rx_ch_config);
    }
    out_cfg.rx_streams.emplace_back(rx_stream_config);
  }
}

static void generate_ru_generic_config(ru_generic_configuration& out_cfg, const gnb_appconfig& config)
{
  const ru_sdr_appconfig& ru_cfg = variant_get<ru_sdr_appconfig>(config.ru_cfg);

  out_cfg.device_driver = ru_cfg.device_driver;
  generate_radio_config(out_cfg.radio_cfg, config);

  for (unsigned i = 0, e = config.cells_cfg.size(); i != e; ++i) {
    out_cfg.lower_phy_config.emplace_back();
    generate_low_phy_config(out_cfg.lower_phy_config.back(),
                            config.cells_cfg[i],
                            ru_cfg,
                            ru_cfg.cells[i],
                            config.expert_execution_cfg.threads.lower_threads,
                            config.expert_phy_cfg.max_processing_delay_slots);
  }
}

static bool parse_mac_address(const std::string& mac_str, span<uint8_t> mac)
{
  std::array<unsigned, 6> data       = {};
  int                     bytes_read = std::sscanf(
      mac_str.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x", &data[0], &data[1], &data[2], &data[3], &data[4], &data[5]);
  if (bytes_read != ether::ETH_ADDR_LEN) {
    fmt::print("Invalid MAC address provided: {}\n", mac_str);
    return false;
  }

  std::copy(data.begin(), data.end(), mac.begin());

  return true;
}

static void
generate_ru_ofh_config(ru_ofh_configuration& out_cfg, const gnb_appconfig& config, span<const du_cell_config> du_cells)
{
  const ru_ofh_appconfig& ru_cfg = variant_get<ru_ofh_appconfig>(config.ru_cfg);

  /// Individual Open Fronthaul sector configurations.
  std::vector<ru_ofh_sector_configuration> sector_configs;
  const base_cell_appconfig&               cell = config.cells_cfg.front().cell;

  out_cfg.gps_Alpha                  = ru_cfg.gps_Alpha;
  out_cfg.gps_Beta                   = ru_cfg.gps_Beta;
  out_cfg.max_processing_delay_slots = config.expert_phy_cfg.max_processing_delay_slots;
  out_cfg.dl_processing_time         = std::chrono::microseconds(ru_cfg.dl_processing_time);
  out_cfg.uses_dpdk                  = config.hal_config.has_value();

  // Add one cell.
  for (unsigned i = 0, e = ru_cfg.cells.size(); i != e; ++i) {
    const ru_ofh_cell_appconfig& cell_cfg    = ru_cfg.cells[i];
    const du_cell_config&        du_cell_cfg = du_cells[i];
    out_cfg.sector_configs.emplace_back();
    ru_ofh_sector_configuration& sector_cfg = out_cfg.sector_configs.back();

    sector_cfg.interface                   = cell_cfg.network_interface;
    sector_cfg.is_promiscuous_mode_enabled = cell_cfg.enable_promiscuous_mode;
    if (!parse_mac_address(cell_cfg.du_mac_address, sector_cfg.mac_src_address)) {
      srsran_terminate("Invalid Distributed Unit MAC address");
    }

    if (!parse_mac_address(cell_cfg.ru_mac_address, sector_cfg.mac_dst_address)) {
      srsran_terminate("Invalid Radio Unit MAC address");
    }

    sector_cfg.cp                                  = cyclic_prefix::NORMAL;
    sector_cfg.scs                                 = cell.common_scs;
    sector_cfg.bw                                  = cell.channel_bw_mhz;
    sector_cfg.nof_antennas_ul                     = cell.nof_antennas_ul;
    sector_cfg.ru_operating_bw                     = cell_cfg.cell.ru_operating_bw;
    sector_cfg.is_uplink_static_comp_hdr_enabled   = cell_cfg.cell.is_uplink_static_comp_hdr_enabled;
    sector_cfg.is_downlink_static_comp_hdr_enabled = cell_cfg.cell.is_downlink_static_comp_hdr_enabled;
    sector_cfg.tx_window_timing_params             = {std::chrono::microseconds(cell_cfg.cell.T1a_max_cp_dl),
                                                      std::chrono::microseconds(cell_cfg.cell.T1a_min_cp_dl),
                                                      std::chrono::microseconds(cell_cfg.cell.T1a_max_cp_ul),
                                                      std::chrono::microseconds(cell_cfg.cell.T1a_min_cp_ul),
                                                      std::chrono::microseconds(cell_cfg.cell.T1a_max_up),
                                                      std::chrono::microseconds(cell_cfg.cell.T1a_min_up)};
    sector_cfg.rx_window_timing_params             = {std::chrono::microseconds(cell_cfg.cell.Ta4_max),
                                                      std::chrono::microseconds(cell_cfg.cell.Ta4_min)};
    sector_cfg.is_prach_control_plane_enabled      = cell_cfg.cell.is_prach_control_plane_enabled;
    sector_cfg.is_downlink_broadcast_enabled       = cell_cfg.cell.is_downlink_broadcast_enabled;
    sector_cfg.ignore_ecpri_payload_size_field     = cell_cfg.cell.ignore_ecpri_payload_size_field;
    sector_cfg.ul_compression_params               = {ofh::to_compression_type(cell_cfg.cell.compression_method_ul),
                                                      cell_cfg.cell.compression_bitwidth_ul};
    sector_cfg.dl_compression_params               = {ofh::to_compression_type(cell_cfg.cell.compression_method_dl),
                                                      cell_cfg.cell.compression_bitwidth_dl};
    sector_cfg.prach_compression_params            = {ofh::to_compression_type(cell_cfg.cell.compression_method_prach),
                                                      cell_cfg.cell.compression_bitwidth_prach};
    sector_cfg.iq_scaling                          = cell_cfg.cell.iq_scaling;

    sector_cfg.tci = cell_cfg.vlan_tag;
    sector_cfg.prach_eaxc.assign(cell_cfg.ru_prach_port_id.begin(), cell_cfg.ru_prach_port_id.end());
    sector_cfg.dl_eaxc.assign(cell_cfg.ru_dl_port_id.begin(), cell_cfg.ru_dl_port_id.end());
    sector_cfg.ul_eaxc.assign(cell_cfg.ru_ul_port_id.begin(), cell_cfg.ru_ul_port_id.end());

    // TDD UL DL config.
    sector_cfg.tdd_config = du_cell_cfg.tdd_ul_dl_cfg_common;
  }

  if (!is_valid_ru_ofh_config(out_cfg)) {
    report_error("Invalid Open Fronthaul Radio Unit configuration detected.\n");
  }
}

ru_configuration srsran::generate_ru_config(const gnb_appconfig& config, span<const du_cell_config> cells)
{
  ru_configuration out_cfg;

  if (variant_holds_alternative<ru_sdr_appconfig>(config.ru_cfg)) {
    ru_generic_configuration& cfg = out_cfg.config.emplace<ru_generic_configuration>();
    generate_ru_generic_config(cfg, config);
  } else {
    ru_ofh_configuration& cfg = out_cfg.config.emplace<ru_ofh_configuration>();
    generate_ru_ofh_config(cfg, config, cells);
  }

  return out_cfg;
}

std::vector<upper_phy_config> srsran::generate_du_low_config(const gnb_appconfig& config)
{
  std::vector<upper_phy_config> out_cfg;
  out_cfg.reserve(config.cells_cfg.size());

  for (unsigned i = 0, e = config.cells_cfg.size(); i != e; ++i) {
    const base_cell_appconfig& cell = config.cells_cfg[i].cell;
    upper_phy_config           cfg;

    // Get bandwidth in PRB.
    const unsigned bw_rb = band_helper::get_n_rbs_from_bw(cell.channel_bw_mhz, cell.common_scs, frequency_range::FR1);
    // Build the biggest CORESET possible assuming a duration of 2 symbols and the maximum channel bandwidth.
    coreset_configuration coreset;
    coreset.id       = to_coreset_id(1);
    coreset.duration = 2;
    coreset.set_freq_domain_resources(~freq_resource_bitmap(bw_rb / pdcch_constants::NOF_RB_PER_FREQ_RESOURCE));
    // Calculate the maximum number of users assuming the CORESET above.
    const unsigned max_nof_users_slot = coreset.get_nof_cces();
    // Assume a maximum of 16 HARQ processes.
    const unsigned max_harq_process = 16;
    // Deduce the number of slots per subframe.
    const unsigned nof_slots_per_subframe = get_nof_slots_per_subframe(config.common_cell_cfg.common_scs);
    // Assume the HARQ softbuffer expiration time in slots is 5 times the number of HARQ processes.
    const unsigned expire_harq_timeout_slots = 100 * nof_slots_per_subframe;
    // Assume the maximum number of active UL HARQ processes is twice the maximum number of users per slot for the
    // maximum number of HARQ processes.
    const unsigned max_softbuffers = 2 * max_nof_users_slot * max_harq_process;
    // Deduce the maximum number of codeblocks that can be scheduled for PUSCH in one slot.
    const unsigned max_nof_pusch_cb_slot =
        (pusch_constants::MAX_NRE_PER_RB * bw_rb * get_bits_per_symbol(modulation_scheme::QAM256)) /
        ldpc::MAX_MESSAGE_SIZE;
    // Assume the minimum number of codeblocks per softbuffer.
    const unsigned min_cb_softbuffer = 2;
    // Assume that the maximum number of codeblocks is equal to the number of HARQ processes times the maximum number
    // of codeblocks per slot.
    const unsigned max_nof_codeblocks =
        std::max(max_harq_process * max_nof_pusch_cb_slot, min_cb_softbuffer * max_softbuffers);

    unsigned                  dl_pipeline_depth    = 4 * config.expert_phy_cfg.max_processing_delay_slots;
    unsigned                  ul_pipeline_depth    = 4 * config.expert_phy_cfg.max_processing_delay_slots;
    static constexpr unsigned prach_pipeline_depth = 1;

    nr_band           band   = config.common_cell_cfg.band.value();
    const duplex_mode duplex = band_helper::get_duplex_mode(band);

    const prach_configuration prach_cfg =
        prach_configuration_get(frequency_range::FR1, duplex, cell.prach_cfg.prach_config_index.value());

    // Maximum number of HARQ processes for a PUSCH HARQ process.
    static constexpr unsigned max_nof_pusch_harq = 16;

    // Maximum concurrent PUSCH processing. If there are no dedicated threads for PUSCH decoding, set the maximum
    // concurrency to one. Otherwise, assume every possible PUSCH transmission for the maximum number of HARQ could be
    // enqueued.
    unsigned max_pusch_concurrency = config.common_cell_cfg.pusch_cfg.max_puschs_per_slot * max_nof_pusch_harq;
    if (config.expert_execution_cfg.threads.upper_threads.nof_pusch_decoder_threads == 0) {
      max_pusch_concurrency = 1;
    }

    cfg.log_level                  = srslog::str_to_basic_level(config.log_cfg.phy_level);
    cfg.enable_logging_broadcast   = config.log_cfg.broadcast_enabled;
    cfg.rx_symbol_printer_filename = config.log_cfg.phy_rx_symbols_filename;
    cfg.logger_max_hex_size        = config.log_cfg.hex_max_size;
    cfg.sector_id                  = i;
    cfg.nof_tx_ports               = cell.nof_antennas_dl;
    cfg.nof_rx_ports               = cell.nof_antennas_ul;
    cfg.ldpc_decoder_iterations    = config.expert_phy_cfg.pusch_decoder_max_iterations;
    cfg.ldpc_decoder_early_stop    = config.expert_phy_cfg.pusch_decoder_early_stop;
    cfg.nof_slots_dl_rg            = dl_pipeline_depth;
    cfg.nof_dl_processors          = dl_pipeline_depth;
    cfg.nof_slots_ul_rg            = ul_pipeline_depth;
    cfg.nof_ul_processors          = ul_pipeline_depth;
    cfg.max_ul_thread_concurrency  = config.expert_execution_cfg.threads.upper_threads.nof_ul_threads + 1;
    cfg.max_pusch_concurrency      = max_pusch_concurrency;
    cfg.nof_pusch_decoder_threads  = config.expert_execution_cfg.threads.upper_threads.nof_pusch_decoder_threads +
                                    config.expert_execution_cfg.threads.upper_threads.nof_ul_threads;
    cfg.nof_prach_buffer           = prach_pipeline_depth * nof_slots_per_subframe;
    cfg.max_nof_td_prach_occasions = prach_cfg.nof_occasions_within_slot;
    cfg.max_nof_fd_prach_occasions = 1;
    cfg.is_prach_long_format       = is_long_preamble(prach_cfg.format);
    cfg.pusch_sinr_calc_method =
        channel_state_information::sinr_type_from_string(config.expert_phy_cfg.pusch_sinr_calc_method);

    cfg.active_scs                                                                = {};
    cfg.active_scs[to_numerology_value(config.cells_cfg.front().cell.common_scs)] = true;

    cfg.dl_bw_rb = bw_rb;
    cfg.ul_bw_rb = bw_rb;

    cfg.softbuffer_config.max_softbuffers      = max_softbuffers;
    cfg.softbuffer_config.max_nof_codeblocks   = max_nof_codeblocks;
    cfg.softbuffer_config.max_codeblock_size   = ldpc::MAX_CODEBLOCK_SIZE;
    cfg.softbuffer_config.expire_timeout_slots = expire_harq_timeout_slots;
    cfg.softbuffer_config.external_soft_bits   = false;

    if (!is_valid_upper_phy_config(cfg)) {
      report_error("Invalid upper PHY configuration.\n");
    }

    out_cfg.push_back(cfg);
  }

  return out_cfg;
}

mac_expert_config srsran::generate_mac_expert_config(const gnb_appconfig& config)
{
  mac_expert_config out_cfg      = {};
  out_cfg.max_consecutive_dl_kos = config.common_cell_cfg.pdsch_cfg.max_consecutive_kos;
  out_cfg.max_consecutive_ul_kos = config.common_cell_cfg.pusch_cfg.max_consecutive_kos;

  return out_cfg;
}

scheduler_expert_config srsran::generate_scheduler_expert_config(const gnb_appconfig& config)
{
  scheduler_expert_config out_cfg = config_helpers::make_default_scheduler_expert_config();

  // UE parameters.
  const pdsch_appconfig& pdsch = config.common_cell_cfg.pdsch_cfg;
  out_cfg.ue.dl_mcs            = {pdsch.min_ue_mcs, pdsch.max_ue_mcs};
  out_cfg.ue.pdsch_rv_sequence.assign(pdsch.rv_sequence.begin(), pdsch.rv_sequence.end());
  out_cfg.ue.dl_harq_la_cqi_drop_threshold     = pdsch.harq_la_cqi_drop_threshold;
  out_cfg.ue.dl_harq_la_ri_drop_threshold      = pdsch.harq_la_ri_drop_threshold;
  out_cfg.ue.max_pdschs_per_slot               = pdsch.max_pdschs_per_slot;
  out_cfg.ue.max_pdcch_alloc_attempts_per_slot = pdsch.max_pdcch_alloc_attempts_per_slot;
  out_cfg.ue.pdsch_nof_rbs                     = {pdsch.min_rb_size, pdsch.max_rb_size};
  out_cfg.ue.olla_dl_target_bler               = pdsch.olla_target_bler;
  out_cfg.ue.olla_cqi_inc                      = pdsch.olla_cqi_inc;
  out_cfg.ue.olla_max_cqi_offset               = pdsch.olla_max_cqi_offset;

  const pusch_appconfig& pusch = config.common_cell_cfg.pusch_cfg;
  out_cfg.ue.ul_mcs            = {pusch.min_ue_mcs, pusch.max_ue_mcs};
  out_cfg.ue.pusch_rv_sequence.assign(pusch.rv_sequence.begin(), pusch.rv_sequence.end());
  out_cfg.ue.initial_ul_dc_offset   = pusch.dc_offset;
  out_cfg.ue.max_puschs_per_slot    = pusch.max_puschs_per_slot;
  out_cfg.ue.olla_ul_target_bler    = pusch.olla_target_bler;
  out_cfg.ue.olla_ul_snr_inc        = pusch.olla_snr_inc;
  out_cfg.ue.olla_max_ul_snr_offset = pusch.olla_max_snr_offset;

  // RA parameters.
  const prach_appconfig& prach = config.common_cell_cfg.prach_cfg;

  out_cfg.ra.rar_mcs_index           = pdsch.fixed_rar_mcs;
  out_cfg.ra.max_nof_msg3_harq_retxs = prach.max_msg3_harq_retx;
  out_cfg.ra.msg3_mcs_index          = prach.fixed_msg3_mcs;

  // SI parameters.
  out_cfg.si.sib1_mcs_index    = pdsch.fixed_sib1_mcs;
  out_cfg.si.sib1_dci_aggr_lev = aggregation_level::n4;

  // Logging and tracing.
  out_cfg.log_broadcast_messages = config.log_cfg.broadcast_enabled;

  const error_type<std::string> error = is_scheduler_expert_config_valid(out_cfg);
  if (!error) {
    report_error("Invalid scheduler expert configuration detected.\n");
  }

  return out_cfg;
}

srsran::sctp_network_gateway_config srsran::generate_e2ap_nw_config(const gnb_appconfig& config, int ppid)
{
  srsran::sctp_network_gateway_config out_cfg;
  out_cfg.connection_name = "NearRT-RIC";
  out_cfg.connect_address = config.e2_cfg.ip_addr;
  out_cfg.connect_port    = config.e2_cfg.port;
  out_cfg.bind_address    = config.e2_cfg.bind_addr;
  out_cfg.ppid            = ppid;

  if (config.e2_cfg.sctp_rto_initial >= 0) {
    out_cfg.rto_initial = config.e2_cfg.sctp_rto_initial;
  }
  if (config.e2_cfg.sctp_rto_min >= 0) {
    out_cfg.rto_min = config.e2_cfg.sctp_rto_min;
  }
  if (config.e2_cfg.sctp_rto_max >= 0) {
    out_cfg.rto_max = config.e2_cfg.sctp_rto_max;
  }
  if (config.e2_cfg.sctp_init_max_attempts >= 0) {
    out_cfg.init_max_attempts = config.e2_cfg.sctp_init_max_attempts;
  }
  if (config.e2_cfg.sctp_max_init_timeo >= 0) {
    out_cfg.max_init_timeo = config.e2_cfg.sctp_max_init_timeo;
  }

  return out_cfg;
}

e2ap_configuration srsran::generate_e2_config(const gnb_appconfig& config)
{
  e2ap_configuration out_cfg = srsran::config_helpers::make_default_e2ap_config();
  out_cfg.gnb_id             = config.gnb_id;
  out_cfg.ran_node_name      = config.ran_node_name;
  out_cfg.plmn               = config.common_cell_cfg.plmn;
  out_cfg.e2sm_kpm_enabled   = config.e2_cfg.e2sm_kpm_enabled;
  out_cfg.e2sm_rc_enabled    = config.e2_cfg.e2sm_rc_enabled;

  return out_cfg;
}

// Derive the parameters set to "auto"-derived for a cell.
static void derive_cell_auto_params(base_cell_appconfig& cell_cfg)
{
  // If NR band is not set, derive a valid one from the DL-ARFCN.
  if (not cell_cfg.band.has_value()) {
    cell_cfg.band = band_helper::get_band_from_dl_arfcn(cell_cfg.dl_arfcn);
  }

  // If in TDD mode, and pattern was not set, generate a pattern DDDDDDXUUU.
  const duplex_mode dplx_mode = band_helper::get_duplex_mode(cell_cfg.band.value());
  if (dplx_mode == duplex_mode::TDD and not cell_cfg.tdd_ul_dl_cfg.has_value()) {
    cell_cfg.tdd_ul_dl_cfg.emplace();
    cell_cfg.tdd_ul_dl_cfg->pattern1.dl_ul_period_slots = 10;
    cell_cfg.tdd_ul_dl_cfg->pattern1.nof_dl_slots       = 6;
    cell_cfg.tdd_ul_dl_cfg->pattern1.nof_dl_symbols     = 8;
    cell_cfg.tdd_ul_dl_cfg->pattern1.nof_ul_slots       = 3;
    cell_cfg.tdd_ul_dl_cfg->pattern1.nof_ul_symbols     = 0;
  }

  // If PRACH configuration Index not set, a default one is assigned.
  if (not cell_cfg.prach_cfg.prach_config_index.has_value()) {
    if (band_helper::get_duplex_mode(cell_cfg.band.value()) == duplex_mode::FDD) {
      cell_cfg.prach_cfg.prach_config_index = 16;
    } else {
      // Valid for TDD period of 5 ms. And, PRACH index 159 is well tested.
      cell_cfg.prach_cfg.prach_config_index = 159;
    }
  }
}

void srsran::derive_auto_params(gnb_appconfig& gnb_params)
{
  derive_cell_auto_params(gnb_params.common_cell_cfg);

  for (auto& cell : gnb_params.cells_cfg) {
    derive_cell_auto_params(cell.cell);
  }
}
