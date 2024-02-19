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

// DiscardTimerExt-r16 ::= ENUMERATED
struct discard_timer_ext_r16_opts {
  enum options { ms0dot5, ms1, ms2, ms4, ms6, ms8, spare2, spare1, nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
using discard_timer_ext_r16_e = enumerated<discard_timer_ext_r16_opts>;

// DiscardTimerExt2-r17 ::= ENUMERATED
struct discard_timer_ext2_r17_opts {
  enum options { ms2000, spare3, spare2, spare1, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using discard_timer_ext2_r17_e = enumerated<discard_timer_ext2_r17_opts>;

// EthernetHeaderCompression-r16 ::= SEQUENCE
struct ethernet_hdr_compress_r16_s {
  struct ehc_common_r16_s_ {
    struct ehc_c_id_len_r16_opts {
      enum options { bits7, bits15, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using ehc_c_id_len_r16_e_ = enumerated<ehc_c_id_len_r16_opts>;

    // member variables
    bool                ext = false;
    ehc_c_id_len_r16_e_ ehc_c_id_len_r16;
    // ...
  };
  struct ehc_dl_r16_s_ {
    bool ext                             = false;
    bool drb_continue_ehc_dl_r16_present = false;
    // ...
  };
  struct ehc_ul_r16_s_ {
    bool     ext                             = false;
    bool     drb_continue_ehc_ul_r16_present = false;
    uint16_t max_c_id_ehc_ul_r16             = 1;
    // ...
  };

  // member variables
  bool              ehc_dl_r16_present = false;
  bool              ehc_ul_r16_present = false;
  ehc_common_r16_s_ ehc_common_r16;
  ehc_dl_r16_s_     ehc_dl_r16;
  ehc_ul_r16_s_     ehc_ul_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-DataSplitThreshold ::= ENUMERATED
struct ul_data_split_thres_opts {
  enum options {
    b0,
    b100,
    b200,
    b400,
    b800,
    b1600,
    b3200,
    b6400,
    b12800,
    b25600,
    b51200,
    b102400,
    b204800,
    b409600,
    b819200,
    b1228800,
    b1638400,
    b2457600,
    b3276800,
    b4096000,
    b4915200,
    b5734400,
    b6553600,
    infinity,
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
  typedef int32_t number_type;

  const char* to_string() const;
  int32_t     to_number() const;
};
using ul_data_split_thres_e = enumerated<ul_data_split_thres_opts>;

// UplinkDataCompression-r17 ::= CHOICE
struct ul_data_compress_r17_c {
  struct new_setup_s_ {
    struct buffer_size_r17_opts {
      enum options { kbyte2, kbyte4, kbyte8, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using buffer_size_r17_e_ = enumerated<buffer_size_r17_opts>;
    struct dictionary_r17_opts {
      enum options { sip_sdp, operator_value, nulltype } value;

      const char* to_string() const;
    };
    using dictionary_r17_e_ = enumerated<dictionary_r17_opts>;

    // member variables
    bool               dictionary_r17_present = false;
    buffer_size_r17_e_ buffer_size_r17;
    dictionary_r17_e_  dictionary_r17;
  };
  struct types_opts {
    enum options { new_setup, drb_continue_udc, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ul_data_compress_r17_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  new_setup_s_& new_setup()
  {
    assert_choice_type(types::new_setup, type_, "UplinkDataCompression-r17");
    return c;
  }
  const new_setup_s_& new_setup() const
  {
    assert_choice_type(types::new_setup, type_, "UplinkDataCompression-r17");
    return c;
  }
  new_setup_s_& set_new_setup();
  void          set_drb_continue_udc();

private:
  types        type_;
  new_setup_s_ c;
};

// CipheringAlgorithm ::= ENUMERATED
struct ciphering_algorithm_opts {
  enum options { nea0, nea1, nea2, nea3, spare4, spare3, spare2, spare1, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using ciphering_algorithm_e = enumerated<ciphering_algorithm_opts, true>;

// IntegrityProtAlgorithm ::= ENUMERATED
struct integrity_prot_algorithm_opts {
  enum options { nia0, nia1, nia2, nia3, spare4, spare3, spare2, spare1, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using integrity_prot_algorithm_e = enumerated<integrity_prot_algorithm_opts, true>;

// PDCP-Config ::= SEQUENCE
struct pdcp_cfg_s {
  struct drb_s_ {
    struct discard_timer_opts {
      enum options {
        ms10,
        ms20,
        ms30,
        ms40,
        ms50,
        ms60,
        ms75,
        ms100,
        ms150,
        ms200,
        ms250,
        ms300,
        ms500,
        ms750,
        ms1500,
        infinity,
        nulltype
      } value;
      typedef int16_t number_type;

      const char* to_string() const;
      int16_t     to_number() const;
    };
    using discard_timer_e_ = enumerated<discard_timer_opts>;
    struct pdcp_sn_size_ul_opts {
      enum options { len12bits, len18bits, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using pdcp_sn_size_ul_e_ = enumerated<pdcp_sn_size_ul_opts>;
    struct pdcp_sn_size_dl_opts {
      enum options { len12bits, len18bits, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using pdcp_sn_size_dl_e_ = enumerated<pdcp_sn_size_dl_opts>;
    struct hdr_compress_c_ {
      struct rohc_s_ {
        struct profiles_s_ {
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

        // member variables
        bool        max_c_id_present          = false;
        bool        drb_continue_rohc_present = false;
        uint16_t    max_c_id                  = 1;
        profiles_s_ profiles;
      };
      struct ul_only_rohc_s_ {
        struct profiles_s_ {
          bool profile0x0006 = false;
        };

        // member variables
        bool        max_c_id_present          = false;
        bool        drb_continue_rohc_present = false;
        uint16_t    max_c_id                  = 1;
        profiles_s_ profiles;
      };
      struct types_opts {
        enum options { not_used, rohc, ul_only_rohc, /*...*/ nulltype } value;

        const char* to_string() const;
      };
      using types = enumerated<types_opts, true>;

      // choice methods
      hdr_compress_c_() = default;
      hdr_compress_c_(const hdr_compress_c_& other);
      hdr_compress_c_& operator=(const hdr_compress_c_& other);
      ~hdr_compress_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      rohc_s_& rohc()
      {
        assert_choice_type(types::rohc, type_, "headerCompression");
        return c.get<rohc_s_>();
      }
      ul_only_rohc_s_& ul_only_rohc()
      {
        assert_choice_type(types::ul_only_rohc, type_, "headerCompression");
        return c.get<ul_only_rohc_s_>();
      }
      const rohc_s_& rohc() const
      {
        assert_choice_type(types::rohc, type_, "headerCompression");
        return c.get<rohc_s_>();
      }
      const ul_only_rohc_s_& ul_only_rohc() const
      {
        assert_choice_type(types::ul_only_rohc, type_, "headerCompression");
        return c.get<ul_only_rohc_s_>();
      }
      void             set_not_used();
      rohc_s_&         set_rohc();
      ul_only_rohc_s_& set_ul_only_rohc();

    private:
      types                                     type_;
      choice_buffer_t<rohc_s_, ul_only_rohc_s_> c;

      void destroy_();
    };

    // member variables
    bool               discard_timer_present          = false;
    bool               pdcp_sn_size_ul_present        = false;
    bool               pdcp_sn_size_dl_present        = false;
    bool               integrity_protection_present   = false;
    bool               status_report_required_present = false;
    bool               out_of_order_delivery_present  = false;
    discard_timer_e_   discard_timer;
    pdcp_sn_size_ul_e_ pdcp_sn_size_ul;
    pdcp_sn_size_dl_e_ pdcp_sn_size_dl;
    hdr_compress_c_    hdr_compress;
  };
  struct more_than_one_rlc_s_ {
    struct primary_path_s_ {
      bool    cell_group_present = false;
      bool    lc_ch_present      = false;
      uint8_t cell_group         = 0;
      uint8_t lc_ch              = 1;
    };

    // member variables
    bool                  ul_data_split_thres_present = false;
    bool                  pdcp_dupl_present           = false;
    primary_path_s_       primary_path;
    ul_data_split_thres_e ul_data_split_thres;
    bool                  pdcp_dupl = false;
  };
  struct t_reordering_opts {
    enum options {
      ms0,
      ms1,
      ms2,
      ms4,
      ms5,
      ms8,
      ms10,
      ms15,
      ms20,
      ms30,
      ms40,
      ms50,
      ms60,
      ms80,
      ms100,
      ms120,
      ms140,
      ms160,
      ms180,
      ms200,
      ms220,
      ms240,
      ms260,
      ms280,
      ms300,
      ms500,
      ms750,
      ms1000,
      ms1250,
      ms1500,
      ms1750,
      ms2000,
      ms2250,
      ms2500,
      ms2750,
      ms3000,
      spare28,
      spare27,
      spare26,
      spare25,
      spare24,
      spare23,
      spare22,
      spare21,
      spare20,
      spare19,
      spare18,
      spare17,
      spare16,
      spare15,
      spare14,
      spare13,
      spare12,
      spare11,
      spare10,
      spare09,
      spare08,
      spare07,
      spare06,
      spare05,
      spare04,
      spare03,
      spare02,
      spare01,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using t_reordering_e_ = enumerated<t_reordering_opts>;
  struct more_than_two_rlc_drb_r16_s_ {
    using dupl_state_r16_l_ = std::array<bool, 3>;

    // member variables
    bool              split_secondary_path_r16_present = false;
    bool              dupl_state_r16_present           = false;
    uint8_t           split_secondary_path_r16         = 1;
    dupl_state_r16_l_ dupl_state_r16;
  };

  // member variables
  bool                 ext                       = false;
  bool                 drb_present               = false;
  bool                 more_than_one_rlc_present = false;
  bool                 t_reordering_present      = false;
  drb_s_               drb;
  more_than_one_rlc_s_ more_than_one_rlc;
  t_reordering_e_      t_reordering;
  // ...
  // group 0
  bool ciphering_disabled_present = false;
  // group 1
  copy_ptr<setup_release_c<discard_timer_ext_r16_e>>     discard_timer_ext_r16;
  copy_ptr<more_than_two_rlc_drb_r16_s_>                 more_than_two_rlc_drb_r16;
  copy_ptr<setup_release_c<ethernet_hdr_compress_r16_s>> ethernet_hdr_compress_r16;
  // group 2
  bool                                                survival_time_state_support_r17_present = false;
  bool                                                init_rx_deliv_r17_present               = false;
  copy_ptr<setup_release_c<ul_data_compress_r17_c>>   ul_data_compress_r17;
  copy_ptr<setup_release_c<discard_timer_ext2_r17_e>> discard_timer_ext2_r17;
  fixed_bitstring<32>                                 init_rx_deliv_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SDAP-Config ::= SEQUENCE
struct sdap_cfg_s {
  struct sdap_hdr_dl_opts {
    enum options { present, absent, nulltype } value;

    const char* to_string() const;
  };
  using sdap_hdr_dl_e_ = enumerated<sdap_hdr_dl_opts>;
  struct sdap_hdr_ul_opts {
    enum options { present, absent, nulltype } value;

    const char* to_string() const;
  };
  using sdap_hdr_ul_e_                 = enumerated<sdap_hdr_ul_opts>;
  using mapped_qos_flows_to_add_l_     = dyn_array<uint8_t>;
  using mapped_qos_flows_to_release_l_ = dyn_array<uint8_t>;

  // member variables
  bool                           ext         = false;
  uint16_t                       pdu_session = 0;
  sdap_hdr_dl_e_                 sdap_hdr_dl;
  sdap_hdr_ul_e_                 sdap_hdr_ul;
  bool                           default_drb = false;
  mapped_qos_flows_to_add_l_     mapped_qos_flows_to_add;
  mapped_qos_flows_to_release_l_ mapped_qos_flows_to_release;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TMGI-r17 ::= SEQUENCE
struct tmgi_r17_s {
  struct plmn_id_r17_c_ {
    struct types_opts {
      enum options { plmn_idx, explicit_value, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    plmn_id_r17_c_() = default;
    plmn_id_r17_c_(const plmn_id_r17_c_& other);
    plmn_id_r17_c_& operator=(const plmn_id_r17_c_& other);
    ~plmn_id_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& plmn_idx()
    {
      assert_choice_type(types::plmn_idx, type_, "plmn-Id-r17");
      return c.get<uint8_t>();
    }
    plmn_id_s& explicit_value()
    {
      assert_choice_type(types::explicit_value, type_, "plmn-Id-r17");
      return c.get<plmn_id_s>();
    }
    const uint8_t& plmn_idx() const
    {
      assert_choice_type(types::plmn_idx, type_, "plmn-Id-r17");
      return c.get<uint8_t>();
    }
    const plmn_id_s& explicit_value() const
    {
      assert_choice_type(types::explicit_value, type_, "plmn-Id-r17");
      return c.get<plmn_id_s>();
    }
    uint8_t&   set_plmn_idx();
    plmn_id_s& set_explicit_value();

  private:
    types                      type_;
    choice_buffer_t<plmn_id_s> c;

    void destroy_();
  };

  // member variables
  plmn_id_r17_c_     plmn_id_r17;
  fixed_octstring<3> service_id_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-ToAddMod ::= SEQUENCE
struct drb_to_add_mod_s {
  struct cn_assoc_c_ {
    struct types_opts {
      enum options { eps_bearer_id, sdap_cfg, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    cn_assoc_c_() = default;
    cn_assoc_c_(const cn_assoc_c_& other);
    cn_assoc_c_& operator=(const cn_assoc_c_& other);
    ~cn_assoc_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& eps_bearer_id()
    {
      assert_choice_type(types::eps_bearer_id, type_, "cnAssociation");
      return c.get<uint8_t>();
    }
    sdap_cfg_s& sdap_cfg()
    {
      assert_choice_type(types::sdap_cfg, type_, "cnAssociation");
      return c.get<sdap_cfg_s>();
    }
    const uint8_t& eps_bearer_id() const
    {
      assert_choice_type(types::eps_bearer_id, type_, "cnAssociation");
      return c.get<uint8_t>();
    }
    const sdap_cfg_s& sdap_cfg() const
    {
      assert_choice_type(types::sdap_cfg, type_, "cnAssociation");
      return c.get<sdap_cfg_s>();
    }
    uint8_t&    set_eps_bearer_id();
    sdap_cfg_s& set_sdap_cfg();

  private:
    types                       type_;
    choice_buffer_t<sdap_cfg_s> c;

    void destroy_();
  };

  // member variables
  bool        ext                      = false;
  bool        cn_assoc_present         = false;
  bool        reestablish_pdcp_present = false;
  bool        recover_pdcp_present     = false;
  bool        pdcp_cfg_present         = false;
  cn_assoc_c_ cn_assoc;
  uint8_t     drb_id = 1;
  pdcp_cfg_s  pdcp_cfg;
  // ...
  // group 0
  bool daps_cfg_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRB-ToAddMod-r17 ::= SEQUENCE
struct mrb_to_add_mod_r17_s {
  bool       ext                          = false;
  bool       mbs_session_id_r17_present   = false;
  bool       mrb_id_new_r17_present       = false;
  bool       reestablish_pdcp_r17_present = false;
  bool       recover_pdcp_r17_present     = false;
  bool       pdcp_cfg_r17_present         = false;
  tmgi_r17_s mbs_session_id_r17;
  uint16_t   mrb_id_r17     = 1;
  uint16_t   mrb_id_new_r17 = 1;
  pdcp_cfg_s pdcp_cfg_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRB-ToAddMod ::= SEQUENCE
struct srb_to_add_mod_s {
  bool       ext                      = false;
  bool       reestablish_pdcp_present = false;
  bool       discard_on_pdcp_present  = false;
  bool       pdcp_cfg_present         = false;
  uint8_t    srb_id                   = 1;
  pdcp_cfg_s pdcp_cfg;
  // ...
  // group 0
  bool    srb_id_v1700_present = false;
  uint8_t srb_id_v1700         = 4;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityAlgorithmConfig ::= SEQUENCE
struct security_algorithm_cfg_s {
  bool                       ext                              = false;
  bool                       integrity_prot_algorithm_present = false;
  ciphering_algorithm_e      ciphering_algorithm;
  integrity_prot_algorithm_e integrity_prot_algorithm;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-ToAddModList ::= SEQUENCE (SIZE (1..29)) OF DRB-ToAddMod
using drb_to_add_mod_list_l = dyn_array<drb_to_add_mod_s>;

// DRB-ToReleaseList ::= SEQUENCE (SIZE (1..29)) OF INTEGER (1..32)
using drb_to_release_list_l = bounded_array<uint8_t, 29>;

// MRB-ToAddModList-r17 ::= SEQUENCE (SIZE (1..32)) OF MRB-ToAddMod-r17
using mrb_to_add_mod_list_r17_l = dyn_array<mrb_to_add_mod_r17_s>;

// MRB-ToReleaseList-r17 ::= SEQUENCE (SIZE (1..32)) OF INTEGER (1..512)
using mrb_to_release_list_r17_l = bounded_array<uint16_t, 32>;

// SRB-ToAddModList ::= SEQUENCE (SIZE (1..2)) OF SRB-ToAddMod
using srb_to_add_mod_list_l = dyn_array<srb_to_add_mod_s>;

// SecurityConfig ::= SEQUENCE
struct security_cfg_s {
  struct key_to_use_opts {
    enum options { master, secondary, nulltype } value;

    const char* to_string() const;
  };
  using key_to_use_e_ = enumerated<key_to_use_opts>;

  // member variables
  bool                     ext                            = false;
  bool                     security_algorithm_cfg_present = false;
  bool                     key_to_use_present             = false;
  security_algorithm_cfg_s security_algorithm_cfg;
  key_to_use_e_            key_to_use;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioBearerConfig ::= SEQUENCE
struct radio_bearer_cfg_s {
  bool                  ext                     = false;
  bool                  srb3_to_release_present = false;
  bool                  security_cfg_present    = false;
  srb_to_add_mod_list_l srb_to_add_mod_list;
  drb_to_add_mod_list_l drb_to_add_mod_list;
  drb_to_release_list_l drb_to_release_list;
  security_cfg_s        security_cfg;
  // ...
  // group 0
  bool                                srb4_to_release_r17_present = false;
  copy_ptr<mrb_to_add_mod_list_r17_l> mrb_to_add_mod_list_r17;
  copy_ptr<mrb_to_release_list_r17_l> mrb_to_release_list_r17;
  copy_ptr<srb_to_add_mod_s>          srb4_to_add_mod_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1
