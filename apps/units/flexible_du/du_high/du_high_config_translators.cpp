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

#include "du_high_config_translators.h"
#include "du_high_config.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/du/du_cell_config_validation.h"
#include "srsran/du/du_update_config_helpers.h"
#include "srsran/e2/e2ap_configuration_helpers.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/rlc/rlc_srb_config_factory.h"
#include "srsran/scheduler/config/cell_config_builder_params.h"
#include "srsran/scheduler/config/csi_helper.h"
#include "srsran/scheduler/config/scheduler_expert_config_validator.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

static tdd_ul_dl_config_common generate_tdd_pattern(subcarrier_spacing scs, const du_high_unit_tdd_ul_dl_config& config)
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

static pcch_config generate_pcch_config(const du_high_unit_base_cell_config& cell)
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

static sib2_info create_sib2_info()
{
  sib2_info sib2;
  sib2.q_hyst_db                 = 3;
  sib2.q_rx_lev_min              = -70;
  sib2.s_intra_search_p          = 31;
  sib2.t_reselection_nr          = 1;
  sib2.cell_reselection_priority = 6;
  sib2.thresh_serving_low_p      = 0;
  return sib2;
}

static sib19_info create_sib19_info(const du_high_unit_config& config)
{
  sib19_info sib19;
  sib19.cell_specific_koffset = config.ntn_cfg.value().cell_specific_koffset;
  sib19.ephemeris_info        = config.ntn_cfg.value().ephemeris_info;

  // These ephemeris parameters are all scaled in accordance with NIMA TR 8350.2.
  if (std::holds_alternative<ecef_coordinates_t>(sib19.ephemeris_info.value())) {
    std::get<ecef_coordinates_t>(sib19.ephemeris_info.value()).position_x /= 1.3;
    std::get<ecef_coordinates_t>(sib19.ephemeris_info.value()).position_y /= 1.3;
    std::get<ecef_coordinates_t>(sib19.ephemeris_info.value()).position_z /= 1.3;
    std::get<ecef_coordinates_t>(sib19.ephemeris_info.value()).velocity_vx /= 0.06;
    std::get<ecef_coordinates_t>(sib19.ephemeris_info.value()).velocity_vy /= 0.06;
    std::get<ecef_coordinates_t>(sib19.ephemeris_info.value()).velocity_vz /= 0.06;
  } else if (std::holds_alternative<orbital_coordinates_t>(sib19.ephemeris_info.value())) {
    std::get<orbital_coordinates_t>(sib19.ephemeris_info.value()).semi_major_axis -= 6500000;
    std::get<orbital_coordinates_t>(sib19.ephemeris_info.value()).semi_major_axis /= 0.004249;
    std::get<orbital_coordinates_t>(sib19.ephemeris_info.value()).eccentricity /= 0.00000001431;
    std::get<orbital_coordinates_t>(sib19.ephemeris_info.value()).periapsis /= 0.00000002341;
    std::get<orbital_coordinates_t>(sib19.ephemeris_info.value()).longitude /= 0.00000002341;
    std::get<orbital_coordinates_t>(sib19.ephemeris_info.value()).inclination /= 0.00000002341;
    std::get<orbital_coordinates_t>(sib19.ephemeris_info.value()).mean_anomaly /= 0.00000002341;
  }
  if (config.ntn_cfg.value().distance_threshold.has_value()) {
    sib19.distance_thres = config.ntn_cfg.value().distance_threshold.value();
  }
  if (config.ntn_cfg.value().epoch_time.has_value()) {
    sib19.epoch_time = config.ntn_cfg.value().epoch_time.value();
  }
  if (config.ntn_cfg.value().k_mac.has_value()) {
    sib19.k_mac = config.ntn_cfg.value().k_mac.value();
  }
  if (config.ntn_cfg.value().ta_info.has_value()) {
    sib19.ta_info = config.ntn_cfg.value().ta_info.value();
  }
  if (config.ntn_cfg.value().reference_location.has_value()) {
    sib19.ref_location = config.ntn_cfg.value().reference_location.value();
  }
  return sib19;
}

static unsigned get_nof_rbs(const du_high_unit_base_cell_config& cell_cfg)
{
  return band_helper::get_n_rbs_from_bw(
      cell_cfg.channel_bw_mhz, cell_cfg.common_scs, band_helper::get_freq_range(*cell_cfg.band));
}

static void fill_csi_resources(serving_cell_config& out_cell, const du_high_unit_base_cell_config& cell_cfg)
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

std::vector<du_cell_config> srsran::generate_du_cell_config(const du_high_unit_config& config)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("GNB", false);

  std::vector<du_cell_config> out_cfg;
  out_cfg.reserve(config.cells_cfg.size());

  for (const auto& cell : config.cells_cfg) {
    cell_config_builder_params           param;
    const du_high_unit_base_cell_config& base_cell = cell.cell;
    param.pci                                      = base_cell.pci;
    param.scs_common                               = base_cell.common_scs;
    param.channel_bw_mhz                           = base_cell.channel_bw_mhz;
    param.dl_arfcn                                 = base_cell.dl_arfcn;
    param.band                                     = *base_cell.band;
    // Enable CSI-RS if the PDSCH mcs is dynamic (min_ue_mcs != max_ue_mcs).
    param.csi_rs_enabled      = base_cell.csi_cfg.csi_rs_enabled;
    param.nof_dl_ports        = base_cell.nof_antennas_dl;
    param.search_space0_index = base_cell.pdcch_cfg.common.ss0_index;
    param.min_k1              = base_cell.pucch_cfg.min_k1;
    param.min_k2              = base_cell.pusch_cfg.min_k2;
    param.coreset0_index      = base_cell.pdcch_cfg.common.coreset0_index;
    // Set maximum CORESET#0 duration to 1 OFDM symbol for BW > 50Mhz to spread CORESET RBs across the BW. This results
    // in one extra symbol to be used for PDSCH.
    if (base_cell.pdcch_cfg.common.max_coreset0_duration.has_value()) {
      param.max_coreset0_duration = base_cell.pdcch_cfg.common.max_coreset0_duration.value();
    } else if (param.channel_bw_mhz > bs_channel_bandwidth_fr1::MHz50) {
      param.max_coreset0_duration = 1;
    }
    const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
        base_cell.channel_bw_mhz, param.scs_common, band_helper::get_freq_range(*param.band));

    std::optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc;
    if (base_cell.pdcch_cfg.common.coreset0_index.has_value()) {
      ssb_freq_loc =
          band_helper::get_ssb_coreset0_freq_location_for_cset0_idx(base_cell.dl_arfcn,
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
                                                                 param.search_space0_index,
                                                                 param.max_coreset0_duration);
    }

    if (!ssb_freq_loc.has_value()) {
      report_error("Unable to derive a valid SSB pointA and k_SSB for cell id ({}).\n", base_cell.pci);
    }

    param.offset_to_point_a = ssb_freq_loc->offset_to_point_A;
    param.k_ssb             = ssb_freq_loc->k_ssb;
    param.coreset0_index    = ssb_freq_loc->coreset0_idx;

    // Set TDD pattern.
    if (band_helper::get_duplex_mode(*param.band) == duplex_mode::TDD) {
      param.tdd_ul_dl_cfg_common.emplace(generate_tdd_pattern(param.scs_common, cell.cell.tdd_ul_dl_cfg.value()));
    }

    // Create the configuration.
    out_cfg.push_back(config_helpers::make_default_du_cell_config(param));
    du_cell_config& out_cell = out_cfg.back();

    // Set the rest of the parameters.
    out_cell.nr_cgi.plmn_id   = plmn_identity::parse(base_cell.plmn).value();
    out_cell.nr_cgi.nci       = nr_cell_identity::create(config.gnb_id, base_cell.sector_id.value()).value();
    out_cell.tac              = base_cell.tac;
    out_cell.searchspace0_idx = param.search_space0_index;

    // Cell selection parameters.
    out_cell.cell_sel_info.q_rx_lev_min = base_cell.q_rx_lev_min;
    out_cell.cell_sel_info.q_qual_min   = base_cell.q_qual_min;

    // SSB config.
    out_cell.ssb_cfg.ssb_period      = static_cast<ssb_periodicity>(base_cell.ssb_cfg.ssb_period_msec);
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
        out_si.si_window_position = base_cell.sib_cfg.si_sched_info[i].si_window_position;
        for (unsigned j = 0; j != base_cell.sib_cfg.si_sched_info[i].sib_mapping_info.size(); ++j) {
          sibs_included.push_back(base_cell.sib_cfg.si_sched_info[i].sib_mapping_info[j]);
          out_si.sib_mapping_info[j] = static_cast<sib_type>(sibs_included.back());
        }
      }
      for (const uint8_t sib_id : sibs_included) {
        sib_info item;
        switch (sib_id) {
          case 2: {
            item = create_sib2_info();
          } break;
          case 19: {
            if (config.ntn_cfg.has_value()) {
              item = create_sib19_info(config);
            } else {
              report_error("SIB19 is not configured, NTN fields required\n");
            }
          } break;
          default:
            report_error("SIB{} not supported\n", sib_id);
        }
        out_cell.si_config->sibs.push_back(item);
      }
      // Enable otherSI search space.
      out_cell.dl_cfg_common.init_dl_bwp.pdcch_common.other_si_search_space_id = to_search_space_id(1);
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
    out_cell.dl_cfg_common.freq_info_dl.scs_carrier_list.back().tx_direct_current_location = dc_offset_helper::pack(
        base_cell.pdsch_cfg.dc_offset, out_cell.dl_cfg_common.freq_info_dl.scs_carrier_list.back().carrier_bandwidth);

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
    rach_cfg.total_nof_ra_preambles   = base_cell.prach_cfg.total_nof_ra_preambles;
    rach_cfg.nof_ssb_per_ro           = base_cell.prach_cfg.nof_ssb_per_ro;
    rach_cfg.nof_cb_preambles_per_ssb = base_cell.prach_cfg.nof_cb_preambles_per_ssb;

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

    if (config.ntn_cfg.has_value()) {
      out_cell.ntn_cs_koffset = config.ntn_cfg.value().cell_specific_koffset;
    }
    // Parameters for PUCCH-ConfigCommon.
    if (not out_cell.ul_cfg_common.init_ul_bwp.pucch_cfg_common.has_value()) {
      out_cell.ul_cfg_common.init_ul_bwp.pucch_cfg_common.emplace();
    }
    out_cell.ul_cfg_common.init_ul_bwp.pucch_cfg_common.value().p0_nominal = base_cell.pucch_cfg.p0_nominal;
    out_cell.ul_cfg_common.init_ul_bwp.pucch_cfg_common.value().pucch_resource_common =
        base_cell.pucch_cfg.pucch_resource_common;

    // Common PDCCH config.
    search_space_configuration& ss1_cfg = out_cell.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces.back();
    ss1_cfg.set_non_ss0_nof_candidates(base_cell.pdcch_cfg.common.ss1_n_candidates);

    // UE-dedicated PDCCH config.
    freq_resource_bitmap freq_resources(pdcch_constants::MAX_NOF_FREQ_RESOURCES);
    unsigned             cset1_start_crb = 0;
    if (base_cell.pdcch_cfg.dedicated.coreset1_rb_start.has_value()) {
      cset1_start_crb = base_cell.pdcch_cfg.dedicated.coreset1_rb_start.value();
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
      cset1_cfg.duration = out_cell.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->duration;
    }
    const std::array<uint8_t, 5> auto_compute_ss2_n_candidates_cfg = {0, 0, 0, 0, 0};
    if (base_cell.pdcch_cfg.dedicated.ss2_n_candidates != auto_compute_ss2_n_candidates_cfg) {
      ss2_cfg.set_non_ss0_nof_candidates(base_cell.pdcch_cfg.dedicated.ss2_n_candidates);
    } else if (base_cell.pdcch_cfg.dedicated.ss2_type != search_space_configuration::type_t::common) {
      ss2_cfg.set_non_ss0_nof_candidates({0,
                                          config_helpers::compute_max_nof_candidates(aggregation_level::n2, cset1_cfg),
                                          config_helpers::compute_max_nof_candidates(aggregation_level::n4, cset1_cfg),
                                          0,
                                          0});
    }

    if (base_cell.pdcch_cfg.dedicated.ss2_type == search_space_configuration::type_t::common) {
      ss2_cfg.set_non_ss0_monitored_dci_formats(search_space_configuration::common_dci_format{.f0_0_and_f1_0 = true});
      // TODO: Handle this implementation defined restriction of max. 4 PDCCH candidates in validator.
      if (base_cell.pdcch_cfg.dedicated.ss2_n_candidates == auto_compute_ss2_n_candidates_cfg) {
        ss2_cfg.set_non_ss0_nof_candidates(
            {0,
             std::min(static_cast<uint8_t>(4U),
                      config_helpers::compute_max_nof_candidates(aggregation_level::n2, cset1_cfg)),
             std::min(static_cast<uint8_t>(4U),
                      config_helpers::compute_max_nof_candidates(aggregation_level::n4, cset1_cfg)),
             0,
             0});
      }
    } else if (not base_cell.pdcch_cfg.dedicated.dci_format_0_1_and_1_1) {
      search_space_configuration& ss_cfg = out_cell.ue_ded_serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces[0];
      ss_cfg.set_non_ss0_monitored_dci_formats(search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0);
    }

    // PDSCH-Config - Update PDSCH time domain resource allocations based on partial slot and/or dedicated PDCCH
    // configuration.
    out_cell.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list =
        config_helpers::make_pdsch_time_domain_resource(param.search_space0_index,
                                                        out_cell.dl_cfg_common.init_dl_bwp.pdcch_common,
                                                        out_cell.ue_ded_serv_cell_cfg.init_dl_bwp.pdcch_cfg,
                                                        band_helper::get_duplex_mode(param.band.value()) ==
                                                                duplex_mode::TDD
                                                            ? out_cell.tdd_ul_dl_cfg_common.value()
                                                            : std::optional<tdd_ul_dl_config_common>{});

    out_cell.ue_ded_serv_cell_cfg.pdsch_serv_cell_cfg->nof_harq_proc =
        static_cast<pdsch_serving_cell_config::nof_harq_proc_for_pdsch>(
            config.cells_cfg.front().cell.pdsch_cfg.nof_harqs);
    // Set DL MCS table.
    out_cell.ue_ded_serv_cell_cfg.init_dl_bwp.pdsch_cfg->mcs_table = base_cell.pdsch_cfg.mcs_table;
    // Set DMRS additional position.
    out_cell.ue_ded_serv_cell_cfg.init_dl_bwp.pdsch_cfg->pdsch_mapping_type_a_dmrs->additional_positions =
        uint_to_dmrs_additional_positions(base_cell.pdsch_cfg.dmrs_add_pos);

    // Parameters for csiMeasConfig.
    if (param.csi_rs_enabled) {
      fill_csi_resources(out_cell.ue_ded_serv_cell_cfg, base_cell);
    }

    // Parameters for PUCCH-Config builder (these parameters will be used later on to generate the PUCCH resources).
    pucch_builder_params&            du_pucch_cfg   = out_cell.pucch_cfg;
    const du_high_unit_pucch_config& user_pucch_cfg = base_cell.pucch_cfg;
    du_pucch_cfg.nof_ue_pucch_f0_or_f1_res_harq     = user_pucch_cfg.nof_ue_pucch_f0_or_f1_res_harq;
    du_pucch_cfg.nof_ue_pucch_f2_res_harq           = user_pucch_cfg.nof_ue_pucch_f2_res_harq;
    du_pucch_cfg.nof_cell_harq_pucch_res_sets       = user_pucch_cfg.nof_cell_harq_pucch_sets;
    du_pucch_cfg.nof_sr_resources                   = user_pucch_cfg.nof_cell_sr_resources;
    du_pucch_cfg.nof_csi_resources                  = user_pucch_cfg.nof_cell_csi_resources;
    if (user_pucch_cfg.use_format_0) {
      auto& f0_params                  = du_pucch_cfg.f0_or_f1_params.emplace<pucch_f0_params>();
      f0_params.intraslot_freq_hopping = user_pucch_cfg.f0_intraslot_freq_hopping;
    } else {
      auto& f1_params                  = du_pucch_cfg.f0_or_f1_params.emplace<pucch_f1_params>();
      f1_params.occ_supported          = user_pucch_cfg.f1_enable_occ;
      f1_params.nof_cyc_shifts         = static_cast<nof_cyclic_shifts>(user_pucch_cfg.nof_cyclic_shift);
      f1_params.intraslot_freq_hopping = user_pucch_cfg.f1_intraslot_freq_hopping;
    }
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
    // Set DMRS additional position.
    out_cell.ue_ded_serv_cell_cfg.ul_config.value()
        .init_ul_bwp.pusch_cfg->pusch_mapping_type_a_dmrs->additional_positions =
        uint_to_dmrs_additional_positions(base_cell.pusch_cfg.dmrs_add_pos);
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
    if (not std::holds_alternative<uci_on_pusch::beta_offsets_semi_static>(
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
    auto& b_offsets = std::get<uci_on_pusch::beta_offsets_semi_static>(out_cell.ue_ded_serv_cell_cfg.ul_config.value()
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
    sr_cng.front().period = static_cast<sr_periodicity>(
        static_cast<unsigned>(get_nof_slots_per_subframe(base_cell.common_scs) * base_cell.pucch_cfg.sr_period_msec));

    // If any dependent parameter needs to be updated, this is the place.
    config_helpers::compute_nof_sr_csi_pucch_res(du_pucch_cfg,
                                                 base_cell.ul_common_cfg.max_pucchs_per_slot,
                                                 base_cell.pucch_cfg.sr_period_msec,
                                                 base_cell.csi_cfg.csi_rs_enabled
                                                     ? std::optional<unsigned>{base_cell.csi_cfg.csi_rs_period_msec}
                                                     : std::nullopt);
    if (update_msg1_frequency_start) {
      rach_cfg.rach_cfg_generic.msg1_frequency_start = config_helpers::compute_prach_frequency_start(
          du_pucch_cfg, out_cell.ul_cfg_common.init_ul_bwp.generic_params.crbs.length(), is_long_prach);
    }

    // Slicing configuration.
    std::vector<std::string> cell_plmns{base_cell.plmn};
    out_cell.rrm_policy_members = generate_du_slicing_rrm_policy_config(cell_plmns, base_cell.slice_cfg, nof_crbs);

    logger.info(
        "SSB derived parameters for cell: {}, band: {}, dl_arfcn:{}, crbs: {} scs:{}, ssb_scs:{}:\n\t - SSB offset "
        "pointA:{} \n\t - k_SSB:{} \n\t - SSB arfcn:{} \n\t - Coreset index:{} \n\t - Searchspace index:{}",
        base_cell.pci,
        *param.band,
        base_cell.dl_arfcn,
        nof_crbs,
        to_string(base_cell.common_scs),
        to_string(out_cfg.back().ssb_cfg.scs),
        ssb_freq_loc->offset_to_point_A.to_uint(),
        ssb_freq_loc->k_ssb.to_uint(),
        ssb_freq_loc->ssb_arfcn,
        ssb_freq_loc->coreset0_idx,
        ssb_freq_loc->searchspace0_idx);

    error_type<std::string> error = is_du_cell_config_valid(out_cfg.back());
    if (!error) {
      report_error("Invalid configuration DU cell detected.\n> {}\n", error.error());
    }
  }

  return out_cfg;
}

static rlc_am_config generate_du_rlc_am_config(const du_high_unit_rlc_am_config& in_cfg)
{
  rlc_am_config out_rlc;
  // AM Config
  //<  TX SN
  if (!from_number(out_rlc.tx.sn_field_length, in_cfg.tx.sn_field_length)) {
    report_error("Invalid RLC AM TX SN: SN={}\n", in_cfg.tx.sn_field_length);
  }
  out_rlc.tx.t_poll_retx     = in_cfg.tx.t_poll_retx;
  out_rlc.tx.max_retx_thresh = in_cfg.tx.max_retx_thresh;
  out_rlc.tx.poll_pdu        = in_cfg.tx.poll_pdu;
  out_rlc.tx.poll_byte       = in_cfg.tx.poll_byte;
  out_rlc.tx.max_window      = in_cfg.tx.max_window;
  out_rlc.tx.queue_size      = in_cfg.tx.queue_size;
  //< RX SN
  if (!from_number(out_rlc.rx.sn_field_length, in_cfg.rx.sn_field_length)) {
    report_error("Invalid RLC AM RX SN: SN={}\n", in_cfg.rx.sn_field_length);
  }
  out_rlc.rx.t_reassembly      = in_cfg.rx.t_reassembly;
  out_rlc.rx.t_status_prohibit = in_cfg.rx.t_status_prohibit;
  if (in_cfg.rx.max_sn_per_status != 0) {
    out_rlc.rx.max_sn_per_status = in_cfg.rx.max_sn_per_status;
  }
  return out_rlc;
}

static mac_lc_config generate_mac_lc_config(const du_high_unit_mac_lc_config& in_cfg)
{
  mac_lc_config out_mac;

  out_mac.priority            = in_cfg.priority;
  out_mac.lcg_id              = uint_to_lcg_id(in_cfg.lc_group_id);
  out_mac.pbr                 = to_prioritized_bit_rate(in_cfg.prioritized_bit_rate_kBps);
  out_mac.bsd                 = to_bucket_size_duration(in_cfg.bucket_size_duration_ms);
  out_mac.lc_sr_mask          = false;
  out_mac.lc_sr_delay_applied = false;
  out_mac.sr_id               = uint_to_sched_req_id(0);
  return out_mac;
}

std::vector<slice_rrm_policy_config>
srsran::generate_du_slicing_rrm_policy_config(span<const std::string>                    plmns,
                                              span<const du_high_unit_cell_slice_config> slice_cfg,
                                              unsigned                                   nof_cell_crbs)
{
  std::vector<slice_rrm_policy_config> rrm_policy_cfgs;
  for (const auto& plmn : plmns) {
    for (const auto& cfg : slice_cfg) {
      rrm_policy_cfgs.emplace_back();
      rrm_policy_cfgs.back().rrc_member.s_nssai = cfg.s_nssai;
      rrm_policy_cfgs.back().rrc_member.plmn_id = plmn;
      rrm_policy_cfgs.back().min_prb            = (nof_cell_crbs * cfg.sched_cfg.min_prb_policy_ratio) / 100;
      rrm_policy_cfgs.back().max_prb            = (nof_cell_crbs * cfg.sched_cfg.max_prb_policy_ratio) / 100;
    }
  }
  return rrm_policy_cfgs;
}

std::map<five_qi_t, du_qos_config> srsran::generate_du_qos_config(const du_high_unit_config& config)
{
  std::map<five_qi_t, du_qos_config> out_cfg = {};
  if (config.qos_cfg.empty()) {
    out_cfg = config_helpers::make_default_du_qos_config_list(config.warn_on_drop, config.metrics.rlc.report_period);
    return out_cfg;
  }

  for (const auto& qos : config.qos_cfg) {
    if (out_cfg.find(qos.five_qi) != out_cfg.end()) {
      report_error("Duplicate 5QI configuration: {}\n", qos.five_qi);
    }
    // Convert RLC config
    auto& out_rlc = out_cfg[qos.five_qi].rlc;
    if (!from_string(out_rlc.mode, qos.rlc.mode)) {
      report_error("Invalid RLC mode: {}, mode={}\n", qos.five_qi, qos.rlc.mode);
    }

    if (out_rlc.mode == rlc_mode::um_bidir) {
      // UM Config
      //< RX SN
      if (!from_number(out_rlc.um.rx.sn_field_length, qos.rlc.um.rx.sn_field_length)) {
        report_error("Invalid RLC UM RX SN: {}, SN={}\n", qos.five_qi, qos.rlc.um.rx.sn_field_length);
      }
      //< RX t-reassembly
      out_rlc.um.rx.t_reassembly = qos.rlc.um.rx.t_reassembly;
      //< TX SN
      if (!from_number(out_rlc.um.tx.sn_field_length, qos.rlc.um.tx.sn_field_length)) {
        report_error("Invalid RLC UM TX SN: {}, SN={}\n", qos.five_qi, qos.rlc.um.tx.sn_field_length);
      }
      out_rlc.um.tx.queue_size = qos.rlc.um.tx.queue_size;
    } else if (out_rlc.mode == rlc_mode::am) {
      // AM Config
      out_rlc.am = generate_du_rlc_am_config(qos.rlc.am);
    }
    out_rlc.metrics_period = std::chrono::milliseconds(config.metrics.rlc.report_period);

    // Convert F1-U config
    auto& out_f1u = out_cfg[qos.five_qi].f1u;
    //< t-Notify
    out_f1u.t_notify     = qos.f1u_du.t_notify;
    out_f1u.warn_on_drop = config.warn_on_drop;

    // Convert MAC config
    out_cfg[qos.five_qi].mac = generate_mac_lc_config(qos.mac);
  }
  return out_cfg;
}

std::map<srb_id_t, du_srb_config> srsran::generate_du_srb_config(const du_high_unit_config& config)
{
  std::map<srb_id_t, du_srb_config> srb_cfg;

  // SRB1
  srb_cfg.insert(std::make_pair(srb_id_t::srb1, du_srb_config{}));
  if (config.srb_cfg.find(srb_id_t::srb1) != config.srb_cfg.end()) {
    auto& out_rlc = srb_cfg[srb_id_t::srb1].rlc;
    out_rlc.mode  = rlc_mode::am;
    out_rlc.am    = generate_du_rlc_am_config(config.srb_cfg.at(srb_id_t::srb1).rlc);
  } else {
    srb_cfg.at(srb_id_t::srb1).rlc = make_default_srb_rlc_config();
  }
  srb_cfg.at(srb_id_t::srb1).mac = make_default_srb_mac_lc_config(LCID_SRB1);

  // SRB2
  srb_cfg.insert(std::make_pair(srb_id_t::srb2, du_srb_config{}));
  if (config.srb_cfg.find(srb_id_t::srb2) != config.srb_cfg.end()) {
    auto& out_rlc = srb_cfg[srb_id_t::srb2].rlc;
    out_rlc.mode  = rlc_mode::am;
    out_rlc.am    = generate_du_rlc_am_config(config.srb_cfg.at(srb_id_t::srb2).rlc);
  } else {
    srb_cfg.at(srb_id_t::srb2).rlc = make_default_srb_rlc_config();
  }
  srb_cfg.at(srb_id_t::srb2).mac = make_default_srb_mac_lc_config(LCID_SRB2);

  // SRB3
  srb_cfg.insert(std::make_pair(srb_id_t::srb3, du_srb_config{}));
  if (config.srb_cfg.find(srb_id_t::srb3) != config.srb_cfg.end()) {
    auto& out_rlc = srb_cfg[srb_id_t::srb3].rlc;
    out_rlc.mode  = rlc_mode::am;
    out_rlc.am    = generate_du_rlc_am_config(config.srb_cfg.at(srb_id_t::srb3).rlc);
  } else {
    srb_cfg.at(srb_id_t::srb3).rlc = make_default_srb_rlc_config();
  }
  srb_cfg.at(srb_id_t::srb3).mac = make_default_srb_mac_lc_config(LCID_SRB3);

  if (config.ntn_cfg.has_value()) {
    ntn_augment_rlc_parameters(config.ntn_cfg.value(), srb_cfg);
  }
  return srb_cfg;
}

mac_expert_config srsran::generate_mac_expert_config(const du_high_unit_config& config)
{
  mac_expert_config                    out_cfg = {};
  const du_high_unit_base_cell_config& cell    = config.cells_cfg.front().cell;
  out_cfg.configs.push_back({.max_consecutive_dl_kos  = cell.pdsch_cfg.max_consecutive_kos,
                             .max_consecutive_ul_kos  = cell.pusch_cfg.max_consecutive_kos,
                             .max_consecutive_csi_dtx = cell.pucch_cfg.max_consecutive_kos});

  return out_cfg;
}

scheduler_expert_config srsran::generate_scheduler_expert_config(const du_high_unit_config& config)
{
  scheduler_expert_config out_cfg = config_helpers::make_default_scheduler_expert_config();

  const du_high_unit_base_cell_config& cell = config.cells_cfg.front().cell;

  // UE parameters.
  const du_high_unit_pdsch_config&            pdsch                = cell.pdsch_cfg;
  const du_high_unit_pusch_config&            pusch                = cell.pusch_cfg;
  const du_high_unit_scheduler_expert_config& app_sched_expert_cfg = cell.sched_expert_cfg;
  out_cfg.ue.dl_mcs                                                = {pdsch.min_ue_mcs, pdsch.max_ue_mcs};
  out_cfg.ue.pdsch_rv_sequence.assign(pdsch.rv_sequence.begin(), pdsch.rv_sequence.end());
  out_cfg.ue.dl_harq_la_cqi_drop_threshold     = pdsch.harq_la_cqi_drop_threshold;
  out_cfg.ue.dl_harq_la_ri_drop_threshold      = pdsch.harq_la_ri_drop_threshold;
  out_cfg.ue.max_nof_harq_retxs                = pdsch.max_nof_harq_retxs;
  out_cfg.ue.max_pdschs_per_slot               = pdsch.max_pdschs_per_slot;
  out_cfg.ue.max_pdcch_alloc_attempts_per_slot = pdsch.max_pdcch_alloc_attempts_per_slot;
  out_cfg.ue.pdsch_nof_rbs                     = {pdsch.min_rb_size, pdsch.max_rb_size};
  out_cfg.ue.pusch_nof_rbs                     = {pusch.min_rb_size, pusch.max_rb_size};
  out_cfg.ue.olla_dl_target_bler               = pdsch.olla_target_bler;
  out_cfg.ue.olla_cqi_inc                      = pdsch.olla_cqi_inc;
  out_cfg.ue.olla_max_cqi_offset               = pdsch.olla_max_cqi_offset;
  if (config.ntn_cfg.has_value()) {
    out_cfg.ue.auto_ack_harq = true;
  }
  out_cfg.ue.ul_mcs = {pusch.min_ue_mcs, pusch.max_ue_mcs};
  out_cfg.ue.pusch_rv_sequence.assign(pusch.rv_sequence.begin(), pusch.rv_sequence.end());
  out_cfg.ue.initial_ul_dc_offset   = pusch.dc_offset;
  out_cfg.ue.max_puschs_per_slot    = pusch.max_puschs_per_slot;
  out_cfg.ue.olla_ul_target_bler    = pusch.olla_target_bler;
  out_cfg.ue.olla_ul_snr_inc        = pusch.olla_snr_inc;
  out_cfg.ue.olla_max_ul_snr_offset = pusch.olla_max_snr_offset;
  out_cfg.ue.pdsch_crb_limits       = {pdsch.start_rb, pdsch.end_rb};
  out_cfg.ue.pusch_crb_limits       = {pusch.start_rb, pusch.end_rb};
  if (std::holds_alternative<time_pf_scheduler_expert_config>(app_sched_expert_cfg.policy_sched_expert_cfg)) {
    out_cfg.ue.strategy_cfg = app_sched_expert_cfg.policy_sched_expert_cfg;
  }

  // PUCCH and scheduler expert parameters.
  out_cfg.ue.max_ul_grants_per_slot = cell.ul_common_cfg.max_ul_grants_per_slot;
  out_cfg.ue.max_pucchs_per_slot    = cell.ul_common_cfg.max_pucchs_per_slot;
  out_cfg.ue.min_k1                 = cell.pucch_cfg.min_k1;

  // RA parameters.
  const du_high_unit_prach_config& prach = cell.prach_cfg;

  out_cfg.ra.rar_mcs_index           = pdsch.fixed_rar_mcs;
  out_cfg.ra.max_nof_msg3_harq_retxs = prach.max_msg3_harq_retx;
  out_cfg.ra.msg3_mcs_index          = prach.fixed_msg3_mcs;

  // SI parameters.
  out_cfg.si.sib1_mcs_index    = pdsch.fixed_sib1_mcs;
  out_cfg.si.sib1_dci_aggr_lev = aggregation_level::n4;

  // Logging and tracing.
  out_cfg.log_broadcast_messages = config.loggers.broadcast_enabled;
  out_cfg.metrics_report_period  = std::chrono::milliseconds{config.metrics.stdout_metrics_period};

  const error_type<std::string> error = is_scheduler_expert_config_valid(out_cfg);
  if (!error) {
    report_error("Invalid scheduler expert configuration detected.\n");
  }

  return out_cfg;
}

e2ap_configuration srsran::generate_e2_config(const du_high_unit_config& du_high)
{
  e2ap_configuration out_cfg = srsran::config_helpers::make_default_e2ap_config();
  out_cfg.gnb_id             = du_high.gnb_id;
  out_cfg.plmn               = du_high.cells_cfg.front().cell.plmn;
  out_cfg.gnb_du_id          = du_high.gnb_du_id;
  out_cfg.e2sm_kpm_enabled   = du_high.e2_cfg.e2sm_kpm_enabled;
  out_cfg.e2sm_rc_enabled    = du_high.e2_cfg.e2sm_rc_enabled;

  return out_cfg;
}

void srsran::ntn_augment_rlc_parameters(const ntn_config& ntn_cfg, std::map<srb_id_t, du_srb_config>& srb_cfgs)
{
  // NTN is enabled, so we need to augment the RLC parameters for the NTN cell.
  for (auto& srb : srb_cfgs) {
    if (ntn_cfg.cell_specific_koffset > 1000) {
      srb.second.rlc.am.tx.t_poll_retx = 4000;
    } else if (ntn_cfg.cell_specific_koffset > 800) {
      srb.second.rlc.am.tx.t_poll_retx = 2000;
    } else if (ntn_cfg.cell_specific_koffset > 500) {
      srb.second.rlc.am.tx.t_poll_retx = 2000;
    } else if (ntn_cfg.cell_specific_koffset > 300) {
      srb.second.rlc.am.tx.t_poll_retx = 1000;
    } else if (ntn_cfg.cell_specific_koffset > 200) {
      srb.second.rlc.am.tx.t_poll_retx = 800;
    } else if (ntn_cfg.cell_specific_koffset > 100) {
      srb.second.rlc.am.tx.t_poll_retx = 400;
    } else if (ntn_cfg.cell_specific_koffset > 50) {
      srb.second.rlc.am.tx.t_poll_retx = 200;
    } else if (ntn_cfg.cell_specific_koffset > 10) {
      srb.second.rlc.am.tx.t_poll_retx = 100;
    } else {
      srb.second.rlc.am.tx.t_poll_retx = 50;
    }
  }
}
