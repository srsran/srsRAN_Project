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

#pragma once

#include <array>
#include <cstdint>

namespace srsran {
namespace fapi {

/// Encodes the optional and mandatory status for the TLVs.
enum class tlv_config_status : uint8_t {
  idle_optional,
  idle_mandatory,
  idle_and_configured_optionals,
  idle_mandatory_configured_optional,
  idle_and_configured_and_running_optionals,
  idle_mandatory_configured_and_running_optionals
};

/// Encodes the signal power profile.
struct signal_power_profile {
  uint8_t reference_rs;
  int16_t power_offset_min;
  int16_t power_offset_max;
};

/// Encodes the cell parameters.
struct cell_param {
  /// Maximum number of config TLVs to report
  static constexpr unsigned MAX_TLVS_TO_REPORT = 1505;

  uint16_t                            release_capability;
  uint16_t                            phy_state;
  uint8_t                             skip_blank_dl_config;
  uint8_t                             skip_blank_ul_config;
  uint16_t                            num_config_tlvs_to_report;
  uint8_t                             power_profiles_supported;
  std::array<signal_power_profile, 8> signal_profile;
  uint8_t                             ss_pbch_block_power_scaling_unit_choice;
  uint16_t                            max_num_pdu_dl_tti;
  uint16_t                            max_num_pdu_ul_tti;
  uint16_t                            max_num_pdu_ul_dci;
};

/// Encodes the carrier parameters.
struct carrier_param {
  uint8_t  cp;
  uint8_t  supported_subcarrier_spacings_dl;
  uint8_t  supported_subcarrier_spacings_ssb;
  uint16_t supported_bandwidth_dl;
  uint8_t  supported_subcarrier_spacings_ul;
  uint16_t supported_bandwidth_ul;
  uint8_t  ss_pbch_multiple_carriers_in_a_band;
  uint8_t  multiple_cells_ss_pbch_in_a_carrier;
  uint8_t  meaning_of_carrier_frequency;
};

/// Encodes the pdcch parameters.
struct pdcch_param {
  uint8_t cce_mapping_type;
  uint8_t coreset_outside_first_3_ofdm_syms_of_slot;
  uint8_t precoder_granularity_coreset;
  uint8_t pdcch_mu_mimo;
  uint8_t pdcch_precoder_cycling;
  uint8_t max_pdcchs_per_slot;
};

/// Encodes the modes for the signaling mechanism of PUCCH group and Sequency hopping.
enum class pucch_signaling_modes : uint8_t { dynamic, semi_static, both };

/// Encodes the pucch parameters.
struct pucch_param {
  uint8_t               pucch_formats;
  uint8_t               max_pucchs_per_slot;
  pucch_signaling_modes pucch_group_and_sequence_hopping;
  uint8_t               max_num_ul_bwp_ids;
  uint8_t               pucch_aggregation;
};

/// Encodes the modulation order di.
enum class modulation_order_dl : uint8_t { QPSK, QAM_16, QAM_64, QAM_256 };

/// Encodes the ssb rate match support.
enum class ssb_rate_match_type : uint8_t { not_supported, SSB_configuration, SSB_pdu_index, both };

/// Encodes the PDSCH parameters.
/// \note not adding numOfRateMAtchPatternLTECrsPerSlot, numOfRateMatchPatternLTEInPhy,
/// lteCrsRateMatchMbsfnDerivation, supportedLTECrsRateMatchMethod as they refer to LTE.
struct pdsch_param {
  uint8_t                 pdsch_mapping_type;
  uint8_t                 pdsch_allocation_types;
  uint8_t                 pdsch_vrb_to_prb_mapping;
  uint8_t                 pdsch_cbg;
  uint8_t                 pdsch_dmrs_config_types;
  uint8_t                 pdsch_dmrs_max_length;
  uint8_t                 pdsch_dmrs_additional_pos;
  uint8_t                 max_pdschs_tbs_per_slot;
  uint8_t                 max_num_mimo_layers_pdsch;
  modulation_order_dl     supported_max_modulation_order_dl;
  uint8_t                 max_mu_mimo_users_dl;
  uint8_t                 pdsch_data_in_dmrs_symbols;
  uint8_t                 preemption_support;
  uint8_t                 pdsch_non_slot_support;
  ssb_rate_match_type     ssb_rate_match;
  uint16_t                supported_rate_match_pattern_type_and_method;
  uint8_t                 pdcch_rate_match;
  uint8_t                 csi_rs_rate_match;
  uint8_t                 pdsch_trans_type_support;
  uint8_t                 pdsch_mac_pdu_bit_alignment;
  std::array<uint16_t, 5> max_number_prb_sym_bitmap_per_slot;
  std::array<uint16_t, 5> max_number_csi_rs_rate_match_per_slot;
  uint8_t                 max_num_ssb_per_pdsch_slot;
  uint8_t                 universal_rate_match;
  uint8_t                 require_pdsch_signaling_associations;
};

/// Encodes the modulation order ul.
enum class modulation_order_ul : uint8_t { QPSK, QAM_16, QAM_64, QAM_256 };

/// Encodes the PUSCH parameters.
struct pusch_param {
  uint8_t             uci_mux_ulsch_in_pusch;
  uint8_t             uci_only_pusch;
  uint8_t             pusch_frequency_hopping;
  uint8_t             pusch_dmrs_config_types;
  uint8_t             pusch_dmrs_max_len;
  uint8_t             pusch_dmrs_additional_pos;
  uint8_t             pusch_cbg;
  uint8_t             pusch_mapping_type;
  uint8_t             pusch_allocation_types;
  uint8_t             pusch_vrb_to_prb_mapping;
  uint8_t             pusch_max_ptrs_ports;
  uint8_t             pusch_max_puschs_tbs_per_slot;
  uint8_t             max_num_mimo_layers_non_cb_pusch;
  uint8_t             max_num_mimo_layers_cb_pusch;
  modulation_order_ul supported_max_modulation_order_ul;
  uint8_t             max_mu_mimo_users_ul;
  uint8_t             dfts_ofdm_support;
  uint8_t             pusch_aggregation_factor;
  uint8_t             pusch_lbrm_support;
  uint8_t             pusch_trans_type_support;
  uint8_t             pusch_mac_pdu_bit_alignment;
};

/// Encodes the MsgA-PUSCH parameters.
struct msg_a_pusch_param {
  uint16_t msg_a_num_pusch_res_config;
  uint8_t  msg_a_pusch_tran_precoding;
  uint16_t msg_a_intra_slot_pusch_freq_hop_h;
  uint8_t  msg_a_max_pusch_fd_occasions;
  uint8_t  msg_a_guard_band;
  uint8_t  msg_a_guard_period;
  uint8_t  msg_a_pusch_mapping_type;
  uint8_t  msg_a_pusch_dmrs_max_len;
  uint8_t  msg_a_pusch_dmrs_cdm_group;
  uint8_t  msg_a_pusch_dmrs_additional_pos;
  uint8_t  msg_a_supported_max_modulation_order_ul;
  uint8_t  msg_a_max_pusch_per_pru;
  uint8_t  msg_a_max_num_cdm_group;
  uint8_t  msg_a_max_num_dmrs_port;
  uint8_t  msg_a_prach_to_pusch_mapping_scheme;
};

/// Encodes the PRACH parameters.
struct prach_param {
  /// Maximum number RO format types.
  static constexpr unsigned MAX_NUM_RO_FORMATS = 16;

  uint8_t                                  prach_long_formats;
  uint16_t                                 prach_short_formats;
  uint8_t                                  prach_restricted_sets;
  uint8_t                                  max_prach_fd_occasions_in_a_slot;
  uint16_t                                 max_num_prach_configurations;
  uint8_t                                  prach_multiple_carriers_in_a_band;
  uint8_t                                  prach_frequency_offset_meaning;
  std::array<uint16_t, 5>                  max_num_td_fd_prach_occasions_per_slot;
  std::array<uint16_t, MAX_NUM_RO_FORMATS> prach_root_processing_rate;
  std::array<uint32_t, MAX_NUM_RO_FORMATS> prach_occasion_queue_size;
};

/// Encodes the MsgA-PRACH parameters.
struct msg_a_prach_param {
  uint16_t msg_a_max_num_prach_confs;
  uint8_t  msg_a_prach_long_formats;
  uint16_t msg_a_prach_short_formats;
  uint8_t  msg_a_prach_restricted_sets;
  uint8_t  msg_a_max_prach_fd_occasions_in_slot;
  uint8_t  msg_a_prach_multiple_carriers_in_a_band;
};

/// Encodes the Measurement reporting parameters.
struct measurement_param {
  uint8_t rssi_measurement_support;
  uint8_t rsrp_measurement_support;
};

/// Encodes the UCI parameters.
struct uci_param {
  uint16_t max_num_uci_maps;
  uint8_t  uci_report_format_pucch_format234;
};

/// Encodes the PHY parameters.
struct phy_param {
  /// Maximum number of downlink port ranges.
  static constexpr unsigned MAX_NUM_DL_PORT_RANGES = 64;
  /// Maximum number of uplink port ranges.
  static constexpr unsigned MAX_NUM_UL_PORT_RANGES = 64;

  // :TODO: Review the parameter DL BB Ports. It is not codified right now.
  uint8_t                                      num_dl_port_ranges;
  std::array<uint16_t, MAX_NUM_DL_PORT_RANGES> dl_port_range_start;
  std::array<uint16_t, MAX_NUM_DL_PORT_RANGES> dl_port_range_length;
  uint8_t                                      num_ul_port_ranges;
  std::array<uint16_t, MAX_NUM_UL_PORT_RANGES> ul_port_range_start;
  std::array<uint16_t, MAX_NUM_UL_PORT_RANGES> ul_port_range_length;
};

/// Encodes the PHY support profiles parameters.
struct phy_profiles_param {
  /// Maximum number of PHY IDs supported by the PHY profile.
  static constexpr unsigned MAX_NUM_PHY_IDS = 64;

  uint8_t                                max_num_phys;
  std::array<phy_param, MAX_NUM_PHY_IDS> phy;
};

/// Encodes the supported phy profiles. Only supported by PHY ID 0.
struct phy_profiles_supported {
  /// Maximum number of PHY profiles supported.
  static constexpr unsigned MAX_NUM_PHY_PROFILES = 64;

  uint16_t                                             num_phy_profiles;
  uint16_t                                             num_dl_bb_ports;
  uint16_t                                             num_ul_bb_ports;
  std::array<phy_profiles_param, MAX_NUM_PHY_PROFILES> phy_profiles;
};

/// Encodes the FAPI PHY protocol version.
struct phy_fapi_protocol {
  uint8_t phy_fapi_protocol_version;
  uint8_t phy_fapi_negotiated_protocol_version;
};

/// Encodes the PHY Support parameters.
struct phy_support_param {
  phy_profiles_supported phy_profiles;
  uint8_t                time_management;
  phy_fapi_protocol      fapi_version;
  std::array<uint8_t, 6> more_than_one_indication_per_slot;
  std::array<uint8_t, 4> more_than_one_request_per_slot;
};

/// Encodes the PHY/DFE profile validity parameters.
struct phy_dfe_profile_param {
  /// Maximum size for the Validity map in Bytes.
  static constexpr unsigned MAX_VALIDITY_MAP_SIZE_IN_BYTES = 8;

  uint16_t                                            num_phy_profiles;
  uint16_t                                            num_dfe_profiles;
  std::array<uint8_t, MAX_VALIDITY_MAP_SIZE_IN_BYTES> profile_validity;
};

/// Encodes the Delay management parameters.
struct delay_management_param {
  uint32_t dl_tti_timing_offset;
  uint32_t ul_tti_timing_offset;
  uint32_t ul_dci_timing_offset;
  uint32_t tx_data_timing_offset;
  uint16_t timing_window;
  uint8_t  timing_info_period;
};

/// Encodes user plane parameters.
struct user_plane_param {
  uint32_t user_plane_encapsulation_control_plane_separation;
  uint8_t  message_encoding_support;
};

/// Encodes the SRS parameters.
struct srs_param {
  uint32_t                 supported_srs_usage_and_pairing;
  std::array<uint16_t, 10> rb_subsampling_resolution_for_srs_derived_report;
  std::array<uint16_t, 10> max_srs_channel_reports_per_slot;
  uint16_t                 max_num_srs_pdus_per_slot;
  uint32_t                 iq_sample_formats;
  uint32_t                 singular_value_formats;
  std::array<uint8_t, 4>   max_srs_tx_ant_ports_per_ue;
  uint8_t                  max_num_consecutive_srs_slots;
  uint16_t                 max_duty_cycle_in_percentage;
  uint8_t                  max_srs_comb_size;
  uint8_t                  max_num_srs_cyclic_shifts;
  std::array<uint16_t, 2>  srs_symbols_bitmap;
  uint8_t                  max_num_srs_symbols_per_slot;
};

/// Encodes MIMO parameters.
struct mimo_param {
  std::array<uint16_t, 5> max_num_carriers_bw_layers_product_dl;
  std::array<uint16_t, 5> max_num_carriers_bw_layers_product_ul;
  std::array<uint16_t, 5> max_num_carriers_bw_antennas_product_dl;
  std::array<uint16_t, 5> max_num_carriers_bw_antennas_product_ul;
  uint32_t                mu_mimo_dl;
  std::array<uint32_t, 3> supported_spatial_mux_per_dl_channel_type;
  std::array<uint8_t, 3>  max_num_mu_mimo_layers_per_dl_channel_type;
  uint16_t                cross_channel_spatial_multiplexing_for_dl;
  uint32_t                mu_mimo_ul;
  std::array<uint32_t, 2> supported_spatial_mux_per_ul_channel_type;
  std::array<uint8_t, 2>  max_num_mu_mimo_layers_per_ul_channel_type;
  uint16_t                cross_channel_spatial_multiplexing_for_ul;
  std::array<uint16_t, 3> max_mu_mimo_groups_in_fd_per_dl_channel;
  std::array<uint8_t, 3>  max_mu_mimo_groups_in_td_per_dl_channel;
  uint16_t                max_mu_mimo_new_precoders_per_slot;
  uint8_t                 all_prbs_groups_prgs;
  std::array<uint16_t, 2> max_mu_mimo_groups_in_fd_per_ul_channel;
  std::array<uint8_t, 2>  max_mu_mimo_groups_in_td_per_ul_channel;
  uint16_t                min_rb_resolution_for_srs_derived_reports;
  uint16_t                max_number_dl_spatial_streams;
  uint32_t                dl_spatial_stream_channel_priority;
  uint16_t                max_number_ul_spatial_streams;
  uint8_t                 ul_tpmi_capability;
};

/// Encodes the validity for a capability.
struct capability_validity {
  uint16_t capability_tag;
  uint8_t  validity_scope;
};

/// Encodes the capability parameters.
struct capability_param {
  /// Maximum number of tag capabilities supported.
  static constexpr unsigned MAX_NUM_SUPPORTED_TAGS = 2048;

  uint16_t                                                num_capabilities;
  std::array<capability_validity, MAX_NUM_SUPPORTED_TAGS> capabilities;
};

} // namespace fapi
} // namespace srsran
