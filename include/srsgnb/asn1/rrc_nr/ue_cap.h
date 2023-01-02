/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/*******************************************************************************
 *
 *                    3GPP TS ASN1 RRC NR v15.11.0 (2020-10)
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
typedef enumerated<mimo_layers_dl_opts> mimo_layers_dl_e;

// MIMO-LayersUL ::= ENUMERATED
struct mimo_layers_ul_opts {
  enum options { one_layer, two_layers, four_layers, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<mimo_layers_ul_opts> mimo_layers_ul_e;

// CA-BandwidthClassEUTRA ::= ENUMERATED
struct ca_bw_class_eutra_opts {
  enum options { a, b, c, d, e, f, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<ca_bw_class_eutra_opts, true> ca_bw_class_eutra_e;

// CA-BandwidthClassNR ::= ENUMERATED
struct ca_bw_class_nr_opts {
  enum options { a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<ca_bw_class_nr_opts, true> ca_bw_class_nr_e;

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
  typedef enumerated<types_opts> types;

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
  typedef enumerated<supported_num_tag_opts> supported_num_tag_e_;

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
  typedef enumerated<ul_sharing_eutra_nr_opts> ul_sharing_eutra_nr_e_;
  struct ul_switching_time_eutra_nr_opts {
    enum options { type1, type2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ul_switching_time_eutra_nr_opts> ul_switching_time_eutra_nr_e_;
  struct intra_band_endc_support_opts {
    enum options { non_contiguous, both, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<intra_band_endc_support_opts> intra_band_endc_support_e_;

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
  typedef enumerated<switching_time_dl_opts> switching_time_dl_e_;
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
  typedef enumerated<switching_time_ul_opts> switching_time_ul_e_;

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
  typedef enumerated<switching_time_dl_opts> switching_time_dl_e_;
  struct switching_time_ul_opts {
    enum options { n0us, n30us, n100us, n140us, n200us, n300us, n500us, n900us, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<switching_time_ul_opts> switching_time_ul_e_;

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
    typedef enumerated<types_opts> types;

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
    typedef enumerated<supported_srs_tx_port_switch_opts> supported_srs_tx_port_switch_e_;

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

// CA-ParametersNR-v1550 ::= SEQUENCE
struct ca_params_nr_v1550_s {
  bool dummy_present = false;

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

// CA-ParametersNR-v1560 ::= SEQUENCE
struct ca_params_nr_v1560_s {
  bool diff_numerology_within_pucch_group_larger_scs_present = false;

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

// BandCombinationList ::= SEQUENCE (SIZE (1..65536)) OF BandCombination
using band_combination_list_l = dyn_array<band_combination_s>;

// BandCombinationList-v1540 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1540
using band_combination_list_v1540_l = dyn_array<band_combination_v1540_s>;

// BandCombinationList-v1550 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1550
using band_combination_list_v1550_l = dyn_array<band_combination_v1550_s>;

// BandCombinationList-v1560 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1560
using band_combination_list_v1560_l = dyn_array<band_combination_v1560_s>;

// SupportedCSI-RS-Resource ::= SEQUENCE
struct supported_csi_rs_res_s {
  struct max_num_tx_ports_per_res_opts {
    enum options { p2, p4, p8, p12, p16, p24, p32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_tx_ports_per_res_opts> max_num_tx_ports_per_res_e_;

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
  typedef enumerated<max_num_ssb_csi_rs_res_one_tx_opts> max_num_ssb_csi_rs_res_one_tx_e_;
  struct max_num_csi_rs_res_opts {
    enum options { n0, n4, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_csi_rs_res_opts> max_num_csi_rs_res_e_;
  struct max_num_csi_rs_res_two_tx_opts {
    enum options { n0, n4, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_csi_rs_res_two_tx_opts> max_num_csi_rs_res_two_tx_e_;
  struct supported_csi_rs_density_opts {
    enum options { one, three, one_and_three, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<supported_csi_rs_density_opts> supported_csi_rs_density_e_;
  struct max_num_aperiodic_csi_rs_res_opts {
    enum options { n0, n1, n4, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_aperiodic_csi_rs_res_opts> max_num_aperiodic_csi_rs_res_e_;

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
  typedef enumerated<max_cfg_num_csi_im_per_cc_opts> max_cfg_num_csi_im_per_cc_e_;

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
  typedef enumerated<max_num_aperiodic_csi_trigger_state_per_cc_opts> max_num_aperiodic_csi_trigger_state_per_cc_e_;

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

// CodebookParameters ::= SEQUENCE
struct codebook_params_s {
  struct type1_s_ {
    struct single_panel_s_ {
      using supported_csi_rs_res_list_l_ = dyn_array<supported_csi_rs_res_s>;
      struct modes_opts {
        enum options { mode1, mode1and_mode2, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<modes_opts> modes_e_;

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
      typedef enumerated<modes_opts> modes_e_;
      struct nrof_panels_opts {
        enum options { n2, n4, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<nrof_panels_opts> nrof_panels_e_;

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
    typedef enumerated<amplitude_scaling_type_opts> amplitude_scaling_type_e_;

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
    typedef enumerated<amplitude_scaling_type_opts> amplitude_scaling_type_e_;

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

// DummyG ::= SEQUENCE
struct dummy_g_s {
  struct max_num_ssb_csi_rs_res_one_tx_opts {
    enum options { n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_ssb_csi_rs_res_one_tx_opts> max_num_ssb_csi_rs_res_one_tx_e_;
  struct max_num_ssb_csi_rs_res_two_tx_opts {
    enum options { n0, n4, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_ssb_csi_rs_res_two_tx_opts> max_num_ssb_csi_rs_res_two_tx_e_;
  struct supported_csi_rs_density_opts {
    enum options { one, three, one_and_three, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<supported_csi_rs_density_opts> supported_csi_rs_density_e_;

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

// SpatialRelations ::= SEQUENCE
struct spatial_relations_s {
  struct max_num_cfg_spatial_relations_opts {
    enum options { n4, n8, n16, n32, n64, n96, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_cfg_spatial_relations_opts> max_num_cfg_spatial_relations_e_;
  struct max_num_active_spatial_relations_opts {
    enum options { n1, n2, n4, n8, n14, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_active_spatial_relations_opts> max_num_active_spatial_relations_e_;
  struct max_num_dl_rs_qcl_type_d_opts {
    enum options { n1, n2, n4, n8, n14, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_dl_rs_qcl_type_d_opts> max_num_dl_rs_qcl_type_d_e_;

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

// MIMO-ParametersPerBand ::= SEQUENCE
struct mimo_params_per_band_s {
  struct tci_state_pdsch_s_ {
    struct max_num_cfg_tci_states_per_cc_opts {
      enum options { n4, n8, n16, n32, n64, n128, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<max_num_cfg_tci_states_per_cc_opts> max_num_cfg_tci_states_per_cc_e_;
    struct max_num_active_tci_per_bwp_opts {
      enum options { n1, n2, n4, n8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<max_num_active_tci_per_bwp_opts> max_num_active_tci_per_bwp_e_;

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
  typedef enumerated<pusch_trans_coherence_opts> pusch_trans_coherence_e_;
  struct max_num_rx_tx_beam_switch_dl_s_ {
    struct scs_15k_hz_opts {
      enum options { n4, n7, n14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_15k_hz_opts> scs_15k_hz_e_;
    struct scs_30k_hz_opts {
      enum options { n4, n7, n14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_30k_hz_opts> scs_30k_hz_e_;
    struct scs_60k_hz_opts {
      enum options { n4, n7, n14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_60k_hz_opts> scs_60k_hz_e_;
    struct scs_120k_hz_opts {
      enum options { n4, n7, n14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_120k_hz_opts> scs_120k_hz_e_;
    struct scs_240k_hz_opts {
      enum options { n4, n7, n14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_240k_hz_opts> scs_240k_hz_e_;

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
  typedef enumerated<max_num_non_group_beam_report_opts> max_num_non_group_beam_report_e_;
  struct ul_beam_management_s_ {
    struct max_num_srs_res_per_set_bm_opts {
      enum options { n2, n4, n8, n16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<max_num_srs_res_per_set_bm_opts> max_num_srs_res_per_set_bm_e_;

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
    typedef enumerated<scs_15k_hz_opts> scs_15k_hz_e_;
    struct scs_30k_hz_opts {
      enum options { sym4, sym8, sym14, sym28, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_30k_hz_opts> scs_30k_hz_e_;
    struct scs_60k_hz_opts {
      enum options { sym8, sym14, sym28, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_60k_hz_opts> scs_60k_hz_e_;
    struct scs_120k_hz_opts {
      enum options { sym14, sym28, sym56, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_120k_hz_opts> scs_120k_hz_e_;

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
    typedef enumerated<scs_60k_hz_opts> scs_60k_hz_e_;
    struct scs_120k_hz_opts {
      enum options { sym14, sym28, sym48, sym224, sym336, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<scs_120k_hz_opts> scs_120k_hz_e_;

    // member variables
    bool           scs_60k_hz_present  = false;
    bool           scs_120k_hz_present = false;
    scs_60k_hz_e_  scs_60k_hz;
    scs_120k_hz_e_ scs_120k_hz;
  };
  using srs_assoc_csi_rs_l_ = dyn_array<supported_csi_rs_res_s>;

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
  srs_res_s                             dummy5;
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
  typedef enumerated<bwp_same_numerology_opts> bwp_same_numerology_e_;
  struct ue_pwr_class_opts {
    enum options { pc1, pc2, pc3, pc4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ue_pwr_class_opts> ue_pwr_class_e_;
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
    typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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
  typedef enumerated<max_ul_duty_cycle_pc2_fr1_opts> max_ul_duty_cycle_pc2_fr1_e_;
  struct max_ul_duty_cycle_fr2_opts {
    enum options { n15, n20, n25, n30, n40, n50, n60, n70, n80, n90, n100, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_ul_duty_cycle_fr2_opts> max_ul_duty_cycle_fr2_e_;
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
    typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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
  typedef enumerated<types_opts> types;

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
  typedef enumerated<max_num_ports_across_nzp_csi_rs_per_cc_opts> max_num_ports_across_nzp_csi_rs_per_cc_e_;
  struct max_num_cs_im_per_cc_opts {
    enum options { n1, n2, n4, n8, n16, n32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_cs_im_per_cc_opts> max_num_cs_im_per_cc_e_;
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
  typedef enumerated<max_num_simul_csi_rs_act_bwp_all_cc_opts> max_num_simul_csi_rs_act_bwp_all_cc_e_;
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
  typedef enumerated<total_num_ports_simul_csi_rs_act_bwp_all_cc_opts> total_num_ports_simul_csi_rs_act_bwp_all_cc_e_;

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
  typedef enumerated<max_num_tx_ports_per_res_opts> max_num_tx_ports_per_res_e_;
  struct supported_codebook_mode_opts {
    enum options { mode1, mode1_and_mode2, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<supported_codebook_mode_opts> supported_codebook_mode_e_;

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
  typedef enumerated<max_num_tx_ports_per_res_opts> max_num_tx_ports_per_res_e_;
  struct supported_codebook_mode_opts {
    enum options { mode1, mode2, both, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<supported_codebook_mode_opts> supported_codebook_mode_e_;
  struct supported_num_panels_opts {
    enum options { n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<supported_num_panels_opts> supported_num_panels_e_;

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
  typedef enumerated<max_num_tx_ports_per_res_opts> max_num_tx_ports_per_res_e_;
  struct amplitude_scaling_type_opts {
    enum options { wideband, wideband_and_subband, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<amplitude_scaling_type_opts> amplitude_scaling_type_e_;

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
  typedef enumerated<max_num_tx_ports_per_res_opts> max_num_tx_ports_per_res_e_;
  struct amplitude_scaling_type_opts {
    enum options { wideband, wideband_and_subband, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<amplitude_scaling_type_opts> amplitude_scaling_type_e_;

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
  enum options { c1, c2, c3, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<freq_separation_class_opts, true> freq_separation_class_e;

// FeatureSetDownlink ::= SEQUENCE
struct feature_set_dl_s {
  using feature_set_list_per_dl_cc_l_ = bounded_array<uint16_t, 32>;
  struct scaling_factor_opts {
    enum options { f0p4, f0p75, f0p8, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<scaling_factor_opts> scaling_factor_e_;
  struct pdcch_monitoring_any_occasions_opts {
    enum options { without_dci_gap, with_dci_gap, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<pdcch_monitoring_any_occasions_opts> pdcch_monitoring_any_occasions_e_;
  struct time_dur_for_qcl_s_ {
    struct scs_60k_hz_opts {
      enum options { s7, s14, s28, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_60k_hz_opts> scs_60k_hz_e_;
    struct scs_120k_hz_opts {
      enum options { s14, s28, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_120k_hz_opts> scs_120k_hz_e_;

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
    typedef enumerated<scs_15k_hz_opts> scs_15k_hz_e_;
    struct scs_30k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_30k_hz_opts> scs_30k_hz_e_;
    struct scs_60k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_60k_hz_opts> scs_60k_hz_e_;
    struct scs_120k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_120k_hz_opts> scs_120k_hz_e_;

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
  bool                                            cross_carrier_sched_other_scs_present                = false;
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
  typedef enumerated<fallback_opts> fallback_e_;
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
    typedef enumerated<scs_15k_hz_opts> scs_15k_hz_e_;
    struct scs_30k_hz_opts {
      enum options { set1, set2, set3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_30k_hz_opts> scs_30k_hz_e_;
    struct scs_60k_hz_opts {
      enum options { set1, set2, set3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_60k_hz_opts> scs_60k_hz_e_;
    struct scs_120k_hz_opts {
      enum options { set1, set2, set3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_120k_hz_opts> scs_120k_hz_e_;

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
    typedef enumerated<different_tb_per_slot_scs_30k_hz_opts> different_tb_per_slot_scs_30k_hz_e_;

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
  bool      supported_srs_res_present = false;
  srs_res_s supported_srs_res;

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
typedef enumerated<mod_order_opts> mod_order_e;

// SupportedBandwidth ::= CHOICE
struct supported_bw_c {
  struct fr1_opts {
    enum options { mhz5, mhz10, mhz15, mhz20, mhz25, mhz30, mhz40, mhz50, mhz60, mhz80, mhz100, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<fr1_opts> fr1_e_;
  struct fr2_opts {
    enum options { mhz50, mhz100, mhz200, mhz400, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<fr2_opts> fr2_e_;
  struct types_opts {
    enum options { fr1, fr2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

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
  typedef enumerated<supported_srs_tx_port_switch_opts> supported_srs_tx_port_switch_e_;

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
  typedef enumerated<scaling_factor_opts> scaling_factor_e_;
  struct pusch_processing_type1_different_tb_per_slot_s_ {
    struct scs_15k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_15k_hz_opts> scs_15k_hz_e_;
    struct scs_30k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_30k_hz_opts> scs_30k_hz_e_;
    struct scs_60k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_60k_hz_opts> scs_60k_hz_e_;
    struct scs_120k_hz_opts {
      enum options { upto2, upto4, upto7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<scs_120k_hz_opts> scs_120k_hz_e_;

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
  bool                                            cross_carrier_sched_other_scs_present                = false;
  bool                                            intra_band_freq_separation_ul_present                = false;
  bool                                            search_space_sharing_ca_ul_present                   = false;
  bool                                            dummy1_present                                       = false;
  bool                                            supported_srs_res_present                            = false;
  bool                                            two_pucch_group_present                              = false;
  bool                                            dyn_switch_sul_present                               = false;
  bool                                            simul_tx_sul_non_sul_present                         = false;
  bool                                            pusch_processing_type1_different_tb_per_slot_present = false;
  bool                                            dummy2_present                                       = false;
  feature_set_list_per_ul_cc_l_                   feature_set_list_per_ul_cc;
  scaling_factor_e_                               scaling_factor;
  freq_separation_class_e                         intra_band_freq_separation_ul;
  dummy_i_s                                       dummy1;
  srs_res_s                                       supported_srs_res;
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
typedef enumerated<aggr_bw_opts> aggr_bw_e;

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
  typedef enumerated<types_opts> types;

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

// InterRAT-Parameters ::= SEQUENCE
struct inter_rat_params_s {
  bool           ext           = false;
  bool           eutra_present = false;
  eutra_params_s eutra;
  // ...

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

// MeasAndMobParametersCommon ::= SEQUENCE
struct meas_and_mob_params_common_s {
  struct max_num_csi_rs_rrm_rs_sinr_opts {
    enum options { n4, n8, n16, n32, n64, n96, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_csi_rs_rrm_rs_sinr_opts> max_num_csi_rs_rrm_rs_sinr_e_;

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
  typedef enumerated<max_num_res_csi_rs_rlm_opts> max_num_res_csi_rs_rlm_e_;

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

// GeneralParametersMRDC-XDD-Diff ::= SEQUENCE
struct general_params_mrdc_xdd_diff_s {
  bool ext                                    = false;
  bool split_srb_with_one_ul_path_present     = false;
  bool split_drb_with_ul_both_mcg_scg_present = false;
  bool srb3_present                           = false;
  bool v2x_eutra_present                      = false;
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
  dyn_octstring                  late_non_crit_ext;

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
  typedef enumerated<bwp_switching_delay_opts> bwp_switching_delay_e_;

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
  typedef enumerated<pdsch_re_map_fr1_per_symbol_opts> pdsch_re_map_fr1_per_symbol_e_;
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
  typedef enumerated<pdsch_re_map_fr1_per_slot_opts> pdsch_re_map_fr1_per_slot_e_;

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
  typedef enumerated<pdsch_re_map_fr2_per_symbol_opts> pdsch_re_map_fr2_per_symbol_e_;
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
  typedef enumerated<pdsch_re_map_fr2_per_slot_opts> pdsch_re_map_fr2_per_slot_e_;

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
  typedef enumerated<supported_dmrs_type_dl_opts> supported_dmrs_type_dl_e_;
  struct supported_dmrs_type_ul_opts {
    enum options { type1, type1_and2, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<supported_dmrs_type_ul_opts> supported_dmrs_type_ul_e_;
  struct mux_sr_harq_ack_csi_pucch_once_per_slot_s_ {
    bool same_symbol_present = false;
    bool diff_symbol_present = false;
  };
  struct pdcch_blind_detection_nrdc_s_ {
    uint8_t pdcch_blind_detection_mcg_ue = 1;
    uint8_t pdcch_blind_detection_scg_ue = 1;
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

// UE-NR-Capability-v1570 ::= SEQUENCE
struct ue_nr_cap_v1570_s {
  bool                nrdc_params_v1570_present = false;
  bool                non_crit_ext_present      = false;
  nrdc_params_v1570_s nrdc_params_v1570;

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
  enum options { rel15, spare7, spare6, spare5, spare4, spare3, spare2, spare1, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<access_stratum_release_opts, true> access_stratum_release_e;

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
  typedef enumerated<max_num_rohc_context_sessions_opts> max_num_rohc_context_sessions_e_;

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

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-Parameters ::= SEQUENCE
struct rlc_params_s {
  bool ext                      = false;
  bool am_with_short_sn_present = false;
  bool um_with_short_sn_present = false;
  bool um_with_long_sn_present  = false;
  // ...

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
