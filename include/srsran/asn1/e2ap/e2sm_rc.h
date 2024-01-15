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
 *                   3GPP TS ASN1 E2SM RC v16.10.0 (2022-06)
 *
 ******************************************************************************/

#pragma once

#include "../asn1_utils.h"

namespace asn1 {
namespace e2sm_rc {

/*******************************************************************************
 *                             Constant Definitions
 ******************************************************************************/

#define ASN1_E2SM_RC_MAXNOOF_MSGS 65535
#define ASN1_E2SM_RC_MAXNOOF_E2_INFO_CHANGES 65535
#define ASN1_E2SM_RC_MAXNOOF_UE_INFO_CHANGES 65535
#define ASN1_E2SM_RC_MAXNOOF_RRC_STATE 8
#define ASN1_E2SM_RC_MAXNOOF_PARAMS_TO_REPORT 65535
#define ASN1_E2SM_RC_MAXNOOF_POLICY_CONDITIONS 65535
#define ASN1_E2SM_RC_MAXNOOF_ASSOCIATED_RAN_PARAMS 65535
#define ASN1_E2SM_RC_MAXNOOF_UE_ID 65535
#define ASN1_E2SM_RC_MAXNOOF_CELL_ID 65535
#define ASN1_E2SM_RC_MAXNOOF_RAN_OUTCOME_PARAMS 255
#define ASN1_E2SM_RC_MAXNOOF_PARAMSIN_STRUCTURE 65535
#define ASN1_E2SM_RC_MAXNOOF_ITEMSIN_LIST 65535
#define ASN1_E2SM_RC_MAXNOOF_UE_INFO 65535
#define ASN1_E2SM_RC_MAXNOOF_CELL_INFO 65535
#define ASN1_E2SM_RC_MAXNOOF_UE_EVENT_INFO 65535
#define ASN1_E2SM_RC_MAXNOOF_RA_NPARAM_TEST 255
#define ASN1_E2SM_RC_MAXNOOF_NEIGHBOUR_CELL 65535
#define ASN1_E2SM_RC_MAXNOOF_RIC_STYLES 63
#define ASN1_E2SM_RC_MAXNOOF_CALL_PROCESS_TYPES 65535
#define ASN1_E2SM_RC_MAXNOOF_CALL_PROCESS_BREAKPOINTS 65535
#define ASN1_E2SM_RC_MAXNOOF_INSERT_IND 65535
#define ASN1_E2SM_RC_MAXNOOF_CTRL_ACTION 65535
#define ASN1_E2SM_RC_MAXNOOF_POLICY_ACTION 65535
#define ASN1_E2SM_RC_MAXNOOF_INSERT_IND_ACTIONS 63
#define ASN1_E2SM_RC_MAXNOOF_MUL_CTRL_ACTIONS 63
#define ASN1_E2SM_RC_MAX_GROUP_DEFINITION_ID_PARAMS 255
#define ASN1_E2SM_RC_MAXNOOF_ASSOCIATED_ENTITY_FILTS 255
#define ASN1_E2SM_RC_MAXNOOF_FORMAT_TYPES 63

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// AdditionalSupportedFormat-UEGroupControl ::= SEQUENCE
struct add_supported_format_ue_group_ctrl_s {
  bool    ext                      = false;
  int64_t ric_ctrl_hdr_format_type = 0;
  int64_t ric_ctrl_msg_format_type = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-ID ::= CHOICE
struct gnb_id_c {
  struct types_opts {
    enum options { gnb_id, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  types       type() const { return types::gnb_id; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  bounded_bitstring<22, 32, false, true>&       gnb_id() { return c; }
  const bounded_bitstring<22, 32, false, true>& gnb_id() const { return c; }

private:
  bounded_bitstring<22, 32, false, true> c;
};

// NgENB-ID ::= CHOICE
struct ng_enb_id_c {
  struct types_opts {
    enum options { macro_ng_enb_id, short_macro_ng_enb_id, long_macro_ng_enb_id, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  ng_enb_id_c() = default;
  ng_enb_id_c(const ng_enb_id_c& other);
  ng_enb_id_c& operator=(const ng_enb_id_c& other);
  ~ng_enb_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<20, false, true>& macro_ng_enb_id()
  {
    assert_choice_type(types::macro_ng_enb_id, type_, "NgENB-ID");
    return c.get<fixed_bitstring<20, false, true>>();
  }
  fixed_bitstring<18, false, true>& short_macro_ng_enb_id()
  {
    assert_choice_type(types::short_macro_ng_enb_id, type_, "NgENB-ID");
    return c.get<fixed_bitstring<18, false, true>>();
  }
  fixed_bitstring<21, false, true>& long_macro_ng_enb_id()
  {
    assert_choice_type(types::long_macro_ng_enb_id, type_, "NgENB-ID");
    return c.get<fixed_bitstring<21, false, true>>();
  }
  const fixed_bitstring<20, false, true>& macro_ng_enb_id() const
  {
    assert_choice_type(types::macro_ng_enb_id, type_, "NgENB-ID");
    return c.get<fixed_bitstring<20, false, true>>();
  }
  const fixed_bitstring<18, false, true>& short_macro_ng_enb_id() const
  {
    assert_choice_type(types::short_macro_ng_enb_id, type_, "NgENB-ID");
    return c.get<fixed_bitstring<18, false, true>>();
  }
  const fixed_bitstring<21, false, true>& long_macro_ng_enb_id() const
  {
    assert_choice_type(types::long_macro_ng_enb_id, type_, "NgENB-ID");
    return c.get<fixed_bitstring<21, false, true>>();
  }
  fixed_bitstring<20, false, true>& set_macro_ng_enb_id();
  fixed_bitstring<18, false, true>& set_short_macro_ng_enb_id();
  fixed_bitstring<21, false, true>& set_long_macro_ng_enb_id();

private:
  types                                             type_;
  choice_buffer_t<fixed_bitstring<21, false, true>> c;

  void destroy_();
};

// ENB-ID ::= CHOICE
struct enb_id_c {
  struct types_opts {
    enum options { macro_enb_id, home_enb_id, /*...*/ short_macro_enb_id, long_macro_enb_id, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true, 2>;

  // choice methods
  enb_id_c() = default;
  enb_id_c(const enb_id_c& other);
  enb_id_c& operator=(const enb_id_c& other);
  ~enb_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<20, false, true>& macro_enb_id()
  {
    assert_choice_type(types::macro_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<20, false, true>>();
  }
  fixed_bitstring<28, false, true>& home_enb_id()
  {
    assert_choice_type(types::home_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<28, false, true>>();
  }
  fixed_bitstring<18, false, true>& short_macro_enb_id()
  {
    assert_choice_type(types::short_macro_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<18, false, true>>();
  }
  fixed_bitstring<21, false, true>& long_macro_enb_id()
  {
    assert_choice_type(types::long_macro_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<21, false, true>>();
  }
  const fixed_bitstring<20, false, true>& macro_enb_id() const
  {
    assert_choice_type(types::macro_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<20, false, true>>();
  }
  const fixed_bitstring<28, false, true>& home_enb_id() const
  {
    assert_choice_type(types::home_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<28, false, true>>();
  }
  const fixed_bitstring<18, false, true>& short_macro_enb_id() const
  {
    assert_choice_type(types::short_macro_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<18, false, true>>();
  }
  const fixed_bitstring<21, false, true>& long_macro_enb_id() const
  {
    assert_choice_type(types::long_macro_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<21, false, true>>();
  }
  fixed_bitstring<20, false, true>& set_macro_enb_id();
  fixed_bitstring<28, false, true>& set_home_enb_id();
  fixed_bitstring<18, false, true>& set_short_macro_enb_id();
  fixed_bitstring<21, false, true>& set_long_macro_enb_id();

private:
  types                                             type_;
  choice_buffer_t<fixed_bitstring<28, false, true>> c;

  void destroy_();
};

// GlobalGNB-ID ::= SEQUENCE
struct global_gnb_id_s {
  bool                     ext = false;
  fixed_octstring<3, true> plmn_id;
  gnb_id_c                 gnb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalNgENB-ID ::= SEQUENCE
struct global_ng_enb_id_s {
  bool                     ext = false;
  fixed_octstring<3, true> plmn_id;
  ng_enb_id_c              ng_enb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LogicalOR ::= ENUMERATED
struct lc_or_opts {
  enum options { true_value, false_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using lc_or_e = enumerated<lc_or_opts, true>;

// RANParameter-TestingCondition ::= CHOICE
struct ran_param_testing_condition_c {
  struct ran_p_choice_comparison_opts {
    enum options { equal, difference, greaterthan, lessthan, contains, starts_with, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using ran_p_choice_comparison_e_ = enumerated<ran_p_choice_comparison_opts, true>;
  struct ran_p_choice_presence_opts {
    enum options { present, cfg, rollover, non_zero, /*...*/ value_change, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ran_p_choice_presence_e_ = enumerated<ran_p_choice_presence_opts, true, 1>;
  struct types_opts {
    enum options { ran_p_choice_comparison, ran_p_choice_presence, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  ran_param_testing_condition_c() = default;
  ran_param_testing_condition_c(const ran_param_testing_condition_c& other);
  ran_param_testing_condition_c& operator=(const ran_param_testing_condition_c& other);
  ~ran_param_testing_condition_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  ran_p_choice_comparison_e_& ran_p_choice_comparison()
  {
    assert_choice_type(types::ran_p_choice_comparison, type_, "RANParameter-TestingCondition");
    return c.get<ran_p_choice_comparison_e_>();
  }
  ran_p_choice_presence_e_& ran_p_choice_presence()
  {
    assert_choice_type(types::ran_p_choice_presence, type_, "RANParameter-TestingCondition");
    return c.get<ran_p_choice_presence_e_>();
  }
  const ran_p_choice_comparison_e_& ran_p_choice_comparison() const
  {
    assert_choice_type(types::ran_p_choice_comparison, type_, "RANParameter-TestingCondition");
    return c.get<ran_p_choice_comparison_e_>();
  }
  const ran_p_choice_presence_e_& ran_p_choice_presence() const
  {
    assert_choice_type(types::ran_p_choice_presence, type_, "RANParameter-TestingCondition");
    return c.get<ran_p_choice_presence_e_>();
  }
  ran_p_choice_comparison_e_& set_ran_p_choice_comparison();
  ran_p_choice_presence_e_&   set_ran_p_choice_presence();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// RANParameter-Value ::= CHOICE
struct ran_param_value_c {
  struct types_opts {
    enum options {
      value_boolean,
      value_int,
      value_real,
      value_bit_s,
      value_oct_s,
      value_printable_string,
      /*...*/ nulltype
    } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  ran_param_value_c() = default;
  ran_param_value_c(const ran_param_value_c& other);
  ran_param_value_c& operator=(const ran_param_value_c& other);
  ~ran_param_value_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  bool& value_boolean()
  {
    assert_choice_type(types::value_boolean, type_, "RANParameter-Value");
    return c.get<bool>();
  }
  int64_t& value_int()
  {
    assert_choice_type(types::value_int, type_, "RANParameter-Value");
    return c.get<int64_t>();
  }
  real_s& value_real()
  {
    assert_choice_type(types::value_real, type_, "RANParameter-Value");
    return c.get<real_s>();
  }
  dyn_bitstring& value_bit_s()
  {
    assert_choice_type(types::value_bit_s, type_, "RANParameter-Value");
    return c.get<dyn_bitstring>();
  }
  unbounded_octstring<true>& value_oct_s()
  {
    assert_choice_type(types::value_oct_s, type_, "RANParameter-Value");
    return c.get<unbounded_octstring<true>>();
  }
  printable_string<0, MAX_ASN_STRING_LENGTH, false, true>& value_printable_string()
  {
    assert_choice_type(types::value_printable_string, type_, "RANParameter-Value");
    return c.get<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>>();
  }
  const bool& value_boolean() const
  {
    assert_choice_type(types::value_boolean, type_, "RANParameter-Value");
    return c.get<bool>();
  }
  const int64_t& value_int() const
  {
    assert_choice_type(types::value_int, type_, "RANParameter-Value");
    return c.get<int64_t>();
  }
  const real_s& value_real() const
  {
    assert_choice_type(types::value_real, type_, "RANParameter-Value");
    return c.get<real_s>();
  }
  const dyn_bitstring& value_bit_s() const
  {
    assert_choice_type(types::value_bit_s, type_, "RANParameter-Value");
    return c.get<dyn_bitstring>();
  }
  const unbounded_octstring<true>& value_oct_s() const
  {
    assert_choice_type(types::value_oct_s, type_, "RANParameter-Value");
    return c.get<unbounded_octstring<true>>();
  }
  const printable_string<0, MAX_ASN_STRING_LENGTH, false, true>& value_printable_string() const
  {
    assert_choice_type(types::value_printable_string, type_, "RANParameter-Value");
    return c.get<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>>();
  }
  bool&                                                    set_value_boolean();
  int64_t&                                                 set_value_int();
  real_s&                                                  set_value_real();
  dyn_bitstring&                                           set_value_bit_s();
  unbounded_octstring<true>&                               set_value_oct_s();
  printable_string<0, MAX_ASN_STRING_LENGTH, false, true>& set_value_printable_string();

private:
  types type_;
  choice_buffer_t<bool,
                  dyn_bitstring,
                  printable_string<0, MAX_ASN_STRING_LENGTH, false, true>,
                  real_s,
                  unbounded_octstring<true>>
      c;

  void destroy_();
};

// UEID-GNB-CU-CP-E1AP-ID-Item ::= SEQUENCE
struct ue_id_gnb_cu_cp_e1ap_id_item_s {
  bool     ext                  = false;
  uint64_t gnb_cu_cp_ue_e1ap_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEID-GNB-CU-CP-F1AP-ID-Item ::= SEQUENCE
struct ue_id_gnb_cu_cp_f1ap_id_item_s {
  bool     ext               = false;
  uint64_t gnb_cu_ue_f1ap_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GUAMI ::= SEQUENCE
struct guami_s {
  bool                             ext = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<8, false, true>  amf_region_id;
  fixed_bitstring<10, false, true> amf_set_id;
  fixed_bitstring<6, false, true>  amf_pointer;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GUMMEI ::= SEQUENCE
struct gummei_s {
  bool                     ext = false;
  fixed_octstring<3, true> plmn_id;
  fixed_octstring<2, true> mme_group_id;
  fixed_octstring<1, true> mme_code;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalENB-ID ::= SEQUENCE
struct global_enb_id_s {
  bool                     ext = false;
  fixed_octstring<3, true> plmn_id;
  enb_id_c                 enb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalNGRANNodeID ::= CHOICE
struct global_ngran_node_id_c {
  struct types_opts {
    enum options { gnb, ng_enb, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  global_ngran_node_id_c() = default;
  global_ngran_node_id_c(const global_ngran_node_id_c& other);
  global_ngran_node_id_c& operator=(const global_ngran_node_id_c& other);
  ~global_ngran_node_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  global_gnb_id_s& gnb()
  {
    assert_choice_type(types::gnb, type_, "GlobalNGRANNodeID");
    return c.get<global_gnb_id_s>();
  }
  global_ng_enb_id_s& ng_enb()
  {
    assert_choice_type(types::ng_enb, type_, "GlobalNGRANNodeID");
    return c.get<global_ng_enb_id_s>();
  }
  const global_gnb_id_s& gnb() const
  {
    assert_choice_type(types::gnb, type_, "GlobalNGRANNodeID");
    return c.get<global_gnb_id_s>();
  }
  const global_ng_enb_id_s& ng_enb() const
  {
    assert_choice_type(types::ng_enb, type_, "GlobalNGRANNodeID");
    return c.get<global_ng_enb_id_s>();
  }
  global_gnb_id_s&    set_gnb();
  global_ng_enb_id_s& set_ng_enb();

private:
  types                                                type_;
  choice_buffer_t<global_gnb_id_s, global_ng_enb_id_s> c;

  void destroy_();
};

// RANParameter-Testing-Item-Choice-ElementFalse ::= SEQUENCE
struct ran_param_testing_item_choice_elem_false_s {
  bool                          ext                     = false;
  bool                          ran_param_value_present = false;
  bool                          lc_or_present           = false;
  ran_param_testing_condition_c ran_param_test_condition;
  ran_param_value_c             ran_param_value;
  lc_or_e                       lc_or;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANParameter-Testing-Item-Choice-ElementTrue ::= SEQUENCE
struct ran_param_testing_item_choice_elem_true_s {
  bool              ext = false;
  ran_param_value_c ran_param_value;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANParameter-Testing-Item-Choice-Structure ::= SEQUENCE
struct ran_param_testing_item_choice_structure_s {
  bool    ext                 = false;
  int64_t ran_param_structure = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEID-GNB-CU-CP-E1AP-ID-List ::= SEQUENCE (SIZE (1..65535)) OF UEID-GNB-CU-CP-E1AP-ID-Item
using ue_id_gnb_cu_cp_e1ap_id_list_l = dyn_array<ue_id_gnb_cu_cp_e1ap_id_item_s>;

// UEID-GNB-CU-F1AP-ID-List ::= SEQUENCE (SIZE (1..4)) OF UEID-GNB-CU-CP-F1AP-ID-Item
using ue_id_gnb_cu_f1ap_id_list_l = dyn_array<ue_id_gnb_cu_cp_f1ap_id_item_s>;

// RANParameter-Testing-Item ::= SEQUENCE
struct ran_param_testing_item_s {
  struct ran_param_type_c_ {
    struct types_opts {
      enum options { ran_p_choice_structure, ran_p_choice_elem_true, ran_p_choice_elem_false, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    ran_param_type_c_() = default;
    ran_param_type_c_(const ran_param_type_c_& other);
    ran_param_type_c_& operator=(const ran_param_type_c_& other);
    ~ran_param_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ran_param_testing_item_choice_structure_s& ran_p_choice_structure()
    {
      assert_choice_type(types::ran_p_choice_structure, type_, "ranParameter-Type");
      return c.get<ran_param_testing_item_choice_structure_s>();
    }
    ran_param_testing_item_choice_elem_true_s& ran_p_choice_elem_true()
    {
      assert_choice_type(types::ran_p_choice_elem_true, type_, "ranParameter-Type");
      return c.get<ran_param_testing_item_choice_elem_true_s>();
    }
    ran_param_testing_item_choice_elem_false_s& ran_p_choice_elem_false()
    {
      assert_choice_type(types::ran_p_choice_elem_false, type_, "ranParameter-Type");
      return c.get<ran_param_testing_item_choice_elem_false_s>();
    }
    const ran_param_testing_item_choice_structure_s& ran_p_choice_structure() const
    {
      assert_choice_type(types::ran_p_choice_structure, type_, "ranParameter-Type");
      return c.get<ran_param_testing_item_choice_structure_s>();
    }
    const ran_param_testing_item_choice_elem_true_s& ran_p_choice_elem_true() const
    {
      assert_choice_type(types::ran_p_choice_elem_true, type_, "ranParameter-Type");
      return c.get<ran_param_testing_item_choice_elem_true_s>();
    }
    const ran_param_testing_item_choice_elem_false_s& ran_p_choice_elem_false() const
    {
      assert_choice_type(types::ran_p_choice_elem_false, type_, "ranParameter-Type");
      return c.get<ran_param_testing_item_choice_elem_false_s>();
    }
    ran_param_testing_item_choice_structure_s&  set_ran_p_choice_structure();
    ran_param_testing_item_choice_elem_true_s&  set_ran_p_choice_elem_true();
    ran_param_testing_item_choice_elem_false_s& set_ran_p_choice_elem_false();

  private:
    types type_;
    choice_buffer_t<ran_param_testing_item_choice_elem_false_s,
                    ran_param_testing_item_choice_elem_true_s,
                    ran_param_testing_item_choice_structure_s>
        c;

    void destroy_();
  };

  // member variables
  bool              ext          = false;
  uint64_t          ran_param_id = 1;
  ran_param_type_c_ ran_param_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEID-EN-GNB ::= SEQUENCE
struct ue_id_en_gnb_s {
  bool                           ext                          = false;
  bool                           m_enb_ue_x2ap_id_ext_present = false;
  bool                           gnb_cu_ue_f1ap_id_present    = false;
  bool                           ran_ue_id_present            = false;
  uint16_t                       m_enb_ue_x2ap_id             = 0;
  uint16_t                       m_enb_ue_x2ap_id_ext         = 0;
  global_enb_id_s                global_enb_id;
  uint64_t                       gnb_cu_ue_f1ap_id = 0;
  ue_id_gnb_cu_cp_e1ap_id_list_l gnb_cu_cp_ue_e1ap_id_list;
  fixed_octstring<8, true>       ran_ue_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEID-ENB ::= SEQUENCE
struct ue_id_enb_s {
  bool            ext                          = false;
  bool            m_enb_ue_x2ap_id_present     = false;
  bool            m_enb_ue_x2ap_id_ext_present = false;
  bool            global_enb_id_present        = false;
  uint64_t        mme_ue_s1ap_id               = 0;
  gummei_s        gummei;
  uint16_t        m_enb_ue_x2ap_id     = 0;
  uint16_t        m_enb_ue_x2ap_id_ext = 0;
  global_enb_id_s global_enb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEID-GNB ::= SEQUENCE
struct ue_id_gnb_s {
  bool                           ext                          = false;
  bool                           ran_ue_id_present            = false;
  bool                           m_ng_ran_ue_xn_ap_id_present = false;
  bool                           global_gnb_id_present        = false;
  uint64_t                       amf_ue_ngap_id               = 0;
  guami_s                        guami;
  ue_id_gnb_cu_f1ap_id_list_l    gnb_cu_ue_f1ap_id_list;
  ue_id_gnb_cu_cp_e1ap_id_list_l gnb_cu_cp_ue_e1ap_id_list;
  fixed_octstring<8, true>       ran_ue_id;
  uint64_t                       m_ng_ran_ue_xn_ap_id = 0;
  global_gnb_id_s                global_gnb_id;
  // ...
  copy_ptr<global_ngran_node_id_c> global_ng_ran_node_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEID-GNB-CU-UP ::= SEQUENCE
struct ue_id_gnb_cu_up_s {
  bool                     ext                  = false;
  bool                     ran_ue_id_present    = false;
  uint64_t                 gnb_cu_cp_ue_e1ap_id = 0;
  fixed_octstring<8, true> ran_ue_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEID-GNB-DU ::= SEQUENCE
struct ue_id_gnb_du_s {
  bool                     ext               = false;
  bool                     ran_ue_id_present = false;
  uint64_t                 gnb_cu_ue_f1ap_id = 0;
  fixed_octstring<8, true> ran_ue_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEID-NG-ENB ::= SEQUENCE
struct ue_id_ng_enb_s {
  bool               ext                           = false;
  bool               ng_enb_cu_ue_w1_ap_id_present = false;
  bool               m_ng_ran_ue_xn_ap_id_present  = false;
  bool               global_ng_enb_id_present      = false;
  uint64_t           amf_ue_ngap_id                = 0;
  guami_s            guami;
  uint64_t           ng_enb_cu_ue_w1_ap_id = 0;
  uint64_t           m_ng_ran_ue_xn_ap_id  = 0;
  global_ng_enb_id_s global_ng_enb_id;
  // ...
  copy_ptr<global_ngran_node_id_c> global_ng_ran_node_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEID-NG-ENB-DU ::= SEQUENCE
struct ue_id_ng_enb_du_s {
  bool     ext                   = false;
  uint64_t ng_enb_cu_ue_w1_ap_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANParameter-Testing ::= SEQUENCE (SIZE (1..255)) OF RANParameter-Testing-Item
using ran_param_testing_l = dyn_array<ran_param_testing_item_s>;

// UEID ::= CHOICE
struct ue_id_c {
  struct types_opts {
    enum options {
      gnb_ue_id,
      gnb_du_ue_id,
      gnb_cu_up_ue_id,
      ng_enb_ue_id,
      ng_enb_du_ue_id,
      en_gnb_ue_id,
      enb_ue_id,
      // ...
      nulltype
    } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  ue_id_c() = default;
  ue_id_c(const ue_id_c& other);
  ue_id_c& operator=(const ue_id_c& other);
  ~ue_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  ue_id_gnb_s& gnb_ue_id()
  {
    assert_choice_type(types::gnb_ue_id, type_, "UEID");
    return c.get<ue_id_gnb_s>();
  }
  ue_id_gnb_du_s& gnb_du_ue_id()
  {
    assert_choice_type(types::gnb_du_ue_id, type_, "UEID");
    return c.get<ue_id_gnb_du_s>();
  }
  ue_id_gnb_cu_up_s& gnb_cu_up_ue_id()
  {
    assert_choice_type(types::gnb_cu_up_ue_id, type_, "UEID");
    return c.get<ue_id_gnb_cu_up_s>();
  }
  ue_id_ng_enb_s& ng_enb_ue_id()
  {
    assert_choice_type(types::ng_enb_ue_id, type_, "UEID");
    return c.get<ue_id_ng_enb_s>();
  }
  ue_id_ng_enb_du_s& ng_enb_du_ue_id()
  {
    assert_choice_type(types::ng_enb_du_ue_id, type_, "UEID");
    return c.get<ue_id_ng_enb_du_s>();
  }
  ue_id_en_gnb_s& en_gnb_ue_id()
  {
    assert_choice_type(types::en_gnb_ue_id, type_, "UEID");
    return c.get<ue_id_en_gnb_s>();
  }
  ue_id_enb_s& enb_ue_id()
  {
    assert_choice_type(types::enb_ue_id, type_, "UEID");
    return c.get<ue_id_enb_s>();
  }
  const ue_id_gnb_s& gnb_ue_id() const
  {
    assert_choice_type(types::gnb_ue_id, type_, "UEID");
    return c.get<ue_id_gnb_s>();
  }
  const ue_id_gnb_du_s& gnb_du_ue_id() const
  {
    assert_choice_type(types::gnb_du_ue_id, type_, "UEID");
    return c.get<ue_id_gnb_du_s>();
  }
  const ue_id_gnb_cu_up_s& gnb_cu_up_ue_id() const
  {
    assert_choice_type(types::gnb_cu_up_ue_id, type_, "UEID");
    return c.get<ue_id_gnb_cu_up_s>();
  }
  const ue_id_ng_enb_s& ng_enb_ue_id() const
  {
    assert_choice_type(types::ng_enb_ue_id, type_, "UEID");
    return c.get<ue_id_ng_enb_s>();
  }
  const ue_id_ng_enb_du_s& ng_enb_du_ue_id() const
  {
    assert_choice_type(types::ng_enb_du_ue_id, type_, "UEID");
    return c.get<ue_id_ng_enb_du_s>();
  }
  const ue_id_en_gnb_s& en_gnb_ue_id() const
  {
    assert_choice_type(types::en_gnb_ue_id, type_, "UEID");
    return c.get<ue_id_en_gnb_s>();
  }
  const ue_id_enb_s& enb_ue_id() const
  {
    assert_choice_type(types::enb_ue_id, type_, "UEID");
    return c.get<ue_id_enb_s>();
  }
  ue_id_gnb_s&       set_gnb_ue_id();
  ue_id_gnb_du_s&    set_gnb_du_ue_id();
  ue_id_gnb_cu_up_s& set_gnb_cu_up_ue_id();
  ue_id_ng_enb_s&    set_ng_enb_ue_id();
  ue_id_ng_enb_du_s& set_ng_enb_du_ue_id();
  ue_id_en_gnb_s&    set_en_gnb_ue_id();
  ue_id_enb_s&       set_enb_ue_id();

private:
  types type_;
  choice_buffer_t<ue_id_en_gnb_s,
                  ue_id_enb_s,
                  ue_id_gnb_cu_up_s,
                  ue_id_gnb_du_s,
                  ue_id_gnb_s,
                  ue_id_ng_enb_du_s,
                  ue_id_ng_enb_s>
      c;

  void destroy_();
};

// EventTrigger-UE-Info-Item-Choice-Group ::= SEQUENCE
struct event_trigger_ue_info_item_choice_group_s {
  bool                ext = false;
  ran_param_testing_l ran_param_testing;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventTrigger-UE-Info-Item-Choice-Individual ::= SEQUENCE
struct event_trigger_ue_info_item_choice_individual_s {
  bool                ext = false;
  ue_id_c             ue_id;
  ran_param_testing_l ran_param_testing;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Associated-UE-Info-Item ::= SEQUENCE
struct associated_ue_info_item_s {
  struct ue_type_c_ {
    struct types_opts {
      enum options { ue_type_choice_individual, ue_type_choice_group, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    ue_type_c_() = default;
    ue_type_c_(const ue_type_c_& other);
    ue_type_c_& operator=(const ue_type_c_& other);
    ~ue_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    event_trigger_ue_info_item_choice_individual_s& ue_type_choice_individual()
    {
      assert_choice_type(types::ue_type_choice_individual, type_, "ueType");
      return c.get<event_trigger_ue_info_item_choice_individual_s>();
    }
    event_trigger_ue_info_item_choice_group_s& ue_type_choice_group()
    {
      assert_choice_type(types::ue_type_choice_group, type_, "ueType");
      return c.get<event_trigger_ue_info_item_choice_group_s>();
    }
    const event_trigger_ue_info_item_choice_individual_s& ue_type_choice_individual() const
    {
      assert_choice_type(types::ue_type_choice_individual, type_, "ueType");
      return c.get<event_trigger_ue_info_item_choice_individual_s>();
    }
    const event_trigger_ue_info_item_choice_group_s& ue_type_choice_group() const
    {
      assert_choice_type(types::ue_type_choice_group, type_, "ueType");
      return c.get<event_trigger_ue_info_item_choice_group_s>();
    }
    event_trigger_ue_info_item_choice_individual_s& set_ue_type_choice_individual();
    event_trigger_ue_info_item_choice_group_s&      set_ue_type_choice_group();

  private:
    types                                                                                                      type_;
    choice_buffer_t<event_trigger_ue_info_item_choice_group_s, event_trigger_ue_info_item_choice_individual_s> c;

    void destroy_();
  };

  // member variables
  bool       ext           = false;
  bool       lc_or_present = false;
  uint32_t   ue_filt_id    = 1;
  ue_type_c_ ue_type;
  lc_or_e    lc_or;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Associated-UE-Info ::= SEQUENCE
struct associated_ue_info_s {
  using associated_ue_info_list_l_ = dyn_array<associated_ue_info_item_s>;

  // member variables
  bool                       ext = false;
  associated_ue_info_list_l_ associated_ue_info_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-CGI ::= SEQUENCE
struct eutra_cgi_s {
  bool                             ext = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<28, false, true> eutra_cell_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-CGI ::= SEQUENCE
struct nr_cgi_s {
  bool                             ext = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<36, false, true> nr_cell_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CGI ::= CHOICE
struct cgi_c {
  struct types_opts {
    enum options { nr_cgi, eutra_cgi, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  cgi_c() = default;
  cgi_c(const cgi_c& other);
  cgi_c& operator=(const cgi_c& other);
  ~cgi_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  nr_cgi_s& nr_cgi()
  {
    assert_choice_type(types::nr_cgi, type_, "CGI");
    return c.get<nr_cgi_s>();
  }
  eutra_cgi_s& eutra_cgi()
  {
    assert_choice_type(types::eutra_cgi, type_, "CGI");
    return c.get<eutra_cgi_s>();
  }
  const nr_cgi_s& nr_cgi() const
  {
    assert_choice_type(types::nr_cgi, type_, "CGI");
    return c.get<nr_cgi_s>();
  }
  const eutra_cgi_s& eutra_cgi() const
  {
    assert_choice_type(types::eutra_cgi, type_, "CGI");
    return c.get<eutra_cgi_s>();
  }
  nr_cgi_s&    set_nr_cgi();
  eutra_cgi_s& set_eutra_cgi();

private:
  types                                  type_;
  choice_buffer_t<eutra_cgi_s, nr_cgi_s> c;

  void destroy_();
};

// RANParameter-Definition-Choice-LIST-Item ::= SEQUENCE
struct ran_param_definition_choice_list_item_s {
  bool                                 ext          = false;
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANParameter-Definition-Choice-STRUCTURE-Item ::= SEQUENCE
struct ran_param_definition_choice_structure_item_s {
  bool                                 ext          = false;
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANParameter-Definition-Choice-LIST ::= SEQUENCE
struct ran_param_definition_choice_list_s {
  using ran_param_list_l_ = dyn_array<ran_param_definition_choice_list_item_s>;

  // member variables
  bool              ext = false;
  ran_param_list_l_ ran_param_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANParameter-Definition-Choice-STRUCTURE ::= SEQUENCE
struct ran_param_definition_choice_structure_s {
  using ran_param_structure_l_ = dyn_array<ran_param_definition_choice_structure_item_s>;

  // member variables
  bool                   ext = false;
  ran_param_structure_l_ ran_param_structure;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANParameter-Definition-Choice ::= CHOICE
struct ran_param_definition_choice_c {
  struct types_opts {
    enum options { choice_list, choice_structure, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  ran_param_definition_choice_c() = default;
  ran_param_definition_choice_c(const ran_param_definition_choice_c& other);
  ran_param_definition_choice_c& operator=(const ran_param_definition_choice_c& other);
  ~ran_param_definition_choice_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  ran_param_definition_choice_list_s& choice_list()
  {
    assert_choice_type(types::choice_list, type_, "RANParameter-Definition-Choice");
    return c.get<ran_param_definition_choice_list_s>();
  }
  ran_param_definition_choice_structure_s& choice_structure()
  {
    assert_choice_type(types::choice_structure, type_, "RANParameter-Definition-Choice");
    return c.get<ran_param_definition_choice_structure_s>();
  }
  const ran_param_definition_choice_list_s& choice_list() const
  {
    assert_choice_type(types::choice_list, type_, "RANParameter-Definition-Choice");
    return c.get<ran_param_definition_choice_list_s>();
  }
  const ran_param_definition_choice_structure_s& choice_structure() const
  {
    assert_choice_type(types::choice_structure, type_, "RANParameter-Definition-Choice");
    return c.get<ran_param_definition_choice_structure_s>();
  }
  ran_param_definition_choice_list_s&      set_choice_list();
  ran_param_definition_choice_structure_s& set_choice_structure();

private:
  types                                                                                        type_;
  choice_buffer_t<ran_param_definition_choice_list_s, ran_param_definition_choice_structure_s> c;

  void destroy_();
};

// RANParameter-Definition ::= SEQUENCE
struct ran_param_definition_s {
  bool                          ext = false;
  ran_param_definition_choice_c ran_param_definition_choice;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CallProcessBreakpoint-RANParameter-Item ::= SEQUENCE
struct call_process_breakpoint_ran_param_item_s {
  bool                                 ext          = false;
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellIdentification-RANParameter-Item ::= SEQUENCE
struct cell_identif_ran_param_item_s {
  bool                                 ext          = false;
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ListOfAdditionalSupportedFormats-UEGroupControl ::= SEQUENCE (SIZE (0..63)) OF
// AdditionalSupportedFormat-UEGroupControl
using list_of_add_supported_formats_ue_group_ctrl_l = dyn_array<add_supported_format_ue_group_ctrl_s>;

// ControlAction-RANParameter-Item ::= SEQUENCE
struct ctrl_action_ran_param_item_s {
  bool                                 ext          = false;
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s>                        ran_param_definition;
  copy_ptr<list_of_add_supported_formats_ue_group_ctrl_l> list_of_add_supported_formats_ue_group_ctrl;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ControlOutcome-RANParameter-Item ::= SEQUENCE
struct ctrl_outcome_ran_param_item_s {
  bool                                 ext          = false;
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CoreCPID ::= CHOICE
struct core_cp_id_c {
  struct types_opts {
    enum options { five_gc, epc, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  core_cp_id_c() = default;
  core_cp_id_c(const core_cp_id_c& other);
  core_cp_id_c& operator=(const core_cp_id_c& other);
  ~core_cp_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  guami_s& five_gc()
  {
    assert_choice_type(types::five_gc, type_, "CoreCPID");
    return c.get<guami_s>();
  }
  gummei_s& epc()
  {
    assert_choice_type(types::epc, type_, "CoreCPID");
    return c.get<gummei_s>();
  }
  const guami_s& five_gc() const
  {
    assert_choice_type(types::five_gc, type_, "CoreCPID");
    return c.get<guami_s>();
  }
  const gummei_s& epc() const
  {
    assert_choice_type(types::epc, type_, "CoreCPID");
    return c.get<gummei_s>();
  }
  guami_s&  set_five_gc();
  gummei_s& set_epc();

private:
  types                              type_;
  choice_buffer_t<guami_s, gummei_s> c;

  void destroy_();
};

// RANParameter-ValueType-Choice-ElementFalse ::= SEQUENCE
struct ran_param_value_type_choice_elem_false_s {
  bool              ext                     = false;
  bool              ran_param_value_present = false;
  ran_param_value_c ran_param_value;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANParameter-ValueType-Choice-ElementTrue ::= SEQUENCE
struct ran_param_value_type_choice_elem_true_s {
  bool              ext = false;
  ran_param_value_c ran_param_value;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANParameter-ValueType ::= CHOICE
struct ran_param_value_type_c {
  struct types_opts {
    enum options { ran_p_choice_elem_true, ran_p_choice_elem_false, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  ran_param_value_type_c() = default;
  ran_param_value_type_c(const ran_param_value_type_c& other);
  ran_param_value_type_c& operator=(const ran_param_value_type_c& other);
  ~ran_param_value_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  ran_param_value_type_choice_elem_true_s& ran_p_choice_elem_true()
  {
    assert_choice_type(types::ran_p_choice_elem_true, type_, "RANParameter-ValueType");
    return c.get<ran_param_value_type_choice_elem_true_s>();
  }
  ran_param_value_type_choice_elem_false_s& ran_p_choice_elem_false()
  {
    assert_choice_type(types::ran_p_choice_elem_false, type_, "RANParameter-ValueType");
    return c.get<ran_param_value_type_choice_elem_false_s>();
  }
  const ran_param_value_type_choice_elem_true_s& ran_p_choice_elem_true() const
  {
    assert_choice_type(types::ran_p_choice_elem_true, type_, "RANParameter-ValueType");
    return c.get<ran_param_value_type_choice_elem_true_s>();
  }
  const ran_param_value_type_choice_elem_false_s& ran_p_choice_elem_false() const
  {
    assert_choice_type(types::ran_p_choice_elem_false, type_, "RANParameter-ValueType");
    return c.get<ran_param_value_type_choice_elem_false_s>();
  }
  ran_param_value_type_choice_elem_true_s&  set_ran_p_choice_elem_true();
  ran_param_value_type_choice_elem_false_s& set_ran_p_choice_elem_false();

private:
  types                                                                                              type_;
  choice_buffer_t<ran_param_value_type_choice_elem_false_s, ran_param_value_type_choice_elem_true_s> c;

  void destroy_();
};

// E2SM-RC-ActionDefinition-Format4-RANP-Item ::= SEQUENCE
struct e2_sm_rc_action_definition_format4_ranp_item_s {
  bool     ext          = false;
  uint64_t ran_param_id = 1;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RIC-PolicyAction-RANParameter-Item ::= SEQUENCE
struct ric_policy_action_ran_param_item_s {
  bool                   ext          = false;
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ActionDefinition-Format4-Indication-Item ::= SEQUENCE
struct e2_sm_rc_action_definition_format4_ind_item_s {
  using ran_p_insert_ind_list_l_ = dyn_array<e2_sm_rc_action_definition_format4_ranp_item_s>;

  // member variables
  bool                     ext               = false;
  uint32_t                 ric_insert_ind_id = 1;
  ran_p_insert_ind_list_l_ ran_p_insert_ind_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RIC-PolicyAction ::= SEQUENCE
struct ric_policy_action_s {
  using ran_params_list_l_ = dyn_array<ric_policy_action_ran_param_item_s>;
  struct ric_policy_decision_opts {
    enum options { accept, reject, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using ric_policy_decision_e_ = enumerated<ric_policy_decision_opts, true>;

  // member variables
  bool               ext                  = false;
  uint32_t           ric_policy_action_id = 1;
  ran_params_list_l_ ran_params_list;
  // ...
  bool                   ric_policy_decision_present = false;
  ric_policy_decision_e_ ric_policy_decision;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ActionDefinition-Format1-Item ::= SEQUENCE
struct e2_sm_rc_action_definition_format1_item_s {
  bool     ext          = false;
  uint64_t ran_param_id = 1;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ActionDefinition-Format2-Item ::= SEQUENCE
struct e2_sm_rc_action_definition_format2_item_s {
  bool                ext = false;
  ric_policy_action_s ric_policy_action;
  ran_param_testing_l ric_policy_condition_definition;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ActionDefinition-Format3-Item ::= SEQUENCE
struct e2_sm_rc_action_definition_format3_item_s {
  bool     ext          = false;
  uint64_t ran_param_id = 1;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ActionDefinition-Format4-Style-Item ::= SEQUENCE
struct e2_sm_rc_action_definition_format4_style_item_s {
  using ric_insert_ind_list_l_ = dyn_array<e2_sm_rc_action_definition_format4_ind_item_s>;

  // member variables
  bool                   ext                         = false;
  int64_t                requested_insert_style_type = 0;
  ric_insert_ind_list_l_ ric_insert_ind_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ActionDefinition-Format1 ::= SEQUENCE
struct e2_sm_rc_action_definition_format1_s {
  using ran_p_to_be_reported_list_l_ = dyn_array<e2_sm_rc_action_definition_format1_item_s>;

  // member variables
  bool                         ext = false;
  ran_p_to_be_reported_list_l_ ran_p_to_be_reported_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ActionDefinition-Format2 ::= SEQUENCE
struct e2_sm_rc_action_definition_format2_s {
  using ric_policy_conditions_list_l_ = dyn_array<e2_sm_rc_action_definition_format2_item_s>;

  // member variables
  bool                          ext = false;
  ric_policy_conditions_list_l_ ric_policy_conditions_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ActionDefinition-Format3 ::= SEQUENCE
struct e2_sm_rc_action_definition_format3_s {
  using ran_p_insert_ind_list_l_ = dyn_array<e2_sm_rc_action_definition_format3_item_s>;

  // member variables
  bool                     ext               = false;
  bool                     ue_id_present     = false;
  uint32_t                 ric_insert_ind_id = 1;
  ran_p_insert_ind_list_l_ ran_p_insert_ind_list;
  ue_id_c                  ue_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ActionDefinition-Format4 ::= SEQUENCE
struct e2_sm_rc_action_definition_format4_s {
  using ric_insert_style_list_l_ = dyn_array<e2_sm_rc_action_definition_format4_style_item_s>;

  // member variables
  bool                     ext           = false;
  bool                     ue_id_present = false;
  ric_insert_style_list_l_ ric_insert_style_list;
  ue_id_c                  ue_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ActionDefinition ::= SEQUENCE
struct e2_sm_rc_action_definition_s {
  struct ric_action_definition_formats_c_ {
    struct types_opts {
      enum options {
        action_definition_format1,
        action_definition_format2,
        action_definition_format3,
        // ...
        action_definition_format4,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true, 1>;

    // choice methods
    ric_action_definition_formats_c_() = default;
    ric_action_definition_formats_c_(const ric_action_definition_formats_c_& other);
    ric_action_definition_formats_c_& operator=(const ric_action_definition_formats_c_& other);
    ~ric_action_definition_formats_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2_sm_rc_action_definition_format1_s& action_definition_format1()
    {
      assert_choice_type(types::action_definition_format1, type_, "ric-actionDefinition-formats");
      return c.get<e2_sm_rc_action_definition_format1_s>();
    }
    e2_sm_rc_action_definition_format2_s& action_definition_format2()
    {
      assert_choice_type(types::action_definition_format2, type_, "ric-actionDefinition-formats");
      return c.get<e2_sm_rc_action_definition_format2_s>();
    }
    e2_sm_rc_action_definition_format3_s& action_definition_format3()
    {
      assert_choice_type(types::action_definition_format3, type_, "ric-actionDefinition-formats");
      return c.get<e2_sm_rc_action_definition_format3_s>();
    }
    e2_sm_rc_action_definition_format4_s& action_definition_format4()
    {
      assert_choice_type(types::action_definition_format4, type_, "ric-actionDefinition-formats");
      return c.get<e2_sm_rc_action_definition_format4_s>();
    }
    const e2_sm_rc_action_definition_format1_s& action_definition_format1() const
    {
      assert_choice_type(types::action_definition_format1, type_, "ric-actionDefinition-formats");
      return c.get<e2_sm_rc_action_definition_format1_s>();
    }
    const e2_sm_rc_action_definition_format2_s& action_definition_format2() const
    {
      assert_choice_type(types::action_definition_format2, type_, "ric-actionDefinition-formats");
      return c.get<e2_sm_rc_action_definition_format2_s>();
    }
    const e2_sm_rc_action_definition_format3_s& action_definition_format3() const
    {
      assert_choice_type(types::action_definition_format3, type_, "ric-actionDefinition-formats");
      return c.get<e2_sm_rc_action_definition_format3_s>();
    }
    const e2_sm_rc_action_definition_format4_s& action_definition_format4() const
    {
      assert_choice_type(types::action_definition_format4, type_, "ric-actionDefinition-formats");
      return c.get<e2_sm_rc_action_definition_format4_s>();
    }
    e2_sm_rc_action_definition_format1_s& set_action_definition_format1();
    e2_sm_rc_action_definition_format2_s& set_action_definition_format2();
    e2_sm_rc_action_definition_format3_s& set_action_definition_format3();
    e2_sm_rc_action_definition_format4_s& set_action_definition_format4();

  private:
    types type_;
    choice_buffer_t<e2_sm_rc_action_definition_format1_s,
                    e2_sm_rc_action_definition_format2_s,
                    e2_sm_rc_action_definition_format3_s,
                    e2_sm_rc_action_definition_format4_s>
        c;

    void destroy_();
  };

  // member variables
  bool                             ext            = false;
  int64_t                          ric_style_type = 0;
  ric_action_definition_formats_c_ ric_action_definition_formats;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-CallProcessID-Format1 ::= SEQUENCE
struct e2_sm_rc_call_process_id_format1_s {
  bool     ext                 = false;
  uint64_t ric_call_process_id = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-CallProcessID ::= SEQUENCE
struct e2_sm_rc_call_process_id_s {
  struct ric_call_process_id_formats_c_ {
    struct types_opts {
      enum options { call_process_id_format1, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    types       type() const { return types::call_process_id_format1; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2_sm_rc_call_process_id_format1_s&       call_process_id_format1() { return c; }
    const e2_sm_rc_call_process_id_format1_s& call_process_id_format1() const { return c; }

  private:
    e2_sm_rc_call_process_id_format1_s c;
  };

  // member variables
  bool                           ext = false;
  ric_call_process_id_formats_c_ ric_call_process_id_formats;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEGroupDefinitionIdentifier-Item ::= SEQUENCE
struct ue_group_definition_id_item_s {
  bool                   ext           = false;
  bool                   lc_or_present = false;
  uint64_t               ran_param_id  = 1;
  ran_param_value_type_c ran_param_value_type;
  lc_or_e                lc_or;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-Group-Definition ::= SEQUENCE
struct ue_group_definition_s {
  using ue_group_definition_id_list_l_ = dyn_array<ue_group_definition_id_item_s>;

  // member variables
  bool                           ext = false;
  ue_group_definition_id_list_l_ ue_group_definition_id_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlHeader-Format1 ::= SEQUENCE
struct e2_sm_rc_ctrl_hdr_format1_s {
  struct ric_ctrl_decision_opts {
    enum options { accept, reject, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using ric_ctrl_decision_e_ = enumerated<ric_ctrl_decision_opts, true>;

  // member variables
  bool                 ext                       = false;
  bool                 ric_ctrl_decision_present = false;
  ue_id_c              ue_id;
  int64_t              ric_style_type     = 0;
  uint32_t             ric_ctrl_action_id = 1;
  ric_ctrl_decision_e_ ric_ctrl_decision;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlHeader-Format2 ::= SEQUENCE
struct e2_sm_rc_ctrl_hdr_format2_s {
  struct ric_ctrl_decision_opts {
    enum options { accept, reject, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using ric_ctrl_decision_e_ = enumerated<ric_ctrl_decision_opts, true>;

  // member variables
  bool                 ext                       = false;
  bool                 ue_id_present             = false;
  bool                 ric_ctrl_decision_present = false;
  ue_id_c              ue_id;
  ric_ctrl_decision_e_ ric_ctrl_decision;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlHeader-Format3 ::= SEQUENCE
struct e2_sm_rc_ctrl_hdr_format3_s {
  bool                  ext         = false;
  uint32_t              ue_group_id = 1;
  ue_group_definition_s ue_group_definition;
  int64_t               ric_style_type     = 0;
  uint32_t              ric_ctrl_action_id = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlHeader ::= SEQUENCE
struct e2_sm_rc_ctrl_hdr_s {
  struct ric_ctrl_hdr_formats_c_ {
    struct types_opts {
      enum options { ctrl_hdr_format1, /*...*/ ctrl_hdr_format2, ctrl_hdr_format3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true, 2>;

    // choice methods
    ric_ctrl_hdr_formats_c_() = default;
    ric_ctrl_hdr_formats_c_(const ric_ctrl_hdr_formats_c_& other);
    ric_ctrl_hdr_formats_c_& operator=(const ric_ctrl_hdr_formats_c_& other);
    ~ric_ctrl_hdr_formats_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2_sm_rc_ctrl_hdr_format1_s& ctrl_hdr_format1()
    {
      assert_choice_type(types::ctrl_hdr_format1, type_, "ric-controlHeader-formats");
      return c.get<e2_sm_rc_ctrl_hdr_format1_s>();
    }
    e2_sm_rc_ctrl_hdr_format2_s& ctrl_hdr_format2()
    {
      assert_choice_type(types::ctrl_hdr_format2, type_, "ric-controlHeader-formats");
      return c.get<e2_sm_rc_ctrl_hdr_format2_s>();
    }
    e2_sm_rc_ctrl_hdr_format3_s& ctrl_hdr_format3()
    {
      assert_choice_type(types::ctrl_hdr_format3, type_, "ric-controlHeader-formats");
      return c.get<e2_sm_rc_ctrl_hdr_format3_s>();
    }
    const e2_sm_rc_ctrl_hdr_format1_s& ctrl_hdr_format1() const
    {
      assert_choice_type(types::ctrl_hdr_format1, type_, "ric-controlHeader-formats");
      return c.get<e2_sm_rc_ctrl_hdr_format1_s>();
    }
    const e2_sm_rc_ctrl_hdr_format2_s& ctrl_hdr_format2() const
    {
      assert_choice_type(types::ctrl_hdr_format2, type_, "ric-controlHeader-formats");
      return c.get<e2_sm_rc_ctrl_hdr_format2_s>();
    }
    const e2_sm_rc_ctrl_hdr_format3_s& ctrl_hdr_format3() const
    {
      assert_choice_type(types::ctrl_hdr_format3, type_, "ric-controlHeader-formats");
      return c.get<e2_sm_rc_ctrl_hdr_format3_s>();
    }
    e2_sm_rc_ctrl_hdr_format1_s& set_ctrl_hdr_format1();
    e2_sm_rc_ctrl_hdr_format2_s& set_ctrl_hdr_format2();
    e2_sm_rc_ctrl_hdr_format3_s& set_ctrl_hdr_format3();

  private:
    types                                                                                                  type_;
    choice_buffer_t<e2_sm_rc_ctrl_hdr_format1_s, e2_sm_rc_ctrl_hdr_format2_s, e2_sm_rc_ctrl_hdr_format3_s> c;

    void destroy_();
  };

  // member variables
  bool                    ext = false;
  ric_ctrl_hdr_formats_c_ ric_ctrl_hdr_formats;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlMessage-Format1-Item ::= SEQUENCE
struct e2_sm_rc_ctrl_msg_format1_item_s {
  bool                   ext          = false;
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlMessage-Format1 ::= SEQUENCE
struct e2_sm_rc_ctrl_msg_format1_s {
  using ran_p_list_l_ = dyn_array<e2_sm_rc_ctrl_msg_format1_item_s>;

  // member variables
  bool          ext = false;
  ran_p_list_l_ ran_p_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlMessage-Format2-ControlAction-Item ::= SEQUENCE
struct e2_sm_rc_ctrl_msg_format2_ctrl_action_item_s {
  bool                        ext                = false;
  uint32_t                    ric_ctrl_action_id = 1;
  e2_sm_rc_ctrl_msg_format1_s ran_p_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EntitySpecific-ranP-ControlParameters ::= SEQUENCE
struct entity_specific_ran_p_ctrl_params_s {
  bool                   ext          = false;
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlMessage-Format2-Style-Item ::= SEQUENCE
struct e2_sm_rc_ctrl_msg_format2_style_item_s {
  using ric_ctrl_action_list_l_ = dyn_array<e2_sm_rc_ctrl_msg_format2_ctrl_action_item_s>;

  // member variables
  bool                    ext                       = false;
  int64_t                 indicated_ctrl_style_type = 0;
  ric_ctrl_action_list_l_ ric_ctrl_action_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-EntityFilter ::= SEQUENCE
struct e2_sm_rc_entity_filt_s {
  using entity_specific_ctrl_ran_p_list_l_ = dyn_array<entity_specific_ran_p_ctrl_params_s>;

  // member variables
  bool                               ext            = false;
  uint16_t                           entity_filt_id = 1;
  ran_param_testing_l                entity_filt_definition;
  entity_specific_ctrl_ran_p_list_l_ entity_specific_ctrl_ran_p_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EntityAgnostic-ranP-ControlParameters ::= SEQUENCE
struct entity_agnostic_ran_p_ctrl_params_s {
  bool                   ext          = false;
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlMessage-Format2 ::= SEQUENCE
struct e2_sm_rc_ctrl_msg_format2_s {
  using ric_ctrl_style_list_l_ = dyn_array<e2_sm_rc_ctrl_msg_format2_style_item_s>;

  // member variables
  bool                   ext = false;
  ric_ctrl_style_list_l_ ric_ctrl_style_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlMessage-Format3 ::= SEQUENCE
struct e2_sm_rc_ctrl_msg_format3_s {
  using list_of_entity_filts_l_            = dyn_array<e2_sm_rc_entity_filt_s>;
  using entity_agnostic_ctrl_ran_p_list_l_ = dyn_array<entity_agnostic_ran_p_ctrl_params_s>;

  // member variables
  bool                               ext                                     = false;
  bool                               list_of_entity_filts_present            = false;
  bool                               entity_agnostic_ctrl_ran_p_list_present = false;
  list_of_entity_filts_l_            list_of_entity_filts;
  entity_agnostic_ctrl_ran_p_list_l_ entity_agnostic_ctrl_ran_p_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlMessage ::= SEQUENCE
struct e2_sm_rc_ctrl_msg_s {
  struct ric_ctrl_msg_formats_c_ {
    struct types_opts {
      enum options { ctrl_msg_format1, /*...*/ ctrl_msg_format2, ctrl_msg_format3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true, 2>;

    // choice methods
    ric_ctrl_msg_formats_c_() = default;
    ric_ctrl_msg_formats_c_(const ric_ctrl_msg_formats_c_& other);
    ric_ctrl_msg_formats_c_& operator=(const ric_ctrl_msg_formats_c_& other);
    ~ric_ctrl_msg_formats_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2_sm_rc_ctrl_msg_format1_s& ctrl_msg_format1()
    {
      assert_choice_type(types::ctrl_msg_format1, type_, "ric-controlMessage-formats");
      return c.get<e2_sm_rc_ctrl_msg_format1_s>();
    }
    e2_sm_rc_ctrl_msg_format2_s& ctrl_msg_format2()
    {
      assert_choice_type(types::ctrl_msg_format2, type_, "ric-controlMessage-formats");
      return c.get<e2_sm_rc_ctrl_msg_format2_s>();
    }
    e2_sm_rc_ctrl_msg_format3_s& ctrl_msg_format3()
    {
      assert_choice_type(types::ctrl_msg_format3, type_, "ric-controlMessage-formats");
      return c.get<e2_sm_rc_ctrl_msg_format3_s>();
    }
    const e2_sm_rc_ctrl_msg_format1_s& ctrl_msg_format1() const
    {
      assert_choice_type(types::ctrl_msg_format1, type_, "ric-controlMessage-formats");
      return c.get<e2_sm_rc_ctrl_msg_format1_s>();
    }
    const e2_sm_rc_ctrl_msg_format2_s& ctrl_msg_format2() const
    {
      assert_choice_type(types::ctrl_msg_format2, type_, "ric-controlMessage-formats");
      return c.get<e2_sm_rc_ctrl_msg_format2_s>();
    }
    const e2_sm_rc_ctrl_msg_format3_s& ctrl_msg_format3() const
    {
      assert_choice_type(types::ctrl_msg_format3, type_, "ric-controlMessage-formats");
      return c.get<e2_sm_rc_ctrl_msg_format3_s>();
    }
    e2_sm_rc_ctrl_msg_format1_s& set_ctrl_msg_format1();
    e2_sm_rc_ctrl_msg_format2_s& set_ctrl_msg_format2();
    e2_sm_rc_ctrl_msg_format3_s& set_ctrl_msg_format3();

  private:
    types                                                                                                  type_;
    choice_buffer_t<e2_sm_rc_ctrl_msg_format1_s, e2_sm_rc_ctrl_msg_format2_s, e2_sm_rc_ctrl_msg_format3_s> c;

    void destroy_();
  };

  // member variables
  bool                    ext = false;
  ric_ctrl_msg_formats_c_ ric_ctrl_msg_formats;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlOutcome-Format2-RANP-Item ::= SEQUENCE
struct e2_sm_rc_ctrl_outcome_format2_ranp_item_s {
  bool              ext          = false;
  uint64_t          ran_param_id = 1;
  ran_param_value_c ran_param_value;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlOutcome-Format2-ControlOutcome-Item ::= SEQUENCE
struct e2_sm_rc_ctrl_outcome_format2_ctrl_outcome_item_s {
  using ran_p_list_l_ = dyn_array<e2_sm_rc_ctrl_outcome_format2_ranp_item_s>;

  // member variables
  bool          ext                = false;
  uint32_t      ric_ctrl_action_id = 1;
  ran_p_list_l_ ran_p_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlOutcome-Format1-Item ::= SEQUENCE
struct e2_sm_rc_ctrl_outcome_format1_item_s {
  bool              ext          = false;
  uint64_t          ran_param_id = 1;
  ran_param_value_c ran_param_value;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlOutcome-Format2-Style-Item ::= SEQUENCE
struct e2_sm_rc_ctrl_outcome_format2_style_item_s {
  using ric_ctrl_outcome_list_l_ = dyn_array<e2_sm_rc_ctrl_outcome_format2_ctrl_outcome_item_s>;

  // member variables
  bool                     ext                       = false;
  int64_t                  indicated_ctrl_style_type = 0;
  ric_ctrl_outcome_list_l_ ric_ctrl_outcome_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlOutcome-Format3-Item ::= SEQUENCE
struct e2_sm_rc_ctrl_outcome_format3_item_s {
  bool                   ext          = false;
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlOutcome-Format1 ::= SEQUENCE
struct e2_sm_rc_ctrl_outcome_format1_s {
  using ran_p_list_l_ = dyn_array<e2_sm_rc_ctrl_outcome_format1_item_s>;

  // member variables
  bool          ext = false;
  ran_p_list_l_ ran_p_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlOutcome-Format2 ::= SEQUENCE
struct e2_sm_rc_ctrl_outcome_format2_s {
  using ric_ctrl_style_list_l_ = dyn_array<e2_sm_rc_ctrl_outcome_format2_style_item_s>;

  // member variables
  bool                   ext = false;
  ric_ctrl_style_list_l_ ric_ctrl_style_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlOutcome-Format3 ::= SEQUENCE
struct e2_sm_rc_ctrl_outcome_format3_s {
  using ran_p_list_l_ = dyn_array<e2_sm_rc_ctrl_outcome_format3_item_s>;

  // member variables
  bool          ext = false;
  ran_p_list_l_ ran_p_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-ControlOutcome ::= SEQUENCE
struct e2_sm_rc_ctrl_outcome_s {
  struct ric_ctrl_outcome_formats_c_ {
    struct types_opts {
      enum options { ctrl_outcome_format1, /*...*/ ctrl_outcome_format2, ctrl_outcome_format3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true, 2>;

    // choice methods
    ric_ctrl_outcome_formats_c_() = default;
    ric_ctrl_outcome_formats_c_(const ric_ctrl_outcome_formats_c_& other);
    ric_ctrl_outcome_formats_c_& operator=(const ric_ctrl_outcome_formats_c_& other);
    ~ric_ctrl_outcome_formats_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2_sm_rc_ctrl_outcome_format1_s& ctrl_outcome_format1()
    {
      assert_choice_type(types::ctrl_outcome_format1, type_, "ric-controlOutcome-formats");
      return c.get<e2_sm_rc_ctrl_outcome_format1_s>();
    }
    e2_sm_rc_ctrl_outcome_format2_s& ctrl_outcome_format2()
    {
      assert_choice_type(types::ctrl_outcome_format2, type_, "ric-controlOutcome-formats");
      return c.get<e2_sm_rc_ctrl_outcome_format2_s>();
    }
    e2_sm_rc_ctrl_outcome_format3_s& ctrl_outcome_format3()
    {
      assert_choice_type(types::ctrl_outcome_format3, type_, "ric-controlOutcome-formats");
      return c.get<e2_sm_rc_ctrl_outcome_format3_s>();
    }
    const e2_sm_rc_ctrl_outcome_format1_s& ctrl_outcome_format1() const
    {
      assert_choice_type(types::ctrl_outcome_format1, type_, "ric-controlOutcome-formats");
      return c.get<e2_sm_rc_ctrl_outcome_format1_s>();
    }
    const e2_sm_rc_ctrl_outcome_format2_s& ctrl_outcome_format2() const
    {
      assert_choice_type(types::ctrl_outcome_format2, type_, "ric-controlOutcome-formats");
      return c.get<e2_sm_rc_ctrl_outcome_format2_s>();
    }
    const e2_sm_rc_ctrl_outcome_format3_s& ctrl_outcome_format3() const
    {
      assert_choice_type(types::ctrl_outcome_format3, type_, "ric-controlOutcome-formats");
      return c.get<e2_sm_rc_ctrl_outcome_format3_s>();
    }
    e2_sm_rc_ctrl_outcome_format1_s& set_ctrl_outcome_format1();
    e2_sm_rc_ctrl_outcome_format2_s& set_ctrl_outcome_format2();
    e2_sm_rc_ctrl_outcome_format3_s& set_ctrl_outcome_format3();

  private:
    types type_;
    choice_buffer_t<e2_sm_rc_ctrl_outcome_format1_s, e2_sm_rc_ctrl_outcome_format2_s, e2_sm_rc_ctrl_outcome_format3_s>
        c;

    void destroy_();
  };

  // member variables
  bool                        ext = false;
  ric_ctrl_outcome_formats_c_ ric_ctrl_outcome_formats;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EN-GNB-ID ::= CHOICE
struct en_gnb_id_c {
  struct types_opts {
    enum options { en_gnb_id, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  types       type() const { return types::en_gnb_id; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  bounded_bitstring<22, 32, false, true>&       en_gnb_id() { return c; }
  const bounded_bitstring<22, 32, false, true>& en_gnb_id() const { return c; }

private:
  bounded_bitstring<22, 32, false, true> c;
};

// GlobalenGNB-ID ::= SEQUENCE
struct globalen_gnb_id_s {
  bool                     ext = false;
  fixed_octstring<3, true> plmn_id;
  en_gnb_id_c              en_gnb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterfaceID-E1 ::= SEQUENCE
struct interface_id_e1_s {
  bool            ext = false;
  global_gnb_id_s global_gnb_id;
  uint64_t        gnb_cu_up_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterfaceID-F1 ::= SEQUENCE
struct interface_id_f1_s {
  bool            ext = false;
  global_gnb_id_s global_gnb_id;
  uint64_t        gnb_du_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterfaceID-NG ::= SEQUENCE
struct interface_id_ng_s {
  bool    ext = false;
  guami_s guami;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterfaceID-S1 ::= SEQUENCE
struct interface_id_s1_s {
  bool     ext = false;
  gummei_s gummei;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterfaceID-W1 ::= SEQUENCE
struct interface_id_w1_s {
  bool               ext = false;
  global_ng_enb_id_s global_ng_enb_id;
  uint64_t           ng_enb_du_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterfaceID-X2 ::= SEQUENCE
struct interface_id_x2_s {
  struct node_type_c_ {
    struct types_opts {
      enum options { global_enb_id, global_en_gnb_id, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    node_type_c_() = default;
    node_type_c_(const node_type_c_& other);
    node_type_c_& operator=(const node_type_c_& other);
    ~node_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    global_enb_id_s& global_enb_id()
    {
      assert_choice_type(types::global_enb_id, type_, "nodeType");
      return c.get<global_enb_id_s>();
    }
    globalen_gnb_id_s& global_en_gnb_id()
    {
      assert_choice_type(types::global_en_gnb_id, type_, "nodeType");
      return c.get<globalen_gnb_id_s>();
    }
    const global_enb_id_s& global_enb_id() const
    {
      assert_choice_type(types::global_enb_id, type_, "nodeType");
      return c.get<global_enb_id_s>();
    }
    const globalen_gnb_id_s& global_en_gnb_id() const
    {
      assert_choice_type(types::global_en_gnb_id, type_, "nodeType");
      return c.get<globalen_gnb_id_s>();
    }
    global_enb_id_s&   set_global_enb_id();
    globalen_gnb_id_s& set_global_en_gnb_id();

  private:
    types                                               type_;
    choice_buffer_t<global_enb_id_s, globalen_gnb_id_s> c;

    void destroy_();
  };

  // member variables
  bool         ext = false;
  node_type_c_ node_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterfaceID-Xn ::= SEQUENCE
struct interface_id_xn_s {
  bool                   ext = false;
  global_ngran_node_id_c global_ng_ran_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRC-State ::= ENUMERATED
struct rrc_state_opts {
  enum options { rrc_connected, rrc_inactive, rrc_idle, any, /*...*/ nulltype } value;

  const char* to_string() const;
};
using rrc_state_e = enumerated<rrc_state_opts, true>;

// RRCclass-LTE ::= ENUMERATED
struct rrc_class_lte_opts {
  enum options {
    bcch_bch,
    bcch_bch_mbms,
    bcch_dl_sch,
    bcch_dl_sch_br,
    bcch_dl_sch_mbms,
    mcch,
    pcch,
    dl_ccch,
    dl_dcch,
    ul_ccch,
    ul_dcch,
    sc_mcch,
    // ...
    nulltype
  } value;

  const char* to_string() const;
};
using rrc_class_lte_e = enumerated<rrc_class_lte_opts, true>;

// RRCclass-NR ::= ENUMERATED
struct rrc_class_nr_opts {
  enum options { bcch_bch, bcch_dl_sch, dl_ccch, dl_dcch, pcch, ul_ccch, ul_ccch1, ul_dcch, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using rrc_class_nr_e = enumerated<rrc_class_nr_opts, true>;

// TriggerType-Choice-MIMOandBFconfig ::= SEQUENCE
struct trigger_type_choice_mim_oand_b_fcfg_s {
  struct mim_otrans_mode_state_opts {
    enum options { enabled, disabled, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using mim_otrans_mode_state_e_ = enumerated<mim_otrans_mode_state_opts, true>;

  // member variables
  bool                     ext = false;
  mim_otrans_mode_state_e_ mim_otrans_mode_state;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventTrigger-Cell-Info-Item-Choice-Group ::= SEQUENCE
struct event_trigger_cell_info_item_choice_group_s {
  bool                ext = false;
  ran_param_testing_l ran_param_testing;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventTrigger-Cell-Info-Item-Choice-Individual ::= SEQUENCE
struct event_trigger_cell_info_item_choice_individual_s {
  bool  ext = false;
  cgi_c cell_global_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Interface-MessageID ::= SEQUENCE
struct interface_msg_id_s {
  struct msg_type_opts {
    enum options { init_msg, successful_outcome, unsuccessful_outcome, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using msg_type_e_ = enumerated<msg_type_opts, true>;

  // member variables
  bool        ext               = false;
  int64_t     interface_proc_id = 0;
  msg_type_e_ msg_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterfaceIdentifier ::= CHOICE
struct interface_id_c {
  struct types_opts {
    enum options { ng, xn, f1, e1, s1, x2, w1, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  interface_id_c() = default;
  interface_id_c(const interface_id_c& other);
  interface_id_c& operator=(const interface_id_c& other);
  ~interface_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  interface_id_ng_s& ng()
  {
    assert_choice_type(types::ng, type_, "InterfaceIdentifier");
    return c.get<interface_id_ng_s>();
  }
  interface_id_xn_s& xn()
  {
    assert_choice_type(types::xn, type_, "InterfaceIdentifier");
    return c.get<interface_id_xn_s>();
  }
  interface_id_f1_s& f1()
  {
    assert_choice_type(types::f1, type_, "InterfaceIdentifier");
    return c.get<interface_id_f1_s>();
  }
  interface_id_e1_s& e1()
  {
    assert_choice_type(types::e1, type_, "InterfaceIdentifier");
    return c.get<interface_id_e1_s>();
  }
  interface_id_s1_s& s1()
  {
    assert_choice_type(types::s1, type_, "InterfaceIdentifier");
    return c.get<interface_id_s1_s>();
  }
  interface_id_x2_s& x2()
  {
    assert_choice_type(types::x2, type_, "InterfaceIdentifier");
    return c.get<interface_id_x2_s>();
  }
  interface_id_w1_s& w1()
  {
    assert_choice_type(types::w1, type_, "InterfaceIdentifier");
    return c.get<interface_id_w1_s>();
  }
  const interface_id_ng_s& ng() const
  {
    assert_choice_type(types::ng, type_, "InterfaceIdentifier");
    return c.get<interface_id_ng_s>();
  }
  const interface_id_xn_s& xn() const
  {
    assert_choice_type(types::xn, type_, "InterfaceIdentifier");
    return c.get<interface_id_xn_s>();
  }
  const interface_id_f1_s& f1() const
  {
    assert_choice_type(types::f1, type_, "InterfaceIdentifier");
    return c.get<interface_id_f1_s>();
  }
  const interface_id_e1_s& e1() const
  {
    assert_choice_type(types::e1, type_, "InterfaceIdentifier");
    return c.get<interface_id_e1_s>();
  }
  const interface_id_s1_s& s1() const
  {
    assert_choice_type(types::s1, type_, "InterfaceIdentifier");
    return c.get<interface_id_s1_s>();
  }
  const interface_id_x2_s& x2() const
  {
    assert_choice_type(types::x2, type_, "InterfaceIdentifier");
    return c.get<interface_id_x2_s>();
  }
  const interface_id_w1_s& w1() const
  {
    assert_choice_type(types::w1, type_, "InterfaceIdentifier");
    return c.get<interface_id_w1_s>();
  }
  interface_id_ng_s& set_ng();
  interface_id_xn_s& set_xn();
  interface_id_f1_s& set_f1();
  interface_id_e1_s& set_e1();
  interface_id_s1_s& set_s1();
  interface_id_x2_s& set_x2();
  interface_id_w1_s& set_w1();

private:
  types type_;
  choice_buffer_t<interface_id_e1_s,
                  interface_id_f1_s,
                  interface_id_ng_s,
                  interface_id_s1_s,
                  interface_id_w1_s,
                  interface_id_x2_s,
                  interface_id_xn_s>
      c;

  void destroy_();
};

// InterfaceType ::= ENUMERATED
struct interface_type_opts {
  enum options { ng, xn, f1, e1, s1, x2, w1, /*...*/ nulltype } value;

  const char* to_string() const;
};
using interface_type_e = enumerated<interface_type_opts, true>;

// L2MACschChgType-Choice ::= CHOICE
struct l2_ma_csch_chg_type_choice_c {
  struct types_opts {
    enum options { trigger_type_choice_mim_oand_b_fcfg, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  types       type() const { return types::trigger_type_choice_mim_oand_b_fcfg; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  trigger_type_choice_mim_oand_b_fcfg_s&       trigger_type_choice_mim_oand_b_fcfg() { return c; }
  const trigger_type_choice_mim_oand_b_fcfg_s& trigger_type_choice_mim_oand_b_fcfg() const { return c; }

private:
  trigger_type_choice_mim_oand_b_fcfg_s c;
};

// RRC-MessageID ::= SEQUENCE
struct rrc_msg_id_s {
  struct rrc_type_c_ {
    struct types_opts {
      enum options { lte, nr, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    rrc_type_c_() = default;
    rrc_type_c_(const rrc_type_c_& other);
    rrc_type_c_& operator=(const rrc_type_c_& other);
    ~rrc_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_class_lte_e& lte()
    {
      assert_choice_type(types::lte, type_, "rrcType");
      return c.get<rrc_class_lte_e>();
    }
    rrc_class_nr_e& nr()
    {
      assert_choice_type(types::nr, type_, "rrcType");
      return c.get<rrc_class_nr_e>();
    }
    const rrc_class_lte_e& lte() const
    {
      assert_choice_type(types::lte, type_, "rrcType");
      return c.get<rrc_class_lte_e>();
    }
    const rrc_class_nr_e& nr() const
    {
      assert_choice_type(types::nr, type_, "rrcType");
      return c.get<rrc_class_nr_e>();
    }
    rrc_class_lte_e& set_lte();
    rrc_class_nr_e&  set_nr();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool        ext = false;
  rrc_type_c_ rrc_type;
  int64_t     msg_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TriggerType-Choice-RRCstate-Item ::= SEQUENCE
struct trigger_type_choice_rrc_state_item_s {
  bool        ext           = false;
  bool        lc_or_present = false;
  rrc_state_e state_changed_to;
  lc_or_e     lc_or;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventTrigger-Cell-Info-Item ::= SEQUENCE
struct event_trigger_cell_info_item_s {
  struct cell_type_c_ {
    struct types_opts {
      enum options { cell_type_choice_individual, cell_type_choice_group, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    cell_type_c_() = default;
    cell_type_c_(const cell_type_c_& other);
    cell_type_c_& operator=(const cell_type_c_& other);
    ~cell_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    event_trigger_cell_info_item_choice_individual_s& cell_type_choice_individual()
    {
      assert_choice_type(types::cell_type_choice_individual, type_, "cellType");
      return c.get<event_trigger_cell_info_item_choice_individual_s>();
    }
    event_trigger_cell_info_item_choice_group_s& cell_type_choice_group()
    {
      assert_choice_type(types::cell_type_choice_group, type_, "cellType");
      return c.get<event_trigger_cell_info_item_choice_group_s>();
    }
    const event_trigger_cell_info_item_choice_individual_s& cell_type_choice_individual() const
    {
      assert_choice_type(types::cell_type_choice_individual, type_, "cellType");
      return c.get<event_trigger_cell_info_item_choice_individual_s>();
    }
    const event_trigger_cell_info_item_choice_group_s& cell_type_choice_group() const
    {
      assert_choice_type(types::cell_type_choice_group, type_, "cellType");
      return c.get<event_trigger_cell_info_item_choice_group_s>();
    }
    event_trigger_cell_info_item_choice_individual_s& set_cell_type_choice_individual();
    event_trigger_cell_info_item_choice_group_s&      set_cell_type_choice_group();

  private:
    types type_;
    choice_buffer_t<event_trigger_cell_info_item_choice_group_s, event_trigger_cell_info_item_choice_individual_s> c;

    void destroy_();
  };

  // member variables
  bool         ext                   = false;
  bool         lc_or_present         = false;
  uint32_t     event_trigger_cell_id = 1;
  cell_type_c_ cell_type;
  lc_or_e      lc_or;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventTrigger-UE-Info-Item ::= SEQUENCE
struct event_trigger_ue_info_item_s {
  struct ue_type_c_ {
    struct types_opts {
      enum options { ue_type_choice_individual, ue_type_choice_group, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    ue_type_c_() = default;
    ue_type_c_(const ue_type_c_& other);
    ue_type_c_& operator=(const ue_type_c_& other);
    ~ue_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    event_trigger_ue_info_item_choice_individual_s& ue_type_choice_individual()
    {
      assert_choice_type(types::ue_type_choice_individual, type_, "ueType");
      return c.get<event_trigger_ue_info_item_choice_individual_s>();
    }
    event_trigger_ue_info_item_choice_group_s& ue_type_choice_group()
    {
      assert_choice_type(types::ue_type_choice_group, type_, "ueType");
      return c.get<event_trigger_ue_info_item_choice_group_s>();
    }
    const event_trigger_ue_info_item_choice_individual_s& ue_type_choice_individual() const
    {
      assert_choice_type(types::ue_type_choice_individual, type_, "ueType");
      return c.get<event_trigger_ue_info_item_choice_individual_s>();
    }
    const event_trigger_ue_info_item_choice_group_s& ue_type_choice_group() const
    {
      assert_choice_type(types::ue_type_choice_group, type_, "ueType");
      return c.get<event_trigger_ue_info_item_choice_group_s>();
    }
    event_trigger_ue_info_item_choice_individual_s& set_ue_type_choice_individual();
    event_trigger_ue_info_item_choice_group_s&      set_ue_type_choice_group();

  private:
    types                                                                                                      type_;
    choice_buffer_t<event_trigger_ue_info_item_choice_group_s, event_trigger_ue_info_item_choice_individual_s> c;

    void destroy_();
  };

  // member variables
  bool       ext                 = false;
  bool       lc_or_present       = false;
  uint32_t   event_trigger_ue_id = 1;
  ue_type_c_ ue_type;
  lc_or_e    lc_or;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventTrigger-UEevent-Info-Item ::= SEQUENCE
struct event_trigger_ue_event_info_item_s {
  bool     ext           = false;
  bool     lc_or_present = false;
  uint32_t ue_event_id   = 1;
  lc_or_e  lc_or;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MessageType-Choice-NI ::= SEQUENCE
struct msg_type_choice_ni_s {
  bool               ext            = false;
  bool               ni_id_present  = false;
  bool               ni_msg_present = false;
  interface_type_e   ni_type;
  interface_id_c     ni_id;
  interface_msg_id_s ni_msg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MessageType-Choice-RRC ::= SEQUENCE
struct msg_type_choice_rrc_s {
  bool         ext = false;
  rrc_msg_id_s rrc_msg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TriggerType-Choice-L2MACschChg ::= SEQUENCE
struct trigger_type_choice_l2_ma_csch_chg_s {
  bool                         ext = false;
  l2_ma_csch_chg_type_choice_c l2_ma_csch_chg_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TriggerType-Choice-L2state ::= SEQUENCE
struct trigger_type_choice_l2state_s {
  bool                ext = false;
  ran_param_testing_l associated_l2variables;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TriggerType-Choice-RRCstate ::= SEQUENCE
struct trigger_type_choice_rrc_state_s {
  using rrc_state_list_l_ = dyn_array<trigger_type_choice_rrc_state_item_s>;

  // member variables
  bool              ext = false;
  rrc_state_list_l_ rrc_state_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TriggerType-Choice-UEID ::= SEQUENCE
struct trigger_type_choice_ue_id_s {
  bool     ext             = false;
  uint16_t ue_id_change_id = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TriggerType-Choice-UEcontext ::= SEQUENCE
struct trigger_type_choice_ue_context_s {
  bool                ext = false;
  ran_param_testing_l associated_ue_ctxt_variables;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventTrigger-Cell-Info ::= SEQUENCE
struct event_trigger_cell_info_s {
  using cell_info_list_l_ = dyn_array<event_trigger_cell_info_item_s>;

  // member variables
  bool              ext = false;
  cell_info_list_l_ cell_info_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventTrigger-UE-Info ::= SEQUENCE
struct event_trigger_ue_info_s {
  using ue_info_list_l_ = dyn_array<event_trigger_ue_info_item_s>;

  // member variables
  bool            ext = false;
  ue_info_list_l_ ue_info_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventTrigger-UEevent-Info ::= SEQUENCE
struct event_trigger_ue_event_info_s {
  using ue_event_list_l_ = dyn_array<event_trigger_ue_event_info_item_s>;

  // member variables
  bool             ext = false;
  ue_event_list_l_ ue_event_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MessageType-Choice ::= CHOICE
struct msg_type_choice_c {
  struct types_opts {
    enum options { msg_type_choice_ni, msg_type_choice_rrc, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  msg_type_choice_c() = default;
  msg_type_choice_c(const msg_type_choice_c& other);
  msg_type_choice_c& operator=(const msg_type_choice_c& other);
  ~msg_type_choice_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  msg_type_choice_ni_s& msg_type_choice_ni()
  {
    assert_choice_type(types::msg_type_choice_ni, type_, "MessageType-Choice");
    return c.get<msg_type_choice_ni_s>();
  }
  msg_type_choice_rrc_s& msg_type_choice_rrc()
  {
    assert_choice_type(types::msg_type_choice_rrc, type_, "MessageType-Choice");
    return c.get<msg_type_choice_rrc_s>();
  }
  const msg_type_choice_ni_s& msg_type_choice_ni() const
  {
    assert_choice_type(types::msg_type_choice_ni, type_, "MessageType-Choice");
    return c.get<msg_type_choice_ni_s>();
  }
  const msg_type_choice_rrc_s& msg_type_choice_rrc() const
  {
    assert_choice_type(types::msg_type_choice_rrc, type_, "MessageType-Choice");
    return c.get<msg_type_choice_rrc_s>();
  }
  msg_type_choice_ni_s&  set_msg_type_choice_ni();
  msg_type_choice_rrc_s& set_msg_type_choice_rrc();

private:
  types                                                        type_;
  choice_buffer_t<msg_type_choice_ni_s, msg_type_choice_rrc_s> c;

  void destroy_();
};

// TriggerType-Choice ::= CHOICE
struct trigger_type_choice_c {
  struct types_opts {
    enum options {
      trigger_type_choice_rrc_state,
      trigger_type_choice_ue_id,
      trigger_type_choice_l2state,
      // ...
      trigger_type_choice_ue_context,
      trigger_type_choice_l2_ma_csch_chg,
      nulltype
    } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true, 2>;

  // choice methods
  trigger_type_choice_c() = default;
  trigger_type_choice_c(const trigger_type_choice_c& other);
  trigger_type_choice_c& operator=(const trigger_type_choice_c& other);
  ~trigger_type_choice_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  trigger_type_choice_rrc_state_s& trigger_type_choice_rrc_state()
  {
    assert_choice_type(types::trigger_type_choice_rrc_state, type_, "TriggerType-Choice");
    return c.get<trigger_type_choice_rrc_state_s>();
  }
  trigger_type_choice_ue_id_s& trigger_type_choice_ue_id()
  {
    assert_choice_type(types::trigger_type_choice_ue_id, type_, "TriggerType-Choice");
    return c.get<trigger_type_choice_ue_id_s>();
  }
  trigger_type_choice_l2state_s& trigger_type_choice_l2state()
  {
    assert_choice_type(types::trigger_type_choice_l2state, type_, "TriggerType-Choice");
    return c.get<trigger_type_choice_l2state_s>();
  }
  trigger_type_choice_ue_context_s& trigger_type_choice_ue_context()
  {
    assert_choice_type(types::trigger_type_choice_ue_context, type_, "TriggerType-Choice");
    return c.get<trigger_type_choice_ue_context_s>();
  }
  trigger_type_choice_l2_ma_csch_chg_s& trigger_type_choice_l2_ma_csch_chg()
  {
    assert_choice_type(types::trigger_type_choice_l2_ma_csch_chg, type_, "TriggerType-Choice");
    return c.get<trigger_type_choice_l2_ma_csch_chg_s>();
  }
  const trigger_type_choice_rrc_state_s& trigger_type_choice_rrc_state() const
  {
    assert_choice_type(types::trigger_type_choice_rrc_state, type_, "TriggerType-Choice");
    return c.get<trigger_type_choice_rrc_state_s>();
  }
  const trigger_type_choice_ue_id_s& trigger_type_choice_ue_id() const
  {
    assert_choice_type(types::trigger_type_choice_ue_id, type_, "TriggerType-Choice");
    return c.get<trigger_type_choice_ue_id_s>();
  }
  const trigger_type_choice_l2state_s& trigger_type_choice_l2state() const
  {
    assert_choice_type(types::trigger_type_choice_l2state, type_, "TriggerType-Choice");
    return c.get<trigger_type_choice_l2state_s>();
  }
  const trigger_type_choice_ue_context_s& trigger_type_choice_ue_context() const
  {
    assert_choice_type(types::trigger_type_choice_ue_context, type_, "TriggerType-Choice");
    return c.get<trigger_type_choice_ue_context_s>();
  }
  const trigger_type_choice_l2_ma_csch_chg_s& trigger_type_choice_l2_ma_csch_chg() const
  {
    assert_choice_type(types::trigger_type_choice_l2_ma_csch_chg, type_, "TriggerType-Choice");
    return c.get<trigger_type_choice_l2_ma_csch_chg_s>();
  }
  trigger_type_choice_rrc_state_s&      set_trigger_type_choice_rrc_state();
  trigger_type_choice_ue_id_s&          set_trigger_type_choice_ue_id();
  trigger_type_choice_l2state_s&        set_trigger_type_choice_l2state();
  trigger_type_choice_ue_context_s&     set_trigger_type_choice_ue_context();
  trigger_type_choice_l2_ma_csch_chg_s& set_trigger_type_choice_l2_ma_csch_chg();

private:
  types type_;
  choice_buffer_t<trigger_type_choice_l2_ma_csch_chg_s,
                  trigger_type_choice_l2state_s,
                  trigger_type_choice_rrc_state_s,
                  trigger_type_choice_ue_context_s,
                  trigger_type_choice_ue_id_s>
      c;

  void destroy_();
};

// E2SM-RC-EventTrigger-Format1-Item ::= SEQUENCE
struct e2_sm_rc_event_trigger_format1_item_s {
  struct msg_direction_opts {
    enum options { incoming, outgoing, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using msg_direction_e_ = enumerated<msg_direction_opts, true>;

  // member variables
  bool                          ext                            = false;
  bool                          msg_direction_present          = false;
  bool                          associated_ue_info_present     = false;
  bool                          associated_ue_event_present    = false;
  bool                          lc_or_present                  = false;
  uint32_t                      ric_event_trigger_condition_id = 1;
  msg_type_choice_c             msg_type;
  msg_direction_e_              msg_direction;
  event_trigger_ue_info_s       associated_ue_info;
  event_trigger_ue_event_info_s associated_ue_event;
  lc_or_e                       lc_or;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-EventTrigger-Format3-Item ::= SEQUENCE
struct e2_sm_rc_event_trigger_format3_item_s {
  bool                      ext                            = false;
  bool                      associated_cell_info_present   = false;
  bool                      lc_or_present                  = false;
  uint32_t                  ric_event_trigger_condition_id = 1;
  uint16_t                  e2_node_info_change_id         = 1;
  event_trigger_cell_info_s associated_cell_info;
  lc_or_e                   lc_or;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-EventTrigger-Format4-Item ::= SEQUENCE
struct e2_sm_rc_event_trigger_format4_item_s {
  bool                    ext                            = false;
  bool                    associated_ue_info_present     = false;
  bool                    lc_or_present                  = false;
  uint32_t                ric_event_trigger_condition_id = 1;
  trigger_type_choice_c   trigger_type;
  event_trigger_ue_info_s associated_ue_info;
  lc_or_e                 lc_or;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-EventTrigger-Format1 ::= SEQUENCE
struct e2_sm_rc_event_trigger_format1_s {
  using msg_list_l_ = dyn_array<e2_sm_rc_event_trigger_format1_item_s>;

  // member variables
  bool                    ext                               = false;
  bool                    global_associated_ue_info_present = false;
  msg_list_l_             msg_list;
  event_trigger_ue_info_s global_associated_ue_info;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-EventTrigger-Format2 ::= SEQUENCE
struct e2_sm_rc_event_trigger_format2_s {
  bool                    ext                            = false;
  bool                    associated_ue_info_present     = false;
  uint32_t                ric_call_process_type_id       = 1;
  uint32_t                ric_call_process_breakpoint_id = 1;
  ran_param_testing_l     associated_e2_node_info;
  event_trigger_ue_info_s associated_ue_info;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-EventTrigger-Format3 ::= SEQUENCE
struct e2_sm_rc_event_trigger_format3_s {
  using e2_node_info_change_list_l_ = dyn_array<e2_sm_rc_event_trigger_format3_item_s>;

  // member variables
  bool                        ext = false;
  e2_node_info_change_list_l_ e2_node_info_change_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-EventTrigger-Format4 ::= SEQUENCE
struct e2_sm_rc_event_trigger_format4_s {
  using ue_info_change_list_l_ = dyn_array<e2_sm_rc_event_trigger_format4_item_s>;

  // member variables
  bool                   ext = false;
  ue_info_change_list_l_ ue_info_change_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-EventTrigger ::= SEQUENCE
struct e2_sm_rc_event_trigger_s {
  struct ric_event_trigger_formats_c_ {
    struct types_opts {
      enum options {
        event_trigger_format1,
        event_trigger_format2,
        event_trigger_format3,
        event_trigger_format4,
        event_trigger_format5,
        // ...
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    ric_event_trigger_formats_c_() = default;
    ric_event_trigger_formats_c_(const ric_event_trigger_formats_c_& other);
    ric_event_trigger_formats_c_& operator=(const ric_event_trigger_formats_c_& other);
    ~ric_event_trigger_formats_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2_sm_rc_event_trigger_format1_s& event_trigger_format1()
    {
      assert_choice_type(types::event_trigger_format1, type_, "ric-eventTrigger-formats");
      return c.get<e2_sm_rc_event_trigger_format1_s>();
    }
    e2_sm_rc_event_trigger_format2_s& event_trigger_format2()
    {
      assert_choice_type(types::event_trigger_format2, type_, "ric-eventTrigger-formats");
      return c.get<e2_sm_rc_event_trigger_format2_s>();
    }
    e2_sm_rc_event_trigger_format3_s& event_trigger_format3()
    {
      assert_choice_type(types::event_trigger_format3, type_, "ric-eventTrigger-formats");
      return c.get<e2_sm_rc_event_trigger_format3_s>();
    }
    e2_sm_rc_event_trigger_format4_s& event_trigger_format4()
    {
      assert_choice_type(types::event_trigger_format4, type_, "ric-eventTrigger-formats");
      return c.get<e2_sm_rc_event_trigger_format4_s>();
    }
    const e2_sm_rc_event_trigger_format1_s& event_trigger_format1() const
    {
      assert_choice_type(types::event_trigger_format1, type_, "ric-eventTrigger-formats");
      return c.get<e2_sm_rc_event_trigger_format1_s>();
    }
    const e2_sm_rc_event_trigger_format2_s& event_trigger_format2() const
    {
      assert_choice_type(types::event_trigger_format2, type_, "ric-eventTrigger-formats");
      return c.get<e2_sm_rc_event_trigger_format2_s>();
    }
    const e2_sm_rc_event_trigger_format3_s& event_trigger_format3() const
    {
      assert_choice_type(types::event_trigger_format3, type_, "ric-eventTrigger-formats");
      return c.get<e2_sm_rc_event_trigger_format3_s>();
    }
    const e2_sm_rc_event_trigger_format4_s& event_trigger_format4() const
    {
      assert_choice_type(types::event_trigger_format4, type_, "ric-eventTrigger-formats");
      return c.get<e2_sm_rc_event_trigger_format4_s>();
    }
    e2_sm_rc_event_trigger_format1_s& set_event_trigger_format1();
    e2_sm_rc_event_trigger_format2_s& set_event_trigger_format2();
    e2_sm_rc_event_trigger_format3_s& set_event_trigger_format3();
    e2_sm_rc_event_trigger_format4_s& set_event_trigger_format4();
    void                              set_event_trigger_format5();

  private:
    types type_;
    choice_buffer_t<e2_sm_rc_event_trigger_format1_s,
                    e2_sm_rc_event_trigger_format2_s,
                    e2_sm_rc_event_trigger_format3_s,
                    e2_sm_rc_event_trigger_format4_s>
        c;

    void destroy_();
  };

  // member variables
  bool                         ext = false;
  ric_event_trigger_formats_c_ ric_event_trigger_formats;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-IndicationHeader-Format1 ::= SEQUENCE
struct e2_sm_rc_ind_hdr_format1_s {
  bool     ext                                    = false;
  bool     ric_event_trigger_condition_id_present = false;
  uint32_t ric_event_trigger_condition_id         = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-IndicationHeader-Format2 ::= SEQUENCE
struct e2_sm_rc_ind_hdr_format2_s {
  bool     ext = false;
  ue_id_c  ue_id;
  int64_t  ric_insert_style_type = 0;
  uint32_t ric_insert_ind_id     = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-IndicationHeader-Format3 ::= SEQUENCE
struct e2_sm_rc_ind_hdr_format3_s {
  bool     ext                                    = false;
  bool     ric_event_trigger_condition_id_present = false;
  bool     ue_id_present                          = false;
  uint32_t ric_event_trigger_condition_id         = 1;
  ue_id_c  ue_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-IndicationHeader ::= SEQUENCE
struct e2_sm_rc_ind_hdr_s {
  struct ric_ind_hdr_formats_c_ {
    struct types_opts {
      enum options { ind_hdr_format1, ind_hdr_format2, /*...*/ ind_hdr_format3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true, 1>;

    // choice methods
    ric_ind_hdr_formats_c_() = default;
    ric_ind_hdr_formats_c_(const ric_ind_hdr_formats_c_& other);
    ric_ind_hdr_formats_c_& operator=(const ric_ind_hdr_formats_c_& other);
    ~ric_ind_hdr_formats_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2_sm_rc_ind_hdr_format1_s& ind_hdr_format1()
    {
      assert_choice_type(types::ind_hdr_format1, type_, "ric-indicationHeader-formats");
      return c.get<e2_sm_rc_ind_hdr_format1_s>();
    }
    e2_sm_rc_ind_hdr_format2_s& ind_hdr_format2()
    {
      assert_choice_type(types::ind_hdr_format2, type_, "ric-indicationHeader-formats");
      return c.get<e2_sm_rc_ind_hdr_format2_s>();
    }
    e2_sm_rc_ind_hdr_format3_s& ind_hdr_format3()
    {
      assert_choice_type(types::ind_hdr_format3, type_, "ric-indicationHeader-formats");
      return c.get<e2_sm_rc_ind_hdr_format3_s>();
    }
    const e2_sm_rc_ind_hdr_format1_s& ind_hdr_format1() const
    {
      assert_choice_type(types::ind_hdr_format1, type_, "ric-indicationHeader-formats");
      return c.get<e2_sm_rc_ind_hdr_format1_s>();
    }
    const e2_sm_rc_ind_hdr_format2_s& ind_hdr_format2() const
    {
      assert_choice_type(types::ind_hdr_format2, type_, "ric-indicationHeader-formats");
      return c.get<e2_sm_rc_ind_hdr_format2_s>();
    }
    const e2_sm_rc_ind_hdr_format3_s& ind_hdr_format3() const
    {
      assert_choice_type(types::ind_hdr_format3, type_, "ric-indicationHeader-formats");
      return c.get<e2_sm_rc_ind_hdr_format3_s>();
    }
    e2_sm_rc_ind_hdr_format1_s& set_ind_hdr_format1();
    e2_sm_rc_ind_hdr_format2_s& set_ind_hdr_format2();
    e2_sm_rc_ind_hdr_format3_s& set_ind_hdr_format3();

  private:
    types                                                                                               type_;
    choice_buffer_t<e2_sm_rc_ind_hdr_format1_s, e2_sm_rc_ind_hdr_format2_s, e2_sm_rc_ind_hdr_format3_s> c;

    void destroy_();
  };

  // member variables
  bool                   ext = false;
  ric_ind_hdr_formats_c_ ric_ind_hdr_formats;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedSULFreqBandItem ::= SEQUENCE
struct supported_sul_freq_band_item_s {
  bool     ext              = false;
  uint16_t freq_band_ind_nr = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedSULBandList ::= SEQUENCE (SIZE (0..32)) OF SupportedSULFreqBandItem
using supported_sul_band_list_l = dyn_array<supported_sul_freq_band_item_s>;

// NRFrequencyBandItem ::= SEQUENCE
struct nr_freq_band_item_s {
  bool                      ext              = false;
  uint16_t                  freq_band_ind_nr = 1;
  supported_sul_band_list_l supported_sul_band_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-ARFCN ::= SEQUENCE
struct nr_arfcn_s {
  bool     ext      = false;
  uint32_t nr_arfcn = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NRFrequencyBand-List ::= SEQUENCE (SIZE (1..32)) OF NRFrequencyBandItem
using nr_freq_band_list_l = dyn_array<nr_freq_band_item_s>;

// NRFrequencyShift7p5khz ::= ENUMERATED
struct nr_freq_shift7p5khz_opts {
  enum options { false_value, true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using nr_freq_shift7p5khz_e = enumerated<nr_freq_shift7p5khz_opts, true>;

// NRFrequencyInfo ::= SEQUENCE
struct nr_freq_info_s {
  bool                  ext                      = false;
  bool                  freq_shift7p5khz_present = false;
  nr_arfcn_s            nr_arfcn;
  nr_freq_band_list_l   freq_band_list;
  nr_freq_shift7p5khz_e freq_shift7p5khz;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeighborCell-Item-Choice-E-UTRA ::= SEQUENCE
struct neighbor_cell_item_choice_e_utra_s {
  struct x2_xn_established_opts {
    enum options { true_value, false_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using x2_xn_established_e_ = enumerated<x2_xn_established_opts, true>;
  struct ho_validated_opts {
    enum options { true_value, false_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using ho_validated_e_ = enumerated<ho_validated_opts, true>;

  // member variables
  bool                     ext = false;
  eutra_cgi_s              eutra_cgi;
  uint16_t                 eutra_pci   = 0;
  uint32_t                 eutra_arfcn = 0;
  fixed_octstring<2, true> eutra_tac;
  x2_xn_established_e_     x2_xn_established;
  ho_validated_e_          ho_validated;
  uint32_t                 version = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeighborCell-Item-Choice-NR ::= SEQUENCE
struct neighbor_cell_item_choice_nr_s {
  struct nr_mode_info_opts {
    enum options { fdd, tdd, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using nr_mode_info_e_ = enumerated<nr_mode_info_opts, true>;
  struct x2_xn_established_opts {
    enum options { true_value, false_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using x2_xn_established_e_ = enumerated<x2_xn_established_opts, true>;
  struct ho_validated_opts {
    enum options { true_value, false_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using ho_validated_e_ = enumerated<ho_validated_opts, true>;

  // member variables
  bool                     ext = false;
  nr_cgi_s                 nr_cgi;
  uint16_t                 nr_pci = 0;
  fixed_octstring<3, true> five_gs_tac;
  nr_mode_info_e_          nr_mode_info;
  nr_freq_info_s           nr_freq_info;
  x2_xn_established_e_     x2_xn_established;
  ho_validated_e_          ho_validated;
  uint32_t                 version = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeighborCell-Item ::= CHOICE
struct neighbor_cell_item_c {
  struct types_opts {
    enum options { ran_type_choice_nr, ran_type_choice_eutra, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  neighbor_cell_item_c() = default;
  neighbor_cell_item_c(const neighbor_cell_item_c& other);
  neighbor_cell_item_c& operator=(const neighbor_cell_item_c& other);
  ~neighbor_cell_item_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  neighbor_cell_item_choice_nr_s& ran_type_choice_nr()
  {
    assert_choice_type(types::ran_type_choice_nr, type_, "NeighborCell-Item");
    return c.get<neighbor_cell_item_choice_nr_s>();
  }
  neighbor_cell_item_choice_e_utra_s& ran_type_choice_eutra()
  {
    assert_choice_type(types::ran_type_choice_eutra, type_, "NeighborCell-Item");
    return c.get<neighbor_cell_item_choice_e_utra_s>();
  }
  const neighbor_cell_item_choice_nr_s& ran_type_choice_nr() const
  {
    assert_choice_type(types::ran_type_choice_nr, type_, "NeighborCell-Item");
    return c.get<neighbor_cell_item_choice_nr_s>();
  }
  const neighbor_cell_item_choice_e_utra_s& ran_type_choice_eutra() const
  {
    assert_choice_type(types::ran_type_choice_eutra, type_, "NeighborCell-Item");
    return c.get<neighbor_cell_item_choice_e_utra_s>();
  }
  neighbor_cell_item_choice_nr_s&     set_ran_type_choice_nr();
  neighbor_cell_item_choice_e_utra_s& set_ran_type_choice_eutra();

private:
  types                                                                               type_;
  choice_buffer_t<neighbor_cell_item_choice_e_utra_s, neighbor_cell_item_choice_nr_s> c;

  void destroy_();
};

// E2SM-RC-IndicationMessage-Format6-RANP-Item ::= SEQUENCE
struct e2_sm_rc_ind_msg_format6_ranp_item_s {
  bool                   ext          = false;
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeighborCell-List ::= SEQUENCE (SIZE (1..65535)) OF NeighborCell-Item
using neighbor_cell_list_l = dyn_array<neighbor_cell_item_c>;

// ServingCell-ARFCN ::= CHOICE
struct serving_cell_arfcn_c {
  struct types_opts {
    enum options { nr, eutra, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  serving_cell_arfcn_c() = default;
  serving_cell_arfcn_c(const serving_cell_arfcn_c& other);
  serving_cell_arfcn_c& operator=(const serving_cell_arfcn_c& other);
  ~serving_cell_arfcn_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  nr_arfcn_s& nr()
  {
    assert_choice_type(types::nr, type_, "ServingCell-ARFCN");
    return c.get<nr_arfcn_s>();
  }
  uint32_t& eutra()
  {
    assert_choice_type(types::eutra, type_, "ServingCell-ARFCN");
    return c.get<uint32_t>();
  }
  const nr_arfcn_s& nr() const
  {
    assert_choice_type(types::nr, type_, "ServingCell-ARFCN");
    return c.get<nr_arfcn_s>();
  }
  const uint32_t& eutra() const
  {
    assert_choice_type(types::eutra, type_, "ServingCell-ARFCN");
    return c.get<uint32_t>();
  }
  nr_arfcn_s& set_nr();
  uint32_t&   set_eutra();

private:
  types                       type_;
  choice_buffer_t<nr_arfcn_s> c;

  void destroy_();
};

// ServingCell-PCI ::= CHOICE
struct serving_cell_pci_c {
  struct types_opts {
    enum options { nr, eutra, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  serving_cell_pci_c() = default;
  serving_cell_pci_c(const serving_cell_pci_c& other);
  serving_cell_pci_c& operator=(const serving_cell_pci_c& other);
  ~serving_cell_pci_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint16_t& nr()
  {
    assert_choice_type(types::nr, type_, "ServingCell-PCI");
    return c.get<uint16_t>();
  }
  uint16_t& eutra()
  {
    assert_choice_type(types::eutra, type_, "ServingCell-PCI");
    return c.get<uint16_t>();
  }
  const uint16_t& nr() const
  {
    assert_choice_type(types::nr, type_, "ServingCell-PCI");
    return c.get<uint16_t>();
  }
  const uint16_t& eutra() const
  {
    assert_choice_type(types::eutra, type_, "ServingCell-PCI");
    return c.get<uint16_t>();
  }
  uint16_t& set_nr();
  uint16_t& set_eutra();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// E2SM-RC-IndicationMessage-Format2-RANParameter-Item ::= SEQUENCE
struct e2_sm_rc_ind_msg_format2_ran_param_item_s {
  bool                   ext          = false;
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-IndicationMessage-Format6-Indication-Item ::= SEQUENCE
struct e2_sm_rc_ind_msg_format6_ind_item_s {
  using ran_p_insert_ind_list_l_ = dyn_array<e2_sm_rc_ind_msg_format6_ranp_item_s>;

  // member variables
  bool                     ext               = false;
  uint32_t                 ric_insert_ind_id = 1;
  ran_p_insert_ind_list_l_ ran_p_insert_ind_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeighborRelation-Info ::= SEQUENCE
struct neighbor_relation_info_s {
  bool                 ext = false;
  serving_cell_pci_c   serving_cell_pci;
  serving_cell_arfcn_c serving_cell_arfcn;
  neighbor_cell_list_l neighbor_cell_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-IndicationMessage-Format1-Item ::= SEQUENCE
struct e2_sm_rc_ind_msg_format1_item_s {
  bool                   ext          = false;
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-IndicationMessage-Format2-Item ::= SEQUENCE
struct e2_sm_rc_ind_msg_format2_item_s {
  using ran_p_list_l_ = dyn_array<e2_sm_rc_ind_msg_format2_ran_param_item_s>;

  // member variables
  bool          ext = false;
  ue_id_c       ue_id;
  ran_p_list_l_ ran_p_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-IndicationMessage-Format3-Item ::= SEQUENCE
struct e2_sm_rc_ind_msg_format3_item_s {
  bool                      ext                             = false;
  bool                      cell_deleted_present            = false;
  bool                      neighbor_relation_table_present = false;
  cgi_c                     cell_global_id;
  unbounded_octstring<true> cell_context_info;
  bool                      cell_deleted = false;
  neighbor_relation_info_s  neighbor_relation_table;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-IndicationMessage-Format5-Item ::= SEQUENCE
struct e2_sm_rc_ind_msg_format5_item_s {
  bool                   ext          = false;
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-IndicationMessage-Format6-Style-Item ::= SEQUENCE
struct e2_sm_rc_ind_msg_format6_style_item_s {
  using ric_insert_ind_list_l_ = dyn_array<e2_sm_rc_ind_msg_format6_ind_item_s>;

  // member variables
  bool                   ext                         = false;
  int64_t                indicated_insert_style_type = 0;
  ric_insert_ind_list_l_ ric_insert_ind_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-IndicationMessage-Format1 ::= SEQUENCE
struct e2_sm_rc_ind_msg_format1_s {
  using ran_p_reported_list_l_ = dyn_array<e2_sm_rc_ind_msg_format1_item_s>;

  // member variables
  bool                   ext = false;
  ran_p_reported_list_l_ ran_p_reported_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-IndicationMessage-Format2 ::= SEQUENCE
struct e2_sm_rc_ind_msg_format2_s {
  using ue_param_list_l_ = dyn_array<e2_sm_rc_ind_msg_format2_item_s>;

  // member variables
  bool             ext = false;
  ue_param_list_l_ ue_param_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-IndicationMessage-Format3 ::= SEQUENCE
struct e2_sm_rc_ind_msg_format3_s {
  using cell_info_list_l_ = dyn_array<e2_sm_rc_ind_msg_format3_item_s>;

  // member variables
  bool              ext = false;
  cell_info_list_l_ cell_info_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-IndicationMessage-Format5 ::= SEQUENCE
struct e2_sm_rc_ind_msg_format5_s {
  using ran_p_requested_list_l_ = dyn_array<e2_sm_rc_ind_msg_format5_item_s>;

  // member variables
  bool                    ext = false;
  ran_p_requested_list_l_ ran_p_requested_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-IndicationMessage-Format6 ::= SEQUENCE
struct e2_sm_rc_ind_msg_format6_s {
  using ric_insert_style_list_l_ = dyn_array<e2_sm_rc_ind_msg_format6_style_item_s>;

  // member variables
  bool                     ext = false;
  ric_insert_style_list_l_ ric_insert_style_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-IndicationMessage ::= SEQUENCE
struct e2_sm_rc_ind_msg_s {
  struct ric_ind_msg_formats_c_ {
    struct types_opts {
      enum options {
        ind_msg_format1,
        ind_msg_format2,
        ind_msg_format3,
        ind_msg_format4,
        ind_msg_format5,
        // ...
        ind_msg_format6,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true, 1>;

    // choice methods
    ric_ind_msg_formats_c_() = default;
    ric_ind_msg_formats_c_(const ric_ind_msg_formats_c_& other);
    ric_ind_msg_formats_c_& operator=(const ric_ind_msg_formats_c_& other);
    ~ric_ind_msg_formats_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2_sm_rc_ind_msg_format1_s& ind_msg_format1()
    {
      assert_choice_type(types::ind_msg_format1, type_, "ric-indicationMessage-formats");
      return c.get<e2_sm_rc_ind_msg_format1_s>();
    }
    e2_sm_rc_ind_msg_format2_s& ind_msg_format2()
    {
      assert_choice_type(types::ind_msg_format2, type_, "ric-indicationMessage-formats");
      return c.get<e2_sm_rc_ind_msg_format2_s>();
    }
    e2_sm_rc_ind_msg_format3_s& ind_msg_format3()
    {
      assert_choice_type(types::ind_msg_format3, type_, "ric-indicationMessage-formats");
      return c.get<e2_sm_rc_ind_msg_format3_s>();
    }
    e2_sm_rc_ind_msg_format5_s& ind_msg_format5()
    {
      assert_choice_type(types::ind_msg_format5, type_, "ric-indicationMessage-formats");
      return c.get<e2_sm_rc_ind_msg_format5_s>();
    }
    e2_sm_rc_ind_msg_format6_s& ind_msg_format6()
    {
      assert_choice_type(types::ind_msg_format6, type_, "ric-indicationMessage-formats");
      return c.get<e2_sm_rc_ind_msg_format6_s>();
    }
    const e2_sm_rc_ind_msg_format1_s& ind_msg_format1() const
    {
      assert_choice_type(types::ind_msg_format1, type_, "ric-indicationMessage-formats");
      return c.get<e2_sm_rc_ind_msg_format1_s>();
    }
    const e2_sm_rc_ind_msg_format2_s& ind_msg_format2() const
    {
      assert_choice_type(types::ind_msg_format2, type_, "ric-indicationMessage-formats");
      return c.get<e2_sm_rc_ind_msg_format2_s>();
    }
    const e2_sm_rc_ind_msg_format3_s& ind_msg_format3() const
    {
      assert_choice_type(types::ind_msg_format3, type_, "ric-indicationMessage-formats");
      return c.get<e2_sm_rc_ind_msg_format3_s>();
    }
    const e2_sm_rc_ind_msg_format5_s& ind_msg_format5() const
    {
      assert_choice_type(types::ind_msg_format5, type_, "ric-indicationMessage-formats");
      return c.get<e2_sm_rc_ind_msg_format5_s>();
    }
    const e2_sm_rc_ind_msg_format6_s& ind_msg_format6() const
    {
      assert_choice_type(types::ind_msg_format6, type_, "ric-indicationMessage-formats");
      return c.get<e2_sm_rc_ind_msg_format6_s>();
    }
    e2_sm_rc_ind_msg_format1_s& set_ind_msg_format1();
    e2_sm_rc_ind_msg_format2_s& set_ind_msg_format2();
    e2_sm_rc_ind_msg_format3_s& set_ind_msg_format3();
    void                        set_ind_msg_format4();
    e2_sm_rc_ind_msg_format5_s& set_ind_msg_format5();
    e2_sm_rc_ind_msg_format6_s& set_ind_msg_format6();

  private:
    types type_;
    choice_buffer_t<e2_sm_rc_ind_msg_format1_s,
                    e2_sm_rc_ind_msg_format2_s,
                    e2_sm_rc_ind_msg_format3_s,
                    e2_sm_rc_ind_msg_format5_s,
                    e2_sm_rc_ind_msg_format6_s>
        c;

    void destroy_();
  };

  // member variables
  bool                   ext = false;
  ric_ind_msg_formats_c_ ric_ind_msg_formats;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-QueryDefinition-Format1-Item ::= SEQUENCE
struct e2_sm_rc_query_definition_format1_item_s {
  bool                   ext                          = false;
  bool                   ran_param_definition_present = false;
  uint64_t               ran_param_id                 = 1;
  ran_param_definition_s ran_param_definition;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-QueryDefinition-Format1 ::= SEQUENCE
struct e2_sm_rc_query_definition_format1_s {
  using ran_p_list_l_ = dyn_array<e2_sm_rc_query_definition_format1_item_s>;

  // member variables
  bool          ext = false;
  ran_p_list_l_ ran_p_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-QueryDefinition ::= SEQUENCE
struct e2_sm_rc_query_definition_s {
  struct ric_query_definition_formats_c_ {
    struct types_opts {
      enum options { query_request_format1, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    types       type() const { return types::query_request_format1; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2_sm_rc_query_definition_format1_s&       query_request_format1() { return c; }
    const e2_sm_rc_query_definition_format1_s& query_request_format1() const { return c; }

  private:
    e2_sm_rc_query_definition_format1_s c;
  };

  // member variables
  bool                            ext = false;
  ric_query_definition_formats_c_ ric_query_definition_formats;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-QueryHeader-Format1 ::= SEQUENCE
struct e2_sm_rc_query_hdr_format1_s {
  bool                 ext                        = false;
  bool                 associated_ue_info_present = false;
  int64_t              ric_style_type             = 0;
  ran_param_testing_l  associated_e2_node_info;
  associated_ue_info_s associated_ue_info;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-QueryHeader ::= SEQUENCE
struct e2_sm_rc_query_hdr_s {
  struct ric_query_hdr_formats_c_ {
    struct types_opts {
      enum options { query_hdr_format1, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    types       type() const { return types::query_hdr_format1; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2_sm_rc_query_hdr_format1_s&       query_hdr_format1() { return c; }
    const e2_sm_rc_query_hdr_format1_s& query_hdr_format1() const { return c; }

  private:
    e2_sm_rc_query_hdr_format1_s c;
  };

  // member variables
  bool                     ext = false;
  ric_query_hdr_formats_c_ ric_query_hdr_formats;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-QueryOutcome-Format1-ItemParameters ::= SEQUENCE
struct e2_sm_rc_query_outcome_format1_item_params_s {
  bool                   ext                          = false;
  bool                   ran_param_value_type_present = false;
  uint64_t               ran_param_id                 = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-QueryOutcome-Format2-ItemParameters ::= SEQUENCE
struct e2_sm_rc_query_outcome_format2_item_params_s {
  bool                   ext                          = false;
  bool                   ran_param_value_type_present = false;
  uint64_t               ran_param_id                 = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-QueryOutcome-Format1-ItemCell ::= SEQUENCE
struct e2_sm_rc_query_outcome_format1_item_cell_s {
  using ran_p_list_l_ = dyn_array<e2_sm_rc_query_outcome_format1_item_params_s>;

  // member variables
  bool                     ext                             = false;
  bool                     neighbor_relation_table_present = false;
  cgi_c                    cell_global_id;
  ran_p_list_l_            ran_p_list;
  neighbor_relation_info_s neighbor_relation_table;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-QueryOutcome-Format2-ItemUE ::= SEQUENCE
struct e2_sm_rc_query_outcome_format2_item_ue_s {
  using ran_p_list_l_ = dyn_array<e2_sm_rc_query_outcome_format2_item_params_s>;

  // member variables
  bool          ext                = false;
  bool          ue_filt_id_present = false;
  ue_id_c       ue_id;
  ran_p_list_l_ ran_p_list;
  uint32_t      ue_filt_id = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-QueryOutcome-Format1 ::= SEQUENCE
struct e2_sm_rc_query_outcome_format1_s {
  using cell_info_list_l_ = dyn_array<e2_sm_rc_query_outcome_format1_item_cell_s>;

  // member variables
  bool              ext = false;
  cell_info_list_l_ cell_info_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-QueryOutcome-Format2 ::= SEQUENCE
struct e2_sm_rc_query_outcome_format2_s {
  using ue_info_list_l_ = dyn_array<e2_sm_rc_query_outcome_format2_item_ue_s>;

  // member variables
  bool            ext = false;
  ue_info_list_l_ ue_info_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-QueryOutcome ::= SEQUENCE
struct e2_sm_rc_query_outcome_s {
  struct ric_query_outcome_formats_c_ {
    struct types_opts {
      enum options { query_outcome_format1, query_outcome_format2, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    ric_query_outcome_formats_c_() = default;
    ric_query_outcome_formats_c_(const ric_query_outcome_formats_c_& other);
    ric_query_outcome_formats_c_& operator=(const ric_query_outcome_formats_c_& other);
    ~ric_query_outcome_formats_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2_sm_rc_query_outcome_format1_s& query_outcome_format1()
    {
      assert_choice_type(types::query_outcome_format1, type_, "ric-queryOutcome-formats");
      return c.get<e2_sm_rc_query_outcome_format1_s>();
    }
    e2_sm_rc_query_outcome_format2_s& query_outcome_format2()
    {
      assert_choice_type(types::query_outcome_format2, type_, "ric-queryOutcome-formats");
      return c.get<e2_sm_rc_query_outcome_format2_s>();
    }
    const e2_sm_rc_query_outcome_format1_s& query_outcome_format1() const
    {
      assert_choice_type(types::query_outcome_format1, type_, "ric-queryOutcome-formats");
      return c.get<e2_sm_rc_query_outcome_format1_s>();
    }
    const e2_sm_rc_query_outcome_format2_s& query_outcome_format2() const
    {
      assert_choice_type(types::query_outcome_format2, type_, "ric-queryOutcome-formats");
      return c.get<e2_sm_rc_query_outcome_format2_s>();
    }
    e2_sm_rc_query_outcome_format1_s& set_query_outcome_format1();
    e2_sm_rc_query_outcome_format2_s& set_query_outcome_format2();

  private:
    types                                                                               type_;
    choice_buffer_t<e2_sm_rc_query_outcome_format1_s, e2_sm_rc_query_outcome_format2_s> c;

    void destroy_();
  };

  // member variables
  bool                         ext = false;
  ric_query_outcome_formats_c_ ric_query_outcome_formats;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InsertIndication-RANParameter-Item ::= SEQUENCE
struct insert_ind_ran_param_item_s {
  bool                                 ext          = false;
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PolicyAction-RANParameter-Item ::= SEQUENCE
struct policy_action_ran_param_item_s {
  bool                                 ext          = false;
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PolicyCondition-RANParameter-Item ::= SEQUENCE
struct policy_condition_ran_param_item_s {
  bool                                 ext          = false;
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Query-RANParameter-Item ::= SEQUENCE
struct query_ran_param_item_s {
  bool                                 ext                          = false;
  bool                                 ran_param_definition_present = false;
  uint64_t                             ran_param_id                 = 1;
  printable_string<1, 150, true, true> ran_param_name;
  ran_param_definition_s               ran_param_definition;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANFunctionDefinition-Control-Action-Item ::= SEQUENCE
struct ran_function_definition_ctrl_action_item_s {
  using ran_ctrl_action_params_list_l_ = dyn_array<ctrl_action_ran_param_item_s>;
  struct ue_group_ctrl_action_supported_opts {
    enum options { true_value, false_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using ue_group_ctrl_action_supported_e_ = enumerated<ue_group_ctrl_action_supported_opts, true>;

  // member variables
  bool                                 ext                = false;
  uint32_t                             ric_ctrl_action_id = 1;
  printable_string<1, 150, true, true> ric_ctrl_action_name;
  ran_ctrl_action_params_list_l_       ran_ctrl_action_params_list;
  // ...
  ue_group_ctrl_action_supported_e_ ue_group_ctrl_action_supported;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANFunctionDefinition-EventTrigger-Breakpoint-Item ::= SEQUENCE
struct ran_function_definition_event_trigger_breakpoint_item_s {
  using ran_call_process_breakpoint_params_list_l_ = dyn_array<call_process_breakpoint_ran_param_item_s>;

  // member variables
  bool                                       ext                        = false;
  uint32_t                                   call_process_breakpoint_id = 1;
  printable_string<1, 150, true, true>       call_process_breakpoint_name;
  ran_call_process_breakpoint_params_list_l_ ran_call_process_breakpoint_params_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANFunctionDefinition-Insert-Indication-Item ::= SEQUENCE
struct ran_function_definition_insert_ind_item_s {
  using ran_insert_ind_params_list_l_ = dyn_array<insert_ind_ran_param_item_s>;

  // member variables
  bool                                 ext               = false;
  uint32_t                             ric_insert_ind_id = 1;
  printable_string<1, 150, true, true> ric_insert_ind_name;
  ran_insert_ind_params_list_l_        ran_insert_ind_params_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANFunctionDefinition-Policy-Action-Item ::= SEQUENCE
struct ran_function_definition_policy_action_item_s {
  using ran_policy_action_params_list_l_    = dyn_array<policy_action_ran_param_item_s>;
  using ran_policy_condition_params_list_l_ = dyn_array<policy_condition_ran_param_item_s>;

  // member variables
  bool                                 ext                  = false;
  uint32_t                             ric_policy_action_id = 1;
  printable_string<1, 150, true, true> ric_policy_action_name;
  int64_t                              ric_action_definition_format_type = 0;
  ran_policy_action_params_list_l_     ran_policy_action_params_list;
  ran_policy_condition_params_list_l_  ran_policy_condition_params_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Report-RANParameter-Item ::= SEQUENCE
struct report_ran_param_item_s {
  bool                                 ext          = false;
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// L2Parameters-RANParameter-Item ::= SEQUENCE
struct l2_params_ran_param_item_s {
  bool                                 ext          = false;
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANFunctionDefinition-Control-Item ::= SEQUENCE
struct ran_function_definition_ctrl_item_s {
  using ric_ctrl_action_list_l_         = dyn_array<ran_function_definition_ctrl_action_item_s>;
  using ran_ctrl_outcome_params_list_l_ = dyn_array<ctrl_outcome_ran_param_item_s>;

  // member variables
  bool                                 ext                                     = false;
  bool                                 ric_call_process_id_format_type_present = false;
  int64_t                              ric_ctrl_style_type                     = 0;
  printable_string<1, 150, true, true> ric_ctrl_style_name;
  ric_ctrl_action_list_l_              ric_ctrl_action_list;
  int64_t                              ric_ctrl_hdr_format_type        = 0;
  int64_t                              ric_ctrl_msg_format_type        = 0;
  int64_t                              ric_call_process_id_format_type = 0;
  int64_t                              ric_ctrl_outcome_format_type    = 0;
  ran_ctrl_outcome_params_list_l_      ran_ctrl_outcome_params_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANFunctionDefinition-EventTrigger-CallProcess-Item ::= SEQUENCE
struct ran_function_definition_event_trigger_call_process_item_s {
  using call_process_breakpoints_list_l_ = dyn_array<ran_function_definition_event_trigger_breakpoint_item_s>;

  // member variables
  bool                                 ext                  = false;
  uint32_t                             call_process_type_id = 1;
  printable_string<1, 150, true, true> call_process_type_name;
  call_process_breakpoints_list_l_     call_process_breakpoints_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANFunctionDefinition-EventTrigger-Style-Item ::= SEQUENCE
struct ran_function_definition_event_trigger_style_item_s {
  bool                                 ext                          = false;
  int64_t                              ric_event_trigger_style_type = 0;
  printable_string<1, 150, true, true> ric_event_trigger_style_name;
  int64_t                              ric_event_trigger_format_type = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANFunctionDefinition-Insert-Item ::= SEQUENCE
struct ran_function_definition_insert_item_s {
  using ric_insert_ind_list_l_ = dyn_array<ran_function_definition_insert_ind_item_s>;

  // member variables
  bool                                 ext                   = false;
  int64_t                              ric_insert_style_type = 0;
  printable_string<1, 150, true, true> ric_insert_style_name;
  int64_t                              ric_supported_event_trigger_style_type = 0;
  int64_t                              ric_action_definition_format_type      = 0;
  ric_insert_ind_list_l_               ric_insert_ind_list;
  int64_t                              ric_ind_hdr_format_type         = 0;
  int64_t                              ric_ind_msg_format_type         = 0;
  int64_t                              ric_call_process_id_format_type = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANFunctionDefinition-Policy-Item ::= SEQUENCE
struct ran_function_definition_policy_item_s {
  using ric_policy_action_list_l_ = dyn_array<ran_function_definition_policy_action_item_s>;

  // member variables
  bool                                 ext                   = false;
  int64_t                              ric_policy_style_type = 0;
  printable_string<1, 150, true, true> ric_policy_style_name;
  int64_t                              ric_supported_event_trigger_style_type = 0;
  ric_policy_action_list_l_            ric_policy_action_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANFunctionDefinition-Query-Item ::= SEQUENCE
struct ran_function_definition_query_item_s {
  using ran_query_params_list_l_ = dyn_array<query_ran_param_item_s>;

  // member variables
  bool                                 ext                  = false;
  int64_t                              ric_query_style_type = 0;
  printable_string<1, 150, true, true> ric_query_style_name;
  int64_t                              ric_query_hdr_format_type        = 0;
  int64_t                              ric_query_definition_format_type = 0;
  int64_t                              ric_query_outcome_format_type    = 0;
  ran_query_params_list_l_             ran_query_params_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANFunctionDefinition-Report-Item ::= SEQUENCE
struct ran_function_definition_report_item_s {
  using ran_report_params_list_l_ = dyn_array<report_ran_param_item_s>;

  // member variables
  bool                                 ext                   = false;
  int64_t                              ric_report_style_type = 0;
  printable_string<1, 150, true, true> ric_report_style_name;
  int64_t                              ric_supported_event_trigger_style_type = 0;
  int64_t                              ric_report_action_format_type          = 0;
  int64_t                              ric_ind_hdr_format_type                = 0;
  int64_t                              ric_ind_msg_format_type                = 0;
  ran_report_params_list_l_            ran_report_params_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEIdentification-RANParameter-Item ::= SEQUENCE
struct ue_identif_ran_param_item_s {
  bool                                 ext          = false;
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANFunctionDefinition-Control ::= SEQUENCE
struct ran_function_definition_ctrl_s {
  using ric_ctrl_style_list_l_ = dyn_array<ran_function_definition_ctrl_item_s>;

  // member variables
  bool                   ext = false;
  ric_ctrl_style_list_l_ ric_ctrl_style_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANFunctionDefinition-EventTrigger ::= SEQUENCE
struct ran_function_definition_event_trigger_s {
  using ric_event_trigger_style_list_l_ = dyn_array<ran_function_definition_event_trigger_style_item_s>;
  using ran_l2_params_list_l_           = dyn_array<l2_params_ran_param_item_s>;
  using ran_call_process_types_list_l_  = dyn_array<ran_function_definition_event_trigger_call_process_item_s>;
  using ran_ue_identif_params_list_l_   = dyn_array<ue_identif_ran_param_item_s>;
  using ran_cell_identif_params_list_l_ = dyn_array<cell_identif_ran_param_item_s>;

  // member variables
  bool                            ext = false;
  ric_event_trigger_style_list_l_ ric_event_trigger_style_list;
  ran_l2_params_list_l_           ran_l2_params_list;
  ran_call_process_types_list_l_  ran_call_process_types_list;
  ran_ue_identif_params_list_l_   ran_ue_identif_params_list;
  ran_cell_identif_params_list_l_ ran_cell_identif_params_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANFunctionDefinition-Insert ::= SEQUENCE
struct ran_function_definition_insert_s {
  using ric_insert_style_list_l_ = dyn_array<ran_function_definition_insert_item_s>;

  // member variables
  bool                     ext = false;
  ric_insert_style_list_l_ ric_insert_style_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANFunctionDefinition-Policy ::= SEQUENCE
struct ran_function_definition_policy_s {
  using ric_policy_style_list_l_ = dyn_array<ran_function_definition_policy_item_s>;

  // member variables
  bool                     ext = false;
  ric_policy_style_list_l_ ric_policy_style_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANFunctionDefinition-Query ::= SEQUENCE
struct ran_function_definition_query_s {
  using ric_query_style_list_l_ = dyn_array<ran_function_definition_query_item_s>;

  // member variables
  bool                    ext = false;
  ric_query_style_list_l_ ric_query_style_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANFunctionDefinition-Report ::= SEQUENCE
struct ran_function_definition_report_s {
  using ric_report_style_list_l_ = dyn_array<ran_function_definition_report_item_s>;

  // member variables
  bool                     ext = false;
  ric_report_style_list_l_ ric_report_style_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANfunction-Name ::= SEQUENCE
struct ra_nfunction_name_s {
  bool                                  ext                           = false;
  bool                                  ran_function_instance_present = false;
  printable_string<1, 150, true, true>  ran_function_short_name;
  printable_string<1, 1000, true, true> ran_function_e2_sm_o_id;
  printable_string<1, 150, true, true>  ran_function_description;
  int64_t                               ran_function_instance = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-RC-RANFunctionDefinition ::= SEQUENCE
struct e2_sm_rc_ran_function_definition_s {
  bool                                    ext                                           = false;
  bool                                    ran_function_definition_event_trigger_present = false;
  bool                                    ran_function_definition_report_present        = false;
  bool                                    ran_function_definition_insert_present        = false;
  bool                                    ran_function_definition_ctrl_present          = false;
  bool                                    ran_function_definition_policy_present        = false;
  ra_nfunction_name_s                     ran_function_name;
  ran_function_definition_event_trigger_s ran_function_definition_event_trigger;
  ran_function_definition_report_s        ran_function_definition_report;
  ran_function_definition_insert_s        ran_function_definition_insert;
  ran_function_definition_ctrl_s          ran_function_definition_ctrl;
  ran_function_definition_policy_s        ran_function_definition_policy;
  // ...
  copy_ptr<ran_function_definition_query_s> ran_function_definition_query;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GroupID ::= CHOICE
struct group_id_c {
  struct types_opts {
    enum options { five_gc, epc, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  group_id_c() = default;
  group_id_c(const group_id_c& other);
  group_id_c& operator=(const group_id_c& other);
  ~group_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint16_t& five_gc()
  {
    assert_choice_type(types::five_gc, type_, "GroupID");
    return c.get<uint16_t>();
  }
  uint16_t& epc()
  {
    assert_choice_type(types::epc, type_, "GroupID");
    return c.get<uint16_t>();
  }
  const uint16_t& five_gc() const
  {
    assert_choice_type(types::five_gc, type_, "GroupID");
    return c.get<uint16_t>();
  }
  const uint16_t& epc() const
  {
    assert_choice_type(types::epc, type_, "GroupID");
    return c.get<uint16_t>();
  }
  uint16_t& set_five_gc();
  uint16_t& set_epc();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// QoSID ::= CHOICE
struct qos_id_c {
  struct types_opts {
    enum options { five_gc, epc, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  qos_id_c() = default;
  qos_id_c(const qos_id_c& other);
  qos_id_c& operator=(const qos_id_c& other);
  ~qos_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint16_t& five_gc()
  {
    assert_choice_type(types::five_gc, type_, "QoSID");
    return c.get<uint16_t>();
  }
  uint16_t& epc()
  {
    assert_choice_type(types::epc, type_, "QoSID");
    return c.get<uint16_t>();
  }
  const uint16_t& five_gc() const
  {
    assert_choice_type(types::five_gc, type_, "QoSID");
    return c.get<uint16_t>();
  }
  const uint16_t& epc() const
  {
    assert_choice_type(types::epc, type_, "QoSID");
    return c.get<uint16_t>();
  }
  uint16_t& set_five_gc();
  uint16_t& set_epc();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// RANParameter-STRUCTURE-Item ::= SEQUENCE
struct ran_param_structure_item_s {
  bool                   ext          = false;
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANParameter-STRUCTURE ::= SEQUENCE
struct ran_param_structure_s {
  using seq_of_ran_params_l_ = dyn_array<ran_param_structure_item_s>;

  // member variables
  bool                 ext = false;
  seq_of_ran_params_l_ seq_of_ran_params;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANParameter-LIST ::= SEQUENCE
struct ran_param_list_s {
  using list_of_ran_param_l_ = dyn_array<ran_param_structure_s>;

  // member variables
  bool                 ext = false;
  list_of_ran_param_l_ list_of_ran_param;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANParameter-Testing-LIST ::= SEQUENCE (SIZE (1..65535)) OF RANParameter-Testing-Item
using ran_param_testing_list_l = dyn_array<ran_param_testing_item_s>;

// RANParameter-Testing-Item-Choice-List ::= SEQUENCE
struct ran_param_testing_item_choice_list_s {
  bool                     ext = false;
  ran_param_testing_list_l ran_param_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANParameter-Testing-STRUCTURE ::= SEQUENCE (SIZE (1..65535)) OF RANParameter-Testing-Item
using ran_param_testing_structure_l = dyn_array<ran_param_testing_item_s>;

// RANParameter-ValueType-Choice-List ::= SEQUENCE
struct ran_param_value_type_choice_list_s {
  bool             ext = false;
  ran_param_list_s ran_param_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANParameter-ValueType-Choice-Structure ::= SEQUENCE
struct ran_param_value_type_choice_structure_s {
  bool                  ext = false;
  ran_param_structure_s ran_param_structure;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// S-NSSAI ::= SEQUENCE
struct s_nssai_s {
  bool                     ext        = false;
  bool                     sd_present = false;
  fixed_octstring<1, true> sst;
  fixed_octstring<3, true> sd;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace e2sm_rc
} // namespace asn1
