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

struct meas_report_s;
struct rrc_recfg_complete_s;
struct rrc_setup_complete_s;
struct rrc_reest_complete_s;
struct rrc_resume_complete_s;
struct security_mode_complete_s;
struct security_mode_fail_s;
struct ul_info_transfer_s;
struct location_meas_ind_s;
struct ue_cap_info_s;
struct counter_check_resp_s;
struct ue_assist_info_s;
struct fail_info_s;
struct ul_info_transfer_mrdc_s;
struct scg_fail_info_s;
struct scg_fail_info_eutra_s;
struct ul_ded_msg_segment_r16_s;
struct ded_sib_request_r16_s;
struct mcg_fail_info_r16_s;
struct ue_info_resp_r16_s;
struct sidelink_ue_info_nr_r16_s;
struct ul_info_transfer_irat_r16_s;
struct iab_other_info_r16_s;
struct mb_si_nterest_ind_r17_s;
struct ue_positioning_assist_info_r17_s;
struct meas_report_app_layer_r17_s;

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// UL-DCCH-MessageType ::= CHOICE
struct ul_dcch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options {
        meas_report,
        rrc_recfg_complete,
        rrc_setup_complete,
        rrc_reest_complete,
        rrc_resume_complete,
        security_mode_complete,
        security_mode_fail,
        ul_info_transfer,
        location_meas_ind,
        ue_cap_info,
        counter_check_resp,
        ue_assist_info,
        fail_info,
        ul_info_transfer_mrdc,
        scg_fail_info,
        scg_fail_info_eutra,
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
    meas_report_s& meas_report()
    {
      assert_choice_type(types::meas_report, type_, "c1");
      return c.get<meas_report_s>();
    }
    rrc_recfg_complete_s& rrc_recfg_complete()
    {
      assert_choice_type(types::rrc_recfg_complete, type_, "c1");
      return c.get<rrc_recfg_complete_s>();
    }
    rrc_setup_complete_s& rrc_setup_complete()
    {
      assert_choice_type(types::rrc_setup_complete, type_, "c1");
      return c.get<rrc_setup_complete_s>();
    }
    rrc_reest_complete_s& rrc_reest_complete()
    {
      assert_choice_type(types::rrc_reest_complete, type_, "c1");
      return c.get<rrc_reest_complete_s>();
    }
    rrc_resume_complete_s& rrc_resume_complete()
    {
      assert_choice_type(types::rrc_resume_complete, type_, "c1");
      return c.get<rrc_resume_complete_s>();
    }
    security_mode_complete_s& security_mode_complete()
    {
      assert_choice_type(types::security_mode_complete, type_, "c1");
      return c.get<security_mode_complete_s>();
    }
    security_mode_fail_s& security_mode_fail()
    {
      assert_choice_type(types::security_mode_fail, type_, "c1");
      return c.get<security_mode_fail_s>();
    }
    ul_info_transfer_s& ul_info_transfer()
    {
      assert_choice_type(types::ul_info_transfer, type_, "c1");
      return c.get<ul_info_transfer_s>();
    }
    location_meas_ind_s& location_meas_ind()
    {
      assert_choice_type(types::location_meas_ind, type_, "c1");
      return c.get<location_meas_ind_s>();
    }
    ue_cap_info_s& ue_cap_info()
    {
      assert_choice_type(types::ue_cap_info, type_, "c1");
      return c.get<ue_cap_info_s>();
    }
    counter_check_resp_s& counter_check_resp()
    {
      assert_choice_type(types::counter_check_resp, type_, "c1");
      return c.get<counter_check_resp_s>();
    }
    ue_assist_info_s& ue_assist_info()
    {
      assert_choice_type(types::ue_assist_info, type_, "c1");
      return c.get<ue_assist_info_s>();
    }
    fail_info_s& fail_info()
    {
      assert_choice_type(types::fail_info, type_, "c1");
      return c.get<fail_info_s>();
    }
    ul_info_transfer_mrdc_s& ul_info_transfer_mrdc()
    {
      assert_choice_type(types::ul_info_transfer_mrdc, type_, "c1");
      return c.get<ul_info_transfer_mrdc_s>();
    }
    scg_fail_info_s& scg_fail_info()
    {
      assert_choice_type(types::scg_fail_info, type_, "c1");
      return c.get<scg_fail_info_s>();
    }
    scg_fail_info_eutra_s& scg_fail_info_eutra()
    {
      assert_choice_type(types::scg_fail_info_eutra, type_, "c1");
      return c.get<scg_fail_info_eutra_s>();
    }
    const meas_report_s& meas_report() const
    {
      assert_choice_type(types::meas_report, type_, "c1");
      return c.get<meas_report_s>();
    }
    const rrc_recfg_complete_s& rrc_recfg_complete() const
    {
      assert_choice_type(types::rrc_recfg_complete, type_, "c1");
      return c.get<rrc_recfg_complete_s>();
    }
    const rrc_setup_complete_s& rrc_setup_complete() const
    {
      assert_choice_type(types::rrc_setup_complete, type_, "c1");
      return c.get<rrc_setup_complete_s>();
    }
    const rrc_reest_complete_s& rrc_reest_complete() const
    {
      assert_choice_type(types::rrc_reest_complete, type_, "c1");
      return c.get<rrc_reest_complete_s>();
    }
    const rrc_resume_complete_s& rrc_resume_complete() const
    {
      assert_choice_type(types::rrc_resume_complete, type_, "c1");
      return c.get<rrc_resume_complete_s>();
    }
    const security_mode_complete_s& security_mode_complete() const
    {
      assert_choice_type(types::security_mode_complete, type_, "c1");
      return c.get<security_mode_complete_s>();
    }
    const security_mode_fail_s& security_mode_fail() const
    {
      assert_choice_type(types::security_mode_fail, type_, "c1");
      return c.get<security_mode_fail_s>();
    }
    const ul_info_transfer_s& ul_info_transfer() const
    {
      assert_choice_type(types::ul_info_transfer, type_, "c1");
      return c.get<ul_info_transfer_s>();
    }
    const location_meas_ind_s& location_meas_ind() const
    {
      assert_choice_type(types::location_meas_ind, type_, "c1");
      return c.get<location_meas_ind_s>();
    }
    const ue_cap_info_s& ue_cap_info() const
    {
      assert_choice_type(types::ue_cap_info, type_, "c1");
      return c.get<ue_cap_info_s>();
    }
    const counter_check_resp_s& counter_check_resp() const
    {
      assert_choice_type(types::counter_check_resp, type_, "c1");
      return c.get<counter_check_resp_s>();
    }
    const ue_assist_info_s& ue_assist_info() const
    {
      assert_choice_type(types::ue_assist_info, type_, "c1");
      return c.get<ue_assist_info_s>();
    }
    const fail_info_s& fail_info() const
    {
      assert_choice_type(types::fail_info, type_, "c1");
      return c.get<fail_info_s>();
    }
    const ul_info_transfer_mrdc_s& ul_info_transfer_mrdc() const
    {
      assert_choice_type(types::ul_info_transfer_mrdc, type_, "c1");
      return c.get<ul_info_transfer_mrdc_s>();
    }
    const scg_fail_info_s& scg_fail_info() const
    {
      assert_choice_type(types::scg_fail_info, type_, "c1");
      return c.get<scg_fail_info_s>();
    }
    const scg_fail_info_eutra_s& scg_fail_info_eutra() const
    {
      assert_choice_type(types::scg_fail_info_eutra, type_, "c1");
      return c.get<scg_fail_info_eutra_s>();
    }
    meas_report_s&            set_meas_report();
    rrc_recfg_complete_s&     set_rrc_recfg_complete();
    rrc_setup_complete_s&     set_rrc_setup_complete();
    rrc_reest_complete_s&     set_rrc_reest_complete();
    rrc_resume_complete_s&    set_rrc_resume_complete();
    security_mode_complete_s& set_security_mode_complete();
    security_mode_fail_s&     set_security_mode_fail();
    ul_info_transfer_s&       set_ul_info_transfer();
    location_meas_ind_s&      set_location_meas_ind();
    ue_cap_info_s&            set_ue_cap_info();
    counter_check_resp_s&     set_counter_check_resp();
    ue_assist_info_s&         set_ue_assist_info();
    fail_info_s&              set_fail_info();
    ul_info_transfer_mrdc_s&  set_ul_info_transfer_mrdc();
    scg_fail_info_s&          set_scg_fail_info();
    scg_fail_info_eutra_s&    set_scg_fail_info_eutra();

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  struct msg_class_ext_c_ {
    struct c2_c_ {
      struct types_opts {
        enum options {
          ul_ded_msg_segment_r16,
          ded_sib_request_r16,
          mcg_fail_info_r16,
          ue_info_resp_r16,
          sidelink_ue_info_nr_r16,
          ul_info_transfer_irat_r16,
          iab_other_info_r16,
          mbs_interest_ind_r17,
          ue_positioning_assist_info_r17,
          meas_report_app_layer_r17,
          spare6,
          spare5,
          spare4,
          spare3,
          spare2,
          spare1,
          nulltype
        } value;

        const char* to_string() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      c2_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      ul_ded_msg_segment_r16_s& ul_ded_msg_segment_r16()
      {
        assert_choice_type(types::ul_ded_msg_segment_r16, type_, "c2");
        return c.get<ul_ded_msg_segment_r16_s>();
      }
      ded_sib_request_r16_s& ded_sib_request_r16()
      {
        assert_choice_type(types::ded_sib_request_r16, type_, "c2");
        return c.get<ded_sib_request_r16_s>();
      }
      mcg_fail_info_r16_s& mcg_fail_info_r16()
      {
        assert_choice_type(types::mcg_fail_info_r16, type_, "c2");
        return c.get<mcg_fail_info_r16_s>();
      }
      ue_info_resp_r16_s& ue_info_resp_r16()
      {
        assert_choice_type(types::ue_info_resp_r16, type_, "c2");
        return c.get<ue_info_resp_r16_s>();
      }
      sidelink_ue_info_nr_r16_s& sidelink_ue_info_nr_r16()
      {
        assert_choice_type(types::sidelink_ue_info_nr_r16, type_, "c2");
        return c.get<sidelink_ue_info_nr_r16_s>();
      }
      ul_info_transfer_irat_r16_s& ul_info_transfer_irat_r16()
      {
        assert_choice_type(types::ul_info_transfer_irat_r16, type_, "c2");
        return c.get<ul_info_transfer_irat_r16_s>();
      }
      iab_other_info_r16_s& iab_other_info_r16()
      {
        assert_choice_type(types::iab_other_info_r16, type_, "c2");
        return c.get<iab_other_info_r16_s>();
      }
      mb_si_nterest_ind_r17_s& mbs_interest_ind_r17()
      {
        assert_choice_type(types::mbs_interest_ind_r17, type_, "c2");
        return c.get<mb_si_nterest_ind_r17_s>();
      }
      ue_positioning_assist_info_r17_s& ue_positioning_assist_info_r17()
      {
        assert_choice_type(types::ue_positioning_assist_info_r17, type_, "c2");
        return c.get<ue_positioning_assist_info_r17_s>();
      }
      meas_report_app_layer_r17_s& meas_report_app_layer_r17()
      {
        assert_choice_type(types::meas_report_app_layer_r17, type_, "c2");
        return c.get<meas_report_app_layer_r17_s>();
      }
      const ul_ded_msg_segment_r16_s& ul_ded_msg_segment_r16() const
      {
        assert_choice_type(types::ul_ded_msg_segment_r16, type_, "c2");
        return c.get<ul_ded_msg_segment_r16_s>();
      }
      const ded_sib_request_r16_s& ded_sib_request_r16() const
      {
        assert_choice_type(types::ded_sib_request_r16, type_, "c2");
        return c.get<ded_sib_request_r16_s>();
      }
      const mcg_fail_info_r16_s& mcg_fail_info_r16() const
      {
        assert_choice_type(types::mcg_fail_info_r16, type_, "c2");
        return c.get<mcg_fail_info_r16_s>();
      }
      const ue_info_resp_r16_s& ue_info_resp_r16() const
      {
        assert_choice_type(types::ue_info_resp_r16, type_, "c2");
        return c.get<ue_info_resp_r16_s>();
      }
      const sidelink_ue_info_nr_r16_s& sidelink_ue_info_nr_r16() const
      {
        assert_choice_type(types::sidelink_ue_info_nr_r16, type_, "c2");
        return c.get<sidelink_ue_info_nr_r16_s>();
      }
      const ul_info_transfer_irat_r16_s& ul_info_transfer_irat_r16() const
      {
        assert_choice_type(types::ul_info_transfer_irat_r16, type_, "c2");
        return c.get<ul_info_transfer_irat_r16_s>();
      }
      const iab_other_info_r16_s& iab_other_info_r16() const
      {
        assert_choice_type(types::iab_other_info_r16, type_, "c2");
        return c.get<iab_other_info_r16_s>();
      }
      const mb_si_nterest_ind_r17_s& mbs_interest_ind_r17() const
      {
        assert_choice_type(types::mbs_interest_ind_r17, type_, "c2");
        return c.get<mb_si_nterest_ind_r17_s>();
      }
      const ue_positioning_assist_info_r17_s& ue_positioning_assist_info_r17() const
      {
        assert_choice_type(types::ue_positioning_assist_info_r17, type_, "c2");
        return c.get<ue_positioning_assist_info_r17_s>();
      }
      const meas_report_app_layer_r17_s& meas_report_app_layer_r17() const
      {
        assert_choice_type(types::meas_report_app_layer_r17, type_, "c2");
        return c.get<meas_report_app_layer_r17_s>();
      }
      ul_ded_msg_segment_r16_s&         set_ul_ded_msg_segment_r16();
      ded_sib_request_r16_s&            set_ded_sib_request_r16();
      mcg_fail_info_r16_s&              set_mcg_fail_info_r16();
      ue_info_resp_r16_s&               set_ue_info_resp_r16();
      sidelink_ue_info_nr_r16_s&        set_sidelink_ue_info_nr_r16();
      ul_info_transfer_irat_r16_s&      set_ul_info_transfer_irat_r16();
      iab_other_info_r16_s&             set_iab_other_info_r16();
      mb_si_nterest_ind_r17_s&          set_mbs_interest_ind_r17();
      ue_positioning_assist_info_r17_s& set_ue_positioning_assist_info_r17();
      meas_report_app_layer_r17_s&      set_meas_report_app_layer_r17();
      void                              set_spare6();
      void                              set_spare5();
      void                              set_spare4();
      void                              set_spare3();
      void                              set_spare2();
      void                              set_spare1();

    private:
      types             type_;
      choice_buffer_ptr c;
    };
    struct types_opts {
      enum options { c2, msg_class_ext_future_r16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    msg_class_ext_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    c2_c_& c2()
    {
      assert_choice_type(types::c2, type_, "messageClassExtension");
      return c;
    }
    const c2_c_& c2() const
    {
      assert_choice_type(types::c2, type_, "messageClassExtension");
      return c;
    }
    c2_c_& set_c2();
    void   set_msg_class_ext_future_r16();

  private:
    types type_;
    c2_c_ c;
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ul_dcch_msg_type_c() = default;
  ul_dcch_msg_type_c(const ul_dcch_msg_type_c& other);
  ul_dcch_msg_type_c& operator=(const ul_dcch_msg_type_c& other);
  ~ul_dcch_msg_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "UL-DCCH-MessageType");
    return c.get<c1_c_>();
  }
  msg_class_ext_c_& msg_class_ext()
  {
    assert_choice_type(types::msg_class_ext, type_, "UL-DCCH-MessageType");
    return c.get<msg_class_ext_c_>();
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "UL-DCCH-MessageType");
    return c.get<c1_c_>();
  }
  const msg_class_ext_c_& msg_class_ext() const
  {
    assert_choice_type(types::msg_class_ext, type_, "UL-DCCH-MessageType");
    return c.get<msg_class_ext_c_>();
  }
  c1_c_&            set_c1();
  msg_class_ext_c_& set_msg_class_ext();

private:
  types                                    type_;
  choice_buffer_t<c1_c_, msg_class_ext_c_> c;

  void destroy_();
};

// UL-DCCH-Message ::= SEQUENCE
struct ul_dcch_msg_s {
  ul_dcch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1
