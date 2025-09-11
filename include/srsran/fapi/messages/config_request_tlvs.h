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

#include "srsran/adt/static_vector.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/dmrs.h"
#include "srsran/ran/prach/prach_format_type.h"
#include "srsran/ran/prach/prach_subcarrier_spacing.h"
#include "srsran/ran/prach/restricted_set_config.h"
#include "srsran/ran/ssb/ssb_properties.h"
#include "srsran/ran/subcarrier_spacing.h"
#include <array>
#include <cstdint>

namespace srsran {
namespace fapi {

/// Encodes the PHY configuration.
struct phy_config {
  uint16_t               profile_id;
  std::array<uint8_t, 6> indication_instances_per_slot;
  std::array<uint8_t, 4> request_instances_per_slot;
  uint16_t               sfn;
  uint8_t                slot;
  subcarrier_spacing     scs;
  cyclic_prefix          cp;
};

/// Encodes the carrier configuration.
struct carrier_config {
  uint16_t                dl_bandwidth;
  uint32_t                dl_f_ref_arfcn;
  std::array<uint16_t, 5> dl_k0;
  std::array<uint16_t, 5> dl_grid_size;
  uint16_t                num_tx_ant;
  uint16_t                ul_bandwidth;
  uint32_t                ul_f_ref_arfcn;
  std::array<uint16_t, 5> ul_k0;
  std::array<uint16_t, 5> ul_grid_size;
  uint16_t                num_rx_ant;
  uint8_t                 freq_shift_7p5kHz;
  uint8_t                 power_profile;
  uint8_t                 power_offset_rs_index;
  dmrs_typeA_position     dmrs_typeA_pos;
};

/// Encodes the cell configuration.
struct cell_config {
  uint16_t phy_cell_id;
  uint8_t  frame_duplex_type;
  uint8_t  pdsch_trans_type_validity;
  uint8_t  pusch_trans_type_validity;
};

/// Encodes the SSB/PBCH configuration.
struct ssb_pbch_config {
  uint32_t ss_pbch_power;
  int16_t  ss_pbch_block_power_scaling;
  uint8_t  bch_payload_flag;
};

/// Encodes the PRACH Fd occasion configuration.
struct prach_fd_occasion_config {
  /// Maximum number of unused root sequences allowed.
  static constexpr unsigned MAX_NUM_UNUSED_ROOT_SEQ = 63;

  uint16_t                                         prach_root_sequence_index;
  uint8_t                                          num_root_sequences;
  int16_t                                          prach_freq_offset;
  uint16_t                                         prach_guardband_offset;
  uint8_t                                          prach_zero_corr_conf;
  static_vector<uint16_t, MAX_NUM_UNUSED_ROOT_SEQ> unused_root_sequences;
};

enum class prach_sequence_length_type : uint8_t { long_sequence, short_sequence };

/// Encodes the PRACH configuration.
struct prach_config {
  /// Maximum number of PRACH Fd occasions allowed.
  static constexpr unsigned MAX_NUM_PRACH_FD_OCCASIONS = 64;

  uint16_t                                                            prach_res_config_index;
  prach_sequence_length_type                                          prach_sequence_length;
  prach_subcarrier_spacing                                            prach_scs;
  subcarrier_spacing                                                  prach_ul_bwp_pusch_scs;
  restricted_set_config                                               restricted_set;
  uint8_t                                                             num_prach_fd_occasions;
  uint8_t                                                             prach_config_index;
  prach_format_type                                                   prach_format;
  uint8_t                                                             num_prach_td_occasions;
  uint8_t                                                             num_preambles;
  uint8_t                                                             start_preamble_index;
  static_vector<prach_fd_occasion_config, MAX_NUM_PRACH_FD_OCCASIONS> fd_occasions;
  uint8_t                                                             ssb_per_rach;
};

/// Encodes the Multi-PRACH configuration.
struct multi_prach_config {
  /// Maximum number of PRACH configurations supported.
  static constexpr unsigned MAX_NUM_PRACH_CONFIGURATIONS = 128;

  static_vector<prach_config, MAX_NUM_PRACH_CONFIGURATIONS> prachs;
};

/// PRACH preamble group structure.
struct prach_preamble_group {
  /// Maximum number of associated dmrs.
  static constexpr unsigned MAX_NUM_ASSOC_DMRS = 16;

  uint16_t                                 pru_preamble_index_for_start_preamble_index;
  uint8_t                                  num_pusch_assoc_dmrs_occasions_for_this_prach_ro;
  std::array<uint16_t, MAX_NUM_ASSOC_DMRS> msg_a_pusch_assoc_dmrs_occasion;
};

/// Encodes the MsgA-PRACH-to-(PRU & DMRS) map multiple PRACH configurations for the carrier.
struct msg_a_prach_to_pru_dmrs_map {
  /// Maximum number of supported preambles.
  static constexpr unsigned MAX_NUM_PREAMBLES = 8;

  uint8_t                                             valid_ro;
  std::array<uint8_t, MAX_NUM_PREAMBLES>              cat_a_or_b_bitmap;
  std::array<prach_preamble_group, MAX_NUM_PREAMBLES> preamble_group;
};

/// Encodes MsgA-PRACH-to-(PRU & DMRS) configurations for the carrier.
struct msg_a_prach_to_pru_dmrs_config {
  /// Maximum number of PRACH FD indices.
  static constexpr unsigned MAX_NUM_PRACH_FD_INDICES = 64;
  /// Maximum number of PRACH TD indices.
  static constexpr unsigned MAX_NUM_PRACH_TD_INDICES = 64;

  std::array<std::array<msg_a_prach_to_pru_dmrs_map, MAX_NUM_PRACH_TD_INDICES>, MAX_NUM_PRACH_FD_INDICES> map;
};

/// Encodes PRACH slot configuration.
struct prach_slot_config {
  /// Maximum number of PRACH slot indices modulus.
  static constexpr unsigned MAX_NUM_PRACH_SLOT = 64;

  uint16_t                                 num_prach_slots_for_prach_to_pru_and_dmrs_map;
  std::array<uint16_t, MAX_NUM_PRACH_SLOT> slot_indices_modulus;
  msg_a_prach_to_pru_dmrs_config           mapping;
};

/// Encodes the MsgA-PUSCH configuration.
struct msg_a_pusch_config {
  /// Maximum number of PRACH slots.
  static constexpr unsigned MAX_NUM_PRACH_SLOTS = 64;

  uint16_t                                           msg_a_pusch_res_config_index;
  uint16_t                                           msg_a_prach_res_config_index;
  uint8_t                                            msg_a_group_a_or_b;
  uint16_t                                           nld_msg_a_pusch;
  uint16_t                                           dmrs_ports;
  uint8_t                                            available_dmrs_sequence_ids;
  std::array<uint16_t, 2>                            pusch_dmrs_scrambling_ids;
  uint8_t                                            num_pusch_occasions_fd;
  uint8_t                                            num_pusch_occasions_td;
  uint8_t                                            num_pusch_occasions_slots;
  uint8_t                                            msg_a_pusch_time_domain_offset;
  uint16_t                                           n_preambles;
  uint8_t                                            association_pattern_period;
  std::array<prach_slot_config, MAX_NUM_PRACH_SLOTS> prach_slots;
};

/// Encodes the Multi-MsgA-PUSCH configuration.
struct multi_msg_a_pusch_config {
  /// Maximum number of configurations.
  static constexpr unsigned MAX_NUM_CONFIGS = 64;

  uint16_t                                        num_configurations;
  std::array<msg_a_pusch_config, MAX_NUM_CONFIGS> configs;
};

/// Encodes the SSB resource configuration.
/// \note mask_size value is 2 or 64. When mask_size == 2 mask field of this struct is valid, otherwise (mask_size ==
/// 64) beam_id field is valid;
struct ssb_config {
  uint16_t                ssb_config_index;
  ssb_offset_to_pointA    ssb_offset_point_a;
  uint8_t                 beta_pss_profile_nr;
  int16_t                 beta_pss_profile_sss;
  ssb_periodicity         ssb_period;
  uint8_t                 ssb_subcarrier_offset;
  ssb_pattern_case        case_kind;
  subcarrier_spacing      scs;
  subcarrier_spacing      sub_carrier_spacing_common;
  uint8_t                 mask_size;
  std::array<uint32_t, 2> ssb_mask;
  std::array<uint8_t, 64> beam_id;
  uint8_t                 l_max;
  uint8_t                 rmsi_presence;
};

/// Encodes the Multi-SSB resource configuration.
struct multi_ssb_config {
  /// Maximum number of supported SSB TLVs.
  static constexpr unsigned MAX_NUM_SSBS = 64;

  uint8_t                              num_configs;
  std::array<ssb_config, MAX_NUM_SSBS> ssbs;
};

/// Encodes the TDD slot symbol type.
enum class tdd_slot_symbol_type : uint8_t { dl_symbol, ul_symbol, flexible_symbol };

/// Encodes the TDD configuration in the PHY.
struct tdd_phy_config {
  /// Maximum TDD periodicity.
  static constexpr unsigned MAX_TDD_PERIODICITY = 160;
  /// Maximum number of symbols per slot.
  static constexpr unsigned MAX_NUM_SYMBOL_PER_SLOT = 14;

  uint8_t                                                                                    tdd_period;
  std::array<std::array<tdd_slot_symbol_type, MAX_NUM_SYMBOL_PER_SLOT>, MAX_TDD_PERIODICITY> slot_config;
};

/// Encodes the measurement configuration.
struct measurement_config {
  uint8_t rssi_measurement;
  uint8_t rsrp_measurement;
};

/// Encodes a map which indicates how a set of UCI part1 parameters map to a length of corresponding UCI part2.
/// \note Maximum value for MAX_SIZE_MAP is 2^MAX_NUM_PART1_PARAMS = 4096.
struct uci_parts_map {
  /// Maximum number of part1 parameters.
  static constexpr unsigned MAX_NUM_PART1_PARAMS = 12;
  /// Maximum size for the map.
  static constexpr unsigned MAX_SIZE_MAP = 1024;

  uint8_t                                   num_part1_params;
  std::array<uint8_t, MAX_NUM_PART1_PARAMS> size_part1_params;
  std::array<uint16_t, MAX_SIZE_MAP>        map;
};

/// Encodes the UCI indication parameters.
struct uci_indication_config {
  /// Maximum number of maps.
  static constexpr unsigned MAX_NUM_MAPS = 128;

  uint16_t                                num_uci_to_maps;
  std::array<uci_parts_map, MAX_NUM_MAPS> maps;
};

/// Encodes the PRB-symbol rate match patterns structure.
struct prb_symbol_rm_pattern {
  uint8_t                 prb_symbol_rate_match_pattern_id;
  std::array<uint8_t, 35> freq_domain_rb;
  uint32_t                symbols_in_rb;
  subcarrier_spacing      scs;
};

/// Encodes the PRB-symbol rate match patterns bitmap configuration.
struct prb_symbol_rm_pattern_config {
  /// Maximum number of PRB-symbol rate-match patterns.
  static constexpr unsigned MAX_NUM_PATTERNS = 64;

  uint8_t                                             num_patterns;
  std::array<prb_symbol_rm_pattern, MAX_NUM_PATTERNS> patterns;
};

/// Encodes the CORESET rate-match pattern structure.
struct coreset_rm_pattern {
  uint8_t                coreset_rate_match_pattern_id;
  std::array<uint8_t, 6> freq_domain_resources;
  uint16_t               symbols_pattern;
  subcarrier_spacing     scs;
};

/// Encodes the CORESET rate-match patterns configuration.
struct coreset_rm_pattern_config {
  /// Maximum number of PRB-symbol rate-match patterns.
  static constexpr unsigned MAX_NUM_PATTERNS = 64;

  uint8_t                                          num_patterns;
  std::array<coreset_rm_pattern, MAX_NUM_PATTERNS> patterns;
};

/// Encodes the UL BWP structure.
struct ul_bwp {
  uint8_t  pucch_group_hopping;
  uint16_t n_id_pucch_hopping;
};

/// Encodes the PUCCH semi-static signaling parameters.
struct pucch_semi_static_config {
  /// Maximum number of UL BWP.
  static constexpr unsigned MAX_NUM_UL_BWP = 64;

  uint8_t                            num_ul_bwp_ids;
  std::array<ul_bwp, MAX_NUM_UL_BWP> ul_bwp_id;
};

/// Encodes the PDSCH semi-static parameters.
struct pdsch_semi_static_config {
  uint8_t pdsch_cbg_scheme;
};

/// Encodes the delay management configuration.
struct delay_management_config {
  uint16_t timing_window;
  uint8_t  timing_info_mode;
  uint8_t  timing_info_period;
};

} // namespace fapi
} // namespace srsran
