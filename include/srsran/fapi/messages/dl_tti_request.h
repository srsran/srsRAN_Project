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
#include "srsran/fapi/messages/dl_pdcch_pdu.h"
#include "srsran/fapi/messages/tti_parameters.h"
#include "srsran/ran/csi_rs/csi_rs_types.h"
#include "srsran/ran/csi_rs/frequency_allocation_type.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/pdsch/pdsch_context.h"
#include "srsran/ran/prs/prs.h"
#include "srsran/ran/sch/ldpc_base_graph.h"
#include "srsran/ran/ssb/ssb_properties.h"
#include "srsran/support/units.h"
#include <array>
#include <bitset>

namespace srsran {
namespace fapi {

enum class pdsch_trans_type : uint8_t {
  non_interleaved_common_ss,
  non_interleaved_other,
  interleaved_common_type0_coreset0,
  interleaved_common_any_coreset0_present,
  interleaved_common_any_coreset0_not_present,
  interleaved_other
};

/// \note For this release num_coreset_rm_patterns = 0.
/// \note For this release num_prb_sym_rm_patts_by_value = 0.
struct dl_pdsch_maintenance_parameters_v3 {
  static constexpr unsigned MAX_SIZE_SSB_PDU_FOR_RM = 8U;
  /// Bit position of the first TB inside the tb_crc_required bitmap.
  static constexpr unsigned TB_BITMAP_FIRST_TB_BIT = 0U;
  /// Bit position of the second TB inside the tb_crc_required bitmap.
  static constexpr unsigned TB_BITMAP_SECOND_TB_BIT = 1U;

  pdsch_trans_type                              trans_type;
  uint16_t                                      coreset_start_point;
  uint16_t                                      initial_dl_bwp_size;
  ldpc_base_graph_type                          ldpc_base_graph;
  units::bytes                                  tb_size_lbrm_bytes;
  uint8_t                                       tb_crc_required;
  std::array<uint16_t, MAX_SIZE_SSB_PDU_FOR_RM> ssb_pdus_for_rate_matching;
  uint16_t                                      ssb_config_for_rate_matching;
  uint8_t                                       prb_sym_rm_pattern_bitmap_size_byref;
  //: TODO: determine max size of this array
  static_vector<uint8_t, 16> prb_sym_rm_patt_bmp_byref;
  uint8_t                    num_prb_sym_rm_patts_by_value;
  uint8_t                    num_coreset_rm_patterns;
  uint16_t                   pdcch_pdu_index;
  uint16_t                   dci_index;
  uint8_t                    lte_crs_rm_pattern_bitmap_size;
  //: TODO: determine max size of this array
  static_vector<uint8_t, 16>  lte_crs_rm_pattern;
  static_vector<uint16_t, 16> csi_for_rm;
  uint8_t                     max_num_cbg_per_tb;
  //: TODO: determine max size of this array.
  static_vector<uint8_t, 16> cbg_tx_information;
};

struct dl_pdsch_parameters_v4 {
  uint8_t coreset_rm_pattern_bitmap_size_by_ref;
  //: TODO: determine max size of this array
  static_vector<uint8_t, 16> coreset_rm_pattern_bmp_by_ref;
  uint8_t                    lte_crs_mbsfn_derivation_method;
  // :TODO: determine max size of this array. This size is either 0 or lte_crs_rm_pattern.size() parameter in
  // maintenance v3.
  static_vector<uint8_t, 16> lte_crs_mbsfn_pattern;
  //: TODO: MU-MIMO fields
};

/// Codeword information.
struct dl_pdsch_codeword {
  uint16_t     target_code_rate;
  uint8_t      qam_mod_order;
  uint8_t      mcs_index;
  uint8_t      mcs_table;
  uint8_t      rv_index;
  units::bytes tb_size;
};

enum class inline_tb_crc_type : uint8_t { data_payload, control_message };
enum class pdsch_ref_point_type : uint8_t { point_a, subcarrier_0 };

/// Power control offset SS defined as 'ratio of NZP CSI-RS EPRE to SSB/PBCH block EPRE' as per SCF-222 v4.0
/// section 2.2.4.5.
enum class power_control_offset_ss : uint8_t { dB_minus_3, dB0, dB3, dB6 };

/// Converts the given value to a power control offset SS value.
inline power_control_offset_ss to_power_control_offset_ss(int value)
{
  switch (value) {
    case -3:
      return power_control_offset_ss::dB_minus_3;
    case 0:
      return power_control_offset_ss::dB0;
    case 3:
      return power_control_offset_ss::dB3;
    case 6:
      return power_control_offset_ss::dB6;
    default:
      srsran_assert(0, "Invalid power control offset SS '{}'", value);
      break;
  }
  return power_control_offset_ss::dB0;
}

/// Converts power control offset SS value to an integer.
inline int to_int(power_control_offset_ss value)
{
  switch (value) {
    case power_control_offset_ss::dB_minus_3:
      return -3;
    case power_control_offset_ss::dB0:
      return 0;
    case power_control_offset_ss::dB3:
      return 3;
    case power_control_offset_ss::dB6:
      break;
  }
  return 6;
}

/// Downlink PDSCH PDU information.
struct dl_pdsch_pdu {
  static constexpr unsigned BITMAP_SIZE = 2U;

  /// Bit position of PTRS in the PDU bitmap.
  static constexpr unsigned PDU_BITMAP_PTRS_BIT = 0U;
  /// Bit position of CBG retransmission control in the PDU bitmap.
  static constexpr unsigned PDU_BITMAP_CBG_RETX_CTRL_BIT = 1U;
  /// Bit position of the first TB in the is_last_cb_present bitmap.
  static constexpr unsigned LAST_CB_BITMAP_FIRST_TB_BIT = 0U;
  /// Bit position of the second TB in the is_last_cb_present bitmap.
  static constexpr unsigned LAST_CB_BITMAP_SECOND_TB_BIT = 1U;

  /// Maximum number of codewords per PDU.
  static constexpr unsigned MAX_NUM_CW_PER_PDU = 2;
  /// Maximum size of the RB bitmap in Bytes.
  static constexpr unsigned MAX_SIZE_RB_BITMAP = 36;
  /// Maximum size of DL TB CRC.
  static constexpr unsigned MAX_SIZE_DL_TB_CRC = 2;

  /// Profile NR power parameters.
  struct power_profile_nr {
    int                     power_control_offset_profile_nr;
    power_control_offset_ss power_control_offset_ss_profile_nr;
  };

  /// Profile SSS power parameters.
  struct power_profile_sss {
    float dmrs_power_offset_sss_db;
    float data_power_offset_sss_db;
  };

  std::bitset<BITMAP_SIZE>                             pdu_bitmap;
  rnti_t                                               rnti;
  uint16_t                                             pdu_index;
  uint16_t                                             bwp_size;
  uint16_t                                             bwp_start;
  subcarrier_spacing                                   scs;
  cyclic_prefix                                        cp;
  static_vector<dl_pdsch_codeword, MAX_NUM_CW_PER_PDU> cws;
  uint16_t                                             nid_pdsch;
  uint8_t                                              num_layers;
  uint8_t                                              transmission_scheme;
  pdsch_ref_point_type                                 ref_point;
  uint16_t                                             dl_dmrs_symb_pos;
  uint16_t                                             pdsch_dmrs_scrambling_id;
  dmrs_cfg_type                                        dmrs_type;
  uint16_t                                             pdsch_dmrs_scrambling_id_compl;
  low_papr_dmrs_type                                   low_papr_dmrs;
  uint8_t                                              nscid;
  uint8_t                                              num_dmrs_cdm_grps_no_data;
  uint16_t                                             dmrs_ports;
  resource_allocation_type                             resource_alloc;
  std::array<uint8_t, MAX_SIZE_RB_BITMAP>              rb_bitmap;
  uint16_t                                             rb_start;
  uint16_t                                             rb_size;
  vrb_to_prb_mapping_type                              vrb_to_prb_mapping;
  uint8_t                                              start_symbol_index;
  uint8_t                                              nr_of_symbols;
  std::variant<power_profile_nr, power_profile_sss>    power_config;
  // :TODO: PTRS
  tx_precoding_and_beamforming_pdu         precoding_and_beamforming;
  uint8_t                                  is_last_cb_present;
  inline_tb_crc_type                       is_inline_tb_crc;
  std::array<uint32_t, MAX_SIZE_DL_TB_CRC> dl_tb_crc_cw;
  dl_pdsch_maintenance_parameters_v3       pdsch_maintenance_v3;
  // :TODO: Rel16 PDSCH params v3
  dl_pdsch_parameters_v4 pdsch_parameters_v4;
  /// Vendor specific parameters.
  std::optional<pdsch_context> context;
};

/// CSI-RS maintenance parameters added in FAPIv3.
struct dl_csi_rs_maintenance_v3 {
  uint16_t csi_rs_pdu_index;
};

/// Downlink CSI-RS PDU information.
struct dl_csi_rs_pdu {
  subcarrier_spacing                scs;
  cyclic_prefix                     cp;
  uint16_t                          start_rb;
  uint16_t                          num_rbs;
  csi_rs_type                       type;
  uint8_t                           row;
  csi_rs::freq_allocation_mask_type freq_domain;
  uint8_t                           symb_L0;
  uint8_t                           symb_L1;
  csi_rs_cdm_type                   cdm_type;
  csi_rs_freq_density_type          freq_density;
  uint16_t                          scramb_id;
  int                               power_control_offset_profile_nr;
  power_control_offset_ss           power_control_offset_ss_profile_nr;
  tx_precoding_and_beamforming_pdu  precoding_and_beamforming;
  dl_csi_rs_maintenance_v3          csi_rs_maintenance_v3;
  //: TODO: csi params v4
  // Vendor specific parameters.
  uint16_t bwp_size;
  uint16_t bwp_start;
};

enum class dmrs_typeA_pos : uint8_t { pos2 = 0, pos3 };

/// PHY generated MIB PDU information.
struct dl_ssb_phy_mib_pdu {
  dmrs_typeA_pos dmrs_typeA_position;
  uint8_t        pdcch_config_sib1;
  bool           cell_barred;
  bool           intrafreq_reselection;
};

/// BCH payload information.
struct dl_ssb_bch_payload {
  union {
    uint32_t           bch_payload;
    dl_ssb_phy_mib_pdu phy_mib_pdu;
  };
};

/// SSB/PBCH maintenance parameters added in FAPIv3.
struct dl_ssb_maintenance_v3 {
  uint8_t            ssb_pdu_index;
  ssb_pattern_case   case_type;
  subcarrier_spacing scs;
  uint8_t            L_max;
};

/// PSS EPRE to SSS EPRE in a SS/PBCH block.
enum class beta_pss_profile_type : uint8_t { dB_0 = 0, dB_3 = 1, beta_pss_profile_sss = 255 };

/// BCH payload generation options.
enum class bch_payload_type : uint8_t { mac_full, phy_timing_info, phy_full };

/// Downlink SSB PDU information.
struct dl_ssb_pdu {
  pci_t                            phys_cell_id;
  beta_pss_profile_type            beta_pss_profile_nr;
  uint8_t                          ssb_block_index;
  uint8_t                          ssb_subcarrier_offset;
  ssb_offset_to_pointA             ssb_offset_pointA;
  bch_payload_type                 bch_payload_flag;
  dl_ssb_bch_payload               bch_payload;
  tx_precoding_and_beamforming_pdu precoding_and_beamforming;
  dl_ssb_maintenance_v3            ssb_maintenance_v3;
  //: TODO: params v4 - MU-MIMO
};

/// Downlink PRS PDU information.
struct dl_prs_pdu {
  subcarrier_spacing               scs;
  cyclic_prefix                    cp;
  uint16_t                         nid_prs;
  uint16_t                         pdu_index;
  prs_comb_size                    comb_size;
  uint8_t                          comb_offset;
  prs_num_symbols                  num_symbols;
  uint8_t                          first_symbol;
  uint16_t                         num_rbs;
  uint16_t                         start_rb;
  std::optional<float>             prs_power_offset;
  tx_precoding_and_beamforming_pdu precoding_and_beamforming;
  // :TODO: Puncturing, spatial stream and backward compatible extension.
};

/// Downlink PDU type ID.
enum class dl_pdu_type : uint16_t { PDCCH, PDSCH, CSI_RS, SSB, PRS = 5 };

inline unsigned to_value(dl_pdu_type value)
{
  return static_cast<unsigned>(value);
}

/// Common downlink PDU information.
struct dl_tti_request_pdu {
  dl_pdu_type pdu_type;
  uint16_t    pdu_size;

  // :TODO: add these fields in a std::variant.
  dl_pdcch_pdu  pdcch_pdu;
  dl_pdsch_pdu  pdsch_pdu;
  dl_csi_rs_pdu csi_rs_pdu;
  dl_ssb_pdu    ssb_pdu;
  dl_prs_pdu    prs_pdu;
};

/// Downlink TTI request message.
struct dl_tti_request_message : public base_message {
  /// Array index for the number of DL DCIs.
  static constexpr unsigned DL_DCI_INDEX = 4;
  /// Maximum supported number of DL PDU types in this release.
  static constexpr unsigned MAX_NUM_DL_TYPES = 6;

  uint16_t                                                sfn;
  uint16_t                                                slot;
  std::array<uint16_t, MAX_NUM_DL_TYPES>                  num_pdus_of_each_type = {};
  uint16_t                                                num_groups;
  static_vector<dl_tti_request_pdu, MAX_DL_PDUS_PER_SLOT> pdus;
  //: TODO: groups array
  //: TODO: top level rate match patterns
};

} // namespace fapi
} // namespace srsran
