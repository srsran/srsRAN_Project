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

/*******************************************************************************
 *
 *                    3GPP TS ASN1 RRC NR v17.4.0 (2023-03)
 *
 ******************************************************************************/

#pragma once

#include "common.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// MsgA-DMRS-Config-r16 ::= SEQUENCE
struct msg_a_dmrs_cfg_r16_s {
  struct msg_a_dmrs_add_position_r16_opts {
    enum options { pos0, pos1, pos3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using msg_a_dmrs_add_position_r16_e_ = enumerated<msg_a_dmrs_add_position_r16_opts>;

  // member variables
  bool                           msg_a_dmrs_add_position_r16_present    = false;
  bool                           msg_a_max_len_r16_present              = false;
  bool                           msg_a_pusch_dmrs_cdm_group_r16_present = false;
  bool                           msg_a_pusch_nrof_ports_r16_present     = false;
  bool                           msg_a_scrambling_id0_r16_present       = false;
  bool                           msg_a_scrambling_id1_r16_present       = false;
  msg_a_dmrs_add_position_r16_e_ msg_a_dmrs_add_position_r16;
  uint8_t                        msg_a_pusch_dmrs_cdm_group_r16 = 0;
  uint8_t                        msg_a_pusch_nrof_ports_r16     = 0;
  uint32_t                       msg_a_scrambling_id0_r16       = 0;
  uint32_t                       msg_a_scrambling_id1_r16       = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RA-Prioritization ::= SEQUENCE
struct ra_prioritization_s {
  struct pwr_ramp_step_high_prio_opts {
    enum options { db0, db2, db4, db6, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using pwr_ramp_step_high_prio_e_ = enumerated<pwr_ramp_step_high_prio_opts>;
  struct scaling_factor_bi_opts {
    enum options { zero, dot25, dot5, dot75, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using scaling_factor_bi_e_ = enumerated<scaling_factor_bi_opts>;

  // member variables
  bool                       ext                       = false;
  bool                       scaling_factor_bi_present = false;
  pwr_ramp_step_high_prio_e_ pwr_ramp_step_high_prio;
  scaling_factor_bi_e_       scaling_factor_bi;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MsgA-PUSCH-Resource-r16 ::= SEQUENCE
struct msg_a_pusch_res_r16_s {
  struct nrof_msg_a_po_per_slot_r16_opts {
    enum options { one, two, three, six, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nrof_msg_a_po_per_slot_r16_e_ = enumerated<nrof_msg_a_po_per_slot_r16_opts>;
  struct map_type_msg_a_pusch_r16_opts {
    enum options { type_a, type_b, nulltype } value;

    const char* to_string() const;
  };
  using map_type_msg_a_pusch_r16_e_ = enumerated<map_type_msg_a_pusch_r16_opts>;
  struct nrof_msg_a_po_fdm_r16_opts {
    enum options { one, two, four, eight, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nrof_msg_a_po_fdm_r16_e_ = enumerated<nrof_msg_a_po_fdm_r16_opts>;
  struct msg_a_alpha_r16_opts {
    enum options { alpha0, alpha04, alpha05, alpha06, alpha07, alpha08, alpha09, alpha1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using msg_a_alpha_r16_e_ = enumerated<msg_a_alpha_r16_opts>;

  // member variables
  bool                          ext                                            = false;
  bool                          msg_a_pusch_time_domain_alloc_r16_present      = false;
  bool                          start_symbol_and_len_msg_a_po_r16_present      = false;
  bool                          map_type_msg_a_pusch_r16_present               = false;
  bool                          guard_period_msg_a_pusch_r16_present           = false;
  bool                          msg_a_intra_slot_freq_hop_r16_present          = false;
  bool                          msg_a_hop_bits_r16_present                     = false;
  bool                          msg_a_alpha_r16_present                        = false;
  bool                          interlace_idx_first_po_msg_a_pusch_r16_present = false;
  bool                          nrof_interlaces_per_msg_a_po_r16_present       = false;
  uint8_t                       msg_a_mcs_r16                                  = 0;
  uint8_t                       nrof_slots_msg_a_pusch_r16                     = 1;
  nrof_msg_a_po_per_slot_r16_e_ nrof_msg_a_po_per_slot_r16;
  uint8_t                       msg_a_pusch_time_domain_offset_r16 = 1;
  uint8_t                       msg_a_pusch_time_domain_alloc_r16  = 1;
  uint8_t                       start_symbol_and_len_msg_a_po_r16  = 0;
  map_type_msg_a_pusch_r16_e_   map_type_msg_a_pusch_r16;
  uint8_t                       guard_period_msg_a_pusch_r16 = 0;
  uint8_t                       guard_band_msg_a_pusch_r16   = 0;
  uint16_t                      freq_start_msg_a_pusch_r16   = 0;
  uint8_t                       nrof_prbs_per_msg_a_po_r16   = 1;
  nrof_msg_a_po_fdm_r16_e_      nrof_msg_a_po_fdm_r16;
  fixed_bitstring<2>            msg_a_hop_bits_r16;
  msg_a_dmrs_cfg_r16_s          msg_a_dmrs_cfg_r16;
  uint8_t                       nrof_dmrs_seqs_r16 = 1;
  msg_a_alpha_r16_e_            msg_a_alpha_r16;
  uint8_t                       interlace_idx_first_po_msg_a_pusch_r16 = 1;
  uint8_t                       nrof_interlaces_per_msg_a_po_r16       = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NSAG-List-r17 ::= SEQUENCE (SIZE (1..8)) OF BIT STRING (SIZE (8))
using nsag_list_r17_l = bounded_array<fixed_bitstring<8>, 8>;

// RA-PrioritizationSliceInfo-r17 ::= SEQUENCE
struct ra_prioritization_slice_info_r17_s {
  using nsag_id_list_r17_l_ = bounded_array<fixed_bitstring<8>, 8>;

  // member variables
  bool                ext = false;
  nsag_id_list_r17_l_ nsag_id_list_r17;
  ra_prioritization_s ra_prioritization_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureCombination-r17 ::= SEQUENCE
struct feature_combination_r17_s {
  bool            red_cap_r17_present      = false;
  bool            small_data_r17_present   = false;
  bool            msg3_repeats_r17_present = false;
  bool            spare4_present           = false;
  bool            spare3_present           = false;
  bool            spare2_present           = false;
  bool            spare1_present           = false;
  nsag_list_r17_l nsag_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MsgA-PUSCH-Config-r16 ::= SEQUENCE
struct msg_a_pusch_cfg_r16_s {
  struct msg_a_transform_precoder_r16_opts {
    enum options { enabled, disabled, nulltype } value;

    const char* to_string() const;
  };
  using msg_a_transform_precoder_r16_e_ = enumerated<msg_a_transform_precoder_r16_opts>;

  // member variables
  bool                            msg_a_pusch_res_group_a_r16_present   = false;
  bool                            msg_a_pusch_res_group_b_r16_present   = false;
  bool                            msg_a_transform_precoder_r16_present  = false;
  bool                            msg_a_data_scrambling_idx_r16_present = false;
  bool                            msg_a_delta_preamb_r16_present        = false;
  msg_a_pusch_res_r16_s           msg_a_pusch_res_group_a_r16;
  msg_a_pusch_res_r16_s           msg_a_pusch_res_group_b_r16;
  msg_a_transform_precoder_r16_e_ msg_a_transform_precoder_r16;
  uint16_t                        msg_a_data_scrambling_idx_r16 = 0;
  int8_t                          msg_a_delta_preamb_r16        = -1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RA-PrioritizationSliceInfoList-r17 ::= SEQUENCE (SIZE (1..8)) OF RA-PrioritizationSliceInfo-r17
using ra_prioritization_slice_info_list_r17_l = dyn_array<ra_prioritization_slice_info_r17_s>;

// FeatureCombinationPreambles-r17 ::= SEQUENCE
struct feature_combination_preambs_r17_s {
  struct group_bcfg_r17_s_ {
    struct ra_size_group_a_r17_opts {
      enum options {
        b56,
        b144,
        b208,
        b256,
        b282,
        b480,
        b640,
        b800,
        b1000,
        b72,
        spare6,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using ra_size_group_a_r17_e_ = enumerated<ra_size_group_a_r17_opts>;
    struct msg_pwr_offset_group_b_r17_opts {
      enum options { minusinfinity, db0, db5, db8, db10, db12, db15, db18, nulltype } value;
      typedef int8_t number_type;

      const char* to_string() const;
      int8_t      to_number() const;
    };
    using msg_pwr_offset_group_b_r17_e_ = enumerated<msg_pwr_offset_group_b_r17_opts>;

    // member variables
    ra_size_group_a_r17_e_        ra_size_group_a_r17;
    msg_pwr_offset_group_b_r17_e_ msg_pwr_offset_group_b_r17;
    uint8_t                       nof_ra_preambs_group_a_r17 = 1;
  };

  // member variables
  bool                      ext                                  = false;
  bool                      ssb_shared_ro_mask_idx_r17_present   = false;
  bool                      group_bcfg_r17_present               = false;
  bool                      separate_msg_a_pusch_cfg_r17_present = false;
  bool                      msg_a_rsrp_thres_r17_present         = false;
  bool                      rsrp_thres_ssb_r17_present           = false;
  bool                      delta_preamb_r17_present             = false;
  feature_combination_r17_s feature_combination_r17;
  uint8_t                   start_preamb_for_this_partition_r17        = 0;
  uint8_t                   nof_preambs_per_ssb_for_this_partition_r17 = 1;
  uint8_t                   ssb_shared_ro_mask_idx_r17                 = 1;
  group_bcfg_r17_s_         group_bcfg_r17;
  msg_a_pusch_cfg_r16_s     separate_msg_a_pusch_cfg_r17;
  uint8_t                   msg_a_rsrp_thres_r17 = 0;
  uint8_t                   rsrp_thres_ssb_r17   = 0;
  int8_t                    delta_preamb_r17     = -1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GroupB-ConfiguredTwoStepRA-r16 ::= SEQUENCE
struct group_b_cfg_two_step_ra_r16_s {
  struct ra_msg_a_size_group_a_opts {
    enum options {
      b56,
      b144,
      b208,
      b256,
      b282,
      b480,
      b640,
      b800,
      b1000,
      b72,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using ra_msg_a_size_group_a_e_ = enumerated<ra_msg_a_size_group_a_opts>;
  struct msg_pwr_offset_group_b_opts {
    enum options { minusinfinity, db0, db5, db8, db10, db12, db15, db18, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using msg_pwr_offset_group_b_e_ = enumerated<msg_pwr_offset_group_b_opts>;

  // member variables
  ra_msg_a_size_group_a_e_  ra_msg_a_size_group_a;
  msg_pwr_offset_group_b_e_ msg_pwr_offset_group_b;
  uint8_t                   nof_ra_preambs_group_a = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-NS-PmaxValue ::= SEQUENCE
struct nr_ns_pmax_value_s {
  bool    add_pmax_present  = false;
  int8_t  add_pmax          = -30;
  uint8_t add_spec_emission = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RA-PrioritizationForSlicing-r17 ::= SEQUENCE
struct ra_prioritization_for_slicing_r17_s {
  bool                                    ext = false;
  ra_prioritization_slice_info_list_r17_l ra_prioritization_slice_info_list_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RACH-ConfigGenericTwoStepRA-r16 ::= SEQUENCE
struct rach_cfg_generic_two_step_ra_r16_s {
  struct msg_a_ro_fdm_r16_opts {
    enum options { one, two, four, eight, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using msg_a_ro_fdm_r16_e_ = enumerated<msg_a_ro_fdm_r16_opts>;
  struct msg_a_preamb_pwr_ramp_step_r16_opts {
    enum options { db0, db2, db4, db6, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using msg_a_preamb_pwr_ramp_step_r16_e_ = enumerated<msg_a_preamb_pwr_ramp_step_r16_opts>;
  struct msg_b_resp_win_r16_opts {
    enum options { sl1, sl2, sl4, sl8, sl10, sl20, sl40, sl80, sl160, sl320, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using msg_b_resp_win_r16_e_ = enumerated<msg_b_resp_win_r16_opts>;
  struct preamb_trans_max_r16_opts {
    enum options { n3, n4, n5, n6, n7, n8, n10, n20, n50, n100, n200, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using preamb_trans_max_r16_e_ = enumerated<preamb_trans_max_r16_opts>;
  struct msg_b_resp_win_v1700_opts {
    enum options { sl240, sl640, sl960, sl1280, sl1920, sl2560, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using msg_b_resp_win_v1700_e_ = enumerated<msg_b_resp_win_v1700_opts>;

  // member variables
  bool                              ext                                         = false;
  bool                              msg_a_prach_cfg_idx_r16_present             = false;
  bool                              msg_a_ro_fdm_r16_present                    = false;
  bool                              msg_a_ro_freq_start_r16_present             = false;
  bool                              msg_a_zero_correlation_zone_cfg_r16_present = false;
  bool                              msg_a_preamb_pwr_ramp_step_r16_present      = false;
  bool                              msg_a_preamb_rx_target_pwr_r16_present      = false;
  bool                              msg_b_resp_win_r16_present                  = false;
  bool                              preamb_trans_max_r16_present                = false;
  uint16_t                          msg_a_prach_cfg_idx_r16                     = 0;
  msg_a_ro_fdm_r16_e_               msg_a_ro_fdm_r16;
  uint16_t                          msg_a_ro_freq_start_r16             = 0;
  uint8_t                           msg_a_zero_correlation_zone_cfg_r16 = 0;
  msg_a_preamb_pwr_ramp_step_r16_e_ msg_a_preamb_pwr_ramp_step_r16;
  int16_t                           msg_a_preamb_rx_target_pwr_r16 = -202;
  msg_b_resp_win_r16_e_             msg_b_resp_win_r16;
  preamb_trans_max_r16_e_           preamb_trans_max_r16;
  // ...
  // group 0
  bool                    msg_b_resp_win_v1700_present = false;
  msg_b_resp_win_v1700_e_ msg_b_resp_win_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-MBSFN-SubframeConfig ::= SEQUENCE
struct eutra_mbsfn_sf_cfg_s {
  struct radioframe_alloc_period_opts {
    enum options { n1, n2, n4, n8, n16, n32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using radioframe_alloc_period_e_ = enumerated<radioframe_alloc_period_opts>;
  struct sf_alloc1_c_ {
    struct types_opts {
      enum options { one_frame, four_frames, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    sf_alloc1_c_() = default;
    sf_alloc1_c_(const sf_alloc1_c_& other);
    sf_alloc1_c_& operator=(const sf_alloc1_c_& other);
    ~sf_alloc1_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<6>& one_frame()
    {
      assert_choice_type(types::one_frame, type_, "subframeAllocation1");
      return c.get<fixed_bitstring<6>>();
    }
    fixed_bitstring<24>& four_frames()
    {
      assert_choice_type(types::four_frames, type_, "subframeAllocation1");
      return c.get<fixed_bitstring<24>>();
    }
    const fixed_bitstring<6>& one_frame() const
    {
      assert_choice_type(types::one_frame, type_, "subframeAllocation1");
      return c.get<fixed_bitstring<6>>();
    }
    const fixed_bitstring<24>& four_frames() const
    {
      assert_choice_type(types::four_frames, type_, "subframeAllocation1");
      return c.get<fixed_bitstring<24>>();
    }
    fixed_bitstring<6>&  set_one_frame();
    fixed_bitstring<24>& set_four_frames();

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<24>> c;

    void destroy_();
  };
  struct sf_alloc2_c_ {
    struct types_opts {
      enum options { one_frame, four_frames, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    sf_alloc2_c_() = default;
    sf_alloc2_c_(const sf_alloc2_c_& other);
    sf_alloc2_c_& operator=(const sf_alloc2_c_& other);
    ~sf_alloc2_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<2>& one_frame()
    {
      assert_choice_type(types::one_frame, type_, "subframeAllocation2");
      return c.get<fixed_bitstring<2>>();
    }
    fixed_bitstring<8>& four_frames()
    {
      assert_choice_type(types::four_frames, type_, "subframeAllocation2");
      return c.get<fixed_bitstring<8>>();
    }
    const fixed_bitstring<2>& one_frame() const
    {
      assert_choice_type(types::one_frame, type_, "subframeAllocation2");
      return c.get<fixed_bitstring<2>>();
    }
    const fixed_bitstring<8>& four_frames() const
    {
      assert_choice_type(types::four_frames, type_, "subframeAllocation2");
      return c.get<fixed_bitstring<8>>();
    }
    fixed_bitstring<2>& set_one_frame();
    fixed_bitstring<8>& set_four_frames();

  private:
    types                               type_;
    choice_buffer_t<fixed_bitstring<8>> c;

    void destroy_();
  };

  // member variables
  bool                       ext               = false;
  bool                       sf_alloc2_present = false;
  radioframe_alloc_period_e_ radioframe_alloc_period;
  uint8_t                    radioframe_alloc_offset = 0;
  sf_alloc1_c_               sf_alloc1;
  sf_alloc2_c_               sf_alloc2;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-NS-PmaxList ::= SEQUENCE (SIZE (1..8)) OF NR-NS-PmaxValue
using nr_ns_pmax_list_l = dyn_array<nr_ns_pmax_value_s>;

// RACH-ConfigCommonTwoStepRA-r16 ::= SEQUENCE
struct rach_cfg_common_two_step_ra_r16_s {
  struct msg_a_ssb_per_rach_occasion_and_cb_preambs_per_ssb_r16_c_ {
    struct one_eighth_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, n36, n40, n44, n48, n52, n56, n60, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using one_eighth_e_ = enumerated<one_eighth_opts>;
    struct one_fourth_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, n36, n40, n44, n48, n52, n56, n60, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using one_fourth_e_ = enumerated<one_fourth_opts>;
    struct one_half_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, n36, n40, n44, n48, n52, n56, n60, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using one_half_e_ = enumerated<one_half_opts>;
    struct one_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, n36, n40, n44, n48, n52, n56, n60, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using one_e_ = enumerated<one_opts>;
    struct two_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using two_e_ = enumerated<two_opts>;
    struct types_opts {
      enum options { one_eighth, one_fourth, one_half, one, two, four, eight, sixteen, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    msg_a_ssb_per_rach_occasion_and_cb_preambs_per_ssb_r16_c_() = default;
    msg_a_ssb_per_rach_occasion_and_cb_preambs_per_ssb_r16_c_(
        const msg_a_ssb_per_rach_occasion_and_cb_preambs_per_ssb_r16_c_& other);
    msg_a_ssb_per_rach_occasion_and_cb_preambs_per_ssb_r16_c_&
    operator=(const msg_a_ssb_per_rach_occasion_and_cb_preambs_per_ssb_r16_c_& other);
    ~msg_a_ssb_per_rach_occasion_and_cb_preambs_per_ssb_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    one_eighth_e_& one_eighth()
    {
      assert_choice_type(types::one_eighth, type_, "msgA-SSB-PerRACH-OccasionAndCB-PreamblesPerSSB-r16");
      return c.get<one_eighth_e_>();
    }
    one_fourth_e_& one_fourth()
    {
      assert_choice_type(types::one_fourth, type_, "msgA-SSB-PerRACH-OccasionAndCB-PreamblesPerSSB-r16");
      return c.get<one_fourth_e_>();
    }
    one_half_e_& one_half()
    {
      assert_choice_type(types::one_half, type_, "msgA-SSB-PerRACH-OccasionAndCB-PreamblesPerSSB-r16");
      return c.get<one_half_e_>();
    }
    one_e_& one()
    {
      assert_choice_type(types::one, type_, "msgA-SSB-PerRACH-OccasionAndCB-PreamblesPerSSB-r16");
      return c.get<one_e_>();
    }
    two_e_& two()
    {
      assert_choice_type(types::two, type_, "msgA-SSB-PerRACH-OccasionAndCB-PreamblesPerSSB-r16");
      return c.get<two_e_>();
    }
    uint8_t& four()
    {
      assert_choice_type(types::four, type_, "msgA-SSB-PerRACH-OccasionAndCB-PreamblesPerSSB-r16");
      return c.get<uint8_t>();
    }
    uint8_t& eight()
    {
      assert_choice_type(types::eight, type_, "msgA-SSB-PerRACH-OccasionAndCB-PreamblesPerSSB-r16");
      return c.get<uint8_t>();
    }
    uint8_t& sixteen()
    {
      assert_choice_type(types::sixteen, type_, "msgA-SSB-PerRACH-OccasionAndCB-PreamblesPerSSB-r16");
      return c.get<uint8_t>();
    }
    const one_eighth_e_& one_eighth() const
    {
      assert_choice_type(types::one_eighth, type_, "msgA-SSB-PerRACH-OccasionAndCB-PreamblesPerSSB-r16");
      return c.get<one_eighth_e_>();
    }
    const one_fourth_e_& one_fourth() const
    {
      assert_choice_type(types::one_fourth, type_, "msgA-SSB-PerRACH-OccasionAndCB-PreamblesPerSSB-r16");
      return c.get<one_fourth_e_>();
    }
    const one_half_e_& one_half() const
    {
      assert_choice_type(types::one_half, type_, "msgA-SSB-PerRACH-OccasionAndCB-PreamblesPerSSB-r16");
      return c.get<one_half_e_>();
    }
    const one_e_& one() const
    {
      assert_choice_type(types::one, type_, "msgA-SSB-PerRACH-OccasionAndCB-PreamblesPerSSB-r16");
      return c.get<one_e_>();
    }
    const two_e_& two() const
    {
      assert_choice_type(types::two, type_, "msgA-SSB-PerRACH-OccasionAndCB-PreamblesPerSSB-r16");
      return c.get<two_e_>();
    }
    const uint8_t& four() const
    {
      assert_choice_type(types::four, type_, "msgA-SSB-PerRACH-OccasionAndCB-PreamblesPerSSB-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& eight() const
    {
      assert_choice_type(types::eight, type_, "msgA-SSB-PerRACH-OccasionAndCB-PreamblesPerSSB-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& sixteen() const
    {
      assert_choice_type(types::sixteen, type_, "msgA-SSB-PerRACH-OccasionAndCB-PreamblesPerSSB-r16");
      return c.get<uint8_t>();
    }
    one_eighth_e_& set_one_eighth();
    one_fourth_e_& set_one_fourth();
    one_half_e_&   set_one_half();
    one_e_&        set_one();
    two_e_&        set_two();
    uint8_t&       set_four();
    uint8_t&       set_eight();
    uint8_t&       set_sixteen();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct msg_a_prach_root_seq_idx_r16_c_ {
    struct types_opts {
      enum options { l839, l139, l571, l1151, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    msg_a_prach_root_seq_idx_r16_c_() = default;
    msg_a_prach_root_seq_idx_r16_c_(const msg_a_prach_root_seq_idx_r16_c_& other);
    msg_a_prach_root_seq_idx_r16_c_& operator=(const msg_a_prach_root_seq_idx_r16_c_& other);
    ~msg_a_prach_root_seq_idx_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t& l839()
    {
      assert_choice_type(types::l839, type_, "msgA-PRACH-RootSequenceIndex-r16");
      return c.get<uint16_t>();
    }
    uint8_t& l139()
    {
      assert_choice_type(types::l139, type_, "msgA-PRACH-RootSequenceIndex-r16");
      return c.get<uint8_t>();
    }
    uint16_t& l571()
    {
      assert_choice_type(types::l571, type_, "msgA-PRACH-RootSequenceIndex-r16");
      return c.get<uint16_t>();
    }
    uint16_t& l1151()
    {
      assert_choice_type(types::l1151, type_, "msgA-PRACH-RootSequenceIndex-r16");
      return c.get<uint16_t>();
    }
    const uint16_t& l839() const
    {
      assert_choice_type(types::l839, type_, "msgA-PRACH-RootSequenceIndex-r16");
      return c.get<uint16_t>();
    }
    const uint8_t& l139() const
    {
      assert_choice_type(types::l139, type_, "msgA-PRACH-RootSequenceIndex-r16");
      return c.get<uint8_t>();
    }
    const uint16_t& l571() const
    {
      assert_choice_type(types::l571, type_, "msgA-PRACH-RootSequenceIndex-r16");
      return c.get<uint16_t>();
    }
    const uint16_t& l1151() const
    {
      assert_choice_type(types::l1151, type_, "msgA-PRACH-RootSequenceIndex-r16");
      return c.get<uint16_t>();
    }
    uint16_t& set_l839();
    uint8_t&  set_l139();
    uint16_t& set_l571();
    uint16_t& set_l1151();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct msg_a_trans_max_r16_opts {
    enum options { n1, n2, n4, n6, n8, n10, n20, n50, n100, n200, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using msg_a_trans_max_r16_e_ = enumerated<msg_a_trans_max_r16_opts>;
  struct msg_a_restricted_set_cfg_r16_opts {
    enum options { unrestricted_set, restricted_set_type_a, restricted_set_type_b, nulltype } value;

    const char* to_string() const;
  };
  using msg_a_restricted_set_cfg_r16_e_ = enumerated<msg_a_restricted_set_cfg_r16_opts>;
  struct ra_prioritization_for_access_id_two_step_r16_s_ {
    ra_prioritization_s ra_prioritization_r16;
    fixed_bitstring<2>  ra_prioritization_for_ai_r16;
  };
  struct ra_contention_resolution_timer_r16_opts {
    enum options { sf8, sf16, sf24, sf32, sf40, sf48, sf56, sf64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ra_contention_resolution_timer_r16_e_   = enumerated<ra_contention_resolution_timer_r16_opts>;
  using feature_combination_preambs_list_r17_l_ = dyn_array<feature_combination_preambs_r17_s>;

  // member variables
  bool                               ext                                                            = false;
  bool                               msg_a_total_nof_ra_preambs_r16_present                         = false;
  bool                               msg_a_ssb_per_rach_occasion_and_cb_preambs_per_ssb_r16_present = false;
  bool                               msg_a_cb_preambs_per_ssb_per_shared_ro_r16_present             = false;
  bool                               msg_a_ssb_shared_ro_mask_idx_r16_present                       = false;
  bool                               group_b_cfg_two_step_ra_r16_present                            = false;
  bool                               msg_a_prach_root_seq_idx_r16_present                           = false;
  bool                               msg_a_trans_max_r16_present                                    = false;
  bool                               msg_a_rsrp_thres_r16_present                                   = false;
  bool                               msg_a_rsrp_thres_ssb_r16_present                               = false;
  bool                               msg_a_subcarrier_spacing_r16_present                           = false;
  bool                               msg_a_restricted_set_cfg_r16_present                           = false;
  bool                               ra_prioritization_for_access_id_two_step_r16_present           = false;
  bool                               ra_contention_resolution_timer_r16_present                     = false;
  rach_cfg_generic_two_step_ra_r16_s rach_cfg_generic_two_step_ra_r16;
  uint8_t                            msg_a_total_nof_ra_preambs_r16 = 1;
  msg_a_ssb_per_rach_occasion_and_cb_preambs_per_ssb_r16_c_ msg_a_ssb_per_rach_occasion_and_cb_preambs_per_ssb_r16;
  uint8_t                                                   msg_a_cb_preambs_per_ssb_per_shared_ro_r16 = 1;
  uint8_t                                                   msg_a_ssb_shared_ro_mask_idx_r16           = 1;
  group_b_cfg_two_step_ra_r16_s                             group_b_cfg_two_step_ra_r16;
  msg_a_prach_root_seq_idx_r16_c_                           msg_a_prach_root_seq_idx_r16;
  msg_a_trans_max_r16_e_                                    msg_a_trans_max_r16;
  uint8_t                                                   msg_a_rsrp_thres_r16     = 0;
  uint8_t                                                   msg_a_rsrp_thres_ssb_r16 = 0;
  subcarrier_spacing_e                                      msg_a_subcarrier_spacing_r16;
  msg_a_restricted_set_cfg_r16_e_                           msg_a_restricted_set_cfg_r16;
  ra_prioritization_for_access_id_two_step_r16_s_           ra_prioritization_for_access_id_two_step_r16;
  ra_contention_resolution_timer_r16_e_                     ra_contention_resolution_timer_r16;
  // ...
  // group 0
  copy_ptr<ra_prioritization_for_slicing_r17_s>     ra_prioritization_for_slicing_two_step_r17;
  copy_ptr<feature_combination_preambs_list_r17_l_> feature_combination_preambs_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RACH-ConfigGeneric ::= SEQUENCE
struct rach_cfg_generic_s {
  struct msg1_fdm_opts {
    enum options { one, two, four, eight, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using msg1_fdm_e_ = enumerated<msg1_fdm_opts>;
  struct preamb_trans_max_opts {
    enum options { n3, n4, n5, n6, n7, n8, n10, n20, n50, n100, n200, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using preamb_trans_max_e_ = enumerated<preamb_trans_max_opts>;
  struct pwr_ramp_step_opts {
    enum options { db0, db2, db4, db6, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using pwr_ramp_step_e_ = enumerated<pwr_ramp_step_opts>;
  struct ra_resp_win_opts {
    enum options { sl1, sl2, sl4, sl8, sl10, sl20, sl40, sl80, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ra_resp_win_e_ = enumerated<ra_resp_win_opts>;
  struct prach_cfg_period_scaling_iab_r16_opts {
    enum options { scf1, scf2, scf4, scf8, scf16, scf32, scf64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using prach_cfg_period_scaling_iab_r16_e_ = enumerated<prach_cfg_period_scaling_iab_r16_opts>;
  struct ra_resp_win_v1610_opts {
    enum options { sl60, sl160, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ra_resp_win_v1610_e_ = enumerated<ra_resp_win_v1610_opts>;
  struct ra_resp_win_v1700_opts {
    enum options { sl240, sl320, sl640, sl960, sl1280, sl1920, sl2560, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using ra_resp_win_v1700_e_ = enumerated<ra_resp_win_v1700_opts>;

  // member variables
  bool                ext           = false;
  uint16_t            prach_cfg_idx = 0;
  msg1_fdm_e_         msg1_fdm;
  uint16_t            msg1_freq_start           = 0;
  uint8_t             zero_correlation_zone_cfg = 0;
  int16_t             preamb_rx_target_pwr      = -202;
  preamb_trans_max_e_ preamb_trans_max;
  pwr_ramp_step_e_    pwr_ramp_step;
  ra_resp_win_e_      ra_resp_win;
  // ...
  // group 0
  bool                                prach_cfg_period_scaling_iab_r16_present = false;
  bool                                prach_cfg_frame_offset_iab_r16_present   = false;
  bool                                prach_cfg_s_offset_iab_r16_present       = false;
  bool                                ra_resp_win_v1610_present                = false;
  bool                                prach_cfg_idx_v1610_present              = false;
  prach_cfg_period_scaling_iab_r16_e_ prach_cfg_period_scaling_iab_r16;
  uint8_t                             prach_cfg_frame_offset_iab_r16 = 0;
  uint8_t                             prach_cfg_s_offset_iab_r16     = 0;
  ra_resp_win_v1610_e_                ra_resp_win_v1610;
  uint16_t                            prach_cfg_idx_v1610 = 256;
  // group 1
  bool                 ra_resp_win_v1700_present = false;
  ra_resp_win_v1700_e_ ra_resp_win_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-MBSFN-SubframeConfigList ::= SEQUENCE (SIZE (1..8)) OF EUTRA-MBSFN-SubframeConfig
using eutra_mbsfn_sf_cfg_list_l = dyn_array<eutra_mbsfn_sf_cfg_s>;

// MsgA-ConfigCommon-r16 ::= SEQUENCE
struct msg_a_cfg_common_r16_s {
  bool                              msg_a_pusch_cfg_r16_present = false;
  rach_cfg_common_two_step_ra_r16_s rach_cfg_common_two_step_ra_r16;
  msg_a_pusch_cfg_r16_s             msg_a_pusch_cfg_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-MultiBandInfo ::= SEQUENCE
struct nr_multi_band_info_s {
  bool              freq_band_ind_nr_present = false;
  uint16_t          freq_band_ind_nr         = 1;
  nr_ns_pmax_list_l nr_ns_pmax_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-TimeDomainResourceAllocation ::= SEQUENCE
struct pdsch_time_domain_res_alloc_s {
  struct map_type_opts {
    enum options { type_a, type_b, nulltype } value;

    const char* to_string() const;
  };
  using map_type_e_ = enumerated<map_type_opts>;

  // member variables
  bool        k0_present = false;
  uint8_t     k0         = 0;
  map_type_e_ map_type;
  uint8_t     start_symbol_and_len = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-TimeDomainResourceAllocation-r16 ::= SEQUENCE
struct pdsch_time_domain_res_alloc_r16_s {
  struct map_type_r16_opts {
    enum options { type_a, type_b, nulltype } value;

    const char* to_string() const;
  };
  using map_type_r16_e_ = enumerated<map_type_r16_opts>;
  struct repeat_num_r16_opts {
    enum options { n2, n3, n4, n5, n6, n7, n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using repeat_num_r16_e_ = enumerated<repeat_num_r16_opts>;
  struct repeat_num_v1730_opts {
    enum options { n2, n3, n4, n5, n6, n7, n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using repeat_num_v1730_e_ = enumerated<repeat_num_v1730_opts>;

  // member variables
  bool              ext                    = false;
  bool              k0_r16_present         = false;
  bool              repeat_num_r16_present = false;
  uint8_t           k0_r16                 = 0;
  map_type_r16_e_   map_type_r16;
  uint8_t           start_symbol_and_len_r16 = 0;
  repeat_num_r16_e_ repeat_num_r16;
  // ...
  // group 0
  bool    k0_v1710_present = false;
  uint8_t k0_v1710         = 33;
  // group 1
  bool                repeat_num_v1730_present = false;
  repeat_num_v1730_e_ repeat_num_v1730;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-TimeDomainResourceAllocation ::= SEQUENCE
struct pusch_time_domain_res_alloc_s {
  struct map_type_opts {
    enum options { type_a, type_b, nulltype } value;

    const char* to_string() const;
  };
  using map_type_e_ = enumerated<map_type_opts>;

  // member variables
  bool        k2_present = false;
  uint8_t     k2         = 0;
  map_type_e_ map_type;
  uint8_t     start_symbol_and_len = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RACH-ConfigCommon ::= SEQUENCE
struct rach_cfg_common_s {
  struct ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_ {
    struct one_eighth_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, n36, n40, n44, n48, n52, n56, n60, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using one_eighth_e_ = enumerated<one_eighth_opts>;
    struct one_fourth_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, n36, n40, n44, n48, n52, n56, n60, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using one_fourth_e_ = enumerated<one_fourth_opts>;
    struct one_half_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, n36, n40, n44, n48, n52, n56, n60, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using one_half_e_ = enumerated<one_half_opts>;
    struct one_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, n36, n40, n44, n48, n52, n56, n60, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using one_e_ = enumerated<one_opts>;
    struct two_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using two_e_ = enumerated<two_opts>;
    struct types_opts {
      enum options { one_eighth, one_fourth, one_half, one, two, four, eight, sixteen, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_() = default;
    ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_(const ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_& other);
    ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_&
    operator=(const ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_& other);
    ~ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    one_eighth_e_& one_eighth()
    {
      assert_choice_type(types::one_eighth, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_eighth_e_>();
    }
    one_fourth_e_& one_fourth()
    {
      assert_choice_type(types::one_fourth, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_fourth_e_>();
    }
    one_half_e_& one_half()
    {
      assert_choice_type(types::one_half, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_half_e_>();
    }
    one_e_& one()
    {
      assert_choice_type(types::one, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_e_>();
    }
    two_e_& two()
    {
      assert_choice_type(types::two, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<two_e_>();
    }
    uint8_t& four()
    {
      assert_choice_type(types::four, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<uint8_t>();
    }
    uint8_t& eight()
    {
      assert_choice_type(types::eight, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<uint8_t>();
    }
    uint8_t& sixteen()
    {
      assert_choice_type(types::sixteen, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<uint8_t>();
    }
    const one_eighth_e_& one_eighth() const
    {
      assert_choice_type(types::one_eighth, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_eighth_e_>();
    }
    const one_fourth_e_& one_fourth() const
    {
      assert_choice_type(types::one_fourth, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_fourth_e_>();
    }
    const one_half_e_& one_half() const
    {
      assert_choice_type(types::one_half, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_half_e_>();
    }
    const one_e_& one() const
    {
      assert_choice_type(types::one, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_e_>();
    }
    const two_e_& two() const
    {
      assert_choice_type(types::two, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<two_e_>();
    }
    const uint8_t& four() const
    {
      assert_choice_type(types::four, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<uint8_t>();
    }
    const uint8_t& eight() const
    {
      assert_choice_type(types::eight, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<uint8_t>();
    }
    const uint8_t& sixteen() const
    {
      assert_choice_type(types::sixteen, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<uint8_t>();
    }
    one_eighth_e_& set_one_eighth();
    one_fourth_e_& set_one_fourth();
    one_half_e_&   set_one_half();
    one_e_&        set_one();
    two_e_&        set_two();
    uint8_t&       set_four();
    uint8_t&       set_eight();
    uint8_t&       set_sixteen();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct group_bcfg_s_ {
    struct ra_msg3_size_group_a_opts {
      enum options {
        b56,
        b144,
        b208,
        b256,
        b282,
        b480,
        b640,
        b800,
        b1000,
        b72,
        spare6,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using ra_msg3_size_group_a_e_ = enumerated<ra_msg3_size_group_a_opts>;
    struct msg_pwr_offset_group_b_opts {
      enum options { minusinfinity, db0, db5, db8, db10, db12, db15, db18, nulltype } value;
      typedef int8_t number_type;

      const char* to_string() const;
      int8_t      to_number() const;
    };
    using msg_pwr_offset_group_b_e_ = enumerated<msg_pwr_offset_group_b_opts>;

    // member variables
    ra_msg3_size_group_a_e_   ra_msg3_size_group_a;
    msg_pwr_offset_group_b_e_ msg_pwr_offset_group_b;
    uint8_t                   nof_ra_preambs_group_a = 1;
  };
  struct ra_contention_resolution_timer_opts {
    enum options { sf8, sf16, sf24, sf32, sf40, sf48, sf56, sf64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ra_contention_resolution_timer_e_ = enumerated<ra_contention_resolution_timer_opts>;
  struct prach_root_seq_idx_c_ {
    struct types_opts {
      enum options { l839, l139, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    prach_root_seq_idx_c_() = default;
    prach_root_seq_idx_c_(const prach_root_seq_idx_c_& other);
    prach_root_seq_idx_c_& operator=(const prach_root_seq_idx_c_& other);
    ~prach_root_seq_idx_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t& l839()
    {
      assert_choice_type(types::l839, type_, "prach-RootSequenceIndex");
      return c.get<uint16_t>();
    }
    uint8_t& l139()
    {
      assert_choice_type(types::l139, type_, "prach-RootSequenceIndex");
      return c.get<uint8_t>();
    }
    const uint16_t& l839() const
    {
      assert_choice_type(types::l839, type_, "prach-RootSequenceIndex");
      return c.get<uint16_t>();
    }
    const uint8_t& l139() const
    {
      assert_choice_type(types::l139, type_, "prach-RootSequenceIndex");
      return c.get<uint8_t>();
    }
    uint16_t& set_l839();
    uint8_t&  set_l139();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct restricted_set_cfg_opts {
    enum options { unrestricted_set, restricted_set_type_a, restricted_set_type_b, nulltype } value;

    const char* to_string() const;
  };
  using restricted_set_cfg_e_ = enumerated<restricted_set_cfg_opts>;
  struct ra_prioritization_for_access_id_r16_s_ {
    ra_prioritization_s ra_prioritization_r16;
    fixed_bitstring<2>  ra_prioritization_for_ai_r16;
  };
  struct prach_root_seq_idx_r16_c_ {
    struct types_opts {
      enum options { l571, l1151, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    prach_root_seq_idx_r16_c_() = default;
    prach_root_seq_idx_r16_c_(const prach_root_seq_idx_r16_c_& other);
    prach_root_seq_idx_r16_c_& operator=(const prach_root_seq_idx_r16_c_& other);
    ~prach_root_seq_idx_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t& l571()
    {
      assert_choice_type(types::l571, type_, "prach-RootSequenceIndex-r16");
      return c.get<uint16_t>();
    }
    uint16_t& l1151()
    {
      assert_choice_type(types::l1151, type_, "prach-RootSequenceIndex-r16");
      return c.get<uint16_t>();
    }
    const uint16_t& l571() const
    {
      assert_choice_type(types::l571, type_, "prach-RootSequenceIndex-r16");
      return c.get<uint16_t>();
    }
    const uint16_t& l1151() const
    {
      assert_choice_type(types::l1151, type_, "prach-RootSequenceIndex-r16");
      return c.get<uint16_t>();
    }
    uint16_t& set_l571();
    uint16_t& set_l1151();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  using feature_combination_preambs_list_r17_l_ = dyn_array<feature_combination_preambs_r17_s>;

  // member variables
  bool                                            ext                                                  = false;
  bool                                            total_nof_ra_preambs_present                         = false;
  bool                                            ssb_per_rach_occasion_and_cb_preambs_per_ssb_present = false;
  bool                                            group_bcfg_present                                   = false;
  bool                                            rsrp_thres_ssb_present                               = false;
  bool                                            rsrp_thres_ssb_sul_present                           = false;
  bool                                            msg1_subcarrier_spacing_present                      = false;
  bool                                            msg3_transform_precoder_present                      = false;
  rach_cfg_generic_s                              rach_cfg_generic;
  uint8_t                                         total_nof_ra_preambs = 1;
  ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_ ssb_per_rach_occasion_and_cb_preambs_per_ssb;
  group_bcfg_s_                                   group_bcfg;
  ra_contention_resolution_timer_e_               ra_contention_resolution_timer;
  uint8_t                                         rsrp_thres_ssb     = 0;
  uint8_t                                         rsrp_thres_ssb_sul = 0;
  prach_root_seq_idx_c_                           prach_root_seq_idx;
  subcarrier_spacing_e                            msg1_subcarrier_spacing;
  restricted_set_cfg_e_                           restricted_set_cfg;
  // ...
  // group 0
  copy_ptr<ra_prioritization_for_access_id_r16_s_> ra_prioritization_for_access_id_r16;
  copy_ptr<prach_root_seq_idx_r16_c_>              prach_root_seq_idx_r16;
  // group 1
  copy_ptr<ra_prioritization_for_slicing_r17_s>     ra_prioritization_for_slicing_r17;
  copy_ptr<feature_combination_preambs_list_r17_l_> feature_combination_preambs_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AdditionalRACH-Config-r17 ::= SEQUENCE
struct add_rach_cfg_r17_s {
  bool                   ext                          = false;
  bool                   rach_cfg_common_r17_present  = false;
  bool                   msg_a_cfg_common_r17_present = false;
  rach_cfg_common_s      rach_cfg_common_r17;
  msg_a_cfg_common_r16_s msg_a_cfg_common_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ControlResourceSet ::= SEQUENCE
struct coreset_s {
  struct cce_reg_map_type_c_ {
    struct interleaved_s_ {
      struct reg_bundle_size_opts {
        enum options { n2, n3, n6, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using reg_bundle_size_e_ = enumerated<reg_bundle_size_opts>;
      struct interleaver_size_opts {
        enum options { n2, n3, n6, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using interleaver_size_e_ = enumerated<interleaver_size_opts>;

      // member variables
      bool                shift_idx_present = false;
      reg_bundle_size_e_  reg_bundle_size;
      interleaver_size_e_ interleaver_size;
      uint16_t            shift_idx = 0;
    };
    struct types_opts {
      enum options { interleaved, non_interleaved, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    cce_reg_map_type_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    interleaved_s_& interleaved()
    {
      assert_choice_type(types::interleaved, type_, "cce-REG-MappingType");
      return c;
    }
    const interleaved_s_& interleaved() const
    {
      assert_choice_type(types::interleaved, type_, "cce-REG-MappingType");
      return c;
    }
    interleaved_s_& set_interleaved();
    void            set_non_interleaved();

  private:
    types          type_;
    interleaved_s_ c;
  };
  struct precoder_granularity_opts {
    enum options { same_as_reg_bundle, all_contiguous_rbs, nulltype } value;

    const char* to_string() const;
  };
  using precoder_granularity_e_             = enumerated<precoder_granularity_opts>;
  using tci_states_pdcch_to_add_list_l_     = dyn_array<uint8_t>;
  using tci_states_pdcch_to_release_list_l_ = dyn_array<uint8_t>;

  // member variables
  bool                                ext                              = false;
  bool                                tci_present_in_dci_present       = false;
  bool                                pdcch_dmrs_scrambling_id_present = false;
  uint8_t                             coreset_id                       = 0;
  fixed_bitstring<45>                 freq_domain_res;
  uint8_t                             dur = 1;
  cce_reg_map_type_c_                 cce_reg_map_type;
  precoder_granularity_e_             precoder_granularity;
  tci_states_pdcch_to_add_list_l_     tci_states_pdcch_to_add_list;
  tci_states_pdcch_to_release_list_l_ tci_states_pdcch_to_release_list;
  uint32_t                            pdcch_dmrs_scrambling_id = 0;
  // ...
  // group 0
  bool    rb_offset_r16_present           = false;
  bool    tci_present_dci_1_2_r16_present = false;
  bool    coreset_pool_idx_r16_present    = false;
  bool    coreset_id_v1610_present        = false;
  uint8_t rb_offset_r16                   = 0;
  uint8_t tci_present_dci_1_2_r16         = 1;
  uint8_t coreset_pool_idx_r16            = 0;
  uint8_t coreset_id_v1610                = 12;
  // group 1
  bool follow_unified_tci_state_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MultiFrequencyBandListNR-SIB ::= SEQUENCE (SIZE (1..8)) OF NR-MultiBandInfo
using multi_freq_band_list_nr_sib_l = dyn_array<nr_multi_band_info_s>;

// PDSCH-TimeDomainResourceAllocationList ::= SEQUENCE (SIZE (1..16)) OF PDSCH-TimeDomainResourceAllocation
using pdsch_time_domain_res_alloc_list_l = dyn_array<pdsch_time_domain_res_alloc_s>;

// PDSCH-TimeDomainResourceAllocationList-r16 ::= SEQUENCE (SIZE (1..16)) OF PDSCH-TimeDomainResourceAllocation-r16
using pdsch_time_domain_res_alloc_list_r16_l = dyn_array<pdsch_time_domain_res_alloc_r16_s>;

// PUSCH-TimeDomainResourceAllocationList ::= SEQUENCE (SIZE (1..16)) OF PUSCH-TimeDomainResourceAllocation
using pusch_time_domain_res_alloc_list_l = dyn_array<pusch_time_domain_res_alloc_s>;

// RateMatchPattern ::= SEQUENCE
struct rate_match_pattern_s {
  struct pattern_type_c_ {
    struct bitmaps_s_ {
      struct symbols_in_res_block_c_ {
        struct types_opts {
          enum options { one_slot, two_slots, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        using types = enumerated<types_opts>;

        // choice methods
        symbols_in_res_block_c_() = default;
        symbols_in_res_block_c_(const symbols_in_res_block_c_& other);
        symbols_in_res_block_c_& operator=(const symbols_in_res_block_c_& other);
        ~symbols_in_res_block_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        fixed_bitstring<14>& one_slot()
        {
          assert_choice_type(types::one_slot, type_, "symbolsInResourceBlock");
          return c.get<fixed_bitstring<14>>();
        }
        fixed_bitstring<28>& two_slots()
        {
          assert_choice_type(types::two_slots, type_, "symbolsInResourceBlock");
          return c.get<fixed_bitstring<28>>();
        }
        const fixed_bitstring<14>& one_slot() const
        {
          assert_choice_type(types::one_slot, type_, "symbolsInResourceBlock");
          return c.get<fixed_bitstring<14>>();
        }
        const fixed_bitstring<28>& two_slots() const
        {
          assert_choice_type(types::two_slots, type_, "symbolsInResourceBlock");
          return c.get<fixed_bitstring<28>>();
        }
        fixed_bitstring<14>& set_one_slot();
        fixed_bitstring<28>& set_two_slots();

      private:
        types                                type_;
        choice_buffer_t<fixed_bitstring<28>> c;

        void destroy_();
      };
      struct periodicity_and_pattern_c_ {
        struct types_opts {
          enum options { n2, n4, n5, n8, n10, n20, n40, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        using types = enumerated<types_opts>;

        // choice methods
        periodicity_and_pattern_c_() = default;
        periodicity_and_pattern_c_(const periodicity_and_pattern_c_& other);
        periodicity_and_pattern_c_& operator=(const periodicity_and_pattern_c_& other);
        ~periodicity_and_pattern_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        fixed_bitstring<2>& n2()
        {
          assert_choice_type(types::n2, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<2>>();
        }
        fixed_bitstring<4>& n4()
        {
          assert_choice_type(types::n4, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<4>>();
        }
        fixed_bitstring<5>& n5()
        {
          assert_choice_type(types::n5, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<5>>();
        }
        fixed_bitstring<8>& n8()
        {
          assert_choice_type(types::n8, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<8>>();
        }
        fixed_bitstring<10>& n10()
        {
          assert_choice_type(types::n10, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<10>>();
        }
        fixed_bitstring<20>& n20()
        {
          assert_choice_type(types::n20, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<20>>();
        }
        fixed_bitstring<40>& n40()
        {
          assert_choice_type(types::n40, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<40>>();
        }
        const fixed_bitstring<2>& n2() const
        {
          assert_choice_type(types::n2, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<2>>();
        }
        const fixed_bitstring<4>& n4() const
        {
          assert_choice_type(types::n4, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<4>>();
        }
        const fixed_bitstring<5>& n5() const
        {
          assert_choice_type(types::n5, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<5>>();
        }
        const fixed_bitstring<8>& n8() const
        {
          assert_choice_type(types::n8, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<8>>();
        }
        const fixed_bitstring<10>& n10() const
        {
          assert_choice_type(types::n10, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<10>>();
        }
        const fixed_bitstring<20>& n20() const
        {
          assert_choice_type(types::n20, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<20>>();
        }
        const fixed_bitstring<40>& n40() const
        {
          assert_choice_type(types::n40, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<40>>();
        }
        fixed_bitstring<2>&  set_n2();
        fixed_bitstring<4>&  set_n4();
        fixed_bitstring<5>&  set_n5();
        fixed_bitstring<8>&  set_n8();
        fixed_bitstring<10>& set_n10();
        fixed_bitstring<20>& set_n20();
        fixed_bitstring<40>& set_n40();

      private:
        types                                type_;
        choice_buffer_t<fixed_bitstring<40>> c;

        void destroy_();
      };

      // member variables
      bool                       ext                             = false;
      bool                       periodicity_and_pattern_present = false;
      fixed_bitstring<275>       res_blocks;
      symbols_in_res_block_c_    symbols_in_res_block;
      periodicity_and_pattern_c_ periodicity_and_pattern;
      // ...
    };
    struct types_opts {
      enum options { bitmaps, coreset, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    pattern_type_c_() = default;
    pattern_type_c_(const pattern_type_c_& other);
    pattern_type_c_& operator=(const pattern_type_c_& other);
    ~pattern_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bitmaps_s_& bitmaps()
    {
      assert_choice_type(types::bitmaps, type_, "patternType");
      return c.get<bitmaps_s_>();
    }
    uint8_t& coreset()
    {
      assert_choice_type(types::coreset, type_, "patternType");
      return c.get<uint8_t>();
    }
    const bitmaps_s_& bitmaps() const
    {
      assert_choice_type(types::bitmaps, type_, "patternType");
      return c.get<bitmaps_s_>();
    }
    const uint8_t& coreset() const
    {
      assert_choice_type(types::coreset, type_, "patternType");
      return c.get<uint8_t>();
    }
    bitmaps_s_& set_bitmaps();
    uint8_t&    set_coreset();

  private:
    types                       type_;
    choice_buffer_t<bitmaps_s_> c;

    void destroy_();
  };
  struct dummy_opts {
    enum options { dyn, semi_static, nulltype } value;

    const char* to_string() const;
  };
  using dummy_e_ = enumerated<dummy_opts>;

  // member variables
  bool                 ext                        = false;
  bool                 subcarrier_spacing_present = false;
  uint8_t              rate_match_pattern_id      = 0;
  pattern_type_c_      pattern_type;
  subcarrier_spacing_e subcarrier_spacing;
  dummy_e_             dummy;
  // ...
  // group 0
  bool    coreset_r16_present = false;
  uint8_t coreset_r16         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RateMatchPatternLTE-CRS ::= SEQUENCE
struct rate_match_pattern_lte_crs_s {
  struct carrier_bw_dl_opts {
    enum options { n6, n15, n25, n50, n75, n100, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using carrier_bw_dl_e_ = enumerated<carrier_bw_dl_opts>;
  struct nrof_crs_ports_opts {
    enum options { n1, n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nrof_crs_ports_e_ = enumerated<nrof_crs_ports_opts>;
  struct v_shift_opts {
    enum options { n0, n1, n2, n3, n4, n5, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using v_shift_e_ = enumerated<v_shift_opts>;

  // member variables
  uint16_t                  carrier_freq_dl = 0;
  carrier_bw_dl_e_          carrier_bw_dl;
  eutra_mbsfn_sf_cfg_list_l mbsfn_sf_cfg_list;
  nrof_crs_ports_e_         nrof_crs_ports;
  v_shift_e_                v_shift;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SearchSpace ::= SEQUENCE
struct search_space_s {
  struct monitoring_slot_periodicity_and_offset_c_ {
    struct types_opts {
      enum options {
        sl1,
        sl2,
        sl4,
        sl5,
        sl8,
        sl10,
        sl16,
        sl20,
        sl40,
        sl80,
        sl160,
        sl320,
        sl640,
        sl1280,
        sl2560,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    monitoring_slot_periodicity_and_offset_c_() = default;
    monitoring_slot_periodicity_and_offset_c_(const monitoring_slot_periodicity_and_offset_c_& other);
    monitoring_slot_periodicity_and_offset_c_& operator=(const monitoring_slot_periodicity_and_offset_c_& other);
    ~monitoring_slot_periodicity_and_offset_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sl2()
    {
      assert_choice_type(types::sl2, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl4()
    {
      assert_choice_type(types::sl4, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl5()
    {
      assert_choice_type(types::sl5, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl8()
    {
      assert_choice_type(types::sl8, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl10()
    {
      assert_choice_type(types::sl10, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl16()
    {
      assert_choice_type(types::sl16, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl20()
    {
      assert_choice_type(types::sl20, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl40()
    {
      assert_choice_type(types::sl40, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl80()
    {
      assert_choice_type(types::sl80, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl160()
    {
      assert_choice_type(types::sl160, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint16_t& sl320()
    {
      assert_choice_type(types::sl320, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    uint16_t& sl640()
    {
      assert_choice_type(types::sl640, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    uint16_t& sl1280()
    {
      assert_choice_type(types::sl1280, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    uint16_t& sl2560()
    {
      assert_choice_type(types::sl2560, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    const uint8_t& sl2() const
    {
      assert_choice_type(types::sl2, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl4() const
    {
      assert_choice_type(types::sl4, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl5() const
    {
      assert_choice_type(types::sl5, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl8() const
    {
      assert_choice_type(types::sl8, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl10() const
    {
      assert_choice_type(types::sl10, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl16() const
    {
      assert_choice_type(types::sl16, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl20() const
    {
      assert_choice_type(types::sl20, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl40() const
    {
      assert_choice_type(types::sl40, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl80() const
    {
      assert_choice_type(types::sl80, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl160() const
    {
      assert_choice_type(types::sl160, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint16_t& sl320() const
    {
      assert_choice_type(types::sl320, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& sl640() const
    {
      assert_choice_type(types::sl640, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& sl1280() const
    {
      assert_choice_type(types::sl1280, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& sl2560() const
    {
      assert_choice_type(types::sl2560, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    void      set_sl1();
    uint8_t&  set_sl2();
    uint8_t&  set_sl4();
    uint8_t&  set_sl5();
    uint8_t&  set_sl8();
    uint8_t&  set_sl10();
    uint8_t&  set_sl16();
    uint8_t&  set_sl20();
    uint8_t&  set_sl40();
    uint8_t&  set_sl80();
    uint8_t&  set_sl160();
    uint16_t& set_sl320();
    uint16_t& set_sl640();
    uint16_t& set_sl1280();
    uint16_t& set_sl2560();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct nrof_candidates_s_ {
    struct aggregation_level1_opts {
      enum options { n0, n1, n2, n3, n4, n5, n6, n8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using aggregation_level1_e_ = enumerated<aggregation_level1_opts>;
    struct aggregation_level2_opts {
      enum options { n0, n1, n2, n3, n4, n5, n6, n8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using aggregation_level2_e_ = enumerated<aggregation_level2_opts>;
    struct aggregation_level4_opts {
      enum options { n0, n1, n2, n3, n4, n5, n6, n8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using aggregation_level4_e_ = enumerated<aggregation_level4_opts>;
    struct aggregation_level8_opts {
      enum options { n0, n1, n2, n3, n4, n5, n6, n8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using aggregation_level8_e_ = enumerated<aggregation_level8_opts>;
    struct aggregation_level16_opts {
      enum options { n0, n1, n2, n3, n4, n5, n6, n8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using aggregation_level16_e_ = enumerated<aggregation_level16_opts>;

    // member variables
    aggregation_level1_e_  aggregation_level1;
    aggregation_level2_e_  aggregation_level2;
    aggregation_level4_e_  aggregation_level4;
    aggregation_level8_e_  aggregation_level8;
    aggregation_level16_e_ aggregation_level16;
  };
  struct search_space_type_c_ {
    struct common_s_ {
      struct dci_format0_0_and_format1_0_s_ {
        bool ext = false;
        // ...
      };
      struct dci_format2_0_s_ {
        struct nrof_candidates_sfi_s_ {
          struct aggregation_level1_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using aggregation_level1_e_ = enumerated<aggregation_level1_opts>;
          struct aggregation_level2_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using aggregation_level2_e_ = enumerated<aggregation_level2_opts>;
          struct aggregation_level4_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using aggregation_level4_e_ = enumerated<aggregation_level4_opts>;
          struct aggregation_level8_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using aggregation_level8_e_ = enumerated<aggregation_level8_opts>;
          struct aggregation_level16_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using aggregation_level16_e_ = enumerated<aggregation_level16_opts>;

          // member variables
          bool                   aggregation_level1_present  = false;
          bool                   aggregation_level2_present  = false;
          bool                   aggregation_level4_present  = false;
          bool                   aggregation_level8_present  = false;
          bool                   aggregation_level16_present = false;
          aggregation_level1_e_  aggregation_level1;
          aggregation_level2_e_  aggregation_level2;
          aggregation_level4_e_  aggregation_level4;
          aggregation_level8_e_  aggregation_level8;
          aggregation_level16_e_ aggregation_level16;
        };

        // member variables
        bool                   ext = false;
        nrof_candidates_sfi_s_ nrof_candidates_sfi;
        // ...
      };
      struct dci_format2_1_s_ {
        bool ext = false;
        // ...
      };
      struct dci_format2_2_s_ {
        bool ext = false;
        // ...
      };
      struct dci_format2_3_s_ {
        struct dummy1_opts {
          enum options { sl1, sl2, sl4, sl5, sl8, sl10, sl16, sl20, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        using dummy1_e_ = enumerated<dummy1_opts>;
        struct dummy2_opts {
          enum options { n1, n2, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        using dummy2_e_ = enumerated<dummy2_opts>;

        // member variables
        bool      ext            = false;
        bool      dummy1_present = false;
        dummy1_e_ dummy1;
        dummy2_e_ dummy2;
        // ...
      };

      // member variables
      bool                           dci_format0_0_and_format1_0_present = false;
      bool                           dci_format2_0_present               = false;
      bool                           dci_format2_1_present               = false;
      bool                           dci_format2_2_present               = false;
      bool                           dci_format2_3_present               = false;
      dci_format0_0_and_format1_0_s_ dci_format0_0_and_format1_0;
      dci_format2_0_s_               dci_format2_0;
      dci_format2_1_s_               dci_format2_1;
      dci_format2_2_s_               dci_format2_2;
      dci_format2_3_s_               dci_format2_3;
    };
    struct ue_specific_s_ {
      struct dci_formats_opts {
        enum options { formats0_neg0_and_neg1_neg0, formats0_neg1_and_neg1_neg1, nulltype } value;

        const char* to_string() const;
      };
      using dci_formats_e_ = enumerated<dci_formats_opts>;
      struct dci_formats_sl_r16_opts {
        enum options {
          formats0_neg0_and_neg1_neg0,
          formats0_neg1_and_neg1_neg1,
          formats3_neg0,
          formats3_neg1,
          formats3_neg0_and_neg3_neg1,
          nulltype
        } value;

        const char* to_string() const;
      };
      using dci_formats_sl_r16_e_ = enumerated<dci_formats_sl_r16_opts>;
      struct dci_formats_ext_r16_opts {
        enum options {
          formats0_neg2_and_neg1_neg2,
          formats0_neg1_and_neg1_neg1_and_neg0_neg2_and_neg1_neg2,
          nulltype
        } value;

        const char* to_string() const;
      };
      using dci_formats_ext_r16_e_ = enumerated<dci_formats_ext_r16_opts>;

      // member variables
      bool           ext = false;
      dci_formats_e_ dci_formats;
      // ...
      // group 0
      bool                   dci_formats_mt_r16_present  = false;
      bool                   dci_formats_sl_r16_present  = false;
      bool                   dci_formats_ext_r16_present = false;
      dci_formats_sl_r16_e_  dci_formats_sl_r16;
      dci_formats_ext_r16_e_ dci_formats_ext_r16;

      // sequence methods
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
    };
    struct types_opts {
      enum options { common, ue_specific, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    search_space_type_c_() = default;
    search_space_type_c_(const search_space_type_c_& other);
    search_space_type_c_& operator=(const search_space_type_c_& other);
    ~search_space_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    common_s_& common()
    {
      assert_choice_type(types::common, type_, "searchSpaceType");
      return c.get<common_s_>();
    }
    ue_specific_s_& ue_specific()
    {
      assert_choice_type(types::ue_specific, type_, "searchSpaceType");
      return c.get<ue_specific_s_>();
    }
    const common_s_& common() const
    {
      assert_choice_type(types::common, type_, "searchSpaceType");
      return c.get<common_s_>();
    }
    const ue_specific_s_& ue_specific() const
    {
      assert_choice_type(types::ue_specific, type_, "searchSpaceType");
      return c.get<ue_specific_s_>();
    }
    common_s_&      set_common();
    ue_specific_s_& set_ue_specific();

  private:
    types                                      type_;
    choice_buffer_t<common_s_, ue_specific_s_> c;

    void destroy_();
  };

  // member variables
  bool                                      coreset_id_present                             = false;
  bool                                      monitoring_slot_periodicity_and_offset_present = false;
  bool                                      dur_present                                    = false;
  bool                                      monitoring_symbols_within_slot_present         = false;
  bool                                      nrof_candidates_present                        = false;
  bool                                      search_space_type_present                      = false;
  uint8_t                                   search_space_id                                = 0;
  uint8_t                                   coreset_id                                     = 0;
  monitoring_slot_periodicity_and_offset_c_ monitoring_slot_periodicity_and_offset;
  uint16_t                                  dur = 2;
  fixed_bitstring<14>                       monitoring_symbols_within_slot;
  nrof_candidates_s_                        nrof_candidates;
  search_space_type_c_                      search_space_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SearchSpaceExt-r16 ::= SEQUENCE
struct search_space_ext_r16_s {
  struct search_space_type_r16_s_ {
    struct common_r16_s_ {
      struct dci_format2_4_r16_s_ {
        struct nrof_candidates_ci_r16_s_ {
          struct aggregation_level1_r16_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using aggregation_level1_r16_e_ = enumerated<aggregation_level1_r16_opts>;
          struct aggregation_level2_r16_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using aggregation_level2_r16_e_ = enumerated<aggregation_level2_r16_opts>;
          struct aggregation_level4_r16_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using aggregation_level4_r16_e_ = enumerated<aggregation_level4_r16_opts>;
          struct aggregation_level8_r16_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using aggregation_level8_r16_e_ = enumerated<aggregation_level8_r16_opts>;
          struct aggregation_level16_r16_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using aggregation_level16_r16_e_ = enumerated<aggregation_level16_r16_opts>;

          // member variables
          bool                       aggregation_level1_r16_present  = false;
          bool                       aggregation_level2_r16_present  = false;
          bool                       aggregation_level4_r16_present  = false;
          bool                       aggregation_level8_r16_present  = false;
          bool                       aggregation_level16_r16_present = false;
          aggregation_level1_r16_e_  aggregation_level1_r16;
          aggregation_level2_r16_e_  aggregation_level2_r16;
          aggregation_level4_r16_e_  aggregation_level4_r16;
          aggregation_level8_r16_e_  aggregation_level8_r16;
          aggregation_level16_r16_e_ aggregation_level16_r16;
        };

        // member variables
        bool                      ext = false;
        nrof_candidates_ci_r16_s_ nrof_candidates_ci_r16;
        // ...
      };
      struct dci_format2_5_r16_s_ {
        struct nrof_candidates_iab_r16_s_ {
          struct aggregation_level1_r16_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using aggregation_level1_r16_e_ = enumerated<aggregation_level1_r16_opts>;
          struct aggregation_level2_r16_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using aggregation_level2_r16_e_ = enumerated<aggregation_level2_r16_opts>;
          struct aggregation_level4_r16_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using aggregation_level4_r16_e_ = enumerated<aggregation_level4_r16_opts>;
          struct aggregation_level8_r16_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using aggregation_level8_r16_e_ = enumerated<aggregation_level8_r16_opts>;
          struct aggregation_level16_r16_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using aggregation_level16_r16_e_ = enumerated<aggregation_level16_r16_opts>;

          // member variables
          bool                       aggregation_level1_r16_present  = false;
          bool                       aggregation_level2_r16_present  = false;
          bool                       aggregation_level4_r16_present  = false;
          bool                       aggregation_level8_r16_present  = false;
          bool                       aggregation_level16_r16_present = false;
          aggregation_level1_r16_e_  aggregation_level1_r16;
          aggregation_level2_r16_e_  aggregation_level2_r16;
          aggregation_level4_r16_e_  aggregation_level4_r16;
          aggregation_level8_r16_e_  aggregation_level8_r16;
          aggregation_level16_r16_e_ aggregation_level16_r16;
        };

        // member variables
        bool                       ext = false;
        nrof_candidates_iab_r16_s_ nrof_candidates_iab_r16;
        // ...
      };
      struct dci_format2_6_r16_s_ {
        bool ext = false;
        // ...
      };

      // member variables
      bool                 ext                       = false;
      bool                 dci_format2_4_r16_present = false;
      bool                 dci_format2_5_r16_present = false;
      bool                 dci_format2_6_r16_present = false;
      dci_format2_4_r16_s_ dci_format2_4_r16;
      dci_format2_5_r16_s_ dci_format2_5_r16;
      dci_format2_6_r16_s_ dci_format2_6_r16;
      // ...
    };

    // member variables
    common_r16_s_ common_r16;
  };
  using search_space_group_id_list_r16_l_ = bounded_array<uint8_t, 2>;

  // member variables
  bool                              coreset_id_r16_present             = false;
  bool                              search_space_type_r16_present      = false;
  bool                              freq_monitor_locations_r16_present = false;
  uint8_t                           coreset_id_r16                     = 0;
  search_space_type_r16_s_          search_space_type_r16;
  search_space_group_id_list_r16_l_ search_space_group_id_list_r16;
  fixed_bitstring<5>                freq_monitor_locations_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SearchSpaceExt-v1700 ::= SEQUENCE
struct search_space_ext_v1700_s {
  struct monitoring_slot_periodicity_and_offset_v1710_c_ {
    struct types_opts {
      enum options { sl32, sl64, sl128, sl5120, sl10240, sl20480, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    monitoring_slot_periodicity_and_offset_v1710_c_() = default;
    monitoring_slot_periodicity_and_offset_v1710_c_(const monitoring_slot_periodicity_and_offset_v1710_c_& other);
    monitoring_slot_periodicity_and_offset_v1710_c_&
    operator=(const monitoring_slot_periodicity_and_offset_v1710_c_& other);
    ~monitoring_slot_periodicity_and_offset_v1710_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sl32()
    {
      assert_choice_type(types::sl32, type_, "monitoringSlotPeriodicityAndOffset-v1710");
      return c.get<uint8_t>();
    }
    uint8_t& sl64()
    {
      assert_choice_type(types::sl64, type_, "monitoringSlotPeriodicityAndOffset-v1710");
      return c.get<uint8_t>();
    }
    uint8_t& sl128()
    {
      assert_choice_type(types::sl128, type_, "monitoringSlotPeriodicityAndOffset-v1710");
      return c.get<uint8_t>();
    }
    uint16_t& sl5120()
    {
      assert_choice_type(types::sl5120, type_, "monitoringSlotPeriodicityAndOffset-v1710");
      return c.get<uint16_t>();
    }
    uint16_t& sl10240()
    {
      assert_choice_type(types::sl10240, type_, "monitoringSlotPeriodicityAndOffset-v1710");
      return c.get<uint16_t>();
    }
    uint16_t& sl20480()
    {
      assert_choice_type(types::sl20480, type_, "monitoringSlotPeriodicityAndOffset-v1710");
      return c.get<uint16_t>();
    }
    const uint8_t& sl32() const
    {
      assert_choice_type(types::sl32, type_, "monitoringSlotPeriodicityAndOffset-v1710");
      return c.get<uint8_t>();
    }
    const uint8_t& sl64() const
    {
      assert_choice_type(types::sl64, type_, "monitoringSlotPeriodicityAndOffset-v1710");
      return c.get<uint8_t>();
    }
    const uint8_t& sl128() const
    {
      assert_choice_type(types::sl128, type_, "monitoringSlotPeriodicityAndOffset-v1710");
      return c.get<uint8_t>();
    }
    const uint16_t& sl5120() const
    {
      assert_choice_type(types::sl5120, type_, "monitoringSlotPeriodicityAndOffset-v1710");
      return c.get<uint16_t>();
    }
    const uint16_t& sl10240() const
    {
      assert_choice_type(types::sl10240, type_, "monitoringSlotPeriodicityAndOffset-v1710");
      return c.get<uint16_t>();
    }
    const uint16_t& sl20480() const
    {
      assert_choice_type(types::sl20480, type_, "monitoringSlotPeriodicityAndOffset-v1710");
      return c.get<uint16_t>();
    }
    uint8_t&  set_sl32();
    uint8_t&  set_sl64();
    uint8_t&  set_sl128();
    uint16_t& set_sl5120();
    uint16_t& set_sl10240();
    uint16_t& set_sl20480();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct monitoring_slots_within_slot_group_r17_c_ {
    struct types_opts {
      enum options { slot_group_len4_r17, slot_group_len8_r17, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    monitoring_slots_within_slot_group_r17_c_() = default;
    monitoring_slots_within_slot_group_r17_c_(const monitoring_slots_within_slot_group_r17_c_& other);
    monitoring_slots_within_slot_group_r17_c_& operator=(const monitoring_slots_within_slot_group_r17_c_& other);
    ~monitoring_slots_within_slot_group_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<4>& slot_group_len4_r17()
    {
      assert_choice_type(types::slot_group_len4_r17, type_, "monitoringSlotsWithinSlotGroup-r17");
      return c.get<fixed_bitstring<4>>();
    }
    fixed_bitstring<8>& slot_group_len8_r17()
    {
      assert_choice_type(types::slot_group_len8_r17, type_, "monitoringSlotsWithinSlotGroup-r17");
      return c.get<fixed_bitstring<8>>();
    }
    const fixed_bitstring<4>& slot_group_len4_r17() const
    {
      assert_choice_type(types::slot_group_len4_r17, type_, "monitoringSlotsWithinSlotGroup-r17");
      return c.get<fixed_bitstring<4>>();
    }
    const fixed_bitstring<8>& slot_group_len8_r17() const
    {
      assert_choice_type(types::slot_group_len8_r17, type_, "monitoringSlotsWithinSlotGroup-r17");
      return c.get<fixed_bitstring<8>>();
    }
    fixed_bitstring<4>& set_slot_group_len4_r17();
    fixed_bitstring<8>& set_slot_group_len8_r17();

  private:
    types                               type_;
    choice_buffer_t<fixed_bitstring<8>> c;

    void destroy_();
  };
  struct search_space_type_r17_s_ {
    struct common_r17_s_ {
      struct dci_format4_0_r17_s_ {
        bool ext = false;
        // ...
      };
      struct dci_format4_1_r17_s_ {
        bool ext = false;
        // ...
      };
      struct dci_format4_2_r17_s_ {
        bool ext = false;
        // ...
      };
      struct dci_format4_1_and_format4_2_r17_s_ {
        bool ext = false;
        // ...
      };
      struct dci_format2_7_r17_s_ {
        struct nrof_candidates_pei_r17_s_ {
          struct aggregation_level4_r17_opts {
            enum options { n0, n1, n2, n3, n4, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using aggregation_level4_r17_e_ = enumerated<aggregation_level4_r17_opts>;
          struct aggregation_level8_r17_opts {
            enum options { n0, n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using aggregation_level8_r17_e_ = enumerated<aggregation_level8_r17_opts>;
          struct aggregation_level16_r17_opts {
            enum options { n0, n1, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using aggregation_level16_r17_e_ = enumerated<aggregation_level16_r17_opts>;

          // member variables
          bool                       aggregation_level4_r17_present  = false;
          bool                       aggregation_level8_r17_present  = false;
          bool                       aggregation_level16_r17_present = false;
          aggregation_level4_r17_e_  aggregation_level4_r17;
          aggregation_level8_r17_e_  aggregation_level8_r17;
          aggregation_level16_r17_e_ aggregation_level16_r17;
        };

        // member variables
        bool                       ext = false;
        nrof_candidates_pei_r17_s_ nrof_candidates_pei_r17;
        // ...
      };

      // member variables
      bool                               dci_format4_0_r17_present               = false;
      bool                               dci_format4_1_r17_present               = false;
      bool                               dci_format4_2_r17_present               = false;
      bool                               dci_format4_1_and_format4_2_r17_present = false;
      bool                               dci_format2_7_r17_present               = false;
      dci_format4_0_r17_s_               dci_format4_0_r17;
      dci_format4_1_r17_s_               dci_format4_1_r17;
      dci_format4_2_r17_s_               dci_format4_2_r17;
      dci_format4_1_and_format4_2_r17_s_ dci_format4_1_and_format4_2_r17;
      dci_format2_7_r17_s_               dci_format2_7_r17;
    };

    // member variables
    common_r17_s_ common_r17;
  };
  using search_space_group_id_list_r17_l_ = bounded_array<uint8_t, 3>;

  // member variables
  bool                                            monitoring_slot_periodicity_and_offset_v1710_present = false;
  bool                                            monitoring_slots_within_slot_group_r17_present       = false;
  bool                                            dur_r17_present                                      = false;
  bool                                            search_space_type_r17_present                        = false;
  bool                                            search_space_linking_id_r17_present                  = false;
  monitoring_slot_periodicity_and_offset_v1710_c_ monitoring_slot_periodicity_and_offset_v1710;
  monitoring_slots_within_slot_group_r17_c_       monitoring_slots_within_slot_group_r17;
  uint16_t                                        dur_r17 = 4;
  search_space_type_r17_s_                        search_space_type_r17;
  search_space_group_id_list_r17_l_               search_space_group_id_list_r17;
  uint8_t                                         search_space_linking_id_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AdditionalRACH-ConfigList-r17 ::= SEQUENCE (SIZE (1..256)) OF AdditionalRACH-Config-r17
using add_rach_cfg_list_r17_l = dyn_array<add_rach_cfg_r17_s>;

// BWP ::= SEQUENCE
struct bwp_s {
  bool                 cp_present      = false;
  uint16_t             location_and_bw = 0;
  subcarrier_spacing_e subcarrier_spacing;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NumberOfMsg3-Repetitions-r17 ::= ENUMERATED
struct nof_msg3_repeats_r17_opts {
  enum options { n1, n2, n3, n4, n7, n8, n12, n16, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using nof_msg3_repeats_r17_e = enumerated<nof_msg3_repeats_r17_opts>;

// PDCCH-ConfigCommon ::= SEQUENCE
struct pdcch_cfg_common_s {
  using common_search_space_list_l_ = dyn_array<search_space_s>;
  struct first_pdcch_monitoring_occasion_of_po_c_ {
    using scs15_kh_zone_t_l_                                                              = bounded_array<uint8_t, 4>;
    using scs30_kh_zone_t_scs15_kh_zhalf_t_l_                                             = bounded_array<uint16_t, 4>;
    using scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_                         = bounded_array<uint16_t, 4>;
    using scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_ = bounded_array<uint16_t, 4>;
    using scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_ =
        bounded_array<uint16_t, 4>;
    using scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_ = bounded_array<uint16_t, 4>;
    using scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_                     = bounded_array<uint16_t, 4>;
    using scs120_kh_zone_sixteenth_t_l_                                            = bounded_array<uint16_t, 4>;
    struct types_opts {
      enum options {
        scs15_kh_zone_t,
        scs30_kh_zone_t_scs15_kh_zhalf_t,
        scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t,
        scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t,
        scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t,
        scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
        scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t,
        scs120_kh_zone_sixteenth_t,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    first_pdcch_monitoring_occasion_of_po_c_() = default;
    first_pdcch_monitoring_occasion_of_po_c_(const first_pdcch_monitoring_occasion_of_po_c_& other);
    first_pdcch_monitoring_occasion_of_po_c_& operator=(const first_pdcch_monitoring_occasion_of_po_c_& other);
    ~first_pdcch_monitoring_occasion_of_po_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scs15_kh_zone_t_l_& scs15_kh_zone_t()
    {
      assert_choice_type(types::scs15_kh_zone_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs15_kh_zone_t_l_>();
    }
    scs30_kh_zone_t_scs15_kh_zhalf_t_l_& scs30_kh_zone_t_scs15_kh_zhalf_t()
    {
      assert_choice_type(types::scs30_kh_zone_t_scs15_kh_zhalf_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
    }
    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_& scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t()
    {
      assert_choice_type(
          types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
    }
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t()
    {
      assert_choice_type(types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
    }
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t()
    {
      assert_choice_type(types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
    }
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t()
    {
      assert_choice_type(types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
    }
    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_& scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t()
    {
      assert_choice_type(
          types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
    }
    scs120_kh_zone_sixteenth_t_l_& scs120_kh_zone_sixteenth_t()
    {
      assert_choice_type(types::scs120_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_sixteenth_t_l_>();
    }
    const scs15_kh_zone_t_l_& scs15_kh_zone_t() const
    {
      assert_choice_type(types::scs15_kh_zone_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs15_kh_zone_t_l_>();
    }
    const scs30_kh_zone_t_scs15_kh_zhalf_t_l_& scs30_kh_zone_t_scs15_kh_zhalf_t() const
    {
      assert_choice_type(types::scs30_kh_zone_t_scs15_kh_zhalf_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
    }
    const scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_&
    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t() const
    {
      assert_choice_type(
          types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
    }
    const scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t() const
    {
      assert_choice_type(types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
    }
    const scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t() const
    {
      assert_choice_type(types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
    }
    const scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t() const
    {
      assert_choice_type(types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
    }
    const scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_&
    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t() const
    {
      assert_choice_type(
          types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
    }
    const scs120_kh_zone_sixteenth_t_l_& scs120_kh_zone_sixteenth_t() const
    {
      assert_choice_type(types::scs120_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_sixteenth_t_l_>();
    }
    scs15_kh_zone_t_l_&                                      set_scs15_kh_zone_t();
    scs30_kh_zone_t_scs15_kh_zhalf_t_l_&                     set_scs30_kh_zone_t_scs15_kh_zhalf_t();
    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_& set_scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t();
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    set_scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t();
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    set_scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t();
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    set_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t();
    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_& set_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t();
    scs120_kh_zone_sixteenth_t_l_&                        set_scs120_kh_zone_sixteenth_t();

  private:
    types type_;
    choice_buffer_t<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_,
                    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_,
                    scs120_kh_zone_sixteenth_t_l_,
                    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_,
                    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_,
                    scs15_kh_zone_t_l_,
                    scs30_kh_zone_t_scs15_kh_zhalf_t_l_,
                    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>
        c;

    void destroy_();
  };
  using common_search_space_list_ext_r16_l_ = dyn_array<search_space_ext_r16_s>;
  struct sdt_search_space_r17_c_ {
    struct types_opts {
      enum options { new_search_space, existing_search_space, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    sdt_search_space_r17_c_() = default;
    sdt_search_space_r17_c_(const sdt_search_space_r17_c_& other);
    sdt_search_space_r17_c_& operator=(const sdt_search_space_r17_c_& other);
    ~sdt_search_space_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    search_space_s& new_search_space()
    {
      assert_choice_type(types::new_search_space, type_, "sdt-SearchSpace-r17");
      return c.get<search_space_s>();
    }
    uint8_t& existing_search_space()
    {
      assert_choice_type(types::existing_search_space, type_, "sdt-SearchSpace-r17");
      return c.get<uint8_t>();
    }
    const search_space_s& new_search_space() const
    {
      assert_choice_type(types::new_search_space, type_, "sdt-SearchSpace-r17");
      return c.get<search_space_s>();
    }
    const uint8_t& existing_search_space() const
    {
      assert_choice_type(types::existing_search_space, type_, "sdt-SearchSpace-r17");
      return c.get<uint8_t>();
    }
    search_space_s& set_new_search_space();
    uint8_t&        set_existing_search_space();

  private:
    types                           type_;
    choice_buffer_t<search_space_s> c;

    void destroy_();
  };
  using common_search_space_list_ext2_r17_l_ = dyn_array<search_space_ext_v1700_s>;
  struct first_pdcch_monitoring_occasion_of_po_v1710_c_ {
    using scs480_kh_zone_eighth_t_l_    = bounded_array<uint16_t, 4>;
    using scs480_kh_zone_sixteenth_t_l_ = bounded_array<uint32_t, 4>;
    struct types_opts {
      enum options { scs480_kh_zone_eighth_t, scs480_kh_zone_sixteenth_t, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    first_pdcch_monitoring_occasion_of_po_v1710_c_() = default;
    first_pdcch_monitoring_occasion_of_po_v1710_c_(const first_pdcch_monitoring_occasion_of_po_v1710_c_& other);
    first_pdcch_monitoring_occasion_of_po_v1710_c_&
    operator=(const first_pdcch_monitoring_occasion_of_po_v1710_c_& other);
    ~first_pdcch_monitoring_occasion_of_po_v1710_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scs480_kh_zone_eighth_t_l_& scs480_kh_zone_eighth_t()
    {
      assert_choice_type(types::scs480_kh_zone_eighth_t, type_, "firstPDCCH-MonitoringOccasionOfPO-v1710");
      return c.get<scs480_kh_zone_eighth_t_l_>();
    }
    scs480_kh_zone_sixteenth_t_l_& scs480_kh_zone_sixteenth_t()
    {
      assert_choice_type(types::scs480_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO-v1710");
      return c.get<scs480_kh_zone_sixteenth_t_l_>();
    }
    const scs480_kh_zone_eighth_t_l_& scs480_kh_zone_eighth_t() const
    {
      assert_choice_type(types::scs480_kh_zone_eighth_t, type_, "firstPDCCH-MonitoringOccasionOfPO-v1710");
      return c.get<scs480_kh_zone_eighth_t_l_>();
    }
    const scs480_kh_zone_sixteenth_t_l_& scs480_kh_zone_sixteenth_t() const
    {
      assert_choice_type(types::scs480_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO-v1710");
      return c.get<scs480_kh_zone_sixteenth_t_l_>();
    }
    scs480_kh_zone_eighth_t_l_&    set_scs480_kh_zone_eighth_t();
    scs480_kh_zone_sixteenth_t_l_& set_scs480_kh_zone_sixteenth_t();

  private:
    types                                                                      type_;
    choice_buffer_t<scs480_kh_zone_eighth_t_l_, scs480_kh_zone_sixteenth_t_l_> c;

    void destroy_();
  };
  struct pei_cfg_bwp_r17_s_ {
    struct first_pdcch_monitoring_occasion_of_pei_o_r17_c_ {
      using scs15_kh_zone_t_l_                                      = bounded_array<uint8_t, 4>;
      using scs30_kh_zone_t_scs15_kh_zhalf_t_l_                     = bounded_array<uint16_t, 4>;
      using scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_ = bounded_array<uint16_t, 4>;
      using scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_ =
          bounded_array<uint16_t, 4>;
      using scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_ =
          bounded_array<uint16_t, 4>;
      using scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_ =
          bounded_array<uint16_t, 4>;
      using scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_ = bounded_array<uint16_t, 4>;
      using scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_                     = bounded_array<uint16_t, 4>;
      using scs480_kh_zone_eighth_t_l_                                             = bounded_array<uint16_t, 4>;
      using scs480_kh_zone_sixteenth_t_l_                                          = bounded_array<uint32_t, 4>;
      struct types_opts {
        enum options {
          scs15_kh_zone_t,
          scs30_kh_zone_t_scs15_kh_zhalf_t,
          scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t,
          scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t,
          scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t,
          scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
          scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t,
          scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t,
          scs480_kh_zone_eighth_t,
          scs480_kh_zone_sixteenth_t,
          nulltype
        } value;

        const char* to_string() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      first_pdcch_monitoring_occasion_of_pei_o_r17_c_() = default;
      first_pdcch_monitoring_occasion_of_pei_o_r17_c_(const first_pdcch_monitoring_occasion_of_pei_o_r17_c_& other);
      first_pdcch_monitoring_occasion_of_pei_o_r17_c_&
      operator=(const first_pdcch_monitoring_occasion_of_pei_o_r17_c_& other);
      ~first_pdcch_monitoring_occasion_of_pei_o_r17_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      scs15_kh_zone_t_l_& scs15_kh_zone_t()
      {
        assert_choice_type(types::scs15_kh_zone_t, type_, "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs15_kh_zone_t_l_>();
      }
      scs30_kh_zone_t_scs15_kh_zhalf_t_l_& scs30_kh_zone_t_scs15_kh_zhalf_t()
      {
        assert_choice_type(types::scs30_kh_zone_t_scs15_kh_zhalf_t, type_, "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
      }
      scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_& scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t()
      {
        assert_choice_type(types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t,
                           type_,
                           "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
      }
      scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
      scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t()
      {
        assert_choice_type(types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t,
                           type_,
                           "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
      }
      scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
      scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t()
      {
        assert_choice_type(
            types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t,
            type_,
            "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
      }
      scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
      scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t()
      {
        assert_choice_type(
            types::scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
            type_,
            "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
      }
      scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_&
      scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t()
      {
        assert_choice_type(types::scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t,
                           type_,
                           "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
      }
      scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_& scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t()
      {
        assert_choice_type(
            types::scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_>();
      }
      scs480_kh_zone_eighth_t_l_& scs480_kh_zone_eighth_t()
      {
        assert_choice_type(types::scs480_kh_zone_eighth_t, type_, "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs480_kh_zone_eighth_t_l_>();
      }
      scs480_kh_zone_sixteenth_t_l_& scs480_kh_zone_sixteenth_t()
      {
        assert_choice_type(types::scs480_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs480_kh_zone_sixteenth_t_l_>();
      }
      const scs15_kh_zone_t_l_& scs15_kh_zone_t() const
      {
        assert_choice_type(types::scs15_kh_zone_t, type_, "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs15_kh_zone_t_l_>();
      }
      const scs30_kh_zone_t_scs15_kh_zhalf_t_l_& scs30_kh_zone_t_scs15_kh_zhalf_t() const
      {
        assert_choice_type(types::scs30_kh_zone_t_scs15_kh_zhalf_t, type_, "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
      }
      const scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_&
      scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t() const
      {
        assert_choice_type(types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t,
                           type_,
                           "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
      }
      const scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
      scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t() const
      {
        assert_choice_type(types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t,
                           type_,
                           "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
      }
      const scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
      scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t() const
      {
        assert_choice_type(
            types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t,
            type_,
            "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
      }
      const scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
      scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t() const
      {
        assert_choice_type(
            types::scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
            type_,
            "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
      }
      const scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_&
      scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t() const
      {
        assert_choice_type(types::scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t,
                           type_,
                           "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
      }
      const scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_& scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t() const
      {
        assert_choice_type(
            types::scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_>();
      }
      const scs480_kh_zone_eighth_t_l_& scs480_kh_zone_eighth_t() const
      {
        assert_choice_type(types::scs480_kh_zone_eighth_t, type_, "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs480_kh_zone_eighth_t_l_>();
      }
      const scs480_kh_zone_sixteenth_t_l_& scs480_kh_zone_sixteenth_t() const
      {
        assert_choice_type(types::scs480_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPEI-O-r17");
        return c.get<scs480_kh_zone_sixteenth_t_l_>();
      }
      scs15_kh_zone_t_l_&                  set_scs15_kh_zone_t();
      scs30_kh_zone_t_scs15_kh_zhalf_t_l_& set_scs30_kh_zone_t_scs15_kh_zhalf_t();
      scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_&
      set_scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t();
      scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
      set_scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t();
      scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
      set_scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t();
      scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
      set_scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t();
      scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_&
      set_scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t();
      scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_& set_scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t();
      scs480_kh_zone_eighth_t_l_&                         set_scs480_kh_zone_eighth_t();
      scs480_kh_zone_sixteenth_t_l_&                      set_scs480_kh_zone_sixteenth_t();

    private:
      types type_;
      choice_buffer_t<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_,
                      scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_,
                      scs15_kh_zone_t_l_,
                      scs30_kh_zone_t_scs15_kh_zhalf_t_l_,
                      scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_,
                      scs480_kh_zone_eighth_t_l_,
                      scs480_kh_zone_sixteenth_t_l_,
                      scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_,
                      scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_,
                      scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>
          c;

      void destroy_();
    };

    // member variables
    uint8_t                                         pei_search_space_r17 = 0;
    first_pdcch_monitoring_occasion_of_pei_o_r17_c_ first_pdcch_monitoring_occasion_of_pei_o_r17;
  };

  // member variables
  bool                        ext                                 = false;
  bool                        coreset_zero_present                = false;
  bool                        common_coreset_present              = false;
  bool                        search_space_zero_present           = false;
  bool                        search_space_sib1_present           = false;
  bool                        search_space_other_sys_info_present = false;
  bool                        paging_search_space_present         = false;
  bool                        ra_search_space_present             = false;
  uint8_t                     coreset_zero                        = 0;
  coreset_s                   common_coreset;
  uint8_t                     search_space_zero = 0;
  common_search_space_list_l_ common_search_space_list;
  uint8_t                     search_space_sib1           = 0;
  uint8_t                     search_space_other_sys_info = 0;
  uint8_t                     paging_search_space         = 0;
  uint8_t                     ra_search_space             = 0;
  // ...
  // group 0
  copy_ptr<first_pdcch_monitoring_occasion_of_po_c_> first_pdcch_monitoring_occasion_of_po;
  // group 1
  copy_ptr<common_search_space_list_ext_r16_l_> common_search_space_list_ext_r16;
  // group 2
  bool                                                     search_space_mcch_r17_present = false;
  bool                                                     search_space_mtch_r17_present = false;
  copy_ptr<sdt_search_space_r17_c_>                        sdt_search_space_r17;
  uint8_t                                                  search_space_mcch_r17 = 0;
  uint8_t                                                  search_space_mtch_r17 = 0;
  copy_ptr<common_search_space_list_ext2_r17_l_>           common_search_space_list_ext2_r17;
  copy_ptr<first_pdcch_monitoring_occasion_of_po_v1710_c_> first_pdcch_monitoring_occasion_of_po_v1710;
  copy_ptr<pei_cfg_bwp_r17_s_>                             pei_cfg_bwp_r17;
  // group 3
  bool follow_unified_tci_state_v1720_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-ConfigCommon ::= SEQUENCE
struct pdsch_cfg_common_s {
  bool                               ext = false;
  pdsch_time_domain_res_alloc_list_l pdsch_time_domain_alloc_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-ConfigCommon ::= SEQUENCE
struct pucch_cfg_common_s {
  struct pucch_group_hop_opts {
    enum options { neither, enable, disable, nulltype } value;

    const char* to_string() const;
  };
  using pucch_group_hop_e_ = enumerated<pucch_group_hop_opts>;
  struct intra_slot_fh_r17_opts {
    enum options { from_lower_edge, from_upper_edge, nulltype } value;

    const char* to_string() const;
  };
  using intra_slot_fh_r17_e_ = enumerated<intra_slot_fh_r17_opts>;
  struct add_prb_offset_r17_opts {
    enum options { n2, n3, n4, n6, n8, n9, n10, n12, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using add_prb_offset_r17_e_ = enumerated<add_prb_offset_r17_opts>;

  // member variables
  bool               ext                      = false;
  bool               pucch_res_common_present = false;
  bool               hop_id_present           = false;
  bool               p0_nominal_present       = false;
  uint8_t            pucch_res_common         = 0;
  pucch_group_hop_e_ pucch_group_hop;
  uint16_t           hop_id     = 0;
  int16_t            p0_nominal = -202;
  // ...
  // group 0
  bool                  nrof_prbs_present                    = false;
  bool                  intra_slot_fh_r17_present            = false;
  bool                  pucch_res_common_red_cap_r17_present = false;
  bool                  add_prb_offset_r17_present           = false;
  uint8_t               nrof_prbs                            = 1;
  intra_slot_fh_r17_e_  intra_slot_fh_r17;
  uint8_t               pucch_res_common_red_cap_r17 = 0;
  add_prb_offset_r17_e_ add_prb_offset_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-ConfigCommon ::= SEQUENCE
struct pusch_cfg_common_s {
  bool                               ext                                           = false;
  bool                               group_hop_enabled_transform_precoding_present = false;
  bool                               msg3_delta_preamb_present                     = false;
  bool                               p0_nominal_with_grant_present                 = false;
  pusch_time_domain_res_alloc_list_l pusch_time_domain_alloc_list;
  int8_t                             msg3_delta_preamb     = -1;
  int16_t                            p0_nominal_with_grant = -202;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingCycle ::= ENUMERATED
struct paging_cycle_opts {
  enum options { rf32, rf64, rf128, rf256, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using paging_cycle_e = enumerated<paging_cycle_opts>;

// SCS-SpecificCarrier ::= SEQUENCE
struct scs_specific_carrier_s {
  bool                 ext               = false;
  uint16_t             offset_to_carrier = 0;
  subcarrier_spacing_e subcarrier_spacing;
  uint16_t             carrier_bw = 1;
  // ...
  // group 0
  bool     tx_direct_current_location_present = false;
  uint16_t tx_direct_current_location         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SubgroupConfig-r17 ::= SEQUENCE
struct subgroup_cfg_r17_s {
  bool    ext                                 = false;
  bool    subgroups_num_for_ue_id_r17_present = false;
  uint8_t subgroups_num_per_po_r17            = 1;
  uint8_t subgroups_num_for_ue_id_r17         = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCCH-Config ::= SEQUENCE
struct bcch_cfg_s {
  struct mod_period_coeff_opts {
    enum options { n2, n4, n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mod_period_coeff_e_ = enumerated<mod_period_coeff_opts>;

  // member variables
  bool                ext = false;
  mod_period_coeff_e_ mod_period_coeff;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP-DownlinkCommon ::= SEQUENCE
struct bwp_dl_common_s {
  bool                                ext                      = false;
  bool                                pdcch_cfg_common_present = false;
  bool                                pdsch_cfg_common_present = false;
  bwp_s                               generic_params;
  setup_release_c<pdcch_cfg_common_s> pdcch_cfg_common;
  setup_release_c<pdsch_cfg_common_s> pdsch_cfg_common;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP-UplinkCommon ::= SEQUENCE
struct bwp_ul_common_s {
  using nof_msg3_repeats_list_r17_l_ = std::array<nof_msg3_repeats_r17_e, 4>;
  using mcs_msg3_repeats_r17_l_      = std::array<uint8_t, 8>;

  // member variables
  bool                                ext                      = false;
  bool                                rach_cfg_common_present  = false;
  bool                                pusch_cfg_common_present = false;
  bool                                pucch_cfg_common_present = false;
  bwp_s                               generic_params;
  setup_release_c<rach_cfg_common_s>  rach_cfg_common;
  setup_release_c<pusch_cfg_common_s> pusch_cfg_common;
  setup_release_c<pucch_cfg_common_s> pucch_cfg_common;
  // ...
  // group 0
  bool                                              use_interlace_pucch_pusch_r16_present = false;
  copy_ptr<setup_release_c<rach_cfg_common_s>>      rach_cfg_common_iab_r16;
  copy_ptr<setup_release_c<msg_a_cfg_common_r16_s>> msg_a_cfg_common_r16;
  // group 1
  bool enable_ra_prioritization_for_slicing_r17_present = false;
  bool rsrp_thres_msg3_r17_present                      = false;
  bool enable_ra_prioritization_for_slicing_r17         = false;
  copy_ptr<setup_release_c<dyn_seq_of<add_rach_cfg_r17_s, 1, 256>>> add_rach_cfg_list_r17;
  uint8_t                                                           rsrp_thres_msg3_r17 = 0;
  copy_ptr<nof_msg3_repeats_list_r17_l_>                            nof_msg3_repeats_list_r17;
  copy_ptr<mcs_msg3_repeats_r17_l_>                                 mcs_msg3_repeats_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FrequencyInfoDL-SIB ::= SEQUENCE
struct freq_info_dl_sib_s {
  using scs_specific_carrier_list_l_ = dyn_array<scs_specific_carrier_s>;

  // member variables
  multi_freq_band_list_nr_sib_l freq_band_list;
  uint16_t                      offset_to_point_a = 0;
  scs_specific_carrier_list_l_  scs_specific_carrier_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FrequencyInfoUL-SIB ::= SEQUENCE
struct freq_info_ul_sib_s {
  using scs_specific_carrier_list_l_ = dyn_array<scs_specific_carrier_s>;

  // member variables
  bool                          ext                           = false;
  bool                          absolute_freq_point_a_present = false;
  bool                          p_max_present                 = false;
  bool                          freq_shift7p5khz_present      = false;
  multi_freq_band_list_nr_sib_l freq_band_list;
  uint32_t                      absolute_freq_point_a = 0;
  scs_specific_carrier_list_l_  scs_specific_carrier_list;
  int8_t                        p_max = -30;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCCH-Config ::= SEQUENCE
struct pcch_cfg_s {
  struct nand_paging_frame_offset_c_ {
    struct types_opts {
      enum options { one_t, half_t, quarter_t, one_eighth_t, one_sixteenth_t, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    nand_paging_frame_offset_c_() = default;
    nand_paging_frame_offset_c_(const nand_paging_frame_offset_c_& other);
    nand_paging_frame_offset_c_& operator=(const nand_paging_frame_offset_c_& other);
    ~nand_paging_frame_offset_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& half_t()
    {
      assert_choice_type(types::half_t, type_, "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    uint8_t& quarter_t()
    {
      assert_choice_type(types::quarter_t, type_, "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    uint8_t& one_eighth_t()
    {
      assert_choice_type(types::one_eighth_t, type_, "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    uint8_t& one_sixteenth_t()
    {
      assert_choice_type(types::one_sixteenth_t, type_, "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& half_t() const
    {
      assert_choice_type(types::half_t, type_, "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& quarter_t() const
    {
      assert_choice_type(types::quarter_t, type_, "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& one_eighth_t() const
    {
      assert_choice_type(types::one_eighth_t, type_, "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& one_sixteenth_t() const
    {
      assert_choice_type(types::one_sixteenth_t, type_, "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    void     set_one_t();
    uint8_t& set_half_t();
    uint8_t& set_quarter_t();
    uint8_t& set_one_eighth_t();
    uint8_t& set_one_sixteenth_t();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct ns_opts {
    enum options { four, two, one, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ns_e_ = enumerated<ns_opts>;
  struct first_pdcch_monitoring_occasion_of_po_c_ {
    using scs15_kh_zone_t_l_                                                              = bounded_array<uint8_t, 4>;
    using scs30_kh_zone_t_scs15_kh_zhalf_t_l_                                             = bounded_array<uint16_t, 4>;
    using scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_                         = bounded_array<uint16_t, 4>;
    using scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_ = bounded_array<uint16_t, 4>;
    using scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_ =
        bounded_array<uint16_t, 4>;
    using scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_ =
        bounded_array<uint16_t, 4>;
    using scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_ = bounded_array<uint16_t, 4>;
    using scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_                     = bounded_array<uint16_t, 4>;
    struct types_opts {
      enum options {
        scs15_kh_zone_t,
        scs30_kh_zone_t_scs15_kh_zhalf_t,
        scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t,
        scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t,
        scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t,
        scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
        scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t,
        scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    first_pdcch_monitoring_occasion_of_po_c_() = default;
    first_pdcch_monitoring_occasion_of_po_c_(const first_pdcch_monitoring_occasion_of_po_c_& other);
    first_pdcch_monitoring_occasion_of_po_c_& operator=(const first_pdcch_monitoring_occasion_of_po_c_& other);
    ~first_pdcch_monitoring_occasion_of_po_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scs15_kh_zone_t_l_& scs15_kh_zone_t()
    {
      assert_choice_type(types::scs15_kh_zone_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs15_kh_zone_t_l_>();
    }
    scs30_kh_zone_t_scs15_kh_zhalf_t_l_& scs30_kh_zone_t_scs15_kh_zhalf_t()
    {
      assert_choice_type(types::scs30_kh_zone_t_scs15_kh_zhalf_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
    }
    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_& scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t()
    {
      assert_choice_type(
          types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
    }
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t()
    {
      assert_choice_type(types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
    }
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t()
    {
      assert_choice_type(types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
    }
    scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t()
    {
      assert_choice_type(types::scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
    }
    scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_&
    scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t()
    {
      assert_choice_type(types::scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
    }
    scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_& scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t()
    {
      assert_choice_type(
          types::scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_>();
    }
    const scs15_kh_zone_t_l_& scs15_kh_zone_t() const
    {
      assert_choice_type(types::scs15_kh_zone_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs15_kh_zone_t_l_>();
    }
    const scs30_kh_zone_t_scs15_kh_zhalf_t_l_& scs30_kh_zone_t_scs15_kh_zhalf_t() const
    {
      assert_choice_type(types::scs30_kh_zone_t_scs15_kh_zhalf_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
    }
    const scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_&
    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t() const
    {
      assert_choice_type(
          types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
    }
    const scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t() const
    {
      assert_choice_type(types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
    }
    const scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t() const
    {
      assert_choice_type(types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
    }
    const scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t() const
    {
      assert_choice_type(types::scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
    }
    const scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_&
    scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t() const
    {
      assert_choice_type(types::scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
    }
    const scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_& scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t() const
    {
      assert_choice_type(
          types::scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_>();
    }
    scs15_kh_zone_t_l_&                                      set_scs15_kh_zone_t();
    scs30_kh_zone_t_scs15_kh_zhalf_t_l_&                     set_scs30_kh_zone_t_scs15_kh_zhalf_t();
    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_& set_scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t();
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    set_scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t();
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    set_scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t();
    scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    set_scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t();
    scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_&
    set_scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t();
    scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_& set_scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t();

  private:
    types type_;
    choice_buffer_t<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_,
                    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_,
                    scs15_kh_zone_t_l_,
                    scs30_kh_zone_t_scs15_kh_zhalf_t_l_,
                    scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_,
                    scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_,
                    scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_,
                    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>
        c;

    void destroy_();
  };
  struct first_pdcch_monitoring_occasion_of_po_v1710_c_ {
    using scs480_kh_zone_eighth_t_l_    = bounded_array<uint16_t, 4>;
    using scs480_kh_zone_sixteenth_t_l_ = bounded_array<uint32_t, 4>;
    struct types_opts {
      enum options { scs480_kh_zone_eighth_t, scs480_kh_zone_sixteenth_t, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    first_pdcch_monitoring_occasion_of_po_v1710_c_() = default;
    first_pdcch_monitoring_occasion_of_po_v1710_c_(const first_pdcch_monitoring_occasion_of_po_v1710_c_& other);
    first_pdcch_monitoring_occasion_of_po_v1710_c_&
    operator=(const first_pdcch_monitoring_occasion_of_po_v1710_c_& other);
    ~first_pdcch_monitoring_occasion_of_po_v1710_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scs480_kh_zone_eighth_t_l_& scs480_kh_zone_eighth_t()
    {
      assert_choice_type(types::scs480_kh_zone_eighth_t, type_, "firstPDCCH-MonitoringOccasionOfPO-v1710");
      return c.get<scs480_kh_zone_eighth_t_l_>();
    }
    scs480_kh_zone_sixteenth_t_l_& scs480_kh_zone_sixteenth_t()
    {
      assert_choice_type(types::scs480_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO-v1710");
      return c.get<scs480_kh_zone_sixteenth_t_l_>();
    }
    const scs480_kh_zone_eighth_t_l_& scs480_kh_zone_eighth_t() const
    {
      assert_choice_type(types::scs480_kh_zone_eighth_t, type_, "firstPDCCH-MonitoringOccasionOfPO-v1710");
      return c.get<scs480_kh_zone_eighth_t_l_>();
    }
    const scs480_kh_zone_sixteenth_t_l_& scs480_kh_zone_sixteenth_t() const
    {
      assert_choice_type(types::scs480_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO-v1710");
      return c.get<scs480_kh_zone_sixteenth_t_l_>();
    }
    scs480_kh_zone_eighth_t_l_&    set_scs480_kh_zone_eighth_t();
    scs480_kh_zone_sixteenth_t_l_& set_scs480_kh_zone_sixteenth_t();

  private:
    types                                                                      type_;
    choice_buffer_t<scs480_kh_zone_eighth_t_l_, scs480_kh_zone_sixteenth_t_l_> c;

    void destroy_();
  };

  // member variables
  bool                                     ext                                           = false;
  bool                                     first_pdcch_monitoring_occasion_of_po_present = false;
  paging_cycle_e                           default_paging_cycle;
  nand_paging_frame_offset_c_              nand_paging_frame_offset;
  ns_e_                                    ns;
  first_pdcch_monitoring_occasion_of_po_c_ first_pdcch_monitoring_occasion_of_po;
  // ...
  // group 0
  bool    nrof_pdcch_monitoring_occasion_per_ssb_in_po_r16_present = false;
  uint8_t nrof_pdcch_monitoring_occasion_per_ssb_in_po_r16         = 2;
  // group 1
  bool                                                     ran_paging_in_idle_po_r17_present = false;
  copy_ptr<first_pdcch_monitoring_occasion_of_po_v1710_c_> first_pdcch_monitoring_occasion_of_po_v1710;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PEI-Config-r17 ::= SEQUENCE
struct pei_cfg_r17_s {
  struct po_num_per_pei_r17_opts {
    enum options { po1, po2, po4, po8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using po_num_per_pei_r17_e_ = enumerated<po_num_per_pei_r17_opts>;

  // member variables
  bool                  ext                             = false;
  bool                  last_used_cell_only_r17_present = false;
  po_num_per_pei_r17_e_ po_num_per_pei_r17;
  uint8_t               payload_size_dci_2_7_r17 = 1;
  uint8_t               pei_frame_offset_r17     = 0;
  subgroup_cfg_r17_s    subgroup_cfg_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-UL-DL-Pattern ::= SEQUENCE
struct tdd_ul_dl_pattern_s {
  struct dl_ul_tx_periodicity_opts {
    enum options { ms0p5, ms0p625, ms1, ms1p25, ms2, ms2p5, ms5, ms10, nulltype } value;

    const char* to_string() const;
  };
  using dl_ul_tx_periodicity_e_ = enumerated<dl_ul_tx_periodicity_opts>;
  struct dl_ul_tx_periodicity_v1530_opts {
    enum options { ms3, ms4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using dl_ul_tx_periodicity_v1530_e_ = enumerated<dl_ul_tx_periodicity_v1530_opts>;

  // member variables
  bool                    ext = false;
  dl_ul_tx_periodicity_e_ dl_ul_tx_periodicity;
  uint16_t                nrof_dl_slots   = 0;
  uint8_t                 nrof_dl_symbols = 0;
  uint16_t                nrof_ul_slots   = 0;
  uint8_t                 nrof_ul_symbols = 0;
  // ...
  // group 0
  bool                          dl_ul_tx_periodicity_v1530_present = false;
  dl_ul_tx_periodicity_v1530_e_ dl_ul_tx_periodicity_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TimeAlignmentTimer ::= ENUMERATED
struct time_align_timer_opts {
  enum options { ms500, ms750, ms1280, ms1920, ms2560, ms5120, ms10240, infinity, nulltype } value;
  typedef int16_t number_type;

  const char* to_string() const;
  int16_t     to_number() const;
};
using time_align_timer_e = enumerated<time_align_timer_opts>;

// DownlinkConfigCommonSIB ::= SEQUENCE
struct dl_cfg_common_sib_s {
  bool               ext = false;
  freq_info_dl_sib_s freq_info_dl;
  bwp_dl_common_s    init_dl_bwp;
  bcch_cfg_s         bcch_cfg;
  pcch_cfg_s         pcch_cfg;
  // ...
  // group 0
  copy_ptr<pei_cfg_r17_s>   pei_cfg_r17;
  copy_ptr<bwp_dl_common_s> init_dl_bwp_red_cap_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HighSpeedConfig-r16 ::= SEQUENCE
struct high_speed_cfg_r16_s {
  bool ext                               = false;
  bool high_speed_meas_flag_r16_present  = false;
  bool high_speed_demod_flag_r16_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HighSpeedConfigFR2-r17 ::= SEQUENCE
struct high_speed_cfg_fr2_r17_s {
  struct high_speed_meas_flag_fr2_r17_opts {
    enum options { set1, set2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using high_speed_meas_flag_fr2_r17_e_ = enumerated<high_speed_meas_flag_fr2_r17_opts>;
  struct high_speed_deployment_type_fr2_r17_opts {
    enum options { unidirectional, bidirectional, nulltype } value;

    const char* to_string() const;
  };
  using high_speed_deployment_type_fr2_r17_e_ = enumerated<high_speed_deployment_type_fr2_r17_opts>;

  // member variables
  bool                                  ext                                                 = false;
  bool                                  high_speed_meas_flag_fr2_r17_present                = false;
  bool                                  high_speed_deployment_type_fr2_r17_present          = false;
  bool                                  high_speed_large_one_step_ul_timing_fr2_r17_present = false;
  high_speed_meas_flag_fr2_r17_e_       high_speed_meas_flag_fr2_r17;
  high_speed_deployment_type_fr2_r17_e_ high_speed_deployment_type_fr2_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SemiStaticChannelAccessConfig-r16 ::= SEQUENCE
struct semi_static_ch_access_cfg_r16_s {
  struct period_opts {
    enum options { ms1, ms2, ms2dot5, ms4, ms5, ms10, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using period_e_ = enumerated<period_opts>;

  // member variables
  period_e_ period;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-UL-DL-ConfigCommon ::= SEQUENCE
struct tdd_ul_dl_cfg_common_s {
  bool                 ext              = false;
  bool                 pattern2_present = false;
  subcarrier_spacing_e ref_subcarrier_spacing;
  tdd_ul_dl_pattern_s  pattern1;
  tdd_ul_dl_pattern_s  pattern2;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkConfigCommonSIB ::= SEQUENCE
struct ul_cfg_common_sib_s {
  freq_info_ul_sib_s freq_info_ul;
  bwp_ul_common_s    init_ul_bwp;
  time_align_timer_e time_align_timer_common;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkConfigCommonSIB-v1700 ::= SEQUENCE
struct ul_cfg_common_sib_v1700_s {
  bool            init_ul_bwp_red_cap_r17_present = false;
  bwp_ul_common_s init_ul_bwp_red_cap_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServingCellConfigCommonSIB ::= SEQUENCE
struct serving_cell_cfg_common_sib_s {
  struct n_timing_advance_offset_opts {
    enum options { n0, n25600, n39936, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using n_timing_advance_offset_e_ = enumerated<n_timing_advance_offset_opts>;
  struct ssb_positions_in_burst_s_ {
    bool               group_presence_present = false;
    fixed_bitstring<8> in_one_group;
    fixed_bitstring<8> group_presence;
  };
  struct ssb_periodicity_serving_cell_opts {
    enum options { ms5, ms10, ms20, ms40, ms80, ms160, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ssb_periodicity_serving_cell_e_ = enumerated<ssb_periodicity_serving_cell_opts>;
  struct ch_access_mode_r16_c_ {
    struct types_opts {
      enum options { dyn, semi_static, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ch_access_mode_r16_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    semi_static_ch_access_cfg_r16_s& semi_static()
    {
      assert_choice_type(types::semi_static, type_, "channelAccessMode-r16");
      return c;
    }
    const semi_static_ch_access_cfg_r16_s& semi_static() const
    {
      assert_choice_type(types::semi_static, type_, "channelAccessMode-r16");
      return c;
    }
    void                             set_dyn();
    semi_static_ch_access_cfg_r16_s& set_semi_static();

  private:
    types                           type_;
    semi_static_ch_access_cfg_r16_s c;
  };
  struct discovery_burst_win_len_r16_opts {
    enum options { ms0dot5, ms1, ms2, ms3, ms4, ms5, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using discovery_burst_win_len_r16_e_ = enumerated<discovery_burst_win_len_r16_opts>;
  struct discovery_burst_win_len_v1700_opts {
    enum options { ms0dot125, ms0dot25, ms0dot5, ms0dot75, ms1, ms1dot25, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using discovery_burst_win_len_v1700_e_ = enumerated<discovery_burst_win_len_v1700_opts>;

  // member variables
  bool                            ext                             = false;
  bool                            ul_cfg_common_present           = false;
  bool                            supplementary_ul_present        = false;
  bool                            n_timing_advance_offset_present = false;
  bool                            tdd_ul_dl_cfg_common_present    = false;
  dl_cfg_common_sib_s             dl_cfg_common;
  ul_cfg_common_sib_s             ul_cfg_common;
  ul_cfg_common_sib_s             supplementary_ul;
  n_timing_advance_offset_e_      n_timing_advance_offset;
  ssb_positions_in_burst_s_       ssb_positions_in_burst;
  ssb_periodicity_serving_cell_e_ ssb_periodicity_serving_cell;
  tdd_ul_dl_cfg_common_s          tdd_ul_dl_cfg_common;
  int8_t                          ss_pbch_block_pwr = -60;
  // ...
  // group 0
  bool                            discovery_burst_win_len_r16_present = false;
  copy_ptr<ch_access_mode_r16_c_> ch_access_mode_r16;
  discovery_burst_win_len_r16_e_  discovery_burst_win_len_r16;
  copy_ptr<high_speed_cfg_r16_s>  high_speed_cfg_r16;
  // group 1
  bool                                ch_access_mode2_r17_present           = false;
  bool                                discovery_burst_win_len_v1700_present = false;
  discovery_burst_win_len_v1700_e_    discovery_burst_win_len_v1700;
  copy_ptr<high_speed_cfg_fr2_r17_s>  high_speed_cfg_fr2_r17;
  copy_ptr<ul_cfg_common_sib_v1700_s> ul_cfg_common_v1700;
  // group 2
  bool enhanced_meas_leo_r17_present = false;
  // group 3
  bool ra_ch_access_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NonCellDefiningSSB-r17 ::= SEQUENCE
struct non_cell_defining_ssb_r17_s {
  struct ssb_periodicity_r17_opts {
    enum options { ms5, ms10, ms20, ms40, ms80, ms160, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ssb_periodicity_r17_e_ = enumerated<ssb_periodicity_r17_opts>;
  struct ssb_time_offset_r17_opts {
    enum options { ms5, ms10, ms15, ms20, ms40, ms80, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ssb_time_offset_r17_e_ = enumerated<ssb_time_offset_r17_opts>;

  // member variables
  bool                   ext                         = false;
  bool                   ssb_periodicity_r17_present = false;
  bool                   ssb_time_offset_r17_present = false;
  uint32_t               absolute_freq_ssb_r17       = 0;
  ssb_periodicity_r17_e_ ssb_periodicity_r17;
  ssb_time_offset_r17_e_ ssb_time_offset_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AvailabilityCombination-r16 ::= SEQUENCE
struct availability_combination_r16_s {
  using res_availability_r16_l_ = dyn_array<uint8_t>;

  // member variables
  uint16_t                availability_combination_id_r16 = 0;
  res_availability_r16_l_ res_availability_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RB-SetGroup-r17 ::= SEQUENCE
struct rb_set_group_r17_s {
  using res_availability_r17_l_ = dyn_array<uint8_t>;
  using rb_sets_r17_l_          = bounded_array<uint8_t, 8>;

  // member variables
  res_availability_r17_l_ res_availability_r17;
  rb_sets_r17_l_          rb_sets_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AvailabilityCombinationRB-Groups-r17 ::= SEQUENCE
struct availability_combination_rb_groups_r17_s {
  using rb_set_groups_r17_l_    = dyn_array<rb_set_group_r17_s>;
  using res_availability_r17_l_ = dyn_array<uint8_t>;

  // member variables
  uint16_t                availability_combination_id_r17 = 0;
  rb_set_groups_r17_l_    rb_set_groups_r17;
  res_availability_r17_l_ res_availability_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AvailabilityCombinationsPerCell-r16 ::= SEQUENCE
struct availability_combinations_per_cell_r16_s {
  using availability_combinations_r16_l_           = dyn_array<availability_combination_r16_s>;
  using availability_combinations_rb_groups_r17_l_ = dyn_array<availability_combination_rb_groups_r17_s>;

  // member variables
  bool                             ext                                        = false;
  bool                             position_in_dci_ai_r16_present             = false;
  uint16_t                         availability_combinations_per_cell_idx_r16 = 0;
  fixed_bitstring<36>              iab_du_cell_id_r16;
  uint8_t                          position_in_dci_ai_r16 = 0;
  availability_combinations_r16_l_ availability_combinations_r16;
  // ...
  // group 0
  copy_ptr<availability_combinations_rb_groups_r17_l_> availability_combinations_rb_groups_r17;
  // group 1
  bool    position_in_dci_ai_rb_groups_v1720_present = false;
  uint8_t position_in_dci_ai_rb_groups_v1720         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AvailabilityIndicator-r16 ::= SEQUENCE
struct availability_ind_r16_s {
  using available_comb_to_add_mod_list_r16_l_ = dyn_array<availability_combinations_per_cell_r16_s>;
  using available_comb_to_release_list_r16_l_ = dyn_array<uint16_t>;

  // member variables
  bool                                  ext                     = false;
  uint32_t                              ai_rnti_r16             = 0;
  uint8_t                               dci_payload_size_ai_r16 = 1;
  available_comb_to_add_mod_list_r16_l_ available_comb_to_add_mod_list_r16;
  available_comb_to_release_list_r16_l_ available_comb_to_release_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AvailableRB-SetsPerCell-r16 ::= SEQUENCE
struct available_rb_sets_per_cell_r16_s {
  uint8_t serving_cell_id_r16 = 0;
  uint8_t position_in_dci_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BFR-CSIRS-Resource ::= SEQUENCE
struct bfr_csi_rs_res_s {
  using ra_occasion_list_l_ = dyn_array<uint16_t>;

  // member variables
  bool                ext                   = false;
  bool                ra_preamb_idx_present = false;
  uint8_t             csi_rs                = 0;
  ra_occasion_list_l_ ra_occasion_list;
  uint8_t             ra_preamb_idx = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BFR-SSB-Resource ::= SEQUENCE
struct bfr_ssb_res_s {
  bool    ext           = false;
  uint8_t ssb           = 0;
  uint8_t ra_preamb_idx = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-FrequencyOccupation ::= SEQUENCE
struct csi_freq_occupation_s {
  bool     ext      = false;
  uint16_t start_rb = 0;
  uint16_t nrof_rbs = 24;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BeamLinkMonitoringRS-r17 ::= SEQUENCE
struct beam_link_monitoring_rs_r17_s {
  struct detection_res_r17_c_ {
    struct types_opts {
      enum options { ssb_idx, csi_rs_idx, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    detection_res_r17_c_() = default;
    detection_res_r17_c_(const detection_res_r17_c_& other);
    detection_res_r17_c_& operator=(const detection_res_r17_c_& other);
    ~detection_res_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx()
    {
      assert_choice_type(types::ssb_idx, type_, "detectionResource-r17");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx()
    {
      assert_choice_type(types::csi_rs_idx, type_, "detectionResource-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_idx() const
    {
      assert_choice_type(types::ssb_idx, type_, "detectionResource-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx() const
    {
      assert_choice_type(types::csi_rs_idx, type_, "detectionResource-r17");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_idx();
    uint8_t& set_csi_rs_idx();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                 ext                            = false;
  uint8_t              beam_link_monitoring_rs_id_r17 = 0;
  detection_res_r17_c_ detection_res_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CI-ConfigurationPerServingCell-r16 ::= SEQUENCE
struct ci_cfg_per_serving_cell_r16_s {
  struct ci_payload_size_r16_opts {
    enum options { n1, n2, n4, n5, n7, n8, n10, n14, n16, n20, n28, n32, n35, n42, n56, n112, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ci_payload_size_r16_e_ = enumerated<ci_payload_size_r16_opts>;
  struct time_freq_region_r16_s_ {
    struct time_dur_for_ci_r16_opts {
      enum options { n2, n4, n7, n14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using time_dur_for_ci_r16_e_ = enumerated<time_dur_for_ci_r16_opts>;
    struct time_granularity_for_ci_r16_opts {
      enum options { n1, n2, n4, n7, n14, n28, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using time_granularity_for_ci_r16_e_ = enumerated<time_granularity_for_ci_r16_opts>;

    // member variables
    bool                           ext                         = false;
    bool                           time_dur_for_ci_r16_present = false;
    time_dur_for_ci_r16_e_         time_dur_for_ci_r16;
    time_granularity_for_ci_r16_e_ time_granularity_for_ci_r16;
    uint16_t                       freq_region_for_ci_r16 = 0;
    uint8_t                        delta_offset_r16       = 0;
    // ...
  };

  // member variables
  bool                    position_in_dci_for_sul_r16_present = false;
  bool                    ul_cancellation_prio_v1610_present  = false;
  uint8_t                 serving_cell_id                     = 0;
  uint8_t                 position_in_dci_r16                 = 0;
  uint8_t                 position_in_dci_for_sul_r16         = 0;
  ci_payload_size_r16_e_  ci_payload_size_r16;
  time_freq_region_r16_s_ time_freq_region_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-RS-ResourceMapping ::= SEQUENCE
struct csi_rs_res_map_s {
  struct freq_domain_alloc_c_ {
    struct types_opts {
      enum options { row1, row2, row4, other, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    freq_domain_alloc_c_() = default;
    freq_domain_alloc_c_(const freq_domain_alloc_c_& other);
    freq_domain_alloc_c_& operator=(const freq_domain_alloc_c_& other);
    ~freq_domain_alloc_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<4>& row1()
    {
      assert_choice_type(types::row1, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<4>>();
    }
    fixed_bitstring<12>& row2()
    {
      assert_choice_type(types::row2, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<12>>();
    }
    fixed_bitstring<3>& row4()
    {
      assert_choice_type(types::row4, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<3>>();
    }
    fixed_bitstring<6>& other()
    {
      assert_choice_type(types::other, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<6>>();
    }
    const fixed_bitstring<4>& row1() const
    {
      assert_choice_type(types::row1, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<4>>();
    }
    const fixed_bitstring<12>& row2() const
    {
      assert_choice_type(types::row2, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<12>>();
    }
    const fixed_bitstring<3>& row4() const
    {
      assert_choice_type(types::row4, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<3>>();
    }
    const fixed_bitstring<6>& other() const
    {
      assert_choice_type(types::other, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<6>>();
    }
    fixed_bitstring<4>&  set_row1();
    fixed_bitstring<12>& set_row2();
    fixed_bitstring<3>&  set_row4();
    fixed_bitstring<6>&  set_other();

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<12>> c;

    void destroy_();
  };
  struct nrof_ports_opts {
    enum options { p1, p2, p4, p8, p12, p16, p24, p32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nrof_ports_e_ = enumerated<nrof_ports_opts>;
  struct cdm_type_opts {
    enum options { no_cdm, fd_cdm2, cdm4_fd2_td2, cdm8_fd2_td4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using cdm_type_e_ = enumerated<cdm_type_opts>;
  struct density_c_ {
    struct dot5_opts {
      enum options { even_prbs, odd_prbs, nulltype } value;

      const char* to_string() const;
    };
    using dot5_e_ = enumerated<dot5_opts>;
    struct types_opts {
      enum options { dot5, one, three, spare, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    density_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dot5_e_& dot5()
    {
      assert_choice_type(types::dot5, type_, "density");
      return c;
    }
    const dot5_e_& dot5() const
    {
      assert_choice_type(types::dot5, type_, "density");
      return c;
    }
    dot5_e_& set_dot5();
    void     set_one();
    void     set_three();
    void     set_spare();

  private:
    types   type_;
    dot5_e_ c;
  };

  // member variables
  bool                  ext                                       = false;
  bool                  first_ofdm_symbol_in_time_domain2_present = false;
  freq_domain_alloc_c_  freq_domain_alloc;
  nrof_ports_e_         nrof_ports;
  uint8_t               first_ofdm_symbol_in_time_domain  = 0;
  uint8_t               first_ofdm_symbol_in_time_domain2 = 2;
  cdm_type_e_           cdm_type;
  density_c_            density;
  csi_freq_occupation_s freq_band;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-ResourcePeriodicityAndOffset ::= CHOICE
struct csi_res_periodicity_and_offset_c {
  struct types_opts {
    enum options {
      slots4,
      slots5,
      slots8,
      slots10,
      slots16,
      slots20,
      slots32,
      slots40,
      slots64,
      slots80,
      slots160,
      slots320,
      slots640,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  csi_res_periodicity_and_offset_c() = default;
  csi_res_periodicity_and_offset_c(const csi_res_periodicity_and_offset_c& other);
  csi_res_periodicity_and_offset_c& operator=(const csi_res_periodicity_and_offset_c& other);
  ~csi_res_periodicity_and_offset_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& slots4()
  {
    assert_choice_type(types::slots4, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots5()
  {
    assert_choice_type(types::slots5, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots8()
  {
    assert_choice_type(types::slots8, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots10()
  {
    assert_choice_type(types::slots10, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots16()
  {
    assert_choice_type(types::slots16, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots20()
  {
    assert_choice_type(types::slots20, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots32()
  {
    assert_choice_type(types::slots32, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots40()
  {
    assert_choice_type(types::slots40, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots64()
  {
    assert_choice_type(types::slots64, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots80()
  {
    assert_choice_type(types::slots80, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots160()
  {
    assert_choice_type(types::slots160, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint16_t& slots320()
  {
    assert_choice_type(types::slots320, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  uint16_t& slots640()
  {
    assert_choice_type(types::slots640, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  const uint8_t& slots4() const
  {
    assert_choice_type(types::slots4, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots5() const
  {
    assert_choice_type(types::slots5, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots8() const
  {
    assert_choice_type(types::slots8, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots10() const
  {
    assert_choice_type(types::slots10, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots16() const
  {
    assert_choice_type(types::slots16, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots20() const
  {
    assert_choice_type(types::slots20, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots32() const
  {
    assert_choice_type(types::slots32, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots40() const
  {
    assert_choice_type(types::slots40, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots64() const
  {
    assert_choice_type(types::slots64, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots80() const
  {
    assert_choice_type(types::slots80, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots160() const
  {
    assert_choice_type(types::slots160, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint16_t& slots320() const
  {
    assert_choice_type(types::slots320, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  const uint16_t& slots640() const
  {
    assert_choice_type(types::slots640, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  uint8_t&  set_slots4();
  uint8_t&  set_slots5();
  uint8_t&  set_slots8();
  uint8_t&  set_slots10();
  uint8_t&  set_slots16();
  uint8_t&  set_slots20();
  uint8_t&  set_slots32();
  uint8_t&  set_slots40();
  uint8_t&  set_slots64();
  uint8_t&  set_slots80();
  uint8_t&  set_slots160();
  uint16_t& set_slots320();
  uint16_t& set_slots640();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// CellGroupForSwitch-r16 ::= SEQUENCE (SIZE (1..16)) OF INTEGER (0..31)
using cell_group_for_switch_r16_l = bounded_array<uint8_t, 16>;

// FDM-TDM-r16 ::= SEQUENCE
struct fdm_tdm_r16_s {
  struct repeat_scheme_r16_opts {
    enum options { fdm_scheme_a, fdm_scheme_b, tdm_scheme_a, nulltype } value;

    const char* to_string() const;
  };
  using repeat_scheme_r16_e_ = enumerated<repeat_scheme_r16_opts>;

  // member variables
  bool                 start_symbol_offset_k_r16_present = false;
  repeat_scheme_r16_e_ repeat_scheme_r16;
  uint8_t              start_symbol_offset_k_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// INT-ConfigurationPerServingCell ::= SEQUENCE
struct int_cfg_per_serving_cell_s {
  uint8_t serving_cell_id = 0;
  uint8_t position_in_dci = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MultiPDSCH-TDRA-r17 ::= SEQUENCE
struct multi_pdsch_tdra_r17_s {
  using pdsch_tdra_list_r17_l_ = dyn_array<pdsch_time_domain_res_alloc_r16_s>;

  // member variables
  bool                   ext = false;
  pdsch_tdra_list_r17_l_ pdsch_tdra_list_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PTRS-DownlinkConfig ::= SEQUENCE
struct ptrs_dl_cfg_s {
  using freq_density_l_ = std::array<uint16_t, 2>;
  using time_density_l_ = std::array<uint8_t, 3>;
  struct res_elem_offset_opts {
    enum options { offset01, offset10, offset11, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using res_elem_offset_e_ = enumerated<res_elem_offset_opts>;
  struct max_nrof_ports_r16_opts {
    enum options { n1, n2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_nrof_ports_r16_e_ = enumerated<max_nrof_ports_r16_opts>;

  // member variables
  bool               ext                     = false;
  bool               freq_density_present    = false;
  bool               time_density_present    = false;
  bool               epre_ratio_present      = false;
  bool               res_elem_offset_present = false;
  freq_density_l_    freq_density;
  time_density_l_    time_density;
  uint8_t            epre_ratio = 0;
  res_elem_offset_e_ res_elem_offset;
  // ...
  // group 0
  bool                  max_nrof_ports_r16_present = false;
  max_nrof_ports_r16_e_ max_nrof_ports_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QCL-Info ::= SEQUENCE
struct qcl_info_s {
  struct ref_sig_c_ {
    struct types_opts {
      enum options { csi_rs, ssb, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ref_sig_c_() = default;
    ref_sig_c_(const ref_sig_c_& other);
    ref_sig_c_& operator=(const ref_sig_c_& other);
    ~ref_sig_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& csi_rs()
    {
      assert_choice_type(types::csi_rs, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& ssb()
    {
      assert_choice_type(types::ssb, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs() const
    {
      assert_choice_type(types::csi_rs, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb() const
    {
      assert_choice_type(types::ssb, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& set_csi_rs();
    uint8_t& set_ssb();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct qcl_type_opts {
    enum options { type_a, type_b, type_c, type_d, nulltype } value;

    const char* to_string() const;
  };
  using qcl_type_e_ = enumerated<qcl_type_opts>;

  // member variables
  bool        ext            = false;
  bool        cell_present   = false;
  bool        bwp_id_present = false;
  uint8_t     cell           = 0;
  uint8_t     bwp_id         = 0;
  ref_sig_c_  ref_sig;
  qcl_type_e_ qcl_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SlotBased-r16 ::= SEQUENCE
struct slot_based_r16_s {
  struct tci_map_r16_opts {
    enum options { cyclic_map, sequential_map, nulltype } value;

    const char* to_string() const;
  };
  using tci_map_r16_e_ = enumerated<tci_map_r16_opts>;

  // member variables
  tci_map_r16_e_ tci_map_r16;
  uint8_t        seq_offset_for_rv_r16 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SlotBased-v1630 ::= SEQUENCE
struct slot_based_v1630_s {
  struct tci_map_r16_opts {
    enum options { cyclic_map, sequential_map, nulltype } value;

    const char* to_string() const;
  };
  using tci_map_r16_e_ = enumerated<tci_map_r16_opts>;

  // member variables
  tci_map_r16_e_ tci_map_r16;
  uint8_t        seq_offset_for_rv_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BeamFailureDetectionSet-r17 ::= SEQUENCE
struct beam_fail_detection_set_r17_s {
  using bfd_res_to_add_mod_list_r17_l_ = dyn_array<beam_link_monitoring_rs_r17_s>;
  using bfd_res_to_release_list_r17_l_ = dyn_array<uint8_t>;
  struct beam_fail_instance_max_count_r17_opts {
    enum options { n1, n2, n3, n4, n5, n6, n8, n10, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using beam_fail_instance_max_count_r17_e_ = enumerated<beam_fail_instance_max_count_r17_opts>;
  struct beam_fail_detection_timer_r17_opts {
    enum options { pbfd1, pbfd2, pbfd3, pbfd4, pbfd5, pbfd6, pbfd8, pbfd10, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using beam_fail_detection_timer_r17_e_ = enumerated<beam_fail_detection_timer_r17_opts>;

  // member variables
  bool                                ext                                      = false;
  bool                                beam_fail_instance_max_count_r17_present = false;
  bool                                beam_fail_detection_timer_r17_present    = false;
  bfd_res_to_add_mod_list_r17_l_      bfd_res_to_add_mod_list_r17;
  bfd_res_to_release_list_r17_l_      bfd_res_to_release_list_r17;
  beam_fail_instance_max_count_r17_e_ beam_fail_instance_max_count_r17;
  beam_fail_detection_timer_r17_e_    beam_fail_detection_timer_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-PPW-PeriodicityAndStartSlot-r17 ::= CHOICE
struct dl_ppw_periodicity_and_start_slot_r17_c {
  struct scs15_c_ {
    struct types_opts {
      enum options {
        n4,
        n5,
        n8,
        n10,
        n16,
        n20,
        n32,
        n40,
        n64,
        n80,
        n160,
        n320,
        n640,
        n1280,
        n2560,
        n5120,
        n10240,
        // ...
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    scs15_c_() = default;
    scs15_c_(const scs15_c_& other);
    scs15_c_& operator=(const scs15_c_& other);
    ~scs15_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& n4()
    {
      assert_choice_type(types::n4, type_, "scs15");
      return c.get<uint8_t>();
    }
    uint8_t& n5()
    {
      assert_choice_type(types::n5, type_, "scs15");
      return c.get<uint8_t>();
    }
    uint8_t& n8()
    {
      assert_choice_type(types::n8, type_, "scs15");
      return c.get<uint8_t>();
    }
    uint8_t& n10()
    {
      assert_choice_type(types::n10, type_, "scs15");
      return c.get<uint8_t>();
    }
    uint8_t& n16()
    {
      assert_choice_type(types::n16, type_, "scs15");
      return c.get<uint8_t>();
    }
    uint8_t& n20()
    {
      assert_choice_type(types::n20, type_, "scs15");
      return c.get<uint8_t>();
    }
    uint8_t& n32()
    {
      assert_choice_type(types::n32, type_, "scs15");
      return c.get<uint8_t>();
    }
    uint8_t& n40()
    {
      assert_choice_type(types::n40, type_, "scs15");
      return c.get<uint8_t>();
    }
    uint8_t& n64()
    {
      assert_choice_type(types::n64, type_, "scs15");
      return c.get<uint8_t>();
    }
    uint8_t& n80()
    {
      assert_choice_type(types::n80, type_, "scs15");
      return c.get<uint8_t>();
    }
    uint8_t& n160()
    {
      assert_choice_type(types::n160, type_, "scs15");
      return c.get<uint8_t>();
    }
    uint16_t& n320()
    {
      assert_choice_type(types::n320, type_, "scs15");
      return c.get<uint16_t>();
    }
    uint16_t& n640()
    {
      assert_choice_type(types::n640, type_, "scs15");
      return c.get<uint16_t>();
    }
    uint16_t& n1280()
    {
      assert_choice_type(types::n1280, type_, "scs15");
      return c.get<uint16_t>();
    }
    uint16_t& n2560()
    {
      assert_choice_type(types::n2560, type_, "scs15");
      return c.get<uint16_t>();
    }
    uint16_t& n5120()
    {
      assert_choice_type(types::n5120, type_, "scs15");
      return c.get<uint16_t>();
    }
    uint16_t& n10240()
    {
      assert_choice_type(types::n10240, type_, "scs15");
      return c.get<uint16_t>();
    }
    const uint8_t& n4() const
    {
      assert_choice_type(types::n4, type_, "scs15");
      return c.get<uint8_t>();
    }
    const uint8_t& n5() const
    {
      assert_choice_type(types::n5, type_, "scs15");
      return c.get<uint8_t>();
    }
    const uint8_t& n8() const
    {
      assert_choice_type(types::n8, type_, "scs15");
      return c.get<uint8_t>();
    }
    const uint8_t& n10() const
    {
      assert_choice_type(types::n10, type_, "scs15");
      return c.get<uint8_t>();
    }
    const uint8_t& n16() const
    {
      assert_choice_type(types::n16, type_, "scs15");
      return c.get<uint8_t>();
    }
    const uint8_t& n20() const
    {
      assert_choice_type(types::n20, type_, "scs15");
      return c.get<uint8_t>();
    }
    const uint8_t& n32() const
    {
      assert_choice_type(types::n32, type_, "scs15");
      return c.get<uint8_t>();
    }
    const uint8_t& n40() const
    {
      assert_choice_type(types::n40, type_, "scs15");
      return c.get<uint8_t>();
    }
    const uint8_t& n64() const
    {
      assert_choice_type(types::n64, type_, "scs15");
      return c.get<uint8_t>();
    }
    const uint8_t& n80() const
    {
      assert_choice_type(types::n80, type_, "scs15");
      return c.get<uint8_t>();
    }
    const uint8_t& n160() const
    {
      assert_choice_type(types::n160, type_, "scs15");
      return c.get<uint8_t>();
    }
    const uint16_t& n320() const
    {
      assert_choice_type(types::n320, type_, "scs15");
      return c.get<uint16_t>();
    }
    const uint16_t& n640() const
    {
      assert_choice_type(types::n640, type_, "scs15");
      return c.get<uint16_t>();
    }
    const uint16_t& n1280() const
    {
      assert_choice_type(types::n1280, type_, "scs15");
      return c.get<uint16_t>();
    }
    const uint16_t& n2560() const
    {
      assert_choice_type(types::n2560, type_, "scs15");
      return c.get<uint16_t>();
    }
    const uint16_t& n5120() const
    {
      assert_choice_type(types::n5120, type_, "scs15");
      return c.get<uint16_t>();
    }
    const uint16_t& n10240() const
    {
      assert_choice_type(types::n10240, type_, "scs15");
      return c.get<uint16_t>();
    }
    uint8_t&  set_n4();
    uint8_t&  set_n5();
    uint8_t&  set_n8();
    uint8_t&  set_n10();
    uint8_t&  set_n16();
    uint8_t&  set_n20();
    uint8_t&  set_n32();
    uint8_t&  set_n40();
    uint8_t&  set_n64();
    uint8_t&  set_n80();
    uint8_t&  set_n160();
    uint16_t& set_n320();
    uint16_t& set_n640();
    uint16_t& set_n1280();
    uint16_t& set_n2560();
    uint16_t& set_n5120();
    uint16_t& set_n10240();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct scs30_c_ {
    struct types_opts {
      enum options {
        n8,
        n10,
        n16,
        n20,
        n32,
        n40,
        n64,
        n80,
        n128,
        n160,
        n320,
        n640,
        n1280,
        n2560,
        n5120,
        n10240,
        n20480,
        // ...
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    scs30_c_() = default;
    scs30_c_(const scs30_c_& other);
    scs30_c_& operator=(const scs30_c_& other);
    ~scs30_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& n8()
    {
      assert_choice_type(types::n8, type_, "scs30");
      return c.get<uint8_t>();
    }
    uint8_t& n10()
    {
      assert_choice_type(types::n10, type_, "scs30");
      return c.get<uint8_t>();
    }
    uint8_t& n16()
    {
      assert_choice_type(types::n16, type_, "scs30");
      return c.get<uint8_t>();
    }
    uint8_t& n20()
    {
      assert_choice_type(types::n20, type_, "scs30");
      return c.get<uint8_t>();
    }
    uint8_t& n32()
    {
      assert_choice_type(types::n32, type_, "scs30");
      return c.get<uint8_t>();
    }
    uint8_t& n40()
    {
      assert_choice_type(types::n40, type_, "scs30");
      return c.get<uint8_t>();
    }
    uint8_t& n64()
    {
      assert_choice_type(types::n64, type_, "scs30");
      return c.get<uint8_t>();
    }
    uint8_t& n80()
    {
      assert_choice_type(types::n80, type_, "scs30");
      return c.get<uint8_t>();
    }
    uint8_t& n128()
    {
      assert_choice_type(types::n128, type_, "scs30");
      return c.get<uint8_t>();
    }
    uint8_t& n160()
    {
      assert_choice_type(types::n160, type_, "scs30");
      return c.get<uint8_t>();
    }
    uint16_t& n320()
    {
      assert_choice_type(types::n320, type_, "scs30");
      return c.get<uint16_t>();
    }
    uint16_t& n640()
    {
      assert_choice_type(types::n640, type_, "scs30");
      return c.get<uint16_t>();
    }
    uint16_t& n1280()
    {
      assert_choice_type(types::n1280, type_, "scs30");
      return c.get<uint16_t>();
    }
    uint16_t& n2560()
    {
      assert_choice_type(types::n2560, type_, "scs30");
      return c.get<uint16_t>();
    }
    uint16_t& n5120()
    {
      assert_choice_type(types::n5120, type_, "scs30");
      return c.get<uint16_t>();
    }
    uint16_t& n10240()
    {
      assert_choice_type(types::n10240, type_, "scs30");
      return c.get<uint16_t>();
    }
    uint16_t& n20480()
    {
      assert_choice_type(types::n20480, type_, "scs30");
      return c.get<uint16_t>();
    }
    const uint8_t& n8() const
    {
      assert_choice_type(types::n8, type_, "scs30");
      return c.get<uint8_t>();
    }
    const uint8_t& n10() const
    {
      assert_choice_type(types::n10, type_, "scs30");
      return c.get<uint8_t>();
    }
    const uint8_t& n16() const
    {
      assert_choice_type(types::n16, type_, "scs30");
      return c.get<uint8_t>();
    }
    const uint8_t& n20() const
    {
      assert_choice_type(types::n20, type_, "scs30");
      return c.get<uint8_t>();
    }
    const uint8_t& n32() const
    {
      assert_choice_type(types::n32, type_, "scs30");
      return c.get<uint8_t>();
    }
    const uint8_t& n40() const
    {
      assert_choice_type(types::n40, type_, "scs30");
      return c.get<uint8_t>();
    }
    const uint8_t& n64() const
    {
      assert_choice_type(types::n64, type_, "scs30");
      return c.get<uint8_t>();
    }
    const uint8_t& n80() const
    {
      assert_choice_type(types::n80, type_, "scs30");
      return c.get<uint8_t>();
    }
    const uint8_t& n128() const
    {
      assert_choice_type(types::n128, type_, "scs30");
      return c.get<uint8_t>();
    }
    const uint8_t& n160() const
    {
      assert_choice_type(types::n160, type_, "scs30");
      return c.get<uint8_t>();
    }
    const uint16_t& n320() const
    {
      assert_choice_type(types::n320, type_, "scs30");
      return c.get<uint16_t>();
    }
    const uint16_t& n640() const
    {
      assert_choice_type(types::n640, type_, "scs30");
      return c.get<uint16_t>();
    }
    const uint16_t& n1280() const
    {
      assert_choice_type(types::n1280, type_, "scs30");
      return c.get<uint16_t>();
    }
    const uint16_t& n2560() const
    {
      assert_choice_type(types::n2560, type_, "scs30");
      return c.get<uint16_t>();
    }
    const uint16_t& n5120() const
    {
      assert_choice_type(types::n5120, type_, "scs30");
      return c.get<uint16_t>();
    }
    const uint16_t& n10240() const
    {
      assert_choice_type(types::n10240, type_, "scs30");
      return c.get<uint16_t>();
    }
    const uint16_t& n20480() const
    {
      assert_choice_type(types::n20480, type_, "scs30");
      return c.get<uint16_t>();
    }
    uint8_t&  set_n8();
    uint8_t&  set_n10();
    uint8_t&  set_n16();
    uint8_t&  set_n20();
    uint8_t&  set_n32();
    uint8_t&  set_n40();
    uint8_t&  set_n64();
    uint8_t&  set_n80();
    uint8_t&  set_n128();
    uint8_t&  set_n160();
    uint16_t& set_n320();
    uint16_t& set_n640();
    uint16_t& set_n1280();
    uint16_t& set_n2560();
    uint16_t& set_n5120();
    uint16_t& set_n10240();
    uint16_t& set_n20480();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct scs60_c_ {
    struct types_opts {
      enum options {
        n16,
        n20,
        n32,
        n40,
        n64,
        n80,
        n128,
        n160,
        n256,
        n320,
        n640,
        n1280,
        n2560,
        n5120,
        n10240,
        n20480,
        n40960,
        // ...
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    scs60_c_() = default;
    scs60_c_(const scs60_c_& other);
    scs60_c_& operator=(const scs60_c_& other);
    ~scs60_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& n16()
    {
      assert_choice_type(types::n16, type_, "scs60");
      return c.get<uint8_t>();
    }
    uint8_t& n20()
    {
      assert_choice_type(types::n20, type_, "scs60");
      return c.get<uint8_t>();
    }
    uint8_t& n32()
    {
      assert_choice_type(types::n32, type_, "scs60");
      return c.get<uint8_t>();
    }
    uint8_t& n40()
    {
      assert_choice_type(types::n40, type_, "scs60");
      return c.get<uint8_t>();
    }
    uint8_t& n64()
    {
      assert_choice_type(types::n64, type_, "scs60");
      return c.get<uint8_t>();
    }
    uint8_t& n80()
    {
      assert_choice_type(types::n80, type_, "scs60");
      return c.get<uint8_t>();
    }
    uint8_t& n128()
    {
      assert_choice_type(types::n128, type_, "scs60");
      return c.get<uint8_t>();
    }
    uint8_t& n160()
    {
      assert_choice_type(types::n160, type_, "scs60");
      return c.get<uint8_t>();
    }
    uint16_t& n256()
    {
      assert_choice_type(types::n256, type_, "scs60");
      return c.get<uint16_t>();
    }
    uint16_t& n320()
    {
      assert_choice_type(types::n320, type_, "scs60");
      return c.get<uint16_t>();
    }
    uint16_t& n640()
    {
      assert_choice_type(types::n640, type_, "scs60");
      return c.get<uint16_t>();
    }
    uint16_t& n1280()
    {
      assert_choice_type(types::n1280, type_, "scs60");
      return c.get<uint16_t>();
    }
    uint16_t& n2560()
    {
      assert_choice_type(types::n2560, type_, "scs60");
      return c.get<uint16_t>();
    }
    uint16_t& n5120()
    {
      assert_choice_type(types::n5120, type_, "scs60");
      return c.get<uint16_t>();
    }
    uint16_t& n10240()
    {
      assert_choice_type(types::n10240, type_, "scs60");
      return c.get<uint16_t>();
    }
    uint16_t& n20480()
    {
      assert_choice_type(types::n20480, type_, "scs60");
      return c.get<uint16_t>();
    }
    uint16_t& n40960()
    {
      assert_choice_type(types::n40960, type_, "scs60");
      return c.get<uint16_t>();
    }
    const uint8_t& n16() const
    {
      assert_choice_type(types::n16, type_, "scs60");
      return c.get<uint8_t>();
    }
    const uint8_t& n20() const
    {
      assert_choice_type(types::n20, type_, "scs60");
      return c.get<uint8_t>();
    }
    const uint8_t& n32() const
    {
      assert_choice_type(types::n32, type_, "scs60");
      return c.get<uint8_t>();
    }
    const uint8_t& n40() const
    {
      assert_choice_type(types::n40, type_, "scs60");
      return c.get<uint8_t>();
    }
    const uint8_t& n64() const
    {
      assert_choice_type(types::n64, type_, "scs60");
      return c.get<uint8_t>();
    }
    const uint8_t& n80() const
    {
      assert_choice_type(types::n80, type_, "scs60");
      return c.get<uint8_t>();
    }
    const uint8_t& n128() const
    {
      assert_choice_type(types::n128, type_, "scs60");
      return c.get<uint8_t>();
    }
    const uint8_t& n160() const
    {
      assert_choice_type(types::n160, type_, "scs60");
      return c.get<uint8_t>();
    }
    const uint16_t& n256() const
    {
      assert_choice_type(types::n256, type_, "scs60");
      return c.get<uint16_t>();
    }
    const uint16_t& n320() const
    {
      assert_choice_type(types::n320, type_, "scs60");
      return c.get<uint16_t>();
    }
    const uint16_t& n640() const
    {
      assert_choice_type(types::n640, type_, "scs60");
      return c.get<uint16_t>();
    }
    const uint16_t& n1280() const
    {
      assert_choice_type(types::n1280, type_, "scs60");
      return c.get<uint16_t>();
    }
    const uint16_t& n2560() const
    {
      assert_choice_type(types::n2560, type_, "scs60");
      return c.get<uint16_t>();
    }
    const uint16_t& n5120() const
    {
      assert_choice_type(types::n5120, type_, "scs60");
      return c.get<uint16_t>();
    }
    const uint16_t& n10240() const
    {
      assert_choice_type(types::n10240, type_, "scs60");
      return c.get<uint16_t>();
    }
    const uint16_t& n20480() const
    {
      assert_choice_type(types::n20480, type_, "scs60");
      return c.get<uint16_t>();
    }
    const uint16_t& n40960() const
    {
      assert_choice_type(types::n40960, type_, "scs60");
      return c.get<uint16_t>();
    }
    uint8_t&  set_n16();
    uint8_t&  set_n20();
    uint8_t&  set_n32();
    uint8_t&  set_n40();
    uint8_t&  set_n64();
    uint8_t&  set_n80();
    uint8_t&  set_n128();
    uint8_t&  set_n160();
    uint16_t& set_n256();
    uint16_t& set_n320();
    uint16_t& set_n640();
    uint16_t& set_n1280();
    uint16_t& set_n2560();
    uint16_t& set_n5120();
    uint16_t& set_n10240();
    uint16_t& set_n20480();
    uint16_t& set_n40960();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct scs120_c_ {
    struct types_opts {
      enum options {
        n32,
        n40,
        n64,
        n80,
        n128,
        n160,
        n256,
        n320,
        n512,
        n640,
        n1280,
        n2560,
        n5120,
        n10240,
        n20480,
        n40960,
        n81920,
        // ...
        nulltype
      } value;
      typedef uint32_t number_type;

      const char* to_string() const;
      uint32_t    to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    scs120_c_() = default;
    scs120_c_(const scs120_c_& other);
    scs120_c_& operator=(const scs120_c_& other);
    ~scs120_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& n32()
    {
      assert_choice_type(types::n32, type_, "scs120");
      return c.get<uint8_t>();
    }
    uint8_t& n40()
    {
      assert_choice_type(types::n40, type_, "scs120");
      return c.get<uint8_t>();
    }
    uint8_t& n64()
    {
      assert_choice_type(types::n64, type_, "scs120");
      return c.get<uint8_t>();
    }
    uint8_t& n80()
    {
      assert_choice_type(types::n80, type_, "scs120");
      return c.get<uint8_t>();
    }
    uint8_t& n128()
    {
      assert_choice_type(types::n128, type_, "scs120");
      return c.get<uint8_t>();
    }
    uint8_t& n160()
    {
      assert_choice_type(types::n160, type_, "scs120");
      return c.get<uint8_t>();
    }
    uint16_t& n256()
    {
      assert_choice_type(types::n256, type_, "scs120");
      return c.get<uint16_t>();
    }
    uint16_t& n320()
    {
      assert_choice_type(types::n320, type_, "scs120");
      return c.get<uint16_t>();
    }
    uint16_t& n512()
    {
      assert_choice_type(types::n512, type_, "scs120");
      return c.get<uint16_t>();
    }
    uint16_t& n640()
    {
      assert_choice_type(types::n640, type_, "scs120");
      return c.get<uint16_t>();
    }
    uint16_t& n1280()
    {
      assert_choice_type(types::n1280, type_, "scs120");
      return c.get<uint16_t>();
    }
    uint16_t& n2560()
    {
      assert_choice_type(types::n2560, type_, "scs120");
      return c.get<uint16_t>();
    }
    uint16_t& n5120()
    {
      assert_choice_type(types::n5120, type_, "scs120");
      return c.get<uint16_t>();
    }
    uint16_t& n10240()
    {
      assert_choice_type(types::n10240, type_, "scs120");
      return c.get<uint16_t>();
    }
    uint16_t& n20480()
    {
      assert_choice_type(types::n20480, type_, "scs120");
      return c.get<uint16_t>();
    }
    uint16_t& n40960()
    {
      assert_choice_type(types::n40960, type_, "scs120");
      return c.get<uint16_t>();
    }
    uint32_t& n81920()
    {
      assert_choice_type(types::n81920, type_, "scs120");
      return c.get<uint32_t>();
    }
    const uint8_t& n32() const
    {
      assert_choice_type(types::n32, type_, "scs120");
      return c.get<uint8_t>();
    }
    const uint8_t& n40() const
    {
      assert_choice_type(types::n40, type_, "scs120");
      return c.get<uint8_t>();
    }
    const uint8_t& n64() const
    {
      assert_choice_type(types::n64, type_, "scs120");
      return c.get<uint8_t>();
    }
    const uint8_t& n80() const
    {
      assert_choice_type(types::n80, type_, "scs120");
      return c.get<uint8_t>();
    }
    const uint8_t& n128() const
    {
      assert_choice_type(types::n128, type_, "scs120");
      return c.get<uint8_t>();
    }
    const uint8_t& n160() const
    {
      assert_choice_type(types::n160, type_, "scs120");
      return c.get<uint8_t>();
    }
    const uint16_t& n256() const
    {
      assert_choice_type(types::n256, type_, "scs120");
      return c.get<uint16_t>();
    }
    const uint16_t& n320() const
    {
      assert_choice_type(types::n320, type_, "scs120");
      return c.get<uint16_t>();
    }
    const uint16_t& n512() const
    {
      assert_choice_type(types::n512, type_, "scs120");
      return c.get<uint16_t>();
    }
    const uint16_t& n640() const
    {
      assert_choice_type(types::n640, type_, "scs120");
      return c.get<uint16_t>();
    }
    const uint16_t& n1280() const
    {
      assert_choice_type(types::n1280, type_, "scs120");
      return c.get<uint16_t>();
    }
    const uint16_t& n2560() const
    {
      assert_choice_type(types::n2560, type_, "scs120");
      return c.get<uint16_t>();
    }
    const uint16_t& n5120() const
    {
      assert_choice_type(types::n5120, type_, "scs120");
      return c.get<uint16_t>();
    }
    const uint16_t& n10240() const
    {
      assert_choice_type(types::n10240, type_, "scs120");
      return c.get<uint16_t>();
    }
    const uint16_t& n20480() const
    {
      assert_choice_type(types::n20480, type_, "scs120");
      return c.get<uint16_t>();
    }
    const uint16_t& n40960() const
    {
      assert_choice_type(types::n40960, type_, "scs120");
      return c.get<uint16_t>();
    }
    const uint32_t& n81920() const
    {
      assert_choice_type(types::n81920, type_, "scs120");
      return c.get<uint32_t>();
    }
    uint8_t&  set_n32();
    uint8_t&  set_n40();
    uint8_t&  set_n64();
    uint8_t&  set_n80();
    uint8_t&  set_n128();
    uint8_t&  set_n160();
    uint16_t& set_n256();
    uint16_t& set_n320();
    uint16_t& set_n512();
    uint16_t& set_n640();
    uint16_t& set_n1280();
    uint16_t& set_n2560();
    uint16_t& set_n5120();
    uint16_t& set_n10240();
    uint16_t& set_n20480();
    uint16_t& set_n40960();
    uint32_t& set_n81920();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct types_opts {
    enum options { scs15, scs30, scs60, scs120, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  dl_ppw_periodicity_and_start_slot_r17_c() = default;
  dl_ppw_periodicity_and_start_slot_r17_c(const dl_ppw_periodicity_and_start_slot_r17_c& other);
  dl_ppw_periodicity_and_start_slot_r17_c& operator=(const dl_ppw_periodicity_and_start_slot_r17_c& other);
  ~dl_ppw_periodicity_and_start_slot_r17_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  scs15_c_& scs15()
  {
    assert_choice_type(types::scs15, type_, "DL-PPW-PeriodicityAndStartSlot-r17");
    return c.get<scs15_c_>();
  }
  scs30_c_& scs30()
  {
    assert_choice_type(types::scs30, type_, "DL-PPW-PeriodicityAndStartSlot-r17");
    return c.get<scs30_c_>();
  }
  scs60_c_& scs60()
  {
    assert_choice_type(types::scs60, type_, "DL-PPW-PeriodicityAndStartSlot-r17");
    return c.get<scs60_c_>();
  }
  scs120_c_& scs120()
  {
    assert_choice_type(types::scs120, type_, "DL-PPW-PeriodicityAndStartSlot-r17");
    return c.get<scs120_c_>();
  }
  const scs15_c_& scs15() const
  {
    assert_choice_type(types::scs15, type_, "DL-PPW-PeriodicityAndStartSlot-r17");
    return c.get<scs15_c_>();
  }
  const scs30_c_& scs30() const
  {
    assert_choice_type(types::scs30, type_, "DL-PPW-PeriodicityAndStartSlot-r17");
    return c.get<scs30_c_>();
  }
  const scs60_c_& scs60() const
  {
    assert_choice_type(types::scs60, type_, "DL-PPW-PeriodicityAndStartSlot-r17");
    return c.get<scs60_c_>();
  }
  const scs120_c_& scs120() const
  {
    assert_choice_type(types::scs120, type_, "DL-PPW-PeriodicityAndStartSlot-r17");
    return c.get<scs120_c_>();
  }
  scs15_c_&  set_scs15();
  scs30_c_&  set_scs30();
  scs60_c_&  set_scs60();
  scs120_c_& set_scs120();

private:
  types                                                    type_;
  choice_buffer_t<scs120_c_, scs15_c_, scs30_c_, scs60_c_> c;

  void destroy_();
};

// DMRS-DownlinkConfig ::= SEQUENCE
struct dmrs_dl_cfg_s {
  struct dmrs_add_position_opts {
    enum options { pos0, pos1, pos3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using dmrs_add_position_e_ = enumerated<dmrs_add_position_opts>;

  // member variables
  bool                           ext                       = false;
  bool                           dmrs_type_present         = false;
  bool                           dmrs_add_position_present = false;
  bool                           max_len_present           = false;
  bool                           scrambling_id0_present    = false;
  bool                           scrambling_id1_present    = false;
  bool                           phase_tracking_rs_present = false;
  dmrs_add_position_e_           dmrs_add_position;
  uint32_t                       scrambling_id0 = 0;
  uint32_t                       scrambling_id1 = 0;
  setup_release_c<ptrs_dl_cfg_s> phase_tracking_rs;
  // ...
  // group 0
  bool dmrs_dl_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkPreemption ::= SEQUENCE
struct dl_preemption_s {
  struct time_freq_set_opts {
    enum options { set0, set1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using time_freq_set_e_            = enumerated<time_freq_set_opts>;
  using int_cfg_per_serving_cell_l_ = dyn_array<int_cfg_per_serving_cell_s>;

  // member variables
  bool                        ext      = false;
  uint32_t                    int_rnti = 0;
  time_freq_set_e_            time_freq_set;
  uint8_t                     dci_payload_size = 0;
  int_cfg_per_serving_cell_l_ int_cfg_per_serving_cell;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Dummy-TDRA-List ::= SEQUENCE (SIZE (1..16)) OF MultiPDSCH-TDRA-r17
using dummy_tdra_list_l = dyn_array<multi_pdsch_tdra_r17_s>;

// MinSchedulingOffsetK0-Values-r16 ::= SEQUENCE (SIZE (1..2)) OF INTEGER (0..16)
using min_sched_offset_k0_values_r16_l = bounded_array<uint8_t, 2>;

// MinSchedulingOffsetK0-Values-r17 ::= SEQUENCE (SIZE (1..2)) OF INTEGER (0..64)
using min_sched_offset_k0_values_r17_l = bounded_array<uint8_t, 2>;

// MultiPDSCH-TDRA-List-r17 ::= SEQUENCE (SIZE (1..64)) OF MultiPDSCH-TDRA-r17
using multi_pdsch_tdra_list_r17_l = dyn_array<multi_pdsch_tdra_r17_s>;

// PUCCH-TPC-CommandConfig ::= SEQUENCE
struct pucch_tpc_cmd_cfg_s {
  bool    ext                         = false;
  bool    tpc_idx_pcell_present       = false;
  bool    tpc_idx_pucch_scell_present = false;
  uint8_t tpc_idx_pcell               = 1;
  uint8_t tpc_idx_pucch_scell         = 1;
  // ...
  // group 0
  bool    tpc_idx_pucch_s_scell_r17_present                       = false;
  bool    tpc_idx_pucch_s_scell_secondary_pucch_group_r17_present = false;
  uint8_t tpc_idx_pucch_s_scell_r17                               = 1;
  uint8_t tpc_idx_pucch_s_scell_secondary_pucch_group_r17         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-TPC-CommandConfig ::= SEQUENCE
struct pusch_tpc_cmd_cfg_s {
  bool    ext                 = false;
  bool    tpc_idx_present     = false;
  bool    tpc_idx_sul_present = false;
  bool    target_cell_present = false;
  uint8_t tpc_idx             = 1;
  uint8_t tpc_idx_sul         = 1;
  uint8_t target_cell         = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct rate_match_pattern_group_item_c_ {
  struct types_opts {
    enum options { cell_level, bwp_level, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  rate_match_pattern_group_item_c_() = default;
  rate_match_pattern_group_item_c_(const rate_match_pattern_group_item_c_& other);
  rate_match_pattern_group_item_c_& operator=(const rate_match_pattern_group_item_c_& other);
  ~rate_match_pattern_group_item_c_() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& cell_level()
  {
    assert_choice_type(types::cell_level, type_, "RateMatchPatternGroup-item");
    return c.get<uint8_t>();
  }
  uint8_t& bwp_level()
  {
    assert_choice_type(types::bwp_level, type_, "RateMatchPatternGroup-item");
    return c.get<uint8_t>();
  }
  const uint8_t& cell_level() const
  {
    assert_choice_type(types::cell_level, type_, "RateMatchPatternGroup-item");
    return c.get<uint8_t>();
  }
  const uint8_t& bwp_level() const
  {
    assert_choice_type(types::bwp_level, type_, "RateMatchPatternGroup-item");
    return c.get<uint8_t>();
  }
  uint8_t& set_cell_level();
  uint8_t& set_bwp_level();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// RateMatchPatternGroup ::= SEQUENCE (SIZE (1..8)) OF RateMatchPatternGroup-item
using rate_match_pattern_group_l = dyn_array<rate_match_pattern_group_item_c_>;

// RepetitionSchemeConfig-r16 ::= CHOICE
struct repeat_scheme_cfg_r16_c {
  struct types_opts {
    enum options { fdm_tdm_r16, slot_based_r16, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  repeat_scheme_cfg_r16_c() = default;
  repeat_scheme_cfg_r16_c(const repeat_scheme_cfg_r16_c& other);
  repeat_scheme_cfg_r16_c& operator=(const repeat_scheme_cfg_r16_c& other);
  ~repeat_scheme_cfg_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_release_c<fdm_tdm_r16_s>& fdm_tdm_r16()
  {
    assert_choice_type(types::fdm_tdm_r16, type_, "RepetitionSchemeConfig-r16");
    return c.get<setup_release_c<fdm_tdm_r16_s>>();
  }
  setup_release_c<slot_based_r16_s>& slot_based_r16()
  {
    assert_choice_type(types::slot_based_r16, type_, "RepetitionSchemeConfig-r16");
    return c.get<setup_release_c<slot_based_r16_s>>();
  }
  const setup_release_c<fdm_tdm_r16_s>& fdm_tdm_r16() const
  {
    assert_choice_type(types::fdm_tdm_r16, type_, "RepetitionSchemeConfig-r16");
    return c.get<setup_release_c<fdm_tdm_r16_s>>();
  }
  const setup_release_c<slot_based_r16_s>& slot_based_r16() const
  {
    assert_choice_type(types::slot_based_r16, type_, "RepetitionSchemeConfig-r16");
    return c.get<setup_release_c<slot_based_r16_s>>();
  }
  setup_release_c<fdm_tdm_r16_s>&    set_fdm_tdm_r16();
  setup_release_c<slot_based_r16_s>& set_slot_based_r16();

private:
  types                                                                              type_;
  choice_buffer_t<setup_release_c<fdm_tdm_r16_s>, setup_release_c<slot_based_r16_s>> c;

  void destroy_();
};

// RepetitionSchemeConfig-v1630 ::= SEQUENCE
struct repeat_scheme_cfg_v1630_s {
  setup_release_c<slot_based_v1630_s> slot_based_v1630;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SPS-Config ::= SEQUENCE
struct sps_cfg_s {
  struct periodicity_opts {
    enum options {
      ms10,
      ms20,
      ms32,
      ms40,
      ms64,
      ms80,
      ms128,
      ms160,
      ms320,
      ms640,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using periodicity_e_ = enumerated<periodicity_opts>;
  struct pdsch_aggregation_factor_r16_opts {
    enum options { n1, n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using pdsch_aggregation_factor_r16_e_ = enumerated<pdsch_aggregation_factor_r16_opts>;

  // member variables
  bool           ext                 = false;
  bool           n1_pucch_an_present = false;
  bool           mcs_table_present   = false;
  periodicity_e_ periodicity;
  uint8_t        nrof_harq_processes = 1;
  uint8_t        n1_pucch_an         = 0;
  // ...
  // group 0
  bool                            sps_cfg_idx_r16_present              = false;
  bool                            harq_proc_id_offset_r16_present      = false;
  bool                            periodicity_ext_r16_present          = false;
  bool                            harq_codebook_id_r16_present         = false;
  bool                            pdsch_aggregation_factor_r16_present = false;
  uint8_t                         sps_cfg_idx_r16                      = 0;
  uint8_t                         harq_proc_id_offset_r16              = 0;
  uint16_t                        periodicity_ext_r16                  = 1;
  uint8_t                         harq_codebook_id_r16                 = 1;
  pdsch_aggregation_factor_r16_e_ pdsch_aggregation_factor_r16;
  // group 1
  bool     sps_harq_deferral_r17_present         = false;
  bool     n1_pucch_an_pucch_s_scell_r17_present = false;
  bool     periodicity_ext_r17_present           = false;
  bool     nrof_harq_processes_v1710_present     = false;
  bool     harq_proc_id_offset_v1700_present     = false;
  uint8_t  sps_harq_deferral_r17                 = 1;
  uint8_t  n1_pucch_an_pucch_s_scell_r17         = 0;
  uint16_t periodicity_ext_r17                   = 1;
  uint8_t  nrof_harq_processes_v1710             = 9;
  uint8_t  harq_proc_id_offset_v1700             = 16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-TPC-CommandConfig ::= SEQUENCE
struct srs_tpc_cmd_cfg_s {
  bool    ext                            = false;
  bool    start_bit_of_format2_3_present = false;
  bool    field_type_format2_3_present   = false;
  uint8_t start_bit_of_format2_3         = 1;
  uint8_t field_type_format2_3           = 0;
  // ...
  // group 0
  bool    start_bit_of_format2_3_sul_present = false;
  uint8_t start_bit_of_format2_3_sul         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SearchSpaceSwitchConfig-r16 ::= SEQUENCE
struct search_space_switch_cfg_r16_s {
  using cell_groups_for_switch_list_r16_l_ = dyn_array<cell_group_for_switch_r16_l>;

  // member variables
  bool                               search_space_switch_delay_r16_present = false;
  cell_groups_for_switch_list_r16_l_ cell_groups_for_switch_list_r16;
  uint8_t                            search_space_switch_delay_r16 = 10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SearchSpaceSwitchConfig-r17 ::= SEQUENCE
struct search_space_switch_cfg_r17_s {
  bool    search_space_switch_timer_r17_present = false;
  bool    search_space_switch_delay_r17_present = false;
  uint8_t search_space_switch_timer_r17         = 1;
  uint8_t search_space_switch_delay_r17         = 10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TCI-State ::= SEQUENCE
struct tci_state_s {
  bool       ext               = false;
  bool       qcl_type2_present = false;
  uint8_t    tci_state_id      = 0;
  qcl_info_s qcl_type1;
  qcl_info_s qcl_type2;
  // ...
  // group 0
  bool    add_pci_r17_present            = false;
  bool    pathloss_ref_rs_id_r17_present = false;
  bool    ul_pwr_ctrl_r17_present        = false;
  uint8_t add_pci_r17                    = 1;
  uint8_t pathloss_ref_rs_id_r17         = 0;
  uint8_t ul_pwr_ctrl_r17                = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkCancellation-r16 ::= SEQUENCE
struct ul_cancellation_r16_s {
  using ci_cfg_per_serving_cell_r16_l_ = dyn_array<ci_cfg_per_serving_cell_r16_s>;

  // member variables
  bool                           ext                         = false;
  uint32_t                       ci_rnti_r16                 = 0;
  uint8_t                        dci_payload_size_for_ci_r16 = 0;
  ci_cfg_per_serving_cell_r16_l_ ci_cfg_per_serving_cell_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ZP-CSI-RS-Resource ::= SEQUENCE
struct zp_csi_rs_res_s {
  bool                             ext                            = false;
  bool                             periodicity_and_offset_present = false;
  uint8_t                          zp_csi_rs_res_id               = 0;
  csi_rs_res_map_s                 res_map;
  csi_res_periodicity_and_offset_c periodicity_and_offset;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ZP-CSI-RS-ResourceSet ::= SEQUENCE
struct zp_csi_rs_res_set_s {
  using zp_csi_rs_res_id_list_l_ = bounded_array<uint8_t, 16>;

  // member variables
  bool                     ext                  = false;
  uint8_t                  zp_csi_rs_res_set_id = 0;
  zp_csi_rs_res_id_list_l_ zp_csi_rs_res_id_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BeamFailureDetection-r17 ::= SEQUENCE
struct beam_fail_detection_r17_s {
  bool                          fail_detection_set1_r17_present = false;
  bool                          fail_detection_set2_r17_present = false;
  bool                          add_pci_r17_present             = false;
  beam_fail_detection_set_r17_s fail_detection_set1_r17;
  beam_fail_detection_set_r17_s fail_detection_set2_r17;
  uint8_t                       add_pci_r17 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CandidateBeamRS-r16 ::= SEQUENCE
struct candidate_beam_rs_r16_s {
  struct candidate_beam_cfg_r16_c_ {
    struct types_opts {
      enum options { ssb_r16, csi_rs_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    candidate_beam_cfg_r16_c_() = default;
    candidate_beam_cfg_r16_c_(const candidate_beam_cfg_r16_c_& other);
    candidate_beam_cfg_r16_c_& operator=(const candidate_beam_cfg_r16_c_& other);
    ~candidate_beam_cfg_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_r16()
    {
      assert_choice_type(types::ssb_r16, type_, "candidateBeamConfig-r16");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_r16()
    {
      assert_choice_type(types::csi_rs_r16, type_, "candidateBeamConfig-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_r16() const
    {
      assert_choice_type(types::ssb_r16, type_, "candidateBeamConfig-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_r16() const
    {
      assert_choice_type(types::csi_rs_r16, type_, "candidateBeamConfig-r16");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_r16();
    uint8_t& set_csi_rs_r16();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                      serving_cell_id_present = false;
  candidate_beam_cfg_r16_c_ candidate_beam_cfg_r16;
  uint8_t                   serving_cell_id = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-PPW-PreConfig-r17 ::= SEQUENCE
struct dl_ppw_pre_cfg_r17_s {
  struct type_r17_opts {
    enum options { type1_a, type1_b, type2, nulltype } value;

    const char* to_string() const;
  };
  using type_r17_e_ = enumerated<type_r17_opts>;
  struct prio_r17_opts {
    enum options { st1, st2, st3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using prio_r17_e_ = enumerated<prio_r17_opts>;

  // member variables
  bool                                    type_r17_present = false;
  bool                                    prio_r17_present = false;
  uint8_t                                 dl_ppw_id_r17    = 0;
  dl_ppw_periodicity_and_start_slot_r17_c dl_ppw_periodicity_and_start_slot_r17;
  uint8_t                                 len_r17 = 1;
  type_r17_e_                             type_r17;
  prio_r17_e_                             prio_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-Config ::= SEQUENCE
struct pdcch_cfg_s {
  using coreset_to_add_mod_list_l_                = dyn_array<coreset_s>;
  using coreset_to_release_list_l_                = bounded_array<uint8_t, 3>;
  using search_spaces_to_add_mod_list_l_          = dyn_array<search_space_s>;
  using search_spaces_to_release_list_l_          = bounded_array<uint8_t, 10>;
  using coreset_to_add_mod_list_size_ext_v1610_l_ = dyn_array<coreset_s>;
  using coreset_to_release_list_size_ext_r16_l_   = bounded_array<uint8_t, 5>;
  using search_spaces_to_add_mod_list_ext_r16_l_  = dyn_array<search_space_ext_r16_s>;
  struct monitoring_cap_cfg_r16_opts {
    enum options { r15monitoringcap, r16monitoringcap, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using monitoring_cap_cfg_r16_e_                  = enumerated<monitoring_cap_cfg_r16_opts>;
  using search_spaces_to_add_mod_list_ext_v1700_l_ = dyn_array<search_space_ext_v1700_s>;
  using pdcch_skipping_dur_list_r17_l_             = bounded_array<uint8_t, 3>;

  // member variables
  bool                                 ext                   = false;
  bool                                 dl_preemption_present = false;
  bool                                 tpc_pusch_present     = false;
  bool                                 tpc_pucch_present     = false;
  bool                                 tpc_srs_present       = false;
  coreset_to_add_mod_list_l_           coreset_to_add_mod_list;
  coreset_to_release_list_l_           coreset_to_release_list;
  search_spaces_to_add_mod_list_l_     search_spaces_to_add_mod_list;
  search_spaces_to_release_list_l_     search_spaces_to_release_list;
  setup_release_c<dl_preemption_s>     dl_preemption;
  setup_release_c<pusch_tpc_cmd_cfg_s> tpc_pusch;
  setup_release_c<pucch_tpc_cmd_cfg_s> tpc_pucch;
  setup_release_c<srs_tpc_cmd_cfg_s>   tpc_srs;
  // ...
  // group 0
  bool                                                monitoring_cap_cfg_r16_present = false;
  copy_ptr<coreset_to_add_mod_list_size_ext_v1610_l_> coreset_to_add_mod_list_size_ext_v1610;
  copy_ptr<coreset_to_release_list_size_ext_r16_l_>   coreset_to_release_list_size_ext_r16;
  copy_ptr<search_spaces_to_add_mod_list_ext_r16_l_>  search_spaces_to_add_mod_list_ext_r16;
  copy_ptr<setup_release_c<ul_cancellation_r16_s>>    ul_cancellation_r16;
  monitoring_cap_cfg_r16_e_                           monitoring_cap_cfg_r16;
  copy_ptr<search_space_switch_cfg_r16_s>             search_space_switch_cfg_r16;
  // group 1
  bool                                                 monitoring_cap_cfg_v1710_present = false;
  copy_ptr<search_spaces_to_add_mod_list_ext_v1700_l_> search_spaces_to_add_mod_list_ext_v1700;
  copy_ptr<search_space_switch_cfg_r17_s>              search_space_switch_cfg_r17;
  copy_ptr<pdcch_skipping_dur_list_r17_l_>             pdcch_skipping_dur_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-Config ::= SEQUENCE
struct pdsch_cfg_s {
  using tci_states_to_add_mod_list_l_ = dyn_array<tci_state_s>;
  using tci_states_to_release_list_l_ = dyn_array<uint8_t>;
  struct vrb_to_prb_interleaver_opts {
    enum options { n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using vrb_to_prb_interleaver_e_ = enumerated<vrb_to_prb_interleaver_opts>;
  struct res_alloc_opts {
    enum options { res_alloc_type0, res_alloc_type1, dyn_switch, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using res_alloc_e_ = enumerated<res_alloc_opts>;
  struct pdsch_aggregation_factor_opts {
    enum options { n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using pdsch_aggregation_factor_e_           = enumerated<pdsch_aggregation_factor_opts>;
  using rate_match_pattern_to_add_mod_list_l_ = dyn_array<rate_match_pattern_s>;
  using rate_match_pattern_to_release_list_l_ = bounded_array<uint8_t, 4>;
  struct rbg_size_opts {
    enum options { cfg1, cfg2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using rbg_size_e_ = enumerated<rbg_size_opts>;
  struct mcs_table_opts {
    enum options { qam256, qam64_low_se, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using mcs_table_e_ = enumerated<mcs_table_opts>;
  struct max_nrof_code_words_sched_by_dci_opts {
    enum options { n1, n2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_nrof_code_words_sched_by_dci_e_ = enumerated<max_nrof_code_words_sched_by_dci_opts>;
  struct prb_bundling_type_c_ {
    struct static_bundling_s_ {
      struct bundle_size_opts {
        enum options { n4, wideband, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using bundle_size_e_ = enumerated<bundle_size_opts>;

      // member variables
      bool           bundle_size_present = false;
      bundle_size_e_ bundle_size;
    };
    struct dyn_bundling_s_ {
      struct bundle_size_set1_opts {
        enum options { n4, wideband, n2_wideband, n4_wideband, nulltype } value;

        const char* to_string() const;
      };
      using bundle_size_set1_e_ = enumerated<bundle_size_set1_opts>;
      struct bundle_size_set2_opts {
        enum options { n4, wideband, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using bundle_size_set2_e_ = enumerated<bundle_size_set2_opts>;

      // member variables
      bool                bundle_size_set1_present = false;
      bool                bundle_size_set2_present = false;
      bundle_size_set1_e_ bundle_size_set1;
      bundle_size_set2_e_ bundle_size_set2;
    };
    struct types_opts {
      enum options { static_bundling, dyn_bundling, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    prb_bundling_type_c_() = default;
    prb_bundling_type_c_(const prb_bundling_type_c_& other);
    prb_bundling_type_c_& operator=(const prb_bundling_type_c_& other);
    ~prb_bundling_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    static_bundling_s_& static_bundling()
    {
      assert_choice_type(types::static_bundling, type_, "prb-BundlingType");
      return c.get<static_bundling_s_>();
    }
    dyn_bundling_s_& dyn_bundling()
    {
      assert_choice_type(types::dyn_bundling, type_, "prb-BundlingType");
      return c.get<dyn_bundling_s_>();
    }
    const static_bundling_s_& static_bundling() const
    {
      assert_choice_type(types::static_bundling, type_, "prb-BundlingType");
      return c.get<static_bundling_s_>();
    }
    const dyn_bundling_s_& dyn_bundling() const
    {
      assert_choice_type(types::dyn_bundling, type_, "prb-BundlingType");
      return c.get<dyn_bundling_s_>();
    }
    static_bundling_s_& set_static_bundling();
    dyn_bundling_s_&    set_dyn_bundling();

  private:
    types                                                type_;
    choice_buffer_t<dyn_bundling_s_, static_bundling_s_> c;

    void destroy_();
  };
  using zp_csi_rs_res_to_add_mod_list_l_                            = dyn_array<zp_csi_rs_res_s>;
  using zp_csi_rs_res_to_release_list_l_                            = bounded_array<uint8_t, 32>;
  using aperiodic_zp_csi_rs_res_sets_to_add_mod_list_l_             = dyn_array<zp_csi_rs_res_set_s>;
  using aperiodic_zp_csi_rs_res_sets_to_release_list_l_             = bounded_array<uint8_t, 16>;
  using sp_zp_csi_rs_res_sets_to_add_mod_list_l_                    = dyn_array<zp_csi_rs_res_set_s>;
  using sp_zp_csi_rs_res_sets_to_release_list_l_                    = bounded_array<uint8_t, 16>;
  using aperiodic_zp_csi_rs_res_sets_to_add_mod_list_dci_1_2_r16_l_ = dyn_array<zp_csi_rs_res_set_s>;
  using aperiodic_zp_csi_rs_res_sets_to_release_list_dci_1_2_r16_l_ = bounded_array<uint8_t, 16>;
  struct mcs_table_dci_1_2_r16_opts {
    enum options { qam256, qam64_low_se, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using mcs_table_dci_1_2_r16_e_ = enumerated<mcs_table_dci_1_2_r16_opts>;
  struct prb_bundling_type_dci_1_2_r16_c_ {
    struct static_bundling_r16_s_ {
      struct bundle_size_r16_opts {
        enum options { n4, wideband, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using bundle_size_r16_e_ = enumerated<bundle_size_r16_opts>;

      // member variables
      bool               bundle_size_r16_present = false;
      bundle_size_r16_e_ bundle_size_r16;
    };
    struct dyn_bundling_r16_s_ {
      struct bundle_size_set1_r16_opts {
        enum options { n4, wideband, n2_wideband, n4_wideband, nulltype } value;

        const char* to_string() const;
      };
      using bundle_size_set1_r16_e_ = enumerated<bundle_size_set1_r16_opts>;
      struct bundle_size_set2_r16_opts {
        enum options { n4, wideband, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using bundle_size_set2_r16_e_ = enumerated<bundle_size_set2_r16_opts>;

      // member variables
      bool                    bundle_size_set1_r16_present = false;
      bool                    bundle_size_set2_r16_present = false;
      bundle_size_set1_r16_e_ bundle_size_set1_r16;
      bundle_size_set2_r16_e_ bundle_size_set2_r16;
    };
    struct types_opts {
      enum options { static_bundling_r16, dyn_bundling_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    prb_bundling_type_dci_1_2_r16_c_() = default;
    prb_bundling_type_dci_1_2_r16_c_(const prb_bundling_type_dci_1_2_r16_c_& other);
    prb_bundling_type_dci_1_2_r16_c_& operator=(const prb_bundling_type_dci_1_2_r16_c_& other);
    ~prb_bundling_type_dci_1_2_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    static_bundling_r16_s_& static_bundling_r16()
    {
      assert_choice_type(types::static_bundling_r16, type_, "prb-BundlingTypeDCI-1-2-r16");
      return c.get<static_bundling_r16_s_>();
    }
    dyn_bundling_r16_s_& dyn_bundling_r16()
    {
      assert_choice_type(types::dyn_bundling_r16, type_, "prb-BundlingTypeDCI-1-2-r16");
      return c.get<dyn_bundling_r16_s_>();
    }
    const static_bundling_r16_s_& static_bundling_r16() const
    {
      assert_choice_type(types::static_bundling_r16, type_, "prb-BundlingTypeDCI-1-2-r16");
      return c.get<static_bundling_r16_s_>();
    }
    const dyn_bundling_r16_s_& dyn_bundling_r16() const
    {
      assert_choice_type(types::dyn_bundling_r16, type_, "prb-BundlingTypeDCI-1-2-r16");
      return c.get<dyn_bundling_r16_s_>();
    }
    static_bundling_r16_s_& set_static_bundling_r16();
    dyn_bundling_r16_s_&    set_dyn_bundling_r16();

  private:
    types                                                        type_;
    choice_buffer_t<dyn_bundling_r16_s_, static_bundling_r16_s_> c;

    void destroy_();
  };
  struct res_alloc_type1_granularity_dci_1_2_r16_opts {
    enum options { n2, n4, n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using res_alloc_type1_granularity_dci_1_2_r16_e_ = enumerated<res_alloc_type1_granularity_dci_1_2_r16_opts>;
  struct vrb_to_prb_interleaver_dci_1_2_r16_opts {
    enum options { n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using vrb_to_prb_interleaver_dci_1_2_r16_e_ = enumerated<vrb_to_prb_interleaver_dci_1_2_r16_opts>;
  struct res_alloc_dci_1_2_r16_opts {
    enum options { res_alloc_type0, res_alloc_type1, dyn_switch, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using res_alloc_dci_1_2_r16_e_ = enumerated<res_alloc_dci_1_2_r16_opts>;
  struct dl_or_joint_tci_state_list_r17_c_ {
    struct explicitlist_s_ {
      using dl_or_joint_tci_state_to_add_mod_list_r17_l_ = dyn_array<tci_state_s>;
      using dl_or_joint_tci_state_to_release_list_r17_l_ = dyn_array<uint8_t>;

      // member variables
      dl_or_joint_tci_state_to_add_mod_list_r17_l_ dl_or_joint_tci_state_to_add_mod_list_r17;
      dl_or_joint_tci_state_to_release_list_r17_l_ dl_or_joint_tci_state_to_release_list_r17;
    };
    struct types_opts {
      enum options { explicitlist, unified_tci_state_ref_r17, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    dl_or_joint_tci_state_list_r17_c_() = default;
    dl_or_joint_tci_state_list_r17_c_(const dl_or_joint_tci_state_list_r17_c_& other);
    dl_or_joint_tci_state_list_r17_c_& operator=(const dl_or_joint_tci_state_list_r17_c_& other);
    ~dl_or_joint_tci_state_list_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    explicitlist_s_& explicitlist()
    {
      assert_choice_type(types::explicitlist, type_, "dl-OrJointTCI-StateList-r17");
      return c.get<explicitlist_s_>();
    }
    serving_cell_and_bwp_id_r17_s& unified_tci_state_ref_r17()
    {
      assert_choice_type(types::unified_tci_state_ref_r17, type_, "dl-OrJointTCI-StateList-r17");
      return c.get<serving_cell_and_bwp_id_r17_s>();
    }
    const explicitlist_s_& explicitlist() const
    {
      assert_choice_type(types::explicitlist, type_, "dl-OrJointTCI-StateList-r17");
      return c.get<explicitlist_s_>();
    }
    const serving_cell_and_bwp_id_r17_s& unified_tci_state_ref_r17() const
    {
      assert_choice_type(types::unified_tci_state_ref_r17, type_, "dl-OrJointTCI-StateList-r17");
      return c.get<serving_cell_and_bwp_id_r17_s>();
    }
    explicitlist_s_&               set_explicitlist();
    serving_cell_and_bwp_id_r17_s& set_unified_tci_state_ref_r17();

  private:
    types                                                           type_;
    choice_buffer_t<explicitlist_s_, serving_cell_and_bwp_id_r17_s> c;

    void destroy_();
  };
  struct beam_app_time_r17_opts {
    enum options {
      n1,
      n2,
      n4,
      n7,
      n14,
      n28,
      n42,
      n56,
      n70,
      n84,
      n98,
      n112,
      n224,
      n336,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using beam_app_time_r17_e_ = enumerated<beam_app_time_r17_opts>;
  struct xoverhead_multicast_r17_opts {
    enum options { xoh6, xoh12, xoh18, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using xoverhead_multicast_r17_e_ = enumerated<xoverhead_multicast_r17_opts>;

  // member variables
  bool                                                              ext                                      = false;
  bool                                                              data_scrambling_id_pdsch_present         = false;
  bool                                                              dmrs_dl_for_pdsch_map_type_a_present     = false;
  bool                                                              dmrs_dl_for_pdsch_map_type_b_present     = false;
  bool                                                              vrb_to_prb_interleaver_present           = false;
  bool                                                              pdsch_time_domain_alloc_list_present     = false;
  bool                                                              pdsch_aggregation_factor_present         = false;
  bool                                                              mcs_table_present                        = false;
  bool                                                              max_nrof_code_words_sched_by_dci_present = false;
  bool                                                              p_zp_csi_rs_res_set_present              = false;
  uint16_t                                                          data_scrambling_id_pdsch                 = 0;
  setup_release_c<dmrs_dl_cfg_s>                                    dmrs_dl_for_pdsch_map_type_a;
  setup_release_c<dmrs_dl_cfg_s>                                    dmrs_dl_for_pdsch_map_type_b;
  tci_states_to_add_mod_list_l_                                     tci_states_to_add_mod_list;
  tci_states_to_release_list_l_                                     tci_states_to_release_list;
  vrb_to_prb_interleaver_e_                                         vrb_to_prb_interleaver;
  res_alloc_e_                                                      res_alloc;
  setup_release_c<dyn_seq_of<pdsch_time_domain_res_alloc_s, 1, 16>> pdsch_time_domain_alloc_list;
  pdsch_aggregation_factor_e_                                       pdsch_aggregation_factor;
  rate_match_pattern_to_add_mod_list_l_                             rate_match_pattern_to_add_mod_list;
  rate_match_pattern_to_release_list_l_                             rate_match_pattern_to_release_list;
  rate_match_pattern_group_l                                        rate_match_pattern_group1;
  rate_match_pattern_group_l                                        rate_match_pattern_group2;
  rbg_size_e_                                                       rbg_size;
  mcs_table_e_                                                      mcs_table;
  max_nrof_code_words_sched_by_dci_e_                               max_nrof_code_words_sched_by_dci;
  prb_bundling_type_c_                                              prb_bundling_type;
  zp_csi_rs_res_to_add_mod_list_l_                                  zp_csi_rs_res_to_add_mod_list;
  zp_csi_rs_res_to_release_list_l_                                  zp_csi_rs_res_to_release_list;
  aperiodic_zp_csi_rs_res_sets_to_add_mod_list_l_                   aperiodic_zp_csi_rs_res_sets_to_add_mod_list;
  aperiodic_zp_csi_rs_res_sets_to_release_list_l_                   aperiodic_zp_csi_rs_res_sets_to_release_list;
  sp_zp_csi_rs_res_sets_to_add_mod_list_l_                          sp_zp_csi_rs_res_sets_to_add_mod_list;
  sp_zp_csi_rs_res_sets_to_release_list_l_                          sp_zp_csi_rs_res_sets_to_release_list;
  setup_release_c<zp_csi_rs_res_set_s>                              p_zp_csi_rs_res_set;
  // ...
  // group 0
  bool                                              ant_ports_field_presence_dci_1_2_r16_present    = false;
  bool                                              dmrs_seq_initization_dci_1_2_r16_present        = false;
  bool                                              harq_process_num_size_dci_1_2_r16_present       = false;
  bool                                              mcs_table_dci_1_2_r16_present                   = false;
  bool                                              nof_bits_for_rv_dci_1_2_r16_present             = false;
  bool                                              prio_ind_dci_1_2_r16_present                    = false;
  bool                                              res_alloc_type1_granularity_dci_1_2_r16_present = false;
  bool                                              vrb_to_prb_interleaver_dci_1_2_r16_present      = false;
  bool                                              ref_of_slivdci_1_2_r16_present                  = false;
  bool                                              res_alloc_dci_1_2_r16_present                   = false;
  bool                                              prio_ind_dci_1_1_r16_present                    = false;
  bool                                              data_scrambling_id_pdsch2_r16_present           = false;
  copy_ptr<setup_release_c<integer<uint8_t, 1, 8>>> max_mimo_layers_r16;
  copy_ptr<setup_release_c<dyn_seq_of<integer<uint8_t, 0, 16>, 1, 2>>> minimum_sched_offset_k0_r16;
  copy_ptr<aperiodic_zp_csi_rs_res_sets_to_add_mod_list_dci_1_2_r16_l_>
      aperiodic_zp_csi_rs_res_sets_to_add_mod_list_dci_1_2_r16;
  copy_ptr<aperiodic_zp_csi_rs_res_sets_to_release_list_dci_1_2_r16_l_>
                                           aperiodic_zp_csi_rs_res_sets_to_release_list_dci_1_2_r16;
  copy_ptr<setup_release_c<dmrs_dl_cfg_s>> dmrs_dl_for_pdsch_map_type_a_dci_1_2_r16;
  copy_ptr<setup_release_c<dmrs_dl_cfg_s>> dmrs_dl_for_pdsch_map_type_b_dci_1_2_r16;
  uint8_t                                  harq_process_num_size_dci_1_2_r16 = 0;
  mcs_table_dci_1_2_r16_e_                 mcs_table_dci_1_2_r16;
  uint8_t                                  nof_bits_for_rv_dci_1_2_r16 = 0;
  copy_ptr<setup_release_c<dyn_seq_of<pdsch_time_domain_res_alloc_r16_s, 1, 16>>>
                                             pdsch_time_domain_alloc_list_dci_1_2_r16;
  copy_ptr<prb_bundling_type_dci_1_2_r16_c_> prb_bundling_type_dci_1_2_r16;
  copy_ptr<rate_match_pattern_group_l>       rate_match_pattern_group1_dci_1_2_r16;
  copy_ptr<rate_match_pattern_group_l>       rate_match_pattern_group2_dci_1_2_r16;
  res_alloc_type1_granularity_dci_1_2_r16_e_ res_alloc_type1_granularity_dci_1_2_r16;
  vrb_to_prb_interleaver_dci_1_2_r16_e_      vrb_to_prb_interleaver_dci_1_2_r16;
  res_alloc_dci_1_2_r16_e_                   res_alloc_dci_1_2_r16;
  uint16_t                                   data_scrambling_id_pdsch2_r16 = 0;
  copy_ptr<setup_release_c<dyn_seq_of<pdsch_time_domain_res_alloc_r16_s, 1, 16>>> pdsch_time_domain_alloc_list_r16;
  copy_ptr<setup_release_c<repeat_scheme_cfg_r16_c>>                              repeat_scheme_cfg_r16;
  // group 1
  copy_ptr<setup_release_c<repeat_scheme_cfg_v1630_s>> repeat_scheme_cfg_v1630;
  // group 2
  bool                                        pdsch_harq_ack_one_shot_feedback_dci_1_2_r17_present = false;
  bool                                        pdsch_harq_ack_enh_type3_dci_1_2_r17_present         = false;
  bool                                        pdsch_harq_ack_enh_type3_dci_field_1_2_r17_present   = false;
  bool                                        pdsch_harq_ack_retx_dci_1_2_r17_present              = false;
  bool                                        pucch_s_scell_dyn_dci_1_2_r17_present                = false;
  bool                                        beam_app_time_r17_present                            = false;
  bool                                        dmrs_fd_occ_disabled_for_rank1_pdsch_r17_present     = false;
  bool                                        harq_process_num_size_dci_1_2_v1700_present          = false;
  bool                                        harq_process_num_size_dci_1_1_r17_present            = false;
  bool                                        mcs_table_r17_present                                = false;
  bool                                        mcs_table_dci_1_2_r17_present                        = false;
  bool                                        xoverhead_multicast_r17_present                      = false;
  bool                                        prio_ind_dci_4_2_r17_present                         = false;
  bool                                        size_dci_4_2_r17_present                             = false;
  copy_ptr<dl_or_joint_tci_state_list_r17_c_> dl_or_joint_tci_state_list_r17;
  beam_app_time_r17_e_                        beam_app_time_r17;
  copy_ptr<setup_release_c<dyn_seq_of<multi_pdsch_tdra_r17_s, 1, 16>>> dummy;
  copy_ptr<setup_release_c<dyn_seq_of<integer<uint8_t, 0, 64>, 1, 2>>> minimum_sched_offset_k0_r17;
  uint8_t                                                              harq_process_num_size_dci_1_2_v1700 = 0;
  uint8_t                                                              harq_process_num_size_dci_1_1_r17   = 5;
  xoverhead_multicast_r17_e_                                           xoverhead_multicast_r17;
  uint8_t                                                              size_dci_4_2_r17 = 20;
  // group 3
  copy_ptr<setup_release_c<dyn_seq_of<multi_pdsch_tdra_r17_s, 1, 64>>> pdsch_time_domain_alloc_list_for_multi_pdsch_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioLinkMonitoringRS ::= SEQUENCE
struct radio_link_monitoring_rs_s {
  struct purpose_opts {
    enum options { beam_fail, rlf, both, nulltype } value;

    const char* to_string() const;
  };
  using purpose_e_ = enumerated<purpose_opts>;
  struct detection_res_c_ {
    struct types_opts {
      enum options { ssb_idx, csi_rs_idx, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    detection_res_c_() = default;
    detection_res_c_(const detection_res_c_& other);
    detection_res_c_& operator=(const detection_res_c_& other);
    ~detection_res_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx()
    {
      assert_choice_type(types::ssb_idx, type_, "detectionResource");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx()
    {
      assert_choice_type(types::csi_rs_idx, type_, "detectionResource");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_idx() const
    {
      assert_choice_type(types::ssb_idx, type_, "detectionResource");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx() const
    {
      assert_choice_type(types::csi_rs_idx, type_, "detectionResource");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_idx();
    uint8_t& set_csi_rs_idx();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool             ext                         = false;
  uint8_t          radio_link_monitoring_rs_id = 0;
  purpose_e_       purpose;
  detection_res_c_ detection_res;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SPS-ConfigDeactivationState-r16 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (0..7)
using sps_cfg_deactivation_state_r16_l = bounded_array<uint8_t, 8>;

// SPS-ConfigMulticastToAddModList-r17 ::= SEQUENCE (SIZE (1..8)) OF SPS-Config
using sps_cfg_multicast_to_add_mod_list_r17_l = dyn_array<sps_cfg_s>;

// SPS-ConfigMulticastToReleaseList-r17 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (0..7)
using sps_cfg_multicast_to_release_list_r17_l = bounded_array<uint8_t, 8>;

// BeamFailureRecoveryRSConfig-r16 ::= SEQUENCE
struct beam_fail_recovery_rs_cfg_r16_s {
  using candidate_beam_rs_list_r16_l_  = dyn_array<candidate_beam_rs_r16_s>;
  using candidate_beam_rs_list2_r17_l_ = dyn_array<candidate_beam_rs_r16_s>;

  // member variables
  bool                          ext                        = false;
  bool                          rsrp_thres_bfr_r16_present = false;
  uint8_t                       rsrp_thres_bfr_r16         = 0;
  candidate_beam_rs_list_r16_l_ candidate_beam_rs_list_r16;
  // ...
  // group 0
  copy_ptr<candidate_beam_rs_list2_r17_l_> candidate_beam_rs_list2_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CFR-ConfigMulticast-r17 ::= SEQUENCE
struct cfr_cfg_multicast_r17_s {
  bool                                    location_and_bw_multicast_r17_present = false;
  bool                                    pdcch_cfg_multicast_r17_present       = false;
  bool                                    pdsch_cfg_multicast_r17_present       = false;
  uint16_t                                location_and_bw_multicast_r17         = 0;
  pdcch_cfg_s                             pdcch_cfg_multicast_r17;
  pdsch_cfg_s                             pdsch_cfg_multicast_r17;
  sps_cfg_multicast_to_add_mod_list_r17_l sps_cfg_multicast_to_add_mod_list_r17;
  sps_cfg_multicast_to_release_list_r17_l sps_cfg_multicast_to_release_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-PPW-PreConfigToAddModList-r17 ::= SEQUENCE (SIZE (1..4)) OF DL-PPW-PreConfig-r17
using dl_ppw_pre_cfg_to_add_mod_list_r17_l = dyn_array<dl_ppw_pre_cfg_r17_s>;

// DL-PPW-PreConfigToReleaseList-r17 ::= SEQUENCE (SIZE (1..4)) OF INTEGER (0..15)
using dl_ppw_pre_cfg_to_release_list_r17_l = bounded_array<uint8_t, 4>;

// RadioLinkMonitoringConfig ::= SEQUENCE
struct radio_link_monitoring_cfg_s {
  using fail_detection_res_to_add_mod_list_l_ = dyn_array<radio_link_monitoring_rs_s>;
  using fail_detection_res_to_release_list_l_ = bounded_array<uint8_t, 10>;
  struct beam_fail_instance_max_count_opts {
    enum options { n1, n2, n3, n4, n5, n6, n8, n10, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using beam_fail_instance_max_count_e_ = enumerated<beam_fail_instance_max_count_opts>;
  struct beam_fail_detection_timer_opts {
    enum options { pbfd1, pbfd2, pbfd3, pbfd4, pbfd5, pbfd6, pbfd8, pbfd10, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using beam_fail_detection_timer_e_ = enumerated<beam_fail_detection_timer_opts>;

  // member variables
  bool                                  ext                                  = false;
  bool                                  beam_fail_instance_max_count_present = false;
  bool                                  beam_fail_detection_timer_present    = false;
  fail_detection_res_to_add_mod_list_l_ fail_detection_res_to_add_mod_list;
  fail_detection_res_to_release_list_l_ fail_detection_res_to_release_list;
  beam_fail_instance_max_count_e_       beam_fail_instance_max_count;
  beam_fail_detection_timer_e_          beam_fail_detection_timer;
  // ...
  // group 0
  copy_ptr<beam_fail_detection_r17_s> beam_fail_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SPS-ConfigDeactivationStateList-r16 ::= SEQUENCE (SIZE (1..16)) OF SPS-ConfigDeactivationState-r16
using sps_cfg_deactivation_state_list_r16_l = dyn_array<sps_cfg_deactivation_state_r16_l>;

// SPS-ConfigToAddModList-r16 ::= SEQUENCE (SIZE (1..8)) OF SPS-Config
using sps_cfg_to_add_mod_list_r16_l = dyn_array<sps_cfg_s>;

// SPS-ConfigToReleaseList-r16 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (0..7)
using sps_cfg_to_release_list_r16_l = bounded_array<uint8_t, 8>;

// BWP-DownlinkDedicated ::= SEQUENCE
struct bwp_dl_ded_s {
  bool                                         ext                               = false;
  bool                                         pdcch_cfg_present                 = false;
  bool                                         pdsch_cfg_present                 = false;
  bool                                         sps_cfg_present                   = false;
  bool                                         radio_link_monitoring_cfg_present = false;
  setup_release_c<pdcch_cfg_s>                 pdcch_cfg;
  setup_release_c<pdsch_cfg_s>                 pdsch_cfg;
  setup_release_c<sps_cfg_s>                   sps_cfg;
  setup_release_c<radio_link_monitoring_cfg_s> radio_link_monitoring_cfg;
  // ...
  // group 0
  copy_ptr<sps_cfg_to_add_mod_list_r16_l>                    sps_cfg_to_add_mod_list_r16;
  copy_ptr<sps_cfg_to_release_list_r16_l>                    sps_cfg_to_release_list_r16;
  copy_ptr<sps_cfg_deactivation_state_list_r16_l>            sps_cfg_deactivation_state_list_r16;
  copy_ptr<setup_release_c<beam_fail_recovery_rs_cfg_r16_s>> beam_fail_recovery_scell_cfg_r16;
  copy_ptr<setup_release_c<pdcch_cfg_s>>                     sl_pdcch_cfg_r16;
  copy_ptr<setup_release_c<pdcch_cfg_s>>                     sl_v2x_pdcch_cfg_r16;
  // group 1
  bool                                                       pre_conf_gap_status_r17_present                  = false;
  bool                                                       harq_feedback_enablingfor_sp_sactive_r17_present = false;
  bool                                                       serving_cell_mo_r17_present                      = false;
  fixed_bitstring<8>                                         pre_conf_gap_status_r17;
  copy_ptr<setup_release_c<beam_fail_recovery_rs_cfg_r16_s>> beam_fail_recovery_sp_cell_cfg_r17;
  bool                                                       harq_feedback_enablingfor_sp_sactive_r17 = false;
  copy_ptr<setup_release_c<cfr_cfg_multicast_r17_s>>         cfr_cfg_multicast_r17;
  copy_ptr<dl_ppw_pre_cfg_to_add_mod_list_r17_l>             dl_ppw_pre_cfg_to_add_mod_list_r17;
  copy_ptr<dl_ppw_pre_cfg_to_release_list_r17_l>             dl_ppw_pre_cfg_to_release_list_r17;
  copy_ptr<non_cell_defining_ssb_r17_s>                      non_cell_defining_ssb_r17;
  uint8_t                                                    serving_cell_mo_r17 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP-Downlink ::= SEQUENCE
struct bwp_dl_s {
  bool            ext                = false;
  bool            bwp_common_present = false;
  bool            bwp_ded_present    = false;
  uint8_t         bwp_id             = 0;
  bwp_dl_common_s bwp_common;
  bwp_dl_ded_s    bwp_ded;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-PathlossReferenceRS-r16 ::= SEQUENCE
struct pucch_pathloss_ref_rs_r16_s {
  struct ref_sig_r16_c_ {
    struct types_opts {
      enum options { ssb_idx_r16, csi_rs_idx_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ref_sig_r16_c_() = default;
    ref_sig_r16_c_(const ref_sig_r16_c_& other);
    ref_sig_r16_c_& operator=(const ref_sig_r16_c_& other);
    ~ref_sig_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx_r16()
    {
      assert_choice_type(types::ssb_idx_r16, type_, "referenceSignal-r16");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx_r16()
    {
      assert_choice_type(types::csi_rs_idx_r16, type_, "referenceSignal-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_idx_r16() const
    {
      assert_choice_type(types::ssb_idx_r16, type_, "referenceSignal-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx_r16() const
    {
      assert_choice_type(types::csi_rs_idx_r16, type_, "referenceSignal-r16");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_idx_r16();
    uint8_t& set_csi_rs_idx_r16();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  uint8_t        pucch_pathloss_ref_rs_id_r16 = 4;
  ref_sig_r16_c_ ref_sig_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathlossReferenceRS-Config ::= CHOICE
struct pathloss_ref_rs_cfg_c {
  struct types_opts {
    enum options { ssb_idx, csi_rs_idx, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  pathloss_ref_rs_cfg_c() = default;
  pathloss_ref_rs_cfg_c(const pathloss_ref_rs_cfg_c& other);
  pathloss_ref_rs_cfg_c& operator=(const pathloss_ref_rs_cfg_c& other);
  ~pathloss_ref_rs_cfg_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& ssb_idx()
  {
    assert_choice_type(types::ssb_idx, type_, "PathlossReferenceRS-Config");
    return c.get<uint8_t>();
  }
  uint8_t& csi_rs_idx()
  {
    assert_choice_type(types::csi_rs_idx, type_, "PathlossReferenceRS-Config");
    return c.get<uint8_t>();
  }
  const uint8_t& ssb_idx() const
  {
    assert_choice_type(types::ssb_idx, type_, "PathlossReferenceRS-Config");
    return c.get<uint8_t>();
  }
  const uint8_t& csi_rs_idx() const
  {
    assert_choice_type(types::csi_rs_idx, type_, "PathlossReferenceRS-Config");
    return c.get<uint8_t>();
  }
  uint8_t& set_ssb_idx();
  uint8_t& set_csi_rs_idx();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// Alpha ::= ENUMERATED
struct alpha_opts {
  enum options { alpha0, alpha04, alpha05, alpha06, alpha07, alpha08, alpha09, alpha1, nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
using alpha_e = enumerated<alpha_opts>;

// BetaOffsets ::= SEQUENCE
struct beta_offsets_s {
  bool    beta_offset_ack_idx1_present       = false;
  bool    beta_offset_ack_idx2_present       = false;
  bool    beta_offset_ack_idx3_present       = false;
  bool    beta_offset_csi_part1_idx1_present = false;
  bool    beta_offset_csi_part1_idx2_present = false;
  bool    beta_offset_csi_part2_idx1_present = false;
  bool    beta_offset_csi_part2_idx2_present = false;
  uint8_t beta_offset_ack_idx1               = 0;
  uint8_t beta_offset_ack_idx2               = 0;
  uint8_t beta_offset_ack_idx3               = 0;
  uint8_t beta_offset_csi_part1_idx1         = 0;
  uint8_t beta_offset_csi_part1_idx2         = 0;
  uint8_t beta_offset_csi_part2_idx1         = 0;
  uint8_t beta_offset_csi_part2_idx2         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BetaOffsetsCrossPri-r17 ::= SEQUENCE (SIZE (3)) OF INTEGER (0..31)
using beta_offsets_cross_pri_r17_l = std::array<uint8_t, 3>;

// DMRS-UplinkTransformPrecoding-r16 ::= SEQUENCE
struct dmrs_ul_transform_precoding_r16_s {
  bool     pi2_bpsk_scrambling_id0_present = false;
  bool     pi2_bpsk_scrambling_id1_present = false;
  uint32_t pi2_bpsk_scrambling_id0         = 0;
  uint32_t pi2_bpsk_scrambling_id1         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// P0-PUCCH ::= SEQUENCE
struct p0_pucch_s {
  uint8_t p0_pucch_id    = 1;
  int8_t  p0_pucch_value = -16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PTRS-UplinkConfig ::= SEQUENCE
struct ptrs_ul_cfg_s {
  struct transform_precoder_disabled_s_ {
    using freq_density_l_ = std::array<uint16_t, 2>;
    using time_density_l_ = std::array<uint8_t, 3>;
    struct max_nrof_ports_opts {
      enum options { n1, n2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_nrof_ports_e_ = enumerated<max_nrof_ports_opts>;
    struct res_elem_offset_opts {
      enum options { offset01, offset10, offset11, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    using res_elem_offset_e_ = enumerated<res_elem_offset_opts>;
    struct ptrs_pwr_opts {
      enum options { p00, p01, p10, p11, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    using ptrs_pwr_e_ = enumerated<ptrs_pwr_opts>;

    // member variables
    bool               freq_density_present    = false;
    bool               time_density_present    = false;
    bool               res_elem_offset_present = false;
    freq_density_l_    freq_density;
    time_density_l_    time_density;
    max_nrof_ports_e_  max_nrof_ports;
    res_elem_offset_e_ res_elem_offset;
    ptrs_pwr_e_        ptrs_pwr;
  };
  struct transform_precoder_enabled_s_ {
    using sample_density_l_ = std::array<uint16_t, 5>;

    // member variables
    bool              time_density_transform_precoding_present = false;
    sample_density_l_ sample_density;
  };

  // member variables
  bool                           ext                                 = false;
  bool                           transform_precoder_disabled_present = false;
  bool                           transform_precoder_enabled_present  = false;
  transform_precoder_disabled_s_ transform_precoder_disabled;
  transform_precoder_enabled_s_  transform_precoder_enabled;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-MaxCodeRate ::= ENUMERATED
struct pucch_max_code_rate_opts {
  enum options { zero_dot08, zero_dot15, zero_dot25, zero_dot35, zero_dot45, zero_dot60, zero_dot80, nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
using pucch_max_code_rate_e = enumerated<pucch_max_code_rate_opts>;

// PUCCH-PathlossReferenceRS ::= SEQUENCE
struct pucch_pathloss_ref_rs_s {
  struct ref_sig_c_ {
    struct types_opts {
      enum options { ssb_idx, csi_rs_idx, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ref_sig_c_() = default;
    ref_sig_c_(const ref_sig_c_& other);
    ref_sig_c_& operator=(const ref_sig_c_& other);
    ~ref_sig_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx()
    {
      assert_choice_type(types::ssb_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx()
    {
      assert_choice_type(types::csi_rs_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_idx() const
    {
      assert_choice_type(types::ssb_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx() const
    {
      assert_choice_type(types::csi_rs_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_idx();
    uint8_t& set_csi_rs_idx();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  uint8_t    pucch_pathloss_ref_rs_id = 0;
  ref_sig_c_ ref_sig;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-SRS ::= SEQUENCE
struct pucch_srs_s {
  uint8_t res    = 0;
  uint8_t ul_bwp = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-format0 ::= SEQUENCE
struct pucch_format0_s {
  uint8_t init_cyclic_shift = 0;
  uint8_t nrof_symbols      = 1;
  uint8_t start_symbol_idx  = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-format1 ::= SEQUENCE
struct pucch_format1_s {
  uint8_t init_cyclic_shift = 0;
  uint8_t nrof_symbols      = 4;
  uint8_t start_symbol_idx  = 0;
  uint8_t time_domain_occ   = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-format2 ::= SEQUENCE
struct pucch_format2_s {
  uint8_t nrof_prbs        = 1;
  uint8_t nrof_symbols     = 1;
  uint8_t start_symbol_idx = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-format3 ::= SEQUENCE
struct pucch_format3_s {
  uint8_t nrof_prbs        = 1;
  uint8_t nrof_symbols     = 4;
  uint8_t start_symbol_idx = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-format4 ::= SEQUENCE
struct pucch_format4_s {
  struct occ_len_opts {
    enum options { n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using occ_len_e_ = enumerated<occ_len_opts>;
  struct occ_idx_opts {
    enum options { n0, n1, n2, n3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using occ_idx_e_ = enumerated<occ_idx_opts>;

  // member variables
  uint8_t    nrof_symbols = 4;
  occ_len_e_ occ_len;
  occ_idx_e_ occ_idx;
  uint8_t    start_symbol_idx = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-Allocation-r16 ::= SEQUENCE
struct pusch_alloc_r16_s {
  struct map_type_r16_opts {
    enum options { type_a, type_b, nulltype } value;

    const char* to_string() const;
  };
  using map_type_r16_e_ = enumerated<map_type_r16_opts>;
  struct nof_repeats_r16_opts {
    enum options { n1, n2, n3, n4, n7, n8, n12, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nof_repeats_r16_e_ = enumerated<nof_repeats_r16_opts>;
  struct nof_repeats_ext_r17_opts {
    enum options {
      n1,
      n2,
      n3,
      n4,
      n7,
      n8,
      n12,
      n16,
      n20,
      n24,
      n28,
      n32,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nof_repeats_ext_r17_e_ = enumerated<nof_repeats_ext_r17_opts>;
  struct nof_slots_t_bo_ms_r17_opts {
    enum options { n1, n2, n4, n8, spare4, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nof_slots_t_bo_ms_r17_e_ = enumerated<nof_slots_t_bo_ms_r17_opts>;

  // member variables
  bool               ext                              = false;
  bool               map_type_r16_present             = false;
  bool               start_symbol_and_len_r16_present = false;
  bool               start_symbol_r16_present         = false;
  bool               len_r16_present                  = false;
  bool               nof_repeats_r16_present          = false;
  map_type_r16_e_    map_type_r16;
  uint8_t            start_symbol_and_len_r16 = 0;
  uint8_t            start_symbol_r16         = 0;
  uint8_t            len_r16                  = 1;
  nof_repeats_r16_e_ nof_repeats_r16;
  // ...
  // group 0
  bool                     nof_repeats_ext_r17_present   = false;
  bool                     nof_slots_t_bo_ms_r17_present = false;
  bool                     extended_k2_r17_present       = false;
  nof_repeats_ext_r17_e_   nof_repeats_ext_r17;
  nof_slots_t_bo_ms_r17_e_ nof_slots_t_bo_ms_r17;
  uint8_t                  extended_k2_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathlossReferenceRS-r16 ::= SEQUENCE
struct pathloss_ref_rs_r16_s {
  uint8_t               srs_pathloss_ref_rs_id_r16 = 0;
  pathloss_ref_rs_cfg_c pathloss_ref_rs_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathlossReferenceRSs-v1610 ::= SEQUENCE (SIZE (1..60)) OF PUCCH-PathlossReferenceRS-r16
using pathloss_ref_rss_v1610_l = dyn_array<pucch_pathloss_ref_rs_r16_s>;

// SPS-PUCCH-AN-r16 ::= SEQUENCE
struct sps_pucch_an_r16_s {
  bool     max_payload_size_r16_present = false;
  uint8_t  sps_pucch_an_res_id_r16      = 0;
  uint16_t max_payload_size_r16         = 4;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BetaOffsetsCrossPriSelCG-r17 ::= CHOICE
struct beta_offsets_cross_pri_sel_cg_r17_c {
  using dyn_r17_l_ = dyn_array<beta_offsets_cross_pri_r17_l>;
  struct types_opts {
    enum options { dyn_r17, semi_static_r17, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  beta_offsets_cross_pri_sel_cg_r17_c() = default;
  beta_offsets_cross_pri_sel_cg_r17_c(const beta_offsets_cross_pri_sel_cg_r17_c& other);
  beta_offsets_cross_pri_sel_cg_r17_c& operator=(const beta_offsets_cross_pri_sel_cg_r17_c& other);
  ~beta_offsets_cross_pri_sel_cg_r17_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  dyn_r17_l_& dyn_r17()
  {
    assert_choice_type(types::dyn_r17, type_, "BetaOffsetsCrossPriSelCG-r17");
    return c.get<dyn_r17_l_>();
  }
  beta_offsets_cross_pri_r17_l& semi_static_r17()
  {
    assert_choice_type(types::semi_static_r17, type_, "BetaOffsetsCrossPriSelCG-r17");
    return c.get<beta_offsets_cross_pri_r17_l>();
  }
  const dyn_r17_l_& dyn_r17() const
  {
    assert_choice_type(types::dyn_r17, type_, "BetaOffsetsCrossPriSelCG-r17");
    return c.get<dyn_r17_l_>();
  }
  const beta_offsets_cross_pri_r17_l& semi_static_r17() const
  {
    assert_choice_type(types::semi_static_r17, type_, "BetaOffsetsCrossPriSelCG-r17");
    return c.get<beta_offsets_cross_pri_r17_l>();
  }
  dyn_r17_l_&                   set_dyn_r17();
  beta_offsets_cross_pri_r17_l& set_semi_static_r17();

private:
  types                                                     type_;
  choice_buffer_t<beta_offsets_cross_pri_r17_l, dyn_r17_l_> c;

  void destroy_();
};

// CG-COT-Sharing-r16 ::= CHOICE
struct cg_cot_sharing_r16_c {
  struct cot_sharing_r16_s_ {
    uint8_t dur_r16            = 1;
    uint8_t offset_r16         = 1;
    uint8_t ch_access_prio_r16 = 1;
  };
  struct types_opts {
    enum options { no_cot_sharing_r16, cot_sharing_r16, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  cg_cot_sharing_r16_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cot_sharing_r16_s_& cot_sharing_r16()
  {
    assert_choice_type(types::cot_sharing_r16, type_, "CG-COT-Sharing-r16");
    return c;
  }
  const cot_sharing_r16_s_& cot_sharing_r16() const
  {
    assert_choice_type(types::cot_sharing_r16, type_, "CG-COT-Sharing-r16");
    return c;
  }
  void                set_no_cot_sharing_r16();
  cot_sharing_r16_s_& set_cot_sharing_r16();

private:
  types              type_;
  cot_sharing_r16_s_ c;
};

// CG-COT-Sharing-r17 ::= CHOICE
struct cg_cot_sharing_r17_c {
  struct cot_sharing_r17_s_ {
    uint16_t dur_r17    = 1;
    uint16_t offset_r17 = 1;
  };
  struct types_opts {
    enum options { no_cot_sharing_r17, cot_sharing_r17, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  cg_cot_sharing_r17_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cot_sharing_r17_s_& cot_sharing_r17()
  {
    assert_choice_type(types::cot_sharing_r17, type_, "CG-COT-Sharing-r17");
    return c;
  }
  const cot_sharing_r17_s_& cot_sharing_r17() const
  {
    assert_choice_type(types::cot_sharing_r17, type_, "CG-COT-Sharing-r17");
    return c;
  }
  void                set_no_cot_sharing_r17();
  cot_sharing_r17_s_& set_cot_sharing_r17();

private:
  types              type_;
  cot_sharing_r17_s_ c;
};

// CG-SDT-Configuration-r17 ::= SEQUENCE
struct cg_sdt_cfg_r17_s {
  struct sdt_ssb_subset_r17_c_ {
    struct types_opts {
      enum options { short_bitmap_r17, medium_bitmap_r17, long_bitmap_r17, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    sdt_ssb_subset_r17_c_() = default;
    sdt_ssb_subset_r17_c_(const sdt_ssb_subset_r17_c_& other);
    sdt_ssb_subset_r17_c_& operator=(const sdt_ssb_subset_r17_c_& other);
    ~sdt_ssb_subset_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<4>& short_bitmap_r17()
    {
      assert_choice_type(types::short_bitmap_r17, type_, "sdt-SSB-Subset-r17");
      return c.get<fixed_bitstring<4>>();
    }
    fixed_bitstring<8>& medium_bitmap_r17()
    {
      assert_choice_type(types::medium_bitmap_r17, type_, "sdt-SSB-Subset-r17");
      return c.get<fixed_bitstring<8>>();
    }
    fixed_bitstring<64>& long_bitmap_r17()
    {
      assert_choice_type(types::long_bitmap_r17, type_, "sdt-SSB-Subset-r17");
      return c.get<fixed_bitstring<64>>();
    }
    const fixed_bitstring<4>& short_bitmap_r17() const
    {
      assert_choice_type(types::short_bitmap_r17, type_, "sdt-SSB-Subset-r17");
      return c.get<fixed_bitstring<4>>();
    }
    const fixed_bitstring<8>& medium_bitmap_r17() const
    {
      assert_choice_type(types::medium_bitmap_r17, type_, "sdt-SSB-Subset-r17");
      return c.get<fixed_bitstring<8>>();
    }
    const fixed_bitstring<64>& long_bitmap_r17() const
    {
      assert_choice_type(types::long_bitmap_r17, type_, "sdt-SSB-Subset-r17");
      return c.get<fixed_bitstring<64>>();
    }
    fixed_bitstring<4>&  set_short_bitmap_r17();
    fixed_bitstring<8>&  set_medium_bitmap_r17();
    fixed_bitstring<64>& set_long_bitmap_r17();

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<64>> c;

    void destroy_();
  };
  struct sdt_ssb_per_cg_pusch_r17_opts {
    enum options { one_eighth, one_fourth, half, one, two, four, eight, sixteen, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using sdt_ssb_per_cg_pusch_r17_e_ = enumerated<sdt_ssb_per_cg_pusch_r17_opts>;
  struct sdt_alpha_r17_opts {
    enum options { alpha0, alpha04, alpha05, alpha06, alpha07, alpha08, alpha09, alpha1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using sdt_alpha_r17_e_ = enumerated<sdt_alpha_r17_opts>;
  struct sdt_dmrs_ports_r17_c_ {
    struct types_opts {
      enum options { dmrs_type1_r17, dmrs_type2_r17, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    sdt_dmrs_ports_r17_c_() = default;
    sdt_dmrs_ports_r17_c_(const sdt_dmrs_ports_r17_c_& other);
    sdt_dmrs_ports_r17_c_& operator=(const sdt_dmrs_ports_r17_c_& other);
    ~sdt_dmrs_ports_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<8>& dmrs_type1_r17()
    {
      assert_choice_type(types::dmrs_type1_r17, type_, "sdt-DMRS-Ports-r17");
      return c.get<fixed_bitstring<8>>();
    }
    fixed_bitstring<12>& dmrs_type2_r17()
    {
      assert_choice_type(types::dmrs_type2_r17, type_, "sdt-DMRS-Ports-r17");
      return c.get<fixed_bitstring<12>>();
    }
    const fixed_bitstring<8>& dmrs_type1_r17() const
    {
      assert_choice_type(types::dmrs_type1_r17, type_, "sdt-DMRS-Ports-r17");
      return c.get<fixed_bitstring<8>>();
    }
    const fixed_bitstring<12>& dmrs_type2_r17() const
    {
      assert_choice_type(types::dmrs_type2_r17, type_, "sdt-DMRS-Ports-r17");
      return c.get<fixed_bitstring<12>>();
    }
    fixed_bitstring<8>&  set_dmrs_type1_r17();
    fixed_bitstring<12>& set_dmrs_type2_r17();

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<12>> c;

    void destroy_();
  };

  // member variables
  bool                        cg_sdt_retx_timer_present        = false;
  bool                        sdt_ssb_subset_r17_present       = false;
  bool                        sdt_ssb_per_cg_pusch_r17_present = false;
  bool                        sdt_p0_pusch_r17_present         = false;
  bool                        sdt_alpha_r17_present            = false;
  bool                        sdt_dmrs_ports_r17_present       = false;
  bool                        sdt_nrof_dmrs_seqs_r17_present   = false;
  uint8_t                     cg_sdt_retx_timer                = 1;
  sdt_ssb_subset_r17_c_       sdt_ssb_subset_r17;
  sdt_ssb_per_cg_pusch_r17_e_ sdt_ssb_per_cg_pusch_r17;
  int8_t                      sdt_p0_pusch_r17 = -16;
  sdt_alpha_r17_e_            sdt_alpha_r17;
  sdt_dmrs_ports_r17_c_       sdt_dmrs_ports_r17;
  uint8_t                     sdt_nrof_dmrs_seqs_r17 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-StartingOffsets-r16 ::= SEQUENCE
struct cg_start_offsets_r16_s {
  using cg_start_full_bw_inside_cot_r16_l_  = bounded_array<uint8_t, 7>;
  using cg_start_full_bw_outside_cot_r16_l_ = bounded_array<uint8_t, 7>;

  // member variables
  bool                                cg_start_partial_bw_inside_cot_r16_present  = false;
  bool                                cg_start_partial_bw_outside_cot_r16_present = false;
  cg_start_full_bw_inside_cot_r16_l_  cg_start_full_bw_inside_cot_r16;
  cg_start_full_bw_outside_cot_r16_l_ cg_start_full_bw_outside_cot_r16;
  uint8_t                             cg_start_partial_bw_inside_cot_r16  = 0;
  uint8_t                             cg_start_partial_bw_outside_cot_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-UCI-OnPUSCH ::= CHOICE
struct cg_uci_on_pusch_c {
  using dyn_l_ = dyn_array<beta_offsets_s>;
  struct types_opts {
    enum options { dyn, semi_static, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  cg_uci_on_pusch_c() = default;
  cg_uci_on_pusch_c(const cg_uci_on_pusch_c& other);
  cg_uci_on_pusch_c& operator=(const cg_uci_on_pusch_c& other);
  ~cg_uci_on_pusch_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  dyn_l_& dyn()
  {
    assert_choice_type(types::dyn, type_, "CG-UCI-OnPUSCH");
    return c.get<dyn_l_>();
  }
  beta_offsets_s& semi_static()
  {
    assert_choice_type(types::semi_static, type_, "CG-UCI-OnPUSCH");
    return c.get<beta_offsets_s>();
  }
  const dyn_l_& dyn() const
  {
    assert_choice_type(types::dyn, type_, "CG-UCI-OnPUSCH");
    return c.get<dyn_l_>();
  }
  const beta_offsets_s& semi_static() const
  {
    assert_choice_type(types::semi_static, type_, "CG-UCI-OnPUSCH");
    return c.get<beta_offsets_s>();
  }
  dyn_l_&         set_dyn();
  beta_offsets_s& set_semi_static();

private:
  types                                   type_;
  choice_buffer_t<beta_offsets_s, dyn_l_> c;

  void destroy_();
};

// DL-DataToUL-ACK-DCI-1-2-r16 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (0..15)
using dl_data_to_ul_ack_dci_1_2_r16_l = bounded_array<uint8_t, 8>;

// DL-DataToUL-ACK-DCI-1-2-r17 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (0..127)
using dl_data_to_ul_ack_dci_1_2_r17_l = bounded_array<uint8_t, 8>;

// DL-DataToUL-ACK-MulticastDCI-Format4-1-r17 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (0..15)
using dl_data_to_ul_ack_multicast_dci_format4_1_r17_l = bounded_array<uint8_t, 8>;

// DL-DataToUL-ACK-r16 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (-1..15)
using dl_data_to_ul_ack_r16_l = bounded_array<int8_t, 8>;

// DL-DataToUL-ACK-r17 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (-1..127)
using dl_data_to_ul_ack_r17_l = bounded_array<int16_t, 8>;

// DL-DataToUL-ACK-v1700 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (16..31)
using dl_data_to_ul_ack_v1700_l = bounded_array<uint8_t, 8>;

// DMRS-BundlingPUCCH-Config-r17 ::= SEQUENCE
struct dmrs_bundling_pucch_cfg_r17_s {
  struct pucch_freq_hop_interv_r17_opts {
    enum options { s2, s4, s5, s10, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using pucch_freq_hop_interv_r17_e_ = enumerated<pucch_freq_hop_interv_r17_opts>;

  // member variables
  bool                         ext                                   = false;
  bool                         pucch_dmrs_bundling_r17_present       = false;
  bool                         pucch_time_domain_win_len_r17_present = false;
  bool                         pucch_win_restart_r17_present         = false;
  bool                         pucch_freq_hop_interv_r17_present     = false;
  uint8_t                      pucch_time_domain_win_len_r17         = 2;
  pucch_freq_hop_interv_r17_e_ pucch_freq_hop_interv_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DMRS-UplinkConfig ::= SEQUENCE
struct dmrs_ul_cfg_s {
  struct dmrs_add_position_opts {
    enum options { pos0, pos1, pos3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using dmrs_add_position_e_ = enumerated<dmrs_add_position_opts>;
  struct transform_precoding_disabled_s_ {
    bool     ext                    = false;
    bool     scrambling_id0_present = false;
    bool     scrambling_id1_present = false;
    uint32_t scrambling_id0         = 0;
    uint32_t scrambling_id1         = 0;
    // ...
    // group 0
    bool dmrs_ul_r16_present = false;

    // sequence methods
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };
  struct transform_precoding_enabled_s_ {
    bool     ext                   = false;
    bool     npusch_id_present     = false;
    bool     seq_group_hop_present = false;
    bool     seq_hop_present       = false;
    uint16_t npusch_id             = 0;
    // ...
    // group 0
    copy_ptr<setup_release_c<dmrs_ul_transform_precoding_r16_s>> dmrs_ul_transform_precoding_r16;

    // sequence methods
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool                            ext                                  = false;
  bool                            dmrs_type_present                    = false;
  bool                            dmrs_add_position_present            = false;
  bool                            phase_tracking_rs_present            = false;
  bool                            max_len_present                      = false;
  bool                            transform_precoding_disabled_present = false;
  bool                            transform_precoding_enabled_present  = false;
  dmrs_add_position_e_            dmrs_add_position;
  setup_release_c<ptrs_ul_cfg_s>  phase_tracking_rs;
  transform_precoding_disabled_s_ transform_precoding_disabled;
  transform_precoding_enabled_s_  transform_precoding_enabled;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DummyPathlossReferenceRS-v1710 ::= SEQUENCE
struct dummy_pathloss_ref_rs_v1710_s {
  bool    add_pci_r17_present          = false;
  uint8_t pusch_pathloss_ref_rs_id_r17 = 0;
  uint8_t add_pci_r17                  = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// P0-PUSCH-AlphaSet ::= SEQUENCE
struct p0_pusch_alpha_set_s {
  bool    p0_present            = false;
  bool    alpha_present         = false;
  uint8_t p0_pusch_alpha_set_id = 0;
  int8_t  p0                    = -16;
  alpha_e alpha;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// P0-PUSCH-Set-r16 ::= SEQUENCE
struct p0_pusch_set_r16_s {
  using p0_list_r16_l_ = bounded_array<int8_t, 2>;

  // member variables
  bool           ext                 = false;
  uint8_t        p0_pusch_set_id_r16 = 0;
  p0_list_r16_l_ p0_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRACH-ResourceDedicatedBFR ::= CHOICE
struct prach_res_ded_bfr_c {
  struct types_opts {
    enum options { ssb, csi_rs, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  prach_res_ded_bfr_c() = default;
  prach_res_ded_bfr_c(const prach_res_ded_bfr_c& other);
  prach_res_ded_bfr_c& operator=(const prach_res_ded_bfr_c& other);
  ~prach_res_ded_bfr_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  bfr_ssb_res_s& ssb()
  {
    assert_choice_type(types::ssb, type_, "PRACH-ResourceDedicatedBFR");
    return c.get<bfr_ssb_res_s>();
  }
  bfr_csi_rs_res_s& csi_rs()
  {
    assert_choice_type(types::csi_rs, type_, "PRACH-ResourceDedicatedBFR");
    return c.get<bfr_csi_rs_res_s>();
  }
  const bfr_ssb_res_s& ssb() const
  {
    assert_choice_type(types::ssb, type_, "PRACH-ResourceDedicatedBFR");
    return c.get<bfr_ssb_res_s>();
  }
  const bfr_csi_rs_res_s& csi_rs() const
  {
    assert_choice_type(types::csi_rs, type_, "PRACH-ResourceDedicatedBFR");
    return c.get<bfr_csi_rs_res_s>();
  }
  bfr_ssb_res_s&    set_ssb();
  bfr_csi_rs_res_s& set_csi_rs();

private:
  types                                            type_;
  choice_buffer_t<bfr_csi_rs_res_s, bfr_ssb_res_s> c;

  void destroy_();
};

// PUCCH-FormatConfig ::= SEQUENCE
struct pucch_format_cfg_s {
  struct nrof_slots_opts {
    enum options { n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nrof_slots_e_ = enumerated<nrof_slots_opts>;

  // member variables
  bool                  interslot_freq_hop_present = false;
  bool                  add_dmrs_present           = false;
  bool                  max_code_rate_present      = false;
  bool                  nrof_slots_present         = false;
  bool                  pi2_bpsk_present           = false;
  bool                  simul_harq_ack_csi_present = false;
  pucch_max_code_rate_e max_code_rate;
  nrof_slots_e_         nrof_slots;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-FormatConfigExt-r17 ::= SEQUENCE
struct pucch_format_cfg_ext_r17_s {
  bool                  ext                          = false;
  bool                  max_code_rate_lp_r17_present = false;
  pucch_max_code_rate_e max_code_rate_lp_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-PowerControl ::= SEQUENCE
struct pucch_pwr_ctrl_s {
  using p0_set_l_           = dyn_array<p0_pucch_s>;
  using pathloss_ref_rss_l_ = dyn_array<pucch_pathloss_ref_rs_s>;

  // member variables
  bool                ext                                    = false;
  bool                delta_f_pucch_f0_present               = false;
  bool                delta_f_pucch_f1_present               = false;
  bool                delta_f_pucch_f2_present               = false;
  bool                delta_f_pucch_f3_present               = false;
  bool                delta_f_pucch_f4_present               = false;
  bool                two_pucch_pc_adjustment_states_present = false;
  int8_t              delta_f_pucch_f0                       = -16;
  int8_t              delta_f_pucch_f1                       = -16;
  int8_t              delta_f_pucch_f2                       = -16;
  int8_t              delta_f_pucch_f3                       = -16;
  int8_t              delta_f_pucch_f4                       = -16;
  p0_set_l_           p0_set;
  pathloss_ref_rss_l_ pathloss_ref_rss;
  // ...
  // group 0
  copy_ptr<setup_release_c<dyn_seq_of<pucch_pathloss_ref_rs_r16_s, 1, 60>>> pathloss_ref_rss_v1610;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-PowerControlSetInfo-r17 ::= SEQUENCE
struct pucch_pwr_ctrl_set_info_r17_s {
  struct pucch_closed_loop_idx_r17_opts {
    enum options { i0, i1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using pucch_closed_loop_idx_r17_e_ = enumerated<pucch_closed_loop_idx_r17_opts>;

  // member variables
  uint8_t                      pucch_pwr_ctrl_set_info_id_r17 = 1;
  uint8_t                      p0_pucch_id_r17                = 1;
  pucch_closed_loop_idx_r17_e_ pucch_closed_loop_idx_r17;
  uint8_t                      pucch_pathloss_ref_rs_id_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-Resource ::= SEQUENCE
struct pucch_res_s {
  struct format_c_ {
    struct types_opts {
      enum options { format0, format1, format2, format3, format4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    format_c_() = default;
    format_c_(const format_c_& other);
    format_c_& operator=(const format_c_& other);
    ~format_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pucch_format0_s& format0()
    {
      assert_choice_type(types::format0, type_, "format");
      return c.get<pucch_format0_s>();
    }
    pucch_format1_s& format1()
    {
      assert_choice_type(types::format1, type_, "format");
      return c.get<pucch_format1_s>();
    }
    pucch_format2_s& format2()
    {
      assert_choice_type(types::format2, type_, "format");
      return c.get<pucch_format2_s>();
    }
    pucch_format3_s& format3()
    {
      assert_choice_type(types::format3, type_, "format");
      return c.get<pucch_format3_s>();
    }
    pucch_format4_s& format4()
    {
      assert_choice_type(types::format4, type_, "format");
      return c.get<pucch_format4_s>();
    }
    const pucch_format0_s& format0() const
    {
      assert_choice_type(types::format0, type_, "format");
      return c.get<pucch_format0_s>();
    }
    const pucch_format1_s& format1() const
    {
      assert_choice_type(types::format1, type_, "format");
      return c.get<pucch_format1_s>();
    }
    const pucch_format2_s& format2() const
    {
      assert_choice_type(types::format2, type_, "format");
      return c.get<pucch_format2_s>();
    }
    const pucch_format3_s& format3() const
    {
      assert_choice_type(types::format3, type_, "format");
      return c.get<pucch_format3_s>();
    }
    const pucch_format4_s& format4() const
    {
      assert_choice_type(types::format4, type_, "format");
      return c.get<pucch_format4_s>();
    }
    pucch_format0_s& set_format0();
    pucch_format1_s& set_format1();
    pucch_format2_s& set_format2();
    pucch_format3_s& set_format3();
    pucch_format4_s& set_format4();

  private:
    types                                                                                                type_;
    choice_buffer_t<pucch_format0_s, pucch_format1_s, pucch_format2_s, pucch_format3_s, pucch_format4_s> c;

    void destroy_();
  };

  // member variables
  bool      intra_slot_freq_hop_present = false;
  bool      second_hop_prb_present      = false;
  uint8_t   pucch_res_id                = 0;
  uint16_t  start_prb                   = 0;
  uint16_t  second_hop_prb              = 0;
  format_c_ format;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-ResourceExt-v1610 ::= SEQUENCE
struct pucch_res_ext_v1610_s {
  struct interlace_alloc_r16_s_ {
    struct interlace0_r16_c_ {
      struct types_opts {
        enum options { scs15, scs30, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      interlace0_r16_c_() = default;
      interlace0_r16_c_(const interlace0_r16_c_& other);
      interlace0_r16_c_& operator=(const interlace0_r16_c_& other);
      ~interlace0_r16_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      uint8_t& scs15()
      {
        assert_choice_type(types::scs15, type_, "interlace0-r16");
        return c.get<uint8_t>();
      }
      uint8_t& scs30()
      {
        assert_choice_type(types::scs30, type_, "interlace0-r16");
        return c.get<uint8_t>();
      }
      const uint8_t& scs15() const
      {
        assert_choice_type(types::scs15, type_, "interlace0-r16");
        return c.get<uint8_t>();
      }
      const uint8_t& scs30() const
      {
        assert_choice_type(types::scs30, type_, "interlace0-r16");
        return c.get<uint8_t>();
      }
      uint8_t& set_scs15();
      uint8_t& set_scs30();

    private:
      types               type_;
      pod_choice_buffer_t c;

      void destroy_();
    };

    // member variables
    uint8_t           rb_set_idx_r16 = 0;
    interlace0_r16_c_ interlace0_r16;
  };
  struct format_v1610_c_ {
    struct occ_v1610_s_ {
      struct occ_len_v1610_opts {
        enum options { n2, n4, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using occ_len_v1610_e_ = enumerated<occ_len_v1610_opts>;
      struct occ_idx_v1610_opts {
        enum options { n0, n1, n2, n3, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using occ_idx_v1610_e_ = enumerated<occ_idx_v1610_opts>;

      // member variables
      bool             occ_len_v1610_present = false;
      bool             occ_idx_v1610_present = false;
      occ_len_v1610_e_ occ_len_v1610;
      occ_idx_v1610_e_ occ_idx_v1610;
    };
    struct types_opts {
      enum options { interlace1_v1610, occ_v1610, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    format_v1610_c_() = default;
    format_v1610_c_(const format_v1610_c_& other);
    format_v1610_c_& operator=(const format_v1610_c_& other);
    ~format_v1610_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& interlace1_v1610()
    {
      assert_choice_type(types::interlace1_v1610, type_, "format-v1610");
      return c.get<uint8_t>();
    }
    occ_v1610_s_& occ_v1610()
    {
      assert_choice_type(types::occ_v1610, type_, "format-v1610");
      return c.get<occ_v1610_s_>();
    }
    const uint8_t& interlace1_v1610() const
    {
      assert_choice_type(types::interlace1_v1610, type_, "format-v1610");
      return c.get<uint8_t>();
    }
    const occ_v1610_s_& occ_v1610() const
    {
      assert_choice_type(types::occ_v1610, type_, "format-v1610");
      return c.get<occ_v1610_s_>();
    }
    uint8_t&      set_interlace1_v1610();
    occ_v1610_s_& set_occ_v1610();

  private:
    types                         type_;
    choice_buffer_t<occ_v1610_s_> c;

    void destroy_();
  };
  struct format_v1700_s_ {
    uint8_t nrof_prbs_r17 = 1;
  };
  struct pucch_repeat_nrof_slots_r17_opts {
    enum options { n1, n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using pucch_repeat_nrof_slots_r17_e_ = enumerated<pucch_repeat_nrof_slots_r17_opts>;

  // member variables
  bool                   ext                         = false;
  bool                   interlace_alloc_r16_present = false;
  bool                   format_v1610_present        = false;
  interlace_alloc_r16_s_ interlace_alloc_r16;
  format_v1610_c_        format_v1610;
  // ...
  // group 0
  bool                           pucch_repeat_nrof_slots_r17_present = false;
  copy_ptr<format_v1700_s_>      format_v1700;
  pucch_repeat_nrof_slots_r17_e_ pucch_repeat_nrof_slots_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-ResourceGroup-r16 ::= SEQUENCE
struct pucch_res_group_r16_s {
  using res_per_group_list_r16_l_ = dyn_array<uint8_t>;

  // member variables
  uint8_t                   pucch_res_group_id_r16 = 0;
  res_per_group_list_r16_l_ res_per_group_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-ResourceSet ::= SEQUENCE
struct pucch_res_set_s {
  using res_list_l_ = bounded_array<uint8_t, 32>;

  // member variables
  bool        max_payload_size_present = false;
  uint8_t     pucch_res_set_id         = 0;
  res_list_l_ res_list;
  uint16_t    max_payload_size = 4;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-SpatialRelationInfo ::= SEQUENCE
struct pucch_spatial_relation_info_s {
  struct ref_sig_c_ {
    struct types_opts {
      enum options { ssb_idx, csi_rs_idx, srs, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ref_sig_c_() = default;
    ref_sig_c_(const ref_sig_c_& other);
    ref_sig_c_& operator=(const ref_sig_c_& other);
    ~ref_sig_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx()
    {
      assert_choice_type(types::ssb_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx()
    {
      assert_choice_type(types::csi_rs_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    pucch_srs_s& srs()
    {
      assert_choice_type(types::srs, type_, "referenceSignal");
      return c.get<pucch_srs_s>();
    }
    const uint8_t& ssb_idx() const
    {
      assert_choice_type(types::ssb_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx() const
    {
      assert_choice_type(types::csi_rs_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const pucch_srs_s& srs() const
    {
      assert_choice_type(types::srs, type_, "referenceSignal");
      return c.get<pucch_srs_s>();
    }
    uint8_t&     set_ssb_idx();
    uint8_t&     set_csi_rs_idx();
    pucch_srs_s& set_srs();

  private:
    types                        type_;
    choice_buffer_t<pucch_srs_s> c;

    void destroy_();
  };
  struct closed_loop_idx_opts {
    enum options { i0, i1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using closed_loop_idx_e_ = enumerated<closed_loop_idx_opts>;

  // member variables
  bool               serving_cell_id_present        = false;
  uint8_t            pucch_spatial_relation_info_id = 1;
  uint8_t            serving_cell_id                = 0;
  ref_sig_c_         ref_sig;
  uint8_t            pucch_pathloss_ref_rs_id = 0;
  uint8_t            p0_pucch_id              = 1;
  closed_loop_idx_e_ closed_loop_idx;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-SpatialRelationInfoExt-r16 ::= SEQUENCE
struct pucch_spatial_relation_info_ext_r16_s {
  bool    ext                                          = false;
  bool    pucch_spatial_relation_info_id_v1610_present = false;
  bool    pucch_pathloss_ref_rs_id_v1610_present       = false;
  uint8_t pucch_spatial_relation_info_id_v1610         = 9;
  uint8_t pucch_pathloss_ref_rs_id_v1610               = 4;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-PathlossReferenceRS ::= SEQUENCE
struct pusch_pathloss_ref_rs_s {
  struct ref_sig_c_ {
    struct types_opts {
      enum options { ssb_idx, csi_rs_idx, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ref_sig_c_() = default;
    ref_sig_c_(const ref_sig_c_& other);
    ref_sig_c_& operator=(const ref_sig_c_& other);
    ~ref_sig_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx()
    {
      assert_choice_type(types::ssb_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx()
    {
      assert_choice_type(types::csi_rs_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_idx() const
    {
      assert_choice_type(types::ssb_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx() const
    {
      assert_choice_type(types::csi_rs_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_idx();
    uint8_t& set_csi_rs_idx();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  uint8_t    pusch_pathloss_ref_rs_id = 0;
  ref_sig_c_ ref_sig;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-PathlossReferenceRS-r16 ::= SEQUENCE
struct pusch_pathloss_ref_rs_r16_s {
  struct ref_sig_r16_c_ {
    struct types_opts {
      enum options { ssb_idx_r16, csi_rs_idx_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ref_sig_r16_c_() = default;
    ref_sig_r16_c_(const ref_sig_r16_c_& other);
    ref_sig_r16_c_& operator=(const ref_sig_r16_c_& other);
    ~ref_sig_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx_r16()
    {
      assert_choice_type(types::ssb_idx_r16, type_, "referenceSignal-r16");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx_r16()
    {
      assert_choice_type(types::csi_rs_idx_r16, type_, "referenceSignal-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_idx_r16() const
    {
      assert_choice_type(types::ssb_idx_r16, type_, "referenceSignal-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx_r16() const
    {
      assert_choice_type(types::csi_rs_idx_r16, type_, "referenceSignal-r16");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_idx_r16();
    uint8_t& set_csi_rs_idx_r16();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  uint8_t        pusch_pathloss_ref_rs_id_r16 = 4;
  ref_sig_r16_c_ ref_sig_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-TimeDomainResourceAllocation-r16 ::= SEQUENCE
struct pusch_time_domain_res_alloc_r16_s {
  using pusch_alloc_list_r16_l_ = dyn_array<pusch_alloc_r16_s>;

  // member variables
  bool                    ext            = false;
  bool                    k2_r16_present = false;
  uint8_t                 k2_r16         = 0;
  pusch_alloc_list_r16_l_ pusch_alloc_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathlossReferenceRSList-r16 ::= SEQUENCE (SIZE (1..64)) OF PathlossReferenceRS-r16
using pathloss_ref_rs_list_r16_l = dyn_array<pathloss_ref_rs_r16_s>;

// SPS-PUCCH-AN-List-r16 ::= SEQUENCE (SIZE (1..4)) OF SPS-PUCCH-AN-r16
using sps_pucch_an_list_r16_l = dyn_array<sps_pucch_an_r16_s>;

// SRI-PUSCH-PowerControl ::= SEQUENCE
struct sri_pusch_pwr_ctrl_s {
  struct sri_pusch_closed_loop_idx_opts {
    enum options { i0, i1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using sri_pusch_closed_loop_idx_e_ = enumerated<sri_pusch_closed_loop_idx_opts>;

  // member variables
  uint8_t                      sri_pusch_pwr_ctrl_id        = 0;
  uint8_t                      sri_pusch_pathloss_ref_rs_id = 0;
  uint8_t                      sri_p0_pusch_alpha_set_id    = 0;
  sri_pusch_closed_loop_idx_e_ sri_pusch_closed_loop_idx;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingRequestResourceConfig ::= SEQUENCE
struct sched_request_res_cfg_s {
  struct periodicity_and_offset_c_ {
    struct types_opts {
      enum options {
        sym2,
        sym6or7,
        sl1,
        sl2,
        sl4,
        sl5,
        sl8,
        sl10,
        sl16,
        sl20,
        sl40,
        sl80,
        sl160,
        sl320,
        sl640,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    periodicity_and_offset_c_() = default;
    periodicity_and_offset_c_(const periodicity_and_offset_c_& other);
    periodicity_and_offset_c_& operator=(const periodicity_and_offset_c_& other);
    ~periodicity_and_offset_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sl2()
    {
      assert_choice_type(types::sl2, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl4()
    {
      assert_choice_type(types::sl4, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl5()
    {
      assert_choice_type(types::sl5, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl8()
    {
      assert_choice_type(types::sl8, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl10()
    {
      assert_choice_type(types::sl10, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl16()
    {
      assert_choice_type(types::sl16, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl20()
    {
      assert_choice_type(types::sl20, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl40()
    {
      assert_choice_type(types::sl40, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl80()
    {
      assert_choice_type(types::sl80, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl160()
    {
      assert_choice_type(types::sl160, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint16_t& sl320()
    {
      assert_choice_type(types::sl320, type_, "periodicityAndOffset");
      return c.get<uint16_t>();
    }
    uint16_t& sl640()
    {
      assert_choice_type(types::sl640, type_, "periodicityAndOffset");
      return c.get<uint16_t>();
    }
    const uint8_t& sl2() const
    {
      assert_choice_type(types::sl2, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl4() const
    {
      assert_choice_type(types::sl4, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl5() const
    {
      assert_choice_type(types::sl5, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl8() const
    {
      assert_choice_type(types::sl8, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl10() const
    {
      assert_choice_type(types::sl10, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl16() const
    {
      assert_choice_type(types::sl16, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl20() const
    {
      assert_choice_type(types::sl20, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl40() const
    {
      assert_choice_type(types::sl40, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl80() const
    {
      assert_choice_type(types::sl80, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl160() const
    {
      assert_choice_type(types::sl160, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint16_t& sl320() const
    {
      assert_choice_type(types::sl320, type_, "periodicityAndOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& sl640() const
    {
      assert_choice_type(types::sl640, type_, "periodicityAndOffset");
      return c.get<uint16_t>();
    }
    void      set_sym2();
    void      set_sym6or7();
    void      set_sl1();
    uint8_t&  set_sl2();
    uint8_t&  set_sl4();
    uint8_t&  set_sl5();
    uint8_t&  set_sl8();
    uint8_t&  set_sl10();
    uint8_t&  set_sl16();
    uint8_t&  set_sl20();
    uint8_t&  set_sl40();
    uint8_t&  set_sl80();
    uint8_t&  set_sl160();
    uint16_t& set_sl320();
    uint16_t& set_sl640();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                      periodicity_and_offset_present = false;
  bool                      res_present                    = false;
  uint8_t                   sched_request_res_id           = 1;
  uint8_t                   sched_request_id               = 0;
  periodicity_and_offset_c_ periodicity_and_offset;
  uint8_t                   res = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingRequestResourceConfigExt-v1610 ::= SEQUENCE
struct sched_request_res_cfg_ext_v1610_s {
  struct phy_prio_idx_r16_opts {
    enum options { p0, p1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using phy_prio_idx_r16_e_ = enumerated<phy_prio_idx_r16_opts>;

  // member variables
  bool                ext                      = false;
  bool                phy_prio_idx_r16_present = false;
  phy_prio_idx_r16_e_ phy_prio_idx_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingRequestResourceConfigExt-v1700 ::= SEQUENCE
struct sched_request_res_cfg_ext_v1700_s {
  struct periodicity_and_offset_r17_c_ {
    struct types_opts {
      enum options { sl1280, sl2560, sl5120, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    periodicity_and_offset_r17_c_() = default;
    periodicity_and_offset_r17_c_(const periodicity_and_offset_r17_c_& other);
    periodicity_and_offset_r17_c_& operator=(const periodicity_and_offset_r17_c_& other);
    ~periodicity_and_offset_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t& sl1280()
    {
      assert_choice_type(types::sl1280, type_, "periodicityAndOffset-r17");
      return c.get<uint16_t>();
    }
    uint16_t& sl2560()
    {
      assert_choice_type(types::sl2560, type_, "periodicityAndOffset-r17");
      return c.get<uint16_t>();
    }
    uint16_t& sl5120()
    {
      assert_choice_type(types::sl5120, type_, "periodicityAndOffset-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& sl1280() const
    {
      assert_choice_type(types::sl1280, type_, "periodicityAndOffset-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& sl2560() const
    {
      assert_choice_type(types::sl2560, type_, "periodicityAndOffset-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& sl5120() const
    {
      assert_choice_type(types::sl5120, type_, "periodicityAndOffset-r17");
      return c.get<uint16_t>();
    }
    uint16_t& set_sl1280();
    uint16_t& set_sl2560();
    uint16_t& set_sl5120();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                          periodicity_and_offset_r17_present = false;
  periodicity_and_offset_r17_c_ periodicity_and_offset_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UCI-OnPUSCH ::= SEQUENCE
struct uci_on_pusch_s {
  struct beta_offsets_c_ {
    using dyn_l_ = std::array<beta_offsets_s, 4>;
    struct types_opts {
      enum options { dyn, semi_static, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    beta_offsets_c_() = default;
    beta_offsets_c_(const beta_offsets_c_& other);
    beta_offsets_c_& operator=(const beta_offsets_c_& other);
    ~beta_offsets_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dyn_l_& dyn()
    {
      assert_choice_type(types::dyn, type_, "betaOffsets");
      return c.get<dyn_l_>();
    }
    beta_offsets_s& semi_static()
    {
      assert_choice_type(types::semi_static, type_, "betaOffsets");
      return c.get<beta_offsets_s>();
    }
    const dyn_l_& dyn() const
    {
      assert_choice_type(types::dyn, type_, "betaOffsets");
      return c.get<dyn_l_>();
    }
    const beta_offsets_s& semi_static() const
    {
      assert_choice_type(types::semi_static, type_, "betaOffsets");
      return c.get<beta_offsets_s>();
    }
    dyn_l_&         set_dyn();
    beta_offsets_s& set_semi_static();

  private:
    types                                   type_;
    choice_buffer_t<beta_offsets_s, dyn_l_> c;

    void destroy_();
  };
  struct scaling_opts {
    enum options { f0p5, f0p65, f0p8, f1, nulltype } value;

    const char* to_string() const;
  };
  using scaling_e_ = enumerated<scaling_opts>;

  // member variables
  bool            beta_offsets_present = false;
  beta_offsets_c_ beta_offsets;
  scaling_e_      scaling;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UCI-OnPUSCH-DCI-0-2-r16 ::= SEQUENCE
struct uci_on_pusch_dci_0_2_r16_s {
  struct beta_offsets_dci_0_2_r16_c_ {
    struct dyn_dci_0_2_r16_c_ {
      using one_bit_r16_l_  = std::array<beta_offsets_s, 2>;
      using two_bits_r16_l_ = std::array<beta_offsets_s, 4>;
      struct types_opts {
        enum options { one_bit_r16, two_bits_r16, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      dyn_dci_0_2_r16_c_() = default;
      dyn_dci_0_2_r16_c_(const dyn_dci_0_2_r16_c_& other);
      dyn_dci_0_2_r16_c_& operator=(const dyn_dci_0_2_r16_c_& other);
      ~dyn_dci_0_2_r16_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      one_bit_r16_l_& one_bit_r16()
      {
        assert_choice_type(types::one_bit_r16, type_, "dynamicDCI-0-2-r16");
        return c.get<one_bit_r16_l_>();
      }
      two_bits_r16_l_& two_bits_r16()
      {
        assert_choice_type(types::two_bits_r16, type_, "dynamicDCI-0-2-r16");
        return c.get<two_bits_r16_l_>();
      }
      const one_bit_r16_l_& one_bit_r16() const
      {
        assert_choice_type(types::one_bit_r16, type_, "dynamicDCI-0-2-r16");
        return c.get<one_bit_r16_l_>();
      }
      const two_bits_r16_l_& two_bits_r16() const
      {
        assert_choice_type(types::two_bits_r16, type_, "dynamicDCI-0-2-r16");
        return c.get<two_bits_r16_l_>();
      }
      one_bit_r16_l_&  set_one_bit_r16();
      two_bits_r16_l_& set_two_bits_r16();

    private:
      types                                            type_;
      choice_buffer_t<one_bit_r16_l_, two_bits_r16_l_> c;

      void destroy_();
    };
    struct types_opts {
      enum options { dyn_dci_0_2_r16, semi_static_dci_0_2_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    beta_offsets_dci_0_2_r16_c_() = default;
    beta_offsets_dci_0_2_r16_c_(const beta_offsets_dci_0_2_r16_c_& other);
    beta_offsets_dci_0_2_r16_c_& operator=(const beta_offsets_dci_0_2_r16_c_& other);
    ~beta_offsets_dci_0_2_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dyn_dci_0_2_r16_c_& dyn_dci_0_2_r16()
    {
      assert_choice_type(types::dyn_dci_0_2_r16, type_, "betaOffsetsDCI-0-2-r16");
      return c.get<dyn_dci_0_2_r16_c_>();
    }
    beta_offsets_s& semi_static_dci_0_2_r16()
    {
      assert_choice_type(types::semi_static_dci_0_2_r16, type_, "betaOffsetsDCI-0-2-r16");
      return c.get<beta_offsets_s>();
    }
    const dyn_dci_0_2_r16_c_& dyn_dci_0_2_r16() const
    {
      assert_choice_type(types::dyn_dci_0_2_r16, type_, "betaOffsetsDCI-0-2-r16");
      return c.get<dyn_dci_0_2_r16_c_>();
    }
    const beta_offsets_s& semi_static_dci_0_2_r16() const
    {
      assert_choice_type(types::semi_static_dci_0_2_r16, type_, "betaOffsetsDCI-0-2-r16");
      return c.get<beta_offsets_s>();
    }
    dyn_dci_0_2_r16_c_& set_dyn_dci_0_2_r16();
    beta_offsets_s&     set_semi_static_dci_0_2_r16();

  private:
    types                                               type_;
    choice_buffer_t<beta_offsets_s, dyn_dci_0_2_r16_c_> c;

    void destroy_();
  };
  struct scaling_dci_0_2_r16_opts {
    enum options { f0p5, f0p65, f0p8, f1, nulltype } value;

    const char* to_string() const;
  };
  using scaling_dci_0_2_r16_e_ = enumerated<scaling_dci_0_2_r16_opts>;

  // member variables
  bool                        beta_offsets_dci_0_2_r16_present = false;
  beta_offsets_dci_0_2_r16_c_ beta_offsets_dci_0_2_r16;
  scaling_dci_0_2_r16_e_      scaling_dci_0_2_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-AccessConfigListDCI-1-1-r16 ::= SEQUENCE (SIZE (1..16)) OF INTEGER (0..15)
using ul_access_cfg_list_dci_1_1_r16_l = bounded_array<uint8_t, 16>;

// UL-AccessConfigListDCI-1-1-r17 ::= SEQUENCE (SIZE (1..3)) OF INTEGER (0..2)
using ul_access_cfg_list_dci_1_1_r17_l = bounded_array<uint8_t, 3>;

// UL-AccessConfigListDCI-1-2-r17 ::= SEQUENCE (SIZE (1..16)) OF INTEGER (0..15)
using ul_access_cfg_list_dci_1_2_r17_l = bounded_array<uint8_t, 16>;

// BetaOffsetsCrossPriSel-r17 ::= CHOICE
struct beta_offsets_cross_pri_sel_r17_c {
  using dyn_r17_l_ = std::array<beta_offsets_cross_pri_r17_l, 4>;
  struct types_opts {
    enum options { dyn_r17, semi_static_r17, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  beta_offsets_cross_pri_sel_r17_c() = default;
  beta_offsets_cross_pri_sel_r17_c(const beta_offsets_cross_pri_sel_r17_c& other);
  beta_offsets_cross_pri_sel_r17_c& operator=(const beta_offsets_cross_pri_sel_r17_c& other);
  ~beta_offsets_cross_pri_sel_r17_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  dyn_r17_l_& dyn_r17()
  {
    assert_choice_type(types::dyn_r17, type_, "BetaOffsetsCrossPriSel-r17");
    return c.get<dyn_r17_l_>();
  }
  beta_offsets_cross_pri_r17_l& semi_static_r17()
  {
    assert_choice_type(types::semi_static_r17, type_, "BetaOffsetsCrossPriSel-r17");
    return c.get<beta_offsets_cross_pri_r17_l>();
  }
  const dyn_r17_l_& dyn_r17() const
  {
    assert_choice_type(types::dyn_r17, type_, "BetaOffsetsCrossPriSel-r17");
    return c.get<dyn_r17_l_>();
  }
  const beta_offsets_cross_pri_r17_l& semi_static_r17() const
  {
    assert_choice_type(types::semi_static_r17, type_, "BetaOffsetsCrossPriSel-r17");
    return c.get<beta_offsets_cross_pri_r17_l>();
  }
  dyn_r17_l_&                   set_dyn_r17();
  beta_offsets_cross_pri_r17_l& set_semi_static_r17();

private:
  types                                                     type_;
  choice_buffer_t<beta_offsets_cross_pri_r17_l, dyn_r17_l_> c;

  void destroy_();
};

// BetaOffsetsCrossPriSelDCI-0-2-r17 ::= CHOICE
struct beta_offsets_cross_pri_sel_dci_0_2_r17_c {
  struct dyn_dci_0_2_r17_c_ {
    using one_bit_r17_l_  = std::array<beta_offsets_cross_pri_r17_l, 2>;
    using two_bits_r17_l_ = std::array<beta_offsets_cross_pri_r17_l, 4>;
    struct types_opts {
      enum options { one_bit_r17, two_bits_r17, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    dyn_dci_0_2_r17_c_() = default;
    dyn_dci_0_2_r17_c_(const dyn_dci_0_2_r17_c_& other);
    dyn_dci_0_2_r17_c_& operator=(const dyn_dci_0_2_r17_c_& other);
    ~dyn_dci_0_2_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    one_bit_r17_l_& one_bit_r17()
    {
      assert_choice_type(types::one_bit_r17, type_, "dynamicDCI-0-2-r17");
      return c.get<one_bit_r17_l_>();
    }
    two_bits_r17_l_& two_bits_r17()
    {
      assert_choice_type(types::two_bits_r17, type_, "dynamicDCI-0-2-r17");
      return c.get<two_bits_r17_l_>();
    }
    const one_bit_r17_l_& one_bit_r17() const
    {
      assert_choice_type(types::one_bit_r17, type_, "dynamicDCI-0-2-r17");
      return c.get<one_bit_r17_l_>();
    }
    const two_bits_r17_l_& two_bits_r17() const
    {
      assert_choice_type(types::two_bits_r17, type_, "dynamicDCI-0-2-r17");
      return c.get<two_bits_r17_l_>();
    }
    one_bit_r17_l_&  set_one_bit_r17();
    two_bits_r17_l_& set_two_bits_r17();

  private:
    types                                            type_;
    choice_buffer_t<one_bit_r17_l_, two_bits_r17_l_> c;

    void destroy_();
  };
  struct types_opts {
    enum options { dyn_dci_0_2_r17, semi_static_dci_0_2_r17, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  beta_offsets_cross_pri_sel_dci_0_2_r17_c() = default;
  beta_offsets_cross_pri_sel_dci_0_2_r17_c(const beta_offsets_cross_pri_sel_dci_0_2_r17_c& other);
  beta_offsets_cross_pri_sel_dci_0_2_r17_c& operator=(const beta_offsets_cross_pri_sel_dci_0_2_r17_c& other);
  ~beta_offsets_cross_pri_sel_dci_0_2_r17_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  dyn_dci_0_2_r17_c_& dyn_dci_0_2_r17()
  {
    assert_choice_type(types::dyn_dci_0_2_r17, type_, "BetaOffsetsCrossPriSelDCI-0-2-r17");
    return c.get<dyn_dci_0_2_r17_c_>();
  }
  beta_offsets_cross_pri_r17_l& semi_static_dci_0_2_r17()
  {
    assert_choice_type(types::semi_static_dci_0_2_r17, type_, "BetaOffsetsCrossPriSelDCI-0-2-r17");
    return c.get<beta_offsets_cross_pri_r17_l>();
  }
  const dyn_dci_0_2_r17_c_& dyn_dci_0_2_r17() const
  {
    assert_choice_type(types::dyn_dci_0_2_r17, type_, "BetaOffsetsCrossPriSelDCI-0-2-r17");
    return c.get<dyn_dci_0_2_r17_c_>();
  }
  const beta_offsets_cross_pri_r17_l& semi_static_dci_0_2_r17() const
  {
    assert_choice_type(types::semi_static_dci_0_2_r17, type_, "BetaOffsetsCrossPriSelDCI-0-2-r17");
    return c.get<beta_offsets_cross_pri_r17_l>();
  }
  dyn_dci_0_2_r17_c_&           set_dyn_dci_0_2_r17();
  beta_offsets_cross_pri_r17_l& set_semi_static_dci_0_2_r17();

private:
  types                                                             type_;
  choice_buffer_t<beta_offsets_cross_pri_r17_l, dyn_dci_0_2_r17_c_> c;

  void destroy_();
};

// CandidateBeamRSListExt-r16 ::= SEQUENCE (SIZE (1..48)) OF PRACH-ResourceDedicatedBFR
using candidate_beam_rs_list_ext_r16_l = dyn_array<prach_res_ded_bfr_c>;

// ConfiguredGrantConfig ::= SEQUENCE
struct cfg_grant_cfg_s {
  struct freq_hop_opts {
    enum options { intra_slot, inter_slot, nulltype } value;

    const char* to_string() const;
  };
  using freq_hop_e_ = enumerated<freq_hop_opts>;
  struct mcs_table_opts {
    enum options { qam256, qam64_low_se, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using mcs_table_e_ = enumerated<mcs_table_opts>;
  struct mcs_table_transform_precoder_opts {
    enum options { qam256, qam64_low_se, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using mcs_table_transform_precoder_e_ = enumerated<mcs_table_transform_precoder_opts>;
  struct res_alloc_opts {
    enum options { res_alloc_type0, res_alloc_type1, dyn_switch, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using res_alloc_e_ = enumerated<res_alloc_opts>;
  struct pwr_ctrl_loop_to_use_opts {
    enum options { n0, n1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using pwr_ctrl_loop_to_use_e_ = enumerated<pwr_ctrl_loop_to_use_opts>;
  struct transform_precoder_opts {
    enum options { enabled, disabled, nulltype } value;

    const char* to_string() const;
  };
  using transform_precoder_e_ = enumerated<transform_precoder_opts>;
  struct rep_k_opts {
    enum options { n1, n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using rep_k_e_ = enumerated<rep_k_opts>;
  struct rep_k_rv_opts {
    enum options { s1_neg0231, s2_neg0303, s3_neg0000, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using rep_k_rv_e_ = enumerated<rep_k_rv_opts>;
  struct periodicity_opts {
    enum options {
      sym2,
      sym7,
      sym1x14,
      sym2x14,
      sym4x14,
      sym5x14,
      sym8x14,
      sym10x14,
      sym16x14,
      sym20x14,
      sym32x14,
      sym40x14,
      sym64x14,
      sym80x14,
      sym128x14,
      sym160x14,
      sym256x14,
      sym320x14,
      sym512x14,
      sym640x14,
      sym1024x14,
      sym1280x14,
      sym2560x14,
      sym5120x14,
      sym6,
      sym1x12,
      sym2x12,
      sym4x12,
      sym5x12,
      sym8x12,
      sym10x12,
      sym16x12,
      sym20x12,
      sym32x12,
      sym40x12,
      sym64x12,
      sym80x12,
      sym128x12,
      sym160x12,
      sym256x12,
      sym320x12,
      sym512x12,
      sym640x12,
      sym1280x12,
      sym2560x12,
      nulltype
    } value;

    const char* to_string() const;
  };
  using periodicity_e_ = enumerated<periodicity_opts>;
  struct rrc_cfg_ul_grant_s_ {
    struct pusch_rep_type_ind_r16_opts {
      enum options { pusch_rep_type_a, pusch_rep_type_b, nulltype } value;

      const char* to_string() const;
    };
    using pusch_rep_type_ind_r16_e_ = enumerated<pusch_rep_type_ind_r16_opts>;
    struct freq_hop_pusch_rep_type_b_r16_opts {
      enum options { inter_repeat, inter_slot, nulltype } value;

      const char* to_string() const;
    };
    using freq_hop_pusch_rep_type_b_r16_e_ = enumerated<freq_hop_pusch_rep_type_b_r16_opts>;

    // member variables
    bool                ext                          = false;
    bool                dmrs_seq_initization_present = false;
    bool                srs_res_ind_present          = false;
    bool                freq_hop_offset_present      = false;
    uint16_t            time_domain_offset           = 0;
    uint8_t             time_domain_alloc            = 0;
    fixed_bitstring<18> freq_domain_alloc;
    uint8_t             ant_port                 = 0;
    uint8_t             dmrs_seq_initization     = 0;
    uint8_t             precoding_and_nof_layers = 0;
    uint8_t             srs_res_ind              = 0;
    uint8_t             mcs_and_tbs              = 0;
    uint16_t            freq_hop_offset          = 1;
    uint8_t             pathloss_ref_idx         = 0;
    // ...
    // group 0
    bool                             pusch_rep_type_ind_r16_present        = false;
    bool                             freq_hop_pusch_rep_type_b_r16_present = false;
    bool                             time_ref_sfn_r16_present              = false;
    pusch_rep_type_ind_r16_e_        pusch_rep_type_ind_r16;
    freq_hop_pusch_rep_type_b_r16_e_ freq_hop_pusch_rep_type_b_r16;
    // group 1
    bool                       pathloss_ref_idx2_r17_present         = false;
    bool                       srs_res_ind2_r17_present              = false;
    bool                       precoding_and_nof_layers2_r17_present = false;
    bool                       time_domain_alloc_v1710_present       = false;
    bool                       time_domain_offset_r17_present        = false;
    uint8_t                    pathloss_ref_idx2_r17                 = 0;
    uint8_t                    srs_res_ind2_r17                      = 0;
    uint8_t                    precoding_and_nof_layers2_r17         = 0;
    uint8_t                    time_domain_alloc_v1710               = 16;
    uint16_t                   time_domain_offset_r17                = 0;
    copy_ptr<cg_sdt_cfg_r17_s> cg_sdt_cfg_r17;

    // sequence methods
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };
  struct cg_min_dfi_delay_r16_opts {
    enum options {
      sym7,
      sym1x14,
      sym2x14,
      sym3x14,
      sym4x14,
      sym5x14,
      sym6x14,
      sym7x14,
      sym8x14,
      sym9x14,
      sym10x14,
      sym11x14,
      sym12x14,
      sym13x14,
      sym14x14,
      sym15x14,
      sym16x14,
      nulltype
    } value;

    const char* to_string() const;
  };
  using cg_min_dfi_delay_r16_e_    = enumerated<cg_min_dfi_delay_r16_opts>;
  using cg_cot_sharing_list_r16_l_ = dyn_array<cg_cot_sharing_r16_c>;
  struct start_from_rv0_r16_opts {
    enum options { on, off, nulltype } value;

    const char* to_string() const;
  };
  using start_from_rv0_r16_e_ = enumerated<start_from_rv0_r16_opts>;
  struct phy_prio_idx_r16_opts {
    enum options { p0, p1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using phy_prio_idx_r16_e_ = enumerated<phy_prio_idx_r16_opts>;
  struct map_pattern_r17_opts {
    enum options { cyclic_map, sequential_map, nulltype } value;

    const char* to_string() const;
  };
  using map_pattern_r17_e_ = enumerated<map_pattern_r17_opts>;
  struct pwr_ctrl_loop_to_use2_r17_opts {
    enum options { n0, n1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using pwr_ctrl_loop_to_use2_r17_e_ = enumerated<pwr_ctrl_loop_to_use2_r17_opts>;
  using cg_cot_sharing_list_r17_l_   = dyn_array<cg_cot_sharing_r17_c>;
  struct rep_k_v1710_opts {
    enum options { n12, n16, n24, n32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using rep_k_v1710_e_ = enumerated<rep_k_v1710_opts>;

  // member variables
  bool                               ext                                  = false;
  bool                               freq_hop_present                     = false;
  bool                               mcs_table_present                    = false;
  bool                               mcs_table_transform_precoder_present = false;
  bool                               uci_on_pusch_present                 = false;
  bool                               rbg_size_present                     = false;
  bool                               transform_precoder_present           = false;
  bool                               rep_k_rv_present                     = false;
  bool                               cfg_grant_timer_present              = false;
  bool                               rrc_cfg_ul_grant_present             = false;
  freq_hop_e_                        freq_hop;
  dmrs_ul_cfg_s                      cg_dmrs_cfg;
  mcs_table_e_                       mcs_table;
  mcs_table_transform_precoder_e_    mcs_table_transform_precoder;
  setup_release_c<cg_uci_on_pusch_c> uci_on_pusch;
  res_alloc_e_                       res_alloc;
  pwr_ctrl_loop_to_use_e_            pwr_ctrl_loop_to_use;
  uint8_t                            p0_pusch_alpha = 0;
  transform_precoder_e_              transform_precoder;
  uint8_t                            nrof_harq_processes = 1;
  rep_k_e_                           rep_k;
  rep_k_rv_e_                        rep_k_rv;
  periodicity_e_                     periodicity;
  uint8_t                            cfg_grant_timer = 1;
  rrc_cfg_ul_grant_s_                rrc_cfg_ul_grant;
  // ...
  // group 0
  bool                                 cg_retx_timer_r16_present         = false;
  bool                                 cg_min_dfi_delay_r16_present      = false;
  bool                                 cg_nrof_pusch_in_slot_r16_present = false;
  bool                                 cg_nrof_slots_r16_present         = false;
  bool                                 cg_uci_mux_r16_present            = false;
  bool                                 cg_cot_sharing_offset_r16_present = false;
  bool                                 beta_offset_cg_uci_r16_present    = false;
  bool                                 harq_proc_id_offset_r16_present   = false;
  bool                                 harq_proc_id_offset2_r16_present  = false;
  bool                                 cfg_grant_cfg_idx_r16_present     = false;
  bool                                 cfg_grant_cfg_idx_mac_r16_present = false;
  bool                                 periodicity_ext_r16_present       = false;
  bool                                 start_from_rv0_r16_present        = false;
  bool                                 phy_prio_idx_r16_present          = false;
  bool                                 autonomous_tx_r16_present         = false;
  uint8_t                              cg_retx_timer_r16                 = 1;
  cg_min_dfi_delay_r16_e_              cg_min_dfi_delay_r16;
  uint8_t                              cg_nrof_pusch_in_slot_r16 = 1;
  uint8_t                              cg_nrof_slots_r16         = 1;
  copy_ptr<cg_start_offsets_r16_s>     cg_start_offsets_r16;
  uint8_t                              cg_cot_sharing_offset_r16 = 1;
  uint8_t                              beta_offset_cg_uci_r16    = 0;
  copy_ptr<cg_cot_sharing_list_r16_l_> cg_cot_sharing_list_r16;
  uint8_t                              harq_proc_id_offset_r16   = 0;
  uint8_t                              harq_proc_id_offset2_r16  = 0;
  uint8_t                              cfg_grant_cfg_idx_r16     = 0;
  uint8_t                              cfg_grant_cfg_idx_mac_r16 = 0;
  uint16_t                             periodicity_ext_r16       = 1;
  start_from_rv0_r16_e_                start_from_rv0_r16;
  phy_prio_idx_r16_e_                  phy_prio_idx_r16;
  // group 1
  bool                                                           map_pattern_r17_present            = false;
  bool                                                           seq_offset_for_rv_r17_present      = false;
  bool                                                           p0_pusch_alpha2_r17_present        = false;
  bool                                                           pwr_ctrl_loop_to_use2_r17_present  = false;
  bool                                                           periodicity_ext_r17_present        = false;
  bool                                                           rep_k_v1710_present                = false;
  bool                                                           nrof_harq_processes_v1700_present  = false;
  bool                                                           harq_proc_id_offset2_v1700_present = false;
  bool                                                           cfg_grant_timer_v1700_present      = false;
  bool                                                           cg_min_dfi_delay_v1710_present     = false;
  copy_ptr<setup_release_c<beta_offsets_cross_pri_sel_cg_r17_c>> cg_beta_offsets_cross_pri0_r17;
  copy_ptr<setup_release_c<beta_offsets_cross_pri_sel_cg_r17_c>> cg_beta_offsets_cross_pri1_r17;
  map_pattern_r17_e_                                             map_pattern_r17;
  uint8_t                                                        seq_offset_for_rv_r17 = 0;
  uint8_t                                                        p0_pusch_alpha2_r17   = 0;
  pwr_ctrl_loop_to_use2_r17_e_                                   pwr_ctrl_loop_to_use2_r17;
  copy_ptr<cg_cot_sharing_list_r17_l_>                           cg_cot_sharing_list_r17;
  uint16_t                                                       periodicity_ext_r17 = 1;
  rep_k_v1710_e_                                                 rep_k_v1710;
  uint8_t                                                        nrof_harq_processes_v1700  = 17;
  uint8_t                                                        harq_proc_id_offset2_v1700 = 16;
  uint16_t                                                       cfg_grant_timer_v1700      = 33;
  uint16_t                                                       cg_min_dfi_delay_v1710     = 238;
  // group 2
  bool     harq_proc_id_offset_v1730_present = false;
  bool     cg_nrof_slots_r17_present         = false;
  uint8_t  harq_proc_id_offset_v1730         = 16;
  uint16_t cg_nrof_slots_r17                 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ConfiguredGrantConfigType2DeactivationState-r16 ::= SEQUENCE (SIZE (1..12)) OF INTEGER (0..11)
using cfg_grant_cfg_type2_deactivation_state_r16_l = bounded_array<uint8_t, 12>;

// DMRS-BundlingPUSCH-Config-r17 ::= SEQUENCE
struct dmrs_bundling_pusch_cfg_r17_s {
  struct pusch_freq_hop_interv_r17_opts {
    enum options { s2, s4, s5, s6, s8, s10, s12, s14, s16, s20, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using pusch_freq_hop_interv_r17_e_ = enumerated<pusch_freq_hop_interv_r17_opts>;

  // member variables
  bool                         ext                                   = false;
  bool                         pusch_dmrs_bundling_r17_present       = false;
  bool                         pusch_time_domain_win_len_r17_present = false;
  bool                         pusch_win_restart_r17_present         = false;
  bool                         pusch_freq_hop_interv_r17_present     = false;
  uint8_t                      pusch_time_domain_win_len_r17         = 2;
  pusch_freq_hop_interv_r17_e_ pusch_freq_hop_interv_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FrequencyHoppingOffsetListsDCI-0-2-r16 ::= SEQUENCE (SIZE (1..4)) OF INTEGER (1..274)
using freq_hop_offset_lists_dci_0_2_r16_l = bounded_array<uint16_t, 4>;

// InvalidSymbolPattern-r16 ::= SEQUENCE
struct invalid_symbol_pattern_r16_s {
  struct symbols_r16_c_ {
    struct types_opts {
      enum options { one_slot, two_slots, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    symbols_r16_c_() = default;
    symbols_r16_c_(const symbols_r16_c_& other);
    symbols_r16_c_& operator=(const symbols_r16_c_& other);
    ~symbols_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<14>& one_slot()
    {
      assert_choice_type(types::one_slot, type_, "symbols-r16");
      return c.get<fixed_bitstring<14>>();
    }
    fixed_bitstring<28>& two_slots()
    {
      assert_choice_type(types::two_slots, type_, "symbols-r16");
      return c.get<fixed_bitstring<28>>();
    }
    const fixed_bitstring<14>& one_slot() const
    {
      assert_choice_type(types::one_slot, type_, "symbols-r16");
      return c.get<fixed_bitstring<14>>();
    }
    const fixed_bitstring<28>& two_slots() const
    {
      assert_choice_type(types::two_slots, type_, "symbols-r16");
      return c.get<fixed_bitstring<28>>();
    }
    fixed_bitstring<14>& set_one_slot();
    fixed_bitstring<28>& set_two_slots();

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<28>> c;

    void destroy_();
  };
  struct periodicity_and_pattern_r16_c_ {
    struct types_opts {
      enum options { n2, n4, n5, n8, n10, n20, n40, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    periodicity_and_pattern_r16_c_() = default;
    periodicity_and_pattern_r16_c_(const periodicity_and_pattern_r16_c_& other);
    periodicity_and_pattern_r16_c_& operator=(const periodicity_and_pattern_r16_c_& other);
    ~periodicity_and_pattern_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<2>& n2()
    {
      assert_choice_type(types::n2, type_, "periodicityAndPattern-r16");
      return c.get<fixed_bitstring<2>>();
    }
    fixed_bitstring<4>& n4()
    {
      assert_choice_type(types::n4, type_, "periodicityAndPattern-r16");
      return c.get<fixed_bitstring<4>>();
    }
    fixed_bitstring<5>& n5()
    {
      assert_choice_type(types::n5, type_, "periodicityAndPattern-r16");
      return c.get<fixed_bitstring<5>>();
    }
    fixed_bitstring<8>& n8()
    {
      assert_choice_type(types::n8, type_, "periodicityAndPattern-r16");
      return c.get<fixed_bitstring<8>>();
    }
    fixed_bitstring<10>& n10()
    {
      assert_choice_type(types::n10, type_, "periodicityAndPattern-r16");
      return c.get<fixed_bitstring<10>>();
    }
    fixed_bitstring<20>& n20()
    {
      assert_choice_type(types::n20, type_, "periodicityAndPattern-r16");
      return c.get<fixed_bitstring<20>>();
    }
    fixed_bitstring<40>& n40()
    {
      assert_choice_type(types::n40, type_, "periodicityAndPattern-r16");
      return c.get<fixed_bitstring<40>>();
    }
    const fixed_bitstring<2>& n2() const
    {
      assert_choice_type(types::n2, type_, "periodicityAndPattern-r16");
      return c.get<fixed_bitstring<2>>();
    }
    const fixed_bitstring<4>& n4() const
    {
      assert_choice_type(types::n4, type_, "periodicityAndPattern-r16");
      return c.get<fixed_bitstring<4>>();
    }
    const fixed_bitstring<5>& n5() const
    {
      assert_choice_type(types::n5, type_, "periodicityAndPattern-r16");
      return c.get<fixed_bitstring<5>>();
    }
    const fixed_bitstring<8>& n8() const
    {
      assert_choice_type(types::n8, type_, "periodicityAndPattern-r16");
      return c.get<fixed_bitstring<8>>();
    }
    const fixed_bitstring<10>& n10() const
    {
      assert_choice_type(types::n10, type_, "periodicityAndPattern-r16");
      return c.get<fixed_bitstring<10>>();
    }
    const fixed_bitstring<20>& n20() const
    {
      assert_choice_type(types::n20, type_, "periodicityAndPattern-r16");
      return c.get<fixed_bitstring<20>>();
    }
    const fixed_bitstring<40>& n40() const
    {
      assert_choice_type(types::n40, type_, "periodicityAndPattern-r16");
      return c.get<fixed_bitstring<40>>();
    }
    fixed_bitstring<2>&  set_n2();
    fixed_bitstring<4>&  set_n4();
    fixed_bitstring<5>&  set_n5();
    fixed_bitstring<8>&  set_n8();
    fixed_bitstring<10>& set_n10();
    fixed_bitstring<20>& set_n20();
    fixed_bitstring<40>& set_n40();

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<40>> c;

    void destroy_();
  };

  // member variables
  bool                           ext                                 = false;
  bool                           periodicity_and_pattern_r16_present = false;
  symbols_r16_c_                 symbols_r16;
  periodicity_and_pattern_r16_c_ periodicity_and_pattern_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MPE-Resource-r17 ::= SEQUENCE
struct mpe_res_r17_s {
  struct mpe_ref_sig_r17_c_ {
    struct types_opts {
      enum options { csi_rs_res_r17, ssb_res_r17, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    mpe_ref_sig_r17_c_() = default;
    mpe_ref_sig_r17_c_(const mpe_ref_sig_r17_c_& other);
    mpe_ref_sig_r17_c_& operator=(const mpe_ref_sig_r17_c_& other);
    ~mpe_ref_sig_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& csi_rs_res_r17()
    {
      assert_choice_type(types::csi_rs_res_r17, type_, "mpe-ReferenceSignal-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ssb_res_r17()
    {
      assert_choice_type(types::ssb_res_r17, type_, "mpe-ReferenceSignal-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_res_r17() const
    {
      assert_choice_type(types::csi_rs_res_r17, type_, "mpe-ReferenceSignal-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_res_r17() const
    {
      assert_choice_type(types::ssb_res_r17, type_, "mpe-ReferenceSignal-r17");
      return c.get<uint8_t>();
    }
    uint8_t& set_csi_rs_res_r17();
    uint8_t& set_ssb_res_r17();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool               cell_r17_present    = false;
  bool               add_pci_r17_present = false;
  uint8_t            mpe_res_id_r17      = 1;
  uint8_t            cell_r17            = 0;
  uint8_t            add_pci_r17         = 1;
  mpe_ref_sig_r17_c_ mpe_ref_sig_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MinSchedulingOffsetK2-Values-r16 ::= SEQUENCE (SIZE (1..2)) OF INTEGER (0..16)
using min_sched_offset_k2_values_r16_l = bounded_array<uint8_t, 2>;

// MinSchedulingOffsetK2-Values-r17 ::= SEQUENCE (SIZE (1..2)) OF INTEGER (0..64)
using min_sched_offset_k2_values_r17_l = bounded_array<uint8_t, 2>;

// PUCCH-Config ::= SEQUENCE
struct pucch_cfg_s {
  using res_set_to_add_mod_list_l_               = dyn_array<pucch_res_set_s>;
  using res_set_to_release_list_l_               = bounded_array<uint8_t, 4>;
  using res_to_add_mod_list_l_                   = dyn_array<pucch_res_s>;
  using res_to_release_list_l_                   = dyn_array<uint8_t>;
  using sched_request_res_to_add_mod_list_l_     = dyn_array<sched_request_res_cfg_s>;
  using sched_request_res_to_release_list_l_     = bounded_array<uint8_t, 8>;
  using multi_csi_pucch_res_list_l_              = bounded_array<uint8_t, 2>;
  using dl_data_to_ul_ack_l_                     = bounded_array<uint8_t, 8>;
  using spatial_relation_info_to_add_mod_list_l_ = dyn_array<pucch_spatial_relation_info_s>;
  using spatial_relation_info_to_release_list_l_ = bounded_array<uint8_t, 8>;
  using res_to_add_mod_list_ext_v1610_l_         = dyn_array<pucch_res_ext_v1610_s>;
  struct subslot_len_for_pucch_r16_c_ {
    struct normal_cp_r16_opts {
      enum options { n2, n7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using normal_cp_r16_e_ = enumerated<normal_cp_r16_opts>;
    struct extended_cp_r16_opts {
      enum options { n2, n6, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using extended_cp_r16_e_ = enumerated<extended_cp_r16_opts>;
    struct types_opts {
      enum options { normal_cp_r16, extended_cp_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    subslot_len_for_pucch_r16_c_() = default;
    subslot_len_for_pucch_r16_c_(const subslot_len_for_pucch_r16_c_& other);
    subslot_len_for_pucch_r16_c_& operator=(const subslot_len_for_pucch_r16_c_& other);
    ~subslot_len_for_pucch_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    normal_cp_r16_e_& normal_cp_r16()
    {
      assert_choice_type(types::normal_cp_r16, type_, "subslotLengthForPUCCH-r16");
      return c.get<normal_cp_r16_e_>();
    }
    extended_cp_r16_e_& extended_cp_r16()
    {
      assert_choice_type(types::extended_cp_r16, type_, "subslotLengthForPUCCH-r16");
      return c.get<extended_cp_r16_e_>();
    }
    const normal_cp_r16_e_& normal_cp_r16() const
    {
      assert_choice_type(types::normal_cp_r16, type_, "subslotLengthForPUCCH-r16");
      return c.get<normal_cp_r16_e_>();
    }
    const extended_cp_r16_e_& extended_cp_r16() const
    {
      assert_choice_type(types::extended_cp_r16, type_, "subslotLengthForPUCCH-r16");
      return c.get<extended_cp_r16_e_>();
    }
    normal_cp_r16_e_&   set_normal_cp_r16();
    extended_cp_r16_e_& set_extended_cp_r16();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  using spatial_relation_info_to_add_mod_list_size_ext_v1610_l_ = dyn_array<pucch_spatial_relation_info_s>;
  using spatial_relation_info_to_release_list_size_ext_v1610_l_ = dyn_array<uint8_t>;
  using spatial_relation_info_to_add_mod_list_ext_v1610_l_      = dyn_array<pucch_spatial_relation_info_ext_r16_s>;
  using spatial_relation_info_to_release_list_ext_v1610_l_      = dyn_array<uint8_t>;
  using res_group_to_add_mod_list_r16_l_                        = dyn_array<pucch_res_group_r16_s>;
  using res_group_to_release_list_r16_l_                        = bounded_array<uint8_t, 4>;
  using sched_request_res_to_add_mod_list_ext_v1610_l_          = dyn_array<sched_request_res_cfg_ext_v1610_s>;
  struct map_pattern_r17_opts {
    enum options { cyclic_map, sequential_map, nulltype } value;

    const char* to_string() const;
  };
  using map_pattern_r17_e_                             = enumerated<map_pattern_r17_opts>;
  using pwr_ctrl_set_info_to_add_mod_list_r17_l_       = dyn_array<pucch_pwr_ctrl_set_info_r17_s>;
  using pwr_ctrl_set_info_to_release_list_r17_l_       = bounded_array<uint8_t, 8>;
  using sched_request_res_to_add_mod_list_ext_v1700_l_ = dyn_array<sched_request_res_cfg_ext_v1700_s>;

  // member variables
  bool                                     ext                    = false;
  bool                                     format1_present        = false;
  bool                                     format2_present        = false;
  bool                                     format3_present        = false;
  bool                                     format4_present        = false;
  bool                                     pucch_pwr_ctrl_present = false;
  res_set_to_add_mod_list_l_               res_set_to_add_mod_list;
  res_set_to_release_list_l_               res_set_to_release_list;
  res_to_add_mod_list_l_                   res_to_add_mod_list;
  res_to_release_list_l_                   res_to_release_list;
  setup_release_c<pucch_format_cfg_s>      format1;
  setup_release_c<pucch_format_cfg_s>      format2;
  setup_release_c<pucch_format_cfg_s>      format3;
  setup_release_c<pucch_format_cfg_s>      format4;
  sched_request_res_to_add_mod_list_l_     sched_request_res_to_add_mod_list;
  sched_request_res_to_release_list_l_     sched_request_res_to_release_list;
  multi_csi_pucch_res_list_l_              multi_csi_pucch_res_list;
  dl_data_to_ul_ack_l_                     dl_data_to_ul_ack;
  spatial_relation_info_to_add_mod_list_l_ spatial_relation_info_to_add_mod_list;
  spatial_relation_info_to_release_list_l_ spatial_relation_info_to_release_list;
  pucch_pwr_ctrl_s                         pucch_pwr_ctrl;
  // ...
  // group 0
  bool                                       nof_bits_for_pucch_res_ind_dci_1_2_r16_present = false;
  bool                                       dmrs_ul_transform_precoding_pucch_r16_present  = false;
  copy_ptr<res_to_add_mod_list_ext_v1610_l_> res_to_add_mod_list_ext_v1610;
  copy_ptr<setup_release_c<dyn_seq_of<integer<int8_t, -1, 15>, 1, 8>>>  dl_data_to_ul_ack_r16;
  copy_ptr<setup_release_c<dyn_seq_of<integer<uint8_t, 0, 15>, 1, 16>>> ul_access_cfg_list_dci_1_1_r16;
  copy_ptr<subslot_len_for_pucch_r16_c_>                                subslot_len_for_pucch_r16;
  copy_ptr<setup_release_c<dyn_seq_of<integer<uint8_t, 0, 15>, 1, 8>>>  dl_data_to_ul_ack_dci_1_2_r16;
  uint8_t                                                               nof_bits_for_pucch_res_ind_dci_1_2_r16 = 0;
  copy_ptr<spatial_relation_info_to_add_mod_list_size_ext_v1610_l_>
      spatial_relation_info_to_add_mod_list_size_ext_v1610;
  copy_ptr<spatial_relation_info_to_release_list_size_ext_v1610_l_>
                                                                  spatial_relation_info_to_release_list_size_ext_v1610;
  copy_ptr<spatial_relation_info_to_add_mod_list_ext_v1610_l_>    spatial_relation_info_to_add_mod_list_ext_v1610;
  copy_ptr<spatial_relation_info_to_release_list_ext_v1610_l_>    spatial_relation_info_to_release_list_ext_v1610;
  copy_ptr<res_group_to_add_mod_list_r16_l_>                      res_group_to_add_mod_list_r16;
  copy_ptr<res_group_to_release_list_r16_l_>                      res_group_to_release_list_r16;
  copy_ptr<setup_release_c<dyn_seq_of<sps_pucch_an_r16_s, 1, 4>>> sps_pucch_an_list_r16;
  copy_ptr<sched_request_res_to_add_mod_list_ext_v1610_l_>        sched_request_res_to_add_mod_list_ext_v1610;
  // group 1
  bool                                                                   map_pattern_r17_present              = false;
  bool                                                                   second_tpc_field_dci_1_1_r17_present = false;
  bool                                                                   second_tpc_field_dci_1_2_r17_present = false;
  copy_ptr<setup_release_c<pucch_format_cfg_s>>                          format0_r17;
  copy_ptr<setup_release_c<pucch_format_cfg_ext_r17_s>>                  format2_ext_r17;
  copy_ptr<setup_release_c<pucch_format_cfg_ext_r17_s>>                  format3_ext_r17;
  copy_ptr<setup_release_c<pucch_format_cfg_ext_r17_s>>                  format4_ext_r17;
  copy_ptr<setup_release_c<dyn_seq_of<integer<uint8_t, 0, 15>, 1, 16>>>  ul_access_cfg_list_dci_1_2_r17;
  map_pattern_r17_e_                                                     map_pattern_r17;
  copy_ptr<pwr_ctrl_set_info_to_add_mod_list_r17_l_>                     pwr_ctrl_set_info_to_add_mod_list_r17;
  copy_ptr<pwr_ctrl_set_info_to_release_list_r17_l_>                     pwr_ctrl_set_info_to_release_list_r17;
  copy_ptr<setup_release_c<dyn_seq_of<integer<int16_t, -1, 127>, 1, 8>>> dl_data_to_ul_ack_r17;
  copy_ptr<setup_release_c<dyn_seq_of<integer<uint8_t, 0, 127>, 1, 8>>>  dl_data_to_ul_ack_dci_1_2_r17;
  copy_ptr<setup_release_c<dyn_seq_of<integer<uint8_t, 0, 2>, 1, 3>>>    ul_access_cfg_list_dci_1_1_r17;
  copy_ptr<sched_request_res_to_add_mod_list_ext_v1700_l_>               sched_request_res_to_add_mod_list_ext_v1700;
  copy_ptr<setup_release_c<dmrs_bundling_pucch_cfg_r17_s>>               dmrs_bundling_pucch_cfg_r17;
  copy_ptr<setup_release_c<dyn_seq_of<integer<uint8_t, 16, 31>, 1, 8>>>  dl_data_to_ul_ack_v1700;
  copy_ptr<setup_release_c<dyn_seq_of<integer<uint8_t, 0, 15>, 1, 8>>>   dl_data_to_ul_ack_multicast_dci_format4_1_r17;
  copy_ptr<setup_release_c<dyn_seq_of<sps_pucch_an_r16_s, 1, 4>>>        sps_pucch_an_list_multicast_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-PowerControl ::= SEQUENCE
struct pusch_pwr_ctrl_s {
  using p0_alpha_sets_l_                   = dyn_array<p0_pusch_alpha_set_s>;
  using pathloss_ref_rs_to_add_mod_list_l_ = dyn_array<pusch_pathloss_ref_rs_s>;
  using pathloss_ref_rs_to_release_list_l_ = bounded_array<uint8_t, 4>;
  using sri_pusch_map_to_add_mod_list_l_   = dyn_array<sri_pusch_pwr_ctrl_s>;
  using sri_pusch_map_to_release_list_l_   = bounded_array<uint8_t, 16>;

  // member variables
  bool                               tpc_accumulation_present               = false;
  bool                               msg3_alpha_present                     = false;
  bool                               p0_nominal_without_grant_present       = false;
  bool                               two_pusch_pc_adjustment_states_present = false;
  bool                               delta_mcs_present                      = false;
  alpha_e                            msg3_alpha;
  int16_t                            p0_nominal_without_grant = -202;
  p0_alpha_sets_l_                   p0_alpha_sets;
  pathloss_ref_rs_to_add_mod_list_l_ pathloss_ref_rs_to_add_mod_list;
  pathloss_ref_rs_to_release_list_l_ pathloss_ref_rs_to_release_list;
  sri_pusch_map_to_add_mod_list_l_   sri_pusch_map_to_add_mod_list;
  sri_pusch_map_to_release_list_l_   sri_pusch_map_to_release_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-PowerControl-v1610 ::= SEQUENCE
struct pusch_pwr_ctrl_v1610_s {
  using pathloss_ref_rs_to_add_mod_list_size_ext_v1610_l_ = dyn_array<pusch_pathloss_ref_rs_r16_s>;
  using pathloss_ref_rs_to_release_list_size_ext_v1610_l_ = dyn_array<uint8_t>;
  using p0_pusch_set_list_r16_l_                          = dyn_array<p0_pusch_set_r16_s>;
  struct olpc_param_set_s_ {
    bool    olpc_param_set_dci_0_1_r16_present = false;
    bool    olpc_param_set_dci_0_2_r16_present = false;
    uint8_t olpc_param_set_dci_0_1_r16         = 1;
    uint8_t olpc_param_set_dci_0_2_r16         = 1;
  };
  using sri_pusch_map_to_add_mod_list2_r17_l_ = dyn_array<sri_pusch_pwr_ctrl_s>;
  using sri_pusch_map_to_release_list2_r17_l_ = bounded_array<uint8_t, 16>;
  using p0_pusch_set_list2_r17_l_             = dyn_array<p0_pusch_set_r16_s>;
  using dummy_l_                              = dyn_array<dummy_pathloss_ref_rs_v1710_s>;

  // member variables
  bool                                              ext                    = false;
  bool                                              olpc_param_set_present = false;
  pathloss_ref_rs_to_add_mod_list_size_ext_v1610_l_ pathloss_ref_rs_to_add_mod_list_size_ext_v1610;
  pathloss_ref_rs_to_release_list_size_ext_v1610_l_ pathloss_ref_rs_to_release_list_size_ext_v1610;
  p0_pusch_set_list_r16_l_                          p0_pusch_set_list_r16;
  olpc_param_set_s_                                 olpc_param_set;
  // ...
  // group 0
  copy_ptr<sri_pusch_map_to_add_mod_list2_r17_l_> sri_pusch_map_to_add_mod_list2_r17;
  copy_ptr<sri_pusch_map_to_release_list2_r17_l_> sri_pusch_map_to_release_list2_r17;
  copy_ptr<p0_pusch_set_list2_r17_l_>             p0_pusch_set_list2_r17;
  copy_ptr<dummy_l_>                              dummy;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-TimeDomainResourceAllocationList-r16 ::= SEQUENCE (SIZE (1..64)) OF PUSCH-TimeDomainResourceAllocation-r16
using pusch_time_domain_res_alloc_list_r16_l = dyn_array<pusch_time_domain_res_alloc_r16_s>;

// SRS-PosResourceSet-r16 ::= SEQUENCE
struct srs_pos_res_set_r16_s {
  using srs_pos_res_id_list_r16_l_ = bounded_array<uint8_t, 16>;
  struct res_type_r16_c_ {
    struct aperiodic_r16_s_ {
      using aperiodic_srs_res_trigger_list_r16_l_ = bounded_array<uint8_t, 3>;

      // member variables
      bool                                  ext = false;
      aperiodic_srs_res_trigger_list_r16_l_ aperiodic_srs_res_trigger_list_r16;
      // ...
    };
    struct semi_persistent_r16_s_ {
      bool ext = false;
      // ...
    };
    struct periodic_r16_s_ {
      bool ext = false;
      // ...
    };
    struct types_opts {
      enum options { aperiodic_r16, semi_persistent_r16, periodic_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    res_type_r16_c_() = default;
    res_type_r16_c_(const res_type_r16_c_& other);
    res_type_r16_c_& operator=(const res_type_r16_c_& other);
    ~res_type_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    aperiodic_r16_s_& aperiodic_r16()
    {
      assert_choice_type(types::aperiodic_r16, type_, "resourceType-r16");
      return c.get<aperiodic_r16_s_>();
    }
    semi_persistent_r16_s_& semi_persistent_r16()
    {
      assert_choice_type(types::semi_persistent_r16, type_, "resourceType-r16");
      return c.get<semi_persistent_r16_s_>();
    }
    periodic_r16_s_& periodic_r16()
    {
      assert_choice_type(types::periodic_r16, type_, "resourceType-r16");
      return c.get<periodic_r16_s_>();
    }
    const aperiodic_r16_s_& aperiodic_r16() const
    {
      assert_choice_type(types::aperiodic_r16, type_, "resourceType-r16");
      return c.get<aperiodic_r16_s_>();
    }
    const semi_persistent_r16_s_& semi_persistent_r16() const
    {
      assert_choice_type(types::semi_persistent_r16, type_, "resourceType-r16");
      return c.get<semi_persistent_r16_s_>();
    }
    const periodic_r16_s_& periodic_r16() const
    {
      assert_choice_type(types::periodic_r16, type_, "resourceType-r16");
      return c.get<periodic_r16_s_>();
    }
    aperiodic_r16_s_&       set_aperiodic_r16();
    semi_persistent_r16_s_& set_semi_persistent_r16();
    periodic_r16_s_&        set_periodic_r16();

  private:
    types                                                                      type_;
    choice_buffer_t<aperiodic_r16_s_, periodic_r16_s_, semi_persistent_r16_s_> c;

    void destroy_();
  };
  struct pathloss_ref_rs_pos_r16_c_ {
    struct types_opts {
      enum options { ssb_idx_serving_r16, ssb_ncell_r16, dl_prs_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    pathloss_ref_rs_pos_r16_c_() = default;
    pathloss_ref_rs_pos_r16_c_(const pathloss_ref_rs_pos_r16_c_& other);
    pathloss_ref_rs_pos_r16_c_& operator=(const pathloss_ref_rs_pos_r16_c_& other);
    ~pathloss_ref_rs_pos_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx_serving_r16()
    {
      assert_choice_type(types::ssb_idx_serving_r16, type_, "pathlossReferenceRS-Pos-r16");
      return c.get<uint8_t>();
    }
    ssb_info_ncell_r16_s& ssb_ncell_r16()
    {
      assert_choice_type(types::ssb_ncell_r16, type_, "pathlossReferenceRS-Pos-r16");
      return c.get<ssb_info_ncell_r16_s>();
    }
    dl_prs_info_r16_s& dl_prs_r16()
    {
      assert_choice_type(types::dl_prs_r16, type_, "pathlossReferenceRS-Pos-r16");
      return c.get<dl_prs_info_r16_s>();
    }
    const uint8_t& ssb_idx_serving_r16() const
    {
      assert_choice_type(types::ssb_idx_serving_r16, type_, "pathlossReferenceRS-Pos-r16");
      return c.get<uint8_t>();
    }
    const ssb_info_ncell_r16_s& ssb_ncell_r16() const
    {
      assert_choice_type(types::ssb_ncell_r16, type_, "pathlossReferenceRS-Pos-r16");
      return c.get<ssb_info_ncell_r16_s>();
    }
    const dl_prs_info_r16_s& dl_prs_r16() const
    {
      assert_choice_type(types::dl_prs_r16, type_, "pathlossReferenceRS-Pos-r16");
      return c.get<dl_prs_info_r16_s>();
    }
    uint8_t&              set_ssb_idx_serving_r16();
    ssb_info_ncell_r16_s& set_ssb_ncell_r16();
    dl_prs_info_r16_s&    set_dl_prs_r16();

  private:
    types                                                    type_;
    choice_buffer_t<dl_prs_info_r16_s, ssb_info_ncell_r16_s> c;

    void destroy_();
  };

  // member variables
  bool                       ext                             = false;
  bool                       alpha_r16_present               = false;
  bool                       p0_r16_present                  = false;
  bool                       pathloss_ref_rs_pos_r16_present = false;
  uint8_t                    srs_pos_res_set_id_r16          = 0;
  srs_pos_res_id_list_r16_l_ srs_pos_res_id_list_r16;
  res_type_r16_c_            res_type_r16;
  alpha_e                    alpha_r16;
  int16_t                    p0_r16 = -202;
  pathloss_ref_rs_pos_r16_c_ pathloss_ref_rs_pos_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-ResourceSet ::= SEQUENCE
struct srs_res_set_s {
  using srs_res_id_list_l_ = bounded_array<uint8_t, 16>;
  struct res_type_c_ {
    struct aperiodic_s_ {
      using aperiodic_srs_res_trigger_list_l_ = bounded_array<uint8_t, 2>;

      // member variables
      bool    ext                       = false;
      bool    csi_rs_present            = false;
      bool    slot_offset_present       = false;
      uint8_t aperiodic_srs_res_trigger = 1;
      uint8_t csi_rs                    = 0;
      uint8_t slot_offset               = 1;
      // ...
      // group 0
      copy_ptr<aperiodic_srs_res_trigger_list_l_> aperiodic_srs_res_trigger_list;

      // sequence methods
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
    };
    struct semi_persistent_s_ {
      bool    ext                       = false;
      bool    associated_csi_rs_present = false;
      uint8_t associated_csi_rs         = 0;
      // ...
    };
    struct periodic_s_ {
      bool    ext                       = false;
      bool    associated_csi_rs_present = false;
      uint8_t associated_csi_rs         = 0;
      // ...
    };
    struct types_opts {
      enum options { aperiodic, semi_persistent, periodic, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    res_type_c_() = default;
    res_type_c_(const res_type_c_& other);
    res_type_c_& operator=(const res_type_c_& other);
    ~res_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    aperiodic_s_& aperiodic()
    {
      assert_choice_type(types::aperiodic, type_, "resourceType");
      return c.get<aperiodic_s_>();
    }
    semi_persistent_s_& semi_persistent()
    {
      assert_choice_type(types::semi_persistent, type_, "resourceType");
      return c.get<semi_persistent_s_>();
    }
    periodic_s_& periodic()
    {
      assert_choice_type(types::periodic, type_, "resourceType");
      return c.get<periodic_s_>();
    }
    const aperiodic_s_& aperiodic() const
    {
      assert_choice_type(types::aperiodic, type_, "resourceType");
      return c.get<aperiodic_s_>();
    }
    const semi_persistent_s_& semi_persistent() const
    {
      assert_choice_type(types::semi_persistent, type_, "resourceType");
      return c.get<semi_persistent_s_>();
    }
    const periodic_s_& periodic() const
    {
      assert_choice_type(types::periodic, type_, "resourceType");
      return c.get<periodic_s_>();
    }
    aperiodic_s_&       set_aperiodic();
    semi_persistent_s_& set_semi_persistent();
    periodic_s_&        set_periodic();

  private:
    types                                                          type_;
    choice_buffer_t<aperiodic_s_, periodic_s_, semi_persistent_s_> c;

    void destroy_();
  };
  struct usage_opts {
    enum options { beam_management, codebook, non_codebook, ant_switching, nulltype } value;

    const char* to_string() const;
  };
  using usage_e_ = enumerated<usage_opts>;
  struct srs_pwr_ctrl_adjustment_states_opts {
    enum options { same_as_fci2, separate_closed_loop, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using srs_pwr_ctrl_adjustment_states_e_ = enumerated<srs_pwr_ctrl_adjustment_states_opts>;
  using available_slot_offset_list_r17_l_ = bounded_array<uint8_t, 4>;

  // member variables
  bool                              ext                                    = false;
  bool                              alpha_present                          = false;
  bool                              p0_present                             = false;
  bool                              pathloss_ref_rs_present                = false;
  bool                              srs_pwr_ctrl_adjustment_states_present = false;
  uint8_t                           srs_res_set_id                         = 0;
  srs_res_id_list_l_                srs_res_id_list;
  res_type_c_                       res_type;
  usage_e_                          usage;
  alpha_e                           alpha;
  int16_t                           p0 = -202;
  pathloss_ref_rs_cfg_c             pathloss_ref_rs;
  srs_pwr_ctrl_adjustment_states_e_ srs_pwr_ctrl_adjustment_states;
  // ...
  // group 0
  copy_ptr<setup_release_c<dyn_seq_of<pathloss_ref_rs_r16_s, 1, 64>>> pathloss_ref_rs_list_r16;
  // group 1
  bool                                        usage_pdc_r17_present                    = false;
  bool                                        follow_unified_tci_state_srs_r17_present = false;
  copy_ptr<available_slot_offset_list_r17_l_> available_slot_offset_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UCI-OnPUSCH-ListDCI-0-1-r16 ::= SEQUENCE (SIZE (1..2)) OF UCI-OnPUSCH
using uci_on_pusch_list_dci_0_1_r16_l = dyn_array<uci_on_pusch_s>;

// UCI-OnPUSCH-ListDCI-0-2-r16 ::= SEQUENCE (SIZE (1..2)) OF UCI-OnPUSCH-DCI-0-2-r16
using uci_on_pusch_list_dci_0_2_r16_l = dyn_array<uci_on_pusch_dci_0_2_r16_s>;

// UL-AccessConfigListDCI-0-1-r16 ::= SEQUENCE (SIZE (1..64)) OF INTEGER (0..63)
using ul_access_cfg_list_dci_0_1_r16_l = dyn_array<uint8_t>;

// UL-AccessConfigListDCI-0-1-r17 ::= SEQUENCE (SIZE (1..3)) OF INTEGER (0..2)
using ul_access_cfg_list_dci_0_1_r17_l = bounded_array<uint8_t, 3>;

// UL-AccessConfigListDCI-0-2-r17 ::= SEQUENCE (SIZE (1..64)) OF INTEGER (0..63)
using ul_access_cfg_list_dci_0_2_r17_l = dyn_array<uint8_t>;

// BeamFailureRecoveryConfig ::= SEQUENCE
struct beam_fail_recovery_cfg_s {
  using candidate_beam_rs_list_l_ = dyn_array<prach_res_ded_bfr_c>;
  struct ssb_per_rach_occasion_opts {
    enum options { one_eighth, one_fourth, one_half, one, two, four, eight, sixteen, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using ssb_per_rach_occasion_e_ = enumerated<ssb_per_rach_occasion_opts>;
  struct beam_fail_recovery_timer_opts {
    enum options { ms10, ms20, ms40, ms60, ms80, ms100, ms150, ms200, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using beam_fail_recovery_timer_e_ = enumerated<beam_fail_recovery_timer_opts>;

  // member variables
  bool                        ext                              = false;
  bool                        root_seq_idx_bfr_present         = false;
  bool                        rach_cfg_bfr_present             = false;
  bool                        rsrp_thres_ssb_present           = false;
  bool                        ssb_per_rach_occasion_present    = false;
  bool                        ra_ssb_occasion_mask_idx_present = false;
  bool                        recovery_search_space_id_present = false;
  bool                        ra_prioritization_present        = false;
  bool                        beam_fail_recovery_timer_present = false;
  uint8_t                     root_seq_idx_bfr                 = 0;
  rach_cfg_generic_s          rach_cfg_bfr;
  uint8_t                     rsrp_thres_ssb = 0;
  candidate_beam_rs_list_l_   candidate_beam_rs_list;
  ssb_per_rach_occasion_e_    ssb_per_rach_occasion;
  uint8_t                     ra_ssb_occasion_mask_idx = 0;
  uint8_t                     recovery_search_space_id = 0;
  ra_prioritization_s         ra_prioritization;
  beam_fail_recovery_timer_e_ beam_fail_recovery_timer;
  // ...
  // group 0
  bool                 msg1_subcarrier_spacing_present = false;
  subcarrier_spacing_e msg1_subcarrier_spacing;
  // group 1
  copy_ptr<ra_prioritization_s>                                     ra_prioritization_two_step_r16;
  copy_ptr<setup_release_c<dyn_seq_of<prach_res_ded_bfr_c, 1, 48>>> candidate_beam_rs_list_ext_v1610;
  // group 2
  bool sp_cell_bfr_cbra_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ConfiguredGrantConfigToAddModList-r16 ::= SEQUENCE (SIZE (1..12)) OF ConfiguredGrantConfig
using cfg_grant_cfg_to_add_mod_list_r16_l = dyn_array<cfg_grant_cfg_s>;

// ConfiguredGrantConfigToReleaseList-r16 ::= SEQUENCE (SIZE (1..12)) OF INTEGER (0..11)
using cfg_grant_cfg_to_release_list_r16_l = bounded_array<uint8_t, 12>;

// ConfiguredGrantConfigType2DeactivationStateList-r16 ::= SEQUENCE (SIZE (1..16)) OF
// ConfiguredGrantConfigType2DeactivationState-r16
using cfg_grant_cfg_type2_deactivation_state_list_r16_l = dyn_array<cfg_grant_cfg_type2_deactivation_state_r16_l>;

// LBT-FailureRecoveryConfig-r16 ::= SEQUENCE
struct lbt_fail_recovery_cfg_r16_s {
  struct lbt_fail_instance_max_count_r16_opts {
    enum options { n4, n8, n16, n32, n64, n128, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using lbt_fail_instance_max_count_r16_e_ = enumerated<lbt_fail_instance_max_count_r16_opts>;
  struct lbt_fail_detection_timer_r16_opts {
    enum options { ms10, ms20, ms40, ms80, ms160, ms320, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using lbt_fail_detection_timer_r16_e_ = enumerated<lbt_fail_detection_timer_r16_opts>;

  // member variables
  bool                               ext = false;
  lbt_fail_instance_max_count_r16_e_ lbt_fail_instance_max_count_r16;
  lbt_fail_detection_timer_r16_e_    lbt_fail_detection_timer_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-ConfigurationList-r16 ::= SEQUENCE (SIZE (1..2)) OF PUCCH-Config
using pucch_cfg_list_r16_l = dyn_array<pucch_cfg_s>;

// PUSCH-Config ::= SEQUENCE
struct pusch_cfg_s {
  struct tx_cfg_opts {
    enum options { codebook, non_codebook, nulltype } value;

    const char* to_string() const;
  };
  using tx_cfg_e_ = enumerated<tx_cfg_opts>;
  struct freq_hop_opts {
    enum options { intra_slot, inter_slot, nulltype } value;

    const char* to_string() const;
  };
  using freq_hop_e_              = enumerated<freq_hop_opts>;
  using freq_hop_offset_lists_l_ = bounded_array<uint16_t, 4>;
  struct res_alloc_opts {
    enum options { res_alloc_type0, res_alloc_type1, dyn_switch, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using res_alloc_e_ = enumerated<res_alloc_opts>;
  struct pusch_aggregation_factor_opts {
    enum options { n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using pusch_aggregation_factor_e_ = enumerated<pusch_aggregation_factor_opts>;
  struct mcs_table_opts {
    enum options { qam256, qam64_low_se, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using mcs_table_e_ = enumerated<mcs_table_opts>;
  struct mcs_table_transform_precoder_opts {
    enum options { qam256, qam64_low_se, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using mcs_table_transform_precoder_e_ = enumerated<mcs_table_transform_precoder_opts>;
  struct transform_precoder_opts {
    enum options { enabled, disabled, nulltype } value;

    const char* to_string() const;
  };
  using transform_precoder_e_ = enumerated<transform_precoder_opts>;
  struct codebook_subset_opts {
    enum options { fully_and_partial_and_non_coherent, partial_and_non_coherent, non_coherent, nulltype } value;

    const char* to_string() const;
  };
  using codebook_subset_e_ = enumerated<codebook_subset_opts>;
  struct freq_hop_dci_0_2_r16_c_ {
    struct pusch_rep_type_a_opts {
      enum options { intra_slot, inter_slot, nulltype } value;

      const char* to_string() const;
    };
    using pusch_rep_type_a_e_ = enumerated<pusch_rep_type_a_opts>;
    struct pusch_rep_type_b_opts {
      enum options { inter_repeat, inter_slot, nulltype } value;

      const char* to_string() const;
    };
    using pusch_rep_type_b_e_ = enumerated<pusch_rep_type_b_opts>;
    struct types_opts {
      enum options { pusch_rep_type_a, pusch_rep_type_b, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    freq_hop_dci_0_2_r16_c_() = default;
    freq_hop_dci_0_2_r16_c_(const freq_hop_dci_0_2_r16_c_& other);
    freq_hop_dci_0_2_r16_c_& operator=(const freq_hop_dci_0_2_r16_c_& other);
    ~freq_hop_dci_0_2_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pusch_rep_type_a_e_& pusch_rep_type_a()
    {
      assert_choice_type(types::pusch_rep_type_a, type_, "frequencyHoppingDCI-0-2-r16");
      return c.get<pusch_rep_type_a_e_>();
    }
    pusch_rep_type_b_e_& pusch_rep_type_b()
    {
      assert_choice_type(types::pusch_rep_type_b, type_, "frequencyHoppingDCI-0-2-r16");
      return c.get<pusch_rep_type_b_e_>();
    }
    const pusch_rep_type_a_e_& pusch_rep_type_a() const
    {
      assert_choice_type(types::pusch_rep_type_a, type_, "frequencyHoppingDCI-0-2-r16");
      return c.get<pusch_rep_type_a_e_>();
    }
    const pusch_rep_type_b_e_& pusch_rep_type_b() const
    {
      assert_choice_type(types::pusch_rep_type_b, type_, "frequencyHoppingDCI-0-2-r16");
      return c.get<pusch_rep_type_b_e_>();
    }
    pusch_rep_type_a_e_& set_pusch_rep_type_a();
    pusch_rep_type_b_e_& set_pusch_rep_type_b();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct codebook_subset_dci_0_2_r16_opts {
    enum options { fully_and_partial_and_non_coherent, partial_and_non_coherent, non_coherent, nulltype } value;

    const char* to_string() const;
  };
  using codebook_subset_dci_0_2_r16_e_ = enumerated<codebook_subset_dci_0_2_r16_opts>;
  struct mcs_table_dci_0_2_r16_opts {
    enum options { qam256, qam64_low_se, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using mcs_table_dci_0_2_r16_e_ = enumerated<mcs_table_dci_0_2_r16_opts>;
  struct mcs_table_transform_precoder_dci_0_2_r16_opts {
    enum options { qam256, qam64_low_se, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using mcs_table_transform_precoder_dci_0_2_r16_e_ = enumerated<mcs_table_transform_precoder_dci_0_2_r16_opts>;
  struct pusch_rep_type_ind_dci_0_2_r16_opts {
    enum options { pusch_rep_type_a, pusch_rep_type_b, nulltype } value;

    const char* to_string() const;
  };
  using pusch_rep_type_ind_dci_0_2_r16_e_ = enumerated<pusch_rep_type_ind_dci_0_2_r16_opts>;
  struct res_alloc_dci_0_2_r16_opts {
    enum options { res_alloc_type0, res_alloc_type1, dyn_switch, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using res_alloc_dci_0_2_r16_e_ = enumerated<res_alloc_dci_0_2_r16_opts>;
  struct res_alloc_type1_granularity_dci_0_2_r16_opts {
    enum options { n2, n4, n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using res_alloc_type1_granularity_dci_0_2_r16_e_ = enumerated<res_alloc_type1_granularity_dci_0_2_r16_opts>;
  struct pusch_rep_type_ind_dci_0_1_r16_opts {
    enum options { pusch_rep_type_a, pusch_rep_type_b, nulltype } value;

    const char* to_string() const;
  };
  using pusch_rep_type_ind_dci_0_1_r16_e_ = enumerated<pusch_rep_type_ind_dci_0_1_r16_opts>;
  struct freq_hop_dci_0_1_r16_opts {
    enum options { inter_repeat, inter_slot, nulltype } value;

    const char* to_string() const;
  };
  using freq_hop_dci_0_1_r16_e_ = enumerated<freq_hop_dci_0_1_r16_opts>;
  struct ul_full_pwr_tx_r16_opts {
    enum options { fullpwr, fullpwr_mode1, fullpwr_mode2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ul_full_pwr_tx_r16_e_ = enumerated<ul_full_pwr_tx_r16_opts>;
  struct map_pattern_r17_opts {
    enum options { cyclic_map, sequential_map, nulltype } value;

    const char* to_string() const;
  };
  using map_pattern_r17_e_                  = enumerated<map_pattern_r17_opts>;
  using mpe_res_pool_to_add_mod_list_r17_l_ = dyn_array<mpe_res_r17_s>;
  using mpe_res_pool_to_release_list_r17_l_ = dyn_array<uint8_t>;

  // member variables
  bool                                                              ext                                  = false;
  bool                                                              data_scrambling_id_pusch_present     = false;
  bool                                                              tx_cfg_present                       = false;
  bool                                                              dmrs_ul_for_pusch_map_type_a_present = false;
  bool                                                              dmrs_ul_for_pusch_map_type_b_present = false;
  bool                                                              pusch_pwr_ctrl_present               = false;
  bool                                                              freq_hop_present                     = false;
  bool                                                              pusch_time_domain_alloc_list_present = false;
  bool                                                              pusch_aggregation_factor_present     = false;
  bool                                                              mcs_table_present                    = false;
  bool                                                              mcs_table_transform_precoder_present = false;
  bool                                                              transform_precoder_present           = false;
  bool                                                              codebook_subset_present              = false;
  bool                                                              max_rank_present                     = false;
  bool                                                              rbg_size_present                     = false;
  bool                                                              uci_on_pusch_present                 = false;
  bool                                                              tp_pi2_bpsk_present                  = false;
  uint16_t                                                          data_scrambling_id_pusch             = 0;
  tx_cfg_e_                                                         tx_cfg;
  setup_release_c<dmrs_ul_cfg_s>                                    dmrs_ul_for_pusch_map_type_a;
  setup_release_c<dmrs_ul_cfg_s>                                    dmrs_ul_for_pusch_map_type_b;
  pusch_pwr_ctrl_s                                                  pusch_pwr_ctrl;
  freq_hop_e_                                                       freq_hop;
  freq_hop_offset_lists_l_                                          freq_hop_offset_lists;
  res_alloc_e_                                                      res_alloc;
  setup_release_c<dyn_seq_of<pusch_time_domain_res_alloc_s, 1, 16>> pusch_time_domain_alloc_list;
  pusch_aggregation_factor_e_                                       pusch_aggregation_factor;
  mcs_table_e_                                                      mcs_table;
  mcs_table_transform_precoder_e_                                   mcs_table_transform_precoder;
  transform_precoder_e_                                             transform_precoder;
  codebook_subset_e_                                                codebook_subset;
  uint8_t                                                           max_rank = 1;
  setup_release_c<uci_on_pusch_s>                                   uci_on_pusch;
  // ...
  // group 0
  bool harq_process_num_size_dci_0_2_r16_present           = false;
  bool dmrs_seq_initization_dci_0_2_r16_present            = false;
  bool nof_bits_for_rv_dci_0_2_r16_present                 = false;
  bool ant_ports_field_presence_dci_0_2_r16_present        = false;
  bool codebook_subset_dci_0_2_r16_present                 = false;
  bool invalid_symbol_pattern_ind_dci_0_2_r16_present      = false;
  bool max_rank_dci_0_2_r16_present                        = false;
  bool mcs_table_dci_0_2_r16_present                       = false;
  bool mcs_table_transform_precoder_dci_0_2_r16_present    = false;
  bool prio_ind_dci_0_2_r16_present                        = false;
  bool pusch_rep_type_ind_dci_0_2_r16_present              = false;
  bool res_alloc_dci_0_2_r16_present                       = false;
  bool res_alloc_type1_granularity_dci_0_2_r16_present     = false;
  bool invalid_symbol_pattern_ind_dci_0_1_r16_present      = false;
  bool prio_ind_dci_0_1_r16_present                        = false;
  bool pusch_rep_type_ind_dci_0_1_r16_present              = false;
  bool freq_hop_dci_0_1_r16_present                        = false;
  bool ul_full_pwr_tx_r16_present                          = false;
  bool nof_invalid_symbols_for_dl_ul_switching_r16_present = false;
  copy_ptr<setup_release_c<dyn_seq_of<integer<uint8_t, 0, 16>, 1, 2>>>    minimum_sched_offset_k2_r16;
  copy_ptr<setup_release_c<dyn_seq_of<integer<uint8_t, 0, 63>, 1, 64>>>   ul_access_cfg_list_dci_0_1_r16;
  uint8_t                                                                 harq_process_num_size_dci_0_2_r16 = 0;
  uint8_t                                                                 nof_bits_for_rv_dci_0_2_r16       = 0;
  copy_ptr<setup_release_c<dmrs_ul_cfg_s>>                                dmrs_ul_for_pusch_map_type_a_dci_0_2_r16;
  copy_ptr<setup_release_c<dmrs_ul_cfg_s>>                                dmrs_ul_for_pusch_map_type_b_dci_0_2_r16;
  copy_ptr<freq_hop_dci_0_2_r16_c_>                                       freq_hop_dci_0_2_r16;
  copy_ptr<setup_release_c<dyn_seq_of<integer<uint16_t, 1, 274>, 1, 4>>>  freq_hop_offset_lists_dci_0_2_r16;
  codebook_subset_dci_0_2_r16_e_                                          codebook_subset_dci_0_2_r16;
  uint8_t                                                                 max_rank_dci_0_2_r16 = 1;
  mcs_table_dci_0_2_r16_e_                                                mcs_table_dci_0_2_r16;
  mcs_table_transform_precoder_dci_0_2_r16_e_                             mcs_table_transform_precoder_dci_0_2_r16;
  pusch_rep_type_ind_dci_0_2_r16_e_                                       pusch_rep_type_ind_dci_0_2_r16;
  res_alloc_dci_0_2_r16_e_                                                res_alloc_dci_0_2_r16;
  res_alloc_type1_granularity_dci_0_2_r16_e_                              res_alloc_type1_granularity_dci_0_2_r16;
  copy_ptr<setup_release_c<dyn_seq_of<uci_on_pusch_dci_0_2_r16_s, 1, 2>>> uci_on_pusch_list_dci_0_2_r16;
  copy_ptr<setup_release_c<dyn_seq_of<pusch_time_domain_res_alloc_r16_s, 1, 64>>>
      pusch_time_domain_alloc_list_dci_0_2_r16;
  copy_ptr<setup_release_c<dyn_seq_of<pusch_time_domain_res_alloc_r16_s, 1, 64>>>
                                                              pusch_time_domain_alloc_list_dci_0_1_r16;
  pusch_rep_type_ind_dci_0_1_r16_e_                           pusch_rep_type_ind_dci_0_1_r16;
  freq_hop_dci_0_1_r16_e_                                     freq_hop_dci_0_1_r16;
  copy_ptr<setup_release_c<dyn_seq_of<uci_on_pusch_s, 1, 2>>> uci_on_pusch_list_dci_0_1_r16;
  copy_ptr<invalid_symbol_pattern_r16_s>                      invalid_symbol_pattern_r16;
  copy_ptr<setup_release_c<pusch_pwr_ctrl_v1610_s>>           pusch_pwr_ctrl_v1610;
  ul_full_pwr_tx_r16_e_                                       ul_full_pwr_tx_r16;
  copy_ptr<setup_release_c<dyn_seq_of<pusch_time_domain_res_alloc_r16_s, 1, 64>>>
          pusch_time_domain_alloc_list_for_multi_pusch_r16;
  uint8_t nof_invalid_symbols_for_dl_ul_switching_r16 = 1;
  // group 1
  bool map_pattern_r17_present                     = false;
  bool second_tpc_field_dci_0_1_r17_present        = false;
  bool second_tpc_field_dci_0_2_r17_present        = false;
  bool seq_offset_for_rv_r17_present               = false;
  bool available_slot_count_r17_present            = false;
  bool harq_process_num_size_dci_0_2_v1700_present = false;
  bool harq_process_num_size_dci_0_1_r17_present   = false;
  copy_ptr<setup_release_c<dyn_seq_of<integer<uint8_t, 0, 63>, 1, 64>>> ul_access_cfg_list_dci_0_2_r17;
  copy_ptr<setup_release_c<beta_offsets_cross_pri_sel_r17_c>>           beta_offsets_cross_pri0_r17;
  copy_ptr<setup_release_c<beta_offsets_cross_pri_sel_r17_c>>           beta_offsets_cross_pri1_r17;
  copy_ptr<setup_release_c<beta_offsets_cross_pri_sel_dci_0_2_r17_c>>   beta_offsets_cross_pri0_dci_0_2_r17;
  copy_ptr<setup_release_c<beta_offsets_cross_pri_sel_dci_0_2_r17_c>>   beta_offsets_cross_pri1_dci_0_2_r17;
  map_pattern_r17_e_                                                    map_pattern_r17;
  uint8_t                                                               seq_offset_for_rv_r17 = 0;
  copy_ptr<setup_release_c<dyn_seq_of<integer<uint8_t, 0, 2>, 1, 3>>>   ul_access_cfg_list_dci_0_1_r17;
  copy_ptr<setup_release_c<dyn_seq_of<integer<uint8_t, 0, 64>, 1, 2>>>  minimum_sched_offset_k2_r17;
  copy_ptr<setup_release_c<dmrs_bundling_pusch_cfg_r17_s>>              dmrs_bundling_pusch_cfg_r17;
  uint8_t                                                               harq_process_num_size_dci_0_2_v1700 = 5;
  uint8_t                                                               harq_process_num_size_dci_0_1_r17   = 5;
  copy_ptr<mpe_res_pool_to_add_mod_list_r17_l_>                         mpe_res_pool_to_add_mod_list_r17;
  copy_ptr<mpe_res_pool_to_release_list_r17_l_>                         mpe_res_pool_to_release_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathlossReferenceRS-r17 ::= SEQUENCE
struct pathloss_ref_rs_r17_s {
  struct ref_sig_r17_c_ {
    struct types_opts {
      enum options { ssb_idx, csi_rs_idx, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ref_sig_r17_c_() = default;
    ref_sig_r17_c_(const ref_sig_r17_c_& other);
    ref_sig_r17_c_& operator=(const ref_sig_r17_c_& other);
    ~ref_sig_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx()
    {
      assert_choice_type(types::ssb_idx, type_, "referenceSignal-r17");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx()
    {
      assert_choice_type(types::csi_rs_idx, type_, "referenceSignal-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_idx() const
    {
      assert_choice_type(types::ssb_idx, type_, "referenceSignal-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx() const
    {
      assert_choice_type(types::csi_rs_idx, type_, "referenceSignal-r17");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_idx();
    uint8_t& set_csi_rs_idx();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool           add_pci_r17_present    = false;
  uint8_t        pathloss_ref_rs_id_r17 = 0;
  ref_sig_r17_c_ ref_sig_r17;
  uint8_t        add_pci_r17 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-Config ::= SEQUENCE
struct srs_cfg_s {
  using srs_res_set_to_release_list_l_             = bounded_array<uint8_t, 16>;
  using srs_res_set_to_add_mod_list_l_             = dyn_array<srs_res_set_s>;
  using srs_res_to_release_list_l_                 = dyn_array<uint8_t>;
  using srs_res_to_add_mod_list_l_                 = dyn_array<srs_res_s>;
  using srs_res_set_to_add_mod_list_dci_0_2_r16_l_ = dyn_array<srs_res_set_s>;
  using srs_res_set_to_release_list_dci_0_2_r16_l_ = bounded_array<uint8_t, 16>;
  using srs_pos_res_set_to_release_list_r16_l_     = bounded_array<uint8_t, 16>;
  using srs_pos_res_set_to_add_mod_list_r16_l_     = dyn_array<srs_pos_res_set_r16_s>;
  using srs_pos_res_to_release_list_r16_l_         = dyn_array<uint8_t>;
  using srs_pos_res_to_add_mod_list_r16_l_         = dyn_array<srs_pos_res_r16_s>;

  // member variables
  bool                           ext                      = false;
  bool                           tpc_accumulation_present = false;
  srs_res_set_to_release_list_l_ srs_res_set_to_release_list;
  srs_res_set_to_add_mod_list_l_ srs_res_set_to_add_mod_list;
  srs_res_to_release_list_l_     srs_res_to_release_list;
  srs_res_to_add_mod_list_l_     srs_res_to_add_mod_list;
  // ...
  // group 0
  bool                                                 srs_request_dci_1_2_r16_present = false;
  bool                                                 srs_request_dci_0_2_r16_present = false;
  uint8_t                                              srs_request_dci_1_2_r16         = 1;
  uint8_t                                              srs_request_dci_0_2_r16         = 1;
  copy_ptr<srs_res_set_to_add_mod_list_dci_0_2_r16_l_> srs_res_set_to_add_mod_list_dci_0_2_r16;
  copy_ptr<srs_res_set_to_release_list_dci_0_2_r16_l_> srs_res_set_to_release_list_dci_0_2_r16;
  copy_ptr<srs_pos_res_set_to_release_list_r16_l_>     srs_pos_res_set_to_release_list_r16;
  copy_ptr<srs_pos_res_set_to_add_mod_list_r16_l_>     srs_pos_res_set_to_add_mod_list_r16;
  copy_ptr<srs_pos_res_to_release_list_r16_l_>         srs_pos_res_to_release_list_r16;
  copy_ptr<srs_pos_res_to_add_mod_list_r16_l_>         srs_pos_res_to_add_mod_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TCI-UL-State-r17 ::= SEQUENCE
struct tci_ul_state_r17_s {
  struct ref_sig_r17_c_ {
    struct types_opts {
      enum options { ssb_idx_r17, csi_rs_idx_r17, srs_r17, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ref_sig_r17_c_() = default;
    ref_sig_r17_c_(const ref_sig_r17_c_& other);
    ref_sig_r17_c_& operator=(const ref_sig_r17_c_& other);
    ~ref_sig_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx_r17()
    {
      assert_choice_type(types::ssb_idx_r17, type_, "referenceSignal-r17");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx_r17()
    {
      assert_choice_type(types::csi_rs_idx_r17, type_, "referenceSignal-r17");
      return c.get<uint8_t>();
    }
    uint8_t& srs_r17()
    {
      assert_choice_type(types::srs_r17, type_, "referenceSignal-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_idx_r17() const
    {
      assert_choice_type(types::ssb_idx_r17, type_, "referenceSignal-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx_r17() const
    {
      assert_choice_type(types::csi_rs_idx_r17, type_, "referenceSignal-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& srs_r17() const
    {
      assert_choice_type(types::srs_r17, type_, "referenceSignal-r17");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_idx_r17();
    uint8_t& set_csi_rs_idx_r17();
    uint8_t& set_srs_r17();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool           ext                            = false;
  bool           serving_cell_id_r17_present    = false;
  bool           bwp_id_r17_present             = false;
  bool           add_pci_r17_present            = false;
  bool           ul_pwr_ctrl_r17_present        = false;
  bool           pathloss_ref_rs_id_r17_present = false;
  uint8_t        tci_ul_state_id_r17            = 0;
  uint8_t        serving_cell_id_r17            = 0;
  uint8_t        bwp_id_r17                     = 0;
  ref_sig_r17_c_ ref_sig_r17;
  uint8_t        add_pci_r17            = 1;
  uint8_t        ul_pwr_ctrl_r17        = 1;
  uint8_t        pathloss_ref_rs_id_r17 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP-UplinkDedicated ::= SEQUENCE
struct bwp_ul_ded_s {
  struct ul_tci_state_list_r17_c_ {
    struct explicitlist_s_ {
      using ul_tci_to_add_mod_list_r17_l_ = dyn_array<tci_ul_state_r17_s>;
      using ul_tci_to_release_list_r17_l_ = dyn_array<uint8_t>;

      // member variables
      ul_tci_to_add_mod_list_r17_l_ ul_tci_to_add_mod_list_r17;
      ul_tci_to_release_list_r17_l_ ul_tci_to_release_list_r17;
    };
    struct types_opts {
      enum options { explicitlist, unified_tci_state_ref_r17, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ul_tci_state_list_r17_c_() = default;
    ul_tci_state_list_r17_c_(const ul_tci_state_list_r17_c_& other);
    ul_tci_state_list_r17_c_& operator=(const ul_tci_state_list_r17_c_& other);
    ~ul_tci_state_list_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    explicitlist_s_& explicitlist()
    {
      assert_choice_type(types::explicitlist, type_, "ul-TCI-StateList-r17");
      return c.get<explicitlist_s_>();
    }
    serving_cell_and_bwp_id_r17_s& unified_tci_state_ref_r17()
    {
      assert_choice_type(types::unified_tci_state_ref_r17, type_, "ul-TCI-StateList-r17");
      return c.get<serving_cell_and_bwp_id_r17_s>();
    }
    const explicitlist_s_& explicitlist() const
    {
      assert_choice_type(types::explicitlist, type_, "ul-TCI-StateList-r17");
      return c.get<explicitlist_s_>();
    }
    const serving_cell_and_bwp_id_r17_s& unified_tci_state_ref_r17() const
    {
      assert_choice_type(types::unified_tci_state_ref_r17, type_, "ul-TCI-StateList-r17");
      return c.get<serving_cell_and_bwp_id_r17_s>();
    }
    explicitlist_s_&               set_explicitlist();
    serving_cell_and_bwp_id_r17_s& set_unified_tci_state_ref_r17();

  private:
    types                                                           type_;
    choice_buffer_t<explicitlist_s_, serving_cell_and_bwp_id_r17_s> c;

    void destroy_();
  };
  using pathloss_ref_rs_to_add_mod_list_r17_l_ = dyn_array<pathloss_ref_rs_r17_s>;
  using pathloss_ref_rs_to_release_list_r17_l_ = dyn_array<uint8_t>;

  // member variables
  bool                                      ext                            = false;
  bool                                      pucch_cfg_present              = false;
  bool                                      pusch_cfg_present              = false;
  bool                                      cfg_grant_cfg_present          = false;
  bool                                      srs_cfg_present                = false;
  bool                                      beam_fail_recovery_cfg_present = false;
  setup_release_c<pucch_cfg_s>              pucch_cfg;
  setup_release_c<pusch_cfg_s>              pusch_cfg;
  setup_release_c<cfg_grant_cfg_s>          cfg_grant_cfg;
  setup_release_c<srs_cfg_s>                srs_cfg;
  setup_release_c<beam_fail_recovery_cfg_s> beam_fail_recovery_cfg;
  // ...
  // group 0
  bool                                                        cp_ext_c2_r16_present                 = false;
  bool                                                        cp_ext_c3_r16_present                 = false;
  bool                                                        use_interlace_pucch_pusch_r16_present = false;
  copy_ptr<setup_release_c<pucch_cfg_s>>                      sl_pucch_cfg_r16;
  uint8_t                                                     cp_ext_c2_r16 = 1;
  uint8_t                                                     cp_ext_c3_r16 = 1;
  copy_ptr<setup_release_c<dyn_seq_of<pucch_cfg_s, 1, 2>>>    pucch_cfg_list_r16;
  copy_ptr<setup_release_c<lbt_fail_recovery_cfg_r16_s>>      lbt_fail_recovery_cfg_r16;
  copy_ptr<cfg_grant_cfg_to_add_mod_list_r16_l>               cfg_grant_cfg_to_add_mod_list_r16;
  copy_ptr<cfg_grant_cfg_to_release_list_r16_l>               cfg_grant_cfg_to_release_list_r16;
  copy_ptr<cfg_grant_cfg_type2_deactivation_state_list_r16_l> cfg_grant_cfg_type2_deactivation_state_list_r16;
  // group 1
  bool                                                     ul_pwr_ctrl_r17_present = false;
  copy_ptr<ul_tci_state_list_r17_c_>                       ul_tci_state_list_r17;
  uint8_t                                                  ul_pwr_ctrl_r17 = 1;
  copy_ptr<setup_release_c<dyn_seq_of<pucch_cfg_s, 1, 2>>> pucch_cfg_list_multicast1_r17;
  copy_ptr<setup_release_c<dyn_seq_of<pucch_cfg_s, 1, 2>>> pucch_cfg_list_multicast2_r17;
  // group 2
  copy_ptr<setup_release_c<pucch_cfg_s>> pucch_cfg_multicast1_r17;
  copy_ptr<setup_release_c<pucch_cfg_s>> pucch_cfg_multicast2_r17;
  // group 3
  copy_ptr<pathloss_ref_rs_to_add_mod_list_r17_l_> pathloss_ref_rs_to_add_mod_list_r17;
  copy_ptr<pathloss_ref_rs_to_release_list_r17_l_> pathloss_ref_rs_to_release_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP-Uplink ::= SEQUENCE
struct bwp_ul_s {
  bool            ext                = false;
  bool            bwp_common_present = false;
  bool            bwp_ded_present    = false;
  uint8_t         bwp_id             = 0;
  bwp_ul_common_s bwp_common;
  bwp_ul_ded_s    bwp_ded;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NZP-CSI-RS-Pairing-r17 ::= SEQUENCE
struct nzp_csi_rs_pairing_r17_s {
  uint8_t nzp_csi_rs_res_id1_r17 = 1;
  uint8_t nzp_csi_rs_res_id2_r17 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CMRGroupingAndPairing-r17 ::= SEQUENCE
struct cmr_grouping_and_pairing_r17_s {
  bool                     pair1_of_nzp_csi_rs_r17_present = false;
  bool                     pair2_of_nzp_csi_rs_r17_present = false;
  uint8_t                  nrof_res_group1_r17             = 1;
  nzp_csi_rs_pairing_r17_s pair1_of_nzp_csi_rs_r17;
  nzp_csi_rs_pairing_r17_s pair2_of_nzp_csi_rs_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CO-DurationsPerCell-r16 ::= SEQUENCE
struct co_durs_per_cell_r16_s {
  using co_dur_list_r16_l_ = dyn_array<uint16_t>;

  // member variables
  uint8_t              serving_cell_id_r16 = 0;
  uint8_t              position_in_dci_r16 = 0;
  subcarrier_spacing_e subcarrier_spacing_r16;
  co_dur_list_r16_l_   co_dur_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CO-DurationsPerCell-r17 ::= SEQUENCE
struct co_durs_per_cell_r17_s {
  using co_dur_list_r17_l_ = dyn_array<uint16_t>;

  // member variables
  uint8_t              serving_cell_id_r17 = 0;
  uint8_t              position_in_dci_r17 = 0;
  subcarrier_spacing_e subcarrier_spacing_r17;
  co_dur_list_r17_l_   co_dur_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-AssociatedReportConfigInfo ::= SEQUENCE
struct csi_associated_report_cfg_info_s {
  struct res_for_ch_c_ {
    struct nzp_csi_rs_s_ {
      using qcl_info_l_ = bounded_array<uint8_t, 16>;

      // member variables
      uint8_t     res_set = 1;
      qcl_info_l_ qcl_info;
    };
    struct types_opts {
      enum options { nzp_csi_rs, csi_ssb_res_set, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    res_for_ch_c_() = default;
    res_for_ch_c_(const res_for_ch_c_& other);
    res_for_ch_c_& operator=(const res_for_ch_c_& other);
    ~res_for_ch_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    nzp_csi_rs_s_& nzp_csi_rs()
    {
      assert_choice_type(types::nzp_csi_rs, type_, "resourcesForChannel");
      return c.get<nzp_csi_rs_s_>();
    }
    uint8_t& csi_ssb_res_set()
    {
      assert_choice_type(types::csi_ssb_res_set, type_, "resourcesForChannel");
      return c.get<uint8_t>();
    }
    const nzp_csi_rs_s_& nzp_csi_rs() const
    {
      assert_choice_type(types::nzp_csi_rs, type_, "resourcesForChannel");
      return c.get<nzp_csi_rs_s_>();
    }
    const uint8_t& csi_ssb_res_set() const
    {
      assert_choice_type(types::csi_ssb_res_set, type_, "resourcesForChannel");
      return c.get<uint8_t>();
    }
    nzp_csi_rs_s_& set_nzp_csi_rs();
    uint8_t&       set_csi_ssb_res_set();

  private:
    types                          type_;
    choice_buffer_t<nzp_csi_rs_s_> c;

    void destroy_();
  };
  struct res_for_ch2_r17_c_ {
    struct nzp_csi_rs2_r17_s_ {
      using qcl_info2_r17_l_ = bounded_array<uint8_t, 16>;

      // member variables
      uint8_t          res_set2_r17 = 1;
      qcl_info2_r17_l_ qcl_info2_r17;
    };
    struct types_opts {
      enum options { nzp_csi_rs2_r17, csi_ssb_res_set2_r17, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    res_for_ch2_r17_c_() = default;
    res_for_ch2_r17_c_(const res_for_ch2_r17_c_& other);
    res_for_ch2_r17_c_& operator=(const res_for_ch2_r17_c_& other);
    ~res_for_ch2_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    nzp_csi_rs2_r17_s_& nzp_csi_rs2_r17()
    {
      assert_choice_type(types::nzp_csi_rs2_r17, type_, "resourcesForChannel2-r17");
      return c.get<nzp_csi_rs2_r17_s_>();
    }
    uint8_t& csi_ssb_res_set2_r17()
    {
      assert_choice_type(types::csi_ssb_res_set2_r17, type_, "resourcesForChannel2-r17");
      return c.get<uint8_t>();
    }
    const nzp_csi_rs2_r17_s_& nzp_csi_rs2_r17() const
    {
      assert_choice_type(types::nzp_csi_rs2_r17, type_, "resourcesForChannel2-r17");
      return c.get<nzp_csi_rs2_r17_s_>();
    }
    const uint8_t& csi_ssb_res_set2_r17() const
    {
      assert_choice_type(types::csi_ssb_res_set2_r17, type_, "resourcesForChannel2-r17");
      return c.get<uint8_t>();
    }
    nzp_csi_rs2_r17_s_& set_nzp_csi_rs2_r17();
    uint8_t&            set_csi_ssb_res_set2_r17();

  private:
    types                               type_;
    choice_buffer_t<nzp_csi_rs2_r17_s_> c;

    void destroy_();
  };

  // member variables
  bool          ext                                     = false;
  bool          csi_im_res_for_interference_present     = false;
  bool          nzp_csi_rs_res_for_interference_present = false;
  uint8_t       report_cfg_id                           = 0;
  res_for_ch_c_ res_for_ch;
  uint8_t       csi_im_res_for_interference     = 1;
  uint8_t       nzp_csi_rs_res_for_interference = 1;
  // ...
  // group 0
  bool                         csi_ssb_res_set_ext_present = false;
  copy_ptr<res_for_ch2_r17_c_> res_for_ch2_r17;
  uint8_t                      csi_ssb_res_set_ext = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-AperiodicTriggerState ::= SEQUENCE
struct csi_aperiodic_trigger_state_s {
  using associated_report_cfg_info_list_l_ = dyn_array<csi_associated_report_cfg_info_s>;

  // member variables
  bool                               ext = false;
  associated_report_cfg_info_list_l_ associated_report_cfg_info_list;
  // ...
  // group 0
  bool ap_csi_mux_mode_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-AperiodicTriggerStateList ::= SEQUENCE (SIZE (1..128)) OF CSI-AperiodicTriggerState
using csi_aperiodic_trigger_state_list_l = dyn_array<csi_aperiodic_trigger_state_s>;

// CSI-IM-Resource ::= SEQUENCE
struct csi_im_res_s {
  struct csi_im_res_elem_pattern_c_ {
    struct pattern0_s_ {
      struct subcarrier_location_p0_opts {
        enum options { s0, s2, s4, s6, s8, s10, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using subcarrier_location_p0_e_ = enumerated<subcarrier_location_p0_opts>;

      // member variables
      subcarrier_location_p0_e_ subcarrier_location_p0;
      uint8_t                   symbol_location_p0 = 0;
    };
    struct pattern1_s_ {
      struct subcarrier_location_p1_opts {
        enum options { s0, s4, s8, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using subcarrier_location_p1_e_ = enumerated<subcarrier_location_p1_opts>;

      // member variables
      subcarrier_location_p1_e_ subcarrier_location_p1;
      uint8_t                   symbol_location_p1 = 0;
    };
    struct types_opts {
      enum options { pattern0, pattern1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    csi_im_res_elem_pattern_c_() = default;
    csi_im_res_elem_pattern_c_(const csi_im_res_elem_pattern_c_& other);
    csi_im_res_elem_pattern_c_& operator=(const csi_im_res_elem_pattern_c_& other);
    ~csi_im_res_elem_pattern_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pattern0_s_& pattern0()
    {
      assert_choice_type(types::pattern0, type_, "csi-IM-ResourceElementPattern");
      return c.get<pattern0_s_>();
    }
    pattern1_s_& pattern1()
    {
      assert_choice_type(types::pattern1, type_, "csi-IM-ResourceElementPattern");
      return c.get<pattern1_s_>();
    }
    const pattern0_s_& pattern0() const
    {
      assert_choice_type(types::pattern0, type_, "csi-IM-ResourceElementPattern");
      return c.get<pattern0_s_>();
    }
    const pattern1_s_& pattern1() const
    {
      assert_choice_type(types::pattern1, type_, "csi-IM-ResourceElementPattern");
      return c.get<pattern1_s_>();
    }
    pattern0_s_& set_pattern0();
    pattern1_s_& set_pattern1();

  private:
    types                                     type_;
    choice_buffer_t<pattern0_s_, pattern1_s_> c;

    void destroy_();
  };

  // member variables
  bool                             ext                             = false;
  bool                             csi_im_res_elem_pattern_present = false;
  bool                             freq_band_present               = false;
  bool                             periodicity_and_offset_present  = false;
  uint8_t                          csi_im_res_id                   = 0;
  csi_im_res_elem_pattern_c_       csi_im_res_elem_pattern;
  csi_freq_occupation_s            freq_band;
  csi_res_periodicity_and_offset_c periodicity_and_offset;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-IM-ResourceSet ::= SEQUENCE
struct csi_im_res_set_s {
  using csi_im_res_l_ = bounded_array<uint8_t, 8>;

  // member variables
  bool          ext               = false;
  uint8_t       csi_im_res_set_id = 0;
  csi_im_res_l_ csi_im_res;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-ReportPeriodicityAndOffset ::= CHOICE
struct csi_report_periodicity_and_offset_c {
  struct types_opts {
    enum options {
      slots4,
      slots5,
      slots8,
      slots10,
      slots16,
      slots20,
      slots40,
      slots80,
      slots160,
      slots320,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  csi_report_periodicity_and_offset_c() = default;
  csi_report_periodicity_and_offset_c(const csi_report_periodicity_and_offset_c& other);
  csi_report_periodicity_and_offset_c& operator=(const csi_report_periodicity_and_offset_c& other);
  ~csi_report_periodicity_and_offset_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& slots4()
  {
    assert_choice_type(types::slots4, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots5()
  {
    assert_choice_type(types::slots5, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots8()
  {
    assert_choice_type(types::slots8, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots10()
  {
    assert_choice_type(types::slots10, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots16()
  {
    assert_choice_type(types::slots16, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots20()
  {
    assert_choice_type(types::slots20, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots40()
  {
    assert_choice_type(types::slots40, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots80()
  {
    assert_choice_type(types::slots80, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots160()
  {
    assert_choice_type(types::slots160, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint16_t& slots320()
  {
    assert_choice_type(types::slots320, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  const uint8_t& slots4() const
  {
    assert_choice_type(types::slots4, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots5() const
  {
    assert_choice_type(types::slots5, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots8() const
  {
    assert_choice_type(types::slots8, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots10() const
  {
    assert_choice_type(types::slots10, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots16() const
  {
    assert_choice_type(types::slots16, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots20() const
  {
    assert_choice_type(types::slots20, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots40() const
  {
    assert_choice_type(types::slots40, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots80() const
  {
    assert_choice_type(types::slots80, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots160() const
  {
    assert_choice_type(types::slots160, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint16_t& slots320() const
  {
    assert_choice_type(types::slots320, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  uint8_t&  set_slots4();
  uint8_t&  set_slots5();
  uint8_t&  set_slots8();
  uint8_t&  set_slots10();
  uint8_t&  set_slots16();
  uint8_t&  set_slots20();
  uint8_t&  set_slots40();
  uint8_t&  set_slots80();
  uint8_t&  set_slots160();
  uint16_t& set_slots320();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// CSI-SemiPersistentOnPUSCH-TriggerState ::= SEQUENCE
struct csi_semi_persistent_on_pusch_trigger_state_s {
  bool    ext                        = false;
  uint8_t associated_report_cfg_info = 0;
  // ...
  // group 0
  bool sp_csi_mux_mode_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookConfig ::= SEQUENCE
struct codebook_cfg_s {
  struct codebook_type_c_ {
    struct type1_s_ {
      struct sub_type_c_ {
        struct type_i_single_panel_s_ {
          struct nr_of_ant_ports_c_ {
            struct two_s_ {
              fixed_bitstring<6> two_tx_codebook_subset_restrict;
            };
            struct more_than_two_s_ {
              struct n1_n2_c_ {
                struct types_opts {
                  enum options {
                    two_one_type_i_single_panel_restrict,
                    two_two_type_i_single_panel_restrict,
                    four_one_type_i_single_panel_restrict,
                    three_two_type_i_single_panel_restrict,
                    six_one_type_i_single_panel_restrict,
                    four_two_type_i_single_panel_restrict,
                    eight_one_type_i_single_panel_restrict,
                    four_three_type_i_single_panel_restrict,
                    six_two_type_i_single_panel_restrict,
                    twelve_one_type_i_single_panel_restrict,
                    four_four_type_i_single_panel_restrict,
                    eight_two_type_i_single_panel_restrict,
                    sixteen_one_type_i_single_panel_restrict,
                    nulltype
                  } value;

                  const char* to_string() const;
                };
                using types = enumerated<types_opts>;

                // choice methods
                n1_n2_c_() = default;
                n1_n2_c_(const n1_n2_c_& other);
                n1_n2_c_& operator=(const n1_n2_c_& other);
                ~n1_n2_c_() { destroy_(); }
                void        set(types::options e = types::nulltype);
                types       type() const { return type_; }
                SRSASN_CODE pack(bit_ref& bref) const;
                SRSASN_CODE unpack(cbit_ref& bref);
                void        to_json(json_writer& j) const;
                // getters
                fixed_bitstring<8>& two_one_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::two_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<8>>();
                }
                fixed_bitstring<64>& two_two_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::two_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<64>>();
                }
                fixed_bitstring<16>& four_one_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::four_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<16>>();
                }
                fixed_bitstring<96>& three_two_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::three_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<96>>();
                }
                fixed_bitstring<24>& six_one_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::six_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<24>>();
                }
                fixed_bitstring<128>& four_two_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::four_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<128>>();
                }
                fixed_bitstring<32>& eight_one_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::eight_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<32>>();
                }
                fixed_bitstring<192>& four_three_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::four_three_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<192>>();
                }
                fixed_bitstring<192>& six_two_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::six_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<192>>();
                }
                fixed_bitstring<48>& twelve_one_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::twelve_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<48>>();
                }
                fixed_bitstring<256>& four_four_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::four_four_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<256>>();
                }
                fixed_bitstring<256>& eight_two_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::eight_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<256>>();
                }
                fixed_bitstring<64>& sixteen_one_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::sixteen_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<64>>();
                }
                const fixed_bitstring<8>& two_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::two_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<8>>();
                }
                const fixed_bitstring<64>& two_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::two_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<64>>();
                }
                const fixed_bitstring<16>& four_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::four_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<16>>();
                }
                const fixed_bitstring<96>& three_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::three_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<96>>();
                }
                const fixed_bitstring<24>& six_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::six_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<24>>();
                }
                const fixed_bitstring<128>& four_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::four_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<128>>();
                }
                const fixed_bitstring<32>& eight_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::eight_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<32>>();
                }
                const fixed_bitstring<192>& four_three_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::four_three_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<192>>();
                }
                const fixed_bitstring<192>& six_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::six_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<192>>();
                }
                const fixed_bitstring<48>& twelve_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::twelve_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<48>>();
                }
                const fixed_bitstring<256>& four_four_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::four_four_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<256>>();
                }
                const fixed_bitstring<256>& eight_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::eight_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<256>>();
                }
                const fixed_bitstring<64>& sixteen_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::sixteen_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<64>>();
                }
                fixed_bitstring<8>&   set_two_one_type_i_single_panel_restrict();
                fixed_bitstring<64>&  set_two_two_type_i_single_panel_restrict();
                fixed_bitstring<16>&  set_four_one_type_i_single_panel_restrict();
                fixed_bitstring<96>&  set_three_two_type_i_single_panel_restrict();
                fixed_bitstring<24>&  set_six_one_type_i_single_panel_restrict();
                fixed_bitstring<128>& set_four_two_type_i_single_panel_restrict();
                fixed_bitstring<32>&  set_eight_one_type_i_single_panel_restrict();
                fixed_bitstring<192>& set_four_three_type_i_single_panel_restrict();
                fixed_bitstring<192>& set_six_two_type_i_single_panel_restrict();
                fixed_bitstring<48>&  set_twelve_one_type_i_single_panel_restrict();
                fixed_bitstring<256>& set_four_four_type_i_single_panel_restrict();
                fixed_bitstring<256>& set_eight_two_type_i_single_panel_restrict();
                fixed_bitstring<64>&  set_sixteen_one_type_i_single_panel_restrict();

              private:
                types                                 type_;
                choice_buffer_t<fixed_bitstring<256>> c;

                void destroy_();
              };

              // member variables
              bool                type_i_single_panel_codebook_subset_restrict_i2_present = false;
              n1_n2_c_            n1_n2;
              fixed_bitstring<16> type_i_single_panel_codebook_subset_restrict_i2;
            };
            struct types_opts {
              enum options { two, more_than_two, nulltype } value;
              typedef uint8_t number_type;

              const char* to_string() const;
              uint8_t     to_number() const;
            };
            using types = enumerated<types_opts>;

            // choice methods
            nr_of_ant_ports_c_() = default;
            nr_of_ant_ports_c_(const nr_of_ant_ports_c_& other);
            nr_of_ant_ports_c_& operator=(const nr_of_ant_ports_c_& other);
            ~nr_of_ant_ports_c_() { destroy_(); }
            void        set(types::options e = types::nulltype);
            types       type() const { return type_; }
            SRSASN_CODE pack(bit_ref& bref) const;
            SRSASN_CODE unpack(cbit_ref& bref);
            void        to_json(json_writer& j) const;
            // getters
            two_s_& two()
            {
              assert_choice_type(types::two, type_, "nrOfAntennaPorts");
              return c.get<two_s_>();
            }
            more_than_two_s_& more_than_two()
            {
              assert_choice_type(types::more_than_two, type_, "nrOfAntennaPorts");
              return c.get<more_than_two_s_>();
            }
            const two_s_& two() const
            {
              assert_choice_type(types::two, type_, "nrOfAntennaPorts");
              return c.get<two_s_>();
            }
            const more_than_two_s_& more_than_two() const
            {
              assert_choice_type(types::more_than_two, type_, "nrOfAntennaPorts");
              return c.get<more_than_two_s_>();
            }
            two_s_&           set_two();
            more_than_two_s_& set_more_than_two();

          private:
            types                                     type_;
            choice_buffer_t<more_than_two_s_, two_s_> c;

            void destroy_();
          };

          // member variables
          nr_of_ant_ports_c_ nr_of_ant_ports;
          fixed_bitstring<8> type_i_single_panel_ri_restrict;
        };
        struct type_i_multi_panel_s_ {
          struct ng_n1_n2_c_ {
            struct types_opts {
              enum options {
                two_two_one_type_i_multi_panel_restrict,
                two_four_one_type_i_multi_panel_restrict,
                four_two_one_type_i_multi_panel_restrict,
                two_two_two_type_i_multi_panel_restrict,
                two_eight_one_type_i_multi_panel_restrict,
                four_four_one_type_i_multi_panel_restrict,
                two_four_two_type_i_multi_panel_restrict,
                four_two_two_type_i_multi_panel_restrict,
                nulltype
              } value;

              const char* to_string() const;
            };
            using types = enumerated<types_opts>;

            // choice methods
            ng_n1_n2_c_() = default;
            ng_n1_n2_c_(const ng_n1_n2_c_& other);
            ng_n1_n2_c_& operator=(const ng_n1_n2_c_& other);
            ~ng_n1_n2_c_() { destroy_(); }
            void        set(types::options e = types::nulltype);
            types       type() const { return type_; }
            SRSASN_CODE pack(bit_ref& bref) const;
            SRSASN_CODE unpack(cbit_ref& bref);
            void        to_json(json_writer& j) const;
            // getters
            fixed_bitstring<8>& two_two_one_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::two_two_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<8>>();
            }
            fixed_bitstring<16>& two_four_one_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::two_four_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<16>>();
            }
            fixed_bitstring<8>& four_two_one_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::four_two_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<8>>();
            }
            fixed_bitstring<64>& two_two_two_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::two_two_two_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<64>>();
            }
            fixed_bitstring<32>& two_eight_one_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::two_eight_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<32>>();
            }
            fixed_bitstring<16>& four_four_one_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::four_four_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<16>>();
            }
            fixed_bitstring<128>& two_four_two_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::two_four_two_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<128>>();
            }
            fixed_bitstring<64>& four_two_two_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::four_two_two_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<64>>();
            }
            const fixed_bitstring<8>& two_two_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::two_two_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<8>>();
            }
            const fixed_bitstring<16>& two_four_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::two_four_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<16>>();
            }
            const fixed_bitstring<8>& four_two_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::four_two_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<8>>();
            }
            const fixed_bitstring<64>& two_two_two_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::two_two_two_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<64>>();
            }
            const fixed_bitstring<32>& two_eight_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::two_eight_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<32>>();
            }
            const fixed_bitstring<16>& four_four_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::four_four_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<16>>();
            }
            const fixed_bitstring<128>& two_four_two_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::two_four_two_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<128>>();
            }
            const fixed_bitstring<64>& four_two_two_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::four_two_two_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<64>>();
            }
            fixed_bitstring<8>&   set_two_two_one_type_i_multi_panel_restrict();
            fixed_bitstring<16>&  set_two_four_one_type_i_multi_panel_restrict();
            fixed_bitstring<8>&   set_four_two_one_type_i_multi_panel_restrict();
            fixed_bitstring<64>&  set_two_two_two_type_i_multi_panel_restrict();
            fixed_bitstring<32>&  set_two_eight_one_type_i_multi_panel_restrict();
            fixed_bitstring<16>&  set_four_four_one_type_i_multi_panel_restrict();
            fixed_bitstring<128>& set_two_four_two_type_i_multi_panel_restrict();
            fixed_bitstring<64>&  set_four_two_two_type_i_multi_panel_restrict();

          private:
            types                                 type_;
            choice_buffer_t<fixed_bitstring<128>> c;

            void destroy_();
          };

          // member variables
          ng_n1_n2_c_        ng_n1_n2;
          fixed_bitstring<4> ri_restrict;
        };
        struct types_opts {
          enum options { type_i_single_panel, type_i_multi_panel, nulltype } value;

          const char* to_string() const;
        };
        using types = enumerated<types_opts>;

        // choice methods
        sub_type_c_() = default;
        sub_type_c_(const sub_type_c_& other);
        sub_type_c_& operator=(const sub_type_c_& other);
        ~sub_type_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        type_i_single_panel_s_& type_i_single_panel()
        {
          assert_choice_type(types::type_i_single_panel, type_, "subType");
          return c.get<type_i_single_panel_s_>();
        }
        type_i_multi_panel_s_& type_i_multi_panel()
        {
          assert_choice_type(types::type_i_multi_panel, type_, "subType");
          return c.get<type_i_multi_panel_s_>();
        }
        const type_i_single_panel_s_& type_i_single_panel() const
        {
          assert_choice_type(types::type_i_single_panel, type_, "subType");
          return c.get<type_i_single_panel_s_>();
        }
        const type_i_multi_panel_s_& type_i_multi_panel() const
        {
          assert_choice_type(types::type_i_multi_panel, type_, "subType");
          return c.get<type_i_multi_panel_s_>();
        }
        type_i_single_panel_s_& set_type_i_single_panel();
        type_i_multi_panel_s_&  set_type_i_multi_panel();

      private:
        types                                                          type_;
        choice_buffer_t<type_i_multi_panel_s_, type_i_single_panel_s_> c;

        void destroy_();
      };

      // member variables
      sub_type_c_ sub_type;
      uint8_t     codebook_mode = 1;
    };
    struct type2_s_ {
      struct sub_type_c_ {
        struct type_ii_s_ {
          struct n1_n2_codebook_subset_restrict_c_ {
            struct types_opts {
              enum options {
                two_one,
                two_two,
                four_one,
                three_two,
                six_one,
                four_two,
                eight_one,
                four_three,
                six_two,
                twelve_one,
                four_four,
                eight_two,
                sixteen_one,
                nulltype
              } value;

              const char* to_string() const;
            };
            using types = enumerated<types_opts>;

            // choice methods
            n1_n2_codebook_subset_restrict_c_() = default;
            n1_n2_codebook_subset_restrict_c_(const n1_n2_codebook_subset_restrict_c_& other);
            n1_n2_codebook_subset_restrict_c_& operator=(const n1_n2_codebook_subset_restrict_c_& other);
            ~n1_n2_codebook_subset_restrict_c_() { destroy_(); }
            void        set(types::options e = types::nulltype);
            types       type() const { return type_; }
            SRSASN_CODE pack(bit_ref& bref) const;
            SRSASN_CODE unpack(cbit_ref& bref);
            void        to_json(json_writer& j) const;
            // getters
            fixed_bitstring<16>& two_one()
            {
              assert_choice_type(types::two_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<16>>();
            }
            fixed_bitstring<43>& two_two()
            {
              assert_choice_type(types::two_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<43>>();
            }
            fixed_bitstring<32>& four_one()
            {
              assert_choice_type(types::four_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<32>>();
            }
            fixed_bitstring<59>& three_two()
            {
              assert_choice_type(types::three_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<59>>();
            }
            fixed_bitstring<48>& six_one()
            {
              assert_choice_type(types::six_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<48>>();
            }
            fixed_bitstring<75>& four_two()
            {
              assert_choice_type(types::four_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<75>>();
            }
            fixed_bitstring<64>& eight_one()
            {
              assert_choice_type(types::eight_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<64>>();
            }
            fixed_bitstring<107>& four_three()
            {
              assert_choice_type(types::four_three, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<107>>();
            }
            fixed_bitstring<107>& six_two()
            {
              assert_choice_type(types::six_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<107>>();
            }
            fixed_bitstring<96>& twelve_one()
            {
              assert_choice_type(types::twelve_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<96>>();
            }
            fixed_bitstring<139>& four_four()
            {
              assert_choice_type(types::four_four, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<139>>();
            }
            fixed_bitstring<139>& eight_two()
            {
              assert_choice_type(types::eight_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<139>>();
            }
            fixed_bitstring<128>& sixteen_one()
            {
              assert_choice_type(types::sixteen_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<128>>();
            }
            const fixed_bitstring<16>& two_one() const
            {
              assert_choice_type(types::two_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<16>>();
            }
            const fixed_bitstring<43>& two_two() const
            {
              assert_choice_type(types::two_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<43>>();
            }
            const fixed_bitstring<32>& four_one() const
            {
              assert_choice_type(types::four_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<32>>();
            }
            const fixed_bitstring<59>& three_two() const
            {
              assert_choice_type(types::three_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<59>>();
            }
            const fixed_bitstring<48>& six_one() const
            {
              assert_choice_type(types::six_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<48>>();
            }
            const fixed_bitstring<75>& four_two() const
            {
              assert_choice_type(types::four_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<75>>();
            }
            const fixed_bitstring<64>& eight_one() const
            {
              assert_choice_type(types::eight_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<64>>();
            }
            const fixed_bitstring<107>& four_three() const
            {
              assert_choice_type(types::four_three, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<107>>();
            }
            const fixed_bitstring<107>& six_two() const
            {
              assert_choice_type(types::six_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<107>>();
            }
            const fixed_bitstring<96>& twelve_one() const
            {
              assert_choice_type(types::twelve_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<96>>();
            }
            const fixed_bitstring<139>& four_four() const
            {
              assert_choice_type(types::four_four, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<139>>();
            }
            const fixed_bitstring<139>& eight_two() const
            {
              assert_choice_type(types::eight_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<139>>();
            }
            const fixed_bitstring<128>& sixteen_one() const
            {
              assert_choice_type(types::sixteen_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<128>>();
            }
            fixed_bitstring<16>&  set_two_one();
            fixed_bitstring<43>&  set_two_two();
            fixed_bitstring<32>&  set_four_one();
            fixed_bitstring<59>&  set_three_two();
            fixed_bitstring<48>&  set_six_one();
            fixed_bitstring<75>&  set_four_two();
            fixed_bitstring<64>&  set_eight_one();
            fixed_bitstring<107>& set_four_three();
            fixed_bitstring<107>& set_six_two();
            fixed_bitstring<96>&  set_twelve_one();
            fixed_bitstring<139>& set_four_four();
            fixed_bitstring<139>& set_eight_two();
            fixed_bitstring<128>& set_sixteen_one();

          private:
            types                                 type_;
            choice_buffer_t<fixed_bitstring<139>> c;

            void destroy_();
          };

          // member variables
          n1_n2_codebook_subset_restrict_c_ n1_n2_codebook_subset_restrict;
          fixed_bitstring<2>                type_ii_ri_restrict;
        };
        struct type_ii_port_sel_s_ {
          struct port_sel_sampling_size_opts {
            enum options { n1, n2, n3, n4, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using port_sel_sampling_size_e_ = enumerated<port_sel_sampling_size_opts>;

          // member variables
          bool                      port_sel_sampling_size_present = false;
          port_sel_sampling_size_e_ port_sel_sampling_size;
          fixed_bitstring<2>        type_ii_port_sel_ri_restrict;
        };
        struct types_opts {
          enum options { type_ii, type_ii_port_sel, nulltype } value;

          const char* to_string() const;
        };
        using types = enumerated<types_opts>;

        // choice methods
        sub_type_c_() = default;
        sub_type_c_(const sub_type_c_& other);
        sub_type_c_& operator=(const sub_type_c_& other);
        ~sub_type_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        type_ii_s_& type_ii()
        {
          assert_choice_type(types::type_ii, type_, "subType");
          return c.get<type_ii_s_>();
        }
        type_ii_port_sel_s_& type_ii_port_sel()
        {
          assert_choice_type(types::type_ii_port_sel, type_, "subType");
          return c.get<type_ii_port_sel_s_>();
        }
        const type_ii_s_& type_ii() const
        {
          assert_choice_type(types::type_ii, type_, "subType");
          return c.get<type_ii_s_>();
        }
        const type_ii_port_sel_s_& type_ii_port_sel() const
        {
          assert_choice_type(types::type_ii_port_sel, type_, "subType");
          return c.get<type_ii_port_sel_s_>();
        }
        type_ii_s_&          set_type_ii();
        type_ii_port_sel_s_& set_type_ii_port_sel();

      private:
        types                                            type_;
        choice_buffer_t<type_ii_port_sel_s_, type_ii_s_> c;

        void destroy_();
      };
      struct phase_alphabet_size_opts {
        enum options { n4, n8, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using phase_alphabet_size_e_ = enumerated<phase_alphabet_size_opts>;
      struct nof_beams_opts {
        enum options { two, three, four, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using nof_beams_e_ = enumerated<nof_beams_opts>;

      // member variables
      sub_type_c_            sub_type;
      phase_alphabet_size_e_ phase_alphabet_size;
      bool                   subband_amplitude = false;
      nof_beams_e_           nof_beams;
    };
    struct types_opts {
      enum options { type1, type2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    codebook_type_c_() = default;
    codebook_type_c_(const codebook_type_c_& other);
    codebook_type_c_& operator=(const codebook_type_c_& other);
    ~codebook_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    type1_s_& type1()
    {
      assert_choice_type(types::type1, type_, "codebookType");
      return c.get<type1_s_>();
    }
    type2_s_& type2()
    {
      assert_choice_type(types::type2, type_, "codebookType");
      return c.get<type2_s_>();
    }
    const type1_s_& type1() const
    {
      assert_choice_type(types::type1, type_, "codebookType");
      return c.get<type1_s_>();
    }
    const type2_s_& type2() const
    {
      assert_choice_type(types::type2, type_, "codebookType");
      return c.get<type2_s_>();
    }
    type1_s_& set_type1();
    type2_s_& set_type2();

  private:
    types                               type_;
    choice_buffer_t<type1_s_, type2_s_> c;

    void destroy_();
  };

  // member variables
  codebook_type_c_ codebook_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookConfig-r16 ::= SEQUENCE
struct codebook_cfg_r16_s {
  struct codebook_type_c_ {
    struct type2_s_ {
      struct sub_type_c_ {
        struct type_ii_r16_s_ {
          struct n1_n2_codebook_subset_restrict_r16_c_ {
            struct types_opts {
              enum options {
                two_one,
                two_two,
                four_one,
                three_two,
                six_one,
                four_two,
                eight_one,
                four_three,
                six_two,
                twelve_one,
                four_four,
                eight_two,
                sixteen_one,
                nulltype
              } value;

              const char* to_string() const;
            };
            using types = enumerated<types_opts>;

            // choice methods
            n1_n2_codebook_subset_restrict_r16_c_() = default;
            n1_n2_codebook_subset_restrict_r16_c_(const n1_n2_codebook_subset_restrict_r16_c_& other);
            n1_n2_codebook_subset_restrict_r16_c_& operator=(const n1_n2_codebook_subset_restrict_r16_c_& other);
            ~n1_n2_codebook_subset_restrict_r16_c_() { destroy_(); }
            void        set(types::options e = types::nulltype);
            types       type() const { return type_; }
            SRSASN_CODE pack(bit_ref& bref) const;
            SRSASN_CODE unpack(cbit_ref& bref);
            void        to_json(json_writer& j) const;
            // getters
            fixed_bitstring<16>& two_one()
            {
              assert_choice_type(types::two_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<16>>();
            }
            fixed_bitstring<43>& two_two()
            {
              assert_choice_type(types::two_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<43>>();
            }
            fixed_bitstring<32>& four_one()
            {
              assert_choice_type(types::four_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<32>>();
            }
            fixed_bitstring<59>& three_two()
            {
              assert_choice_type(types::three_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<59>>();
            }
            fixed_bitstring<48>& six_one()
            {
              assert_choice_type(types::six_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<48>>();
            }
            fixed_bitstring<75>& four_two()
            {
              assert_choice_type(types::four_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<75>>();
            }
            fixed_bitstring<64>& eight_one()
            {
              assert_choice_type(types::eight_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<64>>();
            }
            fixed_bitstring<107>& four_three()
            {
              assert_choice_type(types::four_three, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<107>>();
            }
            fixed_bitstring<107>& six_two()
            {
              assert_choice_type(types::six_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<107>>();
            }
            fixed_bitstring<96>& twelve_one()
            {
              assert_choice_type(types::twelve_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<96>>();
            }
            fixed_bitstring<139>& four_four()
            {
              assert_choice_type(types::four_four, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<139>>();
            }
            fixed_bitstring<139>& eight_two()
            {
              assert_choice_type(types::eight_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<139>>();
            }
            fixed_bitstring<128>& sixteen_one()
            {
              assert_choice_type(types::sixteen_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<128>>();
            }
            const fixed_bitstring<16>& two_one() const
            {
              assert_choice_type(types::two_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<16>>();
            }
            const fixed_bitstring<43>& two_two() const
            {
              assert_choice_type(types::two_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<43>>();
            }
            const fixed_bitstring<32>& four_one() const
            {
              assert_choice_type(types::four_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<32>>();
            }
            const fixed_bitstring<59>& three_two() const
            {
              assert_choice_type(types::three_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<59>>();
            }
            const fixed_bitstring<48>& six_one() const
            {
              assert_choice_type(types::six_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<48>>();
            }
            const fixed_bitstring<75>& four_two() const
            {
              assert_choice_type(types::four_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<75>>();
            }
            const fixed_bitstring<64>& eight_one() const
            {
              assert_choice_type(types::eight_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<64>>();
            }
            const fixed_bitstring<107>& four_three() const
            {
              assert_choice_type(types::four_three, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<107>>();
            }
            const fixed_bitstring<107>& six_two() const
            {
              assert_choice_type(types::six_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<107>>();
            }
            const fixed_bitstring<96>& twelve_one() const
            {
              assert_choice_type(types::twelve_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<96>>();
            }
            const fixed_bitstring<139>& four_four() const
            {
              assert_choice_type(types::four_four, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<139>>();
            }
            const fixed_bitstring<139>& eight_two() const
            {
              assert_choice_type(types::eight_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<139>>();
            }
            const fixed_bitstring<128>& sixteen_one() const
            {
              assert_choice_type(types::sixteen_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<128>>();
            }
            fixed_bitstring<16>&  set_two_one();
            fixed_bitstring<43>&  set_two_two();
            fixed_bitstring<32>&  set_four_one();
            fixed_bitstring<59>&  set_three_two();
            fixed_bitstring<48>&  set_six_one();
            fixed_bitstring<75>&  set_four_two();
            fixed_bitstring<64>&  set_eight_one();
            fixed_bitstring<107>& set_four_three();
            fixed_bitstring<107>& set_six_two();
            fixed_bitstring<96>&  set_twelve_one();
            fixed_bitstring<139>& set_four_four();
            fixed_bitstring<139>& set_eight_two();
            fixed_bitstring<128>& set_sixteen_one();

          private:
            types                                 type_;
            choice_buffer_t<fixed_bitstring<139>> c;

            void destroy_();
          };

          // member variables
          n1_n2_codebook_subset_restrict_r16_c_ n1_n2_codebook_subset_restrict_r16;
          fixed_bitstring<4>                    type_ii_ri_restrict_r16;
        };
        struct type_ii_port_sel_r16_s_ {
          struct port_sel_sampling_size_r16_opts {
            enum options { n1, n2, n3, n4, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using port_sel_sampling_size_r16_e_ = enumerated<port_sel_sampling_size_r16_opts>;

          // member variables
          port_sel_sampling_size_r16_e_ port_sel_sampling_size_r16;
          fixed_bitstring<4>            type_ii_port_sel_ri_restrict_r16;
        };
        struct types_opts {
          enum options { type_ii_r16, type_ii_port_sel_r16, nulltype } value;

          const char* to_string() const;
        };
        using types = enumerated<types_opts>;

        // choice methods
        sub_type_c_() = default;
        sub_type_c_(const sub_type_c_& other);
        sub_type_c_& operator=(const sub_type_c_& other);
        ~sub_type_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        type_ii_r16_s_& type_ii_r16()
        {
          assert_choice_type(types::type_ii_r16, type_, "subType");
          return c.get<type_ii_r16_s_>();
        }
        type_ii_port_sel_r16_s_& type_ii_port_sel_r16()
        {
          assert_choice_type(types::type_ii_port_sel_r16, type_, "subType");
          return c.get<type_ii_port_sel_r16_s_>();
        }
        const type_ii_r16_s_& type_ii_r16() const
        {
          assert_choice_type(types::type_ii_r16, type_, "subType");
          return c.get<type_ii_r16_s_>();
        }
        const type_ii_port_sel_r16_s_& type_ii_port_sel_r16() const
        {
          assert_choice_type(types::type_ii_port_sel_r16, type_, "subType");
          return c.get<type_ii_port_sel_r16_s_>();
        }
        type_ii_r16_s_&          set_type_ii_r16();
        type_ii_port_sel_r16_s_& set_type_ii_port_sel_r16();

      private:
        types                                                    type_;
        choice_buffer_t<type_ii_port_sel_r16_s_, type_ii_r16_s_> c;

        void destroy_();
      };

      // member variables
      sub_type_c_ sub_type;
      uint8_t     nof_pmi_subbands_per_cqi_subband_r16 = 1;
      uint8_t     param_combination_r16                = 1;
    };
    struct types_opts {
      enum options { type2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::type2; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    type2_s_&       type2() { return c; }
    const type2_s_& type2() const { return c; }

  private:
    type2_s_ c;
  };

  // member variables
  codebook_type_c_ codebook_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookConfig-r17 ::= SEQUENCE
struct codebook_cfg_r17_s {
  struct codebook_type_c_ {
    struct type1_s_ {
      struct type_i_single_panel_group1_r17_s_ {
        struct nr_of_ant_ports_c_ {
          struct two_s_ {
            fixed_bitstring<6> two_tx_codebook_subset_restrict1_r17;
          };
          struct more_than_two_s_ {
            struct n1_n2_c_ {
              struct types_opts {
                enum options {
                  two_one_type_i_single_panel_restrict1_r17,
                  two_two_type_i_single_panel_restrict1_r17,
                  four_one_type_i_single_panel_restrict1_r17,
                  three_two_type_i_single_panel_restrict1_r17,
                  six_one_type_i_single_panel_restrict1_r17,
                  four_two_type_i_single_panel_restrict1_r17,
                  eight_one_type_i_single_panel_restrict1_r17,
                  four_three_type_i_single_panel_restrict1_r17,
                  six_two_type_i_single_panel_restrict1_r17,
                  twelve_one_type_i_single_panel_restrict1_r17,
                  four_four_type_i_single_panel_restrict1_r17,
                  eight_two_type_i_single_panel_restrict1_r17,
                  sixteen_one_type_i_single_panel_restrict1_r17,
                  nulltype
                } value;

                const char* to_string() const;
              };
              using types = enumerated<types_opts>;

              // choice methods
              n1_n2_c_() = default;
              n1_n2_c_(const n1_n2_c_& other);
              n1_n2_c_& operator=(const n1_n2_c_& other);
              ~n1_n2_c_() { destroy_(); }
              void        set(types::options e = types::nulltype);
              types       type() const { return type_; }
              SRSASN_CODE pack(bit_ref& bref) const;
              SRSASN_CODE unpack(cbit_ref& bref);
              void        to_json(json_writer& j) const;
              // getters
              fixed_bitstring<8>& two_one_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::two_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<8>>();
              }
              fixed_bitstring<64>& two_two_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::two_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<64>>();
              }
              fixed_bitstring<16>& four_one_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::four_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<16>>();
              }
              fixed_bitstring<96>& three_two_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::three_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<96>>();
              }
              fixed_bitstring<24>& six_one_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::six_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<24>>();
              }
              fixed_bitstring<128>& four_two_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::four_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<128>>();
              }
              fixed_bitstring<32>& eight_one_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::eight_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<32>>();
              }
              fixed_bitstring<192>& four_three_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::four_three_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<192>>();
              }
              fixed_bitstring<192>& six_two_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::six_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<192>>();
              }
              fixed_bitstring<48>& twelve_one_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::twelve_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<48>>();
              }
              fixed_bitstring<256>& four_four_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::four_four_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<256>>();
              }
              fixed_bitstring<256>& eight_two_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::eight_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<256>>();
              }
              fixed_bitstring<64>& sixteen_one_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::sixteen_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<64>>();
              }
              const fixed_bitstring<8>& two_one_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::two_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<8>>();
              }
              const fixed_bitstring<64>& two_two_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::two_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<64>>();
              }
              const fixed_bitstring<16>& four_one_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::four_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<16>>();
              }
              const fixed_bitstring<96>& three_two_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::three_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<96>>();
              }
              const fixed_bitstring<24>& six_one_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::six_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<24>>();
              }
              const fixed_bitstring<128>& four_two_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::four_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<128>>();
              }
              const fixed_bitstring<32>& eight_one_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::eight_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<32>>();
              }
              const fixed_bitstring<192>& four_three_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::four_three_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<192>>();
              }
              const fixed_bitstring<192>& six_two_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::six_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<192>>();
              }
              const fixed_bitstring<48>& twelve_one_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::twelve_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<48>>();
              }
              const fixed_bitstring<256>& four_four_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::four_four_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<256>>();
              }
              const fixed_bitstring<256>& eight_two_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::eight_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<256>>();
              }
              const fixed_bitstring<64>& sixteen_one_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::sixteen_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<64>>();
              }
              fixed_bitstring<8>&   set_two_one_type_i_single_panel_restrict1_r17();
              fixed_bitstring<64>&  set_two_two_type_i_single_panel_restrict1_r17();
              fixed_bitstring<16>&  set_four_one_type_i_single_panel_restrict1_r17();
              fixed_bitstring<96>&  set_three_two_type_i_single_panel_restrict1_r17();
              fixed_bitstring<24>&  set_six_one_type_i_single_panel_restrict1_r17();
              fixed_bitstring<128>& set_four_two_type_i_single_panel_restrict1_r17();
              fixed_bitstring<32>&  set_eight_one_type_i_single_panel_restrict1_r17();
              fixed_bitstring<192>& set_four_three_type_i_single_panel_restrict1_r17();
              fixed_bitstring<192>& set_six_two_type_i_single_panel_restrict1_r17();
              fixed_bitstring<48>&  set_twelve_one_type_i_single_panel_restrict1_r17();
              fixed_bitstring<256>& set_four_four_type_i_single_panel_restrict1_r17();
              fixed_bitstring<256>& set_eight_two_type_i_single_panel_restrict1_r17();
              fixed_bitstring<64>&  set_sixteen_one_type_i_single_panel_restrict1_r17();

            private:
              types                                 type_;
              choice_buffer_t<fixed_bitstring<256>> c;

              void destroy_();
            };

            // member variables
            n1_n2_c_ n1_n2;
          };
          struct types_opts {
            enum options { two, more_than_two, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using types = enumerated<types_opts>;

          // choice methods
          nr_of_ant_ports_c_() = default;
          nr_of_ant_ports_c_(const nr_of_ant_ports_c_& other);
          nr_of_ant_ports_c_& operator=(const nr_of_ant_ports_c_& other);
          ~nr_of_ant_ports_c_() { destroy_(); }
          void        set(types::options e = types::nulltype);
          types       type() const { return type_; }
          SRSASN_CODE pack(bit_ref& bref) const;
          SRSASN_CODE unpack(cbit_ref& bref);
          void        to_json(json_writer& j) const;
          // getters
          two_s_& two()
          {
            assert_choice_type(types::two, type_, "nrOfAntennaPorts");
            return c.get<two_s_>();
          }
          more_than_two_s_& more_than_two()
          {
            assert_choice_type(types::more_than_two, type_, "nrOfAntennaPorts");
            return c.get<more_than_two_s_>();
          }
          const two_s_& two() const
          {
            assert_choice_type(types::two, type_, "nrOfAntennaPorts");
            return c.get<two_s_>();
          }
          const more_than_two_s_& more_than_two() const
          {
            assert_choice_type(types::more_than_two, type_, "nrOfAntennaPorts");
            return c.get<more_than_two_s_>();
          }
          two_s_&           set_two();
          more_than_two_s_& set_more_than_two();

        private:
          types                                     type_;
          choice_buffer_t<more_than_two_s_, two_s_> c;

          void destroy_();
        };

        // member variables
        nr_of_ant_ports_c_ nr_of_ant_ports;
      };
      struct type_i_single_panel_group2_r17_s_ {
        struct nr_of_ant_ports_c_ {
          struct two_s_ {
            fixed_bitstring<6> two_tx_codebook_subset_restrict2_r17;
          };
          struct more_than_two_s_ {
            struct n1_n2_c_ {
              struct types_opts {
                enum options {
                  two_one_type_i_single_panel_restrict2_r17,
                  two_two_type_i_single_panel_restrict2_r17,
                  four_one_type_i_single_panel_restrict2_r17,
                  three_two_type_i_single_panel_restrict2_r17,
                  six_one_type_i_single_panel_restrict2_r17,
                  four_two_type_i_single_panel_restrict2_r17,
                  eight_one_type_i_single_panel_restrict2_r17,
                  four_three_type_i_single_panel_restrict2_r17,
                  six_two_type_i_single_panel_restrict2_r17,
                  twelve_one_type_i_single_panel_restrict2_r17,
                  four_four_type_i_single_panel_restrict2_r17,
                  eight_two_type_i_single_panel_restrict2_r17,
                  sixteen_one_type_i_single_panel_restrict2_r17,
                  nulltype
                } value;

                const char* to_string() const;
              };
              using types = enumerated<types_opts>;

              // choice methods
              n1_n2_c_() = default;
              n1_n2_c_(const n1_n2_c_& other);
              n1_n2_c_& operator=(const n1_n2_c_& other);
              ~n1_n2_c_() { destroy_(); }
              void        set(types::options e = types::nulltype);
              types       type() const { return type_; }
              SRSASN_CODE pack(bit_ref& bref) const;
              SRSASN_CODE unpack(cbit_ref& bref);
              void        to_json(json_writer& j) const;
              // getters
              fixed_bitstring<8>& two_one_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::two_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<8>>();
              }
              fixed_bitstring<64>& two_two_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::two_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<64>>();
              }
              fixed_bitstring<16>& four_one_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::four_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<16>>();
              }
              fixed_bitstring<96>& three_two_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::three_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<96>>();
              }
              fixed_bitstring<24>& six_one_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::six_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<24>>();
              }
              fixed_bitstring<128>& four_two_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::four_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<128>>();
              }
              fixed_bitstring<32>& eight_one_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::eight_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<32>>();
              }
              fixed_bitstring<192>& four_three_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::four_three_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<192>>();
              }
              fixed_bitstring<192>& six_two_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::six_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<192>>();
              }
              fixed_bitstring<48>& twelve_one_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::twelve_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<48>>();
              }
              fixed_bitstring<256>& four_four_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::four_four_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<256>>();
              }
              fixed_bitstring<256>& eight_two_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::eight_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<256>>();
              }
              fixed_bitstring<64>& sixteen_one_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::sixteen_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<64>>();
              }
              const fixed_bitstring<8>& two_one_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::two_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<8>>();
              }
              const fixed_bitstring<64>& two_two_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::two_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<64>>();
              }
              const fixed_bitstring<16>& four_one_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::four_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<16>>();
              }
              const fixed_bitstring<96>& three_two_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::three_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<96>>();
              }
              const fixed_bitstring<24>& six_one_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::six_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<24>>();
              }
              const fixed_bitstring<128>& four_two_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::four_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<128>>();
              }
              const fixed_bitstring<32>& eight_one_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::eight_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<32>>();
              }
              const fixed_bitstring<192>& four_three_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::four_three_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<192>>();
              }
              const fixed_bitstring<192>& six_two_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::six_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<192>>();
              }
              const fixed_bitstring<48>& twelve_one_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::twelve_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<48>>();
              }
              const fixed_bitstring<256>& four_four_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::four_four_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<256>>();
              }
              const fixed_bitstring<256>& eight_two_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::eight_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<256>>();
              }
              const fixed_bitstring<64>& sixteen_one_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::sixteen_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<64>>();
              }
              fixed_bitstring<8>&   set_two_one_type_i_single_panel_restrict2_r17();
              fixed_bitstring<64>&  set_two_two_type_i_single_panel_restrict2_r17();
              fixed_bitstring<16>&  set_four_one_type_i_single_panel_restrict2_r17();
              fixed_bitstring<96>&  set_three_two_type_i_single_panel_restrict2_r17();
              fixed_bitstring<24>&  set_six_one_type_i_single_panel_restrict2_r17();
              fixed_bitstring<128>& set_four_two_type_i_single_panel_restrict2_r17();
              fixed_bitstring<32>&  set_eight_one_type_i_single_panel_restrict2_r17();
              fixed_bitstring<192>& set_four_three_type_i_single_panel_restrict2_r17();
              fixed_bitstring<192>& set_six_two_type_i_single_panel_restrict2_r17();
              fixed_bitstring<48>&  set_twelve_one_type_i_single_panel_restrict2_r17();
              fixed_bitstring<256>& set_four_four_type_i_single_panel_restrict2_r17();
              fixed_bitstring<256>& set_eight_two_type_i_single_panel_restrict2_r17();
              fixed_bitstring<64>&  set_sixteen_one_type_i_single_panel_restrict2_r17();

            private:
              types                                 type_;
              choice_buffer_t<fixed_bitstring<256>> c;

              void destroy_();
            };

            // member variables
            n1_n2_c_ n1_n2;
          };
          struct types_opts {
            enum options { two, more_than_two, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using types = enumerated<types_opts>;

          // choice methods
          nr_of_ant_ports_c_() = default;
          nr_of_ant_ports_c_(const nr_of_ant_ports_c_& other);
          nr_of_ant_ports_c_& operator=(const nr_of_ant_ports_c_& other);
          ~nr_of_ant_ports_c_() { destroy_(); }
          void        set(types::options e = types::nulltype);
          types       type() const { return type_; }
          SRSASN_CODE pack(bit_ref& bref) const;
          SRSASN_CODE unpack(cbit_ref& bref);
          void        to_json(json_writer& j) const;
          // getters
          two_s_& two()
          {
            assert_choice_type(types::two, type_, "nrOfAntennaPorts");
            return c.get<two_s_>();
          }
          more_than_two_s_& more_than_two()
          {
            assert_choice_type(types::more_than_two, type_, "nrOfAntennaPorts");
            return c.get<more_than_two_s_>();
          }
          const two_s_& two() const
          {
            assert_choice_type(types::two, type_, "nrOfAntennaPorts");
            return c.get<two_s_>();
          }
          const more_than_two_s_& more_than_two() const
          {
            assert_choice_type(types::more_than_two, type_, "nrOfAntennaPorts");
            return c.get<more_than_two_s_>();
          }
          two_s_&           set_two();
          more_than_two_s_& set_more_than_two();

        private:
          types                                     type_;
          choice_buffer_t<more_than_two_s_, two_s_> c;

          void destroy_();
        };

        // member variables
        nr_of_ant_ports_c_ nr_of_ant_ports;
      };

      // member variables
      bool                              type_i_single_panel_group1_r17_present           = false;
      bool                              type_i_single_panel_group2_r17_present           = false;
      bool                              type_i_single_panel_ri_restrict_strp_r17_present = false;
      bool                              type_i_single_panel_ri_restrict_sdm_r17_present  = false;
      type_i_single_panel_group1_r17_s_ type_i_single_panel_group1_r17;
      type_i_single_panel_group2_r17_s_ type_i_single_panel_group2_r17;
      fixed_bitstring<8>                type_i_single_panel_ri_restrict_strp_r17;
      fixed_bitstring<4>                type_i_single_panel_ri_restrict_sdm_r17;
    };
    struct type2_s_ {
      struct type_ii_port_sel_r17_s_ {
        struct value_of_n_r17_opts {
          enum options { n2, n4, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        using value_of_n_r17_e_ = enumerated<value_of_n_r17_opts>;

        // member variables
        bool               value_of_n_r17_present                       = false;
        bool               nof_pmi_subbands_per_cqi_subband_r17_present = false;
        uint8_t            param_combination_r17                        = 1;
        value_of_n_r17_e_  value_of_n_r17;
        uint8_t            nof_pmi_subbands_per_cqi_subband_r17 = 1;
        fixed_bitstring<4> type_ii_port_sel_ri_restrict_r17;
      };

      // member variables
      type_ii_port_sel_r17_s_ type_ii_port_sel_r17;
    };
    struct types_opts {
      enum options { type1, type2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    codebook_type_c_() = default;
    codebook_type_c_(const codebook_type_c_& other);
    codebook_type_c_& operator=(const codebook_type_c_& other);
    ~codebook_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    type1_s_& type1()
    {
      assert_choice_type(types::type1, type_, "codebookType");
      return c.get<type1_s_>();
    }
    type2_s_& type2()
    {
      assert_choice_type(types::type2, type_, "codebookType");
      return c.get<type2_s_>();
    }
    const type1_s_& type1() const
    {
      assert_choice_type(types::type1, type_, "codebookType");
      return c.get<type1_s_>();
    }
    const type2_s_& type2() const
    {
      assert_choice_type(types::type2, type_, "codebookType");
      return c.get<type2_s_>();
    }
    type1_s_& set_type1();
    type2_s_& set_type2();

  private:
    types                               type_;
    choice_buffer_t<type1_s_, type2_s_> c;

    void destroy_();
  };

  // member variables
  codebook_type_c_ codebook_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookConfig-v1730 ::= SEQUENCE
struct codebook_cfg_v1730_s {
  struct codebook_type_c_ {
    struct type1_s_ {
      bool    codebook_mode_present = false;
      uint8_t codebook_mode         = 1;
    };
    struct types_opts {
      enum options { type1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::type1; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    type1_s_&       type1() { return c; }
    const type1_s_& type1() const { return c; }

  private:
    type1_s_ c;
  };

  // member variables
  codebook_type_c_ codebook_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-CSI-Resource ::= SEQUENCE
struct pucch_csi_res_s {
  uint8_t ul_bw_part_id = 0;
  uint8_t pucch_res     = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PortIndexFor8Ranks ::= CHOICE
struct port_idx_for8_ranks_c {
  struct port_idx8_s_ {
    using rank2_8_l_ = std::array<uint8_t, 2>;
    using rank3_8_l_ = std::array<uint8_t, 3>;
    using rank4_8_l_ = std::array<uint8_t, 4>;
    using rank5_8_l_ = std::array<uint8_t, 5>;
    using rank6_8_l_ = std::array<uint8_t, 6>;
    using rank7_8_l_ = std::array<uint8_t, 7>;
    using rank8_8_l_ = std::array<uint8_t, 8>;

    // member variables
    bool       rank1_8_present = false;
    bool       rank2_8_present = false;
    bool       rank3_8_present = false;
    bool       rank4_8_present = false;
    bool       rank5_8_present = false;
    bool       rank6_8_present = false;
    bool       rank7_8_present = false;
    bool       rank8_8_present = false;
    uint8_t    rank1_8         = 0;
    rank2_8_l_ rank2_8;
    rank3_8_l_ rank3_8;
    rank4_8_l_ rank4_8;
    rank5_8_l_ rank5_8;
    rank6_8_l_ rank6_8;
    rank7_8_l_ rank7_8;
    rank8_8_l_ rank8_8;
  };
  struct port_idx4_s_ {
    using rank2_4_l_ = std::array<uint8_t, 2>;
    using rank3_4_l_ = std::array<uint8_t, 3>;
    using rank4_4_l_ = std::array<uint8_t, 4>;

    // member variables
    bool       rank1_4_present = false;
    bool       rank2_4_present = false;
    bool       rank3_4_present = false;
    bool       rank4_4_present = false;
    uint8_t    rank1_4         = 0;
    rank2_4_l_ rank2_4;
    rank3_4_l_ rank3_4;
    rank4_4_l_ rank4_4;
  };
  struct port_idx2_s_ {
    using rank2_2_l_ = std::array<uint8_t, 2>;

    // member variables
    bool       rank1_2_present = false;
    bool       rank2_2_present = false;
    uint8_t    rank1_2         = 0;
    rank2_2_l_ rank2_2;
  };
  struct types_opts {
    enum options { port_idx8, port_idx4, port_idx2, port_idx1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  port_idx_for8_ranks_c() = default;
  port_idx_for8_ranks_c(const port_idx_for8_ranks_c& other);
  port_idx_for8_ranks_c& operator=(const port_idx_for8_ranks_c& other);
  ~port_idx_for8_ranks_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  port_idx8_s_& port_idx8()
  {
    assert_choice_type(types::port_idx8, type_, "PortIndexFor8Ranks");
    return c.get<port_idx8_s_>();
  }
  port_idx4_s_& port_idx4()
  {
    assert_choice_type(types::port_idx4, type_, "PortIndexFor8Ranks");
    return c.get<port_idx4_s_>();
  }
  port_idx2_s_& port_idx2()
  {
    assert_choice_type(types::port_idx2, type_, "PortIndexFor8Ranks");
    return c.get<port_idx2_s_>();
  }
  const port_idx8_s_& port_idx8() const
  {
    assert_choice_type(types::port_idx8, type_, "PortIndexFor8Ranks");
    return c.get<port_idx8_s_>();
  }
  const port_idx4_s_& port_idx4() const
  {
    assert_choice_type(types::port_idx4, type_, "PortIndexFor8Ranks");
    return c.get<port_idx4_s_>();
  }
  const port_idx2_s_& port_idx2() const
  {
    assert_choice_type(types::port_idx2, type_, "PortIndexFor8Ranks");
    return c.get<port_idx2_s_>();
  }
  port_idx8_s_& set_port_idx8();
  port_idx4_s_& set_port_idx4();
  port_idx2_s_& set_port_idx2();
  void          set_port_idx1();

private:
  types                                                     type_;
  choice_buffer_t<port_idx2_s_, port_idx4_s_, port_idx8_s_> c;

  void destroy_();
};

// CSI-ReportConfig ::= SEQUENCE
struct csi_report_cfg_s {
  struct report_cfg_type_c_ {
    struct periodic_s_ {
      using pucch_csi_res_list_l_ = dyn_array<pucch_csi_res_s>;

      // member variables
      csi_report_periodicity_and_offset_c report_slot_cfg;
      pucch_csi_res_list_l_               pucch_csi_res_list;
    };
    struct semi_persistent_on_pucch_s_ {
      using pucch_csi_res_list_l_ = dyn_array<pucch_csi_res_s>;

      // member variables
      csi_report_periodicity_and_offset_c report_slot_cfg;
      pucch_csi_res_list_l_               pucch_csi_res_list;
    };
    struct semi_persistent_on_pusch_s_ {
      struct report_slot_cfg_opts {
        enum options { sl5, sl10, sl20, sl40, sl80, sl160, sl320, nulltype } value;
        typedef uint16_t number_type;

        const char* to_string() const;
        uint16_t    to_number() const;
      };
      using report_slot_cfg_e_         = enumerated<report_slot_cfg_opts>;
      using report_slot_offset_list_l_ = bounded_array<uint8_t, 16>;

      // member variables
      report_slot_cfg_e_         report_slot_cfg;
      report_slot_offset_list_l_ report_slot_offset_list;
      uint8_t                    p0alpha = 0;
    };
    struct aperiodic_s_ {
      using report_slot_offset_list_l_ = bounded_array<uint8_t, 16>;

      // member variables
      report_slot_offset_list_l_ report_slot_offset_list;
    };
    struct types_opts {
      enum options { periodic, semi_persistent_on_pucch, semi_persistent_on_pusch, aperiodic, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    report_cfg_type_c_() = default;
    report_cfg_type_c_(const report_cfg_type_c_& other);
    report_cfg_type_c_& operator=(const report_cfg_type_c_& other);
    ~report_cfg_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    periodic_s_& periodic()
    {
      assert_choice_type(types::periodic, type_, "reportConfigType");
      return c.get<periodic_s_>();
    }
    semi_persistent_on_pucch_s_& semi_persistent_on_pucch()
    {
      assert_choice_type(types::semi_persistent_on_pucch, type_, "reportConfigType");
      return c.get<semi_persistent_on_pucch_s_>();
    }
    semi_persistent_on_pusch_s_& semi_persistent_on_pusch()
    {
      assert_choice_type(types::semi_persistent_on_pusch, type_, "reportConfigType");
      return c.get<semi_persistent_on_pusch_s_>();
    }
    aperiodic_s_& aperiodic()
    {
      assert_choice_type(types::aperiodic, type_, "reportConfigType");
      return c.get<aperiodic_s_>();
    }
    const periodic_s_& periodic() const
    {
      assert_choice_type(types::periodic, type_, "reportConfigType");
      return c.get<periodic_s_>();
    }
    const semi_persistent_on_pucch_s_& semi_persistent_on_pucch() const
    {
      assert_choice_type(types::semi_persistent_on_pucch, type_, "reportConfigType");
      return c.get<semi_persistent_on_pucch_s_>();
    }
    const semi_persistent_on_pusch_s_& semi_persistent_on_pusch() const
    {
      assert_choice_type(types::semi_persistent_on_pusch, type_, "reportConfigType");
      return c.get<semi_persistent_on_pusch_s_>();
    }
    const aperiodic_s_& aperiodic() const
    {
      assert_choice_type(types::aperiodic, type_, "reportConfigType");
      return c.get<aperiodic_s_>();
    }
    periodic_s_&                 set_periodic();
    semi_persistent_on_pucch_s_& set_semi_persistent_on_pucch();
    semi_persistent_on_pusch_s_& set_semi_persistent_on_pusch();
    aperiodic_s_&                set_aperiodic();

  private:
    types                                                                                                type_;
    choice_buffer_t<aperiodic_s_, periodic_s_, semi_persistent_on_pucch_s_, semi_persistent_on_pusch_s_> c;

    void destroy_();
  };
  struct report_quant_c_ {
    struct cri_ri_i1_cqi_s_ {
      struct pdsch_bundle_size_for_csi_opts {
        enum options { n2, n4, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using pdsch_bundle_size_for_csi_e_ = enumerated<pdsch_bundle_size_for_csi_opts>;

      // member variables
      bool                         pdsch_bundle_size_for_csi_present = false;
      pdsch_bundle_size_for_csi_e_ pdsch_bundle_size_for_csi;
    };
    struct types_opts {
      enum options {
        none,
        cri_ri_pmi_cqi,
        cri_ri_i1,
        cri_ri_i1_cqi,
        cri_ri_cqi,
        cri_rsrp,
        ssb_idx_rsrp,
        cri_ri_li_pmi_cqi,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    report_quant_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cri_ri_i1_cqi_s_& cri_ri_i1_cqi()
    {
      assert_choice_type(types::cri_ri_i1_cqi, type_, "reportQuantity");
      return c;
    }
    const cri_ri_i1_cqi_s_& cri_ri_i1_cqi() const
    {
      assert_choice_type(types::cri_ri_i1_cqi, type_, "reportQuantity");
      return c;
    }
    void              set_none();
    void              set_cri_ri_pmi_cqi();
    void              set_cri_ri_i1();
    cri_ri_i1_cqi_s_& set_cri_ri_i1_cqi();
    void              set_cri_ri_cqi();
    void              set_cri_rsrp();
    void              set_ssb_idx_rsrp();
    void              set_cri_ri_li_pmi_cqi();

  private:
    types            type_;
    cri_ri_i1_cqi_s_ c;
  };
  struct report_freq_cfg_s_ {
    struct cqi_format_ind_opts {
      enum options { wideband_cqi, subband_cqi, nulltype } value;

      const char* to_string() const;
    };
    using cqi_format_ind_e_ = enumerated<cqi_format_ind_opts>;
    struct pmi_format_ind_opts {
      enum options { wideband_pmi, subband_pmi, nulltype } value;

      const char* to_string() const;
    };
    using pmi_format_ind_e_ = enumerated<pmi_format_ind_opts>;
    struct csi_report_band_c_ {
      struct types_opts {
        enum options {
          subbands3,
          subbands4,
          subbands5,
          subbands6,
          subbands7,
          subbands8,
          subbands9,
          subbands10,
          subbands11,
          subbands12,
          subbands13,
          subbands14,
          subbands15,
          subbands16,
          subbands17,
          subbands18,
          // ...
          subbands19_v1530,
          nulltype
        } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using types = enumerated<types_opts, true, 1>;

      // choice methods
      csi_report_band_c_() = default;
      csi_report_band_c_(const csi_report_band_c_& other);
      csi_report_band_c_& operator=(const csi_report_band_c_& other);
      ~csi_report_band_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      fixed_bitstring<3>& subbands3()
      {
        assert_choice_type(types::subbands3, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<3>>();
      }
      fixed_bitstring<4>& subbands4()
      {
        assert_choice_type(types::subbands4, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<4>>();
      }
      fixed_bitstring<5>& subbands5()
      {
        assert_choice_type(types::subbands5, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<5>>();
      }
      fixed_bitstring<6>& subbands6()
      {
        assert_choice_type(types::subbands6, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<6>>();
      }
      fixed_bitstring<7>& subbands7()
      {
        assert_choice_type(types::subbands7, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<7>>();
      }
      fixed_bitstring<8>& subbands8()
      {
        assert_choice_type(types::subbands8, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<8>>();
      }
      fixed_bitstring<9>& subbands9()
      {
        assert_choice_type(types::subbands9, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<9>>();
      }
      fixed_bitstring<10>& subbands10()
      {
        assert_choice_type(types::subbands10, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<10>>();
      }
      fixed_bitstring<11>& subbands11()
      {
        assert_choice_type(types::subbands11, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<11>>();
      }
      fixed_bitstring<12>& subbands12()
      {
        assert_choice_type(types::subbands12, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<12>>();
      }
      fixed_bitstring<13>& subbands13()
      {
        assert_choice_type(types::subbands13, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<13>>();
      }
      fixed_bitstring<14>& subbands14()
      {
        assert_choice_type(types::subbands14, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<14>>();
      }
      fixed_bitstring<15>& subbands15()
      {
        assert_choice_type(types::subbands15, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<15>>();
      }
      fixed_bitstring<16>& subbands16()
      {
        assert_choice_type(types::subbands16, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<16>>();
      }
      fixed_bitstring<17>& subbands17()
      {
        assert_choice_type(types::subbands17, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<17>>();
      }
      fixed_bitstring<18>& subbands18()
      {
        assert_choice_type(types::subbands18, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<18>>();
      }
      fixed_bitstring<19>& subbands19_v1530()
      {
        assert_choice_type(types::subbands19_v1530, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<19>>();
      }
      const fixed_bitstring<3>& subbands3() const
      {
        assert_choice_type(types::subbands3, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<3>>();
      }
      const fixed_bitstring<4>& subbands4() const
      {
        assert_choice_type(types::subbands4, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<4>>();
      }
      const fixed_bitstring<5>& subbands5() const
      {
        assert_choice_type(types::subbands5, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<5>>();
      }
      const fixed_bitstring<6>& subbands6() const
      {
        assert_choice_type(types::subbands6, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<6>>();
      }
      const fixed_bitstring<7>& subbands7() const
      {
        assert_choice_type(types::subbands7, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<7>>();
      }
      const fixed_bitstring<8>& subbands8() const
      {
        assert_choice_type(types::subbands8, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<8>>();
      }
      const fixed_bitstring<9>& subbands9() const
      {
        assert_choice_type(types::subbands9, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<9>>();
      }
      const fixed_bitstring<10>& subbands10() const
      {
        assert_choice_type(types::subbands10, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<10>>();
      }
      const fixed_bitstring<11>& subbands11() const
      {
        assert_choice_type(types::subbands11, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<11>>();
      }
      const fixed_bitstring<12>& subbands12() const
      {
        assert_choice_type(types::subbands12, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<12>>();
      }
      const fixed_bitstring<13>& subbands13() const
      {
        assert_choice_type(types::subbands13, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<13>>();
      }
      const fixed_bitstring<14>& subbands14() const
      {
        assert_choice_type(types::subbands14, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<14>>();
      }
      const fixed_bitstring<15>& subbands15() const
      {
        assert_choice_type(types::subbands15, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<15>>();
      }
      const fixed_bitstring<16>& subbands16() const
      {
        assert_choice_type(types::subbands16, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<16>>();
      }
      const fixed_bitstring<17>& subbands17() const
      {
        assert_choice_type(types::subbands17, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<17>>();
      }
      const fixed_bitstring<18>& subbands18() const
      {
        assert_choice_type(types::subbands18, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<18>>();
      }
      const fixed_bitstring<19>& subbands19_v1530() const
      {
        assert_choice_type(types::subbands19_v1530, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<19>>();
      }
      fixed_bitstring<3>&  set_subbands3();
      fixed_bitstring<4>&  set_subbands4();
      fixed_bitstring<5>&  set_subbands5();
      fixed_bitstring<6>&  set_subbands6();
      fixed_bitstring<7>&  set_subbands7();
      fixed_bitstring<8>&  set_subbands8();
      fixed_bitstring<9>&  set_subbands9();
      fixed_bitstring<10>& set_subbands10();
      fixed_bitstring<11>& set_subbands11();
      fixed_bitstring<12>& set_subbands12();
      fixed_bitstring<13>& set_subbands13();
      fixed_bitstring<14>& set_subbands14();
      fixed_bitstring<15>& set_subbands15();
      fixed_bitstring<16>& set_subbands16();
      fixed_bitstring<17>& set_subbands17();
      fixed_bitstring<18>& set_subbands18();
      fixed_bitstring<19>& set_subbands19_v1530();

    private:
      types                                type_;
      choice_buffer_t<fixed_bitstring<19>> c;

      void destroy_();
    };

    // member variables
    bool               cqi_format_ind_present  = false;
    bool               pmi_format_ind_present  = false;
    bool               csi_report_band_present = false;
    cqi_format_ind_e_  cqi_format_ind;
    pmi_format_ind_e_  pmi_format_ind;
    csi_report_band_c_ csi_report_band;
  };
  struct time_restrict_for_ch_meass_opts {
    enum options { cfg, not_cfg, nulltype } value;

    const char* to_string() const;
  };
  using time_restrict_for_ch_meass_e_ = enumerated<time_restrict_for_ch_meass_opts>;
  struct time_restrict_for_interference_meass_opts {
    enum options { cfg, not_cfg, nulltype } value;

    const char* to_string() const;
  };
  using time_restrict_for_interference_meass_e_ = enumerated<time_restrict_for_interference_meass_opts>;
  struct dummy_opts {
    enum options { n1, n2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using dummy_e_ = enumerated<dummy_opts>;
  struct group_based_beam_report_c_ {
    struct disabled_s_ {
      struct nrof_reported_rs_opts {
        enum options { n1, n2, n3, n4, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using nrof_reported_rs_e_ = enumerated<nrof_reported_rs_opts>;

      // member variables
      bool                nrof_reported_rs_present = false;
      nrof_reported_rs_e_ nrof_reported_rs;
    };
    struct types_opts {
      enum options { enabled, disabled, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    group_based_beam_report_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    disabled_s_& disabled()
    {
      assert_choice_type(types::disabled, type_, "groupBasedBeamReporting");
      return c;
    }
    const disabled_s_& disabled() const
    {
      assert_choice_type(types::disabled, type_, "groupBasedBeamReporting");
      return c;
    }
    void         set_enabled();
    disabled_s_& set_disabled();

  private:
    types       type_;
    disabled_s_ c;
  };
  struct cqi_table_opts {
    enum options { table1, table2, table3, table4_r17, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using cqi_table_e_ = enumerated<cqi_table_opts>;
  struct subband_size_opts {
    enum options { value1, value2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using subband_size_e_     = enumerated<subband_size_opts>;
  using non_pmi_port_ind_l_ = dyn_array<port_idx_for8_ranks_c>;
  struct semi_persistent_on_pusch_v1530_s_ {
    struct report_slot_cfg_v1530_opts {
      enum options { sl4, sl8, sl16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using report_slot_cfg_v1530_e_ = enumerated<report_slot_cfg_v1530_opts>;

    // member variables
    report_slot_cfg_v1530_e_ report_slot_cfg_v1530;
  };
  struct semi_persistent_on_pusch_v1610_s_ {
    using report_slot_offset_list_dci_0_2_r16_l_ = dyn_array<uint8_t>;
    using report_slot_offset_list_dci_0_1_r16_l_ = dyn_array<uint8_t>;

    // member variables
    report_slot_offset_list_dci_0_2_r16_l_ report_slot_offset_list_dci_0_2_r16;
    report_slot_offset_list_dci_0_1_r16_l_ report_slot_offset_list_dci_0_1_r16;
  };
  struct aperiodic_v1610_s_ {
    using report_slot_offset_list_dci_0_2_r16_l_ = dyn_array<uint8_t>;
    using report_slot_offset_list_dci_0_1_r16_l_ = dyn_array<uint8_t>;

    // member variables
    report_slot_offset_list_dci_0_2_r16_l_ report_slot_offset_list_dci_0_2_r16;
    report_slot_offset_list_dci_0_1_r16_l_ report_slot_offset_list_dci_0_1_r16;
  };
  struct report_quant_r16_c_ {
    struct types_opts {
      enum options { cri_sinr_r16, ssb_idx_sinr_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    report_quant_r16_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    void set_cri_sinr_r16();
    void set_ssb_idx_sinr_r16();

  private:
    types type_;
  };
  struct group_based_beam_report_v1710_s_ {
    struct nrof_reported_groups_r17_opts {
      enum options { n1, n2, n3, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using nrof_reported_groups_r17_e_ = enumerated<nrof_reported_groups_r17_opts>;

    // member variables
    nrof_reported_groups_r17_e_ nrof_reported_groups_r17;
  };
  struct csi_report_mode_r17_opts {
    enum options { mode1, mode2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using csi_report_mode_r17_e_ = enumerated<csi_report_mode_r17_opts>;
  struct nof_single_trp_csi_mode1_r17_opts {
    enum options { n0, n1, n2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nof_single_trp_csi_mode1_r17_e_ = enumerated<nof_single_trp_csi_mode1_r17_opts>;
  struct report_quant_r17_c_ {
    struct types_opts {
      enum options { cri_rsrp_idx_r17, ssb_idx_rsrp_idx_r17, cri_sinr_idx_r17, ssb_idx_sinr_idx_r17, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    report_quant_r17_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    void set_cri_rsrp_idx_r17();
    void set_ssb_idx_rsrp_idx_r17();
    void set_cri_sinr_idx_r17();
    void set_ssb_idx_sinr_idx_r17();

  private:
    types type_;
  };
  struct semi_persistent_on_pusch_v1720_s_ {
    using report_slot_offset_list_r17_l_         = dyn_array<uint8_t>;
    using report_slot_offset_list_dci_0_2_r17_l_ = dyn_array<uint8_t>;
    using report_slot_offset_list_dci_0_1_r17_l_ = dyn_array<uint8_t>;

    // member variables
    report_slot_offset_list_r17_l_         report_slot_offset_list_r17;
    report_slot_offset_list_dci_0_2_r17_l_ report_slot_offset_list_dci_0_2_r17;
    report_slot_offset_list_dci_0_1_r17_l_ report_slot_offset_list_dci_0_1_r17;
  };
  struct aperiodic_v1720_s_ {
    using report_slot_offset_list_r17_l_         = dyn_array<uint8_t>;
    using report_slot_offset_list_dci_0_2_r17_l_ = dyn_array<uint8_t>;
    using report_slot_offset_list_dci_0_1_r17_l_ = dyn_array<uint8_t>;

    // member variables
    report_slot_offset_list_r17_l_         report_slot_offset_list_r17;
    report_slot_offset_list_dci_0_2_r17_l_ report_slot_offset_list_dci_0_2_r17;
    report_slot_offset_list_dci_0_1_r17_l_ report_slot_offset_list_dci_0_1_r17;
  };

  // member variables
  bool                                    ext                                     = false;
  bool                                    carrier_present                         = false;
  bool                                    csi_im_res_for_interference_present     = false;
  bool                                    nzp_csi_rs_res_for_interference_present = false;
  bool                                    report_freq_cfg_present                 = false;
  bool                                    codebook_cfg_present                    = false;
  bool                                    dummy_present                           = false;
  bool                                    cqi_table_present                       = false;
  uint8_t                                 report_cfg_id                           = 0;
  uint8_t                                 carrier                                 = 0;
  uint8_t                                 res_for_ch_meas                         = 0;
  uint8_t                                 csi_im_res_for_interference             = 0;
  uint8_t                                 nzp_csi_rs_res_for_interference         = 0;
  report_cfg_type_c_                      report_cfg_type;
  report_quant_c_                         report_quant;
  report_freq_cfg_s_                      report_freq_cfg;
  time_restrict_for_ch_meass_e_           time_restrict_for_ch_meass;
  time_restrict_for_interference_meass_e_ time_restrict_for_interference_meass;
  codebook_cfg_s                          codebook_cfg;
  dummy_e_                                dummy;
  group_based_beam_report_c_              group_based_beam_report;
  cqi_table_e_                            cqi_table;
  subband_size_e_                         subband_size;
  non_pmi_port_ind_l_                     non_pmi_port_ind;
  // ...
  // group 0
  copy_ptr<semi_persistent_on_pusch_v1530_s_> semi_persistent_on_pusch_v1530;
  // group 1
  copy_ptr<semi_persistent_on_pusch_v1610_s_> semi_persistent_on_pusch_v1610;
  copy_ptr<aperiodic_v1610_s_>                aperiodic_v1610;
  copy_ptr<report_quant_r16_c_>               report_quant_r16;
  copy_ptr<codebook_cfg_r16_s>                codebook_cfg_r16;
  // group 2
  bool                                       cqi_bits_per_subband_r17_present     = false;
  bool                                       shared_cmr_r17_present               = false;
  bool                                       csi_report_mode_r17_present          = false;
  bool                                       nof_single_trp_csi_mode1_r17_present = false;
  copy_ptr<group_based_beam_report_v1710_s_> group_based_beam_report_v1710;
  copy_ptr<codebook_cfg_r17_s>               codebook_cfg_r17;
  csi_report_mode_r17_e_                     csi_report_mode_r17;
  nof_single_trp_csi_mode1_r17_e_            nof_single_trp_csi_mode1_r17;
  copy_ptr<report_quant_r17_c_>              report_quant_r17;
  // group 3
  copy_ptr<semi_persistent_on_pusch_v1720_s_> semi_persistent_on_pusch_v1720;
  copy_ptr<aperiodic_v1720_s_>                aperiodic_v1720;
  // group 4
  copy_ptr<codebook_cfg_v1730_s> codebook_cfg_v1730;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-ResourceConfig ::= SEQUENCE
struct csi_res_cfg_s {
  struct csi_rs_res_set_list_c_ {
    struct nzp_csi_rs_ssb_s_ {
      using nzp_csi_rs_res_set_list_l_ = bounded_array<uint8_t, 16>;
      using csi_ssb_res_set_list_l_    = std::array<uint8_t, 1>;

      // member variables
      bool                       csi_ssb_res_set_list_present = false;
      nzp_csi_rs_res_set_list_l_ nzp_csi_rs_res_set_list;
      csi_ssb_res_set_list_l_    csi_ssb_res_set_list;
    };
    using csi_im_res_set_list_l_ = bounded_array<uint8_t, 16>;
    struct types_opts {
      enum options { nzp_csi_rs_ssb, csi_im_res_set_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    csi_rs_res_set_list_c_() = default;
    csi_rs_res_set_list_c_(const csi_rs_res_set_list_c_& other);
    csi_rs_res_set_list_c_& operator=(const csi_rs_res_set_list_c_& other);
    ~csi_rs_res_set_list_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    nzp_csi_rs_ssb_s_& nzp_csi_rs_ssb()
    {
      assert_choice_type(types::nzp_csi_rs_ssb, type_, "csi-RS-ResourceSetList");
      return c.get<nzp_csi_rs_ssb_s_>();
    }
    csi_im_res_set_list_l_& csi_im_res_set_list()
    {
      assert_choice_type(types::csi_im_res_set_list, type_, "csi-RS-ResourceSetList");
      return c.get<csi_im_res_set_list_l_>();
    }
    const nzp_csi_rs_ssb_s_& nzp_csi_rs_ssb() const
    {
      assert_choice_type(types::nzp_csi_rs_ssb, type_, "csi-RS-ResourceSetList");
      return c.get<nzp_csi_rs_ssb_s_>();
    }
    const csi_im_res_set_list_l_& csi_im_res_set_list() const
    {
      assert_choice_type(types::csi_im_res_set_list, type_, "csi-RS-ResourceSetList");
      return c.get<csi_im_res_set_list_l_>();
    }
    nzp_csi_rs_ssb_s_&      set_nzp_csi_rs_ssb();
    csi_im_res_set_list_l_& set_csi_im_res_set_list();

  private:
    types                                                      type_;
    choice_buffer_t<csi_im_res_set_list_l_, nzp_csi_rs_ssb_s_> c;

    void destroy_();
  };
  struct res_type_opts {
    enum options { aperiodic, semi_persistent, periodic, nulltype } value;

    const char* to_string() const;
  };
  using res_type_e_ = enumerated<res_type_opts>;

  // member variables
  bool                   ext            = false;
  uint8_t                csi_res_cfg_id = 0;
  csi_rs_res_set_list_c_ csi_rs_res_set_list;
  uint8_t                bwp_id = 0;
  res_type_e_            res_type;
  // ...
  // group 0
  bool    csi_ssb_res_set_list_ext_r17_present = false;
  uint8_t csi_ssb_res_set_list_ext_r17         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-SSB-ResourceSet ::= SEQUENCE
struct csi_ssb_res_set_s {
  using csi_ssb_res_list_l_         = dyn_array<uint8_t>;
  using serving_add_pci_list_r17_l_ = dyn_array<uint8_t>;

  // member variables
  bool                ext                = false;
  uint8_t             csi_ssb_res_set_id = 0;
  csi_ssb_res_list_l_ csi_ssb_res_list;
  // ...
  // group 0
  copy_ptr<serving_add_pci_list_r17_l_> serving_add_pci_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-SemiPersistentOnPUSCH-TriggerStateList ::= SEQUENCE (SIZE (1..64)) OF CSI-SemiPersistentOnPUSCH-TriggerState
using csi_semi_persistent_on_pusch_trigger_state_list_l = dyn_array<csi_semi_persistent_on_pusch_trigger_state_s>;

// NZP-CSI-RS-Resource ::= SEQUENCE
struct nzp_csi_rs_res_s {
  struct pwr_ctrl_offset_ss_opts {
    enum options { db_neg3, db0, db3, db6, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using pwr_ctrl_offset_ss_e_ = enumerated<pwr_ctrl_offset_ss_opts>;

  // member variables
  bool                             ext                              = false;
  bool                             pwr_ctrl_offset_ss_present       = false;
  bool                             periodicity_and_offset_present   = false;
  bool                             qcl_info_periodic_csi_rs_present = false;
  uint8_t                          nzp_csi_rs_res_id                = 0;
  csi_rs_res_map_s                 res_map;
  int8_t                           pwr_ctrl_offset = -8;
  pwr_ctrl_offset_ss_e_            pwr_ctrl_offset_ss;
  uint16_t                         scrambling_id = 0;
  csi_res_periodicity_and_offset_c periodicity_and_offset;
  uint8_t                          qcl_info_periodic_csi_rs = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NZP-CSI-RS-ResourceSet ::= SEQUENCE
struct nzp_csi_rs_res_set_s {
  using nzp_csi_rs_res_l_ = dyn_array<uint8_t>;
  struct repeat_opts {
    enum options { on, off, nulltype } value;

    const char* to_string() const;
  };
  using repeat_e_ = enumerated<repeat_opts>;

  // member variables
  bool              ext                              = false;
  bool              repeat_present                   = false;
  bool              aperiodic_trigger_offset_present = false;
  bool              trs_info_present                 = false;
  uint8_t           nzp_csi_res_set_id               = 0;
  nzp_csi_rs_res_l_ nzp_csi_rs_res;
  repeat_e_         repeat;
  uint8_t           aperiodic_trigger_offset = 0;
  // ...
  // group 0
  bool    aperiodic_trigger_offset_r16_present = false;
  uint8_t aperiodic_trigger_offset_r16         = 0;
  // group 1
  bool                                     pdc_info_r17_present                    = false;
  bool                                     aperiodic_trigger_offset_r17_present    = false;
  bool                                     aperiodic_trigger_offset_l2_r17_present = false;
  copy_ptr<cmr_grouping_and_pairing_r17_s> cmr_grouping_and_pairing_r17;
  uint8_t                                  aperiodic_trigger_offset_r17    = 0;
  uint8_t                                  aperiodic_trigger_offset_l2_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCellActivationRS-Config-r17 ::= SEQUENCE
struct scell_activation_rs_cfg_r17_s {
  bool     ext                            = false;
  bool     gap_between_bursts_r17_present = false;
  uint16_t scell_activation_rs_id_r17     = 1;
  uint8_t  res_set_r17                    = 0;
  uint8_t  gap_between_bursts_r17         = 2;
  uint8_t  qcl_info_r17                   = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-MeasConfig ::= SEQUENCE
struct csi_meas_cfg_s {
  using nzp_csi_rs_res_to_add_mod_list_l_              = dyn_array<nzp_csi_rs_res_s>;
  using nzp_csi_rs_res_to_release_list_l_              = dyn_array<uint8_t>;
  using nzp_csi_rs_res_set_to_add_mod_list_l_          = dyn_array<nzp_csi_rs_res_set_s>;
  using nzp_csi_rs_res_set_to_release_list_l_          = dyn_array<uint8_t>;
  using csi_im_res_to_add_mod_list_l_                  = dyn_array<csi_im_res_s>;
  using csi_im_res_to_release_list_l_                  = bounded_array<uint8_t, 32>;
  using csi_im_res_set_to_add_mod_list_l_              = dyn_array<csi_im_res_set_s>;
  using csi_im_res_set_to_release_list_l_              = dyn_array<uint8_t>;
  using csi_ssb_res_set_to_add_mod_list_l_             = dyn_array<csi_ssb_res_set_s>;
  using csi_ssb_res_set_to_release_list_l_             = dyn_array<uint8_t>;
  using csi_res_cfg_to_add_mod_list_l_                 = dyn_array<csi_res_cfg_s>;
  using csi_res_cfg_to_release_list_l_                 = dyn_array<uint8_t>;
  using csi_report_cfg_to_add_mod_list_l_              = dyn_array<csi_report_cfg_s>;
  using csi_report_cfg_to_release_list_l_              = dyn_array<uint8_t>;
  using scell_activation_rs_cfg_to_add_mod_list_r17_l_ = dyn_array<scell_activation_rs_cfg_r17_s>;
  using scell_activation_rs_cfg_to_release_list_r17_l_ = dyn_array<uint16_t>;

  // member variables
  bool                                  ext                                                 = false;
  bool                                  report_trigger_size_present                         = false;
  bool                                  aperiodic_trigger_state_list_present                = false;
  bool                                  semi_persistent_on_pusch_trigger_state_list_present = false;
  nzp_csi_rs_res_to_add_mod_list_l_     nzp_csi_rs_res_to_add_mod_list;
  nzp_csi_rs_res_to_release_list_l_     nzp_csi_rs_res_to_release_list;
  nzp_csi_rs_res_set_to_add_mod_list_l_ nzp_csi_rs_res_set_to_add_mod_list;
  nzp_csi_rs_res_set_to_release_list_l_ nzp_csi_rs_res_set_to_release_list;
  csi_im_res_to_add_mod_list_l_         csi_im_res_to_add_mod_list;
  csi_im_res_to_release_list_l_         csi_im_res_to_release_list;
  csi_im_res_set_to_add_mod_list_l_     csi_im_res_set_to_add_mod_list;
  csi_im_res_set_to_release_list_l_     csi_im_res_set_to_release_list;
  csi_ssb_res_set_to_add_mod_list_l_    csi_ssb_res_set_to_add_mod_list;
  csi_ssb_res_set_to_release_list_l_    csi_ssb_res_set_to_release_list;
  csi_res_cfg_to_add_mod_list_l_        csi_res_cfg_to_add_mod_list;
  csi_res_cfg_to_release_list_l_        csi_res_cfg_to_release_list;
  csi_report_cfg_to_add_mod_list_l_     csi_report_cfg_to_add_mod_list;
  csi_report_cfg_to_release_list_l_     csi_report_cfg_to_release_list;
  uint8_t                               report_trigger_size = 0;
  setup_release_c<dyn_seq_of<csi_aperiodic_trigger_state_s, 1, 128>> aperiodic_trigger_state_list;
  setup_release_c<dyn_seq_of<csi_semi_persistent_on_pusch_trigger_state_s, 1, 64>>
      semi_persistent_on_pusch_trigger_state_list;
  // ...
  // group 0
  bool    report_trigger_size_dci_0_2_r16_present = false;
  uint8_t report_trigger_size_dci_0_2_r16         = 0;
  // group 1
  copy_ptr<scell_activation_rs_cfg_to_add_mod_list_r17_l_> scell_activation_rs_cfg_to_add_mod_list_r17;
  copy_ptr<scell_activation_rs_cfg_to_release_list_r17_l_> scell_activation_rs_cfg_to_release_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-PRS-QCL-Info-r17 ::= CHOICE
struct dl_prs_qcl_info_r17_c {
  struct ssb_r17_s_ {
    struct rs_type_r17_opts {
      enum options { type_c, type_d, type_c_plus_type_d, nulltype } value;

      const char* to_string() const;
    };
    using rs_type_r17_e_ = enumerated<rs_type_r17_opts>;

    // member variables
    bool           ext         = false;
    uint8_t        ssb_idx_r17 = 0;
    rs_type_r17_e_ rs_type_r17;
    // ...
  };
  struct dl_prs_r17_s_ {
    bool    ext                   = false;
    uint8_t qcl_dl_prs_res_id_r17 = 0;
    // ...
  };
  struct types_opts {
    enum options { ssb_r17, dl_prs_r17, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  dl_prs_qcl_info_r17_c() = default;
  dl_prs_qcl_info_r17_c(const dl_prs_qcl_info_r17_c& other);
  dl_prs_qcl_info_r17_c& operator=(const dl_prs_qcl_info_r17_c& other);
  ~dl_prs_qcl_info_r17_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  ssb_r17_s_& ssb_r17()
  {
    assert_choice_type(types::ssb_r17, type_, "DL-PRS-QCL-Info-r17");
    return c.get<ssb_r17_s_>();
  }
  dl_prs_r17_s_& dl_prs_r17()
  {
    assert_choice_type(types::dl_prs_r17, type_, "DL-PRS-QCL-Info-r17");
    return c.get<dl_prs_r17_s_>();
  }
  const ssb_r17_s_& ssb_r17() const
  {
    assert_choice_type(types::ssb_r17, type_, "DL-PRS-QCL-Info-r17");
    return c.get<ssb_r17_s_>();
  }
  const dl_prs_r17_s_& dl_prs_r17() const
  {
    assert_choice_type(types::dl_prs_r17, type_, "DL-PRS-QCL-Info-r17");
    return c.get<dl_prs_r17_s_>();
  }
  ssb_r17_s_&    set_ssb_r17();
  dl_prs_r17_s_& set_dl_prs_r17();

private:
  types                                      type_;
  choice_buffer_t<dl_prs_r17_s_, ssb_r17_s_> c;

  void destroy_();
};

// SRS-CC-SetIndex ::= SEQUENCE
struct srs_cc_set_idx_s {
  bool    cc_set_idx_present           = false;
  bool    cc_idx_in_one_cc_set_present = false;
  uint8_t cc_set_idx                   = 0;
  uint8_t cc_idx_in_one_cc_set         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SlotFormatCombination ::= SEQUENCE
struct slot_format_combination_s {
  using slot_formats_l_ = dyn_array<uint16_t>;

  // member variables
  uint16_t        slot_format_combination_id = 0;
  slot_formats_l_ slot_formats;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17 ::= CHOICE
struct nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c {
  struct scs15_r17_c_ {
    struct types_opts {
      enum options {
        n4_r17,
        n5_r17,
        n8_r17,
        n10_r17,
        n16_r17,
        n20_r17,
        n32_r17,
        n40_r17,
        n64_r17,
        n80_r17,
        n160_r17,
        n320_r17,
        n640_r17,
        n1280_r17,
        n2560_r17,
        n5120_r17,
        n10240_r17,
        // ...
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    scs15_r17_c_() = default;
    scs15_r17_c_(const scs15_r17_c_& other);
    scs15_r17_c_& operator=(const scs15_r17_c_& other);
    ~scs15_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& n4_r17()
    {
      assert_choice_type(types::n4_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n5_r17()
    {
      assert_choice_type(types::n5_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n8_r17()
    {
      assert_choice_type(types::n8_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n10_r17()
    {
      assert_choice_type(types::n10_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n16_r17()
    {
      assert_choice_type(types::n16_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n20_r17()
    {
      assert_choice_type(types::n20_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n32_r17()
    {
      assert_choice_type(types::n32_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n40_r17()
    {
      assert_choice_type(types::n40_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n64_r17()
    {
      assert_choice_type(types::n64_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n80_r17()
    {
      assert_choice_type(types::n80_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n160_r17()
    {
      assert_choice_type(types::n160_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint16_t& n320_r17()
    {
      assert_choice_type(types::n320_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n640_r17()
    {
      assert_choice_type(types::n640_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n1280_r17()
    {
      assert_choice_type(types::n1280_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n2560_r17()
    {
      assert_choice_type(types::n2560_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n5120_r17()
    {
      assert_choice_type(types::n5120_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n10240_r17()
    {
      assert_choice_type(types::n10240_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    const uint8_t& n4_r17() const
    {
      assert_choice_type(types::n4_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n5_r17() const
    {
      assert_choice_type(types::n5_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n8_r17() const
    {
      assert_choice_type(types::n8_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n10_r17() const
    {
      assert_choice_type(types::n10_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n16_r17() const
    {
      assert_choice_type(types::n16_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n20_r17() const
    {
      assert_choice_type(types::n20_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n32_r17() const
    {
      assert_choice_type(types::n32_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n40_r17() const
    {
      assert_choice_type(types::n40_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n64_r17() const
    {
      assert_choice_type(types::n64_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n80_r17() const
    {
      assert_choice_type(types::n80_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n160_r17() const
    {
      assert_choice_type(types::n160_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint16_t& n320_r17() const
    {
      assert_choice_type(types::n320_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n640_r17() const
    {
      assert_choice_type(types::n640_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n1280_r17() const
    {
      assert_choice_type(types::n1280_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n2560_r17() const
    {
      assert_choice_type(types::n2560_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n5120_r17() const
    {
      assert_choice_type(types::n5120_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n10240_r17() const
    {
      assert_choice_type(types::n10240_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    uint8_t&  set_n4_r17();
    uint8_t&  set_n5_r17();
    uint8_t&  set_n8_r17();
    uint8_t&  set_n10_r17();
    uint8_t&  set_n16_r17();
    uint8_t&  set_n20_r17();
    uint8_t&  set_n32_r17();
    uint8_t&  set_n40_r17();
    uint8_t&  set_n64_r17();
    uint8_t&  set_n80_r17();
    uint8_t&  set_n160_r17();
    uint16_t& set_n320_r17();
    uint16_t& set_n640_r17();
    uint16_t& set_n1280_r17();
    uint16_t& set_n2560_r17();
    uint16_t& set_n5120_r17();
    uint16_t& set_n10240_r17();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct scs30_r17_c_ {
    struct types_opts {
      enum options {
        n8_r17,
        n10_r17,
        n16_r17,
        n20_r17,
        n32_r17,
        n40_r17,
        n64_r17,
        n80_r17,
        n128_r17,
        n160_r17,
        n320_r17,
        n640_r17,
        n1280_r17,
        n2560_r17,
        n5120_r17,
        n10240_r17,
        n20480_r17,
        // ...
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    scs30_r17_c_() = default;
    scs30_r17_c_(const scs30_r17_c_& other);
    scs30_r17_c_& operator=(const scs30_r17_c_& other);
    ~scs30_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& n8_r17()
    {
      assert_choice_type(types::n8_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n10_r17()
    {
      assert_choice_type(types::n10_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n16_r17()
    {
      assert_choice_type(types::n16_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n20_r17()
    {
      assert_choice_type(types::n20_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n32_r17()
    {
      assert_choice_type(types::n32_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n40_r17()
    {
      assert_choice_type(types::n40_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n64_r17()
    {
      assert_choice_type(types::n64_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n80_r17()
    {
      assert_choice_type(types::n80_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n128_r17()
    {
      assert_choice_type(types::n128_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n160_r17()
    {
      assert_choice_type(types::n160_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint16_t& n320_r17()
    {
      assert_choice_type(types::n320_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n640_r17()
    {
      assert_choice_type(types::n640_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n1280_r17()
    {
      assert_choice_type(types::n1280_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n2560_r17()
    {
      assert_choice_type(types::n2560_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n5120_r17()
    {
      assert_choice_type(types::n5120_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n10240_r17()
    {
      assert_choice_type(types::n10240_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n20480_r17()
    {
      assert_choice_type(types::n20480_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    const uint8_t& n8_r17() const
    {
      assert_choice_type(types::n8_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n10_r17() const
    {
      assert_choice_type(types::n10_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n16_r17() const
    {
      assert_choice_type(types::n16_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n20_r17() const
    {
      assert_choice_type(types::n20_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n32_r17() const
    {
      assert_choice_type(types::n32_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n40_r17() const
    {
      assert_choice_type(types::n40_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n64_r17() const
    {
      assert_choice_type(types::n64_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n80_r17() const
    {
      assert_choice_type(types::n80_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n128_r17() const
    {
      assert_choice_type(types::n128_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n160_r17() const
    {
      assert_choice_type(types::n160_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint16_t& n320_r17() const
    {
      assert_choice_type(types::n320_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n640_r17() const
    {
      assert_choice_type(types::n640_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n1280_r17() const
    {
      assert_choice_type(types::n1280_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n2560_r17() const
    {
      assert_choice_type(types::n2560_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n5120_r17() const
    {
      assert_choice_type(types::n5120_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n10240_r17() const
    {
      assert_choice_type(types::n10240_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n20480_r17() const
    {
      assert_choice_type(types::n20480_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    uint8_t&  set_n8_r17();
    uint8_t&  set_n10_r17();
    uint8_t&  set_n16_r17();
    uint8_t&  set_n20_r17();
    uint8_t&  set_n32_r17();
    uint8_t&  set_n40_r17();
    uint8_t&  set_n64_r17();
    uint8_t&  set_n80_r17();
    uint8_t&  set_n128_r17();
    uint8_t&  set_n160_r17();
    uint16_t& set_n320_r17();
    uint16_t& set_n640_r17();
    uint16_t& set_n1280_r17();
    uint16_t& set_n2560_r17();
    uint16_t& set_n5120_r17();
    uint16_t& set_n10240_r17();
    uint16_t& set_n20480_r17();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct scs60_r17_c_ {
    struct types_opts {
      enum options {
        n16_r17,
        n20_r17,
        n32_r17,
        n40_r17,
        n64_r17,
        n80_r17,
        n128_r17,
        n160_r17,
        n256_r17,
        n320_r17,
        n640_r17,
        n1280_r17,
        n2560_r17,
        n5120_r17,
        n10240_r17,
        n20480_r17,
        n40960_r17,
        // ...
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    scs60_r17_c_() = default;
    scs60_r17_c_(const scs60_r17_c_& other);
    scs60_r17_c_& operator=(const scs60_r17_c_& other);
    ~scs60_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& n16_r17()
    {
      assert_choice_type(types::n16_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n20_r17()
    {
      assert_choice_type(types::n20_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n32_r17()
    {
      assert_choice_type(types::n32_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n40_r17()
    {
      assert_choice_type(types::n40_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n64_r17()
    {
      assert_choice_type(types::n64_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n80_r17()
    {
      assert_choice_type(types::n80_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n128_r17()
    {
      assert_choice_type(types::n128_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n160_r17()
    {
      assert_choice_type(types::n160_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    uint16_t& n256_r17()
    {
      assert_choice_type(types::n256_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n320_r17()
    {
      assert_choice_type(types::n320_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n640_r17()
    {
      assert_choice_type(types::n640_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n1280_r17()
    {
      assert_choice_type(types::n1280_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n2560_r17()
    {
      assert_choice_type(types::n2560_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n5120_r17()
    {
      assert_choice_type(types::n5120_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n10240_r17()
    {
      assert_choice_type(types::n10240_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n20480_r17()
    {
      assert_choice_type(types::n20480_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n40960_r17()
    {
      assert_choice_type(types::n40960_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    const uint8_t& n16_r17() const
    {
      assert_choice_type(types::n16_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n20_r17() const
    {
      assert_choice_type(types::n20_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n32_r17() const
    {
      assert_choice_type(types::n32_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n40_r17() const
    {
      assert_choice_type(types::n40_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n64_r17() const
    {
      assert_choice_type(types::n64_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n80_r17() const
    {
      assert_choice_type(types::n80_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n128_r17() const
    {
      assert_choice_type(types::n128_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n160_r17() const
    {
      assert_choice_type(types::n160_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    const uint16_t& n256_r17() const
    {
      assert_choice_type(types::n256_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n320_r17() const
    {
      assert_choice_type(types::n320_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n640_r17() const
    {
      assert_choice_type(types::n640_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n1280_r17() const
    {
      assert_choice_type(types::n1280_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n2560_r17() const
    {
      assert_choice_type(types::n2560_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n5120_r17() const
    {
      assert_choice_type(types::n5120_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n10240_r17() const
    {
      assert_choice_type(types::n10240_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n20480_r17() const
    {
      assert_choice_type(types::n20480_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n40960_r17() const
    {
      assert_choice_type(types::n40960_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    uint8_t&  set_n16_r17();
    uint8_t&  set_n20_r17();
    uint8_t&  set_n32_r17();
    uint8_t&  set_n40_r17();
    uint8_t&  set_n64_r17();
    uint8_t&  set_n80_r17();
    uint8_t&  set_n128_r17();
    uint8_t&  set_n160_r17();
    uint16_t& set_n256_r17();
    uint16_t& set_n320_r17();
    uint16_t& set_n640_r17();
    uint16_t& set_n1280_r17();
    uint16_t& set_n2560_r17();
    uint16_t& set_n5120_r17();
    uint16_t& set_n10240_r17();
    uint16_t& set_n20480_r17();
    uint16_t& set_n40960_r17();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct scs120_r17_c_ {
    struct types_opts {
      enum options {
        n32_r17,
        n40_r17,
        n64_r17,
        n80_r17,
        n128_r17,
        n160_r17,
        n256_r17,
        n320_r17,
        n512_r17,
        n640_r17,
        n1280_r17,
        n2560_r17,
        n5120_r17,
        n10240_r17,
        n20480_r17,
        n40960_r17,
        n81920_r17,
        // ...
        nulltype
      } value;
      typedef uint32_t number_type;

      const char* to_string() const;
      uint32_t    to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    scs120_r17_c_() = default;
    scs120_r17_c_(const scs120_r17_c_& other);
    scs120_r17_c_& operator=(const scs120_r17_c_& other);
    ~scs120_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& n32_r17()
    {
      assert_choice_type(types::n32_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n40_r17()
    {
      assert_choice_type(types::n40_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n64_r17()
    {
      assert_choice_type(types::n64_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n80_r17()
    {
      assert_choice_type(types::n80_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n128_r17()
    {
      assert_choice_type(types::n128_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n160_r17()
    {
      assert_choice_type(types::n160_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    uint16_t& n256_r17()
    {
      assert_choice_type(types::n256_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n320_r17()
    {
      assert_choice_type(types::n320_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n512_r17()
    {
      assert_choice_type(types::n512_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n640_r17()
    {
      assert_choice_type(types::n640_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n1280_r17()
    {
      assert_choice_type(types::n1280_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n2560_r17()
    {
      assert_choice_type(types::n2560_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n5120_r17()
    {
      assert_choice_type(types::n5120_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n10240_r17()
    {
      assert_choice_type(types::n10240_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n20480_r17()
    {
      assert_choice_type(types::n20480_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n40960_r17()
    {
      assert_choice_type(types::n40960_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint32_t& n81920_r17()
    {
      assert_choice_type(types::n81920_r17, type_, "scs120-r17");
      return c.get<uint32_t>();
    }
    const uint8_t& n32_r17() const
    {
      assert_choice_type(types::n32_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n40_r17() const
    {
      assert_choice_type(types::n40_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n64_r17() const
    {
      assert_choice_type(types::n64_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n80_r17() const
    {
      assert_choice_type(types::n80_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n128_r17() const
    {
      assert_choice_type(types::n128_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n160_r17() const
    {
      assert_choice_type(types::n160_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    const uint16_t& n256_r17() const
    {
      assert_choice_type(types::n256_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n320_r17() const
    {
      assert_choice_type(types::n320_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n512_r17() const
    {
      assert_choice_type(types::n512_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n640_r17() const
    {
      assert_choice_type(types::n640_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n1280_r17() const
    {
      assert_choice_type(types::n1280_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n2560_r17() const
    {
      assert_choice_type(types::n2560_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n5120_r17() const
    {
      assert_choice_type(types::n5120_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n10240_r17() const
    {
      assert_choice_type(types::n10240_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n20480_r17() const
    {
      assert_choice_type(types::n20480_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n40960_r17() const
    {
      assert_choice_type(types::n40960_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint32_t& n81920_r17() const
    {
      assert_choice_type(types::n81920_r17, type_, "scs120-r17");
      return c.get<uint32_t>();
    }
    uint8_t&  set_n32_r17();
    uint8_t&  set_n40_r17();
    uint8_t&  set_n64_r17();
    uint8_t&  set_n80_r17();
    uint8_t&  set_n128_r17();
    uint8_t&  set_n160_r17();
    uint16_t& set_n256_r17();
    uint16_t& set_n320_r17();
    uint16_t& set_n512_r17();
    uint16_t& set_n640_r17();
    uint16_t& set_n1280_r17();
    uint16_t& set_n2560_r17();
    uint16_t& set_n5120_r17();
    uint16_t& set_n10240_r17();
    uint16_t& set_n20480_r17();
    uint16_t& set_n40960_r17();
    uint32_t& set_n81920_r17();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct types_opts {
    enum options { scs15_r17, scs30_r17, scs60_r17, scs120_r17, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c() = default;
  nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c(const nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c& other);
  nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c&
  operator=(const nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c& other);
  ~nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  scs15_r17_c_& scs15_r17()
  {
    assert_choice_type(types::scs15_r17, type_, "NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17");
    return c.get<scs15_r17_c_>();
  }
  scs30_r17_c_& scs30_r17()
  {
    assert_choice_type(types::scs30_r17, type_, "NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17");
    return c.get<scs30_r17_c_>();
  }
  scs60_r17_c_& scs60_r17()
  {
    assert_choice_type(types::scs60_r17, type_, "NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17");
    return c.get<scs60_r17_c_>();
  }
  scs120_r17_c_& scs120_r17()
  {
    assert_choice_type(types::scs120_r17, type_, "NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17");
    return c.get<scs120_r17_c_>();
  }
  const scs15_r17_c_& scs15_r17() const
  {
    assert_choice_type(types::scs15_r17, type_, "NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17");
    return c.get<scs15_r17_c_>();
  }
  const scs30_r17_c_& scs30_r17() const
  {
    assert_choice_type(types::scs30_r17, type_, "NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17");
    return c.get<scs30_r17_c_>();
  }
  const scs60_r17_c_& scs60_r17() const
  {
    assert_choice_type(types::scs60_r17, type_, "NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17");
    return c.get<scs60_r17_c_>();
  }
  const scs120_r17_c_& scs120_r17() const
  {
    assert_choice_type(types::scs120_r17, type_, "NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17");
    return c.get<scs120_r17_c_>();
  }
  scs15_r17_c_&  set_scs15_r17();
  scs30_r17_c_&  set_scs30_r17();
  scs60_r17_c_&  set_scs60_r17();
  scs120_r17_c_& set_scs120_r17();

private:
  types                                                                    type_;
  choice_buffer_t<scs120_r17_c_, scs15_r17_c_, scs30_r17_c_, scs60_r17_c_> c;

  void destroy_();
};

// NR-DL-PRS-Resource-r17 ::= SEQUENCE
struct nr_dl_prs_res_r17_s {
  struct dl_prs_comb_size_n_and_re_offset_r17_c_ {
    struct types_opts {
      enum options { n2_r17, n4_r17, n6_r17, n12_r17, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    dl_prs_comb_size_n_and_re_offset_r17_c_() = default;
    dl_prs_comb_size_n_and_re_offset_r17_c_(const dl_prs_comb_size_n_and_re_offset_r17_c_& other);
    dl_prs_comb_size_n_and_re_offset_r17_c_& operator=(const dl_prs_comb_size_n_and_re_offset_r17_c_& other);
    ~dl_prs_comb_size_n_and_re_offset_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& n2_r17()
    {
      assert_choice_type(types::n2_r17, type_, "dl-PRS-CombSizeN-AndReOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n4_r17()
    {
      assert_choice_type(types::n4_r17, type_, "dl-PRS-CombSizeN-AndReOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n6_r17()
    {
      assert_choice_type(types::n6_r17, type_, "dl-PRS-CombSizeN-AndReOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n12_r17()
    {
      assert_choice_type(types::n12_r17, type_, "dl-PRS-CombSizeN-AndReOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n2_r17() const
    {
      assert_choice_type(types::n2_r17, type_, "dl-PRS-CombSizeN-AndReOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n4_r17() const
    {
      assert_choice_type(types::n4_r17, type_, "dl-PRS-CombSizeN-AndReOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n6_r17() const
    {
      assert_choice_type(types::n6_r17, type_, "dl-PRS-CombSizeN-AndReOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n12_r17() const
    {
      assert_choice_type(types::n12_r17, type_, "dl-PRS-CombSizeN-AndReOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& set_n2_r17();
    uint8_t& set_n4_r17();
    uint8_t& set_n6_r17();
    uint8_t& set_n12_r17();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                                    ext                         = false;
  bool                                    dl_prs_qcl_info_r17_present = false;
  uint8_t                                 nr_dl_prs_res_id_r17        = 0;
  uint16_t                                dl_prs_seq_id_r17           = 0;
  dl_prs_comb_size_n_and_re_offset_r17_c_ dl_prs_comb_size_n_and_re_offset_r17;
  uint16_t                                dl_prs_res_slot_offset_r17   = 0;
  uint8_t                                 dl_prs_res_symbol_offset_r17 = 0;
  dl_prs_qcl_info_r17_c                   dl_prs_qcl_info_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// P0AlphaSet-r17 ::= SEQUENCE
struct p0_alpha_set_r17_s {
  struct closed_loop_idx_r17_opts {
    enum options { i0, i1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using closed_loop_idx_r17_e_ = enumerated<closed_loop_idx_r17_opts>;

  // member variables
  bool                   p0_r17_present    = false;
  bool                   alpha_r17_present = false;
  int8_t                 p0_r17            = -16;
  alpha_e                alpha_r17;
  closed_loop_idx_r17_e_ closed_loop_idx_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-CodeBlockGroupTransmission ::= SEQUENCE
struct pdsch_code_block_group_tx_s {
  struct max_code_block_groups_per_transport_block_opts {
    enum options { n2, n4, n6, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_code_block_groups_per_transport_block_e_ = enumerated<max_code_block_groups_per_transport_block_opts>;

  // member variables
  bool                                         ext = false;
  max_code_block_groups_per_transport_block_e_ max_code_block_groups_per_transport_block;
  bool                                         code_block_group_flush_ind = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-CodeBlockGroupTransmission ::= SEQUENCE
struct pusch_code_block_group_tx_s {
  struct max_code_block_groups_per_transport_block_opts {
    enum options { n2, n4, n6, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_code_block_groups_per_transport_block_e_ = enumerated<max_code_block_groups_per_transport_block_opts>;

  // member variables
  bool                                         ext = false;
  max_code_block_groups_per_transport_block_e_ max_code_block_groups_per_transport_block;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RepFactorAndTimeGap-r17 ::= SEQUENCE
struct rep_factor_and_time_gap_r17_s {
  struct repeat_factor_r17_opts {
    enum options { n2, n4, n6, n8, n16, n32, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using repeat_factor_r17_e_ = enumerated<repeat_factor_r17_opts>;
  struct time_gap_r17_opts {
    enum options { s1, s2, s4, s8, s16, s32, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using time_gap_r17_e_ = enumerated<time_gap_r17_opts>;

  // member variables
  repeat_factor_r17_e_ repeat_factor_r17;
  time_gap_r17_e_      time_gap_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-TPC-PDCCH-Config ::= SEQUENCE
struct srs_tpc_pdcch_cfg_s {
  using srs_cc_set_idxlist_l_ = dyn_array<srs_cc_set_idx_s>;

  // member variables
  srs_cc_set_idxlist_l_ srs_cc_set_idxlist;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SearchSpaceSwitchTrigger-r16 ::= SEQUENCE
struct search_space_switch_trigger_r16_s {
  uint8_t serving_cell_id_r16 = 0;
  uint8_t position_in_dci_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SlotFormatCombinationsPerCell ::= SEQUENCE
struct slot_format_combinations_per_cell_s {
  using slot_format_combinations_l_ = dyn_array<slot_format_combination_s>;

  // member variables
  bool                        ext                         = false;
  bool                        subcarrier_spacing2_present = false;
  bool                        position_in_dci_present     = false;
  uint8_t                     serving_cell_id             = 0;
  subcarrier_spacing_e        subcarrier_spacing;
  subcarrier_spacing_e        subcarrier_spacing2;
  slot_format_combinations_l_ slot_format_combinations;
  uint8_t                     position_in_dci = 0;
  // ...
  // group 0
  bool enable_cfg_ul_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GuardBand-r16 ::= SEQUENCE
struct guard_band_r16_s {
  uint16_t start_crb_r16  = 0;
  uint8_t  nrof_c_rbs_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LTE-NeighCellsCRS-AssistInfo-r17 ::= SEQUENCE
struct lte_neigh_cells_crs_assist_info_r17_s {
  struct neigh_carrier_bw_dl_r17_opts {
    enum options { n6, n15, n25, n50, n75, n100, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using neigh_carrier_bw_dl_r17_e_ = enumerated<neigh_carrier_bw_dl_r17_opts>;
  struct neigh_nrof_crs_ports_r17_opts {
    enum options { n1, n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using neigh_nrof_crs_ports_r17_e_ = enumerated<neigh_nrof_crs_ports_r17_opts>;
  struct neigh_v_shift_r17_opts {
    enum options { n0, n1, n2, n3, n4, n5, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using neigh_v_shift_r17_e_ = enumerated<neigh_v_shift_r17_opts>;

  // member variables
  bool                        neigh_carrier_bw_dl_r17_present   = false;
  bool                        neigh_carrier_freq_dl_r17_present = false;
  bool                        neigh_cell_id_r17_present         = false;
  bool                        neigh_crs_muting_r17_present      = false;
  bool                        neigh_nrof_crs_ports_r17_present  = false;
  bool                        neigh_v_shift_r17_present         = false;
  neigh_carrier_bw_dl_r17_e_  neigh_carrier_bw_dl_r17;
  uint16_t                    neigh_carrier_freq_dl_r17 = 0;
  uint16_t                    neigh_cell_id_r17         = 0;
  eutra_mbsfn_sf_cfg_list_l   neigh_mbsfn_sf_cfg_list_r17;
  neigh_nrof_crs_ports_r17_e_ neigh_nrof_crs_ports_r17;
  neigh_v_shift_r17_e_        neigh_v_shift_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-DL-PRS-PDC-ResourceSet-r17 ::= SEQUENCE
struct nr_dl_prs_pdc_res_set_r17_s {
  struct num_symbols_r17_opts {
    enum options { n2, n4, n6, n12, spare4, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using num_symbols_r17_e_ = enumerated<num_symbols_r17_opts>;
  using res_list_r17_l_    = dyn_array<nr_dl_prs_res_r17_s>;

  // member variables
  bool                                                ext                                 = false;
  bool                                                rep_factor_and_time_gap_r17_present = false;
  nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c periodicity_and_offset_r17;
  num_symbols_r17_e_                                  num_symbols_r17;
  uint8_t                                             dl_prs_res_bw_r17    = 1;
  uint16_t                                            dl_prs_start_prb_r17 = 0;
  res_list_r17_l_                                     res_list_r17;
  rep_factor_and_time_gap_r17_s                       rep_factor_and_time_gap_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OutsideActiveTimeConfig-r16 ::= SEQUENCE
struct outside_active_time_cfg_r16_s {
  bool    first_outside_active_time_bwp_id_r16_present   = false;
  bool    dormancy_group_outside_active_time_r16_present = false;
  uint8_t first_outside_active_time_bwp_id_r16           = 0;
  uint8_t dormancy_group_outside_active_time_r16         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-CodeBlockGroupTransmissionList-r16 ::= SEQUENCE (SIZE (1..2)) OF PDSCH-CodeBlockGroupTransmission
using pdsch_code_block_group_tx_list_r16_l = dyn_array<pdsch_code_block_group_tx_s>;

// PUSCH-ServingCellConfig ::= SEQUENCE
struct pusch_serving_cell_cfg_s {
  struct xoverhead_opts {
    enum options { xoh6, xoh12, xoh18, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using xoverhead_e_ = enumerated<xoverhead_opts>;

  // member variables
  bool                                         ext                         = false;
  bool                                         code_block_group_tx_present = false;
  bool                                         rate_matching_present       = false;
  bool                                         xoverhead_present           = false;
  setup_release_c<pusch_code_block_group_tx_s> code_block_group_tx;
  xoverhead_e_                                 xoverhead;
  // ...
  // group 0
  bool    max_mimo_layers_present          = false;
  bool    processing_type2_enabled_present = false;
  uint8_t max_mimo_layers                  = 1;
  bool    processing_type2_enabled         = false;
  // group 1
  copy_ptr<setup_release_c<integer<uint8_t, 1, 4>>> max_mimo_layers_dci_0_2_r16;
  // group 2
  bool                                           nrof_harq_processes_for_pusch_r17_present = false;
  copy_ptr<setup_release_c<fixed_bitstring<32>>> ul_harq_mode_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-CarrierSwitching ::= SEQUENCE
struct srs_carrier_switching_s {
  struct srs_switch_from_carrier_opts {
    enum options { sul, nul, nulltype } value;

    const char* to_string() const;
  };
  using srs_switch_from_carrier_e_ = enumerated<srs_switch_from_carrier_opts>;
  struct srs_tpc_pdcch_group_c_ {
    using type_a_l_ = dyn_array<srs_tpc_pdcch_cfg_s>;
    struct types_opts {
      enum options { type_a, type_b, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    srs_tpc_pdcch_group_c_() = default;
    srs_tpc_pdcch_group_c_(const srs_tpc_pdcch_group_c_& other);
    srs_tpc_pdcch_group_c_& operator=(const srs_tpc_pdcch_group_c_& other);
    ~srs_tpc_pdcch_group_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    type_a_l_& type_a()
    {
      assert_choice_type(types::type_a, type_, "srs-TPC-PDCCH-Group");
      return c.get<type_a_l_>();
    }
    srs_tpc_pdcch_cfg_s& type_b()
    {
      assert_choice_type(types::type_b, type_, "srs-TPC-PDCCH-Group");
      return c.get<srs_tpc_pdcch_cfg_s>();
    }
    const type_a_l_& type_a() const
    {
      assert_choice_type(types::type_a, type_, "srs-TPC-PDCCH-Group");
      return c.get<type_a_l_>();
    }
    const srs_tpc_pdcch_cfg_s& type_b() const
    {
      assert_choice_type(types::type_b, type_, "srs-TPC-PDCCH-Group");
      return c.get<srs_tpc_pdcch_cfg_s>();
    }
    type_a_l_&           set_type_a();
    srs_tpc_pdcch_cfg_s& set_type_b();

  private:
    types                                           type_;
    choice_buffer_t<srs_tpc_pdcch_cfg_s, type_a_l_> c;

    void destroy_();
  };
  using monitoring_cells_l_ = bounded_array<uint8_t, 32>;

  // member variables
  bool                       ext                                   = false;
  bool                       srs_switch_from_serv_cell_idx_present = false;
  bool                       srs_tpc_pdcch_group_present           = false;
  uint8_t                    srs_switch_from_serv_cell_idx         = 0;
  srs_switch_from_carrier_e_ srs_switch_from_carrier;
  srs_tpc_pdcch_group_c_     srs_tpc_pdcch_group;
  monitoring_cells_l_        monitoring_cells;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SSB-MTC-AdditionalPCI-r17 ::= SEQUENCE
struct ssb_mtc_add_pci_r17_s {
  struct periodicity_r17_opts {
    enum options { ms5, ms10, ms20, ms40, ms80, ms160, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using periodicity_r17_e_ = enumerated<periodicity_r17_opts>;
  struct ssb_positions_in_burst_r17_c_ {
    struct types_opts {
      enum options { short_bitmap, medium_bitmap, long_bitmap, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ssb_positions_in_burst_r17_c_() = default;
    ssb_positions_in_burst_r17_c_(const ssb_positions_in_burst_r17_c_& other);
    ssb_positions_in_burst_r17_c_& operator=(const ssb_positions_in_burst_r17_c_& other);
    ~ssb_positions_in_burst_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<4>& short_bitmap()
    {
      assert_choice_type(types::short_bitmap, type_, "ssb-PositionsInBurst-r17");
      return c.get<fixed_bitstring<4>>();
    }
    fixed_bitstring<8>& medium_bitmap()
    {
      assert_choice_type(types::medium_bitmap, type_, "ssb-PositionsInBurst-r17");
      return c.get<fixed_bitstring<8>>();
    }
    fixed_bitstring<64>& long_bitmap()
    {
      assert_choice_type(types::long_bitmap, type_, "ssb-PositionsInBurst-r17");
      return c.get<fixed_bitstring<64>>();
    }
    const fixed_bitstring<4>& short_bitmap() const
    {
      assert_choice_type(types::short_bitmap, type_, "ssb-PositionsInBurst-r17");
      return c.get<fixed_bitstring<4>>();
    }
    const fixed_bitstring<8>& medium_bitmap() const
    {
      assert_choice_type(types::medium_bitmap, type_, "ssb-PositionsInBurst-r17");
      return c.get<fixed_bitstring<8>>();
    }
    const fixed_bitstring<64>& long_bitmap() const
    {
      assert_choice_type(types::long_bitmap, type_, "ssb-PositionsInBurst-r17");
      return c.get<fixed_bitstring<64>>();
    }
    fixed_bitstring<4>&  set_short_bitmap();
    fixed_bitstring<8>&  set_medium_bitmap();
    fixed_bitstring<64>& set_long_bitmap();

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<64>> c;

    void destroy_();
  };

  // member variables
  uint8_t                       add_pci_idx_r17 = 1;
  uint16_t                      add_pci_r17     = 0;
  periodicity_r17_e_            periodicity_r17;
  ssb_positions_in_burst_r17_c_ ssb_positions_in_burst_r17;
  int8_t                        ss_pbch_block_pwr_r17 = -60;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SlotFormatIndicator ::= SEQUENCE
struct slot_format_ind_s {
  using slot_format_comb_to_add_mod_list_l_            = dyn_array<slot_format_combinations_per_cell_s>;
  using slot_format_comb_to_release_list_l_            = bounded_array<uint8_t, 16>;
  using available_rb_sets_to_add_mod_list_r16_l_       = dyn_array<available_rb_sets_per_cell_r16_s>;
  using available_rb_sets_to_release_list_r16_l_       = bounded_array<uint8_t, 16>;
  using switch_trigger_to_add_mod_list_r16_l_          = dyn_array<search_space_switch_trigger_r16_s>;
  using switch_trigger_to_release_list_r16_l_          = bounded_array<uint8_t, 4>;
  using co_durs_per_cell_to_add_mod_list_r16_l_        = dyn_array<co_durs_per_cell_r16_s>;
  using co_durs_per_cell_to_release_list_r16_l_        = bounded_array<uint8_t, 16>;
  using switch_trigger_to_add_mod_list_size_ext_r16_l_ = dyn_array<search_space_switch_trigger_r16_s>;
  using switch_trigger_to_release_list_size_ext_r16_l_ = bounded_array<uint8_t, 12>;
  using co_durs_per_cell_to_add_mod_list_r17_l_        = dyn_array<co_durs_per_cell_r17_s>;

  // member variables
  bool                                ext              = false;
  uint32_t                            sfi_rnti         = 0;
  uint8_t                             dci_payload_size = 1;
  slot_format_comb_to_add_mod_list_l_ slot_format_comb_to_add_mod_list;
  slot_format_comb_to_release_list_l_ slot_format_comb_to_release_list;
  // ...
  // group 0
  copy_ptr<available_rb_sets_to_add_mod_list_r16_l_> available_rb_sets_to_add_mod_list_r16;
  copy_ptr<available_rb_sets_to_release_list_r16_l_> available_rb_sets_to_release_list_r16;
  copy_ptr<switch_trigger_to_add_mod_list_r16_l_>    switch_trigger_to_add_mod_list_r16;
  copy_ptr<switch_trigger_to_release_list_r16_l_>    switch_trigger_to_release_list_r16;
  copy_ptr<co_durs_per_cell_to_add_mod_list_r16_l_>  co_durs_per_cell_to_add_mod_list_r16;
  copy_ptr<co_durs_per_cell_to_release_list_r16_l_>  co_durs_per_cell_to_release_list_r16;
  // group 1
  copy_ptr<switch_trigger_to_add_mod_list_size_ext_r16_l_> switch_trigger_to_add_mod_list_size_ext_r16;
  copy_ptr<switch_trigger_to_release_list_size_ext_r16_l_> switch_trigger_to_release_list_size_ext_r16;
  // group 2
  copy_ptr<co_durs_per_cell_to_add_mod_list_r17_l_> co_durs_per_cell_to_add_mod_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-UL-DL-SlotConfig ::= SEQUENCE
struct tdd_ul_dl_slot_cfg_s {
  struct symbols_c_ {
    struct explicit_s_ {
      bool    nrof_dl_symbols_present = false;
      bool    nrof_ul_symbols_present = false;
      uint8_t nrof_dl_symbols         = 1;
      uint8_t nrof_ul_symbols         = 1;
    };
    struct types_opts {
      enum options { all_dl, all_ul, explicit_type, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    symbols_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    explicit_s_& explicit_type()
    {
      assert_choice_type(types::explicit_type, type_, "symbols");
      return c;
    }
    const explicit_s_& explicit_type() const
    {
      assert_choice_type(types::explicit_type, type_, "symbols");
      return c;
    }
    void         set_all_dl();
    void         set_all_ul();
    explicit_s_& set_explicit_type();

  private:
    types       type_;
    explicit_s_ c;
  };

  // member variables
  uint16_t   slot_idx = 0;
  symbols_c_ symbols;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-UL-DL-SlotConfig-IAB-MT-r16 ::= SEQUENCE
struct tdd_ul_dl_slot_cfg_iab_mt_r16_s {
  struct symbols_iab_mt_r16_c_ {
    struct explicit_r16_s_ {
      bool    nrof_dl_symbols_r16_present = false;
      bool    nrof_ul_symbols_r16_present = false;
      uint8_t nrof_dl_symbols_r16         = 1;
      uint8_t nrof_ul_symbols_r16         = 1;
    };
    struct explicit_iab_mt_r16_s_ {
      bool    nrof_dl_symbols_r16_present = false;
      bool    nrof_ul_symbols_r16_present = false;
      uint8_t nrof_dl_symbols_r16         = 1;
      uint8_t nrof_ul_symbols_r16         = 1;
    };
    struct types_opts {
      enum options { all_dl_r16, all_ul_r16, explicit_r16, explicit_iab_mt_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    symbols_iab_mt_r16_c_() = default;
    symbols_iab_mt_r16_c_(const symbols_iab_mt_r16_c_& other);
    symbols_iab_mt_r16_c_& operator=(const symbols_iab_mt_r16_c_& other);
    ~symbols_iab_mt_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    explicit_r16_s_& explicit_r16()
    {
      assert_choice_type(types::explicit_r16, type_, "symbols-IAB-MT-r16");
      return c.get<explicit_r16_s_>();
    }
    explicit_iab_mt_r16_s_& explicit_iab_mt_r16()
    {
      assert_choice_type(types::explicit_iab_mt_r16, type_, "symbols-IAB-MT-r16");
      return c.get<explicit_iab_mt_r16_s_>();
    }
    const explicit_r16_s_& explicit_r16() const
    {
      assert_choice_type(types::explicit_r16, type_, "symbols-IAB-MT-r16");
      return c.get<explicit_r16_s_>();
    }
    const explicit_iab_mt_r16_s_& explicit_iab_mt_r16() const
    {
      assert_choice_type(types::explicit_iab_mt_r16, type_, "symbols-IAB-MT-r16");
      return c.get<explicit_iab_mt_r16_s_>();
    }
    void                    set_all_dl_r16();
    void                    set_all_ul_r16();
    explicit_r16_s_&        set_explicit_r16();
    explicit_iab_mt_r16_s_& set_explicit_iab_mt_r16();

  private:
    types                                                    type_;
    choice_buffer_t<explicit_iab_mt_r16_s_, explicit_r16_s_> c;

    void destroy_();
  };

  // member variables
  uint16_t              slot_idx_r16 = 0;
  symbols_iab_mt_r16_c_ symbols_iab_mt_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Uplink-powerControl-r17 ::= SEQUENCE
struct ul_pwr_ctrl_r17_s {
  bool               p0_alpha_setfor_pusch_r17_present = false;
  bool               p0_alpha_setfor_pucch_r17_present = false;
  bool               p0_alpha_setfor_srs_r17_present   = false;
  uint8_t            ul_pwrctrl_id_r17                 = 1;
  p0_alpha_set_r17_s p0_alpha_setfor_pusch_r17;
  p0_alpha_set_r17_s p0_alpha_setfor_pucch_r17;
  p0_alpha_set_r17_s p0_alpha_setfor_srs_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkTxSwitching-r16 ::= SEQUENCE
struct ul_tx_switching_r16_s {
  struct ul_tx_switching_carrier_r16_opts {
    enum options { carrier1, carrier2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ul_tx_switching_carrier_r16_e_ = enumerated<ul_tx_switching_carrier_r16_opts>;

  // member variables
  bool                           ul_tx_switching_period_location_r16 = false;
  ul_tx_switching_carrier_r16_e_ ul_tx_switching_carrier_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WithinActiveTimeConfig-r16 ::= SEQUENCE
struct within_active_time_cfg_r16_s {
  bool    first_within_active_time_bwp_id_r16_present   = false;
  bool    dormancy_group_within_active_time_r16_present = false;
  uint8_t first_within_active_time_bwp_id_r16           = 0;
  uint8_t dormancy_group_within_active_time_r16         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ChannelAccessConfig-r16 ::= SEQUENCE
struct ch_access_cfg_r16_s {
  struct energy_detection_cfg_r16_c_ {
    struct types_opts {
      enum options { max_energy_detection_thres_r16, energy_detection_thres_offset_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    energy_detection_cfg_r16_c_() = default;
    energy_detection_cfg_r16_c_(const energy_detection_cfg_r16_c_& other);
    energy_detection_cfg_r16_c_& operator=(const energy_detection_cfg_r16_c_& other);
    ~energy_detection_cfg_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    int8_t& max_energy_detection_thres_r16()
    {
      assert_choice_type(types::max_energy_detection_thres_r16, type_, "energyDetectionConfig-r16");
      return c.get<int8_t>();
    }
    int8_t& energy_detection_thres_offset_r16()
    {
      assert_choice_type(types::energy_detection_thres_offset_r16, type_, "energyDetectionConfig-r16");
      return c.get<int8_t>();
    }
    const int8_t& max_energy_detection_thres_r16() const
    {
      assert_choice_type(types::max_energy_detection_thres_r16, type_, "energyDetectionConfig-r16");
      return c.get<int8_t>();
    }
    const int8_t& energy_detection_thres_offset_r16() const
    {
      assert_choice_type(types::energy_detection_thres_offset_r16, type_, "energyDetectionConfig-r16");
      return c.get<int8_t>();
    }
    int8_t& set_max_energy_detection_thres_r16();
    int8_t& set_energy_detection_thres_offset_r16();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                        energy_detection_cfg_r16_present            = false;
  bool                        ul_to_dl_cot_sharing_ed_thres_r16_present   = false;
  bool                        absence_of_any_other_technology_r16_present = false;
  energy_detection_cfg_r16_c_ energy_detection_cfg_r16;
  int8_t                      ul_to_dl_cot_sharing_ed_thres_r16 = -85;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CrossCarrierSchedulingConfig ::= SEQUENCE
struct cross_carrier_sched_cfg_s {
  struct sched_cell_info_c_ {
    struct own_s_ {
      bool cif_presence = false;
    };
    struct other_s_ {
      uint8_t sched_cell_id     = 0;
      uint8_t cif_in_sched_cell = 1;
    };
    struct types_opts {
      enum options { own, other, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    sched_cell_info_c_() = default;
    sched_cell_info_c_(const sched_cell_info_c_& other);
    sched_cell_info_c_& operator=(const sched_cell_info_c_& other);
    ~sched_cell_info_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    own_s_& own()
    {
      assert_choice_type(types::own, type_, "schedulingCellInfo");
      return c.get<own_s_>();
    }
    other_s_& other()
    {
      assert_choice_type(types::other, type_, "schedulingCellInfo");
      return c.get<other_s_>();
    }
    const own_s_& own() const
    {
      assert_choice_type(types::own, type_, "schedulingCellInfo");
      return c.get<own_s_>();
    }
    const other_s_& other() const
    {
      assert_choice_type(types::other, type_, "schedulingCellInfo");
      return c.get<other_s_>();
    }
    own_s_&   set_own();
    other_s_& set_other();

  private:
    types                             type_;
    choice_buffer_t<other_s_, own_s_> c;

    void destroy_();
  };
  struct carrier_ind_size_r16_s_ {
    uint8_t carrier_ind_size_dci_1_2_r16 = 0;
    uint8_t carrier_ind_size_dci_0_2_r16 = 0;
  };
  struct ccs_blind_detection_split_r17_opts {
    enum options {
      one_seventh,
      three_fourteenth,
      two_seventh,
      three_seventh,
      one_half,
      four_seventh,
      five_seventh,
      spare1,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using ccs_blind_detection_split_r17_e_ = enumerated<ccs_blind_detection_split_r17_opts>;

  // member variables
  bool               ext = false;
  sched_cell_info_c_ sched_cell_info;
  // ...
  // group 0
  bool                              enable_default_beam_for_cc_s_r16_present = false;
  copy_ptr<carrier_ind_size_r16_s_> carrier_ind_size_r16;
  // group 1
  bool                             ccs_blind_detection_split_r17_present = false;
  ccs_blind_detection_split_r17_e_ ccs_blind_detection_split_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DormantBWP-Config-r16 ::= SEQUENCE
struct dormant_bwp_cfg_r16_s {
  bool                                           dormant_bwp_id_r16_present          = false;
  bool                                           within_active_time_cfg_r16_present  = false;
  bool                                           outside_active_time_cfg_r16_present = false;
  uint8_t                                        dormant_bwp_id_r16                  = 0;
  setup_release_c<within_active_time_cfg_r16_s>  within_active_time_cfg_r16;
  setup_release_c<outside_active_time_cfg_r16_s> outside_active_time_cfg_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DummyJ ::= SEQUENCE
struct dummy_j_s {
  bool   ul_to_dl_cot_sharing_ed_thres_r16_present   = false;
  bool   absence_of_any_other_technology_r16_present = false;
  int8_t max_energy_detection_thres_r16              = -85;
  int8_t energy_detection_thres_offset_r16           = -20;
  int8_t ul_to_dl_cot_sharing_ed_thres_r16           = -85;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntraCellGuardBandsPerSCS-r16 ::= SEQUENCE
struct intra_cell_guard_bands_per_scs_r16_s {
  using intra_cell_guard_bands_r16_l_ = dyn_array<guard_band_r16_s>;

  // member variables
  subcarrier_spacing_e          guard_band_scs_r16;
  intra_cell_guard_bands_r16_l_ intra_cell_guard_bands_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LTE-CRS-PatternList-r16 ::= SEQUENCE (SIZE (1..3)) OF RateMatchPatternLTE-CRS
using lte_crs_pattern_list_r16_l = dyn_array<rate_match_pattern_lte_crs_s>;

// LTE-NeighCellsCRS-AssistInfoList-r17 ::= SEQUENCE (SIZE (1..8)) OF LTE-NeighCellsCRS-AssistInfo-r17
using lte_neigh_cells_crs_assist_info_list_r17_l = dyn_array<lte_neigh_cells_crs_assist_info_r17_s>;

// MIMOParam-r17 ::= SEQUENCE
struct mimo_param_r17_s {
  using add_pci_to_add_mod_list_r17_l_ = dyn_array<ssb_mtc_add_pci_r17_s>;
  using add_pci_to_release_list_r17_l_ = bounded_array<uint8_t, 7>;
  struct unified_tci_state_type_r17_opts {
    enum options { separate, joint, nulltype } value;

    const char* to_string() const;
  };
  using unified_tci_state_type_r17_e_      = enumerated<unified_tci_state_type_r17_opts>;
  using ul_pwr_ctrl_to_add_mod_list_r17_l_ = dyn_array<ul_pwr_ctrl_r17_s>;
  using ul_pwr_ctrl_to_release_list_r17_l_ = dyn_array<uint8_t>;
  struct sfn_schem_epdcch_r17_opts {
    enum options { sfn_scheme_a, sfn_scheme_b, nulltype } value;

    const char* to_string() const;
  };
  using sfn_schem_epdcch_r17_e_ = enumerated<sfn_schem_epdcch_r17_opts>;
  struct sfn_scheme_pdsch_r17_opts {
    enum options { sfn_scheme_a, sfn_scheme_b, nulltype } value;

    const char* to_string() const;
  };
  using sfn_scheme_pdsch_r17_e_ = enumerated<sfn_scheme_pdsch_r17_opts>;

  // member variables
  bool                               unified_tci_state_type_r17_present = false;
  bool                               sfn_schem_epdcch_r17_present       = false;
  bool                               sfn_scheme_pdsch_r17_present       = false;
  add_pci_to_add_mod_list_r17_l_     add_pci_to_add_mod_list_r17;
  add_pci_to_release_list_r17_l_     add_pci_to_release_list_r17;
  unified_tci_state_type_r17_e_      unified_tci_state_type_r17;
  ul_pwr_ctrl_to_add_mod_list_r17_l_ ul_pwr_ctrl_to_add_mod_list_r17;
  ul_pwr_ctrl_to_release_list_r17_l_ ul_pwr_ctrl_to_release_list_r17;
  sfn_schem_epdcch_r17_e_            sfn_schem_epdcch_r17;
  sfn_scheme_pdsch_r17_e_            sfn_scheme_pdsch_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-DL-PRS-PDC-Info-r17 ::= SEQUENCE
struct nr_dl_prs_pdc_info_r17_s {
  bool                        ext                               = false;
  bool                        nr_dl_prs_pdc_res_set_r17_present = false;
  nr_dl_prs_pdc_res_set_r17_s nr_dl_prs_pdc_res_set_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-ServingCellConfig ::= SEQUENCE
struct pdcch_serving_cell_cfg_s {
  bool                               ext                     = false;
  bool                               slot_format_ind_present = false;
  setup_release_c<slot_format_ind_s> slot_format_ind;
  // ...
  // group 0
  bool                                              search_space_switch_timer_r16_present = false;
  copy_ptr<setup_release_c<availability_ind_r16_s>> availability_ind_r16;
  uint8_t                                           search_space_switch_timer_r16 = 1;
  // group 1
  bool     search_space_switch_timer_v1710_present = false;
  uint16_t search_space_switch_timer_v1710         = 81;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-ServingCellConfig ::= SEQUENCE
struct pdsch_serving_cell_cfg_s {
  struct xoverhead_opts {
    enum options { xoh6, xoh12, xoh18, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using xoverhead_e_ = enumerated<xoverhead_opts>;
  struct nrof_harq_processes_for_pdsch_opts {
    enum options { n2, n4, n6, n10, n12, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nrof_harq_processes_for_pdsch_e_ = enumerated<nrof_harq_processes_for_pdsch_opts>;

  // member variables
  bool                                         ext                                   = false;
  bool                                         code_block_group_tx_present           = false;
  bool                                         xoverhead_present                     = false;
  bool                                         nrof_harq_processes_for_pdsch_present = false;
  bool                                         pucch_cell_present                    = false;
  setup_release_c<pdsch_code_block_group_tx_s> code_block_group_tx;
  xoverhead_e_                                 xoverhead;
  nrof_harq_processes_for_pdsch_e_             nrof_harq_processes_for_pdsch;
  uint8_t                                      pucch_cell = 0;
  // ...
  // group 0
  bool    max_mimo_layers_present          = false;
  bool    processing_type2_enabled_present = false;
  uint8_t max_mimo_layers                  = 1;
  bool    processing_type2_enabled         = false;
  // group 1
  copy_ptr<setup_release_c<dyn_seq_of<pdsch_code_block_group_tx_s, 1, 2>>> pdsch_code_block_group_tx_list_r16;
  // group 2
  bool                                           nrof_harq_processes_for_pdsch_v1700_present = false;
  copy_ptr<setup_release_c<fixed_bitstring<32>>> dl_harq_feedback_disabled_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SemiStaticChannelAccessConfigUE-r17 ::= SEQUENCE
struct semi_static_ch_access_cfg_ue_r17_s {
  struct period_ue_r17_opts {
    enum options { ms1, ms2, ms2dot5, ms4, ms5, ms10, spare2, spare1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using period_ue_r17_e_ = enumerated<period_ue_r17_opts>;

  // member variables
  period_ue_r17_e_ period_ue_r17;
  uint16_t         offset_ue_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TCI-ActivatedConfig-r17 ::= SEQUENCE
struct tci_activ_cfg_r17_s {
  using pdcch_tci_r17_l_ = bounded_array<uint8_t, 5>;

  // member variables
  pdcch_tci_r17_l_          pdcch_tci_r17;
  bounded_bitstring<1, 128> pdsch_tci_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-UL-DL-ConfigDedicated ::= SEQUENCE
struct tdd_ul_dl_cfg_ded_s {
  using slot_specific_cfgs_to_add_mod_list_l_ = dyn_array<tdd_ul_dl_slot_cfg_s>;
  using slot_specific_cfgs_to_release_list_l_ = dyn_array<uint16_t>;

  // member variables
  bool                                  ext = false;
  slot_specific_cfgs_to_add_mod_list_l_ slot_specific_cfgs_to_add_mod_list;
  slot_specific_cfgs_to_release_list_l_ slot_specific_cfgs_to_release_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-UL-DL-ConfigDedicated-IAB-MT-r16 ::= SEQUENCE
struct tdd_ul_dl_cfg_ded_iab_mt_r16_s {
  using slot_specific_cfgs_to_add_mod_list_iab_mt_r16_l_ = dyn_array<tdd_ul_dl_slot_cfg_iab_mt_r16_s>;
  using slot_specific_cfgs_to_release_list_iab_mt_r16_l_ = dyn_array<uint16_t>;

  // member variables
  bool                                             ext = false;
  slot_specific_cfgs_to_add_mod_list_iab_mt_r16_l_ slot_specific_cfgs_to_add_mod_list_iab_mt_r16;
  slot_specific_cfgs_to_release_list_iab_mt_r16_l_ slot_specific_cfgs_to_release_list_iab_mt_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkConfig ::= SEQUENCE
struct ul_cfg_s {
  using ul_bwp_to_release_list_l_ = bounded_array<uint8_t, 4>;
  using ul_bwp_to_add_mod_list_l_ = dyn_array<bwp_ul_s>;
  using ul_ch_bw_per_scs_list_l_  = dyn_array<scs_specific_carrier_s>;

  // member variables
  bool                                      ext                            = false;
  bool                                      init_ul_bwp_present            = false;
  bool                                      first_active_ul_bwp_id_present = false;
  bool                                      pusch_serving_cell_cfg_present = false;
  bool                                      carrier_switching_present      = false;
  bwp_ul_ded_s                              init_ul_bwp;
  ul_bwp_to_release_list_l_                 ul_bwp_to_release_list;
  ul_bwp_to_add_mod_list_l_                 ul_bwp_to_add_mod_list;
  uint8_t                                   first_active_ul_bwp_id = 0;
  setup_release_c<pusch_serving_cell_cfg_s> pusch_serving_cell_cfg;
  setup_release_c<srs_carrier_switching_s>  carrier_switching;
  // ...
  // group 0
  bool                               pwr_boost_pi2_bpsk_present = false;
  bool                               pwr_boost_pi2_bpsk         = false;
  copy_ptr<ul_ch_bw_per_scs_list_l_> ul_ch_bw_per_scs_list;
  // group 1
  bool                                             enable_pl_rs_upd_for_pusch_srs_r16_present      = false;
  bool                                             enable_default_beam_pl_for_pusch0_0_r16_present = false;
  bool                                             enable_default_beam_pl_for_pucch_r16_present    = false;
  bool                                             enable_default_beam_pl_for_srs_r16_present      = false;
  bool                                             mpr_pwr_boost_fr2_r16_present                   = false;
  copy_ptr<setup_release_c<ul_tx_switching_r16_s>> ul_tx_switching_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServingCellConfig ::= SEQUENCE
struct serving_cell_cfg_s {
  using dl_bwp_to_release_list_l_ = bounded_array<uint8_t, 4>;
  using dl_bwp_to_add_mod_list_l_ = dyn_array<bwp_dl_s>;
  struct bwp_inactivity_timer_opts {
    enum options {
      ms2,
      ms3,
      ms4,
      ms5,
      ms6,
      ms8,
      ms10,
      ms20,
      ms30,
      ms40,
      ms50,
      ms60,
      ms80,
      ms100,
      ms200,
      ms300,
      ms500,
      ms750,
      ms1280,
      ms1920,
      ms2560,
      spare10,
      spare9,
      spare8,
      spare7,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using bwp_inactivity_timer_e_ = enumerated<bwp_inactivity_timer_opts>;
  struct scell_deactivation_timer_opts {
    enum options {
      ms20,
      ms40,
      ms80,
      ms160,
      ms200,
      ms240,
      ms320,
      ms400,
      ms480,
      ms520,
      ms640,
      ms720,
      ms840,
      ms1280,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using scell_deactivation_timer_e_ = enumerated<scell_deactivation_timer_opts>;
  struct pathloss_ref_linking_opts {
    enum options { sp_cell, scell, nulltype } value;

    const char* to_string() const;
  };
  using pathloss_ref_linking_e_               = enumerated<pathloss_ref_linking_opts>;
  using rate_match_pattern_to_add_mod_list_l_ = dyn_array<rate_match_pattern_s>;
  using rate_match_pattern_to_release_list_l_ = bounded_array<uint8_t, 4>;
  using dl_ch_bw_per_scs_list_l_              = dyn_array<scs_specific_carrier_s>;
  struct ca_slot_offset_r16_c_ {
    struct types_opts {
      enum options { ref_scs15k_hz, ref_scs30_k_hz, ref_scs60_k_hz, ref_scs120_k_hz, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ca_slot_offset_r16_c_() = default;
    ca_slot_offset_r16_c_(const ca_slot_offset_r16_c_& other);
    ca_slot_offset_r16_c_& operator=(const ca_slot_offset_r16_c_& other);
    ~ca_slot_offset_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    int8_t& ref_scs15k_hz()
    {
      assert_choice_type(types::ref_scs15k_hz, type_, "ca-SlotOffset-r16");
      return c.get<int8_t>();
    }
    int8_t& ref_scs30_k_hz()
    {
      assert_choice_type(types::ref_scs30_k_hz, type_, "ca-SlotOffset-r16");
      return c.get<int8_t>();
    }
    int8_t& ref_scs60_k_hz()
    {
      assert_choice_type(types::ref_scs60_k_hz, type_, "ca-SlotOffset-r16");
      return c.get<int8_t>();
    }
    int8_t& ref_scs120_k_hz()
    {
      assert_choice_type(types::ref_scs120_k_hz, type_, "ca-SlotOffset-r16");
      return c.get<int8_t>();
    }
    const int8_t& ref_scs15k_hz() const
    {
      assert_choice_type(types::ref_scs15k_hz, type_, "ca-SlotOffset-r16");
      return c.get<int8_t>();
    }
    const int8_t& ref_scs30_k_hz() const
    {
      assert_choice_type(types::ref_scs30_k_hz, type_, "ca-SlotOffset-r16");
      return c.get<int8_t>();
    }
    const int8_t& ref_scs60_k_hz() const
    {
      assert_choice_type(types::ref_scs60_k_hz, type_, "ca-SlotOffset-r16");
      return c.get<int8_t>();
    }
    const int8_t& ref_scs120_k_hz() const
    {
      assert_choice_type(types::ref_scs120_k_hz, type_, "ca-SlotOffset-r16");
      return c.get<int8_t>();
    }
    int8_t& set_ref_scs15k_hz();
    int8_t& set_ref_scs30_k_hz();
    int8_t& set_ref_scs60_k_hz();
    int8_t& set_ref_scs120_k_hz();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  using intra_cell_guard_bands_dl_list_r16_l_ = dyn_array<intra_cell_guard_bands_per_scs_r16_s>;
  using intra_cell_guard_bands_ul_list_r16_l_ = dyn_array<intra_cell_guard_bands_per_scs_r16_s>;
  struct nrof_harq_bundling_groups_r17_opts {
    enum options { n1, n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nrof_harq_bundling_groups_r17_e_ = enumerated<nrof_harq_bundling_groups_r17_opts>;

  // member variables
  bool                                      ext                              = false;
  bool                                      tdd_ul_dl_cfg_ded_present        = false;
  bool                                      init_dl_bwp_present              = false;
  bool                                      first_active_dl_bwp_id_present   = false;
  bool                                      bwp_inactivity_timer_present     = false;
  bool                                      default_dl_bwp_id_present        = false;
  bool                                      ul_cfg_present                   = false;
  bool                                      supplementary_ul_present         = false;
  bool                                      pdcch_serving_cell_cfg_present   = false;
  bool                                      pdsch_serving_cell_cfg_present   = false;
  bool                                      csi_meas_cfg_present             = false;
  bool                                      scell_deactivation_timer_present = false;
  bool                                      cross_carrier_sched_cfg_present  = false;
  bool                                      dummy1_present                   = false;
  bool                                      pathloss_ref_linking_present     = false;
  bool                                      serving_cell_mo_present          = false;
  tdd_ul_dl_cfg_ded_s                       tdd_ul_dl_cfg_ded;
  bwp_dl_ded_s                              init_dl_bwp;
  dl_bwp_to_release_list_l_                 dl_bwp_to_release_list;
  dl_bwp_to_add_mod_list_l_                 dl_bwp_to_add_mod_list;
  uint8_t                                   first_active_dl_bwp_id = 0;
  bwp_inactivity_timer_e_                   bwp_inactivity_timer;
  uint8_t                                   default_dl_bwp_id = 0;
  ul_cfg_s                                  ul_cfg;
  ul_cfg_s                                  supplementary_ul;
  setup_release_c<pdcch_serving_cell_cfg_s> pdcch_serving_cell_cfg;
  setup_release_c<pdsch_serving_cell_cfg_s> pdsch_serving_cell_cfg;
  setup_release_c<csi_meas_cfg_s>           csi_meas_cfg;
  scell_deactivation_timer_e_               scell_deactivation_timer;
  cross_carrier_sched_cfg_s                 cross_carrier_sched_cfg;
  uint8_t                                   tag_id = 0;
  pathloss_ref_linking_e_                   pathloss_ref_linking;
  uint8_t                                   serving_cell_mo = 1;
  // ...
  // group 0
  copy_ptr<setup_release_c<rate_match_pattern_lte_crs_s>> lte_crs_to_match_around;
  copy_ptr<rate_match_pattern_to_add_mod_list_l_>         rate_match_pattern_to_add_mod_list;
  copy_ptr<rate_match_pattern_to_release_list_l_>         rate_match_pattern_to_release_list;
  copy_ptr<dl_ch_bw_per_scs_list_l_>                      dl_ch_bw_per_scs_list;
  // group 1
  bool                                             supplementary_ul_release_r16_present                      = false;
  bool                                             csi_rs_validation_with_dci_r16_present                    = false;
  bool                                             crs_rate_match_per_coreset_pool_idx_r16_present           = false;
  bool                                             enable_two_default_tci_states_r16_present                 = false;
  bool                                             enable_default_tci_state_per_coreset_pool_idx_r16_present = false;
  bool                                             enable_beam_switch_timing_r16_present                     = false;
  bool                                             cbg_tx_diff_t_bs_processing_type1_r16_present             = false;
  bool                                             cbg_tx_diff_t_bs_processing_type2_r16_present             = false;
  copy_ptr<tdd_ul_dl_cfg_ded_iab_mt_r16_s>         tdd_ul_dl_cfg_ded_iab_mt_r16;
  copy_ptr<setup_release_c<dormant_bwp_cfg_r16_s>> dormant_bwp_cfg_r16;
  copy_ptr<ca_slot_offset_r16_c_>                  ca_slot_offset_r16;
  copy_ptr<setup_release_c<dummy_j_s>>             dummy2;
  copy_ptr<intra_cell_guard_bands_dl_list_r16_l_>  intra_cell_guard_bands_dl_list_r16;
  copy_ptr<intra_cell_guard_bands_ul_list_r16_l_>  intra_cell_guard_bands_ul_list_r16;
  copy_ptr<setup_release_c<dyn_seq_of<rate_match_pattern_lte_crs_s, 1, 3>>> lte_crs_pattern_list1_r16;
  copy_ptr<setup_release_c<dyn_seq_of<rate_match_pattern_lte_crs_s, 1, 3>>> lte_crs_pattern_list2_r16;
  // group 2
  bool                                           directional_collision_handling_r16_present = false;
  copy_ptr<setup_release_c<ch_access_cfg_r16_s>> ch_access_cfg_r16;
  // group 3
  bool                                                          ch_access_mode2_r17_present                   = false;
  bool                                                          time_domain_harq_bundling_type1_r17_present   = false;
  bool                                                          nrof_harq_bundling_groups_r17_present         = false;
  bool                                                          fdmed_reception_multicast_r17_present         = false;
  bool                                                          more_than_one_nack_only_mode_r17_present      = false;
  bool                                                          directional_collision_handling_dc_r17_present = false;
  copy_ptr<setup_release_c<nr_dl_prs_pdc_info_r17_s>>           nr_dl_prs_pdc_info_r17;
  copy_ptr<setup_release_c<semi_static_ch_access_cfg_ue_r17_s>> semi_static_ch_access_cfg_ue_r17;
  copy_ptr<setup_release_c<mimo_param_r17_s>>                   mimo_param_r17;
  nrof_harq_bundling_groups_r17_e_                              nrof_harq_bundling_groups_r17;
  copy_ptr<tci_activ_cfg_r17_s>                                 tci_activ_cfg_r17;
  copy_ptr<setup_release_c<dyn_seq_of<lte_neigh_cells_crs_assist_info_r17_s, 1, 8>>>
      lte_neigh_cells_crs_assist_info_list_r17;
  // group 4
  bool lte_neigh_cells_crs_assumptions_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1
