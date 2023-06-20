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

/*******************************************************************************
 *
 *                    3GPP TS ASN1 RRC NR v17.4.0 (2023-03)
 *
 ******************************************************************************/

#pragma once

#include "dl_dcch_msg.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// SL-BWP-DiscPoolConfigCommon-r17 ::= SEQUENCE
struct sl_bwp_disc_pool_cfg_common_r17_s {
  using sl_disc_rx_pool_r17_l_     = dyn_array<sl_res_pool_r16_s>;
  using sl_disc_tx_pool_sel_r17_l_ = dyn_array<sl_res_pool_cfg_r16_s>;

  // member variables
  bool                       ext = false;
  sl_disc_rx_pool_r17_l_     sl_disc_rx_pool_r17;
  sl_disc_tx_pool_sel_r17_l_ sl_disc_tx_pool_sel_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-BWP-PoolConfigCommon-r16 ::= SEQUENCE
struct sl_bwp_pool_cfg_common_r16_s {
  using sl_rx_pool_r16_l_            = dyn_array<sl_res_pool_r16_s>;
  using sl_tx_pool_sel_normal_r16_l_ = dyn_array<sl_res_pool_cfg_r16_s>;

  // member variables
  bool                         sl_tx_pool_exceptional_r16_present = false;
  sl_rx_pool_r16_l_            sl_rx_pool_r16;
  sl_tx_pool_sel_normal_r16_l_ sl_tx_pool_sel_normal_r16;
  sl_res_pool_cfg_r16_s        sl_tx_pool_exceptional_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-BWP-ConfigCommon-r16 ::= SEQUENCE
struct sl_bwp_cfg_common_r16_s {
  bool                         ext                                = false;
  bool                         sl_bwp_generic_r16_present         = false;
  bool                         sl_bwp_pool_cfg_common_r16_present = false;
  sl_bwp_generic_r16_s         sl_bwp_generic_r16;
  sl_bwp_pool_cfg_common_r16_s sl_bwp_pool_cfg_common_r16;
  // ...
  // group 0
  copy_ptr<sl_bwp_pool_cfg_common_r16_s>      sl_bwp_pool_cfg_common_ps_r17;
  copy_ptr<sl_bwp_disc_pool_cfg_common_r17_s> sl_bwp_disc_pool_cfg_common_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-EUTRA-AnchorCarrierFreqList-r16 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (0..262143)
using sl_eutra_anchor_carrier_freq_list_r16_l = bounded_array<uint32_t, 8>;

// SL-FreqConfigCommon-r16 ::= SEQUENCE
struct sl_freq_cfg_common_r16_s {
  using sl_scs_specific_carrier_list_r16_l_ = dyn_array<scs_specific_carrier_s>;
  using sl_bwp_list_r16_l_                  = dyn_array<sl_bwp_cfg_common_r16_s>;
  struct sl_sync_prio_r16_opts {
    enum options { gnss, gnb_enb, nulltype } value;

    const char* to_string() const;
  };
  using sl_sync_prio_r16_e_ = enumerated<sl_sync_prio_r16_opts>;

  // member variables
  bool                                ext                              = false;
  bool                                sl_absolute_freq_ssb_r16_present = false;
  bool                                freq_shift7p5khz_sl_r16_present  = false;
  bool                                sl_sync_prio_r16_present         = false;
  bool                                sl_nb_as_sync_r16_present        = false;
  sl_scs_specific_carrier_list_r16_l_ sl_scs_specific_carrier_list_r16;
  uint32_t                            sl_absolute_freq_point_a_r16 = 0;
  uint32_t                            sl_absolute_freq_ssb_r16     = 0;
  int8_t                              value_n_r16                  = -1;
  sl_bwp_list_r16_l_                  sl_bwp_list_r16;
  sl_sync_prio_r16_e_                 sl_sync_prio_r16;
  bool                                sl_nb_as_sync_r16 = false;
  sl_sync_cfg_list_r16_l              sl_sync_cfg_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-MeasConfigCommon-r16 ::= SEQUENCE
struct sl_meas_cfg_common_r16_s {
  bool                     ext                             = false;
  bool                     sl_quant_cfg_common_r16_present = false;
  sl_meas_obj_list_r16_l   sl_meas_obj_list_common_r16;
  sl_report_cfg_list_r16_l sl_report_cfg_list_common_r16;
  sl_meas_id_list_r16_l    sl_meas_id_list_common_r16;
  sl_quant_cfg_r16_s       sl_quant_cfg_common_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-NR-AnchorCarrierFreqList-r16 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (0..3279165)
using sl_nr_anchor_carrier_freq_list_r16_l = bounded_array<uint32_t, 8>;

// SL-PreconfigGeneral-r16 ::= SEQUENCE
struct sl_precfg_general_r16_s {
  bool                   ext                       = false;
  bool                   sl_tdd_cfg_r16_present    = false;
  bool                   reserved_bits_r16_present = false;
  tdd_ul_dl_cfg_common_s sl_tdd_cfg_r16;
  fixed_bitstring<2>     reserved_bits_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TxProfile-r17 ::= ENUMERATED
struct sl_tx_profile_r17_opts {
  enum options { drx_compatible, drx_incompatible, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;

  const char* to_string() const;
};
using sl_tx_profile_r17_e = enumerated<sl_tx_profile_r17_opts>;

// SL-RoHC-Profiles-r16 ::= SEQUENCE
struct sl_ro_hc_profiles_r16_s {
  bool profile0x0001_r16 = false;
  bool profile0x0002_r16 = false;
  bool profile0x0003_r16 = false;
  bool profile0x0004_r16 = false;
  bool profile0x0006_r16 = false;
  bool profile0x0101_r16 = false;
  bool profile0x0102_r16 = false;
  bool profile0x0103_r16 = false;
  bool profile0x0104_r16 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TxProfileList-r17 ::= SEQUENCE (SIZE (1..256)) OF SL-TxProfile-r17
using sl_tx_profile_list_r17_l = dyn_array<sl_tx_profile_r17_e>;

// SidelinkPreconfigNR-r16 ::= SEQUENCE
struct sidelink_precfg_nr_r16_s {
  using sl_precfg_freq_info_list_r16_l_     = dyn_array<sl_freq_cfg_common_r16_s>;
  using sl_radio_bearer_pre_cfg_list_r16_l_ = dyn_array<sl_radio_bearer_cfg_r16_s>;
  using sl_rlc_bearer_pre_cfg_list_r16_l_   = dyn_array<sl_rlc_bearer_cfg_r16_s>;
  struct t400_r16_opts {
    enum options { ms100, ms200, ms300, ms400, ms600, ms1000, ms1500, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using t400_r16_e_ = enumerated<t400_r16_opts>;
  struct sl_max_num_consecutive_dtx_r16_opts {
    enum options { n1, n2, n3, n4, n6, n8, n16, n32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using sl_max_num_consecutive_dtx_r16_e_ = enumerated<sl_max_num_consecutive_dtx_r16_opts>;

  // member variables
  bool                                    ext                                    = false;
  bool                                    sl_meas_pre_cfg_r16_present            = false;
  bool                                    sl_offset_dfn_r16_present              = false;
  bool                                    t400_r16_present                       = false;
  bool                                    sl_max_num_consecutive_dtx_r16_present = false;
  bool                                    sl_ssb_prio_nr_r16_present             = false;
  bool                                    sl_precfg_general_r16_present          = false;
  bool                                    sl_ue_sel_pre_cfg_r16_present          = false;
  bool                                    sl_csi_acquisition_r16_present         = false;
  bool                                    sl_ro_hc_profiles_r16_present          = false;
  bool                                    sl_max_c_id_r16_present                = false;
  sl_precfg_freq_info_list_r16_l_         sl_precfg_freq_info_list_r16;
  sl_nr_anchor_carrier_freq_list_r16_l    sl_precfg_nr_anchor_carrier_freq_list_r16;
  sl_eutra_anchor_carrier_freq_list_r16_l sl_precfg_eutra_anchor_carrier_freq_list_r16;
  sl_radio_bearer_pre_cfg_list_r16_l_     sl_radio_bearer_pre_cfg_list_r16;
  sl_rlc_bearer_pre_cfg_list_r16_l_       sl_rlc_bearer_pre_cfg_list_r16;
  sl_meas_cfg_common_r16_s                sl_meas_pre_cfg_r16;
  uint16_t                                sl_offset_dfn_r16 = 1;
  t400_r16_e_                             t400_r16;
  sl_max_num_consecutive_dtx_r16_e_       sl_max_num_consecutive_dtx_r16;
  uint8_t                                 sl_ssb_prio_nr_r16 = 1;
  sl_precfg_general_r16_s                 sl_precfg_general_r16;
  sl_ue_sel_cfg_r16_s                     sl_ue_sel_pre_cfg_r16;
  sl_ro_hc_profiles_r16_s                 sl_ro_hc_profiles_r16;
  uint16_t                                sl_max_c_id_r16 = 1;
  // ...
  // group 0
  copy_ptr<sl_drx_cfg_gc_bc_r17_s>   sl_drx_pre_cfg_gc_bc_r17;
  copy_ptr<sl_tx_profile_list_r17_l> sl_tx_profile_list_r17;
  copy_ptr<sl_remote_ue_cfg_r17_s>   sl_precfg_disc_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PreconfigurationNR-r16 ::= SEQUENCE
struct sl_precfg_nr_r16_s {
  bool                     ext = false;
  sidelink_precfg_nr_r16_s sidelink_precfg_nr_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1
