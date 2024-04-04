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
 *                       3GPP TS ASN1 E2AP v03.00 (2023)
 *
 ******************************************************************************/

#pragma once

#include "srsran/asn1/asn1_ap_utils.h"

namespace asn1 {
namespace e2ap {

/*******************************************************************************
 *                             Constant Definitions
 ******************************************************************************/

#define ASN1_E2AP_ID_E2SETUP 1
#define ASN1_E2AP_ID_ERROR_IND 2
#define ASN1_E2AP_ID_RESET 3
#define ASN1_E2AP_ID_RIC_CTRL 4
#define ASN1_E2AP_ID_RIC_IND 5
#define ASN1_E2AP_ID_RIC_SERVICE_QUERY 6
#define ASN1_E2AP_ID_RIC_SERVICE_UPD 7
#define ASN1_E2AP_ID_RIC_SUB 8
#define ASN1_E2AP_ID_RIC_SUB_DELETE 9
#define ASN1_E2AP_ID_E2NODE_CFG_UPD 10
#define ASN1_E2AP_ID_E2CONN_UPD 11
#define ASN1_E2AP_ID_RIC_SUB_DELETE_REQUIRED 12
#define ASN1_E2AP_ID_E2REMOVAL 13
#define ASN1_E2AP_ID_RIC_SUB_MOD 14
#define ASN1_E2AP_ID_RIC_SUB_MOD_REQUIRED 15
#define ASN1_E2AP_ID_RIC_QUERY 16
#define ASN1_E2AP_MAX_PROTOCOL_IES 65535
#define ASN1_E2AP_MAXNOOF_ERRORS 256
#define ASN1_E2AP_MAXOF_E2NODE_COMPONENTS 1024
#define ASN1_E2AP_MAXOF_RAN_FUNCTION_ID 256
#define ASN1_E2AP_MAXOF_RIC_ACTION_ID 16
#define ASN1_E2AP_MAXOF_TNL_A 32
#define ASN1_E2AP_MAXOF_RIC_REQUEST_ID 1024
#define ASN1_E2AP_ID_CAUSE 1
#define ASN1_E2AP_ID_CRIT_DIAGNOSTICS 2
#define ASN1_E2AP_ID_GLOBAL_E2NODE_ID 3
#define ASN1_E2AP_ID_GLOBAL_RIC_ID 4
#define ASN1_E2AP_ID_RAN_FUNCTION_ID 5
#define ASN1_E2AP_ID_RAN_FUNCTION_ID_ITEM 6
#define ASN1_E2AP_ID_RAN_FUNCTION_I_ECAUSE_ITEM 7
#define ASN1_E2AP_ID_RAN_FUNCTION_ITEM 8
#define ASN1_E2AP_ID_RAN_FUNCTIONS_ACCEPTED 9
#define ASN1_E2AP_ID_RAN_FUNCTIONS_ADDED 10
#define ASN1_E2AP_ID_RAN_FUNCTIONS_DELETED 11
#define ASN1_E2AP_ID_RAN_FUNCTIONS_MODIFIED 12
#define ASN1_E2AP_ID_RAN_FUNCTIONS_REJECTED 13
#define ASN1_E2AP_ID_RIC_ACTION_ADMITTED_ITEM 14
#define ASN1_E2AP_ID_RIC_ACTION_ID 15
#define ASN1_E2AP_ID_RIC_ACTION_NOT_ADMITTED_ITEM 16
#define ASN1_E2AP_ID_RIC_ACTIONS_ADMITTED 17
#define ASN1_E2AP_ID_RIC_ACTIONS_NOT_ADMITTED 18
#define ASN1_E2AP_ID_RIC_ACTION_TO_BE_SETUP_ITEM 19
#define ASN1_E2AP_ID_RIC_CALL_PROCESS_ID 20
#define ASN1_E2AP_ID_RIC_CTRL_ACK_REQUEST 21
#define ASN1_E2AP_ID_RIC_CTRL_HDR 22
#define ASN1_E2AP_ID_RIC_CTRL_MSG 23
#define ASN1_E2AP_ID_RIC_CTRL_STATUS 24
#define ASN1_E2AP_ID_RIC_IND_HDR 25
#define ASN1_E2AP_ID_RIC_IND_MSG 26
#define ASN1_E2AP_ID_RIC_IND_SN 27
#define ASN1_E2AP_ID_RIC_IND_TYPE 28
#define ASN1_E2AP_ID_RIC_REQUEST_ID 29
#define ASN1_E2AP_ID_RIC_SUB_DETAILS 30
#define ASN1_E2AP_ID_TIME_TO_WAIT 31
#define ASN1_E2AP_ID_RIC_CTRL_OUTCOME 32
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_UPD 33
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_UPD_ITEM 34
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_UPD_ACK 35
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_UPD_ACK_ITEM 36
#define ASN1_E2AP_ID_E2CONN_SETUP 39
#define ASN1_E2AP_ID_E2CONN_SETUP_FAILED 40
#define ASN1_E2AP_ID_E2CONN_SETUP_FAILED_ITEM 41
#define ASN1_E2AP_ID_E2CONN_FAILED_ITEM 42
#define ASN1_E2AP_ID_E2CONN_UPD_ITEM 43
#define ASN1_E2AP_ID_E2CONN_UPD_ADD 44
#define ASN1_E2AP_ID_E2CONN_UPD_MODIFY 45
#define ASN1_E2AP_ID_E2CONN_UPD_REM 46
#define ASN1_E2AP_ID_E2CONN_UPD_REM_ITEM 47
#define ASN1_E2AP_ID_TNL_INFO 48
#define ASN1_E2AP_ID_TRANSACTION_ID 49
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_ADDITION 50
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_ADDITION_ITEM 51
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_ADDITION_ACK 52
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_ADDITION_ACK_ITEM 53
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_REMOVAL 54
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_REMOVAL_ITEM 55
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_REMOVAL_ACK 56
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_REMOVAL_ACK_ITEM 57
#define ASN1_E2AP_ID_E2NODE_TNL_ASSOC_REMOVAL 58
#define ASN1_E2AP_ID_E2NODE_TNL_ASSOC_REMOVAL_ITEM 59
#define ASN1_E2AP_ID_RIC_SUB_TO_BE_REMD 60
#define ASN1_E2AP_ID_RIC_SUB_WITH_CAUSE_ITEM 61
#define ASN1_E2AP_ID_RIC_SUB_START_TIME 62
#define ASN1_E2AP_ID_RIC_SUB_END_TIME 63
#define ASN1_E2AP_ID_RIC_EVENT_TRIGGER_DEFINITION_TO_BE_MODIFIED 64
#define ASN1_E2AP_ID_RIC_ACTIONS_TO_BE_REMD_FOR_MOD_LIST 65
#define ASN1_E2AP_ID_RIC_ACTION_TO_BE_REMD_FOR_MOD_ITEM 66
#define ASN1_E2AP_ID_RIC_ACTIONS_TO_BE_MODIFIED_FOR_MOD_LIST 67
#define ASN1_E2AP_ID_RIC_ACTION_TO_BE_MODIFIED_FOR_MOD_ITEM 68
#define ASN1_E2AP_ID_RIC_ACTIONS_TO_BE_ADDED_FOR_MOD_LIST 69
#define ASN1_E2AP_ID_RIC_ACTION_TO_BE_ADDED_FOR_MOD_ITEM 70
#define ASN1_E2AP_ID_RIC_ACTIONS_REMD_FOR_MOD_LIST 71
#define ASN1_E2AP_ID_RIC_ACTION_REMD_FOR_MOD_ITEM 72
#define ASN1_E2AP_ID_RIC_ACTIONS_FAILED_TO_BE_REMD_FOR_MOD_LIST 73
#define ASN1_E2AP_ID_RIC_ACTION_FAILED_TO_BE_REMD_FOR_MOD_ITEM 74
#define ASN1_E2AP_ID_RIC_ACTIONS_MODIFIED_FOR_MOD_LIST 75
#define ASN1_E2AP_ID_RIC_ACTION_MODIFIED_FOR_MOD_ITEM 76
#define ASN1_E2AP_ID_RIC_ACTIONS_FAILED_TO_BE_MODIFIED_FOR_MOD_LIST 77
#define ASN1_E2AP_ID_RIC_ACTION_FAILED_TO_BE_MODIFIED_FOR_MOD_ITEM 78
#define ASN1_E2AP_ID_RIC_ACTIONS_ADDED_FOR_MOD_LIST 79
#define ASN1_E2AP_ID_RIC_ACTION_ADDED_FOR_MOD_ITEM 80
#define ASN1_E2AP_ID_RIC_ACTIONS_FAILED_TO_BE_ADDED_FOR_MOD_LIST 81
#define ASN1_E2AP_ID_RIC_ACTION_FAILED_TO_BE_ADDED_FOR_MOD_ITEM 82
#define ASN1_E2AP_ID_RIC_ACTIONS_REQUIRED_TO_BE_MODIFIED_LIST 83
#define ASN1_E2AP_ID_RIC_ACTION_REQUIRED_TO_BE_MODIFIED_ITEM 84
#define ASN1_E2AP_ID_RIC_ACTIONS_REQUIRED_TO_BE_REMD_LIST 85
#define ASN1_E2AP_ID_RIC_ACTION_REQUIRED_TO_BE_REMD_ITEM 86
#define ASN1_E2AP_ID_RIC_ACTIONS_CONFIRMED_FOR_MOD_LIST 87
#define ASN1_E2AP_ID_RIC_ACTION_CONFIRMED_FOR_MOD_ITEM 88
#define ASN1_E2AP_ID_RIC_ACTIONS_REFUSED_TO_BE_MODIFIED_LIST 89
#define ASN1_E2AP_ID_RIC_ACTION_REFUSED_TO_BE_MODIFIED_ITEM 90
#define ASN1_E2AP_ID_RIC_ACTIONS_CONFIRMED_FOR_REMOVAL_LIST 91
#define ASN1_E2AP_ID_RIC_ACTION_CONFIRMED_FOR_REMOVAL_ITEM 92
#define ASN1_E2AP_ID_RIC_ACTIONS_REFUSED_TO_BE_REMD_LIST 93
#define ASN1_E2AP_ID_RIC_ACTION_REFUSED_TO_BE_REMD_ITEM 94
#define ASN1_E2AP_ID_RIC_QUERY_HDR 95
#define ASN1_E2AP_ID_RIC_QUERY_DEFINITION 96
#define ASN1_E2AP_ID_RIC_QUERY_OUTCOME 97

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// ENB-ID-Choice ::= CHOICE
struct enb_id_choice_c {
  struct types_opts {
    enum options { enb_id_macro, enb_id_shortmacro, enb_id_longmacro, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  enb_id_choice_c() = default;
  enb_id_choice_c(const enb_id_choice_c& other);
  enb_id_choice_c& operator=(const enb_id_choice_c& other);
  ~enb_id_choice_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<20, false, true>& enb_id_macro()
  {
    assert_choice_type(types::enb_id_macro, type_, "ENB-ID-Choice");
    return c.get<fixed_bitstring<20, false, true>>();
  }
  fixed_bitstring<18, false, true>& enb_id_shortmacro()
  {
    assert_choice_type(types::enb_id_shortmacro, type_, "ENB-ID-Choice");
    return c.get<fixed_bitstring<18, false, true>>();
  }
  fixed_bitstring<21, false, true>& enb_id_longmacro()
  {
    assert_choice_type(types::enb_id_longmacro, type_, "ENB-ID-Choice");
    return c.get<fixed_bitstring<21, false, true>>();
  }
  const fixed_bitstring<20, false, true>& enb_id_macro() const
  {
    assert_choice_type(types::enb_id_macro, type_, "ENB-ID-Choice");
    return c.get<fixed_bitstring<20, false, true>>();
  }
  const fixed_bitstring<18, false, true>& enb_id_shortmacro() const
  {
    assert_choice_type(types::enb_id_shortmacro, type_, "ENB-ID-Choice");
    return c.get<fixed_bitstring<18, false, true>>();
  }
  const fixed_bitstring<21, false, true>& enb_id_longmacro() const
  {
    assert_choice_type(types::enb_id_longmacro, type_, "ENB-ID-Choice");
    return c.get<fixed_bitstring<21, false, true>>();
  }
  fixed_bitstring<20, false, true>& set_enb_id_macro();
  fixed_bitstring<18, false, true>& set_enb_id_shortmacro();
  fixed_bitstring<21, false, true>& set_enb_id_longmacro();

private:
  types                                             type_;
  choice_buffer_t<fixed_bitstring<21, false, true>> c;

  void destroy_();
};

// GNB-ID-Choice ::= CHOICE
struct gnb_id_choice_c {
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

// ENGNB-ID ::= CHOICE
struct en_gnb_id_c {
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

// GlobalgNB-ID ::= SEQUENCE
struct global_gnb_id_s {
  bool                     ext = false;
  fixed_octstring<3, true> plmn_id;
  gnb_id_choice_c          gnb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalngeNB-ID ::= SEQUENCE
struct globalng_enb_id_s {
  bool                     ext = false;
  fixed_octstring<3, true> plmn_id;
  enb_id_choice_c          enb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CauseE2node ::= ENUMERATED
struct cause_e2node_opts {
  enum options { e2node_component_unknown, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using cause_e2node_e = enumerated<cause_e2node_opts, true>;

// CauseMisc ::= ENUMERATED
struct cause_misc_opts {
  enum options { ctrl_processing_overload, hardware_fail, om_intervention, unspecified, /*...*/ nulltype } value;

  const char* to_string() const;
};
using cause_misc_e = enumerated<cause_misc_opts, true>;

// CauseProtocol ::= ENUMERATED
struct cause_protocol_opts {
  enum options {
    transfer_syntax_error,
    abstract_syntax_error_reject,
    abstract_syntax_error_ignore_and_notify,
    msg_not_compatible_with_receiver_state,
    semantic_error,
    abstract_syntax_error_falsely_constructed_msg,
    unspecified,
    // ...
    nulltype
  } value;

  const char* to_string() const;
};
using cause_protocol_e = enumerated<cause_protocol_opts, true>;

// CauseRICrequest ::= ENUMERATED
struct cause_ric_request_opts {
  enum options {
    ran_function_id_invalid,
    action_not_supported,
    excessive_actions,
    duplicate_action,
    duplicate_event_trigger,
    function_res_limit,
    request_id_unknown,
    inconsistent_action_subsequent_action_seq,
    ctrl_msg_invalid,
    ric_call_process_id_invalid,
    ctrl_timer_expired,
    ctrl_failed_to_execute,
    sys_not_ready,
    unspecified,
    // ...
    ric_sub_end_time_expired,
    ric_sub_end_time_invalid,
    duplicate_ric_request_id,
    event_trigger_not_supported,
    requested_info_unavailable,
    invalid_info_request,
    nulltype
  } value;

  const char* to_string() const;
};
using cause_ric_request_e = enumerated<cause_ric_request_opts, true, 6>;

// CauseRICservice ::= ENUMERATED
struct cause_ric_service_opts {
  enum options { ran_function_not_supported, excessive_functions, ric_res_limit, /*...*/ nulltype } value;

  const char* to_string() const;
};
using cause_ric_service_e = enumerated<cause_ric_service_opts, true>;

// CauseTransport ::= ENUMERATED
struct cause_transport_opts {
  enum options { unspecified, transport_res_unavailable, /*...*/ nulltype } value;

  const char* to_string() const;
};
using cause_transport_e = enumerated<cause_transport_opts, true>;

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

// GlobalNG-RANNode-ID ::= CHOICE
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
    assert_choice_type(types::gnb, type_, "GlobalNG-RANNode-ID");
    return c.get<global_gnb_id_s>();
  }
  globalng_enb_id_s& ng_enb()
  {
    assert_choice_type(types::ng_enb, type_, "GlobalNG-RANNode-ID");
    return c.get<globalng_enb_id_s>();
  }
  const global_gnb_id_s& gnb() const
  {
    assert_choice_type(types::gnb, type_, "GlobalNG-RANNode-ID");
    return c.get<global_gnb_id_s>();
  }
  const globalng_enb_id_s& ng_enb() const
  {
    assert_choice_type(types::ng_enb, type_, "GlobalNG-RANNode-ID");
    return c.get<globalng_enb_id_s>();
  }
  global_gnb_id_s&   set_gnb();
  globalng_enb_id_s& set_ng_enb();

private:
  types                                               type_;
  choice_buffer_t<global_gnb_id_s, globalng_enb_id_s> c;

  void destroy_();
};

// GlobalenGNB-ID ::= SEQUENCE
struct globalen_gnb_id_s {
  bool                     ext = false;
  fixed_octstring<3, true> plmn_id;
  en_gnb_id_c              gnb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICsubsequentActionType ::= ENUMERATED
struct ric_subsequent_action_type_opts {
  enum options { continue_value, wait, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ric_subsequent_action_type_e = enumerated<ric_subsequent_action_type_opts, true>;

// RICtimeToWait ::= ENUMERATED
struct ric_time_to_wait_opts {
  enum options {
    w1ms,
    w2ms,
    w5ms,
    w10ms,
    w20ms,
    w30ms,
    w40ms,
    w50ms,
    w100ms,
    w200ms,
    w500ms,
    w1s,
    w2s,
    w5s,
    w10s,
    w20s,
    w60s,
    // ...
    nulltype
  } value;

  const char* to_string() const;
};
using ric_time_to_wait_e = enumerated<ric_time_to_wait_opts, true>;

// Cause ::= CHOICE
struct cause_c {
  struct types_opts {
    enum options { ric_request, ric_service, e2_node, transport, protocol, misc, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  cause_c() = default;
  cause_c(const cause_c& other);
  cause_c& operator=(const cause_c& other);
  ~cause_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cause_ric_request_e& ric_request()
  {
    assert_choice_type(types::ric_request, type_, "Cause");
    return c.get<cause_ric_request_e>();
  }
  cause_ric_service_e& ric_service()
  {
    assert_choice_type(types::ric_service, type_, "Cause");
    return c.get<cause_ric_service_e>();
  }
  cause_e2node_e& e2_node()
  {
    assert_choice_type(types::e2_node, type_, "Cause");
    return c.get<cause_e2node_e>();
  }
  cause_transport_e& transport()
  {
    assert_choice_type(types::transport, type_, "Cause");
    return c.get<cause_transport_e>();
  }
  cause_protocol_e& protocol()
  {
    assert_choice_type(types::protocol, type_, "Cause");
    return c.get<cause_protocol_e>();
  }
  cause_misc_e& misc()
  {
    assert_choice_type(types::misc, type_, "Cause");
    return c.get<cause_misc_e>();
  }
  const cause_ric_request_e& ric_request() const
  {
    assert_choice_type(types::ric_request, type_, "Cause");
    return c.get<cause_ric_request_e>();
  }
  const cause_ric_service_e& ric_service() const
  {
    assert_choice_type(types::ric_service, type_, "Cause");
    return c.get<cause_ric_service_e>();
  }
  const cause_e2node_e& e2_node() const
  {
    assert_choice_type(types::e2_node, type_, "Cause");
    return c.get<cause_e2node_e>();
  }
  const cause_transport_e& transport() const
  {
    assert_choice_type(types::transport, type_, "Cause");
    return c.get<cause_transport_e>();
  }
  const cause_protocol_e& protocol() const
  {
    assert_choice_type(types::protocol, type_, "Cause");
    return c.get<cause_protocol_e>();
  }
  const cause_misc_e& misc() const
  {
    assert_choice_type(types::misc, type_, "Cause");
    return c.get<cause_misc_e>();
  }
  cause_ric_request_e& set_ric_request();
  cause_ric_service_e& set_ric_service();
  cause_e2node_e&      set_e2_node();
  cause_transport_e&   set_transport();
  cause_protocol_e&    set_protocol();
  cause_misc_e&        set_misc();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// E2nodeComponentInterfaceE1 ::= SEQUENCE
struct e2node_component_interface_e1_s {
  bool     ext          = false;
  uint64_t gnb_cu_up_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentInterfaceF1 ::= SEQUENCE
struct e2node_component_interface_f1_s {
  bool     ext       = false;
  uint64_t gnb_du_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentInterfaceNG ::= SEQUENCE
struct e2node_component_interface_ng_s {
  bool                                 ext = false;
  printable_string<1, 150, true, true> amf_name;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentInterfaceS1 ::= SEQUENCE
struct e2node_component_interface_s1_s {
  bool                                 ext = false;
  printable_string<1, 150, true, true> mme_name;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentInterfaceW1 ::= SEQUENCE
struct e2node_component_interface_w1_s {
  bool     ext          = false;
  uint64_t ng_enb_du_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentInterfaceX2 ::= SEQUENCE
struct e2node_component_interface_x2_s {
  bool              ext                      = false;
  bool              global_enb_id_present    = false;
  bool              global_en_gnb_id_present = false;
  global_enb_id_s   global_enb_id;
  globalen_gnb_id_s global_en_gnb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentInterfaceXn ::= SEQUENCE
struct e2node_component_interface_xn_s {
  bool                    ext = false;
  global_ng_ran_node_id_c global_ng_ran_node_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICactionType ::= ENUMERATED
struct ric_action_type_opts {
  enum options { report, insert, policy, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ric_action_type_e = enumerated<ric_action_type_opts, true>;

// RICsubsequentAction ::= SEQUENCE
struct ric_subsequent_action_s {
  bool                         ext = false;
  ric_subsequent_action_type_e ric_subsequent_action_type;
  ric_time_to_wait_e           ric_time_to_wait;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentConfigurationAck ::= SEQUENCE
struct e2node_component_cfg_ack_s {
  struct upd_outcome_opts {
    enum options { success, fail, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using upd_outcome_e_ = enumerated<upd_outcome_opts, true>;

  // member variables
  bool           ext                = false;
  bool           fail_cause_present = false;
  upd_outcome_e_ upd_outcome;
  cause_c        fail_cause;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentConfiguration ::= SEQUENCE
struct e2node_component_cfg_s {
  bool                      ext = false;
  unbounded_octstring<true> e2node_component_request_part;
  unbounded_octstring<true> e2node_component_resp_part;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentID ::= CHOICE
struct e2node_component_id_c {
  struct types_opts {
    enum options {
      e2node_component_interface_type_ng,
      e2node_component_interface_type_xn,
      e2node_component_interface_type_e1,
      e2node_component_interface_type_f1,
      e2node_component_interface_type_w1,
      e2node_component_interface_type_s1,
      e2node_component_interface_type_x2,
      // ...
      nulltype
    } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  e2node_component_id_c() = default;
  e2node_component_id_c(const e2node_component_id_c& other);
  e2node_component_id_c& operator=(const e2node_component_id_c& other);
  ~e2node_component_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  e2node_component_interface_ng_s& e2node_component_interface_type_ng()
  {
    assert_choice_type(types::e2node_component_interface_type_ng, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_ng_s>();
  }
  e2node_component_interface_xn_s& e2node_component_interface_type_xn()
  {
    assert_choice_type(types::e2node_component_interface_type_xn, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_xn_s>();
  }
  e2node_component_interface_e1_s& e2node_component_interface_type_e1()
  {
    assert_choice_type(types::e2node_component_interface_type_e1, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_e1_s>();
  }
  e2node_component_interface_f1_s& e2node_component_interface_type_f1()
  {
    assert_choice_type(types::e2node_component_interface_type_f1, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_f1_s>();
  }
  e2node_component_interface_w1_s& e2node_component_interface_type_w1()
  {
    assert_choice_type(types::e2node_component_interface_type_w1, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_w1_s>();
  }
  e2node_component_interface_s1_s& e2node_component_interface_type_s1()
  {
    assert_choice_type(types::e2node_component_interface_type_s1, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_s1_s>();
  }
  e2node_component_interface_x2_s& e2node_component_interface_type_x2()
  {
    assert_choice_type(types::e2node_component_interface_type_x2, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_x2_s>();
  }
  const e2node_component_interface_ng_s& e2node_component_interface_type_ng() const
  {
    assert_choice_type(types::e2node_component_interface_type_ng, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_ng_s>();
  }
  const e2node_component_interface_xn_s& e2node_component_interface_type_xn() const
  {
    assert_choice_type(types::e2node_component_interface_type_xn, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_xn_s>();
  }
  const e2node_component_interface_e1_s& e2node_component_interface_type_e1() const
  {
    assert_choice_type(types::e2node_component_interface_type_e1, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_e1_s>();
  }
  const e2node_component_interface_f1_s& e2node_component_interface_type_f1() const
  {
    assert_choice_type(types::e2node_component_interface_type_f1, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_f1_s>();
  }
  const e2node_component_interface_w1_s& e2node_component_interface_type_w1() const
  {
    assert_choice_type(types::e2node_component_interface_type_w1, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_w1_s>();
  }
  const e2node_component_interface_s1_s& e2node_component_interface_type_s1() const
  {
    assert_choice_type(types::e2node_component_interface_type_s1, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_s1_s>();
  }
  const e2node_component_interface_x2_s& e2node_component_interface_type_x2() const
  {
    assert_choice_type(types::e2node_component_interface_type_x2, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_x2_s>();
  }
  e2node_component_interface_ng_s& set_e2node_component_interface_type_ng();
  e2node_component_interface_xn_s& set_e2node_component_interface_type_xn();
  e2node_component_interface_e1_s& set_e2node_component_interface_type_e1();
  e2node_component_interface_f1_s& set_e2node_component_interface_type_f1();
  e2node_component_interface_w1_s& set_e2node_component_interface_type_w1();
  e2node_component_interface_s1_s& set_e2node_component_interface_type_s1();
  e2node_component_interface_x2_s& set_e2node_component_interface_type_x2();

private:
  types type_;
  choice_buffer_t<e2node_component_interface_e1_s,
                  e2node_component_interface_f1_s,
                  e2node_component_interface_ng_s,
                  e2node_component_interface_s1_s,
                  e2node_component_interface_w1_s,
                  e2node_component_interface_x2_s,
                  e2node_component_interface_xn_s>
      c;

  void destroy_();
};

// E2nodeComponentInterfaceType ::= ENUMERATED
struct e2node_component_interface_type_opts {
  enum options { ng, xn, e1, f1, w1, s1, x2, /*...*/ nulltype } value;

  const char* to_string() const;
};
using e2node_component_interface_type_e = enumerated<e2node_component_interface_type_opts, true>;

// RICaction-ToBeSetup-Item ::= SEQUENCE
struct ric_action_to_be_setup_item_s {
  bool                      ext                           = false;
  bool                      ric_subsequent_action_present = false;
  uint16_t                  ric_action_id                 = 0;
  ric_action_type_e         ric_action_type;
  unbounded_octstring<true> ric_action_definition;
  ric_subsequent_action_s   ric_subsequent_action;
  // ...
  bool     ric_action_execution_order_present = false;
  uint16_t ric_action_execution_order         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TNLinformation ::= SEQUENCE
struct tnl_info_s {
  bool                                  ext              = false;
  bool                                  tnl_port_present = false;
  bounded_bitstring<1, 160, true, true> tnl_address;
  fixed_bitstring<16, false, true>      tnl_port;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TNLusage ::= ENUMERATED
struct tnl_usage_opts {
  enum options { ric_service, support_function, both, /*...*/ nulltype } value;

  const char* to_string() const;
};
using tnl_usage_e = enumerated<tnl_usage_opts, true>;

// E2connectionSetupFailed-Item ::= SEQUENCE
struct e2conn_setup_failed_item_s {
  bool       ext = false;
  tnl_info_s tnl_info;
  cause_c    cause;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2connectionUpdate-Item ::= SEQUENCE
struct e2conn_upd_item_s {
  bool        ext = false;
  tnl_info_s  tnl_info;
  tnl_usage_e tnl_usage;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2connectionUpdateRemove-Item ::= SEQUENCE
struct e2conn_upd_rem_item_s {
  bool       ext = false;
  tnl_info_s tnl_info;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentConfigAdditionAck-Item ::= SEQUENCE
struct e2node_component_cfg_addition_ack_item_s {
  bool                              ext = false;
  e2node_component_interface_type_e e2node_component_interface_type;
  e2node_component_id_c             e2node_component_id;
  e2node_component_cfg_ack_s        e2node_component_cfg_ack;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentConfigAddition-Item ::= SEQUENCE
struct e2node_component_cfg_addition_item_s {
  bool                              ext = false;
  e2node_component_interface_type_e e2node_component_interface_type;
  e2node_component_id_c             e2node_component_id;
  e2node_component_cfg_s            e2node_component_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentConfigRemovalAck-Item ::= SEQUENCE
struct e2node_component_cfg_removal_ack_item_s {
  bool                              ext = false;
  e2node_component_interface_type_e e2node_component_interface_type;
  e2node_component_id_c             e2node_component_id;
  e2node_component_cfg_ack_s        e2node_component_cfg_ack;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentConfigRemoval-Item ::= SEQUENCE
struct e2node_component_cfg_removal_item_s {
  bool                              ext = false;
  e2node_component_interface_type_e e2node_component_interface_type;
  e2node_component_id_c             e2node_component_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentConfigUpdateAck-Item ::= SEQUENCE
struct e2node_component_cfg_upd_ack_item_s {
  bool                              ext = false;
  e2node_component_interface_type_e e2node_component_interface_type;
  e2node_component_id_c             e2node_component_id;
  e2node_component_cfg_ack_s        e2node_component_cfg_ack;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentConfigUpdate-Item ::= SEQUENCE
struct e2node_component_cfg_upd_item_s {
  bool                              ext = false;
  e2node_component_interface_type_e e2node_component_interface_type;
  e2node_component_id_c             e2node_component_id;
  e2node_component_cfg_s            e2node_component_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeTNLassociationRemoval-Item ::= SEQUENCE
struct e2node_tnl_assoc_removal_item_s {
  bool       ext = false;
  tnl_info_s tnl_info;
  tnl_info_s tnl_info_ric;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANfunctionIDcause-Item ::= SEQUENCE
struct ran_function_id_cause_item_s {
  bool     ext             = false;
  uint16_t ran_function_id = 0;
  cause_c  cause;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANfunctionID-Item ::= SEQUENCE
struct ran_function_id_item_s {
  bool     ext                   = false;
  uint16_t ran_function_id       = 0;
  uint16_t ran_function_revision = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANfunction-Item ::= SEQUENCE
struct ran_function_item_s {
  bool                                  ext             = false;
  uint16_t                              ran_function_id = 0;
  unbounded_octstring<true>             ran_function_definition;
  uint16_t                              ran_function_revision = 0;
  printable_string<1, 1000, true, true> ran_function_o_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-AddedForModification-Item ::= SEQUENCE
struct ric_action_added_for_mod_item_s {
  bool     ext           = false;
  uint16_t ric_action_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-Admitted-Item ::= SEQUENCE
struct ric_action_admitted_item_s {
  bool     ext           = false;
  uint16_t ric_action_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-ConfirmedForModification-Item ::= SEQUENCE
struct ric_action_confirmed_for_mod_item_s {
  bool     ext           = false;
  uint16_t ric_action_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-ConfirmedForRemoval-Item ::= SEQUENCE
struct ric_action_confirmed_for_removal_item_s {
  bool     ext           = false;
  uint16_t ric_action_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-FailedToBeAddedForModification-Item ::= SEQUENCE
struct ric_action_failed_to_be_added_for_mod_item_s {
  bool     ext           = false;
  uint16_t ric_action_id = 0;
  cause_c  cause;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-FailedToBeModifiedForModification-Item ::= SEQUENCE
struct ric_action_failed_to_be_modified_for_mod_item_s {
  bool     ext           = false;
  uint16_t ric_action_id = 0;
  cause_c  cause;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-FailedToBeRemovedForModification-Item ::= SEQUENCE
struct ric_action_failed_to_be_remd_for_mod_item_s {
  bool     ext           = false;
  uint16_t ric_action_id = 0;
  cause_c  cause;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-ModifiedForModification-Item ::= SEQUENCE
struct ric_action_modified_for_mod_item_s {
  bool     ext           = false;
  uint16_t ric_action_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-NotAdmitted-Item ::= SEQUENCE
struct ric_action_not_admitted_item_s {
  bool     ext           = false;
  uint16_t ric_action_id = 0;
  cause_c  cause;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-RefusedToBeModified-Item ::= SEQUENCE
struct ric_action_refused_to_be_modified_item_s {
  bool     ext           = false;
  uint16_t ric_action_id = 0;
  cause_c  cause;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-RefusedToBeRemoved-Item ::= SEQUENCE
struct ric_action_refused_to_be_remd_item_s {
  bool     ext           = false;
  uint16_t ric_action_id = 0;
  cause_c  cause;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-RemovedForModification-Item ::= SEQUENCE
struct ric_action_remd_for_mod_item_s {
  bool     ext           = false;
  uint16_t ric_action_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-RequiredToBeModified-Item ::= SEQUENCE
struct ric_action_required_to_be_modified_item_s {
  bool               ext           = false;
  uint16_t           ric_action_id = 0;
  ric_time_to_wait_e ric_time_to_wait;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-RequiredToBeRemoved-Item ::= SEQUENCE
struct ric_action_required_to_be_remd_item_s {
  bool     ext           = false;
  uint16_t ric_action_id = 0;
  cause_c  cause;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-ToBeAddedForModification-Item ::= SEQUENCE
struct ric_action_to_be_added_for_mod_item_s {
  bool                      ext                           = false;
  bool                      ric_subsequent_action_present = false;
  uint16_t                  ric_action_id                 = 0;
  ric_action_type_e         ric_action_type;
  unbounded_octstring<true> ric_action_definition;
  uint16_t                  ric_action_execution_order = 0;
  ric_subsequent_action_s   ric_subsequent_action;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-ToBeModifiedForModification-Item ::= SEQUENCE
struct ric_action_to_be_modified_for_mod_item_s {
  bool                      ext                                = false;
  bool                      ric_action_execution_order_present = false;
  bool                      ric_subsequent_action_present      = false;
  uint16_t                  ric_action_id                      = 0;
  unbounded_octstring<true> ric_action_definition;
  uint16_t                  ric_action_execution_order = 0;
  ric_subsequent_action_s   ric_subsequent_action;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-ToBeRemovedForModification-Item ::= SEQUENCE
struct ric_action_to_be_remd_for_mod_item_s {
  bool     ext           = false;
  uint16_t ric_action_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-ToBeSetup-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_action_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_action_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_action_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_action_to_be_setup_item_s&       ric_action_to_be_setup_item() { return c; }
    const ric_action_to_be_setup_item_s& ric_action_to_be_setup_item() const { return c; }

  private:
    ric_action_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICrequestID ::= SEQUENCE
struct ric_request_id_s {
  bool     ext              = false;
  uint32_t ric_requestor_id = 0;
  uint32_t ric_instance_id  = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TypeOfError ::= ENUMERATED
struct type_of_error_opts {
  enum options { not_understood, missing, /*...*/ nulltype } value;

  const char* to_string() const;
};
using type_of_error_e = enumerated<type_of_error_opts, true>;

// CriticalityDiagnostics-IE-Item ::= SEQUENCE
struct crit_diagnostics_ie_item_s {
  bool            ext = false;
  crit_e          ie_crit;
  uint32_t        ie_id = 0;
  type_of_error_e type_of_error;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2connectionSetupFailed-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2conn_setup_failed_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2conn_setup_failed_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::e2conn_setup_failed_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2conn_setup_failed_item_s&       e2conn_setup_failed_item() { return c; }
    const e2conn_setup_failed_item_s& e2conn_setup_failed_item() const { return c; }

  private:
    e2conn_setup_failed_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2connectionUpdate-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2conn_upd_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2conn_upd_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::e2conn_upd_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2conn_upd_item_s&       e2conn_upd_item() { return c; }
    const e2conn_upd_item_s& e2conn_upd_item() const { return c; }

  private:
    e2conn_upd_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2connectionUpdateRemove-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2conn_upd_rem_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2conn_upd_rem_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::e2conn_upd_rem_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2conn_upd_rem_item_s&       e2conn_upd_rem_item() { return c; }
    const e2conn_upd_rem_item_s& e2conn_upd_rem_item() const { return c; }

  private:
    e2conn_upd_rem_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2nodeComponentConfigAdditionAck-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_component_cfg_addition_ack_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2node_component_cfg_addition_ack_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::e2node_component_cfg_addition_ack_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2node_component_cfg_addition_ack_item_s&       e2node_component_cfg_addition_ack_item() { return c; }
    const e2node_component_cfg_addition_ack_item_s& e2node_component_cfg_addition_ack_item() const { return c; }

  private:
    e2node_component_cfg_addition_ack_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2nodeComponentConfigAddition-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_component_cfg_addition_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2node_component_cfg_addition_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::e2node_component_cfg_addition_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2node_component_cfg_addition_item_s&       e2node_component_cfg_addition_item() { return c; }
    const e2node_component_cfg_addition_item_s& e2node_component_cfg_addition_item() const { return c; }

  private:
    e2node_component_cfg_addition_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2nodeComponentConfigRemovalAck-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_component_cfg_removal_ack_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2node_component_cfg_removal_ack_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::e2node_component_cfg_removal_ack_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2node_component_cfg_removal_ack_item_s&       e2node_component_cfg_removal_ack_item() { return c; }
    const e2node_component_cfg_removal_ack_item_s& e2node_component_cfg_removal_ack_item() const { return c; }

  private:
    e2node_component_cfg_removal_ack_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2nodeComponentConfigRemoval-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_component_cfg_removal_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2node_component_cfg_removal_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::e2node_component_cfg_removal_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2node_component_cfg_removal_item_s&       e2node_component_cfg_removal_item() { return c; }
    const e2node_component_cfg_removal_item_s& e2node_component_cfg_removal_item() const { return c; }

  private:
    e2node_component_cfg_removal_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2nodeComponentConfigUpdateAck-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_component_cfg_upd_ack_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2node_component_cfg_upd_ack_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::e2node_component_cfg_upd_ack_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2node_component_cfg_upd_ack_item_s&       e2node_component_cfg_upd_ack_item() { return c; }
    const e2node_component_cfg_upd_ack_item_s& e2node_component_cfg_upd_ack_item() const { return c; }

  private:
    e2node_component_cfg_upd_ack_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2nodeComponentConfigUpdate-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_component_cfg_upd_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2node_component_cfg_upd_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::e2node_component_cfg_upd_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2node_component_cfg_upd_item_s&       e2node_component_cfg_upd_item() { return c; }
    const e2node_component_cfg_upd_item_s& e2node_component_cfg_upd_item() const { return c; }

  private:
    e2node_component_cfg_upd_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2nodeTNLassociationRemoval-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_tnl_assoc_removal_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2node_tnl_assoc_removal_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::e2node_tnl_assoc_removal_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2node_tnl_assoc_removal_item_s&       e2node_tnl_assoc_removal_item() { return c; }
    const e2node_tnl_assoc_removal_item_s& e2node_tnl_assoc_removal_item() const { return c; }

  private:
    e2node_tnl_assoc_removal_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RANfunctionIDcause-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ran_function_id_cause_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ran_function_i_ecause_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ran_function_i_ecause_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ran_function_id_cause_item_s&       ran_function_i_ecause_item() { return c; }
    const ran_function_id_cause_item_s& ran_function_i_ecause_item() const { return c; }

  private:
    ran_function_id_cause_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RANfunctionID-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ran_function_id_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ran_function_id_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ran_function_id_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ran_function_id_item_s&       ran_function_id_item() { return c; }
    const ran_function_id_item_s& ran_function_id_item() const { return c; }

  private:
    ran_function_id_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RANfunction-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ran_function_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ran_function_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ran_function_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ran_function_item_s&       ran_function_item() { return c; }
    const ran_function_item_s& ran_function_item() const { return c; }

  private:
    ran_function_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICaction-AddedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_action_added_for_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_action_added_for_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_action_added_for_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_action_added_for_mod_item_s&       ric_action_added_for_mod_item() { return c; }
    const ric_action_added_for_mod_item_s& ric_action_added_for_mod_item() const { return c; }

  private:
    ric_action_added_for_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICaction-Admitted-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_action_admitted_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_action_admitted_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_action_admitted_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_action_admitted_item_s&       ric_action_admitted_item() { return c; }
    const ric_action_admitted_item_s& ric_action_admitted_item() const { return c; }

  private:
    ric_action_admitted_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICaction-ConfirmedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_action_confirmed_for_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_action_confirmed_for_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_action_confirmed_for_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_action_confirmed_for_mod_item_s&       ric_action_confirmed_for_mod_item() { return c; }
    const ric_action_confirmed_for_mod_item_s& ric_action_confirmed_for_mod_item() const { return c; }

  private:
    ric_action_confirmed_for_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICaction-ConfirmedForRemoval-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_action_confirmed_for_removal_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_action_confirmed_for_removal_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_action_confirmed_for_removal_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_action_confirmed_for_removal_item_s&       ric_action_confirmed_for_removal_item() { return c; }
    const ric_action_confirmed_for_removal_item_s& ric_action_confirmed_for_removal_item() const { return c; }

  private:
    ric_action_confirmed_for_removal_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICaction-FailedToBeAddedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_action_failed_to_be_added_for_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_action_failed_to_be_added_for_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_action_failed_to_be_added_for_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_action_failed_to_be_added_for_mod_item_s&       ric_action_failed_to_be_added_for_mod_item() { return c; }
    const ric_action_failed_to_be_added_for_mod_item_s& ric_action_failed_to_be_added_for_mod_item() const { return c; }

  private:
    ric_action_failed_to_be_added_for_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICaction-FailedToBeModifiedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_action_failed_to_be_modified_for_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_action_failed_to_be_modified_for_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_action_failed_to_be_modified_for_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_action_failed_to_be_modified_for_mod_item_s&       ric_action_failed_to_be_modified_for_mod_item() { return c; }
    const ric_action_failed_to_be_modified_for_mod_item_s& ric_action_failed_to_be_modified_for_mod_item() const
    {
      return c;
    }

  private:
    ric_action_failed_to_be_modified_for_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICaction-FailedToBeRemovedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_action_failed_to_be_remd_for_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_action_failed_to_be_remd_for_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_action_failed_to_be_remd_for_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_action_failed_to_be_remd_for_mod_item_s&       ric_action_failed_to_be_remd_for_mod_item() { return c; }
    const ric_action_failed_to_be_remd_for_mod_item_s& ric_action_failed_to_be_remd_for_mod_item() const { return c; }

  private:
    ric_action_failed_to_be_remd_for_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICaction-ModifiedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_action_modified_for_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_action_modified_for_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_action_modified_for_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_action_modified_for_mod_item_s&       ric_action_modified_for_mod_item() { return c; }
    const ric_action_modified_for_mod_item_s& ric_action_modified_for_mod_item() const { return c; }

  private:
    ric_action_modified_for_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICaction-NotAdmitted-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_action_not_admitted_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_action_not_admitted_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_action_not_admitted_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_action_not_admitted_item_s&       ric_action_not_admitted_item() { return c; }
    const ric_action_not_admitted_item_s& ric_action_not_admitted_item() const { return c; }

  private:
    ric_action_not_admitted_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICaction-RefusedToBeModified-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_action_refused_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_action_refused_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_action_refused_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_action_refused_to_be_modified_item_s&       ric_action_refused_to_be_modified_item() { return c; }
    const ric_action_refused_to_be_modified_item_s& ric_action_refused_to_be_modified_item() const { return c; }

  private:
    ric_action_refused_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICaction-RefusedToBeRemoved-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_action_refused_to_be_remd_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_action_refused_to_be_remd_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_action_refused_to_be_remd_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_action_refused_to_be_remd_item_s&       ric_action_refused_to_be_remd_item() { return c; }
    const ric_action_refused_to_be_remd_item_s& ric_action_refused_to_be_remd_item() const { return c; }

  private:
    ric_action_refused_to_be_remd_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICaction-RemovedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_action_remd_for_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_action_remd_for_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_action_remd_for_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_action_remd_for_mod_item_s&       ric_action_remd_for_mod_item() { return c; }
    const ric_action_remd_for_mod_item_s& ric_action_remd_for_mod_item() const { return c; }

  private:
    ric_action_remd_for_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICaction-RequiredToBeModified-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_action_required_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_action_required_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_action_required_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_action_required_to_be_modified_item_s&       ric_action_required_to_be_modified_item() { return c; }
    const ric_action_required_to_be_modified_item_s& ric_action_required_to_be_modified_item() const { return c; }

  private:
    ric_action_required_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICaction-RequiredToBeRemoved-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_action_required_to_be_remd_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_action_required_to_be_remd_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_action_required_to_be_remd_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_action_required_to_be_remd_item_s&       ric_action_required_to_be_remd_item() { return c; }
    const ric_action_required_to_be_remd_item_s& ric_action_required_to_be_remd_item() const { return c; }

  private:
    ric_action_required_to_be_remd_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICaction-ToBeAddedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_action_to_be_added_for_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_action_to_be_added_for_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_action_to_be_added_for_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_action_to_be_added_for_mod_item_s&       ric_action_to_be_added_for_mod_item() { return c; }
    const ric_action_to_be_added_for_mod_item_s& ric_action_to_be_added_for_mod_item() const { return c; }

  private:
    ric_action_to_be_added_for_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICaction-ToBeModifiedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_action_to_be_modified_for_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_action_to_be_modified_for_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_action_to_be_modified_for_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_action_to_be_modified_for_mod_item_s&       ric_action_to_be_modified_for_mod_item() { return c; }
    const ric_action_to_be_modified_for_mod_item_s& ric_action_to_be_modified_for_mod_item() const { return c; }

  private:
    ric_action_to_be_modified_for_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICaction-ToBeRemovedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_action_to_be_remd_for_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_action_to_be_remd_for_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_action_to_be_remd_for_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_action_to_be_remd_for_mod_item_s&       ric_action_to_be_remd_for_mod_item() { return c; }
    const ric_action_to_be_remd_for_mod_item_s& ric_action_to_be_remd_for_mod_item() const { return c; }

  private:
    ric_action_to_be_remd_for_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICsubscription-withCause-Item ::= SEQUENCE
struct ric_sub_with_cause_item_s {
  bool             ext = false;
  ric_request_id_s ric_request_id;
  uint16_t         ran_function_id = 0;
  cause_c          cause;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CriticalityDiagnostics-IE-List ::= SEQUENCE (SIZE (1..256)) OF CriticalityDiagnostics-IE-Item
using crit_diagnostics_ie_list_l = dyn_array<crit_diagnostics_ie_item_s>;

// GlobalE2node-en-gNB-ID ::= SEQUENCE
struct global_e2node_en_gnb_id_s {
  bool              ext                     = false;
  bool              en_gnb_cu_up_id_present = false;
  bool              en_gnb_du_id_present    = false;
  globalen_gnb_id_s global_en_gnb_id;
  uint64_t          en_gnb_cu_up_id = 0;
  uint64_t          en_gnb_du_id    = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalE2node-eNB-ID ::= SEQUENCE
struct global_e2node_enb_id_s {
  bool            ext = false;
  global_enb_id_s global_enb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalE2node-gNB-ID ::= SEQUENCE
struct global_e2node_gnb_id_s {
  bool              ext                      = false;
  bool              global_en_gnb_id_present = false;
  bool              gnb_cu_up_id_present     = false;
  bool              gnb_du_id_present        = false;
  global_gnb_id_s   global_gnb_id;
  globalen_gnb_id_s global_en_gnb_id;
  uint64_t          gnb_cu_up_id = 0;
  uint64_t          gnb_du_id    = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalE2node-ng-eNB-ID ::= SEQUENCE
struct global_e2node_ng_enb_id_s {
  bool              ext                   = false;
  bool              global_enb_id_present = false;
  bool              ng_enb_du_id_present  = false;
  globalng_enb_id_s global_ng_enb_id;
  global_enb_id_s   global_enb_id;
  uint64_t          ng_enb_du_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICactions-ToBeSetup-List ::= SEQUENCE (SIZE (1..16)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES : IEsSetParam}
using ric_actions_to_be_setup_list_l =
    bounded_array<protocol_ie_single_container_s<ric_action_to_be_setup_item_ies_o>, 16>;

// RICsubscription-withCause-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_sub_with_cause_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_sub_with_cause_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_sub_with_cause_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_sub_with_cause_item_s&       ric_sub_with_cause_item() { return c; }
    const ric_sub_with_cause_item_s& ric_sub_with_cause_item() const { return c; }

  private:
    ric_sub_with_cause_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TriggeringMessage ::= ENUMERATED
struct trigger_msg_opts {
  enum options { init_msg, successful_outcome, unsuccessfull_outcome, nulltype } value;

  const char* to_string() const;
};
using trigger_msg_e = enumerated<trigger_msg_opts>;

// CriticalityDiagnostics ::= SEQUENCE
struct crit_diagnostics_s {
  bool                       ext                      = false;
  bool                       proc_code_present        = false;
  bool                       trigger_msg_present      = false;
  bool                       proc_crit_present        = false;
  bool                       ric_requestor_id_present = false;
  uint16_t                   proc_code                = 0;
  trigger_msg_e              trigger_msg;
  crit_e                     proc_crit;
  ric_request_id_s           ric_requestor_id;
  crit_diagnostics_ie_list_l ies_crit_diagnostics;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2connectionSetupFailed-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using e2conn_setup_failed_list_l = bounded_array<protocol_ie_single_container_s<e2conn_setup_failed_item_ies_o>, 32>;

// E2connectionUpdate-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES : IEsSetParam}
using e2conn_upd_list_l = bounded_array<protocol_ie_single_container_s<e2conn_upd_item_ies_o>, 32>;

// E2connectionUpdateRemove-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using e2conn_upd_rem_list_l = bounded_array<protocol_ie_single_container_s<e2conn_upd_rem_item_ies_o>, 32>;

// E2nodeComponentConfigAdditionAck-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using e2node_component_cfg_addition_ack_list_l =
    dyn_array<protocol_ie_single_container_s<e2node_component_cfg_addition_ack_item_ies_o>>;

// E2nodeComponentConfigAddition-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using e2node_component_cfg_addition_list_l =
    dyn_array<protocol_ie_single_container_s<e2node_component_cfg_addition_item_ies_o>>;

// E2nodeComponentConfigRemovalAck-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using e2node_component_cfg_removal_ack_list_l =
    dyn_array<protocol_ie_single_container_s<e2node_component_cfg_removal_ack_item_ies_o>>;

// E2nodeComponentConfigRemoval-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using e2node_component_cfg_removal_list_l =
    dyn_array<protocol_ie_single_container_s<e2node_component_cfg_removal_item_ies_o>>;

// E2nodeComponentConfigUpdateAck-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using e2node_component_cfg_upd_ack_list_l =
    dyn_array<protocol_ie_single_container_s<e2node_component_cfg_upd_ack_item_ies_o>>;

// E2nodeComponentConfigUpdate-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using e2node_component_cfg_upd_list_l = dyn_array<protocol_ie_single_container_s<e2node_component_cfg_upd_item_ies_o>>;

// E2nodeTNLassociationRemoval-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using e2node_tnl_assoc_removal_list_l =
    bounded_array<protocol_ie_single_container_s<e2node_tnl_assoc_removal_item_ies_o>, 32>;

// GlobalE2node-ID ::= CHOICE
struct global_e2node_id_c {
  struct types_opts {
    enum options { gnb, en_gnb, ng_enb, enb, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  global_e2node_id_c() = default;
  global_e2node_id_c(const global_e2node_id_c& other);
  global_e2node_id_c& operator=(const global_e2node_id_c& other);
  ~global_e2node_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  global_e2node_gnb_id_s& gnb()
  {
    assert_choice_type(types::gnb, type_, "GlobalE2node-ID");
    return c.get<global_e2node_gnb_id_s>();
  }
  global_e2node_en_gnb_id_s& en_gnb()
  {
    assert_choice_type(types::en_gnb, type_, "GlobalE2node-ID");
    return c.get<global_e2node_en_gnb_id_s>();
  }
  global_e2node_ng_enb_id_s& ng_enb()
  {
    assert_choice_type(types::ng_enb, type_, "GlobalE2node-ID");
    return c.get<global_e2node_ng_enb_id_s>();
  }
  global_e2node_enb_id_s& enb()
  {
    assert_choice_type(types::enb, type_, "GlobalE2node-ID");
    return c.get<global_e2node_enb_id_s>();
  }
  const global_e2node_gnb_id_s& gnb() const
  {
    assert_choice_type(types::gnb, type_, "GlobalE2node-ID");
    return c.get<global_e2node_gnb_id_s>();
  }
  const global_e2node_en_gnb_id_s& en_gnb() const
  {
    assert_choice_type(types::en_gnb, type_, "GlobalE2node-ID");
    return c.get<global_e2node_en_gnb_id_s>();
  }
  const global_e2node_ng_enb_id_s& ng_enb() const
  {
    assert_choice_type(types::ng_enb, type_, "GlobalE2node-ID");
    return c.get<global_e2node_ng_enb_id_s>();
  }
  const global_e2node_enb_id_s& enb() const
  {
    assert_choice_type(types::enb, type_, "GlobalE2node-ID");
    return c.get<global_e2node_enb_id_s>();
  }
  global_e2node_gnb_id_s&    set_gnb();
  global_e2node_en_gnb_id_s& set_en_gnb();
  global_e2node_ng_enb_id_s& set_ng_enb();
  global_e2node_enb_id_s&    set_enb();

private:
  types type_;
  choice_buffer_t<global_e2node_en_gnb_id_s, global_e2node_enb_id_s, global_e2node_gnb_id_s, global_e2node_ng_enb_id_s>
      c;

  void destroy_();
};

// GlobalRIC-ID ::= SEQUENCE
struct global_ric_id_s {
  bool                             ext = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<20, false, true> ric_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANfunctionsIDcause-List ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES : IEsSetParam}
using ran_functions_id_cause_list_l = dyn_array<protocol_ie_single_container_s<ran_function_id_cause_item_ies_o>>;

// RANfunctionsID-List ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES : IEsSetParam}
using ran_functions_id_list_l = dyn_array<protocol_ie_single_container_s<ran_function_id_item_ies_o>>;

// RANfunctions-List ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES : IEsSetParam}
using ran_functions_list_l = dyn_array<protocol_ie_single_container_s<ran_function_item_ies_o>>;

// RICaction-Admitted-List ::= SEQUENCE (SIZE (1..16)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES : IEsSetParam}
using ric_action_admitted_list_l = bounded_array<protocol_ie_single_container_s<ric_action_admitted_item_ies_o>, 16>;

// RICaction-NotAdmitted-List ::= SEQUENCE (SIZE (0..16)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES : IEsSetParam}
using ric_action_not_admitted_list_l =
    bounded_array<protocol_ie_single_container_s<ric_action_not_admitted_item_ies_o>, 16>;

// RICactions-AddedForModification-List ::= SEQUENCE (SIZE (0..16)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using ric_actions_added_for_mod_list_l =
    bounded_array<protocol_ie_single_container_s<ric_action_added_for_mod_item_ies_o>, 16>;

// RICactions-ConfirmedForModification-List ::= SEQUENCE (SIZE (0..16)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES
// : IEsSetParam}
using ric_actions_confirmed_for_mod_list_l =
    bounded_array<protocol_ie_single_container_s<ric_action_confirmed_for_mod_item_ies_o>, 16>;

// RICactions-ConfirmedForRemoval-List ::= SEQUENCE (SIZE (0..16)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using ric_actions_confirmed_for_removal_list_l =
    bounded_array<protocol_ie_single_container_s<ric_action_confirmed_for_removal_item_ies_o>, 16>;

// RICactions-FailedToBeAddedForModification-List ::= SEQUENCE (SIZE (0..16)) OF
// ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES : IEsSetParam}
using ric_actions_failed_to_be_added_for_mod_list_l =
    bounded_array<protocol_ie_single_container_s<ric_action_failed_to_be_added_for_mod_item_ies_o>, 16>;

// RICactions-FailedToBeModifiedForModification-List ::= SEQUENCE (SIZE (0..16)) OF
// ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES : IEsSetParam}
using ric_actions_failed_to_be_modified_for_mod_list_l =
    bounded_array<protocol_ie_single_container_s<ric_action_failed_to_be_modified_for_mod_item_ies_o>, 16>;

// RICactions-FailedToBeRemovedForModification-List ::= SEQUENCE (SIZE (0..16)) OF
// ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES : IEsSetParam}
using ric_actions_failed_to_be_remd_for_mod_list_l =
    bounded_array<protocol_ie_single_container_s<ric_action_failed_to_be_remd_for_mod_item_ies_o>, 16>;

// RICactions-ModifiedForModification-List ::= SEQUENCE (SIZE (0..16)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using ric_actions_modified_for_mod_list_l =
    bounded_array<protocol_ie_single_container_s<ric_action_modified_for_mod_item_ies_o>, 16>;

// RICactions-RefusedToBeModified-List ::= SEQUENCE (SIZE (0..16)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using ric_actions_refused_to_be_modified_list_l =
    bounded_array<protocol_ie_single_container_s<ric_action_refused_to_be_modified_item_ies_o>, 16>;

// RICactions-RefusedToBeRemoved-List ::= SEQUENCE (SIZE (0..16)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using ric_actions_refused_to_be_remd_list_l =
    bounded_array<protocol_ie_single_container_s<ric_action_refused_to_be_remd_item_ies_o>, 16>;

// RICactions-RemovedForModification-List ::= SEQUENCE (SIZE (0..16)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using ric_actions_remd_for_mod_list_l =
    bounded_array<protocol_ie_single_container_s<ric_action_remd_for_mod_item_ies_o>, 16>;

// RICactions-RequiredToBeModified-List ::= SEQUENCE (SIZE (0..16)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using ric_actions_required_to_be_modified_list_l =
    bounded_array<protocol_ie_single_container_s<ric_action_required_to_be_modified_item_ies_o>, 16>;

// RICactions-RequiredToBeRemoved-List ::= SEQUENCE (SIZE (0..16)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using ric_actions_required_to_be_remd_list_l =
    bounded_array<protocol_ie_single_container_s<ric_action_required_to_be_remd_item_ies_o>, 16>;

// RICactions-ToBeAddedForModification-List ::= SEQUENCE (SIZE (0..16)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES
// : IEsSetParam}
using ric_actions_to_be_added_for_mod_list_l =
    bounded_array<protocol_ie_single_container_s<ric_action_to_be_added_for_mod_item_ies_o>, 16>;

// RICactions-ToBeModifiedForModification-List ::= SEQUENCE (SIZE (0..16)) OF
// ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES : IEsSetParam}
using ric_actions_to_be_modified_for_mod_list_l =
    bounded_array<protocol_ie_single_container_s<ric_action_to_be_modified_for_mod_item_ies_o>, 16>;

// RICactions-ToBeRemovedForModification-List ::= SEQUENCE (SIZE (0..16)) OF
// ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES : IEsSetParam}
using ric_actions_to_be_remd_for_mod_list_l =
    bounded_array<protocol_ie_single_container_s<ric_action_to_be_remd_for_mod_item_ies_o>, 16>;

// RICcontrolAckRequest ::= ENUMERATED
struct ric_ctrl_ack_request_opts {
  enum options { no_ack, ack, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ric_ctrl_ack_request_e = enumerated<ric_ctrl_ack_request_opts, true>;

// RICindicationType ::= ENUMERATED
struct ric_ind_type_opts {
  enum options { report, insert, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ric_ind_type_e = enumerated<ric_ind_type_opts, true>;

// RICsubscriptionDetails ::= SEQUENCE
struct ric_sub_details_s {
  bool                           ext = false;
  unbounded_octstring<true>      ric_event_trigger_definition;
  ric_actions_to_be_setup_list_l ric_action_to_be_setup_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TimeToWait ::= ENUMERATED
struct time_to_wait_opts {
  enum options { v1s, v2s, v5s, v10s, v20s, v60s, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using time_to_wait_e = enumerated<time_to_wait_opts, true>;

// E2RemovalFailureIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2_removal_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, crit_diagnostics, nulltype } value;
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
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
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

// E2RemovalResponseIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2_removal_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, crit_diagnostics, nulltype } value;
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
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
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

// E2connectionUpdateAck-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2conn_upd_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, e2conn_setup, e2conn_setup_failed, nulltype } value;

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
    uint16_t&                         transaction_id();
    e2conn_upd_list_l&                e2conn_setup();
    e2conn_setup_failed_list_l&       e2conn_setup_failed();
    const uint16_t&                   transaction_id() const;
    const e2conn_upd_list_l&          e2conn_setup() const;
    const e2conn_setup_failed_list_l& e2conn_setup_failed() const;

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

// E2connectionUpdateFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2conn_upd_fail_ies_o {
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

// E2connectionUpdate-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2conn_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, e2conn_upd_add, e2conn_upd_rem, e2conn_upd_modify, nulltype } value;

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
    uint16_t&                    transaction_id();
    e2conn_upd_list_l&           e2conn_upd_add();
    e2conn_upd_rem_list_l&       e2conn_upd_rem();
    e2conn_upd_list_l&           e2conn_upd_modify();
    const uint16_t&              transaction_id() const;
    const e2conn_upd_list_l&     e2conn_upd_add() const;
    const e2conn_upd_rem_list_l& e2conn_upd_rem() const;
    const e2conn_upd_list_l&     e2conn_upd_modify() const;

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

// E2nodeConfigurationUpdateAcknowledge-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_cfg_upd_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        e2node_component_cfg_addition_ack,
        e2node_component_cfg_upd_ack,
        e2node_component_cfg_removal_ack,
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
    uint16_t&                                       transaction_id();
    e2node_component_cfg_addition_ack_list_l&       e2node_component_cfg_addition_ack();
    e2node_component_cfg_upd_ack_list_l&            e2node_component_cfg_upd_ack();
    e2node_component_cfg_removal_ack_list_l&        e2node_component_cfg_removal_ack();
    const uint16_t&                                 transaction_id() const;
    const e2node_component_cfg_addition_ack_list_l& e2node_component_cfg_addition_ack() const;
    const e2node_component_cfg_upd_ack_list_l&      e2node_component_cfg_upd_ack() const;
    const e2node_component_cfg_removal_ack_list_l&  e2node_component_cfg_removal_ack() const;

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

// E2nodeConfigurationUpdateFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_cfg_upd_fail_ies_o {
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

// E2nodeConfigurationUpdate-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_cfg_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        global_e2node_id,
        e2node_component_cfg_addition,
        e2node_component_cfg_upd,
        e2node_component_cfg_removal,
        e2node_tnl_assoc_removal,
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
    global_e2node_id_c&                         global_e2node_id();
    e2node_component_cfg_addition_list_l&       e2node_component_cfg_addition();
    e2node_component_cfg_upd_list_l&            e2node_component_cfg_upd();
    e2node_component_cfg_removal_list_l&        e2node_component_cfg_removal();
    e2node_tnl_assoc_removal_list_l&            e2node_tnl_assoc_removal();
    const uint16_t&                             transaction_id() const;
    const global_e2node_id_c&                   global_e2node_id() const;
    const e2node_component_cfg_addition_list_l& e2node_component_cfg_addition() const;
    const e2node_component_cfg_upd_list_l&      e2node_component_cfg_upd() const;
    const e2node_component_cfg_removal_list_l&  e2node_component_cfg_removal() const;
    const e2node_tnl_assoc_removal_list_l&      e2node_tnl_assoc_removal() const;

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

// E2setupFailureIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2setup_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, tnl_info, nulltype } value;
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
    tnl_info_s&               tnl_info();
    const uint16_t&           transaction_id() const;
    const cause_c&            cause() const;
    const time_to_wait_e&     time_to_wait() const;
    const crit_diagnostics_s& crit_diagnostics() const;
    const tnl_info_s&         tnl_info() const;

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

// E2setupRequestIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        global_e2node_id,
        ran_functions_added,
        e2node_component_cfg_addition,
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
    global_e2node_id_c&                         global_e2node_id();
    ran_functions_list_l&                       ran_functions_added();
    e2node_component_cfg_addition_list_l&       e2node_component_cfg_addition();
    const uint16_t&                             transaction_id() const;
    const global_e2node_id_c&                   global_e2node_id() const;
    const ran_functions_list_l&                 ran_functions_added() const;
    const e2node_component_cfg_addition_list_l& e2node_component_cfg_addition() const;

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

// E2setupResponseIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        global_ric_id,
        ran_functions_accepted,
        ran_functions_rejected,
        e2node_component_cfg_addition_ack,
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
    uint16_t&                                       transaction_id();
    global_ric_id_s&                                global_ric_id();
    ran_functions_id_list_l&                        ran_functions_accepted();
    ran_functions_id_cause_list_l&                  ran_functions_rejected();
    e2node_component_cfg_addition_ack_list_l&       e2node_component_cfg_addition_ack();
    const uint16_t&                                 transaction_id() const;
    const global_ric_id_s&                          global_ric_id() const;
    const ran_functions_id_list_l&                  ran_functions_accepted() const;
    const ran_functions_id_cause_list_l&            ran_functions_rejected() const;
    const e2node_component_cfg_addition_ack_list_l& e2node_component_cfg_addition_ack() const;

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

// ErrorIndication-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct error_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, ric_request_id, ran_function_id, cause, crit_diagnostics, nulltype } value;

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
    ric_request_id_s&         ric_request_id();
    uint16_t&                 ran_function_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const ric_request_id_s&   ric_request_id() const;
    const uint16_t&           ran_function_id() const;
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

// ResetRequestIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct reset_request_ies_o {
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

// ResetResponseIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct reset_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, crit_diagnostics, nulltype } value;
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
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
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

// RICcontrolAcknowledge-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_ctrl_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_request_id, ran_function_id, ric_call_process_id, ric_ctrl_outcome, nulltype } value;
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
    ric_request_id_s&                ric_request_id();
    uint16_t&                        ran_function_id();
    unbounded_octstring<true>&       ric_call_process_id();
    unbounded_octstring<true>&       ric_ctrl_outcome();
    const ric_request_id_s&          ric_request_id() const;
    const uint16_t&                  ran_function_id() const;
    const unbounded_octstring<true>& ric_call_process_id() const;
    const unbounded_octstring<true>& ric_ctrl_outcome() const;

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

// RICcontrolFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_ctrl_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_request_id, ran_function_id, ric_call_process_id, cause, ric_ctrl_outcome, nulltype } value;
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
    ric_request_id_s&                ric_request_id();
    uint16_t&                        ran_function_id();
    unbounded_octstring<true>&       ric_call_process_id();
    cause_c&                         cause();
    unbounded_octstring<true>&       ric_ctrl_outcome();
    const ric_request_id_s&          ric_request_id() const;
    const uint16_t&                  ran_function_id() const;
    const unbounded_octstring<true>& ric_call_process_id() const;
    const cause_c&                   cause() const;
    const unbounded_octstring<true>& ric_ctrl_outcome() const;

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

// RICcontrolRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_ctrl_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ric_request_id,
        ran_function_id,
        ric_call_process_id,
        ric_ctrl_hdr,
        ric_ctrl_msg,
        ric_ctrl_ack_request,
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
    ric_request_id_s&                ric_request_id();
    uint16_t&                        ran_function_id();
    unbounded_octstring<true>&       ric_call_process_id();
    unbounded_octstring<true>&       ric_ctrl_hdr();
    unbounded_octstring<true>&       ric_ctrl_msg();
    ric_ctrl_ack_request_e&          ric_ctrl_ack_request();
    const ric_request_id_s&          ric_request_id() const;
    const uint16_t&                  ran_function_id() const;
    const unbounded_octstring<true>& ric_call_process_id() const;
    const unbounded_octstring<true>& ric_ctrl_hdr() const;
    const unbounded_octstring<true>& ric_ctrl_msg() const;
    const ric_ctrl_ack_request_e&    ric_ctrl_ack_request() const;

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

// RICindication-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ric_request_id,
        ran_function_id,
        ric_action_id,
        ric_ind_sn,
        ric_ind_type,
        ric_ind_hdr,
        ric_ind_msg,
        ric_call_process_id,
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
    ric_request_id_s&                ric_request_id();
    uint16_t&                        ran_function_id();
    uint16_t&                        ric_action_id();
    uint32_t&                        ric_ind_sn();
    ric_ind_type_e&                  ric_ind_type();
    unbounded_octstring<true>&       ric_ind_hdr();
    unbounded_octstring<true>&       ric_ind_msg();
    unbounded_octstring<true>&       ric_call_process_id();
    const ric_request_id_s&          ric_request_id() const;
    const uint16_t&                  ran_function_id() const;
    const uint16_t&                  ric_action_id() const;
    const uint32_t&                  ric_ind_sn() const;
    const ric_ind_type_e&            ric_ind_type() const;
    const unbounded_octstring<true>& ric_ind_hdr() const;
    const unbounded_octstring<true>& ric_ind_msg() const;
    const unbounded_octstring<true>& ric_call_process_id() const;

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

// RICQueryFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_query_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_request_id, ran_function_id, cause, crit_diagnostics, nulltype } value;
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
    ric_request_id_s&         ric_request_id();
    uint16_t&                 ran_function_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const ric_request_id_s&   ric_request_id() const;
    const uint16_t&           ran_function_id() const;
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

// RICQueryRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_query_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_request_id, ran_function_id, ric_query_hdr, ric_query_definition, nulltype } value;
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
    ric_request_id_s&                ric_request_id();
    uint16_t&                        ran_function_id();
    unbounded_octstring<true>&       ric_query_hdr();
    unbounded_octstring<true>&       ric_query_definition();
    const ric_request_id_s&          ric_request_id() const;
    const uint16_t&                  ran_function_id() const;
    const unbounded_octstring<true>& ric_query_hdr() const;
    const unbounded_octstring<true>& ric_query_definition() const;

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

// RICQueryResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_query_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_request_id, ran_function_id, ric_query_outcome, nulltype } value;
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
    ric_request_id_s&                ric_request_id();
    uint16_t&                        ran_function_id();
    unbounded_octstring<true>&       ric_query_outcome();
    const ric_request_id_s&          ric_request_id() const;
    const uint16_t&                  ran_function_id() const;
    const unbounded_octstring<true>& ric_query_outcome() const;

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

// RICserviceQuery-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_service_query_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, ran_functions_accepted, nulltype } value;
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
    uint16_t&                      transaction_id();
    ran_functions_id_list_l&       ran_functions_accepted();
    const uint16_t&                transaction_id() const;
    const ran_functions_id_list_l& ran_functions_accepted() const;

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

// RICserviceUpdateAcknowledge-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_service_upd_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, ran_functions_accepted, ran_functions_rejected, nulltype } value;
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
    uint16_t&                            transaction_id();
    ran_functions_id_list_l&             ran_functions_accepted();
    ran_functions_id_cause_list_l&       ran_functions_rejected();
    const uint16_t&                      transaction_id() const;
    const ran_functions_id_list_l&       ran_functions_accepted() const;
    const ran_functions_id_cause_list_l& ran_functions_rejected() const;

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

// RICserviceUpdateFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_service_upd_fail_ies_o {
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

// RICserviceUpdate-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_service_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        ran_functions_added,
        ran_functions_modified,
        ran_functions_deleted,
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
    uint16_t&                      transaction_id();
    ran_functions_list_l&          ran_functions_added();
    ran_functions_list_l&          ran_functions_modified();
    ran_functions_id_list_l&       ran_functions_deleted();
    const uint16_t&                transaction_id() const;
    const ran_functions_list_l&    ran_functions_added() const;
    const ran_functions_list_l&    ran_functions_modified() const;
    const ran_functions_id_list_l& ran_functions_deleted() const;

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

// RICsubscriptionDeleteFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_sub_delete_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_request_id, ran_function_id, cause, crit_diagnostics, nulltype } value;
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
    ric_request_id_s&         ric_request_id();
    uint16_t&                 ran_function_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const ric_request_id_s&   ric_request_id() const;
    const uint16_t&           ran_function_id() const;
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

// RICsubscriptionDeleteRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_sub_delete_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_request_id, ran_function_id, nulltype } value;
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
    ric_request_id_s&       ric_request_id();
    uint16_t&               ran_function_id();
    const ric_request_id_s& ric_request_id() const;
    const uint16_t&         ran_function_id() const;

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

// RICsubscriptionDeleteResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_sub_delete_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_request_id, ran_function_id, nulltype } value;
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
    ric_request_id_s&       ric_request_id();
    uint16_t&               ran_function_id();
    const ric_request_id_s& ric_request_id() const;
    const uint16_t&         ran_function_id() const;

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

// RICsubscriptionFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_sub_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_request_id, ran_function_id, cause, crit_diagnostics, nulltype } value;
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
    ric_request_id_s&         ric_request_id();
    uint16_t&                 ran_function_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const ric_request_id_s&   ric_request_id() const;
    const uint16_t&           ran_function_id() const;
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

// RICsubscription-List-withCause ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using ric_sub_list_with_cause_l = dyn_array<protocol_ie_single_container_s<ric_sub_with_cause_item_ies_o>>;

// RICsubscriptionModificationConfirm-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_sub_mod_confirm_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ric_request_id,
        ran_function_id,
        ric_actions_confirmed_for_mod_list,
        ric_actions_refused_to_be_modified_list,
        ric_actions_confirmed_for_removal_list,
        ric_actions_refused_to_be_remd_list,
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
    ric_request_id_s&                                ric_request_id();
    uint16_t&                                        ran_function_id();
    ric_actions_confirmed_for_mod_list_l&            ric_actions_confirmed_for_mod_list();
    ric_actions_refused_to_be_modified_list_l&       ric_actions_refused_to_be_modified_list();
    ric_actions_confirmed_for_removal_list_l&        ric_actions_confirmed_for_removal_list();
    ric_actions_refused_to_be_remd_list_l&           ric_actions_refused_to_be_remd_list();
    const ric_request_id_s&                          ric_request_id() const;
    const uint16_t&                                  ran_function_id() const;
    const ric_actions_confirmed_for_mod_list_l&      ric_actions_confirmed_for_mod_list() const;
    const ric_actions_refused_to_be_modified_list_l& ric_actions_refused_to_be_modified_list() const;
    const ric_actions_confirmed_for_removal_list_l&  ric_actions_confirmed_for_removal_list() const;
    const ric_actions_refused_to_be_remd_list_l&     ric_actions_refused_to_be_remd_list() const;

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

// RICsubscriptionModificationFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_sub_mod_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_request_id, ran_function_id, cause, crit_diagnostics, nulltype } value;
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
    ric_request_id_s&         ric_request_id();
    uint16_t&                 ran_function_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const ric_request_id_s&   ric_request_id() const;
    const uint16_t&           ran_function_id() const;
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

// RICsubscriptionModificationRefuse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_sub_mod_refuse_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_request_id, ran_function_id, cause, crit_diagnostics, nulltype } value;
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
    ric_request_id_s&         ric_request_id();
    uint16_t&                 ran_function_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const ric_request_id_s&   ric_request_id() const;
    const uint16_t&           ran_function_id() const;
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

// RICsubscriptionModificationRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_sub_mod_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ric_request_id,
        ran_function_id,
        ric_event_trigger_definition_to_be_modified,
        ric_actions_to_be_remd_for_mod_list,
        ric_actions_to_be_modified_for_mod_list,
        ric_actions_to_be_added_for_mod_list,
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
    ric_request_id_s&                                ric_request_id();
    uint16_t&                                        ran_function_id();
    unbounded_octstring<true>&                       ric_event_trigger_definition_to_be_modified();
    ric_actions_to_be_remd_for_mod_list_l&           ric_actions_to_be_remd_for_mod_list();
    ric_actions_to_be_modified_for_mod_list_l&       ric_actions_to_be_modified_for_mod_list();
    ric_actions_to_be_added_for_mod_list_l&          ric_actions_to_be_added_for_mod_list();
    const ric_request_id_s&                          ric_request_id() const;
    const uint16_t&                                  ran_function_id() const;
    const unbounded_octstring<true>&                 ric_event_trigger_definition_to_be_modified() const;
    const ric_actions_to_be_remd_for_mod_list_l&     ric_actions_to_be_remd_for_mod_list() const;
    const ric_actions_to_be_modified_for_mod_list_l& ric_actions_to_be_modified_for_mod_list() const;
    const ric_actions_to_be_added_for_mod_list_l&    ric_actions_to_be_added_for_mod_list() const;

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

// RICsubscriptionModificationRequired-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_sub_mod_required_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ric_request_id,
        ran_function_id,
        ric_actions_required_to_be_modified_list,
        ric_actions_required_to_be_remd_list,
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
    ric_request_id_s&                                 ric_request_id();
    uint16_t&                                         ran_function_id();
    ric_actions_required_to_be_modified_list_l&       ric_actions_required_to_be_modified_list();
    ric_actions_required_to_be_remd_list_l&           ric_actions_required_to_be_remd_list();
    const ric_request_id_s&                           ric_request_id() const;
    const uint16_t&                                   ran_function_id() const;
    const ric_actions_required_to_be_modified_list_l& ric_actions_required_to_be_modified_list() const;
    const ric_actions_required_to_be_remd_list_l&     ric_actions_required_to_be_remd_list() const;

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

// RICsubscriptionModificationResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_sub_mod_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ric_request_id,
        ran_function_id,
        ric_actions_remd_for_mod_list,
        ric_actions_failed_to_be_remd_for_mod_list,
        ric_actions_modified_for_mod_list,
        ric_actions_failed_to_be_modified_for_mod_list,
        ric_actions_added_for_mod_list,
        ric_actions_failed_to_be_added_for_mod_list,
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
    ric_request_id_s&                                       ric_request_id();
    uint16_t&                                               ran_function_id();
    ric_actions_remd_for_mod_list_l&                        ric_actions_remd_for_mod_list();
    ric_actions_failed_to_be_remd_for_mod_list_l&           ric_actions_failed_to_be_remd_for_mod_list();
    ric_actions_modified_for_mod_list_l&                    ric_actions_modified_for_mod_list();
    ric_actions_failed_to_be_modified_for_mod_list_l&       ric_actions_failed_to_be_modified_for_mod_list();
    ric_actions_added_for_mod_list_l&                       ric_actions_added_for_mod_list();
    ric_actions_failed_to_be_added_for_mod_list_l&          ric_actions_failed_to_be_added_for_mod_list();
    const ric_request_id_s&                                 ric_request_id() const;
    const uint16_t&                                         ran_function_id() const;
    const ric_actions_remd_for_mod_list_l&                  ric_actions_remd_for_mod_list() const;
    const ric_actions_failed_to_be_remd_for_mod_list_l&     ric_actions_failed_to_be_remd_for_mod_list() const;
    const ric_actions_modified_for_mod_list_l&              ric_actions_modified_for_mod_list() const;
    const ric_actions_failed_to_be_modified_for_mod_list_l& ric_actions_failed_to_be_modified_for_mod_list() const;
    const ric_actions_added_for_mod_list_l&                 ric_actions_added_for_mod_list() const;
    const ric_actions_failed_to_be_added_for_mod_list_l&    ric_actions_failed_to_be_added_for_mod_list() const;

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

// RICsubscriptionRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_sub_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_request_id, ran_function_id, ric_sub_details, nulltype } value;
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
    ric_request_id_s&        ric_request_id();
    uint16_t&                ran_function_id();
    ric_sub_details_s&       ric_sub_details();
    const ric_request_id_s&  ric_request_id() const;
    const uint16_t&          ran_function_id() const;
    const ric_sub_details_s& ric_sub_details() const;

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

// RICsubscriptionResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_sub_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_request_id, ran_function_id, ric_actions_admitted, ric_actions_not_admitted, nulltype } value;
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
    ric_request_id_s&                     ric_request_id();
    uint16_t&                             ran_function_id();
    ric_action_admitted_list_l&           ric_actions_admitted();
    ric_action_not_admitted_list_l&       ric_actions_not_admitted();
    const ric_request_id_s&               ric_request_id() const;
    const uint16_t&                       ran_function_id() const;
    const ric_action_admitted_list_l&     ric_actions_admitted() const;
    const ric_action_not_admitted_list_l& ric_actions_not_admitted() const;

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

// E2RemovalRequestIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2_removal_request_ies_o {
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

// RICsubscriptionDeleteRequired-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ric_sub_delete_required_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ric_sub_to_be_remd, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ric_sub_to_be_remd; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_sub_list_with_cause_l&       ric_sub_to_be_remd() { return c; }
    const ric_sub_list_with_cause_l& ric_sub_to_be_remd() const { return c; }

  private:
    ric_sub_list_with_cause_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct e2_removal_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct e2_removal_resp_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct e2conn_upd_ack_ies_container {
  bool                       e2conn_setup_present        = false;
  bool                       e2conn_setup_failed_present = false;
  uint16_t                   transaction_id;
  e2conn_upd_list_l          e2conn_setup;
  e2conn_setup_failed_list_l e2conn_setup_failed;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct e2conn_upd_fail_ies_container {
  bool               cause_present            = false;
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

struct e2conn_upd_ies_container {
  bool                  e2conn_upd_add_present    = false;
  bool                  e2conn_upd_rem_present    = false;
  bool                  e2conn_upd_modify_present = false;
  uint16_t              transaction_id;
  e2conn_upd_list_l     e2conn_upd_add;
  e2conn_upd_rem_list_l e2conn_upd_rem;
  e2conn_upd_list_l     e2conn_upd_modify;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct e2node_cfg_upd_ack_ies_container {
  bool                                     e2node_component_cfg_addition_ack_present = false;
  bool                                     e2node_component_cfg_upd_ack_present      = false;
  bool                                     e2node_component_cfg_removal_ack_present  = false;
  uint16_t                                 transaction_id;
  e2node_component_cfg_addition_ack_list_l e2node_component_cfg_addition_ack;
  e2node_component_cfg_upd_ack_list_l      e2node_component_cfg_upd_ack;
  e2node_component_cfg_removal_ack_list_l  e2node_component_cfg_removal_ack;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct e2node_cfg_upd_fail_ies_container {
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

struct e2node_cfg_upd_ies_container {
  bool                                 global_e2node_id_present              = false;
  bool                                 e2node_component_cfg_addition_present = false;
  bool                                 e2node_component_cfg_upd_present      = false;
  bool                                 e2node_component_cfg_removal_present  = false;
  bool                                 e2node_tnl_assoc_removal_present      = false;
  uint16_t                             transaction_id;
  global_e2node_id_c                   global_e2node_id;
  e2node_component_cfg_addition_list_l e2node_component_cfg_addition;
  e2node_component_cfg_upd_list_l      e2node_component_cfg_upd;
  e2node_component_cfg_removal_list_l  e2node_component_cfg_removal;
  e2node_tnl_assoc_removal_list_l      e2node_tnl_assoc_removal;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct e2setup_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  bool               tnl_info_present         = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;
  tnl_info_s         tnl_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct e2setup_request_ies_container {
  uint16_t                             transaction_id;
  global_e2node_id_c                   global_e2node_id;
  ran_functions_list_l                 ran_functions_added;
  e2node_component_cfg_addition_list_l e2node_component_cfg_addition;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct e2setup_resp_ies_container {
  bool                                     ran_functions_accepted_present = false;
  bool                                     ran_functions_rejected_present = false;
  uint16_t                                 transaction_id;
  global_ric_id_s                          global_ric_id;
  ran_functions_id_list_l                  ran_functions_accepted;
  ran_functions_id_cause_list_l            ran_functions_rejected;
  e2node_component_cfg_addition_ack_list_l e2node_component_cfg_addition_ack;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct error_ind_ies_container {
  bool               transaction_id_present   = false;
  bool               ric_request_id_present   = false;
  bool               ran_function_id_present  = false;
  bool               cause_present            = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  ric_request_id_s   ric_request_id;
  uint16_t           ran_function_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct reset_request_ies_container {
  uint16_t transaction_id;
  cause_c  cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct reset_resp_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_ctrl_ack_ies_container {
  bool                      ric_call_process_id_present = false;
  bool                      ric_ctrl_outcome_present    = false;
  ric_request_id_s          ric_request_id;
  uint16_t                  ran_function_id;
  unbounded_octstring<true> ric_call_process_id;
  unbounded_octstring<true> ric_ctrl_outcome;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_ctrl_fail_ies_container {
  bool                      ric_call_process_id_present = false;
  bool                      ric_ctrl_outcome_present    = false;
  ric_request_id_s          ric_request_id;
  uint16_t                  ran_function_id;
  unbounded_octstring<true> ric_call_process_id;
  cause_c                   cause;
  unbounded_octstring<true> ric_ctrl_outcome;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_ctrl_request_ies_container {
  bool                      ric_call_process_id_present  = false;
  bool                      ric_ctrl_ack_request_present = false;
  ric_request_id_s          ric_request_id;
  uint16_t                  ran_function_id;
  unbounded_octstring<true> ric_call_process_id;
  unbounded_octstring<true> ric_ctrl_hdr;
  unbounded_octstring<true> ric_ctrl_msg;
  ric_ctrl_ack_request_e    ric_ctrl_ack_request;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_ind_ies_container {
  bool                      ric_ind_sn_present          = false;
  bool                      ric_call_process_id_present = false;
  ric_request_id_s          ric_request_id;
  uint16_t                  ran_function_id;
  uint16_t                  ric_action_id;
  uint32_t                  ric_ind_sn;
  ric_ind_type_e            ric_ind_type;
  unbounded_octstring<true> ric_ind_hdr;
  unbounded_octstring<true> ric_ind_msg;
  unbounded_octstring<true> ric_call_process_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_query_fail_ies_container {
  bool               crit_diagnostics_present = false;
  ric_request_id_s   ric_request_id;
  uint16_t           ran_function_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_query_request_ies_container {
  ric_request_id_s          ric_request_id;
  uint16_t                  ran_function_id;
  unbounded_octstring<true> ric_query_hdr;
  unbounded_octstring<true> ric_query_definition;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_query_resp_ies_container {
  ric_request_id_s          ric_request_id;
  uint16_t                  ran_function_id;
  unbounded_octstring<true> ric_query_outcome;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_service_query_ies_container {
  bool                    ran_functions_accepted_present = false;
  uint16_t                transaction_id;
  ran_functions_id_list_l ran_functions_accepted;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_service_upd_ack_ies_container {
  bool                          ran_functions_accepted_present = false;
  bool                          ran_functions_rejected_present = false;
  uint16_t                      transaction_id;
  ran_functions_id_list_l       ran_functions_accepted;
  ran_functions_id_cause_list_l ran_functions_rejected;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_service_upd_fail_ies_container {
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

struct ric_service_upd_ies_container {
  bool                    ran_functions_added_present    = false;
  bool                    ran_functions_modified_present = false;
  bool                    ran_functions_deleted_present  = false;
  uint16_t                transaction_id;
  ran_functions_list_l    ran_functions_added;
  ran_functions_list_l    ran_functions_modified;
  ran_functions_id_list_l ran_functions_deleted;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_sub_delete_fail_ies_container {
  bool               crit_diagnostics_present = false;
  ric_request_id_s   ric_request_id;
  uint16_t           ran_function_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_sub_delete_request_ies_container {
  ric_request_id_s ric_request_id;
  uint16_t         ran_function_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_sub_delete_resp_ies_container {
  ric_request_id_s ric_request_id;
  uint16_t         ran_function_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_sub_fail_ies_container {
  bool               crit_diagnostics_present = false;
  ric_request_id_s   ric_request_id;
  uint16_t           ran_function_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_sub_mod_confirm_ies_container {
  bool                                      ric_actions_confirmed_for_mod_list_present      = false;
  bool                                      ric_actions_refused_to_be_modified_list_present = false;
  bool                                      ric_actions_confirmed_for_removal_list_present  = false;
  bool                                      ric_actions_refused_to_be_remd_list_present     = false;
  ric_request_id_s                          ric_request_id;
  uint16_t                                  ran_function_id;
  ric_actions_confirmed_for_mod_list_l      ric_actions_confirmed_for_mod_list;
  ric_actions_refused_to_be_modified_list_l ric_actions_refused_to_be_modified_list;
  ric_actions_confirmed_for_removal_list_l  ric_actions_confirmed_for_removal_list;
  ric_actions_refused_to_be_remd_list_l     ric_actions_refused_to_be_remd_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_sub_mod_fail_ies_container {
  bool               crit_diagnostics_present = false;
  ric_request_id_s   ric_request_id;
  uint16_t           ran_function_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_sub_mod_refuse_ies_container {
  bool               crit_diagnostics_present = false;
  ric_request_id_s   ric_request_id;
  uint16_t           ran_function_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_sub_mod_request_ies_container {
  bool                                      ric_event_trigger_definition_to_be_modified_present = false;
  bool                                      ric_actions_to_be_remd_for_mod_list_present         = false;
  bool                                      ric_actions_to_be_modified_for_mod_list_present     = false;
  bool                                      ric_actions_to_be_added_for_mod_list_present        = false;
  ric_request_id_s                          ric_request_id;
  uint16_t                                  ran_function_id;
  unbounded_octstring<true>                 ric_event_trigger_definition_to_be_modified;
  ric_actions_to_be_remd_for_mod_list_l     ric_actions_to_be_remd_for_mod_list;
  ric_actions_to_be_modified_for_mod_list_l ric_actions_to_be_modified_for_mod_list;
  ric_actions_to_be_added_for_mod_list_l    ric_actions_to_be_added_for_mod_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_sub_mod_required_ies_container {
  bool                                       ric_actions_required_to_be_modified_list_present = false;
  bool                                       ric_actions_required_to_be_remd_list_present     = false;
  ric_request_id_s                           ric_request_id;
  uint16_t                                   ran_function_id;
  ric_actions_required_to_be_modified_list_l ric_actions_required_to_be_modified_list;
  ric_actions_required_to_be_remd_list_l     ric_actions_required_to_be_remd_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_sub_mod_resp_ies_container {
  bool                                             ric_actions_remd_for_mod_list_present                  = false;
  bool                                             ric_actions_failed_to_be_remd_for_mod_list_present     = false;
  bool                                             ric_actions_modified_for_mod_list_present              = false;
  bool                                             ric_actions_failed_to_be_modified_for_mod_list_present = false;
  bool                                             ric_actions_added_for_mod_list_present                 = false;
  bool                                             ric_actions_failed_to_be_added_for_mod_list_present    = false;
  ric_request_id_s                                 ric_request_id;
  uint16_t                                         ran_function_id;
  ric_actions_remd_for_mod_list_l                  ric_actions_remd_for_mod_list;
  ric_actions_failed_to_be_remd_for_mod_list_l     ric_actions_failed_to_be_remd_for_mod_list;
  ric_actions_modified_for_mod_list_l              ric_actions_modified_for_mod_list;
  ric_actions_failed_to_be_modified_for_mod_list_l ric_actions_failed_to_be_modified_for_mod_list;
  ric_actions_added_for_mod_list_l                 ric_actions_added_for_mod_list;
  ric_actions_failed_to_be_added_for_mod_list_l    ric_actions_failed_to_be_added_for_mod_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_sub_request_ies_container {
  ric_request_id_s  ric_request_id;
  uint16_t          ran_function_id;
  ric_sub_details_s ric_sub_details;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct ric_sub_resp_ies_container {
  bool                           ric_actions_not_admitted_present = false;
  ric_request_id_s               ric_request_id;
  uint16_t                       ran_function_id;
  ric_action_admitted_list_l     ric_actions_admitted;
  ric_action_not_admitted_list_l ric_actions_not_admitted;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2RemovalFailure ::= SEQUENCE
using e2_removal_fail_s = elementary_procedure_option<e2_removal_fail_ies_container>;

// E2RemovalRequest ::= SEQUENCE
using e2_removal_request_s = elementary_procedure_option<protocol_ie_container_l<e2_removal_request_ies_o>>;

// E2RemovalResponse ::= SEQUENCE
using e2_removal_resp_s = elementary_procedure_option<e2_removal_resp_ies_container>;

// E2connectionUpdateAcknowledge ::= SEQUENCE
using e2conn_upd_ack_s = elementary_procedure_option<e2conn_upd_ack_ies_container>;

// E2connectionUpdateFailure ::= SEQUENCE
using e2conn_upd_fail_s = elementary_procedure_option<e2conn_upd_fail_ies_container>;

// E2connectionUpdate ::= SEQUENCE
using e2conn_upd_s = elementary_procedure_option<e2conn_upd_ies_container>;

// E2nodeConfigurationUpdateAcknowledge ::= SEQUENCE
using e2node_cfg_upd_ack_s = elementary_procedure_option<e2node_cfg_upd_ack_ies_container>;

// E2nodeConfigurationUpdateFailure ::= SEQUENCE
using e2node_cfg_upd_fail_s = elementary_procedure_option<e2node_cfg_upd_fail_ies_container>;

// E2nodeConfigurationUpdate ::= SEQUENCE
using e2node_cfg_upd_s = elementary_procedure_option<e2node_cfg_upd_ies_container>;

// E2setupFailure ::= SEQUENCE
using e2setup_fail_s = elementary_procedure_option<e2setup_fail_ies_container>;

// E2setupRequest ::= SEQUENCE
using e2setup_request_s = elementary_procedure_option<e2setup_request_ies_container>;

// E2setupResponse ::= SEQUENCE
using e2setup_resp_s = elementary_procedure_option<e2setup_resp_ies_container>;

// ErrorIndication ::= SEQUENCE
using error_ind_s = elementary_procedure_option<error_ind_ies_container>;

// ResetRequest ::= SEQUENCE
using reset_request_s = elementary_procedure_option<reset_request_ies_container>;

// ResetResponse ::= SEQUENCE
using reset_resp_s = elementary_procedure_option<reset_resp_ies_container>;

// RICcontrolAcknowledge ::= SEQUENCE
using ric_ctrl_ack_s = elementary_procedure_option<ric_ctrl_ack_ies_container>;

// RICcontrolFailure ::= SEQUENCE
using ric_ctrl_fail_s = elementary_procedure_option<ric_ctrl_fail_ies_container>;

// RICcontrolRequest ::= SEQUENCE
using ric_ctrl_request_s = elementary_procedure_option<ric_ctrl_request_ies_container>;

// RICindication ::= SEQUENCE
using ric_ind_s = elementary_procedure_option<ric_ind_ies_container>;

// RICQueryFailure ::= SEQUENCE
using ric_query_fail_s = elementary_procedure_option<ric_query_fail_ies_container>;

// RICQueryRequest ::= SEQUENCE
using ric_query_request_s = elementary_procedure_option<ric_query_request_ies_container>;

// RICQueryResponse ::= SEQUENCE
using ric_query_resp_s = elementary_procedure_option<ric_query_resp_ies_container>;

// RICserviceQuery ::= SEQUENCE
using ric_service_query_s = elementary_procedure_option<ric_service_query_ies_container>;

// RICserviceUpdateAcknowledge ::= SEQUENCE
using ric_service_upd_ack_s = elementary_procedure_option<ric_service_upd_ack_ies_container>;

// RICserviceUpdateFailure ::= SEQUENCE
using ric_service_upd_fail_s = elementary_procedure_option<ric_service_upd_fail_ies_container>;

// RICserviceUpdate ::= SEQUENCE
using ric_service_upd_s = elementary_procedure_option<ric_service_upd_ies_container>;

// RICsubscriptionDeleteFailure ::= SEQUENCE
using ric_sub_delete_fail_s = elementary_procedure_option<ric_sub_delete_fail_ies_container>;

// RICsubscriptionDeleteRequest ::= SEQUENCE
using ric_sub_delete_request_s = elementary_procedure_option<ric_sub_delete_request_ies_container>;

// RICsubscriptionDeleteRequired ::= SEQUENCE
using ric_sub_delete_required_s = elementary_procedure_option<protocol_ie_container_l<ric_sub_delete_required_ies_o>>;

// RICsubscriptionDeleteResponse ::= SEQUENCE
using ric_sub_delete_resp_s = elementary_procedure_option<ric_sub_delete_resp_ies_container>;

// RICsubscriptionFailure ::= SEQUENCE
using ric_sub_fail_s = elementary_procedure_option<ric_sub_fail_ies_container>;

// RICsubscriptionModificationConfirm ::= SEQUENCE
using ric_sub_mod_confirm_s = elementary_procedure_option<ric_sub_mod_confirm_ies_container>;

// RICsubscriptionModificationFailure ::= SEQUENCE
using ric_sub_mod_fail_s = elementary_procedure_option<ric_sub_mod_fail_ies_container>;

// RICsubscriptionModificationRefuse ::= SEQUENCE
using ric_sub_mod_refuse_s = elementary_procedure_option<ric_sub_mod_refuse_ies_container>;

// RICsubscriptionModificationRequest ::= SEQUENCE
using ric_sub_mod_request_s = elementary_procedure_option<ric_sub_mod_request_ies_container>;

// RICsubscriptionModificationRequired ::= SEQUENCE
using ric_sub_mod_required_s = elementary_procedure_option<ric_sub_mod_required_ies_container>;

// RICsubscriptionModificationResponse ::= SEQUENCE
using ric_sub_mod_resp_s = elementary_procedure_option<ric_sub_mod_resp_ies_container>;

// RICsubscriptionRequest ::= SEQUENCE
using ric_sub_request_s = elementary_procedure_option<ric_sub_request_ies_container>;

// RICsubscriptionResponse ::= SEQUENCE
using ric_sub_resp_s = elementary_procedure_option<ric_sub_resp_ies_container>;

// E2AP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF E2AP-ELEMENTARY-PROCEDURE
struct e2ap_elem_procs_o {
  // InitiatingMessage ::= OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options {
        ric_sub_request,
        ric_sub_delete_request,
        ric_sub_mod_request,
        ric_sub_mod_required,
        ric_query_request,
        ric_service_upd,
        ric_ctrl_request,
        e2setup_request,
        e2node_cfg_upd,
        e2conn_upd,
        reset_request,
        e2_removal_request,
        ric_ind,
        ric_service_query,
        error_ind,
        ric_sub_delete_required,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    init_msg_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_sub_request_s&               ric_sub_request();
    ric_sub_delete_request_s&        ric_sub_delete_request();
    ric_sub_mod_request_s&           ric_sub_mod_request();
    ric_sub_mod_required_s&          ric_sub_mod_required();
    ric_query_request_s&             ric_query_request();
    ric_service_upd_s&               ric_service_upd();
    ric_ctrl_request_s&              ric_ctrl_request();
    e2setup_request_s&               e2setup_request();
    e2node_cfg_upd_s&                e2node_cfg_upd();
    e2conn_upd_s&                    e2conn_upd();
    reset_request_s&                 reset_request();
    e2_removal_request_s&            e2_removal_request();
    ric_ind_s&                       ric_ind();
    ric_service_query_s&             ric_service_query();
    error_ind_s&                     error_ind();
    ric_sub_delete_required_s&       ric_sub_delete_required();
    const ric_sub_request_s&         ric_sub_request() const;
    const ric_sub_delete_request_s&  ric_sub_delete_request() const;
    const ric_sub_mod_request_s&     ric_sub_mod_request() const;
    const ric_sub_mod_required_s&    ric_sub_mod_required() const;
    const ric_query_request_s&       ric_query_request() const;
    const ric_service_upd_s&         ric_service_upd() const;
    const ric_ctrl_request_s&        ric_ctrl_request() const;
    const e2setup_request_s&         e2setup_request() const;
    const e2node_cfg_upd_s&          e2node_cfg_upd() const;
    const e2conn_upd_s&              e2conn_upd() const;
    const reset_request_s&           reset_request() const;
    const e2_removal_request_s&      e2_removal_request() const;
    const ric_ind_s&                 ric_ind() const;
    const ric_service_query_s&       ric_service_query() const;
    const error_ind_s&               error_ind() const;
    const ric_sub_delete_required_s& ric_sub_delete_required() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  // SuccessfulOutcome ::= OPEN TYPE
  struct successful_outcome_c {
    struct types_opts {
      enum options {
        ric_sub_resp,
        ric_sub_delete_resp,
        ric_sub_mod_resp,
        ric_sub_mod_confirm,
        ric_query_resp,
        ric_service_upd_ack,
        ric_ctrl_ack,
        e2setup_resp,
        e2node_cfg_upd_ack,
        e2conn_upd_ack,
        reset_resp,
        e2_removal_resp,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    successful_outcome_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_sub_resp_s&              ric_sub_resp();
    ric_sub_delete_resp_s&       ric_sub_delete_resp();
    ric_sub_mod_resp_s&          ric_sub_mod_resp();
    ric_sub_mod_confirm_s&       ric_sub_mod_confirm();
    ric_query_resp_s&            ric_query_resp();
    ric_service_upd_ack_s&       ric_service_upd_ack();
    ric_ctrl_ack_s&              ric_ctrl_ack();
    e2setup_resp_s&              e2setup_resp();
    e2node_cfg_upd_ack_s&        e2node_cfg_upd_ack();
    e2conn_upd_ack_s&            e2conn_upd_ack();
    reset_resp_s&                reset_resp();
    e2_removal_resp_s&           e2_removal_resp();
    const ric_sub_resp_s&        ric_sub_resp() const;
    const ric_sub_delete_resp_s& ric_sub_delete_resp() const;
    const ric_sub_mod_resp_s&    ric_sub_mod_resp() const;
    const ric_sub_mod_confirm_s& ric_sub_mod_confirm() const;
    const ric_query_resp_s&      ric_query_resp() const;
    const ric_service_upd_ack_s& ric_service_upd_ack() const;
    const ric_ctrl_ack_s&        ric_ctrl_ack() const;
    const e2setup_resp_s&        e2setup_resp() const;
    const e2node_cfg_upd_ack_s&  e2node_cfg_upd_ack() const;
    const e2conn_upd_ack_s&      e2conn_upd_ack() const;
    const reset_resp_s&          reset_resp() const;
    const e2_removal_resp_s&     e2_removal_resp() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  // UnsuccessfulOutcome ::= OPEN TYPE
  struct unsuccessful_outcome_c {
    struct types_opts {
      enum options {
        ric_sub_fail,
        ric_sub_delete_fail,
        ric_sub_mod_fail,
        ric_sub_mod_refuse,
        ric_query_fail,
        ric_service_upd_fail,
        ric_ctrl_fail,
        e2setup_fail,
        e2node_cfg_upd_fail,
        e2conn_upd_fail,
        e2_removal_fail,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    unsuccessful_outcome_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_sub_fail_s&               ric_sub_fail();
    ric_sub_delete_fail_s&        ric_sub_delete_fail();
    ric_sub_mod_fail_s&           ric_sub_mod_fail();
    ric_sub_mod_refuse_s&         ric_sub_mod_refuse();
    ric_query_fail_s&             ric_query_fail();
    ric_service_upd_fail_s&       ric_service_upd_fail();
    ric_ctrl_fail_s&              ric_ctrl_fail();
    e2setup_fail_s&               e2setup_fail();
    e2node_cfg_upd_fail_s&        e2node_cfg_upd_fail();
    e2conn_upd_fail_s&            e2conn_upd_fail();
    e2_removal_fail_s&            e2_removal_fail();
    const ric_sub_fail_s&         ric_sub_fail() const;
    const ric_sub_delete_fail_s&  ric_sub_delete_fail() const;
    const ric_sub_mod_fail_s&     ric_sub_mod_fail() const;
    const ric_sub_mod_refuse_s&   ric_sub_mod_refuse() const;
    const ric_query_fail_s&       ric_query_fail() const;
    const ric_service_upd_fail_s& ric_service_upd_fail() const;
    const ric_ctrl_fail_s&        ric_ctrl_fail() const;
    const e2setup_fail_s&         e2setup_fail() const;
    const e2node_cfg_upd_fail_s&  e2node_cfg_upd_fail() const;
    const e2conn_upd_fail_s&      e2conn_upd_fail() const;
    const e2_removal_fail_s&      e2_removal_fail() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint16_t               idx_to_proc_code(uint32_t idx);
  static bool                   is_proc_code_valid(const uint16_t& proc_code);
  static init_msg_c             get_init_msg(const uint16_t& proc_code);
  static successful_outcome_c   get_successful_outcome(const uint16_t& proc_code);
  static unsuccessful_outcome_c get_unsuccessful_outcome(const uint16_t& proc_code);
  static crit_e                 get_crit(const uint16_t& proc_code);
};

// ProtocolIE-FieldPair{E2AP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE{{E2AP-PROTOCOL-IES-PAIR}}
template <class ies_set_paramT_>
struct protocol_ie_field_pair_s {
  uint32_t                                 id = 0;
  crit_e                                   first_crit;
  typename ies_set_paramT_::first_value_c  first_value;
  crit_e                                   second_crit;
  typename ies_set_paramT_::second_value_c second_value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint32_t& id_);
};

// InitiatingMessage ::= SEQUENCE{{E2AP-ELEMENTARY-PROCEDURE}}
struct init_msg_s {
  uint16_t                      proc_code = 0;
  crit_e                        crit;
  e2ap_elem_procs_o::init_msg_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// ProtocolIE-ContainerPair{E2AP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE (SIZE (0..65535)) OF ProtocolIE-FieldPair
template <class ies_set_paramT_>
using protocol_ie_container_pair_l = dyn_seq_of<protocol_ie_field_pair_s<ies_set_paramT_>, 0, 65535, true>;

// SuccessfulOutcome ::= SEQUENCE{{E2AP-ELEMENTARY-PROCEDURE}}
struct successful_outcome_s {
  uint16_t                                proc_code = 0;
  crit_e                                  crit;
  e2ap_elem_procs_o::successful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// UnsuccessfulOutcome ::= SEQUENCE{{E2AP-ELEMENTARY-PROCEDURE}}
struct unsuccessful_outcome_s {
  uint16_t                                  proc_code = 0;
  crit_e                                    crit;
  e2ap_elem_procs_o::unsuccessful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// E2AP-ELEMENTARY-PROCEDURES-CLASS-1 ::= OBJECT SET OF E2AP-ELEMENTARY-PROCEDURE
struct e2ap_elem_procs_class_1_o {
  // InitiatingMessage ::= OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options {
        ric_sub_request,
        ric_sub_delete_request,
        ric_sub_mod_request,
        ric_sub_mod_required,
        ric_query_request,
        ric_service_upd,
        ric_ctrl_request,
        e2setup_request,
        e2node_cfg_upd,
        e2conn_upd,
        reset_request,
        e2_removal_request,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    init_msg_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_sub_request_s&              ric_sub_request();
    ric_sub_delete_request_s&       ric_sub_delete_request();
    ric_sub_mod_request_s&          ric_sub_mod_request();
    ric_sub_mod_required_s&         ric_sub_mod_required();
    ric_query_request_s&            ric_query_request();
    ric_service_upd_s&              ric_service_upd();
    ric_ctrl_request_s&             ric_ctrl_request();
    e2setup_request_s&              e2setup_request();
    e2node_cfg_upd_s&               e2node_cfg_upd();
    e2conn_upd_s&                   e2conn_upd();
    reset_request_s&                reset_request();
    e2_removal_request_s&           e2_removal_request();
    const ric_sub_request_s&        ric_sub_request() const;
    const ric_sub_delete_request_s& ric_sub_delete_request() const;
    const ric_sub_mod_request_s&    ric_sub_mod_request() const;
    const ric_sub_mod_required_s&   ric_sub_mod_required() const;
    const ric_query_request_s&      ric_query_request() const;
    const ric_service_upd_s&        ric_service_upd() const;
    const ric_ctrl_request_s&       ric_ctrl_request() const;
    const e2setup_request_s&        e2setup_request() const;
    const e2node_cfg_upd_s&         e2node_cfg_upd() const;
    const e2conn_upd_s&             e2conn_upd() const;
    const reset_request_s&          reset_request() const;
    const e2_removal_request_s&     e2_removal_request() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  // SuccessfulOutcome ::= OPEN TYPE
  struct successful_outcome_c {
    struct types_opts {
      enum options {
        ric_sub_resp,
        ric_sub_delete_resp,
        ric_sub_mod_resp,
        ric_sub_mod_confirm,
        ric_query_resp,
        ric_service_upd_ack,
        ric_ctrl_ack,
        e2setup_resp,
        e2node_cfg_upd_ack,
        e2conn_upd_ack,
        reset_resp,
        e2_removal_resp,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    successful_outcome_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_sub_resp_s&              ric_sub_resp();
    ric_sub_delete_resp_s&       ric_sub_delete_resp();
    ric_sub_mod_resp_s&          ric_sub_mod_resp();
    ric_sub_mod_confirm_s&       ric_sub_mod_confirm();
    ric_query_resp_s&            ric_query_resp();
    ric_service_upd_ack_s&       ric_service_upd_ack();
    ric_ctrl_ack_s&              ric_ctrl_ack();
    e2setup_resp_s&              e2setup_resp();
    e2node_cfg_upd_ack_s&        e2node_cfg_upd_ack();
    e2conn_upd_ack_s&            e2conn_upd_ack();
    reset_resp_s&                reset_resp();
    e2_removal_resp_s&           e2_removal_resp();
    const ric_sub_resp_s&        ric_sub_resp() const;
    const ric_sub_delete_resp_s& ric_sub_delete_resp() const;
    const ric_sub_mod_resp_s&    ric_sub_mod_resp() const;
    const ric_sub_mod_confirm_s& ric_sub_mod_confirm() const;
    const ric_query_resp_s&      ric_query_resp() const;
    const ric_service_upd_ack_s& ric_service_upd_ack() const;
    const ric_ctrl_ack_s&        ric_ctrl_ack() const;
    const e2setup_resp_s&        e2setup_resp() const;
    const e2node_cfg_upd_ack_s&  e2node_cfg_upd_ack() const;
    const e2conn_upd_ack_s&      e2conn_upd_ack() const;
    const reset_resp_s&          reset_resp() const;
    const e2_removal_resp_s&     e2_removal_resp() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  // UnsuccessfulOutcome ::= OPEN TYPE
  struct unsuccessful_outcome_c {
    struct types_opts {
      enum options {
        ric_sub_fail,
        ric_sub_delete_fail,
        ric_sub_mod_fail,
        ric_sub_mod_refuse,
        ric_query_fail,
        ric_service_upd_fail,
        ric_ctrl_fail,
        e2setup_fail,
        e2node_cfg_upd_fail,
        e2conn_upd_fail,
        e2_removal_fail,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    unsuccessful_outcome_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_sub_fail_s&               ric_sub_fail();
    ric_sub_delete_fail_s&        ric_sub_delete_fail();
    ric_sub_mod_fail_s&           ric_sub_mod_fail();
    ric_sub_mod_refuse_s&         ric_sub_mod_refuse();
    ric_query_fail_s&             ric_query_fail();
    ric_service_upd_fail_s&       ric_service_upd_fail();
    ric_ctrl_fail_s&              ric_ctrl_fail();
    e2setup_fail_s&               e2setup_fail();
    e2node_cfg_upd_fail_s&        e2node_cfg_upd_fail();
    e2conn_upd_fail_s&            e2conn_upd_fail();
    e2_removal_fail_s&            e2_removal_fail();
    const ric_sub_fail_s&         ric_sub_fail() const;
    const ric_sub_delete_fail_s&  ric_sub_delete_fail() const;
    const ric_sub_mod_fail_s&     ric_sub_mod_fail() const;
    const ric_sub_mod_refuse_s&   ric_sub_mod_refuse() const;
    const ric_query_fail_s&       ric_query_fail() const;
    const ric_service_upd_fail_s& ric_service_upd_fail() const;
    const ric_ctrl_fail_s&        ric_ctrl_fail() const;
    const e2setup_fail_s&         e2setup_fail() const;
    const e2node_cfg_upd_fail_s&  e2node_cfg_upd_fail() const;
    const e2conn_upd_fail_s&      e2conn_upd_fail() const;
    const e2_removal_fail_s&      e2_removal_fail() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint16_t               idx_to_proc_code(uint32_t idx);
  static bool                   is_proc_code_valid(const uint16_t& proc_code);
  static init_msg_c             get_init_msg(const uint16_t& proc_code);
  static successful_outcome_c   get_successful_outcome(const uint16_t& proc_code);
  static unsuccessful_outcome_c get_unsuccessful_outcome(const uint16_t& proc_code);
  static crit_e                 get_crit(const uint16_t& proc_code);
};

// E2AP-ELEMENTARY-PROCEDURES-CLASS-2 ::= OBJECT SET OF E2AP-ELEMENTARY-PROCEDURE
struct e2ap_elem_procs_class_2_o {
  // InitiatingMessage ::= OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options { ric_ind, ric_service_query, error_ind, ric_sub_delete_required, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    init_msg_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ric_ind_s&                       ric_ind();
    ric_service_query_s&             ric_service_query();
    error_ind_s&                     error_ind();
    ric_sub_delete_required_s&       ric_sub_delete_required();
    const ric_ind_s&                 ric_ind() const;
    const ric_service_query_s&       ric_service_query() const;
    const error_ind_s&               error_ind() const;
    const ric_sub_delete_required_s& ric_sub_delete_required() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  // SuccessfulOutcome ::= OPEN TYPE
  struct successful_outcome_c {
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
  // UnsuccessfulOutcome ::= OPEN TYPE
  struct unsuccessful_outcome_c {
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

  // members lookup methods
  static uint16_t               idx_to_proc_code(uint32_t idx);
  static bool                   is_proc_code_valid(const uint16_t& proc_code);
  static init_msg_c             get_init_msg(const uint16_t& proc_code);
  static successful_outcome_c   get_successful_outcome(const uint16_t& proc_code);
  static unsuccessful_outcome_c get_unsuccessful_outcome(const uint16_t& proc_code);
  static crit_e                 get_crit(const uint16_t& proc_code);
};

// E2AP-PDU ::= CHOICE
struct e2ap_pdu_c {
  struct types_opts {
    enum options { init_msg, successful_outcome, unsuccessful_outcome, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  e2ap_pdu_c() = default;
  e2ap_pdu_c(const e2ap_pdu_c& other);
  e2ap_pdu_c& operator=(const e2ap_pdu_c& other);
  ~e2ap_pdu_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  init_msg_s& init_msg()
  {
    assert_choice_type(types::init_msg, type_, "E2AP-PDU");
    return c.get<init_msg_s>();
  }
  successful_outcome_s& successful_outcome()
  {
    assert_choice_type(types::successful_outcome, type_, "E2AP-PDU");
    return c.get<successful_outcome_s>();
  }
  unsuccessful_outcome_s& unsuccessful_outcome()
  {
    assert_choice_type(types::unsuccessful_outcome, type_, "E2AP-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  const init_msg_s& init_msg() const
  {
    assert_choice_type(types::init_msg, type_, "E2AP-PDU");
    return c.get<init_msg_s>();
  }
  const successful_outcome_s& successful_outcome() const
  {
    assert_choice_type(types::successful_outcome, type_, "E2AP-PDU");
    return c.get<successful_outcome_s>();
  }
  const unsuccessful_outcome_s& unsuccessful_outcome() const
  {
    assert_choice_type(types::unsuccessful_outcome, type_, "E2AP-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  init_msg_s&             set_init_msg();
  successful_outcome_s&   set_successful_outcome();
  unsuccessful_outcome_s& set_unsuccessful_outcome();

private:
  types                                                                     type_;
  choice_buffer_t<init_msg_s, successful_outcome_s, unsuccessful_outcome_s> c;

  void destroy_();
};

} // namespace e2ap
} // namespace asn1

extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ric_action_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2conn_setup_failed_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2conn_upd_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2conn_upd_rem_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2node_component_cfg_addition_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2node_component_cfg_addition_ack_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2node_component_cfg_removal_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2node_component_cfg_removal_ack_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2node_component_cfg_upd_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2node_component_cfg_upd_ack_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2node_tnl_assoc_removal_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ran_function_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ran_function_id_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ran_function_id_cause_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ric_action_admitted_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ric_action_not_admitted_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ric_action_added_for_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ric_action_confirmed_for_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ric_action_confirmed_for_removal_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::e2ap::ric_action_failed_to_be_added_for_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::e2ap::ric_action_failed_to_be_modified_for_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::e2ap::ric_action_failed_to_be_remd_for_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ric_action_modified_for_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ric_action_refused_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ric_action_refused_to_be_remd_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ric_action_remd_for_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ric_action_required_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ric_action_required_to_be_remd_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ric_action_to_be_added_for_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ric_action_to_be_modified_for_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ric_action_to_be_remd_for_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ric_sub_with_cause_item_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2_removal_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2_removal_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2conn_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2conn_upd_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2conn_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2node_cfg_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2node_cfg_upd_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2node_cfg_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::error_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_query_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_query_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_query_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_ctrl_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_ctrl_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_ctrl_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_service_query_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_service_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_service_upd_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_service_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_sub_delete_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_sub_delete_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_sub_delete_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_sub_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_sub_mod_confirm_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_sub_mod_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_sub_mod_refuse_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_sub_mod_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_sub_mod_required_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_sub_mod_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_sub_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ric_sub_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::reset_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::reset_resp_ies_o>;
