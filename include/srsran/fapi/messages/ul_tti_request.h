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
#include "srsran/fapi/messages/base_message.h"
#include "srsran/fapi/messages/srs_pdu_report_type.h"
#include "srsran/fapi/messages/tti_parameters.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/prach/prach_format_type.h"
#include "srsran/ran/pucch/pucch_mapping.h"
#include "srsran/ran/pusch/pusch_context.h"
#include "srsran/ran/pusch/pusch_mcs.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/sch/ldpc_base_graph.h"
#include "srsran/ran/sch/modulation_scheme.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/ran/srs/srs_configuration.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/ran/uci/uci_configuration.h"
#include "srsran/support/units.h"
#include <array>
#include <bitset>

namespace srsran {
namespace fapi {

/// Uplink PDU type ID.
enum class ul_pdu_type : uint16_t { PRACH, PUSCH, PUCCH, SRS, msg_a_PUSCH };

enum class prach_config_scope_type : uint8_t { common_context, phy_context };

/// PRACH maintenance parameters added in FAPIv3.
struct ul_prach_maintenance_v3 {
  uint32_t                handle;
  prach_config_scope_type prach_config_scope;
  uint16_t                prach_res_config_index;
  uint8_t                 num_fd_ra;
  uint8_t                 start_preamble_index;
  uint8_t                 num_preamble_indices;
};

/// Uplink PRACH PDU information.
struct ul_prach_pdu {
  pci_t             phys_cell_id;
  uint8_t           num_prach_ocas;
  prach_format_type prach_format;
  uint8_t           index_fd_ra;
  uint8_t           prach_start_symbol;
  uint16_t          num_cs;
  uint8_t           is_msg_a_prach;
  bool              has_msg_a_pusch_beamforming;
  //: TODO: beamforming struct
  ul_prach_maintenance_v3 maintenance_v3;
  //: TODO: uplink spatial assignment struct
  //: TODO: msgA signalling in v4
  //: TODO: msgA pusch beamforming
};

/// Uplink PUSCH data information.
struct ul_pusch_data {
  /// Maximum number of CB.
  //: TODO: determine size of this array
  static constexpr unsigned MAX_NUM_CB = 128;

  uint8_t                            rv_index;
  uint8_t                            harq_process_id;
  bool                               new_data;
  units::bytes                       tb_size;
  uint16_t                           num_cb;
  static_vector<uint8_t, MAX_NUM_CB> cb_present_and_position;
};

/// Uplink PUSCH UCI information.
struct ul_pusch_uci {
  uint16_t          harq_ack_bit_length;
  uint16_t          csi_part1_bit_length;
  uint16_t          flags_csi_part2;
  alpha_scaling_opt alpha_scaling;
  uint8_t           beta_offset_harq_ack;
  uint8_t           beta_offset_csi1;
  uint8_t           beta_offset_csi2;
};

enum class ul_ptrs_power_type : uint8_t { dB0, dB3, dB4_77, dB6 };

/// Uplink PUSCH PTRS information.
struct ul_pusch_ptrs {
  /// Per-port specific information.
  struct ptrs_port_info {
    uint16_t ptrs_port_index;
    uint8_t  ptrs_dmrs_port;
    uint8_t  ptrs_re_offset;
  };

  static_vector<ptrs_port_info, 2> port_info;
  uint8_t                          ptrs_time_density;
  uint8_t                          ptrs_freq_density;
  ul_ptrs_power_type               ul_ptrs_power;
};

/// Uplink PUSCH DFTs-OFDM information.
struct ul_pusch_dfts_ofdm {
  uint8_t  low_papr_group_number;
  uint16_t low_papr_sequence_number;
  uint8_t  ul_ptrs_sample_density;
  uint8_t  ul_ptrs_time_density_transform_precoding;
};

/// PUSCH PDU maintenance information added in FAPIv3.
struct ul_pusch_maintenance_v3 {
  uint8_t              pusch_trans_type;
  uint16_t             delta_bwp0_start_from_active_bwp;
  uint16_t             initial_ul_bwp_size;
  uint8_t              group_or_sequence_hopping;
  uint16_t             pusch_second_hop_prb;
  ldpc_base_graph_type ldpc_base_graph;
  units::bytes         tb_size_lbrm_bytes;
};

/// PUSCH PDU parameters added in FAPIv4.
struct ul_pusch_params_v4 {
  /// Maximum number of spatial streams.
  static constexpr unsigned MAX_NUM_SPATIAL_STREAMS = 64;

  bool                                         cb_crc_status_request;
  uint32_t                                     srs_tx_ports;
  uint8_t                                      ul_tpmi_index;
  uint8_t                                      num_ul_spatial_streams_ports;
  std::array<uint8_t, MAX_NUM_SPATIAL_STREAMS> ul_spatial_stream_ports;
};

/// UCI information for determining UCI Part1 to Part2 correspondence.
struct uci_part1_to_part2_correspondence_v3 {
  /// Maximum number of part2 info.
  static constexpr unsigned MAX_NUM_PART2_INFO = 100;

  enum class map_scope_type : uint8_t { common_context, phy_context };

  struct part2_info {
    uint16_t                   priority;
    static_vector<uint16_t, 4> param_offsets;
    static_vector<uint8_t, 4>  param_sizes;
    uint16_t                   part2_size_map_index;
    map_scope_type             part2_size_map_scope;
  };

  static_vector<part2_info, MAX_NUM_PART2_INFO> part2;
};

/// Uplink PUSCH PDU information.
struct ul_pusch_pdu {
  static constexpr unsigned RB_BITMAP_SIZE_IN_BYTES = 36U;
  static constexpr unsigned BITMAP_SIZE             = 4U;
  /// Bit position of the pdu_bitmap property.
  static constexpr unsigned PUSCH_DATA_BIT = 0U;
  static constexpr unsigned PUSCH_UCI_BIT  = 1U;
  static constexpr unsigned PUSCH_PTRS_BIT = 2U;
  static constexpr unsigned DFTS_OFDM_BIT  = 3U;

  std::bitset<BITMAP_SIZE>                     pdu_bitmap;
  rnti_t                                       rnti;
  uint32_t                                     handle;
  uint16_t                                     bwp_size;
  uint16_t                                     bwp_start;
  subcarrier_spacing                           scs;
  cyclic_prefix                                cp;
  uint16_t                                     target_code_rate;
  modulation_scheme                            qam_mod_order;
  uint8_t                                      mcs_index;
  pusch_mcs_table                              mcs_table;
  bool                                         transform_precoding;
  uint16_t                                     nid_pusch;
  uint8_t                                      num_layers;
  uint16_t                                     ul_dmrs_symb_pos;
  dmrs_cfg_type                                dmrs_type;
  uint16_t                                     pusch_dmrs_scrambling_id;
  uint16_t                                     pusch_dmrs_scrambling_id_complement;
  low_papr_dmrs_type                           low_papr_dmrs;
  uint16_t                                     pusch_dmrs_identity;
  uint8_t                                      nscid;
  uint8_t                                      num_dmrs_cdm_grps_no_data;
  uint16_t                                     dmrs_ports;
  resource_allocation_type                     resource_alloc;
  std::array<uint8_t, RB_BITMAP_SIZE_IN_BYTES> rb_bitmap;
  uint16_t                                     rb_start;
  uint16_t                                     rb_size;
  vrb_to_prb_mapping_type                      vrb_to_prb_mapping;
  bool                                         intra_slot_frequency_hopping;
  uint16_t                                     tx_direct_current_location;
  bool                                         uplink_frequency_shift_7p5kHz;
  uint8_t                                      start_symbol_index;
  uint8_t                                      nr_of_symbols;
  ul_pusch_data                                pusch_data;
  ul_pusch_uci                                 pusch_uci;
  ul_pusch_ptrs                                pusch_ptrs;
  ul_pusch_dfts_ofdm                           pusch_ofdm;
  //: TODO: beamforming struct
  ul_pusch_maintenance_v3              pusch_maintenance_v3;
  ul_pusch_params_v4                   pusch_params_v4;
  uci_part1_to_part2_correspondence_v3 uci_correspondence;
  // Vendor specific parameters.
  std::optional<pusch_context> context;
};

/// PUCCH PDU maintenance information added in FAPIv3.
struct ul_pucch_maintenance_v3 {
  uint8_t max_code_rate;
  uint8_t ul_bwp_id;
};

/// Encodes PUCCH pdu.
struct ul_pucch_pdu {
  rnti_t                   rnti;
  uint32_t                 handle;
  uint16_t                 bwp_size;
  uint16_t                 bwp_start;
  subcarrier_spacing       scs;
  cyclic_prefix            cp;
  pucch_format             format_type;
  pucch_repetition_tx_slot multi_slot_tx_indicator;
  bool                     pi2_bpsk;
  uint16_t                 prb_start;
  uint16_t                 prb_size;
  uint8_t                  start_symbol_index;
  uint8_t                  nr_of_symbols;
  bool                     intra_slot_frequency_hopping;
  uint16_t                 second_hop_prb;
  pucch_group_hopping      pucch_grp_hopping;
  uint8_t                  reserved;
  uint16_t                 nid_pucch_hopping;
  uint16_t                 initial_cyclic_shift;
  uint16_t                 nid_pucch_scrambling;
  uint8_t                  time_domain_occ_index;
  uint8_t                  pre_dft_occ_idx;
  uint8_t                  pre_dft_occ_len;
  bool                     add_dmrs_flag;
  uint16_t                 nid0_pucch_dmrs_scrambling;
  uint8_t                  m0_pucch_dmrs_cyclic_shift;
  uint8_t                  sr_bit_len;
  uint16_t                 bit_len_harq;
  uint16_t                 csi_part1_bit_length;
  //: TODO: beamforming struct
  ul_pucch_maintenance_v3              pucch_maintenance_v3;
  uci_part1_to_part2_correspondence_v3 uci_correspondence;
};

/// MsgA-PUSCH PDU.
struct ul_msg_a_pusch_pdu {
  uint16_t                pdu_bitmap;
  uint8_t                 prach_to_pru_map_type;
  uint32_t                prach_to_pru_map_indicator;
  uint16_t                msg_a_prach_sfn;
  uint16_t                msg_a_prach_slot;
  uint32_t                handle;
  uint16_t                bwp_size;
  uint16_t                bwp_start;
  subcarrier_spacing      scs;
  cyclic_prefix           cp;
  uint8_t                 msg_a_mcs;
  uint8_t                 transform_precoding;
  uint8_t                 n_id_msg_a_pusch;
  std::array<uint16_t, 2> pusch_dmrs_scrambling_ids;
  uint16_t                ul_dmrs_symb_pos;
  uint8_t                 dmrs_mapping_type;
  uint16_t                start_rb_pusch_ocas;
  uint8_t                 num_rb_pusch_ocas;
  uint8_t                 guard_band;
  uint8_t                 intra_slot_frequency_hopping;
  uint8_t                 intra_slot_frequency_hopping_bits;
  uint16_t                tx_direct_current_location;
  uint8_t                 uplink_frequency_shift_7p5kHz;
  uint8_t                 start_symb_id_pusch_ocas;
  uint8_t                 duration_pusch_ocas;
  uint8_t                 guard_period;
  uint32_t                tb_size;
};

/// SRS parameters in FAPIv4.
struct ul_srs_params_v4 {
  /// Maximum number of UL spatial stream ports.
  static constexpr unsigned MAX_NUM_UL_SPATIAL_STREAM_PORTS = 64;

  struct symbol_info {
    uint16_t srs_bandwidth_start;
    uint8_t  sequence_group;
    uint8_t  sequence_number;
  };

  uint16_t                                             srs_bandwidth_size;
  std::array<symbol_info, 4>                           sym_info;
  uint32_t                                             usage;
  std::bitset<to_value(srs_report_type::last)>         report_type;
  uint8_t                                              singular_value_representation;
  uint8_t                                              iq_representation;
  uint16_t                                             prg_size;
  uint8_t                                              num_total_ue_antennas;
  uint32_t                                             ue_antennas_in_this_srs_resource_set;
  uint32_t                                             sampled_ue_antennas;
  uint8_t                                              report_scope;
  uint8_t                                              num_ul_spatial_streams_ports;
  std::array<uint8_t, MAX_NUM_UL_SPATIAL_STREAM_PORTS> ul_spatial_stream_ports;
};

/// SRS PDU.
struct ul_srs_pdu {
  rnti_t                        rnti;
  uint32_t                      handle;
  uint16_t                      bwp_size;
  uint16_t                      bwp_start;
  subcarrier_spacing            scs;
  cyclic_prefix                 cp;
  uint8_t                       num_ant_ports;
  uint8_t                       num_symbols;
  srs_nof_symbols               num_repetitions;
  uint8_t                       time_start_position;
  uint8_t                       config_index;
  uint16_t                      sequence_id;
  uint8_t                       bandwidth_index;
  tx_comb_size                  comb_size;
  uint8_t                       comb_offset;
  uint8_t                       cyclic_shift;
  uint8_t                       frequency_position;
  uint16_t                      frequency_shift;
  uint8_t                       frequency_hopping;
  srs_group_or_sequence_hopping group_or_sequence_hopping;
  srs_resource_type             resource_type;
  srs_periodicity               t_srs;
  uint16_t                      t_offset;
  // :TODO: beamforming.
  ul_srs_params_v4 srs_params_v4;
};

/// Common uplink PDU information.
struct ul_tti_request_pdu {
  ul_pdu_type pdu_type;
  uint16_t    pdu_size;

  // :TODO: add variant for the PDUs below.
  ul_prach_pdu       prach_pdu;
  ul_pusch_pdu       pusch_pdu;
  ul_pucch_pdu       pucch_pdu;
  ul_msg_a_pusch_pdu msg_a_pusch_pdu;
  ul_srs_pdu         srs_pdu;
};

/// Uplink TTI request message.
struct ul_tti_request_message : public base_message {
  enum class pdu_type : uint8_t { PRACH, PUSCH, PUCCH_format01, PUCCH_format234, SRS, msga_PUSCH };

  /// Maximum number of supported UL PDU types in this release.
  static constexpr unsigned MAX_NUM_UL_TYPES = 6;

  uint16_t                                                sfn;
  uint16_t                                                slot;
  std::array<uint16_t, MAX_NUM_UL_TYPES>                  num_pdus_of_each_type;
  uint16_t                                                num_groups;
  static_vector<ul_tti_request_pdu, MAX_UL_PDUS_PER_SLOT> pdus;
  //: TODO: groups array
};

} // namespace fapi
} // namespace srsran
