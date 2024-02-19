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

#include "srsran/asn1/asn1_utils.h"

namespace asn1 {
namespace rrc_nr {

struct rrc_recfg_s;
struct rrc_resume_s;
struct rrc_release_s;
struct rrc_reest_s;
struct security_mode_cmd_s;
struct dl_info_transfer_s;
struct ue_cap_enquiry_s;
struct counter_check_s;
struct mob_from_nr_cmd_s;
struct dl_ded_msg_segment_r16_s;
struct ue_info_request_r16_s;
struct dl_info_transfer_mrdc_r16_s;
struct logged_meas_cfg_r16_s;

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// DL-DCCH-MessageType ::= CHOICE
struct dl_dcch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options {
        rrc_recfg,
        rrc_resume,
        rrc_release,
        rrc_reest,
        security_mode_cmd,
        dl_info_transfer,
        ue_cap_enquiry,
        counter_check,
        mob_from_nr_cmd,
        dl_ded_msg_segment_r16,
        ue_info_request_r16,
        dl_info_transfer_mrdc_r16,
        logged_meas_cfg_r16,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;

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
    rrc_recfg_s& rrc_recfg()
    {
      assert_choice_type(types::rrc_recfg, type_, "c1");
      return c.get<rrc_recfg_s>();
    }
    rrc_resume_s& rrc_resume()
    {
      assert_choice_type(types::rrc_resume, type_, "c1");
      return c.get<rrc_resume_s>();
    }
    rrc_release_s& rrc_release()
    {
      assert_choice_type(types::rrc_release, type_, "c1");
      return c.get<rrc_release_s>();
    }
    rrc_reest_s& rrc_reest()
    {
      assert_choice_type(types::rrc_reest, type_, "c1");
      return c.get<rrc_reest_s>();
    }
    security_mode_cmd_s& security_mode_cmd()
    {
      assert_choice_type(types::security_mode_cmd, type_, "c1");
      return c.get<security_mode_cmd_s>();
    }
    dl_info_transfer_s& dl_info_transfer()
    {
      assert_choice_type(types::dl_info_transfer, type_, "c1");
      return c.get<dl_info_transfer_s>();
    }
    ue_cap_enquiry_s& ue_cap_enquiry()
    {
      assert_choice_type(types::ue_cap_enquiry, type_, "c1");
      return c.get<ue_cap_enquiry_s>();
    }
    counter_check_s& counter_check()
    {
      assert_choice_type(types::counter_check, type_, "c1");
      return c.get<counter_check_s>();
    }
    mob_from_nr_cmd_s& mob_from_nr_cmd()
    {
      assert_choice_type(types::mob_from_nr_cmd, type_, "c1");
      return c.get<mob_from_nr_cmd_s>();
    }
    dl_ded_msg_segment_r16_s& dl_ded_msg_segment_r16()
    {
      assert_choice_type(types::dl_ded_msg_segment_r16, type_, "c1");
      return c.get<dl_ded_msg_segment_r16_s>();
    }
    ue_info_request_r16_s& ue_info_request_r16()
    {
      assert_choice_type(types::ue_info_request_r16, type_, "c1");
      return c.get<ue_info_request_r16_s>();
    }
    dl_info_transfer_mrdc_r16_s& dl_info_transfer_mrdc_r16()
    {
      assert_choice_type(types::dl_info_transfer_mrdc_r16, type_, "c1");
      return c.get<dl_info_transfer_mrdc_r16_s>();
    }
    logged_meas_cfg_r16_s& logged_meas_cfg_r16()
    {
      assert_choice_type(types::logged_meas_cfg_r16, type_, "c1");
      return c.get<logged_meas_cfg_r16_s>();
    }
    const rrc_recfg_s& rrc_recfg() const
    {
      assert_choice_type(types::rrc_recfg, type_, "c1");
      return c.get<rrc_recfg_s>();
    }
    const rrc_resume_s& rrc_resume() const
    {
      assert_choice_type(types::rrc_resume, type_, "c1");
      return c.get<rrc_resume_s>();
    }
    const rrc_release_s& rrc_release() const
    {
      assert_choice_type(types::rrc_release, type_, "c1");
      return c.get<rrc_release_s>();
    }
    const rrc_reest_s& rrc_reest() const
    {
      assert_choice_type(types::rrc_reest, type_, "c1");
      return c.get<rrc_reest_s>();
    }
    const security_mode_cmd_s& security_mode_cmd() const
    {
      assert_choice_type(types::security_mode_cmd, type_, "c1");
      return c.get<security_mode_cmd_s>();
    }
    const dl_info_transfer_s& dl_info_transfer() const
    {
      assert_choice_type(types::dl_info_transfer, type_, "c1");
      return c.get<dl_info_transfer_s>();
    }
    const ue_cap_enquiry_s& ue_cap_enquiry() const
    {
      assert_choice_type(types::ue_cap_enquiry, type_, "c1");
      return c.get<ue_cap_enquiry_s>();
    }
    const counter_check_s& counter_check() const
    {
      assert_choice_type(types::counter_check, type_, "c1");
      return c.get<counter_check_s>();
    }
    const mob_from_nr_cmd_s& mob_from_nr_cmd() const
    {
      assert_choice_type(types::mob_from_nr_cmd, type_, "c1");
      return c.get<mob_from_nr_cmd_s>();
    }
    const dl_ded_msg_segment_r16_s& dl_ded_msg_segment_r16() const
    {
      assert_choice_type(types::dl_ded_msg_segment_r16, type_, "c1");
      return c.get<dl_ded_msg_segment_r16_s>();
    }
    const ue_info_request_r16_s& ue_info_request_r16() const
    {
      assert_choice_type(types::ue_info_request_r16, type_, "c1");
      return c.get<ue_info_request_r16_s>();
    }
    const dl_info_transfer_mrdc_r16_s& dl_info_transfer_mrdc_r16() const
    {
      assert_choice_type(types::dl_info_transfer_mrdc_r16, type_, "c1");
      return c.get<dl_info_transfer_mrdc_r16_s>();
    }
    const logged_meas_cfg_r16_s& logged_meas_cfg_r16() const
    {
      assert_choice_type(types::logged_meas_cfg_r16, type_, "c1");
      return c.get<logged_meas_cfg_r16_s>();
    }
    rrc_recfg_s&                 set_rrc_recfg();
    rrc_resume_s&                set_rrc_resume();
    rrc_release_s&               set_rrc_release();
    rrc_reest_s&                 set_rrc_reest();
    security_mode_cmd_s&         set_security_mode_cmd();
    dl_info_transfer_s&          set_dl_info_transfer();
    ue_cap_enquiry_s&            set_ue_cap_enquiry();
    counter_check_s&             set_counter_check();
    mob_from_nr_cmd_s&           set_mob_from_nr_cmd();
    dl_ded_msg_segment_r16_s&    set_dl_ded_msg_segment_r16();
    ue_info_request_r16_s&       set_ue_info_request_r16();
    dl_info_transfer_mrdc_r16_s& set_dl_info_transfer_mrdc_r16();
    logged_meas_cfg_r16_s&       set_logged_meas_cfg_r16();
    void                         set_spare3();
    void                         set_spare2();
    void                         set_spare1();

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
  dl_dcch_msg_type_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "DL-DCCH-MessageType");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "DL-DCCH-MessageType");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// DL-DCCH-Message ::= SEQUENCE
struct dl_dcch_msg_s {
  dl_dcch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1
