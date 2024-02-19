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

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// EstablishmentCause ::= ENUMERATED
struct establishment_cause_opts {
  enum options {
    emergency,
    high_prio_access,
    mt_access,
    mo_sig,
    mo_data,
    mo_voice_call,
    mo_video_call,
    mo_sms,
    mps_prio_access,
    mcs_prio_access,
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
using establishment_cause_e = enumerated<establishment_cause_opts>;

// InitialUE-Identity ::= CHOICE
struct init_ue_id_c {
  struct types_opts {
    enum options { ng_5_g_s_tmsi_part1, random_value, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  init_ue_id_c() = default;
  init_ue_id_c(const init_ue_id_c& other);
  init_ue_id_c& operator=(const init_ue_id_c& other);
  ~init_ue_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<39>& ng_5_g_s_tmsi_part1()
  {
    assert_choice_type(types::ng_5_g_s_tmsi_part1, type_, "InitialUE-Identity");
    return c.get<fixed_bitstring<39>>();
  }
  fixed_bitstring<39>& random_value()
  {
    assert_choice_type(types::random_value, type_, "InitialUE-Identity");
    return c.get<fixed_bitstring<39>>();
  }
  const fixed_bitstring<39>& ng_5_g_s_tmsi_part1() const
  {
    assert_choice_type(types::ng_5_g_s_tmsi_part1, type_, "InitialUE-Identity");
    return c.get<fixed_bitstring<39>>();
  }
  const fixed_bitstring<39>& random_value() const
  {
    assert_choice_type(types::random_value, type_, "InitialUE-Identity");
    return c.get<fixed_bitstring<39>>();
  }
  fixed_bitstring<39>& set_ng_5_g_s_tmsi_part1();
  fixed_bitstring<39>& set_random_value();

private:
  types                                type_;
  choice_buffer_t<fixed_bitstring<39>> c;

  void destroy_();
};

// ReestabUE-Identity ::= SEQUENCE
struct reestab_ue_id_s {
  uint32_t            c_rnti = 0;
  uint16_t            pci    = 0;
  fixed_bitstring<16> short_mac_i;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReestablishmentCause ::= ENUMERATED
struct reest_cause_opts {
  enum options { recfg_fail, ho_fail, other_fail, spare1, nulltype } value;

  const char* to_string() const;
};
using reest_cause_e = enumerated<reest_cause_opts>;

// ResumeCause ::= ENUMERATED
struct resume_cause_opts {
  enum options {
    emergency,
    high_prio_access,
    mt_access,
    mo_sig,
    mo_data,
    mo_voice_call,
    mo_video_call,
    mo_sms,
    rna_upd,
    mps_prio_access,
    mcs_prio_access,
    spare1,
    spare2,
    spare3,
    spare4,
    spare5,
    nulltype
  } value;

  const char* to_string() const;
};
using resume_cause_e = enumerated<resume_cause_opts>;

// RRC-PosSystemInfoRequest-r16-IEs ::= SEQUENCE
struct rrc_pos_sys_info_request_r16_ies_s {
  fixed_bitstring<32> requested_pos_si_list;
  fixed_bitstring<11> spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReestablishmentRequest-IEs ::= SEQUENCE
struct rrc_reest_request_ies_s {
  reestab_ue_id_s    ue_id;
  reest_cause_e      reest_cause;
  fixed_bitstring<1> spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResumeRequest-IEs ::= SEQUENCE
struct rrc_resume_request_ies_s {
  fixed_bitstring<24> resume_id;
  fixed_bitstring<16> resume_mac_i;
  resume_cause_e      resume_cause;
  fixed_bitstring<1>  spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSetupRequest-IEs ::= SEQUENCE
struct rrc_setup_request_ies_s {
  init_ue_id_c          ue_id;
  establishment_cause_e establishment_cause;
  fixed_bitstring<1>    spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSystemInfoRequest-IEs ::= SEQUENCE
struct rrc_sys_info_request_ies_s {
  fixed_bitstring<32> requested_si_list;
  fixed_bitstring<12> spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReestablishmentRequest ::= SEQUENCE
struct rrc_reest_request_s {
  rrc_reest_request_ies_s rrc_reest_request;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResumeRequest ::= SEQUENCE
struct rrc_resume_request_s {
  rrc_resume_request_ies_s rrc_resume_request;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSetupRequest ::= SEQUENCE
struct rrc_setup_request_s {
  rrc_setup_request_ies_s rrc_setup_request;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSystemInfoRequest ::= SEQUENCE
struct rrc_sys_info_request_s {
  struct crit_exts_c_ {
    struct crit_exts_future_r16_c_ {
      struct types_opts {
        enum options { rrc_pos_sys_info_request_r16, crit_exts_future, nulltype } value;

        const char* to_string() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      crit_exts_future_r16_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      rrc_pos_sys_info_request_r16_ies_s& rrc_pos_sys_info_request_r16()
      {
        assert_choice_type(types::rrc_pos_sys_info_request_r16, type_, "criticalExtensionsFuture-r16");
        return c;
      }
      const rrc_pos_sys_info_request_r16_ies_s& rrc_pos_sys_info_request_r16() const
      {
        assert_choice_type(types::rrc_pos_sys_info_request_r16, type_, "criticalExtensionsFuture-r16");
        return c;
      }
      rrc_pos_sys_info_request_r16_ies_s& set_rrc_pos_sys_info_request_r16();
      void                                set_crit_exts_future();

    private:
      types                              type_;
      rrc_pos_sys_info_request_r16_ies_s c;
    };
    struct types_opts {
      enum options { rrc_sys_info_request, crit_exts_future_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_sys_info_request_ies_s& rrc_sys_info_request()
    {
      assert_choice_type(types::rrc_sys_info_request, type_, "criticalExtensions");
      return c.get<rrc_sys_info_request_ies_s>();
    }
    crit_exts_future_r16_c_& crit_exts_future_r16()
    {
      assert_choice_type(types::crit_exts_future_r16, type_, "criticalExtensions");
      return c.get<crit_exts_future_r16_c_>();
    }
    const rrc_sys_info_request_ies_s& rrc_sys_info_request() const
    {
      assert_choice_type(types::rrc_sys_info_request, type_, "criticalExtensions");
      return c.get<rrc_sys_info_request_ies_s>();
    }
    const crit_exts_future_r16_c_& crit_exts_future_r16() const
    {
      assert_choice_type(types::crit_exts_future_r16, type_, "criticalExtensions");
      return c.get<crit_exts_future_r16_c_>();
    }
    rrc_sys_info_request_ies_s& set_rrc_sys_info_request();
    crit_exts_future_r16_c_&    set_crit_exts_future_r16();

  private:
    types                                                                type_;
    choice_buffer_t<crit_exts_future_r16_c_, rrc_sys_info_request_ies_s> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1
