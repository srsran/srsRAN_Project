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

#include "du_high_config_yaml_writer.h"
#include "apps/helpers/metrics/metrics_config_yaml_writer.h"
#include "du_high_config.h"
#include "srsran/adt/span.h"

using namespace srsran;

static void fill_du_high_metrics_layers_section(YAML::Node node, const du_high_unit_metrics_layer_config& config)
{
  node["enable_sched"] = config.enable_scheduler;
  node["enable_rlc"]   = config.enable_rlc;
  node["enable_mac"]   = config.enable_mac;
}

static void fill_du_high_metrics_section(YAML::Node node, const du_high_unit_metrics_config& config)
{
  auto perdiodicity_node                = node["periodicity"];
  perdiodicity_node["du_report_period"] = config.du_report_period;

  fill_du_high_metrics_layers_section(node["layers"], config.layers_cfg);
}

static void fill_du_high_pcap_section(YAML::Node node, const du_high_unit_pcap_config& config)
{
  node["f1ap_filename"] = config.f1ap.filename;
  node["f1ap_enable"]   = config.f1ap.enabled;
  node["f1u_filename"]  = config.f1u.filename;
  node["f1u_enable"]    = config.f1u.enabled;
  node["rlc_filename"]  = config.rlc.filename;
  node["rlc_rb_type"]   = config.rlc.rb_type;
  node["rlc_enable"]    = config.rlc.enabled;
  node["mac_filename"]  = config.mac.filename;
  node["mac_type"]      = config.mac.type;
  node["mac_enable"]    = config.mac.enabled;
}

static void fill_du_high_log_section(YAML::Node node, const du_high_unit_logger_config& config)
{
  node["mac_level"]                        = srslog::basic_level_to_string(config.mac_level);
  node["rlc_level"]                        = srslog::basic_level_to_string(config.rlc_level);
  node["f1ap_level"]                       = srslog::basic_level_to_string(config.f1ap_level);
  node["f1u_level"]                        = srslog::basic_level_to_string(config.f1u_level);
  node["gtpu_level"]                       = srslog::basic_level_to_string(config.gtpu_level);
  node["du_level"]                         = srslog::basic_level_to_string(config.du_level);
  node["hex_max_size"]                     = config.hex_max_size;
  node["broadcast_enabled"]                = config.broadcast_enabled;
  node["high_latency_diagnostics_enabled"] = config.high_latency_diagnostics_enabled;
  node["f1ap_json_enabled"]                = config.f1ap_json_enabled;
}

static YAML::Node build_du_section(const du_high_unit_config& config)
{
  YAML::Node node;

  node["warn_on_drop"] = config.warn_on_drop;

  return node;
}

static YAML::Node build_du_high_ntn_section(const ntn_config& config)
{
  YAML::Node node;

  node["cell_specific_koffset"] = config.cell_specific_koffset;

  if (config.ntn_ul_sync_validity_dur) {
    node["ntn_ul_sync_validity_dur"] = config.ntn_ul_sync_validity_dur.value();
  }

  if (config.ta_info) {
    YAML::Node ta_info_node;
    ta_info_node["ta_common"]               = config.ta_info.value().ta_common;
    ta_info_node["ta_common_drift"]         = config.ta_info.value().ta_common_drift;
    ta_info_node["ta_common_drift_variant"] = config.ta_info.value().ta_common_drift_variant;
    ta_info_node["ta_common_offset"]        = config.ta_info.value().ta_common_offset;

    node["ta_info"] = ta_info_node;
  }

  if (config.epoch_timestamp) {
    node["epoch_timestamp"] = config.epoch_timestamp.value();
  }

  if (config.feeder_link_info) {
    YAML::Node fl_node;
    fl_node["enable_doppler_compensation"] = config.feeder_link_info.value().enable_doppler_compensation;
    fl_node["dl_freq"]                     = config.feeder_link_info.value().dl_freq;
    fl_node["ul_freq"]                     = config.feeder_link_info.value().ul_freq;

    node["feeder_link_info"] = fl_node;
  }

  if (config.ntn_gateway_location) {
    YAML::Node gw_loc_node;
    gw_loc_node["latitude"]  = config.ntn_gateway_location.value().latitude;
    gw_loc_node["longitude"] = config.ntn_gateway_location.value().longitude;
    gw_loc_node["altitude"]  = config.ntn_gateway_location.value().altitude;

    node["ntn_gateway_location"] = gw_loc_node;
  }

  if (config.epoch_time.has_value()) {
    YAML::Node epoch_node;
    epoch_node["sfn"]             = config.epoch_time.value().sfn;
    epoch_node["subframe_number"] = config.epoch_time.value().subframe_number;

    node["epoch_time"] = epoch_node;
  }

  if (config.epoch_sfn_offset) {
    node["epoch_sfn_offset"] = config.epoch_sfn_offset.value();
  }

  if (config.use_state_vector) {
    node["use_state_vector"] = config.use_state_vector.value();
  }

  if (std::holds_alternative<ecef_coordinates_t>(config.ephemeris_info)) {
    YAML::Node  ephemeris_node;
    const auto& ephem       = std::get<ecef_coordinates_t>(config.ephemeris_info);
    ephemeris_node["pos_x"] = ephem.position_x;
    ephemeris_node["pos_y"] = ephem.position_y;
    ephemeris_node["pos_z"] = ephem.position_z;
    ephemeris_node["vel_x"] = ephem.velocity_vx;
    ephemeris_node["vel_y"] = ephem.velocity_vy;
    ephemeris_node["vel_z"] = ephem.velocity_vz;

    node["ephemeris_info_ecef"] = ephemeris_node;
  }

  if (std::holds_alternative<orbital_coordinates_t>(config.ephemeris_info)) {
    YAML::Node  orb_node;
    const auto& orb             = std::get<orbital_coordinates_t>(config.ephemeris_info);
    orb_node["semi_major_axis"] = orb.semi_major_axis;
    orb_node["eccentricity"]    = orb.eccentricity;
    orb_node["periapsis"]       = orb.periapsis;
    orb_node["longitude"]       = orb.longitude;
    orb_node["inclination"]     = orb.inclination;
    orb_node["mean_anomaly"]    = orb.mean_anomaly;

    node["ephemeris_orbital"] = orb_node;
  }
  return node;
}

static YAML::Node build_du_high_mac_cell_group_section(const du_high_unit_mac_cell_group_config& config)
{
  YAML::Node node;

  {
    YAML::Node bsr_node;
    bsr_node["periodic_bsr_timer"] = config.bsr_cfg.periodic_bsr_timer;
    bsr_node["retx_bsr_timer"]     = config.bsr_cfg.retx_bsr_timer;
    if (config.bsr_cfg.lc_sr_delay_timer.has_value()) {
      bsr_node["lc_sr_delay_timer"] = config.bsr_cfg.lc_sr_delay_timer.value();
    }

    node["bsr_cfg"] = bsr_node;
  }
  {
    YAML::Node phr_node;
    phr_node["phr_prohibit_timer"] = config.phr_cfg.phr_prohib_timer;

    node["phr_cfg"] = phr_node;
  }
  {
    YAML::Node sr_node;
    sr_node["sr_trans_max"] = config.sr_cfg.sr_trans_max;

    if (config.sr_cfg.sr_prohibit_timer.has_value()) {
      sr_node["sr_prohibit_timer"] = config.sr_cfg.sr_prohibit_timer.value();
    }

    node["sr_cfg"] = sr_node;
  }

  return node;
}

static YAML::Node build_du_high_ssb_section(const du_high_unit_ssb_config& config)
{
  YAML::Node node;

  node["ssb_period"]          = config.ssb_period_msec;
  node["ssb_block_power_dbm"] = config.ssb_block_power;
  node["pss_to_sss_epre_db"]  = config.pss_to_sss_epre == ssb_pss_to_sss_epre::dB_0 ? "0" : "3";

  return node;
}

static YAML::Node build_du_high_sib_section(const du_high_unit_sib_config& config)
{
  YAML::Node node;

  node["si_window_length"] = config.si_window_len_slots;
  node["t300"]             = config.ue_timers_and_constants.t300;
  node["t301"]             = config.ue_timers_and_constants.t301;
  node["t310"]             = config.ue_timers_and_constants.t310;
  node["t311"]             = config.ue_timers_and_constants.t311;
  node["t319"]             = config.ue_timers_and_constants.t319;
  node["n310"]             = config.ue_timers_and_constants.n310;
  node["n311"]             = config.ue_timers_and_constants.n311;

  for (const auto& cell : config.si_sched_info) {
    YAML::Node si_node;
    for (auto& period : cell.sib_mapping_info) {
      si_node["sib_mapping"] = period;
    }
    si_node["si_period"].SetStyle(YAML::EmitterStyle::Flow);

    si_node["sib_mapping"] = cell.sib_mapping_info;

    if (cell.si_window_position.has_value()) {
      si_node["si_window_position"] = cell.si_window_position.value();
    }

    node["si_sched_info"].push_back(si_node);
  }

  return node;
}

static YAML::Node build_du_high_ul_common_section(const du_high_unit_ul_common_config& config)
{
  YAML::Node node;

  if (config.p_max.has_value()) {
    node["p_max"] = config.p_max.value();
  }

  node["max_pucchs_per_slot"]    = config.max_pucchs_per_slot;
  node["max_ul_grants_per_slot"] = config.max_ul_grants_per_slot;

  return node;
}

static YAML::Node build_du_high_pdcch_section(const du_high_unit_pdcch_config& config)
{
  YAML::Node node;

  {
    YAML::Node common_node;
    common_node["ss0_index"] = config.common.ss0_index;
    for (auto ss1 : config.common.ss1_n_candidates) {
      common_node["ss1_n_candidates"].push_back(static_cast<unsigned>(ss1));
    }
    common_node["ss1_n_candidates"].SetStyle(YAML::EmitterStyle::Flow);

    if (config.common.max_coreset0_duration.has_value()) {
      common_node["max_coreset0_duration"] = static_cast<unsigned>(config.common.max_coreset0_duration.value());
    }
    if (config.common.coreset0_index.has_value()) {
      common_node["coreset0_index"] = config.common.coreset0_index.value();
    }

    node["common"] = common_node;
  }
  {
    YAML::Node dedicated_node;

    dedicated_node["dci_format_0_1_and_1_1"] = config.dedicated.dci_format_0_1_and_1_1;
    dedicated_node["ss2_type"] = config.dedicated.ss2_type == search_space_type::common ? "common" : "ue_dedicated";
    if (config.dedicated.coreset1_duration.has_value()) {
      dedicated_node["coreset1_duration"] = config.dedicated.coreset1_duration.value();
    }
    if (config.dedicated.coreset1_rb_start.has_value()) {
      dedicated_node["coreset1_rb_start"] = config.dedicated.coreset1_rb_start.value();
    }
    if (config.dedicated.coreset1_l_crb.has_value()) {
      dedicated_node["coreset1_l_crb"] = config.dedicated.coreset1_l_crb.value();
    }
    for (auto ss2 : config.dedicated.ss2_n_candidates) {
      dedicated_node["ss2_n_candidates"].push_back(static_cast<unsigned>(ss2));
    }
    dedicated_node["ss2_n_candidates"].SetStyle(YAML::EmitterStyle::Flow);
    dedicated_node["al_cqi_offset"] = config.dedicated.al_cqi_offset;

    node["dedicated"] = dedicated_node;
  }

  return node;
}

static std::string to_string(pdsch_mcs_table table)
{
  switch (table) {
    case pdsch_mcs_table::qam64:
      return "qam64";
    case pdsch_mcs_table::qam256:
      return "qam256";
    default:
      srsran_assert(0, "Invalid PDSCH MCS table");
      break;
  }
  return {};
}

static std::string to_string(dc_offset_t dc_offset)
{
  switch (dc_offset) {
    case dc_offset_t::center:
      return "center";
    case dc_offset_t::max:
    case dc_offset_t::min:
      return std::to_string(static_cast<int>(dc_offset));
    case dc_offset_t::outside:
      return "outside";
    case dc_offset_t::undetermined:
      return "undetermined";
    default:
      srsran_assert(0, "Invalid PDSCH MCS table");
      break;
  }
  return {};
}

static YAML::Node build_du_high_pdsch_section(const du_high_unit_pdsch_config& config)
{
  YAML::Node node;

  node["min_ue_mcs"]                       = config.min_ue_mcs;
  node["max_ue_mcs"]                       = config.max_ue_mcs;
  node["fixed_rar_mcs"]                    = config.fixed_rar_mcs;
  node["fixed_sib1_mcs"]                   = config.fixed_sib1_mcs;
  node["nof_harqs"]                        = config.nof_harqs;
  node["max_nof_harq_retxs"]               = config.max_nof_harq_retxs;
  node["max_consecutive_kos"]              = config.max_consecutive_kos;
  node["mcs_table"]                        = to_string(config.mcs_table);
  node["min_rb_size"]                      = config.min_rb_size;
  node["max_rb_size"]                      = config.max_rb_size;
  node["start_rb"]                         = config.start_rb;
  node["end_rb"]                           = config.end_rb;
  node["max_pdschs_per_slot"]              = config.max_pdschs_per_slot;
  node["olla_cqi_inc_step"]                = config.olla_cqi_inc;
  node["olla_target_bler"]                 = config.olla_target_bler;
  node["olla_max_cqi_offset"]              = config.olla_max_cqi_offset;
  node["dc_offset"]                        = to_string(config.dc_offset);
  node["harq_la_cqi_drop_threshold"]       = static_cast<unsigned>(config.harq_la_cqi_drop_threshold);
  node["harq_la_ri_drop_threshold"]        = static_cast<unsigned>(config.harq_la_ri_drop_threshold);
  node["dmrs_additional_position"]         = config.dmrs_add_pos;
  node["interleaving_bundle_size"]         = static_cast<unsigned>(config.interleaving_bundle_size);
  node["enable_csi_rs_pdsch_multiplexing"] = config.enable_csi_rs_pdsch_multiplexing;

  if (config.max_rank.has_value()) {
    node["max_rank"] = *config.max_rank;
  }

  for (auto rv : config.rv_sequence) {
    node["rv_sequence"].push_back(rv);
  }
  node["rv_sequence"].SetStyle(YAML::EmitterStyle::Flow);

  return node;
}

static std::string to_string(pusch_mcs_table table)
{
  switch (table) {
    case pusch_mcs_table::qam64:
      return "qam64";
    case pusch_mcs_table::qam256:
      return "qam256";
    default:
      srsran_assert(0, "Invalid PDSCH MCS table");
      break;
  }
  return {};
}

static YAML::Node build_du_high_pusch_section(const du_high_unit_pusch_config& config)
{
  YAML::Node node;

  node["min_ue_mcs"]                      = config.min_ue_mcs;
  node["max_ue_mcs"]                      = config.max_ue_mcs;
  node["max_consecutive_kos"]             = config.max_consecutive_kos;
  node["mcs_table"]                       = to_string(config.mcs_table);
  node["max_rank"]                        = config.max_rank;
  node["msg3_delta_preamble"]             = config.msg3_delta_preamble;
  node["p0_nominal_with_grant"]           = config.p0_nominal_with_grant;
  node["max_puschs_per_slot"]             = config.max_puschs_per_slot;
  node["beta_offset_ack_idx_1"]           = config.beta_offset_ack_idx_1;
  node["beta_offset_ack_idx_2"]           = config.beta_offset_ack_idx_2;
  node["beta_offset_ack_idx_3"]           = config.beta_offset_ack_idx_3;
  node["beta_offset_csi_p1_idx_1"]        = config.beta_offset_csi_p1_idx_1;
  node["beta_offset_csi_p1_idx_2"]        = config.beta_offset_csi_p1_idx_2;
  node["beta_offset_csi_p2_idx_1"]        = config.beta_offset_csi_p2_idx_1;
  node["beta_offset_csi_p2_idx_2"]        = config.beta_offset_csi_p2_idx_2;
  node["min_k2"]                          = config.min_k2;
  node["dc_offset"]                       = to_string(config.dc_offset);
  node["olla_snr_inc_step"]               = config.olla_snr_inc;
  node["olla_target_bler"]                = config.olla_target_bler;
  node["olla_max_snr_offset"]             = config.olla_max_snr_offset;
  node["dmrs_additional_position"]        = config.dmrs_add_pos;
  node["min_rb_size"]                     = config.min_rb_size;
  node["max_rb_size"]                     = config.max_rb_size;
  node["start_rb"]                        = config.start_rb;
  node["end_rb"]                          = config.end_rb;
  node["enable_closed_loop_pw_control"]   = config.enable_closed_loop_pw_control;
  node["enable_phr_bw_adaptation"]        = config.enable_phr_bw_adaptation;
  node["target_pusch_sinr"]               = config.target_pusch_sinr;
  node["path_loss_for_target_pusch_sinr"] = config.path_loss_for_target_pusch_sinr;
  node["path_loss_compensation_factor"]   = config.path_loss_compensation_factor;
  node["enable_transform_precoding"]      = config.enable_transform_precoding;

  for (auto rv : config.rv_sequence) {
    node["rv_sequence"].push_back(rv);
  }
  node["rv_sequence"].SetStyle(YAML::EmitterStyle::Flow);

  return node;
}

static std::string to_string(max_pucch_code_rate code_rate)
{
  switch (code_rate) {
    case max_pucch_code_rate::dot_08:
      return "dot08";
    case max_pucch_code_rate::dot_15:
      return "dot15";
    case max_pucch_code_rate::dot_25:
      return "dot25";
    case max_pucch_code_rate::dot_35:
      return "dot35";
    case max_pucch_code_rate::dot_45:
      return "dot45";
    case max_pucch_code_rate::dot_60:
      return "dot60";
    case max_pucch_code_rate::dot_80:
      return "dot80";
    default:
      srsran_assert(0, "Invalid PUCCH code rate");
      break;
  }
  return {};
}

static YAML::Node build_du_high_pucch_section(const du_high_unit_pucch_config& config)
{
  YAML::Node node;

  node["p0_nominal"] = config.p0_nominal;
  if (config.pucch_resource_common.has_value()) {
    node["pucch_resource_common"] = config.pucch_resource_common.value();
  }
  node["use_format_0"]                    = config.use_format_0;
  node["pucch_set1_format"]               = static_cast<unsigned>(config.set1_format);
  node["sr_period_ms"]                    = config.sr_period_msec;
  node["nof_ue_res_harq_per_set"]         = config.nof_ue_pucch_res_harq_per_set;
  node["f0_or_f1_nof_cell_res_sr"]        = config.nof_cell_sr_resources;
  node["f0_intraslot_freq_hop"]           = config.f0_intraslot_freq_hopping;
  node["f1_enable_occ"]                   = config.f1_enable_occ;
  node["f1_nof_cyclic_shifts"]            = config.f1_nof_cyclic_shifts;
  node["f1_intraslot_freq_hop"]           = config.f1_intraslot_freq_hopping;
  node["nof_cell_harq_pucch_res_sets"]    = config.nof_cell_harq_pucch_sets;
  node["f2_or_f3_or_f4_nof_cell_res_csi"] = config.nof_cell_csi_resources;
  node["f2_max_nof_rbs"]                  = config.f2_max_nof_rbs;
  node["f2_max_code_rate"]                = to_string(config.f2_max_code_rate);
  node["f2_intraslot_freq_hop"]           = config.f2_intraslot_freq_hopping;
  node["f3_max_nof_rbs"]                  = config.f3_max_nof_rbs;
  node["f3_max_code_rate"]                = to_string(config.f3_max_code_rate);
  node["f3_intraslot_freq_hop"]           = config.f3_intraslot_freq_hopping;
  node["f3_additional_dmrs"]              = config.f3_additional_dmrs;
  node["f3_pi2_bpsk"]                     = config.f3_pi2_bpsk;
  node["f4_intraslot_freq_hop"]           = config.f4_intraslot_freq_hopping;
  node["f4_max_code_rate"]                = to_string(config.f4_max_code_rate);
  node["f4_additional_dmrs"]              = config.f4_additional_dmrs;
  node["f4_pi2_bpsk"]                     = config.f4_pi2_bpsk;
  node["f4_occ_length"]                   = config.f4_occ_length;
  node["min_k1"]                          = config.min_k1;
  node["max_consecutive_kos"]             = config.max_consecutive_kos;
  if (config.f2_max_payload_bits.has_value()) {
    node["f2_max_payload"] = config.f2_max_payload_bits.value();
  }
  if (config.f3_max_payload_bits.has_value()) {
    node["f3_max_payload"] = config.f3_max_payload_bits.value();
  }

  return node;
}

static YAML::Node build_du_high_prach_section(const du_high_unit_prach_config& config)
{
  YAML::Node node;

  if (config.prach_config_index.has_value()) {
    node["prach_config_index"] = config.prach_config_index.value();
  }
  node["total_nof_ra_preambles"] = config.total_nof_ra_preambles;
  if (config.prach_frequency_start.has_value()) {
    node["prach_frequency_start"] = config.prach_frequency_start.value();
  }

  node["prach_root_sequence_index"] = config.prach_root_sequence_index;
  node["zero_correlation_zone"]     = config.zero_correlation_zone;
  node["fixed_msg3_mcs"]            = config.fixed_msg3_mcs;
  node["max_msg3_harq_retx"]        = config.max_msg3_harq_retx;
  node["preamble_rx_target_pw"]     = config.preamble_rx_target_pw;
  node["preamble_trans_max"]        = static_cast<unsigned>(config.preamble_trans_max);
  node["power_ramping_step_db"]     = static_cast<unsigned>(config.power_ramping_step_db);
  node["nof_ssb_per_ro"]            = config.nof_ssb_per_ro;
  node["nof_cb_preambles_per_ssb"]  = static_cast<unsigned>(config.nof_cb_preambles_per_ssb);
  for (auto id : config.ports) {
    node["ports"].push_back(static_cast<unsigned>(id));
  }
  node["ports"].SetStyle(YAML::EmitterStyle::Flow);
  if (config.ra_resp_window.has_value()) {
    node["ra_resp_window"] = config.ra_resp_window.value();
  }

  return node;
}

static void fill_du_high_tdd_section(YAML::Node& node, const tdd_ul_dl_pattern_unit_config& config)
{
  node["dl_ul_tx_period"] = config.dl_ul_period_slots;
  node["nof_dl_slots"]    = config.nof_dl_slots;
  node["nof_dl_symbols"]  = config.nof_dl_symbols;
  node["nof_ul_slots"]    = config.nof_ul_slots;
  node["nof_ul_symbols"]  = config.nof_ul_symbols;
}

static YAML::Node build_du_high_tdd_section(const du_high_unit_tdd_ul_dl_config& config)
{
  YAML::Node node;

  fill_du_high_tdd_section(node, config.pattern1);

  if (config.pattern2.has_value()) {
    YAML::Node p2_node;
    fill_du_high_tdd_section(p2_node, config.pattern2.value());

    node["pattern2"] = p2_node;
  }

  return node;
}

static std::string to_string(pcch_config::nof_pf_per_drx_cycle cycle)
{
  switch (cycle) {
    case pcch_config::nof_pf_per_drx_cycle::halfT:
      return "halfT";
    case pcch_config::nof_pf_per_drx_cycle::oneT:
      return "oneT";
    case pcch_config::nof_pf_per_drx_cycle::quarterT:
      return "quarterT";
    case pcch_config::nof_pf_per_drx_cycle::oneEighthT:
      return "oneEighthT";
    case pcch_config::nof_pf_per_drx_cycle::oneSixteethT:
      return "oneSixteethT";
    default:
      srsran_assert(0, "Invalid PCCH config");
      break;
  }
  return {};
}

static YAML::Node build_du_high_paging_section(const du_high_unit_paging_config& config)
{
  YAML::Node node;

  node["pg_search_space_id"]      = config.paging_search_space_id;
  node["default_pg_cycle_in_rf"]  = config.default_paging_cycle;
  node["nof_pf_per_paging_cycle"] = to_string(config.nof_pf);
  node["pf_offset"]               = config.pf_offset;
  node["nof_po_per_pf"]           = config.nof_po_per_pf;

  return node;
}

static YAML::Node build_du_high_csi_section(const du_high_unit_csi_config& config)
{
  YAML::Node node;

  node["csi_rs_enabled"]  = config.csi_rs_enabled;
  node["csi_rs_period"]   = config.csi_rs_period_msec;
  node["pwr_ctrl_offset"] = config.pwr_ctrl_offset;
  if (config.meas_csi_slot_offset.has_value()) {
    node["meas_csi_slot_offset"] = config.meas_csi_slot_offset.value();
  }
  if (config.tracking_csi_slot_offset.has_value()) {
    node["tracking_csi_rs_slot_offset"] = config.tracking_csi_slot_offset.value();
  }
  if (config.zp_csi_slot_offset.has_value()) {
    node["zp_csi_rs_slot_offset"] = config.zp_csi_slot_offset.value();
  }

  return node;
}

static void fill_du_high_sched_expert_section(YAML::Node& node, const du_high_unit_scheduler_expert_config& config)
{
  if (config.policy_sched_expert_cfg.has_value() and
      std::holds_alternative<time_qos_scheduler_expert_config>(*config.policy_sched_expert_cfg)) {
    YAML::Node sched_node;
    YAML::Node policy_node;
    YAML::Node policy_pf_node;
    policy_pf_node["pf_fairness_coeff"] =
        std::get<time_qos_scheduler_expert_config>(*config.policy_sched_expert_cfg).pf_fairness_coeff;
    policy_pf_node["prio_enabled"] =
        std::get<time_qos_scheduler_expert_config>(*config.policy_sched_expert_cfg).priority_enabled;

    policy_node["qos_sched"]       = policy_pf_node;
    sched_node["policy_sched_cfg"] = policy_node;
    node["sched_expert_cfg"]       = sched_node;
  }
}

static YAML::Node build_du_high_srs_section(const du_high_unit_srs_config& config)
{
  YAML::Node node;

  if (config.srs_period_ms.has_value()) {
    node["srs_period_ms"] = config.srs_period_ms.value();
  }
  node["srs_max_nof_sym_per_slot"] = config.max_nof_symbols_per_slot;
  node["srs_nof_sym_per_resource"] = config.nof_symbols;
  node["srs_tx_comb"]              = config.tx_comb;
  node["srs_cyclic_shift_reuse"]   = config.cyclic_shift_reuse_factor;
  node["srs_sequence_id_reuse"]    = config.sequence_id_reuse_factor;

  return node;
}

static YAML::Node build_du_high_drx_section(const du_high_unit_drx_config& config)
{
  YAML::Node node;

  node["on_duration_timer"] = config.on_duration_timer;
  node["inactivity_timer"]  = config.inactivity_timer;
  node["retx_timer_dl"]     = config.retx_timer_dl;
  node["retx_timer_ul"]     = config.retx_timer_ul;
  node["long_cycle"]        = config.long_cycle;

  return node;
}

static YAML::Node build_cell_entry(const du_high_unit_base_cell_config& config)
{
  YAML::Node node;

  node["pci"]                   = config.pci;
  node["dl_arfcn"]              = config.dl_f_ref_arfcn;
  node["common_scs"]            = scs_to_khz(config.common_scs);
  node["channel_bandwidth_MHz"] = bs_channel_bandwidth_to_MHz(config.channel_bw_mhz);
  node["nof_antennas_ul"]       = config.nof_antennas_ul;
  node["nof_antennas_dl"]       = config.nof_antennas_dl;
  node["plmn"]                  = config.plmn;
  node["additional_plmns"]      = config.additional_plmns;
  node["tac"]                   = config.tac;
  node["q_rx_lev_min"]          = config.q_rx_lev_min;
  node["q_qual_min"]            = config.q_qual_min;
  if (config.pcg_cfg.p_nr_fr1.has_value()) {
    node["pcg_p_nr_fr1"] = config.pcg_cfg.p_nr_fr1.value();
  }
  if (config.band.has_value()) {
    node["band"] = static_cast<unsigned>(config.band.value());
  }
  node["mac_cell_group"] = build_du_high_mac_cell_group_section(config.mcg_cfg);
  node["ssb"]            = build_du_high_ssb_section(config.ssb_cfg);
  node["sib"]            = build_du_high_sib_section(config.sib_cfg);
  node["ul_common"]      = build_du_high_ul_common_section(config.ul_common_cfg);
  node["pdcch"]          = build_du_high_pdcch_section(config.pdcch_cfg);
  node["pdsch"]          = build_du_high_pdsch_section(config.pdsch_cfg);
  node["pusch"]          = build_du_high_pusch_section(config.pusch_cfg);
  node["pucch"]          = build_du_high_pucch_section(config.pucch_cfg);
  node["prach"]          = build_du_high_prach_section(config.prach_cfg);
  if (config.tdd_ul_dl_cfg) {
    node["tdd_ul_dl_cfg"] = build_du_high_tdd_section(config.tdd_ul_dl_cfg.value());
  }

  node["paging"] = build_du_high_paging_section(config.paging_cfg);
  node["csi"]    = build_du_high_csi_section(config.csi_cfg);
  node["srs"]    = build_du_high_srs_section(config.srs_cfg);
  if (config.drx_cfg.long_cycle != 0) {
    node["drx"] = build_du_high_drx_section(config.drx_cfg);
  }
  fill_du_high_sched_expert_section(node, config.sched_expert_cfg);

  if (config.ntn_cfg) {
    node["ntn"] = build_du_high_ntn_section(config.ntn_cfg.value());
  }

  return node;
}

static void build_du_high_cells_section(YAML::Node& node, span<const du_high_unit_cell_config> cells)
{
  for (const auto& cell : cells) {
    node["cells"].push_back(build_cell_entry(cell.cell));
  }
}

static void fill_du_high_am_section(YAML::Node node, const du_high_unit_rlc_am_config& config)
{
  {
    YAML::Node tx_node            = node["tx"];
    tx_node["sn"]                 = config.tx.sn_field_length;
    tx_node["t-poll-retransmit"]  = config.tx.t_poll_retx;
    tx_node["max-retx-threshold"] = config.tx.max_retx_thresh;
    tx_node["poll-pdu"]           = config.tx.poll_pdu;
    tx_node["poll-byte"]          = config.tx.poll_byte;
    tx_node["max_window"]         = config.tx.max_window;
    tx_node["queue-size"]         = config.tx.queue_size;
  }
  {
    YAML::Node rx_node           = node["rx"];
    rx_node["sn"]                = config.rx.sn_field_length;
    rx_node["t-reassembly"]      = config.rx.t_reassembly;
    rx_node["t-status-prohibit"] = config.rx.t_status_prohibit;
    rx_node["max_sn_per_status"] = config.rx.max_sn_per_status;
  }
}

static void fill_du_high_um_bidir_section(YAML::Node node, const du_high_unit_rlc_um_config& config)
{
  {
    YAML::Node tx_node    = node["tx"];
    tx_node["sn"]         = config.tx.sn_field_length;
    tx_node["queue-size"] = config.tx.queue_size;
  }
  {
    YAML::Node rx_node      = node["rx"];
    rx_node["sn"]           = config.rx.sn_field_length;
    rx_node["t-reassembly"] = config.rx.t_reassembly;
  }
}

static void fill_du_high_rlc_qos_section(YAML::Node node, const du_high_unit_rlc_config& config)
{
  node["mode"] = config.mode;
  if (config.mode == "am") {
    fill_du_high_am_section(node["am"], config.am);
  }
  if (config.mode == "um-bidir") {
    fill_du_high_um_bidir_section(node["um-bidir"], config.um);
  }
}

static void fill_du_high_f1u_qos_section(YAML::Node node, const du_high_unit_f1u_du_config& config)
{
  node["backoff_timer"] = config.t_notify;
}

static void fill_du_high_qos_entry(YAML::Node node, const du_high_unit_qos_config& config)
{
  node["five_qi"] = five_qi_to_uint(config.five_qi);
  fill_du_high_rlc_qos_section(node["rlc"], config.rlc);
  fill_du_high_f1u_qos_section(node["f1u_du"], config.f1u_du);
}

static YAML::Node get_last_entry(YAML::Node node)
{
  auto it = node.begin();
  for (unsigned i = 1; i != node.size(); ++i) {
    ++it;
  }
  return *it;
}

static void fill_qos_section(YAML::Node node, span<const du_high_unit_qos_config> qos_cfg)
{
  auto qos_node = node["qos"];
  for (const auto& qos : qos_cfg) {
    auto node_entry = std::find_if(qos_node.begin(), qos_node.end(), [five = qos.five_qi](const YAML::Node& tmp) {
      return static_cast<uint16_t>(five) == tmp["five_qi"].as<uint16_t>();
    });
    if (node_entry != qos_node.end()) {
      YAML::Node node_five = *node_entry;
      fill_du_high_qos_entry(node_five, qos);
    } else {
      qos_node.push_back(YAML::Node());
      fill_du_high_qos_entry(get_last_entry(qos_node), qos);
    }
  }
}

static void build_du_high_sbr_section(YAML::Node& node, const std::map<srb_id_t, du_high_unit_srb_config>& sbrs)
{
  for (const auto& cell : sbrs) {
    YAML::Node entry;
    entry["srb_id"] = cell.second.srb_id;
    fill_du_high_am_section(entry["rlc"], cell.second.rlc);
    node["srbs"].push_back(entry);
  }
}

static YAML::Node build_du_high_testmode_section(const du_high_unit_test_mode_config& config)
{
  YAML::Node node;
  {
    YAML::Node ue_node;

    ue_node["rnti"]    = to_value(config.test_ue.rnti);
    ue_node["nof_ues"] = config.test_ue.nof_ues;
    if (config.test_ue.auto_ack_indication_delay.has_value()) {
      ue_node["auto_ack_indication_delay"] = config.test_ue.auto_ack_indication_delay.value();
    }
    ue_node["pdsch_active"] = config.test_ue.pdsch_active;
    ue_node["pusch_active"] = config.test_ue.pusch_active;
    ue_node["cqi"]          = config.test_ue.cqi;
    ue_node["ri"]           = config.test_ue.ri;
    ue_node["pmi"]          = config.test_ue.pmi;
    ue_node["i_1_1"]        = config.test_ue.i_1_1;
    ue_node["i_1_3"]        = config.test_ue.i_1_3;
    ue_node["i_2"]          = config.test_ue.i_2;

    node["test_ue"] = ue_node;
  }

  return node;
}

void srsran::fill_du_high_config_in_yaml_schema(YAML::Node& node, const du_high_unit_config& config)
{
  node["gnb_id"]            = config.gnb_id.id;
  node["gnb_id_bit_length"] = static_cast<unsigned>(config.gnb_id.bit_length);
  node["gnb_du_id"]         = static_cast<uint64_t>(config.gnb_du_id);

  app_helpers::fill_metrics_appconfig_in_yaml_schema(node, config.metrics.common_metrics_cfg);
  fill_du_high_log_section(node["log"], config.loggers);
  fill_du_high_metrics_section(node["metrics"], config.metrics);
  fill_du_high_pcap_section(node["pcap"], config.pcaps);
  node["du"] = build_du_section(config);
  if (config.test_mode_cfg.test_ue.rnti != rnti_t::INVALID_RNTI) {
    node["test_mode"] = build_du_high_testmode_section(config.test_mode_cfg);
  }

  fill_qos_section(node, config.qos_cfg);
  build_du_high_cells_section(node, config.cells_cfg);
  build_du_high_sbr_section(node, config.srb_cfg);
}
