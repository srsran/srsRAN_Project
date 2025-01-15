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

struct rrc_reject_s;
struct rrc_setup_s;

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// DL-CCCH-MessageType ::= CHOICE
struct dl_ccch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { rrc_reject, rrc_setup, spare2, spare1, nulltype } value;

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
    rrc_reject_s& rrc_reject()
    {
      assert_choice_type(types::rrc_reject, type_, "c1");
      return c.get<rrc_reject_s>();
    }
    rrc_setup_s& rrc_setup()
    {
      assert_choice_type(types::rrc_setup, type_, "c1");
      return c.get<rrc_setup_s>();
    }
    const rrc_reject_s& rrc_reject() const
    {
      assert_choice_type(types::rrc_reject, type_, "c1");
      return c.get<rrc_reject_s>();
    }
    const rrc_setup_s& rrc_setup() const
    {
      assert_choice_type(types::rrc_setup, type_, "c1");
      return c.get<rrc_setup_s>();
    }
    rrc_reject_s& set_rrc_reject();
    rrc_setup_s&  set_rrc_setup();
    void          set_spare2();
    void          set_spare1();

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
  dl_ccch_msg_type_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "DL-CCCH-MessageType");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "DL-CCCH-MessageType");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// DL-CCCH-Message ::= SEQUENCE
struct dl_ccch_msg_s {
  dl_ccch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1
