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
 *                     3GPP TS ASN1 E1AP v16.10.0 (2022-06)
 *
 ******************************************************************************/

#pragma once

#include "e1ap_ies.h"

namespace asn1 {
namespace e1ap {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// BearerContextInactivityNotificationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct bearer_context_inactivity_notif_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_cp_ue_e1ap_id, gnb_cu_up_ue_e1ap_id, activity_info, nulltype } value;

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
    uint64_t&              gnb_cu_cp_ue_e1ap_id();
    uint64_t&              gnb_cu_up_ue_e1ap_id();
    activity_info_c&       activity_info();
    const uint64_t&        gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&        gnb_cu_up_ue_e1ap_id() const;
    const activity_info_c& activity_info() const;

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

struct bearer_context_inactivity_notif_ies_container {
  uint64_t        gnb_cu_cp_ue_e1ap_id;
  uint64_t        gnb_cu_up_ue_e1ap_id;
  activity_info_c activity_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BearerContextInactivityNotification ::= SEQUENCE
using bearer_context_inactivity_notif_s = elementary_procedure_option<bearer_context_inactivity_notif_ies_container>;

// EUTRAN-BearerContextModificationConfirm ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct eutran_bearer_context_mod_confirm_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drb_confirm_modified_list_eutran, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::drb_confirm_modified_list_eutran; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drb_confirm_modified_list_eutran_l&       drb_confirm_modified_list_eutran() { return c; }
    const drb_confirm_modified_list_eutran_l& drb_confirm_modified_list_eutran() const { return c; }

  private:
    drb_confirm_modified_list_eutran_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NG-RAN-BearerContextModificationConfirm ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct ng_ran_bearer_context_mod_confirm_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { pdu_session_res_confirm_modified_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::pdu_session_res_confirm_modified_list; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pdu_session_res_confirm_modified_list_l&       pdu_session_res_confirm_modified_list() { return c; }
    const pdu_session_res_confirm_modified_list_l& pdu_session_res_confirm_modified_list() const { return c; }

  private:
    pdu_session_res_confirm_modified_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// System-BearerContextModificationConfirm-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using sys_bearer_context_mod_confirm_ext_ies_o = protocol_ies_empty_o;

// System-BearerContextModificationConfirm ::= CHOICE
struct sys_bearer_context_mod_confirm_c {
  struct types_opts {
    enum options { e_utran_bearer_context_mod_confirm, ng_ran_bearer_context_mod_confirm, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  sys_bearer_context_mod_confirm_c() = default;
  sys_bearer_context_mod_confirm_c(const sys_bearer_context_mod_confirm_c& other);
  sys_bearer_context_mod_confirm_c& operator=(const sys_bearer_context_mod_confirm_c& other);
  ~sys_bearer_context_mod_confirm_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  protocol_ie_container_l<eutran_bearer_context_mod_confirm_o>& e_utran_bearer_context_mod_confirm()
  {
    assert_choice_type(types::e_utran_bearer_context_mod_confirm, type_, "System-BearerContextModificationConfirm");
    return c.get<protocol_ie_container_l<eutran_bearer_context_mod_confirm_o>>();
  }
  protocol_ie_container_l<ng_ran_bearer_context_mod_confirm_o>& ng_ran_bearer_context_mod_confirm()
  {
    assert_choice_type(types::ng_ran_bearer_context_mod_confirm, type_, "System-BearerContextModificationConfirm");
    return c.get<protocol_ie_container_l<ng_ran_bearer_context_mod_confirm_o>>();
  }
  protocol_ie_single_container_s<sys_bearer_context_mod_confirm_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "System-BearerContextModificationConfirm");
    return c.get<protocol_ie_single_container_s<sys_bearer_context_mod_confirm_ext_ies_o>>();
  }
  const protocol_ie_container_l<eutran_bearer_context_mod_confirm_o>& e_utran_bearer_context_mod_confirm() const
  {
    assert_choice_type(types::e_utran_bearer_context_mod_confirm, type_, "System-BearerContextModificationConfirm");
    return c.get<protocol_ie_container_l<eutran_bearer_context_mod_confirm_o>>();
  }
  const protocol_ie_container_l<ng_ran_bearer_context_mod_confirm_o>& ng_ran_bearer_context_mod_confirm() const
  {
    assert_choice_type(types::ng_ran_bearer_context_mod_confirm, type_, "System-BearerContextModificationConfirm");
    return c.get<protocol_ie_container_l<ng_ran_bearer_context_mod_confirm_o>>();
  }
  const protocol_ie_single_container_s<sys_bearer_context_mod_confirm_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "System-BearerContextModificationConfirm");
    return c.get<protocol_ie_single_container_s<sys_bearer_context_mod_confirm_ext_ies_o>>();
  }
  protocol_ie_container_l<eutran_bearer_context_mod_confirm_o>&             set_e_utran_bearer_context_mod_confirm();
  protocol_ie_container_l<ng_ran_bearer_context_mod_confirm_o>&             set_ng_ran_bearer_context_mod_confirm();
  protocol_ie_single_container_s<sys_bearer_context_mod_confirm_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<protocol_ie_container_l<eutran_bearer_context_mod_confirm_o>,
                  protocol_ie_container_l<ng_ran_bearer_context_mod_confirm_o>,
                  protocol_ie_single_container_s<sys_bearer_context_mod_confirm_ext_ies_o>>
      c;

  void destroy_();
};

// BearerContextModificationConfirmIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct bearer_context_mod_confirm_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_cp_ue_e1ap_id, gnb_cu_up_ue_e1ap_id, sys_bearer_context_mod_confirm, nulltype } value;

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
    uint64_t&                               gnb_cu_cp_ue_e1ap_id();
    uint64_t&                               gnb_cu_up_ue_e1ap_id();
    sys_bearer_context_mod_confirm_c&       sys_bearer_context_mod_confirm();
    const uint64_t&                         gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&                         gnb_cu_up_ue_e1ap_id() const;
    const sys_bearer_context_mod_confirm_c& sys_bearer_context_mod_confirm() const;

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

struct bearer_context_mod_confirm_ies_container {
  bool                             sys_bearer_context_mod_confirm_present = false;
  uint64_t                         gnb_cu_cp_ue_e1ap_id;
  uint64_t                         gnb_cu_up_ue_e1ap_id;
  sys_bearer_context_mod_confirm_c sys_bearer_context_mod_confirm;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BearerContextModificationConfirm ::= SEQUENCE
using bearer_context_mod_confirm_s = elementary_procedure_option<bearer_context_mod_confirm_ies_container>;

// BearerContextModificationFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct bearer_context_mod_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_cp_ue_e1ap_id, gnb_cu_up_ue_e1ap_id, cause, crit_diagnostics, nulltype } value;

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
    uint64_t&                 gnb_cu_cp_ue_e1ap_id();
    uint64_t&                 gnb_cu_up_ue_e1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&           gnb_cu_up_ue_e1ap_id() const;
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

struct bearer_context_mod_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_cp_ue_e1ap_id;
  uint64_t           gnb_cu_up_ue_e1ap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BearerContextModificationFailure ::= SEQUENCE
using bearer_context_mod_fail_s = elementary_procedure_option<bearer_context_mod_fail_ies_container>;

// EUTRAN-BearerContextModificationRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct eutran_bearer_context_mod_request_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        drb_to_setup_mod_list_eutran,
        drb_to_modify_list_eutran,
        drb_to_rem_list_eutran,
        sub_profile_id_for_rfp,
        add_rrm_prio_idx,
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
    drb_to_setup_mod_list_eutran_l&         drb_to_setup_mod_list_eutran();
    drb_to_modify_list_eutran_l&            drb_to_modify_list_eutran();
    drb_to_rem_list_eutran_l&               drb_to_rem_list_eutran();
    uint16_t&                               sub_profile_id_for_rfp();
    fixed_bitstring<32, false, true>&       add_rrm_prio_idx();
    const drb_to_setup_mod_list_eutran_l&   drb_to_setup_mod_list_eutran() const;
    const drb_to_modify_list_eutran_l&      drb_to_modify_list_eutran() const;
    const drb_to_rem_list_eutran_l&         drb_to_rem_list_eutran() const;
    const uint16_t&                         sub_profile_id_for_rfp() const;
    const fixed_bitstring<32, false, true>& add_rrm_prio_idx() const;

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

// NG-RAN-BearerContextModificationRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct ng_ran_bearer_context_mod_request_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        pdu_session_res_to_setup_mod_list,
        pdu_session_res_to_modify_list,
        pdu_session_res_to_rem_list,
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
    pdu_session_res_to_setup_mod_list_l&       pdu_session_res_to_setup_mod_list();
    pdu_session_res_to_modify_list_l&          pdu_session_res_to_modify_list();
    pdu_session_res_to_rem_list_l&             pdu_session_res_to_rem_list();
    const pdu_session_res_to_setup_mod_list_l& pdu_session_res_to_setup_mod_list() const;
    const pdu_session_res_to_modify_list_l&    pdu_session_res_to_modify_list() const;
    const pdu_session_res_to_rem_list_l&       pdu_session_res_to_rem_list() const;

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

// System-BearerContextModificationRequest-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using sys_bearer_context_mod_request_ext_ies_o = protocol_ies_empty_o;

struct eutran_bearer_context_mod_request_container {
  bool                             drb_to_setup_mod_list_eutran_present = false;
  bool                             drb_to_modify_list_eutran_present    = false;
  bool                             drb_to_rem_list_eutran_present       = false;
  bool                             sub_profile_id_for_rfp_present       = false;
  bool                             add_rrm_prio_idx_present             = false;
  drb_to_setup_mod_list_eutran_l   drb_to_setup_mod_list_eutran;
  drb_to_modify_list_eutran_l      drb_to_modify_list_eutran;
  drb_to_rem_list_eutran_l         drb_to_rem_list_eutran;
  uint16_t                         sub_profile_id_for_rfp;
  fixed_bitstring<32, false, true> add_rrm_prio_idx;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ng_ran_bearer_context_mod_request_container {
  bool                                pdu_session_res_to_setup_mod_list_present = false;
  bool                                pdu_session_res_to_modify_list_present    = false;
  bool                                pdu_session_res_to_rem_list_present       = false;
  pdu_session_res_to_setup_mod_list_l pdu_session_res_to_setup_mod_list;
  pdu_session_res_to_modify_list_l    pdu_session_res_to_modify_list;
  pdu_session_res_to_rem_list_l       pdu_session_res_to_rem_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// System-BearerContextModificationRequest ::= CHOICE
struct sys_bearer_context_mod_request_c {
  struct types_opts {
    enum options { e_utran_bearer_context_mod_request, ng_ran_bearer_context_mod_request, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  sys_bearer_context_mod_request_c() = default;
  sys_bearer_context_mod_request_c(const sys_bearer_context_mod_request_c& other);
  sys_bearer_context_mod_request_c& operator=(const sys_bearer_context_mod_request_c& other);
  ~sys_bearer_context_mod_request_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutran_bearer_context_mod_request_container& e_utran_bearer_context_mod_request()
  {
    assert_choice_type(types::e_utran_bearer_context_mod_request, type_, "System-BearerContextModificationRequest");
    return c.get<eutran_bearer_context_mod_request_container>();
  }
  ng_ran_bearer_context_mod_request_container& ng_ran_bearer_context_mod_request()
  {
    assert_choice_type(types::ng_ran_bearer_context_mod_request, type_, "System-BearerContextModificationRequest");
    return c.get<ng_ran_bearer_context_mod_request_container>();
  }
  protocol_ie_single_container_s<sys_bearer_context_mod_request_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "System-BearerContextModificationRequest");
    return c.get<protocol_ie_single_container_s<sys_bearer_context_mod_request_ext_ies_o>>();
  }
  const eutran_bearer_context_mod_request_container& e_utran_bearer_context_mod_request() const
  {
    assert_choice_type(types::e_utran_bearer_context_mod_request, type_, "System-BearerContextModificationRequest");
    return c.get<eutran_bearer_context_mod_request_container>();
  }
  const ng_ran_bearer_context_mod_request_container& ng_ran_bearer_context_mod_request() const
  {
    assert_choice_type(types::ng_ran_bearer_context_mod_request, type_, "System-BearerContextModificationRequest");
    return c.get<ng_ran_bearer_context_mod_request_container>();
  }
  const protocol_ie_single_container_s<sys_bearer_context_mod_request_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "System-BearerContextModificationRequest");
    return c.get<protocol_ie_single_container_s<sys_bearer_context_mod_request_ext_ies_o>>();
  }
  eutran_bearer_context_mod_request_container&                              set_e_utran_bearer_context_mod_request();
  ng_ran_bearer_context_mod_request_container&                              set_ng_ran_bearer_context_mod_request();
  protocol_ie_single_container_s<sys_bearer_context_mod_request_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<eutran_bearer_context_mod_request_container,
                  ng_ran_bearer_context_mod_request_container,
                  protocol_ie_single_container_s<sys_bearer_context_mod_request_ext_ies_o>>
      c;

  void destroy_();
};

// BearerContextModificationRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct bearer_context_mod_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_cp_ue_e1ap_id,
        gnb_cu_up_ue_e1ap_id,
        security_info,
        ue_dl_aggr_max_bit_rate,
        ue_dl_max_integrity_protected_data_rate,
        bearer_context_status_change,
        new_ul_tnl_info_required,
        ue_inactivity_timer,
        data_discard_required,
        sys_bearer_context_mod_request,
        ran_ue_id,
        gnb_du_id,
        activity_notif_level,
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
    uint64_t&                               gnb_cu_cp_ue_e1ap_id();
    uint64_t&                               gnb_cu_up_ue_e1ap_id();
    security_info_s&                        security_info();
    uint64_t&                               ue_dl_aggr_max_bit_rate();
    uint64_t&                               ue_dl_max_integrity_protected_data_rate();
    bearer_context_status_change_e&         bearer_context_status_change();
    new_ul_tnl_info_required_e&             new_ul_tnl_info_required();
    uint16_t&                               ue_inactivity_timer();
    data_discard_required_e&                data_discard_required();
    sys_bearer_context_mod_request_c&       sys_bearer_context_mod_request();
    fixed_octstring<8, true>&               ran_ue_id();
    uint64_t&                               gnb_du_id();
    activity_notif_level_e&                 activity_notif_level();
    const uint64_t&                         gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&                         gnb_cu_up_ue_e1ap_id() const;
    const security_info_s&                  security_info() const;
    const uint64_t&                         ue_dl_aggr_max_bit_rate() const;
    const uint64_t&                         ue_dl_max_integrity_protected_data_rate() const;
    const bearer_context_status_change_e&   bearer_context_status_change() const;
    const new_ul_tnl_info_required_e&       new_ul_tnl_info_required() const;
    const uint16_t&                         ue_inactivity_timer() const;
    const data_discard_required_e&          data_discard_required() const;
    const sys_bearer_context_mod_request_c& sys_bearer_context_mod_request() const;
    const fixed_octstring<8, true>&         ran_ue_id() const;
    const uint64_t&                         gnb_du_id() const;
    const activity_notif_level_e&           activity_notif_level() const;

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

struct bearer_context_mod_request_ies_container {
  bool                             security_info_present                           = false;
  bool                             ue_dl_aggr_max_bit_rate_present                 = false;
  bool                             ue_dl_max_integrity_protected_data_rate_present = false;
  bool                             bearer_context_status_change_present            = false;
  bool                             new_ul_tnl_info_required_present                = false;
  bool                             ue_inactivity_timer_present                     = false;
  bool                             data_discard_required_present                   = false;
  bool                             sys_bearer_context_mod_request_present          = false;
  bool                             ran_ue_id_present                               = false;
  bool                             gnb_du_id_present                               = false;
  bool                             activity_notif_level_present                    = false;
  uint64_t                         gnb_cu_cp_ue_e1ap_id;
  uint64_t                         gnb_cu_up_ue_e1ap_id;
  security_info_s                  security_info;
  uint64_t                         ue_dl_aggr_max_bit_rate;
  uint64_t                         ue_dl_max_integrity_protected_data_rate;
  bearer_context_status_change_e   bearer_context_status_change;
  new_ul_tnl_info_required_e       new_ul_tnl_info_required;
  uint16_t                         ue_inactivity_timer;
  data_discard_required_e          data_discard_required;
  sys_bearer_context_mod_request_c sys_bearer_context_mod_request;
  fixed_octstring<8, true>         ran_ue_id;
  uint64_t                         gnb_du_id;
  activity_notif_level_e           activity_notif_level;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BearerContextModificationRequest ::= SEQUENCE
using bearer_context_mod_request_s = elementary_procedure_option<bearer_context_mod_request_ies_container>;

// EUTRAN-BearerContextModificationRequired ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct eutran_bearer_context_mod_required_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drb_required_to_modify_list_eutran, drb_required_to_rem_list_eutran, nulltype } value;

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
    drb_required_to_modify_list_eutran_l&       drb_required_to_modify_list_eutran();
    drb_required_to_rem_list_eutran_l&          drb_required_to_rem_list_eutran();
    const drb_required_to_modify_list_eutran_l& drb_required_to_modify_list_eutran() const;
    const drb_required_to_rem_list_eutran_l&    drb_required_to_rem_list_eutran() const;

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

// NG-RAN-BearerContextModificationRequired ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct ng_ran_bearer_context_mod_required_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { pdu_session_res_required_to_modify_list, pdu_session_res_to_rem_list, nulltype } value;

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
    pdu_session_res_required_to_modify_list_l&       pdu_session_res_required_to_modify_list();
    pdu_session_res_to_rem_list_l&                   pdu_session_res_to_rem_list();
    const pdu_session_res_required_to_modify_list_l& pdu_session_res_required_to_modify_list() const;
    const pdu_session_res_to_rem_list_l&             pdu_session_res_to_rem_list() const;

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

// System-BearerContextModificationRequired-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using sys_bearer_context_mod_required_ext_ies_o = protocol_ies_empty_o;

struct eutran_bearer_context_mod_required_container {
  bool                                 drb_required_to_modify_list_eutran_present = false;
  bool                                 drb_required_to_rem_list_eutran_present    = false;
  drb_required_to_modify_list_eutran_l drb_required_to_modify_list_eutran;
  drb_required_to_rem_list_eutran_l    drb_required_to_rem_list_eutran;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ng_ran_bearer_context_mod_required_container {
  bool                                      pdu_session_res_required_to_modify_list_present = false;
  bool                                      pdu_session_res_to_rem_list_present             = false;
  pdu_session_res_required_to_modify_list_l pdu_session_res_required_to_modify_list;
  pdu_session_res_to_rem_list_l             pdu_session_res_to_rem_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// System-BearerContextModificationRequired ::= CHOICE
struct sys_bearer_context_mod_required_c {
  struct types_opts {
    enum options {
      e_utran_bearer_context_mod_required,
      ng_ran_bearer_context_mod_required,
      choice_ext,
      nulltype
    } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  sys_bearer_context_mod_required_c() = default;
  sys_bearer_context_mod_required_c(const sys_bearer_context_mod_required_c& other);
  sys_bearer_context_mod_required_c& operator=(const sys_bearer_context_mod_required_c& other);
  ~sys_bearer_context_mod_required_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutran_bearer_context_mod_required_container& e_utran_bearer_context_mod_required()
  {
    assert_choice_type(types::e_utran_bearer_context_mod_required, type_, "System-BearerContextModificationRequired");
    return c.get<eutran_bearer_context_mod_required_container>();
  }
  ng_ran_bearer_context_mod_required_container& ng_ran_bearer_context_mod_required()
  {
    assert_choice_type(types::ng_ran_bearer_context_mod_required, type_, "System-BearerContextModificationRequired");
    return c.get<ng_ran_bearer_context_mod_required_container>();
  }
  protocol_ie_single_container_s<sys_bearer_context_mod_required_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "System-BearerContextModificationRequired");
    return c.get<protocol_ie_single_container_s<sys_bearer_context_mod_required_ext_ies_o>>();
  }
  const eutran_bearer_context_mod_required_container& e_utran_bearer_context_mod_required() const
  {
    assert_choice_type(types::e_utran_bearer_context_mod_required, type_, "System-BearerContextModificationRequired");
    return c.get<eutran_bearer_context_mod_required_container>();
  }
  const ng_ran_bearer_context_mod_required_container& ng_ran_bearer_context_mod_required() const
  {
    assert_choice_type(types::ng_ran_bearer_context_mod_required, type_, "System-BearerContextModificationRequired");
    return c.get<ng_ran_bearer_context_mod_required_container>();
  }
  const protocol_ie_single_container_s<sys_bearer_context_mod_required_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "System-BearerContextModificationRequired");
    return c.get<protocol_ie_single_container_s<sys_bearer_context_mod_required_ext_ies_o>>();
  }
  eutran_bearer_context_mod_required_container&                              set_e_utran_bearer_context_mod_required();
  ng_ran_bearer_context_mod_required_container&                              set_ng_ran_bearer_context_mod_required();
  protocol_ie_single_container_s<sys_bearer_context_mod_required_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<eutran_bearer_context_mod_required_container,
                  ng_ran_bearer_context_mod_required_container,
                  protocol_ie_single_container_s<sys_bearer_context_mod_required_ext_ies_o>>
      c;

  void destroy_();
};

// BearerContextModificationRequiredIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct bearer_context_mod_required_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_cp_ue_e1ap_id, gnb_cu_up_ue_e1ap_id, sys_bearer_context_mod_required, nulltype } value;

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
    uint64_t&                                gnb_cu_cp_ue_e1ap_id();
    uint64_t&                                gnb_cu_up_ue_e1ap_id();
    sys_bearer_context_mod_required_c&       sys_bearer_context_mod_required();
    const uint64_t&                          gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&                          gnb_cu_up_ue_e1ap_id() const;
    const sys_bearer_context_mod_required_c& sys_bearer_context_mod_required() const;

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

struct bearer_context_mod_required_ies_container {
  uint64_t                          gnb_cu_cp_ue_e1ap_id;
  uint64_t                          gnb_cu_up_ue_e1ap_id;
  sys_bearer_context_mod_required_c sys_bearer_context_mod_required;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BearerContextModificationRequired ::= SEQUENCE
using bearer_context_mod_required_s = elementary_procedure_option<bearer_context_mod_required_ies_container>;

// EUTRAN-BearerContextModificationResponse ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct eutran_bearer_context_mod_resp_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        drb_setup_mod_list_eutran,
        drb_failed_mod_list_eutran,
        drb_modified_list_eutran,
        drb_failed_to_modify_list_eutran,
        retainability_meass_info,
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
    drb_setup_mod_list_eutran_l&              drb_setup_mod_list_eutran();
    drb_failed_mod_list_eutran_l&             drb_failed_mod_list_eutran();
    drb_modified_list_eutran_l&               drb_modified_list_eutran();
    drb_failed_to_modify_list_eutran_l&       drb_failed_to_modify_list_eutran();
    retainability_meass_info_l&               retainability_meass_info();
    const drb_setup_mod_list_eutran_l&        drb_setup_mod_list_eutran() const;
    const drb_failed_mod_list_eutran_l&       drb_failed_mod_list_eutran() const;
    const drb_modified_list_eutran_l&         drb_modified_list_eutran() const;
    const drb_failed_to_modify_list_eutran_l& drb_failed_to_modify_list_eutran() const;
    const retainability_meass_info_l&         retainability_meass_info() const;

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

// NG-RAN-BearerContextModificationResponse ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct ng_ran_bearer_context_mod_resp_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        pdu_session_res_setup_mod_list,
        pdu_session_res_failed_mod_list,
        pdu_session_res_modified_list,
        pdu_session_res_failed_to_modify_list,
        retainability_meass_info,
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
    pdu_session_res_setup_mod_list_l&              pdu_session_res_setup_mod_list();
    pdu_session_res_failed_mod_list_l&             pdu_session_res_failed_mod_list();
    pdu_session_res_modified_list_l&               pdu_session_res_modified_list();
    pdu_session_res_failed_to_modify_list_l&       pdu_session_res_failed_to_modify_list();
    retainability_meass_info_l&                    retainability_meass_info();
    const pdu_session_res_setup_mod_list_l&        pdu_session_res_setup_mod_list() const;
    const pdu_session_res_failed_mod_list_l&       pdu_session_res_failed_mod_list() const;
    const pdu_session_res_modified_list_l&         pdu_session_res_modified_list() const;
    const pdu_session_res_failed_to_modify_list_l& pdu_session_res_failed_to_modify_list() const;
    const retainability_meass_info_l&              retainability_meass_info() const;

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

// System-BearerContextModificationResponse-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using sys_bearer_context_mod_resp_ext_ies_o = protocol_ies_empty_o;

struct eutran_bearer_context_mod_resp_container {
  bool                               drb_setup_mod_list_eutran_present        = false;
  bool                               drb_failed_mod_list_eutran_present       = false;
  bool                               drb_modified_list_eutran_present         = false;
  bool                               drb_failed_to_modify_list_eutran_present = false;
  bool                               retainability_meass_info_present         = false;
  drb_setup_mod_list_eutran_l        drb_setup_mod_list_eutran;
  drb_failed_mod_list_eutran_l       drb_failed_mod_list_eutran;
  drb_modified_list_eutran_l         drb_modified_list_eutran;
  drb_failed_to_modify_list_eutran_l drb_failed_to_modify_list_eutran;
  retainability_meass_info_l         retainability_meass_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ng_ran_bearer_context_mod_resp_container {
  bool                                    pdu_session_res_setup_mod_list_present        = false;
  bool                                    pdu_session_res_failed_mod_list_present       = false;
  bool                                    pdu_session_res_modified_list_present         = false;
  bool                                    pdu_session_res_failed_to_modify_list_present = false;
  bool                                    retainability_meass_info_present              = false;
  pdu_session_res_setup_mod_list_l        pdu_session_res_setup_mod_list;
  pdu_session_res_failed_mod_list_l       pdu_session_res_failed_mod_list;
  pdu_session_res_modified_list_l         pdu_session_res_modified_list;
  pdu_session_res_failed_to_modify_list_l pdu_session_res_failed_to_modify_list;
  retainability_meass_info_l              retainability_meass_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// System-BearerContextModificationResponse ::= CHOICE
struct sys_bearer_context_mod_resp_c {
  struct types_opts {
    enum options { e_utran_bearer_context_mod_resp, ng_ran_bearer_context_mod_resp, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  sys_bearer_context_mod_resp_c() = default;
  sys_bearer_context_mod_resp_c(const sys_bearer_context_mod_resp_c& other);
  sys_bearer_context_mod_resp_c& operator=(const sys_bearer_context_mod_resp_c& other);
  ~sys_bearer_context_mod_resp_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutran_bearer_context_mod_resp_container& e_utran_bearer_context_mod_resp()
  {
    assert_choice_type(types::e_utran_bearer_context_mod_resp, type_, "System-BearerContextModificationResponse");
    return c.get<eutran_bearer_context_mod_resp_container>();
  }
  ng_ran_bearer_context_mod_resp_container& ng_ran_bearer_context_mod_resp()
  {
    assert_choice_type(types::ng_ran_bearer_context_mod_resp, type_, "System-BearerContextModificationResponse");
    return c.get<ng_ran_bearer_context_mod_resp_container>();
  }
  protocol_ie_single_container_s<sys_bearer_context_mod_resp_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "System-BearerContextModificationResponse");
    return c.get<protocol_ie_single_container_s<sys_bearer_context_mod_resp_ext_ies_o>>();
  }
  const eutran_bearer_context_mod_resp_container& e_utran_bearer_context_mod_resp() const
  {
    assert_choice_type(types::e_utran_bearer_context_mod_resp, type_, "System-BearerContextModificationResponse");
    return c.get<eutran_bearer_context_mod_resp_container>();
  }
  const ng_ran_bearer_context_mod_resp_container& ng_ran_bearer_context_mod_resp() const
  {
    assert_choice_type(types::ng_ran_bearer_context_mod_resp, type_, "System-BearerContextModificationResponse");
    return c.get<ng_ran_bearer_context_mod_resp_container>();
  }
  const protocol_ie_single_container_s<sys_bearer_context_mod_resp_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "System-BearerContextModificationResponse");
    return c.get<protocol_ie_single_container_s<sys_bearer_context_mod_resp_ext_ies_o>>();
  }
  eutran_bearer_context_mod_resp_container&                              set_e_utran_bearer_context_mod_resp();
  ng_ran_bearer_context_mod_resp_container&                              set_ng_ran_bearer_context_mod_resp();
  protocol_ie_single_container_s<sys_bearer_context_mod_resp_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<eutran_bearer_context_mod_resp_container,
                  ng_ran_bearer_context_mod_resp_container,
                  protocol_ie_single_container_s<sys_bearer_context_mod_resp_ext_ies_o>>
      c;

  void destroy_();
};

// BearerContextModificationResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct bearer_context_mod_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_cp_ue_e1ap_id, gnb_cu_up_ue_e1ap_id, sys_bearer_context_mod_resp, nulltype } value;

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
    uint64_t&                            gnb_cu_cp_ue_e1ap_id();
    uint64_t&                            gnb_cu_up_ue_e1ap_id();
    sys_bearer_context_mod_resp_c&       sys_bearer_context_mod_resp();
    const uint64_t&                      gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&                      gnb_cu_up_ue_e1ap_id() const;
    const sys_bearer_context_mod_resp_c& sys_bearer_context_mod_resp() const;

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

struct bearer_context_mod_resp_ies_container {
  bool                          sys_bearer_context_mod_resp_present = false;
  uint64_t                      gnb_cu_cp_ue_e1ap_id;
  uint64_t                      gnb_cu_up_ue_e1ap_id;
  sys_bearer_context_mod_resp_c sys_bearer_context_mod_resp;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BearerContextModificationResponse ::= SEQUENCE
using bearer_context_mod_resp_s = elementary_procedure_option<bearer_context_mod_resp_ies_container>;

// BearerContextReleaseCommandIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct bearer_context_release_cmd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_cp_ue_e1ap_id, gnb_cu_up_ue_e1ap_id, cause, nulltype } value;

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
    uint64_t&       gnb_cu_cp_ue_e1ap_id();
    uint64_t&       gnb_cu_up_ue_e1ap_id();
    cause_c&        cause();
    const uint64_t& gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t& gnb_cu_up_ue_e1ap_id() const;
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

struct bearer_context_release_cmd_ies_container {
  uint64_t gnb_cu_cp_ue_e1ap_id;
  uint64_t gnb_cu_up_ue_e1ap_id;
  cause_c  cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BearerContextReleaseCommand ::= SEQUENCE
using bearer_context_release_cmd_s = elementary_procedure_option<bearer_context_release_cmd_ies_container>;

// BearerContextReleaseCompleteIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct bearer_context_release_complete_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_cp_ue_e1ap_id,
        gnb_cu_up_ue_e1ap_id,
        crit_diagnostics,
        retainability_meass_info,
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
    uint64_t&                         gnb_cu_cp_ue_e1ap_id();
    uint64_t&                         gnb_cu_up_ue_e1ap_id();
    crit_diagnostics_s&               crit_diagnostics();
    retainability_meass_info_l&       retainability_meass_info();
    const uint64_t&                   gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&                   gnb_cu_up_ue_e1ap_id() const;
    const crit_diagnostics_s&         crit_diagnostics() const;
    const retainability_meass_info_l& retainability_meass_info() const;

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

struct bearer_context_release_complete_ies_container {
  bool                       crit_diagnostics_present         = false;
  bool                       retainability_meass_info_present = false;
  uint64_t                   gnb_cu_cp_ue_e1ap_id;
  uint64_t                   gnb_cu_up_ue_e1ap_id;
  crit_diagnostics_s         crit_diagnostics;
  retainability_meass_info_l retainability_meass_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BearerContextReleaseComplete ::= SEQUENCE
using bearer_context_release_complete_s = elementary_procedure_option<bearer_context_release_complete_ies_container>;

// DRB-Status-List ::= SEQUENCE (SIZE (1..32)) OF DRB-Status-Item
using drb_status_list_l = dyn_array<drb_status_item_s>;

// BearerContextReleaseRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct bearer_context_release_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_cp_ue_e1ap_id, gnb_cu_up_ue_e1ap_id, drb_status_list, cause, nulltype } value;

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
    uint64_t&                gnb_cu_cp_ue_e1ap_id();
    uint64_t&                gnb_cu_up_ue_e1ap_id();
    drb_status_list_l&       drb_status_list();
    cause_c&                 cause();
    const uint64_t&          gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&          gnb_cu_up_ue_e1ap_id() const;
    const drb_status_list_l& drb_status_list() const;
    const cause_c&           cause() const;

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

struct bearer_context_release_request_ies_container {
  bool              drb_status_list_present = false;
  uint64_t          gnb_cu_cp_ue_e1ap_id;
  uint64_t          gnb_cu_up_ue_e1ap_id;
  drb_status_list_l drb_status_list;
  cause_c           cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BearerContextReleaseRequest ::= SEQUENCE
using bearer_context_release_request_s = elementary_procedure_option<bearer_context_release_request_ies_container>;

// BearerContextSetupFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct bearer_context_setup_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_cp_ue_e1ap_id, gnb_cu_up_ue_e1ap_id, cause, crit_diagnostics, nulltype } value;

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
    uint64_t&                 gnb_cu_cp_ue_e1ap_id();
    uint64_t&                 gnb_cu_up_ue_e1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&           gnb_cu_up_ue_e1ap_id() const;
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

struct bearer_context_setup_fail_ies_container {
  bool               gnb_cu_up_ue_e1ap_id_present = false;
  bool               crit_diagnostics_present     = false;
  uint64_t           gnb_cu_cp_ue_e1ap_id;
  uint64_t           gnb_cu_up_ue_e1ap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BearerContextSetupFailure ::= SEQUENCE
using bearer_context_setup_fail_s = elementary_procedure_option<bearer_context_setup_fail_ies_container>;

// EUTRAN-BearerContextSetupRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct eutran_bearer_context_setup_request_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drb_to_setup_list_eutran, sub_profile_id_for_rfp, add_rrm_prio_idx, nulltype } value;
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
    drb_to_setup_list_eutran_l&             drb_to_setup_list_eutran();
    uint16_t&                               sub_profile_id_for_rfp();
    fixed_bitstring<32, false, true>&       add_rrm_prio_idx();
    const drb_to_setup_list_eutran_l&       drb_to_setup_list_eutran() const;
    const uint16_t&                         sub_profile_id_for_rfp() const;
    const fixed_bitstring<32, false, true>& add_rrm_prio_idx() const;

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

// NG-RAN-BearerContextSetupRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct ng_ran_bearer_context_setup_request_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { pdu_session_res_to_setup_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::pdu_session_res_to_setup_list; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pdu_session_res_to_setup_list_l&       pdu_session_res_to_setup_list() { return c; }
    const pdu_session_res_to_setup_list_l& pdu_session_res_to_setup_list() const { return c; }

  private:
    pdu_session_res_to_setup_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// System-BearerContextSetupRequest-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using sys_bearer_context_setup_request_ext_ies_o = protocol_ies_empty_o;

struct eutran_bearer_context_setup_request_container {
  bool                             sub_profile_id_for_rfp_present = false;
  bool                             add_rrm_prio_idx_present       = false;
  drb_to_setup_list_eutran_l       drb_to_setup_list_eutran;
  uint16_t                         sub_profile_id_for_rfp;
  fixed_bitstring<32, false, true> add_rrm_prio_idx;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// System-BearerContextSetupRequest ::= CHOICE
struct sys_bearer_context_setup_request_c {
  struct types_opts {
    enum options {
      e_utran_bearer_context_setup_request,
      ng_ran_bearer_context_setup_request,
      choice_ext,
      nulltype
    } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  sys_bearer_context_setup_request_c() = default;
  sys_bearer_context_setup_request_c(const sys_bearer_context_setup_request_c& other);
  sys_bearer_context_setup_request_c& operator=(const sys_bearer_context_setup_request_c& other);
  ~sys_bearer_context_setup_request_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutran_bearer_context_setup_request_container& e_utran_bearer_context_setup_request()
  {
    assert_choice_type(types::e_utran_bearer_context_setup_request, type_, "System-BearerContextSetupRequest");
    return c.get<eutran_bearer_context_setup_request_container>();
  }
  protocol_ie_container_l<ng_ran_bearer_context_setup_request_o>& ng_ran_bearer_context_setup_request()
  {
    assert_choice_type(types::ng_ran_bearer_context_setup_request, type_, "System-BearerContextSetupRequest");
    return c.get<protocol_ie_container_l<ng_ran_bearer_context_setup_request_o>>();
  }
  protocol_ie_single_container_s<sys_bearer_context_setup_request_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "System-BearerContextSetupRequest");
    return c.get<protocol_ie_single_container_s<sys_bearer_context_setup_request_ext_ies_o>>();
  }
  const eutran_bearer_context_setup_request_container& e_utran_bearer_context_setup_request() const
  {
    assert_choice_type(types::e_utran_bearer_context_setup_request, type_, "System-BearerContextSetupRequest");
    return c.get<eutran_bearer_context_setup_request_container>();
  }
  const protocol_ie_container_l<ng_ran_bearer_context_setup_request_o>& ng_ran_bearer_context_setup_request() const
  {
    assert_choice_type(types::ng_ran_bearer_context_setup_request, type_, "System-BearerContextSetupRequest");
    return c.get<protocol_ie_container_l<ng_ran_bearer_context_setup_request_o>>();
  }
  const protocol_ie_single_container_s<sys_bearer_context_setup_request_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "System-BearerContextSetupRequest");
    return c.get<protocol_ie_single_container_s<sys_bearer_context_setup_request_ext_ies_o>>();
  }
  eutran_bearer_context_setup_request_container&                  set_e_utran_bearer_context_setup_request();
  protocol_ie_container_l<ng_ran_bearer_context_setup_request_o>& set_ng_ran_bearer_context_setup_request();
  protocol_ie_single_container_s<sys_bearer_context_setup_request_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<eutran_bearer_context_setup_request_container,
                  protocol_ie_container_l<ng_ran_bearer_context_setup_request_o>,
                  protocol_ie_single_container_s<sys_bearer_context_setup_request_ext_ies_o>>
      c;

  void destroy_();
};

// BearerContextSetupRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct bearer_context_setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_cp_ue_e1ap_id,
        security_info,
        ue_dl_aggr_max_bit_rate,
        ue_dl_max_integrity_protected_data_rate,
        serving_plmn,
        activity_notif_level,
        ue_inactivity_timer,
        bearer_context_status_change,
        sys_bearer_context_setup_request,
        ran_ue_id,
        gnb_du_id,
        trace_activation,
        npn_context_info,
        management_based_mdt_plmn_list,
        cho_initiation,
        add_ho_info,
        direct_forwarding_path_availability,
        gnb_cu_up_ue_e1ap_id,
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
    uint64_t&                                    gnb_cu_cp_ue_e1ap_id();
    security_info_s&                             security_info();
    uint64_t&                                    ue_dl_aggr_max_bit_rate();
    uint64_t&                                    ue_dl_max_integrity_protected_data_rate();
    fixed_octstring<3, true>&                    serving_plmn();
    activity_notif_level_e&                      activity_notif_level();
    uint16_t&                                    ue_inactivity_timer();
    bearer_context_status_change_e&              bearer_context_status_change();
    sys_bearer_context_setup_request_c&          sys_bearer_context_setup_request();
    fixed_octstring<8, true>&                    ran_ue_id();
    uint64_t&                                    gnb_du_id();
    trace_activation_s&                          trace_activation();
    npn_context_info_c&                          npn_context_info();
    mdt_plmn_list_l&                             management_based_mdt_plmn_list();
    cho_initiation_e&                            cho_initiation();
    add_ho_info_e&                               add_ho_info();
    direct_forwarding_path_availability_e&       direct_forwarding_path_availability();
    uint64_t&                                    gnb_cu_up_ue_e1ap_id();
    const uint64_t&                              gnb_cu_cp_ue_e1ap_id() const;
    const security_info_s&                       security_info() const;
    const uint64_t&                              ue_dl_aggr_max_bit_rate() const;
    const uint64_t&                              ue_dl_max_integrity_protected_data_rate() const;
    const fixed_octstring<3, true>&              serving_plmn() const;
    const activity_notif_level_e&                activity_notif_level() const;
    const uint16_t&                              ue_inactivity_timer() const;
    const bearer_context_status_change_e&        bearer_context_status_change() const;
    const sys_bearer_context_setup_request_c&    sys_bearer_context_setup_request() const;
    const fixed_octstring<8, true>&              ran_ue_id() const;
    const uint64_t&                              gnb_du_id() const;
    const trace_activation_s&                    trace_activation() const;
    const npn_context_info_c&                    npn_context_info() const;
    const mdt_plmn_list_l&                       management_based_mdt_plmn_list() const;
    const cho_initiation_e&                      cho_initiation() const;
    const add_ho_info_e&                         add_ho_info() const;
    const direct_forwarding_path_availability_e& direct_forwarding_path_availability() const;
    const uint64_t&                              gnb_cu_up_ue_e1ap_id() const;

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

struct bearer_context_setup_request_ies_container {
  bool                                  ue_dl_max_integrity_protected_data_rate_present = false;
  bool                                  ue_inactivity_timer_present                     = false;
  bool                                  bearer_context_status_change_present            = false;
  bool                                  ran_ue_id_present                               = false;
  bool                                  gnb_du_id_present                               = false;
  bool                                  trace_activation_present                        = false;
  bool                                  npn_context_info_present                        = false;
  bool                                  management_based_mdt_plmn_list_present          = false;
  bool                                  cho_initiation_present                          = false;
  bool                                  add_ho_info_present                             = false;
  bool                                  direct_forwarding_path_availability_present     = false;
  bool                                  gnb_cu_up_ue_e1ap_id_present                    = false;
  uint64_t                              gnb_cu_cp_ue_e1ap_id;
  security_info_s                       security_info;
  uint64_t                              ue_dl_aggr_max_bit_rate;
  uint64_t                              ue_dl_max_integrity_protected_data_rate;
  fixed_octstring<3, true>              serving_plmn;
  activity_notif_level_e                activity_notif_level;
  uint16_t                              ue_inactivity_timer;
  bearer_context_status_change_e        bearer_context_status_change;
  sys_bearer_context_setup_request_c    sys_bearer_context_setup_request;
  fixed_octstring<8, true>              ran_ue_id;
  uint64_t                              gnb_du_id;
  trace_activation_s                    trace_activation;
  npn_context_info_c                    npn_context_info;
  mdt_plmn_list_l                       management_based_mdt_plmn_list;
  cho_initiation_e                      cho_initiation;
  add_ho_info_e                         add_ho_info;
  direct_forwarding_path_availability_e direct_forwarding_path_availability;
  uint64_t                              gnb_cu_up_ue_e1ap_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BearerContextSetupRequest ::= SEQUENCE
using bearer_context_setup_request_s = elementary_procedure_option<bearer_context_setup_request_ies_container>;

// EUTRAN-BearerContextSetupResponse ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct eutran_bearer_context_setup_resp_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drb_setup_list_eutran, drb_failed_list_eutran, nulltype } value;

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
    drb_setup_list_eutran_l&        drb_setup_list_eutran();
    drb_failed_list_eutran_l&       drb_failed_list_eutran();
    const drb_setup_list_eutran_l&  drb_setup_list_eutran() const;
    const drb_failed_list_eutran_l& drb_failed_list_eutran() const;

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

// NG-RAN-BearerContextSetupResponse ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct ng_ran_bearer_context_setup_resp_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { pdu_session_res_setup_list, pdu_session_res_failed_list, nulltype } value;

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
    pdu_session_res_setup_list_l&        pdu_session_res_setup_list();
    pdu_session_res_failed_list_l&       pdu_session_res_failed_list();
    const pdu_session_res_setup_list_l&  pdu_session_res_setup_list() const;
    const pdu_session_res_failed_list_l& pdu_session_res_failed_list() const;

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

// System-BearerContextSetupResponse-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using sys_bearer_context_setup_resp_ext_ies_o = protocol_ies_empty_o;

struct eutran_bearer_context_setup_resp_container {
  bool                     drb_failed_list_eutran_present = false;
  drb_setup_list_eutran_l  drb_setup_list_eutran;
  drb_failed_list_eutran_l drb_failed_list_eutran;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ng_ran_bearer_context_setup_resp_container {
  bool                          pdu_session_res_failed_list_present = false;
  pdu_session_res_setup_list_l  pdu_session_res_setup_list;
  pdu_session_res_failed_list_l pdu_session_res_failed_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// System-BearerContextSetupResponse ::= CHOICE
struct sys_bearer_context_setup_resp_c {
  struct types_opts {
    enum options { e_utran_bearer_context_setup_resp, ng_ran_bearer_context_setup_resp, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  sys_bearer_context_setup_resp_c() = default;
  sys_bearer_context_setup_resp_c(const sys_bearer_context_setup_resp_c& other);
  sys_bearer_context_setup_resp_c& operator=(const sys_bearer_context_setup_resp_c& other);
  ~sys_bearer_context_setup_resp_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutran_bearer_context_setup_resp_container& e_utran_bearer_context_setup_resp()
  {
    assert_choice_type(types::e_utran_bearer_context_setup_resp, type_, "System-BearerContextSetupResponse");
    return c.get<eutran_bearer_context_setup_resp_container>();
  }
  ng_ran_bearer_context_setup_resp_container& ng_ran_bearer_context_setup_resp()
  {
    assert_choice_type(types::ng_ran_bearer_context_setup_resp, type_, "System-BearerContextSetupResponse");
    return c.get<ng_ran_bearer_context_setup_resp_container>();
  }
  protocol_ie_single_container_s<sys_bearer_context_setup_resp_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "System-BearerContextSetupResponse");
    return c.get<protocol_ie_single_container_s<sys_bearer_context_setup_resp_ext_ies_o>>();
  }
  const eutran_bearer_context_setup_resp_container& e_utran_bearer_context_setup_resp() const
  {
    assert_choice_type(types::e_utran_bearer_context_setup_resp, type_, "System-BearerContextSetupResponse");
    return c.get<eutran_bearer_context_setup_resp_container>();
  }
  const ng_ran_bearer_context_setup_resp_container& ng_ran_bearer_context_setup_resp() const
  {
    assert_choice_type(types::ng_ran_bearer_context_setup_resp, type_, "System-BearerContextSetupResponse");
    return c.get<ng_ran_bearer_context_setup_resp_container>();
  }
  const protocol_ie_single_container_s<sys_bearer_context_setup_resp_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "System-BearerContextSetupResponse");
    return c.get<protocol_ie_single_container_s<sys_bearer_context_setup_resp_ext_ies_o>>();
  }
  eutran_bearer_context_setup_resp_container&                              set_e_utran_bearer_context_setup_resp();
  ng_ran_bearer_context_setup_resp_container&                              set_ng_ran_bearer_context_setup_resp();
  protocol_ie_single_container_s<sys_bearer_context_setup_resp_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<eutran_bearer_context_setup_resp_container,
                  ng_ran_bearer_context_setup_resp_container,
                  protocol_ie_single_container_s<sys_bearer_context_setup_resp_ext_ies_o>>
      c;

  void destroy_();
};

// BearerContextSetupResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct bearer_context_setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_cp_ue_e1ap_id, gnb_cu_up_ue_e1ap_id, sys_bearer_context_setup_resp, nulltype } value;

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
    uint64_t&                              gnb_cu_cp_ue_e1ap_id();
    uint64_t&                              gnb_cu_up_ue_e1ap_id();
    sys_bearer_context_setup_resp_c&       sys_bearer_context_setup_resp();
    const uint64_t&                        gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&                        gnb_cu_up_ue_e1ap_id() const;
    const sys_bearer_context_setup_resp_c& sys_bearer_context_setup_resp() const;

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

struct bearer_context_setup_resp_ies_container {
  uint64_t                        gnb_cu_cp_ue_e1ap_id;
  uint64_t                        gnb_cu_up_ue_e1ap_id;
  sys_bearer_context_setup_resp_c sys_bearer_context_setup_resp;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BearerContextSetupResponse ::= SEQUENCE
using bearer_context_setup_resp_s = elementary_procedure_option<bearer_context_setup_resp_ies_container>;

// CellTrafficTraceIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct cell_traffic_trace_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_cp_ue_e1ap_id,
        gnb_cu_up_ue_e1ap_id,
        trace_id,
        trace_collection_entity_ip_address,
        privacy_ind,
        ur_iaddress,
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
    uint64_t&                                                    gnb_cu_cp_ue_e1ap_id();
    uint64_t&                                                    gnb_cu_up_ue_e1ap_id();
    fixed_octstring<8, true>&                                    trace_id();
    bounded_bitstring<1, 160, true, true>&                       trace_collection_entity_ip_address();
    privacy_ind_e&                                               privacy_ind();
    visible_string<0, MAX_ASN_STRING_LENGTH, false, true>&       ur_iaddress();
    const uint64_t&                                              gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&                                              gnb_cu_up_ue_e1ap_id() const;
    const fixed_octstring<8, true>&                              trace_id() const;
    const bounded_bitstring<1, 160, true, true>&                 trace_collection_entity_ip_address() const;
    const privacy_ind_e&                                         privacy_ind() const;
    const visible_string<0, MAX_ASN_STRING_LENGTH, false, true>& ur_iaddress() const;

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
  bool                                                  privacy_ind_present = false;
  bool                                                  ur_iaddress_present = false;
  uint64_t                                              gnb_cu_cp_ue_e1ap_id;
  uint64_t                                              gnb_cu_up_ue_e1ap_id;
  fixed_octstring<8, true>                              trace_id;
  bounded_bitstring<1, 160, true, true>                 trace_collection_entity_ip_address;
  privacy_ind_e                                         privacy_ind;
  visible_string<0, MAX_ASN_STRING_LENGTH, false, true> ur_iaddress;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellTrafficTrace ::= SEQUENCE
using cell_traffic_trace_s = elementary_procedure_option<cell_traffic_trace_ies_container>;

// DLDataNotificationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct dl_data_notif_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_cp_ue_e1ap_id, gnb_cu_up_ue_e1ap_id, ppi, pdu_session_to_notify_list, nulltype } value;

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
    uint64_t&                           gnb_cu_cp_ue_e1ap_id();
    uint64_t&                           gnb_cu_up_ue_e1ap_id();
    uint8_t&                            ppi();
    pdu_session_to_notify_list_l&       pdu_session_to_notify_list();
    const uint64_t&                     gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&                     gnb_cu_up_ue_e1ap_id() const;
    const uint8_t&                      ppi() const;
    const pdu_session_to_notify_list_l& pdu_session_to_notify_list() const;

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

struct dl_data_notif_ies_container {
  bool                         ppi_present                        = false;
  bool                         pdu_session_to_notify_list_present = false;
  uint64_t                     gnb_cu_cp_ue_e1ap_id;
  uint64_t                     gnb_cu_up_ue_e1ap_id;
  uint8_t                      ppi;
  pdu_session_to_notify_list_l pdu_session_to_notify_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLDataNotification ::= SEQUENCE
using dl_data_notif_s = elementary_procedure_option<dl_data_notif_ies_container>;

// DLUPTNLAddressToUpdateList ::= SEQUENCE (SIZE (1..8)) OF DLUPTNLAddressToUpdateItem
using dl_up_tnl_address_to_upd_list_l = dyn_array<dl_up_tnl_address_to_upd_item_s>;

// DataUsageReportIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct data_usage_report_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_cp_ue_e1ap_id, gnb_cu_up_ue_e1ap_id, data_usage_report_list, nulltype } value;

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
    uint64_t&                       gnb_cu_cp_ue_e1ap_id();
    uint64_t&                       gnb_cu_up_ue_e1ap_id();
    data_usage_report_list_l&       data_usage_report_list();
    const uint64_t&                 gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&                 gnb_cu_up_ue_e1ap_id() const;
    const data_usage_report_list_l& data_usage_report_list() const;

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

struct data_usage_report_ies_container {
  uint64_t                 gnb_cu_cp_ue_e1ap_id;
  uint64_t                 gnb_cu_up_ue_e1ap_id;
  data_usage_report_list_l data_usage_report_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DataUsageReport ::= SEQUENCE
using data_usage_report_s = elementary_procedure_option<data_usage_report_ies_container>;

// DeactivateTraceIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct deactiv_trace_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_cp_ue_e1ap_id, gnb_cu_up_ue_e1ap_id, trace_id, nulltype } value;

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
    uint64_t&                       gnb_cu_cp_ue_e1ap_id();
    uint64_t&                       gnb_cu_up_ue_e1ap_id();
    fixed_octstring<8, true>&       trace_id();
    const uint64_t&                 gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&                 gnb_cu_up_ue_e1ap_id() const;
    const fixed_octstring<8, true>& trace_id() const;

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
  uint64_t                 gnb_cu_cp_ue_e1ap_id;
  uint64_t                 gnb_cu_up_ue_e1ap_id;
  fixed_octstring<8, true> trace_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DeactivateTrace ::= SEQUENCE
using deactiv_trace_s = elementary_procedure_option<deactiv_trace_ies_container>;

// SupportedPLMNs-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct supported_plmns_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { npn_support_info, extended_slice_support_list, extended_nr_cgi_support_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    npn_support_info_c&                   npn_support_info();
    extended_slice_support_list_l&        extended_slice_support_list();
    extended_nr_cgi_support_list_l&       extended_nr_cgi_support_list();
    const npn_support_info_c&             npn_support_info() const;
    const extended_slice_support_list_l&  extended_slice_support_list() const;
    const extended_nr_cgi_support_list_l& extended_nr_cgi_support_list() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-associatedLogicalE1-ConnectionItemRes ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct ue_associated_lc_e1_conn_item_res_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_associated_lc_e1_conn_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ue_associated_lc_e1_conn_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_associated_lc_e1_conn_item_s&       ue_associated_lc_e1_conn_item() { return c; }
    const ue_associated_lc_e1_conn_item_s& ue_associated_lc_e1_conn_item() const { return c; }

  private:
    ue_associated_lc_e1_conn_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// EUTRAN-GNB-CU-UP-CounterCheckRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct eutran_gnb_cu_up_counter_check_request_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_subject_to_counter_check_list_eutran, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::drbs_subject_to_counter_check_list_eutran; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_subject_to_counter_check_list_eutran_l&       drbs_subject_to_counter_check_list_eutran() { return c; }
    const drbs_subject_to_counter_check_list_eutran_l& drbs_subject_to_counter_check_list_eutran() const { return c; }

  private:
    drbs_subject_to_counter_check_list_eutran_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NG-RAN-GNB-CU-UP-CounterCheckRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct ng_ran_gnb_cu_up_counter_check_request_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_subject_to_counter_check_list_ng_ran, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::drbs_subject_to_counter_check_list_ng_ran; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_subject_to_counter_check_list_ng_ran_l&       drbs_subject_to_counter_check_list_ng_ran() { return c; }
    const drbs_subject_to_counter_check_list_ng_ran_l& drbs_subject_to_counter_check_list_ng_ran() const { return c; }

  private:
    drbs_subject_to_counter_check_list_ng_ran_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ResetAll ::= ENUMERATED
struct reset_all_opts {
  enum options { reset_all, /*...*/ nulltype } value;

  const char* to_string() const;
};
using reset_all_e = enumerated<reset_all_opts, true>;

// ResetType-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using reset_type_ext_ies_o = protocol_ies_empty_o;

struct supported_plmns_ext_ies_container {
  bool                           npn_support_info_present             = false;
  bool                           extended_slice_support_list_present  = false;
  bool                           extended_nr_cgi_support_list_present = false;
  npn_support_info_c             npn_support_info;
  extended_slice_support_list_l  extended_slice_support_list;
  extended_nr_cgi_support_list_l extended_nr_cgi_support_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedPLMNs-Item ::= SEQUENCE
struct supported_plmns_item_s {
  bool                              ext                             = false;
  bool                              qos_params_support_list_present = false;
  bool                              ie_exts_present                 = false;
  fixed_octstring<3, true>          plmn_id;
  slice_support_list_l              slice_support_list;
  nr_cgi_support_list_l             nr_cgi_support_list;
  qos_params_support_list_s         qos_params_support_list;
  supported_plmns_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// System-GNB-CU-UP-CounterCheckRequest-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using sys_gnb_cu_up_counter_check_request_ext_ies_o = protocol_ies_empty_o;

// UE-associatedLogicalE1-ConnectionItemResAck ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct ue_associated_lc_e1_conn_item_res_ack_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_associated_lc_e1_conn_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ue_associated_lc_e1_conn_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_associated_lc_e1_conn_item_s&       ue_associated_lc_e1_conn_item() { return c; }
    const ue_associated_lc_e1_conn_item_s& ue_associated_lc_e1_conn_item() const { return c; }

  private:
    ue_associated_lc_e1_conn_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-associatedLogicalE1-ConnectionListRes ::= SEQUENCE (SIZE (1..65536)) OF
// ProtocolIE-SingleContainer{E1AP-PROTOCOL-IES : IEsSetParam}
using ue_associated_lc_e1_conn_list_res_l =
    dyn_array<protocol_ie_single_container_s<ue_associated_lc_e1_conn_item_res_o>>;

// GNB-CU-CP-TNLA-Failed-To-Setup-List ::= SEQUENCE (SIZE (1..32)) OF GNB-CU-CP-TNLA-Failed-To-Setup-Item
using gnb_cu_cp_tnl_a_failed_to_setup_list_l = dyn_array<gnb_cu_cp_tnl_a_failed_to_setup_item_s>;

// GNB-CU-CP-TNLA-Setup-List ::= SEQUENCE (SIZE (1..32)) OF GNB-CU-CP-TNLA-Setup-Item
using gnb_cu_cp_tnl_a_setup_list_l = dyn_array<gnb_cu_cp_tnl_a_setup_item_s>;

// GNB-CU-CP-TNLA-To-Add-List ::= SEQUENCE (SIZE (1..32)) OF GNB-CU-CP-TNLA-To-Add-Item
using gnb_cu_cp_tnl_a_to_add_list_l = dyn_array<gnb_cu_cp_tnl_a_to_add_item_s>;

// GNB-CU-CP-TNLA-To-Remove-List ::= SEQUENCE (SIZE (1..32)) OF GNB-CU-CP-TNLA-To-Remove-Item
using gnb_cu_cp_tnl_a_to_rem_list_l = dyn_array<gnb_cu_cp_tnl_a_to_rem_item_s>;

// GNB-CU-CP-TNLA-To-Update-List ::= SEQUENCE (SIZE (1..32)) OF GNB-CU-CP-TNLA-To-Update-Item
using gnb_cu_cp_tnl_a_to_upd_list_l = dyn_array<gnb_cu_cp_tnl_a_to_upd_item_s>;

// GNB-CU-UP-TNLA-To-Remove-List ::= SEQUENCE (SIZE (1..32)) OF GNB-CU-UP-TNLA-To-Remove-Item
using gnb_cu_up_tnl_a_to_rem_list_l = dyn_array<gnb_cu_up_tnl_a_to_rem_item_s>;

// IAB-Donor-CU-UPPSKInfo ::= SEQUENCE (SIZE (1..256)) OF IAB-Donor-CU-UPPSKInfo-Item
using iab_donor_cu_up_psk_info_l = dyn_array<iab_donor_cu_up_psk_info_item_s>;

// ResetType ::= CHOICE
struct reset_type_c {
  struct types_opts {
    enum options { e1_interface, part_of_e1_interface, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  reset_type_c() = default;
  reset_type_c(const reset_type_c& other);
  reset_type_c& operator=(const reset_type_c& other);
  ~reset_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  reset_all_e& e1_interface()
  {
    assert_choice_type(types::e1_interface, type_, "ResetType");
    return c.get<reset_all_e>();
  }
  ue_associated_lc_e1_conn_list_res_l& part_of_e1_interface()
  {
    assert_choice_type(types::part_of_e1_interface, type_, "ResetType");
    return c.get<ue_associated_lc_e1_conn_list_res_l>();
  }
  protocol_ie_single_container_s<reset_type_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "ResetType");
    return c.get<protocol_ie_single_container_s<reset_type_ext_ies_o>>();
  }
  const reset_all_e& e1_interface() const
  {
    assert_choice_type(types::e1_interface, type_, "ResetType");
    return c.get<reset_all_e>();
  }
  const ue_associated_lc_e1_conn_list_res_l& part_of_e1_interface() const
  {
    assert_choice_type(types::part_of_e1_interface, type_, "ResetType");
    return c.get<ue_associated_lc_e1_conn_list_res_l>();
  }
  const protocol_ie_single_container_s<reset_type_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "ResetType");
    return c.get<protocol_ie_single_container_s<reset_type_ext_ies_o>>();
  }
  reset_all_e&                                          set_e1_interface();
  ue_associated_lc_e1_conn_list_res_l&                  set_part_of_e1_interface();
  protocol_ie_single_container_s<reset_type_ext_ies_o>& set_choice_ext();

private:
  types                                                                                                      type_;
  choice_buffer_t<protocol_ie_single_container_s<reset_type_ext_ies_o>, ue_associated_lc_e1_conn_list_res_l> c;

  void destroy_();
};

// SupportedPLMNs-List ::= SEQUENCE (SIZE (1..12)) OF SupportedPLMNs-Item
using supported_plmns_list_l = dyn_array<supported_plmns_item_s>;

// System-GNB-CU-UP-CounterCheckRequest ::= CHOICE
struct sys_gnb_cu_up_counter_check_request_c {
  struct types_opts {
    enum options {
      e_utran_gnb_cu_up_counter_check_request,
      ng_ran_gnb_cu_up_counter_check_request,
      choice_ext,
      nulltype
    } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  sys_gnb_cu_up_counter_check_request_c() = default;
  sys_gnb_cu_up_counter_check_request_c(const sys_gnb_cu_up_counter_check_request_c& other);
  sys_gnb_cu_up_counter_check_request_c& operator=(const sys_gnb_cu_up_counter_check_request_c& other);
  ~sys_gnb_cu_up_counter_check_request_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  protocol_ie_container_l<eutran_gnb_cu_up_counter_check_request_o>& e_utran_gnb_cu_up_counter_check_request()
  {
    assert_choice_type(types::e_utran_gnb_cu_up_counter_check_request, type_, "System-GNB-CU-UP-CounterCheckRequest");
    return c.get<protocol_ie_container_l<eutran_gnb_cu_up_counter_check_request_o>>();
  }
  protocol_ie_container_l<ng_ran_gnb_cu_up_counter_check_request_o>& ng_ran_gnb_cu_up_counter_check_request()
  {
    assert_choice_type(types::ng_ran_gnb_cu_up_counter_check_request, type_, "System-GNB-CU-UP-CounterCheckRequest");
    return c.get<protocol_ie_container_l<ng_ran_gnb_cu_up_counter_check_request_o>>();
  }
  protocol_ie_single_container_s<sys_gnb_cu_up_counter_check_request_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "System-GNB-CU-UP-CounterCheckRequest");
    return c.get<protocol_ie_single_container_s<sys_gnb_cu_up_counter_check_request_ext_ies_o>>();
  }
  const protocol_ie_container_l<eutran_gnb_cu_up_counter_check_request_o>&
  e_utran_gnb_cu_up_counter_check_request() const
  {
    assert_choice_type(types::e_utran_gnb_cu_up_counter_check_request, type_, "System-GNB-CU-UP-CounterCheckRequest");
    return c.get<protocol_ie_container_l<eutran_gnb_cu_up_counter_check_request_o>>();
  }
  const protocol_ie_container_l<ng_ran_gnb_cu_up_counter_check_request_o>&
  ng_ran_gnb_cu_up_counter_check_request() const
  {
    assert_choice_type(types::ng_ran_gnb_cu_up_counter_check_request, type_, "System-GNB-CU-UP-CounterCheckRequest");
    return c.get<protocol_ie_container_l<ng_ran_gnb_cu_up_counter_check_request_o>>();
  }
  const protocol_ie_single_container_s<sys_gnb_cu_up_counter_check_request_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "System-GNB-CU-UP-CounterCheckRequest");
    return c.get<protocol_ie_single_container_s<sys_gnb_cu_up_counter_check_request_ext_ies_o>>();
  }
  protocol_ie_container_l<eutran_gnb_cu_up_counter_check_request_o>& set_e_utran_gnb_cu_up_counter_check_request();
  protocol_ie_container_l<ng_ran_gnb_cu_up_counter_check_request_o>& set_ng_ran_gnb_cu_up_counter_check_request();
  protocol_ie_single_container_s<sys_gnb_cu_up_counter_check_request_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<protocol_ie_container_l<eutran_gnb_cu_up_counter_check_request_o>,
                  protocol_ie_container_l<ng_ran_gnb_cu_up_counter_check_request_o>,
                  protocol_ie_single_container_s<sys_gnb_cu_up_counter_check_request_ext_ies_o>>
      c;

  void destroy_();
};

// UE-associatedLogicalE1-ConnectionListResAck ::= SEQUENCE (SIZE (1..65536)) OF
// ProtocolIE-SingleContainer{E1AP-PROTOCOL-IES : IEsSetParam}
using ue_associated_lc_e1_conn_list_res_ack_l =
    dyn_array<protocol_ie_single_container_s<ue_associated_lc_e1_conn_item_res_ack_o>>;

// ULUPTNLAddressToUpdateList ::= SEQUENCE (SIZE (1..8)) OF ULUPTNLAddressToUpdateItem
using ul_up_tnl_address_to_upd_list_l = dyn_array<ul_up_tnl_address_to_upd_item_s>;

// E1ReleaseRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct e1_release_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, nulltype } value;
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
    uint16_t&       transaction_id();
    cause_c&        cause();
    const uint16_t& transaction_id() const;
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

// E1ReleaseResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct e1_release_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::transaction_id; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&       transaction_id() { return c; }
    const uint16_t& transaction_id() const { return c; }

  private:
    uint16_t c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// EarlyForwardingSNTransferIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct early_forwarding_sn_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_cp_ue_e1ap_id,
        gnb_cu_up_ue_e1ap_id,
        drbs_subject_to_early_forwarding_list,
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
    uint64_t&                                      gnb_cu_cp_ue_e1ap_id();
    uint64_t&                                      gnb_cu_up_ue_e1ap_id();
    drbs_subject_to_early_forwarding_list_l&       drbs_subject_to_early_forwarding_list();
    const uint64_t&                                gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&                                gnb_cu_up_ue_e1ap_id() const;
    const drbs_subject_to_early_forwarding_list_l& drbs_subject_to_early_forwarding_list() const;

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

// ErrorIndication-IEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct error_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        gnb_cu_cp_ue_e1ap_id,
        gnb_cu_up_ue_e1ap_id,
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
    uint16_t&                 transaction_id();
    uint64_t&                 gnb_cu_cp_ue_e1ap_id();
    uint64_t&                 gnb_cu_up_ue_e1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const uint64_t&           gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&           gnb_cu_up_ue_e1ap_id() const;
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

// GNB-CU-CP-ConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct gnb_cu_cp_cfg_upd_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        crit_diagnostics,
        gnb_cu_cp_tnl_a_setup_list,
        gnb_cu_cp_tnl_a_failed_to_setup_list,
        transport_layer_address_info,
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
    uint16_t&                                     transaction_id();
    crit_diagnostics_s&                           crit_diagnostics();
    gnb_cu_cp_tnl_a_setup_list_l&                 gnb_cu_cp_tnl_a_setup_list();
    gnb_cu_cp_tnl_a_failed_to_setup_list_l&       gnb_cu_cp_tnl_a_failed_to_setup_list();
    transport_layer_address_info_s&               transport_layer_address_info();
    const uint16_t&                               transaction_id() const;
    const crit_diagnostics_s&                     crit_diagnostics() const;
    const gnb_cu_cp_tnl_a_setup_list_l&           gnb_cu_cp_tnl_a_setup_list() const;
    const gnb_cu_cp_tnl_a_failed_to_setup_list_l& gnb_cu_cp_tnl_a_failed_to_setup_list() const;
    const transport_layer_address_info_s&         transport_layer_address_info() const;

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

// GNB-CU-CP-ConfigurationUpdateFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct gnb_cu_cp_cfg_upd_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, nulltype } value;
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
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
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

// GNB-CU-CP-ConfigurationUpdateIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct gnb_cu_cp_cfg_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        gnb_cu_cp_name,
        gnb_cu_cp_tnl_a_to_add_list,
        gnb_cu_cp_tnl_a_to_rem_list,
        gnb_cu_cp_tnl_a_to_upd_list,
        transport_layer_address_info,
        extended_gnb_cu_cp_name,
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
    uint16_t&                                   transaction_id();
    printable_string<1, 150, true, true>&       gnb_cu_cp_name();
    gnb_cu_cp_tnl_a_to_add_list_l&              gnb_cu_cp_tnl_a_to_add_list();
    gnb_cu_cp_tnl_a_to_rem_list_l&              gnb_cu_cp_tnl_a_to_rem_list();
    gnb_cu_cp_tnl_a_to_upd_list_l&              gnb_cu_cp_tnl_a_to_upd_list();
    transport_layer_address_info_s&             transport_layer_address_info();
    extended_gnb_cu_cp_name_s&                  extended_gnb_cu_cp_name();
    const uint16_t&                             transaction_id() const;
    const printable_string<1, 150, true, true>& gnb_cu_cp_name() const;
    const gnb_cu_cp_tnl_a_to_add_list_l&        gnb_cu_cp_tnl_a_to_add_list() const;
    const gnb_cu_cp_tnl_a_to_rem_list_l&        gnb_cu_cp_tnl_a_to_rem_list() const;
    const gnb_cu_cp_tnl_a_to_upd_list_l&        gnb_cu_cp_tnl_a_to_upd_list() const;
    const transport_layer_address_info_s&       transport_layer_address_info() const;
    const extended_gnb_cu_cp_name_s&            extended_gnb_cu_cp_name() const;

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

// GNB-CU-CP-E1SetupFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct gnb_cu_cp_e1_setup_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, nulltype } value;
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
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
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

// GNB-CU-CP-E1SetupRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct gnb_cu_cp_e1_setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        gnb_cu_cp_name,
        transport_layer_address_info,
        extended_gnb_cu_cp_name,
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
    uint16_t&                                   transaction_id();
    printable_string<1, 150, true, true>&       gnb_cu_cp_name();
    transport_layer_address_info_s&             transport_layer_address_info();
    extended_gnb_cu_cp_name_s&                  extended_gnb_cu_cp_name();
    const uint16_t&                             transaction_id() const;
    const printable_string<1, 150, true, true>& gnb_cu_cp_name() const;
    const transport_layer_address_info_s&       transport_layer_address_info() const;
    const extended_gnb_cu_cp_name_s&            extended_gnb_cu_cp_name() const;

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

// GNB-CU-CP-E1SetupResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct gnb_cu_cp_e1_setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        gnb_cu_up_id,
        gnb_cu_up_name,
        cn_support,
        supported_plmns,
        gnb_cu_up_capacity,
        transport_layer_address_info,
        extended_gnb_cu_up_name,
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
    uint16_t&                                   transaction_id();
    uint64_t&                                   gnb_cu_up_id();
    printable_string<1, 150, true, true>&       gnb_cu_up_name();
    cn_support_e&                               cn_support();
    supported_plmns_list_l&                     supported_plmns();
    uint16_t&                                   gnb_cu_up_capacity();
    transport_layer_address_info_s&             transport_layer_address_info();
    extended_gnb_cu_up_name_s&                  extended_gnb_cu_up_name();
    const uint16_t&                             transaction_id() const;
    const uint64_t&                             gnb_cu_up_id() const;
    const printable_string<1, 150, true, true>& gnb_cu_up_name() const;
    const cn_support_e&                         cn_support() const;
    const supported_plmns_list_l&               supported_plmns() const;
    const uint16_t&                             gnb_cu_up_capacity() const;
    const transport_layer_address_info_s&       transport_layer_address_info() const;
    const extended_gnb_cu_up_name_s&            extended_gnb_cu_up_name() const;

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

// GNB-CU-CPMeasurementResultsInformationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct gnb_cu_cp_meas_results_info_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_cp_ue_e1ap_id, gnb_cu_up_ue_e1ap_id, drb_meas_results_info_list, nulltype } value;

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
    uint64_t&                           gnb_cu_cp_ue_e1ap_id();
    uint64_t&                           gnb_cu_up_ue_e1ap_id();
    drb_meas_results_info_list_l&       drb_meas_results_info_list();
    const uint64_t&                     gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&                     gnb_cu_up_ue_e1ap_id() const;
    const drb_meas_results_info_list_l& drb_meas_results_info_list() const;

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

// GNB-CU-UP-ConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct gnb_cu_up_cfg_upd_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, crit_diagnostics, transport_layer_address_info, nulltype } value;
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
    uint16_t&                             transaction_id();
    crit_diagnostics_s&                   crit_diagnostics();
    transport_layer_address_info_s&       transport_layer_address_info();
    const uint16_t&                       transaction_id() const;
    const crit_diagnostics_s&             crit_diagnostics() const;
    const transport_layer_address_info_s& transport_layer_address_info() const;

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

// GNB-CU-UP-ConfigurationUpdateFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct gnb_cu_up_cfg_upd_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, nulltype } value;
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
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
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

// GNB-CU-UP-ConfigurationUpdateIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct gnb_cu_up_cfg_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        gnb_cu_up_id,
        gnb_cu_up_name,
        supported_plmns,
        gnb_cu_up_capacity,
        gnb_cu_up_tnl_a_to_rem_list,
        transport_layer_address_info,
        extended_gnb_cu_up_name,
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
    uint16_t&                                   transaction_id();
    uint64_t&                                   gnb_cu_up_id();
    printable_string<1, 150, true, true>&       gnb_cu_up_name();
    supported_plmns_list_l&                     supported_plmns();
    uint16_t&                                   gnb_cu_up_capacity();
    gnb_cu_up_tnl_a_to_rem_list_l&              gnb_cu_up_tnl_a_to_rem_list();
    transport_layer_address_info_s&             transport_layer_address_info();
    extended_gnb_cu_up_name_s&                  extended_gnb_cu_up_name();
    const uint16_t&                             transaction_id() const;
    const uint64_t&                             gnb_cu_up_id() const;
    const printable_string<1, 150, true, true>& gnb_cu_up_name() const;
    const supported_plmns_list_l&               supported_plmns() const;
    const uint16_t&                             gnb_cu_up_capacity() const;
    const gnb_cu_up_tnl_a_to_rem_list_l&        gnb_cu_up_tnl_a_to_rem_list() const;
    const transport_layer_address_info_s&       transport_layer_address_info() const;
    const extended_gnb_cu_up_name_s&            extended_gnb_cu_up_name() const;

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

// GNB-CU-UP-CounterCheckRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct gnb_cu_up_counter_check_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_cp_ue_e1ap_id, gnb_cu_up_ue_e1ap_id, sys_gnb_cu_up_counter_check_request, nulltype } value;

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
    uint64_t&                                    gnb_cu_cp_ue_e1ap_id();
    uint64_t&                                    gnb_cu_up_ue_e1ap_id();
    sys_gnb_cu_up_counter_check_request_c&       sys_gnb_cu_up_counter_check_request();
    const uint64_t&                              gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&                              gnb_cu_up_ue_e1ap_id() const;
    const sys_gnb_cu_up_counter_check_request_c& sys_gnb_cu_up_counter_check_request() const;

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

// GNB-CU-UP-E1SetupFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct gnb_cu_up_e1_setup_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, nulltype } value;
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
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
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

// GNB-CU-UP-E1SetupRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct gnb_cu_up_e1_setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        gnb_cu_up_id,
        gnb_cu_up_name,
        cn_support,
        supported_plmns,
        gnb_cu_up_capacity,
        transport_layer_address_info,
        extended_gnb_cu_up_name,
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
    uint16_t&                                   transaction_id();
    uint64_t&                                   gnb_cu_up_id();
    printable_string<1, 150, true, true>&       gnb_cu_up_name();
    cn_support_e&                               cn_support();
    supported_plmns_list_l&                     supported_plmns();
    uint16_t&                                   gnb_cu_up_capacity();
    transport_layer_address_info_s&             transport_layer_address_info();
    extended_gnb_cu_up_name_s&                  extended_gnb_cu_up_name();
    const uint16_t&                             transaction_id() const;
    const uint64_t&                             gnb_cu_up_id() const;
    const printable_string<1, 150, true, true>& gnb_cu_up_name() const;
    const cn_support_e&                         cn_support() const;
    const supported_plmns_list_l&               supported_plmns() const;
    const uint16_t&                             gnb_cu_up_capacity() const;
    const transport_layer_address_info_s&       transport_layer_address_info() const;
    const extended_gnb_cu_up_name_s&            extended_gnb_cu_up_name() const;

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

// GNB-CU-UP-E1SetupResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct gnb_cu_up_e1_setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        gnb_cu_cp_name,
        transport_layer_address_info,
        extended_gnb_cu_cp_name,
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
    uint16_t&                                   transaction_id();
    printable_string<1, 150, true, true>&       gnb_cu_cp_name();
    transport_layer_address_info_s&             transport_layer_address_info();
    extended_gnb_cu_cp_name_s&                  extended_gnb_cu_cp_name();
    const uint16_t&                             transaction_id() const;
    const printable_string<1, 150, true, true>& gnb_cu_cp_name() const;
    const transport_layer_address_info_s&       transport_layer_address_info() const;
    const extended_gnb_cu_cp_name_s&            extended_gnb_cu_cp_name() const;

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

// GNB-CU-UP-StatusIndicationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct gnb_cu_up_status_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, gnb_cu_up_overload_info, nulltype } value;
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
    uint16_t&                        transaction_id();
    gnb_cu_up_overload_info_e&       gnb_cu_up_overload_info();
    const uint16_t&                  transaction_id() const;
    const gnb_cu_up_overload_info_e& gnb_cu_up_overload_info() const;

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

// IAB-UPTNLAddressUpdateAcknowledgeIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct iab_up_tnl_address_upd_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, crit_diagnostics, ul_up_tnl_address_to_upd_list, nulltype } value;
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
    uint16_t&                              transaction_id();
    crit_diagnostics_s&                    crit_diagnostics();
    ul_up_tnl_address_to_upd_list_l&       ul_up_tnl_address_to_upd_list();
    const uint16_t&                        transaction_id() const;
    const crit_diagnostics_s&              crit_diagnostics() const;
    const ul_up_tnl_address_to_upd_list_l& ul_up_tnl_address_to_upd_list() const;

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

// IAB-UPTNLAddressUpdateFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct iab_up_tnl_address_upd_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, nulltype } value;
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
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
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

// IAB-UPTNLAddressUpdateIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct iab_up_tnl_address_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, dl_up_tnl_address_to_upd_list, nulltype } value;
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
    uint16_t&                              transaction_id();
    dl_up_tnl_address_to_upd_list_l&       dl_up_tnl_address_to_upd_list();
    const uint16_t&                        transaction_id() const;
    const dl_up_tnl_address_to_upd_list_l& dl_up_tnl_address_to_upd_list() const;

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

// IABPSKNotificationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct iabpsk_notif_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, iab_donor_cu_up_psk_info, nulltype } value;
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
    uint16_t&                         transaction_id();
    iab_donor_cu_up_psk_info_l&       iab_donor_cu_up_psk_info();
    const uint16_t&                   transaction_id() const;
    const iab_donor_cu_up_psk_info_l& iab_donor_cu_up_psk_info() const;

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

// MRDC-DataUsageReportIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct mrdc_data_usage_report_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_cp_ue_e1ap_id, gnb_cu_up_ue_e1ap_id, pdu_session_res_data_usage_list, nulltype } value;

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
    uint64_t&                                gnb_cu_cp_ue_e1ap_id();
    uint64_t&                                gnb_cu_up_ue_e1ap_id();
    pdu_session_res_data_usage_list_l&       pdu_session_res_data_usage_list();
    const uint64_t&                          gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&                          gnb_cu_up_ue_e1ap_id() const;
    const pdu_session_res_data_usage_list_l& pdu_session_res_data_usage_list() const;

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

struct e1ap_private_ies_empty_o {
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
// PrivateMessage-IEs ::= OBJECT SET OF E1AP-PRIVATE-IES
using private_msg_ies_o = e1ap_private_ies_empty_o;

// ResetAcknowledgeIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct reset_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, ue_associated_lc_e1_conn_list_res_ack, crit_diagnostics, nulltype } value;
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
    uint16_t&                                      transaction_id();
    ue_associated_lc_e1_conn_list_res_ack_l&       ue_associated_lc_e1_conn_list_res_ack();
    crit_diagnostics_s&                            crit_diagnostics();
    const uint16_t&                                transaction_id() const;
    const ue_associated_lc_e1_conn_list_res_ack_l& ue_associated_lc_e1_conn_list_res_ack() const;
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

// ResetIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct reset_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, reset_type, nulltype } value;
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
    uint16_t&           transaction_id();
    cause_c&            cause();
    reset_type_c&       reset_type();
    const uint16_t&     transaction_id() const;
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

// ResourceStatusFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct res_status_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, gnb_cu_cp_meas_id, gnb_cu_up_meas_id, cause, crit_diagnostics, nulltype } value;

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
    uint16_t&                 transaction_id();
    uint16_t&                 gnb_cu_cp_meas_id();
    uint16_t&                 gnb_cu_up_meas_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const uint16_t&           gnb_cu_cp_meas_id() const;
    const uint16_t&           gnb_cu_up_meas_id() const;
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

// ResourceStatusRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct res_status_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        gnb_cu_cp_meas_id,
        gnb_cu_up_meas_id,
        regist_request,
        report_characteristics,
        report_periodicity,
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
    uint16_t&                               transaction_id();
    uint16_t&                               gnb_cu_cp_meas_id();
    uint16_t&                               gnb_cu_up_meas_id();
    regist_request_e&                       regist_request();
    fixed_bitstring<36, false, true>&       report_characteristics();
    report_periodicity_e&                   report_periodicity();
    const uint16_t&                         transaction_id() const;
    const uint16_t&                         gnb_cu_cp_meas_id() const;
    const uint16_t&                         gnb_cu_up_meas_id() const;
    const regist_request_e&                 regist_request() const;
    const fixed_bitstring<36, false, true>& report_characteristics() const;
    const report_periodicity_e&             report_periodicity() const;

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

// ResourceStatusResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct res_status_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, gnb_cu_cp_meas_id, gnb_cu_up_meas_id, crit_diagnostics, nulltype } value;

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
    uint16_t&                 transaction_id();
    uint16_t&                 gnb_cu_cp_meas_id();
    uint16_t&                 gnb_cu_up_meas_id();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const uint16_t&           gnb_cu_cp_meas_id() const;
    const uint16_t&           gnb_cu_up_meas_id() const;
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

// ResourceStatusUpdateIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct res_status_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        gnb_cu_cp_meas_id,
        gnb_cu_up_meas_id,
        tnl_available_capacity_ind,
        hw_capacity_ind,
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
    uint16_t&                           transaction_id();
    uint16_t&                           gnb_cu_cp_meas_id();
    uint16_t&                           gnb_cu_up_meas_id();
    tnl_available_capacity_ind_s&       tnl_available_capacity_ind();
    hw_capacity_ind_s&                  hw_capacity_ind();
    const uint16_t&                     transaction_id() const;
    const uint16_t&                     gnb_cu_cp_meas_id() const;
    const uint16_t&                     gnb_cu_up_meas_id() const;
    const tnl_available_capacity_ind_s& tnl_available_capacity_ind() const;
    const hw_capacity_ind_s&            hw_capacity_ind() const;

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

// TraceStartIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct trace_start_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_cp_ue_e1ap_id, gnb_cu_up_ue_e1ap_id, trace_activation, nulltype } value;

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
    uint64_t&                 gnb_cu_cp_ue_e1ap_id();
    uint64_t&                 gnb_cu_up_ue_e1ap_id();
    trace_activation_s&       trace_activation();
    const uint64_t&           gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&           gnb_cu_up_ue_e1ap_id() const;
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

// ULDataNotificationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct ul_data_notif_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_cp_ue_e1ap_id, gnb_cu_up_ue_e1ap_id, pdu_session_to_notify_list, nulltype } value;

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
    uint64_t&                           gnb_cu_cp_ue_e1ap_id();
    uint64_t&                           gnb_cu_up_ue_e1ap_id();
    pdu_session_to_notify_list_l&       pdu_session_to_notify_list();
    const uint64_t&                     gnb_cu_cp_ue_e1ap_id() const;
    const uint64_t&                     gnb_cu_up_ue_e1ap_id() const;
    const pdu_session_to_notify_list_l& pdu_session_to_notify_list() const;

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

struct e1_release_request_ies_container {
  uint16_t transaction_id;
  cause_c  cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E1ReleaseRequest ::= SEQUENCE
using e1_release_request_s = elementary_procedure_option<e1_release_request_ies_container>;

// E1ReleaseResponse ::= SEQUENCE
using e1_release_resp_s = elementary_procedure_option<protocol_ie_container_l<e1_release_resp_ies_o>>;

struct early_forwarding_sn_transfer_ies_container {
  uint64_t                                gnb_cu_cp_ue_e1ap_id;
  uint64_t                                gnb_cu_up_ue_e1ap_id;
  drbs_subject_to_early_forwarding_list_l drbs_subject_to_early_forwarding_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EarlyForwardingSNTransfer ::= SEQUENCE
using early_forwarding_sn_transfer_s = elementary_procedure_option<early_forwarding_sn_transfer_ies_container>;

struct error_ind_ies_container {
  bool               gnb_cu_cp_ue_e1ap_id_present = false;
  bool               gnb_cu_up_ue_e1ap_id_present = false;
  bool               cause_present                = false;
  bool               crit_diagnostics_present     = false;
  uint16_t           transaction_id;
  uint64_t           gnb_cu_cp_ue_e1ap_id;
  uint64_t           gnb_cu_up_ue_e1ap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ErrorIndication ::= SEQUENCE
using error_ind_s = elementary_procedure_option<error_ind_ies_container>;

struct gnb_cu_cp_cfg_upd_ies_container {
  bool                                 gnb_cu_cp_name_present               = false;
  bool                                 gnb_cu_cp_tnl_a_to_add_list_present  = false;
  bool                                 gnb_cu_cp_tnl_a_to_rem_list_present  = false;
  bool                                 gnb_cu_cp_tnl_a_to_upd_list_present  = false;
  bool                                 transport_layer_address_info_present = false;
  bool                                 extended_gnb_cu_cp_name_present      = false;
  uint16_t                             transaction_id;
  printable_string<1, 150, true, true> gnb_cu_cp_name;
  gnb_cu_cp_tnl_a_to_add_list_l        gnb_cu_cp_tnl_a_to_add_list;
  gnb_cu_cp_tnl_a_to_rem_list_l        gnb_cu_cp_tnl_a_to_rem_list;
  gnb_cu_cp_tnl_a_to_upd_list_l        gnb_cu_cp_tnl_a_to_upd_list;
  transport_layer_address_info_s       transport_layer_address_info;
  extended_gnb_cu_cp_name_s            extended_gnb_cu_cp_name;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-CP-ConfigurationUpdate ::= SEQUENCE
using gnb_cu_cp_cfg_upd_s = elementary_procedure_option<gnb_cu_cp_cfg_upd_ies_container>;

struct gnb_cu_cp_cfg_upd_ack_ies_container {
  bool                                   crit_diagnostics_present                     = false;
  bool                                   gnb_cu_cp_tnl_a_setup_list_present           = false;
  bool                                   gnb_cu_cp_tnl_a_failed_to_setup_list_present = false;
  bool                                   transport_layer_address_info_present         = false;
  uint16_t                               transaction_id;
  crit_diagnostics_s                     crit_diagnostics;
  gnb_cu_cp_tnl_a_setup_list_l           gnb_cu_cp_tnl_a_setup_list;
  gnb_cu_cp_tnl_a_failed_to_setup_list_l gnb_cu_cp_tnl_a_failed_to_setup_list;
  transport_layer_address_info_s         transport_layer_address_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-CP-ConfigurationUpdateAcknowledge ::= SEQUENCE
using gnb_cu_cp_cfg_upd_ack_s = elementary_procedure_option<gnb_cu_cp_cfg_upd_ack_ies_container>;

struct gnb_cu_cp_cfg_upd_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-CP-ConfigurationUpdateFailure ::= SEQUENCE
using gnb_cu_cp_cfg_upd_fail_s = elementary_procedure_option<gnb_cu_cp_cfg_upd_fail_ies_container>;

struct gnb_cu_cp_e1_setup_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-CP-E1SetupFailure ::= SEQUENCE
using gnb_cu_cp_e1_setup_fail_s = elementary_procedure_option<gnb_cu_cp_e1_setup_fail_ies_container>;

struct gnb_cu_cp_e1_setup_request_ies_container {
  bool                                 gnb_cu_cp_name_present               = false;
  bool                                 transport_layer_address_info_present = false;
  bool                                 extended_gnb_cu_cp_name_present      = false;
  uint16_t                             transaction_id;
  printable_string<1, 150, true, true> gnb_cu_cp_name;
  transport_layer_address_info_s       transport_layer_address_info;
  extended_gnb_cu_cp_name_s            extended_gnb_cu_cp_name;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-CP-E1SetupRequest ::= SEQUENCE
using gnb_cu_cp_e1_setup_request_s = elementary_procedure_option<gnb_cu_cp_e1_setup_request_ies_container>;

struct gnb_cu_cp_e1_setup_resp_ies_container {
  bool                                 gnb_cu_up_name_present               = false;
  bool                                 gnb_cu_up_capacity_present           = false;
  bool                                 transport_layer_address_info_present = false;
  bool                                 extended_gnb_cu_up_name_present      = false;
  uint16_t                             transaction_id;
  uint64_t                             gnb_cu_up_id;
  printable_string<1, 150, true, true> gnb_cu_up_name;
  cn_support_e                         cn_support;
  supported_plmns_list_l               supported_plmns;
  uint16_t                             gnb_cu_up_capacity;
  transport_layer_address_info_s       transport_layer_address_info;
  extended_gnb_cu_up_name_s            extended_gnb_cu_up_name;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-CP-E1SetupResponse ::= SEQUENCE
using gnb_cu_cp_e1_setup_resp_s = elementary_procedure_option<gnb_cu_cp_e1_setup_resp_ies_container>;

struct gnb_cu_cp_meas_results_info_ies_container {
  uint64_t                     gnb_cu_cp_ue_e1ap_id;
  uint64_t                     gnb_cu_up_ue_e1ap_id;
  drb_meas_results_info_list_l drb_meas_results_info_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-CPMeasurementResultsInformation ::= SEQUENCE
using gnb_cu_cp_meas_results_info_s = elementary_procedure_option<gnb_cu_cp_meas_results_info_ies_container>;

struct gnb_cu_up_cfg_upd_ies_container {
  bool                                 gnb_cu_up_name_present               = false;
  bool                                 supported_plmns_present              = false;
  bool                                 gnb_cu_up_capacity_present           = false;
  bool                                 gnb_cu_up_tnl_a_to_rem_list_present  = false;
  bool                                 transport_layer_address_info_present = false;
  bool                                 extended_gnb_cu_up_name_present      = false;
  uint16_t                             transaction_id;
  uint64_t                             gnb_cu_up_id;
  printable_string<1, 150, true, true> gnb_cu_up_name;
  supported_plmns_list_l               supported_plmns;
  uint16_t                             gnb_cu_up_capacity;
  gnb_cu_up_tnl_a_to_rem_list_l        gnb_cu_up_tnl_a_to_rem_list;
  transport_layer_address_info_s       transport_layer_address_info;
  extended_gnb_cu_up_name_s            extended_gnb_cu_up_name;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-UP-ConfigurationUpdate ::= SEQUENCE
using gnb_cu_up_cfg_upd_s = elementary_procedure_option<gnb_cu_up_cfg_upd_ies_container>;

struct gnb_cu_up_cfg_upd_ack_ies_container {
  bool                           crit_diagnostics_present             = false;
  bool                           transport_layer_address_info_present = false;
  uint16_t                       transaction_id;
  crit_diagnostics_s             crit_diagnostics;
  transport_layer_address_info_s transport_layer_address_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-UP-ConfigurationUpdateAcknowledge ::= SEQUENCE
using gnb_cu_up_cfg_upd_ack_s = elementary_procedure_option<gnb_cu_up_cfg_upd_ack_ies_container>;

struct gnb_cu_up_cfg_upd_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-UP-ConfigurationUpdateFailure ::= SEQUENCE
using gnb_cu_up_cfg_upd_fail_s = elementary_procedure_option<gnb_cu_up_cfg_upd_fail_ies_container>;

struct gnb_cu_up_counter_check_request_ies_container {
  uint64_t                              gnb_cu_cp_ue_e1ap_id;
  uint64_t                              gnb_cu_up_ue_e1ap_id;
  sys_gnb_cu_up_counter_check_request_c sys_gnb_cu_up_counter_check_request;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-UP-CounterCheckRequest ::= SEQUENCE
using gnb_cu_up_counter_check_request_s = elementary_procedure_option<gnb_cu_up_counter_check_request_ies_container>;

struct gnb_cu_up_e1_setup_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-UP-E1SetupFailure ::= SEQUENCE
using gnb_cu_up_e1_setup_fail_s = elementary_procedure_option<gnb_cu_up_e1_setup_fail_ies_container>;

struct gnb_cu_up_e1_setup_request_ies_container {
  bool                                 gnb_cu_up_name_present               = false;
  bool                                 gnb_cu_up_capacity_present           = false;
  bool                                 transport_layer_address_info_present = false;
  bool                                 extended_gnb_cu_up_name_present      = false;
  uint16_t                             transaction_id;
  uint64_t                             gnb_cu_up_id;
  printable_string<1, 150, true, true> gnb_cu_up_name;
  cn_support_e                         cn_support;
  supported_plmns_list_l               supported_plmns;
  uint16_t                             gnb_cu_up_capacity;
  transport_layer_address_info_s       transport_layer_address_info;
  extended_gnb_cu_up_name_s            extended_gnb_cu_up_name;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-UP-E1SetupRequest ::= SEQUENCE
using gnb_cu_up_e1_setup_request_s = elementary_procedure_option<gnb_cu_up_e1_setup_request_ies_container>;

struct gnb_cu_up_e1_setup_resp_ies_container {
  bool                                 gnb_cu_cp_name_present               = false;
  bool                                 transport_layer_address_info_present = false;
  bool                                 extended_gnb_cu_cp_name_present      = false;
  uint16_t                             transaction_id;
  printable_string<1, 150, true, true> gnb_cu_cp_name;
  transport_layer_address_info_s       transport_layer_address_info;
  extended_gnb_cu_cp_name_s            extended_gnb_cu_cp_name;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-UP-E1SetupResponse ::= SEQUENCE
using gnb_cu_up_e1_setup_resp_s = elementary_procedure_option<gnb_cu_up_e1_setup_resp_ies_container>;

struct gnb_cu_up_status_ind_ies_container {
  uint16_t                  transaction_id;
  gnb_cu_up_overload_info_e gnb_cu_up_overload_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-UP-StatusIndication ::= SEQUENCE
using gnb_cu_up_status_ind_s = elementary_procedure_option<gnb_cu_up_status_ind_ies_container>;

struct iab_up_tnl_address_upd_ies_container {
  bool                            dl_up_tnl_address_to_upd_list_present = false;
  uint16_t                        transaction_id;
  dl_up_tnl_address_to_upd_list_l dl_up_tnl_address_to_upd_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IAB-UPTNLAddressUpdate ::= SEQUENCE
using iab_up_tnl_address_upd_s = elementary_procedure_option<iab_up_tnl_address_upd_ies_container>;

struct iab_up_tnl_address_upd_ack_ies_container {
  bool                            crit_diagnostics_present              = false;
  bool                            ul_up_tnl_address_to_upd_list_present = false;
  uint16_t                        transaction_id;
  crit_diagnostics_s              crit_diagnostics;
  ul_up_tnl_address_to_upd_list_l ul_up_tnl_address_to_upd_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IAB-UPTNLAddressUpdateAcknowledge ::= SEQUENCE
using iab_up_tnl_address_upd_ack_s = elementary_procedure_option<iab_up_tnl_address_upd_ack_ies_container>;

struct iab_up_tnl_address_upd_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IAB-UPTNLAddressUpdateFailure ::= SEQUENCE
using iab_up_tnl_address_upd_fail_s = elementary_procedure_option<iab_up_tnl_address_upd_fail_ies_container>;

struct iabpsk_notif_ies_container {
  uint16_t                   transaction_id;
  iab_donor_cu_up_psk_info_l iab_donor_cu_up_psk_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IABPSKNotification ::= SEQUENCE
using iabpsk_notif_s = elementary_procedure_option<iabpsk_notif_ies_container>;

struct mrdc_data_usage_report_ies_container {
  uint64_t                          gnb_cu_cp_ue_e1ap_id;
  uint64_t                          gnb_cu_up_ue_e1ap_id;
  pdu_session_res_data_usage_list_l pdu_session_res_data_usage_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRDC-DataUsageReport ::= SEQUENCE
using mrdc_data_usage_report_s = elementary_procedure_option<mrdc_data_usage_report_ies_container>;

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

struct reset_ies_container {
  uint16_t     transaction_id;
  cause_c      cause;
  reset_type_c reset_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Reset ::= SEQUENCE
using reset_s = elementary_procedure_option<reset_ies_container>;

struct reset_ack_ies_container {
  bool                                    ue_associated_lc_e1_conn_list_res_ack_present = false;
  bool                                    crit_diagnostics_present                      = false;
  uint16_t                                transaction_id;
  ue_associated_lc_e1_conn_list_res_ack_l ue_associated_lc_e1_conn_list_res_ack;
  crit_diagnostics_s                      crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResetAcknowledge ::= SEQUENCE
using reset_ack_s = elementary_procedure_option<reset_ack_ies_container>;

struct res_status_fail_ies_container {
  bool               gnb_cu_up_meas_id_present = false;
  bool               crit_diagnostics_present  = false;
  uint16_t           transaction_id;
  uint16_t           gnb_cu_cp_meas_id;
  uint16_t           gnb_cu_up_meas_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceStatusFailure ::= SEQUENCE
using res_status_fail_s = elementary_procedure_option<res_status_fail_ies_container>;

struct res_status_request_ies_container {
  bool                             gnb_cu_up_meas_id_present      = false;
  bool                             report_characteristics_present = false;
  bool                             report_periodicity_present     = false;
  uint16_t                         transaction_id;
  uint16_t                         gnb_cu_cp_meas_id;
  uint16_t                         gnb_cu_up_meas_id;
  regist_request_e                 regist_request;
  fixed_bitstring<36, false, true> report_characteristics;
  report_periodicity_e             report_periodicity;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceStatusRequest ::= SEQUENCE
using res_status_request_s = elementary_procedure_option<res_status_request_ies_container>;

struct res_status_resp_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  uint16_t           gnb_cu_cp_meas_id;
  uint16_t           gnb_cu_up_meas_id;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceStatusResponse ::= SEQUENCE
using res_status_resp_s = elementary_procedure_option<res_status_resp_ies_container>;

struct res_status_upd_ies_container {
  bool                         gnb_cu_up_meas_id_present          = false;
  bool                         tnl_available_capacity_ind_present = false;
  uint16_t                     transaction_id;
  uint16_t                     gnb_cu_cp_meas_id;
  uint16_t                     gnb_cu_up_meas_id;
  tnl_available_capacity_ind_s tnl_available_capacity_ind;
  hw_capacity_ind_s            hw_capacity_ind;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceStatusUpdate ::= SEQUENCE
using res_status_upd_s = elementary_procedure_option<res_status_upd_ies_container>;

struct trace_start_ies_container {
  uint64_t           gnb_cu_cp_ue_e1ap_id;
  uint64_t           gnb_cu_up_ue_e1ap_id;
  trace_activation_s trace_activation;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TraceStart ::= SEQUENCE
using trace_start_s = elementary_procedure_option<trace_start_ies_container>;

struct ul_data_notif_ies_container {
  uint64_t                     gnb_cu_cp_ue_e1ap_id;
  uint64_t                     gnb_cu_up_ue_e1ap_id;
  pdu_session_to_notify_list_l pdu_session_to_notify_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULDataNotification ::= SEQUENCE
using ul_data_notif_s = elementary_procedure_option<ul_data_notif_ies_container>;

} // namespace e1ap
} // namespace asn1

extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_inactivity_notif_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_mod_confirm_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_mod_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::eutran_bearer_context_mod_request_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::ng_ran_bearer_context_mod_request_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_mod_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::eutran_bearer_context_mod_required_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::ng_ran_bearer_context_mod_required_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_mod_required_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::eutran_bearer_context_mod_resp_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::ng_ran_bearer_context_mod_resp_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_mod_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_release_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_release_complete_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_release_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::eutran_bearer_context_setup_request_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::eutran_bearer_context_setup_resp_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::ng_ran_bearer_context_setup_resp_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::cell_traffic_trace_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::dl_data_notif_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::data_usage_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::deactiv_trace_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::supported_plmns_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e1ap::ue_associated_lc_e1_conn_item_res_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e1ap::ue_associated_lc_e1_conn_item_res_ack_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::e1_release_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::early_forwarding_sn_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::error_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::gnb_cu_cp_cfg_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::gnb_cu_cp_cfg_upd_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::gnb_cu_cp_cfg_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::gnb_cu_cp_e1_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::gnb_cu_cp_e1_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::gnb_cu_cp_e1_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::gnb_cu_cp_meas_results_info_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::gnb_cu_up_cfg_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::gnb_cu_up_cfg_upd_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::gnb_cu_up_cfg_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::gnb_cu_up_counter_check_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::gnb_cu_up_e1_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::gnb_cu_up_e1_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::gnb_cu_up_e1_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::gnb_cu_up_status_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::iab_up_tnl_address_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::iab_up_tnl_address_upd_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::iab_up_tnl_address_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::iabpsk_notif_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::mrdc_data_usage_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::reset_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::reset_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::res_status_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::res_status_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::res_status_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::res_status_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::trace_start_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::ul_data_notif_ies_o>;
