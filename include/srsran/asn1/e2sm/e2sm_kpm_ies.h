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

#include "e2sm_common_ies.h"

namespace asn1 {
namespace e2sm {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// MeasValueTestCond-Expression ::= ENUMERATED
struct meas_value_test_cond_expression_opts {
  enum options { equal, greaterthan, lessthan, contains, present, /*...*/ nulltype } value;

  const char* to_string() const;
};
using meas_value_test_cond_expression_e = enumerated<meas_value_test_cond_expression_opts, true>;

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

// TestCond-Value ::= CHOICE
struct test_cond_value_c {
  struct types_opts {
    enum options {
      value_int,
      value_enum,
      value_bool,
      value_bit_s,
      value_oct_s,
      value_prt_s,
      /*...*/ value_real,
      nulltype
    } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true, 1>;

  // choice methods
  test_cond_value_c() = default;
  test_cond_value_c(const test_cond_value_c& other);
  test_cond_value_c& operator=(const test_cond_value_c& other);
  ~test_cond_value_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  int64_t& value_int()
  {
    assert_choice_type(types::value_int, type_, "TestCond-Value");
    return c.get<int64_t>();
  }
  int64_t& value_enum()
  {
    assert_choice_type(types::value_enum, type_, "TestCond-Value");
    return c.get<int64_t>();
  }
  bool& value_bool()
  {
    assert_choice_type(types::value_bool, type_, "TestCond-Value");
    return c.get<bool>();
  }
  dyn_bitstring& value_bit_s()
  {
    assert_choice_type(types::value_bit_s, type_, "TestCond-Value");
    return c.get<dyn_bitstring>();
  }
  unbounded_octstring<true>& value_oct_s()
  {
    assert_choice_type(types::value_oct_s, type_, "TestCond-Value");
    return c.get<unbounded_octstring<true>>();
  }
  printable_string<0, MAX_ASN_STRING_LENGTH, false, true>& value_prt_s()
  {
    assert_choice_type(types::value_prt_s, type_, "TestCond-Value");
    return c.get<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>>();
  }
  real_s& value_real()
  {
    assert_choice_type(types::value_real, type_, "TestCond-Value");
    return c.get<real_s>();
  }
  const int64_t& value_int() const
  {
    assert_choice_type(types::value_int, type_, "TestCond-Value");
    return c.get<int64_t>();
  }
  const int64_t& value_enum() const
  {
    assert_choice_type(types::value_enum, type_, "TestCond-Value");
    return c.get<int64_t>();
  }
  const bool& value_bool() const
  {
    assert_choice_type(types::value_bool, type_, "TestCond-Value");
    return c.get<bool>();
  }
  const dyn_bitstring& value_bit_s() const
  {
    assert_choice_type(types::value_bit_s, type_, "TestCond-Value");
    return c.get<dyn_bitstring>();
  }
  const unbounded_octstring<true>& value_oct_s() const
  {
    assert_choice_type(types::value_oct_s, type_, "TestCond-Value");
    return c.get<unbounded_octstring<true>>();
  }
  const printable_string<0, MAX_ASN_STRING_LENGTH, false, true>& value_prt_s() const
  {
    assert_choice_type(types::value_prt_s, type_, "TestCond-Value");
    return c.get<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>>();
  }
  const real_s& value_real() const
  {
    assert_choice_type(types::value_real, type_, "TestCond-Value");
    return c.get<real_s>();
  }
  int64_t&                                                 set_value_int();
  int64_t&                                                 set_value_enum();
  bool&                                                    set_value_bool();
  dyn_bitstring&                                           set_value_bit_s();
  unbounded_octstring<true>&                               set_value_oct_s();
  printable_string<0, MAX_ASN_STRING_LENGTH, false, true>& set_value_prt_s();
  real_s&                                                  set_value_real();

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

// LogicalOR ::= ENUMERATED
struct lc_or_opts {
  enum options { true_value, false_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using lc_or_e = enumerated<lc_or_opts, true>;

// MeasurementLabel ::= SEQUENCE
struct meas_label_s {
  struct no_label_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using no_label_e_ = enumerated<no_label_opts, true>;
  struct sum_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using sum_e_ = enumerated<sum_opts, true>;
  struct pre_label_override_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using pre_label_override_e_ = enumerated<pre_label_override_opts, true>;
  struct start_end_ind_opts {
    enum options { start, end, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using start_end_ind_e_ = enumerated<start_end_ind_opts, true>;
  struct min_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using min_e_ = enumerated<min_opts, true>;
  struct max_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using max_e_ = enumerated<max_opts, true>;
  struct avg_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using avg_e_ = enumerated<avg_opts, true>;

  // member variables
  bool                     ext                        = false;
  bool                     no_label_present           = false;
  bool                     plmn_id_present            = false;
  bool                     slice_id_present           = false;
  bool                     five_qi_present            = false;
  bool                     qfi_present                = false;
  bool                     qci_present                = false;
  bool                     qc_imax_present            = false;
  bool                     qc_imin_present            = false;
  bool                     ar_pmax_present            = false;
  bool                     ar_pmin_present            = false;
  bool                     bitrate_range_present      = false;
  bool                     layer_mu_mimo_present      = false;
  bool                     sum_present                = false;
  bool                     dist_bin_x_present         = false;
  bool                     dist_bin_y_present         = false;
  bool                     dist_bin_z_present         = false;
  bool                     pre_label_override_present = false;
  bool                     start_end_ind_present      = false;
  bool                     min_present                = false;
  bool                     max_present                = false;
  bool                     avg_present                = false;
  no_label_e_              no_label;
  fixed_octstring<3, true> plmn_id;
  s_nssai_s                slice_id;
  uint16_t                 five_qi       = 0;
  uint8_t                  qfi           = 0;
  uint16_t                 qci           = 0;
  uint16_t                 qc_imax       = 0;
  uint16_t                 qc_imin       = 0;
  uint8_t                  ar_pmax       = 1;
  uint8_t                  ar_pmin       = 1;
  uint32_t                 bitrate_range = 1;
  uint32_t                 layer_mu_mimo = 1;
  sum_e_                   sum;
  uint32_t                 dist_bin_x = 1;
  uint32_t                 dist_bin_y = 1;
  uint32_t                 dist_bin_z = 1;
  pre_label_override_e_    pre_label_override;
  start_end_ind_e_         start_end_ind;
  min_e_                   min;
  max_e_                   max;
  avg_e_                   avg;
  // ...
  bool                ssb_idx_present              = false;
  bool                non_go_b_b_fmode_idx_present = false;
  bool                mimo_mode_idx_present        = false;
  uint32_t            ssb_idx                      = 1;
  uint32_t            non_go_b_b_fmode_idx         = 1;
  uint8_t             mimo_mode_idx                = 1;
  copy_ptr<cgi_c>     cell_global_id;
  copy_ptr<beam_id_c> beam_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasValueReportCond ::= SEQUENCE
struct meas_value_report_cond_s {
  bool                              ext = false;
  meas_value_test_cond_expression_e test_expr;
  test_cond_value_c                 test_value;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BinRangeValue ::= CHOICE
struct bin_range_value_c {
  struct types_opts {
    enum options { value_int, value_real, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  bin_range_value_c() = default;
  bin_range_value_c(const bin_range_value_c& other);
  bin_range_value_c& operator=(const bin_range_value_c& other);
  ~bin_range_value_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  int64_t& value_int()
  {
    assert_choice_type(types::value_int, type_, "BinRangeValue");
    return c.get<int64_t>();
  }
  real_s& value_real()
  {
    assert_choice_type(types::value_real, type_, "BinRangeValue");
    return c.get<real_s>();
  }
  const int64_t& value_int() const
  {
    assert_choice_type(types::value_int, type_, "BinRangeValue");
    return c.get<int64_t>();
  }
  const real_s& value_real() const
  {
    assert_choice_type(types::value_real, type_, "BinRangeValue");
    return c.get<real_s>();
  }
  int64_t& set_value_int();
  real_s&  set_value_real();

private:
  types                   type_;
  choice_buffer_t<real_s> c;

  void destroy_();
};

// LabelInfoItem ::= SEQUENCE
struct label_info_item_s {
  bool         ext = false;
  meas_label_s meas_label;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MatchCondReportItem ::= SEQUENCE
struct match_cond_report_item_s {
  bool                     ext           = false;
  bool                     lc_or_present = false;
  meas_value_report_cond_s meas_value_report_cond;
  lc_or_e                  lc_or;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementRecordItem ::= CHOICE
struct meas_record_item_c {
  struct types_opts {
    enum options { integer, real, no_value, /*...*/ not_satisfied, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true, 1>;

  // choice methods
  meas_record_item_c() = default;
  meas_record_item_c(const meas_record_item_c& other);
  meas_record_item_c& operator=(const meas_record_item_c& other);
  ~meas_record_item_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint64_t& integer()
  {
    assert_choice_type(types::integer, type_, "MeasurementRecordItem");
    return c.get<uint64_t>();
  }
  real_s& real()
  {
    assert_choice_type(types::real, type_, "MeasurementRecordItem");
    return c.get<real_s>();
  }
  const uint64_t& integer() const
  {
    assert_choice_type(types::integer, type_, "MeasurementRecordItem");
    return c.get<uint64_t>();
  }
  const real_s& real() const
  {
    assert_choice_type(types::real, type_, "MeasurementRecordItem");
    return c.get<real_s>();
  }
  uint64_t& set_integer();
  real_s&   set_real();
  void      set_no_value();
  void      set_not_satisfied();

private:
  types                   type_;
  choice_buffer_t<real_s> c;

  void destroy_();
};

// TestCond-Expression ::= ENUMERATED
struct test_cond_expression_opts {
  enum options { equal, greaterthan, lessthan, contains, present, /*...*/ nulltype } value;

  const char* to_string() const;
};
using test_cond_expression_e = enumerated<test_cond_expression_opts, true>;

// TestCond-Type ::= CHOICE
struct test_cond_type_c {
  struct gbr_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using gbr_e_ = enumerated<gbr_opts, true>;
  struct ambr_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using ambr_e_ = enumerated<ambr_opts, true>;
  struct is_stat_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using is_stat_e_ = enumerated<is_stat_opts, true>;
  struct is_cat_m_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using is_cat_m_e_ = enumerated<is_cat_m_opts, true>;
  struct rsrp_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using rsrp_e_ = enumerated<rsrp_opts, true>;
  struct rsrq_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using rsrq_e_ = enumerated<rsrq_opts, true>;
  struct ul_r_srp_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using ul_r_srp_e_ = enumerated<ul_r_srp_opts, true>;
  struct cqi_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using cqi_e_ = enumerated<cqi_opts, true>;
  struct five_qi_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using five_qi_e_ = enumerated<five_qi_opts, true>;
  struct qci_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using qci_e_ = enumerated<qci_opts, true>;
  struct snssai_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using snssai_e_ = enumerated<snssai_opts, true>;
  struct types_opts {
    enum options {
      gbr,
      ambr,
      is_stat,
      is_cat_m,
      rsrp,
      rsrq,
      /*...*/ ul_r_srp,
      cqi,
      five_qi,
      qci,
      snssai,
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts, true, 5>;

  // choice methods
  test_cond_type_c() = default;
  test_cond_type_c(const test_cond_type_c& other);
  test_cond_type_c& operator=(const test_cond_type_c& other);
  ~test_cond_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  gbr_e_& gbr()
  {
    assert_choice_type(types::gbr, type_, "TestCond-Type");
    return c.get<gbr_e_>();
  }
  ambr_e_& ambr()
  {
    assert_choice_type(types::ambr, type_, "TestCond-Type");
    return c.get<ambr_e_>();
  }
  is_stat_e_& is_stat()
  {
    assert_choice_type(types::is_stat, type_, "TestCond-Type");
    return c.get<is_stat_e_>();
  }
  is_cat_m_e_& is_cat_m()
  {
    assert_choice_type(types::is_cat_m, type_, "TestCond-Type");
    return c.get<is_cat_m_e_>();
  }
  rsrp_e_& rsrp()
  {
    assert_choice_type(types::rsrp, type_, "TestCond-Type");
    return c.get<rsrp_e_>();
  }
  rsrq_e_& rsrq()
  {
    assert_choice_type(types::rsrq, type_, "TestCond-Type");
    return c.get<rsrq_e_>();
  }
  ul_r_srp_e_& ul_r_srp()
  {
    assert_choice_type(types::ul_r_srp, type_, "TestCond-Type");
    return c.get<ul_r_srp_e_>();
  }
  cqi_e_& cqi()
  {
    assert_choice_type(types::cqi, type_, "TestCond-Type");
    return c.get<cqi_e_>();
  }
  five_qi_e_& five_qi()
  {
    assert_choice_type(types::five_qi, type_, "TestCond-Type");
    return c.get<five_qi_e_>();
  }
  qci_e_& qci()
  {
    assert_choice_type(types::qci, type_, "TestCond-Type");
    return c.get<qci_e_>();
  }
  snssai_e_& snssai()
  {
    assert_choice_type(types::snssai, type_, "TestCond-Type");
    return c.get<snssai_e_>();
  }
  const gbr_e_& gbr() const
  {
    assert_choice_type(types::gbr, type_, "TestCond-Type");
    return c.get<gbr_e_>();
  }
  const ambr_e_& ambr() const
  {
    assert_choice_type(types::ambr, type_, "TestCond-Type");
    return c.get<ambr_e_>();
  }
  const is_stat_e_& is_stat() const
  {
    assert_choice_type(types::is_stat, type_, "TestCond-Type");
    return c.get<is_stat_e_>();
  }
  const is_cat_m_e_& is_cat_m() const
  {
    assert_choice_type(types::is_cat_m, type_, "TestCond-Type");
    return c.get<is_cat_m_e_>();
  }
  const rsrp_e_& rsrp() const
  {
    assert_choice_type(types::rsrp, type_, "TestCond-Type");
    return c.get<rsrp_e_>();
  }
  const rsrq_e_& rsrq() const
  {
    assert_choice_type(types::rsrq, type_, "TestCond-Type");
    return c.get<rsrq_e_>();
  }
  const ul_r_srp_e_& ul_r_srp() const
  {
    assert_choice_type(types::ul_r_srp, type_, "TestCond-Type");
    return c.get<ul_r_srp_e_>();
  }
  const cqi_e_& cqi() const
  {
    assert_choice_type(types::cqi, type_, "TestCond-Type");
    return c.get<cqi_e_>();
  }
  const five_qi_e_& five_qi() const
  {
    assert_choice_type(types::five_qi, type_, "TestCond-Type");
    return c.get<five_qi_e_>();
  }
  const qci_e_& qci() const
  {
    assert_choice_type(types::qci, type_, "TestCond-Type");
    return c.get<qci_e_>();
  }
  const snssai_e_& snssai() const
  {
    assert_choice_type(types::snssai, type_, "TestCond-Type");
    return c.get<snssai_e_>();
  }
  gbr_e_&      set_gbr();
  ambr_e_&     set_ambr();
  is_stat_e_&  set_is_stat();
  is_cat_m_e_& set_is_cat_m();
  rsrp_e_&     set_rsrp();
  rsrq_e_&     set_rsrq();
  ul_r_srp_e_& set_ul_r_srp();
  cqi_e_&      set_cqi();
  five_qi_e_&  set_five_qi();
  qci_e_&      set_qci();
  snssai_e_&   set_snssai();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// BinRangeItem ::= SEQUENCE
struct bin_range_item_s {
  bool              ext     = false;
  uint32_t          bin_idx = 1;
  bin_range_value_c start_value;
  bin_range_value_c end_value;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LabelInfoList ::= SEQUENCE (SIZE (1..2147483647)) OF LabelInfoItem
using label_info_list_l = dyn_array<label_info_item_s>;

// MatchCondReportList ::= SEQUENCE (SIZE (1..32768)) OF MatchCondReportItem
using match_cond_report_list_l = dyn_array<match_cond_report_item_s>;

// MatchingUEidItem-PerGP ::= SEQUENCE
struct matching_ue_id_item_per_gp_s {
  bool    ext = false;
  ue_id_c ue_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementRecord ::= SEQUENCE (SIZE (1..2147483647)) OF MeasurementRecordItem
using meas_record_l = dyn_array<meas_record_item_c>;

// MeasurementType ::= CHOICE
struct meas_type_c {
  struct types_opts {
    enum options { meas_name, meas_id, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  meas_type_c() = default;
  meas_type_c(const meas_type_c& other);
  meas_type_c& operator=(const meas_type_c& other);
  ~meas_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  printable_string<1, 150, true, true>& meas_name()
  {
    assert_choice_type(types::meas_name, type_, "MeasurementType");
    return c.get<printable_string<1, 150, true, true>>();
  }
  uint32_t& meas_id()
  {
    assert_choice_type(types::meas_id, type_, "MeasurementType");
    return c.get<uint32_t>();
  }
  const printable_string<1, 150, true, true>& meas_name() const
  {
    assert_choice_type(types::meas_name, type_, "MeasurementType");
    return c.get<printable_string<1, 150, true, true>>();
  }
  const uint32_t& meas_id() const
  {
    assert_choice_type(types::meas_id, type_, "MeasurementType");
    return c.get<uint32_t>();
  }
  printable_string<1, 150, true, true>& set_meas_name();
  uint32_t&                             set_meas_id();

private:
  types                                                 type_;
  choice_buffer_t<printable_string<1, 150, true, true>> c;

  void destroy_();
};

// TestCondInfo ::= SEQUENCE
struct test_cond_info_s {
  bool                   ext                = false;
  bool                   test_expr_present  = false;
  bool                   test_value_present = false;
  test_cond_type_c       test_type;
  test_cond_expression_e test_expr;
  test_cond_value_c      test_value;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BinRangeList ::= SEQUENCE (SIZE (1..65535)) OF BinRangeItem
using bin_range_list_l = dyn_array<bin_range_item_s>;

// MatchingCondItem-Choice ::= CHOICE
struct matching_cond_item_choice_c {
  struct types_opts {
    enum options { meas_label, test_cond_info, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  matching_cond_item_choice_c() = default;
  matching_cond_item_choice_c(const matching_cond_item_choice_c& other);
  matching_cond_item_choice_c& operator=(const matching_cond_item_choice_c& other);
  ~matching_cond_item_choice_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  meas_label_s& meas_label()
  {
    assert_choice_type(types::meas_label, type_, "MatchingCondItem-Choice");
    return c.get<meas_label_s>();
  }
  test_cond_info_s& test_cond_info()
  {
    assert_choice_type(types::test_cond_info, type_, "MatchingCondItem-Choice");
    return c.get<test_cond_info_s>();
  }
  const meas_label_s& meas_label() const
  {
    assert_choice_type(types::meas_label, type_, "MatchingCondItem-Choice");
    return c.get<meas_label_s>();
  }
  const test_cond_info_s& test_cond_info() const
  {
    assert_choice_type(types::test_cond_info, type_, "MatchingCondItem-Choice");
    return c.get<test_cond_info_s>();
  }
  meas_label_s&     set_meas_label();
  test_cond_info_s& set_test_cond_info();

private:
  types                                           type_;
  choice_buffer_t<meas_label_s, test_cond_info_s> c;

  void destroy_();
};

// MatchingUEidList-PerGP ::= SEQUENCE (SIZE (1..65535)) OF MatchingUEidItem-PerGP
using matching_ue_id_list_per_gp_l = dyn_array<matching_ue_id_item_per_gp_s>;

// MeasurementDataItem ::= SEQUENCE
struct meas_data_item_s {
  struct incomplete_flag_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using incomplete_flag_e_ = enumerated<incomplete_flag_opts, true>;

  // member variables
  bool               ext                     = false;
  bool               incomplete_flag_present = false;
  meas_record_l      meas_record;
  incomplete_flag_e_ incomplete_flag;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementInfoItem ::= SEQUENCE
struct meas_info_item_s {
  bool              ext = false;
  meas_type_c       meas_type;
  label_info_list_l label_info_list;
  // ...
  copy_ptr<match_cond_report_list_l> match_cond_report_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BinRangeDefinition ::= SEQUENCE
struct bin_range_definition_s {
  bool             ext = false;
  bin_range_list_l bin_range_list_x;
  bin_range_list_l bin_range_list_y;
  bin_range_list_l bin_range_list_z;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MatchingCondItem ::= SEQUENCE
struct matching_cond_item_s {
  bool                        ext           = false;
  bool                        lc_or_present = false;
  matching_cond_item_choice_c matching_cond_choice;
  lc_or_e                     lc_or;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MatchingUEidItem ::= SEQUENCE
struct matching_ue_id_item_s {
  bool    ext = false;
  ue_id_c ue_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MatchingUEidPerGP-Item ::= SEQUENCE
struct matching_ue_id_per_gp_item_s {
  struct matched_per_gp_c_ {
    struct no_ue_matched_opts {
      enum options { true_value, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    using no_ue_matched_e_ = enumerated<no_ue_matched_opts, true>;
    struct types_opts {
      enum options { no_ue_matched, one_or_more_ue_matched, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    matched_per_gp_c_() = default;
    matched_per_gp_c_(const matched_per_gp_c_& other);
    matched_per_gp_c_& operator=(const matched_per_gp_c_& other);
    ~matched_per_gp_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    no_ue_matched_e_& no_ue_matched()
    {
      assert_choice_type(types::no_ue_matched, type_, "matchedPerGP");
      return c.get<no_ue_matched_e_>();
    }
    matching_ue_id_list_per_gp_l& one_or_more_ue_matched()
    {
      assert_choice_type(types::one_or_more_ue_matched, type_, "matchedPerGP");
      return c.get<matching_ue_id_list_per_gp_l>();
    }
    const no_ue_matched_e_& no_ue_matched() const
    {
      assert_choice_type(types::no_ue_matched, type_, "matchedPerGP");
      return c.get<no_ue_matched_e_>();
    }
    const matching_ue_id_list_per_gp_l& one_or_more_ue_matched() const
    {
      assert_choice_type(types::one_or_more_ue_matched, type_, "matchedPerGP");
      return c.get<matching_ue_id_list_per_gp_l>();
    }
    no_ue_matched_e_&             set_no_ue_matched();
    matching_ue_id_list_per_gp_l& set_one_or_more_ue_matched();

  private:
    types                                         type_;
    choice_buffer_t<matching_ue_id_list_per_gp_l> c;

    void destroy_();
  };

  // member variables
  bool              ext = false;
  matched_per_gp_c_ matched_per_gp;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementData ::= SEQUENCE (SIZE (1..65535)) OF MeasurementDataItem
using meas_data_l = dyn_array<meas_data_item_s>;

// MeasurementInfoList ::= SEQUENCE (SIZE (1..65535)) OF MeasurementInfoItem
using meas_info_list_l = dyn_array<meas_info_item_s>;

// DistMeasurementBinRangeItem ::= SEQUENCE
struct dist_meas_bin_range_item_s {
  bool                   ext = false;
  meas_type_c            meas_type;
  bin_range_definition_s bin_range_def;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-IndicationMessage-Format1 ::= SEQUENCE
struct e2sm_kpm_ind_msg_format1_s {
  bool             ext                   = false;
  bool             granul_period_present = false;
  meas_data_l      meas_data;
  meas_info_list_l meas_info_list;
  uint64_t         granul_period = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MatchingCondList ::= SEQUENCE (SIZE (1..32768)) OF MatchingCondItem
using matching_cond_list_l = dyn_array<matching_cond_item_s>;

// MatchingUEidList ::= SEQUENCE (SIZE (1..65535)) OF MatchingUEidItem
using matching_ue_id_list_l = dyn_array<matching_ue_id_item_s>;

// MatchingUEidPerGP ::= SEQUENCE (SIZE (1..65535)) OF MatchingUEidPerGP-Item
using matching_ue_id_per_gp_l = dyn_array<matching_ue_id_per_gp_item_s>;

// DistMeasurementBinRangeList ::= SEQUENCE (SIZE (1..65535)) OF DistMeasurementBinRangeItem
using dist_meas_bin_range_list_l = dyn_array<dist_meas_bin_range_item_s>;

// MatchingUeCondPerSubItem ::= SEQUENCE
struct matching_ue_cond_per_sub_item_s {
  bool             ext = false;
  test_cond_info_s test_cond_info;
  // ...
  bool    lc_or_present = false;
  lc_or_e lc_or;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MatchingUEidPerSubItem ::= SEQUENCE
struct matching_ue_id_per_sub_item_s {
  bool    ext = false;
  ue_id_c ue_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementCondItem ::= SEQUENCE
struct meas_cond_item_s {
  bool                 ext = false;
  meas_type_c          meas_type;
  matching_cond_list_l matching_cond;
  // ...
  copy_ptr<bin_range_definition_s> bin_range_def;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementCondUEidItem ::= SEQUENCE
struct meas_cond_ue_id_item_s {
  bool                  ext = false;
  meas_type_c           meas_type;
  matching_cond_list_l  matching_cond;
  matching_ue_id_list_l matching_ue_id_list;
  // ...
  copy_ptr<matching_ue_id_per_gp_l> matching_ue_id_per_gp;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementInfo-Action-Item ::= SEQUENCE
struct meas_info_action_item_s {
  bool                                 ext             = false;
  bool                                 meas_id_present = false;
  printable_string<1, 150, true, true> meas_name;
  uint32_t                             meas_id = 1;
  // ...
  copy_ptr<bin_range_definition_s> bin_range_def;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEMeasurementReportItem ::= SEQUENCE
struct ue_meas_report_item_s {
  bool                       ext = false;
  ue_id_c                    ue_id;
  e2sm_kpm_ind_msg_format1_s meas_report;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-ActionDefinition-Format1 ::= SEQUENCE
struct e2sm_kpm_action_definition_format1_s {
  bool             ext                    = false;
  bool             cell_global_id_present = false;
  meas_info_list_l meas_info_list;
  uint64_t         granul_period = 1;
  cgi_c            cell_global_id;
  // ...
  copy_ptr<dist_meas_bin_range_list_l> dist_meas_bin_range_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MatchingUeCondPerSubList ::= SEQUENCE (SIZE (1..32768)) OF MatchingUeCondPerSubItem
using matching_ue_cond_per_sub_list_l = dyn_array<matching_ue_cond_per_sub_item_s>;

// MatchingUEidPerSubList ::= SEQUENCE (SIZE (2..65535)) OF MatchingUEidPerSubItem
using matching_ue_id_per_sub_list_l = dyn_array<matching_ue_id_per_sub_item_s>;

// MeasurementCondList ::= SEQUENCE (SIZE (1..65535)) OF MeasurementCondItem
using meas_cond_list_l = dyn_array<meas_cond_item_s>;

// MeasurementCondUEidList ::= SEQUENCE (SIZE (1..65535)) OF MeasurementCondUEidItem
using meas_cond_ue_id_list_l = dyn_array<meas_cond_ue_id_item_s>;

// MeasurementInfo-Action-List ::= SEQUENCE (SIZE (1..65535)) OF MeasurementInfo-Action-Item
using meas_info_action_list_l = dyn_array<meas_info_action_item_s>;

// UEMeasurementReportList ::= SEQUENCE (SIZE (1..65535)) OF UEMeasurementReportItem
using ue_meas_report_list_l = dyn_array<ue_meas_report_item_s>;

// E2SM-KPM-ActionDefinition-Format2 ::= SEQUENCE
struct e2sm_kpm_action_definition_format2_s {
  bool                                 ext = false;
  ue_id_c                              ue_id;
  e2sm_kpm_action_definition_format1_s subscript_info;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-ActionDefinition-Format3 ::= SEQUENCE
struct e2sm_kpm_action_definition_format3_s {
  bool             ext                    = false;
  bool             cell_global_id_present = false;
  meas_cond_list_l meas_cond_list;
  uint64_t         granul_period = 1;
  cgi_c            cell_global_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-ActionDefinition-Format4 ::= SEQUENCE
struct e2sm_kpm_action_definition_format4_s {
  bool                                 ext = false;
  matching_ue_cond_per_sub_list_l      matching_ue_cond_list;
  e2sm_kpm_action_definition_format1_s sub_info;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-ActionDefinition-Format5 ::= SEQUENCE
struct e2sm_kpm_action_definition_format5_s {
  bool                                 ext = false;
  matching_ue_id_per_sub_list_l        matching_ue_id_list;
  e2sm_kpm_action_definition_format1_s sub_info;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-EventTriggerDefinition-Format1 ::= SEQUENCE
struct e2sm_kpm_event_trigger_definition_format1_s {
  bool     ext           = false;
  uint64_t report_period = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-IndicationHeader-Format1 ::= SEQUENCE
struct e2sm_kpm_ind_hdr_format1_s {
  bool                                  ext                        = false;
  bool                                  file_formatversion_present = false;
  bool                                  sender_name_present        = false;
  bool                                  sender_type_present        = false;
  bool                                  vendor_name_present        = false;
  fixed_octstring<8, true>              collet_start_time;
  printable_string<0, 15, false, true>  file_formatversion;
  printable_string<0, 400, false, true> sender_name;
  printable_string<0, 8, false, true>   sender_type;
  printable_string<0, 32, false, true>  vendor_name;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-IndicationMessage-Format2 ::= SEQUENCE
struct e2sm_kpm_ind_msg_format2_s {
  bool                   ext                   = false;
  bool                   granul_period_present = false;
  meas_data_l            meas_data;
  meas_cond_ue_id_list_l meas_cond_ue_id_list;
  uint64_t               granul_period = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-IndicationMessage-Format3 ::= SEQUENCE
struct e2sm_kpm_ind_msg_format3_s {
  bool                  ext = false;
  ue_meas_report_list_l ue_meas_report_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RIC-EventTriggerStyle-Item ::= SEQUENCE
struct ric_event_trigger_style_item_s {
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

// RIC-ReportStyle-Item ::= SEQUENCE
struct ric_report_style_item_s {
  bool                                 ext                   = false;
  int64_t                              ric_report_style_type = 0;
  printable_string<1, 150, true, true> ric_report_style_name;
  int64_t                              ric_action_format_type = 0;
  meas_info_action_list_l              meas_info_action_list;
  int64_t                              ric_ind_hdr_format_type = 0;
  int64_t                              ric_ind_msg_format_type = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-ActionDefinition ::= SEQUENCE
struct e2sm_kpm_action_definition_s {
  struct action_definition_formats_c_ {
    struct types_opts {
      enum options {
        action_definition_format1,
        action_definition_format2,
        action_definition_format3,
        // ...
        action_definition_format4,
        action_definition_format5,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true, 2>;

    // choice methods
    action_definition_formats_c_() = default;
    action_definition_formats_c_(const action_definition_formats_c_& other);
    action_definition_formats_c_& operator=(const action_definition_formats_c_& other);
    ~action_definition_formats_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2sm_kpm_action_definition_format1_s& action_definition_format1()
    {
      assert_choice_type(types::action_definition_format1, type_, "actionDefinition-formats");
      return c.get<e2sm_kpm_action_definition_format1_s>();
    }
    e2sm_kpm_action_definition_format2_s& action_definition_format2()
    {
      assert_choice_type(types::action_definition_format2, type_, "actionDefinition-formats");
      return c.get<e2sm_kpm_action_definition_format2_s>();
    }
    e2sm_kpm_action_definition_format3_s& action_definition_format3()
    {
      assert_choice_type(types::action_definition_format3, type_, "actionDefinition-formats");
      return c.get<e2sm_kpm_action_definition_format3_s>();
    }
    e2sm_kpm_action_definition_format4_s& action_definition_format4()
    {
      assert_choice_type(types::action_definition_format4, type_, "actionDefinition-formats");
      return c.get<e2sm_kpm_action_definition_format4_s>();
    }
    e2sm_kpm_action_definition_format5_s& action_definition_format5()
    {
      assert_choice_type(types::action_definition_format5, type_, "actionDefinition-formats");
      return c.get<e2sm_kpm_action_definition_format5_s>();
    }
    const e2sm_kpm_action_definition_format1_s& action_definition_format1() const
    {
      assert_choice_type(types::action_definition_format1, type_, "actionDefinition-formats");
      return c.get<e2sm_kpm_action_definition_format1_s>();
    }
    const e2sm_kpm_action_definition_format2_s& action_definition_format2() const
    {
      assert_choice_type(types::action_definition_format2, type_, "actionDefinition-formats");
      return c.get<e2sm_kpm_action_definition_format2_s>();
    }
    const e2sm_kpm_action_definition_format3_s& action_definition_format3() const
    {
      assert_choice_type(types::action_definition_format3, type_, "actionDefinition-formats");
      return c.get<e2sm_kpm_action_definition_format3_s>();
    }
    const e2sm_kpm_action_definition_format4_s& action_definition_format4() const
    {
      assert_choice_type(types::action_definition_format4, type_, "actionDefinition-formats");
      return c.get<e2sm_kpm_action_definition_format4_s>();
    }
    const e2sm_kpm_action_definition_format5_s& action_definition_format5() const
    {
      assert_choice_type(types::action_definition_format5, type_, "actionDefinition-formats");
      return c.get<e2sm_kpm_action_definition_format5_s>();
    }
    e2sm_kpm_action_definition_format1_s& set_action_definition_format1();
    e2sm_kpm_action_definition_format2_s& set_action_definition_format2();
    e2sm_kpm_action_definition_format3_s& set_action_definition_format3();
    e2sm_kpm_action_definition_format4_s& set_action_definition_format4();
    e2sm_kpm_action_definition_format5_s& set_action_definition_format5();

  private:
    types type_;
    choice_buffer_t<e2sm_kpm_action_definition_format1_s,
                    e2sm_kpm_action_definition_format2_s,
                    e2sm_kpm_action_definition_format3_s,
                    e2sm_kpm_action_definition_format4_s,
                    e2sm_kpm_action_definition_format5_s>
        c;

    void destroy_();
  };

  // member variables
  bool                         ext            = false;
  int64_t                      ric_style_type = 0;
  action_definition_formats_c_ action_definition_formats;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-EventTriggerDefinition ::= SEQUENCE
struct e2sm_kpm_event_trigger_definition_s {
  struct event_definition_formats_c_ {
    struct types_opts {
      enum options { event_definition_format1, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    types       type() const { return types::event_definition_format1; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2sm_kpm_event_trigger_definition_format1_s&       event_definition_format1() { return c; }
    const e2sm_kpm_event_trigger_definition_format1_s& event_definition_format1() const { return c; }

  private:
    e2sm_kpm_event_trigger_definition_format1_s c;
  };

  // member variables
  bool                        ext = false;
  event_definition_formats_c_ event_definition_formats;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-IndicationHeader ::= SEQUENCE
struct e2sm_kpm_ind_hdr_s {
  struct ind_hdr_formats_c_ {
    struct types_opts {
      enum options { ind_hdr_format1, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    types       type() const { return types::ind_hdr_format1; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2sm_kpm_ind_hdr_format1_s&       ind_hdr_format1() { return c; }
    const e2sm_kpm_ind_hdr_format1_s& ind_hdr_format1() const { return c; }

  private:
    e2sm_kpm_ind_hdr_format1_s c;
  };

  // member variables
  bool               ext = false;
  ind_hdr_formats_c_ ind_hdr_formats;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-IndicationMessage ::= SEQUENCE
struct e2sm_kpm_ind_msg_s {
  struct ind_msg_formats_c_ {
    struct types_opts {
      enum options { ind_msg_format1, ind_msg_format2, /*...*/ ind_msg_format3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true, 1>;

    // choice methods
    ind_msg_formats_c_() = default;
    ind_msg_formats_c_(const ind_msg_formats_c_& other);
    ind_msg_formats_c_& operator=(const ind_msg_formats_c_& other);
    ~ind_msg_formats_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2sm_kpm_ind_msg_format1_s& ind_msg_format1()
    {
      assert_choice_type(types::ind_msg_format1, type_, "indicationMessage-formats");
      return c.get<e2sm_kpm_ind_msg_format1_s>();
    }
    e2sm_kpm_ind_msg_format2_s& ind_msg_format2()
    {
      assert_choice_type(types::ind_msg_format2, type_, "indicationMessage-formats");
      return c.get<e2sm_kpm_ind_msg_format2_s>();
    }
    e2sm_kpm_ind_msg_format3_s& ind_msg_format3()
    {
      assert_choice_type(types::ind_msg_format3, type_, "indicationMessage-formats");
      return c.get<e2sm_kpm_ind_msg_format3_s>();
    }
    const e2sm_kpm_ind_msg_format1_s& ind_msg_format1() const
    {
      assert_choice_type(types::ind_msg_format1, type_, "indicationMessage-formats");
      return c.get<e2sm_kpm_ind_msg_format1_s>();
    }
    const e2sm_kpm_ind_msg_format2_s& ind_msg_format2() const
    {
      assert_choice_type(types::ind_msg_format2, type_, "indicationMessage-formats");
      return c.get<e2sm_kpm_ind_msg_format2_s>();
    }
    const e2sm_kpm_ind_msg_format3_s& ind_msg_format3() const
    {
      assert_choice_type(types::ind_msg_format3, type_, "indicationMessage-formats");
      return c.get<e2sm_kpm_ind_msg_format3_s>();
    }
    e2sm_kpm_ind_msg_format1_s& set_ind_msg_format1();
    e2sm_kpm_ind_msg_format2_s& set_ind_msg_format2();
    e2sm_kpm_ind_msg_format3_s& set_ind_msg_format3();

  private:
    types                                                                                               type_;
    choice_buffer_t<e2sm_kpm_ind_msg_format1_s, e2sm_kpm_ind_msg_format2_s, e2sm_kpm_ind_msg_format3_s> c;

    void destroy_();
  };

  // member variables
  bool               ext = false;
  ind_msg_formats_c_ ind_msg_formats;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-RANfunction-Description ::= SEQUENCE
struct e2sm_kpm_ran_function_description_s {
  using ric_event_trigger_style_list_l_ = dyn_array<ric_event_trigger_style_item_s>;
  using ric_report_style_list_l_        = dyn_array<ric_report_style_item_s>;

  // member variables
  bool                            ext = false;
  ran_function_name_s             ran_function_name;
  ric_event_trigger_style_list_l_ ric_event_trigger_style_list;
  ric_report_style_list_l_        ric_report_style_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace e2sm
} // namespace asn1
