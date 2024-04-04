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
 *                       3GPP TS ASN1 E2SM v05.00 (2024)
 *
 ******************************************************************************/

#pragma once

#include "srsran/asn1/asn1_ap_utils.h"

namespace asn1 {
namespace e2sm {

/*******************************************************************************
 *                             Constant Definitions
 ******************************************************************************/

#define ASN1_E2SM_MAXNOOF_MSGS 65535
#define ASN1_E2SM_MAXNOOF_E2_INFO_CHANGES 65535
#define ASN1_E2SM_MAXNOOF_UE_INFO_CHANGES 65535
#define ASN1_E2SM_MAXNOOF_RRC_STATE 8
#define ASN1_E2SM_MAXNOOF_PARAMS_TO_REPORT 65535
#define ASN1_E2SM_MAXNOOF_POLICY_CONDITIONS 65535
#define ASN1_E2SM_MAXNOOF_ASSOCIATED_RAN_PARAMS 65535
#define ASN1_E2SM_MAXNOOF_UE_ID 65535
#define ASN1_E2SM_MAXNOOF_CELL_ID 65535
#define ASN1_E2SM_MAXNOOF_RAN_OUTCOME_PARAMS 255
#define ASN1_E2SM_MAXNOOF_PARAMSIN_STRUCTURE 65535
#define ASN1_E2SM_MAXNOOF_ITEMSIN_LIST 65535
#define ASN1_E2SM_MAXNOOF_UE_INFO 65535
#define ASN1_E2SM_MAXNOOF_CELL_INFO 65535
#define ASN1_E2SM_MAXNOOF_UE_EVENT_INFO 65535
#define ASN1_E2SM_MAXNOOF_RAN_PARAM_TEST 255
#define ASN1_E2SM_MAXNOOF_NEIGHBOUR_CELL 65535
#define ASN1_E2SM_MAXNOOF_RIC_STYLES 63
#define ASN1_E2SM_MAXNOOF_CALL_PROCESS_TYPES 65535
#define ASN1_E2SM_MAXNOOF_CALL_PROCESS_BREAKPOINTS 65535
#define ASN1_E2SM_MAXNOOF_INSERT_IND 65535
#define ASN1_E2SM_MAXNOOF_CTRL_ACTION 65535
#define ASN1_E2SM_MAXNOOF_POLICY_ACTION 65535
#define ASN1_E2SM_MAXNOOF_INSERT_IND_ACTIONS 63
#define ASN1_E2SM_MAXNOOF_MUL_CTRL_ACTIONS 63
#define ASN1_E2SM_MAX_GROUP_DEFINITION_ID_PARAMS 255
#define ASN1_E2SM_MAXNOOF_ASSOCIATED_ENTITY_FILTS 255
#define ASN1_E2SM_MAXNOOF_FORMAT_TYPES 63
#define ASN1_E2SM_MAX_E1AP_ID 65535
#define ASN1_E2SM_MAX_F1AP_ID 4
#define ASN1_E2SM_MAX_EARFCN 65535
#define ASN1_E2SM_MAX_NR_ARFCN 3279165
#define ASN1_E2SM_MAXNOOF_NR_CELL_BANDS 32
#define ASN1_E2SM_MAX_NROF_SS_BS_1 63
#define ASN1_E2SM_MAXNOOF_CELLS 16384
#define ASN1_E2SM_MAXNOOF_RIC_STYLES 63
#define ASN1_E2SM_MAXNOOF_MEAS_INFO 65535
#define ASN1_E2SM_MAXNOOF_LABEL_INFO 2147483647
#define ASN1_E2SM_MAXNOOF_MEAS_RECORD 65535
#define ASN1_E2SM_MAXNOOF_MEAS_VALUE 2147483647
#define ASN1_E2SM_MAXNOOF_CONDITION_INFO 32768
#define ASN1_E2SM_MAXNOOF_UE_ID 65535
#define ASN1_E2SM_MAXNOOF_CONDITION_INFO_PER_SUB 32768
#define ASN1_E2SM_MAXNOOF_UE_ID_PER_SUB 65535
#define ASN1_E2SM_MAXNOOF_UE_MEAS_REPORT 65535
#define ASN1_E2SM_MAXNOOF_BIN 65535

/*******************************************************************************
 *                             Forward Declarations
 ******************************************************************************/

struct ran_param_testing_item_choice_list_s;
struct ran_param_testing_item_choice_structure_s;
struct ran_param_testing_item_choice_elem_true_s;
struct ran_param_testing_item_choice_elem_false_s;
struct ran_param_definition_choice_list_s;
struct ran_param_definition_choice_structure_s;
struct ran_param_value_type_choice_elem_true_s;
struct ran_param_value_type_choice_elem_false_s;
struct ran_param_value_type_choice_structure_s;
struct ran_param_value_type_choice_list_s;

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

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

// Beam-ID ::= CHOICE
struct beam_id_c {
  struct types_opts {
    enum options { nr_beam_id, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  types       type() const { return types::nr_beam_id; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t&       nr_beam_id() { return c; }
  const uint8_t& nr_beam_id() const { return c; }

private:
  uint8_t c;
};

// Cell-RNTI ::= SEQUENCE
struct cell_rnti_s {
  bool     ext    = false;
  uint32_t c_rnti = 0;
  cgi_c    cell_global_id;
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
struct global_ng_ran_node_id_c {
  struct types_opts {
    enum options { gnb, ng_enb, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  global_ng_ran_node_id_c() = default;
  global_ng_ran_node_id_c(const global_ng_ran_node_id_c& other);
  global_ng_ran_node_id_c& operator=(const global_ng_ran_node_id_c& other);
  ~global_ng_ran_node_id_c() { destroy_(); }
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

// SupportedSULBandList ::= SEQUENCE (SIZE (0..32)) OF SupportedSULFreqBandItem
using supported_sul_band_list_l = dyn_array<supported_sul_freq_band_item_s>;

// UEID-GNB-CU-CP-E1AP-ID-List ::= SEQUENCE (SIZE (1..65535)) OF UEID-GNB-CU-CP-E1AP-ID-Item
using ue_id_gnb_cu_cp_e1ap_id_list_l = dyn_array<ue_id_gnb_cu_cp_e1ap_id_item_s>;

// UEID-GNB-CU-F1AP-ID-List ::= SEQUENCE (SIZE (1..4)) OF UEID-GNB-CU-CP-F1AP-ID-Item
using ue_id_gnb_cu_f1ap_id_list_l = dyn_array<ue_id_gnb_cu_cp_f1ap_id_item_s>;

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
  copy_ptr<cell_rnti_s> cell_rnti;

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
  copy_ptr<cell_rnti_s> cell_rnti;

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
  copy_ptr<cell_rnti_s> cell_rnti;

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
  copy_ptr<global_ng_ran_node_id_c> global_ng_ran_node_id;
  copy_ptr<cell_rnti_s>             cell_rnti;

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
  copy_ptr<cell_rnti_s> cell_rnti;

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
  copy_ptr<global_ng_ran_node_id_c> global_ng_ran_node_id;
  copy_ptr<cell_rnti_s>             cell_rnti;

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
  bool                    ext = false;
  global_ng_ran_node_id_c global_ng_ran_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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

// InterfaceType ::= ENUMERATED
struct interface_type_opts {
  enum options { ng, xn, f1, e1, s1, x2, w1, /*...*/ nulltype } value;

  const char* to_string() const;
};
using interface_type_e = enumerated<interface_type_opts, true>;

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

// RANfunction-Name ::= SEQUENCE
struct ran_function_name_s {
  bool                                  ext                           = false;
  bool                                  ran_function_instance_present = false;
  printable_string<1, 150, true, true>  ran_function_short_name;
  printable_string<1, 1000, true, true> ran_function_e2sm_o_id;
  printable_string<1, 150, true, true>  ran_function_description;
  int64_t                               ran_function_instance = 0;
  // ...

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

// PartialUEID ::= SEQUENCE
struct partial_ue_id_s {
  bool                     ext                           = false;
  bool                     amf_ue_ngap_id_present        = false;
  bool                     guami_present                 = false;
  bool                     gnb_cu_ue_f1ap_id_present     = false;
  bool                     gnb_cu_cp_ue_e1ap_id_present  = false;
  bool                     ran_ue_id_present             = false;
  bool                     m_ng_ran_ue_xn_ap_id_present  = false;
  bool                     global_ng_ran_node_id_present = false;
  bool                     cell_rnti_present             = false;
  bool                     ng_enb_cu_ue_w1_ap_id_present = false;
  bool                     m_enb_ue_x2ap_id_present      = false;
  bool                     m_enb_ue_x2ap_id_ext_present  = false;
  bool                     global_enb_id_present         = false;
  bool                     mme_ue_s1ap_id_present        = false;
  bool                     gummei_present                = false;
  uint64_t                 amf_ue_ngap_id                = 0;
  guami_s                  guami;
  uint64_t                 gnb_cu_ue_f1ap_id    = 0;
  uint64_t                 gnb_cu_cp_ue_e1ap_id = 0;
  fixed_octstring<8, true> ran_ue_id;
  uint64_t                 m_ng_ran_ue_xn_ap_id = 0;
  global_ng_ran_node_id_c  global_ng_ran_node_id;
  cell_rnti_s              cell_rnti;
  uint64_t                 ng_enb_cu_ue_w1_ap_id = 0;
  uint16_t                 m_enb_ue_x2ap_id      = 0;
  uint16_t                 m_enb_ue_x2ap_id_ext  = 0;
  global_enb_id_s          global_enb_id;
  uint64_t                 mme_ue_s1ap_id = 0;
  gummei_s                 gummei;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
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

} // namespace e2sm
} // namespace asn1
