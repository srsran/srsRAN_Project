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

#include "meas_cfg.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// MUSIM-Starting-SFN-AndSubframe-r17 ::= SEQUENCE
struct mu_si_m_start_sfn_and_sf_r17_s {
  uint16_t start_sfn_r17 = 0;
  uint8_t  start_sf_r17  = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MUSIM-GapInfo-r17 ::= SEQUENCE
struct mu_si_m_gap_info_r17_s {
  struct musim_gap_len_r17_opts {
    enum options { ms3, ms4, ms6, ms10, ms20, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using musim_gap_len_r17_e_ = enumerated<musim_gap_len_r17_opts>;
  struct musim_gap_repeat_and_offset_r17_c_ {
    struct types_opts {
      enum options {
        ms20_r17,
        ms40_r17,
        ms80_r17,
        ms160_r17,
        ms320_r17,
        ms640_r17,
        ms1280_r17,
        ms2560_r17,
        ms5120_r17,
        // ...
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    musim_gap_repeat_and_offset_r17_c_() = default;
    musim_gap_repeat_and_offset_r17_c_(const musim_gap_repeat_and_offset_r17_c_& other);
    musim_gap_repeat_and_offset_r17_c_& operator=(const musim_gap_repeat_and_offset_r17_c_& other);
    ~musim_gap_repeat_and_offset_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ms20_r17()
    {
      assert_choice_type(types::ms20_r17, type_, "musim-GapRepetitionAndOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms40_r17()
    {
      assert_choice_type(types::ms40_r17, type_, "musim-GapRepetitionAndOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms80_r17()
    {
      assert_choice_type(types::ms80_r17, type_, "musim-GapRepetitionAndOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms160_r17()
    {
      assert_choice_type(types::ms160_r17, type_, "musim-GapRepetitionAndOffset-r17");
      return c.get<uint8_t>();
    }
    uint16_t& ms320_r17()
    {
      assert_choice_type(types::ms320_r17, type_, "musim-GapRepetitionAndOffset-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms640_r17()
    {
      assert_choice_type(types::ms640_r17, type_, "musim-GapRepetitionAndOffset-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms1280_r17()
    {
      assert_choice_type(types::ms1280_r17, type_, "musim-GapRepetitionAndOffset-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms2560_r17()
    {
      assert_choice_type(types::ms2560_r17, type_, "musim-GapRepetitionAndOffset-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms5120_r17()
    {
      assert_choice_type(types::ms5120_r17, type_, "musim-GapRepetitionAndOffset-r17");
      return c.get<uint16_t>();
    }
    const uint8_t& ms20_r17() const
    {
      assert_choice_type(types::ms20_r17, type_, "musim-GapRepetitionAndOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms40_r17() const
    {
      assert_choice_type(types::ms40_r17, type_, "musim-GapRepetitionAndOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms80_r17() const
    {
      assert_choice_type(types::ms80_r17, type_, "musim-GapRepetitionAndOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms160_r17() const
    {
      assert_choice_type(types::ms160_r17, type_, "musim-GapRepetitionAndOffset-r17");
      return c.get<uint8_t>();
    }
    const uint16_t& ms320_r17() const
    {
      assert_choice_type(types::ms320_r17, type_, "musim-GapRepetitionAndOffset-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms640_r17() const
    {
      assert_choice_type(types::ms640_r17, type_, "musim-GapRepetitionAndOffset-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms1280_r17() const
    {
      assert_choice_type(types::ms1280_r17, type_, "musim-GapRepetitionAndOffset-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms2560_r17() const
    {
      assert_choice_type(types::ms2560_r17, type_, "musim-GapRepetitionAndOffset-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms5120_r17() const
    {
      assert_choice_type(types::ms5120_r17, type_, "musim-GapRepetitionAndOffset-r17");
      return c.get<uint16_t>();
    }
    uint8_t&  set_ms20_r17();
    uint8_t&  set_ms40_r17();
    uint8_t&  set_ms80_r17();
    uint8_t&  set_ms160_r17();
    uint16_t& set_ms320_r17();
    uint16_t& set_ms640_r17();
    uint16_t& set_ms1280_r17();
    uint16_t& set_ms2560_r17();
    uint16_t& set_ms5120_r17();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                               musim_start_sfn_and_sf_r17_present      = false;
  bool                               musim_gap_len_r17_present               = false;
  bool                               musim_gap_repeat_and_offset_r17_present = false;
  mu_si_m_start_sfn_and_sf_r17_s     musim_start_sfn_and_sf_r17;
  musim_gap_len_r17_e_               musim_gap_len_r17;
  musim_gap_repeat_and_offset_r17_c_ musim_gap_repeat_and_offset_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeighbourCellInfo-r17 ::= SEQUENCE
struct neighbour_cell_info_r17_s {
  epoch_time_r17_s     epoch_time_r17;
  ephemeris_info_r17_c ephemeris_info_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RAN-VisibleParameters-r17 ::= SEQUENCE
struct ran_visible_params_r17_s {
  struct ran_visible_periodicity_r17_opts {
    enum options { ms120, ms240, ms480, ms640, ms1024, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using ran_visible_periodicity_r17_e_ = enumerated<ran_visible_periodicity_r17_opts>;

  // member variables
  bool                           ext                                                = false;
  bool                           ran_visible_periodicity_r17_present                = false;
  bool                           nof_buffer_level_entries_r17_present               = false;
  bool                           report_playout_delay_for_media_startup_r17_present = false;
  ran_visible_periodicity_r17_e_ ran_visible_periodicity_r17;
  uint8_t                        nof_buffer_level_entries_r17               = 1;
  bool                           report_playout_delay_for_media_startup_r17 = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BFD-RelaxationReportingConfig-r17 ::= SEQUENCE
struct bfd_relaxation_report_cfg_r17_s {
  struct bfd_relaxtion_report_prohibit_timer_opts {
    enum options {
      s0,
      s0dot5,
      s1,
      s2,
      s5,
      s10,
      s20,
      s30,
      s60,
      s90,
      s120,
      s300,
      s600,
      infinity,
      spare2,
      spare1,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using bfd_relaxtion_report_prohibit_timer_e_ = enumerated<bfd_relaxtion_report_prohibit_timer_opts>;

  // member variables
  bfd_relaxtion_report_prohibit_timer_e_ bfd_relaxtion_report_prohibit_timer;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CandidateServingFreqListNR-r16 ::= SEQUENCE (SIZE (1..128)) OF INTEGER (0..3279165)
using candidate_serving_freq_list_nr_r16_l = dyn_array<uint32_t>;

// MUSIM-GapAssistanceConfig-r17 ::= SEQUENCE
struct mu_si_m_gap_assist_cfg_r17_s {
  struct musim_gap_prohibit_timer_r17_opts {
    enum options {
      s0,
      s0dot1,
      s0dot2,
      s0dot3,
      s0dot4,
      s0dot5,
      s1,
      s2,
      s3,
      s4,
      s5,
      s6,
      s7,
      s8,
      s9,
      s10,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using musim_gap_prohibit_timer_r17_e_ = enumerated<musim_gap_prohibit_timer_r17_opts>;

  // member variables
  musim_gap_prohibit_timer_r17_e_ musim_gap_prohibit_timer_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MUSIM-Gap-r17 ::= SEQUENCE
struct mu_si_m_gap_r17_s {
  uint8_t                musim_gap_id_r17 = 0;
  mu_si_m_gap_info_r17_s musim_gap_info_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MUSIM-LeaveAssistanceConfig-r17 ::= SEQUENCE
struct mu_si_m_leave_assist_cfg_r17_s {
  struct musim_leave_without_resp_timer_r17_opts {
    enum options { ms10, ms20, ms40, ms60, ms80, ms100, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using musim_leave_without_resp_timer_r17_e_ = enumerated<musim_leave_without_resp_timer_r17_opts>;

  // member variables
  musim_leave_without_resp_timer_r17_e_ musim_leave_without_resp_timer_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PropDelayDiffReportConfig-r17 ::= SEQUENCE
struct prop_delay_diff_report_cfg_r17_s {
  struct thresh_prop_delay_diff_r17_opts {
    enum options {
      ms0dot5,
      ms1,
      ms2,
      ms3,
      ms4,
      ms5,
      ms6,
      ms7,
      ms8,
      ms9,
      ms10,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using thresh_prop_delay_diff_r17_e_ = enumerated<thresh_prop_delay_diff_r17_opts>;
  using neigh_cell_info_list_r17_l_   = dyn_array<neighbour_cell_info_r17_s>;

  // member variables
  bool                          thresh_prop_delay_diff_r17_present = false;
  thresh_prop_delay_diff_r17_e_ thresh_prop_delay_diff_r17;
  neigh_cell_info_list_r17_l_   neigh_cell_info_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLM-RelaxationReportingConfig-r17 ::= SEQUENCE
struct rlm_relaxation_report_cfg_r17_s {
  struct rlm_relaxtion_report_prohibit_timer_opts {
    enum options {
      s0,
      s0dot5,
      s1,
      s2,
      s5,
      s10,
      s20,
      s30,
      s60,
      s90,
      s120,
      s300,
      s600,
      infinity,
      spare2,
      spare1,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using rlm_relaxtion_report_prohibit_timer_e_ = enumerated<rlm_relaxtion_report_prohibit_timer_opts>;

  // member variables
  rlm_relaxtion_report_prohibit_timer_e_ rlm_relaxtion_report_prohibit_timer;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRM-MeasRelaxationReportingConfig-r17 ::= SEQUENCE
struct rrm_meas_relaxation_report_cfg_r17_s {
  struct s_search_delta_p_stationary_r17_opts {
    enum options { db2, db3, db6, db9, db12, db15, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using s_search_delta_p_stationary_r17_e_ = enumerated<s_search_delta_p_stationary_r17_opts>;
  struct t_search_delta_p_stationary_r17_opts {
    enum options {
      s5,
      s10,
      s20,
      s30,
      s60,
      s120,
      s180,
      s240,
      s300,
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
  using t_search_delta_p_stationary_r17_e_ = enumerated<t_search_delta_p_stationary_r17_opts>;

  // member variables
  s_search_delta_p_stationary_r17_e_ s_search_delta_p_stationary_r17;
  t_search_delta_p_stationary_r17_e_ t_search_delta_p_stationary_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-DeactivationPreferenceConfig-r17 ::= SEQUENCE
struct scg_deactivation_pref_cfg_r17_s {
  struct scg_deactivation_pref_prohibit_timer_r17_opts {
    enum options { s0, s1, s2, s4, s8, s10, s15, s30, s60, s120, s180, s240, s300, s600, s900, s1800, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using scg_deactivation_pref_prohibit_timer_r17_e_ = enumerated<scg_deactivation_pref_prohibit_timer_r17_opts>;

  // member variables
  scg_deactivation_pref_prohibit_timer_r17_e_ scg_deactivation_pref_prohibit_timer_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-RemoteUE-ToAddMod-r17 ::= SEQUENCE
struct sl_remote_ue_to_add_mod_r17_s {
  bool                ext                           = false;
  bool                sl_srap_cfg_relay_r17_present = false;
  fixed_bitstring<24> sl_l2_id_remote_r17;
  sl_srap_cfg_r17_s   sl_srap_cfg_relay_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SuccessHO-Config-r17 ::= SEQUENCE
struct success_ho_cfg_r17_s {
  struct thres_percentage_t304_r17_opts {
    enum options { p40, p60, p80, spare5, spare4, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using thres_percentage_t304_r17_e_ = enumerated<thres_percentage_t304_r17_opts>;
  struct thres_percentage_t310_r17_opts {
    enum options { p40, p60, p80, spare5, spare4, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using thres_percentage_t310_r17_e_ = enumerated<thres_percentage_t310_r17_opts>;
  struct thres_percentage_t312_r17_opts {
    enum options { p20, p40, p60, p80, spare4, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using thres_percentage_t312_r17_e_ = enumerated<thres_percentage_t312_r17_opts>;

  // member variables
  bool                         ext                                 = false;
  bool                         thres_percentage_t304_r17_present   = false;
  bool                         thres_percentage_t310_r17_present   = false;
  bool                         thres_percentage_t312_r17_present   = false;
  bool                         source_daps_fail_report_r17_present = false;
  thres_percentage_t304_r17_e_ thres_percentage_t304_r17;
  thres_percentage_t310_r17_e_ thres_percentage_t310_r17;
  thres_percentage_t312_r17_e_ thres_percentage_t312_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BAP-RoutingID-r16 ::= SEQUENCE
struct bap_routing_id_r16_s {
  fixed_bitstring<10> bap_address_r16;
  fixed_bitstring<10> bap_path_id_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CondReconfigToAddMod-r16 ::= SEQUENCE
struct cond_recfg_to_add_mod_r16_s {
  using cond_execution_cond_r16_l_ = bounded_array<uint8_t, 2>;

  // member variables
  bool                       ext               = false;
  uint8_t                    cond_recfg_id_r16 = 1;
  cond_execution_cond_r16_l_ cond_execution_cond_r16;
  dyn_octstring              cond_rrc_recfg_r16;
  // ...
  // group 0
  dyn_octstring cond_execution_cond_scg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRX-PreferenceConfig-r16 ::= SEQUENCE
struct drx_pref_cfg_r16_s {
  struct drx_pref_prohibit_timer_r16_opts {
    enum options { s0, s0dot5, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s20, s30, spare2, spare1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using drx_pref_prohibit_timer_r16_e_ = enumerated<drx_pref_prohibit_timer_r16_opts>;

  // member variables
  drx_pref_prohibit_timer_r16_e_ drx_pref_prohibit_timer_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IAB-IP-Address-r16 ::= CHOICE
struct iab_ip_address_r16_c {
  struct types_opts {
    enum options { ip_v4_address_r16, ip_v6_address_r16, ip_v6_prefix_r16, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  iab_ip_address_r16_c() = default;
  iab_ip_address_r16_c(const iab_ip_address_r16_c& other);
  iab_ip_address_r16_c& operator=(const iab_ip_address_r16_c& other);
  ~iab_ip_address_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<32>& ip_v4_address_r16()
  {
    assert_choice_type(types::ip_v4_address_r16, type_, "IAB-IP-Address-r16");
    return c.get<fixed_bitstring<32>>();
  }
  fixed_bitstring<128>& ip_v6_address_r16()
  {
    assert_choice_type(types::ip_v6_address_r16, type_, "IAB-IP-Address-r16");
    return c.get<fixed_bitstring<128>>();
  }
  fixed_bitstring<64>& ip_v6_prefix_r16()
  {
    assert_choice_type(types::ip_v6_prefix_r16, type_, "IAB-IP-Address-r16");
    return c.get<fixed_bitstring<64>>();
  }
  const fixed_bitstring<32>& ip_v4_address_r16() const
  {
    assert_choice_type(types::ip_v4_address_r16, type_, "IAB-IP-Address-r16");
    return c.get<fixed_bitstring<32>>();
  }
  const fixed_bitstring<128>& ip_v6_address_r16() const
  {
    assert_choice_type(types::ip_v6_address_r16, type_, "IAB-IP-Address-r16");
    return c.get<fixed_bitstring<128>>();
  }
  const fixed_bitstring<64>& ip_v6_prefix_r16() const
  {
    assert_choice_type(types::ip_v6_prefix_r16, type_, "IAB-IP-Address-r16");
    return c.get<fixed_bitstring<64>>();
  }
  fixed_bitstring<32>&  set_ip_v4_address_r16();
  fixed_bitstring<128>& set_ip_v6_address_r16();
  fixed_bitstring<64>&  set_ip_v6_prefix_r16();

private:
  types                                 type_;
  choice_buffer_t<fixed_bitstring<128>> c;

  void destroy_();
};

// IAB-IP-Usage-r16 ::= ENUMERATED
struct iab_ip_usage_r16_opts {
  enum options { f1_c, f1_u, non_f1, spare, nulltype } value;

  const char* to_string() const;
};
using iab_ip_usage_r16_e = enumerated<iab_ip_usage_r16_opts>;

// IDC-AssistanceConfig-r16 ::= SEQUENCE
struct idc_assist_cfg_r16_s {
  bool                                 ext = false;
  candidate_serving_freq_list_nr_r16_l candidate_serving_freq_list_nr_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MaxBW-PreferenceConfig-r16 ::= SEQUENCE
struct max_bw_pref_cfg_r16_s {
  struct max_bw_pref_prohibit_timer_r16_opts {
    enum options { s0, s0dot5, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s20, s30, spare2, spare1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using max_bw_pref_prohibit_timer_r16_e_ = enumerated<max_bw_pref_prohibit_timer_r16_opts>;

  // member variables
  max_bw_pref_prohibit_timer_r16_e_ max_bw_pref_prohibit_timer_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MaxCC-PreferenceConfig-r16 ::= SEQUENCE
struct max_cc_pref_cfg_r16_s {
  struct max_cc_pref_prohibit_timer_r16_opts {
    enum options { s0, s0dot5, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s20, s30, spare2, spare1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using max_cc_pref_prohibit_timer_r16_e_ = enumerated<max_cc_pref_prohibit_timer_r16_opts>;

  // member variables
  max_cc_pref_prohibit_timer_r16_e_ max_cc_pref_prohibit_timer_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MaxMIMO-LayerPreferenceConfig-r16 ::= SEQUENCE
struct max_mimo_layer_pref_cfg_r16_s {
  struct max_mimo_layer_pref_prohibit_timer_r16_opts {
    enum options { s0, s0dot5, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s20, s30, spare2, spare1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using max_mimo_layer_pref_prohibit_timer_r16_e_ = enumerated<max_mimo_layer_pref_prohibit_timer_r16_opts>;

  // member variables
  max_mimo_layer_pref_prohibit_timer_r16_e_ max_mimo_layer_pref_prohibit_timer_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasConfigAppLayer-r17 ::= SEQUENCE
struct meas_cfg_app_layer_r17_s {
  struct service_type_r17_opts {
    enum options { streaming, mtsi, vr, spare5, spare4, spare3, spare2, spare1, nulltype } value;

    const char* to_string() const;
  };
  using service_type_r17_e_ = enumerated<service_type_r17_opts>;

  // member variables
  bool                                      ext                                  = false;
  bool                                      service_type_r17_present             = false;
  bool                                      pause_report_r17_present             = false;
  bool                                      tx_of_session_start_stop_r17_present = false;
  bool                                      ran_visible_params_r17_present       = false;
  uint8_t                                   meas_cfg_app_layer_id_r17            = 0;
  bounded_octstring<1, 8000>                meas_cfg_app_layer_container_r17;
  service_type_r17_e_                       service_type_r17;
  bool                                      pause_report_r17             = false;
  bool                                      tx_of_session_start_stop_r17 = false;
  setup_release_c<ran_visible_params_r17_s> ran_visible_params_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MinSchedulingOffsetPreferenceConfig-r16 ::= SEQUENCE
struct min_sched_offset_pref_cfg_r16_s {
  struct min_sched_offset_pref_prohibit_timer_r16_opts {
    enum options { s0, s0dot5, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s20, s30, spare2, spare1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using min_sched_offset_pref_prohibit_timer_r16_e_ = enumerated<min_sched_offset_pref_prohibit_timer_r16_opts>;

  // member variables
  min_sched_offset_pref_prohibit_timer_r16_e_ min_sched_offset_pref_prohibit_timer_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MUSIM-GapConfig-r17 ::= SEQUENCE
struct mu_si_m_gap_cfg_r17_s {
  using musim_gap_to_release_list_r17_l_ = bounded_array<uint8_t, 3>;
  using musim_gap_to_add_mod_list_r17_l_ = dyn_array<mu_si_m_gap_r17_s>;

  // member variables
  bool                             ext                             = false;
  bool                             musim_aperiodic_gap_r17_present = false;
  musim_gap_to_release_list_r17_l_ musim_gap_to_release_list_r17;
  musim_gap_to_add_mod_list_r17_l_ musim_gap_to_add_mod_list_r17;
  mu_si_m_gap_info_r17_s           musim_aperiodic_gap_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeedForGapNCSG-ConfigEUTRA-r17 ::= SEQUENCE
struct need_for_gap_ncsg_cfg_eutra_r17_s {
  using requested_target_band_filt_ncsg_eutra_r17_l_ = dyn_array<uint16_t>;

  // member variables
  requested_target_band_filt_ncsg_eutra_r17_l_ requested_target_band_filt_ncsg_eutra_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeedForGapNCSG-ConfigNR-r17 ::= SEQUENCE
struct need_for_gap_ncsg_cfg_nr_r17_s {
  using requested_target_band_filt_ncsg_nr_r17_l_ = dyn_array<uint16_t>;

  // member variables
  requested_target_band_filt_ncsg_nr_r17_l_ requested_target_band_filt_ncsg_nr_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReleasePreferenceConfig-r16 ::= SEQUENCE
struct release_pref_cfg_r16_s {
  struct release_pref_prohibit_timer_r16_opts {
    enum options { s0, s0dot5, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s20, s30, infinity, spare1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using release_pref_prohibit_timer_r16_e_ = enumerated<release_pref_prohibit_timer_r16_opts>;

  // member variables
  bool                               connected_report_present = false;
  release_pref_prohibit_timer_r16_e_ release_pref_prohibit_timer_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-L2RelayUE-Config-r17 ::= SEQUENCE
struct sl_l2_relay_ue_cfg_r17_s {
  using sl_remote_ue_to_add_mod_list_r17_l_ = dyn_array<sl_remote_ue_to_add_mod_r17_s>;
  using sl_remote_ue_to_release_list_r17_l_ = bounded_array<fixed_bitstring<24>, 32>;

  // member variables
  bool                                ext = false;
  sl_remote_ue_to_add_mod_list_r17_l_ sl_remote_ue_to_add_mod_list_r17;
  sl_remote_ue_to_release_list_r17_l_ sl_remote_ue_to_release_list_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TimeOffsetEUTRA-r16 ::= ENUMERATED
struct sl_time_offset_eutra_r16_opts {
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
using sl_time_offset_eutra_r16_e = enumerated<sl_time_offset_eutra_r16_opts>;

// UE-TxTEG-RequestUL-TDOA-Config-r17 ::= CHOICE
struct ue_tx_teg_request_ul_tdoa_cfg_r17_c {
  struct periodic_report_r17_opts {
    enum options { ms160, ms320, ms1280, ms2560, ms61440, ms81920, ms368640, ms737280, nulltype } value;
    typedef uint32_t number_type;

    const char* to_string() const;
    uint32_t    to_number() const;
  };
  using periodic_report_r17_e_ = enumerated<periodic_report_r17_opts>;
  struct types_opts {
    enum options { one_shot_r17, periodic_report_r17, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ue_tx_teg_request_ul_tdoa_cfg_r17_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  periodic_report_r17_e_& periodic_report_r17()
  {
    assert_choice_type(types::periodic_report_r17, type_, "UE-TxTEG-RequestUL-TDOA-Config-r17");
    return c;
  }
  const periodic_report_r17_e_& periodic_report_r17() const
  {
    assert_choice_type(types::periodic_report_r17, type_, "UE-TxTEG-RequestUL-TDOA-Config-r17");
    return c;
  }
  void                    set_one_shot_r17();
  periodic_report_r17_e_& set_periodic_report_r17();

private:
  types                  type_;
  periodic_report_r17_e_ c;
};

// UL-GapFR2-Config-r17 ::= SEQUENCE
struct ul_gap_fr2_cfg_r17_s {
  struct ugl_r17_opts {
    enum options { ms0dot125, ms0dot25, ms0dot5, ms1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using ugl_r17_e_ = enumerated<ugl_r17_opts>;
  struct ugrp_r17_opts {
    enum options { ms5, ms20, ms40, ms160, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ugrp_r17_e_ = enumerated<ugrp_r17_opts>;

  // member variables
  bool        ref_fr2_serv_cell_async_ca_r17_present = false;
  uint8_t     gap_offset_r17                         = 0;
  ugl_r17_e_  ugl_r17;
  ugrp_r17_e_ ugrp_r17;
  uint8_t     ref_fr2_serv_cell_async_ca_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ValidityCellList ::= SEQUENCE (SIZE (1..8)) OF PCI-Range
using validity_cell_list_l = dyn_array<pci_range_s>;

// AppLayerMeasConfig-r17 ::= SEQUENCE
struct app_layer_meas_cfg_r17_s {
  using meas_cfg_app_layer_to_add_mod_list_r17_l_ = dyn_array<meas_cfg_app_layer_r17_s>;
  using meas_cfg_app_layer_to_release_list_r17_l_ = bounded_array<uint8_t, 16>;

  // member variables
  bool                                      ext                         = false;
  bool                                      rrc_seg_allowed_r17_present = false;
  meas_cfg_app_layer_to_add_mod_list_r17_l_ meas_cfg_app_layer_to_add_mod_list_r17;
  meas_cfg_app_layer_to_release_list_r17_l_ meas_cfg_app_layer_to_release_list_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BAP-Config-r16 ::= SEQUENCE
struct bap_cfg_r16_s {
  struct flow_ctrl_feedback_type_r16_opts {
    enum options { per_bh_rlc_ch, per_routing_id, both, nulltype } value;

    const char* to_string() const;
  };
  using flow_ctrl_feedback_type_r16_e_ = enumerated<flow_ctrl_feedback_type_r16_opts>;

  // member variables
  bool                           ext                                   = false;
  bool                           bap_address_r16_present               = false;
  bool                           default_ul_bap_routing_id_r16_present = false;
  bool                           default_ul_bh_rlc_ch_r16_present      = false;
  bool                           flow_ctrl_feedback_type_r16_present   = false;
  fixed_bitstring<10>            bap_address_r16;
  bap_routing_id_r16_s           default_ul_bap_routing_id_r16;
  fixed_bitstring<16>            default_ul_bh_rlc_ch_r16;
  flow_ctrl_feedback_type_r16_e_ flow_ctrl_feedback_type_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CGI-Info-Logging-r16 ::= SEQUENCE
struct cgi_info_logging_r16_s {
  bool                tac_r16_present = false;
  plmn_id_s           plmn_id_r16;
  fixed_bitstring<36> cell_id_r16;
  fixed_bitstring<24> tac_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CondReconfigToAddModList-r16 ::= SEQUENCE (SIZE (1..8)) OF CondReconfigToAddMod-r16
using cond_recfg_to_add_mod_list_r16_l = dyn_array<cond_recfg_to_add_mod_r16_s>;

// CondReconfigToRemoveList-r16 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..8)
using cond_recfg_to_rem_list_r16_l = bounded_array<uint8_t, 8>;

// IAB-IP-AddressConfiguration-r16 ::= SEQUENCE
struct iab_ip_address_cfg_r16_s {
  bool                 ext                                  = false;
  bool                 iab_ip_address_r16_present           = false;
  bool                 iab_ip_usage_r16_present             = false;
  bool                 iab_donor_du_bap_address_r16_present = false;
  uint8_t              iab_ip_address_idx_r16               = 1;
  iab_ip_address_r16_c iab_ip_address_r16;
  iab_ip_usage_r16_e   iab_ip_usage_r16;
  fixed_bitstring<10>  iab_donor_du_bap_address_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeedForGapsConfigNR-r16 ::= SEQUENCE
struct need_for_gaps_cfg_nr_r16_s {
  using requested_target_band_filt_nr_r16_l_ = dyn_array<uint16_t>;

  // member variables
  requested_target_band_filt_nr_r16_l_ requested_target_band_filt_nr_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OnDemandSIB-Request-r16 ::= SEQUENCE
struct on_demand_sib_request_r16_s {
  struct on_demand_sib_request_prohibit_timer_r16_opts {
    enum options { s0, s0dot5, s1, s2, s5, s10, s20, s30, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using on_demand_sib_request_prohibit_timer_r16_e_ = enumerated<on_demand_sib_request_prohibit_timer_r16_opts>;

  // member variables
  on_demand_sib_request_prohibit_timer_r16_e_ on_demand_sib_request_prohibit_timer_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OtherConfig-v1700 ::= SEQUENCE
struct other_cfg_v1700_s {
  bool                                                  ul_gap_fr2_pref_cfg_r17_present            = false;
  bool                                                  musim_gap_assist_cfg_r17_present           = false;
  bool                                                  musim_leave_assist_cfg_r17_present         = false;
  bool                                                  success_ho_cfg_r17_present                 = false;
  bool                                                  max_bw_pref_cfg_fr2_2_r17_present          = false;
  bool                                                  max_mimo_layer_pref_cfg_fr2_2_r17_present  = false;
  bool                                                  min_sched_offset_pref_cfg_ext_r17_present  = false;
  bool                                                  rlm_relaxation_report_cfg_r17_present      = false;
  bool                                                  bfd_relaxation_report_cfg_r17_present      = false;
  bool                                                  scg_deactivation_pref_cfg_r17_present      = false;
  bool                                                  rrm_meas_relaxation_report_cfg_r17_present = false;
  bool                                                  prop_delay_diff_report_cfg_r17_present     = false;
  setup_release_c<mu_si_m_gap_assist_cfg_r17_s>         musim_gap_assist_cfg_r17;
  setup_release_c<mu_si_m_leave_assist_cfg_r17_s>       musim_leave_assist_cfg_r17;
  setup_release_c<success_ho_cfg_r17_s>                 success_ho_cfg_r17;
  setup_release_c<rlm_relaxation_report_cfg_r17_s>      rlm_relaxation_report_cfg_r17;
  setup_release_c<bfd_relaxation_report_cfg_r17_s>      bfd_relaxation_report_cfg_r17;
  setup_release_c<scg_deactivation_pref_cfg_r17_s>      scg_deactivation_pref_cfg_r17;
  setup_release_c<rrm_meas_relaxation_report_cfg_r17_s> rrm_meas_relaxation_report_cfg_r17;
  setup_release_c<prop_delay_diff_report_cfg_r17_s>     prop_delay_diff_report_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-ConfigDedicatedEUTRA-Info-r16 ::= SEQUENCE
struct sl_cfg_ded_eutra_info_r16_s {
  using sl_time_offset_eutra_list_r16_l_ = std::array<sl_time_offset_eutra_r16_e, 8>;

  // member variables
  bool                             sl_time_offset_eutra_list_r16_present = false;
  dyn_octstring                    sl_cfg_ded_eutra_r16;
  sl_time_offset_eutra_list_r16_l_ sl_time_offset_eutra_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// T316-r16 ::= ENUMERATED
struct t316_r16_opts {
  enum options { ms50, ms100, ms200, ms300, ms400, ms500, ms600, ms1000, ms1500, ms2000, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using t316_r16_e = enumerated<t316_r16_opts>;

// ValidityArea-r16 ::= SEQUENCE
struct validity_area_r16_s {
  uint32_t             carrier_freq_r16 = 0;
  validity_cell_list_l validity_cell_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ConditionalReconfiguration-r16 ::= SEQUENCE
struct conditional_recfg_r16_s {
  bool                             ext                            = false;
  bool                             attempt_cond_recfg_r16_present = false;
  cond_recfg_to_rem_list_r16_l     cond_recfg_to_rem_list_r16;
  cond_recfg_to_add_mod_list_r16_l cond_recfg_to_add_mod_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IAB-IP-AddressConfigurationList-r16 ::= SEQUENCE
struct iab_ip_address_cfg_list_r16_s {
  using iab_ip_address_to_add_mod_list_r16_l_ = dyn_array<iab_ip_address_cfg_r16_s>;
  using iab_ip_address_to_release_list_r16_l_ = bounded_array<uint8_t, 32>;

  // member variables
  bool                                  ext = false;
  iab_ip_address_to_add_mod_list_r16_l_ iab_ip_address_to_add_mod_list_r16;
  iab_ip_address_to_release_list_r16_l_ iab_ip_address_to_release_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRDC-SecondaryCellGroupConfig ::= SEQUENCE
struct mrdc_secondary_cell_group_cfg_s {
  struct mrdc_secondary_cell_group_c_ {
    struct types_opts {
      enum options { nr_scg, eutra_scg, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    mrdc_secondary_cell_group_c_() = default;
    mrdc_secondary_cell_group_c_(const mrdc_secondary_cell_group_c_& other);
    mrdc_secondary_cell_group_c_& operator=(const mrdc_secondary_cell_group_c_& other);
    ~mrdc_secondary_cell_group_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dyn_octstring& nr_scg()
    {
      assert_choice_type(types::nr_scg, type_, "mrdc-SecondaryCellGroup");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& eutra_scg()
    {
      assert_choice_type(types::eutra_scg, type_, "mrdc-SecondaryCellGroup");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& nr_scg() const
    {
      assert_choice_type(types::nr_scg, type_, "mrdc-SecondaryCellGroup");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& eutra_scg() const
    {
      assert_choice_type(types::eutra_scg, type_, "mrdc-SecondaryCellGroup");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& set_nr_scg();
    dyn_octstring& set_eutra_scg();

  private:
    types                          type_;
    choice_buffer_t<dyn_octstring> c;

    void destroy_();
  };

  // member variables
  bool                         mrdc_release_and_add_present = false;
  mrdc_secondary_cell_group_c_ mrdc_secondary_cell_group;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OtherConfig-v1610 ::= SEQUENCE
struct other_cfg_v1610_s {
  bool                                                         idc_assist_cfg_r16_present            = false;
  bool                                                         drx_pref_cfg_r16_present              = false;
  bool                                                         max_bw_pref_cfg_r16_present           = false;
  bool                                                         max_cc_pref_cfg_r16_present           = false;
  bool                                                         max_mimo_layer_pref_cfg_r16_present   = false;
  bool                                                         min_sched_offset_pref_cfg_r16_present = false;
  bool                                                         release_pref_cfg_r16_present          = false;
  bool                                                         ref_time_pref_report_r16_present      = false;
  bool                                                         bt_name_list_r16_present              = false;
  bool                                                         wlan_name_list_r16_present            = false;
  bool                                                         sensor_name_list_r16_present          = false;
  bool                                                         obtain_common_location_r16_present    = false;
  bool                                                         sl_assist_cfg_nr_r16_present          = false;
  setup_release_c<idc_assist_cfg_r16_s>                        idc_assist_cfg_r16;
  setup_release_c<drx_pref_cfg_r16_s>                          drx_pref_cfg_r16;
  setup_release_c<max_bw_pref_cfg_r16_s>                       max_bw_pref_cfg_r16;
  setup_release_c<max_cc_pref_cfg_r16_s>                       max_cc_pref_cfg_r16;
  setup_release_c<max_mimo_layer_pref_cfg_r16_s>               max_mimo_layer_pref_cfg_r16;
  setup_release_c<min_sched_offset_pref_cfg_r16_s>             min_sched_offset_pref_cfg_r16;
  setup_release_c<release_pref_cfg_r16_s>                      release_pref_cfg_r16;
  setup_release_c<dyn_seq_of<bounded_octstring<1, 248>, 1, 4>> bt_name_list_r16;
  setup_release_c<dyn_seq_of<bounded_octstring<1, 32>, 1, 4>>  wlan_name_list_r16;
  setup_release_c<sensor_name_list_r16_s>                      sensor_name_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OverheatingAssistanceConfig ::= SEQUENCE
struct overheat_assist_cfg_s {
  struct overheat_ind_prohibit_timer_opts {
    enum options {
      s0,
      s0dot5,
      s1,
      s2,
      s5,
      s10,
      s20,
      s30,
      s60,
      s90,
      s120,
      s300,
      s600,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using overheat_ind_prohibit_timer_e_ = enumerated<overheat_ind_prohibit_timer_opts>;

  // member variables
  overheat_ind_prohibit_timer_e_ overheat_ind_prohibit_timer;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RAN-AreaConfig ::= SEQUENCE
struct ran_area_cfg_s {
  using ran_area_code_list_l_ = bounded_array<uint16_t, 32>;

  // member variables
  fixed_bitstring<24>   tac;
  ran_area_code_list_l_ ran_area_code_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfiguration-v1700-IEs ::= SEQUENCE
struct rrc_recfg_v1700_ies_s {
  bool                                                 other_cfg_v1700_present                   = false;
  bool                                                 sl_l2_relay_ue_cfg_r17_present            = false;
  bool                                                 sl_l2_remote_ue_cfg_r17_present           = false;
  bool                                                 need_for_gap_ncsg_cfg_nr_r17_present      = false;
  bool                                                 need_for_gap_ncsg_cfg_eutra_r17_present   = false;
  bool                                                 musim_gap_cfg_r17_present                 = false;
  bool                                                 ul_gap_fr2_cfg_r17_present                = false;
  bool                                                 scg_state_r17_present                     = false;
  bool                                                 app_layer_meas_cfg_r17_present            = false;
  bool                                                 ue_tx_teg_request_ul_tdoa_cfg_r17_present = false;
  bool                                                 non_crit_ext_present                      = false;
  other_cfg_v1700_s                                    other_cfg_v1700;
  setup_release_c<sl_l2_relay_ue_cfg_r17_s>            sl_l2_relay_ue_cfg_r17;
  setup_release_c<sl_l2_remote_ue_cfg_r17_s>           sl_l2_remote_ue_cfg_r17;
  dyn_octstring                                        ded_paging_delivery_r17;
  setup_release_c<need_for_gap_ncsg_cfg_nr_r17_s>      need_for_gap_ncsg_cfg_nr_r17;
  setup_release_c<need_for_gap_ncsg_cfg_eutra_r17_s>   need_for_gap_ncsg_cfg_eutra_r17;
  setup_release_c<mu_si_m_gap_cfg_r17_s>               musim_gap_cfg_r17;
  setup_release_c<ul_gap_fr2_cfg_r17_s>                ul_gap_fr2_cfg_r17;
  app_layer_meas_cfg_r17_s                             app_layer_meas_cfg_r17;
  setup_release_c<ue_tx_teg_request_ul_tdoa_cfg_r17_c> ue_tx_teg_request_ul_tdoa_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SliceInfoDedicated-r17 ::= SEQUENCE
struct slice_info_ded_r17_s {
  bool                  nsag_cell_resel_prio_r17_present     = false;
  bool                  nsag_cell_resel_sub_prio_r17_present = false;
  nsag_id_info_r17_s    nsag_id_info_r17;
  uint8_t               nsag_cell_resel_prio_r17 = 0;
  cell_resel_sub_prio_e nsag_cell_resel_sub_prio_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TrackingAreaIdentity-r16 ::= SEQUENCE
struct tracking_area_id_r16_s {
  plmn_id_s           plmn_id_r16;
  fixed_bitstring<24> tac_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ValidityAreaList-r16 ::= SEQUENCE (SIZE (1..8)) OF ValidityArea-r16
using validity_area_list_r16_l = dyn_array<validity_area_r16_s>;

// CellGlobalIdList-r16 ::= SEQUENCE (SIZE (1..32)) OF CGI-Info-Logging-r16
using cell_global_id_list_r16_l = dyn_array<cgi_info_logging_r16_s>;

// MeasIdleConfigDedicated-r16 ::= SEQUENCE
struct meas_idle_cfg_ded_r16_s {
  using meas_idle_carrier_list_nr_r16_l_    = dyn_array<meas_idle_carrier_nr_r16_s>;
  using meas_idle_carrier_list_eutra_r16_l_ = dyn_array<meas_idle_carrier_eutra_r16_s>;
  struct meas_idle_dur_r16_opts {
    enum options { sec10, sec30, sec60, sec120, sec180, sec240, sec300, spare, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using meas_idle_dur_r16_e_ = enumerated<meas_idle_dur_r16_opts>;

  // member variables
  bool                                ext = false;
  meas_idle_carrier_list_nr_r16_l_    meas_idle_carrier_list_nr_r16;
  meas_idle_carrier_list_eutra_r16_l_ meas_idle_carrier_list_eutra_r16;
  meas_idle_dur_r16_e_                meas_idle_dur_r16;
  validity_area_list_r16_l            validity_area_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-RAN-AreaCell ::= SEQUENCE
struct plmn_ran_area_cell_s {
  using ran_area_cells_l_ = bounded_array<fixed_bitstring<36>, 32>;

  // member variables
  bool              plmn_id_present = false;
  plmn_id_s         plmn_id;
  ran_area_cells_l_ ran_area_cells;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-RAN-AreaConfig ::= SEQUENCE
struct plmn_ran_area_cfg_s {
  using ran_area_l_ = dyn_array<ran_area_cfg_s>;

  // member variables
  bool        plmn_id_present = false;
  plmn_id_s   plmn_id;
  ran_area_l_ ran_area;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfiguration-v1610-IEs ::= SEQUENCE
struct rrc_recfg_v1610_ies_s {
  bool                                         other_cfg_v1610_present             = false;
  bool                                         bap_cfg_r16_present                 = false;
  bool                                         iab_ip_address_cfg_list_r16_present = false;
  bool                                         conditional_recfg_r16_present       = false;
  bool                                         daps_source_release_r16_present     = false;
  bool                                         t316_r16_present                    = false;
  bool                                         need_for_gaps_cfg_nr_r16_present    = false;
  bool                                         on_demand_sib_request_r16_present   = false;
  bool                                         sl_cfg_ded_nr_r16_present           = false;
  bool                                         sl_cfg_ded_eutra_info_r16_present   = false;
  bool                                         target_cell_smtc_scg_r16_present    = false;
  bool                                         non_crit_ext_present                = false;
  other_cfg_v1610_s                            other_cfg_v1610;
  setup_release_c<bap_cfg_r16_s>               bap_cfg_r16;
  iab_ip_address_cfg_list_r16_s                iab_ip_address_cfg_list_r16;
  conditional_recfg_r16_s                      conditional_recfg_r16;
  setup_release_c<t316_r16_e>                  t316_r16;
  setup_release_c<need_for_gaps_cfg_nr_r16_s>  need_for_gaps_cfg_nr_r16;
  setup_release_c<on_demand_sib_request_r16_s> on_demand_sib_request_r16;
  dyn_octstring                                ded_pos_sys_info_delivery_r16;
  setup_release_c<sl_cfg_ded_nr_r16_s>         sl_cfg_ded_nr_r16;
  setup_release_c<sl_cfg_ded_eutra_info_r16_s> sl_cfg_ded_eutra_info_r16;
  ssb_mtc_s                                    target_cell_smtc_scg_r16;
  rrc_recfg_v1700_ies_s                        non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCRelease-v1710-IEs ::= SEQUENCE
struct rrc_release_v1710_ies_s {
  bool no_last_cell_upd_r17_present = false;
  bool non_crit_ext_present         = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SliceInfoListDedicated-r17 ::= SEQUENCE (SIZE (1..8)) OF SliceInfoDedicated-r17
using slice_info_list_ded_r17_l = dyn_array<slice_info_ded_r17_s>;

// TrackingAreaCodeList-r16 ::= SEQUENCE (SIZE (1..8)) OF BIT STRING (SIZE (24))
using tac_list_r16_l = bounded_array<fixed_bitstring<24>, 8>;

// TrackingAreaIdentityList-r16 ::= SEQUENCE (SIZE (1..8)) OF TrackingAreaIdentity-r16
using tracking_area_id_list_r16_l = dyn_array<tracking_area_id_r16_s>;

// AreaConfig-r16 ::= CHOICE
struct area_cfg_r16_c {
  struct types_opts {
    enum options { cell_global_id_list_r16, tac_list_r16, tracking_area_id_list_r16, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  area_cfg_r16_c() = default;
  area_cfg_r16_c(const area_cfg_r16_c& other);
  area_cfg_r16_c& operator=(const area_cfg_r16_c& other);
  ~area_cfg_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cell_global_id_list_r16_l& cell_global_id_list_r16()
  {
    assert_choice_type(types::cell_global_id_list_r16, type_, "AreaConfig-r16");
    return c.get<cell_global_id_list_r16_l>();
  }
  tac_list_r16_l& tac_list_r16()
  {
    assert_choice_type(types::tac_list_r16, type_, "AreaConfig-r16");
    return c.get<tac_list_r16_l>();
  }
  tracking_area_id_list_r16_l& tracking_area_id_list_r16()
  {
    assert_choice_type(types::tracking_area_id_list_r16, type_, "AreaConfig-r16");
    return c.get<tracking_area_id_list_r16_l>();
  }
  const cell_global_id_list_r16_l& cell_global_id_list_r16() const
  {
    assert_choice_type(types::cell_global_id_list_r16, type_, "AreaConfig-r16");
    return c.get<cell_global_id_list_r16_l>();
  }
  const tac_list_r16_l& tac_list_r16() const
  {
    assert_choice_type(types::tac_list_r16, type_, "AreaConfig-r16");
    return c.get<tac_list_r16_l>();
  }
  const tracking_area_id_list_r16_l& tracking_area_id_list_r16() const
  {
    assert_choice_type(types::tracking_area_id_list_r16, type_, "AreaConfig-r16");
    return c.get<tracking_area_id_list_r16_l>();
  }
  cell_global_id_list_r16_l&   set_cell_global_id_list_r16();
  tac_list_r16_l&              set_tac_list_r16();
  tracking_area_id_list_r16_l& set_tracking_area_id_list_r16();

private:
  types                                                                                   type_;
  choice_buffer_t<cell_global_id_list_r16_l, tac_list_r16_l, tracking_area_id_list_r16_l> c;

  void destroy_();
};

// FreqPriorityDedicatedSlicing-r17 ::= SEQUENCE
struct freq_prio_ded_slicing_r17_s {
  uint32_t                  dl_explicit_carrier_freq_r17 = 0;
  slice_info_list_ded_r17_l slice_info_list_ded_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqPriorityEUTRA ::= SEQUENCE
struct freq_prio_eutra_s {
  bool                  cell_resel_sub_prio_present = false;
  uint32_t              carrier_freq                = 0;
  uint8_t               cell_resel_prio             = 0;
  cell_resel_sub_prio_e cell_resel_sub_prio;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqPriorityNR ::= SEQUENCE
struct freq_prio_nr_s {
  bool                  cell_resel_sub_prio_present = false;
  uint32_t              carrier_freq                = 0;
  uint8_t               cell_resel_prio             = 0;
  cell_resel_sub_prio_e cell_resel_sub_prio;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqTargetInfo-r16 ::= SEQUENCE
struct inter_freq_target_info_r16_s {
  using cell_list_r16_l_ = bounded_array<uint16_t, 32>;

  // member variables
  uint32_t         dl_carrier_freq_r16 = 0;
  cell_list_r16_l_ cell_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OtherConfig-v1540 ::= SEQUENCE
struct other_cfg_v1540_s {
  bool                                   ext                         = false;
  bool                                   overheat_assist_cfg_present = false;
  setup_release_c<overheat_assist_cfg_s> overheat_assist_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-RAN-AreaCellList ::= SEQUENCE (SIZE (1..8)) OF PLMN-RAN-AreaCell
using plmn_ran_area_cell_list_l = dyn_array<plmn_ran_area_cell_s>;

// PLMN-RAN-AreaConfigList ::= SEQUENCE (SIZE (1..8)) OF PLMN-RAN-AreaConfig
using plmn_ran_area_cfg_list_l = dyn_array<plmn_ran_area_cfg_s>;

// RAT-Type ::= ENUMERATED
struct rat_type_opts {
  enum options { nr, eutra_nr, eutra, utra_fdd_v1610, /*...*/ nulltype } value;

  const char* to_string() const;
};
using rat_type_e = enumerated<rat_type_opts, true>;

// RRCReconfiguration-v1560-IEs ::= SEQUENCE
struct rrc_recfg_v1560_ies_s {
  bool                                             mrdc_secondary_cell_group_cfg_present = false;
  bool                                             sk_counter_present                    = false;
  bool                                             non_crit_ext_present                  = false;
  setup_release_c<mrdc_secondary_cell_group_cfg_s> mrdc_secondary_cell_group_cfg;
  dyn_octstring                                    radio_bearer_cfg2;
  uint32_t                                         sk_counter = 0;
  rrc_recfg_v1610_ies_s                            non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCRelease-v1650-IEs ::= SEQUENCE
struct rrc_release_v1650_ies_s {
  bool                    mps_prio_ind_r16_present = false;
  bool                    non_crit_ext_present     = false;
  rrc_release_v1710_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResume-v1700-IEs ::= SEQUENCE
struct rrc_resume_v1700_ies_s {
  bool                                               sl_cfg_ded_nr_r17_present               = false;
  bool                                               sl_l2_remote_ue_cfg_r17_present         = false;
  bool                                               need_for_gap_ncsg_cfg_nr_r17_present    = false;
  bool                                               need_for_gap_ncsg_cfg_eutra_r17_present = false;
  bool                                               scg_state_r17_present                   = false;
  bool                                               app_layer_meas_cfg_r17_present          = false;
  bool                                               non_crit_ext_present                    = false;
  setup_release_c<sl_cfg_ded_nr_r16_s>               sl_cfg_ded_nr_r17;
  setup_release_c<sl_l2_remote_ue_cfg_r17_s>         sl_l2_remote_ue_cfg_r17;
  setup_release_c<need_for_gap_ncsg_cfg_nr_r17_s>    need_for_gap_ncsg_cfg_nr_r17;
  setup_release_c<need_for_gap_ncsg_cfg_eutra_r17_s> need_for_gap_ncsg_cfg_eutra_r17;
  app_layer_meas_cfg_r17_s                           app_layer_meas_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RxTxTimeDiff-r17 ::= SEQUENCE
struct rx_tx_time_diff_r17_s {
  bool     ext                   = false;
  bool     result_k5_r17_present = false;
  uint16_t result_k5_r17         = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SDT-Config-r17 ::= SEQUENCE
struct sdt_cfg_r17_s {
  using sdt_drb_list_r17_l_ = bounded_array<uint8_t, 29>;
  struct sdt_drb_continue_rohc_r17_opts {
    enum options { cell, rna, nulltype } value;

    const char* to_string() const;
  };
  using sdt_drb_continue_rohc_r17_e_ = enumerated<sdt_drb_continue_rohc_r17_opts>;

  // member variables
  bool                           sdt_drb_list_r17_present          = false;
  bool                           sdt_srb2_ind_r17_present          = false;
  bool                           sdt_mac_phy_cg_cfg_r17_present    = false;
  bool                           sdt_drb_continue_rohc_r17_present = false;
  sdt_drb_list_r17_l_            sdt_drb_list_r17;
  setup_release_c<dyn_octstring> sdt_mac_phy_cg_cfg_r17;
  sdt_drb_continue_rohc_r17_e_   sdt_drb_continue_rohc_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaConfiguration-v1700 ::= SEQUENCE
struct area_cfg_v1700_s {
  using inter_freq_target_list_r17_l_ = dyn_array<inter_freq_target_info_r16_s>;

  // member variables
  bool                          area_cfg_r17_present = false;
  area_cfg_r16_c                area_cfg_r17;
  inter_freq_target_list_r17_l_ inter_freq_target_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierInfoNR ::= SEQUENCE
struct carrier_info_nr_s {
  bool                 ext          = false;
  bool                 smtc_present = false;
  uint32_t             carrier_freq = 0;
  subcarrier_spacing_e ssb_subcarrier_spacing;
  ssb_mtc_s            smtc;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransfer-v1700-IEs ::= SEQUENCE
struct dl_info_transfer_v1700_ies_s {
  struct ta_pdc_r17_opts {
    enum options { activ, deactiv, nulltype } value;

    const char* to_string() const;
  };
  using ta_pdc_r17_e_ = enumerated<ta_pdc_r17_opts>;

  // member variables
  bool                  rx_tx_time_diff_gnb_r17_present = false;
  bool                  ta_pdc_r17_present              = false;
  bool                  sib9_fallback_r17_present       = false;
  bool                  non_crit_ext_present            = false;
  dyn_octstring         ded_info_f1c_r17;
  rx_tx_time_diff_r17_s rx_tx_time_diff_gnb_r17;
  ta_pdc_r17_e_         ta_pdc_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-CountMSB-Info ::= SEQUENCE
struct drb_count_msb_info_s {
  uint8_t  drb_id       = 1;
  uint32_t count_msb_ul = 0;
  uint32_t count_msb_dl = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventType-r16 ::= CHOICE
struct event_type_r16_c {
  struct event_l1_s_ {
    meas_trigger_quant_c l1_thres;
    uint8_t              hysteresis = 0;
    time_to_trigger_e    time_to_trigger;
  };
  struct types_opts {
    enum options { out_of_coverage, event_l1, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  event_type_r16_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  event_l1_s_& event_l1()
  {
    assert_choice_type(types::event_l1, type_, "EventType-r16");
    return c;
  }
  const event_l1_s_& event_l1() const
  {
    assert_choice_type(types::event_l1, type_, "EventType-r16");
    return c;
  }
  void         set_out_of_coverage();
  event_l1_s_& set_event_l1();

private:
  types       type_;
  event_l1_s_ c;
};

// ExtendedPagingCycle-r17 ::= ENUMERATED
struct extended_paging_cycle_r17_opts {
  enum options { rf256, rf512, rf1024, spare1, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using extended_paging_cycle_r17_e = enumerated<extended_paging_cycle_r17_opts>;

// FreqPriorityListDedicatedSlicing-r17 ::= SEQUENCE (SIZE (1..8)) OF FreqPriorityDedicatedSlicing-r17
using freq_prio_list_ded_slicing_r17_l = dyn_array<freq_prio_ded_slicing_r17_s>;

// FreqPriorityListEUTRA ::= SEQUENCE (SIZE (1..8)) OF FreqPriorityEUTRA
using freq_prio_list_eutra_l = dyn_array<freq_prio_eutra_s>;

// FreqPriorityListNR ::= SEQUENCE (SIZE (1..8)) OF FreqPriorityNR
using freq_prio_list_nr_l = dyn_array<freq_prio_nr_s>;

// LoggingInterval-r16 ::= ENUMERATED
struct logging_interv_r16_opts {
  enum options {
    ms320,
    ms640,
    ms1280,
    ms2560,
    ms5120,
    ms10240,
    ms20480,
    ms30720,
    ms40960,
    ms61440,
    infinity,
    nulltype
  } value;
  typedef int32_t number_type;

  const char* to_string() const;
  int32_t     to_number() const;
};
using logging_interv_r16_e = enumerated<logging_interv_r16_opts>;

// MasterKeyUpdate ::= SEQUENCE
struct master_key_upd_s {
  bool          ext                     = false;
  bool          key_set_change_ind      = false;
  uint8_t       next_hop_chaining_count = 0;
  dyn_octstring nas_container;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OtherConfig ::= SEQUENCE
struct other_cfg_s {
  struct delay_budget_report_cfg_c_ {
    struct setup_s_ {
      struct delay_budget_report_prohibit_timer_opts {
        enum options { s0, s0dot4, s0dot8, s1dot6, s3, s6, s12, s30, nulltype } value;
        typedef float number_type;

        const char* to_string() const;
        float       to_number() const;
        const char* to_number_string() const;
      };
      using delay_budget_report_prohibit_timer_e_ = enumerated<delay_budget_report_prohibit_timer_opts>;

      // member variables
      delay_budget_report_prohibit_timer_e_ delay_budget_report_prohibit_timer;
    };
    struct types_opts {
      enum options { release, setup, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    delay_budget_report_cfg_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "delayBudgetReportingConfig");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "delayBudgetReportingConfig");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                       delay_budget_report_cfg_present = false;
  delay_budget_report_cfg_c_ delay_budget_report_cfg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PeriodicRNAU-TimerValue ::= ENUMERATED
struct periodic_rnau_timer_value_opts {
  enum options { min5, min10, min20, min30, min60, min120, min360, min720, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using periodic_rnau_timer_value_e = enumerated<periodic_rnau_timer_value_opts>;

// RAN-NotificationAreaInfo ::= CHOICE
struct ran_notif_area_info_c {
  struct types_opts {
    enum options { cell_list, ran_area_cfg_list, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  ran_notif_area_info_c() = default;
  ran_notif_area_info_c(const ran_notif_area_info_c& other);
  ran_notif_area_info_c& operator=(const ran_notif_area_info_c& other);
  ~ran_notif_area_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  plmn_ran_area_cell_list_l& cell_list()
  {
    assert_choice_type(types::cell_list, type_, "RAN-NotificationAreaInfo");
    return c.get<plmn_ran_area_cell_list_l>();
  }
  plmn_ran_area_cfg_list_l& ran_area_cfg_list()
  {
    assert_choice_type(types::ran_area_cfg_list, type_, "RAN-NotificationAreaInfo");
    return c.get<plmn_ran_area_cfg_list_l>();
  }
  const plmn_ran_area_cell_list_l& cell_list() const
  {
    assert_choice_type(types::cell_list, type_, "RAN-NotificationAreaInfo");
    return c.get<plmn_ran_area_cell_list_l>();
  }
  const plmn_ran_area_cfg_list_l& ran_area_cfg_list() const
  {
    assert_choice_type(types::ran_area_cfg_list, type_, "RAN-NotificationAreaInfo");
    return c.get<plmn_ran_area_cfg_list_l>();
  }
  plmn_ran_area_cell_list_l& set_cell_list();
  plmn_ran_area_cfg_list_l&  set_ran_area_cfg_list();

private:
  types                                                                type_;
  choice_buffer_t<plmn_ran_area_cell_list_l, plmn_ran_area_cfg_list_l> c;

  void destroy_();
};

// RedirectedCarrierInfo-EUTRA ::= SEQUENCE
struct redirected_carrier_info_eutra_s {
  struct cn_type_opts {
    enum options { epc, five_gc, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using cn_type_e_ = enumerated<cn_type_opts>;

  // member variables
  bool       cn_type_present = false;
  uint32_t   eutra_freq      = 0;
  cn_type_e_ cn_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfiguration-v1540-IEs ::= SEQUENCE
struct rrc_recfg_v1540_ies_s {
  bool                  other_cfg_v1540_present = false;
  bool                  non_crit_ext_present    = false;
  other_cfg_v1540_s     other_cfg_v1540;
  rrc_recfg_v1560_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCRelease-v1610-IEs ::= SEQUENCE
struct rrc_release_v1610_ies_s {
  bool                                     voice_fallback_ind_r16_present = false;
  bool                                     meas_idle_cfg_r16_present      = false;
  bool                                     non_crit_ext_present           = false;
  setup_release_c<meas_idle_cfg_ded_r16_s> meas_idle_cfg_r16;
  rrc_release_v1650_ies_s                  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResume-v1610-IEs ::= SEQUENCE
struct rrc_resume_v1610_ies_s {
  struct mrdc_secondary_cell_group_r16_c_ {
    struct types_opts {
      enum options { nr_scg_r16, eutra_scg_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    mrdc_secondary_cell_group_r16_c_() = default;
    mrdc_secondary_cell_group_r16_c_(const mrdc_secondary_cell_group_r16_c_& other);
    mrdc_secondary_cell_group_r16_c_& operator=(const mrdc_secondary_cell_group_r16_c_& other);
    ~mrdc_secondary_cell_group_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dyn_octstring& nr_scg_r16()
    {
      assert_choice_type(types::nr_scg_r16, type_, "mrdc-SecondaryCellGroup-r16");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& eutra_scg_r16()
    {
      assert_choice_type(types::eutra_scg_r16, type_, "mrdc-SecondaryCellGroup-r16");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& nr_scg_r16() const
    {
      assert_choice_type(types::nr_scg_r16, type_, "mrdc-SecondaryCellGroup-r16");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& eutra_scg_r16() const
    {
      assert_choice_type(types::eutra_scg_r16, type_, "mrdc-SecondaryCellGroup-r16");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& set_nr_scg_r16();
    dyn_octstring& set_eutra_scg_r16();

  private:
    types                          type_;
    choice_buffer_t<dyn_octstring> c;

    void destroy_();
  };

  // member variables
  bool                                        idle_mode_meas_req_r16_present        = false;
  bool                                        restore_mcg_scells_r16_present        = false;
  bool                                        restore_scg_r16_present               = false;
  bool                                        mrdc_secondary_cell_group_r16_present = false;
  bool                                        need_for_gaps_cfg_nr_r16_present      = false;
  bool                                        non_crit_ext_present                  = false;
  mrdc_secondary_cell_group_r16_c_            mrdc_secondary_cell_group_r16;
  setup_release_c<need_for_gaps_cfg_nr_r16_s> need_for_gaps_cfg_nr_r16;
  rrc_resume_v1700_ies_s                      non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TraceReference-r16 ::= SEQUENCE
struct trace_ref_r16_s {
  plmn_id_s          plmn_id_r16;
  fixed_octstring<3> trace_id_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRAT-Request ::= SEQUENCE
struct ue_cap_rat_request_s {
  bool          ext = false;
  rat_type_e    rat_type;
  dyn_octstring cap_request_filt;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaConfiguration-r16 ::= SEQUENCE
struct area_cfg_r16_s {
  using inter_freq_target_list_r16_l_ = dyn_array<inter_freq_target_info_r16_s>;

  // member variables
  area_cfg_r16_c                area_cfg_r16;
  inter_freq_target_list_r16_l_ inter_freq_target_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellReselectionPriorities ::= SEQUENCE
struct cell_resel_priorities_s {
  struct t320_opts {
    enum options { min5, min10, min20, min30, min60, min120, min180, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using t320_e_ = enumerated<t320_opts>;

  // member variables
  bool                   ext          = false;
  bool                   t320_present = false;
  freq_prio_list_eutra_l freq_prio_list_eutra;
  freq_prio_list_nr_l    freq_prio_list_nr;
  t320_e_                t320;
  // ...
  // group 0
  copy_ptr<freq_prio_list_ded_slicing_r17_l> freq_prio_list_ded_slicing_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransfer-v1610-IEs ::= SEQUENCE
struct dl_info_transfer_v1610_ies_s {
  bool                         ref_time_info_r16_present = false;
  bool                         non_crit_ext_present      = false;
  ref_time_info_r16_s          ref_time_info_r16;
  dl_info_transfer_v1700_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-CountMSB-InfoList ::= SEQUENCE (SIZE (1..29)) OF DRB-CountMSB-Info
using drb_count_msb_info_list_l = dyn_array<drb_count_msb_info_s>;

// LoggedEventTriggerConfig-r16 ::= SEQUENCE
struct logged_event_trigger_cfg_r16_s {
  bool                 ext = false;
  event_type_r16_c     event_type_r16;
  logging_interv_r16_e logging_interv_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LoggedMeasurementConfiguration-v1700-IEs ::= SEQUENCE
struct logged_meas_cfg_v1700_ies_s {
  bool             sig_logged_meas_type_r17_present = false;
  bool             early_meas_ind_r17_present       = false;
  bool             area_cfg_v1700_present           = false;
  bool             non_crit_ext_present             = false;
  area_cfg_v1700_s area_cfg_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LoggedPeriodicalReportConfig-r16 ::= SEQUENCE
struct logged_periodical_report_cfg_r16_s {
  bool                 ext = false;
  logging_interv_r16_e logging_interv_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LoggingDuration-r16 ::= ENUMERATED
struct logging_dur_r16_opts {
  enum options { min10, min20, min40, min60, min90, min120, spare2, spare1, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using logging_dur_r16_e = enumerated<logging_dur_r16_opts>;

// MobilityFromNRCommand-v1610-IEs ::= SEQUENCE
struct mob_from_nr_cmd_v1610_ies_s {
  bool voice_fallback_ind_r16_present = false;
  bool non_crit_ext_present           = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityList2-r16 ::= SEQUENCE (SIZE (1..16)) OF PLMN-Identity
using plmn_id_list2_r16_l = dyn_array<plmn_id_s>;

// RedirectedCarrierInfo ::= CHOICE
struct redirected_carrier_info_c {
  struct types_opts {
    enum options { nr, eutra, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  redirected_carrier_info_c() = default;
  redirected_carrier_info_c(const redirected_carrier_info_c& other);
  redirected_carrier_info_c& operator=(const redirected_carrier_info_c& other);
  ~redirected_carrier_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  carrier_info_nr_s& nr()
  {
    assert_choice_type(types::nr, type_, "RedirectedCarrierInfo");
    return c.get<carrier_info_nr_s>();
  }
  redirected_carrier_info_eutra_s& eutra()
  {
    assert_choice_type(types::eutra, type_, "RedirectedCarrierInfo");
    return c.get<redirected_carrier_info_eutra_s>();
  }
  const carrier_info_nr_s& nr() const
  {
    assert_choice_type(types::nr, type_, "RedirectedCarrierInfo");
    return c.get<carrier_info_nr_s>();
  }
  const redirected_carrier_info_eutra_s& eutra() const
  {
    assert_choice_type(types::eutra, type_, "RedirectedCarrierInfo");
    return c.get<redirected_carrier_info_eutra_s>();
  }
  carrier_info_nr_s&               set_nr();
  redirected_carrier_info_eutra_s& set_eutra();

private:
  types                                                               type_;
  choice_buffer_t<carrier_info_nr_s, redirected_carrier_info_eutra_s> c;

  void destroy_();
};

// RRCReconfiguration-v1530-IEs ::= SEQUENCE
struct rrc_recfg_v1530_ies_s {
  using ded_nas_msg_list_l_ = bounded_array<dyn_octstring, 29>;

  // member variables
  bool                  full_cfg_present       = false;
  bool                  master_key_upd_present = false;
  bool                  other_cfg_present      = false;
  bool                  non_crit_ext_present   = false;
  dyn_octstring         master_cell_group;
  ded_nas_msg_list_l_   ded_nas_msg_list;
  master_key_upd_s      master_key_upd;
  dyn_octstring         ded_sib1_delivery;
  dyn_octstring         ded_sys_info_delivery;
  other_cfg_s           other_cfg;
  rrc_recfg_v1540_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReestablishment-v1700-IEs ::= SEQUENCE
struct rrc_reest_v1700_ies_s {
  bool                                       sl_l2_remote_ue_cfg_r17_present = false;
  bool                                       non_crit_ext_present            = false;
  setup_release_c<sl_l2_remote_ue_cfg_r17_s> sl_l2_remote_ue_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCRelease-v1540-IEs ::= SEQUENCE
struct rrc_release_v1540_ies_s {
  bool                    wait_time_present    = false;
  bool                    non_crit_ext_present = false;
  uint8_t                 wait_time            = 1;
  rrc_release_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResume-v1560-IEs ::= SEQUENCE
struct rrc_resume_v1560_ies_s {
  bool                   sk_counter_present   = false;
  bool                   non_crit_ext_present = false;
  dyn_octstring          radio_bearer_cfg2;
  uint32_t               sk_counter = 0;
  rrc_resume_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityConfigSMC ::= SEQUENCE
struct security_cfg_smc_s {
  bool                     ext = false;
  security_algorithm_cfg_s security_algorithm_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SuspendConfig ::= SEQUENCE
struct suspend_cfg_s {
  bool                        ext                         = false;
  bool                        ran_notif_area_info_present = false;
  bool                        t380_present                = false;
  fixed_bitstring<40>         full_i_rnti;
  fixed_bitstring<24>         short_i_rnti;
  paging_cycle_e              ran_paging_cycle;
  ran_notif_area_info_c       ran_notif_area_info;
  periodic_rnau_timer_value_e t380;
  uint8_t                     next_hop_chaining_count = 0;
  // ...
  // group 0
  bool                                     sl_ue_id_remote_r17_present           = false;
  bool                                     ran_extended_paging_cycle_r17_present = false;
  uint32_t                                 sl_ue_id_remote_r17                   = 0;
  copy_ptr<setup_release_c<sdt_cfg_r17_s>> sdt_cfg_r17;
  copy_ptr<setup_release_c<dyn_octstring>> srs_pos_rrc_inactive_r17;
  extended_paging_cycle_r17_e              ran_extended_paging_cycle_r17;
  // group 1
  copy_ptr<setup_release_c<non_cell_defining_ssb_r17_s>> ncd_ssb_red_cap_init_bwp_sdt_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRAT-RequestList ::= SEQUENCE (SIZE (1..8)) OF UE-CapabilityRAT-Request
using ue_cap_rat_request_list_l = dyn_array<ue_cap_rat_request_s>;

// UEInformationRequest-v1700-IEs ::= SEQUENCE
struct ue_info_request_v1700_ies_s {
  bool success_ho_report_req_r17_present   = false;
  bool coarse_location_request_r17_present = false;
  bool non_crit_ext_present                = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CounterCheck-IEs ::= SEQUENCE
struct counter_check_ies_s {
  bool                      non_crit_ext_present = false;
  drb_count_msb_info_list_l drb_count_msb_info_list;
  dyn_octstring             late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLDedicatedMessageSegment-r16-IEs ::= SEQUENCE
struct dl_ded_msg_segment_r16_ies_s {
  struct rrc_msg_segment_type_r16_opts {
    enum options { not_last_segment, last_segment, nulltype } value;

    const char* to_string() const;
  };
  using rrc_msg_segment_type_r16_e_ = enumerated<rrc_msg_segment_type_r16_opts>;

  // member variables
  bool                        non_crit_ext_present = false;
  uint8_t                     segment_num_r16      = 0;
  dyn_octstring               rrc_msg_segment_container_r16;
  rrc_msg_segment_type_r16_e_ rrc_msg_segment_type_r16;
  dyn_octstring               late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransfer-IEs ::= SEQUENCE
struct dl_info_transfer_ies_s {
  bool                         non_crit_ext_present = false;
  dyn_octstring                ded_nas_msg;
  dyn_octstring                late_non_crit_ext;
  dl_info_transfer_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransferMRDC-r16-IEs ::= SEQUENCE
struct dl_info_transfer_mrdc_r16_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring dl_dcch_msg_nr_r16;
  dyn_octstring dl_dcch_msg_eutra_r16;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LoggedMeasurementConfiguration-r16-IEs ::= SEQUENCE
struct logged_meas_cfg_r16_ies_s {
  struct report_type_c_ {
    struct types_opts {
      enum options { periodical, event_triggered, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    report_type_c_() = default;
    report_type_c_(const report_type_c_& other);
    report_type_c_& operator=(const report_type_c_& other);
    ~report_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    logged_periodical_report_cfg_r16_s& periodical()
    {
      assert_choice_type(types::periodical, type_, "reportType");
      return c.get<logged_periodical_report_cfg_r16_s>();
    }
    logged_event_trigger_cfg_r16_s& event_triggered()
    {
      assert_choice_type(types::event_triggered, type_, "reportType");
      return c.get<logged_event_trigger_cfg_r16_s>();
    }
    const logged_periodical_report_cfg_r16_s& periodical() const
    {
      assert_choice_type(types::periodical, type_, "reportType");
      return c.get<logged_periodical_report_cfg_r16_s>();
    }
    const logged_event_trigger_cfg_r16_s& event_triggered() const
    {
      assert_choice_type(types::event_triggered, type_, "reportType");
      return c.get<logged_event_trigger_cfg_r16_s>();
    }
    logged_periodical_report_cfg_r16_s& set_periodical();
    logged_event_trigger_cfg_r16_s&     set_event_triggered();

  private:
    types                                                                               type_;
    choice_buffer_t<logged_event_trigger_cfg_r16_s, logged_periodical_report_cfg_r16_s> c;

    void destroy_();
  };

  // member variables
  bool                                                         area_cfg_r16_present         = false;
  bool                                                         bt_name_list_r16_present     = false;
  bool                                                         wlan_name_list_r16_present   = false;
  bool                                                         sensor_name_list_r16_present = false;
  bool                                                         non_crit_ext_present         = false;
  trace_ref_r16_s                                              trace_ref_r16;
  fixed_octstring<2>                                           trace_recording_session_ref_r16;
  fixed_octstring<1>                                           tce_id_r16;
  fixed_bitstring<48>                                          absolute_time_info_r16;
  area_cfg_r16_s                                               area_cfg_r16;
  plmn_id_list2_r16_l                                          plmn_id_list_r16;
  setup_release_c<dyn_seq_of<bounded_octstring<1, 248>, 1, 4>> bt_name_list_r16;
  setup_release_c<dyn_seq_of<bounded_octstring<1, 32>, 1, 4>>  wlan_name_list_r16;
  setup_release_c<sensor_name_list_r16_s>                      sensor_name_list_r16;
  logging_dur_r16_e                                            logging_dur_r16;
  report_type_c_                                               report_type;
  dyn_octstring                                                late_non_crit_ext;
  logged_meas_cfg_v1700_ies_s                                  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityFromNRCommand-IEs ::= SEQUENCE
struct mob_from_nr_cmd_ies_s {
  struct target_rat_type_opts {
    enum options { eutra, utra_fdd_v1610, spare2, spare1, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using target_rat_type_e_ = enumerated<target_rat_type_opts, true>;

  // member variables
  bool                        non_crit_ext_present = false;
  target_rat_type_e_          target_rat_type;
  dyn_octstring               target_rat_msg_container;
  dyn_octstring               nas_security_param_from_nr;
  dyn_octstring               late_non_crit_ext;
  mob_from_nr_cmd_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfiguration-IEs ::= SEQUENCE
struct rrc_recfg_ies_s {
  bool                  radio_bearer_cfg_present = false;
  bool                  meas_cfg_present         = false;
  bool                  non_crit_ext_present     = false;
  radio_bearer_cfg_s    radio_bearer_cfg;
  dyn_octstring         secondary_cell_group;
  meas_cfg_s            meas_cfg;
  dyn_octstring         late_non_crit_ext;
  rrc_recfg_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReestablishment-IEs ::= SEQUENCE
struct rrc_reest_ies_s {
  bool                  non_crit_ext_present    = false;
  uint8_t               next_hop_chaining_count = 0;
  dyn_octstring         late_non_crit_ext;
  rrc_reest_v1700_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCRelease-IEs ::= SEQUENCE
struct rrc_release_ies_s {
  struct depriorit_req_s_ {
    struct depriorit_type_opts {
      enum options { freq, nr, nulltype } value;

      const char* to_string() const;
    };
    using depriorit_type_e_ = enumerated<depriorit_type_opts>;
    struct depriorit_timer_opts {
      enum options { min5, min10, min15, min30, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using depriorit_timer_e_ = enumerated<depriorit_timer_opts>;

    // member variables
    depriorit_type_e_  depriorit_type;
    depriorit_timer_e_ depriorit_timer;
  };

  // member variables
  bool                      redirected_carrier_info_present = false;
  bool                      cell_resel_priorities_present   = false;
  bool                      suspend_cfg_present             = false;
  bool                      depriorit_req_present           = false;
  bool                      non_crit_ext_present            = false;
  redirected_carrier_info_c redirected_carrier_info;
  cell_resel_priorities_s   cell_resel_priorities;
  suspend_cfg_s             suspend_cfg;
  depriorit_req_s_          depriorit_req;
  dyn_octstring             late_non_crit_ext;
  rrc_release_v1540_ies_s   non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResume-IEs ::= SEQUENCE
struct rrc_resume_ies_s {
  bool                   radio_bearer_cfg_present = false;
  bool                   meas_cfg_present         = false;
  bool                   full_cfg_present         = false;
  bool                   non_crit_ext_present     = false;
  radio_bearer_cfg_s     radio_bearer_cfg;
  dyn_octstring          master_cell_group;
  meas_cfg_s             meas_cfg;
  dyn_octstring          late_non_crit_ext;
  rrc_resume_v1560_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeCommand-IEs ::= SEQUENCE
struct security_mode_cmd_ies_s {
  bool               non_crit_ext_present = false;
  security_cfg_smc_s security_cfg_smc;
  dyn_octstring      late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityEnquiry-IEs ::= SEQUENCE
struct ue_cap_enquiry_ies_s {
  ue_cap_rat_request_list_l ue_cap_rat_request_list;
  dyn_octstring             late_non_crit_ext;
  dyn_octstring             ue_cap_enquiry_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationRequest-r16-IEs ::= SEQUENCE
struct ue_info_request_r16_ies_s {
  bool                        idle_mode_meas_req_r16_present       = false;
  bool                        log_meas_report_req_r16_present      = false;
  bool                        conn_est_fail_report_req_r16_present = false;
  bool                        ra_report_req_r16_present            = false;
  bool                        rlf_report_req_r16_present           = false;
  bool                        mob_history_report_req_r16_present   = false;
  bool                        non_crit_ext_present                 = false;
  dyn_octstring               late_non_crit_ext;
  ue_info_request_v1700_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CounterCheck ::= SEQUENCE
struct counter_check_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { counter_check, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    counter_check_ies_s& counter_check()
    {
      assert_choice_type(types::counter_check, type_, "criticalExtensions");
      return c;
    }
    const counter_check_ies_s& counter_check() const
    {
      assert_choice_type(types::counter_check, type_, "criticalExtensions");
      return c;
    }
    counter_check_ies_s& set_counter_check();
    void                 set_crit_exts_future();

  private:
    types               type_;
    counter_check_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLDedicatedMessageSegment-r16 ::= SEQUENCE
struct dl_ded_msg_segment_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { dl_ded_msg_segment_r16, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dl_ded_msg_segment_r16_ies_s& dl_ded_msg_segment_r16()
    {
      assert_choice_type(types::dl_ded_msg_segment_r16, type_, "criticalExtensions");
      return c;
    }
    const dl_ded_msg_segment_r16_ies_s& dl_ded_msg_segment_r16() const
    {
      assert_choice_type(types::dl_ded_msg_segment_r16, type_, "criticalExtensions");
      return c;
    }
    dl_ded_msg_segment_r16_ies_s& set_dl_ded_msg_segment_r16();
    void                          set_crit_exts_future();

  private:
    types                        type_;
    dl_ded_msg_segment_r16_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransferMRDC-r16 ::= SEQUENCE
struct dl_info_transfer_mrdc_r16_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { dl_info_transfer_mrdc_r16, spare3, spare2, spare1, nulltype } value;

        const char* to_string() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      c1_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      dl_info_transfer_mrdc_r16_ies_s& dl_info_transfer_mrdc_r16()
      {
        assert_choice_type(types::dl_info_transfer_mrdc_r16, type_, "c1");
        return c;
      }
      const dl_info_transfer_mrdc_r16_ies_s& dl_info_transfer_mrdc_r16() const
      {
        assert_choice_type(types::dl_info_transfer_mrdc_r16, type_, "c1");
        return c;
      }
      dl_info_transfer_mrdc_r16_ies_s& set_dl_info_transfer_mrdc_r16();
      void                             set_spare3();
      void                             set_spare2();
      void                             set_spare1();

    private:
      types                           type_;
      dl_info_transfer_mrdc_r16_ies_s c;
    };
    struct types_opts {
      enum options { c1, crit_exts_future, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    c1_c_& c1()
    {
      assert_choice_type(types::c1, type_, "criticalExtensions");
      return c;
    }
    const c1_c_& c1() const
    {
      assert_choice_type(types::c1, type_, "criticalExtensions");
      return c;
    }
    c1_c_& set_c1();
    void   set_crit_exts_future();

  private:
    types type_;
    c1_c_ c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransfer ::= SEQUENCE
struct dl_info_transfer_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { dl_info_transfer, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dl_info_transfer_ies_s& dl_info_transfer()
    {
      assert_choice_type(types::dl_info_transfer, type_, "criticalExtensions");
      return c;
    }
    const dl_info_transfer_ies_s& dl_info_transfer() const
    {
      assert_choice_type(types::dl_info_transfer, type_, "criticalExtensions");
      return c;
    }
    dl_info_transfer_ies_s& set_dl_info_transfer();
    void                    set_crit_exts_future();

  private:
    types                  type_;
    dl_info_transfer_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LoggedMeasurementConfiguration-r16 ::= SEQUENCE
struct logged_meas_cfg_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { logged_meas_cfg_r16, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    logged_meas_cfg_r16_ies_s& logged_meas_cfg_r16()
    {
      assert_choice_type(types::logged_meas_cfg_r16, type_, "criticalExtensions");
      return c;
    }
    const logged_meas_cfg_r16_ies_s& logged_meas_cfg_r16() const
    {
      assert_choice_type(types::logged_meas_cfg_r16, type_, "criticalExtensions");
      return c;
    }
    logged_meas_cfg_r16_ies_s& set_logged_meas_cfg_r16();
    void                       set_crit_exts_future();

  private:
    types                     type_;
    logged_meas_cfg_r16_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityFromNRCommand ::= SEQUENCE
struct mob_from_nr_cmd_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { mob_from_nr_cmd, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mob_from_nr_cmd_ies_s& mob_from_nr_cmd()
    {
      assert_choice_type(types::mob_from_nr_cmd, type_, "criticalExtensions");
      return c;
    }
    const mob_from_nr_cmd_ies_s& mob_from_nr_cmd() const
    {
      assert_choice_type(types::mob_from_nr_cmd, type_, "criticalExtensions");
      return c;
    }
    mob_from_nr_cmd_ies_s& set_mob_from_nr_cmd();
    void                   set_crit_exts_future();

  private:
    types                 type_;
    mob_from_nr_cmd_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfiguration ::= SEQUENCE
struct rrc_recfg_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_recfg, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_recfg_ies_s& rrc_recfg()
    {
      assert_choice_type(types::rrc_recfg, type_, "criticalExtensions");
      return c;
    }
    const rrc_recfg_ies_s& rrc_recfg() const
    {
      assert_choice_type(types::rrc_recfg, type_, "criticalExtensions");
      return c;
    }
    rrc_recfg_ies_s& set_rrc_recfg();
    void             set_crit_exts_future();

  private:
    types           type_;
    rrc_recfg_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReestablishment ::= SEQUENCE
struct rrc_reest_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_reest, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_reest_ies_s& rrc_reest()
    {
      assert_choice_type(types::rrc_reest, type_, "criticalExtensions");
      return c;
    }
    const rrc_reest_ies_s& rrc_reest() const
    {
      assert_choice_type(types::rrc_reest, type_, "criticalExtensions");
      return c;
    }
    rrc_reest_ies_s& set_rrc_reest();
    void             set_crit_exts_future();

  private:
    types           type_;
    rrc_reest_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCRelease ::= SEQUENCE
struct rrc_release_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_release, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_release_ies_s& rrc_release()
    {
      assert_choice_type(types::rrc_release, type_, "criticalExtensions");
      return c;
    }
    const rrc_release_ies_s& rrc_release() const
    {
      assert_choice_type(types::rrc_release, type_, "criticalExtensions");
      return c;
    }
    rrc_release_ies_s& set_rrc_release();
    void               set_crit_exts_future();

  private:
    types             type_;
    rrc_release_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResume ::= SEQUENCE
struct rrc_resume_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_resume, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_resume_ies_s& rrc_resume()
    {
      assert_choice_type(types::rrc_resume, type_, "criticalExtensions");
      return c;
    }
    const rrc_resume_ies_s& rrc_resume() const
    {
      assert_choice_type(types::rrc_resume, type_, "criticalExtensions");
      return c;
    }
    rrc_resume_ies_s& set_rrc_resume();
    void              set_crit_exts_future();

  private:
    types            type_;
    rrc_resume_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeCommand ::= SEQUENCE
struct security_mode_cmd_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { security_mode_cmd, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    security_mode_cmd_ies_s& security_mode_cmd()
    {
      assert_choice_type(types::security_mode_cmd, type_, "criticalExtensions");
      return c;
    }
    const security_mode_cmd_ies_s& security_mode_cmd() const
    {
      assert_choice_type(types::security_mode_cmd, type_, "criticalExtensions");
      return c;
    }
    security_mode_cmd_ies_s& set_security_mode_cmd();
    void                     set_crit_exts_future();

  private:
    types                   type_;
    security_mode_cmd_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityEnquiry ::= SEQUENCE
struct ue_cap_enquiry_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ue_cap_enquiry, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_cap_enquiry_ies_s& ue_cap_enquiry()
    {
      assert_choice_type(types::ue_cap_enquiry, type_, "criticalExtensions");
      return c;
    }
    const ue_cap_enquiry_ies_s& ue_cap_enquiry() const
    {
      assert_choice_type(types::ue_cap_enquiry, type_, "criticalExtensions");
      return c;
    }
    ue_cap_enquiry_ies_s& set_ue_cap_enquiry();
    void                  set_crit_exts_future();

  private:
    types                type_;
    ue_cap_enquiry_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationRequest-r16 ::= SEQUENCE
struct ue_info_request_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ue_info_request_r16, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_info_request_r16_ies_s& ue_info_request_r16()
    {
      assert_choice_type(types::ue_info_request_r16, type_, "criticalExtensions");
      return c;
    }
    const ue_info_request_r16_ies_s& ue_info_request_r16() const
    {
      assert_choice_type(types::ue_info_request_r16, type_, "criticalExtensions");
      return c;
    }
    ue_info_request_r16_ies_s& set_ue_info_request_r16();
    void                       set_crit_exts_future();

  private:
    types                     type_;
    ue_info_request_r16_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1
