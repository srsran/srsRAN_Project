/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/*******************************************************************************
 *
 *                    3GPP TS ASN1 RRC NR v17.4.0 (2023-03)
 *
 ******************************************************************************/

#pragma once

#include "srsran/asn1/asn1_utils.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                             Forward Declarations
 ******************************************************************************/

struct rrc_setup_request_s;
struct rrc_resume_request_s;
struct rrc_reest_request_s;
struct rrc_sys_info_request_s;

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// UL-CCCH-MessageType ::= CHOICE
struct ul_ccch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { rrc_setup_request, rrc_resume_request, rrc_reest_request, rrc_sys_info_request, nulltype } value;

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
    rrc_setup_request_s& rrc_setup_request()
    {
      assert_choice_type(types::rrc_setup_request, type_, "c1");
      return c.get<rrc_setup_request_s>();
    }
    rrc_resume_request_s& rrc_resume_request()
    {
      assert_choice_type(types::rrc_resume_request, type_, "c1");
      return c.get<rrc_resume_request_s>();
    }
    rrc_reest_request_s& rrc_reest_request()
    {
      assert_choice_type(types::rrc_reest_request, type_, "c1");
      return c.get<rrc_reest_request_s>();
    }
    rrc_sys_info_request_s& rrc_sys_info_request()
    {
      assert_choice_type(types::rrc_sys_info_request, type_, "c1");
      return c.get<rrc_sys_info_request_s>();
    }
    const rrc_setup_request_s& rrc_setup_request() const
    {
      assert_choice_type(types::rrc_setup_request, type_, "c1");
      return c.get<rrc_setup_request_s>();
    }
    const rrc_resume_request_s& rrc_resume_request() const
    {
      assert_choice_type(types::rrc_resume_request, type_, "c1");
      return c.get<rrc_resume_request_s>();
    }
    const rrc_reest_request_s& rrc_reest_request() const
    {
      assert_choice_type(types::rrc_reest_request, type_, "c1");
      return c.get<rrc_reest_request_s>();
    }
    const rrc_sys_info_request_s& rrc_sys_info_request() const
    {
      assert_choice_type(types::rrc_sys_info_request, type_, "c1");
      return c.get<rrc_sys_info_request_s>();
    }
    rrc_setup_request_s&    set_rrc_setup_request();
    rrc_resume_request_s&   set_rrc_resume_request();
    rrc_reest_request_s&    set_rrc_reest_request();
    rrc_sys_info_request_s& set_rrc_sys_info_request();

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ul_ccch_msg_type_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "UL-CCCH-MessageType");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "UL-CCCH-MessageType");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// UL-CCCH-Message ::= SEQUENCE
struct ul_ccch_msg_s {
  ul_ccch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1
