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
 *                     3GPP TS ASN1 NGAP v17.4.0 (2023-03)
 *
 ******************************************************************************/

#pragma once

#include "ngap_ies.h"

namespace asn1 {
namespace ngap {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// AMFCPRelocationIndicationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct amf_cp_relocation_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, s_nssai, allowed_nssai, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&              amf_ue_ngap_id();
    uint64_t&              ran_ue_ngap_id();
    s_nssai_s&             s_nssai();
    allowed_nssai_l&       allowed_nssai();
    const uint64_t&        amf_ue_ngap_id() const;
    const uint64_t&        ran_ue_ngap_id() const;
    const s_nssai_s&       s_nssai() const;
    const allowed_nssai_l& allowed_nssai() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct amf_cp_relocation_ind_ies_container {
  bool            s_nssai_present       = false;
  bool            allowed_nssai_present = false;
  uint64_t        amf_ue_ngap_id;
  uint64_t        ran_ue_ngap_id;
  s_nssai_s       s_nssai;
  allowed_nssai_l allowed_nssai;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AMFCPRelocationIndication ::= SEQUENCE
using amf_cp_relocation_ind_s = elementary_procedure_option<amf_cp_relocation_ind_ies_container>;

// AMFConfigurationUpdateIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct amf_cfg_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_name,
        served_guami_list,
        relative_amf_capacity,
        plmn_support_list,
        amf_tnl_assoc_to_add_list,
        amf_tnl_assoc_to_rem_list,
        amf_tnl_assoc_to_upd_list,
        extended_amf_name,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    printable_string<1, 150, true, true>&       amf_name();
    served_guami_list_l&                        served_guami_list();
    uint16_t&                                   relative_amf_capacity();
    plmn_support_list_l&                        plmn_support_list();
    amf_tnl_assoc_to_add_list_l&                amf_tnl_assoc_to_add_list();
    amf_tnl_assoc_to_rem_list_l&                amf_tnl_assoc_to_rem_list();
    amf_tnl_assoc_to_upd_list_l&                amf_tnl_assoc_to_upd_list();
    extended_amf_name_s&                        extended_amf_name();
    const printable_string<1, 150, true, true>& amf_name() const;
    const served_guami_list_l&                  served_guami_list() const;
    const uint16_t&                             relative_amf_capacity() const;
    const plmn_support_list_l&                  plmn_support_list() const;
    const amf_tnl_assoc_to_add_list_l&          amf_tnl_assoc_to_add_list() const;
    const amf_tnl_assoc_to_rem_list_l&          amf_tnl_assoc_to_rem_list() const;
    const amf_tnl_assoc_to_upd_list_l&          amf_tnl_assoc_to_upd_list() const;
    const extended_amf_name_s&                  extended_amf_name() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct amf_cfg_upd_ies_container {
  bool                                 amf_name_present                  = false;
  bool                                 served_guami_list_present         = false;
  bool                                 relative_amf_capacity_present     = false;
  bool                                 plmn_support_list_present         = false;
  bool                                 amf_tnl_assoc_to_add_list_present = false;
  bool                                 amf_tnl_assoc_to_rem_list_present = false;
  bool                                 amf_tnl_assoc_to_upd_list_present = false;
  bool                                 extended_amf_name_present         = false;
  printable_string<1, 150, true, true> amf_name;
  served_guami_list_l                  served_guami_list;
  uint16_t                             relative_amf_capacity;
  plmn_support_list_l                  plmn_support_list;
  amf_tnl_assoc_to_add_list_l          amf_tnl_assoc_to_add_list;
  amf_tnl_assoc_to_rem_list_l          amf_tnl_assoc_to_rem_list;
  amf_tnl_assoc_to_upd_list_l          amf_tnl_assoc_to_upd_list;
  extended_amf_name_s                  extended_amf_name;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AMFConfigurationUpdate ::= SEQUENCE
using amf_cfg_upd_s = elementary_procedure_option<amf_cfg_upd_ies_container>;

// AMFConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct amf_cfg_upd_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_tnl_assoc_setup_list, amf_tnl_assoc_failed_to_setup_list, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    amf_tnl_assoc_setup_list_l&       amf_tnl_assoc_setup_list();
    tnl_assoc_list_l&                 amf_tnl_assoc_failed_to_setup_list();
    crit_diagnostics_s&               crit_diagnostics();
    const amf_tnl_assoc_setup_list_l& amf_tnl_assoc_setup_list() const;
    const tnl_assoc_list_l&           amf_tnl_assoc_failed_to_setup_list() const;
    const crit_diagnostics_s&         crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct amf_cfg_upd_ack_ies_container {
  bool                       amf_tnl_assoc_setup_list_present           = false;
  bool                       amf_tnl_assoc_failed_to_setup_list_present = false;
  bool                       crit_diagnostics_present                   = false;
  amf_tnl_assoc_setup_list_l amf_tnl_assoc_setup_list;
  tnl_assoc_list_l           amf_tnl_assoc_failed_to_setup_list;
  crit_diagnostics_s         crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AMFConfigurationUpdateAcknowledge ::= SEQUENCE
using amf_cfg_upd_ack_s = elementary_procedure_option<amf_cfg_upd_ack_ies_container>;

// AMFConfigurationUpdateFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct amf_cfg_upd_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cause, time_to_wait, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const cause_c&            cause() const;
    const time_to_wait_e&     time_to_wait() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct amf_cfg_upd_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AMFConfigurationUpdateFailure ::= SEQUENCE
using amf_cfg_upd_fail_s = elementary_procedure_option<amf_cfg_upd_fail_ies_container>;

// AMFStatusIndicationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct amf_status_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { unavailable_guami_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::unavailable_guami_list; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    unavailable_guami_list_l&       unavailable_guami_list() { return c; }
    const unavailable_guami_list_l& unavailable_guami_list() const { return c; }

  private:
    unavailable_guami_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// AMFStatusIndication ::= SEQUENCE
using amf_status_ind_s = elementary_procedure_option<protocol_ie_container_l<amf_status_ind_ies_o>>;

// BroadcastSessionModificationFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct broadcast_session_mod_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, mbs_session_mod_fail_transfer, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&                mbs_session_id();
    unbounded_octstring<true>&       mbs_session_mod_fail_transfer();
    cause_c&                         cause();
    crit_diagnostics_s&              crit_diagnostics();
    const mbs_session_id_s&          mbs_session_id() const;
    const unbounded_octstring<true>& mbs_session_mod_fail_transfer() const;
    const cause_c&                   cause() const;
    const crit_diagnostics_s&        crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_session_mod_fail_ies_container {
  bool                      mbs_session_mod_fail_transfer_present = false;
  bool                      crit_diagnostics_present              = false;
  mbs_session_id_s          mbs_session_id;
  unbounded_octstring<true> mbs_session_mod_fail_transfer;
  cause_c                   cause;
  crit_diagnostics_s        crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastSessionModificationFailure ::= SEQUENCE
using broadcast_session_mod_fail_s = elementary_procedure_option<broadcast_session_mod_fail_ies_container>;

// BroadcastSessionModificationRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct broadcast_session_mod_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, mbs_service_area, mbs_session_mod_request_transfer, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&                mbs_session_id();
    mbs_service_area_c&              mbs_service_area();
    unbounded_octstring<true>&       mbs_session_mod_request_transfer();
    const mbs_session_id_s&          mbs_session_id() const;
    const mbs_service_area_c&        mbs_service_area() const;
    const unbounded_octstring<true>& mbs_session_mod_request_transfer() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_session_mod_request_ies_container {
  bool                      mbs_service_area_present                 = false;
  bool                      mbs_session_mod_request_transfer_present = false;
  mbs_session_id_s          mbs_session_id;
  mbs_service_area_c        mbs_service_area;
  unbounded_octstring<true> mbs_session_mod_request_transfer;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastSessionModificationRequest ::= SEQUENCE
using broadcast_session_mod_request_s = elementary_procedure_option<broadcast_session_mod_request_ies_container>;

// BroadcastSessionModificationResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct broadcast_session_mod_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, mbs_session_mod_resp_transfer, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&                mbs_session_id();
    unbounded_octstring<true>&       mbs_session_mod_resp_transfer();
    crit_diagnostics_s&              crit_diagnostics();
    const mbs_session_id_s&          mbs_session_id() const;
    const unbounded_octstring<true>& mbs_session_mod_resp_transfer() const;
    const crit_diagnostics_s&        crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_session_mod_resp_ies_container {
  bool                      mbs_session_mod_resp_transfer_present = false;
  bool                      crit_diagnostics_present              = false;
  mbs_session_id_s          mbs_session_id;
  unbounded_octstring<true> mbs_session_mod_resp_transfer;
  crit_diagnostics_s        crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastSessionModificationResponse ::= SEQUENCE
using broadcast_session_mod_resp_s = elementary_procedure_option<broadcast_session_mod_resp_ies_container>;

// BroadcastSessionReleaseRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct broadcast_session_release_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, cause, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&       mbs_session_id();
    cause_c&                cause();
    const mbs_session_id_s& mbs_session_id() const;
    const cause_c&          cause() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_session_release_request_ies_container {
  mbs_session_id_s mbs_session_id;
  cause_c          cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastSessionReleaseRequest ::= SEQUENCE
using broadcast_session_release_request_s =
    elementary_procedure_option<broadcast_session_release_request_ies_container>;

// BroadcastSessionReleaseRequiredIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct broadcast_session_release_required_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, cause, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&       mbs_session_id();
    cause_c&                cause();
    const mbs_session_id_s& mbs_session_id() const;
    const cause_c&          cause() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_session_release_required_ies_container {
  mbs_session_id_s mbs_session_id;
  cause_c          cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastSessionReleaseRequired ::= SEQUENCE
using broadcast_session_release_required_s =
    elementary_procedure_option<broadcast_session_release_required_ies_container>;

// BroadcastSessionReleaseResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct broadcast_session_release_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, mbs_session_release_resp_transfer, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&                mbs_session_id();
    unbounded_octstring<true>&       mbs_session_release_resp_transfer();
    crit_diagnostics_s&              crit_diagnostics();
    const mbs_session_id_s&          mbs_session_id() const;
    const unbounded_octstring<true>& mbs_session_release_resp_transfer() const;
    const crit_diagnostics_s&        crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_session_release_resp_ies_container {
  bool                      mbs_session_release_resp_transfer_present = false;
  bool                      crit_diagnostics_present                  = false;
  mbs_session_id_s          mbs_session_id;
  unbounded_octstring<true> mbs_session_release_resp_transfer;
  crit_diagnostics_s        crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastSessionReleaseResponse ::= SEQUENCE
using broadcast_session_release_resp_s = elementary_procedure_option<broadcast_session_release_resp_ies_container>;

// BroadcastSessionSetupFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct broadcast_session_setup_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, mbs_session_setup_fail_transfer, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&                mbs_session_id();
    unbounded_octstring<true>&       mbs_session_setup_fail_transfer();
    cause_c&                         cause();
    crit_diagnostics_s&              crit_diagnostics();
    const mbs_session_id_s&          mbs_session_id() const;
    const unbounded_octstring<true>& mbs_session_setup_fail_transfer() const;
    const cause_c&                   cause() const;
    const crit_diagnostics_s&        crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_session_setup_fail_ies_container {
  bool                      mbs_session_setup_fail_transfer_present = false;
  bool                      crit_diagnostics_present                = false;
  mbs_session_id_s          mbs_session_id;
  unbounded_octstring<true> mbs_session_setup_fail_transfer;
  cause_c                   cause;
  crit_diagnostics_s        crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastSessionSetupFailure ::= SEQUENCE
using broadcast_session_setup_fail_s = elementary_procedure_option<broadcast_session_setup_fail_ies_container>;

// BroadcastSessionSetupRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct broadcast_session_setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, s_nssai, mbs_service_area, mbs_session_setup_request_transfer, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&                mbs_session_id();
    s_nssai_s&                       s_nssai();
    mbs_service_area_c&              mbs_service_area();
    unbounded_octstring<true>&       mbs_session_setup_request_transfer();
    const mbs_session_id_s&          mbs_session_id() const;
    const s_nssai_s&                 s_nssai() const;
    const mbs_service_area_c&        mbs_service_area() const;
    const unbounded_octstring<true>& mbs_session_setup_request_transfer() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_session_setup_request_ies_container {
  mbs_session_id_s          mbs_session_id;
  s_nssai_s                 s_nssai;
  mbs_service_area_c        mbs_service_area;
  unbounded_octstring<true> mbs_session_setup_request_transfer;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastSessionSetupRequest ::= SEQUENCE
using broadcast_session_setup_request_s = elementary_procedure_option<broadcast_session_setup_request_ies_container>;

// BroadcastSessionSetupResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct broadcast_session_setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, mbs_session_setup_resp_transfer, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&                mbs_session_id();
    unbounded_octstring<true>&       mbs_session_setup_resp_transfer();
    crit_diagnostics_s&              crit_diagnostics();
    const mbs_session_id_s&          mbs_session_id() const;
    const unbounded_octstring<true>& mbs_session_setup_resp_transfer() const;
    const crit_diagnostics_s&        crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_session_setup_resp_ies_container {
  bool                      mbs_session_setup_resp_transfer_present = false;
  bool                      crit_diagnostics_present                = false;
  mbs_session_id_s          mbs_session_id;
  unbounded_octstring<true> mbs_session_setup_resp_transfer;
  crit_diagnostics_s        crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastSessionSetupResponse ::= SEQUENCE
using broadcast_session_setup_resp_s = elementary_procedure_option<broadcast_session_setup_resp_ies_container>;

// CellTrafficTraceIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct cell_traffic_trace_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        ngran_trace_id,
        ngran_cgi,
        trace_collection_entity_ip_address,
        privacy_ind,
        trace_collection_entity_uri,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                                    amf_ue_ngap_id();
    uint64_t&                                                    ran_ue_ngap_id();
    fixed_octstring<8, true>&                                    ngran_trace_id();
    ngran_cgi_c&                                                 ngran_cgi();
    bounded_bitstring<1, 160, true, true>&                       trace_collection_entity_ip_address();
    privacy_ind_e&                                               privacy_ind();
    visible_string<0, MAX_ASN_STRING_LENGTH, false, true>&       trace_collection_entity_uri();
    const uint64_t&                                              amf_ue_ngap_id() const;
    const uint64_t&                                              ran_ue_ngap_id() const;
    const fixed_octstring<8, true>&                              ngran_trace_id() const;
    const ngran_cgi_c&                                           ngran_cgi() const;
    const bounded_bitstring<1, 160, true, true>&                 trace_collection_entity_ip_address() const;
    const privacy_ind_e&                                         privacy_ind() const;
    const visible_string<0, MAX_ASN_STRING_LENGTH, false, true>& trace_collection_entity_uri() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct cell_traffic_trace_ies_container {
  bool                                                  privacy_ind_present                 = false;
  bool                                                  trace_collection_entity_uri_present = false;
  uint64_t                                              amf_ue_ngap_id;
  uint64_t                                              ran_ue_ngap_id;
  fixed_octstring<8, true>                              ngran_trace_id;
  ngran_cgi_c                                           ngran_cgi;
  bounded_bitstring<1, 160, true, true>                 trace_collection_entity_ip_address;
  privacy_ind_e                                         privacy_ind;
  visible_string<0, MAX_ASN_STRING_LENGTH, false, true> trace_collection_entity_uri;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellTrafficTrace ::= SEQUENCE
using cell_traffic_trace_s = elementary_procedure_option<cell_traffic_trace_ies_container>;

// ConnectionEstablishmentIndicationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct conn_establishment_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        ue_radio_cap,
        end_ind,
        s_nssai,
        allowed_nssai,
        ue_diff_info,
        dl_cp_security_info,
        nb_iot_ue_prio,
        enhanced_coverage_restrict,
        ce_mode_brestricted,
        ue_radio_cap_id,
        masked_imeisv,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                               amf_ue_ngap_id();
    uint64_t&                               ran_ue_ngap_id();
    unbounded_octstring<true>&              ue_radio_cap();
    end_ind_e&                              end_ind();
    s_nssai_s&                              s_nssai();
    allowed_nssai_l&                        allowed_nssai();
    ue_diff_info_s&                         ue_diff_info();
    dl_cp_security_info_s&                  dl_cp_security_info();
    uint16_t&                               nb_iot_ue_prio();
    enhanced_coverage_restrict_e&           enhanced_coverage_restrict();
    ce_mode_brestricted_e&                  ce_mode_brestricted();
    unbounded_octstring<true>&              ue_radio_cap_id();
    fixed_bitstring<64, false, true>&       masked_imeisv();
    const uint64_t&                         amf_ue_ngap_id() const;
    const uint64_t&                         ran_ue_ngap_id() const;
    const unbounded_octstring<true>&        ue_radio_cap() const;
    const end_ind_e&                        end_ind() const;
    const s_nssai_s&                        s_nssai() const;
    const allowed_nssai_l&                  allowed_nssai() const;
    const ue_diff_info_s&                   ue_diff_info() const;
    const dl_cp_security_info_s&            dl_cp_security_info() const;
    const uint16_t&                         nb_iot_ue_prio() const;
    const enhanced_coverage_restrict_e&     enhanced_coverage_restrict() const;
    const ce_mode_brestricted_e&            ce_mode_brestricted() const;
    const unbounded_octstring<true>&        ue_radio_cap_id() const;
    const fixed_bitstring<64, false, true>& masked_imeisv() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct conn_establishment_ind_ies_container {
  bool                             ue_radio_cap_present               = false;
  bool                             end_ind_present                    = false;
  bool                             s_nssai_present                    = false;
  bool                             allowed_nssai_present              = false;
  bool                             ue_diff_info_present               = false;
  bool                             dl_cp_security_info_present        = false;
  bool                             nb_iot_ue_prio_present             = false;
  bool                             enhanced_coverage_restrict_present = false;
  bool                             ce_mode_brestricted_present        = false;
  bool                             ue_radio_cap_id_present            = false;
  bool                             masked_imeisv_present              = false;
  uint64_t                         amf_ue_ngap_id;
  uint64_t                         ran_ue_ngap_id;
  unbounded_octstring<true>        ue_radio_cap;
  end_ind_e                        end_ind;
  s_nssai_s                        s_nssai;
  allowed_nssai_l                  allowed_nssai;
  ue_diff_info_s                   ue_diff_info;
  dl_cp_security_info_s            dl_cp_security_info;
  uint16_t                         nb_iot_ue_prio;
  enhanced_coverage_restrict_e     enhanced_coverage_restrict;
  ce_mode_brestricted_e            ce_mode_brestricted;
  unbounded_octstring<true>        ue_radio_cap_id;
  fixed_bitstring<64, false, true> masked_imeisv;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ConnectionEstablishmentIndication ::= SEQUENCE
using conn_establishment_ind_s = elementary_procedure_option<conn_establishment_ind_ies_container>;

// DeactivateTraceIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct deactiv_trace_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, ngran_trace_id, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                       amf_ue_ngap_id();
    uint64_t&                       ran_ue_ngap_id();
    fixed_octstring<8, true>&       ngran_trace_id();
    const uint64_t&                 amf_ue_ngap_id() const;
    const uint64_t&                 ran_ue_ngap_id() const;
    const fixed_octstring<8, true>& ngran_trace_id() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct deactiv_trace_ies_container {
  uint64_t                 amf_ue_ngap_id;
  uint64_t                 ran_ue_ngap_id;
  fixed_octstring<8, true> ngran_trace_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DeactivateTrace ::= SEQUENCE
using deactiv_trace_s = elementary_procedure_option<deactiv_trace_ies_container>;

// DistributionReleaseRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct distribution_release_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mbs_session_id,
        mbs_area_session_id,
        mbs_distribution_release_request_transfer,
        cause,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&                mbs_session_id();
    uint32_t&                        mbs_area_session_id();
    unbounded_octstring<true>&       mbs_distribution_release_request_transfer();
    cause_c&                         cause();
    const mbs_session_id_s&          mbs_session_id() const;
    const uint32_t&                  mbs_area_session_id() const;
    const unbounded_octstring<true>& mbs_distribution_release_request_transfer() const;
    const cause_c&                   cause() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct distribution_release_request_ies_container {
  bool                      mbs_area_session_id_present = false;
  mbs_session_id_s          mbs_session_id;
  uint32_t                  mbs_area_session_id;
  unbounded_octstring<true> mbs_distribution_release_request_transfer;
  cause_c                   cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DistributionReleaseRequest ::= SEQUENCE
using distribution_release_request_s = elementary_procedure_option<distribution_release_request_ies_container>;

// DistributionReleaseResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct distribution_release_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, mbs_area_session_id, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&         mbs_session_id();
    uint32_t&                 mbs_area_session_id();
    crit_diagnostics_s&       crit_diagnostics();
    const mbs_session_id_s&   mbs_session_id() const;
    const uint32_t&           mbs_area_session_id() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct distribution_release_resp_ies_container {
  bool               mbs_area_session_id_present = false;
  bool               crit_diagnostics_present    = false;
  mbs_session_id_s   mbs_session_id;
  uint32_t           mbs_area_session_id;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DistributionReleaseResponse ::= SEQUENCE
using distribution_release_resp_s = elementary_procedure_option<distribution_release_resp_ies_container>;

// DistributionSetupFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct distribution_setup_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mbs_session_id,
        mbs_area_session_id,
        mbs_distribution_setup_unsuccessful_transfer,
        cause,
        crit_diagnostics,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&                mbs_session_id();
    uint32_t&                        mbs_area_session_id();
    unbounded_octstring<true>&       mbs_distribution_setup_unsuccessful_transfer();
    cause_c&                         cause();
    crit_diagnostics_s&              crit_diagnostics();
    const mbs_session_id_s&          mbs_session_id() const;
    const uint32_t&                  mbs_area_session_id() const;
    const unbounded_octstring<true>& mbs_distribution_setup_unsuccessful_transfer() const;
    const cause_c&                   cause() const;
    const crit_diagnostics_s&        crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct distribution_setup_fail_ies_container {
  bool                      mbs_area_session_id_present = false;
  bool                      crit_diagnostics_present    = false;
  mbs_session_id_s          mbs_session_id;
  uint32_t                  mbs_area_session_id;
  unbounded_octstring<true> mbs_distribution_setup_unsuccessful_transfer;
  cause_c                   cause;
  crit_diagnostics_s        crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DistributionSetupFailure ::= SEQUENCE
using distribution_setup_fail_s = elementary_procedure_option<distribution_setup_fail_ies_container>;

// DistributionSetupRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct distribution_setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, mbs_area_session_id, mbs_distribution_setup_request_transfer, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&                mbs_session_id();
    uint32_t&                        mbs_area_session_id();
    unbounded_octstring<true>&       mbs_distribution_setup_request_transfer();
    const mbs_session_id_s&          mbs_session_id() const;
    const uint32_t&                  mbs_area_session_id() const;
    const unbounded_octstring<true>& mbs_distribution_setup_request_transfer() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct distribution_setup_request_ies_container {
  bool                      mbs_area_session_id_present = false;
  mbs_session_id_s          mbs_session_id;
  uint32_t                  mbs_area_session_id;
  unbounded_octstring<true> mbs_distribution_setup_request_transfer;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DistributionSetupRequest ::= SEQUENCE
using distribution_setup_request_s = elementary_procedure_option<distribution_setup_request_ies_container>;

// DistributionSetupResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct distribution_setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mbs_session_id,
        mbs_area_session_id,
        mbs_distribution_setup_resp_transfer,
        crit_diagnostics,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&                mbs_session_id();
    uint32_t&                        mbs_area_session_id();
    unbounded_octstring<true>&       mbs_distribution_setup_resp_transfer();
    crit_diagnostics_s&              crit_diagnostics();
    const mbs_session_id_s&          mbs_session_id() const;
    const uint32_t&                  mbs_area_session_id() const;
    const unbounded_octstring<true>& mbs_distribution_setup_resp_transfer() const;
    const crit_diagnostics_s&        crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct distribution_setup_resp_ies_container {
  bool                      mbs_area_session_id_present = false;
  bool                      crit_diagnostics_present    = false;
  mbs_session_id_s          mbs_session_id;
  uint32_t                  mbs_area_session_id;
  unbounded_octstring<true> mbs_distribution_setup_resp_transfer;
  crit_diagnostics_s        crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DistributionSetupResponse ::= SEQUENCE
using distribution_setup_resp_s = elementary_procedure_option<distribution_setup_resp_ies_container>;

// DownlinkNASTransport-IEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct dl_nas_transport_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        old_amf,
        ran_paging_prio,
        nas_pdu,
        mob_restrict_list,
        idx_to_rfsp,
        ue_aggr_max_bit_rate,
        allowed_nssai,
        srvcc_operation_possible,
        enhanced_coverage_restrict,
        extended_connected_time,
        ue_diff_info,
        ce_mode_brestricted,
        ue_radio_cap,
        ue_cap_info_request,
        end_ind,
        ue_radio_cap_id,
        target_nssai_info,
        masked_imeisv,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                   amf_ue_ngap_id();
    uint64_t&                                   ran_ue_ngap_id();
    printable_string<1, 150, true, true>&       old_amf();
    uint16_t&                                   ran_paging_prio();
    unbounded_octstring<true>&                  nas_pdu();
    mob_restrict_list_s&                        mob_restrict_list();
    uint16_t&                                   idx_to_rfsp();
    ue_aggr_max_bit_rate_s&                     ue_aggr_max_bit_rate();
    allowed_nssai_l&                            allowed_nssai();
    srvcc_operation_possible_e&                 srvcc_operation_possible();
    enhanced_coverage_restrict_e&               enhanced_coverage_restrict();
    uint16_t&                                   extended_connected_time();
    ue_diff_info_s&                             ue_diff_info();
    ce_mode_brestricted_e&                      ce_mode_brestricted();
    unbounded_octstring<true>&                  ue_radio_cap();
    ue_cap_info_request_e&                      ue_cap_info_request();
    end_ind_e&                                  end_ind();
    unbounded_octstring<true>&                  ue_radio_cap_id();
    target_nssai_info_s&                        target_nssai_info();
    fixed_bitstring<64, false, true>&           masked_imeisv();
    const uint64_t&                             amf_ue_ngap_id() const;
    const uint64_t&                             ran_ue_ngap_id() const;
    const printable_string<1, 150, true, true>& old_amf() const;
    const uint16_t&                             ran_paging_prio() const;
    const unbounded_octstring<true>&            nas_pdu() const;
    const mob_restrict_list_s&                  mob_restrict_list() const;
    const uint16_t&                             idx_to_rfsp() const;
    const ue_aggr_max_bit_rate_s&               ue_aggr_max_bit_rate() const;
    const allowed_nssai_l&                      allowed_nssai() const;
    const srvcc_operation_possible_e&           srvcc_operation_possible() const;
    const enhanced_coverage_restrict_e&         enhanced_coverage_restrict() const;
    const uint16_t&                             extended_connected_time() const;
    const ue_diff_info_s&                       ue_diff_info() const;
    const ce_mode_brestricted_e&                ce_mode_brestricted() const;
    const unbounded_octstring<true>&            ue_radio_cap() const;
    const ue_cap_info_request_e&                ue_cap_info_request() const;
    const end_ind_e&                            end_ind() const;
    const unbounded_octstring<true>&            ue_radio_cap_id() const;
    const target_nssai_info_s&                  target_nssai_info() const;
    const fixed_bitstring<64, false, true>&     masked_imeisv() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct dl_nas_transport_ies_container {
  bool                                 old_amf_present                    = false;
  bool                                 ran_paging_prio_present            = false;
  bool                                 mob_restrict_list_present          = false;
  bool                                 idx_to_rfsp_present                = false;
  bool                                 ue_aggr_max_bit_rate_present       = false;
  bool                                 allowed_nssai_present              = false;
  bool                                 srvcc_operation_possible_present   = false;
  bool                                 enhanced_coverage_restrict_present = false;
  bool                                 extended_connected_time_present    = false;
  bool                                 ue_diff_info_present               = false;
  bool                                 ce_mode_brestricted_present        = false;
  bool                                 ue_radio_cap_present               = false;
  bool                                 ue_cap_info_request_present        = false;
  bool                                 end_ind_present                    = false;
  bool                                 ue_radio_cap_id_present            = false;
  bool                                 target_nssai_info_present          = false;
  bool                                 masked_imeisv_present              = false;
  uint64_t                             amf_ue_ngap_id;
  uint64_t                             ran_ue_ngap_id;
  printable_string<1, 150, true, true> old_amf;
  uint16_t                             ran_paging_prio;
  unbounded_octstring<true>            nas_pdu;
  mob_restrict_list_s                  mob_restrict_list;
  uint16_t                             idx_to_rfsp;
  ue_aggr_max_bit_rate_s               ue_aggr_max_bit_rate;
  allowed_nssai_l                      allowed_nssai;
  srvcc_operation_possible_e           srvcc_operation_possible;
  enhanced_coverage_restrict_e         enhanced_coverage_restrict;
  uint16_t                             extended_connected_time;
  ue_diff_info_s                       ue_diff_info;
  ce_mode_brestricted_e                ce_mode_brestricted;
  unbounded_octstring<true>            ue_radio_cap;
  ue_cap_info_request_e                ue_cap_info_request;
  end_ind_e                            end_ind;
  unbounded_octstring<true>            ue_radio_cap_id;
  target_nssai_info_s                  target_nssai_info;
  fixed_bitstring<64, false, true>     masked_imeisv;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkNASTransport ::= SEQUENCE
using dl_nas_transport_s = elementary_procedure_option<dl_nas_transport_ies_container>;

// DownlinkNonUEAssociatedNRPPaTransportIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct dl_non_ue_associated_nrppa_transport_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { routing_id, nrppa_pdu, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    unbounded_octstring<true>&       routing_id();
    unbounded_octstring<true>&       nrppa_pdu();
    const unbounded_octstring<true>& routing_id() const;
    const unbounded_octstring<true>& nrppa_pdu() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct dl_non_ue_associated_nrppa_transport_ies_container {
  unbounded_octstring<true> routing_id;
  unbounded_octstring<true> nrppa_pdu;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkNonUEAssociatedNRPPaTransport ::= SEQUENCE
using dl_non_ue_associated_nrppa_transport_s =
    elementary_procedure_option<dl_non_ue_associated_nrppa_transport_ies_container>;

// DownlinkRANConfigurationTransferIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct dl_ran_cfg_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { son_cfg_transfer_dl, endc_son_cfg_transfer_dl, intersys_son_cfg_transfer_dl, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    son_cfg_transfer_s&                son_cfg_transfer_dl();
    unbounded_octstring<true>&         endc_son_cfg_transfer_dl();
    intersys_son_cfg_transfer_s&       intersys_son_cfg_transfer_dl();
    const son_cfg_transfer_s&          son_cfg_transfer_dl() const;
    const unbounded_octstring<true>&   endc_son_cfg_transfer_dl() const;
    const intersys_son_cfg_transfer_s& intersys_son_cfg_transfer_dl() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct dl_ran_cfg_transfer_ies_container {
  bool                        son_cfg_transfer_dl_present          = false;
  bool                        endc_son_cfg_transfer_dl_present     = false;
  bool                        intersys_son_cfg_transfer_dl_present = false;
  son_cfg_transfer_s          son_cfg_transfer_dl;
  unbounded_octstring<true>   endc_son_cfg_transfer_dl;
  intersys_son_cfg_transfer_s intersys_son_cfg_transfer_dl;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkRANConfigurationTransfer ::= SEQUENCE
using dl_ran_cfg_transfer_s = elementary_procedure_option<dl_ran_cfg_transfer_ies_container>;

// DownlinkRANEarlyStatusTransferIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct dl_ran_early_status_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, early_status_transfer_transparent_container, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                            amf_ue_ngap_id();
    uint64_t&                                            ran_ue_ngap_id();
    early_status_transfer_transparent_container_s&       early_status_transfer_transparent_container();
    const uint64_t&                                      amf_ue_ngap_id() const;
    const uint64_t&                                      ran_ue_ngap_id() const;
    const early_status_transfer_transparent_container_s& early_status_transfer_transparent_container() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct dl_ran_early_status_transfer_ies_container {
  uint64_t                                      amf_ue_ngap_id;
  uint64_t                                      ran_ue_ngap_id;
  early_status_transfer_transparent_container_s early_status_transfer_transparent_container;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkRANEarlyStatusTransfer ::= SEQUENCE
using dl_ran_early_status_transfer_s = elementary_procedure_option<dl_ran_early_status_transfer_ies_container>;

// DownlinkRANStatusTransferIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct dl_ran_status_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, ran_status_transfer_transparent_container, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                          amf_ue_ngap_id();
    uint64_t&                                          ran_ue_ngap_id();
    ran_status_transfer_transparent_container_s&       ran_status_transfer_transparent_container();
    const uint64_t&                                    amf_ue_ngap_id() const;
    const uint64_t&                                    ran_ue_ngap_id() const;
    const ran_status_transfer_transparent_container_s& ran_status_transfer_transparent_container() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct dl_ran_status_transfer_ies_container {
  uint64_t                                    amf_ue_ngap_id;
  uint64_t                                    ran_ue_ngap_id;
  ran_status_transfer_transparent_container_s ran_status_transfer_transparent_container;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkRANStatusTransfer ::= SEQUENCE
using dl_ran_status_transfer_s = elementary_procedure_option<dl_ran_status_transfer_ies_container>;

// DownlinkRIMInformationTransferIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct dl_rim_info_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { rim_info_transfer, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::rim_info_transfer; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rim_info_transfer_s&       rim_info_transfer() { return c; }
    const rim_info_transfer_s& rim_info_transfer() const { return c; }

  private:
    rim_info_transfer_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DownlinkRIMInformationTransfer ::= SEQUENCE
using dl_rim_info_transfer_s = elementary_procedure_option<protocol_ie_container_l<dl_rim_info_transfer_ies_o>>;

// DownlinkUEAssociatedNRPPaTransportIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct dl_ue_associated_nrppa_transport_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, routing_id, nrppa_pdu, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                        amf_ue_ngap_id();
    uint64_t&                        ran_ue_ngap_id();
    unbounded_octstring<true>&       routing_id();
    unbounded_octstring<true>&       nrppa_pdu();
    const uint64_t&                  amf_ue_ngap_id() const;
    const uint64_t&                  ran_ue_ngap_id() const;
    const unbounded_octstring<true>& routing_id() const;
    const unbounded_octstring<true>& nrppa_pdu() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct dl_ue_associated_nrppa_transport_ies_container {
  uint64_t                  amf_ue_ngap_id;
  uint64_t                  ran_ue_ngap_id;
  unbounded_octstring<true> routing_id;
  unbounded_octstring<true> nrppa_pdu;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkUEAssociatedNRPPaTransport ::= SEQUENCE
using dl_ue_associated_nrppa_transport_s = elementary_procedure_option<dl_ue_associated_nrppa_transport_ies_container>;

// ErrorIndicationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct error_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, cause, crit_diagnostics, five_g_s_tmsi, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                 amf_ue_ngap_id();
    uint64_t&                 ran_ue_ngap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    five_g_s_tmsi_s&          five_g_s_tmsi();
    const uint64_t&           amf_ue_ngap_id() const;
    const uint64_t&           ran_ue_ngap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;
    const five_g_s_tmsi_s&    five_g_s_tmsi() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct error_ind_ies_container {
  bool               amf_ue_ngap_id_present   = false;
  bool               ran_ue_ngap_id_present   = false;
  bool               cause_present            = false;
  bool               crit_diagnostics_present = false;
  bool               five_g_s_tmsi_present    = false;
  uint64_t           amf_ue_ngap_id;
  uint64_t           ran_ue_ngap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;
  five_g_s_tmsi_s    five_g_s_tmsi;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ErrorIndication ::= SEQUENCE
using error_ind_s = elementary_procedure_option<error_ind_ies_container>;

// HandoverCancelIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ho_cancel_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, cause, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&       amf_ue_ngap_id();
    uint64_t&       ran_ue_ngap_id();
    cause_c&        cause();
    const uint64_t& amf_ue_ngap_id() const;
    const uint64_t& ran_ue_ngap_id() const;
    const cause_c&  cause() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct ho_cancel_ies_container {
  uint64_t amf_ue_ngap_id;
  uint64_t ran_ue_ngap_id;
  cause_c  cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverCancel ::= SEQUENCE
using ho_cancel_s = elementary_procedure_option<ho_cancel_ies_container>;

// HandoverCancelAcknowledgeIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ho_cancel_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                 amf_ue_ngap_id();
    uint64_t&                 ran_ue_ngap_id();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           amf_ue_ngap_id() const;
    const uint64_t&           ran_ue_ngap_id() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct ho_cancel_ack_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           amf_ue_ngap_id;
  uint64_t           ran_ue_ngap_id;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverCancelAcknowledge ::= SEQUENCE
using ho_cancel_ack_s = elementary_procedure_option<ho_cancel_ack_ies_container>;

// HandoverCommandIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ho_cmd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        handov_type,
        nas_security_params_from_ngran,
        pdu_session_res_ho_list,
        pdu_session_res_to_release_list_ho_cmd,
        target_to_source_transparent_container,
        crit_diagnostics,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                       amf_ue_ngap_id();
    uint64_t&                                       ran_ue_ngap_id();
    handov_type_e&                                  handov_type();
    unbounded_octstring<true>&                      nas_security_params_from_ngran();
    pdu_session_res_ho_list_l&                      pdu_session_res_ho_list();
    pdu_session_res_to_release_list_ho_cmd_l&       pdu_session_res_to_release_list_ho_cmd();
    unbounded_octstring<true>&                      target_to_source_transparent_container();
    crit_diagnostics_s&                             crit_diagnostics();
    const uint64_t&                                 amf_ue_ngap_id() const;
    const uint64_t&                                 ran_ue_ngap_id() const;
    const handov_type_e&                            handov_type() const;
    const unbounded_octstring<true>&                nas_security_params_from_ngran() const;
    const pdu_session_res_ho_list_l&                pdu_session_res_ho_list() const;
    const pdu_session_res_to_release_list_ho_cmd_l& pdu_session_res_to_release_list_ho_cmd() const;
    const unbounded_octstring<true>&                target_to_source_transparent_container() const;
    const crit_diagnostics_s&                       crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct ho_cmd_ies_container {
  bool                                     nas_security_params_from_ngran_present         = false;
  bool                                     pdu_session_res_ho_list_present                = false;
  bool                                     pdu_session_res_to_release_list_ho_cmd_present = false;
  bool                                     crit_diagnostics_present                       = false;
  uint64_t                                 amf_ue_ngap_id;
  uint64_t                                 ran_ue_ngap_id;
  handov_type_e                            handov_type;
  unbounded_octstring<true>                nas_security_params_from_ngran;
  pdu_session_res_ho_list_l                pdu_session_res_ho_list;
  pdu_session_res_to_release_list_ho_cmd_l pdu_session_res_to_release_list_ho_cmd;
  unbounded_octstring<true>                target_to_source_transparent_container;
  crit_diagnostics_s                       crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverCommand ::= SEQUENCE
using ho_cmd_s = elementary_procedure_option<ho_cmd_ies_container>;

// HandoverFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ho_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        cause,
        crit_diagnostics,
        targetto_source_fail_transparent_container,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                        amf_ue_ngap_id();
    cause_c&                         cause();
    crit_diagnostics_s&              crit_diagnostics();
    unbounded_octstring<true>&       targetto_source_fail_transparent_container();
    const uint64_t&                  amf_ue_ngap_id() const;
    const cause_c&                   cause() const;
    const crit_diagnostics_s&        crit_diagnostics() const;
    const unbounded_octstring<true>& targetto_source_fail_transparent_container() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct ho_fail_ies_container {
  bool                      crit_diagnostics_present                           = false;
  bool                      targetto_source_fail_transparent_container_present = false;
  uint64_t                  amf_ue_ngap_id;
  cause_c                   cause;
  crit_diagnostics_s        crit_diagnostics;
  unbounded_octstring<true> targetto_source_fail_transparent_container;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverFailure ::= SEQUENCE
using ho_fail_s = elementary_procedure_option<ho_fail_ies_container>;

// HandoverNotifyIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ho_notify_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, user_location_info, notify_source_ngran_node, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                         amf_ue_ngap_id();
    uint64_t&                         ran_ue_ngap_id();
    user_location_info_c&             user_location_info();
    notify_source_ngran_node_e&       notify_source_ngran_node();
    const uint64_t&                   amf_ue_ngap_id() const;
    const uint64_t&                   ran_ue_ngap_id() const;
    const user_location_info_c&       user_location_info() const;
    const notify_source_ngran_node_e& notify_source_ngran_node() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct ho_notify_ies_container {
  bool                       notify_source_ngran_node_present = false;
  uint64_t                   amf_ue_ngap_id;
  uint64_t                   ran_ue_ngap_id;
  user_location_info_c       user_location_info;
  notify_source_ngran_node_e notify_source_ngran_node;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverNotify ::= SEQUENCE
using ho_notify_s = elementary_procedure_option<ho_notify_ies_container>;

// HandoverPreparationFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ho_prep_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        cause,
        crit_diagnostics,
        targetto_source_fail_transparent_container,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                        amf_ue_ngap_id();
    uint64_t&                        ran_ue_ngap_id();
    cause_c&                         cause();
    crit_diagnostics_s&              crit_diagnostics();
    unbounded_octstring<true>&       targetto_source_fail_transparent_container();
    const uint64_t&                  amf_ue_ngap_id() const;
    const uint64_t&                  ran_ue_ngap_id() const;
    const cause_c&                   cause() const;
    const crit_diagnostics_s&        crit_diagnostics() const;
    const unbounded_octstring<true>& targetto_source_fail_transparent_container() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct ho_prep_fail_ies_container {
  bool                      crit_diagnostics_present                           = false;
  bool                      targetto_source_fail_transparent_container_present = false;
  uint64_t                  amf_ue_ngap_id;
  uint64_t                  ran_ue_ngap_id;
  cause_c                   cause;
  crit_diagnostics_s        crit_diagnostics;
  unbounded_octstring<true> targetto_source_fail_transparent_container;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationFailure ::= SEQUENCE
using ho_prep_fail_s = elementary_procedure_option<ho_prep_fail_ies_container>;

// HandoverRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ho_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        handov_type,
        cause,
        ue_aggr_max_bit_rate,
        core_network_assist_info_for_inactive,
        ue_security_cap,
        security_context,
        new_security_context_ind,
        nasc,
        pdu_session_res_setup_list_ho_req,
        allowed_nssai,
        trace_activation,
        masked_imeisv,
        source_to_target_transparent_container,
        mob_restrict_list,
        location_report_request_type,
        rrc_inactive_transition_report_request,
        guami,
        redirection_voice_fallback,
        cn_assisted_ran_tuning,
        srvcc_operation_possible,
        iab_authorized,
        enhanced_coverage_restrict,
        ue_diff_info,
        nr_v2x_services_authorized,
        ltev2x_services_authorized,
        nr_ue_sidelink_aggr_max_bitrate,
        lte_ue_sidelink_aggr_max_bitrate,
        pc5_qos_params,
        ce_mode_brestricted,
        ue_up_c_iot_support,
        management_based_mdt_plmn_list,
        ue_radio_cap_id,
        extended_connected_time,
        time_sync_assist_info,
        ue_slice_max_bit_rate_list,
        five_g_pro_se_authorized,
        five_g_pro_se_ue_pc5_aggr_max_bit_rate,
        five_g_pro_se_pc5_qos_params,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                       amf_ue_ngap_id();
    handov_type_e&                                  handov_type();
    cause_c&                                        cause();
    ue_aggr_max_bit_rate_s&                         ue_aggr_max_bit_rate();
    core_network_assist_info_for_inactive_s&        core_network_assist_info_for_inactive();
    ue_security_cap_s&                              ue_security_cap();
    security_context_s&                             security_context();
    new_security_context_ind_e&                     new_security_context_ind();
    unbounded_octstring<true>&                      nasc();
    pdu_session_res_setup_list_ho_req_l&            pdu_session_res_setup_list_ho_req();
    allowed_nssai_l&                                allowed_nssai();
    trace_activation_s&                             trace_activation();
    fixed_bitstring<64, false, true>&               masked_imeisv();
    unbounded_octstring<true>&                      source_to_target_transparent_container();
    mob_restrict_list_s&                            mob_restrict_list();
    location_report_request_type_s&                 location_report_request_type();
    rrc_inactive_transition_report_request_e&       rrc_inactive_transition_report_request();
    guami_s&                                        guami();
    redirection_voice_fallback_e&                   redirection_voice_fallback();
    cn_assisted_ran_tuning_s&                       cn_assisted_ran_tuning();
    srvcc_operation_possible_e&                     srvcc_operation_possible();
    iab_authorized_e&                               iab_authorized();
    enhanced_coverage_restrict_e&                   enhanced_coverage_restrict();
    ue_diff_info_s&                                 ue_diff_info();
    nr_v2x_services_authorized_s&                   nr_v2x_services_authorized();
    ltev2x_services_authorized_s&                   ltev2x_services_authorized();
    nr_ue_sidelink_aggr_max_bitrate_s&              nr_ue_sidelink_aggr_max_bitrate();
    lte_ue_sidelink_aggr_max_bitrate_s&             lte_ue_sidelink_aggr_max_bitrate();
    pc5_qos_params_s&                               pc5_qos_params();
    ce_mode_brestricted_e&                          ce_mode_brestricted();
    ue_up_c_iot_support_e&                          ue_up_c_iot_support();
    mdt_plmn_list_l&                                management_based_mdt_plmn_list();
    unbounded_octstring<true>&                      ue_radio_cap_id();
    uint16_t&                                       extended_connected_time();
    time_sync_assist_info_s&                        time_sync_assist_info();
    ue_slice_max_bit_rate_list_l&                   ue_slice_max_bit_rate_list();
    five_g_pro_se_authorized_s&                     five_g_pro_se_authorized();
    nr_ue_sidelink_aggr_max_bitrate_s&              five_g_pro_se_ue_pc5_aggr_max_bit_rate();
    five_g_pro_se_pc5_qos_params_s&                 five_g_pro_se_pc5_qos_params();
    const uint64_t&                                 amf_ue_ngap_id() const;
    const handov_type_e&                            handov_type() const;
    const cause_c&                                  cause() const;
    const ue_aggr_max_bit_rate_s&                   ue_aggr_max_bit_rate() const;
    const core_network_assist_info_for_inactive_s&  core_network_assist_info_for_inactive() const;
    const ue_security_cap_s&                        ue_security_cap() const;
    const security_context_s&                       security_context() const;
    const new_security_context_ind_e&               new_security_context_ind() const;
    const unbounded_octstring<true>&                nasc() const;
    const pdu_session_res_setup_list_ho_req_l&      pdu_session_res_setup_list_ho_req() const;
    const allowed_nssai_l&                          allowed_nssai() const;
    const trace_activation_s&                       trace_activation() const;
    const fixed_bitstring<64, false, true>&         masked_imeisv() const;
    const unbounded_octstring<true>&                source_to_target_transparent_container() const;
    const mob_restrict_list_s&                      mob_restrict_list() const;
    const location_report_request_type_s&           location_report_request_type() const;
    const rrc_inactive_transition_report_request_e& rrc_inactive_transition_report_request() const;
    const guami_s&                                  guami() const;
    const redirection_voice_fallback_e&             redirection_voice_fallback() const;
    const cn_assisted_ran_tuning_s&                 cn_assisted_ran_tuning() const;
    const srvcc_operation_possible_e&               srvcc_operation_possible() const;
    const iab_authorized_e&                         iab_authorized() const;
    const enhanced_coverage_restrict_e&             enhanced_coverage_restrict() const;
    const ue_diff_info_s&                           ue_diff_info() const;
    const nr_v2x_services_authorized_s&             nr_v2x_services_authorized() const;
    const ltev2x_services_authorized_s&             ltev2x_services_authorized() const;
    const nr_ue_sidelink_aggr_max_bitrate_s&        nr_ue_sidelink_aggr_max_bitrate() const;
    const lte_ue_sidelink_aggr_max_bitrate_s&       lte_ue_sidelink_aggr_max_bitrate() const;
    const pc5_qos_params_s&                         pc5_qos_params() const;
    const ce_mode_brestricted_e&                    ce_mode_brestricted() const;
    const ue_up_c_iot_support_e&                    ue_up_c_iot_support() const;
    const mdt_plmn_list_l&                          management_based_mdt_plmn_list() const;
    const unbounded_octstring<true>&                ue_radio_cap_id() const;
    const uint16_t&                                 extended_connected_time() const;
    const time_sync_assist_info_s&                  time_sync_assist_info() const;
    const ue_slice_max_bit_rate_list_l&             ue_slice_max_bit_rate_list() const;
    const five_g_pro_se_authorized_s&               five_g_pro_se_authorized() const;
    const nr_ue_sidelink_aggr_max_bitrate_s&        five_g_pro_se_ue_pc5_aggr_max_bit_rate() const;
    const five_g_pro_se_pc5_qos_params_s&           five_g_pro_se_pc5_qos_params() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct ho_request_ies_container {
  bool                                     core_network_assist_info_for_inactive_present  = false;
  bool                                     new_security_context_ind_present               = false;
  bool                                     nasc_present                                   = false;
  bool                                     trace_activation_present                       = false;
  bool                                     masked_imeisv_present                          = false;
  bool                                     mob_restrict_list_present                      = false;
  bool                                     location_report_request_type_present           = false;
  bool                                     rrc_inactive_transition_report_request_present = false;
  bool                                     redirection_voice_fallback_present             = false;
  bool                                     cn_assisted_ran_tuning_present                 = false;
  bool                                     srvcc_operation_possible_present               = false;
  bool                                     iab_authorized_present                         = false;
  bool                                     enhanced_coverage_restrict_present             = false;
  bool                                     ue_diff_info_present                           = false;
  bool                                     nr_v2x_services_authorized_present             = false;
  bool                                     ltev2x_services_authorized_present             = false;
  bool                                     nr_ue_sidelink_aggr_max_bitrate_present        = false;
  bool                                     lte_ue_sidelink_aggr_max_bitrate_present       = false;
  bool                                     pc5_qos_params_present                         = false;
  bool                                     ce_mode_brestricted_present                    = false;
  bool                                     ue_up_c_iot_support_present                    = false;
  bool                                     management_based_mdt_plmn_list_present         = false;
  bool                                     ue_radio_cap_id_present                        = false;
  bool                                     extended_connected_time_present                = false;
  bool                                     time_sync_assist_info_present                  = false;
  bool                                     ue_slice_max_bit_rate_list_present             = false;
  bool                                     five_g_pro_se_authorized_present               = false;
  bool                                     five_g_pro_se_ue_pc5_aggr_max_bit_rate_present = false;
  bool                                     five_g_pro_se_pc5_qos_params_present           = false;
  uint64_t                                 amf_ue_ngap_id;
  handov_type_e                            handov_type;
  cause_c                                  cause;
  ue_aggr_max_bit_rate_s                   ue_aggr_max_bit_rate;
  core_network_assist_info_for_inactive_s  core_network_assist_info_for_inactive;
  ue_security_cap_s                        ue_security_cap;
  security_context_s                       security_context;
  new_security_context_ind_e               new_security_context_ind;
  unbounded_octstring<true>                nasc;
  pdu_session_res_setup_list_ho_req_l      pdu_session_res_setup_list_ho_req;
  allowed_nssai_l                          allowed_nssai;
  trace_activation_s                       trace_activation;
  fixed_bitstring<64, false, true>         masked_imeisv;
  unbounded_octstring<true>                source_to_target_transparent_container;
  mob_restrict_list_s                      mob_restrict_list;
  location_report_request_type_s           location_report_request_type;
  rrc_inactive_transition_report_request_e rrc_inactive_transition_report_request;
  guami_s                                  guami;
  redirection_voice_fallback_e             redirection_voice_fallback;
  cn_assisted_ran_tuning_s                 cn_assisted_ran_tuning;
  srvcc_operation_possible_e               srvcc_operation_possible;
  iab_authorized_e                         iab_authorized;
  enhanced_coverage_restrict_e             enhanced_coverage_restrict;
  ue_diff_info_s                           ue_diff_info;
  nr_v2x_services_authorized_s             nr_v2x_services_authorized;
  ltev2x_services_authorized_s             ltev2x_services_authorized;
  nr_ue_sidelink_aggr_max_bitrate_s        nr_ue_sidelink_aggr_max_bitrate;
  lte_ue_sidelink_aggr_max_bitrate_s       lte_ue_sidelink_aggr_max_bitrate;
  pc5_qos_params_s                         pc5_qos_params;
  ce_mode_brestricted_e                    ce_mode_brestricted;
  ue_up_c_iot_support_e                    ue_up_c_iot_support;
  mdt_plmn_list_l                          management_based_mdt_plmn_list;
  unbounded_octstring<true>                ue_radio_cap_id;
  uint16_t                                 extended_connected_time;
  time_sync_assist_info_s                  time_sync_assist_info;
  ue_slice_max_bit_rate_list_l             ue_slice_max_bit_rate_list;
  five_g_pro_se_authorized_s               five_g_pro_se_authorized;
  nr_ue_sidelink_aggr_max_bitrate_s        five_g_pro_se_ue_pc5_aggr_max_bit_rate;
  five_g_pro_se_pc5_qos_params_s           five_g_pro_se_pc5_qos_params;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverRequest ::= SEQUENCE
using ho_request_s = elementary_procedure_option<ho_request_ies_container>;

// HandoverRequestAcknowledgeIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ho_request_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_admitted_list,
        pdu_session_res_failed_to_setup_list_ho_ack,
        target_to_source_transparent_container,
        crit_diagnostics,
        npn_access_info,
        red_cap_ind,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                            amf_ue_ngap_id();
    uint64_t&                                            ran_ue_ngap_id();
    pdu_session_res_admitted_list_l&                     pdu_session_res_admitted_list();
    pdu_session_res_failed_to_setup_list_ho_ack_l&       pdu_session_res_failed_to_setup_list_ho_ack();
    unbounded_octstring<true>&                           target_to_source_transparent_container();
    crit_diagnostics_s&                                  crit_diagnostics();
    npn_access_info_c&                                   npn_access_info();
    red_cap_ind_e&                                       red_cap_ind();
    const uint64_t&                                      amf_ue_ngap_id() const;
    const uint64_t&                                      ran_ue_ngap_id() const;
    const pdu_session_res_admitted_list_l&               pdu_session_res_admitted_list() const;
    const pdu_session_res_failed_to_setup_list_ho_ack_l& pdu_session_res_failed_to_setup_list_ho_ack() const;
    const unbounded_octstring<true>&                     target_to_source_transparent_container() const;
    const crit_diagnostics_s&                            crit_diagnostics() const;
    const npn_access_info_c&                             npn_access_info() const;
    const red_cap_ind_e&                                 red_cap_ind() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct ho_request_ack_ies_container {
  bool                                          pdu_session_res_failed_to_setup_list_ho_ack_present = false;
  bool                                          crit_diagnostics_present                            = false;
  bool                                          npn_access_info_present                             = false;
  bool                                          red_cap_ind_present                                 = false;
  uint64_t                                      amf_ue_ngap_id;
  uint64_t                                      ran_ue_ngap_id;
  pdu_session_res_admitted_list_l               pdu_session_res_admitted_list;
  pdu_session_res_failed_to_setup_list_ho_ack_l pdu_session_res_failed_to_setup_list_ho_ack;
  unbounded_octstring<true>                     target_to_source_transparent_container;
  crit_diagnostics_s                            crit_diagnostics;
  npn_access_info_c                             npn_access_info;
  red_cap_ind_e                                 red_cap_ind;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverRequestAcknowledge ::= SEQUENCE
using ho_request_ack_s = elementary_procedure_option<ho_request_ack_ies_container>;

// HandoverRequiredIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ho_required_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        handov_type,
        cause,
        target_id,
        direct_forwarding_path_availability,
        pdu_session_res_list_ho_rqd,
        source_to_target_transparent_container,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                    amf_ue_ngap_id();
    uint64_t&                                    ran_ue_ngap_id();
    handov_type_e&                               handov_type();
    cause_c&                                     cause();
    target_id_c&                                 target_id();
    direct_forwarding_path_availability_e&       direct_forwarding_path_availability();
    pdu_session_res_list_ho_rqd_l&               pdu_session_res_list_ho_rqd();
    unbounded_octstring<true>&                   source_to_target_transparent_container();
    const uint64_t&                              amf_ue_ngap_id() const;
    const uint64_t&                              ran_ue_ngap_id() const;
    const handov_type_e&                         handov_type() const;
    const cause_c&                               cause() const;
    const target_id_c&                           target_id() const;
    const direct_forwarding_path_availability_e& direct_forwarding_path_availability() const;
    const pdu_session_res_list_ho_rqd_l&         pdu_session_res_list_ho_rqd() const;
    const unbounded_octstring<true>&             source_to_target_transparent_container() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct ho_required_ies_container {
  bool                                  direct_forwarding_path_availability_present = false;
  uint64_t                              amf_ue_ngap_id;
  uint64_t                              ran_ue_ngap_id;
  handov_type_e                         handov_type;
  cause_c                               cause;
  target_id_c                           target_id;
  direct_forwarding_path_availability_e direct_forwarding_path_availability;
  pdu_session_res_list_ho_rqd_l         pdu_session_res_list_ho_rqd;
  unbounded_octstring<true>             source_to_target_transparent_container;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverRequired ::= SEQUENCE
using ho_required_s = elementary_procedure_option<ho_required_ies_container>;

// HandoverSuccessIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ho_success_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&       amf_ue_ngap_id();
    uint64_t&       ran_ue_ngap_id();
    const uint64_t& amf_ue_ngap_id() const;
    const uint64_t& ran_ue_ngap_id() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct ho_success_ies_container {
  uint64_t amf_ue_ngap_id;
  uint64_t ran_ue_ngap_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverSuccess ::= SEQUENCE
using ho_success_s = elementary_procedure_option<ho_success_ies_container>;

// InitialContextSetupFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct init_context_setup_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_failed_to_setup_list_cxt_fail,
        cause,
        crit_diagnostics,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                              amf_ue_ngap_id();
    uint64_t&                                              ran_ue_ngap_id();
    pdu_session_res_failed_to_setup_list_cxt_fail_l&       pdu_session_res_failed_to_setup_list_cxt_fail();
    cause_c&                                               cause();
    crit_diagnostics_s&                                    crit_diagnostics();
    const uint64_t&                                        amf_ue_ngap_id() const;
    const uint64_t&                                        ran_ue_ngap_id() const;
    const pdu_session_res_failed_to_setup_list_cxt_fail_l& pdu_session_res_failed_to_setup_list_cxt_fail() const;
    const cause_c&                                         cause() const;
    const crit_diagnostics_s&                              crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct init_context_setup_fail_ies_container {
  bool                                            pdu_session_res_failed_to_setup_list_cxt_fail_present = false;
  bool                                            crit_diagnostics_present                              = false;
  uint64_t                                        amf_ue_ngap_id;
  uint64_t                                        ran_ue_ngap_id;
  pdu_session_res_failed_to_setup_list_cxt_fail_l pdu_session_res_failed_to_setup_list_cxt_fail;
  cause_c                                         cause;
  crit_diagnostics_s                              crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InitialContextSetupFailure ::= SEQUENCE
using init_context_setup_fail_s = elementary_procedure_option<init_context_setup_fail_ies_container>;

// InitialContextSetupRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct init_context_setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        old_amf,
        ue_aggr_max_bit_rate,
        core_network_assist_info_for_inactive,
        guami,
        pdu_session_res_setup_list_cxt_req,
        allowed_nssai,
        ue_security_cap,
        security_key,
        trace_activation,
        mob_restrict_list,
        ue_radio_cap,
        idx_to_rfsp,
        masked_imeisv,
        nas_pdu,
        emergency_fallback_ind,
        rrc_inactive_transition_report_request,
        ue_radio_cap_for_paging,
        redirection_voice_fallback,
        location_report_request_type,
        cn_assisted_ran_tuning,
        srvcc_operation_possible,
        iab_authorized,
        enhanced_coverage_restrict,
        extended_connected_time,
        ue_diff_info,
        nr_v2x_services_authorized,
        ltev2x_services_authorized,
        nr_ue_sidelink_aggr_max_bitrate,
        lte_ue_sidelink_aggr_max_bitrate,
        pc5_qos_params,
        ce_mode_brestricted,
        ue_up_c_iot_support,
        rg_level_wireline_access_characteristics,
        management_based_mdt_plmn_list,
        ue_radio_cap_id,
        time_sync_assist_info,
        q_mcc_onfig_info,
        target_nssai_info,
        ue_slice_max_bit_rate_list,
        five_g_pro_se_authorized,
        five_g_pro_se_ue_pc5_aggr_max_bit_rate,
        five_g_pro_se_pc5_qos_params,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                       amf_ue_ngap_id();
    uint64_t&                                       ran_ue_ngap_id();
    printable_string<1, 150, true, true>&           old_amf();
    ue_aggr_max_bit_rate_s&                         ue_aggr_max_bit_rate();
    core_network_assist_info_for_inactive_s&        core_network_assist_info_for_inactive();
    guami_s&                                        guami();
    pdu_session_res_setup_list_cxt_req_l&           pdu_session_res_setup_list_cxt_req();
    allowed_nssai_l&                                allowed_nssai();
    ue_security_cap_s&                              ue_security_cap();
    fixed_bitstring<256, false, true>&              security_key();
    trace_activation_s&                             trace_activation();
    mob_restrict_list_s&                            mob_restrict_list();
    unbounded_octstring<true>&                      ue_radio_cap();
    uint16_t&                                       idx_to_rfsp();
    fixed_bitstring<64, false, true>&               masked_imeisv();
    unbounded_octstring<true>&                      nas_pdu();
    emergency_fallback_ind_s&                       emergency_fallback_ind();
    rrc_inactive_transition_report_request_e&       rrc_inactive_transition_report_request();
    ue_radio_cap_for_paging_s&                      ue_radio_cap_for_paging();
    redirection_voice_fallback_e&                   redirection_voice_fallback();
    location_report_request_type_s&                 location_report_request_type();
    cn_assisted_ran_tuning_s&                       cn_assisted_ran_tuning();
    srvcc_operation_possible_e&                     srvcc_operation_possible();
    iab_authorized_e&                               iab_authorized();
    enhanced_coverage_restrict_e&                   enhanced_coverage_restrict();
    uint16_t&                                       extended_connected_time();
    ue_diff_info_s&                                 ue_diff_info();
    nr_v2x_services_authorized_s&                   nr_v2x_services_authorized();
    ltev2x_services_authorized_s&                   ltev2x_services_authorized();
    nr_ue_sidelink_aggr_max_bitrate_s&              nr_ue_sidelink_aggr_max_bitrate();
    lte_ue_sidelink_aggr_max_bitrate_s&             lte_ue_sidelink_aggr_max_bitrate();
    pc5_qos_params_s&                               pc5_qos_params();
    ce_mode_brestricted_e&                          ce_mode_brestricted();
    ue_up_c_iot_support_e&                          ue_up_c_iot_support();
    unbounded_octstring<true>&                      rg_level_wireline_access_characteristics();
    mdt_plmn_list_l&                                management_based_mdt_plmn_list();
    unbounded_octstring<true>&                      ue_radio_cap_id();
    time_sync_assist_info_s&                        time_sync_assist_info();
    q_mcc_onfig_info_s&                             q_mcc_onfig_info();
    target_nssai_info_s&                            target_nssai_info();
    ue_slice_max_bit_rate_list_l&                   ue_slice_max_bit_rate_list();
    five_g_pro_se_authorized_s&                     five_g_pro_se_authorized();
    nr_ue_sidelink_aggr_max_bitrate_s&              five_g_pro_se_ue_pc5_aggr_max_bit_rate();
    five_g_pro_se_pc5_qos_params_s&                 five_g_pro_se_pc5_qos_params();
    const uint64_t&                                 amf_ue_ngap_id() const;
    const uint64_t&                                 ran_ue_ngap_id() const;
    const printable_string<1, 150, true, true>&     old_amf() const;
    const ue_aggr_max_bit_rate_s&                   ue_aggr_max_bit_rate() const;
    const core_network_assist_info_for_inactive_s&  core_network_assist_info_for_inactive() const;
    const guami_s&                                  guami() const;
    const pdu_session_res_setup_list_cxt_req_l&     pdu_session_res_setup_list_cxt_req() const;
    const allowed_nssai_l&                          allowed_nssai() const;
    const ue_security_cap_s&                        ue_security_cap() const;
    const fixed_bitstring<256, false, true>&        security_key() const;
    const trace_activation_s&                       trace_activation() const;
    const mob_restrict_list_s&                      mob_restrict_list() const;
    const unbounded_octstring<true>&                ue_radio_cap() const;
    const uint16_t&                                 idx_to_rfsp() const;
    const fixed_bitstring<64, false, true>&         masked_imeisv() const;
    const unbounded_octstring<true>&                nas_pdu() const;
    const emergency_fallback_ind_s&                 emergency_fallback_ind() const;
    const rrc_inactive_transition_report_request_e& rrc_inactive_transition_report_request() const;
    const ue_radio_cap_for_paging_s&                ue_radio_cap_for_paging() const;
    const redirection_voice_fallback_e&             redirection_voice_fallback() const;
    const location_report_request_type_s&           location_report_request_type() const;
    const cn_assisted_ran_tuning_s&                 cn_assisted_ran_tuning() const;
    const srvcc_operation_possible_e&               srvcc_operation_possible() const;
    const iab_authorized_e&                         iab_authorized() const;
    const enhanced_coverage_restrict_e&             enhanced_coverage_restrict() const;
    const uint16_t&                                 extended_connected_time() const;
    const ue_diff_info_s&                           ue_diff_info() const;
    const nr_v2x_services_authorized_s&             nr_v2x_services_authorized() const;
    const ltev2x_services_authorized_s&             ltev2x_services_authorized() const;
    const nr_ue_sidelink_aggr_max_bitrate_s&        nr_ue_sidelink_aggr_max_bitrate() const;
    const lte_ue_sidelink_aggr_max_bitrate_s&       lte_ue_sidelink_aggr_max_bitrate() const;
    const pc5_qos_params_s&                         pc5_qos_params() const;
    const ce_mode_brestricted_e&                    ce_mode_brestricted() const;
    const ue_up_c_iot_support_e&                    ue_up_c_iot_support() const;
    const unbounded_octstring<true>&                rg_level_wireline_access_characteristics() const;
    const mdt_plmn_list_l&                          management_based_mdt_plmn_list() const;
    const unbounded_octstring<true>&                ue_radio_cap_id() const;
    const time_sync_assist_info_s&                  time_sync_assist_info() const;
    const q_mcc_onfig_info_s&                       q_mcc_onfig_info() const;
    const target_nssai_info_s&                      target_nssai_info() const;
    const ue_slice_max_bit_rate_list_l&             ue_slice_max_bit_rate_list() const;
    const five_g_pro_se_authorized_s&               five_g_pro_se_authorized() const;
    const nr_ue_sidelink_aggr_max_bitrate_s&        five_g_pro_se_ue_pc5_aggr_max_bit_rate() const;
    const five_g_pro_se_pc5_qos_params_s&           five_g_pro_se_pc5_qos_params() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct init_context_setup_request_ies_container {
  bool                                     old_amf_present                                  = false;
  bool                                     ue_aggr_max_bit_rate_present                     = false;
  bool                                     core_network_assist_info_for_inactive_present    = false;
  bool                                     pdu_session_res_setup_list_cxt_req_present       = false;
  bool                                     trace_activation_present                         = false;
  bool                                     mob_restrict_list_present                        = false;
  bool                                     ue_radio_cap_present                             = false;
  bool                                     idx_to_rfsp_present                              = false;
  bool                                     masked_imeisv_present                            = false;
  bool                                     nas_pdu_present                                  = false;
  bool                                     emergency_fallback_ind_present                   = false;
  bool                                     rrc_inactive_transition_report_request_present   = false;
  bool                                     ue_radio_cap_for_paging_present                  = false;
  bool                                     redirection_voice_fallback_present               = false;
  bool                                     location_report_request_type_present             = false;
  bool                                     cn_assisted_ran_tuning_present                   = false;
  bool                                     srvcc_operation_possible_present                 = false;
  bool                                     iab_authorized_present                           = false;
  bool                                     enhanced_coverage_restrict_present               = false;
  bool                                     extended_connected_time_present                  = false;
  bool                                     ue_diff_info_present                             = false;
  bool                                     nr_v2x_services_authorized_present               = false;
  bool                                     ltev2x_services_authorized_present               = false;
  bool                                     nr_ue_sidelink_aggr_max_bitrate_present          = false;
  bool                                     lte_ue_sidelink_aggr_max_bitrate_present         = false;
  bool                                     pc5_qos_params_present                           = false;
  bool                                     ce_mode_brestricted_present                      = false;
  bool                                     ue_up_c_iot_support_present                      = false;
  bool                                     rg_level_wireline_access_characteristics_present = false;
  bool                                     management_based_mdt_plmn_list_present           = false;
  bool                                     ue_radio_cap_id_present                          = false;
  bool                                     time_sync_assist_info_present                    = false;
  bool                                     q_mcc_onfig_info_present                         = false;
  bool                                     target_nssai_info_present                        = false;
  bool                                     ue_slice_max_bit_rate_list_present               = false;
  bool                                     five_g_pro_se_authorized_present                 = false;
  bool                                     five_g_pro_se_ue_pc5_aggr_max_bit_rate_present   = false;
  bool                                     five_g_pro_se_pc5_qos_params_present             = false;
  uint64_t                                 amf_ue_ngap_id;
  uint64_t                                 ran_ue_ngap_id;
  printable_string<1, 150, true, true>     old_amf;
  ue_aggr_max_bit_rate_s                   ue_aggr_max_bit_rate;
  core_network_assist_info_for_inactive_s  core_network_assist_info_for_inactive;
  guami_s                                  guami;
  pdu_session_res_setup_list_cxt_req_l     pdu_session_res_setup_list_cxt_req;
  allowed_nssai_l                          allowed_nssai;
  ue_security_cap_s                        ue_security_cap;
  fixed_bitstring<256, false, true>        security_key;
  trace_activation_s                       trace_activation;
  mob_restrict_list_s                      mob_restrict_list;
  unbounded_octstring<true>                ue_radio_cap;
  uint16_t                                 idx_to_rfsp;
  fixed_bitstring<64, false, true>         masked_imeisv;
  unbounded_octstring<true>                nas_pdu;
  emergency_fallback_ind_s                 emergency_fallback_ind;
  rrc_inactive_transition_report_request_e rrc_inactive_transition_report_request;
  ue_radio_cap_for_paging_s                ue_radio_cap_for_paging;
  redirection_voice_fallback_e             redirection_voice_fallback;
  location_report_request_type_s           location_report_request_type;
  cn_assisted_ran_tuning_s                 cn_assisted_ran_tuning;
  srvcc_operation_possible_e               srvcc_operation_possible;
  iab_authorized_e                         iab_authorized;
  enhanced_coverage_restrict_e             enhanced_coverage_restrict;
  uint16_t                                 extended_connected_time;
  ue_diff_info_s                           ue_diff_info;
  nr_v2x_services_authorized_s             nr_v2x_services_authorized;
  ltev2x_services_authorized_s             ltev2x_services_authorized;
  nr_ue_sidelink_aggr_max_bitrate_s        nr_ue_sidelink_aggr_max_bitrate;
  lte_ue_sidelink_aggr_max_bitrate_s       lte_ue_sidelink_aggr_max_bitrate;
  pc5_qos_params_s                         pc5_qos_params;
  ce_mode_brestricted_e                    ce_mode_brestricted;
  ue_up_c_iot_support_e                    ue_up_c_iot_support;
  unbounded_octstring<true>                rg_level_wireline_access_characteristics;
  mdt_plmn_list_l                          management_based_mdt_plmn_list;
  unbounded_octstring<true>                ue_radio_cap_id;
  time_sync_assist_info_s                  time_sync_assist_info;
  q_mcc_onfig_info_s                       q_mcc_onfig_info;
  target_nssai_info_s                      target_nssai_info;
  ue_slice_max_bit_rate_list_l             ue_slice_max_bit_rate_list;
  five_g_pro_se_authorized_s               five_g_pro_se_authorized;
  nr_ue_sidelink_aggr_max_bitrate_s        five_g_pro_se_ue_pc5_aggr_max_bit_rate;
  five_g_pro_se_pc5_qos_params_s           five_g_pro_se_pc5_qos_params;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InitialContextSetupRequest ::= SEQUENCE
using init_context_setup_request_s = elementary_procedure_option<init_context_setup_request_ies_container>;

// InitialContextSetupResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct init_context_setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_setup_list_cxt_res,
        pdu_session_res_failed_to_setup_list_cxt_res,
        crit_diagnostics,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                             amf_ue_ngap_id();
    uint64_t&                                             ran_ue_ngap_id();
    pdu_session_res_setup_list_cxt_res_l&                 pdu_session_res_setup_list_cxt_res();
    pdu_session_res_failed_to_setup_list_cxt_res_l&       pdu_session_res_failed_to_setup_list_cxt_res();
    crit_diagnostics_s&                                   crit_diagnostics();
    const uint64_t&                                       amf_ue_ngap_id() const;
    const uint64_t&                                       ran_ue_ngap_id() const;
    const pdu_session_res_setup_list_cxt_res_l&           pdu_session_res_setup_list_cxt_res() const;
    const pdu_session_res_failed_to_setup_list_cxt_res_l& pdu_session_res_failed_to_setup_list_cxt_res() const;
    const crit_diagnostics_s&                             crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct init_context_setup_resp_ies_container {
  bool                                           pdu_session_res_setup_list_cxt_res_present           = false;
  bool                                           pdu_session_res_failed_to_setup_list_cxt_res_present = false;
  bool                                           crit_diagnostics_present                             = false;
  uint64_t                                       amf_ue_ngap_id;
  uint64_t                                       ran_ue_ngap_id;
  pdu_session_res_setup_list_cxt_res_l           pdu_session_res_setup_list_cxt_res;
  pdu_session_res_failed_to_setup_list_cxt_res_l pdu_session_res_failed_to_setup_list_cxt_res;
  crit_diagnostics_s                             crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InitialContextSetupResponse ::= SEQUENCE
using init_context_setup_resp_s = elementary_procedure_option<init_context_setup_resp_ies_container>;

// InitialUEMessage-IEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct init_ue_msg_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ran_ue_ngap_id,
        nas_pdu,
        user_location_info,
        rrc_establishment_cause,
        five_g_s_tmsi,
        amf_set_id,
        ue_context_request,
        allowed_nssai,
        source_to_target_amf_info_reroute,
        sel_plmn_id,
        iab_node_ind,
        ce_mode_b_support_ind,
        ltem_ind,
        edt_session,
        authenticated_ind,
        npn_access_info,
        red_cap_ind,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                  ran_ue_ngap_id();
    unbounded_octstring<true>&                 nas_pdu();
    user_location_info_c&                      user_location_info();
    rrc_establishment_cause_e&                 rrc_establishment_cause();
    five_g_s_tmsi_s&                           five_g_s_tmsi();
    fixed_bitstring<10, false, true>&          amf_set_id();
    ue_context_request_e&                      ue_context_request();
    allowed_nssai_l&                           allowed_nssai();
    source_to_target_amf_info_reroute_s&       source_to_target_amf_info_reroute();
    fixed_octstring<3, true>&                  sel_plmn_id();
    iab_node_ind_e&                            iab_node_ind();
    ce_mode_b_support_ind_e&                   ce_mode_b_support_ind();
    ltem_ind_e&                                ltem_ind();
    edt_session_e&                             edt_session();
    authenticated_ind_e&                       authenticated_ind();
    npn_access_info_c&                         npn_access_info();
    red_cap_ind_e&                             red_cap_ind();
    const uint64_t&                            ran_ue_ngap_id() const;
    const unbounded_octstring<true>&           nas_pdu() const;
    const user_location_info_c&                user_location_info() const;
    const rrc_establishment_cause_e&           rrc_establishment_cause() const;
    const five_g_s_tmsi_s&                     five_g_s_tmsi() const;
    const fixed_bitstring<10, false, true>&    amf_set_id() const;
    const ue_context_request_e&                ue_context_request() const;
    const allowed_nssai_l&                     allowed_nssai() const;
    const source_to_target_amf_info_reroute_s& source_to_target_amf_info_reroute() const;
    const fixed_octstring<3, true>&            sel_plmn_id() const;
    const iab_node_ind_e&                      iab_node_ind() const;
    const ce_mode_b_support_ind_e&             ce_mode_b_support_ind() const;
    const ltem_ind_e&                          ltem_ind() const;
    const edt_session_e&                       edt_session() const;
    const authenticated_ind_e&                 authenticated_ind() const;
    const npn_access_info_c&                   npn_access_info() const;
    const red_cap_ind_e&                       red_cap_ind() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct init_ue_msg_ies_container {
  bool                                five_g_s_tmsi_present                     = false;
  bool                                amf_set_id_present                        = false;
  bool                                ue_context_request_present                = false;
  bool                                allowed_nssai_present                     = false;
  bool                                source_to_target_amf_info_reroute_present = false;
  bool                                sel_plmn_id_present                       = false;
  bool                                iab_node_ind_present                      = false;
  bool                                ce_mode_b_support_ind_present             = false;
  bool                                ltem_ind_present                          = false;
  bool                                edt_session_present                       = false;
  bool                                authenticated_ind_present                 = false;
  bool                                npn_access_info_present                   = false;
  bool                                red_cap_ind_present                       = false;
  uint64_t                            ran_ue_ngap_id;
  unbounded_octstring<true>           nas_pdu;
  user_location_info_c                user_location_info;
  rrc_establishment_cause_e           rrc_establishment_cause;
  five_g_s_tmsi_s                     five_g_s_tmsi;
  fixed_bitstring<10, false, true>    amf_set_id;
  ue_context_request_e                ue_context_request;
  allowed_nssai_l                     allowed_nssai;
  source_to_target_amf_info_reroute_s source_to_target_amf_info_reroute;
  fixed_octstring<3, true>            sel_plmn_id;
  iab_node_ind_e                      iab_node_ind;
  ce_mode_b_support_ind_e             ce_mode_b_support_ind;
  ltem_ind_e                          ltem_ind;
  edt_session_e                       edt_session;
  authenticated_ind_e                 authenticated_ind;
  npn_access_info_c                   npn_access_info;
  red_cap_ind_e                       red_cap_ind;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InitialUEMessage ::= SEQUENCE
using init_ue_msg_s = elementary_procedure_option<init_ue_msg_ies_container>;

// LocationReportIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct location_report_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        user_location_info,
        ue_presence_in_area_of_interest_list,
        location_report_request_type,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                     amf_ue_ngap_id();
    uint64_t&                                     ran_ue_ngap_id();
    user_location_info_c&                         user_location_info();
    ue_presence_in_area_of_interest_list_l&       ue_presence_in_area_of_interest_list();
    location_report_request_type_s&               location_report_request_type();
    const uint64_t&                               amf_ue_ngap_id() const;
    const uint64_t&                               ran_ue_ngap_id() const;
    const user_location_info_c&                   user_location_info() const;
    const ue_presence_in_area_of_interest_list_l& ue_presence_in_area_of_interest_list() const;
    const location_report_request_type_s&         location_report_request_type() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// LocationReportingControlIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct location_report_ctrl_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, location_report_request_type, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                             amf_ue_ngap_id();
    uint64_t&                             ran_ue_ngap_id();
    location_report_request_type_s&       location_report_request_type();
    const uint64_t&                       amf_ue_ngap_id() const;
    const uint64_t&                       ran_ue_ngap_id() const;
    const location_report_request_type_s& location_report_request_type() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// LocationReportingFailureIndicationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct location_report_fail_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, cause, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&       amf_ue_ngap_id();
    uint64_t&       ran_ue_ngap_id();
    cause_c&        cause();
    const uint64_t& amf_ue_ngap_id() const;
    const uint64_t& ran_ue_ngap_id() const;
    const cause_c&  cause() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastGroupPagingIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct multicast_group_paging_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, mbs_service_area, multicast_group_paging_area_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&                         mbs_session_id();
    mbs_service_area_c&                       mbs_service_area();
    multicast_group_paging_area_list_l&       multicast_group_paging_area_list();
    const mbs_session_id_s&                   mbs_session_id() const;
    const mbs_service_area_c&                 mbs_service_area() const;
    const multicast_group_paging_area_list_l& multicast_group_paging_area_list() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastSessionActivationFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct multicast_session_activation_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&         mbs_session_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const mbs_session_id_s&   mbs_session_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastSessionActivationRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct multicast_session_activation_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, multicast_session_activation_request_transfer, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&                mbs_session_id();
    unbounded_octstring<true>&       multicast_session_activation_request_transfer();
    const mbs_session_id_s&          mbs_session_id() const;
    const unbounded_octstring<true>& multicast_session_activation_request_transfer() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastSessionActivationResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct multicast_session_activation_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&         mbs_session_id();
    crit_diagnostics_s&       crit_diagnostics();
    const mbs_session_id_s&   mbs_session_id() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastSessionDeactivationRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct multicast_session_deactivation_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, multicast_session_deactivation_request_transfer, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&                mbs_session_id();
    unbounded_octstring<true>&       multicast_session_deactivation_request_transfer();
    const mbs_session_id_s&          mbs_session_id() const;
    const unbounded_octstring<true>& multicast_session_deactivation_request_transfer() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastSessionDeactivationResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct multicast_session_deactivation_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&         mbs_session_id();
    crit_diagnostics_s&       crit_diagnostics();
    const mbs_session_id_s&   mbs_session_id() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastSessionUpdateFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct multicast_session_upd_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, mbs_area_session_id, cause, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&         mbs_session_id();
    uint32_t&                 mbs_area_session_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const mbs_session_id_s&   mbs_session_id() const;
    const uint32_t&           mbs_area_session_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastSessionUpdateRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct multicast_session_upd_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, mbs_area_session_id, multicast_session_upd_request_transfer, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&                mbs_session_id();
    uint32_t&                        mbs_area_session_id();
    unbounded_octstring<true>&       multicast_session_upd_request_transfer();
    const mbs_session_id_s&          mbs_session_id() const;
    const uint32_t&                  mbs_area_session_id() const;
    const unbounded_octstring<true>& multicast_session_upd_request_transfer() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastSessionUpdateResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct multicast_session_upd_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, mbs_area_session_id, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&         mbs_session_id();
    uint32_t&                 mbs_area_session_id();
    crit_diagnostics_s&       crit_diagnostics();
    const mbs_session_id_s&   mbs_session_id() const;
    const uint32_t&           mbs_area_session_id() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NASNonDeliveryIndication-IEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct nas_non_delivery_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, nas_pdu, cause, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                        amf_ue_ngap_id();
    uint64_t&                        ran_ue_ngap_id();
    unbounded_octstring<true>&       nas_pdu();
    cause_c&                         cause();
    const uint64_t&                  amf_ue_ngap_id() const;
    const uint64_t&                  ran_ue_ngap_id() const;
    const unbounded_octstring<true>& nas_pdu() const;
    const cause_c&                   cause() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NGResetAcknowledgeIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ng_reset_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_associated_lc_ng_conn_list, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_associated_lc_ng_conn_list_l&       ue_associated_lc_ng_conn_list();
    crit_diagnostics_s&                    crit_diagnostics();
    const ue_associated_lc_ng_conn_list_l& ue_associated_lc_ng_conn_list() const;
    const crit_diagnostics_s&              crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NGResetIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ng_reset_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cause, reset_type, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c&            cause();
    reset_type_c&       reset_type();
    const cause_c&      cause() const;
    const reset_type_c& reset_type() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NGSetupFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ng_setup_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cause, time_to_wait, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const cause_c&            cause() const;
    const time_to_wait_e&     time_to_wait() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NGSetupRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ng_setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        global_ran_node_id,
        ran_node_name,
        supported_ta_list,
        default_paging_drx,
        ue_retention_info,
        nb_iot_default_paging_drx,
        extended_ran_node_name,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    global_ran_node_id_c&                       global_ran_node_id();
    printable_string<1, 150, true, true>&       ran_node_name();
    supported_ta_list_l&                        supported_ta_list();
    paging_drx_e&                               default_paging_drx();
    ue_retention_info_e&                        ue_retention_info();
    nb_iot_default_paging_drx_e&                nb_iot_default_paging_drx();
    extended_ran_node_name_s&                   extended_ran_node_name();
    const global_ran_node_id_c&                 global_ran_node_id() const;
    const printable_string<1, 150, true, true>& ran_node_name() const;
    const supported_ta_list_l&                  supported_ta_list() const;
    const paging_drx_e&                         default_paging_drx() const;
    const ue_retention_info_e&                  ue_retention_info() const;
    const nb_iot_default_paging_drx_e&          nb_iot_default_paging_drx() const;
    const extended_ran_node_name_s&             extended_ran_node_name() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NGSetupResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ng_setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_name,
        served_guami_list,
        relative_amf_capacity,
        plmn_support_list,
        crit_diagnostics,
        ue_retention_info,
        iab_supported,
        extended_amf_name,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    printable_string<1, 150, true, true>&       amf_name();
    served_guami_list_l&                        served_guami_list();
    uint16_t&                                   relative_amf_capacity();
    plmn_support_list_l&                        plmn_support_list();
    crit_diagnostics_s&                         crit_diagnostics();
    ue_retention_info_e&                        ue_retention_info();
    iab_supported_e&                            iab_supported();
    extended_amf_name_s&                        extended_amf_name();
    const printable_string<1, 150, true, true>& amf_name() const;
    const served_guami_list_l&                  served_guami_list() const;
    const uint16_t&                             relative_amf_capacity() const;
    const plmn_support_list_l&                  plmn_support_list() const;
    const crit_diagnostics_s&                   crit_diagnostics() const;
    const ue_retention_info_e&                  ue_retention_info() const;
    const iab_supported_e&                      iab_supported() const;
    const extended_amf_name_s&                  extended_amf_name() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// OverloadStartIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct overload_start_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_overload_resp, amf_traffic_load_reduction_ind, overload_start_nssai_list, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    overload_resp_c&                   amf_overload_resp();
    uint8_t&                           amf_traffic_load_reduction_ind();
    overload_start_nssai_list_l&       overload_start_nssai_list();
    const overload_resp_c&             amf_overload_resp() const;
    const uint8_t&                     amf_traffic_load_reduction_ind() const;
    const overload_start_nssai_list_l& overload_start_nssai_list() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// OverloadStopIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using overload_stop_ies_o = protocol_ies_empty_o;

// PDUSessionResourceModifyConfirmIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_modify_confirm_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_modify_list_mod_cfm,
        pdu_session_res_failed_to_modify_list_mod_cfm,
        crit_diagnostics,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                              amf_ue_ngap_id();
    uint64_t&                                              ran_ue_ngap_id();
    pdu_session_res_modify_list_mod_cfm_l&                 pdu_session_res_modify_list_mod_cfm();
    pdu_session_res_failed_to_modify_list_mod_cfm_l&       pdu_session_res_failed_to_modify_list_mod_cfm();
    crit_diagnostics_s&                                    crit_diagnostics();
    const uint64_t&                                        amf_ue_ngap_id() const;
    const uint64_t&                                        ran_ue_ngap_id() const;
    const pdu_session_res_modify_list_mod_cfm_l&           pdu_session_res_modify_list_mod_cfm() const;
    const pdu_session_res_failed_to_modify_list_mod_cfm_l& pdu_session_res_failed_to_modify_list_mod_cfm() const;
    const crit_diagnostics_s&                              crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceModifyIndicationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_modify_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_modify_list_mod_ind,
        user_location_info,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                    amf_ue_ngap_id();
    uint64_t&                                    ran_ue_ngap_id();
    pdu_session_res_modify_list_mod_ind_l&       pdu_session_res_modify_list_mod_ind();
    user_location_info_c&                        user_location_info();
    const uint64_t&                              amf_ue_ngap_id() const;
    const uint64_t&                              ran_ue_ngap_id() const;
    const pdu_session_res_modify_list_mod_ind_l& pdu_session_res_modify_list_mod_ind() const;
    const user_location_info_c&                  user_location_info() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceModifyRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_modify_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        ran_paging_prio,
        pdu_session_res_modify_list_mod_req,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                    amf_ue_ngap_id();
    uint64_t&                                    ran_ue_ngap_id();
    uint16_t&                                    ran_paging_prio();
    pdu_session_res_modify_list_mod_req_l&       pdu_session_res_modify_list_mod_req();
    const uint64_t&                              amf_ue_ngap_id() const;
    const uint64_t&                              ran_ue_ngap_id() const;
    const uint16_t&                              ran_paging_prio() const;
    const pdu_session_res_modify_list_mod_req_l& pdu_session_res_modify_list_mod_req() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceModifyResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_modify_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_modify_list_mod_res,
        pdu_session_res_failed_to_modify_list_mod_res,
        user_location_info,
        crit_diagnostics,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                              amf_ue_ngap_id();
    uint64_t&                                              ran_ue_ngap_id();
    pdu_session_res_modify_list_mod_res_l&                 pdu_session_res_modify_list_mod_res();
    pdu_session_res_failed_to_modify_list_mod_res_l&       pdu_session_res_failed_to_modify_list_mod_res();
    user_location_info_c&                                  user_location_info();
    crit_diagnostics_s&                                    crit_diagnostics();
    const uint64_t&                                        amf_ue_ngap_id() const;
    const uint64_t&                                        ran_ue_ngap_id() const;
    const pdu_session_res_modify_list_mod_res_l&           pdu_session_res_modify_list_mod_res() const;
    const pdu_session_res_failed_to_modify_list_mod_res_l& pdu_session_res_failed_to_modify_list_mod_res() const;
    const user_location_info_c&                            user_location_info() const;
    const crit_diagnostics_s&                              crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceNotifyIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_notify_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_notify_list,
        pdu_session_res_released_list_not,
        user_location_info,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                  amf_ue_ngap_id();
    uint64_t&                                  ran_ue_ngap_id();
    pdu_session_res_notify_list_l&             pdu_session_res_notify_list();
    pdu_session_res_released_list_not_l&       pdu_session_res_released_list_not();
    user_location_info_c&                      user_location_info();
    const uint64_t&                            amf_ue_ngap_id() const;
    const uint64_t&                            ran_ue_ngap_id() const;
    const pdu_session_res_notify_list_l&       pdu_session_res_notify_list() const;
    const pdu_session_res_released_list_not_l& pdu_session_res_released_list_not() const;
    const user_location_info_c&                user_location_info() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceReleaseCommandIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_release_cmd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        ran_paging_prio,
        nas_pdu,
        pdu_session_res_to_release_list_rel_cmd,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                        amf_ue_ngap_id();
    uint64_t&                                        ran_ue_ngap_id();
    uint16_t&                                        ran_paging_prio();
    unbounded_octstring<true>&                       nas_pdu();
    pdu_session_res_to_release_list_rel_cmd_l&       pdu_session_res_to_release_list_rel_cmd();
    const uint64_t&                                  amf_ue_ngap_id() const;
    const uint64_t&                                  ran_ue_ngap_id() const;
    const uint16_t&                                  ran_paging_prio() const;
    const unbounded_octstring<true>&                 nas_pdu() const;
    const pdu_session_res_to_release_list_rel_cmd_l& pdu_session_res_to_release_list_rel_cmd() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceReleaseResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_release_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_released_list_rel_res,
        user_location_info,
        crit_diagnostics,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                      amf_ue_ngap_id();
    uint64_t&                                      ran_ue_ngap_id();
    pdu_session_res_released_list_rel_res_l&       pdu_session_res_released_list_rel_res();
    user_location_info_c&                          user_location_info();
    crit_diagnostics_s&                            crit_diagnostics();
    const uint64_t&                                amf_ue_ngap_id() const;
    const uint64_t&                                ran_ue_ngap_id() const;
    const pdu_session_res_released_list_rel_res_l& pdu_session_res_released_list_rel_res() const;
    const user_location_info_c&                    user_location_info() const;
    const crit_diagnostics_s&                      crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceSetupRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        ran_paging_prio,
        nas_pdu,
        pdu_session_res_setup_list_su_req,
        ue_aggr_max_bit_rate,
        ue_slice_max_bit_rate_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                  amf_ue_ngap_id();
    uint64_t&                                  ran_ue_ngap_id();
    uint16_t&                                  ran_paging_prio();
    unbounded_octstring<true>&                 nas_pdu();
    pdu_session_res_setup_list_su_req_l&       pdu_session_res_setup_list_su_req();
    ue_aggr_max_bit_rate_s&                    ue_aggr_max_bit_rate();
    ue_slice_max_bit_rate_list_l&              ue_slice_max_bit_rate_list();
    const uint64_t&                            amf_ue_ngap_id() const;
    const uint64_t&                            ran_ue_ngap_id() const;
    const uint16_t&                            ran_paging_prio() const;
    const unbounded_octstring<true>&           nas_pdu() const;
    const pdu_session_res_setup_list_su_req_l& pdu_session_res_setup_list_su_req() const;
    const ue_aggr_max_bit_rate_s&              ue_aggr_max_bit_rate() const;
    const ue_slice_max_bit_rate_list_l&        ue_slice_max_bit_rate_list() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceSetupResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_setup_list_su_res,
        pdu_session_res_failed_to_setup_list_su_res,
        crit_diagnostics,
        user_location_info,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                            amf_ue_ngap_id();
    uint64_t&                                            ran_ue_ngap_id();
    pdu_session_res_setup_list_su_res_l&                 pdu_session_res_setup_list_su_res();
    pdu_session_res_failed_to_setup_list_su_res_l&       pdu_session_res_failed_to_setup_list_su_res();
    crit_diagnostics_s&                                  crit_diagnostics();
    user_location_info_c&                                user_location_info();
    const uint64_t&                                      amf_ue_ngap_id() const;
    const uint64_t&                                      ran_ue_ngap_id() const;
    const pdu_session_res_setup_list_su_res_l&           pdu_session_res_setup_list_su_res() const;
    const pdu_session_res_failed_to_setup_list_su_res_l& pdu_session_res_failed_to_setup_list_su_res() const;
    const crit_diagnostics_s&                            crit_diagnostics() const;
    const user_location_info_c&                          user_location_info() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWSCancelRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pws_cancel_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { msg_id, serial_num, warning_area_list, cancel_all_warning_msgs, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<16, false, true>&       msg_id();
    fixed_bitstring<16, false, true>&       serial_num();
    warning_area_list_c&                    warning_area_list();
    cancel_all_warning_msgs_e&              cancel_all_warning_msgs();
    const fixed_bitstring<16, false, true>& msg_id() const;
    const fixed_bitstring<16, false, true>& serial_num() const;
    const warning_area_list_c&              warning_area_list() const;
    const cancel_all_warning_msgs_e&        cancel_all_warning_msgs() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWSCancelResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pws_cancel_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { msg_id, serial_num, broadcast_cancelled_area_list, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<16, false, true>&       msg_id();
    fixed_bitstring<16, false, true>&       serial_num();
    broadcast_cancelled_area_list_c&        broadcast_cancelled_area_list();
    crit_diagnostics_s&                     crit_diagnostics();
    const fixed_bitstring<16, false, true>& msg_id() const;
    const fixed_bitstring<16, false, true>& serial_num() const;
    const broadcast_cancelled_area_list_c&  broadcast_cancelled_area_list() const;
    const crit_diagnostics_s&               crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWSFailureIndicationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pws_fail_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { pws_failed_cell_id_list, global_ran_node_id, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pws_failed_cell_id_list_c&       pws_failed_cell_id_list();
    global_ran_node_id_c&            global_ran_node_id();
    const pws_failed_cell_id_list_c& pws_failed_cell_id_list() const;
    const global_ran_node_id_c&      global_ran_node_id() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWSRestartIndicationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pws_restart_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        cell_id_list_for_restart,
        global_ran_node_id,
        tai_list_for_restart,
        emergency_area_id_list_for_restart,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cell_id_list_for_restart_c&                 cell_id_list_for_restart();
    global_ran_node_id_c&                       global_ran_node_id();
    tai_list_for_restart_l&                     tai_list_for_restart();
    emergency_area_id_list_for_restart_l&       emergency_area_id_list_for_restart();
    const cell_id_list_for_restart_c&           cell_id_list_for_restart() const;
    const global_ran_node_id_c&                 global_ran_node_id() const;
    const tai_list_for_restart_l&               tai_list_for_restart() const;
    const emergency_area_id_list_for_restart_l& emergency_area_id_list_for_restart() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PagingIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct paging_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ue_paging_id,
        paging_drx,
        tai_list_for_paging,
        paging_prio,
        ue_radio_cap_for_paging,
        paging_origin,
        assist_data_for_paging,
        nb_iot_paging_e_drx_info,
        nb_iot_paging_drx,
        enhanced_coverage_restrict,
        wus_assist_info,
        eutra_paginge_drx_info,
        ce_mode_brestricted,
        nr_paginge_drx_info,
        paging_cause,
        pe_ip_sassist_info,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_paging_id_c&                     ue_paging_id();
    paging_drx_e&                       paging_drx();
    tai_list_for_paging_l&              tai_list_for_paging();
    paging_prio_e&                      paging_prio();
    ue_radio_cap_for_paging_s&          ue_radio_cap_for_paging();
    paging_origin_e&                    paging_origin();
    assist_data_for_paging_s&           assist_data_for_paging();
    nb_iot_paging_e_drx_info_s&         nb_iot_paging_e_drx_info();
    nb_iot_paging_drx_e&                nb_iot_paging_drx();
    enhanced_coverage_restrict_e&       enhanced_coverage_restrict();
    wus_assist_info_s&                  wus_assist_info();
    eutra_paginge_drx_info_s&           eutra_paginge_drx_info();
    ce_mode_brestricted_e&              ce_mode_brestricted();
    nr_paginge_drx_info_s&              nr_paginge_drx_info();
    paging_cause_e&                     paging_cause();
    pe_ip_sassist_info_s&               pe_ip_sassist_info();
    const ue_paging_id_c&               ue_paging_id() const;
    const paging_drx_e&                 paging_drx() const;
    const tai_list_for_paging_l&        tai_list_for_paging() const;
    const paging_prio_e&                paging_prio() const;
    const ue_radio_cap_for_paging_s&    ue_radio_cap_for_paging() const;
    const paging_origin_e&              paging_origin() const;
    const assist_data_for_paging_s&     assist_data_for_paging() const;
    const nb_iot_paging_e_drx_info_s&   nb_iot_paging_e_drx_info() const;
    const nb_iot_paging_drx_e&          nb_iot_paging_drx() const;
    const enhanced_coverage_restrict_e& enhanced_coverage_restrict() const;
    const wus_assist_info_s&            wus_assist_info() const;
    const eutra_paginge_drx_info_s&     eutra_paginge_drx_info() const;
    const ce_mode_brestricted_e&        ce_mode_brestricted() const;
    const nr_paginge_drx_info_s&        nr_paginge_drx_info() const;
    const paging_cause_e&               paging_cause() const;
    const pe_ip_sassist_info_s&         pe_ip_sassist_info() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PathSwitchRequestAcknowledgeIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct path_switch_request_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        ue_security_cap,
        security_context,
        new_security_context_ind,
        pdu_session_res_switched_list,
        pdu_session_res_released_list_ps_ack,
        allowed_nssai,
        core_network_assist_info_for_inactive,
        rrc_inactive_transition_report_request,
        crit_diagnostics,
        redirection_voice_fallback,
        cn_assisted_ran_tuning,
        srvcc_operation_possible,
        enhanced_coverage_restrict,
        extended_connected_time,
        ue_diff_info,
        nr_v2x_services_authorized,
        ltev2x_services_authorized,
        nr_ue_sidelink_aggr_max_bitrate,
        lte_ue_sidelink_aggr_max_bitrate,
        pc5_qos_params,
        ce_mode_brestricted,
        ue_up_c_iot_support,
        ue_radio_cap_id,
        management_based_mdt_plmn_list,
        time_sync_assist_info,
        five_g_pro_se_authorized,
        five_g_pro_se_ue_pc5_aggr_max_bit_rate,
        five_g_pro_se_pc5_qos_params,
        management_based_mdt_plmn_mod_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                       amf_ue_ngap_id();
    uint64_t&                                       ran_ue_ngap_id();
    ue_security_cap_s&                              ue_security_cap();
    security_context_s&                             security_context();
    new_security_context_ind_e&                     new_security_context_ind();
    pdu_session_res_switched_list_l&                pdu_session_res_switched_list();
    pdu_session_res_released_list_ps_ack_l&         pdu_session_res_released_list_ps_ack();
    allowed_nssai_l&                                allowed_nssai();
    core_network_assist_info_for_inactive_s&        core_network_assist_info_for_inactive();
    rrc_inactive_transition_report_request_e&       rrc_inactive_transition_report_request();
    crit_diagnostics_s&                             crit_diagnostics();
    redirection_voice_fallback_e&                   redirection_voice_fallback();
    cn_assisted_ran_tuning_s&                       cn_assisted_ran_tuning();
    srvcc_operation_possible_e&                     srvcc_operation_possible();
    enhanced_coverage_restrict_e&                   enhanced_coverage_restrict();
    uint16_t&                                       extended_connected_time();
    ue_diff_info_s&                                 ue_diff_info();
    nr_v2x_services_authorized_s&                   nr_v2x_services_authorized();
    ltev2x_services_authorized_s&                   ltev2x_services_authorized();
    nr_ue_sidelink_aggr_max_bitrate_s&              nr_ue_sidelink_aggr_max_bitrate();
    lte_ue_sidelink_aggr_max_bitrate_s&             lte_ue_sidelink_aggr_max_bitrate();
    pc5_qos_params_s&                               pc5_qos_params();
    ce_mode_brestricted_e&                          ce_mode_brestricted();
    ue_up_c_iot_support_e&                          ue_up_c_iot_support();
    unbounded_octstring<true>&                      ue_radio_cap_id();
    mdt_plmn_list_l&                                management_based_mdt_plmn_list();
    time_sync_assist_info_s&                        time_sync_assist_info();
    five_g_pro_se_authorized_s&                     five_g_pro_se_authorized();
    nr_ue_sidelink_aggr_max_bitrate_s&              five_g_pro_se_ue_pc5_aggr_max_bit_rate();
    five_g_pro_se_pc5_qos_params_s&                 five_g_pro_se_pc5_qos_params();
    mdt_plmn_mod_list_l&                            management_based_mdt_plmn_mod_list();
    const uint64_t&                                 amf_ue_ngap_id() const;
    const uint64_t&                                 ran_ue_ngap_id() const;
    const ue_security_cap_s&                        ue_security_cap() const;
    const security_context_s&                       security_context() const;
    const new_security_context_ind_e&               new_security_context_ind() const;
    const pdu_session_res_switched_list_l&          pdu_session_res_switched_list() const;
    const pdu_session_res_released_list_ps_ack_l&   pdu_session_res_released_list_ps_ack() const;
    const allowed_nssai_l&                          allowed_nssai() const;
    const core_network_assist_info_for_inactive_s&  core_network_assist_info_for_inactive() const;
    const rrc_inactive_transition_report_request_e& rrc_inactive_transition_report_request() const;
    const crit_diagnostics_s&                       crit_diagnostics() const;
    const redirection_voice_fallback_e&             redirection_voice_fallback() const;
    const cn_assisted_ran_tuning_s&                 cn_assisted_ran_tuning() const;
    const srvcc_operation_possible_e&               srvcc_operation_possible() const;
    const enhanced_coverage_restrict_e&             enhanced_coverage_restrict() const;
    const uint16_t&                                 extended_connected_time() const;
    const ue_diff_info_s&                           ue_diff_info() const;
    const nr_v2x_services_authorized_s&             nr_v2x_services_authorized() const;
    const ltev2x_services_authorized_s&             ltev2x_services_authorized() const;
    const nr_ue_sidelink_aggr_max_bitrate_s&        nr_ue_sidelink_aggr_max_bitrate() const;
    const lte_ue_sidelink_aggr_max_bitrate_s&       lte_ue_sidelink_aggr_max_bitrate() const;
    const pc5_qos_params_s&                         pc5_qos_params() const;
    const ce_mode_brestricted_e&                    ce_mode_brestricted() const;
    const ue_up_c_iot_support_e&                    ue_up_c_iot_support() const;
    const unbounded_octstring<true>&                ue_radio_cap_id() const;
    const mdt_plmn_list_l&                          management_based_mdt_plmn_list() const;
    const time_sync_assist_info_s&                  time_sync_assist_info() const;
    const five_g_pro_se_authorized_s&               five_g_pro_se_authorized() const;
    const nr_ue_sidelink_aggr_max_bitrate_s&        five_g_pro_se_ue_pc5_aggr_max_bit_rate() const;
    const five_g_pro_se_pc5_qos_params_s&           five_g_pro_se_pc5_qos_params() const;
    const mdt_plmn_mod_list_l&                      management_based_mdt_plmn_mod_list() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PathSwitchRequestFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct path_switch_request_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_released_list_ps_fail,
        crit_diagnostics,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                      amf_ue_ngap_id();
    uint64_t&                                      ran_ue_ngap_id();
    pdu_session_res_released_list_ps_fail_l&       pdu_session_res_released_list_ps_fail();
    crit_diagnostics_s&                            crit_diagnostics();
    const uint64_t&                                amf_ue_ngap_id() const;
    const uint64_t&                                ran_ue_ngap_id() const;
    const pdu_session_res_released_list_ps_fail_l& pdu_session_res_released_list_ps_fail() const;
    const crit_diagnostics_s&                      crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PathSwitchRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct path_switch_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ran_ue_ngap_id,
        source_amf_ue_ngap_id,
        user_location_info,
        ue_security_cap,
        pdu_session_res_to_be_switched_dl_list,
        pdu_session_res_failed_to_setup_list_ps_req,
        rrc_resume_cause,
        red_cap_ind,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                            ran_ue_ngap_id();
    uint64_t&                                            source_amf_ue_ngap_id();
    user_location_info_c&                                user_location_info();
    ue_security_cap_s&                                   ue_security_cap();
    pdu_session_res_to_be_switched_dl_list_l&            pdu_session_res_to_be_switched_dl_list();
    pdu_session_res_failed_to_setup_list_ps_req_l&       pdu_session_res_failed_to_setup_list_ps_req();
    rrc_establishment_cause_e&                           rrc_resume_cause();
    red_cap_ind_e&                                       red_cap_ind();
    const uint64_t&                                      ran_ue_ngap_id() const;
    const uint64_t&                                      source_amf_ue_ngap_id() const;
    const user_location_info_c&                          user_location_info() const;
    const ue_security_cap_s&                             ue_security_cap() const;
    const pdu_session_res_to_be_switched_dl_list_l&      pdu_session_res_to_be_switched_dl_list() const;
    const pdu_session_res_failed_to_setup_list_ps_req_l& pdu_session_res_failed_to_setup_list_ps_req() const;
    const rrc_establishment_cause_e&                     rrc_resume_cause() const;
    const red_cap_ind_e&                                 red_cap_ind() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct ngap_private_ies_empty_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::nulltype; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };
};
// PrivateMessageIEs ::= OBJECT SET OF NGAP-PRIVATE-IES
using private_msg_ies_o = ngap_private_ies_empty_o;

// RANCPRelocationIndicationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ran_cp_relocation_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ran_ue_ngap_id, five_g_s_tmsi, eutra_cgi, tai, ul_cp_security_info, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                    ran_ue_ngap_id();
    five_g_s_tmsi_s&             five_g_s_tmsi();
    eutra_cgi_s&                 eutra_cgi();
    tai_s&                       tai();
    ul_cp_security_info_s&       ul_cp_security_info();
    const uint64_t&              ran_ue_ngap_id() const;
    const five_g_s_tmsi_s&       five_g_s_tmsi() const;
    const eutra_cgi_s&           eutra_cgi() const;
    const tai_s&                 tai() const;
    const ul_cp_security_info_s& ul_cp_security_info() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RANConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ran_cfg_upd_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::crit_diagnostics; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    crit_diagnostics_s&       crit_diagnostics() { return c; }
    const crit_diagnostics_s& crit_diagnostics() const { return c; }

  private:
    crit_diagnostics_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RANConfigurationUpdateFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ran_cfg_upd_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cause, time_to_wait, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const cause_c&            cause() const;
    const time_to_wait_e&     time_to_wait() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RANConfigurationUpdateIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ran_cfg_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ran_node_name,
        supported_ta_list,
        default_paging_drx,
        global_ran_node_id,
        ngran_tnl_assoc_to_rem_list,
        nb_iot_default_paging_drx,
        extended_ran_node_name,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    printable_string<1, 150, true, true>&       ran_node_name();
    supported_ta_list_l&                        supported_ta_list();
    paging_drx_e&                               default_paging_drx();
    global_ran_node_id_c&                       global_ran_node_id();
    ngran_tnl_assoc_to_rem_list_l&              ngran_tnl_assoc_to_rem_list();
    nb_iot_default_paging_drx_e&                nb_iot_default_paging_drx();
    extended_ran_node_name_s&                   extended_ran_node_name();
    const printable_string<1, 150, true, true>& ran_node_name() const;
    const supported_ta_list_l&                  supported_ta_list() const;
    const paging_drx_e&                         default_paging_drx() const;
    const global_ran_node_id_c&                 global_ran_node_id() const;
    const ngran_tnl_assoc_to_rem_list_l&        ngran_tnl_assoc_to_rem_list() const;
    const nb_iot_default_paging_drx_e&          nb_iot_default_paging_drx() const;
    const extended_ran_node_name_s&             extended_ran_node_name() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RRCInactiveTransitionReportIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct rrc_inactive_transition_report_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, rrc_state, user_location_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                   amf_ue_ngap_id();
    uint64_t&                   ran_ue_ngap_id();
    rrc_state_e&                rrc_state();
    user_location_info_c&       user_location_info();
    const uint64_t&             amf_ue_ngap_id() const;
    const uint64_t&             ran_ue_ngap_id() const;
    const rrc_state_e&          rrc_state() const;
    const user_location_info_c& user_location_info() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RerouteNASRequest-IEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct reroute_nas_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ran_ue_ngap_id,
        amf_ue_ngap_id,
        ngap_msg,
        amf_set_id,
        allowed_nssai,
        source_to_target_amf_info_reroute,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                  ran_ue_ngap_id();
    uint64_t&                                  amf_ue_ngap_id();
    unbounded_octstring<true>&                 ngap_msg();
    fixed_bitstring<10, false, true>&          amf_set_id();
    allowed_nssai_l&                           allowed_nssai();
    source_to_target_amf_info_reroute_s&       source_to_target_amf_info_reroute();
    const uint64_t&                            ran_ue_ngap_id() const;
    const uint64_t&                            amf_ue_ngap_id() const;
    const unbounded_octstring<true>&           ngap_msg() const;
    const fixed_bitstring<10, false, true>&    amf_set_id() const;
    const allowed_nssai_l&                     allowed_nssai() const;
    const source_to_target_amf_info_reroute_s& source_to_target_amf_info_reroute() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RetrieveUEInformationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct retrieve_ue_info_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { five_g_s_tmsi, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::five_g_s_tmsi; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    five_g_s_tmsi_s&       five_g_s_tmsi() { return c; }
    const five_g_s_tmsi_s& five_g_s_tmsi() const { return c; }

  private:
    five_g_s_tmsi_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SecondaryRATDataUsageReportIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct secondary_rat_data_usage_report_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_secondary_rat_usage_list,
        ho_flag,
        user_location_info,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                         amf_ue_ngap_id();
    uint64_t&                                         ran_ue_ngap_id();
    pdu_session_res_secondary_rat_usage_list_l&       pdu_session_res_secondary_rat_usage_list();
    ho_flag_e&                                        ho_flag();
    user_location_info_c&                             user_location_info();
    const uint64_t&                                   amf_ue_ngap_id() const;
    const uint64_t&                                   ran_ue_ngap_id() const;
    const pdu_session_res_secondary_rat_usage_list_l& pdu_session_res_secondary_rat_usage_list() const;
    const ho_flag_e&                                  ho_flag() const;
    const user_location_info_c&                       user_location_info() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TraceFailureIndicationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct trace_fail_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, ngran_trace_id, cause, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                       amf_ue_ngap_id();
    uint64_t&                       ran_ue_ngap_id();
    fixed_octstring<8, true>&       ngran_trace_id();
    cause_c&                        cause();
    const uint64_t&                 amf_ue_ngap_id() const;
    const uint64_t&                 ran_ue_ngap_id() const;
    const fixed_octstring<8, true>& ngran_trace_id() const;
    const cause_c&                  cause() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TraceStartIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct trace_start_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, trace_activation, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                 amf_ue_ngap_id();
    uint64_t&                 ran_ue_ngap_id();
    trace_activation_s&       trace_activation();
    const uint64_t&           amf_ue_ngap_id() const;
    const uint64_t&           ran_ue_ngap_id() const;
    const trace_activation_s& trace_activation() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextModificationFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_context_mod_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                 amf_ue_ngap_id();
    uint64_t&                 ran_ue_ngap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           amf_ue_ngap_id() const;
    const uint64_t&           ran_ue_ngap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextModificationRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_context_mod_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        ran_paging_prio,
        security_key,
        idx_to_rfsp,
        ue_aggr_max_bit_rate,
        ue_security_cap,
        core_network_assist_info_for_inactive,
        emergency_fallback_ind,
        new_amf_ue_ngap_id,
        rrc_inactive_transition_report_request,
        new_guami,
        cn_assisted_ran_tuning,
        srvcc_operation_possible,
        iab_authorized,
        nr_v2x_services_authorized,
        ltev2x_services_authorized,
        nr_ue_sidelink_aggr_max_bitrate,
        lte_ue_sidelink_aggr_max_bitrate,
        pc5_qos_params,
        ue_radio_cap_id,
        rg_level_wireline_access_characteristics,
        time_sync_assist_info,
        q_mcc_onfig_info,
        qmc_deactivation,
        ue_slice_max_bit_rate_list,
        management_based_mdt_plmn_mod_list,
        five_g_pro_se_authorized,
        five_g_pro_se_ue_pc5_aggr_max_bit_rate,
        five_g_pro_se_pc5_qos_params,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                       amf_ue_ngap_id();
    uint64_t&                                       ran_ue_ngap_id();
    uint16_t&                                       ran_paging_prio();
    fixed_bitstring<256, false, true>&              security_key();
    uint16_t&                                       idx_to_rfsp();
    ue_aggr_max_bit_rate_s&                         ue_aggr_max_bit_rate();
    ue_security_cap_s&                              ue_security_cap();
    core_network_assist_info_for_inactive_s&        core_network_assist_info_for_inactive();
    emergency_fallback_ind_s&                       emergency_fallback_ind();
    uint64_t&                                       new_amf_ue_ngap_id();
    rrc_inactive_transition_report_request_e&       rrc_inactive_transition_report_request();
    guami_s&                                        new_guami();
    cn_assisted_ran_tuning_s&                       cn_assisted_ran_tuning();
    srvcc_operation_possible_e&                     srvcc_operation_possible();
    iab_authorized_e&                               iab_authorized();
    nr_v2x_services_authorized_s&                   nr_v2x_services_authorized();
    ltev2x_services_authorized_s&                   ltev2x_services_authorized();
    nr_ue_sidelink_aggr_max_bitrate_s&              nr_ue_sidelink_aggr_max_bitrate();
    lte_ue_sidelink_aggr_max_bitrate_s&             lte_ue_sidelink_aggr_max_bitrate();
    pc5_qos_params_s&                               pc5_qos_params();
    unbounded_octstring<true>&                      ue_radio_cap_id();
    unbounded_octstring<true>&                      rg_level_wireline_access_characteristics();
    time_sync_assist_info_s&                        time_sync_assist_info();
    q_mcc_onfig_info_s&                             q_mcc_onfig_info();
    qmc_deactivation_s&                             qmc_deactivation();
    ue_slice_max_bit_rate_list_l&                   ue_slice_max_bit_rate_list();
    mdt_plmn_mod_list_l&                            management_based_mdt_plmn_mod_list();
    five_g_pro_se_authorized_s&                     five_g_pro_se_authorized();
    nr_ue_sidelink_aggr_max_bitrate_s&              five_g_pro_se_ue_pc5_aggr_max_bit_rate();
    five_g_pro_se_pc5_qos_params_s&                 five_g_pro_se_pc5_qos_params();
    const uint64_t&                                 amf_ue_ngap_id() const;
    const uint64_t&                                 ran_ue_ngap_id() const;
    const uint16_t&                                 ran_paging_prio() const;
    const fixed_bitstring<256, false, true>&        security_key() const;
    const uint16_t&                                 idx_to_rfsp() const;
    const ue_aggr_max_bit_rate_s&                   ue_aggr_max_bit_rate() const;
    const ue_security_cap_s&                        ue_security_cap() const;
    const core_network_assist_info_for_inactive_s&  core_network_assist_info_for_inactive() const;
    const emergency_fallback_ind_s&                 emergency_fallback_ind() const;
    const uint64_t&                                 new_amf_ue_ngap_id() const;
    const rrc_inactive_transition_report_request_e& rrc_inactive_transition_report_request() const;
    const guami_s&                                  new_guami() const;
    const cn_assisted_ran_tuning_s&                 cn_assisted_ran_tuning() const;
    const srvcc_operation_possible_e&               srvcc_operation_possible() const;
    const iab_authorized_e&                         iab_authorized() const;
    const nr_v2x_services_authorized_s&             nr_v2x_services_authorized() const;
    const ltev2x_services_authorized_s&             ltev2x_services_authorized() const;
    const nr_ue_sidelink_aggr_max_bitrate_s&        nr_ue_sidelink_aggr_max_bitrate() const;
    const lte_ue_sidelink_aggr_max_bitrate_s&       lte_ue_sidelink_aggr_max_bitrate() const;
    const pc5_qos_params_s&                         pc5_qos_params() const;
    const unbounded_octstring<true>&                ue_radio_cap_id() const;
    const unbounded_octstring<true>&                rg_level_wireline_access_characteristics() const;
    const time_sync_assist_info_s&                  time_sync_assist_info() const;
    const q_mcc_onfig_info_s&                       q_mcc_onfig_info() const;
    const qmc_deactivation_s&                       qmc_deactivation() const;
    const ue_slice_max_bit_rate_list_l&             ue_slice_max_bit_rate_list() const;
    const mdt_plmn_mod_list_l&                      management_based_mdt_plmn_mod_list() const;
    const five_g_pro_se_authorized_s&               five_g_pro_se_authorized() const;
    const nr_ue_sidelink_aggr_max_bitrate_s&        five_g_pro_se_ue_pc5_aggr_max_bit_rate() const;
    const five_g_pro_se_pc5_qos_params_s&           five_g_pro_se_pc5_qos_params() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextModificationResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_context_mod_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, rrc_state, user_location_info, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                   amf_ue_ngap_id();
    uint64_t&                   ran_ue_ngap_id();
    rrc_state_e&                rrc_state();
    user_location_info_c&       user_location_info();
    crit_diagnostics_s&         crit_diagnostics();
    const uint64_t&             amf_ue_ngap_id() const;
    const uint64_t&             ran_ue_ngap_id() const;
    const rrc_state_e&          rrc_state() const;
    const user_location_info_c& user_location_info() const;
    const crit_diagnostics_s&   crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextReleaseCommand-IEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_context_release_cmd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_ngap_ids, cause, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_ngap_ids_c&       ue_ngap_ids();
    cause_c&             cause();
    const ue_ngap_ids_c& ue_ngap_ids() const;
    const cause_c&       cause() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextReleaseComplete-IEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_context_release_complete_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        user_location_info,
        info_on_recommended_cells_and_ran_nodes_for_paging,
        pdu_session_res_list_cxt_rel_cpl,
        crit_diagnostics,
        paging_assis_datafor_c_ecapab_ue,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                             amf_ue_ngap_id();
    uint64_t&                                             ran_ue_ngap_id();
    user_location_info_c&                                 user_location_info();
    info_on_recommended_cells_and_ran_nodes_for_paging_s& info_on_recommended_cells_and_ran_nodes_for_paging();
    pdu_session_res_list_cxt_rel_cpl_l&                   pdu_session_res_list_cxt_rel_cpl();
    crit_diagnostics_s&                                   crit_diagnostics();
    paging_assis_datafor_c_ecapab_ue_s&                   paging_assis_datafor_c_ecapab_ue();
    const uint64_t&                                       amf_ue_ngap_id() const;
    const uint64_t&                                       ran_ue_ngap_id() const;
    const user_location_info_c&                           user_location_info() const;
    const info_on_recommended_cells_and_ran_nodes_for_paging_s&
                                              info_on_recommended_cells_and_ran_nodes_for_paging() const;
    const pdu_session_res_list_cxt_rel_cpl_l& pdu_session_res_list_cxt_rel_cpl() const;
    const crit_diagnostics_s&                 crit_diagnostics() const;
    const paging_assis_datafor_c_ecapab_ue_s& paging_assis_datafor_c_ecapab_ue() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextReleaseRequest-IEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_context_release_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, pdu_session_res_list_cxt_rel_req, cause, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                 amf_ue_ngap_id();
    uint64_t&                                 ran_ue_ngap_id();
    pdu_session_res_list_cxt_rel_req_l&       pdu_session_res_list_cxt_rel_req();
    cause_c&                                  cause();
    const uint64_t&                           amf_ue_ngap_id() const;
    const uint64_t&                           ran_ue_ngap_id() const;
    const pdu_session_res_list_cxt_rel_req_l& pdu_session_res_list_cxt_rel_req() const;
    const cause_c&                            cause() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextResumeFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_context_resume_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                 amf_ue_ngap_id();
    uint64_t&                 ran_ue_ngap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           amf_ue_ngap_id() const;
    const uint64_t&           ran_ue_ngap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextResumeRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_context_resume_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        rrc_resume_cause,
        pdu_session_res_resume_list_res_req,
        pdu_session_res_failed_to_resume_list_res_req,
        suspend_request_ind,
        info_on_recommended_cells_and_ran_nodes_for_paging,
        paging_assis_datafor_c_ecapab_ue,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                              amf_ue_ngap_id();
    uint64_t&                                              ran_ue_ngap_id();
    rrc_establishment_cause_e&                             rrc_resume_cause();
    pdu_session_res_resume_list_res_req_l&                 pdu_session_res_resume_list_res_req();
    pdu_session_res_failed_to_resume_list_res_req_l&       pdu_session_res_failed_to_resume_list_res_req();
    suspend_request_ind_e&                                 suspend_request_ind();
    info_on_recommended_cells_and_ran_nodes_for_paging_s&  info_on_recommended_cells_and_ran_nodes_for_paging();
    paging_assis_datafor_c_ecapab_ue_s&                    paging_assis_datafor_c_ecapab_ue();
    const uint64_t&                                        amf_ue_ngap_id() const;
    const uint64_t&                                        ran_ue_ngap_id() const;
    const rrc_establishment_cause_e&                       rrc_resume_cause() const;
    const pdu_session_res_resume_list_res_req_l&           pdu_session_res_resume_list_res_req() const;
    const pdu_session_res_failed_to_resume_list_res_req_l& pdu_session_res_failed_to_resume_list_res_req() const;
    const suspend_request_ind_e&                           suspend_request_ind() const;
    const info_on_recommended_cells_and_ran_nodes_for_paging_s&
                                              info_on_recommended_cells_and_ran_nodes_for_paging() const;
    const paging_assis_datafor_c_ecapab_ue_s& paging_assis_datafor_c_ecapab_ue() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextResumeResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_context_resume_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        pdu_session_res_resume_list_res_res,
        pdu_session_res_failed_to_resume_list_res_res,
        security_context,
        suspend_resp_ind,
        extended_connected_time,
        crit_diagnostics,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                              amf_ue_ngap_id();
    uint64_t&                                              ran_ue_ngap_id();
    pdu_session_res_resume_list_res_res_l&                 pdu_session_res_resume_list_res_res();
    pdu_session_res_failed_to_resume_list_res_res_l&       pdu_session_res_failed_to_resume_list_res_res();
    security_context_s&                                    security_context();
    suspend_resp_ind_e&                                    suspend_resp_ind();
    uint16_t&                                              extended_connected_time();
    crit_diagnostics_s&                                    crit_diagnostics();
    const uint64_t&                                        amf_ue_ngap_id() const;
    const uint64_t&                                        ran_ue_ngap_id() const;
    const pdu_session_res_resume_list_res_res_l&           pdu_session_res_resume_list_res_res() const;
    const pdu_session_res_failed_to_resume_list_res_res_l& pdu_session_res_failed_to_resume_list_res_res() const;
    const security_context_s&                              security_context() const;
    const suspend_resp_ind_e&                              suspend_resp_ind() const;
    const uint16_t&                                        extended_connected_time() const;
    const crit_diagnostics_s&                              crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextSuspendFailureIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_context_suspend_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                 amf_ue_ngap_id();
    uint64_t&                 ran_ue_ngap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           amf_ue_ngap_id() const;
    const uint64_t&           ran_ue_ngap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextSuspendRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_context_suspend_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        info_on_recommended_cells_and_ran_nodes_for_paging,
        paging_assis_datafor_c_ecapab_ue,
        pdu_session_res_suspend_list_sus_req,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                             amf_ue_ngap_id();
    uint64_t&                                             ran_ue_ngap_id();
    info_on_recommended_cells_and_ran_nodes_for_paging_s& info_on_recommended_cells_and_ran_nodes_for_paging();
    paging_assis_datafor_c_ecapab_ue_s&                   paging_assis_datafor_c_ecapab_ue();
    pdu_session_res_suspend_list_sus_req_l&               pdu_session_res_suspend_list_sus_req();
    const uint64_t&                                       amf_ue_ngap_id() const;
    const uint64_t&                                       ran_ue_ngap_id() const;
    const info_on_recommended_cells_and_ran_nodes_for_paging_s&
                                                  info_on_recommended_cells_and_ran_nodes_for_paging() const;
    const paging_assis_datafor_c_ecapab_ue_s&     paging_assis_datafor_c_ecapab_ue() const;
    const pdu_session_res_suspend_list_sus_req_l& pdu_session_res_suspend_list_sus_req() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextSuspendResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_context_suspend_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, security_context, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                 amf_ue_ngap_id();
    uint64_t&                 ran_ue_ngap_id();
    security_context_s&       security_context();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           amf_ue_ngap_id() const;
    const uint64_t&           ran_ue_ngap_id() const;
    const security_context_s& security_context() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEInformationTransferIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_info_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        five_g_s_tmsi,
        nb_iot_ue_prio,
        ue_radio_cap,
        s_nssai,
        allowed_nssai,
        ue_diff_info,
        masked_imeisv,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    five_g_s_tmsi_s&                        five_g_s_tmsi();
    uint16_t&                               nb_iot_ue_prio();
    unbounded_octstring<true>&              ue_radio_cap();
    s_nssai_s&                              s_nssai();
    allowed_nssai_l&                        allowed_nssai();
    ue_diff_info_s&                         ue_diff_info();
    fixed_bitstring<64, false, true>&       masked_imeisv();
    const five_g_s_tmsi_s&                  five_g_s_tmsi() const;
    const uint16_t&                         nb_iot_ue_prio() const;
    const unbounded_octstring<true>&        ue_radio_cap() const;
    const s_nssai_s&                        s_nssai() const;
    const allowed_nssai_l&                  allowed_nssai() const;
    const ue_diff_info_s&                   ue_diff_info() const;
    const fixed_bitstring<64, false, true>& masked_imeisv() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UERadioCapabilityCheckRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_radio_cap_check_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, ue_radio_cap, ue_radio_cap_id, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                        amf_ue_ngap_id();
    uint64_t&                        ran_ue_ngap_id();
    unbounded_octstring<true>&       ue_radio_cap();
    unbounded_octstring<true>&       ue_radio_cap_id();
    const uint64_t&                  amf_ue_ngap_id() const;
    const uint64_t&                  ran_ue_ngap_id() const;
    const unbounded_octstring<true>& ue_radio_cap() const;
    const unbounded_octstring<true>& ue_radio_cap_id() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UERadioCapabilityCheckResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_radio_cap_check_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, ims_voice_support_ind, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                      amf_ue_ngap_id();
    uint64_t&                      ran_ue_ngap_id();
    ims_voice_support_ind_e&       ims_voice_support_ind();
    crit_diagnostics_s&            crit_diagnostics();
    const uint64_t&                amf_ue_ngap_id() const;
    const uint64_t&                ran_ue_ngap_id() const;
    const ims_voice_support_ind_e& ims_voice_support_ind() const;
    const crit_diagnostics_s&      crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UERadioCapabilityIDMappingRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_radio_cap_id_map_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_radio_cap_id, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ue_radio_cap_id; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    unbounded_octstring<true>&       ue_radio_cap_id() { return c; }
    const unbounded_octstring<true>& ue_radio_cap_id() const { return c; }

  private:
    unbounded_octstring<true> c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UERadioCapabilityIDMappingResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_radio_cap_id_map_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_radio_cap_id, ue_radio_cap, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    unbounded_octstring<true>&       ue_radio_cap_id();
    unbounded_octstring<true>&       ue_radio_cap();
    crit_diagnostics_s&              crit_diagnostics();
    const unbounded_octstring<true>& ue_radio_cap_id() const;
    const unbounded_octstring<true>& ue_radio_cap() const;
    const crit_diagnostics_s&        crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UERadioCapabilityInfoIndicationIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_radio_cap_info_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        ue_radio_cap,
        ue_radio_cap_for_paging,
        ue_radio_cap_eutra_format,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                        amf_ue_ngap_id();
    uint64_t&                        ran_ue_ngap_id();
    unbounded_octstring<true>&       ue_radio_cap();
    ue_radio_cap_for_paging_s&       ue_radio_cap_for_paging();
    unbounded_octstring<true>&       ue_radio_cap_eutra_format();
    const uint64_t&                  amf_ue_ngap_id() const;
    const uint64_t&                  ran_ue_ngap_id() const;
    const unbounded_octstring<true>& ue_radio_cap() const;
    const ue_radio_cap_for_paging_s& ue_radio_cap_for_paging() const;
    const unbounded_octstring<true>& ue_radio_cap_eutra_format() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UETNLABindingReleaseRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ue_tnla_binding_release_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&       amf_ue_ngap_id();
    uint64_t&       ran_ue_ngap_id();
    const uint64_t& amf_ue_ngap_id() const;
    const uint64_t& ran_ue_ngap_id() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UplinkNASTransport-IEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ul_nas_transport_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        amf_ue_ngap_id,
        ran_ue_ngap_id,
        nas_pdu,
        user_location_info,
        w_agf_id_info,
        tngf_id_info,
        twif_id_info,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                        amf_ue_ngap_id();
    uint64_t&                        ran_ue_ngap_id();
    unbounded_octstring<true>&       nas_pdu();
    user_location_info_c&            user_location_info();
    unbounded_octstring<true>&       w_agf_id_info();
    unbounded_octstring<true>&       tngf_id_info();
    unbounded_octstring<true>&       twif_id_info();
    const uint64_t&                  amf_ue_ngap_id() const;
    const uint64_t&                  ran_ue_ngap_id() const;
    const unbounded_octstring<true>& nas_pdu() const;
    const user_location_info_c&      user_location_info() const;
    const unbounded_octstring<true>& w_agf_id_info() const;
    const unbounded_octstring<true>& tngf_id_info() const;
    const unbounded_octstring<true>& twif_id_info() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UplinkNonUEAssociatedNRPPaTransportIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ul_non_ue_associated_nrppa_transport_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { routing_id, nrppa_pdu, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    unbounded_octstring<true>&       routing_id();
    unbounded_octstring<true>&       nrppa_pdu();
    const unbounded_octstring<true>& routing_id() const;
    const unbounded_octstring<true>& nrppa_pdu() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UplinkRANConfigurationTransferIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ul_ran_cfg_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { son_cfg_transfer_ul, endc_son_cfg_transfer_ul, intersys_son_cfg_transfer_ul, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    son_cfg_transfer_s&                son_cfg_transfer_ul();
    unbounded_octstring<true>&         endc_son_cfg_transfer_ul();
    intersys_son_cfg_transfer_s&       intersys_son_cfg_transfer_ul();
    const son_cfg_transfer_s&          son_cfg_transfer_ul() const;
    const unbounded_octstring<true>&   endc_son_cfg_transfer_ul() const;
    const intersys_son_cfg_transfer_s& intersys_son_cfg_transfer_ul() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UplinkRANEarlyStatusTransferIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ul_ran_early_status_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, early_status_transfer_transparent_container, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                            amf_ue_ngap_id();
    uint64_t&                                            ran_ue_ngap_id();
    early_status_transfer_transparent_container_s&       early_status_transfer_transparent_container();
    const uint64_t&                                      amf_ue_ngap_id() const;
    const uint64_t&                                      ran_ue_ngap_id() const;
    const early_status_transfer_transparent_container_s& early_status_transfer_transparent_container() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UplinkRANStatusTransferIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ul_ran_status_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, ran_status_transfer_transparent_container, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                          amf_ue_ngap_id();
    uint64_t&                                          ran_ue_ngap_id();
    ran_status_transfer_transparent_container_s&       ran_status_transfer_transparent_container();
    const uint64_t&                                    amf_ue_ngap_id() const;
    const uint64_t&                                    ran_ue_ngap_id() const;
    const ran_status_transfer_transparent_container_s& ran_status_transfer_transparent_container() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UplinkRIMInformationTransferIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ul_rim_info_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { rim_info_transfer, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::rim_info_transfer; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rim_info_transfer_s&       rim_info_transfer() { return c; }
    const rim_info_transfer_s& rim_info_transfer() const { return c; }

  private:
    rim_info_transfer_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UplinkUEAssociatedNRPPaTransportIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct ul_ue_associated_nrppa_transport_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { amf_ue_ngap_id, ran_ue_ngap_id, routing_id, nrppa_pdu, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                        amf_ue_ngap_id();
    uint64_t&                        ran_ue_ngap_id();
    unbounded_octstring<true>&       routing_id();
    unbounded_octstring<true>&       nrppa_pdu();
    const uint64_t&                  amf_ue_ngap_id() const;
    const uint64_t&                  ran_ue_ngap_id() const;
    const unbounded_octstring<true>& routing_id() const;
    const unbounded_octstring<true>& nrppa_pdu() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// WriteReplaceWarningRequestIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct write_replace_warning_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        msg_id,
        serial_num,
        warning_area_list,
        repeat_period,
        nof_broadcasts_requested,
        warning_type,
        warning_security_info,
        data_coding_scheme,
        warning_msg_contents,
        concurrent_warning_msg_ind,
        warning_area_coordinates,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<16, false, true>&       msg_id();
    fixed_bitstring<16, false, true>&       serial_num();
    warning_area_list_c&                    warning_area_list();
    uint32_t&                               repeat_period();
    uint32_t&                               nof_broadcasts_requested();
    fixed_octstring<2, true>&               warning_type();
    fixed_octstring<50, true>&              warning_security_info();
    fixed_bitstring<8, false, true>&        data_coding_scheme();
    bounded_octstring<1, 9600, true>&       warning_msg_contents();
    concurrent_warning_msg_ind_e&           concurrent_warning_msg_ind();
    bounded_octstring<1, 1024, true>&       warning_area_coordinates();
    const fixed_bitstring<16, false, true>& msg_id() const;
    const fixed_bitstring<16, false, true>& serial_num() const;
    const warning_area_list_c&              warning_area_list() const;
    const uint32_t&                         repeat_period() const;
    const uint32_t&                         nof_broadcasts_requested() const;
    const fixed_octstring<2, true>&         warning_type() const;
    const fixed_octstring<50, true>&        warning_security_info() const;
    const fixed_bitstring<8, false, true>&  data_coding_scheme() const;
    const bounded_octstring<1, 9600, true>& warning_msg_contents() const;
    const concurrent_warning_msg_ind_e&     concurrent_warning_msg_ind() const;
    const bounded_octstring<1, 1024, true>& warning_area_coordinates() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// WriteReplaceWarningResponseIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct write_replace_warning_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { msg_id, serial_num, broadcast_completed_area_list, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<16, false, true>&       msg_id();
    fixed_bitstring<16, false, true>&       serial_num();
    broadcast_completed_area_list_c&        broadcast_completed_area_list();
    crit_diagnostics_s&                     crit_diagnostics();
    const fixed_bitstring<16, false, true>& msg_id() const;
    const fixed_bitstring<16, false, true>& serial_num() const;
    const broadcast_completed_area_list_c&  broadcast_completed_area_list() const;
    const crit_diagnostics_s&               crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct location_report_ies_container {
  bool                                   ue_presence_in_area_of_interest_list_present = false;
  uint64_t                               amf_ue_ngap_id;
  uint64_t                               ran_ue_ngap_id;
  user_location_info_c                   user_location_info;
  ue_presence_in_area_of_interest_list_l ue_presence_in_area_of_interest_list;
  location_report_request_type_s         location_report_request_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LocationReport ::= SEQUENCE
using location_report_s = elementary_procedure_option<location_report_ies_container>;

struct location_report_ctrl_ies_container {
  uint64_t                       amf_ue_ngap_id;
  uint64_t                       ran_ue_ngap_id;
  location_report_request_type_s location_report_request_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LocationReportingControl ::= SEQUENCE
using location_report_ctrl_s = elementary_procedure_option<location_report_ctrl_ies_container>;

struct location_report_fail_ind_ies_container {
  uint64_t amf_ue_ngap_id;
  uint64_t ran_ue_ngap_id;
  cause_c  cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LocationReportingFailureIndication ::= SEQUENCE
using location_report_fail_ind_s = elementary_procedure_option<location_report_fail_ind_ies_container>;

struct multicast_group_paging_ies_container {
  bool                               mbs_service_area_present = false;
  mbs_session_id_s                   mbs_session_id;
  mbs_service_area_c                 mbs_service_area;
  multicast_group_paging_area_list_l multicast_group_paging_area_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastGroupPaging ::= SEQUENCE
using multicast_group_paging_s = elementary_procedure_option<multicast_group_paging_ies_container>;

struct multicast_session_activation_fail_ies_container {
  bool               crit_diagnostics_present = false;
  mbs_session_id_s   mbs_session_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastSessionActivationFailure ::= SEQUENCE
using multicast_session_activation_fail_s =
    elementary_procedure_option<multicast_session_activation_fail_ies_container>;

struct multicast_session_activation_request_ies_container {
  mbs_session_id_s          mbs_session_id;
  unbounded_octstring<true> multicast_session_activation_request_transfer;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastSessionActivationRequest ::= SEQUENCE
using multicast_session_activation_request_s =
    elementary_procedure_option<multicast_session_activation_request_ies_container>;

struct multicast_session_activation_resp_ies_container {
  bool               crit_diagnostics_present = false;
  mbs_session_id_s   mbs_session_id;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastSessionActivationResponse ::= SEQUENCE
using multicast_session_activation_resp_s =
    elementary_procedure_option<multicast_session_activation_resp_ies_container>;

struct multicast_session_deactivation_request_ies_container {
  mbs_session_id_s          mbs_session_id;
  unbounded_octstring<true> multicast_session_deactivation_request_transfer;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastSessionDeactivationRequest ::= SEQUENCE
using multicast_session_deactivation_request_s =
    elementary_procedure_option<multicast_session_deactivation_request_ies_container>;

struct multicast_session_deactivation_resp_ies_container {
  bool               crit_diagnostics_present = false;
  mbs_session_id_s   mbs_session_id;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastSessionDeactivationResponse ::= SEQUENCE
using multicast_session_deactivation_resp_s =
    elementary_procedure_option<multicast_session_deactivation_resp_ies_container>;

struct multicast_session_upd_fail_ies_container {
  bool               mbs_area_session_id_present = false;
  bool               crit_diagnostics_present    = false;
  mbs_session_id_s   mbs_session_id;
  uint32_t           mbs_area_session_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastSessionUpdateFailure ::= SEQUENCE
using multicast_session_upd_fail_s = elementary_procedure_option<multicast_session_upd_fail_ies_container>;

struct multicast_session_upd_request_ies_container {
  bool                      mbs_area_session_id_present = false;
  mbs_session_id_s          mbs_session_id;
  uint32_t                  mbs_area_session_id;
  unbounded_octstring<true> multicast_session_upd_request_transfer;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastSessionUpdateRequest ::= SEQUENCE
using multicast_session_upd_request_s = elementary_procedure_option<multicast_session_upd_request_ies_container>;

struct multicast_session_upd_resp_ies_container {
  bool               mbs_area_session_id_present = false;
  bool               crit_diagnostics_present    = false;
  mbs_session_id_s   mbs_session_id;
  uint32_t           mbs_area_session_id;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastSessionUpdateResponse ::= SEQUENCE
using multicast_session_upd_resp_s = elementary_procedure_option<multicast_session_upd_resp_ies_container>;

struct nas_non_delivery_ind_ies_container {
  uint64_t                  amf_ue_ngap_id;
  uint64_t                  ran_ue_ngap_id;
  unbounded_octstring<true> nas_pdu;
  cause_c                   cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NASNonDeliveryIndication ::= SEQUENCE
using nas_non_delivery_ind_s = elementary_procedure_option<nas_non_delivery_ind_ies_container>;

struct ng_reset_ies_container {
  cause_c      cause;
  reset_type_c reset_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NGReset ::= SEQUENCE
using ng_reset_s = elementary_procedure_option<ng_reset_ies_container>;

struct ng_reset_ack_ies_container {
  bool                            ue_associated_lc_ng_conn_list_present = false;
  bool                            crit_diagnostics_present              = false;
  ue_associated_lc_ng_conn_list_l ue_associated_lc_ng_conn_list;
  crit_diagnostics_s              crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NGResetAcknowledge ::= SEQUENCE
using ng_reset_ack_s = elementary_procedure_option<ng_reset_ack_ies_container>;

struct ng_setup_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NGSetupFailure ::= SEQUENCE
using ng_setup_fail_s = elementary_procedure_option<ng_setup_fail_ies_container>;

struct ng_setup_request_ies_container {
  bool                                 ran_node_name_present             = false;
  bool                                 ue_retention_info_present         = false;
  bool                                 nb_iot_default_paging_drx_present = false;
  bool                                 extended_ran_node_name_present    = false;
  global_ran_node_id_c                 global_ran_node_id;
  printable_string<1, 150, true, true> ran_node_name;
  supported_ta_list_l                  supported_ta_list;
  paging_drx_e                         default_paging_drx;
  ue_retention_info_e                  ue_retention_info;
  nb_iot_default_paging_drx_e          nb_iot_default_paging_drx;
  extended_ran_node_name_s             extended_ran_node_name;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NGSetupRequest ::= SEQUENCE
using ng_setup_request_s = elementary_procedure_option<ng_setup_request_ies_container>;

struct ng_setup_resp_ies_container {
  bool                                 crit_diagnostics_present  = false;
  bool                                 ue_retention_info_present = false;
  bool                                 iab_supported_present     = false;
  bool                                 extended_amf_name_present = false;
  printable_string<1, 150, true, true> amf_name;
  served_guami_list_l                  served_guami_list;
  uint16_t                             relative_amf_capacity;
  plmn_support_list_l                  plmn_support_list;
  crit_diagnostics_s                   crit_diagnostics;
  ue_retention_info_e                  ue_retention_info;
  iab_supported_e                      iab_supported;
  extended_amf_name_s                  extended_amf_name;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NGSetupResponse ::= SEQUENCE
using ng_setup_resp_s = elementary_procedure_option<ng_setup_resp_ies_container>;

struct overload_start_ies_container {
  bool                        amf_overload_resp_present              = false;
  bool                        amf_traffic_load_reduction_ind_present = false;
  bool                        overload_start_nssai_list_present      = false;
  overload_resp_c             amf_overload_resp;
  uint8_t                     amf_traffic_load_reduction_ind;
  overload_start_nssai_list_l overload_start_nssai_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OverloadStart ::= SEQUENCE
using overload_start_s = elementary_procedure_option<overload_start_ies_container>;

using overload_stop_ies_container = protocol_ie_container_empty_l;

// OverloadStop ::= SEQUENCE
using overload_stop_s = elementary_procedure_option<overload_stop_ies_container>;

struct pdu_session_res_modify_confirm_ies_container {
  bool                                            pdu_session_res_modify_list_mod_cfm_present           = false;
  bool                                            pdu_session_res_failed_to_modify_list_mod_cfm_present = false;
  bool                                            crit_diagnostics_present                              = false;
  uint64_t                                        amf_ue_ngap_id;
  uint64_t                                        ran_ue_ngap_id;
  pdu_session_res_modify_list_mod_cfm_l           pdu_session_res_modify_list_mod_cfm;
  pdu_session_res_failed_to_modify_list_mod_cfm_l pdu_session_res_failed_to_modify_list_mod_cfm;
  crit_diagnostics_s                              crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyConfirm ::= SEQUENCE
using pdu_session_res_modify_confirm_s = elementary_procedure_option<pdu_session_res_modify_confirm_ies_container>;

struct pdu_session_res_modify_ind_ies_container {
  bool                                  user_location_info_present = false;
  uint64_t                              amf_ue_ngap_id;
  uint64_t                              ran_ue_ngap_id;
  pdu_session_res_modify_list_mod_ind_l pdu_session_res_modify_list_mod_ind;
  user_location_info_c                  user_location_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyIndication ::= SEQUENCE
using pdu_session_res_modify_ind_s = elementary_procedure_option<pdu_session_res_modify_ind_ies_container>;

struct pdu_session_res_modify_request_ies_container {
  bool                                  ran_paging_prio_present = false;
  uint64_t                              amf_ue_ngap_id;
  uint64_t                              ran_ue_ngap_id;
  uint16_t                              ran_paging_prio;
  pdu_session_res_modify_list_mod_req_l pdu_session_res_modify_list_mod_req;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyRequest ::= SEQUENCE
using pdu_session_res_modify_request_s = elementary_procedure_option<pdu_session_res_modify_request_ies_container>;

struct pdu_session_res_modify_resp_ies_container {
  bool                                            pdu_session_res_modify_list_mod_res_present           = false;
  bool                                            pdu_session_res_failed_to_modify_list_mod_res_present = false;
  bool                                            user_location_info_present                            = false;
  bool                                            crit_diagnostics_present                              = false;
  uint64_t                                        amf_ue_ngap_id;
  uint64_t                                        ran_ue_ngap_id;
  pdu_session_res_modify_list_mod_res_l           pdu_session_res_modify_list_mod_res;
  pdu_session_res_failed_to_modify_list_mod_res_l pdu_session_res_failed_to_modify_list_mod_res;
  user_location_info_c                            user_location_info;
  crit_diagnostics_s                              crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyResponse ::= SEQUENCE
using pdu_session_res_modify_resp_s = elementary_procedure_option<pdu_session_res_modify_resp_ies_container>;

struct pdu_session_res_notify_ies_container {
  bool                                pdu_session_res_notify_list_present       = false;
  bool                                pdu_session_res_released_list_not_present = false;
  bool                                user_location_info_present                = false;
  uint64_t                            amf_ue_ngap_id;
  uint64_t                            ran_ue_ngap_id;
  pdu_session_res_notify_list_l       pdu_session_res_notify_list;
  pdu_session_res_released_list_not_l pdu_session_res_released_list_not;
  user_location_info_c                user_location_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceNotify ::= SEQUENCE
using pdu_session_res_notify_s = elementary_procedure_option<pdu_session_res_notify_ies_container>;

struct pdu_session_res_release_cmd_ies_container {
  bool                                      ran_paging_prio_present = false;
  bool                                      nas_pdu_present         = false;
  uint64_t                                  amf_ue_ngap_id;
  uint64_t                                  ran_ue_ngap_id;
  uint16_t                                  ran_paging_prio;
  unbounded_octstring<true>                 nas_pdu;
  pdu_session_res_to_release_list_rel_cmd_l pdu_session_res_to_release_list_rel_cmd;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceReleaseCommand ::= SEQUENCE
using pdu_session_res_release_cmd_s = elementary_procedure_option<pdu_session_res_release_cmd_ies_container>;

struct pdu_session_res_release_resp_ies_container {
  bool                                    user_location_info_present = false;
  bool                                    crit_diagnostics_present   = false;
  uint64_t                                amf_ue_ngap_id;
  uint64_t                                ran_ue_ngap_id;
  pdu_session_res_released_list_rel_res_l pdu_session_res_released_list_rel_res;
  user_location_info_c                    user_location_info;
  crit_diagnostics_s                      crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceReleaseResponse ::= SEQUENCE
using pdu_session_res_release_resp_s = elementary_procedure_option<pdu_session_res_release_resp_ies_container>;

struct pdu_session_res_setup_request_ies_container {
  bool                                ran_paging_prio_present            = false;
  bool                                nas_pdu_present                    = false;
  bool                                ue_aggr_max_bit_rate_present       = false;
  bool                                ue_slice_max_bit_rate_list_present = false;
  uint64_t                            amf_ue_ngap_id;
  uint64_t                            ran_ue_ngap_id;
  uint16_t                            ran_paging_prio;
  unbounded_octstring<true>           nas_pdu;
  pdu_session_res_setup_list_su_req_l pdu_session_res_setup_list_su_req;
  ue_aggr_max_bit_rate_s              ue_aggr_max_bit_rate;
  ue_slice_max_bit_rate_list_l        ue_slice_max_bit_rate_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSetupRequest ::= SEQUENCE
using pdu_session_res_setup_request_s = elementary_procedure_option<pdu_session_res_setup_request_ies_container>;

struct pdu_session_res_setup_resp_ies_container {
  bool                                          pdu_session_res_setup_list_su_res_present           = false;
  bool                                          pdu_session_res_failed_to_setup_list_su_res_present = false;
  bool                                          crit_diagnostics_present                            = false;
  bool                                          user_location_info_present                          = false;
  uint64_t                                      amf_ue_ngap_id;
  uint64_t                                      ran_ue_ngap_id;
  pdu_session_res_setup_list_su_res_l           pdu_session_res_setup_list_su_res;
  pdu_session_res_failed_to_setup_list_su_res_l pdu_session_res_failed_to_setup_list_su_res;
  crit_diagnostics_s                            crit_diagnostics;
  user_location_info_c                          user_location_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSetupResponse ::= SEQUENCE
using pdu_session_res_setup_resp_s = elementary_procedure_option<pdu_session_res_setup_resp_ies_container>;

struct pws_cancel_request_ies_container {
  bool                             warning_area_list_present       = false;
  bool                             cancel_all_warning_msgs_present = false;
  fixed_bitstring<16, false, true> msg_id;
  fixed_bitstring<16, false, true> serial_num;
  warning_area_list_c              warning_area_list;
  cancel_all_warning_msgs_e        cancel_all_warning_msgs;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSCancelRequest ::= SEQUENCE
using pws_cancel_request_s = elementary_procedure_option<pws_cancel_request_ies_container>;

struct pws_cancel_resp_ies_container {
  bool                             broadcast_cancelled_area_list_present = false;
  bool                             crit_diagnostics_present              = false;
  fixed_bitstring<16, false, true> msg_id;
  fixed_bitstring<16, false, true> serial_num;
  broadcast_cancelled_area_list_c  broadcast_cancelled_area_list;
  crit_diagnostics_s               crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSCancelResponse ::= SEQUENCE
using pws_cancel_resp_s = elementary_procedure_option<pws_cancel_resp_ies_container>;

struct pws_fail_ind_ies_container {
  pws_failed_cell_id_list_c pws_failed_cell_id_list;
  global_ran_node_id_c      global_ran_node_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSFailureIndication ::= SEQUENCE
using pws_fail_ind_s = elementary_procedure_option<pws_fail_ind_ies_container>;

struct pws_restart_ind_ies_container {
  bool                                 emergency_area_id_list_for_restart_present = false;
  cell_id_list_for_restart_c           cell_id_list_for_restart;
  global_ran_node_id_c                 global_ran_node_id;
  tai_list_for_restart_l               tai_list_for_restart;
  emergency_area_id_list_for_restart_l emergency_area_id_list_for_restart;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSRestartIndication ::= SEQUENCE
using pws_restart_ind_s = elementary_procedure_option<pws_restart_ind_ies_container>;

struct paging_ies_container {
  bool                         paging_drx_present                 = false;
  bool                         paging_prio_present                = false;
  bool                         ue_radio_cap_for_paging_present    = false;
  bool                         paging_origin_present              = false;
  bool                         assist_data_for_paging_present     = false;
  bool                         nb_iot_paging_e_drx_info_present   = false;
  bool                         nb_iot_paging_drx_present          = false;
  bool                         enhanced_coverage_restrict_present = false;
  bool                         wus_assist_info_present            = false;
  bool                         eutra_paginge_drx_info_present     = false;
  bool                         ce_mode_brestricted_present        = false;
  bool                         nr_paginge_drx_info_present        = false;
  bool                         paging_cause_present               = false;
  bool                         pe_ip_sassist_info_present         = false;
  ue_paging_id_c               ue_paging_id;
  paging_drx_e                 paging_drx;
  tai_list_for_paging_l        tai_list_for_paging;
  paging_prio_e                paging_prio;
  ue_radio_cap_for_paging_s    ue_radio_cap_for_paging;
  paging_origin_e              paging_origin;
  assist_data_for_paging_s     assist_data_for_paging;
  nb_iot_paging_e_drx_info_s   nb_iot_paging_e_drx_info;
  nb_iot_paging_drx_e          nb_iot_paging_drx;
  enhanced_coverage_restrict_e enhanced_coverage_restrict;
  wus_assist_info_s            wus_assist_info;
  eutra_paginge_drx_info_s     eutra_paginge_drx_info;
  ce_mode_brestricted_e        ce_mode_brestricted;
  nr_paginge_drx_info_s        nr_paginge_drx_info;
  paging_cause_e               paging_cause;
  pe_ip_sassist_info_s         pe_ip_sassist_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Paging ::= SEQUENCE
using paging_s = elementary_procedure_option<paging_ies_container>;

struct path_switch_request_ies_container {
  bool                                          pdu_session_res_failed_to_setup_list_ps_req_present = false;
  bool                                          rrc_resume_cause_present                            = false;
  bool                                          red_cap_ind_present                                 = false;
  uint64_t                                      ran_ue_ngap_id;
  uint64_t                                      source_amf_ue_ngap_id;
  user_location_info_c                          user_location_info;
  ue_security_cap_s                             ue_security_cap;
  pdu_session_res_to_be_switched_dl_list_l      pdu_session_res_to_be_switched_dl_list;
  pdu_session_res_failed_to_setup_list_ps_req_l pdu_session_res_failed_to_setup_list_ps_req;
  rrc_establishment_cause_e                     rrc_resume_cause;
  red_cap_ind_e                                 red_cap_ind;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathSwitchRequest ::= SEQUENCE
using path_switch_request_s = elementary_procedure_option<path_switch_request_ies_container>;

struct path_switch_request_ack_ies_container {
  bool                                     ue_security_cap_present                        = false;
  bool                                     new_security_context_ind_present               = false;
  bool                                     pdu_session_res_released_list_ps_ack_present   = false;
  bool                                     core_network_assist_info_for_inactive_present  = false;
  bool                                     rrc_inactive_transition_report_request_present = false;
  bool                                     crit_diagnostics_present                       = false;
  bool                                     redirection_voice_fallback_present             = false;
  bool                                     cn_assisted_ran_tuning_present                 = false;
  bool                                     srvcc_operation_possible_present               = false;
  bool                                     enhanced_coverage_restrict_present             = false;
  bool                                     extended_connected_time_present                = false;
  bool                                     ue_diff_info_present                           = false;
  bool                                     nr_v2x_services_authorized_present             = false;
  bool                                     ltev2x_services_authorized_present             = false;
  bool                                     nr_ue_sidelink_aggr_max_bitrate_present        = false;
  bool                                     lte_ue_sidelink_aggr_max_bitrate_present       = false;
  bool                                     pc5_qos_params_present                         = false;
  bool                                     ce_mode_brestricted_present                    = false;
  bool                                     ue_up_c_iot_support_present                    = false;
  bool                                     ue_radio_cap_id_present                        = false;
  bool                                     management_based_mdt_plmn_list_present         = false;
  bool                                     time_sync_assist_info_present                  = false;
  bool                                     five_g_pro_se_authorized_present               = false;
  bool                                     five_g_pro_se_ue_pc5_aggr_max_bit_rate_present = false;
  bool                                     five_g_pro_se_pc5_qos_params_present           = false;
  bool                                     management_based_mdt_plmn_mod_list_present     = false;
  uint64_t                                 amf_ue_ngap_id;
  uint64_t                                 ran_ue_ngap_id;
  ue_security_cap_s                        ue_security_cap;
  security_context_s                       security_context;
  new_security_context_ind_e               new_security_context_ind;
  pdu_session_res_switched_list_l          pdu_session_res_switched_list;
  pdu_session_res_released_list_ps_ack_l   pdu_session_res_released_list_ps_ack;
  allowed_nssai_l                          allowed_nssai;
  core_network_assist_info_for_inactive_s  core_network_assist_info_for_inactive;
  rrc_inactive_transition_report_request_e rrc_inactive_transition_report_request;
  crit_diagnostics_s                       crit_diagnostics;
  redirection_voice_fallback_e             redirection_voice_fallback;
  cn_assisted_ran_tuning_s                 cn_assisted_ran_tuning;
  srvcc_operation_possible_e               srvcc_operation_possible;
  enhanced_coverage_restrict_e             enhanced_coverage_restrict;
  uint16_t                                 extended_connected_time;
  ue_diff_info_s                           ue_diff_info;
  nr_v2x_services_authorized_s             nr_v2x_services_authorized;
  ltev2x_services_authorized_s             ltev2x_services_authorized;
  nr_ue_sidelink_aggr_max_bitrate_s        nr_ue_sidelink_aggr_max_bitrate;
  lte_ue_sidelink_aggr_max_bitrate_s       lte_ue_sidelink_aggr_max_bitrate;
  pc5_qos_params_s                         pc5_qos_params;
  ce_mode_brestricted_e                    ce_mode_brestricted;
  ue_up_c_iot_support_e                    ue_up_c_iot_support;
  unbounded_octstring<true>                ue_radio_cap_id;
  mdt_plmn_list_l                          management_based_mdt_plmn_list;
  time_sync_assist_info_s                  time_sync_assist_info;
  five_g_pro_se_authorized_s               five_g_pro_se_authorized;
  nr_ue_sidelink_aggr_max_bitrate_s        five_g_pro_se_ue_pc5_aggr_max_bit_rate;
  five_g_pro_se_pc5_qos_params_s           five_g_pro_se_pc5_qos_params;
  mdt_plmn_mod_list_l                      management_based_mdt_plmn_mod_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathSwitchRequestAcknowledge ::= SEQUENCE
using path_switch_request_ack_s = elementary_procedure_option<path_switch_request_ack_ies_container>;

struct path_switch_request_fail_ies_container {
  bool                                    crit_diagnostics_present = false;
  uint64_t                                amf_ue_ngap_id;
  uint64_t                                ran_ue_ngap_id;
  pdu_session_res_released_list_ps_fail_l pdu_session_res_released_list_ps_fail;
  crit_diagnostics_s                      crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathSwitchRequestFailure ::= SEQUENCE
using path_switch_request_fail_s = elementary_procedure_option<path_switch_request_fail_ies_container>;

struct private_ie_container_empty_l {
  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};
using private_msg_ies_container = private_ie_container_empty_l;

// PrivateMessage ::= SEQUENCE
struct private_msg_s {
  bool                      ext = false;
  private_msg_ies_container private_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ran_cp_relocation_ind_ies_container {
  uint64_t              ran_ue_ngap_id;
  five_g_s_tmsi_s       five_g_s_tmsi;
  eutra_cgi_s           eutra_cgi;
  tai_s                 tai;
  ul_cp_security_info_s ul_cp_security_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANCPRelocationIndication ::= SEQUENCE
using ran_cp_relocation_ind_s = elementary_procedure_option<ran_cp_relocation_ind_ies_container>;

struct ran_cfg_upd_ies_container {
  bool                                 ran_node_name_present               = false;
  bool                                 supported_ta_list_present           = false;
  bool                                 default_paging_drx_present          = false;
  bool                                 global_ran_node_id_present          = false;
  bool                                 ngran_tnl_assoc_to_rem_list_present = false;
  bool                                 nb_iot_default_paging_drx_present   = false;
  bool                                 extended_ran_node_name_present      = false;
  printable_string<1, 150, true, true> ran_node_name;
  supported_ta_list_l                  supported_ta_list;
  paging_drx_e                         default_paging_drx;
  global_ran_node_id_c                 global_ran_node_id;
  ngran_tnl_assoc_to_rem_list_l        ngran_tnl_assoc_to_rem_list;
  nb_iot_default_paging_drx_e          nb_iot_default_paging_drx;
  extended_ran_node_name_s             extended_ran_node_name;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANConfigurationUpdate ::= SEQUENCE
using ran_cfg_upd_s = elementary_procedure_option<ran_cfg_upd_ies_container>;

// RANConfigurationUpdateAcknowledge ::= SEQUENCE
using ran_cfg_upd_ack_s = elementary_procedure_option<protocol_ie_container_l<ran_cfg_upd_ack_ies_o>>;

struct ran_cfg_upd_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANConfigurationUpdateFailure ::= SEQUENCE
using ran_cfg_upd_fail_s = elementary_procedure_option<ran_cfg_upd_fail_ies_container>;

struct rrc_inactive_transition_report_ies_container {
  uint64_t             amf_ue_ngap_id;
  uint64_t             ran_ue_ngap_id;
  rrc_state_e          rrc_state;
  user_location_info_c user_location_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCInactiveTransitionReport ::= SEQUENCE
using rrc_inactive_transition_report_s = elementary_procedure_option<rrc_inactive_transition_report_ies_container>;

struct reroute_nas_request_ies_container {
  bool                                amf_ue_ngap_id_present                    = false;
  bool                                allowed_nssai_present                     = false;
  bool                                source_to_target_amf_info_reroute_present = false;
  uint64_t                            ran_ue_ngap_id;
  uint64_t                            amf_ue_ngap_id;
  unbounded_octstring<true>           ngap_msg;
  fixed_bitstring<10, false, true>    amf_set_id;
  allowed_nssai_l                     allowed_nssai;
  source_to_target_amf_info_reroute_s source_to_target_amf_info_reroute;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RerouteNASRequest ::= SEQUENCE
using reroute_nas_request_s = elementary_procedure_option<reroute_nas_request_ies_container>;

// RetrieveUEInformation ::= SEQUENCE
using retrieve_ue_info_s = elementary_procedure_option<protocol_ie_container_l<retrieve_ue_info_ies_o>>;

struct secondary_rat_data_usage_report_ies_container {
  bool                                       ho_flag_present            = false;
  bool                                       user_location_info_present = false;
  uint64_t                                   amf_ue_ngap_id;
  uint64_t                                   ran_ue_ngap_id;
  pdu_session_res_secondary_rat_usage_list_l pdu_session_res_secondary_rat_usage_list;
  ho_flag_e                                  ho_flag;
  user_location_info_c                       user_location_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecondaryRATDataUsageReport ::= SEQUENCE
using secondary_rat_data_usage_report_s = elementary_procedure_option<secondary_rat_data_usage_report_ies_container>;

struct trace_fail_ind_ies_container {
  uint64_t                 amf_ue_ngap_id;
  uint64_t                 ran_ue_ngap_id;
  fixed_octstring<8, true> ngran_trace_id;
  cause_c                  cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TraceFailureIndication ::= SEQUENCE
using trace_fail_ind_s = elementary_procedure_option<trace_fail_ind_ies_container>;

struct trace_start_ies_container {
  uint64_t           amf_ue_ngap_id;
  uint64_t           ran_ue_ngap_id;
  trace_activation_s trace_activation;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TraceStart ::= SEQUENCE
using trace_start_s = elementary_procedure_option<trace_start_ies_container>;

struct ue_context_mod_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           amf_ue_ngap_id;
  uint64_t           ran_ue_ngap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationFailure ::= SEQUENCE
using ue_context_mod_fail_s = elementary_procedure_option<ue_context_mod_fail_ies_container>;

struct ue_context_mod_request_ies_container {
  bool                                     ran_paging_prio_present                          = false;
  bool                                     security_key_present                             = false;
  bool                                     idx_to_rfsp_present                              = false;
  bool                                     ue_aggr_max_bit_rate_present                     = false;
  bool                                     ue_security_cap_present                          = false;
  bool                                     core_network_assist_info_for_inactive_present    = false;
  bool                                     emergency_fallback_ind_present                   = false;
  bool                                     new_amf_ue_ngap_id_present                       = false;
  bool                                     rrc_inactive_transition_report_request_present   = false;
  bool                                     new_guami_present                                = false;
  bool                                     cn_assisted_ran_tuning_present                   = false;
  bool                                     srvcc_operation_possible_present                 = false;
  bool                                     iab_authorized_present                           = false;
  bool                                     nr_v2x_services_authorized_present               = false;
  bool                                     ltev2x_services_authorized_present               = false;
  bool                                     nr_ue_sidelink_aggr_max_bitrate_present          = false;
  bool                                     lte_ue_sidelink_aggr_max_bitrate_present         = false;
  bool                                     pc5_qos_params_present                           = false;
  bool                                     ue_radio_cap_id_present                          = false;
  bool                                     rg_level_wireline_access_characteristics_present = false;
  bool                                     time_sync_assist_info_present                    = false;
  bool                                     q_mcc_onfig_info_present                         = false;
  bool                                     qmc_deactivation_present                         = false;
  bool                                     ue_slice_max_bit_rate_list_present               = false;
  bool                                     management_based_mdt_plmn_mod_list_present       = false;
  bool                                     five_g_pro_se_authorized_present                 = false;
  bool                                     five_g_pro_se_ue_pc5_aggr_max_bit_rate_present   = false;
  bool                                     five_g_pro_se_pc5_qos_params_present             = false;
  uint64_t                                 amf_ue_ngap_id;
  uint64_t                                 ran_ue_ngap_id;
  uint16_t                                 ran_paging_prio;
  fixed_bitstring<256, false, true>        security_key;
  uint16_t                                 idx_to_rfsp;
  ue_aggr_max_bit_rate_s                   ue_aggr_max_bit_rate;
  ue_security_cap_s                        ue_security_cap;
  core_network_assist_info_for_inactive_s  core_network_assist_info_for_inactive;
  emergency_fallback_ind_s                 emergency_fallback_ind;
  uint64_t                                 new_amf_ue_ngap_id;
  rrc_inactive_transition_report_request_e rrc_inactive_transition_report_request;
  guami_s                                  new_guami;
  cn_assisted_ran_tuning_s                 cn_assisted_ran_tuning;
  srvcc_operation_possible_e               srvcc_operation_possible;
  iab_authorized_e                         iab_authorized;
  nr_v2x_services_authorized_s             nr_v2x_services_authorized;
  ltev2x_services_authorized_s             ltev2x_services_authorized;
  nr_ue_sidelink_aggr_max_bitrate_s        nr_ue_sidelink_aggr_max_bitrate;
  lte_ue_sidelink_aggr_max_bitrate_s       lte_ue_sidelink_aggr_max_bitrate;
  pc5_qos_params_s                         pc5_qos_params;
  unbounded_octstring<true>                ue_radio_cap_id;
  unbounded_octstring<true>                rg_level_wireline_access_characteristics;
  time_sync_assist_info_s                  time_sync_assist_info;
  q_mcc_onfig_info_s                       q_mcc_onfig_info;
  qmc_deactivation_s                       qmc_deactivation;
  ue_slice_max_bit_rate_list_l             ue_slice_max_bit_rate_list;
  mdt_plmn_mod_list_l                      management_based_mdt_plmn_mod_list;
  five_g_pro_se_authorized_s               five_g_pro_se_authorized;
  nr_ue_sidelink_aggr_max_bitrate_s        five_g_pro_se_ue_pc5_aggr_max_bit_rate;
  five_g_pro_se_pc5_qos_params_s           five_g_pro_se_pc5_qos_params;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationRequest ::= SEQUENCE
using ue_context_mod_request_s = elementary_procedure_option<ue_context_mod_request_ies_container>;

struct ue_context_mod_resp_ies_container {
  bool                 rrc_state_present          = false;
  bool                 user_location_info_present = false;
  bool                 crit_diagnostics_present   = false;
  uint64_t             amf_ue_ngap_id;
  uint64_t             ran_ue_ngap_id;
  rrc_state_e          rrc_state;
  user_location_info_c user_location_info;
  crit_diagnostics_s   crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationResponse ::= SEQUENCE
using ue_context_mod_resp_s = elementary_procedure_option<ue_context_mod_resp_ies_container>;

struct ue_context_release_cmd_ies_container {
  ue_ngap_ids_c ue_ngap_ids;
  cause_c       cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextReleaseCommand ::= SEQUENCE
using ue_context_release_cmd_s = elementary_procedure_option<ue_context_release_cmd_ies_container>;

struct ue_context_release_complete_ies_container {
  bool                 user_location_info_present                                 = false;
  bool                 info_on_recommended_cells_and_ran_nodes_for_paging_present = false;
  bool                 pdu_session_res_list_cxt_rel_cpl_present                   = false;
  bool                 crit_diagnostics_present                                   = false;
  bool                 paging_assis_datafor_c_ecapab_ue_present                   = false;
  uint64_t             amf_ue_ngap_id;
  uint64_t             ran_ue_ngap_id;
  user_location_info_c user_location_info;
  info_on_recommended_cells_and_ran_nodes_for_paging_s info_on_recommended_cells_and_ran_nodes_for_paging;
  pdu_session_res_list_cxt_rel_cpl_l                   pdu_session_res_list_cxt_rel_cpl;
  crit_diagnostics_s                                   crit_diagnostics;
  paging_assis_datafor_c_ecapab_ue_s                   paging_assis_datafor_c_ecapab_ue;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextReleaseComplete ::= SEQUENCE
using ue_context_release_complete_s = elementary_procedure_option<ue_context_release_complete_ies_container>;

struct ue_context_release_request_ies_container {
  bool                               pdu_session_res_list_cxt_rel_req_present = false;
  uint64_t                           amf_ue_ngap_id;
  uint64_t                           ran_ue_ngap_id;
  pdu_session_res_list_cxt_rel_req_l pdu_session_res_list_cxt_rel_req;
  cause_c                            cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextReleaseRequest ::= SEQUENCE
using ue_context_release_request_s = elementary_procedure_option<ue_context_release_request_ies_container>;

struct ue_context_resume_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           amf_ue_ngap_id;
  uint64_t           ran_ue_ngap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextResumeFailure ::= SEQUENCE
using ue_context_resume_fail_s = elementary_procedure_option<ue_context_resume_fail_ies_container>;

struct ue_context_resume_request_ies_container {
  bool                                            pdu_session_res_resume_list_res_req_present                = false;
  bool                                            pdu_session_res_failed_to_resume_list_res_req_present      = false;
  bool                                            suspend_request_ind_present                                = false;
  bool                                            info_on_recommended_cells_and_ran_nodes_for_paging_present = false;
  bool                                            paging_assis_datafor_c_ecapab_ue_present                   = false;
  uint64_t                                        amf_ue_ngap_id;
  uint64_t                                        ran_ue_ngap_id;
  rrc_establishment_cause_e                       rrc_resume_cause;
  pdu_session_res_resume_list_res_req_l           pdu_session_res_resume_list_res_req;
  pdu_session_res_failed_to_resume_list_res_req_l pdu_session_res_failed_to_resume_list_res_req;
  suspend_request_ind_e                           suspend_request_ind;
  info_on_recommended_cells_and_ran_nodes_for_paging_s info_on_recommended_cells_and_ran_nodes_for_paging;
  paging_assis_datafor_c_ecapab_ue_s                   paging_assis_datafor_c_ecapab_ue;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextResumeRequest ::= SEQUENCE
using ue_context_resume_request_s = elementary_procedure_option<ue_context_resume_request_ies_container>;

struct ue_context_resume_resp_ies_container {
  bool                                            pdu_session_res_resume_list_res_res_present           = false;
  bool                                            pdu_session_res_failed_to_resume_list_res_res_present = false;
  bool                                            security_context_present                              = false;
  bool                                            suspend_resp_ind_present                              = false;
  bool                                            extended_connected_time_present                       = false;
  bool                                            crit_diagnostics_present                              = false;
  uint64_t                                        amf_ue_ngap_id;
  uint64_t                                        ran_ue_ngap_id;
  pdu_session_res_resume_list_res_res_l           pdu_session_res_resume_list_res_res;
  pdu_session_res_failed_to_resume_list_res_res_l pdu_session_res_failed_to_resume_list_res_res;
  security_context_s                              security_context;
  suspend_resp_ind_e                              suspend_resp_ind;
  uint16_t                                        extended_connected_time;
  crit_diagnostics_s                              crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextResumeResponse ::= SEQUENCE
using ue_context_resume_resp_s = elementary_procedure_option<ue_context_resume_resp_ies_container>;

struct ue_context_suspend_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           amf_ue_ngap_id;
  uint64_t           ran_ue_ngap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextSuspendFailure ::= SEQUENCE
using ue_context_suspend_fail_s = elementary_procedure_option<ue_context_suspend_fail_ies_container>;

struct ue_context_suspend_request_ies_container {
  bool     info_on_recommended_cells_and_ran_nodes_for_paging_present = false;
  bool     paging_assis_datafor_c_ecapab_ue_present                   = false;
  bool     pdu_session_res_suspend_list_sus_req_present               = false;
  uint64_t amf_ue_ngap_id;
  uint64_t ran_ue_ngap_id;
  info_on_recommended_cells_and_ran_nodes_for_paging_s info_on_recommended_cells_and_ran_nodes_for_paging;
  paging_assis_datafor_c_ecapab_ue_s                   paging_assis_datafor_c_ecapab_ue;
  pdu_session_res_suspend_list_sus_req_l               pdu_session_res_suspend_list_sus_req;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextSuspendRequest ::= SEQUENCE
using ue_context_suspend_request_s = elementary_procedure_option<ue_context_suspend_request_ies_container>;

struct ue_context_suspend_resp_ies_container {
  bool               security_context_present = false;
  bool               crit_diagnostics_present = false;
  uint64_t           amf_ue_ngap_id;
  uint64_t           ran_ue_ngap_id;
  security_context_s security_context;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextSuspendResponse ::= SEQUENCE
using ue_context_suspend_resp_s = elementary_procedure_option<ue_context_suspend_resp_ies_container>;

struct ue_info_transfer_ies_container {
  bool                             nb_iot_ue_prio_present = false;
  bool                             ue_radio_cap_present   = false;
  bool                             s_nssai_present        = false;
  bool                             allowed_nssai_present  = false;
  bool                             ue_diff_info_present   = false;
  bool                             masked_imeisv_present  = false;
  five_g_s_tmsi_s                  five_g_s_tmsi;
  uint16_t                         nb_iot_ue_prio;
  unbounded_octstring<true>        ue_radio_cap;
  s_nssai_s                        s_nssai;
  allowed_nssai_l                  allowed_nssai;
  ue_diff_info_s                   ue_diff_info;
  fixed_bitstring<64, false, true> masked_imeisv;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationTransfer ::= SEQUENCE
using ue_info_transfer_s = elementary_procedure_option<ue_info_transfer_ies_container>;

struct ue_radio_cap_check_request_ies_container {
  bool                      ue_radio_cap_present    = false;
  bool                      ue_radio_cap_id_present = false;
  uint64_t                  amf_ue_ngap_id;
  uint64_t                  ran_ue_ngap_id;
  unbounded_octstring<true> ue_radio_cap;
  unbounded_octstring<true> ue_radio_cap_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioCapabilityCheckRequest ::= SEQUENCE
using ue_radio_cap_check_request_s = elementary_procedure_option<ue_radio_cap_check_request_ies_container>;

struct ue_radio_cap_check_resp_ies_container {
  bool                    crit_diagnostics_present = false;
  uint64_t                amf_ue_ngap_id;
  uint64_t                ran_ue_ngap_id;
  ims_voice_support_ind_e ims_voice_support_ind;
  crit_diagnostics_s      crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioCapabilityCheckResponse ::= SEQUENCE
using ue_radio_cap_check_resp_s = elementary_procedure_option<ue_radio_cap_check_resp_ies_container>;

// UERadioCapabilityIDMappingRequest ::= SEQUENCE
using ue_radio_cap_id_map_request_s =
    elementary_procedure_option<protocol_ie_container_l<ue_radio_cap_id_map_request_ies_o>>;

struct ue_radio_cap_id_map_resp_ies_container {
  bool                      crit_diagnostics_present = false;
  unbounded_octstring<true> ue_radio_cap_id;
  unbounded_octstring<true> ue_radio_cap;
  crit_diagnostics_s        crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioCapabilityIDMappingResponse ::= SEQUENCE
using ue_radio_cap_id_map_resp_s = elementary_procedure_option<ue_radio_cap_id_map_resp_ies_container>;

struct ue_radio_cap_info_ind_ies_container {
  bool                      ue_radio_cap_for_paging_present   = false;
  bool                      ue_radio_cap_eutra_format_present = false;
  uint64_t                  amf_ue_ngap_id;
  uint64_t                  ran_ue_ngap_id;
  unbounded_octstring<true> ue_radio_cap;
  ue_radio_cap_for_paging_s ue_radio_cap_for_paging;
  unbounded_octstring<true> ue_radio_cap_eutra_format;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioCapabilityInfoIndication ::= SEQUENCE
using ue_radio_cap_info_ind_s = elementary_procedure_option<ue_radio_cap_info_ind_ies_container>;

struct ue_tnla_binding_release_request_ies_container {
  uint64_t amf_ue_ngap_id;
  uint64_t ran_ue_ngap_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UETNLABindingReleaseRequest ::= SEQUENCE
using ue_tnla_binding_release_request_s = elementary_procedure_option<ue_tnla_binding_release_request_ies_container>;

struct ul_nas_transport_ies_container {
  bool                      w_agf_id_info_present = false;
  bool                      tngf_id_info_present  = false;
  bool                      twif_id_info_present  = false;
  uint64_t                  amf_ue_ngap_id;
  uint64_t                  ran_ue_ngap_id;
  unbounded_octstring<true> nas_pdu;
  user_location_info_c      user_location_info;
  unbounded_octstring<true> w_agf_id_info;
  unbounded_octstring<true> tngf_id_info;
  unbounded_octstring<true> twif_id_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkNASTransport ::= SEQUENCE
using ul_nas_transport_s = elementary_procedure_option<ul_nas_transport_ies_container>;

struct ul_non_ue_associated_nrppa_transport_ies_container {
  unbounded_octstring<true> routing_id;
  unbounded_octstring<true> nrppa_pdu;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkNonUEAssociatedNRPPaTransport ::= SEQUENCE
using ul_non_ue_associated_nrppa_transport_s =
    elementary_procedure_option<ul_non_ue_associated_nrppa_transport_ies_container>;

struct ul_ran_cfg_transfer_ies_container {
  bool                        son_cfg_transfer_ul_present          = false;
  bool                        endc_son_cfg_transfer_ul_present     = false;
  bool                        intersys_son_cfg_transfer_ul_present = false;
  son_cfg_transfer_s          son_cfg_transfer_ul;
  unbounded_octstring<true>   endc_son_cfg_transfer_ul;
  intersys_son_cfg_transfer_s intersys_son_cfg_transfer_ul;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkRANConfigurationTransfer ::= SEQUENCE
using ul_ran_cfg_transfer_s = elementary_procedure_option<ul_ran_cfg_transfer_ies_container>;

struct ul_ran_early_status_transfer_ies_container {
  uint64_t                                      amf_ue_ngap_id;
  uint64_t                                      ran_ue_ngap_id;
  early_status_transfer_transparent_container_s early_status_transfer_transparent_container;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkRANEarlyStatusTransfer ::= SEQUENCE
using ul_ran_early_status_transfer_s = elementary_procedure_option<ul_ran_early_status_transfer_ies_container>;

struct ul_ran_status_transfer_ies_container {
  uint64_t                                    amf_ue_ngap_id;
  uint64_t                                    ran_ue_ngap_id;
  ran_status_transfer_transparent_container_s ran_status_transfer_transparent_container;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkRANStatusTransfer ::= SEQUENCE
using ul_ran_status_transfer_s = elementary_procedure_option<ul_ran_status_transfer_ies_container>;

// UplinkRIMInformationTransfer ::= SEQUENCE
using ul_rim_info_transfer_s = elementary_procedure_option<protocol_ie_container_l<ul_rim_info_transfer_ies_o>>;

struct ul_ue_associated_nrppa_transport_ies_container {
  uint64_t                  amf_ue_ngap_id;
  uint64_t                  ran_ue_ngap_id;
  unbounded_octstring<true> routing_id;
  unbounded_octstring<true> nrppa_pdu;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkUEAssociatedNRPPaTransport ::= SEQUENCE
using ul_ue_associated_nrppa_transport_s = elementary_procedure_option<ul_ue_associated_nrppa_transport_ies_container>;

struct write_replace_warning_request_ies_container {
  bool                             warning_area_list_present          = false;
  bool                             warning_type_present               = false;
  bool                             warning_security_info_present      = false;
  bool                             data_coding_scheme_present         = false;
  bool                             warning_msg_contents_present       = false;
  bool                             concurrent_warning_msg_ind_present = false;
  bool                             warning_area_coordinates_present   = false;
  fixed_bitstring<16, false, true> msg_id;
  fixed_bitstring<16, false, true> serial_num;
  warning_area_list_c              warning_area_list;
  uint32_t                         repeat_period;
  uint32_t                         nof_broadcasts_requested;
  fixed_octstring<2, true>         warning_type;
  fixed_octstring<50, true>        warning_security_info;
  fixed_bitstring<8, false, true>  data_coding_scheme;
  bounded_octstring<1, 9600, true> warning_msg_contents;
  concurrent_warning_msg_ind_e     concurrent_warning_msg_ind;
  bounded_octstring<1, 1024, true> warning_area_coordinates;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WriteReplaceWarningRequest ::= SEQUENCE
using write_replace_warning_request_s = elementary_procedure_option<write_replace_warning_request_ies_container>;

struct write_replace_warning_resp_ies_container {
  bool                             broadcast_completed_area_list_present = false;
  bool                             crit_diagnostics_present              = false;
  fixed_bitstring<16, false, true> msg_id;
  fixed_bitstring<16, false, true> serial_num;
  broadcast_completed_area_list_c  broadcast_completed_area_list;
  crit_diagnostics_s               crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WriteReplaceWarningResponse ::= SEQUENCE
using write_replace_warning_resp_s = elementary_procedure_option<write_replace_warning_resp_ies_container>;

} // namespace ngap
} // namespace asn1

extern template struct asn1::protocol_ie_field_s<asn1::ngap::amf_cp_relocation_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::amf_cfg_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::amf_cfg_upd_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::amf_cfg_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::broadcast_session_mod_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::broadcast_session_mod_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::broadcast_session_mod_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::broadcast_session_release_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::broadcast_session_release_required_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::broadcast_session_release_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::broadcast_session_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::broadcast_session_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::broadcast_session_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::cell_traffic_trace_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::conn_establishment_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::deactiv_trace_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::distribution_release_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::distribution_release_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::distribution_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::distribution_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::distribution_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::dl_nas_transport_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::dl_non_ue_associated_nrppa_transport_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::dl_ran_cfg_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::dl_ran_early_status_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::dl_ran_status_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::dl_ue_associated_nrppa_transport_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::error_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_cancel_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_cancel_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_notify_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_prep_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_request_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_required_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ho_success_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::init_context_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::init_context_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::init_context_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::init_ue_msg_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::location_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::location_report_ctrl_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::location_report_fail_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::multicast_group_paging_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::multicast_session_activation_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::multicast_session_activation_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::multicast_session_activation_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::multicast_session_deactivation_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::multicast_session_deactivation_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::multicast_session_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::multicast_session_upd_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::multicast_session_upd_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::nas_non_delivery_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ng_reset_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ng_reset_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ng_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ng_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ng_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::overload_start_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_modify_confirm_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_modify_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_modify_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_modify_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_notify_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_release_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_release_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pdu_session_res_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pws_cancel_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pws_cancel_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pws_fail_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::pws_restart_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::paging_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::path_switch_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::path_switch_request_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::path_switch_request_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ran_cp_relocation_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ran_cfg_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ran_cfg_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::rrc_inactive_transition_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::reroute_nas_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::secondary_rat_data_usage_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::trace_fail_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::trace_start_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_mod_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_mod_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_mod_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_release_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_release_complete_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_release_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_resume_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_resume_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_resume_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_suspend_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_suspend_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_context_suspend_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_info_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_radio_cap_check_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_radio_cap_check_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_radio_cap_id_map_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_radio_cap_info_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ue_tnla_binding_release_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ul_nas_transport_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ul_non_ue_associated_nrppa_transport_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ul_ran_cfg_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ul_ran_early_status_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ul_ran_status_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::ul_ue_associated_nrppa_transport_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::write_replace_warning_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::ngap::write_replace_warning_resp_ies_o>;
