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

// MIMO-LayersDL ::= ENUMERATED
struct mimo_layers_dl_opts {
  enum options { two_layers, four_layers, eight_layers, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using mimo_layers_dl_e = enumerated<mimo_layers_dl_opts>;

// MIMO-LayersUL ::= ENUMERATED
struct mimo_layers_ul_opts {
  enum options { one_layer, two_layers, four_layers, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using mimo_layers_ul_e = enumerated<mimo_layers_ul_opts>;

// CA-BandwidthClassEUTRA ::= ENUMERATED
struct ca_bw_class_eutra_opts {
  enum options { a, b, c, d, e, f, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ca_bw_class_eutra_e = enumerated<ca_bw_class_eutra_opts, true>;

// CA-BandwidthClassNR ::= ENUMERATED
struct ca_bw_class_nr_opts {
  enum options {
    a,
    b,
    c,
    d,
    e,
    f,
    g,
    h,
    i,
    j,
    k,
    l,
    m,
    n,
    o,
    p,
    q,
    // ...
    r2_v1730,
    r3_v1730,
    r4_v1730,
    r5_v1730,
    r6_v1730,
    r7_v1730,
    r8_v1730,
    r9_v1730,
    r10_v1730,
    r11_v1730,
    r12_v1730,
    nulltype
  } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using ca_bw_class_nr_e = enumerated<ca_bw_class_nr_opts, true, 11>;

// BandParameters ::= CHOICE
struct band_params_c {
  struct eutra_s_ {
    bool                ca_bw_class_dl_eutra_present = false;
    bool                ca_bw_class_ul_eutra_present = false;
    uint16_t            band_eutra                   = 1;
    ca_bw_class_eutra_e ca_bw_class_dl_eutra;
    ca_bw_class_eutra_e ca_bw_class_ul_eutra;
  };
  struct nr_s_ {
    bool             ca_bw_class_dl_nr_present = false;
    bool             ca_bw_class_ul_nr_present = false;
    uint16_t         band_nr                   = 1;
    ca_bw_class_nr_e ca_bw_class_dl_nr;
    ca_bw_class_nr_e ca_bw_class_ul_nr;
  };
  struct types_opts {
    enum options { eutra, nr, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  band_params_c() = default;
  band_params_c(const band_params_c& other);
  band_params_c& operator=(const band_params_c& other);
  ~band_params_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutra_s_& eutra()
  {
    assert_choice_type(types::eutra, type_, "BandParameters");
    return c.get<eutra_s_>();
  }
  nr_s_& nr()
  {
    assert_choice_type(types::nr, type_, "BandParameters");
    return c.get<nr_s_>();
  }
  const eutra_s_& eutra() const
  {
    assert_choice_type(types::eutra, type_, "BandParameters");
    return c.get<eutra_s_>();
  }
  const nr_s_& nr() const
  {
    assert_choice_type(types::nr, type_, "BandParameters");
    return c.get<nr_s_>();
  }
  eutra_s_& set_eutra();
  nr_s_&    set_nr();

private:
  types                            type_;
  choice_buffer_t<eutra_s_, nr_s_> c;

  void destroy_();
};

// CA-ParametersEUTRA ::= SEQUENCE
struct ca_params_eutra_s {
  bool                     ext                                              = false;
  bool                     multiple_timing_advance_present                  = false;
  bool                     simul_rx_tx_present                              = false;
  bool                     supported_naics_2_crs_ap_present                 = false;
  bool                     add_rx_tx_performance_req_present                = false;
  bool                     ue_ca_pwr_class_n_present                        = false;
  bool                     supported_bw_combination_set_eutra_v1530_present = false;
  bounded_bitstring<1, 8>  supported_naics_2_crs_ap;
  bounded_bitstring<1, 32> supported_bw_combination_set_eutra_v1530;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNR ::= SEQUENCE
struct ca_params_nr_s {
  struct supported_num_tag_opts {
    enum options { n2, n3, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using supported_num_tag_e_ = enumerated<supported_num_tag_opts>;

  // member variables
  bool                 ext                                                    = false;
  bool                 dummy_present                                          = false;
  bool                 parallel_tx_srs_pucch_pusch_present                    = false;
  bool                 parallel_tx_prach_srs_pucch_pusch_present              = false;
  bool                 simul_rx_tx_inter_band_ca_present                      = false;
  bool                 simul_rx_tx_sul_present                                = false;
  bool                 diff_numerology_across_pucch_group_present             = false;
  bool                 diff_numerology_within_pucch_group_smaller_scs_present = false;
  bool                 supported_num_tag_present                              = false;
  supported_num_tag_e_ supported_num_tag;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRDC-Parameters ::= SEQUENCE
struct mrdc_params_s {
  struct ul_sharing_eutra_nr_opts {
    enum options { tdm, fdm, both, nulltype } value;

    const char* to_string() const;
  };
  using ul_sharing_eutra_nr_e_ = enumerated<ul_sharing_eutra_nr_opts>;
  struct ul_switching_time_eutra_nr_opts {
    enum options { type1, type2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ul_switching_time_eutra_nr_e_ = enumerated<ul_switching_time_eutra_nr_opts>;
  struct intra_band_endc_support_opts {
    enum options { non_contiguous, both, nulltype } value;

    const char* to_string() const;
  };
  using intra_band_endc_support_e_ = enumerated<intra_band_endc_support_opts>;

  // member variables
  bool                          ext                                 = false;
  bool                          single_ul_tx_present                = false;
  bool                          dyn_pwr_sharing_endc_present        = false;
  bool                          tdm_pattern_present                 = false;
  bool                          ul_sharing_eutra_nr_present         = false;
  bool                          ul_switching_time_eutra_nr_present  = false;
  bool                          simul_rx_tx_inter_band_endc_present = false;
  bool                          async_intra_band_endc_present       = false;
  ul_sharing_eutra_nr_e_        ul_sharing_eutra_nr;
  ul_switching_time_eutra_nr_e_ ul_switching_time_eutra_nr;
  // ...
  // group 0
  bool                       dual_pa_architecture_present     = false;
  bool                       intra_band_endc_support_present  = false;
  bool                       ul_timing_align_eutra_nr_present = false;
  intra_band_endc_support_e_ intra_band_endc_support;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination ::= SEQUENCE
struct band_combination_s {
  using band_list_l_ = dyn_array<band_params_c>;

  // member variables
  bool                     ca_params_eutra_present              = false;
  bool                     ca_params_nr_present                 = false;
  bool                     mrdc_params_present                  = false;
  bool                     supported_bw_combination_set_present = false;
  bool                     pwr_class_v1530_present              = false;
  band_list_l_             band_list;
  uint16_t                 feature_set_combination = 0;
  ca_params_eutra_s        ca_params_eutra;
  ca_params_nr_s           ca_params_nr;
  mrdc_params_s            mrdc_params;
  bounded_bitstring<1, 32> supported_bw_combination_set;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNR-v1540 ::= SEQUENCE
struct ca_params_nr_v1540_s {
  struct csi_rs_im_reception_for_feedback_per_band_comb_s_ {
    bool     max_num_simul_nzp_csi_rs_act_bwp_all_cc_present         = false;
    bool     total_num_ports_simul_nzp_csi_rs_act_bwp_all_cc_present = false;
    uint8_t  max_num_simul_nzp_csi_rs_act_bwp_all_cc                 = 1;
    uint16_t total_num_ports_simul_nzp_csi_rs_act_bwp_all_cc         = 2;
  };

  // member variables
  bool                                              simul_srs_assoc_csi_rs_all_cc_present                  = false;
  bool                                              csi_rs_im_reception_for_feedback_per_band_comb_present = false;
  bool                                              simul_csi_reports_all_cc_present                       = false;
  bool                                              dual_pa_architecture_present                           = false;
  uint8_t                                           simul_srs_assoc_csi_rs_all_cc                          = 5;
  csi_rs_im_reception_for_feedback_per_band_comb_s_ csi_rs_im_reception_for_feedback_per_band_comb;
  uint8_t                                           simul_csi_reports_all_cc = 5;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNR-v1550 ::= SEQUENCE
struct ca_params_nr_v1550_s {
  bool dummy_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNR-v1560 ::= SEQUENCE
struct ca_params_nr_v1560_s {
  bool diff_numerology_within_pucch_group_larger_scs_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookComboParametersAdditionPerBC-r16 ::= SEQUENCE
struct codebook_combo_params_addition_per_bc_r16_s {
  using type1_sp_type2_null_r16_l_         = bounded_array<uint16_t, 16>;
  using type1_sp_type2_ps_null_r16_l_      = bounded_array<uint16_t, 16>;
  using type1_sp_e_type2_r1_null_r16_l_    = bounded_array<uint16_t, 16>;
  using type1_sp_e_type2_r2_null_r16_l_    = bounded_array<uint16_t, 16>;
  using type1_sp_e_type2_r1_ps_null_r16_l_ = bounded_array<uint16_t, 16>;
  using type1_sp_e_type2_r2_ps_null_r16_l_ = bounded_array<uint16_t, 16>;
  using type1_sp_type2_type2_ps_r16_l_     = bounded_array<uint16_t, 16>;
  using type1_mp_type2_null_r16_l_         = bounded_array<uint16_t, 16>;
  using type1_mp_type2_ps_null_r16_l_      = bounded_array<uint16_t, 16>;
  using type1_mp_e_type2_r1_null_r16_l_    = bounded_array<uint16_t, 16>;
  using type1_mp_e_type2_r2_null_r16_l_    = bounded_array<uint16_t, 16>;
  using type1_mp_e_type2_r1_ps_null_r16_l_ = bounded_array<uint16_t, 16>;
  using type1_mp_e_type2_r2_ps_null_r16_l_ = bounded_array<uint16_t, 16>;
  using type1_mp_type2_type2_ps_r16_l_     = bounded_array<uint16_t, 16>;

  // member variables
  type1_sp_type2_null_r16_l_         type1_sp_type2_null_r16;
  type1_sp_type2_ps_null_r16_l_      type1_sp_type2_ps_null_r16;
  type1_sp_e_type2_r1_null_r16_l_    type1_sp_e_type2_r1_null_r16;
  type1_sp_e_type2_r2_null_r16_l_    type1_sp_e_type2_r2_null_r16;
  type1_sp_e_type2_r1_ps_null_r16_l_ type1_sp_e_type2_r1_ps_null_r16;
  type1_sp_e_type2_r2_ps_null_r16_l_ type1_sp_e_type2_r2_ps_null_r16;
  type1_sp_type2_type2_ps_r16_l_     type1_sp_type2_type2_ps_r16;
  type1_mp_type2_null_r16_l_         type1_mp_type2_null_r16;
  type1_mp_type2_ps_null_r16_l_      type1_mp_type2_ps_null_r16;
  type1_mp_e_type2_r1_null_r16_l_    type1_mp_e_type2_r1_null_r16;
  type1_mp_e_type2_r2_null_r16_l_    type1_mp_e_type2_r2_null_r16;
  type1_mp_e_type2_r1_ps_null_r16_l_ type1_mp_e_type2_r1_ps_null_r16;
  type1_mp_e_type2_r2_ps_null_r16_l_ type1_mp_e_type2_r2_ps_null_r16;
  type1_mp_type2_type2_ps_r16_l_     type1_mp_type2_type2_ps_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookParameters-v1610 ::= SEQUENCE
struct codebook_params_v1610_s {
  struct supported_csi_rs_res_list_alt_r16_s_ {
    using type1_single_panel_r16_l_ = bounded_array<uint16_t, 7>;
    using type1_multi_panel_r16_l_  = bounded_array<uint16_t, 7>;
    using type2_r16_l_              = bounded_array<uint16_t, 7>;
    using type2_port_sel_r16_l_     = bounded_array<uint16_t, 7>;

    // member variables
    type1_single_panel_r16_l_ type1_single_panel_r16;
    type1_multi_panel_r16_l_  type1_multi_panel_r16;
    type2_r16_l_              type2_r16;
    type2_port_sel_r16_l_     type2_port_sel_r16;
  };

  // member variables
  bool                                 supported_csi_rs_res_list_alt_r16_present = false;
  supported_csi_rs_res_list_alt_r16_s_ supported_csi_rs_res_list_alt_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookParametersAdditionPerBC-r16 ::= SEQUENCE
struct codebook_params_addition_per_bc_r16_s {
  using etype2_r1_r16_l_          = bounded_array<uint16_t, 16>;
  using etype2_r2_r16_l_          = bounded_array<uint16_t, 16>;
  using etype2_r1_port_sel_r16_l_ = bounded_array<uint16_t, 16>;
  using etype2_r2_port_sel_r16_l_ = bounded_array<uint16_t, 16>;

  // member variables
  etype2_r1_r16_l_          etype2_r1_r16;
  etype2_r2_r16_l_          etype2_r2_r16;
  etype2_r1_port_sel_r16_l_ etype2_r1_port_sel_r16;
  etype2_r2_port_sel_r16_l_ etype2_r2_port_sel_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-SwitchingTimeEUTRA ::= SEQUENCE
struct srs_switching_time_eutra_s {
  struct switching_time_dl_opts {
    enum options {
      n0,
      n0dot5,
      n1,
      n1dot5,
      n2,
      n2dot5,
      n3,
      n3dot5,
      n4,
      n4dot5,
      n5,
      n5dot5,
      n6,
      n6dot5,
      n7,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using switching_time_dl_e_ = enumerated<switching_time_dl_opts>;
  struct switching_time_ul_opts {
    enum options {
      n0,
      n0dot5,
      n1,
      n1dot5,
      n2,
      n2dot5,
      n3,
      n3dot5,
      n4,
      n4dot5,
      n5,
      n5dot5,
      n6,
      n6dot5,
      n7,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using switching_time_ul_e_ = enumerated<switching_time_ul_opts>;

  // member variables
  bool                 switching_time_dl_present = false;
  bool                 switching_time_ul_present = false;
  switching_time_dl_e_ switching_time_dl;
  switching_time_ul_e_ switching_time_ul;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-SwitchingTimeNR ::= SEQUENCE
struct srs_switching_time_nr_s {
  struct switching_time_dl_opts {
    enum options { n0us, n30us, n100us, n140us, n200us, n300us, n500us, n900us, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using switching_time_dl_e_ = enumerated<switching_time_dl_opts>;
  struct switching_time_ul_opts {
    enum options { n0us, n30us, n100us, n140us, n200us, n300us, n500us, n900us, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using switching_time_ul_e_ = enumerated<switching_time_ul_opts>;

  // member variables
  bool                 switching_time_dl_present = false;
  bool                 switching_time_ul_present = false;
  switching_time_dl_e_ switching_time_dl;
  switching_time_ul_e_ switching_time_ul;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParameters-v1540 ::= SEQUENCE
struct band_params_v1540_s {
  struct srs_carrier_switch_c_ {
    struct nr_s_ {
      using srs_switching_times_list_nr_l_ = dyn_array<srs_switching_time_nr_s>;

      // member variables
      srs_switching_times_list_nr_l_ srs_switching_times_list_nr;
    };
    struct eutra_s_ {
      using srs_switching_times_list_eutra_l_ = dyn_array<srs_switching_time_eutra_s>;

      // member variables
      srs_switching_times_list_eutra_l_ srs_switching_times_list_eutra;
    };
    struct types_opts {
      enum options { nr, eutra, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    srs_carrier_switch_c_() = default;
    srs_carrier_switch_c_(const srs_carrier_switch_c_& other);
    srs_carrier_switch_c_& operator=(const srs_carrier_switch_c_& other);
    ~srs_carrier_switch_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    nr_s_& nr()
    {
      assert_choice_type(types::nr, type_, "srs-CarrierSwitch");
      return c.get<nr_s_>();
    }
    eutra_s_& eutra()
    {
      assert_choice_type(types::eutra, type_, "srs-CarrierSwitch");
      return c.get<eutra_s_>();
    }
    const nr_s_& nr() const
    {
      assert_choice_type(types::nr, type_, "srs-CarrierSwitch");
      return c.get<nr_s_>();
    }
    const eutra_s_& eutra() const
    {
      assert_choice_type(types::eutra, type_, "srs-CarrierSwitch");
      return c.get<eutra_s_>();
    }
    nr_s_&    set_nr();
    eutra_s_& set_eutra();

  private:
    types                            type_;
    choice_buffer_t<eutra_s_, nr_s_> c;

    void destroy_();
  };
  struct srs_tx_switch_s_ {
    struct supported_srs_tx_port_switch_opts {
      enum options { t1r2, t1r4, t2r4, t1r4_t2r4, t1r1, t2r2, t4r4, not_supported, nulltype } value;

      const char* to_string() const;
    };
    using supported_srs_tx_port_switch_e_ = enumerated<supported_srs_tx_port_switch_opts>;

    // member variables
    bool                            tx_switch_impact_to_rx_present      = false;
    bool                            tx_switch_with_another_band_present = false;
    supported_srs_tx_port_switch_e_ supported_srs_tx_port_switch;
    uint8_t                         tx_switch_impact_to_rx      = 1;
    uint8_t                         tx_switch_with_another_band = 1;
  };

  // member variables
  bool                  srs_carrier_switch_present = false;
  bool                  srs_tx_switch_present      = false;
  srs_carrier_switch_c_ srs_carrier_switch;
  srs_tx_switch_s_      srs_tx_switch;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParameters-v1610 ::= SEQUENCE
struct band_params_v1610_s {
  struct srs_tx_switch_v1610_s_ {
    struct supported_srs_tx_port_switch_v1610_opts {
      enum options {
        t1r1_t1r2,
        t1r1_t1r2_t1r4,
        t1r1_t1r2_t2r2_t2r4,
        t1r1_t1r2_t2r2_t1r4_t2r4,
        t1r1_t2r2,
        t1r1_t2r2_t4r4,
        nulltype
      } value;

      const char* to_string() const;
    };
    using supported_srs_tx_port_switch_v1610_e_ = enumerated<supported_srs_tx_port_switch_v1610_opts>;

    // member variables
    supported_srs_tx_port_switch_v1610_e_ supported_srs_tx_port_switch_v1610;
  };

  // member variables
  bool                   srs_tx_switch_v1610_present = false;
  srs_tx_switch_v1610_s_ srs_tx_switch_v1610;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersEUTRA-v1560 ::= SEQUENCE
struct ca_params_eutra_v1560_s {
  bool    fd_mimo_total_weighted_layers_present = false;
  uint8_t fd_mimo_total_weighted_layers         = 2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersEUTRA-v1570 ::= SEQUENCE
struct ca_params_eutra_v1570_s {
  bool    dl_1024_qam_total_weighted_layers_present = false;
  uint8_t dl_1024_qam_total_weighted_layers         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNR-v1610 ::= SEQUENCE
struct ca_params_nr_v1610_s {
  struct cross_carrier_a_csi_trig_diff_scs_r16_opts {
    enum options { higher_a_csi_scs, lower_a_csi_scs, both, nulltype } value;

    const char* to_string() const;
  };
  using cross_carrier_a_csi_trig_diff_scs_r16_e_ = enumerated<cross_carrier_a_csi_trig_diff_scs_r16_opts>;
  struct default_qcl_cross_carrier_a_csi_trig_r16_opts {
    enum options { diff_only, both, nulltype } value;

    const char* to_string() const;
  };
  using default_qcl_cross_carrier_a_csi_trig_r16_e_ = enumerated<default_qcl_cross_carrier_a_csi_trig_r16_opts>;
  struct inter_freq_daps_r16_s_ {
    struct inter_freq_dyn_pwr_sharing_daps_r16_opts {
      enum options { short_value, long_value, nulltype } value;

      const char* to_string() const;
    };
    using inter_freq_dyn_pwr_sharing_daps_r16_e_ = enumerated<inter_freq_dyn_pwr_sharing_daps_r16_opts>;

    // member variables
    bool                                   inter_freq_async_daps_r16_present                         = false;
    bool                                   inter_freq_diff_scs_daps_r16_present                      = false;
    bool                                   inter_freq_multi_ul_tx_daps_r16_present                   = false;
    bool                                   inter_freq_semi_static_pwr_sharing_daps_mode1_r16_present = false;
    bool                                   inter_freq_semi_static_pwr_sharing_daps_mode2_r16_present = false;
    bool                                   inter_freq_dyn_pwr_sharing_daps_r16_present               = false;
    bool                                   inter_freq_ul_trans_cancellation_daps_r16_present         = false;
    inter_freq_dyn_pwr_sharing_daps_r16_e_ inter_freq_dyn_pwr_sharing_daps_r16;
  };
  struct pdcch_monitoring_ca_r16_s_ {
    struct supported_span_arrangement_r16_opts {
      enum options { aligned_only, aligned_and_non_aligned, nulltype } value;

      const char* to_string() const;
    };
    using supported_span_arrangement_r16_e_ = enumerated<supported_span_arrangement_r16_opts>;

    // member variables
    uint8_t                           max_nof_monitoring_cc_r16 = 2;
    supported_span_arrangement_r16_e_ supported_span_arrangement_r16;
  };
  struct pdcch_blind_detection_ca_mixed_r16_s_ {
    struct supported_span_arrangement_r16_opts {
      enum options { aligned_only, aligned_and_non_aligned, nulltype } value;

      const char* to_string() const;
    };
    using supported_span_arrangement_r16_e_ = enumerated<supported_span_arrangement_r16_opts>;

    // member variables
    uint8_t                           pdcch_blind_detection_ca1_r16 = 1;
    uint8_t                           pdcch_blind_detection_ca2_r16 = 1;
    supported_span_arrangement_r16_e_ supported_span_arrangement_r16;
  };
  struct pdcch_blind_detection_mcg_ue_mixed_r16_s_ {
    uint8_t pdcch_blind_detection_mcg_ue1_r16 = 0;
    uint8_t pdcch_blind_detection_mcg_ue2_r16 = 0;
  };
  struct pdcch_blind_detection_scg_ue_mixed_r16_s_ {
    uint8_t pdcch_blind_detection_scg_ue1_r16 = 0;
    uint8_t pdcch_blind_detection_scg_ue2_r16 = 0;
  };
  struct cross_carrier_sched_dl_diff_scs_r16_opts {
    enum options { low_to_high, high_to_low, both, nulltype } value;

    const char* to_string() const;
  };
  using cross_carrier_sched_dl_diff_scs_r16_e_ = enumerated<cross_carrier_sched_dl_diff_scs_r16_opts>;
  struct cross_carrier_sched_default_qcl_r16_opts {
    enum options { diff_only, both, nulltype } value;

    const char* to_string() const;
  };
  using cross_carrier_sched_default_qcl_r16_e_ = enumerated<cross_carrier_sched_default_qcl_r16_opts>;
  struct cross_carrier_sched_ul_diff_scs_r16_opts {
    enum options { low_to_high, high_to_low, both, nulltype } value;

    const char* to_string() const;
  };
  using cross_carrier_sched_ul_diff_scs_r16_e_ = enumerated<cross_carrier_sched_ul_diff_scs_r16_opts>;

  // member variables
  bool                                        parallel_tx_msg_a_srs_pucch_pusch_r16_present      = false;
  bool                                        msg_a_sul_r16_present                              = false;
  bool                                        joint_search_space_switch_across_cells_r16_present = false;
  bool                                        half_duplex_tdd_ca_same_scs_r16_present            = false;
  bool                                        scell_dormancy_within_active_time_r16_present      = false;
  bool                                        scell_dormancy_outside_active_time_r16_present     = false;
  bool                                        cross_carrier_a_csi_trig_diff_scs_r16_present      = false;
  bool                                        default_qcl_cross_carrier_a_csi_trig_r16_present   = false;
  bool                                        inter_ca_non_aligned_frame_r16_present             = false;
  bool                                        simul_srs_trans_bc_r16_present                     = false;
  bool                                        inter_freq_daps_r16_present                        = false;
  bool                                        codebook_params_per_bc_r16_present                 = false;
  bool                                        blind_detect_factor_r16_present                    = false;
  bool                                        pdcch_monitoring_ca_r16_present                    = false;
  bool                                        pdcch_blind_detection_ca_mixed_r16_present         = false;
  bool                                        pdcch_blind_detection_mcg_ue_r16_present           = false;
  bool                                        pdcch_blind_detection_scg_ue_r16_present           = false;
  bool                                        pdcch_blind_detection_mcg_ue_mixed_r16_present     = false;
  bool                                        pdcch_blind_detection_scg_ue_mixed_r16_present     = false;
  bool                                        cross_carrier_sched_dl_diff_scs_r16_present        = false;
  bool                                        cross_carrier_sched_default_qcl_r16_present        = false;
  bool                                        cross_carrier_sched_ul_diff_scs_r16_present        = false;
  bool                                        simul_srs_mimo_trans_bc_r16_present                = false;
  bool                                        codebook_params_addition_per_bc_r16_present        = false;
  bool                                        codebook_combo_params_addition_per_bc_r16_present  = false;
  cross_carrier_a_csi_trig_diff_scs_r16_e_    cross_carrier_a_csi_trig_diff_scs_r16;
  default_qcl_cross_carrier_a_csi_trig_r16_e_ default_qcl_cross_carrier_a_csi_trig_r16;
  inter_freq_daps_r16_s_                      inter_freq_daps_r16;
  codebook_params_v1610_s                     codebook_params_per_bc_r16;
  uint8_t                                     blind_detect_factor_r16 = 1;
  pdcch_monitoring_ca_r16_s_                  pdcch_monitoring_ca_r16;
  pdcch_blind_detection_ca_mixed_r16_s_       pdcch_blind_detection_ca_mixed_r16;
  uint8_t                                     pdcch_blind_detection_mcg_ue_r16 = 1;
  uint8_t                                     pdcch_blind_detection_scg_ue_r16 = 1;
  pdcch_blind_detection_mcg_ue_mixed_r16_s_   pdcch_blind_detection_mcg_ue_mixed_r16;
  pdcch_blind_detection_scg_ue_mixed_r16_s_   pdcch_blind_detection_scg_ue_mixed_r16;
  cross_carrier_sched_dl_diff_scs_r16_e_      cross_carrier_sched_dl_diff_scs_r16;
  cross_carrier_sched_default_qcl_r16_e_      cross_carrier_sched_default_qcl_r16;
  cross_carrier_sched_ul_diff_scs_r16_e_      cross_carrier_sched_ul_diff_scs_r16;
  codebook_params_addition_per_bc_r16_s       codebook_params_addition_per_bc_r16;
  codebook_combo_params_addition_per_bc_r16_s codebook_combo_params_addition_per_bc_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNRDC ::= SEQUENCE
struct ca_params_nrdc_s {
  bool                 ca_params_nr_for_dc_present        = false;
  bool                 ca_params_nr_for_dc_v1540_present  = false;
  bool                 ca_params_nr_for_dc_v1550_present  = false;
  bool                 ca_params_nr_for_dc_v1560_present  = false;
  bool                 feature_set_combination_dc_present = false;
  ca_params_nr_s       ca_params_nr_for_dc;
  ca_params_nr_v1540_s ca_params_nr_for_dc_v1540;
  ca_params_nr_v1550_s ca_params_nr_for_dc_v1550;
  ca_params_nr_v1560_s ca_params_nr_for_dc_v1560;
  uint16_t             feature_set_combination_dc = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNRDC-v1610 ::= SEQUENCE
struct ca_params_nrdc_v1610_s {
  struct intra_fr_nr_dc_dyn_pwr_sharing_r16_opts {
    enum options { short_value, long_value, nulltype } value;

    const char* to_string() const;
  };
  using intra_fr_nr_dc_dyn_pwr_sharing_r16_e_ = enumerated<intra_fr_nr_dc_dyn_pwr_sharing_r16_opts>;

  // member variables
  bool                                  intra_fr_nr_dc_pwr_sharing_mode1_r16_present = false;
  bool                                  intra_fr_nr_dc_pwr_sharing_mode2_r16_present = false;
  bool                                  intra_fr_nr_dc_dyn_pwr_sharing_r16_present   = false;
  bool                                  async_nrdc_r16_present                       = false;
  intra_fr_nr_dc_dyn_pwr_sharing_r16_e_ intra_fr_nr_dc_dyn_pwr_sharing_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRDC-Parameters-v1580 ::= SEQUENCE
struct mrdc_params_v1580_s {
  bool dyn_pwr_sharing_nedc_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRDC-Parameters-v1590 ::= SEQUENCE
struct mrdc_params_v1590_s {
  bool inter_band_contiguous_mrdc_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRDC-Parameters-v1620 ::= SEQUENCE
struct mrdc_params_v1620_s {
  struct max_ul_duty_cycle_inter_band_endc_tdd_pc2_r16_s_ {
    struct eutra_tdd_cfg0_r16_opts {
      enum options { n20, n40, n50, n60, n70, n80, n90, n100, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using eutra_tdd_cfg0_r16_e_ = enumerated<eutra_tdd_cfg0_r16_opts>;
    struct eutra_tdd_cfg1_r16_opts {
      enum options { n20, n40, n50, n60, n70, n80, n90, n100, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using eutra_tdd_cfg1_r16_e_ = enumerated<eutra_tdd_cfg1_r16_opts>;
    struct eutra_tdd_cfg2_r16_opts {
      enum options { n20, n40, n50, n60, n70, n80, n90, n100, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using eutra_tdd_cfg2_r16_e_ = enumerated<eutra_tdd_cfg2_r16_opts>;
    struct eutra_tdd_cfg3_r16_opts {
      enum options { n20, n40, n50, n60, n70, n80, n90, n100, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using eutra_tdd_cfg3_r16_e_ = enumerated<eutra_tdd_cfg3_r16_opts>;
    struct eutra_tdd_cfg4_r16_opts {
      enum options { n20, n40, n50, n60, n70, n80, n90, n100, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using eutra_tdd_cfg4_r16_e_ = enumerated<eutra_tdd_cfg4_r16_opts>;
    struct eutra_tdd_cfg5_r16_opts {
      enum options { n20, n40, n50, n60, n70, n80, n90, n100, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using eutra_tdd_cfg5_r16_e_ = enumerated<eutra_tdd_cfg5_r16_opts>;
    struct eutra_tdd_cfg6_r16_opts {
      enum options { n20, n40, n50, n60, n70, n80, n90, n100, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using eutra_tdd_cfg6_r16_e_ = enumerated<eutra_tdd_cfg6_r16_opts>;

    // member variables
    bool                  eutra_tdd_cfg0_r16_present = false;
    bool                  eutra_tdd_cfg1_r16_present = false;
    bool                  eutra_tdd_cfg2_r16_present = false;
    bool                  eutra_tdd_cfg3_r16_present = false;
    bool                  eutra_tdd_cfg4_r16_present = false;
    bool                  eutra_tdd_cfg5_r16_present = false;
    bool                  eutra_tdd_cfg6_r16_present = false;
    eutra_tdd_cfg0_r16_e_ eutra_tdd_cfg0_r16;
    eutra_tdd_cfg1_r16_e_ eutra_tdd_cfg1_r16;
    eutra_tdd_cfg2_r16_e_ eutra_tdd_cfg2_r16;
    eutra_tdd_cfg3_r16_e_ eutra_tdd_cfg3_r16;
    eutra_tdd_cfg4_r16_e_ eutra_tdd_cfg4_r16;
    eutra_tdd_cfg5_r16_e_ eutra_tdd_cfg5_r16;
    eutra_tdd_cfg6_r16_e_ eutra_tdd_cfg6_r16;
  };

  // member variables
  bool                                             max_ul_duty_cycle_inter_band_endc_tdd_pc2_r16_present = false;
  bool                                             tdm_restrict_tdd_endc_r16_present                     = false;
  bool                                             tdm_restrict_fdd_endc_r16_present                     = false;
  bool                                             single_ul_harq_offset_tdd_pcell_r16_present           = false;
  bool                                             tdm_restrict_dual_tx_fdd_endc_r16_present             = false;
  max_ul_duty_cycle_inter_band_endc_tdd_pc2_r16_s_ max_ul_duty_cycle_inter_band_endc_tdd_pc2_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v1540 ::= SEQUENCE
struct band_combination_v1540_s {
  using band_list_v1540_l_ = dyn_array<band_params_v1540_s>;

  // member variables
  bool                 ca_params_nr_v1540_present = false;
  band_list_v1540_l_   band_list_v1540;
  ca_params_nr_v1540_s ca_params_nr_v1540;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v1560 ::= SEQUENCE
struct band_combination_v1560_s {
  bool                    ne_dc_bc_present              = false;
  bool                    ca_params_nrdc_present        = false;
  bool                    ca_params_eutra_v1560_present = false;
  bool                    ca_params_nr_v1560_present    = false;
  ca_params_nrdc_s        ca_params_nrdc;
  ca_params_eutra_v1560_s ca_params_eutra_v1560;
  ca_params_nr_v1560_s    ca_params_nr_v1560;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v1570 ::= SEQUENCE
struct band_combination_v1570_s {
  ca_params_eutra_v1570_s ca_params_eutra_v1570;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v1580 ::= SEQUENCE
struct band_combination_v1580_s {
  mrdc_params_v1580_s mrdc_params_v1580;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v1590 ::= SEQUENCE
struct band_combination_v1590_s {
  bool                     supported_bw_combination_set_intra_endc_present = false;
  bounded_bitstring<1, 32> supported_bw_combination_set_intra_endc;
  mrdc_params_v1590_s      mrdc_params_v1590;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v1610 ::= SEQUENCE
struct band_combination_v1610_s {
  using band_list_v1610_l_ = dyn_array<band_params_v1610_s>;
  struct pwr_class_nr_part_r16_opts {
    enum options { pc1, pc2, pc3, pc5, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using pwr_class_nr_part_r16_e_ = enumerated<pwr_class_nr_part_r16_opts>;

  // member variables
  bool                     ca_params_nr_v1610_present               = false;
  bool                     ca_params_nrdc_v1610_present             = false;
  bool                     pwr_class_v1610_present                  = false;
  bool                     pwr_class_nr_part_r16_present            = false;
  bool                     feature_set_combination_daps_r16_present = false;
  bool                     mrdc_params_v1620_present                = false;
  band_list_v1610_l_       band_list_v1610;
  ca_params_nr_v1610_s     ca_params_nr_v1610;
  ca_params_nrdc_v1610_s   ca_params_nrdc_v1610;
  pwr_class_nr_part_r16_e_ pwr_class_nr_part_r16;
  uint16_t                 feature_set_combination_daps_r16 = 0;
  mrdc_params_v1620_s      mrdc_params_v1620;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULTxSwitchingBandPair-r16 ::= SEQUENCE
struct ul_tx_switching_band_pair_r16_s {
  struct ul_tx_switching_period_r16_opts {
    enum options { n35us, n140us, n210us, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ul_tx_switching_period_r16_e_ = enumerated<ul_tx_switching_period_r16_opts>;

  // member variables
  bool                          ul_tx_switching_dl_interruption_r16_present = false;
  uint8_t                       band_idx_ul1_r16                            = 1;
  uint8_t                       band_idx_ul2_r16                            = 1;
  ul_tx_switching_period_r16_e_ ul_tx_switching_period_r16;
  bounded_bitstring<1, 32>      ul_tx_switching_dl_interruption_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-UplinkTxSwitch-r16 ::= SEQUENCE
struct band_combination_ul_tx_switch_r16_s {
  using supported_band_pair_list_nr_r16_l_ = dyn_array<ul_tx_switching_band_pair_r16_s>;
  struct ul_tx_switching_option_support_r16_opts {
    enum options { switched_ul, dual_ul, both, nulltype } value;

    const char* to_string() const;
  };
  using ul_tx_switching_option_support_r16_e_ = enumerated<ul_tx_switching_option_support_r16_opts>;
  struct ul_tx_switching_pusch_trans_coherence_r16_opts {
    enum options { non_coherent, full_coherent, nulltype } value;

    const char* to_string() const;
  };
  using ul_tx_switching_pusch_trans_coherence_r16_e_ = enumerated<ul_tx_switching_pusch_trans_coherence_r16_opts>;

  // member variables
  bool                                  ext                                        = false;
  bool                                  band_combination_v1540_present             = false;
  bool                                  band_combination_v1560_present             = false;
  bool                                  band_combination_v1570_present             = false;
  bool                                  band_combination_v1580_present             = false;
  bool                                  band_combination_v1590_present             = false;
  bool                                  band_combination_v1610_present             = false;
  bool                                  ul_tx_switching_option_support_r16_present = false;
  bool                                  ul_tx_switching_pwr_boosting_r16_present   = false;
  band_combination_s                    band_combination_r16;
  band_combination_v1540_s              band_combination_v1540;
  band_combination_v1560_s              band_combination_v1560;
  band_combination_v1570_s              band_combination_v1570;
  band_combination_v1580_s              band_combination_v1580;
  band_combination_v1590_s              band_combination_v1590;
  band_combination_v1610_s              band_combination_v1610;
  supported_band_pair_list_nr_r16_l_    supported_band_pair_list_nr_r16;
  ul_tx_switching_option_support_r16_e_ ul_tx_switching_option_support_r16;
  // ...
  // group 0
  bool                                         ul_tx_switching_pusch_trans_coherence_r16_present = false;
  ul_tx_switching_pusch_trans_coherence_r16_e_ ul_tx_switching_pusch_trans_coherence_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SimulSRS-ForAntennaSwitching-r16 ::= SEQUENCE
struct simul_srs_for_ant_switching_r16_s {
  bool support_srs_x_ty_r_x_less_than_y_r16_present = false;
  bool support_srs_x_ty_r_x_equal_to_y_r16_present  = false;
  bool support_srs_ant_switching_r16_present        = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNR-v1630 ::= SEQUENCE
struct ca_params_nr_v1630_s {
  struct beam_management_type_r16_opts {
    enum options { ibm, dummy, nulltype } value;

    const char* to_string() const;
  };
  using beam_management_type_r16_e_ = enumerated<beam_management_type_r16_opts>;
  struct intra_band_freq_separation_ul_agg_bw_gap_bw_r16_opts {
    enum options { class_i, class_ii, class_iii, nulltype } value;

    const char* to_string() const;
  };
  using intra_band_freq_separation_ul_agg_bw_gap_bw_r16_e_ =
      enumerated<intra_band_freq_separation_ul_agg_bw_gap_bw_r16_opts>;

  // member variables
  bool                                               simul_tx_srs_ant_switching_inter_band_ul_ca_r16_present = false;
  bool                                               beam_management_type_r16_present                        = false;
  bool                                               intra_band_freq_separation_ul_agg_bw_gap_bw_r16_present = false;
  bool                                               inter_ca_non_aligned_frame_b_r16_present                = false;
  simul_srs_for_ant_switching_r16_s                  simul_tx_srs_ant_switching_inter_band_ul_ca_r16;
  beam_management_type_r16_e_                        beam_management_type_r16;
  intra_band_freq_separation_ul_agg_bw_gap_bw_r16_e_ intra_band_freq_separation_ul_agg_bw_gap_bw_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNRDC-v1630 ::= SEQUENCE
struct ca_params_nrdc_v1630_s {
  bool                 ca_params_nr_for_dc_v1610_present = false;
  bool                 ca_params_nr_for_dc_v1630_present = false;
  ca_params_nr_v1610_s ca_params_nr_for_dc_v1610;
  ca_params_nr_v1630_s ca_params_nr_for_dc_v1630;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRDC-Parameters-v1630 ::= SEQUENCE
struct mrdc_params_v1630_s {
  struct max_ul_duty_cycle_inter_band_endc_fdd_tdd_pc2_r16_s_ {
    struct max_ul_duty_cycle_fdd_tdd_en_dc1_r16_opts {
      enum options { n30, n40, n50, n60, n70, n80, n90, n100, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_ul_duty_cycle_fdd_tdd_en_dc1_r16_e_ = enumerated<max_ul_duty_cycle_fdd_tdd_en_dc1_r16_opts>;
    struct max_ul_duty_cycle_fdd_tdd_en_dc2_r16_opts {
      enum options { n30, n40, n50, n60, n70, n80, n90, n100, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_ul_duty_cycle_fdd_tdd_en_dc2_r16_e_ = enumerated<max_ul_duty_cycle_fdd_tdd_en_dc2_r16_opts>;

    // member variables
    bool                                    max_ul_duty_cycle_fdd_tdd_en_dc1_r16_present = false;
    bool                                    max_ul_duty_cycle_fdd_tdd_en_dc2_r16_present = false;
    max_ul_duty_cycle_fdd_tdd_en_dc1_r16_e_ max_ul_duty_cycle_fdd_tdd_en_dc1_r16;
    max_ul_duty_cycle_fdd_tdd_en_dc2_r16_e_ max_ul_duty_cycle_fdd_tdd_en_dc2_r16;
  };

  // member variables
  bool max_ul_duty_cycle_inter_band_endc_fdd_tdd_pc2_r16_present = false;
  bool inter_band_mrdc_with_overlap_dl_bands_r16_present         = false;
  max_ul_duty_cycle_inter_band_endc_fdd_tdd_pc2_r16_s_ max_ul_duty_cycle_inter_band_endc_fdd_tdd_pc2_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ScalingFactorSidelink-r16 ::= ENUMERATED
struct scaling_factor_sidelink_r16_opts {
  enum options { f0p4, f0p75, f0p8, f1, nulltype } value;

  const char* to_string() const;
};
using scaling_factor_sidelink_r16_e = enumerated<scaling_factor_sidelink_r16_opts>;

// BandCombination-v1630 ::= SEQUENCE
struct band_combination_v1630_s {
  using scaling_factor_tx_sidelink_r16_l_ = dyn_array<scaling_factor_sidelink_r16_e>;
  using scaling_factor_rx_sidelink_r16_l_ = dyn_array<scaling_factor_sidelink_r16_e>;

  // member variables
  bool                              ca_params_nr_v1630_present                              = false;
  bool                              ca_params_nrdc_v1630_present                            = false;
  bool                              mrdc_params_v1630_present                               = false;
  bool                              supported_tx_band_comb_list_per_bc_sidelink_r16_present = false;
  bool                              supported_rx_band_comb_list_per_bc_sidelink_r16_present = false;
  ca_params_nr_v1630_s              ca_params_nr_v1630;
  ca_params_nrdc_v1630_s            ca_params_nrdc_v1630;
  mrdc_params_v1630_s               mrdc_params_v1630;
  bounded_bitstring<1, 65536>       supported_tx_band_comb_list_per_bc_sidelink_r16;
  bounded_bitstring<1, 65536>       supported_rx_band_comb_list_per_bc_sidelink_r16;
  scaling_factor_tx_sidelink_r16_l_ scaling_factor_tx_sidelink_r16;
  scaling_factor_rx_sidelink_r16_l_ scaling_factor_rx_sidelink_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-UplinkTxSwitch-v1630 ::= SEQUENCE
struct band_combination_ul_tx_switch_v1630_s {
  bool                     band_combination_v1630_present = false;
  band_combination_v1630_s band_combination_v1630;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-Grp-CarrierTypes-r16 ::= SEQUENCE
struct pucch_grp_carrier_types_r16_s {
  bool fr1_non_shared_tdd_r16_present = false;
  bool fr1_shared_tdd_r16_present     = false;
  bool fr1_non_shared_fdd_r16_present = false;
  bool fr2_r16_present                = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TwoPUCCH-Grp-ConfigParams-r16 ::= SEQUENCE
struct two_pucch_grp_cfg_params_r16_s {
  pucch_grp_carrier_types_r16_s pucch_group_map_r16;
  pucch_grp_carrier_types_r16_s pucch_tx_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TwoPUCCH-Grp-Configurations-r16 ::= SEQUENCE
struct two_pucch_grp_cfgs_r16_s {
  two_pucch_grp_cfg_params_r16_s pucch_primary_group_map_r16;
  two_pucch_grp_cfg_params_r16_s pucch_secondary_group_map_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNR-v1640 ::= SEQUENCE
struct ca_params_nr_v1640_s {
  using two_pucch_grp_cfgs_list_r16_l_ = dyn_array<two_pucch_grp_cfgs_r16_s>;
  struct pdcch_blind_detection_ca_mixed_non_aligned_span_r16_s_ {
    uint8_t pdcch_blind_detection_ca1_r16 = 1;
    uint8_t pdcch_blind_detection_ca2_r16 = 1;
  };

  // member variables
  bool                           ul_tx_dc_two_carrier_report_r16_present                                  = false;
  bool                           max_up_to3_diff_numerologies_cfg_single_pucch_grp_r16_present            = false;
  bool                           max_up_to4_diff_numerologies_cfg_single_pucch_grp_r16_present            = false;
  bool                           diff_numerology_across_pucch_group_carrier_types_r16_present             = false;
  bool                           diff_numerology_within_pucch_group_smaller_scs_carrier_types_r16_present = false;
  bool                           diff_numerology_within_pucch_group_larger_scs_carrier_types_r16_present  = false;
  bool                           pdcch_monitoring_ca_non_aligned_span_r16_present                         = false;
  bool                           pdcch_blind_detection_ca_mixed_non_aligned_span_r16_present              = false;
  pucch_grp_carrier_types_r16_s  max_up_to3_diff_numerologies_cfg_single_pucch_grp_r16;
  pucch_grp_carrier_types_r16_s  max_up_to4_diff_numerologies_cfg_single_pucch_grp_r16;
  two_pucch_grp_cfgs_list_r16_l_ two_pucch_grp_cfgs_list_r16;
  uint8_t                        pdcch_monitoring_ca_non_aligned_span_r16 = 2;
  pdcch_blind_detection_ca_mixed_non_aligned_span_r16_s_ pdcch_blind_detection_ca_mixed_non_aligned_span_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNRDC-v1640 ::= SEQUENCE
struct ca_params_nrdc_v1640_s {
  bool                 ca_params_nr_for_dc_v1640_present = false;
  ca_params_nr_v1640_s ca_params_nr_for_dc_v1640;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v1640 ::= SEQUENCE
struct band_combination_v1640_s {
  bool                   ca_params_nr_v1640_present   = false;
  bool                   ca_params_nrdc_v1640_present = false;
  ca_params_nr_v1640_s   ca_params_nr_v1640;
  ca_params_nrdc_v1640_s ca_params_nrdc_v1640;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-UplinkTxSwitch-v1640 ::= SEQUENCE
struct band_combination_ul_tx_switch_v1640_s {
  bool                     band_combination_v1640_present = false;
  band_combination_v1640_s band_combination_v1640;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNRDC-v1650 ::= SEQUENCE
struct ca_params_nrdc_v1650_s {
  bool                     supported_cell_grouping_r16_present = false;
  bounded_bitstring<1, 32> supported_cell_grouping_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v1650 ::= SEQUENCE
struct band_combination_v1650_s {
  bool                   ca_params_nrdc_v1650_present = false;
  ca_params_nrdc_v1650_s ca_params_nrdc_v1650;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-UplinkTxSwitch-v1650 ::= SEQUENCE
struct band_combination_ul_tx_switch_v1650_s {
  bool                     band_combination_v1650_present = false;
  band_combination_v1650_s band_combination_v1650;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNR-v15g0 ::= SEQUENCE
struct ca_params_nr_v15g0_s {
  bool                      simul_rx_tx_inter_band_ca_per_band_pair_present = false;
  bool                      simul_rx_tx_sul_per_band_pair_present           = false;
  bounded_bitstring<3, 496> simul_rx_tx_inter_band_ca_per_band_pair;
  bounded_bitstring<3, 496> simul_rx_tx_sul_per_band_pair;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNRDC-v15g0 ::= SEQUENCE
struct ca_params_nrdc_v15g0_s {
  bool                 ca_params_nr_for_dc_v15g0_present = false;
  ca_params_nr_v15g0_s ca_params_nr_for_dc_v15g0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRDC-Parameters-v15g0 ::= SEQUENCE
struct mrdc_params_v15g0_s {
  bool                      simul_rx_tx_inter_band_end_cp_er_band_pair_present = false;
  bounded_bitstring<3, 496> simul_rx_tx_inter_band_end_cp_er_band_pair;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v15g0 ::= SEQUENCE
struct band_combination_v15g0_s {
  bool                   ca_params_nr_v15g0_present   = false;
  bool                   ca_params_nrdc_v15g0_present = false;
  bool                   mrdc_params_v15g0_present    = false;
  ca_params_nr_v15g0_s   ca_params_nr_v15g0;
  ca_params_nrdc_v15g0_s ca_params_nrdc_v15g0;
  mrdc_params_v15g0_s    mrdc_params_v15g0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-UplinkTxSwitch-v1670 ::= SEQUENCE
struct band_combination_ul_tx_switch_v1670_s {
  bool                     band_combination_v15g0_present = false;
  band_combination_v15g0_s band_combination_v15g0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierTypePair-r16 ::= SEQUENCE
struct carrier_type_pair_r16_s {
  pucch_grp_carrier_types_r16_s carrier_for_csi_meas_r16;
  pucch_grp_carrier_types_r16_s carrier_for_csi_report_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNR-v1690 ::= SEQUENCE
struct ca_params_nr_v1690_s {
  struct csi_report_cross_pucch_grp_r16_s_ {
    struct computation_time_for_a_csi_r16_opts {
      enum options { same_as_no_cross, relaxed, nulltype } value;

      const char* to_string() const;
    };
    using computation_time_for_a_csi_r16_e_ = enumerated<computation_time_for_a_csi_r16_opts>;
    struct add_symbols_r16_s_ {
      struct scs_15k_hz_add_symbols_r16_opts {
        enum options { s14, s28, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using scs_15k_hz_add_symbols_r16_e_ = enumerated<scs_15k_hz_add_symbols_r16_opts>;
      struct scs_30k_hz_add_symbols_r16_opts {
        enum options { s14, s28, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using scs_30k_hz_add_symbols_r16_e_ = enumerated<scs_30k_hz_add_symbols_r16_opts>;
      struct scs_60k_hz_add_symbols_r16_opts {
        enum options { s14, s28, s56, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using scs_60k_hz_add_symbols_r16_e_ = enumerated<scs_60k_hz_add_symbols_r16_opts>;
      struct scs_120k_hz_add_symbols_r16_opts {
        enum options { s14, s28, s56, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using scs_120k_hz_add_symbols_r16_e_ = enumerated<scs_120k_hz_add_symbols_r16_opts>;

      // member variables
      bool                           scs_15k_hz_add_symbols_r16_present  = false;
      bool                           scs_30k_hz_add_symbols_r16_present  = false;
      bool                           scs_60k_hz_add_symbols_r16_present  = false;
      bool                           scs_120k_hz_add_symbols_r16_present = false;
      scs_15k_hz_add_symbols_r16_e_  scs_15k_hz_add_symbols_r16;
      scs_30k_hz_add_symbols_r16_e_  scs_30k_hz_add_symbols_r16;
      scs_60k_hz_add_symbols_r16_e_  scs_60k_hz_add_symbols_r16;
      scs_120k_hz_add_symbols_r16_e_ scs_120k_hz_add_symbols_r16;
    };
    using carrier_type_pair_list_r16_l_ = dyn_array<carrier_type_pair_r16_s>;

    // member variables
    bool                              add_symbols_r16_present            = false;
    bool                              sp_csi_report_on_pucch_r16_present = false;
    bool                              sp_csi_report_on_pusch_r16_present = false;
    computation_time_for_a_csi_r16_e_ computation_time_for_a_csi_r16;
    add_symbols_r16_s_                add_symbols_r16;
    carrier_type_pair_list_r16_l_     carrier_type_pair_list_r16;
  };

  // member variables
  bool                              csi_report_cross_pucch_grp_r16_present = false;
  csi_report_cross_pucch_grp_r16_s_ csi_report_cross_pucch_grp_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v1690 ::= SEQUENCE
struct band_combination_v1690_s {
  bool                 ca_params_nr_v1690_present = false;
  ca_params_nr_v1690_s ca_params_nr_v1690;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-UplinkTxSwitch-v1690 ::= SEQUENCE
struct band_combination_ul_tx_switch_v1690_s {
  bool                     band_combination_v1690_present = false;
  band_combination_v1690_s band_combination_v1690;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-MultiTRP-SupportedCombinations-r17 ::= SEQUENCE
struct csi_multi_trp_supported_combinations_r17_s {
  struct max_num_tx_ports_r17_opts {
    enum options { n2, n4, n8, n12, n16, n24, n32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_tx_ports_r17_e_ = enumerated<max_num_tx_ports_r17_opts>;

  // member variables
  max_num_tx_ports_r17_e_ max_num_tx_ports_r17;
  uint8_t                 max_total_num_cmr_r17                 = 2;
  uint16_t                max_total_num_tx_ports_nzp_csi_rs_r17 = 2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookComboParameterMixedTypePerBC-r17 ::= SEQUENCE
struct codebook_combo_param_mixed_type_per_bc_r17_s {
  using type1_sp_fe_type2_ps_null_r17_l_             = bounded_array<uint16_t, 16>;
  using type1_sp_fe_type2_ps_m2_r1_null_r17_l_       = bounded_array<uint16_t, 16>;
  using type1_sp_fe_type2_ps_m2_r2_null_r17_l_       = bounded_array<uint16_t, 16>;
  using type1_sp_type2_fe_type2_ps_m1_r17_l_         = bounded_array<uint16_t, 16>;
  using type1_sp_type2_fe_type2_ps_m2_r1_r17_l_      = bounded_array<uint16_t, 16>;
  using type1_sp_e_type2_r1_fe_type2_ps_m1_r17_l_    = bounded_array<uint16_t, 16>;
  using type1_sp_e_type2_r1_fe_type2_ps_m2_r1_r17_l_ = bounded_array<uint16_t, 16>;
  using type1_mp_fe_type2_ps_null_r17_l_             = bounded_array<uint16_t, 16>;
  using type1_mp_fe_type2_ps_m2_r1_null_r17_l_       = bounded_array<uint16_t, 16>;
  using type1_mp_fe_type2_ps_m2_r2_null_r17_l_       = bounded_array<uint16_t, 16>;
  using type1_mp_type2_fe_type2_ps_m1_r17_l_         = bounded_array<uint16_t, 16>;
  using type1_mp_type2_fe_type2_ps_m2_r1_r17_l_      = bounded_array<uint16_t, 16>;
  using type1_mp_e_type2_r1_fe_type2_ps_m1_r17_l_    = bounded_array<uint16_t, 16>;
  using type1_mp_e_type2_r1_fe_type2_ps_m2_r1_r17_l_ = bounded_array<uint16_t, 16>;

  // member variables
  type1_sp_fe_type2_ps_null_r17_l_             type1_sp_fe_type2_ps_null_r17;
  type1_sp_fe_type2_ps_m2_r1_null_r17_l_       type1_sp_fe_type2_ps_m2_r1_null_r17;
  type1_sp_fe_type2_ps_m2_r2_null_r17_l_       type1_sp_fe_type2_ps_m2_r2_null_r17;
  type1_sp_type2_fe_type2_ps_m1_r17_l_         type1_sp_type2_fe_type2_ps_m1_r17;
  type1_sp_type2_fe_type2_ps_m2_r1_r17_l_      type1_sp_type2_fe_type2_ps_m2_r1_r17;
  type1_sp_e_type2_r1_fe_type2_ps_m1_r17_l_    type1_sp_e_type2_r1_fe_type2_ps_m1_r17;
  type1_sp_e_type2_r1_fe_type2_ps_m2_r1_r17_l_ type1_sp_e_type2_r1_fe_type2_ps_m2_r1_r17;
  type1_mp_fe_type2_ps_null_r17_l_             type1_mp_fe_type2_ps_null_r17;
  type1_mp_fe_type2_ps_m2_r1_null_r17_l_       type1_mp_fe_type2_ps_m2_r1_null_r17;
  type1_mp_fe_type2_ps_m2_r2_null_r17_l_       type1_mp_fe_type2_ps_m2_r2_null_r17;
  type1_mp_type2_fe_type2_ps_m1_r17_l_         type1_mp_type2_fe_type2_ps_m1_r17;
  type1_mp_type2_fe_type2_ps_m2_r1_r17_l_      type1_mp_type2_fe_type2_ps_m2_r1_r17;
  type1_mp_e_type2_r1_fe_type2_ps_m1_r17_l_    type1_mp_e_type2_r1_fe_type2_ps_m1_r17;
  type1_mp_e_type2_r1_fe_type2_ps_m2_r1_r17_l_ type1_mp_e_type2_r1_fe_type2_ps_m2_r1_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookComboParameterMultiTRP-PerBC-r17 ::= SEQUENCE
struct codebook_combo_param_multi_trp_per_bc_r17_s {
  using ncjt_null_null_l_                            = bounded_array<uint16_t, 16>;
  using ncjt1_sp_null_null_l_                        = bounded_array<uint16_t, 16>;
  using ncjt_type2_null_r16_l_                       = bounded_array<uint16_t, 16>;
  using ncjt_type2_ps_null_r16_l_                    = bounded_array<uint16_t, 16>;
  using ncjt_e_type2_r1_null_r16_l_                  = bounded_array<uint16_t, 16>;
  using ncjt_e_type2_r2_null_r16_l_                  = bounded_array<uint16_t, 16>;
  using ncjt_e_type2_r1_ps_null_r16_l_               = bounded_array<uint16_t, 16>;
  using ncjt_e_type2_r2_ps_null_r16_l_               = bounded_array<uint16_t, 16>;
  using ncjt_type2_type2_ps_r16_l_                   = bounded_array<uint16_t, 16>;
  using ncjt1_sp_type2_null_r16_l_                   = bounded_array<uint16_t, 16>;
  using ncjt1_sp_type2_ps_null_r16_l_                = bounded_array<uint16_t, 16>;
  using ncjt1_sp_e_type2_r1_null_r16_l_              = bounded_array<uint16_t, 16>;
  using ncjt1_sp_e_type2_r2_null_r16_l_              = bounded_array<uint16_t, 16>;
  using ncjt1_sp_e_type2_r1_ps_null_r16_l_           = bounded_array<uint16_t, 16>;
  using ncjt1_sp_e_type2_r2_ps_null_r16_l_           = bounded_array<uint16_t, 16>;
  using ncjt1_sp_type2_type2_ps_r16_l_               = bounded_array<uint16_t, 16>;
  using ncjt_fe_type2_ps_null_r17_l_                 = bounded_array<uint16_t, 16>;
  using ncjt_fe_type2_ps_m2_r1_null_r17_l_           = bounded_array<uint16_t, 16>;
  using ncjt_fe_type2_ps_m2_r2_null_r17_l_           = bounded_array<uint16_t, 16>;
  using ncjt_type2_fe_type2_ps_m1_r17_l_             = bounded_array<uint16_t, 16>;
  using ncjt_type2_fe_type2_ps_m2_r1_r17_l_          = bounded_array<uint16_t, 16>;
  using ncjt_e_type2_r1_fe_type2_ps_m1_r17_l_        = bounded_array<uint16_t, 16>;
  using ncjt_e_type2_r1_fe_type2_ps_m2_r1_r17_l_     = bounded_array<uint16_t, 16>;
  using ncjt1_sp_fe_type2_ps_null_r17_l_             = bounded_array<uint16_t, 16>;
  using ncjt1_sp_fe_type2_ps_m2_r1_null_r17_l_       = bounded_array<uint16_t, 16>;
  using ncjt1_sp_fe_type2_ps_m2_r2_null_r1_l_        = bounded_array<uint16_t, 16>;
  using ncjt1_sp_type2_fe_type2_ps_m1_r17_l_         = bounded_array<uint16_t, 16>;
  using ncjt1_sp_type2_fe_type2_ps_m2_r1_r17_l_      = bounded_array<uint16_t, 16>;
  using ncjt1_sp_e_type2_r1_fe_type2_ps_m1_r17_l_    = bounded_array<uint16_t, 16>;
  using ncjt1_sp_e_type2_r1_fe_type2_ps_m2_r1_r17_l_ = bounded_array<uint16_t, 16>;

  // member variables
  ncjt_null_null_l_                            ncjt_null_null;
  ncjt1_sp_null_null_l_                        ncjt1_sp_null_null;
  ncjt_type2_null_r16_l_                       ncjt_type2_null_r16;
  ncjt_type2_ps_null_r16_l_                    ncjt_type2_ps_null_r16;
  ncjt_e_type2_r1_null_r16_l_                  ncjt_e_type2_r1_null_r16;
  ncjt_e_type2_r2_null_r16_l_                  ncjt_e_type2_r2_null_r16;
  ncjt_e_type2_r1_ps_null_r16_l_               ncjt_e_type2_r1_ps_null_r16;
  ncjt_e_type2_r2_ps_null_r16_l_               ncjt_e_type2_r2_ps_null_r16;
  ncjt_type2_type2_ps_r16_l_                   ncjt_type2_type2_ps_r16;
  ncjt1_sp_type2_null_r16_l_                   ncjt1_sp_type2_null_r16;
  ncjt1_sp_type2_ps_null_r16_l_                ncjt1_sp_type2_ps_null_r16;
  ncjt1_sp_e_type2_r1_null_r16_l_              ncjt1_sp_e_type2_r1_null_r16;
  ncjt1_sp_e_type2_r2_null_r16_l_              ncjt1_sp_e_type2_r2_null_r16;
  ncjt1_sp_e_type2_r1_ps_null_r16_l_           ncjt1_sp_e_type2_r1_ps_null_r16;
  ncjt1_sp_e_type2_r2_ps_null_r16_l_           ncjt1_sp_e_type2_r2_ps_null_r16;
  ncjt1_sp_type2_type2_ps_r16_l_               ncjt1_sp_type2_type2_ps_r16;
  ncjt_fe_type2_ps_null_r17_l_                 ncjt_fe_type2_ps_null_r17;
  ncjt_fe_type2_ps_m2_r1_null_r17_l_           ncjt_fe_type2_ps_m2_r1_null_r17;
  ncjt_fe_type2_ps_m2_r2_null_r17_l_           ncjt_fe_type2_ps_m2_r2_null_r17;
  ncjt_type2_fe_type2_ps_m1_r17_l_             ncjt_type2_fe_type2_ps_m1_r17;
  ncjt_type2_fe_type2_ps_m2_r1_r17_l_          ncjt_type2_fe_type2_ps_m2_r1_r17;
  ncjt_e_type2_r1_fe_type2_ps_m1_r17_l_        ncjt_e_type2_r1_fe_type2_ps_m1_r17;
  ncjt_e_type2_r1_fe_type2_ps_m2_r1_r17_l_     ncjt_e_type2_r1_fe_type2_ps_m2_r1_r17;
  ncjt1_sp_fe_type2_ps_null_r17_l_             ncjt1_sp_fe_type2_ps_null_r17;
  ncjt1_sp_fe_type2_ps_m2_r1_null_r17_l_       ncjt1_sp_fe_type2_ps_m2_r1_null_r17;
  ncjt1_sp_fe_type2_ps_m2_r2_null_r1_l_        ncjt1_sp_fe_type2_ps_m2_r2_null_r1;
  ncjt1_sp_type2_fe_type2_ps_m1_r17_l_         ncjt1_sp_type2_fe_type2_ps_m1_r17;
  ncjt1_sp_type2_fe_type2_ps_m2_r1_r17_l_      ncjt1_sp_type2_fe_type2_ps_m2_r1_r17;
  ncjt1_sp_e_type2_r1_fe_type2_ps_m1_r17_l_    ncjt1_sp_e_type2_r1_fe_type2_ps_m1_r17;
  ncjt1_sp_e_type2_r1_fe_type2_ps_m2_r1_r17_l_ ncjt1_sp_e_type2_r1_fe_type2_ps_m2_r1_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookParametersfetype2PerBC-r17 ::= SEQUENCE
struct codebook_paramsfetype2_per_bc_r17_s {
  using fetype2basic_r17_l_ = bounded_array<uint16_t, 16>;
  using fetype2_r1_r17_l_   = bounded_array<uint16_t, 8>;
  using fetype2_r2_r17_l_   = bounded_array<uint16_t, 8>;

  // member variables
  fetype2basic_r17_l_ fetype2basic_r17;
  fetype2_r1_r17_l_   fetype2_r1_r17;
  fetype2_r2_r17_l_   fetype2_r2_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CrossCarrierSchedulingSCell-SpCell-r17 ::= SEQUENCE
struct cross_carrier_sched_scell_sp_cell_r17_s {
  struct supported_scs_combinations_r17_s_ {
    bool                      scs15k_hz_15k_hz_r17_present = false;
    bool                      scs15k_hz_30k_hz_r17_present = false;
    bool                      scs15k_hz_60k_hz_r17_present = false;
    bool                      scs30k_hz_30k_hz_r17_present = false;
    bool                      scs30k_hz_60k_hz_r17_present = false;
    bool                      scs60k_hz_60k_hz_r17_present = false;
    bounded_bitstring<1, 496> scs30k_hz_30k_hz_r17;
    bounded_bitstring<1, 496> scs30k_hz_60k_hz_r17;
    bounded_bitstring<1, 496> scs60k_hz_60k_hz_r17;
  };
  struct pdcch_monitoring_occasion_r17_opts {
    enum options { val1, val2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using pdcch_monitoring_occasion_r17_e_ = enumerated<pdcch_monitoring_occasion_r17_opts>;

  // member variables
  supported_scs_combinations_r17_s_ supported_scs_combinations_r17;
  pdcch_monitoring_occasion_r17_e_  pdcch_monitoring_occasion_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParameters-v1710 ::= SEQUENCE
struct band_params_v1710_s {
  struct srs_ant_switching_beyond4_rx_r17_s_ {
    bool                entry_num_affect_beyond4_rx_r17_present = false;
    bool                entry_num_switch_beyond4_rx_r17_present = false;
    fixed_bitstring<11> supported_srs_tx_port_switch_beyond4_rx_r17;
    uint8_t             entry_num_affect_beyond4_rx_r17 = 1;
    uint8_t             entry_num_switch_beyond4_rx_r17 = 1;
  };

  // member variables
  bool                                srs_ant_switching_beyond4_rx_r17_present = false;
  srs_ant_switching_beyond4_rx_r17_s_ srs_ant_switching_beyond4_rx_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNR-v1700 ::= SEQUENCE
struct ca_params_nr_v1700_s {
  struct max_ul_duty_cycle_inter_band_ca_pc2_r17_opts {
    enum options { n50, n60, n70, n80, n90, n100, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_ul_duty_cycle_inter_band_ca_pc2_r17_e_ = enumerated<max_ul_duty_cycle_inter_band_ca_pc2_r17_opts>;
  struct max_ul_duty_cycle_sul_combination_pc2_r17_opts {
    enum options { n50, n60, n70, n80, n90, n100, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_ul_duty_cycle_sul_combination_pc2_r17_e_ = enumerated<max_ul_duty_cycle_sul_combination_pc2_r17_opts>;
  struct mtrp_csi_enhancement_per_bc_r17_s_ {
    struct csi_report_mode_r17_opts {
      enum options { mode1, mode2, both, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using csi_report_mode_r17_e_            = enumerated<csi_report_mode_r17_opts>;
    using supported_combo_across_ccs_r17_l_ = dyn_array<csi_multi_trp_supported_combinations_r17_s>;
    struct codebook_mode_ncjt_r17_opts {
      enum options { mode1, mode1_and2, nulltype } value;

      const char* to_string() const;
    };
    using codebook_mode_ncjt_r17_e_ = enumerated<codebook_mode_ncjt_r17_opts>;

    // member variables
    uint8_t                           max_num_nzp_csi_rs_r17 = 2;
    csi_report_mode_r17_e_            csi_report_mode_r17;
    supported_combo_across_ccs_r17_l_ supported_combo_across_ccs_r17;
    codebook_mode_ncjt_r17_e_         codebook_mode_ncjt_r17;
  };
  struct max_cc_32_dl_harq_process_fr2_2_r17_opts {
    enum options { n1, n2, n3, n4, n6, n8, n16, n32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_cc_32_dl_harq_process_fr2_2_r17_e_ = enumerated<max_cc_32_dl_harq_process_fr2_2_r17_opts>;
  struct max_cc_32_ul_harq_process_fr2_2_r17_opts {
    enum options { n1, n2, n3, n4, n5, n8, n16, n32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_cc_32_ul_harq_process_fr2_2_r17_e_ = enumerated<max_cc_32_ul_harq_process_fr2_2_r17_opts>;
  struct non_aligned_frame_boundaries_r17_s_ {
    bool                      scs15k_hz_15k_hz_r17_present = false;
    bool                      scs15k_hz_30k_hz_r17_present = false;
    bool                      scs15k_hz_60k_hz_r17_present = false;
    bool                      scs30k_hz_30k_hz_r17_present = false;
    bool                      scs30k_hz_60k_hz_r17_present = false;
    bool                      scs60k_hz_60k_hz_r17_present = false;
    bounded_bitstring<1, 496> scs15k_hz_15k_hz_r17;
    bounded_bitstring<1, 496> scs15k_hz_30k_hz_r17;
    bounded_bitstring<1, 496> scs15k_hz_60k_hz_r17;
    bounded_bitstring<1, 496> scs30k_hz_30k_hz_r17;
    bounded_bitstring<1, 496> scs30k_hz_60k_hz_r17;
    bounded_bitstring<1, 496> scs60k_hz_60k_hz_r17;
  };

  // member variables
  bool                                         codebook_paramsfetype2_per_bc_r17_present            = false;
  bool                                         demod_enhancement_ca_r17_present                     = false;
  bool                                         max_ul_duty_cycle_inter_band_ca_pc2_r17_present      = false;
  bool                                         max_ul_duty_cycle_sul_combination_pc2_r17_present    = false;
  bool                                         beam_management_type_cbm_r17_present                 = false;
  bool                                         parallel_tx_pucch_pusch_r17_present                  = false;
  bool                                         codebook_combo_param_mixed_type_per_bc_r17_present   = false;
  bool                                         mtrp_csi_enhancement_per_bc_r17_present              = false;
  bool                                         codebook_combo_param_multi_trp_per_bc_r17_present    = false;
  bool                                         max_cc_32_dl_harq_process_fr2_2_r17_present          = false;
  bool                                         max_cc_32_ul_harq_process_fr2_2_r17_present          = false;
  bool                                         cross_carrier_sched_scell_sp_cell_type_b_r17_present = false;
  bool                                         cross_carrier_sched_scell_sp_cell_type_a_r17_present = false;
  bool                                         dci_formats_pcell_pscell_uss_sets_r17_present        = false;
  bool                                         disabling_scaling_factor_deact_scell_r17_present     = false;
  bool                                         disabling_scaling_factor_dormant_scell_r17_present   = false;
  bool                                         non_aligned_frame_boundaries_r17_present             = false;
  codebook_paramsfetype2_per_bc_r17_s          codebook_paramsfetype2_per_bc_r17;
  max_ul_duty_cycle_inter_band_ca_pc2_r17_e_   max_ul_duty_cycle_inter_band_ca_pc2_r17;
  max_ul_duty_cycle_sul_combination_pc2_r17_e_ max_ul_duty_cycle_sul_combination_pc2_r17;
  codebook_combo_param_mixed_type_per_bc_r17_s codebook_combo_param_mixed_type_per_bc_r17;
  mtrp_csi_enhancement_per_bc_r17_s_           mtrp_csi_enhancement_per_bc_r17;
  codebook_combo_param_multi_trp_per_bc_r17_s  codebook_combo_param_multi_trp_per_bc_r17;
  max_cc_32_dl_harq_process_fr2_2_r17_e_       max_cc_32_dl_harq_process_fr2_2_r17;
  max_cc_32_ul_harq_process_fr2_2_r17_e_       max_cc_32_ul_harq_process_fr2_2_r17;
  cross_carrier_sched_scell_sp_cell_r17_s      cross_carrier_sched_scell_sp_cell_type_b_r17;
  cross_carrier_sched_scell_sp_cell_r17_s      cross_carrier_sched_scell_sp_cell_type_a_r17;
  non_aligned_frame_boundaries_r17_s_          non_aligned_frame_boundaries_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNRDC-v1700 ::= SEQUENCE
struct ca_params_nrdc_v1700_s {
  bool simul_rx_tx_iab_multiple_parents_r17_present        = false;
  bool cond_pscell_addition_nrdc_r17_present               = false;
  bool scg_activation_deactivation_nrdc_r17_present        = false;
  bool scg_activation_deactivation_resume_nrdc_r17_present = false;
  bool beam_management_type_cbm_r17_present                = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRDC-Parameters-v1700 ::= SEQUENCE
struct mrdc_params_v1700_s {
  bool cond_pscell_addition_endc_r17_present               = false;
  bool scg_activation_deactivation_endc_r17_present        = false;
  bool scg_activation_deactivation_resume_endc_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v1700 ::= SEQUENCE
struct band_combination_v1700_s {
  using band_list_v1710_l_ = dyn_array<band_params_v1710_s>;

  // member variables
  bool                        ca_params_nr_v1700_present                                         = false;
  bool                        ca_params_nrdc_v1700_present                                       = false;
  bool                        mrdc_params_v1700_present                                          = false;
  bool                        supported_band_comb_list_per_bc_sl_relay_discovery_r17_present     = false;
  bool                        supported_band_comb_list_per_bc_sl_non_relay_discovery_r17_present = false;
  ca_params_nr_v1700_s        ca_params_nr_v1700;
  ca_params_nrdc_v1700_s      ca_params_nrdc_v1700;
  mrdc_params_v1700_s         mrdc_params_v1700;
  band_list_v1710_l_          band_list_v1710;
  bounded_bitstring<1, 65536> supported_band_comb_list_per_bc_sl_relay_discovery_r17;
  bounded_bitstring<1, 65536> supported_band_comb_list_per_bc_sl_non_relay_discovery_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULTxSwitchingBandPair-v1700 ::= SEQUENCE
struct ul_tx_switching_band_pair_v1700_s {
  struct ul_tx_switching_period2_t2_t_r17_opts {
    enum options { n35us, n140us, n210us, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ul_tx_switching_period2_t2_t_r17_e_ = enumerated<ul_tx_switching_period2_t2_t_r17_opts>;

  // member variables
  bool                                ul_tx_switching_period2_t2_t_r17_present = false;
  ul_tx_switching_period2_t2_t_r17_e_ ul_tx_switching_period2_t2_t_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkTxSwitchingBandParameters-v1700 ::= SEQUENCE
struct ul_tx_switching_band_params_v1700_s {
  struct ul_tx_switching2_t2_t_pusch_trans_coherence_r17_opts {
    enum options { non_coherent, full_coherent, nulltype } value;

    const char* to_string() const;
  };
  using ul_tx_switching2_t2_t_pusch_trans_coherence_r17_e_ =
      enumerated<ul_tx_switching2_t2_t_pusch_trans_coherence_r17_opts>;

  // member variables
  bool                                               ul_tx_switching2_t2_t_pusch_trans_coherence_r17_present = false;
  uint8_t                                            band_idx_r17                                            = 1;
  ul_tx_switching2_t2_t_pusch_trans_coherence_r17_e_ ul_tx_switching2_t2_t_pusch_trans_coherence_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-UplinkTxSwitch-v1700 ::= SEQUENCE
struct band_combination_ul_tx_switch_v1700_s {
  using supported_band_pair_list_nr_v1700_l_      = dyn_array<ul_tx_switching_band_pair_v1700_s>;
  using ul_tx_switching_band_params_list_v1700_l_ = dyn_array<ul_tx_switching_band_params_v1700_s>;

  // member variables
  bool                                      band_combination_v1700_present = false;
  band_combination_v1700_s                  band_combination_v1700;
  supported_band_pair_list_nr_v1700_l_      supported_band_pair_list_nr_v1700;
  ul_tx_switching_band_params_list_v1700_l_ ul_tx_switching_band_params_list_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-BlindDetectionCA-Mixed-r17 ::= SEQUENCE
struct pdcch_blind_detection_ca_mixed_r17_s {
  bool    pdcch_blind_detection_ca1_r17_present = false;
  bool    pdcch_blind_detection_ca2_r17_present = false;
  uint8_t pdcch_blind_detection_ca1_r17         = 1;
  uint8_t pdcch_blind_detection_ca2_r17         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-BlindDetectionCA-Mixed1-r17 ::= SEQUENCE
struct pdcch_blind_detection_ca_mixed1_r17_s {
  bool    pdcch_blind_detection_ca1_r17_present = false;
  bool    pdcch_blind_detection_ca2_r17_present = false;
  bool    pdcch_blind_detection_ca3_r17_present = false;
  uint8_t pdcch_blind_detection_ca1_r17         = 1;
  uint8_t pdcch_blind_detection_ca2_r17         = 1;
  uint8_t pdcch_blind_detection_ca3_r17         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-BlindDetectionCG-UE-Mixed-r17 ::= SEQUENCE
struct pdcch_blind_detection_cg_ue_mixed_r17_s {
  uint8_t pdcch_blind_detection_cg_ue1_r17 = 0;
  uint8_t pdcch_blind_detection_cg_ue2_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-BlindDetectionCG-UE-Mixed1-r17 ::= SEQUENCE
struct pdcch_blind_detection_cg_ue_mixed1_r17_s {
  uint8_t pdcch_blind_detection_cg_ue1_r17 = 0;
  uint8_t pdcch_blind_detection_cg_ue2_r17 = 0;
  uint8_t pdcch_blind_detection_cg_ue3_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-Group-Config-r17 ::= SEQUENCE
struct pucch_group_cfg_r17_s {
  bool fr1_fr1_non_shared_tdd_r17_present = false;
  bool fr2_fr2_non_shared_tdd_r17_present = false;
  bool fr1_fr2_non_shared_tdd_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-BlindDetectionMCG-SCG-r17 ::= SEQUENCE
struct pdcch_blind_detection_mcg_scg_r17_s {
  uint8_t pdcch_blind_detection_mcg_ue_r17 = 1;
  uint8_t pdcch_blind_detection_scg_ue_r17 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-BlindDetectionMixed-r17 ::= SEQUENCE
struct pdcch_blind_detection_mixed_r17_s {
  struct pdcch_blind_detection_cg_ue_mixed_r17_s_ {
    pdcch_blind_detection_cg_ue_mixed_r17_s pdcch_blind_detection_mcg_ue_mixed_v17;
    pdcch_blind_detection_cg_ue_mixed_r17_s pdcch_blind_detection_scg_ue_mixed_v17;
  };

  // member variables
  bool                                     pdcch_blind_detection_ca_mixed_r17_present    = false;
  bool                                     pdcch_blind_detection_cg_ue_mixed_r17_present = false;
  pdcch_blind_detection_ca_mixed_r17_s     pdcch_blind_detection_ca_mixed_r17;
  pdcch_blind_detection_cg_ue_mixed_r17_s_ pdcch_blind_detection_cg_ue_mixed_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-BlindDetectionMixed1-r17 ::= SEQUENCE
struct pdcch_blind_detection_mixed1_r17_s {
  struct pdcch_blind_detection_cg_ue_mixed1_r17_s_ {
    pdcch_blind_detection_cg_ue_mixed1_r17_s pdcch_blind_detection_mcg_ue_mixed1_v17;
    pdcch_blind_detection_cg_ue_mixed1_r17_s pdcch_blind_detection_scg_ue_mixed1_v17;
  };

  // member variables
  bool                                      pdcch_blind_detection_ca_mixed1_r17_present    = false;
  bool                                      pdcch_blind_detection_cg_ue_mixed1_r17_present = false;
  pdcch_blind_detection_ca_mixed1_r17_s     pdcch_blind_detection_ca_mixed1_r17;
  pdcch_blind_detection_cg_ue_mixed1_r17_s_ pdcch_blind_detection_cg_ue_mixed1_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TwoPUCCH-Grp-Configurations-r17 ::= SEQUENCE
struct two_pucch_grp_cfgs_r17_s {
  pucch_group_cfg_r17_s primary_pucch_group_cfg_r17;
  pucch_group_cfg_r17_s secondary_pucch_group_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNR-v1720 ::= SEQUENCE
struct ca_params_nr_v1720_s {
  struct semi_static_pucch_cell_switch_single_group_r17_s_ {
    struct pucch_group_r17_opts {
      enum options { primary_group_only, secondary_group_only, either_primary_or_secondary_group, nulltype } value;

      const char* to_string() const;
    };
    using pucch_group_r17_e_ = enumerated<pucch_group_r17_opts>;

    // member variables
    pucch_group_r17_e_    pucch_group_r17;
    pucch_group_cfg_r17_s pucch_group_cfg_r17;
  };
  using semi_static_pucch_cell_switch_two_groups_r17_l_ = dyn_array<two_pucch_grp_cfgs_r17_s>;
  struct dyn_pucch_cell_switch_same_len_single_group_r17_s_ {
    struct pucch_group_r17_opts {
      enum options { primary_group_only, secondary_group_only, either_primary_or_secondary_group, nulltype } value;

      const char* to_string() const;
    };
    using pucch_group_r17_e_ = enumerated<pucch_group_r17_opts>;

    // member variables
    pucch_group_r17_e_    pucch_group_r17;
    pucch_group_cfg_r17_s pucch_group_cfg_r17;
  };
  struct dyn_pucch_cell_switch_diff_len_single_group_r17_s_ {
    struct pucch_group_r17_opts {
      enum options { primary_group_only, secondary_group_only, either_primary_or_secondary_group, nulltype } value;

      const char* to_string() const;
    };
    using pucch_group_r17_e_ = enumerated<pucch_group_r17_opts>;

    // member variables
    pucch_group_r17_e_    pucch_group_r17;
    pucch_group_cfg_r17_s pucch_group_cfg_r17;
  };
  using dyn_pucch_cell_switch_same_len_two_groups_r17_l_ = dyn_array<two_pucch_grp_cfgs_r17_s>;
  using dyn_pucch_cell_switch_diff_len_two_groups_r17_l_ = dyn_array<two_pucch_grp_cfgs_r17_s>;
  using pdcch_blind_detection_mcg_scg_list_r17_l_        = dyn_array<pdcch_blind_detection_mcg_scg_r17_s>;
  using pdcch_blind_detection_mixed_list1_r17_l_         = dyn_array<pdcch_blind_detection_mixed_r17_s>;
  using pdcch_blind_detection_mixed_list2_r17_l_         = dyn_array<pdcch_blind_detection_mixed_r17_s>;
  using pdcch_blind_detection_mixed_list3_r17_l_         = dyn_array<pdcch_blind_detection_mixed1_r17_s>;

  // member variables
  bool                                               parallel_tx_srs_pucch_pusch_intra_band_r17_present        = false;
  bool                                               parallel_tx_prach_srs_pucch_pusch_intra_band_r17_present  = false;
  bool                                               semi_static_pucch_cell_switch_single_group_r17_present    = false;
  bool                                               dyn_pucch_cell_switch_same_len_single_group_r17_present   = false;
  bool                                               dyn_pucch_cell_switch_diff_len_single_group_r17_present   = false;
  bool                                               ack_nack_feedback_for_multicast_r17_present               = false;
  bool                                               ptp_retx_multicast_r17_present                            = false;
  bool                                               nack_only_feedback_for_multicast_r17_present              = false;
  bool                                               nack_only_feedback_specific_res_for_multicast_r17_present = false;
  bool                                               ack_nack_feedback_for_sps_multicast_r17_present           = false;
  bool                                               ptp_retx_sps_multicast_r17_present                        = false;
  bool                                               higher_pwr_limit_r17_present                              = false;
  bool                                               parallel_tx_msg_a_srs_pucch_pusch_intra_band_r17_present  = false;
  bool                                               pdcch_monitoring_ca_r17_present                           = false;
  semi_static_pucch_cell_switch_single_group_r17_s_  semi_static_pucch_cell_switch_single_group_r17;
  semi_static_pucch_cell_switch_two_groups_r17_l_    semi_static_pucch_cell_switch_two_groups_r17;
  dyn_pucch_cell_switch_same_len_single_group_r17_s_ dyn_pucch_cell_switch_same_len_single_group_r17;
  dyn_pucch_cell_switch_diff_len_single_group_r17_s_ dyn_pucch_cell_switch_diff_len_single_group_r17;
  dyn_pucch_cell_switch_same_len_two_groups_r17_l_   dyn_pucch_cell_switch_same_len_two_groups_r17;
  dyn_pucch_cell_switch_diff_len_two_groups_r17_l_   dyn_pucch_cell_switch_diff_len_two_groups_r17;
  uint8_t                                            pdcch_monitoring_ca_r17 = 4;
  pdcch_blind_detection_mcg_scg_list_r17_l_          pdcch_blind_detection_mcg_scg_list_r17;
  pdcch_blind_detection_mixed_list1_r17_l_           pdcch_blind_detection_mixed_list1_r17;
  pdcch_blind_detection_mixed_list2_r17_l_           pdcch_blind_detection_mixed_list2_r17;
  pdcch_blind_detection_mixed_list3_r17_l_           pdcch_blind_detection_mixed_list3_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNRDC-v1720 ::= SEQUENCE
struct ca_params_nrdc_v1720_s {
  bool                 ca_params_nr_for_dc_v1700_present = false;
  bool                 ca_params_nr_for_dc_v1720_present = false;
  ca_params_nr_v1700_s ca_params_nr_for_dc_v1700;
  ca_params_nr_v1720_s ca_params_nr_for_dc_v1720;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v1720 ::= SEQUENCE
struct band_combination_v1720_s {
  bool                   ca_params_nr_v1720_present   = false;
  bool                   ca_params_nrdc_v1720_present = false;
  ca_params_nr_v1720_s   ca_params_nr_v1720;
  ca_params_nrdc_v1720_s ca_params_nrdc_v1720;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-UplinkTxSwitch-v1720 ::= SEQUENCE
struct band_combination_ul_tx_switch_v1720_s {
  struct ul_tx_switching_option_support2_t2_t_r17_opts {
    enum options { switched_ul, dual_ul, both, nulltype } value;

    const char* to_string() const;
  };
  using ul_tx_switching_option_support2_t2_t_r17_e_ = enumerated<ul_tx_switching_option_support2_t2_t_r17_opts>;

  // member variables
  bool                                        band_combination_v1720_present                   = false;
  bool                                        ul_tx_switching_option_support2_t2_t_r17_present = false;
  band_combination_v1720_s                    band_combination_v1720;
  ul_tx_switching_option_support2_t2_t_r17_e_ ul_tx_switching_option_support2_t2_t_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNR-v1730 ::= SEQUENCE
struct ca_params_nr_v1730_s {
  bool    dmrs_bundling_pusch_rep_type_a_per_bc_r17_present                 = false;
  bool    dmrs_bundling_pusch_rep_type_b_per_bc_r17_present                 = false;
  bool    dmrs_bundling_pusch_multi_slot_per_bc_r17_present                 = false;
  bool    dmrs_bundling_pucch_rep_per_bc_r17_present                        = false;
  bool    dmrs_bundling_restart_per_bc_r17_present                          = false;
  bool    dmrs_bundling_non_back_to_back_tx_per_bc_r17_present              = false;
  bool    stay_on_target_cc_srs_carrier_switch_r17_present                  = false;
  bool    fdm_codebook_for_mux_unicast_multicast_harq_ack_r17_present       = false;
  bool    mode2_tdm_codebook_for_mux_unicast_multicast_harq_ack_r17_present = false;
  bool    mode1_for_type1_codebook_generation_r17_present                   = false;
  bool    nack_only_feedback_specific_res_for_sps_multicast_r17_present     = false;
  bool    multi_pucch_cfg_for_multicast_r17_present                         = false;
  bool    pucch_cfg_for_sps_multicast_r17_present                           = false;
  bool    max_num_g_rnti_harq_ack_codebook_r17_present                      = false;
  bool    mux_harq_ack_unicast_multicast_r17_present                        = false;
  uint8_t max_num_g_rnti_harq_ack_codebook_r17                              = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParameters-v1730 ::= SEQUENCE
struct band_params_v1730_s {
  using srs_switching_affected_bands_list_nr_r17_l_ = bounded_array<bounded_bitstring<1, 32>, 32>;

  // member variables
  srs_switching_affected_bands_list_nr_r17_l_ srs_switching_affected_bands_list_nr_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNRDC-v1730 ::= SEQUENCE
struct ca_params_nrdc_v1730_s {
  bool                 ca_params_nr_for_dc_v1730_present = false;
  ca_params_nr_v1730_s ca_params_nr_for_dc_v1730;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v1730 ::= SEQUENCE
struct band_combination_v1730_s {
  using band_list_v1730_l_ = dyn_array<band_params_v1730_s>;

  // member variables
  bool                   ca_params_nr_v1730_present   = false;
  bool                   ca_params_nrdc_v1730_present = false;
  ca_params_nr_v1730_s   ca_params_nr_v1730;
  ca_params_nrdc_v1730_s ca_params_nrdc_v1730;
  band_list_v1730_l_     band_list_v1730;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-UplinkTxSwitch-v1730 ::= SEQUENCE
struct band_combination_ul_tx_switch_v1730_s {
  bool                     band_combination_v1730_present = false;
  band_combination_v1730_s band_combination_v1730;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersNR-v1740 ::= SEQUENCE
struct ca_params_nr_v1740_s {
  bool nack_only_feedback_for_sps_multicast_r17_present = false;
  bool single_pucch_cfg_for_multicast_r17_present       = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v1740 ::= SEQUENCE
struct band_combination_v1740_s {
  bool                 ca_params_nr_v1740_present = false;
  ca_params_nr_v1740_s ca_params_nr_v1740;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-UplinkTxSwitch-v1740 ::= SEQUENCE
struct band_combination_ul_tx_switch_v1740_s {
  bool                     band_combination_v1740_present = false;
  band_combination_v1740_s band_combination_v1740;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v1550 ::= SEQUENCE
struct band_combination_v1550_s {
  ca_params_nr_v1550_s ca_params_nr_v1550;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntraBandPowerClass-r16 ::= ENUMERATED
struct intra_band_pwr_class_r16_opts {
  enum options { pc2, pc3, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using intra_band_pwr_class_r16_e = enumerated<intra_band_pwr_class_r16_opts>;

// BandCombination-v1680 ::= SEQUENCE
struct band_combination_v1680_s {
  using intraband_concurrent_operation_pwr_class_r16_l_ = dyn_array<intra_band_pwr_class_r16_e>;

  // member variables
  intraband_concurrent_operation_pwr_class_r16_l_ intraband_concurrent_operation_pwr_class_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationList ::= SEQUENCE (SIZE (1..65536)) OF BandCombination
using band_combination_list_l = dyn_array<band_combination_s>;

// BandCombinationList-UplinkTxSwitch-r16 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-UplinkTxSwitch-r16
using band_combination_list_ul_tx_switch_r16_l = dyn_array<band_combination_ul_tx_switch_r16_s>;

// BandCombinationList-UplinkTxSwitch-v1630 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-UplinkTxSwitch-v1630
using band_combination_list_ul_tx_switch_v1630_l = dyn_array<band_combination_ul_tx_switch_v1630_s>;

// BandCombinationList-UplinkTxSwitch-v1640 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-UplinkTxSwitch-v1640
using band_combination_list_ul_tx_switch_v1640_l = dyn_array<band_combination_ul_tx_switch_v1640_s>;

// BandCombinationList-UplinkTxSwitch-v1650 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-UplinkTxSwitch-v1650
using band_combination_list_ul_tx_switch_v1650_l = dyn_array<band_combination_ul_tx_switch_v1650_s>;

// BandCombinationList-UplinkTxSwitch-v1670 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-UplinkTxSwitch-v1670
using band_combination_list_ul_tx_switch_v1670_l = dyn_array<band_combination_ul_tx_switch_v1670_s>;

// BandCombinationList-UplinkTxSwitch-v1690 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-UplinkTxSwitch-v1690
using band_combination_list_ul_tx_switch_v1690_l = dyn_array<band_combination_ul_tx_switch_v1690_s>;

// BandCombinationList-UplinkTxSwitch-v1700 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-UplinkTxSwitch-v1700
using band_combination_list_ul_tx_switch_v1700_l = dyn_array<band_combination_ul_tx_switch_v1700_s>;

// BandCombinationList-UplinkTxSwitch-v1720 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-UplinkTxSwitch-v1720
using band_combination_list_ul_tx_switch_v1720_l = dyn_array<band_combination_ul_tx_switch_v1720_s>;

// BandCombinationList-UplinkTxSwitch-v1730 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-UplinkTxSwitch-v1730
using band_combination_list_ul_tx_switch_v1730_l = dyn_array<band_combination_ul_tx_switch_v1730_s>;

// BandCombinationList-UplinkTxSwitch-v1740 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-UplinkTxSwitch-v1740
using band_combination_list_ul_tx_switch_v1740_l = dyn_array<band_combination_ul_tx_switch_v1740_s>;

// BandCombinationList-v1540 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1540
using band_combination_list_v1540_l = dyn_array<band_combination_v1540_s>;

// BandCombinationList-v1550 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1550
using band_combination_list_v1550_l = dyn_array<band_combination_v1550_s>;

// BandCombinationList-v1560 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1560
using band_combination_list_v1560_l = dyn_array<band_combination_v1560_s>;

// BandCombinationList-v1610 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1610
using band_combination_list_v1610_l = dyn_array<band_combination_v1610_s>;

// BandCombinationList-v1630 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1630
using band_combination_list_v1630_l = dyn_array<band_combination_v1630_s>;

// BandCombinationList-v1640 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1640
using band_combination_list_v1640_l = dyn_array<band_combination_v1640_s>;

// BandCombinationList-v1650 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1650
using band_combination_list_v1650_l = dyn_array<band_combination_v1650_s>;

// BandCombinationList-v1680 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1680
using band_combination_list_v1680_l = dyn_array<band_combination_v1680_s>;

// BandCombinationList-v1690 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1690
using band_combination_list_v1690_l = dyn_array<band_combination_v1690_s>;

// BandCombinationList-v1700 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1700
using band_combination_list_v1700_l = dyn_array<band_combination_v1700_s>;

// BandCombinationList-v1720 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1720
using band_combination_list_v1720_l = dyn_array<band_combination_v1720_s>;

// BandCombinationList-v1730 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1730
using band_combination_list_v1730_l = dyn_array<band_combination_v1730_s>;

// BandCombinationList-v1740 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1740
using band_combination_list_v1740_l = dyn_array<band_combination_v1740_s>;

// BandParametersSidelinkDiscovery-r17 ::= SEQUENCE
struct band_params_sidelink_discovery_r17_s {
  struct sl_tx_mode2_partial_sensing_r17_s_ {
    struct harq_tx_process_mode_two_sidelink_r17_opts {
      enum options { n8, n16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using harq_tx_process_mode_two_sidelink_r17_e_ = enumerated<harq_tx_process_mode_two_sidelink_r17_opts>;
    struct scs_cp_pattern_tx_sidelink_mode_two_r17_c_ {
      struct fr1_r17_s_ {
        bool                scs_15k_hz_r17_present = false;
        bool                scs_30k_hz_r17_present = false;
        bool                scs_60k_hz_r17_present = false;
        fixed_bitstring<16> scs_15k_hz_r17;
        fixed_bitstring<16> scs_30k_hz_r17;
        fixed_bitstring<16> scs_60k_hz_r17;
      };
      struct fr2_r17_s_ {
        bool                scs_60k_hz_r17_present  = false;
        bool                scs_120k_hz_r17_present = false;
        fixed_bitstring<16> scs_60k_hz_r17;
        fixed_bitstring<16> scs_120k_hz_r17;
      };
      struct types_opts {
        enum options { fr1_r17, fr2_r17, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      scs_cp_pattern_tx_sidelink_mode_two_r17_c_() = default;
      scs_cp_pattern_tx_sidelink_mode_two_r17_c_(const scs_cp_pattern_tx_sidelink_mode_two_r17_c_& other);
      scs_cp_pattern_tx_sidelink_mode_two_r17_c_& operator=(const scs_cp_pattern_tx_sidelink_mode_two_r17_c_& other);
      ~scs_cp_pattern_tx_sidelink_mode_two_r17_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      fr1_r17_s_& fr1_r17()
      {
        assert_choice_type(types::fr1_r17, type_, "scs-CP-PatternTxSidelinkModeTwo-r17");
        return c.get<fr1_r17_s_>();
      }
      fr2_r17_s_& fr2_r17()
      {
        assert_choice_type(types::fr2_r17, type_, "scs-CP-PatternTxSidelinkModeTwo-r17");
        return c.get<fr2_r17_s_>();
      }
      const fr1_r17_s_& fr1_r17() const
      {
        assert_choice_type(types::fr1_r17, type_, "scs-CP-PatternTxSidelinkModeTwo-r17");
        return c.get<fr1_r17_s_>();
      }
      const fr2_r17_s_& fr2_r17() const
      {
        assert_choice_type(types::fr2_r17, type_, "scs-CP-PatternTxSidelinkModeTwo-r17");
        return c.get<fr2_r17_s_>();
      }
      fr1_r17_s_& set_fr1_r17();
      fr2_r17_s_& set_fr2_r17();

    private:
      types                                   type_;
      choice_buffer_t<fr1_r17_s_, fr2_r17_s_> c;

      void destroy_();
    };

    // member variables
    bool                                       scs_cp_pattern_tx_sidelink_mode_two_r17_present = false;
    bool                                       extended_cp_mode2_partial_sensing_r17_present   = false;
    bool                                       dl_open_loop_pc_sidelink_r17_present            = false;
    harq_tx_process_mode_two_sidelink_r17_e_   harq_tx_process_mode_two_sidelink_r17;
    scs_cp_pattern_tx_sidelink_mode_two_r17_c_ scs_cp_pattern_tx_sidelink_mode_two_r17;
  };

  // member variables
  bool                               sl_cross_carrier_sched_r17_present        = false;
  bool                               sl_tx_mode2_partial_sensing_r17_present   = false;
  bool                               tx_iuc_scheme1_mode2_sidelink_r17_present = false;
  sl_tx_mode2_partial_sensing_r17_s_ sl_tx_mode2_partial_sensing_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationListSL-Discovery-r17 ::= SEQUENCE (SIZE (1..32)) OF BandParametersSidelinkDiscovery-r17
using band_combination_list_sl_discovery_r17_l = dyn_array<band_params_sidelink_discovery_r17_s>;

// BandParametersSidelink-r16 ::= SEQUENCE
struct band_params_sidelink_r16_s {
  uint16_t freq_band_sidelink_r16 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParametersSidelinkEUTRA-NR-r16 ::= CHOICE
struct band_params_sidelink_eutra_nr_r16_c {
  struct eutra_s_ {
    dyn_octstring band_params_sidelink_eutra1_r16;
    dyn_octstring band_params_sidelink_eutra2_r16;
  };
  struct nr_s_ {
    band_params_sidelink_r16_s band_params_sidelink_nr_r16;
  };
  struct types_opts {
    enum options { eutra, nr, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  band_params_sidelink_eutra_nr_r16_c() = default;
  band_params_sidelink_eutra_nr_r16_c(const band_params_sidelink_eutra_nr_r16_c& other);
  band_params_sidelink_eutra_nr_r16_c& operator=(const band_params_sidelink_eutra_nr_r16_c& other);
  ~band_params_sidelink_eutra_nr_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutra_s_& eutra()
  {
    assert_choice_type(types::eutra, type_, "BandParametersSidelinkEUTRA-NR-r16");
    return c.get<eutra_s_>();
  }
  nr_s_& nr()
  {
    assert_choice_type(types::nr, type_, "BandParametersSidelinkEUTRA-NR-r16");
    return c.get<nr_s_>();
  }
  const eutra_s_& eutra() const
  {
    assert_choice_type(types::eutra, type_, "BandParametersSidelinkEUTRA-NR-r16");
    return c.get<eutra_s_>();
  }
  const nr_s_& nr() const
  {
    assert_choice_type(types::nr, type_, "BandParametersSidelinkEUTRA-NR-r16");
    return c.get<nr_s_>();
  }
  eutra_s_& set_eutra();
  nr_s_&    set_nr();

private:
  types                            type_;
  choice_buffer_t<eutra_s_, nr_s_> c;

  void destroy_();
};

// BandCombinationParametersSidelinkEUTRA-NR-r16 ::= SEQUENCE (SIZE (1..32)) OF BandParametersSidelinkEUTRA-NR-r16
using band_combination_params_sidelink_eutra_nr_r16_l = dyn_array<band_params_sidelink_eutra_nr_r16_c>;

// BandCombinationListSidelinkEUTRA-NR-r16 ::= SEQUENCE (SIZE (1..65536)) OF
// BandCombinationParametersSidelinkEUTRA-NR-r16
using band_combination_list_sidelink_eutra_nr_r16_l = dyn_array<band_combination_params_sidelink_eutra_nr_r16_l>;

// BandParametersSidelinkEUTRA-NR-v1630 ::= CHOICE
struct band_params_sidelink_eutra_nr_v1630_c {
  struct nr_s_ {
    bool tx_sidelink_r16_present            = false;
    bool rx_sidelink_r16_present            = false;
    bool sl_cross_carrier_sched_r16_present = false;
  };
  struct types_opts {
    enum options { eutra, nr, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  band_params_sidelink_eutra_nr_v1630_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  nr_s_& nr()
  {
    assert_choice_type(types::nr, type_, "BandParametersSidelinkEUTRA-NR-v1630");
    return c;
  }
  const nr_s_& nr() const
  {
    assert_choice_type(types::nr, type_, "BandParametersSidelinkEUTRA-NR-v1630");
    return c;
  }
  void   set_eutra();
  nr_s_& set_nr();

private:
  types type_;
  nr_s_ c;
};

// BandCombinationParametersSidelinkEUTRA-NR-v1630 ::= SEQUENCE (SIZE (1..32)) OF BandParametersSidelinkEUTRA-NR-v1630
using band_combination_params_sidelink_eutra_nr_v1630_l = dyn_array<band_params_sidelink_eutra_nr_v1630_c>;

// BandCombinationListSidelinkEUTRA-NR-v1630 ::= SEQUENCE (SIZE (1..65536)) OF
// BandCombinationParametersSidelinkEUTRA-NR-v1630
using band_combination_list_sidelink_eutra_nr_v1630_l = dyn_array<band_combination_params_sidelink_eutra_nr_v1630_l>;

// BandParametersSidelinkEUTRA-NR-v1710 ::= CHOICE
struct band_params_sidelink_eutra_nr_v1710_c {
  struct nr_s_ {
    struct sl_tx_mode2_partial_sensing_r17_s_ {
      struct harq_tx_process_mode_two_sidelink_r17_opts {
        enum options { n8, n16, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using harq_tx_process_mode_two_sidelink_r17_e_ = enumerated<harq_tx_process_mode_two_sidelink_r17_opts>;
      struct scs_cp_pattern_tx_sidelink_mode_two_r17_c_ {
        struct fr1_r17_s_ {
          bool                scs_15k_hz_r17_present = false;
          bool                scs_30k_hz_r17_present = false;
          bool                scs_60k_hz_r17_present = false;
          fixed_bitstring<16> scs_15k_hz_r17;
          fixed_bitstring<16> scs_30k_hz_r17;
          fixed_bitstring<16> scs_60k_hz_r17;
        };
        struct fr2_r17_s_ {
          bool                scs_60k_hz_r17_present  = false;
          bool                scs_120k_hz_r17_present = false;
          fixed_bitstring<16> scs_60k_hz_r17;
          fixed_bitstring<16> scs_120k_hz_r17;
        };
        struct types_opts {
          enum options { fr1_r17, fr2_r17, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        using types = enumerated<types_opts>;

        // choice methods
        scs_cp_pattern_tx_sidelink_mode_two_r17_c_() = default;
        scs_cp_pattern_tx_sidelink_mode_two_r17_c_(const scs_cp_pattern_tx_sidelink_mode_two_r17_c_& other);
        scs_cp_pattern_tx_sidelink_mode_two_r17_c_& operator=(const scs_cp_pattern_tx_sidelink_mode_two_r17_c_& other);
        ~scs_cp_pattern_tx_sidelink_mode_two_r17_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        fr1_r17_s_& fr1_r17()
        {
          assert_choice_type(types::fr1_r17, type_, "scs-CP-PatternTxSidelinkModeTwo-r17");
          return c.get<fr1_r17_s_>();
        }
        fr2_r17_s_& fr2_r17()
        {
          assert_choice_type(types::fr2_r17, type_, "scs-CP-PatternTxSidelinkModeTwo-r17");
          return c.get<fr2_r17_s_>();
        }
        const fr1_r17_s_& fr1_r17() const
        {
          assert_choice_type(types::fr1_r17, type_, "scs-CP-PatternTxSidelinkModeTwo-r17");
          return c.get<fr1_r17_s_>();
        }
        const fr2_r17_s_& fr2_r17() const
        {
          assert_choice_type(types::fr2_r17, type_, "scs-CP-PatternTxSidelinkModeTwo-r17");
          return c.get<fr2_r17_s_>();
        }
        fr1_r17_s_& set_fr1_r17();
        fr2_r17_s_& set_fr2_r17();

      private:
        types                                   type_;
        choice_buffer_t<fr1_r17_s_, fr2_r17_s_> c;

        void destroy_();
      };

      // member variables
      bool                                       scs_cp_pattern_tx_sidelink_mode_two_r17_present = false;
      bool                                       extended_cp_mode2_partial_sensing_r17_present   = false;
      bool                                       dl_open_loop_pc_sidelink_r17_present            = false;
      harq_tx_process_mode_two_sidelink_r17_e_   harq_tx_process_mode_two_sidelink_r17;
      scs_cp_pattern_tx_sidelink_mode_two_r17_c_ scs_cp_pattern_tx_sidelink_mode_two_r17;
    };
    struct rx_sidelink_psfch_r17_opts {
      enum options { n5, n15, n25, n32, n35, n45, n50, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using rx_sidelink_psfch_r17_e_ = enumerated<rx_sidelink_psfch_r17_opts>;
    struct tx_iuc_scheme2_mode2_sidelink_r17_opts {
      enum options { n4, n8, n16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using tx_iuc_scheme2_mode2_sidelink_r17_e_ = enumerated<tx_iuc_scheme2_mode2_sidelink_r17_opts>;

    // member variables
    bool                                 sl_tx_mode2_partial_sensing_r17_present   = false;
    bool                                 rx_sidelink_psfch_r17_present             = false;
    bool                                 tx_iuc_scheme1_mode2_sidelink_r17_present = false;
    bool                                 tx_iuc_scheme2_mode2_sidelink_r17_present = false;
    sl_tx_mode2_partial_sensing_r17_s_   sl_tx_mode2_partial_sensing_r17;
    rx_sidelink_psfch_r17_e_             rx_sidelink_psfch_r17;
    tx_iuc_scheme2_mode2_sidelink_r17_e_ tx_iuc_scheme2_mode2_sidelink_r17;
  };
  struct types_opts {
    enum options { eutra, nr, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  band_params_sidelink_eutra_nr_v1710_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  nr_s_& nr()
  {
    assert_choice_type(types::nr, type_, "BandParametersSidelinkEUTRA-NR-v1710");
    return c;
  }
  const nr_s_& nr() const
  {
    assert_choice_type(types::nr, type_, "BandParametersSidelinkEUTRA-NR-v1710");
    return c;
  }
  void   set_eutra();
  nr_s_& set_nr();

private:
  types type_;
  nr_s_ c;
};

// BandCombinationParametersSidelinkEUTRA-NR-v1710 ::= SEQUENCE (SIZE (1..32)) OF BandParametersSidelinkEUTRA-NR-v1710
using band_combination_params_sidelink_eutra_nr_v1710_l = dyn_array<band_params_sidelink_eutra_nr_v1710_c>;

// BandCombinationListSidelinkEUTRA-NR-v1710 ::= SEQUENCE (SIZE (1..65536)) OF
// BandCombinationParametersSidelinkEUTRA-NR-v1710
using band_combination_list_sidelink_eutra_nr_v1710_l = dyn_array<band_combination_params_sidelink_eutra_nr_v1710_l>;

// SupportedCSI-RS-Resource ::= SEQUENCE
struct supported_csi_rs_res_s {
  struct max_num_tx_ports_per_res_opts {
    enum options { p2, p4, p8, p12, p16, p24, p32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_tx_ports_per_res_e_ = enumerated<max_num_tx_ports_per_res_opts>;

  // member variables
  max_num_tx_ports_per_res_e_ max_num_tx_ports_per_res;
  uint8_t                     max_num_res_per_band        = 1;
  uint16_t                    total_num_tx_ports_per_band = 2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BeamManagementSSB-CSI-RS ::= SEQUENCE
struct beam_management_ssb_csi_rs_s {
  struct max_num_ssb_csi_rs_res_one_tx_opts {
    enum options { n0, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_ssb_csi_rs_res_one_tx_e_ = enumerated<max_num_ssb_csi_rs_res_one_tx_opts>;
  struct max_num_csi_rs_res_opts {
    enum options { n0, n4, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_csi_rs_res_e_ = enumerated<max_num_csi_rs_res_opts>;
  struct max_num_csi_rs_res_two_tx_opts {
    enum options { n0, n4, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_csi_rs_res_two_tx_e_ = enumerated<max_num_csi_rs_res_two_tx_opts>;
  struct supported_csi_rs_density_opts {
    enum options { one, three, one_and_three, nulltype } value;

    const char* to_string() const;
  };
  using supported_csi_rs_density_e_ = enumerated<supported_csi_rs_density_opts>;
  struct max_num_aperiodic_csi_rs_res_opts {
    enum options { n0, n1, n4, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_aperiodic_csi_rs_res_e_ = enumerated<max_num_aperiodic_csi_rs_res_opts>;

  // member variables
  bool                             supported_csi_rs_density_present = false;
  max_num_ssb_csi_rs_res_one_tx_e_ max_num_ssb_csi_rs_res_one_tx;
  max_num_csi_rs_res_e_            max_num_csi_rs_res;
  max_num_csi_rs_res_two_tx_e_     max_num_csi_rs_res_two_tx;
  supported_csi_rs_density_e_      supported_csi_rs_density;
  max_num_aperiodic_csi_rs_res_e_  max_num_aperiodic_csi_rs_res;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-RS-ForTracking ::= SEQUENCE
struct csi_rs_for_tracking_s {
  uint8_t  max_burst_len             = 1;
  uint8_t  max_simul_res_sets_per_cc = 1;
  uint8_t  max_cfg_res_sets_per_cc   = 1;
  uint16_t max_cfg_res_sets_all_cc   = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-RS-IM-ReceptionForFeedback ::= SEQUENCE
struct csi_rs_im_reception_for_feedback_s {
  struct max_cfg_num_csi_im_per_cc_opts {
    enum options { n1, n2, n4, n8, n16, n32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_cfg_num_csi_im_per_cc_e_ = enumerated<max_cfg_num_csi_im_per_cc_opts>;

  // member variables
  uint8_t                      max_cfg_num_nzp_csi_rs_per_cc              = 1;
  uint16_t                     max_cfg_num_ports_across_nzp_csi_rs_per_cc = 2;
  max_cfg_num_csi_im_per_cc_e_ max_cfg_num_csi_im_per_cc;
  uint8_t                      max_num_simul_nzp_csi_rs_per_cc         = 1;
  uint16_t                     total_num_ports_simul_nzp_csi_rs_per_cc = 2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-RS-ProcFrameworkForSRS ::= SEQUENCE
struct csi_rs_proc_framework_for_srs_s {
  uint8_t max_num_periodic_srs_assoc_csi_rs_per_bwp  = 1;
  uint8_t max_num_aperiodic_srs_assoc_csi_rs_per_bwp = 1;
  uint8_t max_num_sp_srs_assoc_csi_rs_per_bwp        = 0;
  uint8_t simul_srs_assoc_csi_rs_per_cc              = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-ReportFramework ::= SEQUENCE
struct csi_report_framework_s {
  struct max_num_aperiodic_csi_trigger_state_per_cc_opts {
    enum options { n3, n7, n15, n31, n63, n128, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_aperiodic_csi_trigger_state_per_cc_e_ = enumerated<max_num_aperiodic_csi_trigger_state_per_cc_opts>;

  // member variables
  uint8_t                                       max_num_periodic_csi_per_bwp_for_csi_report        = 1;
  uint8_t                                       max_num_aperiodic_csi_per_bwp_for_csi_report       = 1;
  uint8_t                                       max_num_semi_persistent_csi_per_bwp_for_csi_report = 0;
  uint8_t                                       max_num_periodic_csi_per_bwp_for_beam_report       = 1;
  uint8_t                                       max_num_aperiodic_csi_per_bwp_for_beam_report      = 1;
  max_num_aperiodic_csi_trigger_state_per_cc_e_ max_num_aperiodic_csi_trigger_state_per_cc;
  uint8_t                                       max_num_semi_persistent_csi_per_bwp_for_beam_report = 0;
  uint8_t                                       simul_csi_reports_per_cc                            = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-ReportFrameworkExt-r16 ::= SEQUENCE
struct csi_report_framework_ext_r16_s {
  uint8_t max_num_aperiodic_csi_per_bwp_for_csi_report_ext_r16 = 5;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookComboParameterMixedType-r17 ::= SEQUENCE
struct codebook_combo_param_mixed_type_r17_s {
  using type1_sp_fe_type2_ps_null_r17_l_             = bounded_array<uint16_t, 16>;
  using type1_sp_fe_type2_ps_m2_r1_null_r17_l_       = bounded_array<uint16_t, 16>;
  using type1_sp_fe_type2_ps_m2_r2_null_r1_l_        = bounded_array<uint16_t, 16>;
  using type1_sp_type2_fe_type2_ps_m1_r17_l_         = bounded_array<uint16_t, 16>;
  using type1_sp_type2_fe_type2_ps_m2_r1_r17_l_      = bounded_array<uint16_t, 16>;
  using type1_sp_e_type2_r1_fe_type2_ps_m1_r17_l_    = bounded_array<uint16_t, 16>;
  using type1_sp_e_type2_r1_fe_type2_ps_m2_r1_r17_l_ = bounded_array<uint16_t, 16>;
  using type1_mp_fe_type2_ps_null_r17_l_             = bounded_array<uint16_t, 16>;
  using type1_mp_fe_type2_ps_m2_r1_null_r17_l_       = bounded_array<uint16_t, 16>;
  using type1_mp_fe_type2_ps_m2_r2_null_r17_l_       = bounded_array<uint16_t, 16>;
  using type1_mp_type2_fe_type2_ps_m1_r17_l_         = bounded_array<uint16_t, 16>;
  using type1_mp_type2_fe_type2_ps_m2_r1_r17_l_      = bounded_array<uint16_t, 16>;
  using type1_mp_e_type2_r1_fe_type2_ps_m1_r17_l_    = bounded_array<uint16_t, 16>;
  using type1_mp_e_type2_r1_fe_type2_ps_m2_r1_r17_l_ = bounded_array<uint16_t, 16>;

  // member variables
  type1_sp_fe_type2_ps_null_r17_l_             type1_sp_fe_type2_ps_null_r17;
  type1_sp_fe_type2_ps_m2_r1_null_r17_l_       type1_sp_fe_type2_ps_m2_r1_null_r17;
  type1_sp_fe_type2_ps_m2_r2_null_r1_l_        type1_sp_fe_type2_ps_m2_r2_null_r1;
  type1_sp_type2_fe_type2_ps_m1_r17_l_         type1_sp_type2_fe_type2_ps_m1_r17;
  type1_sp_type2_fe_type2_ps_m2_r1_r17_l_      type1_sp_type2_fe_type2_ps_m2_r1_r17;
  type1_sp_e_type2_r1_fe_type2_ps_m1_r17_l_    type1_sp_e_type2_r1_fe_type2_ps_m1_r17;
  type1_sp_e_type2_r1_fe_type2_ps_m2_r1_r17_l_ type1_sp_e_type2_r1_fe_type2_ps_m2_r1_r17;
  type1_mp_fe_type2_ps_null_r17_l_             type1_mp_fe_type2_ps_null_r17;
  type1_mp_fe_type2_ps_m2_r1_null_r17_l_       type1_mp_fe_type2_ps_m2_r1_null_r17;
  type1_mp_fe_type2_ps_m2_r2_null_r17_l_       type1_mp_fe_type2_ps_m2_r2_null_r17;
  type1_mp_type2_fe_type2_ps_m1_r17_l_         type1_mp_type2_fe_type2_ps_m1_r17;
  type1_mp_type2_fe_type2_ps_m2_r1_r17_l_      type1_mp_type2_fe_type2_ps_m2_r1_r17;
  type1_mp_e_type2_r1_fe_type2_ps_m1_r17_l_    type1_mp_e_type2_r1_fe_type2_ps_m1_r17;
  type1_mp_e_type2_r1_fe_type2_ps_m2_r1_r17_l_ type1_mp_e_type2_r1_fe_type2_ps_m2_r1_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookComboParameterMultiTRP-r17 ::= SEQUENCE
struct codebook_combo_param_multi_trp_r17_s {
  using ncjt_null_null_l_                            = bounded_array<uint16_t, 16>;
  using ncjt1_sp_null_null_l_                        = bounded_array<uint16_t, 16>;
  using ncjt_type2_null_r16_l_                       = bounded_array<uint16_t, 16>;
  using ncjt_type2_ps_null_r16_l_                    = bounded_array<uint16_t, 16>;
  using ncjt_e_type2_r1_null_r16_l_                  = bounded_array<uint16_t, 16>;
  using ncjt_e_type2_r2_null_r16_l_                  = bounded_array<uint16_t, 16>;
  using ncjt_e_type2_r1_ps_null_r16_l_               = bounded_array<uint16_t, 16>;
  using ncjt_e_type2_r2_ps_null_r16_l_               = bounded_array<uint16_t, 16>;
  using ncjt_type2_type2_ps_r16_l_                   = bounded_array<uint16_t, 16>;
  using ncjt1_sp_type2_null_r16_l_                   = bounded_array<uint16_t, 16>;
  using ncjt1_sp_type2_ps_null_r16_l_                = bounded_array<uint16_t, 16>;
  using ncjt1_sp_e_type2_r1_null_r16_l_              = bounded_array<uint16_t, 16>;
  using ncjt1_sp_e_type2_r2_null_r16_l_              = bounded_array<uint16_t, 16>;
  using ncjt1_sp_e_type2_r1_ps_null_r16_l_           = bounded_array<uint16_t, 16>;
  using ncjt1_sp_e_type2_r2_ps_null_r16_l_           = bounded_array<uint16_t, 16>;
  using ncjt1_sp_type2_type2_ps_r16_l_               = bounded_array<uint16_t, 16>;
  using ncjt_fe_type2_ps_null_r17_l_                 = bounded_array<uint16_t, 16>;
  using ncjt_fe_type2_ps_m2_r1_null_r17_l_           = bounded_array<uint16_t, 16>;
  using ncjt_fe_type2_ps_m2_r2_null_r17_l_           = bounded_array<uint16_t, 16>;
  using ncjt_type2_fe_type2_ps_m1_r17_l_             = bounded_array<uint16_t, 16>;
  using ncjt_type2_fe_type2_ps_m2_r1_r17_l_          = bounded_array<uint16_t, 16>;
  using ncjt_e_type2_r1_fe_type2_ps_m1_r17_l_        = bounded_array<uint16_t, 16>;
  using ncjt_e_type2_r1_fe_type2_ps_m2_r1_r17_l_     = bounded_array<uint16_t, 16>;
  using ncjt1_sp_fe_type2_ps_null_r17_l_             = bounded_array<uint16_t, 16>;
  using ncjt1_sp_fe_type2_ps_m2_r1_null_r17_l_       = bounded_array<uint16_t, 16>;
  using ncjt1_sp_fe_type2_ps_m2_r2_null_r1_l_        = bounded_array<uint16_t, 16>;
  using ncjt1_sp_type2_fe_type2_ps_m1_r17_l_         = bounded_array<uint16_t, 16>;
  using ncjt1_sp_type2_fe_type2_ps_m2_r1_r17_l_      = bounded_array<uint16_t, 16>;
  using ncjt1_sp_e_type2_r1_fe_type2_ps_m1_r17_l_    = bounded_array<uint16_t, 16>;
  using ncjt1_sp_e_type2_r1_fe_type2_ps_m2_r1_r17_l_ = bounded_array<uint16_t, 16>;

  // member variables
  ncjt_null_null_l_                            ncjt_null_null;
  ncjt1_sp_null_null_l_                        ncjt1_sp_null_null;
  ncjt_type2_null_r16_l_                       ncjt_type2_null_r16;
  ncjt_type2_ps_null_r16_l_                    ncjt_type2_ps_null_r16;
  ncjt_e_type2_r1_null_r16_l_                  ncjt_e_type2_r1_null_r16;
  ncjt_e_type2_r2_null_r16_l_                  ncjt_e_type2_r2_null_r16;
  ncjt_e_type2_r1_ps_null_r16_l_               ncjt_e_type2_r1_ps_null_r16;
  ncjt_e_type2_r2_ps_null_r16_l_               ncjt_e_type2_r2_ps_null_r16;
  ncjt_type2_type2_ps_r16_l_                   ncjt_type2_type2_ps_r16;
  ncjt1_sp_type2_null_r16_l_                   ncjt1_sp_type2_null_r16;
  ncjt1_sp_type2_ps_null_r16_l_                ncjt1_sp_type2_ps_null_r16;
  ncjt1_sp_e_type2_r1_null_r16_l_              ncjt1_sp_e_type2_r1_null_r16;
  ncjt1_sp_e_type2_r2_null_r16_l_              ncjt1_sp_e_type2_r2_null_r16;
  ncjt1_sp_e_type2_r1_ps_null_r16_l_           ncjt1_sp_e_type2_r1_ps_null_r16;
  ncjt1_sp_e_type2_r2_ps_null_r16_l_           ncjt1_sp_e_type2_r2_ps_null_r16;
  ncjt1_sp_type2_type2_ps_r16_l_               ncjt1_sp_type2_type2_ps_r16;
  ncjt_fe_type2_ps_null_r17_l_                 ncjt_fe_type2_ps_null_r17;
  ncjt_fe_type2_ps_m2_r1_null_r17_l_           ncjt_fe_type2_ps_m2_r1_null_r17;
  ncjt_fe_type2_ps_m2_r2_null_r17_l_           ncjt_fe_type2_ps_m2_r2_null_r17;
  ncjt_type2_fe_type2_ps_m1_r17_l_             ncjt_type2_fe_type2_ps_m1_r17;
  ncjt_type2_fe_type2_ps_m2_r1_r17_l_          ncjt_type2_fe_type2_ps_m2_r1_r17;
  ncjt_e_type2_r1_fe_type2_ps_m1_r17_l_        ncjt_e_type2_r1_fe_type2_ps_m1_r17;
  ncjt_e_type2_r1_fe_type2_ps_m2_r1_r17_l_     ncjt_e_type2_r1_fe_type2_ps_m2_r1_r17;
  ncjt1_sp_fe_type2_ps_null_r17_l_             ncjt1_sp_fe_type2_ps_null_r17;
  ncjt1_sp_fe_type2_ps_m2_r1_null_r17_l_       ncjt1_sp_fe_type2_ps_m2_r1_null_r17;
  ncjt1_sp_fe_type2_ps_m2_r2_null_r1_l_        ncjt1_sp_fe_type2_ps_m2_r2_null_r1;
  ncjt1_sp_type2_fe_type2_ps_m1_r17_l_         ncjt1_sp_type2_fe_type2_ps_m1_r17;
  ncjt1_sp_type2_fe_type2_ps_m2_r1_r17_l_      ncjt1_sp_type2_fe_type2_ps_m2_r1_r17;
  ncjt1_sp_e_type2_r1_fe_type2_ps_m1_r17_l_    ncjt1_sp_e_type2_r1_fe_type2_ps_m1_r17;
  ncjt1_sp_e_type2_r1_fe_type2_ps_m2_r1_r17_l_ ncjt1_sp_e_type2_r1_fe_type2_ps_m2_r1_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookComboParametersAddition-r16 ::= SEQUENCE
struct codebook_combo_params_addition_r16_s {
  struct type1_sp_type2_null_r16_s_ {
    using supported_csi_rs_res_list_add_r16_l_ = bounded_array<uint16_t, 16>;

    // member variables
    supported_csi_rs_res_list_add_r16_l_ supported_csi_rs_res_list_add_r16;
  };
  struct type1_sp_type2_ps_null_r16_s_ {
    using supported_csi_rs_res_list_add_r16_l_ = bounded_array<uint16_t, 16>;

    // member variables
    supported_csi_rs_res_list_add_r16_l_ supported_csi_rs_res_list_add_r16;
  };
  struct type1_sp_e_type2_r1_null_r16_s_ {
    using supported_csi_rs_res_list_add_r16_l_ = bounded_array<uint16_t, 16>;

    // member variables
    supported_csi_rs_res_list_add_r16_l_ supported_csi_rs_res_list_add_r16;
  };
  struct type1_sp_e_type2_r2_null_r16_s_ {
    using supported_csi_rs_res_list_add_r16_l_ = bounded_array<uint16_t, 16>;

    // member variables
    supported_csi_rs_res_list_add_r16_l_ supported_csi_rs_res_list_add_r16;
  };
  struct type1_sp_e_type2_r1_ps_null_r16_s_ {
    using supported_csi_rs_res_list_add_r16_l_ = bounded_array<uint16_t, 16>;

    // member variables
    supported_csi_rs_res_list_add_r16_l_ supported_csi_rs_res_list_add_r16;
  };
  struct type1_sp_e_type2_r2_ps_null_r16_s_ {
    using supported_csi_rs_res_list_add_r16_l_ = bounded_array<uint16_t, 16>;

    // member variables
    supported_csi_rs_res_list_add_r16_l_ supported_csi_rs_res_list_add_r16;
  };
  struct type1_sp_type2_type2_ps_r16_s_ {
    using supported_csi_rs_res_list_add_r16_l_ = bounded_array<uint16_t, 16>;

    // member variables
    supported_csi_rs_res_list_add_r16_l_ supported_csi_rs_res_list_add_r16;
  };
  struct type1_mp_type2_null_r16_s_ {
    using supported_csi_rs_res_list_add_r16_l_ = bounded_array<uint16_t, 16>;

    // member variables
    supported_csi_rs_res_list_add_r16_l_ supported_csi_rs_res_list_add_r16;
  };
  struct type1_mp_type2_ps_null_r16_s_ {
    using supported_csi_rs_res_list_add_r16_l_ = bounded_array<uint16_t, 16>;

    // member variables
    supported_csi_rs_res_list_add_r16_l_ supported_csi_rs_res_list_add_r16;
  };
  struct type1_mp_e_type2_r1_null_r16_s_ {
    using supported_csi_rs_res_list_add_r16_l_ = bounded_array<uint16_t, 16>;

    // member variables
    supported_csi_rs_res_list_add_r16_l_ supported_csi_rs_res_list_add_r16;
  };
  struct type1_mp_e_type2_r2_null_r16_s_ {
    using supported_csi_rs_res_list_add_r16_l_ = bounded_array<uint16_t, 16>;

    // member variables
    supported_csi_rs_res_list_add_r16_l_ supported_csi_rs_res_list_add_r16;
  };
  struct type1_mp_e_type2_r1_ps_null_r16_s_ {
    using supported_csi_rs_res_list_add_r16_l_ = bounded_array<uint16_t, 16>;

    // member variables
    supported_csi_rs_res_list_add_r16_l_ supported_csi_rs_res_list_add_r16;
  };
  struct type1_mp_e_type2_r2_ps_null_r16_s_ {
    using supported_csi_rs_res_list_add_r16_l_ = bounded_array<uint16_t, 16>;

    // member variables
    supported_csi_rs_res_list_add_r16_l_ supported_csi_rs_res_list_add_r16;
  };
  struct type1_mp_type2_type2_ps_r16_s_ {
    using supported_csi_rs_res_list_add_r16_l_ = bounded_array<uint16_t, 16>;

    // member variables
    supported_csi_rs_res_list_add_r16_l_ supported_csi_rs_res_list_add_r16;
  };

  // member variables
  bool                               type1_sp_type2_null_r16_present         = false;
  bool                               type1_sp_type2_ps_null_r16_present      = false;
  bool                               type1_sp_e_type2_r1_null_r16_present    = false;
  bool                               type1_sp_e_type2_r2_null_r16_present    = false;
  bool                               type1_sp_e_type2_r1_ps_null_r16_present = false;
  bool                               type1_sp_e_type2_r2_ps_null_r16_present = false;
  bool                               type1_sp_type2_type2_ps_r16_present     = false;
  bool                               type1_mp_type2_null_r16_present         = false;
  bool                               type1_mp_type2_ps_null_r16_present      = false;
  bool                               type1_mp_e_type2_r1_null_r16_present    = false;
  bool                               type1_mp_e_type2_r2_null_r16_present    = false;
  bool                               type1_mp_e_type2_r1_ps_null_r16_present = false;
  bool                               type1_mp_e_type2_r2_ps_null_r16_present = false;
  bool                               type1_mp_type2_type2_ps_r16_present     = false;
  type1_sp_type2_null_r16_s_         type1_sp_type2_null_r16;
  type1_sp_type2_ps_null_r16_s_      type1_sp_type2_ps_null_r16;
  type1_sp_e_type2_r1_null_r16_s_    type1_sp_e_type2_r1_null_r16;
  type1_sp_e_type2_r2_null_r16_s_    type1_sp_e_type2_r2_null_r16;
  type1_sp_e_type2_r1_ps_null_r16_s_ type1_sp_e_type2_r1_ps_null_r16;
  type1_sp_e_type2_r2_ps_null_r16_s_ type1_sp_e_type2_r2_ps_null_r16;
  type1_sp_type2_type2_ps_r16_s_     type1_sp_type2_type2_ps_r16;
  type1_mp_type2_null_r16_s_         type1_mp_type2_null_r16;
  type1_mp_type2_ps_null_r16_s_      type1_mp_type2_ps_null_r16;
  type1_mp_e_type2_r1_null_r16_s_    type1_mp_e_type2_r1_null_r16;
  type1_mp_e_type2_r2_null_r16_s_    type1_mp_e_type2_r2_null_r16;
  type1_mp_e_type2_r1_ps_null_r16_s_ type1_mp_e_type2_r1_ps_null_r16;
  type1_mp_e_type2_r2_ps_null_r16_s_ type1_mp_e_type2_r2_ps_null_r16;
  type1_mp_type2_type2_ps_r16_s_     type1_mp_type2_type2_ps_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookParameters ::= SEQUENCE
struct codebook_params_s {
  struct type1_s_ {
    struct single_panel_s_ {
      using supported_csi_rs_res_list_l_ = dyn_array<supported_csi_rs_res_s>;
      struct modes_opts {
        enum options { mode1, mode1and_mode2, nulltype } value;

        const char* to_string() const;
      };
      using modes_e_ = enumerated<modes_opts>;

      // member variables
      supported_csi_rs_res_list_l_ supported_csi_rs_res_list;
      modes_e_                     modes;
      uint8_t                      max_num_csi_rs_per_res_set = 1;
    };
    struct multi_panel_s_ {
      using supported_csi_rs_res_list_l_ = dyn_array<supported_csi_rs_res_s>;
      struct modes_opts {
        enum options { mode1, mode2, both, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using modes_e_ = enumerated<modes_opts>;
      struct nrof_panels_opts {
        enum options { n2, n4, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using nrof_panels_e_ = enumerated<nrof_panels_opts>;

      // member variables
      supported_csi_rs_res_list_l_ supported_csi_rs_res_list;
      modes_e_                     modes;
      nrof_panels_e_               nrof_panels;
      uint8_t                      max_num_csi_rs_per_res_set = 1;
    };

    // member variables
    bool            multi_panel_present = false;
    single_panel_s_ single_panel;
    multi_panel_s_  multi_panel;
  };
  struct type2_s_ {
    using supported_csi_rs_res_list_l_ = dyn_array<supported_csi_rs_res_s>;
    struct amplitude_scaling_type_opts {
      enum options { wideband, wideband_and_subband, nulltype } value;

      const char* to_string() const;
    };
    using amplitude_scaling_type_e_ = enumerated<amplitude_scaling_type_opts>;

    // member variables
    bool                         amplitude_subset_restrict_present = false;
    supported_csi_rs_res_list_l_ supported_csi_rs_res_list;
    uint8_t                      param_lx = 2;
    amplitude_scaling_type_e_    amplitude_scaling_type;
  };
  struct type2_port_sel_s_ {
    using supported_csi_rs_res_list_l_ = dyn_array<supported_csi_rs_res_s>;
    struct amplitude_scaling_type_opts {
      enum options { wideband, wideband_and_subband, nulltype } value;

      const char* to_string() const;
    };
    using amplitude_scaling_type_e_ = enumerated<amplitude_scaling_type_opts>;

    // member variables
    supported_csi_rs_res_list_l_ supported_csi_rs_res_list;
    uint8_t                      param_lx = 2;
    amplitude_scaling_type_e_    amplitude_scaling_type;
  };

  // member variables
  bool              type2_present          = false;
  bool              type2_port_sel_present = false;
  type1_s_          type1;
  type2_s_          type2;
  type2_port_sel_s_ type2_port_sel;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookParametersAddition-r16 ::= SEQUENCE
struct codebook_params_addition_r16_s {
  struct etype2_r16_s_ {
    struct etype2_r1_r16_s_ {
      using supported_csi_rs_res_list_add_r16_l_ = bounded_array<uint16_t, 16>;

      // member variables
      supported_csi_rs_res_list_add_r16_l_ supported_csi_rs_res_list_add_r16;
    };
    struct etype2_r2_r16_s_ {
      using supported_csi_rs_res_list_add_r16_l_ = bounded_array<uint16_t, 16>;

      // member variables
      supported_csi_rs_res_list_add_r16_l_ supported_csi_rs_res_list_add_r16;
    };

    // member variables
    bool             etype2_r2_r16_present                 = false;
    bool             param_comb7_8_r16_present             = false;
    bool             rank3_4_r16_present                   = false;
    bool             amplitude_subset_restrict_r16_present = false;
    etype2_r1_r16_s_ etype2_r1_r16;
    etype2_r2_r16_s_ etype2_r2_r16;
  };
  struct etype2_ps_r16_s_ {
    struct etype2_r1_port_sel_r16_s_ {
      using supported_csi_rs_res_list_add_r16_l_ = bounded_array<uint16_t, 16>;

      // member variables
      supported_csi_rs_res_list_add_r16_l_ supported_csi_rs_res_list_add_r16;
    };
    struct etype2_r2_port_sel_r16_s_ {
      using supported_csi_rs_res_list_add_r16_l_ = bounded_array<uint16_t, 16>;

      // member variables
      supported_csi_rs_res_list_add_r16_l_ supported_csi_rs_res_list_add_r16;
    };

    // member variables
    bool                      etype2_r2_port_sel_r16_present = false;
    bool                      rank3_4_r16_present            = false;
    etype2_r1_port_sel_r16_s_ etype2_r1_port_sel_r16;
    etype2_r2_port_sel_r16_s_ etype2_r2_port_sel_r16;
  };

  // member variables
  bool             etype2_r16_present    = false;
  bool             etype2_ps_r16_present = false;
  etype2_r16_s_    etype2_r16;
  etype2_ps_r16_s_ etype2_ps_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookParametersfetype2-r17 ::= SEQUENCE
struct codebook_paramsfetype2_r17_s {
  using fetype2basic_r17_l_ = bounded_array<uint16_t, 16>;
  using fetype2_r1_r17_l_   = bounded_array<uint16_t, 8>;
  using fetype2_r2_r17_l_   = bounded_array<uint16_t, 8>;

  // member variables
  bool                fetype2_rank3_rank4_r17_present = false;
  fetype2basic_r17_l_ fetype2basic_r17;
  fetype2_r1_r17_l_   fetype2_r1_r17;
  fetype2_r2_r17_l_   fetype2_r2_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DummyG ::= SEQUENCE
struct dummy_g_s {
  struct max_num_ssb_csi_rs_res_one_tx_opts {
    enum options { n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_ssb_csi_rs_res_one_tx_e_ = enumerated<max_num_ssb_csi_rs_res_one_tx_opts>;
  struct max_num_ssb_csi_rs_res_two_tx_opts {
    enum options { n0, n4, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_ssb_csi_rs_res_two_tx_e_ = enumerated<max_num_ssb_csi_rs_res_two_tx_opts>;
  struct supported_csi_rs_density_opts {
    enum options { one, three, one_and_three, nulltype } value;

    const char* to_string() const;
  };
  using supported_csi_rs_density_e_ = enumerated<supported_csi_rs_density_opts>;

  // member variables
  max_num_ssb_csi_rs_res_one_tx_e_ max_num_ssb_csi_rs_res_one_tx;
  max_num_ssb_csi_rs_res_two_tx_e_ max_num_ssb_csi_rs_res_two_tx;
  supported_csi_rs_density_e_      supported_csi_rs_density;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DummyH ::= SEQUENCE
struct dummy_h_s {
  uint8_t burst_len                 = 1;
  uint8_t max_simul_res_sets_per_cc = 1;
  uint8_t max_cfg_res_sets_per_cc   = 1;
  uint8_t max_cfg_res_sets_all_cc   = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PTRS-DensityRecommendationDL ::= SEQUENCE
struct ptrs_density_recommendation_dl_s {
  uint16_t freq_density1 = 1;
  uint16_t freq_density2 = 1;
  uint8_t  time_density1 = 0;
  uint8_t  time_density2 = 0;
  uint8_t  time_density3 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PTRS-DensityRecommendationUL ::= SEQUENCE
struct ptrs_density_recommendation_ul_s {
  uint16_t freq_density1   = 1;
  uint16_t freq_density2   = 1;
  uint8_t  time_density1   = 0;
  uint8_t  time_density2   = 0;
  uint8_t  time_density3   = 0;
  uint16_t sample_density1 = 1;
  uint16_t sample_density2 = 1;
  uint16_t sample_density3 = 1;
  uint16_t sample_density4 = 1;
  uint16_t sample_density5 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-Resources ::= SEQUENCE
struct srs_res_features_s {
  struct max_num_aperiodic_srs_per_bwp_opts {
    enum options { n1, n2, n4, n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_aperiodic_srs_per_bwp_e_ = enumerated<max_num_aperiodic_srs_per_bwp_opts>;
  struct max_num_periodic_srs_per_bwp_opts {
    enum options { n1, n2, n4, n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_periodic_srs_per_bwp_e_ = enumerated<max_num_periodic_srs_per_bwp_opts>;
  struct max_num_semi_persistent_srs_per_bwp_opts {
    enum options { n1, n2, n4, n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_semi_persistent_srs_per_bwp_e_ = enumerated<max_num_semi_persistent_srs_per_bwp_opts>;
  struct max_num_srs_ports_per_res_opts {
    enum options { n1, n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_srs_ports_per_res_e_ = enumerated<max_num_srs_ports_per_res_opts>;

  // member variables
  max_num_aperiodic_srs_per_bwp_e_       max_num_aperiodic_srs_per_bwp;
  uint8_t                                max_num_aperiodic_srs_per_bwp_per_slot = 1;
  max_num_periodic_srs_per_bwp_e_        max_num_periodic_srs_per_bwp;
  uint8_t                                max_num_periodic_srs_per_bwp_per_slot = 1;
  max_num_semi_persistent_srs_per_bwp_e_ max_num_semi_persistent_srs_per_bwp;
  uint8_t                                max_num_semi_persistent_srs_per_bwp_per_slot = 1;
  max_num_srs_ports_per_res_e_           max_num_srs_ports_per_res;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SpatialRelations ::= SEQUENCE
struct spatial_relations_s {
  struct max_num_cfg_spatial_relations_opts {
    enum options { n4, n8, n16, n32, n64, n96, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_cfg_spatial_relations_e_ = enumerated<max_num_cfg_spatial_relations_opts>;
  struct max_num_active_spatial_relations_opts {
    enum options { n1, n2, n4, n8, n14, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_active_spatial_relations_e_ = enumerated<max_num_active_spatial_relations_opts>;
  struct max_num_dl_rs_qcl_type_d_opts {
    enum options { n1, n2, n4, n8, n14, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_dl_rs_qcl_type_d_e_ = enumerated<max_num_dl_rs_qcl_type_d_opts>;

  // member variables
  bool                                add_active_spatial_relation_pucch_present = false;
  max_num_cfg_spatial_relations_e_    max_num_cfg_spatial_relations;
  max_num_active_spatial_relations_e_ max_num_active_spatial_relations;
  max_num_dl_rs_qcl_type_d_e_         max_num_dl_rs_qcl_type_d;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FR2-2-AccessParamsPerBand-r17 ::= SEQUENCE
struct fr2_2_access_params_per_band_r17_s {
  struct enhanced_pdcch_monitoring_scs_960k_hz_r17_s_ {
    bool pdcch_monitoring4_1_r17_present = false;
    bool pdcch_monitoring4_2_r17_present = false;
    bool pdcch_monitoring8_4_r17_present = false;
  };
  struct support32_dl_harq_process_per_scs_r17_s_ {
    bool scs_120k_hz_r17_present = false;
    bool scs_480k_hz_r17_present = false;
    bool scs_960k_hz_r17_present = false;
  };
  struct support32_ul_harq_process_per_scs_r17_s_ {
    bool scs_120k_hz_r17_present = false;
    bool scs_480k_hz_r17_present = false;
    bool scs_960k_hz_r17_present = false;
  };

  // member variables
  bool                                         ext                                                  = false;
  bool                                         dl_fr2_2_scs_120k_hz_r17_present                     = false;
  bool                                         ul_fr2_2_scs_120k_hz_r17_present                     = false;
  bool                                         init_access_ssb_120k_hz_r17_present                  = false;
  bool                                         wideband_prach_scs_120k_hz_r17_present               = false;
  bool                                         multi_rb_pucch_scs_120k_hz_r17_present               = false;
  bool                                         multi_pdsch_single_dci_fr2_2_scs_120k_hz_r17_present = false;
  bool                                         multi_pusch_single_dci_fr2_2_scs_120k_hz_r17_present = false;
  bool                                         dl_fr2_2_scs_480k_hz_r17_present                     = false;
  bool                                         ul_fr2_2_scs_480k_hz_r17_present                     = false;
  bool                                         init_access_ssb_480k_hz_r17_present                  = false;
  bool                                         wideband_prach_scs_480k_hz_r17_present               = false;
  bool                                         multi_rb_pucch_scs_480k_hz_r17_present               = false;
  bool                                         enhanced_pdcch_monitoring_scs_480k_hz_r17_present    = false;
  bool                                         dl_fr2_2_scs_960k_hz_r17_present                     = false;
  bool                                         ul_fr2_2_scs_960k_hz_r17_present                     = false;
  bool                                         multi_rb_pucch_scs_960k_hz_r17_present               = false;
  bool                                         enhanced_pdcch_monitoring_scs_960k_hz_r17_present    = false;
  bool                                         type1_ch_access_fr2_2_r17_present                    = false;
  bool                                         type2_ch_access_fr2_2_r17_present                    = false;
  bool                                         reduced_beam_switch_timing_fr2_2_r17_present         = false;
  bool                                         support32_dl_harq_process_per_scs_r17_present        = false;
  bool                                         support32_ul_harq_process_per_scs_r17_present        = false;
  enhanced_pdcch_monitoring_scs_960k_hz_r17_s_ enhanced_pdcch_monitoring_scs_960k_hz_r17;
  support32_dl_harq_process_per_scs_r17_s_     support32_dl_harq_process_per_scs_r17;
  support32_ul_harq_process_per_scs_r17_s_     support32_ul_harq_process_per_scs_r17;
  // ...
  // group 0
  bool mod64_qam_pusch_fr2_2_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-ParametersPerBand ::= SEQUENCE
struct mimo_params_per_band_s {
  struct tci_state_pdsch_s_ {
    struct max_num_cfg_tci_states_per_cc_opts {
      enum options { n4, n8, n16, n32, n64, n128, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_cfg_tci_states_per_cc_e_ = enumerated<max_num_cfg_tci_states_per_cc_opts>;
    struct max_num_active_tci_per_bwp_opts {
      enum options { n1, n2, n4, n8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_active_tci_per_bwp_e_ = enumerated<max_num_active_tci_per_bwp_opts>;

    // member variables
    bool                             max_num_cfg_tci_states_per_cc_present = false;
    bool                             max_num_active_tci_per_bwp_present    = false;
    max_num_cfg_tci_states_per_cc_e_ max_num_cfg_tci_states_per_cc;
    max_num_active_tci_per_bwp_e_    max_num_active_tci_per_bwp;
  };
  struct pusch_trans_coherence_opts {
    enum options { non_coherent, partial_coherent, full_coherent, nulltype } value;

    const char* to_string() const;
  };
  using pusch_trans_coherence_e_ = enumerated<pusch_trans_coherence_opts>;
  struct max_num_rx_tx_beam_switch_dl_s_ {
    struct scs_15k_hz_opts {
      enum options { n4, n7, n14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_15k_hz_e_ = enumerated<scs_15k_hz_opts>;
    struct scs_30k_hz_opts {
      enum options { n4, n7, n14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_30k_hz_e_ = enumerated<scs_30k_hz_opts>;
    struct scs_60k_hz_opts {
      enum options { n4, n7, n14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_60k_hz_e_ = enumerated<scs_60k_hz_opts>;
    struct scs_120k_hz_opts {
      enum options { n4, n7, n14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_120k_hz_e_ = enumerated<scs_120k_hz_opts>;
    struct scs_240k_hz_opts {
      enum options { n4, n7, n14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_240k_hz_e_ = enumerated<scs_240k_hz_opts>;

    // member variables
    bool           scs_15k_hz_present  = false;
    bool           scs_30k_hz_present  = false;
    bool           scs_60k_hz_present  = false;
    bool           scs_120k_hz_present = false;
    bool           scs_240k_hz_present = false;
    scs_15k_hz_e_  scs_15k_hz;
    scs_30k_hz_e_  scs_30k_hz;
    scs_60k_hz_e_  scs_60k_hz;
    scs_120k_hz_e_ scs_120k_hz;
    scs_240k_hz_e_ scs_240k_hz;
  };
  struct max_num_non_group_beam_report_opts {
    enum options { n1, n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_non_group_beam_report_e_ = enumerated<max_num_non_group_beam_report_opts>;
  struct ul_beam_management_s_ {
    struct max_num_srs_res_per_set_bm_opts {
      enum options { n2, n4, n8, n16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_srs_res_per_set_bm_e_ = enumerated<max_num_srs_res_per_set_bm_opts>;

    // member variables
    max_num_srs_res_per_set_bm_e_ max_num_srs_res_per_set_bm;
    uint8_t                       max_num_srs_res_set = 1;
  };
  struct beam_report_timing_s_ {
    struct scs_15k_hz_opts {
      enum options { sym2, sym4, sym8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_15k_hz_e_ = enumerated<scs_15k_hz_opts>;
    struct scs_30k_hz_opts {
      enum options { sym4, sym8, sym14, sym28, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_30k_hz_e_ = enumerated<scs_30k_hz_opts>;
    struct scs_60k_hz_opts {
      enum options { sym8, sym14, sym28, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_60k_hz_e_ = enumerated<scs_60k_hz_opts>;
    struct scs_120k_hz_opts {
      enum options { sym14, sym28, sym56, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_120k_hz_e_ = enumerated<scs_120k_hz_opts>;

    // member variables
    bool           scs_15k_hz_present  = false;
    bool           scs_30k_hz_present  = false;
    bool           scs_60k_hz_present  = false;
    bool           scs_120k_hz_present = false;
    scs_15k_hz_e_  scs_15k_hz;
    scs_30k_hz_e_  scs_30k_hz;
    scs_60k_hz_e_  scs_60k_hz;
    scs_120k_hz_e_ scs_120k_hz;
  };
  struct ptrs_density_recommendation_set_dl_s_ {
    bool                             scs_15k_hz_present  = false;
    bool                             scs_30k_hz_present  = false;
    bool                             scs_60k_hz_present  = false;
    bool                             scs_120k_hz_present = false;
    ptrs_density_recommendation_dl_s scs_15k_hz;
    ptrs_density_recommendation_dl_s scs_30k_hz;
    ptrs_density_recommendation_dl_s scs_60k_hz;
    ptrs_density_recommendation_dl_s scs_120k_hz;
  };
  struct ptrs_density_recommendation_set_ul_s_ {
    bool                             scs_15k_hz_present  = false;
    bool                             scs_30k_hz_present  = false;
    bool                             scs_60k_hz_present  = false;
    bool                             scs_120k_hz_present = false;
    ptrs_density_recommendation_ul_s scs_15k_hz;
    ptrs_density_recommendation_ul_s scs_30k_hz;
    ptrs_density_recommendation_ul_s scs_60k_hz;
    ptrs_density_recommendation_ul_s scs_120k_hz;
  };
  struct beam_switch_timing_s_ {
    struct scs_60k_hz_opts {
      enum options { sym14, sym28, sym48, sym224, sym336, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using scs_60k_hz_e_ = enumerated<scs_60k_hz_opts>;
    struct scs_120k_hz_opts {
      enum options { sym14, sym28, sym48, sym224, sym336, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using scs_120k_hz_e_ = enumerated<scs_120k_hz_opts>;

    // member variables
    bool           scs_60k_hz_present  = false;
    bool           scs_120k_hz_present = false;
    scs_60k_hz_e_  scs_60k_hz;
    scs_120k_hz_e_ scs_120k_hz;
  };
  using srs_assoc_csi_rs_l_ = dyn_array<supported_csi_rs_res_s>;
  struct max_num_scell_bfr_r16_opts {
    enum options { n1, n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_scell_bfr_r16_e_ = enumerated<max_num_scell_bfr_r16_opts>;
  struct ssb_csirs_sinr_meas_r16_s_ {
    struct max_num_ssb_csi_rs_one_tx_cmr_r16_opts {
      enum options { n8, n16, n32, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_ssb_csi_rs_one_tx_cmr_r16_e_ = enumerated<max_num_ssb_csi_rs_one_tx_cmr_r16_opts>;
    struct max_num_csi_im_nzp_imr_res_r16_opts {
      enum options { n8, n16, n32, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_csi_im_nzp_imr_res_r16_e_ = enumerated<max_num_csi_im_nzp_imr_res_r16_opts>;
    struct max_num_csi_rs_2_tx_res_r16_opts {
      enum options { n0, n4, n8, n16, n32, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_csi_rs_2_tx_res_r16_e_ = enumerated<max_num_csi_rs_2_tx_res_r16_opts>;
    struct max_num_ssb_csi_rs_res_r16_opts {
      enum options { n8, n16, n32, n64, n128, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_ssb_csi_rs_res_r16_e_ = enumerated<max_num_ssb_csi_rs_res_r16_opts>;
    struct max_num_csi_im_nzp_imr_res_mem_r16_opts {
      enum options { n8, n16, n32, n64, n128, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_csi_im_nzp_imr_res_mem_r16_e_ = enumerated<max_num_csi_im_nzp_imr_res_mem_r16_opts>;
    struct supported_csi_rs_density_cmr_r16_opts {
      enum options { one, three, one_and_three, nulltype } value;

      const char* to_string() const;
    };
    using supported_csi_rs_density_cmr_r16_e_ = enumerated<supported_csi_rs_density_cmr_r16_opts>;
    struct max_num_aperiodic_csi_rs_res_r16_opts {
      enum options { n2, n4, n8, n16, n32, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_aperiodic_csi_rs_res_r16_e_ = enumerated<max_num_aperiodic_csi_rs_res_r16_opts>;
    struct supported_sinr_meas_r16_opts {
      enum options { ssb_with_csi_im, ssb_with_nzp_imr, csirs_with_nzp_imr, csi_rs_without_imr, nulltype } value;

      const char* to_string() const;
    };
    using supported_sinr_meas_r16_e_ = enumerated<supported_sinr_meas_r16_opts>;

    // member variables
    bool                                  supported_sinr_meas_r16_present = false;
    max_num_ssb_csi_rs_one_tx_cmr_r16_e_  max_num_ssb_csi_rs_one_tx_cmr_r16;
    max_num_csi_im_nzp_imr_res_r16_e_     max_num_csi_im_nzp_imr_res_r16;
    max_num_csi_rs_2_tx_res_r16_e_        max_num_csi_rs_2_tx_res_r16;
    max_num_ssb_csi_rs_res_r16_e_         max_num_ssb_csi_rs_res_r16;
    max_num_csi_im_nzp_imr_res_mem_r16_e_ max_num_csi_im_nzp_imr_res_mem_r16;
    supported_csi_rs_density_cmr_r16_e_   supported_csi_rs_density_cmr_r16;
    max_num_aperiodic_csi_rs_res_r16_e_   max_num_aperiodic_csi_rs_res_r16;
    supported_sinr_meas_r16_e_            supported_sinr_meas_r16;
  };
  struct non_group_sinr_report_r16_opts {
    enum options { n1, n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using non_group_sinr_report_r16_e_ = enumerated<non_group_sinr_report_r16_opts>;
  struct multi_dci_multi_trp_params_r16_s_ {
    struct out_of_order_operation_dl_r16_s_ {
      bool support_pdcch_to_pdsch_r16_present    = false;
      bool support_pdsch_to_harq_ack_r16_present = false;
    };
    struct max_num_activ_tci_states_r16_s_ {
      struct max_num_per_coreset_pool_r16_opts {
        enum options { n1, n2, n4, n8, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using max_num_per_coreset_pool_r16_e_ = enumerated<max_num_per_coreset_pool_r16_opts>;
      struct max_total_num_across_coreset_pool_r16_opts {
        enum options { n2, n4, n8, n16, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using max_total_num_across_coreset_pool_r16_e_ = enumerated<max_total_num_across_coreset_pool_r16_opts>;

      // member variables
      max_num_per_coreset_pool_r16_e_          max_num_per_coreset_pool_r16;
      max_total_num_across_coreset_pool_r16_e_ max_total_num_across_coreset_pool_r16;
    };

    // member variables
    bool                             overlap_pdsc_hs_fully_freq_time_r16_present        = false;
    bool                             overlap_pdsc_hs_in_time_partially_freq_r16_present = false;
    bool                             out_of_order_operation_dl_r16_present              = false;
    bool                             out_of_order_operation_ul_r16_present              = false;
    bool                             separate_crs_rate_matching_r16_present             = false;
    bool                             default_qcl_per_coreset_pool_idx_r16_present       = false;
    bool                             max_num_activ_tci_states_r16_present               = false;
    uint8_t                          overlap_pdsc_hs_fully_freq_time_r16                = 1;
    out_of_order_operation_dl_r16_s_ out_of_order_operation_dl_r16;
    max_num_activ_tci_states_r16_s_  max_num_activ_tci_states_r16;
  };
  struct single_dci_sdm_scheme_params_r16_s_ {
    struct support_new_dmrs_port_r16_opts {
      enum options { supported1, supported2, supported3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using support_new_dmrs_port_r16_e_ = enumerated<support_new_dmrs_port_r16_opts>;

    // member variables
    bool                         support_new_dmrs_port_r16_present    = false;
    bool                         support_two_port_dl_ptrs_r16_present = false;
    support_new_dmrs_port_r16_e_ support_new_dmrs_port_r16;
  };
  struct support_tdm_scheme_a_r16_opts {
    enum options { kb3, kb5, kb10, kb20, no_restrict, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using support_tdm_scheme_a_r16_e_ = enumerated<support_tdm_scheme_a_r16_opts>;
  struct support_inter_slot_tdm_r16_s_ {
    struct support_rep_num_pdsch_tdra_r16_opts {
      enum options { n2, n3, n4, n5, n6, n7, n8, n16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using support_rep_num_pdsch_tdra_r16_e_ = enumerated<support_rep_num_pdsch_tdra_r16_opts>;
    struct max_tbs_size_r16_opts {
      enum options { kb3, kb5, kb10, kb20, no_restrict, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_tbs_size_r16_e_ = enumerated<max_tbs_size_r16_opts>;

    // member variables
    support_rep_num_pdsch_tdra_r16_e_ support_rep_num_pdsch_tdra_r16;
    max_tbs_size_r16_e_               max_tbs_size_r16;
    uint8_t                           max_num_tci_states_r16 = 1;
  };
  struct beam_switch_timing_r16_s_ {
    struct scs_60k_hz_r16_opts {
      enum options { sym224, sym336, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using scs_60k_hz_r16_e_ = enumerated<scs_60k_hz_r16_opts>;
    struct scs_120k_hz_r16_opts {
      enum options { sym224, sym336, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using scs_120k_hz_r16_e_ = enumerated<scs_120k_hz_r16_opts>;

    // member variables
    bool               scs_60k_hz_r16_present  = false;
    bool               scs_120k_hz_r16_present = false;
    scs_60k_hz_r16_e_  scs_60k_hz_r16;
    scs_120k_hz_r16_e_ scs_120k_hz_r16;
  };
  struct semi_persistent_l1_sinr_report_pucch_r16_s_ {
    bool support_report_format1_2_ofdm_syms_r16_present  = false;
    bool support_report_format4_14_ofdm_syms_r16_present = false;
  };
  struct spatial_relations_v1640_s_ {
    struct max_num_cfg_spatial_relations_v1640_opts {
      enum options { n96, n128, n160, n192, n224, n256, n288, n320, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using max_num_cfg_spatial_relations_v1640_e_ = enumerated<max_num_cfg_spatial_relations_v1640_opts>;

    // member variables
    max_num_cfg_spatial_relations_v1640_e_ max_num_cfg_spatial_relations_v1640;
  };
  struct mtrp_pucch_inter_slot_r17_opts {
    enum options { pf0_neg2, pf1_neg3_neg4, pf0_neg4, nulltype } value;

    const char* to_string() const;
  };
  using mtrp_pucch_inter_slot_r17_e_ = enumerated<mtrp_pucch_inter_slot_r17_opts>;
  struct mtrp_bfr_two_bfd_rs_set_r17_s_ {
    struct max_bfd_rs_res_per_set_per_bwp_r17_opts {
      enum options { n1, n2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_bfd_rs_res_per_set_per_bwp_r17_e_ = enumerated<max_bfd_rs_res_per_set_per_bwp_r17_opts>;
    struct max_bfd_rs_res_across_sets_per_bwp_r17_opts {
      enum options { n2, n3, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_bfd_rs_res_across_sets_per_bwp_r17_e_ = enumerated<max_bfd_rs_res_across_sets_per_bwp_r17_opts>;

    // member variables
    max_bfd_rs_res_per_set_per_bwp_r17_e_     max_bfd_rs_res_per_set_per_bwp_r17;
    uint8_t                                   max_bfr_r17 = 1;
    max_bfd_rs_res_across_sets_per_bwp_r17_e_ max_bfd_rs_res_across_sets_per_bwp_r17;
  };
  struct mtrp_bfr_pucch_sr_per_cg_r17_opts {
    enum options { n1, n2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mtrp_bfr_pucch_sr_per_cg_r17_e_ = enumerated<mtrp_bfr_pucch_sr_per_cg_r17_opts>;
  struct srs_trigger_offset_r17_opts {
    enum options { n1, n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using srs_trigger_offset_r17_e_ = enumerated<srs_trigger_offset_r17_opts>;
  struct unified_joint_tci_r17_s_ {
    struct max_cfg_joint_tci_r17_opts {
      enum options { n8, n12, n16, n24, n32, n48, n64, n128, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_cfg_joint_tci_r17_e_ = enumerated<max_cfg_joint_tci_r17_opts>;
    struct max_activ_tci_across_cc_r17_opts {
      enum options { n1, n2, n4, n8, n16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_activ_tci_across_cc_r17_e_ = enumerated<max_activ_tci_across_cc_r17_opts>;

    // member variables
    max_cfg_joint_tci_r17_e_       max_cfg_joint_tci_r17;
    max_activ_tci_across_cc_r17_e_ max_activ_tci_across_cc_r17;
  };
  struct unified_joint_tci_multi_mac_ce_r17_s_ {
    struct min_beam_application_time_r17_opts {
      enum options { n1, n2, n4, n7, n14, n28, n42, n56, n70, n84, n98, n112, n224, n336, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using min_beam_application_time_r17_e_ = enumerated<min_beam_application_time_r17_opts>;
    struct max_num_mac_ce_per_cc_opts {
      enum options { n2, n3, n4, n5, n6, n7, n8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_mac_ce_per_cc_e_ = enumerated<max_num_mac_ce_per_cc_opts>;

    // member variables
    bool                             min_beam_application_time_r17_present = false;
    min_beam_application_time_r17_e_ min_beam_application_time_r17;
    max_num_mac_ce_per_cc_e_         max_num_mac_ce_per_cc;
  };
  struct unified_joint_tci_list_sharing_ca_r17_opts {
    enum options { n1, n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using unified_joint_tci_list_sharing_ca_r17_e_ = enumerated<unified_joint_tci_list_sharing_ca_r17_opts>;
  struct unified_joint_tci_inter_cell_r17_s_ {
    struct add_mac_ce_per_cc_r17_opts {
      enum options { n0, n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using add_mac_ce_per_cc_r17_e_ = enumerated<add_mac_ce_per_cc_r17_opts>;
    struct add_mac_ce_across_cc_r17_opts {
      enum options { n0, n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using add_mac_ce_across_cc_r17_e_ = enumerated<add_mac_ce_across_cc_r17_opts>;

    // member variables
    add_mac_ce_per_cc_r17_e_    add_mac_ce_per_cc_r17;
    add_mac_ce_across_cc_r17_e_ add_mac_ce_across_cc_r17;
  };
  struct unified_separate_tci_r17_s_ {
    struct max_cfg_dl_tci_r17_opts {
      enum options { n4, n8, n12, n16, n24, n32, n48, n64, n128, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_cfg_dl_tci_r17_e_ = enumerated<max_cfg_dl_tci_r17_opts>;
    struct max_cfg_ul_tci_r17_opts {
      enum options { n4, n8, n12, n16, n24, n32, n48, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_cfg_ul_tci_r17_e_ = enumerated<max_cfg_ul_tci_r17_opts>;
    struct max_activ_dl_tci_across_cc_r17_opts {
      enum options { n1, n2, n4, n8, n16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_activ_dl_tci_across_cc_r17_e_ = enumerated<max_activ_dl_tci_across_cc_r17_opts>;
    struct max_activ_ul_tci_across_cc_r17_opts {
      enum options { n1, n2, n4, n8, n16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_activ_ul_tci_across_cc_r17_e_ = enumerated<max_activ_ul_tci_across_cc_r17_opts>;

    // member variables
    max_cfg_dl_tci_r17_e_             max_cfg_dl_tci_r17;
    max_cfg_ul_tci_r17_e_             max_cfg_ul_tci_r17;
    max_activ_dl_tci_across_cc_r17_e_ max_activ_dl_tci_across_cc_r17;
    max_activ_ul_tci_across_cc_r17_e_ max_activ_ul_tci_across_cc_r17;
  };
  struct unified_separate_tci_multi_mac_ce_r17_s_ {
    struct min_beam_application_time_r17_opts {
      enum options { n1, n2, n4, n7, n14, n28, n42, n56, n70, n84, n98, n112, n224, n336, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using min_beam_application_time_r17_e_ = enumerated<min_beam_application_time_r17_opts>;

    // member variables
    min_beam_application_time_r17_e_ min_beam_application_time_r17;
    uint8_t                          max_activ_dl_tci_per_cc_r17 = 2;
    uint8_t                          max_activ_ul_tci_per_cc_r17 = 2;
  };
  struct unified_separate_tci_list_sharing_ca_r17_s_ {
    struct max_num_list_dl_tci_r17_opts {
      enum options { n1, n2, n4, n8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_list_dl_tci_r17_e_ = enumerated<max_num_list_dl_tci_r17_opts>;
    struct max_num_list_ul_tci_r17_opts {
      enum options { n1, n2, n4, n8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_list_ul_tci_r17_e_ = enumerated<max_num_list_ul_tci_r17_opts>;

    // member variables
    bool                       max_num_list_dl_tci_r17_present = false;
    bool                       max_num_list_ul_tci_r17_present = false;
    max_num_list_dl_tci_r17_e_ max_num_list_dl_tci_r17;
    max_num_list_ul_tci_r17_e_ max_num_list_ul_tci_r17;
  };
  struct unified_separate_tci_inter_cell_r17_s_ {
    struct k_dl_per_cc_r17_opts {
      enum options { n0, n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using k_dl_per_cc_r17_e_ = enumerated<k_dl_per_cc_r17_opts>;
    struct k_ul_per_cc_r17_opts {
      enum options { n0, n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using k_ul_per_cc_r17_e_ = enumerated<k_ul_per_cc_r17_opts>;
    struct k_dl_across_cc_r17_opts {
      enum options { n0, n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using k_dl_across_cc_r17_e_ = enumerated<k_dl_across_cc_r17_opts>;
    struct k_ul_across_cc_r17_opts {
      enum options { n0, n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using k_ul_across_cc_r17_e_ = enumerated<k_ul_across_cc_r17_opts>;

    // member variables
    k_dl_per_cc_r17_e_    k_dl_per_cc_r17;
    k_ul_per_cc_r17_e_    k_ul_per_cc_r17;
    k_dl_across_cc_r17_e_ k_dl_across_cc_r17;
    k_ul_across_cc_r17_e_ k_ul_across_cc_r17;
  };
  struct unified_joint_tci_m_trp_inter_cell_bm_r17_s_ {
    struct max_num_ssb_res_l1_rsrp_across_cc_r17_opts {
      enum options { n1, n2, n4, n8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_ssb_res_l1_rsrp_across_cc_r17_e_ = enumerated<max_num_ssb_res_l1_rsrp_across_cc_r17_opts>;

    // member variables
    uint8_t                                  max_num_add_pci_l1_rsrp_r17 = 1;
    max_num_ssb_res_l1_rsrp_across_cc_r17_e_ max_num_ssb_res_l1_rsrp_across_cc_r17;
  };
  struct mpe_mitigation_r17_s_ {
    struct max_num_conf_rs_r17_opts {
      enum options { n1, n2, n4, n8, n12, n16, n28, n32, n48, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_conf_rs_r17_e_ = enumerated<max_num_conf_rs_r17_opts>;

    // member variables
    uint8_t                max_num_p_mpr_ri_pairs_r17 = 1;
    max_num_conf_rs_r17_e_ max_num_conf_rs_r17;
  };
  struct srs_port_report_r17_s_ {
    struct cap_val1_r17_opts {
      enum options { n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using cap_val1_r17_e_ = enumerated<cap_val1_r17_opts>;
    struct cap_val2_r17_opts {
      enum options { n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using cap_val2_r17_e_ = enumerated<cap_val2_r17_opts>;
    struct cap_val3_r17_opts {
      enum options { n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using cap_val3_r17_e_ = enumerated<cap_val3_r17_opts>;
    struct cap_val4_r17_opts {
      enum options { n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using cap_val4_r17_e_ = enumerated<cap_val4_r17_opts>;

    // member variables
    bool            cap_val1_r17_present = false;
    bool            cap_val2_r17_present = false;
    bool            cap_val3_r17_present = false;
    bool            cap_val4_r17_present = false;
    cap_val1_r17_e_ cap_val1_r17;
    cap_val2_r17_e_ cap_val2_r17;
    cap_val3_r17_e_ cap_val3_r17;
    cap_val4_r17_e_ cap_val4_r17;
  };
  struct mtrp_pusch_csi_rs_r17_s_ {
    uint8_t max_num_periodic_srs_r17     = 1;
    uint8_t max_num_aperiodic_srs_r17    = 1;
    uint8_t max_num_sp_srs_r17           = 0;
    uint8_t num_srs_res_per_cc_r17       = 1;
    uint8_t num_srs_res_non_codebook_r17 = 1;
  };
  struct mtrp_pusch_cyclic_map_r17_opts {
    enum options { type_a, type_b, both, nulltype } value;

    const char* to_string() const;
  };
  using mtrp_pusch_cyclic_map_r17_e_ = enumerated<mtrp_pusch_cyclic_map_r17_opts>;
  struct mtrp_inter_cell_r17_s_ {
    uint8_t max_num_add_pci_case1_r17 = 1;
    uint8_t max_num_add_pci_case2_r17 = 0;
  };
  struct mtrp_group_based_l1_rsrp_r17_s_ {
    struct max_num_rs_within_slot_r17_opts {
      enum options { n2, n3, n4, n8, n16, n32, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_rs_within_slot_r17_e_ = enumerated<max_num_rs_within_slot_r17_opts>;
    struct max_num_rs_across_slot_r17_opts {
      enum options { n8, n16, n32, n64, n128, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_rs_across_slot_r17_e_ = enumerated<max_num_rs_across_slot_r17_opts>;

    // member variables
    uint8_t                       max_num_beam_groups_r17 = 1;
    max_num_rs_within_slot_r17_e_ max_num_rs_within_slot_r17;
    max_num_rs_across_slot_r17_e_ max_num_rs_across_slot_r17;
  };
  struct mtrp_bfd_rs_mac_ce_r17_opts {
    enum options { n4, n8, n12, n16, n32, n48, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mtrp_bfd_rs_mac_ce_r17_e_ = enumerated<mtrp_bfd_rs_mac_ce_r17_opts>;
  struct mtrp_csi_enhancement_per_band_r17_s_ {
    struct csi_report_mode_r17_opts {
      enum options { mode1, mode2, both, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using csi_report_mode_r17_e_            = enumerated<csi_report_mode_r17_opts>;
    using supported_combo_across_ccs_r17_l_ = dyn_array<csi_multi_trp_supported_combinations_r17_s>;
    struct codebook_mode_ncjt_r17_opts {
      enum options { mode1, mode1_and2, nulltype } value;

      const char* to_string() const;
    };
    using codebook_mode_ncjt_r17_e_ = enumerated<codebook_mode_ncjt_r17_opts>;

    // member variables
    uint8_t                           max_num_nzp_csi_rs_r17 = 2;
    csi_report_mode_r17_e_            csi_report_mode_r17;
    supported_combo_across_ccs_r17_l_ supported_combo_across_ccs_r17;
    codebook_mode_ncjt_r17_e_         codebook_mode_ncjt_r17;
  };
  struct mtrp_csi_add_csi_r17_opts {
    enum options { x1, x2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mtrp_csi_add_csi_r17_e_ = enumerated<mtrp_csi_add_csi_r17_opts>;
  struct beam_switch_timing_v1710_s_ {
    struct scs_480k_hz_opts {
      enum options { sym56, sym112, sym192, sym896, sym1344, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using scs_480k_hz_e_ = enumerated<scs_480k_hz_opts>;
    struct scs_960k_hz_opts {
      enum options { sym112, sym224, sym384, sym1792, sym2688, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using scs_960k_hz_e_ = enumerated<scs_960k_hz_opts>;

    // member variables
    bool           scs_480k_hz_present = false;
    bool           scs_960k_hz_present = false;
    scs_480k_hz_e_ scs_480k_hz;
    scs_960k_hz_e_ scs_960k_hz;
  };
  struct beam_switch_timing_r17_s_ {
    struct scs_480k_hz_r17_opts {
      enum options { sym896, sym1344, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using scs_480k_hz_r17_e_ = enumerated<scs_480k_hz_r17_opts>;
    struct scs_960k_hz_r17_opts {
      enum options { sym1792, sym2688, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using scs_960k_hz_r17_e_ = enumerated<scs_960k_hz_r17_opts>;

    // member variables
    bool               scs_480k_hz_r17_present = false;
    bool               scs_960k_hz_r17_present = false;
    scs_480k_hz_r17_e_ scs_480k_hz_r17;
    scs_960k_hz_r17_e_ scs_960k_hz_r17;
  };
  struct beam_report_timing_v1710_s_ {
    struct scs_480k_hz_r17_opts {
      enum options { sym56, sym112, sym224, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_480k_hz_r17_e_ = enumerated<scs_480k_hz_r17_opts>;
    struct scs_960k_hz_r17_opts {
      enum options { sym112, sym224, sym448, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using scs_960k_hz_r17_e_ = enumerated<scs_960k_hz_r17_opts>;

    // member variables
    bool               scs_480k_hz_r17_present = false;
    bool               scs_960k_hz_r17_present = false;
    scs_480k_hz_r17_e_ scs_480k_hz_r17;
    scs_960k_hz_r17_e_ scs_960k_hz_r17;
  };
  struct max_num_rx_tx_beam_switch_dl_v1710_s_ {
    struct scs_480k_hz_r17_opts {
      enum options { n2, n4, n7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_480k_hz_r17_e_ = enumerated<scs_480k_hz_r17_opts>;
    struct scs_960k_hz_r17_opts {
      enum options { n1, n2, n4, n7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_960k_hz_r17_e_ = enumerated<scs_960k_hz_r17_opts>;

    // member variables
    bool               scs_480k_hz_r17_present = false;
    bool               scs_960k_hz_r17_present = false;
    scs_480k_hz_r17_e_ scs_480k_hz_r17;
    scs_960k_hz_r17_e_ scs_960k_hz_r17;
  };
  struct mtrp_csi_num_cp_u_r17_opts {
    enum options { n2, n3, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mtrp_csi_num_cp_u_r17_e_ = enumerated<mtrp_csi_num_cp_u_r17_opts>;
  struct support_rep_num_pdsch_tdra_dci_1_2_r17_opts {
    enum options { n2, n3, n4, n5, n6, n7, n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using support_rep_num_pdsch_tdra_dci_1_2_r17_e_ = enumerated<support_rep_num_pdsch_tdra_dci_1_2_r17_opts>;

  // member variables
  bool                                  ext                                                  = false;
  bool                                  tci_state_pdsch_present                              = false;
  bool                                  add_active_tci_state_pdcch_present                   = false;
  bool                                  pusch_trans_coherence_present                        = false;
  bool                                  beam_correspondence_without_ul_beam_sweeping_present = false;
  bool                                  periodic_beam_report_present                         = false;
  bool                                  aperiodic_beam_report_present                        = false;
  bool                                  sp_beam_report_pucch_present                         = false;
  bool                                  sp_beam_report_pusch_present                         = false;
  bool                                  dummy1_present                                       = false;
  bool                                  max_num_rx_beam_present                              = false;
  bool                                  max_num_rx_tx_beam_switch_dl_present                 = false;
  bool                                  max_num_non_group_beam_report_present                = false;
  bool                                  group_beam_report_present                            = false;
  bool                                  ul_beam_management_present                           = false;
  bool                                  max_num_csi_rs_bfd_present                           = false;
  bool                                  max_num_ssb_bfd_present                              = false;
  bool                                  max_num_csi_rs_ssb_cbd_present                       = false;
  bool                                  dummy2_present                                       = false;
  bool                                  two_ports_ptrs_ul_present                            = false;
  bool                                  dummy5_present                                       = false;
  bool                                  dummy3_present                                       = false;
  bool                                  beam_report_timing_present                           = false;
  bool                                  ptrs_density_recommendation_set_dl_present           = false;
  bool                                  ptrs_density_recommendation_set_ul_present           = false;
  bool                                  dummy4_present                                       = false;
  bool                                  aperiodic_trs_present                                = false;
  tci_state_pdsch_s_                    tci_state_pdsch;
  pusch_trans_coherence_e_              pusch_trans_coherence;
  dummy_g_s                             dummy1;
  uint8_t                               max_num_rx_beam = 2;
  max_num_rx_tx_beam_switch_dl_s_       max_num_rx_tx_beam_switch_dl;
  max_num_non_group_beam_report_e_      max_num_non_group_beam_report;
  ul_beam_management_s_                 ul_beam_management;
  uint8_t                               max_num_csi_rs_bfd     = 1;
  uint8_t                               max_num_ssb_bfd        = 1;
  uint16_t                              max_num_csi_rs_ssb_cbd = 1;
  srs_res_features_s                    dummy5;
  uint8_t                               dummy3 = 1;
  beam_report_timing_s_                 beam_report_timing;
  ptrs_density_recommendation_set_dl_s_ ptrs_density_recommendation_set_dl;
  ptrs_density_recommendation_set_ul_s_ ptrs_density_recommendation_set_ul;
  dummy_h_s                             dummy4;
  // ...
  // group 0
  bool                                         dummy6_present = false;
  copy_ptr<beam_management_ssb_csi_rs_s>       beam_management_ssb_csi_rs;
  copy_ptr<beam_switch_timing_s_>              beam_switch_timing;
  copy_ptr<codebook_params_s>                  codebook_params;
  copy_ptr<csi_rs_im_reception_for_feedback_s> csi_rs_im_reception_for_feedback;
  copy_ptr<csi_rs_proc_framework_for_srs_s>    csi_rs_proc_framework_for_srs;
  copy_ptr<csi_report_framework_s>             csi_report_framework;
  copy_ptr<csi_rs_for_tracking_s>              csi_rs_for_tracking;
  copy_ptr<srs_assoc_csi_rs_l_>                srs_assoc_csi_rs;
  copy_ptr<spatial_relations_s>                spatial_relations;
  // group 1
  bool                                           default_qcl_two_tci_r16_present                        = false;
  bool                                           simul_spatial_relation_upd_pucch_res_group_r16_present = false;
  bool                                           max_num_scell_bfr_r16_present                          = false;
  bool                                           simul_reception_diff_type_d_r16_present                = false;
  bool                                           non_group_sinr_report_r16_present                      = false;
  bool                                           group_sinr_report_r16_present                          = false;
  bool                                           support_fdm_scheme_a_r16_present                       = false;
  bool                                           support_code_word_soft_combining_r16_present           = false;
  bool                                           support_tdm_scheme_a_r16_present                       = false;
  bool                                           low_papr_dmrs_pdsch_r16_present                        = false;
  bool                                           low_papr_dmrs_pusc_hwithout_precoding_r16_present      = false;
  bool                                           low_papr_dmrs_pucch_r16_present                        = false;
  bool                                           low_papr_dmrs_pusc_hwith_precoding_r16_present         = false;
  bool                                           beam_correspondence_ssb_based_r16_present              = false;
  bool                                           beam_correspondence_csi_rs_based_r16_present           = false;
  copy_ptr<codebook_params_v1610_s>              codebook_params_per_band_r16;
  max_num_scell_bfr_r16_e_                       max_num_scell_bfr_r16;
  copy_ptr<ssb_csirs_sinr_meas_r16_s_>           ssb_csirs_sinr_meas_r16;
  non_group_sinr_report_r16_e_                   non_group_sinr_report_r16;
  copy_ptr<multi_dci_multi_trp_params_r16_s_>    multi_dci_multi_trp_params_r16;
  copy_ptr<single_dci_sdm_scheme_params_r16_s_>  single_dci_sdm_scheme_params_r16;
  support_tdm_scheme_a_r16_e_                    support_tdm_scheme_a_r16;
  copy_ptr<support_inter_slot_tdm_r16_s_>        support_inter_slot_tdm_r16;
  copy_ptr<csi_report_framework_ext_r16_s>       csi_report_framework_ext_r16;
  copy_ptr<codebook_params_addition_r16_s>       codebook_params_addition_r16;
  copy_ptr<codebook_combo_params_addition_r16_s> codebook_combo_params_addition_r16;
  copy_ptr<beam_switch_timing_r16_s_>            beam_switch_timing_r16;
  // group 2
  bool                                                  semi_persistent_l1_sinr_report_pusch_r16_present = false;
  copy_ptr<semi_persistent_l1_sinr_report_pucch_r16_s_> semi_persistent_l1_sinr_report_pucch_r16;
  // group 3
  bool                                 support64_candidate_beam_rs_bfr_r16_present = false;
  copy_ptr<spatial_relations_v1640_s_> spatial_relations_v1640;
  // group 4
  bool max_mimo_layers_for_multi_dci_m_trp_r16_present = false;
  // group 5
  bool               supported_sinr_meas_v1670_present = false;
  fixed_bitstring<4> supported_sinr_meas_v1670;
  // group 6
  bool                                                   srs_increased_repeat_r17_present                 = false;
  bool                                                   srs_partial_freq_sr17_present                    = false;
  bool                                                   srs_start_rb_location_hop_partial_r17_present    = false;
  bool                                                   srs_comb_eight_r17_present                       = false;
  bool                                                   mtrp_pusch_two_csi_rs_r17_present                = false;
  bool                                                   mtrp_pucch_inter_slot_r17_present                = false;
  bool                                                   mtrp_pucch_cyclic_map_r17_present                = false;
  bool                                                   mtrp_pucch_second_tpc_r17_present                = false;
  bool                                                   mtrp_bfr_pucch_sr_per_cg_r17_present             = false;
  bool                                                   mtrp_bfr_assoc_pucch_sr_r17_present              = false;
  bool                                                   sfn_simul_two_tci_across_multi_cc_r17_present    = false;
  bool                                                   sfn_default_dl_beam_setup_r17_present            = false;
  bool                                                   sfn_default_ul_beam_setup_r17_present            = false;
  bool                                                   srs_trigger_offset_r17_present                   = false;
  bool                                                   srs_trigger_dci_r17_present                      = false;
  bool                                                   unified_joint_tci_per_bwp_ca_r17_present         = false;
  bool                                                   unified_joint_tci_list_sharing_ca_r17_present    = false;
  bool                                                   unified_joint_tci_common_multi_cc_r17_present    = false;
  bool                                                   unified_joint_tci_beam_align_dl_rs_r17_present   = false;
  bool                                                   unified_joint_tci_pc_assoc_r17_present           = false;
  bool                                                   unified_joint_tci_legacy_r17_present             = false;
  bool                                                   unified_joint_tci_legacy_srs_r17_present         = false;
  bool                                                   unified_joint_tci_legacy_coreset0_r17_present    = false;
  bool                                                   unified_joint_tci_scell_bfr_r17_present          = false;
  bool                                                   unified_separate_tci_per_bwp_ca_r17_present      = false;
  bool                                                   unified_separate_tci_common_multi_cc_r17_present = false;
  bool                                                   mtrp_pdcch_individual_r17_present                = false;
  bool                                                   mtrp_pdcch_any_span_3_symbols_r17_present        = false;
  bool                                                   mtrp_pdcch_two_qcl_type_d_r17_present            = false;
  bool                                                   mtrp_pusch_cyclic_map_r17_present                = false;
  bool                                                   mtrp_pusch_second_tpc_r17_present                = false;
  bool                                                   mtrp_pusch_two_phr_report_r17_present            = false;
  bool                                                   mtrp_pusch_a_csi_r17_present                     = false;
  bool                                                   mtrp_pusch_sp_csi_r17_present                    = false;
  bool                                                   mtrp_pusch_cg_r17_present                        = false;
  bool                                                   mtrp_pucch_mac_ce_r17_present                    = false;
  bool                                                   mtrp_pucch_max_num_pc_fr1_r17_present            = false;
  bool                                                   mtrp_bfd_rs_mac_ce_r17_present                   = false;
  bool                                                   mtrp_csi_add_csi_r17_present                     = false;
  bool                                                   mtrp_csi_n_max2_r17_present                      = false;
  bool                                                   mtrp_csi_cmr_r17_present                         = false;
  bool                                                   srs_partial_freq_nohop_sr17_present              = false;
  copy_ptr<codebook_paramsfetype2_r17_s>                 codebook_paramsfetype2_r17;
  mtrp_pucch_inter_slot_r17_e_                           mtrp_pucch_inter_slot_r17;
  copy_ptr<mtrp_bfr_two_bfd_rs_set_r17_s_>               mtrp_bfr_two_bfd_rs_set_r17;
  mtrp_bfr_pucch_sr_per_cg_r17_e_                        mtrp_bfr_pucch_sr_per_cg_r17;
  srs_trigger_offset_r17_e_                              srs_trigger_offset_r17;
  copy_ptr<codebook_combo_param_mixed_type_r17_s>        codebook_combo_param_mixed_type_r17;
  copy_ptr<unified_joint_tci_r17_s_>                     unified_joint_tci_r17;
  copy_ptr<unified_joint_tci_multi_mac_ce_r17_s_>        unified_joint_tci_multi_mac_ce_r17;
  unified_joint_tci_list_sharing_ca_r17_e_               unified_joint_tci_list_sharing_ca_r17;
  copy_ptr<unified_joint_tci_inter_cell_r17_s_>          unified_joint_tci_inter_cell_r17;
  copy_ptr<unified_separate_tci_r17_s_>                  unified_separate_tci_r17;
  copy_ptr<unified_separate_tci_multi_mac_ce_r17_s_>     unified_separate_tci_multi_mac_ce_r17;
  copy_ptr<unified_separate_tci_list_sharing_ca_r17_s_>  unified_separate_tci_list_sharing_ca_r17;
  copy_ptr<unified_separate_tci_inter_cell_r17_s_>       unified_separate_tci_inter_cell_r17;
  copy_ptr<unified_joint_tci_m_trp_inter_cell_bm_r17_s_> unified_joint_tci_m_trp_inter_cell_bm_r17;
  copy_ptr<mpe_mitigation_r17_s_>                        mpe_mitigation_r17;
  copy_ptr<srs_port_report_r17_s_>                       srs_port_report_r17;
  copy_ptr<mtrp_pusch_csi_rs_r17_s_>                     mtrp_pusch_csi_rs_r17;
  mtrp_pusch_cyclic_map_r17_e_                           mtrp_pusch_cyclic_map_r17;
  uint8_t                                                mtrp_pucch_max_num_pc_fr1_r17 = 3;
  copy_ptr<mtrp_inter_cell_r17_s_>                       mtrp_inter_cell_r17;
  copy_ptr<mtrp_group_based_l1_rsrp_r17_s_>              mtrp_group_based_l1_rsrp_r17;
  mtrp_bfd_rs_mac_ce_r17_e_                              mtrp_bfd_rs_mac_ce_r17;
  copy_ptr<mtrp_csi_enhancement_per_band_r17_s_>         mtrp_csi_enhancement_per_band_r17;
  copy_ptr<codebook_combo_param_multi_trp_r17_s>         codebook_combo_param_multi_trp_r17;
  mtrp_csi_add_csi_r17_e_                                mtrp_csi_add_csi_r17;
  copy_ptr<beam_switch_timing_v1710_s_>                  beam_switch_timing_v1710;
  copy_ptr<beam_switch_timing_r17_s_>                    beam_switch_timing_r17;
  copy_ptr<beam_report_timing_v1710_s_>                  beam_report_timing_v1710;
  copy_ptr<max_num_rx_tx_beam_switch_dl_v1710_s_>        max_num_rx_tx_beam_switch_dl_v1710;
  // group 7
  bool                     srs_port_report_sp_ap_r17_present            = false;
  bool                     max_num_rx_beam_v1720_present                = false;
  bool                     sfn_implicit_rs_two_tci_r17_present          = false;
  bool                     sfn_qcl_type_d_collision_two_tci_r17_present = false;
  bool                     mtrp_csi_num_cp_u_r17_present                = false;
  uint8_t                  max_num_rx_beam_v1720                        = 9;
  mtrp_csi_num_cp_u_r17_e_ mtrp_csi_num_cp_u_r17;
  // group 8
  bool                                      support_rep_num_pdsch_tdra_dci_1_2_r17_present = false;
  support_rep_num_pdsch_tdra_dci_1_2_r17_e_ support_rep_num_pdsch_tdra_dci_1_2_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OLPC-SRS-Pos-r16 ::= SEQUENCE
struct olpc_srs_pos_r16_s {
  struct max_num_path_loss_estimate_per_serving_r16_opts {
    enum options { n1, n4, n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_path_loss_estimate_per_serving_r16_e_ = enumerated<max_num_path_loss_estimate_per_serving_r16_opts>;

  // member variables
  bool                                          olpc_srs_pos_based_on_prs_serving_r16_present      = false;
  bool                                          olpc_srs_pos_based_on_ssb_neigh_r16_present        = false;
  bool                                          olpc_srs_pos_based_on_prs_neigh_r16_present        = false;
  bool                                          max_num_path_loss_estimate_per_serving_r16_present = false;
  max_num_path_loss_estimate_per_serving_r16_e_ max_num_path_loss_estimate_per_serving_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRS-ProcessingCapabilityOutsideMGinPPWperType-r17 ::= SEQUENCE
struct prs_processing_cap_outside_m_gin_ppw_per_type_r17_s {
  struct prs_processing_type_r17_opts {
    enum options { type1_a, type1_b, type2, nulltype } value;

    const char* to_string() const;
  };
  using prs_processing_type_r17_e_ = enumerated<prs_processing_type_r17_opts>;
  struct ppw_dl_prs_buffer_type_r17_opts {
    enum options { type1, type2, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ppw_dl_prs_buffer_type_r17_e_ = enumerated<ppw_dl_prs_buffer_type_r17_opts, true>;
  struct ppw_dur_of_prs_processing_r17_c_ {
    struct ppw_dur_of_prs_processing1_r17_s_ {
      struct ppw_dur_of_prs_processing_symbols_n_r17_opts {
        enum options {
          ms_dot125,
          ms_dot25,
          ms_dot5,
          ms1,
          ms2,
          ms4,
          ms6,
          ms8,
          ms12,
          ms16,
          ms20,
          ms25,
          ms30,
          ms32,
          ms35,
          ms40,
          ms45,
          ms50,
          nulltype
        } value;

        const char* to_string() const;
      };
      using ppw_dur_of_prs_processing_symbols_n_r17_e_ = enumerated<ppw_dur_of_prs_processing_symbols_n_r17_opts>;
      struct ppw_dur_of_prs_processing_symbols_t_r17_opts {
        enum options { ms1, ms2, ms4, ms8, ms16, ms20, ms30, ms40, ms80, ms160, ms320, ms640, ms1280, nulltype } value;
        typedef uint16_t number_type;

        const char* to_string() const;
        uint16_t    to_number() const;
      };
      using ppw_dur_of_prs_processing_symbols_t_r17_e_ = enumerated<ppw_dur_of_prs_processing_symbols_t_r17_opts>;

      // member variables
      ppw_dur_of_prs_processing_symbols_n_r17_e_ ppw_dur_of_prs_processing_symbols_n_r17;
      ppw_dur_of_prs_processing_symbols_t_r17_e_ ppw_dur_of_prs_processing_symbols_t_r17;
    };
    struct ppw_dur_of_prs_processing2_r17_s_ {
      struct ppw_dur_of_prs_processing_symbols_n2_r17_opts {
        enum options { ms_dot125, ms_dot25, ms_dot5, ms1, ms2, ms3, ms4, ms5, ms6, ms8, ms12, nulltype } value;

        const char* to_string() const;
      };
      using ppw_dur_of_prs_processing_symbols_n2_r17_e_ = enumerated<ppw_dur_of_prs_processing_symbols_n2_r17_opts>;
      struct ppw_dur_of_prs_processing_symbols_t2_r17_opts {
        enum options { ms4, ms5, ms6, ms8, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using ppw_dur_of_prs_processing_symbols_t2_r17_e_ = enumerated<ppw_dur_of_prs_processing_symbols_t2_r17_opts>;

      // member variables
      ppw_dur_of_prs_processing_symbols_n2_r17_e_ ppw_dur_of_prs_processing_symbols_n2_r17;
      ppw_dur_of_prs_processing_symbols_t2_r17_e_ ppw_dur_of_prs_processing_symbols_t2_r17;
    };
    struct types_opts {
      enum options { ppw_dur_of_prs_processing1_r17, ppw_dur_of_prs_processing2_r17, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ppw_dur_of_prs_processing_r17_c_() = default;
    ppw_dur_of_prs_processing_r17_c_(const ppw_dur_of_prs_processing_r17_c_& other);
    ppw_dur_of_prs_processing_r17_c_& operator=(const ppw_dur_of_prs_processing_r17_c_& other);
    ~ppw_dur_of_prs_processing_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ppw_dur_of_prs_processing1_r17_s_& ppw_dur_of_prs_processing1_r17()
    {
      assert_choice_type(types::ppw_dur_of_prs_processing1_r17, type_, "ppw-durationOfPRS-Processing-r17");
      return c.get<ppw_dur_of_prs_processing1_r17_s_>();
    }
    ppw_dur_of_prs_processing2_r17_s_& ppw_dur_of_prs_processing2_r17()
    {
      assert_choice_type(types::ppw_dur_of_prs_processing2_r17, type_, "ppw-durationOfPRS-Processing-r17");
      return c.get<ppw_dur_of_prs_processing2_r17_s_>();
    }
    const ppw_dur_of_prs_processing1_r17_s_& ppw_dur_of_prs_processing1_r17() const
    {
      assert_choice_type(types::ppw_dur_of_prs_processing1_r17, type_, "ppw-durationOfPRS-Processing-r17");
      return c.get<ppw_dur_of_prs_processing1_r17_s_>();
    }
    const ppw_dur_of_prs_processing2_r17_s_& ppw_dur_of_prs_processing2_r17() const
    {
      assert_choice_type(types::ppw_dur_of_prs_processing2_r17, type_, "ppw-durationOfPRS-Processing-r17");
      return c.get<ppw_dur_of_prs_processing2_r17_s_>();
    }
    ppw_dur_of_prs_processing1_r17_s_& set_ppw_dur_of_prs_processing1_r17();
    ppw_dur_of_prs_processing2_r17_s_& set_ppw_dur_of_prs_processing2_r17();

  private:
    types                                                                                 type_;
    choice_buffer_t<ppw_dur_of_prs_processing1_r17_s_, ppw_dur_of_prs_processing2_r17_s_> c;

    void destroy_();
  };
  struct ppw_max_num_of_dl_prs_res_processed_per_slot_r17_s_ {
    struct scs15_r17_opts {
      enum options { n1, n2, n4, n6, n8, n12, n16, n24, n32, n48, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs15_r17_e_ = enumerated<scs15_r17_opts>;
    struct scs30_r17_opts {
      enum options { n1, n2, n4, n6, n8, n12, n16, n24, n32, n48, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs30_r17_e_ = enumerated<scs30_r17_opts>;
    struct scs60_r17_opts {
      enum options { n1, n2, n4, n6, n8, n12, n16, n24, n32, n48, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs60_r17_e_ = enumerated<scs60_r17_opts>;
    struct scs120_r17_opts {
      enum options { n1, n2, n4, n6, n8, n12, n16, n24, n32, n48, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs120_r17_e_ = enumerated<scs120_r17_opts>;

    // member variables
    bool          ext                = false;
    bool          scs15_r17_present  = false;
    bool          scs30_r17_present  = false;
    bool          scs60_r17_present  = false;
    bool          scs120_r17_present = false;
    scs15_r17_e_  scs15_r17;
    scs30_r17_e_  scs30_r17;
    scs60_r17_e_  scs60_r17;
    scs120_r17_e_ scs120_r17;
    // ...
  };
  struct ppw_max_num_of_dl_bw_r17_c_ {
    struct fr1_r17_opts {
      enum options { mhz5, mhz10, mhz20, mhz40, mhz50, mhz80, mhz100, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using fr1_r17_e_ = enumerated<fr1_r17_opts>;
    struct fr2_r17_opts {
      enum options { mhz50, mhz100, mhz200, mhz400, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using fr2_r17_e_ = enumerated<fr2_r17_opts>;
    struct types_opts {
      enum options { fr1_r17, fr2_r17, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ppw_max_num_of_dl_bw_r17_c_() = default;
    ppw_max_num_of_dl_bw_r17_c_(const ppw_max_num_of_dl_bw_r17_c_& other);
    ppw_max_num_of_dl_bw_r17_c_& operator=(const ppw_max_num_of_dl_bw_r17_c_& other);
    ~ppw_max_num_of_dl_bw_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fr1_r17_e_& fr1_r17()
    {
      assert_choice_type(types::fr1_r17, type_, "ppw-maxNumOfDL-Bandwidth-r17");
      return c.get<fr1_r17_e_>();
    }
    fr2_r17_e_& fr2_r17()
    {
      assert_choice_type(types::fr2_r17, type_, "ppw-maxNumOfDL-Bandwidth-r17");
      return c.get<fr2_r17_e_>();
    }
    const fr1_r17_e_& fr1_r17() const
    {
      assert_choice_type(types::fr1_r17, type_, "ppw-maxNumOfDL-Bandwidth-r17");
      return c.get<fr1_r17_e_>();
    }
    const fr2_r17_e_& fr2_r17() const
    {
      assert_choice_type(types::fr2_r17, type_, "ppw-maxNumOfDL-Bandwidth-r17");
      return c.get<fr2_r17_e_>();
    }
    fr1_r17_e_& set_fr1_r17();
    fr2_r17_e_& set_fr2_r17();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                                                ppw_dur_of_prs_processing_r17_present = false;
  bool                                                ppw_max_num_of_dl_bw_r17_present      = false;
  prs_processing_type_r17_e_                          prs_processing_type_r17;
  ppw_dl_prs_buffer_type_r17_e_                       ppw_dl_prs_buffer_type_r17;
  ppw_dur_of_prs_processing_r17_c_                    ppw_dur_of_prs_processing_r17;
  ppw_max_num_of_dl_prs_res_processed_per_slot_r17_s_ ppw_max_num_of_dl_prs_res_processed_per_slot_r17;
  ppw_max_num_of_dl_bw_r17_c_                         ppw_max_num_of_dl_bw_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PosSRS-RRC-Inactive-OutsideInitialUL-BWP-r17 ::= SEQUENCE
struct pos_srs_rrc_inactive_outside_init_ul_bwp_r17_s {
  struct max_srs_pos_bw_for_each_scs_within_cc_fr1_r17_opts {
    enum options {
      bw5,
      bw10,
      bw15,
      bw20,
      bw25,
      bw30,
      bw35,
      bw40,
      bw45,
      bw50,
      bw60,
      bw70,
      bw80,
      bw90,
      bw100,
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_srs_pos_bw_for_each_scs_within_cc_fr1_r17_e_ =
      enumerated<max_srs_pos_bw_for_each_scs_within_cc_fr1_r17_opts>;
  struct max_srs_pos_bw_for_each_scs_within_cc_fr2_r17_opts {
    enum options { bw50, bw100, bw200, bw400, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using max_srs_pos_bw_for_each_scs_within_cc_fr2_r17_e_ =
      enumerated<max_srs_pos_bw_for_each_scs_within_cc_fr2_r17_opts>;
  struct max_num_of_srs_pos_res_sets_r17_opts {
    enum options { n1, n2, n4, n8, n12, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_of_srs_pos_res_sets_r17_e_ = enumerated<max_num_of_srs_pos_res_sets_r17_opts>;
  struct max_num_of_periodic_srs_pos_res_r17_opts {
    enum options { n1, n2, n4, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_of_periodic_srs_pos_res_r17_e_ = enumerated<max_num_of_periodic_srs_pos_res_r17_opts>;
  struct max_num_of_periodic_srs_pos_res_per_slot_r17_opts {
    enum options { n1, n2, n3, n4, n5, n6, n8, n10, n12, n14, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_of_periodic_srs_pos_res_per_slot_r17_e_ = enumerated<max_num_of_periodic_srs_pos_res_per_slot_r17_opts>;
  struct max_num_of_periodic_and_semipersistent_srs_pos_res_r17_opts {
    enum options { n1, n2, n4, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_of_periodic_and_semipersistent_srs_pos_res_r17_e_ =
      enumerated<max_num_of_periodic_and_semipersistent_srs_pos_res_r17_opts>;
  struct max_num_of_periodic_and_semipersistent_srs_pos_res_per_slot_r17_opts {
    enum options { n1, n2, n3, n4, n5, n6, n8, n10, n12, n14, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_of_periodic_and_semipersistent_srs_pos_res_per_slot_r17_e_ =
      enumerated<max_num_of_periodic_and_semipersistent_srs_pos_res_per_slot_r17_opts>;
  struct switching_time_srs_tx_other_tx_r17_opts {
    enum options { us100, us140, us200, us300, us500, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using switching_time_srs_tx_other_tx_r17_e_ = enumerated<switching_time_srs_tx_other_tx_r17_opts>;
  struct max_num_of_semi_persistent_srs_pos_res_r17_opts {
    enum options { n1, n2, n4, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_of_semi_persistent_srs_pos_res_r17_e_ = enumerated<max_num_of_semi_persistent_srs_pos_res_r17_opts>;
  struct max_num_of_semi_persistent_srs_pos_res_per_slot_r17_opts {
    enum options { n1, n2, n3, n4, n5, n6, n8, n10, n12, n14, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_of_semi_persistent_srs_pos_res_per_slot_r17_e_ =
      enumerated<max_num_of_semi_persistent_srs_pos_res_per_slot_r17_opts>;

  // member variables
  bool ext                                                                     = false;
  bool max_srs_pos_bw_for_each_scs_within_cc_fr1_r17_present                   = false;
  bool max_srs_pos_bw_for_each_scs_within_cc_fr2_r17_present                   = false;
  bool max_num_of_srs_pos_res_sets_r17_present                                 = false;
  bool max_num_of_periodic_srs_pos_res_r17_present                             = false;
  bool max_num_of_periodic_srs_pos_res_per_slot_r17_present                    = false;
  bool different_numerology_between_srs_pos_and_init_bwp_r17_present           = false;
  bool srs_pos_without_restrict_on_bwp_r17_present                             = false;
  bool max_num_of_periodic_and_semipersistent_srs_pos_res_r17_present          = false;
  bool max_num_of_periodic_and_semipersistent_srs_pos_res_per_slot_r17_present = false;
  bool different_center_freq_between_srs_pos_and_init_bwp_r17_present          = false;
  bool switching_time_srs_tx_other_tx_r17_present                              = false;
  bool max_num_of_semi_persistent_srs_pos_res_r17_present                      = false;
  bool max_num_of_semi_persistent_srs_pos_res_per_slot_r17_present             = false;
  max_srs_pos_bw_for_each_scs_within_cc_fr1_r17_e_          max_srs_pos_bw_for_each_scs_within_cc_fr1_r17;
  max_srs_pos_bw_for_each_scs_within_cc_fr2_r17_e_          max_srs_pos_bw_for_each_scs_within_cc_fr2_r17;
  max_num_of_srs_pos_res_sets_r17_e_                        max_num_of_srs_pos_res_sets_r17;
  max_num_of_periodic_srs_pos_res_r17_e_                    max_num_of_periodic_srs_pos_res_r17;
  max_num_of_periodic_srs_pos_res_per_slot_r17_e_           max_num_of_periodic_srs_pos_res_per_slot_r17;
  max_num_of_periodic_and_semipersistent_srs_pos_res_r17_e_ max_num_of_periodic_and_semipersistent_srs_pos_res_r17;
  max_num_of_periodic_and_semipersistent_srs_pos_res_per_slot_r17_e_
                                                max_num_of_periodic_and_semipersistent_srs_pos_res_per_slot_r17;
  switching_time_srs_tx_other_tx_r17_e_         switching_time_srs_tx_other_tx_r17;
  max_num_of_semi_persistent_srs_pos_res_r17_e_ max_num_of_semi_persistent_srs_pos_res_r17;
  max_num_of_semi_persistent_srs_pos_res_per_slot_r17_e_ max_num_of_semi_persistent_srs_pos_res_per_slot_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-AllPosResourcesRRC-Inactive-r17 ::= SEQUENCE
struct srs_all_pos_res_rrc_inactive_r17_s {
  struct srs_pos_res_rrc_inactive_r17_s_ {
    struct max_num_srs_pos_res_set_per_bwp_r17_opts {
      enum options { n1, n2, n4, n8, n12, n16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_srs_pos_res_set_per_bwp_r17_e_ = enumerated<max_num_srs_pos_res_set_per_bwp_r17_opts>;
    struct max_num_srs_pos_res_per_bwp_r17_opts {
      enum options { n1, n2, n4, n8, n16, n32, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_srs_pos_res_per_bwp_r17_e_ = enumerated<max_num_srs_pos_res_per_bwp_r17_opts>;
    struct max_num_srs_res_per_bwp_per_slot_r17_opts {
      enum options { n1, n2, n3, n4, n5, n6, n8, n10, n12, n14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_srs_res_per_bwp_per_slot_r17_e_ = enumerated<max_num_srs_res_per_bwp_per_slot_r17_opts>;
    struct max_num_periodic_srs_pos_res_per_bwp_r17_opts {
      enum options { n1, n2, n4, n8, n16, n32, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_periodic_srs_pos_res_per_bwp_r17_e_ = enumerated<max_num_periodic_srs_pos_res_per_bwp_r17_opts>;
    struct max_num_periodic_srs_pos_res_per_bwp_per_slot_r17_opts {
      enum options { n1, n2, n3, n4, n5, n6, n8, n10, n12, n14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_periodic_srs_pos_res_per_bwp_per_slot_r17_e_ =
        enumerated<max_num_periodic_srs_pos_res_per_bwp_per_slot_r17_opts>;
    struct dummy1_opts {
      enum options { n1, n2, n4, n8, n16, n32, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using dummy1_e_ = enumerated<dummy1_opts>;
    struct dummy2_opts {
      enum options { n1, n2, n3, n4, n5, n6, n8, n10, n12, n14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using dummy2_e_ = enumerated<dummy2_opts>;

    // member variables
    max_num_srs_pos_res_set_per_bwp_r17_e_               max_num_srs_pos_res_set_per_bwp_r17;
    max_num_srs_pos_res_per_bwp_r17_e_                   max_num_srs_pos_res_per_bwp_r17;
    max_num_srs_res_per_bwp_per_slot_r17_e_              max_num_srs_res_per_bwp_per_slot_r17;
    max_num_periodic_srs_pos_res_per_bwp_r17_e_          max_num_periodic_srs_pos_res_per_bwp_r17;
    max_num_periodic_srs_pos_res_per_bwp_per_slot_r17_e_ max_num_periodic_srs_pos_res_per_bwp_per_slot_r17;
    dummy1_e_                                            dummy1;
    dummy2_e_                                            dummy2;
  };

  // member variables
  srs_pos_res_rrc_inactive_r17_s_ srs_pos_res_rrc_inactive_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SharedSpectrumChAccessParamsPerBand-r16 ::= SEQUENCE
struct shared_spec_ch_access_params_per_band_r16_s {
  bool    ul_dyn_ch_access_r16_present                    = false;
  bool    ul_semi_static_ch_access_r16_present            = false;
  bool    ssb_rrm_dyn_ch_access_r16_present               = false;
  bool    ssb_rrm_semi_static_ch_access_r16_present       = false;
  bool    mib_acquisition_r16_present                     = false;
  bool    ssb_rlm_dyn_ch_access_r16_present               = false;
  bool    ssb_rlm_semi_static_ch_access_r16_present       = false;
  bool    sib1_acquisition_r16_present                    = false;
  bool    ext_ra_resp_win_r16_present                     = false;
  bool    ssb_bfd_cbd_dyn_ch_access_r16_present           = false;
  bool    ssb_bfd_cbd_semi_static_ch_access_r16_present   = false;
  bool    csi_rs_bfd_cbd_r16_present                      = false;
  bool    ul_ch_bw_scell_10mhz_r16_present                = false;
  bool    rssi_ch_occupancy_report_r16_present            = false;
  bool    srs_start_any_ofdm_symbol_r16_present           = false;
  bool    search_space_freq_monitor_location_r16_present  = false;
  bool    coreset_rb_offset_r16_present                   = false;
  bool    cgi_acquisition_r16_present                     = false;
  bool    cfg_ul_tx_r16_present                           = false;
  bool    prach_wideband_r16_present                      = false;
  bool    dci_available_rb_set_r16_present                = false;
  bool    dci_ch_occupancy_dur_r16_present                = false;
  bool    type_b_pdsch_len_r16_present                    = false;
  bool    search_space_switch_with_dci_r16_present        = false;
  bool    search_space_switch_without_dci_r16_present     = false;
  bool    search_space_switch_cap2_r16_present            = false;
  bool    non_numerical_pdsch_harq_timing_r16_present     = false;
  bool    enhanced_dyn_harq_codebook_r16_present          = false;
  bool    one_shot_harq_feedback_r16_present              = false;
  bool    multi_pusch_ul_grant_r16_present                = false;
  bool    csi_rs_rlm_r16_present                          = false;
  bool    dummy_present                                   = false;
  bool    periodic_and_semi_persistent_csi_rs_r16_present = false;
  bool    pusch_prb_interlace_r16_present                 = false;
  bool    pucch_f0_f1_prb_interlace_r16_present           = false;
  bool    occ_prb_pf2_pf3_r16_present                     = false;
  bool    ext_cp_range_cg_pusch_r16_present               = false;
  bool    cfg_grant_with_re_tx_r16_present                = false;
  bool    ed_thres_r16_present                            = false;
  bool    ul_dl_cot_sharing_r16_present                   = false;
  bool    mux_cg_uci_harq_ack_r16_present                 = false;
  bool    cg_res_cfg_r16_present                          = false;
  uint8_t search_space_freq_monitor_location_r16          = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SharedSpectrumChAccessParamsPerBand-v1630 ::= SEQUENCE
struct shared_spec_ch_access_params_per_band_v1630_s {
  bool dl_reception_intra_cell_guardband_r16_present = false;
  bool dl_reception_lbt_subset_rb_r16_present        = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SharedSpectrumChAccessParamsPerBand-v1640 ::= SEQUENCE
struct shared_spec_ch_access_params_per_band_v1640_s {
  bool csi_rsrp_and_rsrq_meas_with_ssb_r16_present    = false;
  bool csi_rsrp_and_rsrq_meas_without_ssb_r16_present = false;
  bool csi_sinr_meas_r16_present                      = false;
  bool ssb_and_csi_rs_rlm_r16_present                 = false;
  bool csi_rs_cfra_for_ho_r16_present                 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SharedSpectrumChAccessParamsPerBand-v1650 ::= SEQUENCE
struct shared_spec_ch_access_params_per_band_v1650_s {
  bool extended_search_space_switch_with_dci_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SharedSpectrumChAccessParamsPerBand-v1710 ::= SEQUENCE
struct shared_spec_ch_access_params_per_band_v1710_s {
  bool ul_semi_static_ch_access_dependent_cfg_r17_present   = false;
  bool ul_semi_static_ch_access_independent_cfg_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SpatialRelationsSRS-Pos-r16 ::= SEQUENCE
struct spatial_relations_srs_pos_r16_s {
  bool spatial_relation_srs_pos_based_on_ssb_serving_r16_present    = false;
  bool spatial_relation_srs_pos_based_on_csi_rs_serving_r16_present = false;
  bool spatial_relation_srs_pos_based_on_prs_serving_r16_present    = false;
  bool spatial_relation_srs_pos_based_on_srs_r16_present            = false;
  bool spatial_relation_srs_pos_based_on_ssb_neigh_r16_present      = false;
  bool spatial_relation_srs_pos_based_on_prs_neigh_r16_present      = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandNR ::= SEQUENCE
struct band_nr_s {
  struct bwp_same_numerology_opts {
    enum options { upto2, upto4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using bwp_same_numerology_e_ = enumerated<bwp_same_numerology_opts>;
  struct ue_pwr_class_opts {
    enum options { pc1, pc2, pc3, pc4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ue_pwr_class_e_ = enumerated<ue_pwr_class_opts>;
  struct ch_bws_dl_c_ {
    struct fr1_s_ {
      bool                scs_15k_hz_present = false;
      bool                scs_30k_hz_present = false;
      bool                scs_60k_hz_present = false;
      fixed_bitstring<10> scs_15k_hz;
      fixed_bitstring<10> scs_30k_hz;
      fixed_bitstring<10> scs_60k_hz;
    };
    struct fr2_s_ {
      bool               scs_60k_hz_present  = false;
      bool               scs_120k_hz_present = false;
      fixed_bitstring<3> scs_60k_hz;
      fixed_bitstring<3> scs_120k_hz;
    };
    struct types_opts {
      enum options { fr1, fr2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ch_bws_dl_c_() = default;
    ch_bws_dl_c_(const ch_bws_dl_c_& other);
    ch_bws_dl_c_& operator=(const ch_bws_dl_c_& other);
    ~ch_bws_dl_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fr1_s_& fr1()
    {
      assert_choice_type(types::fr1, type_, "channelBWs-DL");
      return c.get<fr1_s_>();
    }
    fr2_s_& fr2()
    {
      assert_choice_type(types::fr2, type_, "channelBWs-DL");
      return c.get<fr2_s_>();
    }
    const fr1_s_& fr1() const
    {
      assert_choice_type(types::fr1, type_, "channelBWs-DL");
      return c.get<fr1_s_>();
    }
    const fr2_s_& fr2() const
    {
      assert_choice_type(types::fr2, type_, "channelBWs-DL");
      return c.get<fr2_s_>();
    }
    fr1_s_& set_fr1();
    fr2_s_& set_fr2();

  private:
    types                           type_;
    choice_buffer_t<fr1_s_, fr2_s_> c;

    void destroy_();
  };
  struct ch_bws_ul_c_ {
    struct fr1_s_ {
      bool                scs_15k_hz_present = false;
      bool                scs_30k_hz_present = false;
      bool                scs_60k_hz_present = false;
      fixed_bitstring<10> scs_15k_hz;
      fixed_bitstring<10> scs_30k_hz;
      fixed_bitstring<10> scs_60k_hz;
    };
    struct fr2_s_ {
      bool               scs_60k_hz_present  = false;
      bool               scs_120k_hz_present = false;
      fixed_bitstring<3> scs_60k_hz;
      fixed_bitstring<3> scs_120k_hz;
    };
    struct types_opts {
      enum options { fr1, fr2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ch_bws_ul_c_() = default;
    ch_bws_ul_c_(const ch_bws_ul_c_& other);
    ch_bws_ul_c_& operator=(const ch_bws_ul_c_& other);
    ~ch_bws_ul_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fr1_s_& fr1()
    {
      assert_choice_type(types::fr1, type_, "channelBWs-UL");
      return c.get<fr1_s_>();
    }
    fr2_s_& fr2()
    {
      assert_choice_type(types::fr2, type_, "channelBWs-UL");
      return c.get<fr2_s_>();
    }
    const fr1_s_& fr1() const
    {
      assert_choice_type(types::fr1, type_, "channelBWs-UL");
      return c.get<fr1_s_>();
    }
    const fr2_s_& fr2() const
    {
      assert_choice_type(types::fr2, type_, "channelBWs-UL");
      return c.get<fr2_s_>();
    }
    fr1_s_& set_fr1();
    fr2_s_& set_fr2();

  private:
    types                           type_;
    choice_buffer_t<fr1_s_, fr2_s_> c;

    void destroy_();
  };
  struct max_ul_duty_cycle_pc2_fr1_opts {
    enum options { n60, n70, n80, n90, n100, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_ul_duty_cycle_pc2_fr1_e_ = enumerated<max_ul_duty_cycle_pc2_fr1_opts>;
  struct max_ul_duty_cycle_fr2_opts {
    enum options { n15, n20, n25, n30, n40, n50, n60, n70, n80, n90, n100, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_ul_duty_cycle_fr2_e_ = enumerated<max_ul_duty_cycle_fr2_opts>;
  struct ch_bws_dl_v1590_c_ {
    struct fr1_s_ {
      bool                scs_15k_hz_present = false;
      bool                scs_30k_hz_present = false;
      bool                scs_60k_hz_present = false;
      fixed_bitstring<16> scs_15k_hz;
      fixed_bitstring<16> scs_30k_hz;
      fixed_bitstring<16> scs_60k_hz;
    };
    struct fr2_s_ {
      bool               scs_60k_hz_present  = false;
      bool               scs_120k_hz_present = false;
      fixed_bitstring<8> scs_60k_hz;
      fixed_bitstring<8> scs_120k_hz;
    };
    struct types_opts {
      enum options { fr1, fr2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ch_bws_dl_v1590_c_() = default;
    ch_bws_dl_v1590_c_(const ch_bws_dl_v1590_c_& other);
    ch_bws_dl_v1590_c_& operator=(const ch_bws_dl_v1590_c_& other);
    ~ch_bws_dl_v1590_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fr1_s_& fr1()
    {
      assert_choice_type(types::fr1, type_, "channelBWs-DL-v1590");
      return c.get<fr1_s_>();
    }
    fr2_s_& fr2()
    {
      assert_choice_type(types::fr2, type_, "channelBWs-DL-v1590");
      return c.get<fr2_s_>();
    }
    const fr1_s_& fr1() const
    {
      assert_choice_type(types::fr1, type_, "channelBWs-DL-v1590");
      return c.get<fr1_s_>();
    }
    const fr2_s_& fr2() const
    {
      assert_choice_type(types::fr2, type_, "channelBWs-DL-v1590");
      return c.get<fr2_s_>();
    }
    fr1_s_& set_fr1();
    fr2_s_& set_fr2();

  private:
    types                           type_;
    choice_buffer_t<fr1_s_, fr2_s_> c;

    void destroy_();
  };
  struct ch_bws_ul_v1590_c_ {
    struct fr1_s_ {
      bool                scs_15k_hz_present = false;
      bool                scs_30k_hz_present = false;
      bool                scs_60k_hz_present = false;
      fixed_bitstring<16> scs_15k_hz;
      fixed_bitstring<16> scs_30k_hz;
      fixed_bitstring<16> scs_60k_hz;
    };
    struct fr2_s_ {
      bool               scs_60k_hz_present  = false;
      bool               scs_120k_hz_present = false;
      fixed_bitstring<8> scs_60k_hz;
      fixed_bitstring<8> scs_120k_hz;
    };
    struct types_opts {
      enum options { fr1, fr2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ch_bws_ul_v1590_c_() = default;
    ch_bws_ul_v1590_c_(const ch_bws_ul_v1590_c_& other);
    ch_bws_ul_v1590_c_& operator=(const ch_bws_ul_v1590_c_& other);
    ~ch_bws_ul_v1590_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fr1_s_& fr1()
    {
      assert_choice_type(types::fr1, type_, "channelBWs-UL-v1590");
      return c.get<fr1_s_>();
    }
    fr2_s_& fr2()
    {
      assert_choice_type(types::fr2, type_, "channelBWs-UL-v1590");
      return c.get<fr2_s_>();
    }
    const fr1_s_& fr1() const
    {
      assert_choice_type(types::fr1, type_, "channelBWs-UL-v1590");
      return c.get<fr1_s_>();
    }
    const fr2_s_& fr2() const
    {
      assert_choice_type(types::fr2, type_, "channelBWs-UL-v1590");
      return c.get<fr2_s_>();
    }
    fr1_s_& set_fr1();
    fr2_s_& set_fr2();

  private:
    types                           type_;
    choice_buffer_t<fr1_s_, fr2_s_> c;

    void destroy_();
  };
  struct multiple_rate_matching_eutra_crs_r16_s_ {
    uint8_t max_num_patterns_r16             = 2;
    uint8_t max_num_non_overlap_patterns_r16 = 1;
  };
  struct ch_bw_dl_iab_r16_c_ {
    struct fr1_100mhz_s_ {
      bool scs_15k_hz_present = false;
      bool scs_30k_hz_present = false;
      bool scs_60k_hz_present = false;
    };
    struct fr2_200mhz_s_ {
      bool scs_60k_hz_present  = false;
      bool scs_120k_hz_present = false;
    };
    struct types_opts {
      enum options { fr1_100mhz, fr2_200mhz, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ch_bw_dl_iab_r16_c_() = default;
    ch_bw_dl_iab_r16_c_(const ch_bw_dl_iab_r16_c_& other);
    ch_bw_dl_iab_r16_c_& operator=(const ch_bw_dl_iab_r16_c_& other);
    ~ch_bw_dl_iab_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fr1_100mhz_s_& fr1_100mhz()
    {
      assert_choice_type(types::fr1_100mhz, type_, "channelBW-DL-IAB-r16");
      return c.get<fr1_100mhz_s_>();
    }
    fr2_200mhz_s_& fr2_200mhz()
    {
      assert_choice_type(types::fr2_200mhz, type_, "channelBW-DL-IAB-r16");
      return c.get<fr2_200mhz_s_>();
    }
    const fr1_100mhz_s_& fr1_100mhz() const
    {
      assert_choice_type(types::fr1_100mhz, type_, "channelBW-DL-IAB-r16");
      return c.get<fr1_100mhz_s_>();
    }
    const fr2_200mhz_s_& fr2_200mhz() const
    {
      assert_choice_type(types::fr2_200mhz, type_, "channelBW-DL-IAB-r16");
      return c.get<fr2_200mhz_s_>();
    }
    fr1_100mhz_s_& set_fr1_100mhz();
    fr2_200mhz_s_& set_fr2_200mhz();

  private:
    types                                         type_;
    choice_buffer_t<fr1_100mhz_s_, fr2_200mhz_s_> c;

    void destroy_();
  };
  struct ch_bw_ul_iab_r16_c_ {
    struct fr1_100mhz_s_ {
      bool scs_15k_hz_present = false;
      bool scs_30k_hz_present = false;
      bool scs_60k_hz_present = false;
    };
    struct fr2_200mhz_s_ {
      bool scs_60k_hz_present  = false;
      bool scs_120k_hz_present = false;
    };
    struct types_opts {
      enum options { fr1_100mhz, fr2_200mhz, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ch_bw_ul_iab_r16_c_() = default;
    ch_bw_ul_iab_r16_c_(const ch_bw_ul_iab_r16_c_& other);
    ch_bw_ul_iab_r16_c_& operator=(const ch_bw_ul_iab_r16_c_& other);
    ~ch_bw_ul_iab_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fr1_100mhz_s_& fr1_100mhz()
    {
      assert_choice_type(types::fr1_100mhz, type_, "channelBW-UL-IAB-r16");
      return c.get<fr1_100mhz_s_>();
    }
    fr2_200mhz_s_& fr2_200mhz()
    {
      assert_choice_type(types::fr2_200mhz, type_, "channelBW-UL-IAB-r16");
      return c.get<fr2_200mhz_s_>();
    }
    const fr1_100mhz_s_& fr1_100mhz() const
    {
      assert_choice_type(types::fr1_100mhz, type_, "channelBW-UL-IAB-r16");
      return c.get<fr1_100mhz_s_>();
    }
    const fr2_200mhz_s_& fr2_200mhz() const
    {
      assert_choice_type(types::fr2_200mhz, type_, "channelBW-UL-IAB-r16");
      return c.get<fr2_200mhz_s_>();
    }
    fr1_100mhz_s_& set_fr1_100mhz();
    fr2_200mhz_s_& set_fr2_200mhz();

  private:
    types                                         type_;
    choice_buffer_t<fr1_100mhz_s_, fr2_200mhz_s_> c;

    void destroy_();
  };
  struct active_cfg_grant_r16_s_ {
    struct max_num_cfgs_per_bwp_r16_opts {
      enum options { n1, n2, n4, n8, n12, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_cfgs_per_bwp_r16_e_ = enumerated<max_num_cfgs_per_bwp_r16_opts>;

    // member variables
    max_num_cfgs_per_bwp_r16_e_ max_num_cfgs_per_bwp_r16;
    uint8_t                     max_num_cfgs_all_cc_r16 = 2;
  };
  struct sps_r16_s_ {
    uint8_t max_num_cfgs_per_bwp_r16 = 1;
    uint8_t max_num_cfgs_all_cc_r16  = 2;
  };
  struct trs_add_bw_r16_opts {
    enum options { trs_add_bw_set1, trs_add_bw_set2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using trs_add_bw_r16_e_ = enumerated<trs_add_bw_r16_opts>;
  struct enhanced_ul_transient_period_r16_opts {
    enum options { us2, us4, us7, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using enhanced_ul_transient_period_r16_e_ = enumerated<enhanced_ul_transient_period_r16_opts>;
  struct max_ul_duty_cycle_pc1dot5_mpe_fr1_r16_opts {
    enum options { n10, n15, n20, n25, n30, n40, n50, n60, n70, n80, n90, n100, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_ul_duty_cycle_pc1dot5_mpe_fr1_r16_e_ = enumerated<max_ul_duty_cycle_pc1dot5_mpe_fr1_r16_opts>;
  struct ue_pwr_class_v1700_opts {
    enum options { pc5, pc6, pc7, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ue_pwr_class_v1700_e_ = enumerated<ue_pwr_class_v1700_opts>;
  struct max_harq_process_num_r17_opts {
    enum options { u16d32, u32d16, u32d32, nulltype } value;

    const char* to_string() const;
  };
  using max_harq_process_num_r17_e_ = enumerated<max_harq_process_num_r17_opts>;
  struct nr_ue_tx_teg_id_max_support_r17_opts {
    enum options { n1, n2, n3, n4, n6, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nr_ue_tx_teg_id_max_support_r17_e_ = enumerated<nr_ue_tx_teg_id_max_support_r17_opts>;
  struct prs_processing_win_type1_a_r17_opts {
    enum options { option1, option2, option3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using prs_processing_win_type1_a_r17_e_ = enumerated<prs_processing_win_type1_a_r17_opts>;
  struct prs_processing_win_type1_b_r17_opts {
    enum options { option1, option2, option3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using prs_processing_win_type1_b_r17_e_ = enumerated<prs_processing_win_type1_b_r17_opts>;
  struct prs_processing_win_type2_r17_opts {
    enum options { option1, option2, option3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using prs_processing_win_type2_r17_e_ = enumerated<prs_processing_win_type2_r17_opts>;
  struct max_dur_dmrs_bundling_r17_s_ {
    struct fdd_r17_opts {
      enum options { n4, n8, n16, n32, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using fdd_r17_e_ = enumerated<fdd_r17_opts>;
    struct tdd_r17_opts {
      enum options { n2, n4, n8, n16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using tdd_r17_e_ = enumerated<tdd_r17_opts>;

    // member variables
    bool       fdd_r17_present = false;
    bool       tdd_r17_present = false;
    fdd_r17_e_ fdd_r17;
    tdd_r17_e_ tdd_r17;
  };
  struct max_num_ngso_satellites_within_one_smtc_r17_opts {
    enum options { n1, n2, n3, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_ngso_satellites_within_one_smtc_r17_e_ = enumerated<max_num_ngso_satellites_within_one_smtc_r17_opts>;
  struct aperiodic_csi_rs_fast_scell_activation_r17_s_ {
    struct max_num_aperiodic_csi_rs_per_cc_r17_opts {
      enum options { n8, n16, n32, n48, n64, n128, n255, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using max_num_aperiodic_csi_rs_per_cc_r17_e_ = enumerated<max_num_aperiodic_csi_rs_per_cc_r17_opts>;
    struct max_num_aperiodic_csi_rs_across_ccs_r17_opts {
      enum options { n8, n16, n32, n64, n128, n256, n512, n1024, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using max_num_aperiodic_csi_rs_across_ccs_r17_e_ = enumerated<max_num_aperiodic_csi_rs_across_ccs_r17_opts>;

    // member variables
    max_num_aperiodic_csi_rs_per_cc_r17_e_     max_num_aperiodic_csi_rs_per_cc_r17;
    max_num_aperiodic_csi_rs_across_ccs_r17_e_ max_num_aperiodic_csi_rs_across_ccs_r17;
  };
  struct aperiodic_csi_rs_add_bw_r17_opts {
    enum options { add_bw_set1, add_bw_set2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using aperiodic_csi_rs_add_bw_r17_e_ = enumerated<aperiodic_csi_rs_add_bw_r17_opts>;
  struct enhanced_type3_harq_codebook_feedback_r17_s_ {
    struct enhanced_type3_harq_codebooks_r17_opts {
      enum options { n1, n2, n4, n8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using enhanced_type3_harq_codebooks_r17_e_ = enumerated<enhanced_type3_harq_codebooks_r17_opts>;
    struct max_num_pucch_txs_r17_opts {
      enum options { n1, n2, n3, n4, n5, n6, n7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_pucch_txs_r17_e_ = enumerated<max_num_pucch_txs_r17_opts>;

    // member variables
    enhanced_type3_harq_codebooks_r17_e_ enhanced_type3_harq_codebooks_r17;
    max_num_pucch_txs_r17_e_             max_num_pucch_txs_r17;
  };
  struct triggered_harq_codebook_retx_r17_s_ {
    struct min_harq_retx_offset_r17_opts {
      enum options { n_neg7, n_neg5, n_neg3, n_neg1, n1, nulltype } value;
      typedef int8_t number_type;

      const char* to_string() const;
      int8_t      to_number() const;
    };
    using min_harq_retx_offset_r17_e_ = enumerated<min_harq_retx_offset_r17_opts>;
    struct max_harq_retx_offset_r17_opts {
      enum options { n4, n6, n8, n10, n12, n14, n16, n18, n20, n22, n24, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_harq_retx_offset_r17_e_ = enumerated<max_harq_retx_offset_r17_opts>;

    // member variables
    min_harq_retx_offset_r17_e_ min_harq_retx_offset_r17;
    max_harq_retx_offset_r17_e_ max_harq_retx_offset_r17;
  };
  struct max_mod_order_for_multicast_r17_c_ {
    struct fr1_r17_opts {
      enum options { qam256, qam1024, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using fr1_r17_e_ = enumerated<fr1_r17_opts>;
    struct fr2_r17_opts {
      enum options { qam64, qam256, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using fr2_r17_e_ = enumerated<fr2_r17_opts>;
    struct types_opts {
      enum options { fr1_r17, fr2_r17, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    max_mod_order_for_multicast_r17_c_() = default;
    max_mod_order_for_multicast_r17_c_(const max_mod_order_for_multicast_r17_c_& other);
    max_mod_order_for_multicast_r17_c_& operator=(const max_mod_order_for_multicast_r17_c_& other);
    ~max_mod_order_for_multicast_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fr1_r17_e_& fr1_r17()
    {
      assert_choice_type(types::fr1_r17, type_, "maxModulationOrderForMulticast-r17");
      return c.get<fr1_r17_e_>();
    }
    fr2_r17_e_& fr2_r17()
    {
      assert_choice_type(types::fr2_r17, type_, "maxModulationOrderForMulticast-r17");
      return c.get<fr2_r17_e_>();
    }
    const fr1_r17_e_& fr1_r17() const
    {
      assert_choice_type(types::fr1_r17, type_, "maxModulationOrderForMulticast-r17");
      return c.get<fr1_r17_e_>();
    }
    const fr2_r17_e_& fr2_r17() const
    {
      assert_choice_type(types::fr2_r17, type_, "maxModulationOrderForMulticast-r17");
      return c.get<fr2_r17_e_>();
    }
    fr1_r17_e_& set_fr1_r17();
    fr2_r17_e_& set_fr2_r17();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct dyn_slot_repeat_multicast_tn_non_shared_spec_ch_access_r17_opts {
    enum options { n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using dyn_slot_repeat_multicast_tn_non_shared_spec_ch_access_r17_e_ =
      enumerated<dyn_slot_repeat_multicast_tn_non_shared_spec_ch_access_r17_opts>;
  struct dyn_slot_repeat_multicast_ntn_shared_spec_ch_access_r17_opts {
    enum options { n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using dyn_slot_repeat_multicast_ntn_shared_spec_ch_access_r17_e_ =
      enumerated<dyn_slot_repeat_multicast_ntn_shared_spec_ch_access_r17_opts>;
  struct prs_meas_without_mg_r17_opts {
    enum options { cp_len, quarter_symbol, half_symbol, half_slot, nulltype } value;

    const char* to_string() const;
  };
  using prs_meas_without_mg_r17_e_                  = enumerated<prs_meas_without_mg_r17_opts>;
  using prs_processing_cap_outside_m_gin_ppw_r17_l_ = dyn_array<prs_processing_cap_outside_m_gin_ppw_per_type_r17_s>;
  struct srs_semi_persistent_pos_res_rrc_inactive_r17_s_ {
    struct max_num_of_semi_persistent_srs_pos_res_r17_opts {
      enum options { n1, n2, n4, n8, n16, n32, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_of_semi_persistent_srs_pos_res_r17_e_ = enumerated<max_num_of_semi_persistent_srs_pos_res_r17_opts>;
    struct max_num_of_semi_persistent_srs_pos_res_per_slot_r17_opts {
      enum options { n1, n2, n3, n4, n5, n6, n8, n10, n12, n14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_of_semi_persistent_srs_pos_res_per_slot_r17_e_ =
        enumerated<max_num_of_semi_persistent_srs_pos_res_per_slot_r17_opts>;

    // member variables
    max_num_of_semi_persistent_srs_pos_res_r17_e_          max_num_of_semi_persistent_srs_pos_res_r17;
    max_num_of_semi_persistent_srs_pos_res_per_slot_r17_e_ max_num_of_semi_persistent_srs_pos_res_per_slot_r17;
  };
  struct max_dyn_slot_repeat_for_sps_multicast_r17_opts {
    enum options { n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_dyn_slot_repeat_for_sps_multicast_r17_e_ = enumerated<max_dyn_slot_repeat_for_sps_multicast_r17_opts>;

  // member variables
  bool                   ext                                  = false;
  bool                   modified_mpr_behaviour_present       = false;
  bool                   mimo_params_per_band_present         = false;
  bool                   extended_cp_present                  = false;
  bool                   multiple_tci_present                 = false;
  bool                   bwp_without_restrict_present         = false;
  bool                   bwp_same_numerology_present          = false;
  bool                   bwp_diff_numerology_present          = false;
  bool                   cross_carrier_sched_same_scs_present = false;
  bool                   pdsch_256_qam_fr2_present            = false;
  bool                   pusch_256_qam_present                = false;
  bool                   ue_pwr_class_present                 = false;
  bool                   rate_matching_lte_crs_present        = false;
  bool                   ch_bws_dl_present                    = false;
  bool                   ch_bws_ul_present                    = false;
  uint16_t               band_nr                              = 1;
  fixed_bitstring<8>     modified_mpr_behaviour;
  mimo_params_per_band_s mimo_params_per_band;
  bwp_same_numerology_e_ bwp_same_numerology;
  ue_pwr_class_e_        ue_pwr_class;
  ch_bws_dl_c_           ch_bws_dl;
  ch_bws_ul_c_           ch_bws_ul;
  // ...
  // group 0
  bool                         max_ul_duty_cycle_pc2_fr1_present = false;
  max_ul_duty_cycle_pc2_fr1_e_ max_ul_duty_cycle_pc2_fr1;
  // group 1
  bool pucch_spatial_rel_info_mac_ce_present = false;
  bool pwr_boosting_pi2_bpsk_present         = false;
  // group 2
  bool                     max_ul_duty_cycle_fr2_present = false;
  max_ul_duty_cycle_fr2_e_ max_ul_duty_cycle_fr2;
  // group 3
  copy_ptr<ch_bws_dl_v1590_c_> ch_bws_dl_v1590;
  copy_ptr<ch_bws_ul_v1590_c_> ch_bws_ul_v1590;
  // group 4
  bool                     asymmetric_bw_combination_set_present = false;
  bounded_bitstring<1, 32> asymmetric_bw_combination_set;
  // group 5
  bool                                                  cancel_overlapping_pusch_r16_present              = false;
  bool                                                  overlap_rate_matching_eutra_crs_r16_present       = false;
  bool                                                  pdsch_map_type_b_alt_r16_present                  = false;
  bool                                                  one_slot_periodic_trs_r16_present                 = false;
  bool                                                  simul_srs_mimo_trans_within_band_r16_present      = false;
  bool                                                  raster_shift7dot5_iab_r16_present                 = false;
  bool                                                  ue_pwr_class_v1610_present                        = false;
  bool                                                  cond_ho_r16_present                               = false;
  bool                                                  cond_ho_fail_r16_present                          = false;
  bool                                                  cond_ho_two_trigger_events_r16_present            = false;
  bool                                                  cond_pscell_change_r16_present                    = false;
  bool                                                  cond_pscell_change_two_trigger_events_r16_present = false;
  bool                                                  mpr_pwr_boost_fr2_r16_present                     = false;
  bool                                                  joint_release_cfg_grant_type2_r16_present         = false;
  bool                                                  joint_release_sps_r16_present                     = false;
  bool                                                  simul_srs_trans_within_band_r16_present           = false;
  bool                                                  trs_add_bw_r16_present                            = false;
  bool                                                  ho_intra_f_iab_r16_present                        = false;
  copy_ptr<shared_spec_ch_access_params_per_band_r16_s> shared_spec_ch_access_params_per_band_r16;
  copy_ptr<multiple_rate_matching_eutra_crs_r16_s_>     multiple_rate_matching_eutra_crs_r16;
  copy_ptr<olpc_srs_pos_r16_s>                          olpc_srs_pos_r16;
  copy_ptr<spatial_relations_srs_pos_r16_s>             spatial_relations_srs_pos_r16;
  copy_ptr<ch_bw_dl_iab_r16_c_>                         ch_bw_dl_iab_r16;
  copy_ptr<ch_bw_ul_iab_r16_c_>                         ch_bw_ul_iab_r16;
  copy_ptr<active_cfg_grant_r16_s_>                     active_cfg_grant_r16;
  copy_ptr<sps_r16_s_>                                  sps_r16;
  trs_add_bw_r16_e_                                     trs_add_bw_r16;
  // group 6
  copy_ptr<simul_srs_for_ant_switching_r16_s>             simul_tx_srs_ant_switching_intra_band_ul_ca_r16;
  copy_ptr<shared_spec_ch_access_params_per_band_v1630_s> shared_spec_ch_access_params_per_band_v1630;
  // group 7
  bool                                                    ho_utra_fdd_r16_present                  = false;
  bool                                                    enhanced_ul_transient_period_r16_present = false;
  enhanced_ul_transient_period_r16_e_                     enhanced_ul_transient_period_r16;
  copy_ptr<shared_spec_ch_access_params_per_band_v1640_s> shared_spec_ch_access_params_per_band_v1640;
  // group 8
  bool                                                    type1_pusch_repeat_multi_slots_v1650_present = false;
  bool                                                    type2_pusch_repeat_multi_slots_v1650_present = false;
  bool                                                    pusch_repeat_multi_slots_v1650_present       = false;
  bool                                                    cfg_ul_grant_type1_v1650_present             = false;
  bool                                                    cfg_ul_grant_type2_v1650_present             = false;
  copy_ptr<shared_spec_ch_access_params_per_band_v1650_s> shared_spec_ch_access_params_per_band_v1650;
  // group 9
  bool enhanced_skip_ul_tx_cfg_v1660_present = false;
  bool enhanced_skip_ul_tx_dyn_v1660_present = false;
  // group 10
  bool                                     max_ul_duty_cycle_pc1dot5_mpe_fr1_r16_present = false;
  bool                                     tx_diversity_r16_present                      = false;
  max_ul_duty_cycle_pc1dot5_mpe_fr1_r16_e_ max_ul_duty_cycle_pc1dot5_mpe_fr1_r16;
  // group 11
  bool                                         pdsch_1024_qam_fr1_r17_present                          = false;
  bool                                         ue_pwr_class_v1700_present                              = false;
  bool                                         rlm_relaxation_r17_present                              = false;
  bool                                         bfd_relaxation_r17_present                              = false;
  bool                                         cg_sdt_r17_present                                      = false;
  bool                                         location_based_cond_ho_r17_present                      = false;
  bool                                         time_based_cond_ho_r17_present                          = false;
  bool                                         event_a4_based_cond_ho_r17_present                      = false;
  bool                                         mn_initiated_cond_pscell_change_nrdc_r17_present        = false;
  bool                                         sn_initiated_cond_pscell_change_nrdc_r17_present        = false;
  bool                                         pdcch_skipping_without_sssg_r17_present                 = false;
  bool                                         sssg_switching_1_bit_ind_r17_present                    = false;
  bool                                         sssg_switching_2_bit_ind_r17_present                    = false;
  bool                                         pdcch_skipping_with_sssg_r17_present                    = false;
  bool                                         search_space_set_grp_switch_cap2_r17_present            = false;
  bool                                         ul_pre_compensation_r17_present                         = false;
  bool                                         ul_ta_report_r17_present                                = false;
  bool                                         max_harq_process_num_r17_present                        = false;
  bool                                         type2_harq_codebook_r17_present                         = false;
  bool                                         type1_harq_codebook_r17_present                         = false;
  bool                                         type3_harq_codebook_r17_present                         = false;
  bool                                         ue_specific_k_offset_r17_present                        = false;
  bool                                         multi_pdsch_single_dci_fr2_1_scs_120k_hz_r17_present    = false;
  bool                                         multi_pusch_single_dci_fr2_1_scs_120k_hz_r17_present    = false;
  bool                                         parallel_prs_meas_rrc_inactive_r17_present              = false;
  bool                                         nr_ue_tx_teg_id_max_support_r17_present                 = false;
  bool                                         prs_processing_rrc_inactive_r17_present                 = false;
  bool                                         prs_processing_win_type1_a_r17_present                  = false;
  bool                                         prs_processing_win_type1_b_r17_present                  = false;
  bool                                         prs_processing_win_type2_r17_present                    = false;
  bool                                         max_num_pusch_type_a_repeat_r17_present                 = false;
  bool                                         pusch_type_a_repeats_avail_slot_r17_present             = false;
  bool                                         tb_processing_multi_slot_pusch_r17_present              = false;
  bool                                         tb_processing_rep_multi_slot_pusch_r17_present          = false;
  bool                                         pusch_repeat_msg3_r17_present                           = false;
  bool                                         parallel_meas_without_restrict_r17_present              = false;
  bool                                         max_num_ngso_satellites_within_one_smtc_r17_present     = false;
  bool                                         k1_range_ext_r17_present                                = false;
  bool                                         aperiodic_csi_rs_add_bw_r17_present                     = false;
  bool                                         bwp_without_cd_ssb_or_ncd_ssb_red_cap_r17_present       = false;
  bool                                         half_duplex_fdd_type_a_red_cap_r17_present              = false;
  bool                                         ch_bws_dl_scs_480k_hz_fr2_2_r17_present                 = false;
  bool                                         ch_bws_ul_scs_480k_hz_fr2_2_r17_present                 = false;
  bool                                         ch_bws_dl_scs_960k_hz_fr2_2_r17_present                 = false;
  bool                                         ch_bws_ul_scs_960k_hz_fr2_2_r17_present                 = false;
  bool                                         ul_gap_fr2_r17_present                                  = false;
  bool                                         one_shot_harq_feedback_triggered_by_dci_1_2_r17_present = false;
  bool                                         one_shot_harq_feedback_phy_prio_r17_present             = false;
  ue_pwr_class_v1700_e_                        ue_pwr_class_v1700;
  copy_ptr<fr2_2_access_params_per_band_r17_s> fr2_2_access_params_per_band_r17;
  max_harq_process_num_r17_e_                  max_harq_process_num_r17;
  nr_ue_tx_teg_id_max_support_r17_e_           nr_ue_tx_teg_id_max_support_r17;
  prs_processing_win_type1_a_r17_e_            prs_processing_win_type1_a_r17;
  prs_processing_win_type1_b_r17_e_            prs_processing_win_type1_b_r17;
  prs_processing_win_type2_r17_e_              prs_processing_win_type2_r17;
  copy_ptr<srs_all_pos_res_rrc_inactive_r17_s> srs_all_pos_res_rrc_inactive_r17;
  copy_ptr<olpc_srs_pos_r16_s>                 olpc_srs_pos_rrc_inactive_r17;
  copy_ptr<spatial_relations_srs_pos_r16_s>    spatial_relations_srs_pos_rrc_inactive_r17;
  copy_ptr<max_dur_dmrs_bundling_r17_s_>       max_dur_dmrs_bundling_r17;
  copy_ptr<shared_spec_ch_access_params_per_band_v1710_s>  shared_spec_ch_access_params_per_band_v1710;
  max_num_ngso_satellites_within_one_smtc_r17_e_           max_num_ngso_satellites_within_one_smtc_r17;
  copy_ptr<aperiodic_csi_rs_fast_scell_activation_r17_s_>  aperiodic_csi_rs_fast_scell_activation_r17;
  aperiodic_csi_rs_add_bw_r17_e_                           aperiodic_csi_rs_add_bw_r17;
  copy_ptr<pos_srs_rrc_inactive_outside_init_ul_bwp_r17_s> pos_srs_rrc_inactive_outside_init_ul_bwp_r17;
  fixed_bitstring<8>                                       ch_bws_dl_scs_480k_hz_fr2_2_r17;
  fixed_bitstring<8>                                       ch_bws_ul_scs_480k_hz_fr2_2_r17;
  fixed_bitstring<8>                                       ch_bws_dl_scs_960k_hz_fr2_2_r17;
  fixed_bitstring<8>                                       ch_bws_ul_scs_960k_hz_fr2_2_r17;
  copy_ptr<enhanced_type3_harq_codebook_feedback_r17_s_>   enhanced_type3_harq_codebook_feedback_r17;
  copy_ptr<triggered_harq_codebook_retx_r17_s_>            triggered_harq_codebook_retx_r17;
  // group 12
  bool    ue_one_shot_ul_timing_adj_r17_present                              = false;
  bool    pucch_repeat_f0_2_r17_present                                      = false;
  bool    cqi_4_bits_subband_ntn_shared_spec_ch_access_r17_present           = false;
  bool    mux_harq_ack_diff_priorities_r17_present                           = false;
  bool    ta_based_pdc_ntn_shared_spec_ch_access_r17_present                 = false;
  bool    ack_nack_feedback_for_multicast_with_dci_enabler_r17_present       = false;
  bool    max_num_g_rnti_r17_present                                         = false;
  bool    dyn_multicast_dci_format4_2_r17_present                            = false;
  bool    dyn_slot_repeat_multicast_tn_non_shared_spec_ch_access_r17_present = false;
  bool    dyn_slot_repeat_multicast_ntn_shared_spec_ch_access_r17_present    = false;
  bool    nack_only_feedback_for_multicast_with_dci_enabler_r17_present      = false;
  bool    ack_nack_feedback_for_sps_multicast_with_dci_enabler_r17_present   = false;
  bool    max_num_g_cs_rnti_r17_present                                      = false;
  bool    re_level_rate_matching_for_multicast_r17_present                   = false;
  bool    pdsch_1024_qam_2_mimo_fr1_r17_present                              = false;
  bool    prs_meas_without_mg_r17_present                                    = false;
  bool    max_num_leo_satellites_per_carrier_r17_present                     = false;
  bool    ch_bws_dl_scs_120k_hz_fr2_2_r17_present                            = false;
  bool    ch_bws_ul_scs_120k_hz_fr2_2_r17_present                            = false;
  uint8_t max_num_g_rnti_r17                                                 = 2;
  copy_ptr<max_mod_order_for_multicast_r17_c_> max_mod_order_for_multicast_r17;
  dyn_slot_repeat_multicast_tn_non_shared_spec_ch_access_r17_e_
                                                             dyn_slot_repeat_multicast_tn_non_shared_spec_ch_access_r17;
  dyn_slot_repeat_multicast_ntn_shared_spec_ch_access_r17_e_ dyn_slot_repeat_multicast_ntn_shared_spec_ch_access_r17;
  uint8_t                                                    max_num_g_cs_rnti_r17 = 2;
  prs_meas_without_mg_r17_e_                                 prs_meas_without_mg_r17;
  uint8_t                                                    max_num_leo_satellites_per_carrier_r17 = 3;
  copy_ptr<prs_processing_cap_outside_m_gin_ppw_r17_l_>      prs_processing_cap_outside_m_gin_ppw_r17;
  copy_ptr<srs_semi_persistent_pos_res_rrc_inactive_r17_s_>  srs_semi_persistent_pos_res_rrc_inactive_r17;
  fixed_bitstring<8>                                         ch_bws_dl_scs_120k_hz_fr2_2_r17;
  fixed_bitstring<8>                                         ch_bws_ul_scs_120k_hz_fr2_2_r17;
  // group 13
  bool dmrs_bundling_pusch_rep_type_a_r17_present                = false;
  bool dmrs_bundling_pusch_rep_type_b_r17_present                = false;
  bool dmrs_bundling_pusch_multi_slot_r17_present                = false;
  bool dmrs_bundling_pucch_rep_r17_present                       = false;
  bool inter_slot_freq_hop_inter_slot_bundling_pusch_r17_present = false;
  bool inter_slot_freq_hop_pucch_r17_present                     = false;
  bool dmrs_bundling_restart_r17_present                         = false;
  bool dmrs_bundling_non_back_to_back_tx_r17_present             = false;
  // group 14
  bool max_dyn_slot_repeat_for_sps_multicast_r17_present                 = false;
  bool nack_only_feedback_for_sps_multicast_with_dci_enabler_r17_present = false;
  bool sps_multicast_dci_format4_2_r17_present                           = false;
  bool sps_multicast_multi_cfg_r17_present                               = false;
  bool prio_ind_in_dci_multicast_r17_present                             = false;
  bool prio_ind_in_dci_sps_multicast_r17_present                         = false;
  bool two_harq_ack_codebook_for_unicast_and_multicast_r17_present       = false;
  bool multi_pucch_harq_ack_for_multicast_unicast_r17_present            = false;
  bool release_sps_multicast_with_cs_rnti_r17_present                    = false;
  max_dyn_slot_repeat_for_sps_multicast_r17_e_ max_dyn_slot_repeat_for_sps_multicast_r17;
  uint8_t                                      sps_multicast_multi_cfg_r17 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandSidelink-r16 ::= SEQUENCE
struct band_sidelink_r16_s {
  struct sl_reception_r16_s_ {
    struct harq_rx_process_sidelink_r16_opts {
      enum options { n16, n24, n32, n48, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using harq_rx_process_sidelink_r16_e_ = enumerated<harq_rx_process_sidelink_r16_opts>;
    struct pscch_rx_sidelink_r16_opts {
      enum options { value1, value2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using pscch_rx_sidelink_r16_e_ = enumerated<pscch_rx_sidelink_r16_opts>;
    struct scs_cp_pattern_rx_sidelink_r16_c_ {
      struct fr1_r16_s_ {
        bool                scs_15k_hz_r16_present = false;
        bool                scs_30k_hz_r16_present = false;
        bool                scs_60k_hz_r16_present = false;
        fixed_bitstring<16> scs_15k_hz_r16;
        fixed_bitstring<16> scs_30k_hz_r16;
        fixed_bitstring<16> scs_60k_hz_r16;
      };
      struct fr2_r16_s_ {
        bool                scs_60k_hz_r16_present  = false;
        bool                scs_120k_hz_r16_present = false;
        fixed_bitstring<16> scs_60k_hz_r16;
        fixed_bitstring<16> scs_120k_hz_r16;
      };
      struct types_opts {
        enum options { fr1_r16, fr2_r16, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      scs_cp_pattern_rx_sidelink_r16_c_() = default;
      scs_cp_pattern_rx_sidelink_r16_c_(const scs_cp_pattern_rx_sidelink_r16_c_& other);
      scs_cp_pattern_rx_sidelink_r16_c_& operator=(const scs_cp_pattern_rx_sidelink_r16_c_& other);
      ~scs_cp_pattern_rx_sidelink_r16_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      fr1_r16_s_& fr1_r16()
      {
        assert_choice_type(types::fr1_r16, type_, "scs-CP-PatternRxSidelink-r16");
        return c.get<fr1_r16_s_>();
      }
      fr2_r16_s_& fr2_r16()
      {
        assert_choice_type(types::fr2_r16, type_, "scs-CP-PatternRxSidelink-r16");
        return c.get<fr2_r16_s_>();
      }
      const fr1_r16_s_& fr1_r16() const
      {
        assert_choice_type(types::fr1_r16, type_, "scs-CP-PatternRxSidelink-r16");
        return c.get<fr1_r16_s_>();
      }
      const fr2_r16_s_& fr2_r16() const
      {
        assert_choice_type(types::fr2_r16, type_, "scs-CP-PatternRxSidelink-r16");
        return c.get<fr2_r16_s_>();
      }
      fr1_r16_s_& set_fr1_r16();
      fr2_r16_s_& set_fr2_r16();

    private:
      types                                   type_;
      choice_buffer_t<fr1_r16_s_, fr2_r16_s_> c;

      void destroy_();
    };

    // member variables
    bool                              scs_cp_pattern_rx_sidelink_r16_present = false;
    bool                              extended_cp_rx_sidelink_r16_present    = false;
    harq_rx_process_sidelink_r16_e_   harq_rx_process_sidelink_r16;
    pscch_rx_sidelink_r16_e_          pscch_rx_sidelink_r16;
    scs_cp_pattern_rx_sidelink_r16_c_ scs_cp_pattern_rx_sidelink_r16;
  };
  struct sl_tx_mode1_r16_s_ {
    struct harq_tx_process_mode_one_sidelink_r16_opts {
      enum options { n8, n16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using harq_tx_process_mode_one_sidelink_r16_e_ = enumerated<harq_tx_process_mode_one_sidelink_r16_opts>;
    struct scs_cp_pattern_tx_sidelink_mode_one_r16_c_ {
      struct fr1_r16_s_ {
        bool                scs_15k_hz_r16_present = false;
        bool                scs_30k_hz_r16_present = false;
        bool                scs_60k_hz_r16_present = false;
        fixed_bitstring<16> scs_15k_hz_r16;
        fixed_bitstring<16> scs_30k_hz_r16;
        fixed_bitstring<16> scs_60k_hz_r16;
      };
      struct fr2_r16_s_ {
        bool                scs_60k_hz_r16_present  = false;
        bool                scs_120k_hz_r16_present = false;
        fixed_bitstring<16> scs_60k_hz_r16;
        fixed_bitstring<16> scs_120k_hz_r16;
      };
      struct types_opts {
        enum options { fr1_r16, fr2_r16, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      scs_cp_pattern_tx_sidelink_mode_one_r16_c_() = default;
      scs_cp_pattern_tx_sidelink_mode_one_r16_c_(const scs_cp_pattern_tx_sidelink_mode_one_r16_c_& other);
      scs_cp_pattern_tx_sidelink_mode_one_r16_c_& operator=(const scs_cp_pattern_tx_sidelink_mode_one_r16_c_& other);
      ~scs_cp_pattern_tx_sidelink_mode_one_r16_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      fr1_r16_s_& fr1_r16()
      {
        assert_choice_type(types::fr1_r16, type_, "scs-CP-PatternTxSidelinkModeOne-r16");
        return c.get<fr1_r16_s_>();
      }
      fr2_r16_s_& fr2_r16()
      {
        assert_choice_type(types::fr2_r16, type_, "scs-CP-PatternTxSidelinkModeOne-r16");
        return c.get<fr2_r16_s_>();
      }
      const fr1_r16_s_& fr1_r16() const
      {
        assert_choice_type(types::fr1_r16, type_, "scs-CP-PatternTxSidelinkModeOne-r16");
        return c.get<fr1_r16_s_>();
      }
      const fr2_r16_s_& fr2_r16() const
      {
        assert_choice_type(types::fr2_r16, type_, "scs-CP-PatternTxSidelinkModeOne-r16");
        return c.get<fr2_r16_s_>();
      }
      fr1_r16_s_& set_fr1_r16();
      fr2_r16_s_& set_fr2_r16();

    private:
      types                                   type_;
      choice_buffer_t<fr1_r16_s_, fr2_r16_s_> c;

      void destroy_();
    };

    // member variables
    bool                                       extended_cp_tx_sidelink_r16_present = false;
    bool                                       harq_report_on_pucch_r16_present    = false;
    harq_tx_process_mode_one_sidelink_r16_e_   harq_tx_process_mode_one_sidelink_r16;
    scs_cp_pattern_tx_sidelink_mode_one_r16_c_ scs_cp_pattern_tx_sidelink_mode_one_r16;
  };
  struct sync_sidelink_r16_s_ {
    bool gnb_sync_r16_present                              = false;
    bool gnb_gnss_ue_sync_with_prio_on_gnb_enb_r16_present = false;
    bool gnb_gnss_ue_sync_with_prio_on_gnss_r16_present    = false;
  };
  struct psfch_format_zero_sidelink_r16_s_ {
    struct psfch_rx_num_opts {
      enum options { n5, n15, n25, n32, n35, n45, n50, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using psfch_rx_num_e_ = enumerated<psfch_rx_num_opts>;
    struct psfch_tx_num_opts {
      enum options { n4, n8, n16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using psfch_tx_num_e_ = enumerated<psfch_tx_num_opts>;

    // member variables
    psfch_rx_num_e_ psfch_rx_num;
    psfch_tx_num_e_ psfch_tx_num;
  };
  struct sl_tx_mode2_r16_s_ {
    struct harq_tx_process_mode_two_sidelink_r16_opts {
      enum options { n8, n16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using harq_tx_process_mode_two_sidelink_r16_e_ = enumerated<harq_tx_process_mode_two_sidelink_r16_opts>;

    // member variables
    bool                                     scs_cp_pattern_tx_sidelink_mode_two_r16_present = false;
    bool                                     dl_open_loop_pc_sidelink_r16_present            = false;
    harq_tx_process_mode_two_sidelink_r16_e_ harq_tx_process_mode_two_sidelink_r16;
  };
  struct congestion_ctrl_sidelink_r16_s_ {
    struct cbr_cr_time_limit_sidelink_r16_opts {
      enum options { time1, time2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using cbr_cr_time_limit_sidelink_r16_e_ = enumerated<cbr_cr_time_limit_sidelink_r16_opts>;

    // member variables
    bool                              cbr_report_sidelink_r16_present = false;
    cbr_cr_time_limit_sidelink_r16_e_ cbr_cr_time_limit_sidelink_r16;
  };
  struct ue_pwr_class_sidelink_r16_opts {
    enum options { pc2, pc3, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ue_pwr_class_sidelink_r16_e_ = enumerated<ue_pwr_class_sidelink_r16_opts>;
  struct sl_tx_mode2_random_res_sel_r17_s_ {
    struct harq_tx_process_mode_two_sidelink_r17_opts {
      enum options { n8, n16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using harq_tx_process_mode_two_sidelink_r17_e_ = enumerated<harq_tx_process_mode_two_sidelink_r17_opts>;
    struct scs_cp_pattern_tx_sidelink_mode_two_r17_c_ {
      struct fr1_r17_s_ {
        bool                scs_15k_hz_r17_present = false;
        bool                scs_30k_hz_r17_present = false;
        bool                scs_60k_hz_r17_present = false;
        fixed_bitstring<16> scs_15k_hz_r17;
        fixed_bitstring<16> scs_30k_hz_r17;
        fixed_bitstring<16> scs_60k_hz_r17;
      };
      struct fr2_r17_s_ {
        bool                scs_60k_hz_r17_present  = false;
        bool                scs_120k_hz_r17_present = false;
        fixed_bitstring<16> scs_60k_hz_r17;
        fixed_bitstring<16> scs_120k_hz_r17;
      };
      struct types_opts {
        enum options { fr1_r17, fr2_r17, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      scs_cp_pattern_tx_sidelink_mode_two_r17_c_() = default;
      scs_cp_pattern_tx_sidelink_mode_two_r17_c_(const scs_cp_pattern_tx_sidelink_mode_two_r17_c_& other);
      scs_cp_pattern_tx_sidelink_mode_two_r17_c_& operator=(const scs_cp_pattern_tx_sidelink_mode_two_r17_c_& other);
      ~scs_cp_pattern_tx_sidelink_mode_two_r17_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      fr1_r17_s_& fr1_r17()
      {
        assert_choice_type(types::fr1_r17, type_, "scs-CP-PatternTxSidelinkModeTwo-r17");
        return c.get<fr1_r17_s_>();
      }
      fr2_r17_s_& fr2_r17()
      {
        assert_choice_type(types::fr2_r17, type_, "scs-CP-PatternTxSidelinkModeTwo-r17");
        return c.get<fr2_r17_s_>();
      }
      const fr1_r17_s_& fr1_r17() const
      {
        assert_choice_type(types::fr1_r17, type_, "scs-CP-PatternTxSidelinkModeTwo-r17");
        return c.get<fr1_r17_s_>();
      }
      const fr2_r17_s_& fr2_r17() const
      {
        assert_choice_type(types::fr2_r17, type_, "scs-CP-PatternTxSidelinkModeTwo-r17");
        return c.get<fr2_r17_s_>();
      }
      fr1_r17_s_& set_fr1_r17();
      fr2_r17_s_& set_fr2_r17();

    private:
      types                                   type_;
      choice_buffer_t<fr1_r17_s_, fr2_r17_s_> c;

      void destroy_();
    };

    // member variables
    bool                                       scs_cp_pattern_tx_sidelink_mode_two_r17_present = false;
    bool                                       extended_cp_mode2_random_r17_present            = false;
    bool                                       dl_open_loop_pc_sidelink_r17_present            = false;
    harq_tx_process_mode_two_sidelink_r17_e_   harq_tx_process_mode_two_sidelink_r17;
    scs_cp_pattern_tx_sidelink_mode_two_r17_c_ scs_cp_pattern_tx_sidelink_mode_two_r17;
  };
  struct sync_sidelink_v1710_s_ {
    bool sync_gnss_r17_present                             = false;
    bool gnb_sync_r17_present                              = false;
    bool gnb_gnss_ue_sync_with_prio_on_gnb_enb_r17_present = false;
    bool gnb_gnss_ue_sync_with_prio_on_gnss_r17_present    = false;
  };
  struct rx_iuc_scheme2_mode2_sidelink_r17_opts {
    enum options { n5, n15, n25, n32, n35, n45, n50, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using rx_iuc_scheme2_mode2_sidelink_r17_e_ = enumerated<rx_iuc_scheme2_mode2_sidelink_r17_opts>;

  // member variables
  bool                              ext                                          = false;
  bool                              sl_reception_r16_present                     = false;
  bool                              sl_tx_mode1_r16_present                      = false;
  bool                              sync_sidelink_r16_present                    = false;
  bool                              sl_tx_256_qam_r16_present                    = false;
  bool                              psfch_format_zero_sidelink_r16_present       = false;
  bool                              low_se_64_qam_mcs_table_sidelink_r16_present = false;
  bool                              enb_sync_sidelink_r16_present                = false;
  uint16_t                          freq_band_sidelink_r16                       = 1;
  sl_reception_r16_s_               sl_reception_r16;
  sl_tx_mode1_r16_s_                sl_tx_mode1_r16;
  sync_sidelink_r16_s_              sync_sidelink_r16;
  psfch_format_zero_sidelink_r16_s_ psfch_format_zero_sidelink_r16;
  // ...
  // group 0
  bool                                      fewer_symbol_slot_sidelink_r16_present           = false;
  bool                                      sl_open_loop_pc_rsrp_report_sidelink_r16_present = false;
  bool                                      sl_rx_256_qam_r16_present                        = false;
  copy_ptr<sl_tx_mode2_r16_s_>              sl_tx_mode2_r16;
  copy_ptr<congestion_ctrl_sidelink_r16_s_> congestion_ctrl_sidelink_r16;
  // group 1
  bool                         ue_pwr_class_sidelink_r16_present = false;
  ue_pwr_class_sidelink_r16_e_ ue_pwr_class_sidelink_r16;
  // group 2
  bool                                        enb_sync_sidelink_v1710_present                         = false;
  bool                                        rx_iuc_scheme1_preferred_mode2_sidelink_r17_present     = false;
  bool                                        rx_iuc_scheme1_non_preferred_mode2_sidelink_r17_present = false;
  bool                                        rx_iuc_scheme2_mode2_sidelink_r17_present               = false;
  bool                                        rx_iuc_scheme1_sci_r17_present                          = false;
  bool                                        rx_iuc_scheme1_sci_explicit_req_r17_present             = false;
  copy_ptr<sl_tx_mode2_random_res_sel_r17_s_> sl_tx_mode2_random_res_sel_r17;
  copy_ptr<sync_sidelink_v1710_s_>            sync_sidelink_v1710;
  rx_iuc_scheme2_mode2_sidelink_r17_e_        rx_iuc_scheme2_mode2_sidelink_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandSidelinkEUTRA-r16 ::= SEQUENCE
struct band_sidelink_eutra_r16_s {
  struct gnb_sched_mode3_sidelink_eutra_r16_s_ {
    struct gnb_sched_mode3_delay_sidelink_eutra_r16_opts {
      enum options {
        ms0,
        ms0dot25,
        ms0dot5,
        ms0dot625,
        ms0dot75,
        ms1,
        ms1dot25,
        ms1dot5,
        ms1dot75,
        ms2,
        ms2dot5,
        ms3,
        ms4,
        ms5,
        ms6,
        ms8,
        ms10,
        ms20,
        nulltype
      } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    using gnb_sched_mode3_delay_sidelink_eutra_r16_e_ = enumerated<gnb_sched_mode3_delay_sidelink_eutra_r16_opts>;

    // member variables
    gnb_sched_mode3_delay_sidelink_eutra_r16_e_ gnb_sched_mode3_delay_sidelink_eutra_r16;
  };

  // member variables
  bool                                  gnb_sched_mode3_sidelink_eutra_r16_present = false;
  bool                                  gnb_sched_mode4_sidelink_eutra_r16_present = false;
  uint16_t                              freq_band_sidelink_eutra_r16               = 1;
  gnb_sched_mode3_sidelink_eutra_r16_s_ gnb_sched_mode3_sidelink_eutra_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookVariantsList-r16 ::= SEQUENCE (SIZE (1..512)) OF SupportedCSI-RS-Resource
using codebook_variants_list_r16_l = dyn_array<supported_csi_rs_res_s>;

// EUTRA-ParametersCommon ::= SEQUENCE
struct eutra_params_common_s {
  bool                ext                                 = false;
  bool                mfbi_eutra_present                  = false;
  bool                modified_mpr_behavior_eutra_present = false;
  bool                multi_ns_pmax_eutra_present         = false;
  bool                rs_sinr_meas_eutra_present          = false;
  fixed_bitstring<32> modified_mpr_behavior_eutra;
  // ...
  // group 0
  bool ne_dc_present = false;
  // group 1
  bool nr_ho_to_en_dc_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-ParametersXDD-Diff ::= SEQUENCE
struct eutra_params_xdd_diff_s {
  bool ext                              = false;
  bool rsrq_meas_wideband_eutra_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-Parameters ::= SEQUENCE
struct eutra_params_s {
  using supported_band_list_eutra_l_ = dyn_array<uint16_t>;

  // member variables
  bool                         ext                           = false;
  bool                         eutra_params_common_present   = false;
  bool                         eutra_params_xdd_diff_present = false;
  supported_band_list_eutra_l_ supported_band_list_eutra;
  eutra_params_common_s        eutra_params_common;
  eutra_params_xdd_diff_s      eutra_params_xdd_diff;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSet ::= CHOICE
struct feature_set_c {
  struct eutra_s_ {
    uint16_t dl_set_eutra = 0;
    uint16_t ul_set_eutra = 0;
  };
  struct nr_s_ {
    uint16_t dl_set_nr = 0;
    uint16_t ul_set_nr = 0;
  };
  struct types_opts {
    enum options { eutra, nr, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  feature_set_c() = default;
  feature_set_c(const feature_set_c& other);
  feature_set_c& operator=(const feature_set_c& other);
  ~feature_set_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutra_s_& eutra()
  {
    assert_choice_type(types::eutra, type_, "FeatureSet");
    return c.get<eutra_s_>();
  }
  nr_s_& nr()
  {
    assert_choice_type(types::nr, type_, "FeatureSet");
    return c.get<nr_s_>();
  }
  const eutra_s_& eutra() const
  {
    assert_choice_type(types::eutra, type_, "FeatureSet");
    return c.get<eutra_s_>();
  }
  const nr_s_& nr() const
  {
    assert_choice_type(types::nr, type_, "FeatureSet");
    return c.get<nr_s_>();
  }
  eutra_s_& set_eutra();
  nr_s_&    set_nr();

private:
  types                            type_;
  choice_buffer_t<eutra_s_, nr_s_> c;

  void destroy_();
};

// FeatureSetsPerBand ::= SEQUENCE (SIZE (1..128)) OF FeatureSet
using feature_sets_per_band_l = dyn_array<feature_set_c>;

// FeatureSetCombination ::= SEQUENCE (SIZE (1..32)) OF FeatureSetsPerBand
using feature_set_combination_l = dyn_array<feature_sets_per_band_l>;

// DummyA ::= SEQUENCE
struct dummy_a_s {
  struct max_num_ports_across_nzp_csi_rs_per_cc_opts {
    enum options {
      p2,
      p4,
      p8,
      p12,
      p16,
      p24,
      p32,
      p40,
      p48,
      p56,
      p64,
      p72,
      p80,
      p88,
      p96,
      p104,
      p112,
      p120,
      p128,
      p136,
      p144,
      p152,
      p160,
      p168,
      p176,
      p184,
      p192,
      p200,
      p208,
      p216,
      p224,
      p232,
      p240,
      p248,
      p256,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using max_num_ports_across_nzp_csi_rs_per_cc_e_ = enumerated<max_num_ports_across_nzp_csi_rs_per_cc_opts>;
  struct max_num_cs_im_per_cc_opts {
    enum options { n1, n2, n4, n8, n16, n32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_cs_im_per_cc_e_ = enumerated<max_num_cs_im_per_cc_opts>;
  struct max_num_simul_csi_rs_act_bwp_all_cc_opts {
    enum options {
      n5,
      n6,
      n7,
      n8,
      n9,
      n10,
      n12,
      n14,
      n16,
      n18,
      n20,
      n22,
      n24,
      n26,
      n28,
      n30,
      n32,
      n34,
      n36,
      n38,
      n40,
      n42,
      n44,
      n46,
      n48,
      n50,
      n52,
      n54,
      n56,
      n58,
      n60,
      n62,
      n64,
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_simul_csi_rs_act_bwp_all_cc_e_ = enumerated<max_num_simul_csi_rs_act_bwp_all_cc_opts>;
  struct total_num_ports_simul_csi_rs_act_bwp_all_cc_opts {
    enum options {
      p8,
      p12,
      p16,
      p24,
      p32,
      p40,
      p48,
      p56,
      p64,
      p72,
      p80,
      p88,
      p96,
      p104,
      p112,
      p120,
      p128,
      p136,
      p144,
      p152,
      p160,
      p168,
      p176,
      p184,
      p192,
      p200,
      p208,
      p216,
      p224,
      p232,
      p240,
      p248,
      p256,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using total_num_ports_simul_csi_rs_act_bwp_all_cc_e_ = enumerated<total_num_ports_simul_csi_rs_act_bwp_all_cc_opts>;

  // member variables
  uint8_t                                        max_num_nzp_csi_rs_per_cc = 1;
  max_num_ports_across_nzp_csi_rs_per_cc_e_      max_num_ports_across_nzp_csi_rs_per_cc;
  max_num_cs_im_per_cc_e_                        max_num_cs_im_per_cc;
  max_num_simul_csi_rs_act_bwp_all_cc_e_         max_num_simul_csi_rs_act_bwp_all_cc;
  total_num_ports_simul_csi_rs_act_bwp_all_cc_e_ total_num_ports_simul_csi_rs_act_bwp_all_cc;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DummyB ::= SEQUENCE
struct dummy_b_s {
  struct max_num_tx_ports_per_res_opts {
    enum options { p2, p4, p8, p12, p16, p24, p32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_tx_ports_per_res_e_ = enumerated<max_num_tx_ports_per_res_opts>;
  struct supported_codebook_mode_opts {
    enum options { mode1, mode1_and_mode2, nulltype } value;

    const char* to_string() const;
  };
  using supported_codebook_mode_e_ = enumerated<supported_codebook_mode_opts>;

  // member variables
  max_num_tx_ports_per_res_e_ max_num_tx_ports_per_res;
  uint8_t                     max_num_res        = 1;
  uint16_t                    total_num_tx_ports = 2;
  supported_codebook_mode_e_  supported_codebook_mode;
  uint8_t                     max_num_csi_rs_per_res_set = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DummyC ::= SEQUENCE
struct dummy_c_s {
  struct max_num_tx_ports_per_res_opts {
    enum options { p8, p16, p32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_tx_ports_per_res_e_ = enumerated<max_num_tx_ports_per_res_opts>;
  struct supported_codebook_mode_opts {
    enum options { mode1, mode2, both, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using supported_codebook_mode_e_ = enumerated<supported_codebook_mode_opts>;
  struct supported_num_panels_opts {
    enum options { n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using supported_num_panels_e_ = enumerated<supported_num_panels_opts>;

  // member variables
  max_num_tx_ports_per_res_e_ max_num_tx_ports_per_res;
  uint8_t                     max_num_res        = 1;
  uint16_t                    total_num_tx_ports = 2;
  supported_codebook_mode_e_  supported_codebook_mode;
  supported_num_panels_e_     supported_num_panels;
  uint8_t                     max_num_csi_rs_per_res_set = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DummyD ::= SEQUENCE
struct dummy_d_s {
  struct max_num_tx_ports_per_res_opts {
    enum options { p4, p8, p12, p16, p24, p32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_tx_ports_per_res_e_ = enumerated<max_num_tx_ports_per_res_opts>;
  struct amplitude_scaling_type_opts {
    enum options { wideband, wideband_and_subband, nulltype } value;

    const char* to_string() const;
  };
  using amplitude_scaling_type_e_ = enumerated<amplitude_scaling_type_opts>;

  // member variables
  bool                        amplitude_subset_restrict_present = false;
  max_num_tx_ports_per_res_e_ max_num_tx_ports_per_res;
  uint8_t                     max_num_res        = 1;
  uint16_t                    total_num_tx_ports = 2;
  uint8_t                     param_lx           = 2;
  amplitude_scaling_type_e_   amplitude_scaling_type;
  uint8_t                     max_num_csi_rs_per_res_set = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DummyE ::= SEQUENCE
struct dummy_e_s {
  struct max_num_tx_ports_per_res_opts {
    enum options { p4, p8, p12, p16, p24, p32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_tx_ports_per_res_e_ = enumerated<max_num_tx_ports_per_res_opts>;
  struct amplitude_scaling_type_opts {
    enum options { wideband, wideband_and_subband, nulltype } value;

    const char* to_string() const;
  };
  using amplitude_scaling_type_e_ = enumerated<amplitude_scaling_type_opts>;

  // member variables
  max_num_tx_ports_per_res_e_ max_num_tx_ports_per_res;
  uint8_t                     max_num_res        = 1;
  uint16_t                    total_num_tx_ports = 2;
  uint8_t                     param_lx           = 2;
  amplitude_scaling_type_e_   amplitude_scaling_type;
  uint8_t                     max_num_csi_rs_per_res_set = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqSeparationClass ::= ENUMERATED
struct freq_separation_class_opts {
  enum options { mhz800, mhz1200, mhz1400, /*...*/ mhz400_v1650, mhz600_v1650, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using freq_separation_class_e = enumerated<freq_separation_class_opts, true, 2>;

// FeatureSetDownlink ::= SEQUENCE
struct feature_set_dl_s {
  using feature_set_list_per_dl_cc_l_ = bounded_array<uint16_t, 32>;
  struct scaling_factor_opts {
    enum options { f0p4, f0p75, f0p8, nulltype } value;

    const char* to_string() const;
  };
  using scaling_factor_e_ = enumerated<scaling_factor_opts>;
  struct pdcch_monitoring_any_occasions_opts {
    enum options { without_dci_gap, with_dci_gap, nulltype } value;

    const char* to_string() const;
  };
  using pdcch_monitoring_any_occasions_e_ = enumerated<pdcch_monitoring_any_occasions_opts>;
  struct time_dur_for_qcl_s_ {
    struct scs_60k_hz_opts {
      enum options { s7, s14, s28, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_60k_hz_e_ = enumerated<scs_60k_hz_opts>;
    struct scs_120k_hz_opts {
      enum options { s14, s28, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_120k_hz_e_ = enumerated<scs_120k_hz_opts>;

    // member variables
    bool           scs_60k_hz_present  = false;
    bool           scs_120k_hz_present = false;
    scs_60k_hz_e_  scs_60k_hz;
    scs_120k_hz_e_ scs_120k_hz;
  };
  struct pdsch_processing_type1_different_tb_per_slot_s_ {
    struct scs_15k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_15k_hz_e_ = enumerated<scs_15k_hz_opts>;
    struct scs_30k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_30k_hz_e_ = enumerated<scs_30k_hz_opts>;
    struct scs_60k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_60k_hz_e_ = enumerated<scs_60k_hz_opts>;
    struct scs_120k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_120k_hz_e_ = enumerated<scs_120k_hz_opts>;

    // member variables
    bool           scs_15k_hz_present  = false;
    bool           scs_30k_hz_present  = false;
    bool           scs_60k_hz_present  = false;
    bool           scs_120k_hz_present = false;
    scs_15k_hz_e_  scs_15k_hz;
    scs_30k_hz_e_  scs_30k_hz;
    scs_60k_hz_e_  scs_60k_hz;
    scs_120k_hz_e_ scs_120k_hz;
  };
  using dummy4_l_ = dyn_array<dummy_b_s>;
  using dummy5_l_ = dyn_array<dummy_c_s>;
  using dummy6_l_ = dyn_array<dummy_d_s>;
  using dummy7_l_ = dyn_array<dummy_e_s>;

  // member variables
  bool                                            intra_band_freq_separation_dl_present                = false;
  bool                                            scaling_factor_present                               = false;
  bool                                            dummy8_present                                       = false;
  bool                                            scell_without_ssb_present                            = false;
  bool                                            csi_rs_meas_scell_without_ssb_present                = false;
  bool                                            dummy1_present                                       = false;
  bool                                            type1_3_css_present                                  = false;
  bool                                            pdcch_monitoring_any_occasions_present               = false;
  bool                                            dummy2_present                                       = false;
  bool                                            ue_specific_ul_dl_assign_present                     = false;
  bool                                            search_space_sharing_ca_dl_present                   = false;
  bool                                            time_dur_for_qcl_present                             = false;
  bool                                            pdsch_processing_type1_different_tb_per_slot_present = false;
  bool                                            dummy3_present                                       = false;
  feature_set_list_per_dl_cc_l_                   feature_set_list_per_dl_cc;
  freq_separation_class_e                         intra_band_freq_separation_dl;
  scaling_factor_e_                               scaling_factor;
  pdcch_monitoring_any_occasions_e_               pdcch_monitoring_any_occasions;
  time_dur_for_qcl_s_                             time_dur_for_qcl;
  pdsch_processing_type1_different_tb_per_slot_s_ pdsch_processing_type1_different_tb_per_slot;
  dummy_a_s                                       dummy3;
  dummy4_l_                                       dummy4;
  dummy5_l_                                       dummy5;
  dummy6_l_                                       dummy6;
  dummy7_l_                                       dummy7;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ProcessingParameters ::= SEQUENCE
struct processing_params_s {
  struct fallback_opts {
    enum options { sc, cap1_only, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using fallback_e_ = enumerated<fallback_opts>;
  struct different_tb_per_slot_s_ {
    bool    upto1_present = false;
    bool    upto2_present = false;
    bool    upto4_present = false;
    bool    upto7_present = false;
    uint8_t upto1         = 1;
    uint8_t upto2         = 1;
    uint8_t upto4         = 1;
    uint8_t upto7         = 1;
  };

  // member variables
  bool                     different_tb_per_slot_present = false;
  fallback_e_              fallback;
  different_tb_per_slot_s_ different_tb_per_slot;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetDownlink-v1540 ::= SEQUENCE
struct feature_set_dl_v1540_s {
  struct pdcch_monitoring_any_occasions_with_span_gap_s_ {
    struct scs_15k_hz_opts {
      enum options { set1, set2, set3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_15k_hz_e_ = enumerated<scs_15k_hz_opts>;
    struct scs_30k_hz_opts {
      enum options { set1, set2, set3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_30k_hz_e_ = enumerated<scs_30k_hz_opts>;
    struct scs_60k_hz_opts {
      enum options { set1, set2, set3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_60k_hz_e_ = enumerated<scs_60k_hz_opts>;
    struct scs_120k_hz_opts {
      enum options { set1, set2, set3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_120k_hz_e_ = enumerated<scs_120k_hz_opts>;

    // member variables
    bool           scs_15k_hz_present  = false;
    bool           scs_30k_hz_present  = false;
    bool           scs_60k_hz_present  = false;
    bool           scs_120k_hz_present = false;
    scs_15k_hz_e_  scs_15k_hz;
    scs_30k_hz_e_  scs_30k_hz;
    scs_60k_hz_e_  scs_60k_hz;
    scs_120k_hz_e_ scs_120k_hz;
  };
  struct pdsch_processing_type2_s_ {
    bool                scs_15k_hz_present = false;
    bool                scs_30k_hz_present = false;
    bool                scs_60k_hz_present = false;
    processing_params_s scs_15k_hz;
    processing_params_s scs_30k_hz;
    processing_params_s scs_60k_hz;
  };
  struct pdsch_processing_type2_limited_s_ {
    struct different_tb_per_slot_scs_30k_hz_opts {
      enum options { upto1, upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using different_tb_per_slot_scs_30k_hz_e_ = enumerated<different_tb_per_slot_scs_30k_hz_opts>;

    // member variables
    different_tb_per_slot_scs_30k_hz_e_ different_tb_per_slot_scs_30k_hz;
  };

  // member variables
  bool                                            one_fl_dmrs_two_add_dmrs_dl_present                  = false;
  bool                                            add_dmrs_dl_alt_present                              = false;
  bool                                            two_fl_dmrs_two_add_dmrs_dl_present                  = false;
  bool                                            one_fl_dmrs_three_add_dmrs_dl_present                = false;
  bool                                            pdcch_monitoring_any_occasions_with_span_gap_present = false;
  bool                                            pdsch_separation_with_gap_present                    = false;
  bool                                            pdsch_processing_type2_present                       = false;
  bool                                            pdsch_processing_type2_limited_present               = false;
  bool                                            dl_mcs_table_alt_dyn_ind_present                     = false;
  pdcch_monitoring_any_occasions_with_span_gap_s_ pdcch_monitoring_any_occasions_with_span_gap;
  pdsch_processing_type2_s_                       pdsch_processing_type2;
  pdsch_processing_type2_limited_s_               pdsch_processing_type2_limited;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetDownlink-v15a0 ::= SEQUENCE
struct feature_set_dl_v15a0_s {
  bool               supported_srs_res_features_present = false;
  srs_res_features_s supported_srs_res_features;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqSeparationClassDL-Only-r16 ::= ENUMERATED
struct freq_separation_class_dl_only_r16_opts {
  enum options { mhz200, mhz400, mhz600, mhz800, mhz1000, mhz1200, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using freq_separation_class_dl_only_r16_e = enumerated<freq_separation_class_dl_only_r16_opts>;

// FreqSeparationClassDL-v1620 ::= ENUMERATED
struct freq_separation_class_dl_v1620_opts {
  enum options { mhz1000, mhz1600, mhz1800, mhz2000, mhz2200, mhz2400, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using freq_separation_class_dl_v1620_e = enumerated<freq_separation_class_dl_v1620_opts>;

// PDCCH-MonitoringOccasions-r16 ::= SEQUENCE
struct pdcch_monitoring_occasions_r16_s {
  bool period7span3_r16_present = false;
  bool period4span3_r16_present = false;
  bool period2span2_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetDownlink-v1610 ::= SEQUENCE
struct feature_set_dl_v1610_s {
  struct cbg_pdsch_processing_type1_different_tb_per_slot_r16_s_ {
    struct scs_15k_hz_r16_opts {
      enum options { one, upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_15k_hz_r16_e_ = enumerated<scs_15k_hz_r16_opts>;
    struct scs_30k_hz_r16_opts {
      enum options { one, upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_30k_hz_r16_e_ = enumerated<scs_30k_hz_r16_opts>;
    struct scs_60k_hz_r16_opts {
      enum options { one, upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_60k_hz_r16_e_ = enumerated<scs_60k_hz_r16_opts>;
    struct scs_120k_hz_r16_opts {
      enum options { one, upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_120k_hz_r16_e_ = enumerated<scs_120k_hz_r16_opts>;

    // member variables
    bool               scs_15k_hz_r16_present  = false;
    bool               scs_30k_hz_r16_present  = false;
    bool               scs_60k_hz_r16_present  = false;
    bool               scs_120k_hz_r16_present = false;
    scs_15k_hz_r16_e_  scs_15k_hz_r16;
    scs_30k_hz_r16_e_  scs_30k_hz_r16;
    scs_60k_hz_r16_e_  scs_60k_hz_r16;
    scs_120k_hz_r16_e_ scs_120k_hz_r16;
  };
  struct cbg_pdsch_processing_type2_different_tb_per_slot_r16_s_ {
    struct scs_15k_hz_r16_opts {
      enum options { one, upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_15k_hz_r16_e_ = enumerated<scs_15k_hz_r16_opts>;
    struct scs_30k_hz_r16_opts {
      enum options { one, upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_30k_hz_r16_e_ = enumerated<scs_30k_hz_r16_opts>;
    struct scs_60k_hz_r16_opts {
      enum options { one, upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_60k_hz_r16_e_ = enumerated<scs_60k_hz_r16_opts>;
    struct scs_120k_hz_r16_opts {
      enum options { one, upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_120k_hz_r16_e_ = enumerated<scs_120k_hz_r16_opts>;

    // member variables
    bool               scs_15k_hz_r16_present  = false;
    bool               scs_30k_hz_r16_present  = false;
    bool               scs_60k_hz_r16_present  = false;
    bool               scs_120k_hz_r16_present = false;
    scs_15k_hz_r16_e_  scs_15k_hz_r16;
    scs_30k_hz_r16_e_  scs_30k_hz_r16;
    scs_60k_hz_r16_e_  scs_60k_hz_r16;
    scs_120k_hz_r16_e_ scs_120k_hz_r16;
  };
  struct intra_freq_daps_r16_s_ {
    bool intra_freq_diff_scs_daps_r16_present = false;
    bool intra_freq_async_daps_r16_present    = false;
  };
  struct pdcch_monitoring_r16_s_ {
    struct pdsch_processing_type1_r16_s_ {
      bool                             scs_15k_hz_r16_present = false;
      bool                             scs_30k_hz_r16_present = false;
      pdcch_monitoring_occasions_r16_s scs_15k_hz_r16;
      pdcch_monitoring_occasions_r16_s scs_30k_hz_r16;
    };
    struct pdsch_processing_type2_r16_s_ {
      bool                             scs_15k_hz_r16_present = false;
      bool                             scs_30k_hz_r16_present = false;
      pdcch_monitoring_occasions_r16_s scs_15k_hz_r16;
      pdcch_monitoring_occasions_r16_s scs_30k_hz_r16;
    };

    // member variables
    bool                          pdsch_processing_type1_r16_present = false;
    bool                          pdsch_processing_type2_r16_present = false;
    pdsch_processing_type1_r16_s_ pdsch_processing_type1_r16;
    pdsch_processing_type2_r16_s_ pdsch_processing_type2_r16;
  };
  struct cross_carrier_sched_processing_diff_scs_r16_s_ {
    struct scs_15k_hz_120k_hz_r16_opts {
      enum options { n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_15k_hz_120k_hz_r16_e_ = enumerated<scs_15k_hz_120k_hz_r16_opts>;
    struct scs_15k_hz_60k_hz_r16_opts {
      enum options { n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_15k_hz_60k_hz_r16_e_ = enumerated<scs_15k_hz_60k_hz_r16_opts>;
    struct scs_30k_hz_120k_hz_r16_opts {
      enum options { n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_30k_hz_120k_hz_r16_e_ = enumerated<scs_30k_hz_120k_hz_r16_opts>;

    // member variables
    bool                      scs_15k_hz_120k_hz_r16_present = false;
    bool                      scs_15k_hz_60k_hz_r16_present  = false;
    bool                      scs_30k_hz_120k_hz_r16_present = false;
    bool                      scs_15k_hz_30k_hz_r16_present  = false;
    bool                      scs_30k_hz_60k_hz_r16_present  = false;
    bool                      scs_60k_hz_120k_hz_r16_present = false;
    scs_15k_hz_120k_hz_r16_e_ scs_15k_hz_120k_hz_r16;
    scs_15k_hz_60k_hz_r16_e_  scs_15k_hz_60k_hz_r16;
    scs_30k_hz_120k_hz_r16_e_ scs_30k_hz_120k_hz_r16;
  };

  // member variables
  bool cbg_pdsch_processing_type1_different_tb_per_slot_r16_present = false;
  bool cbg_pdsch_processing_type2_different_tb_per_slot_r16_present = false;
  bool intra_freq_daps_r16_present                                  = false;
  bool intra_band_freq_separation_dl_v1620_present                  = false;
  bool intra_band_freq_separation_dl_only_r16_present               = false;
  bool pdcch_monitoring_r16_present                                 = false;
  bool pdcch_monitoring_mixed_r16_present                           = false;
  bool cross_carrier_sched_processing_diff_scs_r16_present          = false;
  bool single_dci_sdm_scheme_r16_present                            = false;
  cbg_pdsch_processing_type1_different_tb_per_slot_r16_s_ cbg_pdsch_processing_type1_different_tb_per_slot_r16;
  cbg_pdsch_processing_type2_different_tb_per_slot_r16_s_ cbg_pdsch_processing_type2_different_tb_per_slot_r16;
  intra_freq_daps_r16_s_                                  intra_freq_daps_r16;
  freq_separation_class_dl_v1620_e                        intra_band_freq_separation_dl_v1620;
  freq_separation_class_dl_only_r16_e                     intra_band_freq_separation_dl_only_r16;
  pdcch_monitoring_r16_s_                                 pdcch_monitoring_r16;
  cross_carrier_sched_processing_diff_scs_r16_s_          cross_carrier_sched_processing_diff_scs_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-RepetitionParameters-r17 ::= SEQUENCE
struct pdcch_repeat_params_r17_s {
  struct supported_mode_r17_opts {
    enum options { intra_span, inter_span, both, nulltype } value;

    const char* to_string() const;
  };
  using supported_mode_r17_e_ = enumerated<supported_mode_r17_opts>;
  struct limit_x_per_cc_r17_opts {
    enum options { n4, n8, n16, n32, n44, n64, nolimit, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using limit_x_per_cc_r17_e_ = enumerated<limit_x_per_cc_r17_opts>;
  struct limit_x_across_cc_r17_opts {
    enum options { n4, n8, n16, n32, n44, n64, n128, n256, n512, nolimit, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using limit_x_across_cc_r17_e_ = enumerated<limit_x_across_cc_r17_opts>;

  // member variables
  bool                     limit_x_per_cc_r17_present    = false;
  bool                     limit_x_across_cc_r17_present = false;
  supported_mode_r17_e_    supported_mode_r17;
  limit_x_per_cc_r17_e_    limit_x_per_cc_r17;
  limit_x_across_cc_r17_e_ limit_x_across_cc_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetDownlink-v1700 ::= SEQUENCE
struct feature_set_dl_v1700_s {
  struct scaling_factor_1024_qam_fr1_r17_opts {
    enum options { f0p4, f0p75, f0p8, nulltype } value;

    const char* to_string() const;
  };
  using scaling_factor_1024_qam_fr1_r17_e_ = enumerated<scaling_factor_1024_qam_fr1_r17_opts>;
  struct time_dur_for_qcl_v1710_s_ {
    struct scs_480k_hz_opts {
      enum options { s56, s112, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_480k_hz_e_ = enumerated<scs_480k_hz_opts>;
    struct scs_960k_hz_opts {
      enum options { s112, s224, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_960k_hz_e_ = enumerated<scs_960k_hz_opts>;

    // member variables
    bool           scs_480k_hz_present = false;
    bool           scs_960k_hz_present = false;
    scs_480k_hz_e_ scs_480k_hz;
    scs_960k_hz_e_ scs_960k_hz;
  };
  struct mtrp_pdcch_case2_1_span_gap_r17_s_ {
    bool                      scs_15k_hz_r17_present  = false;
    bool                      scs_30k_hz_r17_present  = false;
    bool                      scs_60k_hz_r17_present  = false;
    bool                      scs_120k_hz_r17_present = false;
    pdcch_repeat_params_r17_s scs_15k_hz_r17;
    pdcch_repeat_params_r17_s scs_30k_hz_r17;
    pdcch_repeat_params_r17_s scs_60k_hz_r17;
    pdcch_repeat_params_r17_s scs_120k_hz_r17;
  };
  struct mtrp_pdcch_legacy_monitoring_r17_s_ {
    bool                      scs_15k_hz_r17_present = false;
    bool                      scs_30k_hz_r17_present = false;
    pdcch_repeat_params_r17_s scs_15k_hz_r17;
    pdcch_repeat_params_r17_s scs_30k_hz_r17;
  };
  struct mtrp_pdcch_repeat_r17_s_ {
    struct max_num_overlaps_r17_opts {
      enum options { n1, n2, n3, n5, n10, n20, n40, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_overlaps_r17_e_ = enumerated<max_num_overlaps_r17_opts>;

    // member variables
    uint8_t                 num_bd_two_pdcch_r17 = 2;
    max_num_overlaps_r17_e_ max_num_overlaps_r17;
  };

  // member variables
  bool                                scaling_factor_1024_qam_fr1_r17_present    = false;
  bool                                time_dur_for_qcl_v1710_present             = false;
  bool                                sfn_scheme_a_r17_present                   = false;
  bool                                sfn_scheme_a_pdcch_only_r17_present        = false;
  bool                                sfn_scheme_a_dyn_switching_r17_present     = false;
  bool                                sfn_scheme_a_pdsch_only_r17_present        = false;
  bool                                sfn_scheme_b_r17_present                   = false;
  bool                                sfn_scheme_b_dyn_switching_r17_present     = false;
  bool                                sfn_scheme_b_pdsch_only_r17_present        = false;
  bool                                mtrp_pdcch_case2_1_span_gap_r17_present    = false;
  bool                                mtrp_pdcch_legacy_monitoring_r17_present   = false;
  bool                                mtrp_pdcch_multi_dci_multi_trp_r17_present = false;
  bool                                dyn_multicast_pcell_r17_present            = false;
  bool                                mtrp_pdcch_repeat_r17_present              = false;
  scaling_factor_1024_qam_fr1_r17_e_  scaling_factor_1024_qam_fr1_r17;
  time_dur_for_qcl_v1710_s_           time_dur_for_qcl_v1710;
  mtrp_pdcch_case2_1_span_gap_r17_s_  mtrp_pdcch_case2_1_span_gap_r17;
  mtrp_pdcch_legacy_monitoring_r17_s_ mtrp_pdcch_legacy_monitoring_r17;
  mtrp_pdcch_repeat_r17_s_            mtrp_pdcch_repeat_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ModulationOrder ::= ENUMERATED
struct mod_order_opts {
  enum options { bpsk_halfpi, bpsk, qpsk, qam16, qam64, qam256, nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
using mod_order_e = enumerated<mod_order_opts>;

// SupportedBandwidth ::= CHOICE
struct supported_bw_c {
  struct fr1_opts {
    enum options { mhz5, mhz10, mhz15, mhz20, mhz25, mhz30, mhz40, mhz50, mhz60, mhz80, mhz100, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using fr1_e_ = enumerated<fr1_opts>;
  struct fr2_opts {
    enum options { mhz50, mhz100, mhz200, mhz400, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using fr2_e_ = enumerated<fr2_opts>;
  struct types_opts {
    enum options { fr1, fr2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  supported_bw_c() = default;
  supported_bw_c(const supported_bw_c& other);
  supported_bw_c& operator=(const supported_bw_c& other);
  ~supported_bw_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fr1_e_& fr1()
  {
    assert_choice_type(types::fr1, type_, "SupportedBandwidth");
    return c.get<fr1_e_>();
  }
  fr2_e_& fr2()
  {
    assert_choice_type(types::fr2, type_, "SupportedBandwidth");
    return c.get<fr2_e_>();
  }
  const fr1_e_& fr1() const
  {
    assert_choice_type(types::fr1, type_, "SupportedBandwidth");
    return c.get<fr1_e_>();
  }
  const fr2_e_& fr2() const
  {
    assert_choice_type(types::fr2, type_, "SupportedBandwidth");
    return c.get<fr2_e_>();
  }
  fr1_e_& set_fr1();
  fr2_e_& set_fr2();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// FeatureSetDownlinkPerCC ::= SEQUENCE
struct feature_set_dl_per_cc_s {
  bool                 ch_bw_90mhz_present               = false;
  bool                 max_num_mimo_layers_pdsch_present = false;
  bool                 supported_mod_order_dl_present    = false;
  subcarrier_spacing_e supported_subcarrier_spacing_dl;
  supported_bw_c       supported_bw_dl;
  mimo_layers_dl_e     max_num_mimo_layers_pdsch;
  mod_order_e          supported_mod_order_dl;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MultiDCI-MultiTRP-r16 ::= SEQUENCE
struct multi_dci_multi_trp_r16_s {
  struct max_num_coreset_r16_opts {
    enum options { n2, n3, n4, n5, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_coreset_r16_e_ = enumerated<max_num_coreset_r16_opts>;
  struct max_num_unicast_pdsch_per_pool_r16_opts {
    enum options { n1, n2, n3, n4, n7, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_unicast_pdsch_per_pool_r16_e_ = enumerated<max_num_unicast_pdsch_per_pool_r16_opts>;

  // member variables
  max_num_coreset_r16_e_                max_num_coreset_r16;
  uint8_t                               max_num_coreset_per_pool_idx_r16 = 1;
  max_num_unicast_pdsch_per_pool_r16_e_ max_num_unicast_pdsch_per_pool_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetDownlinkPerCC-v1620 ::= SEQUENCE
struct feature_set_dl_per_cc_v1620_s {
  bool                      multi_dci_multi_trp_r16_present  = false;
  bool                      support_fdm_scheme_b_r16_present = false;
  multi_dci_multi_trp_r16_s multi_dci_multi_trp_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CRS-InterfMitigation-r17 ::= SEQUENCE
struct crs_interf_mitigation_r17_s {
  bool crs_im_dss_15k_hz_scs_r17_present         = false;
  bool crs_im_non_dss_15k_hz_scs_r17_present     = false;
  bool crs_im_non_dss_nwa_15k_hz_scs_r17_present = false;
  bool crs_im_non_dss_30k_hz_scs_r17_present     = false;
  bool crs_im_non_dss_nwa_30k_hz_scs_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandwidth-v1700 ::= CHOICE
struct supported_bw_v1700_c {
  struct fr1_r17_opts {
    enum options {
      mhz5,
      mhz10,
      mhz15,
      mhz20,
      mhz25,
      mhz30,
      mhz35,
      mhz40,
      mhz45,
      mhz50,
      mhz60,
      mhz70,
      mhz80,
      mhz90,
      mhz100,
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using fr1_r17_e_ = enumerated<fr1_r17_opts>;
  struct fr2_r17_opts {
    enum options { mhz50, mhz100, mhz200, mhz400, mhz800, mhz1600, mhz2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using fr2_r17_e_ = enumerated<fr2_r17_opts>;
  struct types_opts {
    enum options { fr1_r17, fr2_r17, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  supported_bw_v1700_c() = default;
  supported_bw_v1700_c(const supported_bw_v1700_c& other);
  supported_bw_v1700_c& operator=(const supported_bw_v1700_c& other);
  ~supported_bw_v1700_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fr1_r17_e_& fr1_r17()
  {
    assert_choice_type(types::fr1_r17, type_, "SupportedBandwidth-v1700");
    return c.get<fr1_r17_e_>();
  }
  fr2_r17_e_& fr2_r17()
  {
    assert_choice_type(types::fr2_r17, type_, "SupportedBandwidth-v1700");
    return c.get<fr2_r17_e_>();
  }
  const fr1_r17_e_& fr1_r17() const
  {
    assert_choice_type(types::fr1_r17, type_, "SupportedBandwidth-v1700");
    return c.get<fr1_r17_e_>();
  }
  const fr2_r17_e_& fr2_r17() const
  {
    assert_choice_type(types::fr2_r17, type_, "SupportedBandwidth-v1700");
    return c.get<fr2_r17_e_>();
  }
  fr1_r17_e_& set_fr1_r17();
  fr2_r17_e_& set_fr2_r17();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// FeatureSetDownlinkPerCC-v1700 ::= SEQUENCE
struct feature_set_dl_per_cc_v1700_s {
  struct max_num_mimo_layers_multicast_pdsch_r17_opts {
    enum options { n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_mimo_layers_multicast_pdsch_r17_e_ = enumerated<max_num_mimo_layers_multicast_pdsch_r17_opts>;

  // member variables
  bool                                       supported_min_bw_dl_r17_present                 = false;
  bool                                       broadcast_scell_r17_present                     = false;
  bool                                       max_num_mimo_layers_multicast_pdsch_r17_present = false;
  bool                                       dyn_multicast_scell_r17_present                 = false;
  bool                                       supported_bw_dl_v1710_present                   = false;
  bool                                       supported_crs_interf_mitigation_r17_present     = false;
  supported_bw_v1700_c                       supported_min_bw_dl_r17;
  max_num_mimo_layers_multicast_pdsch_r17_e_ max_num_mimo_layers_multicast_pdsch_r17;
  supported_bw_v1700_c                       supported_bw_dl_v1710;
  crs_interf_mitigation_r17_s                supported_crs_interf_mitigation_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DummyF ::= SEQUENCE
struct dummy_f_s {
  uint8_t max_num_periodic_csi_report_per_bwp        = 1;
  uint8_t max_num_aperiodic_csi_report_per_bwp       = 1;
  uint8_t max_num_semi_persistent_csi_report_per_bwp = 0;
  uint8_t simul_csi_reports_all_cc                   = 5;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DummyI ::= SEQUENCE
struct dummy_i_s {
  struct supported_srs_tx_port_switch_opts {
    enum options { t1r2, t1r4, t2r4, t1r4_t2r4, tr_equal, nulltype } value;

    const char* to_string() const;
  };
  using supported_srs_tx_port_switch_e_ = enumerated<supported_srs_tx_port_switch_opts>;

  // member variables
  bool                            tx_switch_impact_to_rx_present = false;
  supported_srs_tx_port_switch_e_ supported_srs_tx_port_switch;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetUplink ::= SEQUENCE
struct feature_set_ul_s {
  using feature_set_list_per_ul_cc_l_ = bounded_array<uint16_t, 32>;
  struct scaling_factor_opts {
    enum options { f0p4, f0p75, f0p8, nulltype } value;

    const char* to_string() const;
  };
  using scaling_factor_e_ = enumerated<scaling_factor_opts>;
  struct pusch_processing_type1_different_tb_per_slot_s_ {
    struct scs_15k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_15k_hz_e_ = enumerated<scs_15k_hz_opts>;
    struct scs_30k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_30k_hz_e_ = enumerated<scs_30k_hz_opts>;
    struct scs_60k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_60k_hz_e_ = enumerated<scs_60k_hz_opts>;
    struct scs_120k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_120k_hz_e_ = enumerated<scs_120k_hz_opts>;

    // member variables
    bool           scs_15k_hz_present  = false;
    bool           scs_30k_hz_present  = false;
    bool           scs_60k_hz_present  = false;
    bool           scs_120k_hz_present = false;
    scs_15k_hz_e_  scs_15k_hz;
    scs_30k_hz_e_  scs_30k_hz;
    scs_60k_hz_e_  scs_60k_hz;
    scs_120k_hz_e_ scs_120k_hz;
  };

  // member variables
  bool                                            scaling_factor_present                               = false;
  bool                                            dummy3_present                                       = false;
  bool                                            intra_band_freq_separation_ul_present                = false;
  bool                                            search_space_sharing_ca_ul_present                   = false;
  bool                                            dummy1_present                                       = false;
  bool                                            supported_srs_res_features_present                   = false;
  bool                                            two_pucch_group_present                              = false;
  bool                                            dyn_switch_sul_present                               = false;
  bool                                            simul_tx_sul_non_sul_present                         = false;
  bool                                            pusch_processing_type1_different_tb_per_slot_present = false;
  bool                                            dummy2_present                                       = false;
  feature_set_list_per_ul_cc_l_                   feature_set_list_per_ul_cc;
  scaling_factor_e_                               scaling_factor;
  freq_separation_class_e                         intra_band_freq_separation_ul;
  dummy_i_s                                       dummy1;
  srs_res_features_s                              supported_srs_res_features;
  pusch_processing_type1_different_tb_per_slot_s_ pusch_processing_type1_different_tb_per_slot;
  dummy_f_s                                       dummy2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetUplink-v1540 ::= SEQUENCE
struct feature_set_ul_v1540_s {
  struct pusch_processing_type2_s_ {
    bool                scs_15k_hz_present = false;
    bool                scs_30k_hz_present = false;
    bool                scs_60k_hz_present = false;
    processing_params_s scs_15k_hz;
    processing_params_s scs_30k_hz;
    processing_params_s scs_60k_hz;
  };

  // member variables
  bool                      zero_slot_offset_aperiodic_srs_present = false;
  bool                      pa_phase_discontinuity_impacts_present = false;
  bool                      pusch_separation_with_gap_present      = false;
  bool                      pusch_processing_type2_present         = false;
  bool                      ul_mcs_table_alt_dyn_ind_present       = false;
  pusch_processing_type2_s_ pusch_processing_type2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetUplink-v1610 ::= SEQUENCE
struct feature_set_ul_v1610_s {
  struct pusch_repeat_type_b_r16_s_ {
    struct max_num_pusch_tx_r16_opts {
      enum options { n2, n3, n4, n7, n8, n12, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_pusch_tx_r16_e_ = enumerated<max_num_pusch_tx_r16_opts>;
    struct hop_scheme_r16_opts {
      enum options { inter_slot_hop, inter_repeat_hop, both, nulltype } value;

      const char* to_string() const;
    };
    using hop_scheme_r16_e_ = enumerated<hop_scheme_r16_opts>;

    // member variables
    max_num_pusch_tx_r16_e_ max_num_pusch_tx_r16;
    hop_scheme_r16_e_       hop_scheme_r16;
  };
  struct ul_full_pwr_mode2_max_srs_res_in_set_r16_opts {
    enum options { n1, n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ul_full_pwr_mode2_max_srs_res_in_set_r16_e_ = enumerated<ul_full_pwr_mode2_max_srs_res_in_set_r16_opts>;
  struct cbg_pusch_processing_type1_different_tb_per_slot_r16_s_ {
    struct scs_15k_hz_r16_opts {
      enum options { one_pusch, upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_15k_hz_r16_e_ = enumerated<scs_15k_hz_r16_opts>;
    struct scs_30k_hz_r16_opts {
      enum options { one_pusch, upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_30k_hz_r16_e_ = enumerated<scs_30k_hz_r16_opts>;
    struct scs_60k_hz_r16_opts {
      enum options { one_pusch, upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_60k_hz_r16_e_ = enumerated<scs_60k_hz_r16_opts>;
    struct scs_120k_hz_r16_opts {
      enum options { one_pusch, upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_120k_hz_r16_e_ = enumerated<scs_120k_hz_r16_opts>;

    // member variables
    bool               scs_15k_hz_r16_present  = false;
    bool               scs_30k_hz_r16_present  = false;
    bool               scs_60k_hz_r16_present  = false;
    bool               scs_120k_hz_r16_present = false;
    scs_15k_hz_r16_e_  scs_15k_hz_r16;
    scs_30k_hz_r16_e_  scs_30k_hz_r16;
    scs_60k_hz_r16_e_  scs_60k_hz_r16;
    scs_120k_hz_r16_e_ scs_120k_hz_r16;
  };
  struct cbg_pusch_processing_type2_different_tb_per_slot_r16_s_ {
    struct scs_15k_hz_r16_opts {
      enum options { one_pusch, upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_15k_hz_r16_e_ = enumerated<scs_15k_hz_r16_opts>;
    struct scs_30k_hz_r16_opts {
      enum options { one_pusch, upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_30k_hz_r16_e_ = enumerated<scs_30k_hz_r16_opts>;
    struct scs_60k_hz_r16_opts {
      enum options { one_pusch, upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_60k_hz_r16_e_ = enumerated<scs_60k_hz_r16_opts>;
    struct scs_120k_hz_r16_opts {
      enum options { one_pusch, upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_120k_hz_r16_e_ = enumerated<scs_120k_hz_r16_opts>;

    // member variables
    bool               scs_15k_hz_r16_present  = false;
    bool               scs_30k_hz_r16_present  = false;
    bool               scs_60k_hz_r16_present  = false;
    bool               scs_120k_hz_r16_present = false;
    scs_15k_hz_r16_e_  scs_15k_hz_r16;
    scs_30k_hz_r16_e_  scs_30k_hz_r16;
    scs_60k_hz_r16_e_  scs_60k_hz_r16;
    scs_120k_hz_r16_e_ scs_120k_hz_r16;
  };
  struct intra_freq_daps_ul_r16_s_ {
    struct dummy3_opts {
      enum options { short_value, long_value, nulltype } value;

      const char* to_string() const;
    };
    using dummy3_e_ = enumerated<dummy3_opts>;

    // member variables
    bool      dummy_present                        = false;
    bool      intra_freq_two_tags_daps_r16_present = false;
    bool      dummy1_present                       = false;
    bool      dummy2_present                       = false;
    bool      dummy3_present                       = false;
    dummy3_e_ dummy3;
  };
  struct multi_pucch_r16_s_ {
    struct sub_slot_cfg_n_cp_r16_opts {
      enum options { set1, set2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using sub_slot_cfg_n_cp_r16_e_ = enumerated<sub_slot_cfg_n_cp_r16_opts>;
    struct sub_slot_cfg_e_cp_r16_opts {
      enum options { set1, set2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using sub_slot_cfg_e_cp_r16_e_ = enumerated<sub_slot_cfg_e_cp_r16_opts>;

    // member variables
    bool                     sub_slot_cfg_n_cp_r16_present = false;
    bool                     sub_slot_cfg_e_cp_r16_present = false;
    sub_slot_cfg_n_cp_r16_e_ sub_slot_cfg_n_cp_r16;
    sub_slot_cfg_e_cp_r16_e_ sub_slot_cfg_e_cp_r16;
  };
  struct ul_intra_ue_mux_r16_s_ {
    struct pusch_prep_low_prio_r16_opts {
      enum options { sym0, sym1, sym2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using pusch_prep_low_prio_r16_e_ = enumerated<pusch_prep_low_prio_r16_opts>;
    struct pusch_prep_high_prio_r16_opts {
      enum options { sym0, sym1, sym2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using pusch_prep_high_prio_r16_e_ = enumerated<pusch_prep_high_prio_r16_opts>;

    // member variables
    pusch_prep_low_prio_r16_e_  pusch_prep_low_prio_r16;
    pusch_prep_high_prio_r16_e_ pusch_prep_high_prio_r16;
  };
  struct cross_carrier_sched_processing_diff_scs_r16_s_ {
    struct scs_15k_hz_120k_hz_r16_opts {
      enum options { n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_15k_hz_120k_hz_r16_e_ = enumerated<scs_15k_hz_120k_hz_r16_opts>;
    struct scs_15k_hz_60k_hz_r16_opts {
      enum options { n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_15k_hz_60k_hz_r16_e_ = enumerated<scs_15k_hz_60k_hz_r16_opts>;
    struct scs_30k_hz_120k_hz_r16_opts {
      enum options { n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_30k_hz_120k_hz_r16_e_ = enumerated<scs_30k_hz_120k_hz_r16_opts>;

    // member variables
    bool                      scs_15k_hz_120k_hz_r16_present = false;
    bool                      scs_15k_hz_60k_hz_r16_present  = false;
    bool                      scs_30k_hz_120k_hz_r16_present = false;
    bool                      scs_15k_hz_30k_hz_r16_present  = false;
    bool                      scs_30k_hz_60k_hz_r16_present  = false;
    bool                      scs_60k_hz_120k_hz_r16_present = false;
    scs_15k_hz_120k_hz_r16_e_ scs_15k_hz_120k_hz_r16;
    scs_15k_hz_60k_hz_r16_e_  scs_15k_hz_60k_hz_r16;
    scs_30k_hz_120k_hz_r16_e_ scs_30k_hz_120k_hz_r16;
  };
  struct ul_full_pwr_mode2_srs_cfg_diff_num_srs_ports_r16_opts {
    enum options { p1_neg2, p1_neg4, p1_neg2_neg4, nulltype } value;

    const char* to_string() const;
  };
  using ul_full_pwr_mode2_srs_cfg_diff_num_srs_ports_r16_e_ =
      enumerated<ul_full_pwr_mode2_srs_cfg_diff_num_srs_ports_r16_opts>;
  struct ul_full_pwr_mode2_tpmi_group_r16_s_ {
    struct four_ports_non_coherent_r16_opts {
      enum options { g0, g1, g2, g3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using four_ports_non_coherent_r16_e_ = enumerated<four_ports_non_coherent_r16_opts>;
    struct four_ports_partial_coherent_r16_opts {
      enum options { g0, g1, g2, g3, g4, g5, g6, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using four_ports_partial_coherent_r16_e_ = enumerated<four_ports_partial_coherent_r16_opts>;

    // member variables
    bool                               two_ports_r16_present                   = false;
    bool                               four_ports_non_coherent_r16_present     = false;
    bool                               four_ports_partial_coherent_r16_present = false;
    fixed_bitstring<2>                 two_ports_r16;
    four_ports_non_coherent_r16_e_     four_ports_non_coherent_r16;
    four_ports_partial_coherent_r16_e_ four_ports_partial_coherent_r16;
  };

  // member variables
  bool                                        pusch_repeat_type_b_r16_present                              = false;
  bool                                        ul_cancellation_self_carrier_r16_present                     = false;
  bool                                        ul_cancellation_cross_carrier_r16_present                    = false;
  bool                                        ul_full_pwr_mode2_max_srs_res_in_set_r16_present             = false;
  bool                                        cbg_pusch_processing_type1_different_tb_per_slot_r16_present = false;
  bool                                        cbg_pusch_processing_type2_different_tb_per_slot_r16_present = false;
  bool                                        supported_srs_pos_res_r16_present                            = false;
  bool                                        intra_freq_daps_ul_r16_present                               = false;
  bool                                        intra_band_freq_separation_ul_v1620_present                  = false;
  bool                                        multi_pucch_r16_present                                      = false;
  bool                                        two_pucch_type1_r16_present                                  = false;
  bool                                        two_pucch_type2_r16_present                                  = false;
  bool                                        two_pucch_type3_r16_present                                  = false;
  bool                                        two_pucch_type4_r16_present                                  = false;
  bool                                        mux_sr_harq_ack_r16_present                                  = false;
  bool                                        dummy1_present                                               = false;
  bool                                        dummy2_present                                               = false;
  bool                                        two_pucch_type5_r16_present                                  = false;
  bool                                        two_pucch_type6_r16_present                                  = false;
  bool                                        two_pucch_type7_r16_present                                  = false;
  bool                                        two_pucch_type8_r16_present                                  = false;
  bool                                        two_pucch_type9_r16_present                                  = false;
  bool                                        two_pucch_type10_r16_present                                 = false;
  bool                                        two_pucch_type11_r16_present                                 = false;
  bool                                        ul_intra_ue_mux_r16_present                                  = false;
  bool                                        ul_full_pwr_mode_r16_present                                 = false;
  bool                                        cross_carrier_sched_processing_diff_scs_r16_present          = false;
  bool                                        ul_full_pwr_mode1_r16_present                                = false;
  bool                                        ul_full_pwr_mode2_srs_cfg_diff_num_srs_ports_r16_present     = false;
  bool                                        ul_full_pwr_mode2_tpmi_group_r16_present                     = false;
  pusch_repeat_type_b_r16_s_                  pusch_repeat_type_b_r16;
  ul_full_pwr_mode2_max_srs_res_in_set_r16_e_ ul_full_pwr_mode2_max_srs_res_in_set_r16;
  cbg_pusch_processing_type1_different_tb_per_slot_r16_s_ cbg_pusch_processing_type1_different_tb_per_slot_r16;
  cbg_pusch_processing_type2_different_tb_per_slot_r16_s_ cbg_pusch_processing_type2_different_tb_per_slot_r16;
  srs_all_pos_res_r16_s                                   supported_srs_pos_res_r16;
  intra_freq_daps_ul_r16_s_                               intra_freq_daps_ul_r16;
  multi_pucch_r16_s_                                      multi_pucch_r16;
  ul_intra_ue_mux_r16_s_                                  ul_intra_ue_mux_r16;
  cross_carrier_sched_processing_diff_scs_r16_s_          cross_carrier_sched_processing_diff_scs_r16;
  ul_full_pwr_mode2_srs_cfg_diff_num_srs_ports_r16_e_     ul_full_pwr_mode2_srs_cfg_diff_num_srs_ports_r16;
  ul_full_pwr_mode2_tpmi_group_r16_s_                     ul_full_pwr_mode2_tpmi_group_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SubSlot-Config-r16 ::= SEQUENCE
struct sub_slot_cfg_r16_s {
  struct sub_slot_cfg_n_cp_r16_opts {
    enum options { n4, n5, n6, n7, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using sub_slot_cfg_n_cp_r16_e_ = enumerated<sub_slot_cfg_n_cp_r16_opts>;
  struct sub_slot_cfg_e_cp_r16_opts {
    enum options { n4, n5, n6, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using sub_slot_cfg_e_cp_r16_e_ = enumerated<sub_slot_cfg_e_cp_r16_opts>;

  // member variables
  bool                     sub_slot_cfg_n_cp_r16_present = false;
  bool                     sub_slot_cfg_e_cp_r16_present = false;
  sub_slot_cfg_n_cp_r16_e_ sub_slot_cfg_n_cp_r16;
  sub_slot_cfg_e_cp_r16_e_ sub_slot_cfg_e_cp_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetUplink-v1640 ::= SEQUENCE
struct feature_set_ul_v1640_s {
  struct offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_ {
    struct scs_15k_hz_r16_opts {
      enum options { set1, set2, set3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_15k_hz_r16_e_ = enumerated<scs_15k_hz_r16_opts>;
    struct scs_30k_hz_r16_opts {
      enum options { set1, set2, set3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_30k_hz_r16_e_ = enumerated<scs_30k_hz_r16_opts>;
    struct scs_60k_hz_r16_opts {
      enum options { set1, set2, set3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using scs_60k_hz_r16_e_ = enumerated<scs_60k_hz_r16_opts>;

    // member variables
    bool              scs_15k_hz_r16_present = false;
    bool              scs_30k_hz_r16_present = false;
    bool              scs_60k_hz_r16_present = false;
    scs_15k_hz_r16_e_ scs_15k_hz_r16;
    scs_30k_hz_r16_e_ scs_30k_hz_r16;
    scs_60k_hz_r16_e_ scs_60k_hz_r16;
  };

  // member variables
  bool               two_harq_ack_codebook_type1_r16_present                                 = false;
  bool               two_harq_ack_codebook_type2_r16_present                                 = false;
  bool               offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_present = false;
  sub_slot_cfg_r16_s two_harq_ack_codebook_type1_r16;
  sub_slot_cfg_r16_s two_harq_ack_codebook_type2_r16;
  offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_
      offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetUplinkPerCC ::= SEQUENCE
struct feature_set_ul_per_cc_s {
  struct mimo_cb_pusch_s_ {
    bool             max_num_mimo_layers_cb_pusch_present = false;
    mimo_layers_ul_e max_num_mimo_layers_cb_pusch;
    uint8_t          max_num_srs_res_per_set = 1;
  };

  // member variables
  bool                 ch_bw_90mhz_present                      = false;
  bool                 mimo_cb_pusch_present                    = false;
  bool                 max_num_mimo_layers_non_cb_pusch_present = false;
  bool                 supported_mod_order_ul_present           = false;
  subcarrier_spacing_e supported_subcarrier_spacing_ul;
  supported_bw_c       supported_bw_ul;
  mimo_cb_pusch_s_     mimo_cb_pusch;
  mimo_layers_ul_e     max_num_mimo_layers_non_cb_pusch;
  mod_order_e          supported_mod_order_ul;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetUplinkPerCC-v1700 ::= SEQUENCE
struct feature_set_ul_per_cc_v1700_s {
  struct mtrp_pusch_repeat_type_b_r17_opts {
    enum options { n1, n2, n3, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mtrp_pusch_repeat_type_b_r17_e_ = enumerated<mtrp_pusch_repeat_type_b_r17_opts>;
  struct mtrp_pusch_type_b_cb_r17_opts {
    enum options { n1, n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mtrp_pusch_type_b_cb_r17_e_ = enumerated<mtrp_pusch_type_b_cb_r17_opts>;

  // member variables
  bool                            supported_min_bw_ul_r17_present      = false;
  bool                            mtrp_pusch_repeat_type_b_r17_present = false;
  bool                            mtrp_pusch_type_b_cb_r17_present     = false;
  bool                            supported_bw_ul_v1710_present        = false;
  supported_bw_v1700_c            supported_min_bw_ul_r17;
  mtrp_pusch_repeat_type_b_r17_e_ mtrp_pusch_repeat_type_b_r17;
  mtrp_pusch_type_b_cb_r17_e_     mtrp_pusch_type_b_cb_r17;
  supported_bw_v1700_c            supported_bw_ul_v1710;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetDownlink-v1720 ::= SEQUENCE
struct feature_set_dl_v1720_s {
  struct rtt_based_pdc_prs_r17_s_ {
    struct max_num_prs_res_r17_opts {
      enum options { n1, n2, n4, n8, n16, n32, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_prs_res_r17_e_ = enumerated<max_num_prs_res_r17_opts>;
    struct max_num_prs_res_processed_per_slot_r17_s_ {
      struct scs_15k_hz_r17_opts {
        enum options { n1, n2, n4, n6, n8, n12, n16, n24, n32, n48, n64, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using scs_15k_hz_r17_e_ = enumerated<scs_15k_hz_r17_opts>;
      struct scs_30k_hz_r17_opts {
        enum options { n1, n2, n4, n6, n8, n12, n16, n24, n32, n48, n64, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using scs_30k_hz_r17_e_ = enumerated<scs_30k_hz_r17_opts>;
      struct scs_60k_hz_r17_opts {
        enum options { n1, n2, n4, n6, n8, n12, n16, n24, n32, n48, n64, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using scs_60k_hz_r17_e_ = enumerated<scs_60k_hz_r17_opts>;
      struct scs_120k_hz_r17_opts {
        enum options { n1, n2, n4, n6, n8, n12, n16, n24, n32, n48, n64, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using scs_120k_hz_r17_e_ = enumerated<scs_120k_hz_r17_opts>;

      // member variables
      bool               scs_15k_hz_r17_present  = false;
      bool               scs_30k_hz_r17_present  = false;
      bool               scs_60k_hz_r17_present  = false;
      bool               scs_120k_hz_r17_present = false;
      scs_15k_hz_r17_e_  scs_15k_hz_r17;
      scs_30k_hz_r17_e_  scs_30k_hz_r17;
      scs_60k_hz_r17_e_  scs_60k_hz_r17;
      scs_120k_hz_r17_e_ scs_120k_hz_r17;
    };

    // member variables
    max_num_prs_res_r17_e_                    max_num_prs_res_r17;
    max_num_prs_res_processed_per_slot_r17_s_ max_num_prs_res_processed_per_slot_r17;
  };

  // member variables
  bool                     rtt_based_pdc_csi_rs_for_tracking_r17_present = false;
  bool                     rtt_based_pdc_prs_r17_present                 = false;
  bool                     sps_multicast_r17_present                     = false;
  rtt_based_pdc_prs_r17_s_ rtt_based_pdc_prs_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetDownlink-v1730 ::= SEQUENCE
struct feature_set_dl_v1730_s {
  bool prs_as_spatial_relation_rs_for_srs_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetDownlinkPerCC-v1720 ::= SEQUENCE
struct feature_set_dl_per_cc_v1720_s {
  struct max_mod_order_for_multicast_data_rate_calculation_r17_opts {
    enum options { qam64, qam256, qam1024, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using max_mod_order_for_multicast_data_rate_calculation_r17_e_ =
      enumerated<max_mod_order_for_multicast_data_rate_calculation_r17_opts>;

  // member variables
  bool max_mod_order_for_multicast_data_rate_calculation_r17_present = false;
  bool fdm_broadcast_unicast_r17_present                             = false;
  bool fdm_multicast_unicast_r17_present                             = false;
  max_mod_order_for_multicast_data_rate_calculation_r17_e_ max_mod_order_for_multicast_data_rate_calculation_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetDownlinkPerCC-v1730 ::= SEQUENCE
struct feature_set_dl_per_cc_v1730_s {
  struct intra_slot_tdm_unicast_group_common_pdsch_r17_opts {
    enum options { yes, no, nulltype } value;

    const char* to_string() const;
  };
  using intra_slot_tdm_unicast_group_common_pdsch_r17_e_ =
      enumerated<intra_slot_tdm_unicast_group_common_pdsch_r17_opts>;

  // member variables
  bool                                             intra_slot_tdm_unicast_group_common_pdsch_r17_present = false;
  bool                                             sps_multicast_scell_r17_present                       = false;
  bool                                             sps_multicast_scell_multi_cfg_r17_present             = false;
  bool                                             dci_broadcast_with16_repeats_r17_present              = false;
  intra_slot_tdm_unicast_group_common_pdsch_r17_e_ intra_slot_tdm_unicast_group_common_pdsch_r17;
  uint8_t                                          sps_multicast_scell_multi_cfg_r17 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetUplink-v1630 ::= SEQUENCE
struct feature_set_ul_v1630_s {
  bool offset_srs_cb_pusch_ant_switch_fr1_r16_present                        = false;
  bool offset_srs_cb_pusch_pdcch_monitor_single_occ_fr1_r16_present          = false;
  bool offset_srs_cb_pusch_pdcch_monitor_any_occ_without_gap_fr1_r16_present = false;
  bool offset_srs_cb_pusch_pdcch_monitor_any_occ_with_gap_fr1_r16_present    = false;
  bool dummy_present                                                         = false;
  bool partial_cancellation_pucch_pusch_prach_tx_r16_present                 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetUplink-v1710 ::= SEQUENCE
struct feature_set_ul_v1710_s {
  struct mtrp_pusch_type_a_cb_r17_opts {
    enum options { n1, n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mtrp_pusch_type_a_cb_r17_e_ = enumerated<mtrp_pusch_type_a_cb_r17_opts>;
  struct mtrp_pusch_repeat_type_a_r17_opts {
    enum options { n1, n2, n3, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mtrp_pusch_repeat_type_a_r17_e_ = enumerated<mtrp_pusch_repeat_type_a_r17_opts>;
  struct mtrp_pucch_intra_slot_r17_opts {
    enum options { pf0_neg2, pf1_neg3_neg4, pf0_neg4, nulltype } value;

    const char* to_string() const;
  };
  using mtrp_pucch_intra_slot_r17_e_ = enumerated<mtrp_pucch_intra_slot_r17_opts>;
  struct ue_pwr_class_per_band_per_bc_r17_opts {
    enum options { pc1dot5, pc2, pc3, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using ue_pwr_class_per_band_per_bc_r17_e_ = enumerated<ue_pwr_class_per_band_per_bc_r17_opts>;

  // member variables
  bool                                mtrp_pusch_type_a_cb_r17_present             = false;
  bool                                mtrp_pusch_repeat_type_a_r17_present         = false;
  bool                                mtrp_pucch_intra_slot_r17_present            = false;
  bool                                srs_ant_switching2_sp_1_periodic_r17_present = false;
  bool                                srs_ext_aperiodic_srs_r17_present            = false;
  bool                                srs_one_ap_srs_r17_present                   = false;
  bool                                ue_pwr_class_per_band_per_bc_r17_present     = false;
  bool                                tx_support_ul_gap_fr2_r17_present            = false;
  mtrp_pusch_type_a_cb_r17_e_         mtrp_pusch_type_a_cb_r17;
  mtrp_pusch_repeat_type_a_r17_e_     mtrp_pusch_repeat_type_a_r17;
  mtrp_pucch_intra_slot_r17_e_        mtrp_pucch_intra_slot_r17;
  ue_pwr_class_per_band_per_bc_r17_e_ ue_pwr_class_per_band_per_bc_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetUplink-v1720 ::= SEQUENCE
struct feature_set_ul_v1720_s {
  struct phy_prioritization_high_prio_dg_low_prio_cg_r17_s_ {
    struct pusch_prep_low_prio_r17_opts {
      enum options { sym0, sym1, sym2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using pusch_prep_low_prio_r17_e_ = enumerated<pusch_prep_low_prio_r17_opts>;
    struct add_cancellation_time_r17_s_ {
      struct scs_15k_hz_r17_opts {
        enum options { sym0, sym1, sym2, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using scs_15k_hz_r17_e_ = enumerated<scs_15k_hz_r17_opts>;
      struct scs_30k_hz_r17_opts {
        enum options { sym0, sym1, sym2, sym3, sym4, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using scs_30k_hz_r17_e_ = enumerated<scs_30k_hz_r17_opts>;
      struct scs_60k_hz_r17_opts {
        enum options { sym0, sym1, sym2, sym3, sym4, sym5, sym6, sym7, sym8, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using scs_60k_hz_r17_e_ = enumerated<scs_60k_hz_r17_opts>;
      struct scs_120k_hz_r17_opts {
        enum options {
          sym0,
          sym1,
          sym2,
          sym3,
          sym4,
          sym5,
          sym6,
          sym7,
          sym8,
          sym9,
          sym10,
          sym11,
          sym12,
          sym13,
          sym14,
          sym15,
          sym16,
          nulltype
        } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using scs_120k_hz_r17_e_ = enumerated<scs_120k_hz_r17_opts>;

      // member variables
      bool               scs_15k_hz_r17_present  = false;
      bool               scs_30k_hz_r17_present  = false;
      bool               scs_60k_hz_r17_present  = false;
      bool               scs_120k_hz_r17_present = false;
      scs_15k_hz_r17_e_  scs_15k_hz_r17;
      scs_30k_hz_r17_e_  scs_30k_hz_r17;
      scs_60k_hz_r17_e_  scs_60k_hz_r17;
      scs_120k_hz_r17_e_ scs_120k_hz_r17;
    };

    // member variables
    pusch_prep_low_prio_r17_e_   pusch_prep_low_prio_r17;
    add_cancellation_time_r17_s_ add_cancellation_time_r17;
    uint8_t                      max_num_carriers_r17 = 1;
  };

  // member variables
  bool                                               pucch_repeat_f0_1_2_3_4_rrc_cfg_r17_present              = false;
  bool                                               pucch_repeat_f0_1_2_3_4_dyn_ind_r17_present              = false;
  bool                                               inter_subslot_freq_hop_pucch_r17_present                 = false;
  bool                                               semi_static_harq_ack_codebook_sub_slot_pucch_r17_present = false;
  bool                                               phy_prioritization_low_prio_dg_high_prio_cg_r17_present  = false;
  bool                                               phy_prioritization_high_prio_dg_low_prio_cg_r17_present  = false;
  bool                                               extended_dc_location_report_r17_present                  = false;
  uint8_t                                            phy_prioritization_low_prio_dg_high_prio_cg_r17          = 1;
  phy_prioritization_high_prio_dg_low_prio_cg_r17_s_ phy_prioritization_high_prio_dg_low_prio_cg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetUplinkPerCC-v1540 ::= SEQUENCE
struct feature_set_ul_per_cc_v1540_s {
  struct mimo_non_cb_pusch_s_ {
    uint8_t max_num_srs_res_per_set  = 1;
    uint8_t max_num_simul_srs_res_tx = 1;
  };

  // member variables
  bool                 mimo_non_cb_pusch_present = false;
  mimo_non_cb_pusch_s_ mimo_non_cb_pusch;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSets ::= SEQUENCE
struct feature_sets_s {
  using feature_sets_dl_l_              = dyn_array<feature_set_dl_s>;
  using feature_sets_dl_per_cc_l_       = dyn_array<feature_set_dl_per_cc_s>;
  using feature_sets_ul_l_              = dyn_array<feature_set_ul_s>;
  using feature_sets_ul_per_cc_l_       = dyn_array<feature_set_ul_per_cc_s>;
  using feature_sets_dl_v1540_l_        = dyn_array<feature_set_dl_v1540_s>;
  using feature_sets_ul_v1540_l_        = dyn_array<feature_set_ul_v1540_s>;
  using feature_sets_ul_per_cc_v1540_l_ = dyn_array<feature_set_ul_per_cc_v1540_s>;
  using feature_sets_dl_v15a0_l_        = dyn_array<feature_set_dl_v15a0_s>;
  using feature_sets_dl_v1610_l_        = dyn_array<feature_set_dl_v1610_s>;
  using feature_sets_ul_v1610_l_        = dyn_array<feature_set_ul_v1610_s>;
  using feature_set_dl_per_cc_v1620_l_  = dyn_array<feature_set_dl_per_cc_v1620_s>;
  using feature_sets_ul_v1630_l_        = dyn_array<feature_set_ul_v1630_s>;
  using feature_sets_ul_v1640_l_        = dyn_array<feature_set_ul_v1640_s>;
  using feature_sets_dl_v1700_l_        = dyn_array<feature_set_dl_v1700_s>;
  using feature_sets_dl_per_cc_v1700_l_ = dyn_array<feature_set_dl_per_cc_v1700_s>;
  using feature_sets_ul_v1710_l_        = dyn_array<feature_set_ul_v1710_s>;
  using feature_sets_ul_per_cc_v1700_l_ = dyn_array<feature_set_ul_per_cc_v1700_s>;
  using feature_sets_dl_v1720_l_        = dyn_array<feature_set_dl_v1720_s>;
  using feature_sets_dl_per_cc_v1720_l_ = dyn_array<feature_set_dl_per_cc_v1720_s>;
  using feature_sets_ul_v1720_l_        = dyn_array<feature_set_ul_v1720_s>;
  using feature_sets_dl_v1730_l_        = dyn_array<feature_set_dl_v1730_s>;
  using feature_sets_dl_per_cc_v1730_l_ = dyn_array<feature_set_dl_per_cc_v1730_s>;

  // member variables
  bool                      ext = false;
  feature_sets_dl_l_        feature_sets_dl;
  feature_sets_dl_per_cc_l_ feature_sets_dl_per_cc;
  feature_sets_ul_l_        feature_sets_ul;
  feature_sets_ul_per_cc_l_ feature_sets_ul_per_cc;
  // ...
  // group 0
  copy_ptr<feature_sets_dl_v1540_l_>        feature_sets_dl_v1540;
  copy_ptr<feature_sets_ul_v1540_l_>        feature_sets_ul_v1540;
  copy_ptr<feature_sets_ul_per_cc_v1540_l_> feature_sets_ul_per_cc_v1540;
  // group 1
  copy_ptr<feature_sets_dl_v15a0_l_> feature_sets_dl_v15a0;
  // group 2
  copy_ptr<feature_sets_dl_v1610_l_>       feature_sets_dl_v1610;
  copy_ptr<feature_sets_ul_v1610_l_>       feature_sets_ul_v1610;
  copy_ptr<feature_set_dl_per_cc_v1620_l_> feature_set_dl_per_cc_v1620;
  // group 3
  copy_ptr<feature_sets_ul_v1630_l_> feature_sets_ul_v1630;
  // group 4
  copy_ptr<feature_sets_ul_v1640_l_> feature_sets_ul_v1640;
  // group 5
  copy_ptr<feature_sets_dl_v1700_l_>        feature_sets_dl_v1700;
  copy_ptr<feature_sets_dl_per_cc_v1700_l_> feature_sets_dl_per_cc_v1700;
  copy_ptr<feature_sets_ul_v1710_l_>        feature_sets_ul_v1710;
  copy_ptr<feature_sets_ul_per_cc_v1700_l_> feature_sets_ul_per_cc_v1700;
  // group 6
  copy_ptr<feature_sets_dl_v1720_l_>        feature_sets_dl_v1720;
  copy_ptr<feature_sets_dl_per_cc_v1720_l_> feature_sets_dl_per_cc_v1720;
  copy_ptr<feature_sets_ul_v1720_l_>        feature_sets_ul_v1720;
  // group 7
  copy_ptr<feature_sets_dl_v1730_l_>        feature_sets_dl_v1730;
  copy_ptr<feature_sets_dl_per_cc_v1730_l_> feature_sets_dl_per_cc_v1730;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AggregatedBandwidth ::= ENUMERATED
struct aggr_bw_opts {
  enum options {
    mhz50,
    mhz100,
    mhz150,
    mhz200,
    mhz250,
    mhz300,
    mhz350,
    mhz400,
    mhz450,
    mhz500,
    mhz550,
    mhz600,
    mhz650,
    mhz700,
    mhz750,
    mhz800,
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using aggr_bw_e = enumerated<aggr_bw_opts>;

// FreqBandInformationEUTRA ::= SEQUENCE
struct freq_band_info_eutra_s {
  bool                ca_bw_class_dl_eutra_present = false;
  bool                ca_bw_class_ul_eutra_present = false;
  uint16_t            band_eutra                   = 1;
  ca_bw_class_eutra_e ca_bw_class_dl_eutra;
  ca_bw_class_eutra_e ca_bw_class_ul_eutra;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqBandInformationNR ::= SEQUENCE
struct freq_band_info_nr_s {
  bool      max_bw_requested_dl_present       = false;
  bool      max_bw_requested_ul_present       = false;
  bool      max_carriers_requested_dl_present = false;
  bool      max_carriers_requested_ul_present = false;
  uint16_t  band_nr                           = 1;
  aggr_bw_e max_bw_requested_dl;
  aggr_bw_e max_bw_requested_ul;
  uint8_t   max_carriers_requested_dl = 1;
  uint8_t   max_carriers_requested_ul = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqBandInformation ::= CHOICE
struct freq_band_info_c {
  struct types_opts {
    enum options { band_info_eutra, band_info_nr, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  freq_band_info_c() = default;
  freq_band_info_c(const freq_band_info_c& other);
  freq_band_info_c& operator=(const freq_band_info_c& other);
  ~freq_band_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  freq_band_info_eutra_s& band_info_eutra()
  {
    assert_choice_type(types::band_info_eutra, type_, "FreqBandInformation");
    return c.get<freq_band_info_eutra_s>();
  }
  freq_band_info_nr_s& band_info_nr()
  {
    assert_choice_type(types::band_info_nr, type_, "FreqBandInformation");
    return c.get<freq_band_info_nr_s>();
  }
  const freq_band_info_eutra_s& band_info_eutra() const
  {
    assert_choice_type(types::band_info_eutra, type_, "FreqBandInformation");
    return c.get<freq_band_info_eutra_s>();
  }
  const freq_band_info_nr_s& band_info_nr() const
  {
    assert_choice_type(types::band_info_nr, type_, "FreqBandInformation");
    return c.get<freq_band_info_nr_s>();
  }
  freq_band_info_eutra_s& set_band_info_eutra();
  freq_band_info_nr_s&    set_band_info_nr();

private:
  types                                                        type_;
  choice_buffer_t<freq_band_info_eutra_s, freq_band_info_nr_s> c;

  void destroy_();
};

// FreqBandList ::= SEQUENCE (SIZE (1..1280)) OF FreqBandInformation
using freq_band_list_l = dyn_array<freq_band_info_c>;

// IMS-ParametersCommon ::= SEQUENCE
struct ims_params_common_s {
  bool ext                           = false;
  bool voice_over_eutra_5_gc_present = false;
  // ...
  // group 0
  bool voice_over_scg_bearer_eutra_5_gc_present = false;
  // group 1
  bool voice_fallback_ind_eps_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IMS-ParametersFRX-Diff ::= SEQUENCE
struct ims_params_frx_diff_s {
  bool ext                   = false;
  bool voice_over_nr_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IMS-Parameters ::= SEQUENCE
struct ims_params_s {
  bool                  ext                         = false;
  bool                  ims_params_common_present   = false;
  bool                  ims_params_frx_diff_present = false;
  ims_params_common_s   ims_params_common;
  ims_params_frx_diff_s ims_params_frx_diff;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IMS-ParametersFR2-2-r17 ::= SEQUENCE
struct ims_params_fr2_2_r17_s {
  bool ext                       = false;
  bool voice_over_nr_r17_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IMS-Parameters-v1700 ::= SEQUENCE
struct ims_params_v1700_s {
  bool                   ims_params_fr2_2_r17_present = false;
  ims_params_fr2_2_r17_s ims_params_fr2_2_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandUTRA-FDD-r16 ::= ENUMERATED
struct supported_band_utra_fdd_r16_opts {
  enum options {
    band_i,
    band_ii,
    band_iii,
    band_iv,
    band_v,
    band_vi,
    band_vii,
    band_viii,
    band_ix,
    band_x,
    band_xi,
    band_xii,
    band_xiii,
    band_xiv,
    band_xv,
    band_xvi,
    band_xvii,
    band_xviii,
    band_xix,
    band_xx,
    band_xxi,
    band_xxii,
    band_xxiii,
    band_xxiv,
    band_xxv,
    band_xxvi,
    band_xxvii,
    band_xxviii,
    band_xxix,
    band_xxx,
    band_xxxi,
    band_xxxii,
    nulltype
  } value;

  const char* to_string() const;
};
using supported_band_utra_fdd_r16_e = enumerated<supported_band_utra_fdd_r16_opts>;

// UTRA-FDD-Parameters-r16 ::= SEQUENCE
struct utra_fdd_params_r16_s {
  using supported_band_list_utra_fdd_r16_l_ = dyn_array<supported_band_utra_fdd_r16_e>;

  // member variables
  bool                                ext = false;
  supported_band_list_utra_fdd_r16_l_ supported_band_list_utra_fdd_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterRAT-Parameters ::= SEQUENCE
struct inter_rat_params_s {
  bool           ext           = false;
  bool           eutra_present = false;
  eutra_params_s eutra;
  // ...
  // group 0
  copy_ptr<utra_fdd_params_r16_s> utra_fdd_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-ParametersCommon ::= SEQUENCE
struct mac_params_common_s {
  bool ext                           = false;
  bool lcp_restrict_present          = false;
  bool dummy_present                 = false;
  bool lch_to_scell_restrict_present = false;
  // ...
  // group 0
  bool recommended_bit_rate_present       = false;
  bool recommended_bit_rate_query_present = false;
  // group 1
  bool recommended_bit_rate_multiplier_r16_present = false;
  bool pre_emptive_bsr_r16_present                 = false;
  bool autonomous_tx_r16_present                   = false;
  bool lch_prio_based_prioritization_r16_present   = false;
  bool lch_to_cfg_grant_map_r16_present            = false;
  bool lch_to_grant_prio_restrict_r16_present      = false;
  bool single_phr_p_r16_present                    = false;
  bool ul_lbt_fail_detection_recovery_r16_present  = false;
  bool tdd_mpe_p_mpr_report_r16_present            = false;
  bool lcid_ext_iab_r16_present                    = false;
  // group 2
  bool sp_cell_bfr_cbra_r16_present = false;
  // group 3
  bool srs_res_id_ext_r16_present = false;
  // group 4
  bool enhanced_uu_drx_for_sidelink_r17_present       = false;
  bool mg_activation_request_prs_meas_r17_present     = false;
  bool mg_activation_comm_prs_meas_r17_present        = false;
  bool intra_cg_prioritization_r17_present            = false;
  bool joint_prioritization_cg_retx_timer_r17_present = false;
  bool survival_time_r17_present                      = false;
  bool lcg_ext_iab_r17_present                        = false;
  bool harq_feedback_disabled_r17_present             = false;
  bool ul_harq_mode_b_r17_present                     = false;
  bool sr_triggered_by_ta_report_r17_present          = false;
  bool extended_drx_cycle_inactive_r17_present        = false;
  bool simul_sr_pusch_diff_pucch_groups_r17_present   = false;
  bool last_tx_ul_r17_present                         = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-ParametersXDD-Diff ::= SEQUENCE
struct mac_params_xdd_diff_s {
  bool ext                          = false;
  bool skip_ul_tx_dyn_present       = false;
  bool lc_ch_sr_delay_timer_present = false;
  bool long_drx_cycle_present       = false;
  bool short_drx_cycle_present      = false;
  bool multiple_sr_cfgs_present     = false;
  bool multiple_cfg_grants_present  = false;
  // ...
  // group 0
  bool secondary_drx_group_r16_present = false;
  // group 1
  bool enhanced_skip_ul_tx_dyn_r16_present = false;
  bool enhanced_skip_ul_tx_cfg_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-Parameters ::= SEQUENCE
struct mac_params_s {
  bool                  mac_params_common_present   = false;
  bool                  mac_params_xdd_diff_present = false;
  mac_params_common_s   mac_params_common;
  mac_params_xdd_diff_s mac_params_xdd_diff;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MinTimeGap-r16 ::= SEQUENCE
struct min_time_gap_r16_s {
  struct scs_15k_hz_r16_opts {
    enum options { sl1, sl3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using scs_15k_hz_r16_e_ = enumerated<scs_15k_hz_r16_opts>;
  struct scs_30k_hz_r16_opts {
    enum options { sl1, sl6, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using scs_30k_hz_r16_e_ = enumerated<scs_30k_hz_r16_opts>;
  struct scs_60k_hz_r16_opts {
    enum options { sl1, sl12, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using scs_60k_hz_r16_e_ = enumerated<scs_60k_hz_r16_opts>;
  struct scs_120k_hz_r16_opts {
    enum options { sl2, sl24, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using scs_120k_hz_r16_e_ = enumerated<scs_120k_hz_r16_opts>;

  // member variables
  bool               scs_15k_hz_r16_present  = false;
  bool               scs_30k_hz_r16_present  = false;
  bool               scs_60k_hz_r16_present  = false;
  bool               scs_120k_hz_r16_present = false;
  scs_15k_hz_r16_e_  scs_15k_hz_r16;
  scs_30k_hz_r16_e_  scs_30k_hz_r16;
  scs_60k_hz_r16_e_  scs_60k_hz_r16;
  scs_120k_hz_r16_e_ scs_120k_hz_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-ParametersFRX-Diff-r16 ::= SEQUENCE
struct mac_params_frx_diff_r16_s {
  struct drx_adaptation_r16_s_ {
    bool               non_shared_spec_ch_access_r16_present = false;
    bool               shared_spec_ch_access_r16_present     = false;
    min_time_gap_r16_s non_shared_spec_ch_access_r16;
    min_time_gap_r16_s shared_spec_ch_access_r16;
  };

  // member variables
  bool                  ext                                            = false;
  bool                  direct_mcg_scell_activation_r16_present        = false;
  bool                  direct_mcg_scell_activation_resume_r16_present = false;
  bool                  direct_scg_scell_activation_r16_present        = false;
  bool                  direct_scg_scell_activation_resume_r16_present = false;
  bool                  drx_adaptation_r16_present                     = false;
  drx_adaptation_r16_s_ drx_adaptation_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-Parameters-v1610 ::= SEQUENCE
struct mac_params_v1610_s {
  bool                      mac_params_frx_diff_r16_present = false;
  mac_params_frx_diff_r16_s mac_params_frx_diff_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MinTimeGapFR2-2-r17 ::= SEQUENCE
struct min_time_gap_fr2_2_r17_s {
  struct scs_120k_hz_r17_opts {
    enum options { sl2, sl24, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using scs_120k_hz_r17_e_ = enumerated<scs_120k_hz_r17_opts>;
  struct scs_480k_hz_r17_opts {
    enum options { sl8, sl96, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using scs_480k_hz_r17_e_ = enumerated<scs_480k_hz_r17_opts>;
  struct scs_960k_hz_r17_opts {
    enum options { sl16, sl192, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using scs_960k_hz_r17_e_ = enumerated<scs_960k_hz_r17_opts>;

  // member variables
  bool               scs_120k_hz_r17_present = false;
  bool               scs_480k_hz_r17_present = false;
  bool               scs_960k_hz_r17_present = false;
  scs_120k_hz_r17_e_ scs_120k_hz_r17;
  scs_480k_hz_r17_e_ scs_480k_hz_r17;
  scs_960k_hz_r17_e_ scs_960k_hz_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-ParametersFR2-2-r17 ::= SEQUENCE
struct mac_params_fr2_2_r17_s {
  struct drx_adaptation_r17_s_ {
    bool                     non_shared_spec_ch_access_r17_present = false;
    bool                     shared_spec_ch_access_r17_present     = false;
    min_time_gap_fr2_2_r17_s non_shared_spec_ch_access_r17;
    min_time_gap_fr2_2_r17_s shared_spec_ch_access_r17;
  };

  // member variables
  bool                  ext                                            = false;
  bool                  direct_mcg_scell_activation_r17_present        = false;
  bool                  direct_mcg_scell_activation_resume_r17_present = false;
  bool                  direct_scg_scell_activation_r17_present        = false;
  bool                  direct_scg_scell_activation_resume_r17_present = false;
  bool                  drx_adaptation_r17_present                     = false;
  drx_adaptation_r17_s_ drx_adaptation_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-Parameters-v1700 ::= SEQUENCE
struct mac_params_v1700_s {
  bool                   mac_params_fr2_2_r17_present = false;
  mac_params_fr2_2_r17_s mac_params_fr2_2_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-ParametersSidelinkCommon-r16 ::= SEQUENCE
struct mac_params_sidelink_common_r16_s {
  bool ext                                      = false;
  bool lcp_restrict_sidelink_r16_present        = false;
  bool multiple_cfg_grants_sidelink_r16_present = false;
  // ...
  // group 0
  bool drx_on_sidelink_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-ParametersSidelinkXDD-Diff-r16 ::= SEQUENCE
struct mac_params_sidelink_xdd_diff_r16_s {
  bool ext                                       = false;
  bool multiple_sr_cfgs_sidelink_r16_present     = false;
  bool lc_ch_sr_delay_timer_sidelink_r16_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-ParametersSidelink-r16 ::= SEQUENCE
struct mac_params_sidelink_r16_s {
  bool                               ext                                      = false;
  bool                               mac_params_sidelink_common_r16_present   = false;
  bool                               mac_params_sidelink_xdd_diff_r16_present = false;
  mac_params_sidelink_common_r16_s   mac_params_sidelink_common_r16;
  mac_params_sidelink_xdd_diff_r16_s mac_params_sidelink_xdd_diff_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersCommon ::= SEQUENCE
struct meas_and_mob_params_common_s {
  struct max_num_csi_rs_rrm_rs_sinr_opts {
    enum options { n4, n8, n16, n32, n64, n96, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_csi_rs_rrm_rs_sinr_e_ = enumerated<max_num_csi_rs_rrm_rs_sinr_opts>;
  struct cond_ho_params_common_r16_s_ {
    bool cond_ho_fdd_tdd_r16_present = false;
    bool cond_ho_fr1_fr2_r16_present = false;
  };
  struct max_num_cli_rssi_r16_opts {
    enum options { n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_cli_rssi_r16_e_ = enumerated<max_num_cli_rssi_r16_opts>;
  struct max_num_cli_srs_rsrp_r16_opts {
    enum options { n4, n8, n16, n32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_cli_srs_rsrp_r16_e_ = enumerated<max_num_cli_srs_rsrp_r16_opts>;
  struct max_num_per_slot_cli_srs_rsrp_r16_opts {
    enum options { n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_per_slot_cli_srs_rsrp_r16_e_ = enumerated<max_num_per_slot_cli_srs_rsrp_r16_opts>;
  struct concurrent_meas_gap_r17_c_ {
    struct types_opts {
      enum options { concurrent_per_ue_only_meas_gap_r17, concurrent_per_ue_per_fr_comb_meas_gap_r17, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    concurrent_meas_gap_r17_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    void set_concurrent_per_ue_only_meas_gap_r17();
    void set_concurrent_per_ue_per_fr_comb_meas_gap_r17();

  private:
    types type_;
  };
  struct independent_gap_cfg_max_cc_r17_s_ {
    bool    fr1_only_r17_present    = false;
    bool    fr2_only_r17_present    = false;
    bool    fr1_and_fr2_r17_present = false;
    uint8_t fr1_only_r17            = 1;
    uint8_t fr2_only_r17            = 1;
    uint8_t fr1_and_fr2_r17         = 1;
  };

  // member variables
  bool                ext                           = false;
  bool                supported_gap_pattern_present = false;
  bool                ssb_rlm_present               = false;
  bool                ssb_and_csi_rs_rlm_present    = false;
  fixed_bitstring<22> supported_gap_pattern;
  // ...
  // group 0
  bool event_b_meas_and_report_present = false;
  bool ho_fdd_tdd_present              = false;
  bool eutra_cgi_report_present        = false;
  bool nr_cgi_report_present           = false;
  // group 1
  bool                          independent_gap_cfg_present            = false;
  bool                          periodic_eutra_meas_and_report_present = false;
  bool                          ho_fr1_fr2_present                     = false;
  bool                          max_num_csi_rs_rrm_rs_sinr_present     = false;
  max_num_csi_rs_rrm_rs_sinr_e_ max_num_csi_rs_rrm_rs_sinr;
  // group 2
  bool nr_cgi_report_endc_present = false;
  // group 3
  bool eutra_cgi_report_nedc_present = false;
  bool eutra_cgi_report_nrdc_present = false;
  bool nr_cgi_report_nedc_present    = false;
  bool nr_cgi_report_nrdc_present    = false;
  // group 4
  bool                                   report_add_neigh_meas_for_periodic_r16_present = false;
  bool                                   nr_need_for_gap_report_r16_present             = false;
  bool                                   supported_gap_pattern_nr_only_r16_present      = false;
  bool                                   supported_gap_pattern_nr_only_nedc_r16_present = false;
  bool                                   max_num_cli_rssi_r16_present                   = false;
  bool                                   max_num_cli_srs_rsrp_r16_present               = false;
  bool                                   max_num_per_slot_cli_srs_rsrp_r16_present      = false;
  bool                                   mfbi_iab_r16_present                           = false;
  bool                                   dummy_present                                  = false;
  bool                                   nr_cgi_report_npn_r16_present                  = false;
  bool                                   idle_inactive_eutra_meas_report_r16_present    = false;
  bool                                   idle_inactive_validity_area_r16_present        = false;
  bool                                   eutra_autonomous_gaps_r16_present              = false;
  bool                                   eutra_autonomous_gaps_nedc_r16_present         = false;
  bool                                   eutra_autonomous_gaps_nrdc_r16_present         = false;
  bool                                   pcell_t312_r16_present                         = false;
  bool                                   supported_gap_pattern_r16_present              = false;
  copy_ptr<cond_ho_params_common_r16_s_> cond_ho_params_common_r16;
  fixed_bitstring<10>                    supported_gap_pattern_nr_only_r16;
  max_num_cli_rssi_r16_e_                max_num_cli_rssi_r16;
  max_num_cli_srs_rsrp_r16_e_            max_num_cli_srs_rsrp_r16;
  max_num_per_slot_cli_srs_rsrp_r16_e_   max_num_per_slot_cli_srs_rsrp_r16;
  fixed_bitstring<2>                     supported_gap_pattern_r16;
  // group 5
  bool                                 nr_need_for_gap_ncsg_report_r17_present          = false;
  bool                                 eutra_need_for_gap_ncsg_report_r17_present       = false;
  bool                                 ncsg_meas_gap_per_fr_r17_present                 = false;
  bool                                 ncsg_meas_gap_patterns_r17_present               = false;
  bool                                 ncsg_meas_gap_nr_patterns_r17_present            = false;
  bool                                 precfg_ue_autonomous_meas_gap_r17_present        = false;
  bool                                 precfg_nw_ctrl_meas_gap_r17_present              = false;
  bool                                 ho_fr1_fr2_2_r17_present                         = false;
  bool                                 ho_fr2_1_fr2_2_r17_present                       = false;
  bool                                 independent_gap_cfg_prs_r17_present              = false;
  bool                                 rrm_relaxation_rrc_connected_red_cap_r17_present = false;
  bool                                 parallel_meas_gap_r17_present                    = false;
  bool                                 cond_ho_with_scg_nrdc_r17_present                = false;
  bool                                 gnb_id_len_report_r17_present                    = false;
  bool                                 gnb_id_len_report_endc_r17_present               = false;
  bool                                 gnb_id_len_report_nedc_r17_present               = false;
  bool                                 gnb_id_len_report_nrdc_r17_present               = false;
  bool                                 gnb_id_len_report_npn_r17_present                = false;
  copy_ptr<concurrent_meas_gap_r17_c_> concurrent_meas_gap_r17;
  fixed_bitstring<24>                  ncsg_meas_gap_patterns_r17;
  fixed_bitstring<24>                  ncsg_meas_gap_nr_patterns_r17;
  // group 6
  bool parallel_smtc_r17_present                             = false;
  bool concurrent_meas_gap_eutra_r17_present                 = false;
  bool service_link_prop_delay_diff_report_r17_present       = false;
  bool ncsg_symbol_level_schedule_restrict_inter_r17_present = false;
  // group 7
  bool                                        event_d1_meas_report_trigger_r17_present = false;
  copy_ptr<independent_gap_cfg_max_cc_r17_s_> independent_gap_cfg_max_cc_r17;
  // group 8
  bool inter_sat_meas_r17_present                          = false;
  bool derive_ssb_idx_from_cell_inter_non_ncsg_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersFRX-Diff ::= SEQUENCE
struct meas_and_mob_params_frx_diff_s {
  struct max_num_res_csi_rs_rlm_opts {
    enum options { n2, n4, n6, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_res_csi_rs_rlm_e_ = enumerated<max_num_res_csi_rs_rlm_opts>;

  // member variables
  bool ext                                        = false;
  bool ss_sinr_meas_present                       = false;
  bool csi_rsrp_and_rsrq_meas_with_ssb_present    = false;
  bool csi_rsrp_and_rsrq_meas_without_ssb_present = false;
  bool csi_sinr_meas_present                      = false;
  bool csi_rs_rlm_present                         = false;
  // ...
  // group 0
  bool ho_inter_f_present  = false;
  bool ho_lte_epc_present  = false;
  bool ho_lte_5_gc_present = false;
  // group 1
  bool                      max_num_res_csi_rs_rlm_present = false;
  max_num_res_csi_rs_rlm_e_ max_num_res_csi_rs_rlm;
  // group 2
  bool simul_rx_data_ssb_diff_numerology_present = false;
  // group 3
  bool nr_autonomous_gaps_r16_present                      = false;
  bool nr_autonomous_gaps_endc_r16_present                 = false;
  bool nr_autonomous_gaps_nedc_r16_present                 = false;
  bool nr_autonomous_gaps_nrdc_r16_present                 = false;
  bool dummy_present                                       = false;
  bool cli_rssi_meas_r16_present                           = false;
  bool cli_srs_rsrp_meas_r16_present                       = false;
  bool interfreq_meas_no_gap_r16_present                   = false;
  bool simul_rx_data_ssb_diff_numerology_inter_r16_present = false;
  bool idle_inactive_nr_meas_report_r16_present            = false;
  bool idle_inactive_nr_meas_beam_report_r16_present       = false;
  // group 4
  bool increased_numof_csi_rs_per_mo_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersXDD-Diff ::= SEQUENCE
struct meas_and_mob_params_xdd_diff_s {
  bool ext                                       = false;
  bool intra_and_inter_f_meas_and_report_present = false;
  bool event_a_meas_and_report_present           = false;
  // ...
  // group 0
  bool ho_inter_f_present  = false;
  bool ho_lte_epc_present  = false;
  bool ho_lte_5_gc_present = false;
  // group 1
  bool sftd_meas_nr_neigh_present     = false;
  bool sftd_meas_nr_neigh_drx_present = false;
  // group 2
  bool dummy_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParameters ::= SEQUENCE
struct meas_and_mob_params_s {
  bool                           meas_and_mob_params_common_present   = false;
  bool                           meas_and_mob_params_xdd_diff_present = false;
  bool                           meas_and_mob_params_frx_diff_present = false;
  meas_and_mob_params_common_s   meas_and_mob_params_common;
  meas_and_mob_params_xdd_diff_s meas_and_mob_params_xdd_diff;
  meas_and_mob_params_frx_diff_s meas_and_mob_params_frx_diff;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersFR2-2-r17 ::= SEQUENCE
struct meas_and_mob_params_fr2_2_r17_s {
  bool ext                                      = false;
  bool ho_inter_f_r17_present                   = false;
  bool ho_lte_epc_r17_present                   = false;
  bool ho_lte_5_gc_r17_present                  = false;
  bool idle_inactive_nr_meas_report_r17_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParameters-v1700 ::= SEQUENCE
struct meas_and_mob_params_v1700_s {
  bool                            meas_and_mob_params_fr2_2_r17_present = false;
  meas_and_mob_params_fr2_2_r17_s meas_and_mob_params_fr2_2_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersMRDC-Common ::= SEQUENCE
struct meas_and_mob_params_mrdc_common_s {
  bool independent_gap_cfg_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersMRDC-FRX-Diff ::= SEQUENCE
struct meas_and_mob_params_mrdc_frx_diff_s {
  bool simul_rx_data_ssb_diff_numerology_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersMRDC-XDD-Diff ::= SEQUENCE
struct meas_and_mob_params_mrdc_xdd_diff_s {
  bool sftd_meas_pscell_present  = false;
  bool sftd_meas_nr_cell_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersMRDC ::= SEQUENCE
struct meas_and_mob_params_mrdc_s {
  bool                                meas_and_mob_params_mrdc_common_present   = false;
  bool                                meas_and_mob_params_mrdc_xdd_diff_present = false;
  bool                                meas_and_mob_params_mrdc_frx_diff_present = false;
  meas_and_mob_params_mrdc_common_s   meas_and_mob_params_mrdc_common;
  meas_and_mob_params_mrdc_xdd_diff_s meas_and_mob_params_mrdc_xdd_diff;
  meas_and_mob_params_mrdc_frx_diff_s meas_and_mob_params_mrdc_frx_diff;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersMRDC-Common-v1610 ::= SEQUENCE
struct meas_and_mob_params_mrdc_common_v1610_s {
  struct cond_pscell_change_params_common_r16_s_ {
    bool cond_pscell_change_fdd_tdd_r16_present = false;
    bool cond_pscell_change_fr1_fr2_r16_present = false;
  };

  // member variables
  bool                                    cond_pscell_change_params_common_r16_present = false;
  bool                                    pscell_t312_r16_present                      = false;
  cond_pscell_change_params_common_r16_s_ cond_pscell_change_params_common_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersMRDC-v1610 ::= SEQUENCE
struct meas_and_mob_params_mrdc_v1610_s {
  bool                                    meas_and_mob_params_mrdc_common_v1610_present = false;
  bool                                    inter_nr_meas_eutra_iab_r16_present           = false;
  meas_and_mob_params_mrdc_common_v1610_s meas_and_mob_params_mrdc_common_v1610;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersMRDC-Common-v1700 ::= SEQUENCE
struct meas_and_mob_params_mrdc_common_v1700_s {
  struct cond_pscell_change_params_r17_s_ {
    bool inter_sn_cond_pscell_change_fdd_tdd_nrdc_r17_present     = false;
    bool inter_sn_cond_pscell_change_fr1_fr2_nrdc_r17_present     = false;
    bool inter_sn_cond_pscell_change_fdd_tdd_endc_r17_present     = false;
    bool inter_sn_cond_pscell_change_fr1_fr2_endc_r17_present     = false;
    bool mn_initiated_cond_pscell_change_fr1_fdd_endc_r17_present = false;
    bool mn_initiated_cond_pscell_change_fr1_tdd_endc_r17_present = false;
    bool mn_initiated_cond_pscell_change_fr2_tdd_endc_r17_present = false;
    bool sn_initiated_cond_pscell_change_fr1_fdd_endc_r17_present = false;
    bool sn_initiated_cond_pscell_change_fr1_tdd_endc_r17_present = false;
    bool sn_initiated_cond_pscell_change_fr2_tdd_endc_r17_present = false;
  };

  // member variables
  bool                             cond_pscell_change_params_r17_present = false;
  bool                             cond_ho_with_scg_endc_r17_present     = false;
  bool                             cond_ho_with_scg_nedc_r17_present     = false;
  cond_pscell_change_params_r17_s_ cond_pscell_change_params_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersMRDC-v1700 ::= SEQUENCE
struct meas_and_mob_params_mrdc_v1700_s {
  bool                                    meas_and_mob_params_mrdc_common_v1700_present = false;
  meas_and_mob_params_mrdc_common_v1700_s meas_and_mob_params_mrdc_common_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GeneralParametersMRDC-XDD-Diff ::= SEQUENCE
struct general_params_mrdc_xdd_diff_s {
  bool ext                                    = false;
  bool split_srb_with_one_ul_path_present     = false;
  bool split_drb_with_ul_both_mcg_scg_present = false;
  bool srb3_present                           = false;
  bool dummy_present                          = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-MRDC-CapabilityAddFRX-Mode ::= SEQUENCE
struct ue_mrdc_cap_add_frx_mode_s {
  meas_and_mob_params_mrdc_frx_diff_s meas_and_mob_params_mrdc_frx_diff;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-MRDC-CapabilityAddXDD-Mode ::= SEQUENCE
struct ue_mrdc_cap_add_xdd_mode_s {
  bool                                meas_and_mob_params_mrdc_xdd_diff_present = false;
  bool                                general_params_mrdc_xdd_diff_present      = false;
  meas_and_mob_params_mrdc_xdd_diff_s meas_and_mob_params_mrdc_xdd_diff;
  general_params_mrdc_xdd_diff_s      general_params_mrdc_xdd_diff;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NRDC-Parameters ::= SEQUENCE
struct nrdc_params_s {
  bool                           meas_and_mob_params_nrdc_present = false;
  bool                           general_params_nrdc_present      = false;
  bool                           fdd_add_ue_nrdc_cap_present      = false;
  bool                           tdd_add_ue_nrdc_cap_present      = false;
  bool                           fr1_add_ue_nrdc_cap_present      = false;
  bool                           fr2_add_ue_nrdc_cap_present      = false;
  bool                           dummy_present                    = false;
  meas_and_mob_params_mrdc_s     meas_and_mob_params_nrdc;
  general_params_mrdc_xdd_diff_s general_params_nrdc;
  ue_mrdc_cap_add_xdd_mode_s     fdd_add_ue_nrdc_cap;
  ue_mrdc_cap_add_xdd_mode_s     tdd_add_ue_nrdc_cap;
  ue_mrdc_cap_add_frx_mode_s     fr1_add_ue_nrdc_cap;
  ue_mrdc_cap_add_frx_mode_s     fr2_add_ue_nrdc_cap;
  dyn_octstring                  dummy2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NRDC-Parameters-v1610 ::= SEQUENCE
struct nrdc_params_v1610_s {
  bool                             meas_and_mob_params_nrdc_v1610_present = false;
  meas_and_mob_params_mrdc_v1610_s meas_and_mob_params_nrdc_v1610;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NRDC-Parameters-v1700 ::= SEQUENCE
struct nrdc_params_v1700_s {
  bool                             f1c_over_nr_rrc_r17_present = false;
  meas_and_mob_params_mrdc_v1700_s meas_and_mob_params_nrdc_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierAggregationVariant ::= SEQUENCE
struct carrier_aggregation_variant_s {
  bool fr1fdd_fr1_tdd_ca_sp_cell_on_fr1_fdd_present         = false;
  bool fr1fdd_fr1_tdd_ca_sp_cell_on_fr1_tdd_present         = false;
  bool fr1fdd_fr2_tdd_ca_sp_cell_on_fr1_fdd_present         = false;
  bool fr1fdd_fr2_tdd_ca_sp_cell_on_fr2_tdd_present         = false;
  bool fr1tdd_fr2_tdd_ca_sp_cell_on_fr1_tdd_present         = false;
  bool fr1tdd_fr2_tdd_ca_sp_cell_on_fr2_tdd_present         = false;
  bool fr1fdd_fr1_tdd_fr2_tdd_ca_sp_cell_on_fr1_fdd_present = false;
  bool fr1fdd_fr1_tdd_fr2_tdd_ca_sp_cell_on_fr1_tdd_present = false;
  bool fr1fdd_fr1_tdd_fr2_tdd_ca_sp_cell_on_fr2_tdd_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Phy-ParametersCommon ::= SEQUENCE
struct phy_params_common_s {
  struct bwp_switching_delay_opts {
    enum options { type1, type2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using bwp_switching_delay_e_ = enumerated<bwp_switching_delay_opts>;
  struct cross_slot_sched_r16_s_ {
    bool non_shared_spec_ch_access_r16_present = false;
    bool shared_spec_ch_access_r16_present     = false;
  };
  struct max_num_srs_pos_path_loss_estimate_all_serving_cells_r16_opts {
    enum options { n1, n4, n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_srs_pos_path_loss_estimate_all_serving_cells_r16_e_ =
      enumerated<max_num_srs_pos_path_loss_estimate_all_serving_cells_r16_opts>;
  struct pusch_repeat_type_a_r16_s_ {
    bool shared_spec_ch_access_r16_present     = false;
    bool non_shared_spec_ch_access_r16_present = false;
  };
  struct max_num_pathloss_rs_upd_r16_opts {
    enum options { n4, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_pathloss_rs_upd_r16_e_ = enumerated<max_num_pathloss_rs_upd_r16_opts>;
  struct max_total_res_for_across_freq_ranges_r16_s_ {
    struct max_num_res_within_slot_across_cc_across_fr_r16_opts {
      enum options { n2, n4, n8, n12, n16, n32, n64, n128, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_res_within_slot_across_cc_across_fr_r16_e_ =
        enumerated<max_num_res_within_slot_across_cc_across_fr_r16_opts>;
    struct max_num_res_across_cc_across_fr_r16_opts {
      enum options { n2, n4, n8, n12, n16, n32, n40, n48, n64, n72, n80, n96, n128, n256, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using max_num_res_across_cc_across_fr_r16_e_ = enumerated<max_num_res_across_cc_across_fr_r16_opts>;

    // member variables
    bool                                               max_num_res_within_slot_across_cc_across_fr_r16_present = false;
    bool                                               max_num_res_across_cc_across_fr_r16_present             = false;
    max_num_res_within_slot_across_cc_across_fr_r16_e_ max_num_res_within_slot_across_cc_across_fr_r16;
    max_num_res_across_cc_across_fr_r16_e_             max_num_res_across_cc_across_fr_r16;
  };
  struct harq_ack_separate_multi_dci_multi_trp_r16_s_ {
    struct max_num_long_pucch_s_r16_opts {
      enum options { long_and_long, long_and_short, short_and_short, nulltype } value;

      const char* to_string() const;
    };
    using max_num_long_pucch_s_r16_e_ = enumerated<max_num_long_pucch_s_r16_opts>;

    // member variables
    bool                        max_num_long_pucch_s_r16_present = false;
    max_num_long_pucch_s_r16_e_ max_num_long_pucch_s_r16;
  };
  struct bwp_switching_multi_ccs_r16_c_ {
    struct type1_r16_opts {
      enum options { us100, us200, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using type1_r16_e_ = enumerated<type1_r16_opts>;
    struct type2_r16_opts {
      enum options { us200, us400, us800, us1000, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using type2_r16_e_ = enumerated<type2_r16_opts>;
    struct types_opts {
      enum options { type1_r16, type2_r16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    bwp_switching_multi_ccs_r16_c_() = default;
    bwp_switching_multi_ccs_r16_c_(const bwp_switching_multi_ccs_r16_c_& other);
    bwp_switching_multi_ccs_r16_c_& operator=(const bwp_switching_multi_ccs_r16_c_& other);
    ~bwp_switching_multi_ccs_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    type1_r16_e_& type1_r16()
    {
      assert_choice_type(types::type1_r16, type_, "bwp-SwitchingMultiCCs-r16");
      return c.get<type1_r16_e_>();
    }
    type2_r16_e_& type2_r16()
    {
      assert_choice_type(types::type2_r16, type_, "bwp-SwitchingMultiCCs-r16");
      return c.get<type2_r16_e_>();
    }
    const type1_r16_e_& type1_r16() const
    {
      assert_choice_type(types::type1_r16, type_, "bwp-SwitchingMultiCCs-r16");
      return c.get<type1_r16_e_>();
    }
    const type2_r16_e_& type2_r16() const
    {
      assert_choice_type(types::type2_r16, type_, "bwp-SwitchingMultiCCs-r16");
      return c.get<type2_r16_e_>();
    }
    type1_r16_e_& set_type1_r16();
    type2_r16_e_& set_type2_r16();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct bwp_switching_multi_dormancy_ccs_r16_c_ {
    struct type1_r16_opts {
      enum options { us100, us200, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using type1_r16_e_ = enumerated<type1_r16_opts>;
    struct type2_r16_opts {
      enum options { us200, us400, us800, us1000, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using type2_r16_e_ = enumerated<type2_r16_opts>;
    struct types_opts {
      enum options { type1_r16, type2_r16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    bwp_switching_multi_dormancy_ccs_r16_c_() = default;
    bwp_switching_multi_dormancy_ccs_r16_c_(const bwp_switching_multi_dormancy_ccs_r16_c_& other);
    bwp_switching_multi_dormancy_ccs_r16_c_& operator=(const bwp_switching_multi_dormancy_ccs_r16_c_& other);
    ~bwp_switching_multi_dormancy_ccs_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    type1_r16_e_& type1_r16()
    {
      assert_choice_type(types::type1_r16, type_, "bwp-SwitchingMultiDormancyCCs-r16");
      return c.get<type1_r16_e_>();
    }
    type2_r16_e_& type2_r16()
    {
      assert_choice_type(types::type2_r16, type_, "bwp-SwitchingMultiDormancyCCs-r16");
      return c.get<type2_r16_e_>();
    }
    const type1_r16_e_& type1_r16() const
    {
      assert_choice_type(types::type1_r16, type_, "bwp-SwitchingMultiDormancyCCs-r16");
      return c.get<type1_r16_e_>();
    }
    const type2_r16_e_& type2_r16() const
    {
      assert_choice_type(types::type2_r16, type_, "bwp-SwitchingMultiDormancyCCs-r16");
      return c.get<type2_r16_e_>();
    }
    type1_r16_e_& set_type1_r16();
    type2_r16_e_& set_type2_r16();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct pdcch_monitoring_any_occasions_with_span_gap_cross_carrier_sch_r16_opts {
    enum options { mode2, mode3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using pdcch_monitoring_any_occasions_with_span_gap_cross_carrier_sch_r16_e_ =
      enumerated<pdcch_monitoring_any_occasions_with_span_gap_cross_carrier_sch_r16_opts>;
  struct sps_harq_ack_deferral_r17_s_ {
    bool non_shared_spec_ch_access_r17_present = false;
    bool shared_spec_ch_access_r17_present     = false;
  };
  struct supported_activ_prs_processing_win_r17_opts {
    enum options { n2, n3, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using supported_activ_prs_processing_win_r17_e_ = enumerated<supported_activ_prs_processing_win_r17_opts>;

  // member variables
  bool                   ext                                         = false;
  bool                   csi_rs_cfra_for_ho_present                  = false;
  bool                   dyn_prb_bundling_dl_present                 = false;
  bool                   sp_csi_report_pucch_present                 = false;
  bool                   sp_csi_report_pusch_present                 = false;
  bool                   nzp_csi_rs_intef_mgmt_present               = false;
  bool                   type2_sp_csi_feedback_long_pucch_present    = false;
  bool                   precoder_granularity_coreset_present        = false;
  bool                   dyn_harq_ack_codebook_present               = false;
  bool                   semi_static_harq_ack_codebook_present       = false;
  bool                   spatial_bundling_harq_ack_present           = false;
  bool                   dyn_beta_offset_ind_harq_ack_csi_present    = false;
  bool                   pucch_repeat_f1_3_4_present                 = false;
  bool                   ra_type0_pusch_present                      = false;
  bool                   dyn_switch_ra_type0_1_pdsch_present         = false;
  bool                   dyn_switch_ra_type0_1_pusch_present         = false;
  bool                   pdsch_map_type_a_present                    = false;
  bool                   pdsch_map_type_b_present                    = false;
  bool                   interleaving_vrb_to_prb_pdsch_present       = false;
  bool                   inter_slot_freq_hop_pusch_present           = false;
  bool                   type1_pusch_repeat_multi_slots_present      = false;
  bool                   type2_pusch_repeat_multi_slots_present      = false;
  bool                   pusch_repeat_multi_slots_present            = false;
  bool                   pdsch_repeat_multi_slots_present            = false;
  bool                   dl_sps_present                              = false;
  bool                   cfg_ul_grant_type1_present                  = false;
  bool                   cfg_ul_grant_type2_present                  = false;
  bool                   pre_empt_ind_dl_present                     = false;
  bool                   cbg_trans_ind_dl_present                    = false;
  bool                   cbg_trans_ind_ul_present                    = false;
  bool                   cbg_flush_ind_dl_present                    = false;
  bool                   dyn_harq_ack_code_b_cbg_retx_dl_present     = false;
  bool                   rate_matching_resrc_set_semi_static_present = false;
  bool                   rate_matching_resrc_set_dyn_present         = false;
  bool                   bwp_switching_delay_present                 = false;
  bwp_switching_delay_e_ bwp_switching_delay;
  // ...
  // group 0
  bool dummy_present = false;
  // group 1
  bool max_num_search_spaces_present            = false;
  bool rate_matching_ctrl_resrc_set_dyn_present = false;
  bool max_layers_mimo_ind_present              = false;
  // group 2
  copy_ptr<carrier_aggregation_variant_s> sp_cell_placement;
  // group 3
  bool                              two_step_rach_r16_present                                        = false;
  bool                              dci_format1_2_and0_2_r16_present                                 = false;
  bool                              monitoring_dci_same_search_space_r16_present                     = false;
  bool                              type2_cg_release_dci_0_1_r16_present                             = false;
  bool                              type2_cg_release_dci_0_2_r16_present                             = false;
  bool                              sps_release_dci_1_1_r16_present                                  = false;
  bool                              sps_release_dci_1_2_r16_present                                  = false;
  bool                              csi_trigger_state_non_active_bwp_r16_present                     = false;
  bool                              separate_smtc_inter_iab_support_r16_present                      = false;
  bool                              separate_rach_iab_support_r16_present                            = false;
  bool                              ul_flex_dl_slot_format_semi_static_iab_r16_present               = false;
  bool                              ul_flex_dl_slot_format_dyns_iab_r16_present                      = false;
  bool                              dft_s_ofdm_waveform_ul_iab_r16_present                           = false;
  bool                              dci_25_ai_rnti_support_iab_r16_present                           = false;
  bool                              t_delta_reception_support_iab_r16_present                        = false;
  bool                              guard_symbol_report_reception_iab_r16_present                    = false;
  bool                              harq_ack_cb_spatial_bundling_pucch_group_r16_present             = false;
  bool                              max_num_srs_pos_path_loss_estimate_all_serving_cells_r16_present = false;
  bool                              extended_cg_periodicities_r16_present                            = false;
  bool                              extended_sps_periodicities_r16_present                           = false;
  bool                              dci_dl_prio_ind_r16_present                                      = false;
  bool                              dci_ul_prio_ind_r16_present                                      = false;
  bool                              max_num_pathloss_rs_upd_r16_present                              = false;
  bool                              type2_harq_ack_codebook_r16_present                              = false;
  bool                              harq_ack_joint_multi_dci_multi_trp_r16_present                   = false;
  copy_ptr<cross_slot_sched_r16_s_> cross_slot_sched_r16;
  max_num_srs_pos_path_loss_estimate_all_serving_cells_r16_e_ max_num_srs_pos_path_loss_estimate_all_serving_cells_r16;
  copy_ptr<codebook_variants_list_r16_l>                      codebook_variants_list_r16;
  copy_ptr<pusch_repeat_type_a_r16_s_>                        pusch_repeat_type_a_r16;
  max_num_pathloss_rs_upd_r16_e_                              max_num_pathloss_rs_upd_r16;
  copy_ptr<max_total_res_for_across_freq_ranges_r16_s_>       max_total_res_for_across_freq_ranges_r16;
  copy_ptr<harq_ack_separate_multi_dci_multi_trp_r16_s_>      harq_ack_separate_multi_dci_multi_trp_r16;
  copy_ptr<bwp_switching_multi_ccs_r16_c_>                    bwp_switching_multi_ccs_r16;
  // group 4
  bool target_smtc_scg_r16_present               = false;
  bool support_repeat_zero_offset_rv_r16_present = false;
  bool cbg_trans_in_order_pusch_ul_r16_present   = false;
  // group 5
  bool support_retx_diff_coreset_pool_multi_dci_trp_r16_present                   = false;
  bool pdcch_monitoring_any_occasions_with_span_gap_cross_carrier_sch_r16_present = false;
  copy_ptr<bwp_switching_multi_dormancy_ccs_r16_c_> bwp_switching_multi_dormancy_ccs_r16;
  pdcch_monitoring_any_occasions_with_span_gap_cross_carrier_sch_r16_e_
      pdcch_monitoring_any_occasions_with_span_gap_cross_carrier_sch_r16;
  // group 6
  bool new_beam_identifs2_port_csi_rs_r16_present   = false;
  bool pathloss_estimation2_port_csi_rs_r16_present = false;
  // group 7
  bool mux_harq_ack_without_pucch_on_pusch_r16_present = false;
  // group 8
  bool                                      guard_symbol_report_reception_iab_r17_present  = false;
  bool                                      restricted_iab_du_beam_reception_r17_present   = false;
  bool                                      recommended_iab_mt_beam_tx_r17_present         = false;
  bool                                      case6_timing_align_reception_iab_r17_present   = false;
  bool                                      case7_timing_align_reception_iab_r17_present   = false;
  bool                                      dl_tx_pwr_adjustment_iab_r17_present           = false;
  bool                                      desired_ul_tx_pwr_adjustment_r17_present       = false;
  bool                                      fdm_soft_res_availability_dyn_ind_r17_present  = false;
  bool                                      upd_t_delta_range_recption_r17_present         = false;
  bool                                      slot_based_dyn_pucch_rep_r17_present           = false;
  bool                                      unified_joint_tci_common_upd_r17_present       = false;
  bool                                      mtrp_pdcch_single_span_r17_present             = false;
  bool                                      supported_activ_prs_processing_win_r17_present = false;
  bool                                      cg_time_domain_alloc_ext_r17_present           = false;
  copy_ptr<sps_harq_ack_deferral_r17_s_>    sps_harq_ack_deferral_r17;
  uint8_t                                   unified_joint_tci_common_upd_r17 = 1;
  supported_activ_prs_processing_win_r17_e_ supported_activ_prs_processing_win_r17;
  // group 9
  bool ta_based_pdc_tn_non_shared_spec_ch_access_r17_present = false;
  bool directional_collision_dc_iab_r17_present              = false;
  // group 10
  bool dummy1_present                  = false;
  bool dummy2_present                  = false;
  bool dummy3_present                  = false;
  bool dummy4_present                  = false;
  bool srs_add_repeat_r17_present      = false;
  bool pusch_repeat_cg_sdt_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Phy-ParametersFR1 ::= SEQUENCE
struct phy_params_fr1_s {
  struct pdsch_re_map_fr1_per_symbol_opts {
    enum options { n10, n20, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using pdsch_re_map_fr1_per_symbol_e_ = enumerated<pdsch_re_map_fr1_per_symbol_opts>;
  struct pdsch_re_map_fr1_per_slot_opts {
    enum options {
      n16,
      n32,
      n48,
      n64,
      n80,
      n96,
      n112,
      n128,
      n144,
      n160,
      n176,
      n192,
      n208,
      n224,
      n240,
      n256,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using pdsch_re_map_fr1_per_slot_e_ = enumerated<pdsch_re_map_fr1_per_slot_opts>;

  // member variables
  bool                           ext                                      = false;
  bool                           pdcch_monitoring_single_occasion_present = false;
  bool                           scs_60k_hz_present                       = false;
  bool                           pdsch_256_qam_fr1_present                = false;
  bool                           pdsch_re_map_fr1_per_symbol_present      = false;
  pdsch_re_map_fr1_per_symbol_e_ pdsch_re_map_fr1_per_symbol;
  // ...
  // group 0
  bool                         pdsch_re_map_fr1_per_slot_present = false;
  pdsch_re_map_fr1_per_slot_e_ pdsch_re_map_fr1_per_slot;
  // group 1
  bool pdcch_monitoring_single_span_first4_sym_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Phy-ParametersFR2 ::= SEQUENCE
struct phy_params_fr2_s {
  struct pdsch_re_map_fr2_per_symbol_opts {
    enum options { n6, n20, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using pdsch_re_map_fr2_per_symbol_e_ = enumerated<pdsch_re_map_fr2_per_symbol_opts>;
  struct pdsch_re_map_fr2_per_slot_opts {
    enum options {
      n16,
      n32,
      n48,
      n64,
      n80,
      n96,
      n112,
      n128,
      n144,
      n160,
      n176,
      n192,
      n208,
      n224,
      n240,
      n256,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using pdsch_re_map_fr2_per_slot_e_ = enumerated<pdsch_re_map_fr2_per_slot_opts>;
  struct max_num_srs_pos_spatial_relations_all_serving_cells_r16_opts {
    enum options { n0, n1, n2, n4, n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_srs_pos_spatial_relations_all_serving_cells_r16_e_ =
      enumerated<max_num_srs_pos_spatial_relations_all_serving_cells_r16_opts>;

  // member variables
  bool                           ext                                 = false;
  bool                           dummy_present                       = false;
  bool                           pdsch_re_map_fr2_per_symbol_present = false;
  pdsch_re_map_fr2_per_symbol_e_ pdsch_re_map_fr2_per_symbol;
  // ...
  // group 0
  bool                         pcell_fr2_present                 = false;
  bool                         pdsch_re_map_fr2_per_slot_present = false;
  pdsch_re_map_fr2_per_slot_e_ pdsch_re_map_fr2_per_slot;
  // group 1
  bool default_spatial_relation_pathloss_rs_r16_present                = false;
  bool spatial_relation_upd_ap_srs_r16_present                         = false;
  bool max_num_srs_pos_spatial_relations_all_serving_cells_r16_present = false;
  max_num_srs_pos_spatial_relations_all_serving_cells_r16_e_ max_num_srs_pos_spatial_relations_all_serving_cells_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Phy-ParametersFRX-Diff ::= SEQUENCE
struct phy_params_frx_diff_s {
  struct supported_dmrs_type_dl_opts {
    enum options { type1, type1_and2, nulltype } value;

    const char* to_string() const;
  };
  using supported_dmrs_type_dl_e_ = enumerated<supported_dmrs_type_dl_opts>;
  struct supported_dmrs_type_ul_opts {
    enum options { type1, type1_and2, nulltype } value;

    const char* to_string() const;
  };
  using supported_dmrs_type_ul_e_ = enumerated<supported_dmrs_type_ul_opts>;
  struct mux_sr_harq_ack_csi_pucch_once_per_slot_s_ {
    bool same_symbol_present = false;
    bool diff_symbol_present = false;
  };
  struct pdcch_blind_detection_nrdc_s_ {
    uint8_t pdcch_blind_detection_mcg_ue = 1;
    uint8_t pdcch_blind_detection_scg_ue = 1;
  };
  struct max_total_res_for_one_freq_range_r16_s_ {
    struct max_num_res_within_slot_across_cc_one_fr_r16_opts {
      enum options { n2, n4, n8, n12, n16, n32, n64, n128, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using max_num_res_within_slot_across_cc_one_fr_r16_e_ =
        enumerated<max_num_res_within_slot_across_cc_one_fr_r16_opts>;
    struct max_num_res_across_cc_one_fr_r16_opts {
      enum options { n2, n4, n8, n12, n16, n32, n40, n48, n64, n72, n80, n96, n128, n256, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using max_num_res_across_cc_one_fr_r16_e_ = enumerated<max_num_res_across_cc_one_fr_r16_opts>;

    // member variables
    bool                                            max_num_res_within_slot_across_cc_one_fr_r16_present = false;
    bool                                            max_num_res_across_cc_one_fr_r16_present             = false;
    max_num_res_within_slot_across_cc_one_fr_r16_e_ max_num_res_within_slot_across_cc_one_fr_r16;
    max_num_res_across_cc_one_fr_r16_e_             max_num_res_across_cc_one_fr_r16;
  };

  // member variables
  bool                      ext                                              = false;
  bool                      dyn_sfi_present                                  = false;
  bool                      dummy1_present                                   = false;
  bool                      two_fl_dmrs_present                              = false;
  bool                      dummy2_present                                   = false;
  bool                      dummy3_present                                   = false;
  bool                      supported_dmrs_type_dl_present                   = false;
  bool                      supported_dmrs_type_ul_present                   = false;
  bool                      semi_open_loop_csi_present                       = false;
  bool                      csi_report_without_pmi_present                   = false;
  bool                      csi_report_without_cqi_present                   = false;
  bool                      one_ports_ptrs_present                           = false;
  bool                      two_pucch_f0_2_consec_symbols_present            = false;
  bool                      pucch_f2_with_fh_present                         = false;
  bool                      pucch_f3_with_fh_present                         = false;
  bool                      pucch_f4_with_fh_present                         = false;
  bool                      pucch_f0_2_without_fh_present                    = false;
  bool                      pucch_f1_3_4_without_fh_present                  = false;
  bool                      mux_sr_harq_ack_csi_pucch_multi_per_slot_present = false;
  bool                      uci_code_block_segmentation_present              = false;
  bool                      one_pucch_long_and_short_format_present          = false;
  bool                      two_pucch_any_others_in_slot_present             = false;
  bool                      intra_slot_freq_hop_pusch_present                = false;
  bool                      pusch_lbrm_present                               = false;
  bool                      pdcch_blind_detection_ca_present                 = false;
  bool                      tpc_pusch_rnti_present                           = false;
  bool                      tpc_pucch_rnti_present                           = false;
  bool                      tpc_srs_rnti_present                             = false;
  bool                      absolute_tpc_cmd_present                         = false;
  bool                      two_different_tpc_loop_pusch_present             = false;
  bool                      two_different_tpc_loop_pucch_present             = false;
  bool                      pusch_half_pi_bpsk_present                       = false;
  bool                      pucch_f3_4_half_pi_bpsk_present                  = false;
  bool                      almost_contiguous_cp_ofdm_ul_present             = false;
  bool                      sp_csi_rs_present                                = false;
  bool                      sp_csi_im_present                                = false;
  bool                      tdd_multi_dl_ul_switch_per_slot_present          = false;
  bool                      multiple_coreset_present                         = false;
  fixed_bitstring<2>        dummy1;
  fixed_bitstring<2>        two_fl_dmrs;
  fixed_bitstring<2>        dummy2;
  fixed_bitstring<2>        dummy3;
  supported_dmrs_type_dl_e_ supported_dmrs_type_dl;
  supported_dmrs_type_ul_e_ supported_dmrs_type_ul;
  fixed_bitstring<2>        one_ports_ptrs;
  uint8_t                   pdcch_blind_detection_ca = 4;
  // ...
  // group 0
  bool                                                 mux_sr_harq_ack_pucch_present              = false;
  bool                                                 mux_multiple_group_ctrl_ch_overlap_present = false;
  bool                                                 dl_sched_offset_pdsch_type_a_present       = false;
  bool                                                 dl_sched_offset_pdsch_type_b_present       = false;
  bool                                                 ul_sched_offset_present                    = false;
  bool                                                 dl_64_qam_mcs_table_alt_present            = false;
  bool                                                 ul_64_qam_mcs_table_alt_present            = false;
  bool                                                 cqi_table_alt_present                      = false;
  bool                                                 one_fl_dmrs_two_add_dmrs_ul_present        = false;
  bool                                                 two_fl_dmrs_two_add_dmrs_ul_present        = false;
  bool                                                 one_fl_dmrs_three_add_dmrs_ul_present      = false;
  copy_ptr<csi_rs_im_reception_for_feedback_s>         csi_rs_im_reception_for_feedback;
  copy_ptr<csi_rs_proc_framework_for_srs_s>            csi_rs_proc_framework_for_srs;
  copy_ptr<csi_report_framework_s>                     csi_report_framework;
  copy_ptr<mux_sr_harq_ack_csi_pucch_once_per_slot_s_> mux_sr_harq_ack_csi_pucch_once_per_slot;
  // group 1
  bool                                    mux_harq_ack_pusch_diff_symbol_present = false;
  copy_ptr<pdcch_blind_detection_nrdc_s_> pdcch_blind_detection_nrdc;
  // group 2
  bool                                              type1_harq_ack_codebook_r16_present            = false;
  bool                                              enhanced_pwr_ctrl_r16_present                  = false;
  bool                                              simul_tci_act_multiple_cc_r16_present          = false;
  bool                                              simul_spatial_relation_multiple_cc_r16_present = false;
  bool                                              cli_rssi_fdm_dl_r16_present                    = false;
  bool                                              cli_srs_rsrp_fdm_dl_r16_present                = false;
  bool                                              max_layers_mimo_adaptation_r16_present         = false;
  bool                                              aggregation_factor_sps_dl_r16_present          = false;
  copy_ptr<max_total_res_for_one_freq_range_r16_s_> max_total_res_for_one_freq_range_r16;
  copy_ptr<csi_report_framework_ext_r16_s>          csi_report_framework_ext_r16;
  // group 3
  bool two_tci_act_serving_cell_in_cc_list_r16_present = false;
  // group 4
  bool cri_ri_cqi_without_non_pmi_port_ind_r16_present = false;
  // group 5
  bool cqi_4_bits_subband_tn_non_shared_spec_ch_access_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Phy-ParametersXDD-Diff ::= SEQUENCE
struct phy_params_xdd_diff_s {
  bool ext                                   = false;
  bool dyn_sfi_present                       = false;
  bool two_pucch_f0_2_consec_symbols_present = false;
  bool two_different_tpc_loop_pusch_present  = false;
  bool two_different_tpc_loop_pucch_present  = false;
  // ...
  // group 0
  bool dl_sched_offset_pdsch_type_a_present = false;
  bool dl_sched_offset_pdsch_type_b_present = false;
  bool ul_sched_offset_present              = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Phy-Parameters ::= SEQUENCE
struct phy_params_s {
  bool                  phy_params_common_present   = false;
  bool                  phy_params_xdd_diff_present = false;
  bool                  phy_params_frx_diff_present = false;
  bool                  phy_params_fr1_present      = false;
  bool                  phy_params_fr2_present      = false;
  phy_params_common_s   phy_params_common;
  phy_params_xdd_diff_s phy_params_xdd_diff;
  phy_params_frx_diff_s phy_params_frx_diff;
  phy_params_fr1_s      phy_params_fr1;
  phy_params_fr2_s      phy_params_fr2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SON-Parameters-r16 ::= SEQUENCE
struct son_params_r16_s {
  bool ext                     = false;
  bool rach_report_r16_present = false;
  // ...
  // group 0
  bool rlf_report_cho_r17_present       = false;
  bool rlf_report_daps_r17_present      = false;
  bool success_ho_report_r17_present    = false;
  bool two_step_rach_report_r17_present = false;
  bool pscell_mhi_report_r17_present    = false;
  bool on_demand_si_report_r17_present  = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-BasedPerfMeas-Parameters-r16 ::= SEQUENCE
struct ue_based_perf_meas_params_r16_s {
  bool ext                                 = false;
  bool barometer_meas_report_r16_present   = false;
  bool imm_meas_bt_r16_present             = false;
  bool imm_meas_wlan_r16_present           = false;
  bool logged_meas_bt_r16_present          = false;
  bool logged_meass_r16_present            = false;
  bool logged_meas_wlan_r16_present        = false;
  bool orientation_meas_report_r16_present = false;
  bool speed_meas_report_r16_present       = false;
  bool gnss_location_r16_present           = false;
  bool ul_pdcp_delay_r16_present           = false;
  // ...
  // group 0
  bool sig_based_log_mdt_override_protect_r17_present = false;
  bool multiple_cef_report_r17_present                = false;
  bool excess_packet_delay_r17_present                = false;
  bool early_meas_log_r17_present                     = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-CapabilityAddFRX-Mode ::= SEQUENCE
struct ue_nr_cap_add_frx_mode_s {
  bool                           phy_params_frx_diff_present          = false;
  bool                           meas_and_mob_params_frx_diff_present = false;
  phy_params_frx_diff_s          phy_params_frx_diff;
  meas_and_mob_params_frx_diff_s meas_and_mob_params_frx_diff;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-CapabilityAddXDD-Mode ::= SEQUENCE
struct ue_nr_cap_add_xdd_mode_s {
  bool                           phy_params_xdd_diff_present          = false;
  bool                           mac_params_xdd_diff_present          = false;
  bool                           meas_and_mob_params_xdd_diff_present = false;
  phy_params_xdd_diff_s          phy_params_xdd_diff;
  mac_params_xdd_diff_s          mac_params_xdd_diff;
  meas_and_mob_params_xdd_diff_s meas_and_mob_params_xdd_diff;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NTN-Parameters-r17 ::= SEQUENCE
struct ntn_params_r17_s {
  bool                            inactive_state_ntn_r17_present            = false;
  bool                            ra_sdt_ntn_r17_present                    = false;
  bool                            srb_sdt_ntn_r17_present                   = false;
  bool                            meas_and_mob_params_ntn_r17_present       = false;
  bool                            mac_params_ntn_r17_present                = false;
  bool                            phy_params_ntn_r17_present                = false;
  bool                            fdd_add_ue_nr_cap_ntn_r17_present         = false;
  bool                            fr1_add_ue_nr_cap_ntn_r17_present         = false;
  bool                            ue_based_perf_meas_params_ntn_r17_present = false;
  bool                            son_params_ntn_r17_present                = false;
  meas_and_mob_params_s           meas_and_mob_params_ntn_r17;
  mac_params_s                    mac_params_ntn_r17;
  phy_params_s                    phy_params_ntn_r17;
  ue_nr_cap_add_xdd_mode_s        fdd_add_ue_nr_cap_ntn_r17;
  ue_nr_cap_add_frx_mode_s        fr1_add_ue_nr_cap_ntn_r17;
  ue_based_perf_meas_params_r16_s ue_based_perf_meas_params_ntn_r17;
  son_params_r16_s                son_params_ntn_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-Parameters ::= SEQUENCE
struct pdcp_params_s {
  struct supported_rohc_profiles_s_ {
    bool profile0x0000 = false;
    bool profile0x0001 = false;
    bool profile0x0002 = false;
    bool profile0x0003 = false;
    bool profile0x0004 = false;
    bool profile0x0006 = false;
    bool profile0x0101 = false;
    bool profile0x0102 = false;
    bool profile0x0103 = false;
    bool profile0x0104 = false;
  };
  struct max_num_rohc_context_sessions_opts {
    enum options {
      cs2,
      cs4,
      cs8,
      cs12,
      cs16,
      cs24,
      cs32,
      cs48,
      cs64,
      cs128,
      cs256,
      cs512,
      cs1024,
      cs16384,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using max_num_rohc_context_sessions_e_ = enumerated<max_num_rohc_context_sessions_opts>;
  struct max_num_ehc_contexts_r16_opts {
    enum options {
      cs2,
      cs4,
      cs8,
      cs16,
      cs32,
      cs64,
      cs128,
      cs256,
      cs512,
      cs1024,
      cs2048,
      cs4096,
      cs8192,
      cs16384,
      cs32768,
      cs65536,
      nulltype
    } value;
    typedef uint32_t number_type;

    const char* to_string() const;
    uint32_t    to_number() const;
  };
  using max_num_ehc_contexts_r16_e_ = enumerated<max_num_ehc_contexts_r16_opts>;
  struct udc_r17_s_ {
    struct operator_dictionary_r17_s_ {
      uint8_t   version_of_dictionary_r17 = 0;
      plmn_id_s associated_plmn_id_r17;
    };
    struct support_of_buffer_size_r17_opts {
      enum options { kbyte4, kbyte8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using support_of_buffer_size_r17_e_ = enumerated<support_of_buffer_size_r17_opts>;

    // member variables
    bool                          standard_dictionary_r17_present    = false;
    bool                          operator_dictionary_r17_present    = false;
    bool                          continue_udc_r17_present           = false;
    bool                          support_of_buffer_size_r17_present = false;
    operator_dictionary_r17_s_    operator_dictionary_r17;
    support_of_buffer_size_r17_e_ support_of_buffer_size_r17;
  };

  // member variables
  bool                             ext                              = false;
  bool                             ul_only_rohc_profiles_present    = false;
  bool                             continue_rohc_context_present    = false;
  bool                             out_of_order_delivery_present    = false;
  bool                             short_sn_present                 = false;
  bool                             pdcp_dupl_srb_present            = false;
  bool                             pdcp_dupl_mcg_or_scg_drb_present = false;
  supported_rohc_profiles_s_       supported_rohc_profiles;
  max_num_rohc_context_sessions_e_ max_num_rohc_context_sessions;
  // ...
  // group 0
  bool                        drb_iab_r16_present                     = false;
  bool                        non_drb_iab_r16_present                 = false;
  bool                        extended_discard_timer_r16_present      = false;
  bool                        continue_ehc_context_r16_present        = false;
  bool                        ehc_r16_present                         = false;
  bool                        max_num_ehc_contexts_r16_present        = false;
  bool                        joint_ehc_rohc_cfg_r16_present          = false;
  bool                        pdcp_dupl_more_than_two_rlc_r16_present = false;
  max_num_ehc_contexts_r16_e_ max_num_ehc_contexts_r16;
  // group 1
  bool                 long_sn_red_cap_r17_present = false;
  copy_ptr<udc_r17_s_> udc_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PowSav-ParametersCommon-r16 ::= SEQUENCE
struct pow_sav_params_common_r16_s {
  bool ext                               = false;
  bool drx_pref_r16_present              = false;
  bool max_cc_pref_r16_present           = false;
  bool release_pref_r16_present          = false;
  bool min_sched_offset_pref_r16_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PowSav-ParametersFRX-Diff-r16 ::= SEQUENCE
struct pow_sav_params_frx_diff_r16_s {
  bool ext                             = false;
  bool max_bw_pref_r16_present         = false;
  bool max_mimo_layer_pref_r16_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PowSav-Parameters-r16 ::= SEQUENCE
struct pow_sav_params_r16_s {
  bool                          ext                                 = false;
  bool                          pow_sav_params_common_r16_present   = false;
  bool                          pow_sav_params_frx_diff_r16_present = false;
  pow_sav_params_common_r16_s   pow_sav_params_common_r16;
  pow_sav_params_frx_diff_r16_s pow_sav_params_frx_diff_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PowSav-ParametersFR2-2-r17 ::= SEQUENCE
struct pow_sav_params_fr2_2_r17_s {
  bool ext                             = false;
  bool max_bw_pref_r17_present         = false;
  bool max_mimo_layer_pref_r17_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PowSav-Parameters-v1700 ::= SEQUENCE
struct pow_sav_params_v1700_s {
  bool                       ext                              = false;
  bool                       pow_sav_params_fr2_2_r17_present = false;
  pow_sav_params_fr2_2_r17_s pow_sav_params_fr2_2_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RF-Parameters ::= SEQUENCE
struct rf_params_s {
  using supported_band_list_nr_l_ = dyn_array<band_nr_s>;

  // member variables
  bool                      ext = false;
  supported_band_list_nr_l_ supported_band_list_nr;
  band_combination_list_l   supported_band_combination_list;
  freq_band_list_l          applied_freq_band_list_filt;
  // ...
  // group 0
  bool                                    srs_switching_time_requested_present = false;
  copy_ptr<band_combination_list_v1540_l> supported_band_combination_list_v1540;
  // group 1
  copy_ptr<band_combination_list_v1550_l> supported_band_combination_list_v1550;
  // group 2
  copy_ptr<band_combination_list_v1560_l> supported_band_combination_list_v1560;
  // group 3
  copy_ptr<band_combination_list_v1610_l>                 supported_band_combination_list_v1610;
  copy_ptr<band_combination_list_sidelink_eutra_nr_r16_l> supported_band_combination_list_sidelink_eutra_nr_r16;
  copy_ptr<band_combination_list_ul_tx_switch_r16_l>      supported_band_combination_list_ul_tx_switch_r16;
  // group 4
  copy_ptr<band_combination_list_v1630_l>                   supported_band_combination_list_v1630;
  copy_ptr<band_combination_list_sidelink_eutra_nr_v1630_l> supported_band_combination_list_sidelink_eutra_nr_v1630;
  copy_ptr<band_combination_list_ul_tx_switch_v1630_l>      supported_band_combination_list_ul_tx_switch_v1630;
  // group 5
  copy_ptr<band_combination_list_v1640_l>              supported_band_combination_list_v1640;
  copy_ptr<band_combination_list_ul_tx_switch_v1640_l> supported_band_combination_list_ul_tx_switch_v1640;
  // group 6
  copy_ptr<band_combination_list_v1650_l>              supported_band_combination_list_v1650;
  copy_ptr<band_combination_list_ul_tx_switch_v1650_l> supported_band_combination_list_ul_tx_switch_v1650;
  // group 7
  bool extended_band_n77_r16_present = false;
  // group 8
  copy_ptr<band_combination_list_ul_tx_switch_v1670_l> supported_band_combination_list_ul_tx_switch_v1670;
  // group 9
  copy_ptr<band_combination_list_v1680_l> supported_band_combination_list_v1680;
  // group 10
  copy_ptr<band_combination_list_v1690_l>              supported_band_combination_list_v1690;
  copy_ptr<band_combination_list_ul_tx_switch_v1690_l> supported_band_combination_list_ul_tx_switch_v1690;
  // group 11
  bool                                                      sidelink_requested_r17_present  = false;
  bool                                                      extended_band_n77_2_r17_present = false;
  copy_ptr<band_combination_list_v1700_l>                   supported_band_combination_list_v1700;
  copy_ptr<band_combination_list_ul_tx_switch_v1700_l>      supported_band_combination_list_ul_tx_switch_v1700;
  dyn_octstring                                             supported_band_combination_list_sl_relay_discovery_r17;
  dyn_octstring                                             supported_band_combination_list_sl_non_relay_discovery_r17;
  copy_ptr<band_combination_list_sidelink_eutra_nr_v1710_l> supported_band_combination_list_sidelink_eutra_nr_v1710;
  // group 12
  copy_ptr<band_combination_list_v1720_l>              supported_band_combination_list_v1720;
  copy_ptr<band_combination_list_ul_tx_switch_v1720_l> supported_band_combination_list_ul_tx_switch_v1720;
  // group 13
  copy_ptr<band_combination_list_v1730_l>              supported_band_combination_list_v1730;
  copy_ptr<band_combination_list_ul_tx_switch_v1730_l> supported_band_combination_list_ul_tx_switch_v1730;
  copy_ptr<band_combination_list_sl_discovery_r17_l>   supported_band_combination_list_sl_relay_discovery_v1730;
  copy_ptr<band_combination_list_sl_discovery_r17_l>   supported_band_combination_list_sl_non_relay_discovery_v1730;
  // group 14
  copy_ptr<band_combination_list_v1740_l>              supported_band_combination_list_v1740;
  copy_ptr<band_combination_list_ul_tx_switch_v1740_l> supported_band_combination_list_ul_tx_switch_v1740;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-ParametersSidelink-r16 ::= SEQUENCE
struct rlc_params_sidelink_r16_s {
  bool ext                                  = false;
  bool am_with_long_sn_sidelink_r16_present = false;
  bool um_with_long_sn_sidelink_r16_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RelayParameters-r17 ::= SEQUENCE
struct relay_params_r17_s {
  bool ext                                                      = false;
  bool relay_ue_operation_l2_r17_present                        = false;
  bool remote_ue_operation_l2_r17_present                       = false;
  bool remote_ue_path_switch_to_idle_inactive_relay_r17_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-SidelinkCapabilityAddXDD-Mode-r16 ::= SEQUENCE
struct ue_sidelink_cap_add_xdd_mode_r16_s {
  bool                               mac_params_sidelink_xdd_diff_r16_present = false;
  mac_params_sidelink_xdd_diff_r16_s mac_params_sidelink_xdd_diff_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SidelinkParametersEUTRA-r16 ::= SEQUENCE
struct sidelink_params_eutra_r16_s {
  using supported_band_list_sidelink_eutra_r16_l_ = dyn_array<band_sidelink_eutra_r16_s>;

  // member variables
  bool                                      ext = false;
  dyn_octstring                             sl_params_eutra1_r16;
  dyn_octstring                             sl_params_eutra2_r16;
  dyn_octstring                             sl_params_eutra3_r16;
  supported_band_list_sidelink_eutra_r16_l_ supported_band_list_sidelink_eutra_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SidelinkParametersNR-r16 ::= SEQUENCE
struct sidelink_params_nr_r16_s {
  using supported_band_list_sidelink_r16_l_ = dyn_array<band_sidelink_r16_s>;

  // member variables
  bool                                ext                                 = false;
  bool                                rlc_params_sidelink_r16_present     = false;
  bool                                mac_params_sidelink_r16_present     = false;
  bool                                fdd_add_ue_sidelink_cap_r16_present = false;
  bool                                tdd_add_ue_sidelink_cap_r16_present = false;
  rlc_params_sidelink_r16_s           rlc_params_sidelink_r16;
  mac_params_sidelink_r16_s           mac_params_sidelink_r16;
  ue_sidelink_cap_add_xdd_mode_r16_s  fdd_add_ue_sidelink_cap_r16;
  ue_sidelink_cap_add_xdd_mode_r16_s  tdd_add_ue_sidelink_cap_r16;
  supported_band_list_sidelink_r16_l_ supported_band_list_sidelink_r16;
  // ...
  // group 0
  copy_ptr<relay_params_r17_s> relay_params_r17;
  // group 1
  bool p0_olpc_sidelink_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SidelinkParameters-r16 ::= SEQUENCE
struct sidelink_params_r16_s {
  bool                        sidelink_params_nr_r16_present    = false;
  bool                        sidelink_params_eutra_r16_present = false;
  sidelink_params_nr_r16_s    sidelink_params_nr_r16;
  sidelink_params_eutra_r16_s sidelink_params_eutra_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RedCapParameters-v1740 ::= SEQUENCE
struct red_cap_params_v1740_s {
  bool ncd_ssb_for_red_cap_init_bwp_sdt_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AppLayerMeasParameters-r17 ::= SEQUENCE
struct app_layer_meas_params_r17_s {
  bool ext                                                = false;
  bool qoe_streaming_meas_report_r17_present              = false;
  bool qoe_mt_si_meas_report_r17_present                  = false;
  bool qoe_vr_meas_report_r17_present                     = false;
  bool ran_visible_qo_e_streaming_meas_report_r17_present = false;
  bool ran_visible_qo_e_vr_meas_report_r17_present        = false;
  bool ul_meas_report_app_layer_seg_r17_present           = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BAP-Parameters-v1700 ::= SEQUENCE
struct bap_params_v1700_s {
  bool bap_hdr_rewriting_rerouting_r17_present = false;
  bool bap_hdr_rewriting_routing_r17_present   = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HighSpeedParameters-v1700 ::= SEQUENCE
struct high_speed_params_v1700_s {
  bool meas_enhancement_ca_r17_present         = false;
  bool meas_enhancement_inter_freq_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBS-Parameters-r17 ::= SEQUENCE
struct mbs_params_r17_s {
  bool    max_mrb_add_r17_present = false;
  uint8_t max_mrb_add_r17         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RedCapParameters-r17 ::= SEQUENCE
struct red_cap_params_r17_s {
  bool support_of_red_cap_r17_present       = false;
  bool support_of16_drb_red_cap_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability-v1740 ::= SEQUENCE
struct ue_nr_cap_v1740_s {
  bool                   non_crit_ext_present = false;
  red_cap_params_v1740_s red_cap_params_v1740;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-RadioPagingInfo-r17 ::= SEQUENCE
struct ue_radio_paging_info_r17_s {
  using pei_subgrouping_support_band_list_r17_l_ = dyn_array<uint16_t>;

  // member variables
  bool                                     ext = false;
  pei_subgrouping_support_band_list_r17_l_ pei_subgrouping_support_band_list_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability-v1700 ::= SEQUENCE
struct ue_nr_cap_v1700_s {
  struct ntn_scenario_support_r17_opts {
    enum options { gso, ngso, nulltype } value;

    const char* to_string() const;
  };
  using ntn_scenario_support_r17_e_ = enumerated<ntn_scenario_support_r17_opts>;

  // member variables
  bool                        inactive_state_po_determination_r17_present = false;
  bool                        high_speed_params_v1700_present             = false;
  bool                        pow_sav_params_v1700_present                = false;
  bool                        mac_params_v1700_present                    = false;
  bool                        ims_params_v1700_present                    = false;
  bool                        app_layer_meas_params_r17_present           = false;
  bool                        red_cap_params_r17_present                  = false;
  bool                        ra_sdt_r17_present                          = false;
  bool                        srb_sdt_r17_present                         = false;
  bool                        gnb_side_rtt_based_pdc_r17_present          = false;
  bool                        bh_rlf_detection_recovery_ind_r17_present   = false;
  bool                        nrdc_params_v1700_present                   = false;
  bool                        bap_params_v1700_present                    = false;
  bool                        musim_gap_pref_r17_present                  = false;
  bool                        musim_leave_connected_r17_present           = false;
  bool                        non_terrestrial_network_r17_present         = false;
  bool                        ntn_scenario_support_r17_present            = false;
  bool                        slice_infofor_cell_resel_r17_present        = false;
  bool                        ue_radio_paging_info_r17_present            = false;
  bool                        ul_gap_fr2_pattern_r17_present              = false;
  bool                        ntn_params_r17_present                      = false;
  bool                        non_crit_ext_present                        = false;
  high_speed_params_v1700_s   high_speed_params_v1700;
  pow_sav_params_v1700_s      pow_sav_params_v1700;
  mac_params_v1700_s          mac_params_v1700;
  ims_params_v1700_s          ims_params_v1700;
  meas_and_mob_params_v1700_s meas_and_mob_params_v1700;
  app_layer_meas_params_r17_s app_layer_meas_params_r17;
  red_cap_params_r17_s        red_cap_params_r17;
  nrdc_params_v1700_s         nrdc_params_v1700;
  bap_params_v1700_s          bap_params_v1700;
  mbs_params_r17_s            mbs_params_r17;
  ntn_scenario_support_r17_e_ ntn_scenario_support_r17;
  ue_radio_paging_info_r17_s  ue_radio_paging_info_r17;
  fixed_bitstring<4>          ul_gap_fr2_pattern_r17;
  ntn_params_r17_s            ntn_params_r17;
  ue_nr_cap_v1740_s           non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HighSpeedParameters-v1650 ::= CHOICE
struct high_speed_params_v1650_c {
  struct types_opts {
    enum options { intra_nr_meas_enhancement_r16, inter_rat_meas_enhancement_r16, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  high_speed_params_v1650_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  void set_intra_nr_meas_enhancement_r16();
  void set_inter_rat_meas_enhancement_r16();

private:
  types type_;
};

// UE-NR-Capability-v1690 ::= SEQUENCE
struct ue_nr_cap_v1690_s {
  bool              ul_rrc_segmentation_r16_present = false;
  bool              non_crit_ext_present            = false;
  ue_nr_cap_v1700_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Phy-ParametersSharedSpectrumChAccess-r16 ::= SEQUENCE
struct phy_params_shared_spec_ch_access_r16_s {
  struct mux_sr_harq_ack_csi_pucch_once_per_slot_r16_s_ {
    bool same_symbol_r16_present = false;
    bool diff_symbol_r16_present = false;
  };

  // member variables
  bool                                           ext                                                  = false;
  bool                                           ss_sinr_meas_r16_present                             = false;
  bool                                           sp_csi_report_pucch_r16_present                      = false;
  bool                                           sp_csi_report_pusch_r16_present                      = false;
  bool                                           dyn_sfi_r16_present                                  = false;
  bool                                           mux_sr_harq_ack_csi_pucch_once_per_slot_r16_present  = false;
  bool                                           mux_sr_harq_ack_pucch_r16_present                    = false;
  bool                                           mux_sr_harq_ack_csi_pucch_multi_per_slot_r16_present = false;
  bool                                           mux_harq_ack_pusch_diff_symbol_r16_present           = false;
  bool                                           pucch_repeat_f1_3_4_r16_present                      = false;
  bool                                           type1_pusch_repeat_multi_slots_r16_present           = false;
  bool                                           type2_pusch_repeat_multi_slots_r16_present           = false;
  bool                                           pusch_repeat_multi_slots_r16_present                 = false;
  bool                                           pdsch_repeat_multi_slots_r16_present                 = false;
  bool                                           dl_sps_r16_present                                   = false;
  bool                                           cfg_ul_grant_type1_r16_present                       = false;
  bool                                           cfg_ul_grant_type2_r16_present                       = false;
  bool                                           pre_empt_ind_dl_r16_present                          = false;
  mux_sr_harq_ack_csi_pucch_once_per_slot_r16_s_ mux_sr_harq_ack_csi_pucch_once_per_slot_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability-v1650 ::= SEQUENCE
struct ue_nr_cap_v1650_s {
  bool                      mps_prio_ind_r16_present        = false;
  bool                      high_speed_params_v1650_present = false;
  bool                      non_crit_ext_present            = false;
  high_speed_params_v1650_c high_speed_params_v1650;
  ue_nr_cap_v1690_s         non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BAP-Parameters-r16 ::= SEQUENCE
struct bap_params_r16_s {
  bool flow_ctrl_bh_rlc_ch_based_r16_present  = false;
  bool flow_ctrl_routing_id_based_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HighSpeedParameters-r16 ::= SEQUENCE
struct high_speed_params_r16_s {
  bool meas_enhancement_r16_present  = false;
  bool demod_enhancement_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability-v1640 ::= SEQUENCE
struct ue_nr_cap_v1640_s {
  bool                                   redirect_at_resume_by_nas_r16_present        = false;
  bool                                   phy_params_shared_spec_ch_access_r16_present = false;
  bool                                   non_crit_ext_present                         = false;
  phy_params_shared_spec_ch_access_r16_s phy_params_shared_spec_ch_access_r16;
  ue_nr_cap_v1650_s                      non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-CapabilityAddFRX-Mode-v1610 ::= SEQUENCE
struct ue_nr_cap_add_frx_mode_v1610_s {
  bool                          pow_sav_params_frx_diff_r16_present = false;
  bool                          mac_params_frx_diff_r16_present     = false;
  pow_sav_params_frx_diff_r16_s pow_sav_params_frx_diff_r16;
  mac_params_frx_diff_r16_s     mac_params_frx_diff_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NRDC-Parameters-v1570 ::= SEQUENCE
struct nrdc_params_v1570_s {
  bool sfn_sync_nrdc_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability-v1610 ::= SEQUENCE
struct ue_nr_cap_v1610_s {
  bool                            in_dev_coex_ind_r16_present                  = false;
  bool                            dl_ded_msg_segmentation_r16_present          = false;
  bool                            nrdc_params_v1610_present                    = false;
  bool                            pow_sav_params_r16_present                   = false;
  bool                            fr1_add_ue_nr_cap_v1610_present              = false;
  bool                            fr2_add_ue_nr_cap_v1610_present              = false;
  bool                            bh_rlf_ind_r16_present                       = false;
  bool                            direct_sn_addition_first_rrc_iab_r16_present = false;
  bool                            bap_params_r16_present                       = false;
  bool                            ref_time_provision_r16_present               = false;
  bool                            sidelink_params_r16_present                  = false;
  bool                            high_speed_params_r16_present                = false;
  bool                            mac_params_v1610_present                     = false;
  bool                            mcg_rlf_recovery_via_scg_r16_present         = false;
  bool                            resume_with_stored_mcg_scells_r16_present    = false;
  bool                            resume_with_stored_scg_r16_present           = false;
  bool                            resume_with_scg_cfg_r16_present              = false;
  bool                            ue_based_perf_meas_params_r16_present        = false;
  bool                            son_params_r16_present                       = false;
  bool                            on_demand_sib_connected_r16_present          = false;
  bool                            non_crit_ext_present                         = false;
  nrdc_params_v1610_s             nrdc_params_v1610;
  pow_sav_params_r16_s            pow_sav_params_r16;
  ue_nr_cap_add_frx_mode_v1610_s  fr1_add_ue_nr_cap_v1610;
  ue_nr_cap_add_frx_mode_v1610_s  fr2_add_ue_nr_cap_v1610;
  bap_params_r16_s                bap_params_r16;
  sidelink_params_r16_s           sidelink_params_r16;
  high_speed_params_r16_s         high_speed_params_r16;
  mac_params_v1610_s              mac_params_v1610;
  ue_based_perf_meas_params_r16_s ue_based_perf_meas_params_r16;
  son_params_r16_s                son_params_r16;
  ue_nr_cap_v1640_s               non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability-v1570 ::= SEQUENCE
struct ue_nr_cap_v1570_s {
  bool                nrdc_params_v1570_present = false;
  bool                non_crit_ext_present      = false;
  nrdc_params_v1570_s nrdc_params_v1570;
  ue_nr_cap_v1610_s   non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability-v1560 ::= SEQUENCE
struct ue_nr_cap_v1560_s {
  bool              nrdc_params_present  = false;
  bool              non_crit_ext_present = false;
  nrdc_params_s     nrdc_params;
  dyn_octstring     rx_filts;
  ue_nr_cap_v1570_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SDAP-Parameters ::= SEQUENCE
struct sdap_params_s {
  bool ext                       = false;
  bool as_reflective_qos_present = false;
  // ...
  // group 0
  bool sdap_qos_iab_r16_present = false;
  bool sdap_hdr_iab_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability-v1550 ::= SEQUENCE
struct ue_nr_cap_v1550_s {
  bool              reduced_cp_latency_present = false;
  bool              non_crit_ext_present       = false;
  ue_nr_cap_v1560_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-CapabilityAddFRX-Mode-v1540 ::= SEQUENCE
struct ue_nr_cap_add_frx_mode_v1540_s {
  bool                  ims_params_frx_diff_present = false;
  ims_params_frx_diff_s ims_params_frx_diff;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability-v1540 ::= SEQUENCE
struct ue_nr_cap_v1540_s {
  bool                           sdap_params_present             = false;
  bool                           overheat_ind_present            = false;
  bool                           ims_params_present              = false;
  bool                           fr1_add_ue_nr_cap_v1540_present = false;
  bool                           fr2_add_ue_nr_cap_v1540_present = false;
  bool                           fr1_fr2_add_ue_nr_cap_present   = false;
  bool                           non_crit_ext_present            = false;
  sdap_params_s                  sdap_params;
  ims_params_s                   ims_params;
  ue_nr_cap_add_frx_mode_v1540_s fr1_add_ue_nr_cap_v1540;
  ue_nr_cap_add_frx_mode_v1540_s fr2_add_ue_nr_cap_v1540;
  ue_nr_cap_add_frx_mode_s       fr1_fr2_add_ue_nr_cap;
  ue_nr_cap_v1550_s              non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-CapabilityAddXDD-Mode-v1530 ::= SEQUENCE
struct ue_nr_cap_add_xdd_mode_v1530_s {
  eutra_params_xdd_diff_s eutra_params_xdd_diff;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AccessStratumRelease ::= ENUMERATED
struct access_stratum_release_opts {
  enum options { rel15, rel16, rel17, spare5, spare4, spare3, spare2, spare1, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using access_stratum_release_e = enumerated<access_stratum_release_opts, true>;

// RLC-Parameters ::= SEQUENCE
struct rlc_params_s {
  bool ext                      = false;
  bool am_with_short_sn_present = false;
  bool um_with_short_sn_present = false;
  bool um_with_long_sn_present  = false;
  // ...
  // group 0
  bool extended_t_poll_retx_r16_present       = false;
  bool extended_t_status_prohibit_r16_present = false;
  // group 1
  bool am_with_long_sn_red_cap_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability-v1530 ::= SEQUENCE
struct ue_nr_cap_v1530_s {
  bool                           fdd_add_ue_nr_cap_v1530_present = false;
  bool                           tdd_add_ue_nr_cap_v1530_present = false;
  bool                           dummy_present                   = false;
  bool                           inter_rat_params_present        = false;
  bool                           inactive_state_present          = false;
  bool                           delay_budget_report_present     = false;
  bool                           non_crit_ext_present            = false;
  ue_nr_cap_add_xdd_mode_v1530_s fdd_add_ue_nr_cap_v1530;
  ue_nr_cap_add_xdd_mode_v1530_s tdd_add_ue_nr_cap_v1530;
  inter_rat_params_s             inter_rat_params;
  ue_nr_cap_v1540_s              non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NR-Capability ::= SEQUENCE
struct ue_nr_cap_s {
  using feature_set_combinations_l_ = dyn_array<feature_set_combination_l>;

  // member variables
  bool                        rlc_params_present          = false;
  bool                        mac_params_present          = false;
  bool                        meas_and_mob_params_present = false;
  bool                        fdd_add_ue_nr_cap_present   = false;
  bool                        tdd_add_ue_nr_cap_present   = false;
  bool                        fr1_add_ue_nr_cap_present   = false;
  bool                        fr2_add_ue_nr_cap_present   = false;
  bool                        feature_sets_present        = false;
  bool                        non_crit_ext_present        = false;
  access_stratum_release_e    access_stratum_release;
  pdcp_params_s               pdcp_params;
  rlc_params_s                rlc_params;
  mac_params_s                mac_params;
  phy_params_s                phy_params;
  rf_params_s                 rf_params;
  meas_and_mob_params_s       meas_and_mob_params;
  ue_nr_cap_add_xdd_mode_s    fdd_add_ue_nr_cap;
  ue_nr_cap_add_xdd_mode_s    tdd_add_ue_nr_cap;
  ue_nr_cap_add_frx_mode_s    fr1_add_ue_nr_cap;
  ue_nr_cap_add_frx_mode_s    fr2_add_ue_nr_cap;
  feature_sets_s              feature_sets;
  feature_set_combinations_l_ feature_set_combinations;
  dyn_octstring               late_non_crit_ext;
  ue_nr_cap_v1530_s           non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1
