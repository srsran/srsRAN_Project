/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/asn1/asn1_utils.h"

namespace asn1 {
namespace e1ap {

/*******************************************************************************
 *                             Constant Definitions
 ******************************************************************************/

#define ASN1_E1AP_ID_RESET 0
#define ASN1_E1AP_ID_ERROR_IND 1
#define ASN1_E1AP_ID_PRIVATE_MSG 2
#define ASN1_E1AP_ID_GNB_CU_UP_E1_SETUP 3
#define ASN1_E1AP_ID_GNB_CU_CP_E1_SETUP 4
#define ASN1_E1AP_ID_GNB_CU_UP_CFG_UPD 5
#define ASN1_E1AP_ID_GNB_CU_CP_CFG_UPD 6
#define ASN1_E1AP_ID_E1_RELEASE 7
#define ASN1_E1AP_ID_BEARER_CONTEXT_SETUP 8
#define ASN1_E1AP_ID_BEARER_CONTEXT_MOD 9
#define ASN1_E1AP_ID_BEARER_CONTEXT_MOD_REQUIRED 10
#define ASN1_E1AP_ID_BEARER_CONTEXT_RELEASE 11
#define ASN1_E1AP_ID_BEARER_CONTEXT_RELEASE_REQUEST 12
#define ASN1_E1AP_ID_BEARER_CONTEXT_INACTIVITY_NOTIF 13
#define ASN1_E1AP_ID_D_L_DATA_NOTIF 14
#define ASN1_E1AP_ID_DATA_USAGE_REPORT 15
#define ASN1_E1AP_ID_GNB_CU_UP_COUNTER_CHECK 16
#define ASN1_E1AP_ID_GNB_CU_UP_STATUS_IND 17
#define ASN1_E1AP_ID_U_L_DATA_NOTIF 18
#define ASN1_E1AP_ID_M_RDC_DATA_USAGE_REPORT 19
#define ASN1_E1AP_ID_TRACE_START 20
#define ASN1_E1AP_ID_DEACTIV_TRACE 21
#define ASN1_E1AP_ID_RES_STATUS_REPORT_INITIATION 22
#define ASN1_E1AP_ID_RES_STATUS_REPORT 23
#define ASN1_E1AP_ID_I_AB_UP_TNL_ADDRESS_UPD 24
#define ASN1_E1AP_ID_CELL_TRAFFIC_TRACE 25
#define ASN1_E1AP_ID_EARLY_FORWARDING_SN_TRANSFER 26
#define ASN1_E1AP_ID_GNB_CU_CP_MEAS_RESULTS_INFO 27
#define ASN1_E1AP_ID_I_ABPSK_NOTIF 28
#define ASN1_E1AP_MAXNOOF_ERRORS 256
#define ASN1_E1AP_MAXNOOF_SPLMNS 12
#define ASN1_E1AP_MAXNOOF_SLICE_ITEMS 1024
#define ASN1_E1AP_MAXNOOF_INDIVIDUAL_E1_CONNS_TO_RESET 65536
#define ASN1_E1AP_MAXNOOF_EUTRAN_QOS_PARAMS 256
#define ASN1_E1AP_MAXNOOF_NGRANQOS_PARAMS 256
#define ASN1_E1AP_MAXNOOF_DRBS 32
#define ASN1_E1AP_MAXNOOF_NR_CGI 512
#define ASN1_E1AP_MAXNOOF_PDU_SESSION_RES 256
#define ASN1_E1AP_MAXNOOF_QOS_FLOWS 64
#define ASN1_E1AP_MAXNOOF_UP_PARAMS 8
#define ASN1_E1AP_MAXNOOF_CELL_GROUPS 4
#define ASN1_E1AP_MAXNOOFTIMEPERIODS 2
#define ASN1_E1AP_MAXNOOF_TNL_ASSOCS 32
#define ASN1_E1AP_MAXNOOF_TLAS 16
#define ASN1_E1AP_MAXNOOF_GTP_TLAS 16
#define ASN1_E1AP_MAXNOOF_TNL_ADDRESSES 8
#define ASN1_E1AP_MAXNOOF_MDT_PLMNS 16
#define ASN1_E1AP_MAXNOOF_QOS_PARA_SETS 8
#define ASN1_E1AP_MAXNOOF_EXT_SLICE_ITEMS 65535
#define ASN1_E1AP_MAXNOOF_DATA_FORWARDING_TUNNELTO_E_UTRAN 256
#define ASN1_E1AP_MAXNOOF_EXT_NR_CGI 16384
#define ASN1_E1AP_MAXNOOF_PS_KS 256
#define ASN1_E1AP_ID_CAUSE 0
#define ASN1_E1AP_ID_CRIT_DIAGNOSTICS 1
#define ASN1_E1AP_ID_GNB_CU_CP_UE_E1AP_ID 2
#define ASN1_E1AP_ID_GNB_CU_UP_UE_E1AP_ID 3
#define ASN1_E1AP_ID_RESET_TYPE 4
#define ASN1_E1AP_ID_UE_ASSOCIATED_LC_E1_CONN_ITEM 5
#define ASN1_E1AP_ID_UE_ASSOCIATED_LC_E1_CONN_LIST_RES_ACK 6
#define ASN1_E1AP_ID_GNB_CU_UP_ID 7
#define ASN1_E1AP_ID_GNB_CU_UP_NAME 8
#define ASN1_E1AP_ID_GNB_CU_CP_NAME 9
#define ASN1_E1AP_ID_CN_SUPPORT 10
#define ASN1_E1AP_ID_SUPPORTED_PLMNS 11
#define ASN1_E1AP_ID_TIME_TO_WAIT 12
#define ASN1_E1AP_ID_SECURITY_INFO 13
#define ASN1_E1AP_ID_UE_DL_AGGR_MAX_BIT_RATE 14
#define ASN1_E1AP_ID_SYS_BEARER_CONTEXT_SETUP_REQUEST 15
#define ASN1_E1AP_ID_SYS_BEARER_CONTEXT_SETUP_RESP 16
#define ASN1_E1AP_ID_BEARER_CONTEXT_STATUS_CHANGE 17
#define ASN1_E1AP_ID_SYS_BEARER_CONTEXT_MOD_REQUEST 18
#define ASN1_E1AP_ID_SYS_BEARER_CONTEXT_MOD_RESP 19
#define ASN1_E1AP_ID_SYS_BEARER_CONTEXT_MOD_CONFIRM 20
#define ASN1_E1AP_ID_SYS_BEARER_CONTEXT_MOD_REQUIRED 21
#define ASN1_E1AP_ID_DRB_STATUS_LIST 22
#define ASN1_E1AP_ID_ACTIVITY_NOTIF_LEVEL 23
#define ASN1_E1AP_ID_ACTIVITY_INFO 24
#define ASN1_E1AP_ID_DATA_USAGE_REPORT_LIST 25
#define ASN1_E1AP_ID_NEW_UL_TNL_INFO_REQUIRED 26
#define ASN1_E1AP_ID_GNB_CU_CP_TNL_A_TO_ADD_LIST 27
#define ASN1_E1AP_ID_GNB_CU_CP_TNL_A_TO_REM_LIST 28
#define ASN1_E1AP_ID_GNB_CU_CP_TNL_A_TO_UPD_LIST 29
#define ASN1_E1AP_ID_GNB_CU_CP_TNL_A_SETUP_LIST 30
#define ASN1_E1AP_ID_GNB_CU_CP_TNL_A_FAILED_TO_SETUP_LIST 31
#define ASN1_E1AP_ID_DRB_TO_SETUP_LIST_EUTRAN 32
#define ASN1_E1AP_ID_DRB_TO_MODIFY_LIST_EUTRAN 33
#define ASN1_E1AP_ID_DRB_TO_REM_LIST_EUTRAN 34
#define ASN1_E1AP_ID_DRB_REQUIRED_TO_MODIFY_LIST_EUTRAN 35
#define ASN1_E1AP_ID_DRB_REQUIRED_TO_REM_LIST_EUTRAN 36
#define ASN1_E1AP_ID_DRB_SETUP_LIST_EUTRAN 37
#define ASN1_E1AP_ID_DRB_FAILED_LIST_EUTRAN 38
#define ASN1_E1AP_ID_DRB_MODIFIED_LIST_EUTRAN 39
#define ASN1_E1AP_ID_DRB_FAILED_TO_MODIFY_LIST_EUTRAN 40
#define ASN1_E1AP_ID_DRB_CONFIRM_MODIFIED_LIST_EUTRAN 41
#define ASN1_E1AP_ID_PDU_SESSION_RES_TO_SETUP_LIST 42
#define ASN1_E1AP_ID_PDU_SESSION_RES_TO_MODIFY_LIST 43
#define ASN1_E1AP_ID_PDU_SESSION_RES_TO_REM_LIST 44
#define ASN1_E1AP_ID_PDU_SESSION_RES_REQUIRED_TO_MODIFY_LIST 45
#define ASN1_E1AP_ID_PDU_SESSION_RES_SETUP_LIST 46
#define ASN1_E1AP_ID_PDU_SESSION_RES_FAILED_LIST 47
#define ASN1_E1AP_ID_PDU_SESSION_RES_MODIFIED_LIST 48
#define ASN1_E1AP_ID_PDU_SESSION_RES_FAILED_TO_MODIFY_LIST 49
#define ASN1_E1AP_ID_PDU_SESSION_RES_CONFIRM_MODIFIED_LIST 50
#define ASN1_E1AP_ID_DRB_TO_SETUP_MOD_LIST_EUTRAN 51
#define ASN1_E1AP_ID_DRB_SETUP_MOD_LIST_EUTRAN 52
#define ASN1_E1AP_ID_DRB_FAILED_MOD_LIST_EUTRAN 53
#define ASN1_E1AP_ID_PDU_SESSION_RES_SETUP_MOD_LIST 54
#define ASN1_E1AP_ID_PDU_SESSION_RES_FAILED_MOD_LIST 55
#define ASN1_E1AP_ID_PDU_SESSION_RES_TO_SETUP_MOD_LIST 56
#define ASN1_E1AP_ID_TRANSACTION_ID 57
#define ASN1_E1AP_ID_SERVING_PLMN 58
#define ASN1_E1AP_ID_UE_INACTIVITY_TIMER 59
#define ASN1_E1AP_ID_SYS_GNB_CU_UP_COUNTER_CHECK_REQUEST 60
#define ASN1_E1AP_ID_DRBS_SUBJECT_TO_COUNTER_CHECK_LIST_EUTRAN 61
#define ASN1_E1AP_ID_DRBS_SUBJECT_TO_COUNTER_CHECK_LIST_NG_RAN 62
#define ASN1_E1AP_ID_PPI 63
#define ASN1_E1AP_ID_GNB_CU_UP_CAPACITY 64
#define ASN1_E1AP_ID_GNB_CU_UP_OVERLOAD_INFO 65
#define ASN1_E1AP_ID_UE_DL_MAX_INTEGRITY_PROTECTED_DATA_RATE 66
#define ASN1_E1AP_ID_PDU_SESSION_TO_NOTIFY_LIST 67
#define ASN1_E1AP_ID_PDU_SESSION_RES_DATA_USAGE_LIST 68
#define ASN1_E1AP_ID_SNSSAI 69
#define ASN1_E1AP_ID_DATA_DISCARD_REQUIRED 70
#define ASN1_E1AP_ID_OLD_QOS_FLOW_MAP_UL_ENDMARKEREXPECTED 71
#define ASN1_E1AP_ID_DRB_QOS 72
#define ASN1_E1AP_ID_GNB_CU_UP_TNL_A_TO_REM_LIST 73
#define ASN1_E1AP_ID_ENDPOINT_IP_ADDRESS_AND_PORT 74
#define ASN1_E1AP_ID_TNL_ASSOC_TRANSPORT_LAYER_ADDRESS_GNB_CU_UP 75
#define ASN1_E1AP_ID_RAN_UE_ID 76
#define ASN1_E1AP_ID_GNB_DU_ID 77
#define ASN1_E1AP_ID_COMMON_NETWORK_INSTANCE 78
#define ASN1_E1AP_ID_NETWORK_INSTANCE 79
#define ASN1_E1AP_ID_QOS_FLOW_MAP_IND 80
#define ASN1_E1AP_ID_TRACE_ACTIVATION 81
#define ASN1_E1AP_ID_TRACE_ID 82
#define ASN1_E1AP_ID_SUB_PROFILE_ID_FOR_RFP 83
#define ASN1_E1AP_ID_ADD_RRM_PRIO_IDX 84
#define ASN1_E1AP_ID_RETAINABILITY_MEASS_INFO 85
#define ASN1_E1AP_ID_TRANSPORT_LAYER_ADDRESS_INFO 86
#define ASN1_E1AP_ID_QOS_MONITORING_REQUEST 87
#define ASN1_E1AP_ID_PDCP_STATUS_REPORT_IND 88
#define ASN1_E1AP_ID_GNB_CU_CP_MEAS_ID 89
#define ASN1_E1AP_ID_GNB_CU_UP_MEAS_ID 90
#define ASN1_E1AP_ID_REGIST_REQUEST 91
#define ASN1_E1AP_ID_REPORT_CHARACTERISTICS 92
#define ASN1_E1AP_ID_REPORT_PERIODICITY 93
#define ASN1_E1AP_ID_TNL_AVAILABLE_CAPACITY_IND 94
#define ASN1_E1AP_ID_HW_CAPACITY_IND 95
#define ASN1_E1AP_ID_REDUNDANT_COMMON_NETWORK_INSTANCE 96
#define ASN1_E1AP_ID_REDUNDANT_N_G_UL_UP_TNL_INFO 97
#define ASN1_E1AP_ID_REDUNDANT_N_G_DL_UP_TNL_INFO 98
#define ASN1_E1AP_ID_REDUNDANT_QOS_FLOW_IND 99
#define ASN1_E1AP_ID_TSC_TRAFFIC_CHARACTERISTICS 100
#define ASN1_E1AP_ID_CN_PACKET_DELAY_BUDGET_DL 101
#define ASN1_E1AP_ID_CN_PACKET_DELAY_BUDGET_UL 102
#define ASN1_E1AP_ID_EXTENDED_PACKET_DELAY_BUDGET 103
#define ASN1_E1AP_ID_ADD_PDCP_DUPL_INFO 104
#define ASN1_E1AP_ID_REDUNDANT_PDU_SESSION_INFO 105
#define ASN1_E1AP_ID_REDUNDANT_PDU_SESSION_INFO_USED 106
#define ASN1_E1AP_ID_QOS_MAP_INFO 107
#define ASN1_E1AP_ID_DL_UP_TNL_ADDRESS_TO_UPD_LIST 108
#define ASN1_E1AP_ID_UL_UP_TNL_ADDRESS_TO_UPD_LIST 109
#define ASN1_E1AP_ID_NPN_SUPPORT_INFO 110
#define ASN1_E1AP_ID_NPN_CONTEXT_INFO 111
#define ASN1_E1AP_ID_MDT_CFG 112
#define ASN1_E1AP_ID_MANAGEMENT_BASED_MDT_PLMN_LIST 113
#define ASN1_E1AP_ID_TRACE_COLLECTION_ENTITY_IP_ADDRESS 114
#define ASN1_E1AP_ID_PRIVACY_IND 115
#define ASN1_E1AP_ID_TRACE_COLLECTION_ENTITY_URI 116
#define ASN1_E1AP_ID_UR_IADDRESS 117
#define ASN1_E1AP_ID_EHC_PARAMS 118
#define ASN1_E1AP_ID_DRBS_SUBJECT_TO_EARLY_FORWARDING_LIST 119
#define ASN1_E1AP_ID_DAPS_REQUEST_INFO 120
#define ASN1_E1AP_ID_CHO_INITIATION 121
#define ASN1_E1AP_ID_EARLY_FORWARDING_COUNT_REQ 122
#define ASN1_E1AP_ID_EARLY_FORWARDING_COUNT_INFO 123
#define ASN1_E1AP_ID_ALT_QOS_PARA_SET_LIST 124
#define ASN1_E1AP_ID_EXTENDED_SLICE_SUPPORT_LIST 125
#define ASN1_E1AP_ID_MCG_OFFERED_GBR_QOS_FLOW_INFO 126
#define ASN1_E1AP_ID_NOF_TUNNELS 127
#define ASN1_E1AP_ID_DRB_MEAS_RESULTS_INFO_LIST 128
#define ASN1_E1AP_ID_EXTENDED_GNB_CU_CP_NAME 129
#define ASN1_E1AP_ID_EXTENDED_GNB_CU_UP_NAME 130
#define ASN1_E1AP_ID_DATA_FORWARDINGTO_E_UTRAN_INFO_LIST 131
#define ASN1_E1AP_ID_QOS_MONITORING_REPORT_FREQ 132
#define ASN1_E1AP_ID_QOS_MONITORING_DISABLED 133
#define ASN1_E1AP_ID_ADD_HO_INFO 134
#define ASN1_E1AP_ID_EXTENDED_NR_CGI_SUPPORT_LIST 135
#define ASN1_E1AP_ID_DATA_FORWARDINGTO_NG_RAN_QOS_FLOW_INFO_LIST 136
#define ASN1_E1AP_ID_MAX_C_ID_EHC_DL 137
#define ASN1_E1AP_ID_IGNORE_MAP_RULE_IND 138
#define ASN1_E1AP_ID_DIRECT_FORWARDING_PATH_AVAILABILITY 139
#define ASN1_E1AP_ID_EARLY_DATA_FORWARDING_IND 140
#define ASN1_E1AP_ID_QOS_FLOWS_DRB_REMAP 141
#define ASN1_E1AP_ID_DATA_FORWARDING_SOURCE_IP_ADDRESS 142
#define ASN1_E1AP_ID_SECURITY_IND_MODIFY 143
#define ASN1_E1AP_ID_IAB_DONOR_CU_UP_PSK_INFO 144
#define ASN1_E1AP_ID_DISCARD_TIMER_EXTENDED 177
#define ASN1_E1AP_MAX_PRIVATE_IES 65535
#define ASN1_E1AP_MAX_PROTOCOL_EXTS 65535
#define ASN1_E1AP_MAX_PROTOCOL_IES 65535

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// DRB-Activity ::= ENUMERATED
struct drb_activity_opts {
  enum options { active, not_active, /*...*/ nulltype } value;

  const char* to_string() const;
};
using drb_activity_e = enumerated<drb_activity_opts, true>;

// DRB-Activity-ItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_activity_item_ext_ies_o = protocol_ext_empty_o;

// PDU-Session-Resource-Activity ::= ENUMERATED
struct pdu_session_res_activity_opts {
  enum options { active, not_active, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pdu_session_res_activity_e = enumerated<pdu_session_res_activity_opts, true>;

// PDU-Session-Resource-Activity-ItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdu_session_res_activity_item_ext_ies_o = protocol_ext_empty_o;

using drb_activity_item_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Activity-Item ::= SEQUENCE
struct drb_activity_item_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  uint8_t                             drb_id          = 1;
  drb_activity_e                      drb_activity;
  drb_activity_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_activity_item_ext_ies_container = protocol_ext_container_empty_l;

// PDU-Session-Resource-Activity-Item ::= SEQUENCE
struct pdu_session_res_activity_item_s {
  bool                                            ext             = false;
  bool                                            ie_exts_present = false;
  uint16_t                                        pdu_session_id  = 0;
  pdu_session_res_activity_e                      pdu_session_res_activity;
  pdu_session_res_activity_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ActivityInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using activity_info_ext_ies_o = protocol_ies_empty_o;

// DRB-Activity-List ::= SEQUENCE (SIZE (1..32)) OF DRB-Activity-Item
using drb_activity_list_l = dyn_array<drb_activity_item_s>;

// PDU-Session-Resource-Activity-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-Activity-Item
using pdu_session_res_activity_list_l = dyn_array<pdu_session_res_activity_item_s>;

// UE-Activity ::= ENUMERATED
struct ue_activity_opts {
  enum options { active, not_active, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ue_activity_e = enumerated<ue_activity_opts, true>;

// ActivityInformation ::= CHOICE
struct activity_info_c {
  struct types_opts {
    enum options { drb_activity_list, pdu_session_res_activity_list, ue_activity, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  activity_info_c() = default;
  activity_info_c(const activity_info_c& other);
  activity_info_c& operator=(const activity_info_c& other);
  ~activity_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  drb_activity_list_l& drb_activity_list()
  {
    assert_choice_type(types::drb_activity_list, type_, "ActivityInformation");
    return c.get<drb_activity_list_l>();
  }
  pdu_session_res_activity_list_l& pdu_session_res_activity_list()
  {
    assert_choice_type(types::pdu_session_res_activity_list, type_, "ActivityInformation");
    return c.get<pdu_session_res_activity_list_l>();
  }
  ue_activity_e& ue_activity()
  {
    assert_choice_type(types::ue_activity, type_, "ActivityInformation");
    return c.get<ue_activity_e>();
  }
  protocol_ie_single_container_s<activity_info_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "ActivityInformation");
    return c.get<protocol_ie_single_container_s<activity_info_ext_ies_o>>();
  }
  const drb_activity_list_l& drb_activity_list() const
  {
    assert_choice_type(types::drb_activity_list, type_, "ActivityInformation");
    return c.get<drb_activity_list_l>();
  }
  const pdu_session_res_activity_list_l& pdu_session_res_activity_list() const
  {
    assert_choice_type(types::pdu_session_res_activity_list, type_, "ActivityInformation");
    return c.get<pdu_session_res_activity_list_l>();
  }
  const ue_activity_e& ue_activity() const
  {
    assert_choice_type(types::ue_activity, type_, "ActivityInformation");
    return c.get<ue_activity_e>();
  }
  const protocol_ie_single_container_s<activity_info_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "ActivityInformation");
    return c.get<protocol_ie_single_container_s<activity_info_ext_ies_o>>();
  }
  drb_activity_list_l&                                     set_drb_activity_list();
  pdu_session_res_activity_list_l&                         set_pdu_session_res_activity_list();
  ue_activity_e&                                           set_ue_activity();
  protocol_ie_single_container_s<activity_info_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<drb_activity_list_l,
                  pdu_session_res_activity_list_l,
                  protocol_ie_single_container_s<activity_info_ext_ies_o>>
      c;

  void destroy_();
};

// PacketErrorRate-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using packet_error_rate_ext_ies_o = protocol_ext_empty_o;

// AlternativeQoSParaSetItem-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using alt_qos_para_set_item_ext_ies_o = protocol_ext_empty_o;

using packet_error_rate_ext_ies_container = protocol_ext_container_empty_l;

// PacketErrorRate ::= SEQUENCE
struct packet_error_rate_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  uint8_t                             per_scalar      = 0;
  uint8_t                             per_exponent    = 0;
  packet_error_rate_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using alt_qos_para_set_item_ext_ies_container = protocol_ext_container_empty_l;

// AlternativeQoSParaSetItem ::= SEQUENCE
struct alt_qos_para_set_item_s {
  bool                                    ext                                 = false;
  bool                                    guaranteed_flow_bit_rate_dl_present = false;
  bool                                    guaranteed_flow_bit_rate_ul_present = false;
  bool                                    packet_delay_budget_present         = false;
  bool                                    packet_error_rate_present           = false;
  bool                                    ie_exts_present                     = false;
  uint8_t                                 alt_qos_param_idx                   = 1;
  uint64_t                                guaranteed_flow_bit_rate_dl         = 0;
  uint64_t                                guaranteed_flow_bit_rate_ul         = 0;
  uint16_t                                packet_delay_budget                 = 0;
  packet_error_rate_s                     packet_error_rate;
  alt_qos_para_set_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AlternativeQoSParaSetList ::= SEQUENCE (SIZE (1..8)) OF AlternativeQoSParaSetItem
using alt_qos_para_set_list_l = dyn_array<alt_qos_para_set_item_s>;

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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                            type_;
    choice_buffer_t<activity_info_c> c;

    void destroy_();
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

// Cell-Group-Information-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct cell_group_info_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { nof_tunnels, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::nof_tunnels; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t&       nof_tunnels() { return c; }
    const uint8_t& nof_tunnels() const { return c; }

  private:
    uint8_t c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DL-TX-Stop ::= ENUMERATED
struct dl_tx_stop_opts {
  enum options { stop, resume, /*...*/ nulltype } value;

  const char* to_string() const;
};
using dl_tx_stop_e = enumerated<dl_tx_stop_opts, true>;

// RAT-Type ::= ENUMERATED
struct rat_type_opts {
  enum options { e_utra, nr, /*...*/ nulltype } value;

  const char* to_string() const;
};
using rat_type_e = enumerated<rat_type_opts, true>;

// UL-Configuration ::= ENUMERATED
struct ul_cfg_opts {
  enum options { no_data, shared, only, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ul_cfg_e = enumerated<ul_cfg_opts, true>;

// Cell-Group-Information-Item ::= SEQUENCE
struct cell_group_info_item_s {
  bool                                                     ext                = false;
  bool                                                     ul_cfg_present     = false;
  bool                                                     dl_tx_stop_present = false;
  bool                                                     rat_type_present   = false;
  uint8_t                                                  cell_group_id      = 0;
  ul_cfg_e                                                 ul_cfg;
  dl_tx_stop_e                                             dl_tx_stop;
  rat_type_e                                               rat_type;
  protocol_ext_container_l<cell_group_info_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Cell-Group-Information ::= SEQUENCE (SIZE (1..4)) OF Cell-Group-Information-Item
using cell_group_info_l = dyn_array<cell_group_info_item_s>;

// DRB-Confirm-Modified-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_confirm_modified_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

using drb_confirm_modified_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Confirm-Modified-Item-NG-RAN ::= SEQUENCE
struct drb_confirm_modified_item_ng_ran_s {
  bool                                               ext             = false;
  bool                                               ie_exts_present = false;
  uint8_t                                            drb_id          = 1;
  cell_group_info_l                                  cell_group_info;
  drb_confirm_modified_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Confirm-Modified-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_confirm_modified_item_eutran_ext_ies_o = protocol_ext_empty_o;

// DRB-Confirm-Modified-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Confirm-Modified-Item-NG-RAN
using drb_confirm_modified_list_ng_ran_l = dyn_array<drb_confirm_modified_item_ng_ran_s>;

// PDU-Session-Resource-Confirm-Modified-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdu_session_res_confirm_modified_item_ext_ies_o = protocol_ext_empty_o;

using drb_confirm_modified_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Confirm-Modified-Item-EUTRAN ::= SEQUENCE
struct drb_confirm_modified_item_eutran_s {
  bool                                               ext             = false;
  bool                                               ie_exts_present = false;
  uint8_t                                            drb_id          = 1;
  cell_group_info_l                                  cell_group_info;
  drb_confirm_modified_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_confirm_modified_item_ext_ies_container = protocol_ext_container_empty_l;

// PDU-Session-Resource-Confirm-Modified-Item ::= SEQUENCE
struct pdu_session_res_confirm_modified_item_s {
  bool                                                    ext             = false;
  bool                                                    ie_exts_present = false;
  uint16_t                                                pdu_session_id  = 0;
  drb_confirm_modified_list_ng_ran_l                      drb_confirm_modified_list_ng_ran;
  pdu_session_res_confirm_modified_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Confirm-Modified-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Confirm-Modified-Item-EUTRAN
using drb_confirm_modified_list_eutran_l = dyn_array<drb_confirm_modified_item_eutran_s>;

// PDU-Session-Resource-Confirm-Modified-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-Confirm-Modified-Item
using pdu_session_res_confirm_modified_list_l = dyn_array<pdu_session_res_confirm_modified_item_s>;

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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                             type_;
    choice_buffer_t<sys_bearer_context_mod_confirm_c> c;

    void destroy_();
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

// CriticalityDiagnostics-IE-List-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using crit_diagnostics_ie_list_ext_ies_o = protocol_ext_empty_o;

// TypeOfError ::= ENUMERATED
struct type_of_error_opts {
  enum options { not_understood, missing, /*...*/ nulltype } value;

  const char* to_string() const;
};
using type_of_error_e = enumerated<type_of_error_opts, true>;

// Cause-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using cause_ext_ies_o = protocol_ies_empty_o;

// CauseMisc ::= ENUMERATED
struct cause_misc_opts {
  enum options {
    ctrl_processing_overload,
    not_enough_user_plane_processing_res,
    hardware_fail,
    om_intervention,
    unspecified,
    // ...
    nulltype
  } value;

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

// CauseRadioNetwork ::= ENUMERATED
struct cause_radio_network_opts {
  enum options {
    unspecified,
    unknown_or_already_allocated_gnb_cu_cp_ue_e1ap_id,
    unknown_or_already_allocated_gnb_cu_up_ue_e1ap_id,
    unknown_or_inconsistent_pair_of_ue_e1ap_id,
    interaction_with_other_proc,
    ppdcp_count_wrap_around,
    not_supported_qci_value,
    not_supported_5qi_value,
    encryption_algorithms_not_supported,
    integrity_protection_algorithms_not_supported,
    up_integrity_protection_not_possible,
    up_confidentiality_protection_not_possible,
    multiple_pdu_session_id_instances,
    unknown_pdu_session_id,
    multiple_qos_flow_id_instances,
    unknown_qos_flow_id,
    multiple_drb_id_instances,
    unknown_drb_id,
    invalid_qos_combination,
    proc_cancelled,
    normal_release,
    no_radio_res_available,
    action_desirable_for_radio_reasons,
    res_not_available_for_the_slice,
    pdcp_cfg_not_supported,
    // ...
    ue_dl_max_ip_data_rate_reason,
    up_integrity_protection_fail,
    release_due_to_pre_emption,
    rsn_not_available_for_the_up,
    npn_not_supported,
    report_characteristic_empty,
    existing_meas_id,
    meas_temporarily_not_available,
    meas_not_supported_for_the_obj,
    nulltype
  } value;

  const char* to_string() const;
};
using cause_radio_network_e = enumerated<cause_radio_network_opts, true, 9>;

// CauseTransport ::= ENUMERATED
struct cause_transport_opts {
  enum options { unspecified, transport_res_unavailable, /*...*/ unknown_tnl_address_for_iab, nulltype } value;

  const char* to_string() const;
};
using cause_transport_e = enumerated<cause_transport_opts, true, 1>;

// CriticalityDiagnostics-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using crit_diagnostics_ext_ies_o = protocol_ext_empty_o;

using crit_diagnostics_ie_list_ext_ies_container = protocol_ext_container_empty_l;

struct crit_diagnostics_ie_list_item_s_ {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  crit_e                                     ie_crit;
  uint32_t                                   ie_id = 0;
  type_of_error_e                            type_of_error;
  crit_diagnostics_ie_list_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CriticalityDiagnostics-IE-List ::= SEQUENCE (SIZE (1..256)) OF CriticalityDiagnostics-IE-List-item
using crit_diagnostics_ie_list_l = dyn_array<crit_diagnostics_ie_list_item_s_>;

// TriggeringMessage ::= ENUMERATED
struct trigger_msg_opts {
  enum options { init_msg, successful_outcome, unsuccessful_outcome, nulltype } value;

  const char* to_string() const;
};
using trigger_msg_e = enumerated<trigger_msg_opts>;

// Cause ::= CHOICE
struct cause_c {
  struct types_opts {
    enum options { radio_network, transport, protocol, misc, choice_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

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
  cause_radio_network_e& radio_network()
  {
    assert_choice_type(types::radio_network, type_, "Cause");
    return c.get<cause_radio_network_e>();
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
  protocol_ie_single_container_s<cause_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "Cause");
    return c.get<protocol_ie_single_container_s<cause_ext_ies_o>>();
  }
  const cause_radio_network_e& radio_network() const
  {
    assert_choice_type(types::radio_network, type_, "Cause");
    return c.get<cause_radio_network_e>();
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
  const protocol_ie_single_container_s<cause_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "Cause");
    return c.get<protocol_ie_single_container_s<cause_ext_ies_o>>();
  }
  cause_radio_network_e&                           set_radio_network();
  cause_transport_e&                               set_transport();
  cause_protocol_e&                                set_protocol();
  cause_misc_e&                                    set_misc();
  protocol_ie_single_container_s<cause_ext_ies_o>& set_choice_ext();

private:
  types                                                            type_;
  choice_buffer_t<protocol_ie_single_container_s<cause_ext_ies_o>> c;

  void destroy_();
};

using crit_diagnostics_ext_ies_container = protocol_ext_container_empty_l;

// CriticalityDiagnostics ::= SEQUENCE
struct crit_diagnostics_s {
  bool                               ext                    = false;
  bool                               proc_code_present      = false;
  bool                               trigger_msg_present    = false;
  bool                               proc_crit_present      = false;
  bool                               transaction_id_present = false;
  bool                               ie_exts_present        = false;
  uint16_t                           proc_code              = 0;
  trigger_msg_e                      trigger_msg;
  crit_e                             proc_crit;
  uint16_t                           transaction_id = 0;
  crit_diagnostics_ie_list_l         ies_crit_diagnostics;
  crit_diagnostics_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
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

// Dynamic5QIDescriptor-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct dyn_5qi_descriptor_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        extended_packet_delay_budget,
        cn_packet_delay_budget_dl,
        cn_packet_delay_budget_ul,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t&       extended_packet_delay_budget();
    uint32_t&       cn_packet_delay_budget_dl();
    uint32_t&       cn_packet_delay_budget_ul();
    const uint32_t& extended_packet_delay_budget() const;
    const uint32_t& cn_packet_delay_budget_dl() const;
    const uint32_t& cn_packet_delay_budget_ul() const;

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GBR-QosFlowInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct gbr_qos_flow_info_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { alt_qos_para_set_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::alt_qos_para_set_list; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    alt_qos_para_set_list_l&       alt_qos_para_set_list() { return c; }
    const alt_qos_para_set_list_l& alt_qos_para_set_list() const { return c; }

  private:
    alt_qos_para_set_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Non-Dynamic5QIDescriptor-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct non_dyn_5qi_descriptor_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { cn_packet_delay_budget_dl, cn_packet_delay_budget_ul, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t&       cn_packet_delay_budget_dl();
    uint32_t&       cn_packet_delay_budget_ul();
    const uint32_t& cn_packet_delay_budget_dl() const;
    const uint32_t& cn_packet_delay_budget_ul() const;

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TSCTrafficInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using tsc_traffic_info_ext_ies_o = protocol_ext_empty_o;

// DataForwardingtoNG-RANQoSFlowInformationList-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using data_forwardingto_ng_ran_qos_flow_info_list_item_ext_ies_o = protocol_ext_empty_o;

struct dyn_5qi_descriptor_ext_ies_container {
  bool     extended_packet_delay_budget_present = false;
  bool     cn_packet_delay_budget_dl_present    = false;
  bool     cn_packet_delay_budget_ul_present    = false;
  uint32_t extended_packet_delay_budget;
  uint32_t cn_packet_delay_budget_dl;
  uint32_t cn_packet_delay_budget_ul;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Dynamic5QIDescriptor ::= SEQUENCE
struct dyn_5qi_descriptor_s {
  struct delay_crit_opts {
    enum options { delay_crit, non_delay_crit, nulltype } value;

    const char* to_string() const;
  };
  using delay_crit_e_ = enumerated<delay_crit_opts>;

  // member variables
  bool                                 five_qi_present               = false;
  bool                                 delay_crit_present            = false;
  bool                                 averaging_win_present         = false;
  bool                                 max_data_burst_volume_present = false;
  bool                                 ie_exts_present               = false;
  uint8_t                              qos_prio_level                = 0;
  uint16_t                             packet_delay_budget           = 0;
  packet_error_rate_s                  packet_error_rate;
  uint16_t                             five_qi = 0;
  delay_crit_e_                        delay_crit;
  uint16_t                             averaging_win         = 0;
  uint16_t                             max_data_burst_volume = 0;
  dyn_5qi_descriptor_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EHC-Common-Parameters-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using ehc_common_params_ext_ies_o = protocol_ext_empty_o;

// EHC-Downlink-Parameters-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct ehc_dl_params_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { max_c_id_ehc_dl, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::max_c_id_ehc_dl; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&       max_c_id_ehc_dl() { return c; }
    const uint16_t& max_c_id_ehc_dl() const { return c; }

  private:
    uint16_t c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// EHC-Uplink-Parameters-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using ehc_ul_params_ext_ies_o = protocol_ext_empty_o;

// GBR-QoSFlowInformation ::= SEQUENCE
struct gbr_qos_flow_info_s {
  bool                                                  ext                             = false;
  bool                                                  max_packet_loss_rate_dl_present = false;
  bool                                                  max_packet_loss_rate_ul_present = false;
  uint64_t                                              max_flow_bit_rate_dl            = 0;
  uint64_t                                              max_flow_bit_rate_ul            = 0;
  uint64_t                                              guaranteed_flow_bit_rate_dl     = 0;
  uint64_t                                              guaranteed_flow_bit_rate_ul     = 0;
  uint16_t                                              max_packet_loss_rate_dl         = 0;
  uint16_t                                              max_packet_loss_rate_ul         = 0;
  protocol_ext_container_l<gbr_qos_flow_info_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GTPTunnel-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using gtp_tunnel_ext_ies_o = protocol_ext_empty_o;

// NGRANAllocationAndRetentionPriority-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using ngran_alloc_and_retention_prio_ext_ies_o = protocol_ext_empty_o;

struct non_dyn_5qi_descriptor_ext_ies_container {
  bool     cn_packet_delay_budget_dl_present = false;
  bool     cn_packet_delay_budget_ul_present = false;
  uint32_t cn_packet_delay_budget_dl;
  uint32_t cn_packet_delay_budget_ul;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Non-Dynamic5QIDescriptor ::= SEQUENCE
struct non_dyn_5qi_descriptor_s {
  bool                                     qos_prio_level_present        = false;
  bool                                     averaging_win_present         = false;
  bool                                     max_data_burst_volume_present = false;
  bool                                     ie_exts_present               = false;
  uint16_t                                 five_qi                       = 0;
  uint8_t                                  qos_prio_level                = 0;
  uint16_t                                 averaging_win                 = 0;
  uint16_t                                 max_data_burst_volume         = 0;
  non_dyn_5qi_descriptor_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-Count-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdcp_count_ext_ies_o = protocol_ext_empty_o;

// Pre-emptionCapability ::= ENUMERATED
struct pre_emption_cap_opts {
  enum options { shall_not_trigger_pre_emption, may_trigger_pre_emption, nulltype } value;

  const char* to_string() const;
};
using pre_emption_cap_e = enumerated<pre_emption_cap_opts>;

// Pre-emptionVulnerability ::= ENUMERATED
struct pre_emption_vulnerability_opts {
  enum options { not_pre_emptable, pre_emptable, nulltype } value;

  const char* to_string() const;
};
using pre_emption_vulnerability_e = enumerated<pre_emption_vulnerability_opts>;

// QoS-Characteristics-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using qos_characteristics_ext_ies_o = protocol_ies_empty_o;

// QoS-Flow-Mapping-Indication ::= ENUMERATED
struct qos_flow_map_ind_opts {
  enum options { ul, dl, /*...*/ nulltype } value;

  const char* to_string() const;
};
using qos_flow_map_ind_e = enumerated<qos_flow_map_ind_opts, true>;

// QosMonitoringDisabled ::= ENUMERATED
struct qos_monitoring_disabled_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using qos_monitoring_disabled_e = enumerated<qos_monitoring_disabled_opts, true>;

// QosMonitoringRequest ::= ENUMERATED
struct qos_monitoring_request_opts {
  enum options { ul, dl, both, nulltype } value;

  const char* to_string() const;
};
using qos_monitoring_request_e = enumerated<qos_monitoring_request_opts>;

// TSCTrafficCharacteristics-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using tsc_traffic_characteristics_ext_ies_o = protocol_ext_empty_o;

using tsc_traffic_info_ext_ies_container = protocol_ext_container_empty_l;

// TSCTrafficInformation ::= SEQUENCE
struct tsc_traffic_info_s {
  bool                               ie_exts_present = false;
  uint32_t                           periodicity     = 1;
  unbounded_octstring<true>          burst_arrival_time;
  tsc_traffic_info_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLDiscarding-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using dl_discarding_ext_ies_o = protocol_ext_empty_o;

using data_forwardingto_ng_ran_qos_flow_info_list_item_ext_ies_container = protocol_ext_container_empty_l;

// DataForwardingtoNG-RANQoSFlowInformationList-Item ::= SEQUENCE
struct data_forwardingto_ng_ran_qos_flow_info_list_item_s {
  bool                                                               ext             = false;
  bool                                                               ie_exts_present = false;
  uint8_t                                                            qos_flow_id     = 0;
  data_forwardingto_ng_ran_qos_flow_info_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using ehc_common_params_ext_ies_container = protocol_ext_container_empty_l;

// EHC-Common-Parameters ::= SEQUENCE
struct ehc_common_params_s {
  struct ehc_c_id_len_opts {
    enum options { bits7, bits15, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ehc_c_id_len_e_ = enumerated<ehc_c_id_len_opts, true>;

  // member variables
  bool                                ie_exts_present = false;
  ehc_c_id_len_e_                     ehc_c_id_len;
  ehc_common_params_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EHC-Downlink-Parameters ::= SEQUENCE
struct ehc_dl_params_s {
  struct drb_continue_ehc_dl_opts {
    enum options { true_value, /*...*/ false_value, nulltype } value;

    const char* to_string() const;
  };
  using drb_continue_ehc_dl_e_ = enumerated<drb_continue_ehc_dl_opts, true, 1>;

  // member variables
  drb_continue_ehc_dl_e_                            drb_continue_ehc_dl;
  protocol_ext_container_l<ehc_dl_params_ext_ies_o> ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EHC-Parameters-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using ehc_params_ext_ies_o = protocol_ext_empty_o;

using ehc_ul_params_ext_ies_container = protocol_ext_container_empty_l;

// EHC-Uplink-Parameters ::= SEQUENCE
struct ehc_ul_params_s {
  struct drb_continue_ehc_ul_opts {
    enum options { true_value, /*...*/ false_value, nulltype } value;

    const char* to_string() const;
  };
  using drb_continue_ehc_ul_e_ = enumerated<drb_continue_ehc_ul_opts, true, 1>;

  // member variables
  bool                            ie_exts_present = false;
  drb_continue_ehc_ul_e_          drb_continue_ehc_ul;
  ehc_ul_params_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FirstDLCount-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using first_dl_count_ext_ies_o = protocol_ext_empty_o;

using gtp_tunnel_ext_ies_container = protocol_ext_container_empty_l;

// GTPTunnel ::= SEQUENCE
struct gtp_tunnel_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  bounded_bitstring<1, 160, true, true> transport_layer_address;
  fixed_octstring<4, true>              gtp_teid;
  gtp_tunnel_ext_ies_container          ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using ngran_alloc_and_retention_prio_ext_ies_container = protocol_ext_container_empty_l;

// NGRANAllocationAndRetentionPriority ::= SEQUENCE
struct ngran_alloc_and_retention_prio_s {
  bool                                             ie_exts_present = false;
  uint8_t                                          prio_level      = 0;
  pre_emption_cap_e                                pre_emption_cap;
  pre_emption_vulnerability_e                      pre_emption_vulnerability;
  ngran_alloc_and_retention_prio_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdcp_count_ext_ies_container = protocol_ext_container_empty_l;

// PDCP-Count ::= SEQUENCE
struct pdcp_count_s {
  bool                         ext             = false;
  bool                         ie_exts_present = false;
  uint32_t                     pdcp_sn         = 0;
  uint64_t                     hfn             = 0;
  pdcp_count_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoS-Characteristics ::= CHOICE
struct qos_characteristics_c {
  struct types_opts {
    enum options { non_dyn_5qi, dyn_5qi, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  qos_characteristics_c() = default;
  qos_characteristics_c(const qos_characteristics_c& other);
  qos_characteristics_c& operator=(const qos_characteristics_c& other);
  ~qos_characteristics_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  non_dyn_5qi_descriptor_s& non_dyn_5qi()
  {
    assert_choice_type(types::non_dyn_5qi, type_, "QoS-Characteristics");
    return c.get<non_dyn_5qi_descriptor_s>();
  }
  dyn_5qi_descriptor_s& dyn_5qi()
  {
    assert_choice_type(types::dyn_5qi, type_, "QoS-Characteristics");
    return c.get<dyn_5qi_descriptor_s>();
  }
  protocol_ie_single_container_s<qos_characteristics_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "QoS-Characteristics");
    return c.get<protocol_ie_single_container_s<qos_characteristics_ext_ies_o>>();
  }
  const non_dyn_5qi_descriptor_s& non_dyn_5qi() const
  {
    assert_choice_type(types::non_dyn_5qi, type_, "QoS-Characteristics");
    return c.get<non_dyn_5qi_descriptor_s>();
  }
  const dyn_5qi_descriptor_s& dyn_5qi() const
  {
    assert_choice_type(types::dyn_5qi, type_, "QoS-Characteristics");
    return c.get<dyn_5qi_descriptor_s>();
  }
  const protocol_ie_single_container_s<qos_characteristics_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "QoS-Characteristics");
    return c.get<protocol_ie_single_container_s<qos_characteristics_ext_ies_o>>();
  }
  non_dyn_5qi_descriptor_s&                                      set_non_dyn_5qi();
  dyn_5qi_descriptor_s&                                          set_dyn_5qi();
  protocol_ie_single_container_s<qos_characteristics_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<dyn_5qi_descriptor_s,
                  non_dyn_5qi_descriptor_s,
                  protocol_ie_single_container_s<qos_characteristics_ext_ies_o>>
      c;

  void destroy_();
};

// QoS-Flow-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct qos_flow_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { qos_flow_map_ind, data_forwarding_source_ip_address, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    qos_flow_map_ind_e&                          qos_flow_map_ind();
    bounded_bitstring<1, 160, true, true>&       data_forwarding_source_ip_address();
    const qos_flow_map_ind_e&                    qos_flow_map_ind() const;
    const bounded_bitstring<1, 160, true, true>& data_forwarding_source_ip_address() const;

  private:
    types                                                  type_;
    choice_buffer_t<bounded_bitstring<1, 160, true, true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// QoS-Flow-Mapping-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using qos_flow_map_item_ext_ies_o = protocol_ext_empty_o;

// QoS-Flows-to-be-forwarded-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using qos_flows_to_be_forwarded_item_ext_ies_o = protocol_ext_empty_o;

// QoS-Mapping-Information ::= SEQUENCE
struct qos_map_info_s {
  bool                             ext                = false;
  bool                             dscp_present       = false;
  bool                             flow_label_present = false;
  fixed_bitstring<6, false, true>  dscp;
  fixed_bitstring<20, false, true> flow_label;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoSFlowLevelQoSParameters-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct qos_flow_level_qos_params_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        qos_monitoring_request,
        mcg_offered_gbr_qos_flow_info,
        qos_monitoring_report_freq,
        qos_monitoring_disabled,
        data_forwarding_source_ip_address,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    qos_monitoring_request_e&                    qos_monitoring_request();
    gbr_qos_flow_info_s&                         mcg_offered_gbr_qos_flow_info();
    uint16_t&                                    qos_monitoring_report_freq();
    qos_monitoring_disabled_e&                   qos_monitoring_disabled();
    bounded_bitstring<1, 160, true, true>&       data_forwarding_source_ip_address();
    const qos_monitoring_request_e&              qos_monitoring_request() const;
    const gbr_qos_flow_info_s&                   mcg_offered_gbr_qos_flow_info() const;
    const uint16_t&                              qos_monitoring_report_freq() const;
    const qos_monitoring_disabled_e&             qos_monitoring_disabled() const;
    const bounded_bitstring<1, 160, true, true>& data_forwarding_source_ip_address() const;

  private:
    types                                                                       type_;
    choice_buffer_t<bounded_bitstring<1, 160, true, true>, gbr_qos_flow_info_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ROHC-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using rohc_ext_ies_o = protocol_ext_empty_o;

// RedundantQoSFlowIndicator ::= ENUMERATED
struct redundant_qos_flow_ind_opts {
  enum options { true_value, false_value, nulltype } value;

  const char* to_string() const;
};
using redundant_qos_flow_ind_e = enumerated<redundant_qos_flow_ind_opts>;

using tsc_traffic_characteristics_ext_ies_container = protocol_ext_container_empty_l;

// TSCTrafficCharacteristics ::= SEQUENCE
struct tsc_traffic_characteristics_s {
  bool                                          tsc_traffic_characteristics_ul_present = false;
  bool                                          tsc_traffic_characteristics_dl_present = false;
  bool                                          ie_exts_present                        = false;
  tsc_traffic_info_s                            tsc_traffic_characteristics_ul;
  tsc_traffic_info_s                            tsc_traffic_characteristics_dl;
  tsc_traffic_characteristics_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UP-TNL-Information-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using up_tnl_info_ext_ies_o = protocol_ies_empty_o;

// UplinkOnlyROHC-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using ul_only_rohc_ext_ies_o = protocol_ext_empty_o;

// AdditionalPDCPduplicationInformation ::= ENUMERATED
struct add_pdcp_dupl_info_opts {
  enum options { three, four, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using add_pdcp_dupl_info_e = enumerated<add_pdcp_dupl_info_opts, true>;

// DAPSRequestInfo-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using daps_request_info_ext_ies_o = protocol_ext_empty_o;

using dl_discarding_ext_ies_container = protocol_ext_container_empty_l;

// DLDiscarding ::= SEQUENCE
struct dl_discarding_s {
  bool                            ie_exts_present = false;
  pdcp_count_s                    dl_discarding_count_val;
  dl_discarding_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBBStatusTransfer-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_b_status_transfer_ext_ies_o = protocol_ext_empty_o;

// DataForwardingtoNG-RANQoSFlowInformationList ::= SEQUENCE (SIZE (1..64)) OF
// DataForwardingtoNG-RANQoSFlowInformationList-Item
using data_forwardingto_ng_ran_qos_flow_info_list_l = dyn_array<data_forwardingto_ng_ran_qos_flow_info_list_item_s>;

// DiscardTimerExtended ::= ENUMERATED
struct discard_timer_extended_opts {
  enum options { ms0dot5, ms1, ms2, ms4, ms6, ms8, /*...*/ nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
using discard_timer_extended_e = enumerated<discard_timer_extended_opts, true>;

using ehc_params_ext_ies_container = protocol_ext_container_empty_l;

// EHC-Parameters ::= SEQUENCE
struct ehc_params_s {
  bool                         ehc_dl_present  = false;
  bool                         ehc_ul_present  = false;
  bool                         ie_exts_present = false;
  ehc_common_params_s          ehc_common;
  ehc_dl_params_s              ehc_dl;
  ehc_ul_params_s              ehc_ul;
  ehc_params_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EarlyForwardingCOUNTInfo-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using early_forwarding_count_info_ext_ies_o = protocol_ies_empty_o;

using first_dl_count_ext_ies_container = protocol_ext_container_empty_l;

// FirstDLCount ::= SEQUENCE
struct first_dl_count_s {
  bool                             ie_exts_present = false;
  pdcp_count_s                     first_dl_count_val;
  first_dl_count_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-StatusReportIndication ::= ENUMERATED
struct pdcp_status_report_ind_opts {
  enum options { dl, ul, both, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pdcp_status_report_ind_e = enumerated<pdcp_status_report_ind_opts, true>;

struct qos_flow_item_ext_ies_container {
  bool                                  qos_flow_map_ind_present                  = false;
  bool                                  data_forwarding_source_ip_address_present = false;
  qos_flow_map_ind_e                    qos_flow_map_ind;
  bounded_bitstring<1, 160, true, true> data_forwarding_source_ip_address;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoS-Flow-Item ::= SEQUENCE
struct qos_flow_item_s {
  bool                            ext             = false;
  bool                            ie_exts_present = false;
  uint8_t                         qos_flow_id     = 0;
  qos_flow_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using qos_flow_map_item_ext_ies_container = protocol_ext_container_empty_l;

// QoS-Flow-Mapping-Item ::= SEQUENCE
struct qos_flow_map_item_s {
  bool                                ext                      = false;
  bool                                qos_flow_map_ind_present = false;
  bool                                ie_exts_present          = false;
  uint8_t                             qos_flow_id              = 0;
  qos_flow_map_ind_e                  qos_flow_map_ind;
  qos_flow_map_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoS-Flow-QoS-Parameter-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct qos_flow_qos_param_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { redundant_qos_flow_ind, tsc_traffic_characteristics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    redundant_qos_flow_ind_e&            redundant_qos_flow_ind();
    tsc_traffic_characteristics_s&       tsc_traffic_characteristics();
    const redundant_qos_flow_ind_e&      redundant_qos_flow_ind() const;
    const tsc_traffic_characteristics_s& tsc_traffic_characteristics() const;

  private:
    types                                          type_;
    choice_buffer_t<tsc_traffic_characteristics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using qos_flows_to_be_forwarded_item_ext_ies_container = protocol_ext_container_empty_l;

// QoS-Flows-to-be-forwarded-Item ::= SEQUENCE
struct qos_flows_to_be_forwarded_item_s {
  bool                                             ext             = false;
  bool                                             ie_exts_present = false;
  uint8_t                                          qos_flow_id     = 0;
  qos_flows_to_be_forwarded_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct qos_flow_level_qos_params_ext_ies_container {
  bool                                  qos_monitoring_request_present            = false;
  bool                                  mcg_offered_gbr_qos_flow_info_present     = false;
  bool                                  qos_monitoring_report_freq_present        = false;
  bool                                  qos_monitoring_disabled_present           = false;
  bool                                  data_forwarding_source_ip_address_present = false;
  qos_monitoring_request_e              qos_monitoring_request;
  gbr_qos_flow_info_s                   mcg_offered_gbr_qos_flow_info;
  uint16_t                              qos_monitoring_report_freq;
  qos_monitoring_disabled_e             qos_monitoring_disabled;
  bounded_bitstring<1, 160, true, true> data_forwarding_source_ip_address;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoSFlowLevelQoSParameters ::= SEQUENCE
struct qos_flow_level_qos_params_s {
  struct reflective_qos_attribute_opts {
    enum options { subject_to, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using reflective_qos_attribute_e_ = enumerated<reflective_qos_attribute_opts, true>;
  struct add_qos_info_opts {
    enum options { more_likely, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using add_qos_info_e_ = enumerated<add_qos_info_opts, true>;
  struct reflective_qos_ind_opts {
    enum options { enabled, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using reflective_qos_ind_e_ = enumerated<reflective_qos_ind_opts, true>;

  // member variables
  bool                                        gbr_qos_flow_info_present        = false;
  bool                                        reflective_qos_attribute_present = false;
  bool                                        add_qos_info_present             = false;
  bool                                        paging_policy_ind_present        = false;
  bool                                        reflective_qos_ind_present       = false;
  bool                                        ie_exts_present                  = false;
  qos_characteristics_c                       qos_characteristics;
  ngran_alloc_and_retention_prio_s            ngra_nalloc_retention_prio;
  gbr_qos_flow_info_s                         gbr_qos_flow_info;
  reflective_qos_attribute_e_                 reflective_qos_attribute;
  add_qos_info_e_                             add_qos_info;
  uint8_t                                     paging_policy_ind = 1;
  reflective_qos_ind_e_                       reflective_qos_ind;
  qos_flow_level_qos_params_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using rohc_ext_ies_container = protocol_ext_container_empty_l;

// ROHC ::= SEQUENCE
struct rohc_s {
  struct continue_rohc_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using continue_rohc_e_ = enumerated<continue_rohc_opts, true>;

  // member variables
  bool                   continue_rohc_present = false;
  bool                   ie_exts_present       = false;
  uint16_t               max_c_id              = 0;
  uint16_t               rohc_profiles         = 0;
  continue_rohc_e_       continue_rohc;
  rohc_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ROHC-Parameters-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using rohc_params_ext_ies_o = protocol_ies_empty_o;

// T-Reordering ::= ENUMERATED
struct t_reordering_opts {
  enum options {
    ms0,
    ms1,
    ms2,
    ms4,
    ms5,
    ms8,
    ms10,
    ms15,
    ms20,
    ms30,
    ms40,
    ms50,
    ms60,
    ms80,
    ms100,
    ms120,
    ms140,
    ms160,
    ms180,
    ms200,
    ms220,
    ms240,
    ms260,
    ms280,
    ms300,
    ms500,
    ms750,
    ms1000,
    ms1250,
    ms1500,
    ms1750,
    ms2000,
    ms2250,
    ms2500,
    ms2750,
    ms3000,
    // ...
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using t_reordering_e = enumerated<t_reordering_opts, true>;

// T-ReorderingTimer-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using t_reordering_timer_ext_ies_o = protocol_ext_empty_o;

// UP-Parameters-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct up_params_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { qos_map_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::qos_map_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    qos_map_info_s&       qos_map_info() { return c; }
    const qos_map_info_s& qos_map_info() const { return c; }

  private:
    qos_map_info_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UP-TNL-Information ::= CHOICE
struct up_tnl_info_c {
  struct types_opts {
    enum options { gtp_tunnel, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  up_tnl_info_c() = default;
  up_tnl_info_c(const up_tnl_info_c& other);
  up_tnl_info_c& operator=(const up_tnl_info_c& other);
  ~up_tnl_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  gtp_tunnel_s& gtp_tunnel()
  {
    assert_choice_type(types::gtp_tunnel, type_, "UP-TNL-Information");
    return c.get<gtp_tunnel_s>();
  }
  protocol_ie_single_container_s<up_tnl_info_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "UP-TNL-Information");
    return c.get<protocol_ie_single_container_s<up_tnl_info_ext_ies_o>>();
  }
  const gtp_tunnel_s& gtp_tunnel() const
  {
    assert_choice_type(types::gtp_tunnel, type_, "UP-TNL-Information");
    return c.get<gtp_tunnel_s>();
  }
  const protocol_ie_single_container_s<up_tnl_info_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "UP-TNL-Information");
    return c.get<protocol_ie_single_container_s<up_tnl_info_ext_ies_o>>();
  }
  gtp_tunnel_s&                                          set_gtp_tunnel();
  protocol_ie_single_container_s<up_tnl_info_ext_ies_o>& set_choice_ext();

private:
  types                                                                                type_;
  choice_buffer_t<gtp_tunnel_s, protocol_ie_single_container_s<up_tnl_info_ext_ies_o>> c;

  void destroy_();
};

using ul_only_rohc_ext_ies_container = protocol_ext_container_empty_l;

// UplinkOnlyROHC ::= SEQUENCE
struct ul_only_rohc_s {
  struct continue_rohc_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using continue_rohc_e_ = enumerated<continue_rohc_opts, true>;

  // member variables
  bool                           continue_rohc_present = false;
  bool                           ie_exts_present       = false;
  uint16_t                       max_c_id              = 0;
  uint16_t                       rohc_profiles         = 0;
  continue_rohc_e_               continue_rohc;
  ul_only_rohc_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using daps_request_info_ext_ies_container = protocol_ext_container_empty_l;

// DAPSRequestInfo ::= SEQUENCE
struct daps_request_info_s {
  struct daps_ind_opts {
    enum options { daps_ho_required, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using daps_ind_e_ = enumerated<daps_ind_opts, true>;

  // member variables
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  daps_ind_e_                         daps_ind;
  daps_request_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_b_status_transfer_ext_ies_container = protocol_ext_container_empty_l;

// DRBBStatusTransfer ::= SEQUENCE
struct drb_b_status_transfer_s {
  bool                                      ext                               = false;
  bool                                      receive_statusof_pdcp_sdu_present = false;
  bool                                      ie_ext_present                    = false;
  bounded_bitstring<1, 131072, false, true> receive_statusof_pdcp_sdu;
  pdcp_count_s                              count_value;
  drb_b_status_transfer_ext_ies_container   ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Data-Forwarding-Information-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct data_forwarding_info_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { data_forwardingto_ng_ran_qos_flow_info_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::data_forwardingto_ng_ran_qos_flow_info_list; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    data_forwardingto_ng_ran_qos_flow_info_list_l&       data_forwardingto_ng_ran_qos_flow_info_list() { return c; }
    const data_forwardingto_ng_ran_qos_flow_info_list_l& data_forwardingto_ng_ran_qos_flow_info_list() const
    {
      return c;
    }

  private:
    data_forwardingto_ng_ran_qos_flow_info_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Data-Forwarding-Information-Request-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using data_forwarding_info_request_ext_ies_o = protocol_ext_empty_o;

// Data-Forwarding-Request ::= ENUMERATED
struct data_forwarding_request_opts {
  enum options { ul, dl, both, /*...*/ nulltype } value;

  const char* to_string() const;
};
using data_forwarding_request_e = enumerated<data_forwarding_request_opts, true>;

// DataForwardingtoE-UTRANInformationListItem-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using data_forwardingto_e_utran_info_list_item_ext_ies_o = protocol_ext_empty_o;

// DefaultDRB ::= ENUMERATED
struct default_drb_opts {
  enum options { true_value, false_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using default_drb_e = enumerated<default_drb_opts, true>;

// DiscardTimer ::= ENUMERATED
struct discard_timer_opts {
  enum options {
    ms10,
    ms20,
    ms30,
    ms40,
    ms50,
    ms60,
    ms75,
    ms100,
    ms150,
    ms200,
    ms250,
    ms300,
    ms500,
    ms750,
    ms1500,
    infinity,
    nulltype
  } value;
  typedef int16_t number_type;

  const char* to_string() const;
  int16_t     to_number() const;
};
using discard_timer_e = enumerated<discard_timer_opts>;

// Duplication-Activation ::= ENUMERATED
struct dupl_activation_opts {
  enum options { active, inactive, /*...*/ nulltype } value;

  const char* to_string() const;
};
using dupl_activation_e = enumerated<dupl_activation_opts, true>;

// EarlyDataForwardingIndicator ::= ENUMERATED
struct early_data_forwarding_ind_opts {
  enum options { stop, /*...*/ nulltype } value;

  const char* to_string() const;
};
using early_data_forwarding_ind_e = enumerated<early_data_forwarding_ind_opts, true>;

// EarlyForwardingCOUNTInfo ::= CHOICE
struct early_forwarding_count_info_c {
  struct types_opts {
    enum options { first_dl_count, dl_discarding_count, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  early_forwarding_count_info_c() = default;
  early_forwarding_count_info_c(const early_forwarding_count_info_c& other);
  early_forwarding_count_info_c& operator=(const early_forwarding_count_info_c& other);
  ~early_forwarding_count_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  first_dl_count_s& first_dl_count()
  {
    assert_choice_type(types::first_dl_count, type_, "EarlyForwardingCOUNTInfo");
    return c.get<first_dl_count_s>();
  }
  dl_discarding_s& dl_discarding_count()
  {
    assert_choice_type(types::dl_discarding_count, type_, "EarlyForwardingCOUNTInfo");
    return c.get<dl_discarding_s>();
  }
  protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "EarlyForwardingCOUNTInfo");
    return c.get<protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>>();
  }
  const first_dl_count_s& first_dl_count() const
  {
    assert_choice_type(types::first_dl_count, type_, "EarlyForwardingCOUNTInfo");
    return c.get<first_dl_count_s>();
  }
  const dl_discarding_s& dl_discarding_count() const
  {
    assert_choice_type(types::dl_discarding_count, type_, "EarlyForwardingCOUNTInfo");
    return c.get<dl_discarding_s>();
  }
  const protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "EarlyForwardingCOUNTInfo");
    return c.get<protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>>();
  }
  first_dl_count_s&                                                      set_first_dl_count();
  dl_discarding_s&                                                       set_dl_discarding_count();
  protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<dl_discarding_s,
                  first_dl_count_s,
                  protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>>
      c;

  void destroy_();
};

// EarlyForwardingCOUNTReq ::= ENUMERATED
struct early_forwarding_count_req_opts {
  enum options { first_dl_count, dl_discarding, /*...*/ nulltype } value;

  const char* to_string() const;
};
using early_forwarding_count_req_e = enumerated<early_forwarding_count_req_opts, true>;

// IgnoreMappingRuleIndication ::= ENUMERATED
struct ignore_map_rule_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ignore_map_rule_ind_e = enumerated<ignore_map_rule_ind_opts, true>;

// MaxIPrate ::= ENUMERATED
struct max_ip_rate_opts {
  enum options { bitrate64kbs, max_ue_rate, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using max_ip_rate_e = enumerated<max_ip_rate_opts, true>;

// MaximumIPdatarate-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using max_ip_datarate_ext_ies_o = protocol_ext_empty_o;

// OutOfOrderDelivery ::= ENUMERATED
struct out_of_order_delivery_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using out_of_order_delivery_e = enumerated<out_of_order_delivery_opts, true>;

// PDCP-Configuration-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct pdcp_cfg_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { pdcp_status_report_ind, add_pdcp_dupl_info, ehc_params, discard_timer_extended, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pdcp_status_report_ind_e&       pdcp_status_report_ind();
    add_pdcp_dupl_info_e&           add_pdcp_dupl_info();
    ehc_params_s&                   ehc_params();
    discard_timer_extended_e&       discard_timer_extended();
    const pdcp_status_report_ind_e& pdcp_status_report_ind() const;
    const add_pdcp_dupl_info_e&     add_pdcp_dupl_info() const;
    const ehc_params_s&             ehc_params() const;
    const discard_timer_extended_e& discard_timer_extended() const;

  private:
    types                         type_;
    choice_buffer_t<ehc_params_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDCP-DataRecovery ::= ENUMERATED
struct pdcp_data_recovery_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pdcp_data_recovery_e = enumerated<pdcp_data_recovery_opts, true>;

// PDCP-Duplication ::= ENUMERATED
struct pdcp_dupl_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pdcp_dupl_e = enumerated<pdcp_dupl_opts, true>;

// PDCP-Reestablishment ::= ENUMERATED
struct pdcp_reest_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pdcp_reest_e = enumerated<pdcp_reest_opts, true>;

// PDCP-SN-Size ::= ENUMERATED
struct pdcp_sn_size_opts {
  enum options { s_neg12, s_neg18, /*...*/ nulltype } value;
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
using pdcp_sn_size_e = enumerated<pdcp_sn_size_opts, true>;

// PDCP-SN-Status-Information-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdcp_sn_status_info_ext_ies_o = protocol_ext_empty_o;

// QoS-Flow-List ::= SEQUENCE (SIZE (1..64)) OF QoS-Flow-Item
using qos_flow_list_l = dyn_array<qos_flow_item_s>;

// QoS-Flow-Mapping-List ::= SEQUENCE (SIZE (1..64)) OF QoS-Flow-Mapping-Item
using qos_flow_map_list_l = dyn_array<qos_flow_map_item_s>;

struct qos_flow_qos_param_item_ext_ies_container {
  bool                          redundant_qos_flow_ind_present      = false;
  bool                          tsc_traffic_characteristics_present = false;
  redundant_qos_flow_ind_e      redundant_qos_flow_ind;
  tsc_traffic_characteristics_s tsc_traffic_characteristics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoS-Flow-QoS-Parameter-Item ::= SEQUENCE
struct qos_flow_qos_param_item_s {
  bool                                      ext                      = false;
  bool                                      qos_flow_map_ind_present = false;
  bool                                      ie_exts_present          = false;
  uint8_t                                   qos_flow_id              = 0;
  qos_flow_level_qos_params_s               qos_flow_level_qos_params;
  qos_flow_map_ind_e                        qos_flow_map_ind;
  qos_flow_qos_param_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoS-Flows-DRB-Remapping ::= ENUMERATED
struct qos_flows_drb_remap_opts {
  enum options { upd, source_cfg, /*...*/ nulltype } value;

  const char* to_string() const;
};
using qos_flows_drb_remap_e = enumerated<qos_flows_drb_remap_opts, true>;

// QoS-Flows-to-be-forwarded-List ::= SEQUENCE (SIZE (1..64)) OF QoS-Flows-to-be-forwarded-Item
using qos_flows_to_be_forwarded_list_l = dyn_array<qos_flows_to_be_forwarded_item_s>;

// RLC-Mode ::= ENUMERATED
struct rlc_mode_opts {
  enum options {
    rlc_tm,
    rlc_am,
    rlc_um_bidirectional,
    rlc_um_unidirectional_ul,
    rlc_um_unidirectional_dl,
    /*...*/ nulltype
  } value;

  const char* to_string() const;
};
using rlc_mode_e = enumerated<rlc_mode_opts, true>;

// ROHC-Parameters ::= CHOICE
struct rohc_params_c {
  struct types_opts {
    enum options { rohc, ul_only_rohc, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  rohc_params_c() = default;
  rohc_params_c(const rohc_params_c& other);
  rohc_params_c& operator=(const rohc_params_c& other);
  ~rohc_params_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  rohc_s& rohc()
  {
    assert_choice_type(types::rohc, type_, "ROHC-Parameters");
    return c.get<rohc_s>();
  }
  ul_only_rohc_s& ul_only_rohc()
  {
    assert_choice_type(types::ul_only_rohc, type_, "ROHC-Parameters");
    return c.get<ul_only_rohc_s>();
  }
  protocol_ie_single_container_s<rohc_params_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "ROHC-Parameters");
    return c.get<protocol_ie_single_container_s<rohc_params_ext_ies_o>>();
  }
  const rohc_s& rohc() const
  {
    assert_choice_type(types::rohc, type_, "ROHC-Parameters");
    return c.get<rohc_s>();
  }
  const ul_only_rohc_s& ul_only_rohc() const
  {
    assert_choice_type(types::ul_only_rohc, type_, "ROHC-Parameters");
    return c.get<ul_only_rohc_s>();
  }
  const protocol_ie_single_container_s<rohc_params_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "ROHC-Parameters");
    return c.get<protocol_ie_single_container_s<rohc_params_ext_ies_o>>();
  }
  rohc_s&                                                set_rohc();
  ul_only_rohc_s&                                        set_ul_only_rohc();
  protocol_ie_single_container_s<rohc_params_ext_ies_o>& set_choice_ext();

private:
  types                                                                                          type_;
  choice_buffer_t<protocol_ie_single_container_s<rohc_params_ext_ies_o>, rohc_s, ul_only_rohc_s> c;

  void destroy_();
};

// SDAP-Configuration-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using sdap_cfg_ext_ies_o = protocol_ext_empty_o;

// SDAP-Header-DL ::= ENUMERATED
struct sdap_hdr_dl_opts {
  enum options { present, absent, /*...*/ nulltype } value;

  const char* to_string() const;
};
using sdap_hdr_dl_e = enumerated<sdap_hdr_dl_opts, true>;

// SDAP-Header-UL ::= ENUMERATED
struct sdap_hdr_ul_opts {
  enum options { present, absent, /*...*/ nulltype } value;

  const char* to_string() const;
};
using sdap_hdr_ul_e = enumerated<sdap_hdr_ul_opts, true>;

using t_reordering_timer_ext_ies_container = protocol_ext_container_empty_l;

// T-ReorderingTimer ::= SEQUENCE
struct t_reordering_timer_s {
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  t_reordering_e                       t_reordering;
  t_reordering_timer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULDataSplitThreshold ::= ENUMERATED
struct ul_data_split_thres_opts {
  enum options {
    b0,
    b100,
    b200,
    b400,
    b800,
    b1600,
    b3200,
    b6400,
    b12800,
    b25600,
    b51200,
    b102400,
    b204800,
    b409600,
    b819200,
    b1228800,
    b1638400,
    b2457600,
    b3276800,
    b4096000,
    b4915200,
    b5734400,
    b6553600,
    infinity,
    // ...
    nulltype
  } value;
  typedef int32_t number_type;

  const char* to_string() const;
  int32_t     to_number() const;
};
using ul_data_split_thres_e = enumerated<ul_data_split_thres_opts, true>;

// UP-Parameters-Item ::= SEQUENCE
struct up_params_item_s {
  bool                                               ext = false;
  up_tnl_info_c                                      up_tnl_info;
  uint8_t                                            cell_group_id = 0;
  protocol_ext_container_l<up_params_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ConfidentialityProtectionIndication ::= ENUMERATED
struct confidentiality_protection_ind_opts {
  enum options { required, preferred, not_needed, /*...*/ nulltype } value;

  const char* to_string() const;
};
using confidentiality_protection_ind_e = enumerated<confidentiality_protection_ind_opts, true>;

// DRB-To-Modify-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct drb_to_modify_item_ng_ran_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        old_qos_flow_map_ul_endmarkerexpected,
        drb_qos,
        early_forwarding_count_req,
        early_forwarding_count_info,
        daps_request_info,
        early_data_forwarding_ind,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    qos_flow_list_l&                     old_qos_flow_map_ul_endmarkerexpected();
    qos_flow_level_qos_params_s&         drb_qos();
    early_forwarding_count_req_e&        early_forwarding_count_req();
    early_forwarding_count_info_c&       early_forwarding_count_info();
    daps_request_info_s&                 daps_request_info();
    early_data_forwarding_ind_e&         early_data_forwarding_ind();
    const qos_flow_list_l&               old_qos_flow_map_ul_endmarkerexpected() const;
    const qos_flow_level_qos_params_s&   drb_qos() const;
    const early_forwarding_count_req_e&  early_forwarding_count_req() const;
    const early_forwarding_count_info_c& early_forwarding_count_info() const;
    const daps_request_info_s&           daps_request_info() const;
    const early_data_forwarding_ind_e&   early_data_forwarding_ind() const;

  private:
    types type_;
    choice_buffer_t<daps_request_info_s, early_forwarding_count_info_c, qos_flow_level_qos_params_s, qos_flow_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRB-To-Remove-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_to_rem_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

// DRB-To-Setup-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct drb_to_setup_item_ng_ran_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { drb_qos, daps_request_info, ignore_map_rule_ind, qos_flows_drb_remap, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    qos_flow_level_qos_params_s&       drb_qos();
    daps_request_info_s&               daps_request_info();
    ignore_map_rule_ind_e&             ignore_map_rule_ind();
    qos_flows_drb_remap_e&             qos_flows_drb_remap();
    const qos_flow_level_qos_params_s& drb_qos() const;
    const daps_request_info_s&         daps_request_info() const;
    const ignore_map_rule_ind_e&       ignore_map_rule_ind() const;
    const qos_flows_drb_remap_e&       qos_flows_drb_remap() const;

  private:
    types                                                             type_;
    choice_buffer_t<daps_request_info_s, qos_flow_level_qos_params_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRB-To-Setup-Mod-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct drb_to_setup_mod_item_ng_ran_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { drb_qos, ignore_map_rule_ind, daps_request_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    qos_flow_level_qos_params_s&       drb_qos();
    ignore_map_rule_ind_e&             ignore_map_rule_ind();
    daps_request_info_s&               daps_request_info();
    const qos_flow_level_qos_params_s& drb_qos() const;
    const ignore_map_rule_ind_e&       ignore_map_rule_ind() const;
    const daps_request_info_s&         daps_request_info() const;

  private:
    types                                                             type_;
    choice_buffer_t<daps_request_info_s, qos_flow_level_qos_params_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Data-Forwarding-Information ::= SEQUENCE
struct data_forwarding_info_s {
  bool                                                     ext                        = false;
  bool                                                     ul_data_forwarding_present = false;
  bool                                                     dl_data_forwarding_present = false;
  up_tnl_info_c                                            ul_data_forwarding;
  up_tnl_info_c                                            dl_data_forwarding;
  protocol_ext_container_l<data_forwarding_info_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using data_forwarding_info_request_ext_ies_container = protocol_ext_container_empty_l;

// Data-Forwarding-Information-Request ::= SEQUENCE
struct data_forwarding_info_request_s {
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  data_forwarding_request_e                      data_forwarding_request;
  qos_flow_map_list_l                            qos_flows_forwarded_on_fwd_tunnels;
  data_forwarding_info_request_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using data_forwardingto_e_utran_info_list_item_ext_ies_container = protocol_ext_container_empty_l;

// DataForwardingtoE-UTRANInformationListItem ::= SEQUENCE
struct data_forwardingto_e_utran_info_list_item_s {
  bool                                                       ext             = false;
  bool                                                       ie_exts_present = false;
  up_tnl_info_c                                              data_forwarding_tunnel_info;
  qos_flows_to_be_forwarded_list_l                           qos_flows_to_be_forwarded_list;
  data_forwardingto_e_utran_info_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRANAllocationAndRetentionPriority-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using eutran_alloc_and_retention_prio_ext_ies_o = protocol_ext_empty_o;

// GBR-QosInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using gbr_qos_info_ext_ies_o = protocol_ext_empty_o;

// IntegrityProtectionIndication ::= ENUMERATED
struct integrity_protection_ind_opts {
  enum options { required, preferred, not_needed, /*...*/ nulltype } value;

  const char* to_string() const;
};
using integrity_protection_ind_e = enumerated<integrity_protection_ind_opts, true>;

using max_ip_datarate_ext_ies_container = protocol_ext_container_empty_l;

// MaximumIPdatarate ::= SEQUENCE
struct max_ip_datarate_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  max_ip_rate_e                     max_ip_rate;
  max_ip_datarate_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct pdcp_cfg_ext_ies_container {
  bool                     pdcp_status_report_ind_present = false;
  bool                     add_pdcp_dupl_info_present     = false;
  bool                     ehc_params_present             = false;
  bool                     discard_timer_extended_present = false;
  pdcp_status_report_ind_e pdcp_status_report_ind;
  add_pdcp_dupl_info_e     add_pdcp_dupl_info;
  ehc_params_s             ehc_params;
  discard_timer_extended_e discard_timer_extended;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-Configuration ::= SEQUENCE
struct pdcp_cfg_s {
  bool                       ext                           = false;
  bool                       rohc_params_present           = false;
  bool                       t_reordering_timer_present    = false;
  bool                       discard_timer_present         = false;
  bool                       ul_data_split_thres_present   = false;
  bool                       pdcp_dupl_present             = false;
  bool                       pdcp_reest_present            = false;
  bool                       pdcp_data_recovery_present    = false;
  bool                       dupl_activation_present       = false;
  bool                       out_of_order_delivery_present = false;
  bool                       ie_exts_present               = false;
  pdcp_sn_size_e             pdcp_sn_size_ul;
  pdcp_sn_size_e             pdcp_sn_size_dl;
  rlc_mode_e                 rlc_mode;
  rohc_params_c              rohc_params;
  t_reordering_timer_s       t_reordering_timer;
  discard_timer_e            discard_timer;
  ul_data_split_thres_e      ul_data_split_thres;
  pdcp_dupl_e                pdcp_dupl;
  pdcp_reest_e               pdcp_reest;
  pdcp_data_recovery_e       pdcp_data_recovery;
  dupl_activation_e          dupl_activation;
  out_of_order_delivery_e    out_of_order_delivery;
  pdcp_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdcp_sn_status_info_ext_ies_container = protocol_ext_container_empty_l;

// PDCP-SN-Status-Information ::= SEQUENCE
struct pdcp_sn_status_info_s {
  bool                                  ext            = false;
  bool                                  ie_ext_present = false;
  drb_b_status_transfer_s               pdcp_status_transfer_ul;
  pdcp_count_s                          pdcp_status_transfer_dl;
  pdcp_sn_status_info_ext_ies_container ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-SN-Status-Request ::= ENUMERATED
struct pdcp_sn_status_request_opts {
  enum options { requested, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pdcp_sn_status_request_e = enumerated<pdcp_sn_status_request_opts, true>;

// QoS-Flow-QoS-Parameter-List ::= SEQUENCE (SIZE (1..64)) OF QoS-Flow-QoS-Parameter-Item
using qos_flow_qos_param_list_l = dyn_array<qos_flow_qos_param_item_s>;

using sdap_cfg_ext_ies_container = protocol_ext_container_empty_l;

// SDAP-Configuration ::= SEQUENCE
struct sdap_cfg_s {
  bool                       ext             = false;
  bool                       ie_exts_present = false;
  default_drb_e              default_drb;
  sdap_hdr_ul_e              sdap_hdr_ul;
  sdap_hdr_dl_e              sdap_hdr_dl;
  sdap_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SNSSAI-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using snssai_ext_ies_o = protocol_ext_empty_o;

// SecurityIndication-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using security_ind_ext_ies_o = protocol_ext_empty_o;

// UP-Parameters ::= SEQUENCE (SIZE (1..8)) OF UP-Parameters-Item
using up_params_l = dyn_array<up_params_item_s>;

struct drb_to_modify_item_ng_ran_ext_ies_container {
  bool                          old_qos_flow_map_ul_endmarkerexpected_present = false;
  bool                          drb_qos_present                               = false;
  bool                          early_forwarding_count_req_present            = false;
  bool                          early_forwarding_count_info_present           = false;
  bool                          daps_request_info_present                     = false;
  bool                          early_data_forwarding_ind_present             = false;
  qos_flow_list_l               old_qos_flow_map_ul_endmarkerexpected;
  qos_flow_level_qos_params_s   drb_qos;
  early_forwarding_count_req_e  early_forwarding_count_req;
  early_forwarding_count_info_c early_forwarding_count_info;
  daps_request_info_s           daps_request_info;
  early_data_forwarding_ind_e   early_data_forwarding_ind;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-To-Modify-Item-NG-RAN ::= SEQUENCE
struct drb_to_modify_item_ng_ran_s {
  bool                                        ext                              = false;
  bool                                        sdap_cfg_present                 = false;
  bool                                        pdcp_cfg_present                 = false;
  bool                                        drb_data_forwarding_info_present = false;
  bool                                        pdcp_sn_status_request_present   = false;
  bool                                        pdcp_sn_status_info_present      = false;
  bool                                        drb_inactivity_timer_present     = false;
  bool                                        ie_exts_present                  = false;
  uint8_t                                     drb_id                           = 1;
  sdap_cfg_s                                  sdap_cfg;
  pdcp_cfg_s                                  pdcp_cfg;
  data_forwarding_info_s                      drb_data_forwarding_info;
  pdcp_sn_status_request_e                    pdcp_sn_status_request;
  pdcp_sn_status_info_s                       pdcp_sn_status_info;
  up_params_l                                 dl_up_params;
  cell_group_info_l                           cell_group_to_add;
  cell_group_info_l                           cell_group_to_modify;
  cell_group_info_l                           cell_group_to_rem;
  qos_flow_qos_param_list_l                   flow_map_info;
  uint16_t                                    drb_inactivity_timer = 1;
  drb_to_modify_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_to_rem_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-To-Remove-Item-NG-RAN ::= SEQUENCE
struct drb_to_rem_item_ng_ran_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  uint8_t                                  drb_id          = 1;
  drb_to_rem_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct drb_to_setup_item_ng_ran_ext_ies_container {
  bool                        drb_qos_present             = false;
  bool                        daps_request_info_present   = false;
  bool                        ignore_map_rule_ind_present = false;
  bool                        qos_flows_drb_remap_present = false;
  qos_flow_level_qos_params_s drb_qos;
  daps_request_info_s         daps_request_info;
  ignore_map_rule_ind_e       ignore_map_rule_ind;
  qos_flows_drb_remap_e       qos_flows_drb_remap;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-To-Setup-Item-NG-RAN ::= SEQUENCE
struct drb_to_setup_item_ng_ran_s {
  bool                                       ext                                      = false;
  bool                                       drb_data_forwarding_info_request_present = false;
  bool                                       drb_inactivity_timer_present             = false;
  bool                                       pdcp_sn_status_info_present              = false;
  bool                                       ie_exts_present                          = false;
  uint8_t                                    drb_id                                   = 1;
  sdap_cfg_s                                 sdap_cfg;
  pdcp_cfg_s                                 pdcp_cfg;
  cell_group_info_l                          cell_group_info;
  qos_flow_qos_param_list_l                  qos_flow_info_to_be_setup;
  data_forwarding_info_request_s             drb_data_forwarding_info_request;
  uint16_t                                   drb_inactivity_timer = 1;
  pdcp_sn_status_info_s                      pdcp_sn_status_info;
  drb_to_setup_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct drb_to_setup_mod_item_ng_ran_ext_ies_container {
  bool                        drb_qos_present             = false;
  bool                        ignore_map_rule_ind_present = false;
  bool                        daps_request_info_present   = false;
  qos_flow_level_qos_params_s drb_qos;
  ignore_map_rule_ind_e       ignore_map_rule_ind;
  daps_request_info_s         daps_request_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-To-Setup-Mod-Item-NG-RAN ::= SEQUENCE
struct drb_to_setup_mod_item_ng_ran_s {
  bool                                           ext                                      = false;
  bool                                           drb_data_forwarding_info_request_present = false;
  bool                                           drb_inactivity_timer_present             = false;
  bool                                           pdcp_sn_status_info_present              = false;
  bool                                           ie_exts_present                          = false;
  uint8_t                                        drb_id                                   = 1;
  sdap_cfg_s                                     sdap_cfg;
  pdcp_cfg_s                                     pdcp_cfg;
  cell_group_info_l                              cell_group_info;
  qos_flow_qos_param_list_l                      flow_map_info;
  data_forwarding_info_request_s                 drb_data_forwarding_info_request;
  uint16_t                                       drb_inactivity_timer = 1;
  pdcp_sn_status_info_s                          pdcp_sn_status_info;
  drb_to_setup_mod_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DataForwardingtoE-UTRANInformationList ::= SEQUENCE (SIZE (1..256)) OF DataForwardingtoE-UTRANInformationListItem
using data_forwardingto_e_utran_info_list_l = dyn_array<data_forwardingto_e_utran_info_list_item_s>;

// EUTRAN-QoS-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using eutran_qos_ext_ies_o = protocol_ext_empty_o;

using eutran_alloc_and_retention_prio_ext_ies_container = protocol_ext_container_empty_l;

// EUTRANAllocationAndRetentionPriority ::= SEQUENCE
struct eutran_alloc_and_retention_prio_s {
  bool                                              ext             = false;
  bool                                              ie_exts_present = false;
  uint8_t                                           prio_level      = 0;
  pre_emption_cap_e                                 pre_emption_cap;
  pre_emption_vulnerability_e                       pre_emption_vulnerability;
  eutran_alloc_and_retention_prio_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using gbr_qos_info_ext_ies_container = protocol_ext_container_empty_l;

// GBR-QosInformation ::= SEQUENCE
struct gbr_qos_info_s {
  bool                           ext                        = false;
  bool                           ie_exts_present            = false;
  uint64_t                       erab_max_bitrate_dl        = 0;
  uint64_t                       erab_max_bitrate_ul        = 0;
  uint64_t                       erab_guaranteed_bitrate_dl = 0;
  uint64_t                       erab_guaranteed_bitrate_ul = 0;
  gbr_qos_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using snssai_ext_ies_container = protocol_ext_container_empty_l;

// SNSSAI ::= SEQUENCE
struct snssai_s {
  bool                     ext             = false;
  bool                     sd_present      = false;
  bool                     ie_exts_present = false;
  fixed_octstring<1, true> sst;
  fixed_octstring<3, true> sd;
  snssai_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using security_ind_ext_ies_container = protocol_ext_container_empty_l;

// SecurityIndication ::= SEQUENCE
struct security_ind_s {
  bool                             ext                     = false;
  bool                             max_ip_datarate_present = false;
  bool                             ie_exts_present         = false;
  integrity_protection_ind_e       integrity_protection_ind;
  confidentiality_protection_ind_e confidentiality_protection_ind;
  max_ip_datarate_s                max_ip_datarate;
  security_ind_ext_ies_container   ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-To-Modify-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_to_modify_item_eutran_ext_ies_o = protocol_ext_empty_o;

// DRB-To-Modify-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-To-Modify-Item-NG-RAN
using drb_to_modify_list_ng_ran_l = dyn_array<drb_to_modify_item_ng_ran_s>;

// DRB-To-Remove-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_to_rem_item_eutran_ext_ies_o = protocol_ext_empty_o;

// DRB-To-Remove-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-To-Remove-Item-NG-RAN
using drb_to_rem_list_ng_ran_l = dyn_array<drb_to_rem_item_ng_ran_s>;

// DRB-To-Setup-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-To-Setup-Item-NG-RAN
using drb_to_setup_list_ng_ran_l = dyn_array<drb_to_setup_item_ng_ran_s>;

// DRB-To-Setup-Mod-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct drb_to_setup_mod_item_eutran_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { data_forwarding_source_ip_address, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::data_forwarding_source_ip_address; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bounded_bitstring<1, 160, true, true>&       data_forwarding_source_ip_address() { return c; }
    const bounded_bitstring<1, 160, true, true>& data_forwarding_source_ip_address() const { return c; }

  private:
    bounded_bitstring<1, 160, true, true> c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRB-To-Setup-Mod-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-To-Setup-Mod-Item-NG-RAN
using drb_to_setup_mod_list_ng_ran_l = dyn_array<drb_to_setup_mod_item_ng_ran_s>;

using eutran_qos_ext_ies_container = protocol_ext_container_empty_l;

// EUTRAN-QoS ::= SEQUENCE
struct eutran_qos_s {
  bool                              ext                  = false;
  bool                              gbr_qos_info_present = false;
  bool                              ie_exts_present      = false;
  uint16_t                          qci                  = 0;
  eutran_alloc_and_retention_prio_s eutran_alloc_and_retention_prio;
  gbr_qos_info_s                    gbr_qos_info;
  eutran_qos_ext_ies_container      ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-Resource-To-Modify-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct pdu_session_res_to_modify_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        snssai,
        common_network_instance,
        redundant_n_g_ul_up_tnl_info,
        redundant_common_network_instance,
        data_forwardingto_e_utran_info_list,
        security_ind_modify,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    snssai_s&                                    snssai();
    unbounded_octstring<true>&                   common_network_instance();
    up_tnl_info_c&                               redundant_n_g_ul_up_tnl_info();
    unbounded_octstring<true>&                   redundant_common_network_instance();
    data_forwardingto_e_utran_info_list_l&       data_forwardingto_e_utran_info_list();
    security_ind_s&                              security_ind_modify();
    const snssai_s&                              snssai() const;
    const unbounded_octstring<true>&             common_network_instance() const;
    const up_tnl_info_c&                         redundant_n_g_ul_up_tnl_info() const;
    const unbounded_octstring<true>&             redundant_common_network_instance() const;
    const data_forwardingto_e_utran_info_list_l& data_forwardingto_e_utran_info_list() const;
    const security_ind_s&                        security_ind_modify() const;

  private:
    types type_;
    choice_buffer_t<data_forwardingto_e_utran_info_list_l,
                    security_ind_s,
                    snssai_s,
                    unbounded_octstring<true>,
                    up_tnl_info_c>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDU-Session-Resource-To-Remove-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct pdu_session_res_to_rem_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { cause, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::cause; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c&       cause() { return c; }
    const cause_c& cause() const { return c; }

  private:
    cause_c c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDU-Session-Resource-To-Setup-Mod-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct pdu_session_res_to_setup_mod_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        network_instance,
        common_network_instance,
        redundant_n_g_ul_up_tnl_info,
        redundant_common_network_instance,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                        network_instance();
    unbounded_octstring<true>&       common_network_instance();
    up_tnl_info_c&                   redundant_n_g_ul_up_tnl_info();
    unbounded_octstring<true>&       redundant_common_network_instance();
    const uint16_t&                  network_instance() const;
    const unbounded_octstring<true>& common_network_instance() const;
    const up_tnl_info_c&             redundant_n_g_ul_up_tnl_info() const;
    const unbounded_octstring<true>& redundant_common_network_instance() const;

  private:
    types                                                     type_;
    choice_buffer_t<unbounded_octstring<true>, up_tnl_info_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDU-Session-Type ::= ENUMERATED
struct pdu_session_type_opts {
  enum options { ipv4, ipv6, ipv4v6, ethernet, unstructured, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pdu_session_type_e = enumerated<pdu_session_type_opts, true>;

using drb_to_modify_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-To-Modify-Item-EUTRAN ::= SEQUENCE
struct drb_to_modify_item_eutran_s {
  bool                                        ext                            = false;
  bool                                        pdcp_cfg_present               = false;
  bool                                        eutran_qos_present             = false;
  bool                                        s1_ul_up_tnl_info_present      = false;
  bool                                        data_forwarding_info_present   = false;
  bool                                        pdcp_sn_status_request_present = false;
  bool                                        pdcp_sn_status_info_present    = false;
  bool                                        drb_inactivity_timer_present   = false;
  bool                                        ie_exts_present                = false;
  uint8_t                                     drb_id                         = 1;
  pdcp_cfg_s                                  pdcp_cfg;
  eutran_qos_s                                eutran_qos;
  up_tnl_info_c                               s1_ul_up_tnl_info;
  data_forwarding_info_s                      data_forwarding_info;
  pdcp_sn_status_request_e                    pdcp_sn_status_request;
  pdcp_sn_status_info_s                       pdcp_sn_status_info;
  up_params_l                                 dl_up_params;
  cell_group_info_l                           cell_group_to_add;
  cell_group_info_l                           cell_group_to_modify;
  cell_group_info_l                           cell_group_to_rem;
  uint16_t                                    drb_inactivity_timer = 1;
  drb_to_modify_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_to_rem_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-To-Remove-Item-EUTRAN ::= SEQUENCE
struct drb_to_rem_item_eutran_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  uint8_t                                  drb_id          = 1;
  drb_to_rem_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-To-Setup-Mod-Item-EUTRAN ::= SEQUENCE
struct drb_to_setup_mod_item_eutran_s {
  bool                                                             ext                                  = false;
  bool                                                             data_forwarding_info_request_present = false;
  bool                                                             drb_inactivity_timer_present         = false;
  uint8_t                                                          drb_id                               = 1;
  pdcp_cfg_s                                                       pdcp_cfg;
  eutran_qos_s                                                     eutran_qos;
  up_tnl_info_c                                                    s1_ul_up_tnl_info;
  data_forwarding_info_request_s                                   data_forwarding_info_request;
  cell_group_info_l                                                cell_group_info;
  up_params_l                                                      dl_up_params;
  uint16_t                                                         drb_inactivity_timer = 1;
  protocol_ext_container_l<drb_to_setup_mod_item_eutran_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct pdu_session_res_to_modify_item_ext_ies_container {
  bool                                  snssai_present                              = false;
  bool                                  common_network_instance_present             = false;
  bool                                  redundant_n_g_ul_up_tnl_info_present        = false;
  bool                                  redundant_common_network_instance_present   = false;
  bool                                  data_forwardingto_e_utran_info_list_present = false;
  bool                                  security_ind_modify_present                 = false;
  snssai_s                              snssai;
  unbounded_octstring<true>             common_network_instance;
  up_tnl_info_c                         redundant_n_g_ul_up_tnl_info;
  unbounded_octstring<true>             redundant_common_network_instance;
  data_forwardingto_e_utran_info_list_l data_forwardingto_e_utran_info_list;
  security_ind_s                        security_ind_modify;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-Resource-To-Modify-Item ::= SEQUENCE
struct pdu_session_res_to_modify_item_s {
  bool                                             ext                                              = false;
  bool                                             security_ind_present                             = false;
  bool                                             pdu_session_res_dl_ambr_present                  = false;
  bool                                             ng_ul_up_tnl_info_present                        = false;
  bool                                             pdu_session_data_forwarding_info_request_present = false;
  bool                                             pdu_session_data_forwarding_info_present         = false;
  bool                                             pdu_session_inactivity_timer_present             = false;
  bool                                             network_instance_present                         = false;
  bool                                             ie_exts_present                                  = false;
  uint16_t                                         pdu_session_id                                   = 0;
  security_ind_s                                   security_ind;
  uint64_t                                         pdu_session_res_dl_ambr = 0;
  up_tnl_info_c                                    ng_ul_up_tnl_info;
  data_forwarding_info_request_s                   pdu_session_data_forwarding_info_request;
  data_forwarding_info_s                           pdu_session_data_forwarding_info;
  uint16_t                                         pdu_session_inactivity_timer = 1;
  uint16_t                                         network_instance             = 1;
  drb_to_setup_list_ng_ran_l                       drb_to_setup_list_ng_ran;
  drb_to_modify_list_ng_ran_l                      drb_to_modify_list_ng_ran;
  drb_to_rem_list_ng_ran_l                         drb_to_rem_list_ng_ran;
  pdu_session_res_to_modify_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-Resource-To-Remove-Item ::= SEQUENCE
struct pdu_session_res_to_rem_item_s {
  bool                                                            ext            = false;
  uint16_t                                                        pdu_session_id = 0;
  protocol_ext_container_l<pdu_session_res_to_rem_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct pdu_session_res_to_setup_mod_item_ext_ies_container {
  bool                      network_instance_present                  = false;
  bool                      common_network_instance_present           = false;
  bool                      redundant_n_g_ul_up_tnl_info_present      = false;
  bool                      redundant_common_network_instance_present = false;
  uint16_t                  network_instance;
  unbounded_octstring<true> common_network_instance;
  up_tnl_info_c             redundant_n_g_ul_up_tnl_info;
  unbounded_octstring<true> redundant_common_network_instance;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-Resource-To-Setup-Mod-Item ::= SEQUENCE
struct pdu_session_res_to_setup_mod_item_s {
  bool                                                ext                                              = false;
  bool                                                pdu_session_res_ambr_present                     = false;
  bool                                                pdu_session_data_forwarding_info_request_present = false;
  bool                                                pdu_session_inactivity_timer_present             = false;
  bool                                                ie_exts_present                                  = false;
  uint16_t                                            pdu_session_id                                   = 0;
  pdu_session_type_e                                  pdu_session_type;
  snssai_s                                            snssai;
  security_ind_s                                      security_ind;
  uint64_t                                            pdu_session_res_ambr = 0;
  up_tnl_info_c                                       ng_ul_up_tnl_info;
  data_forwarding_info_request_s                      pdu_session_data_forwarding_info_request;
  uint16_t                                            pdu_session_inactivity_timer = 1;
  drb_to_setup_mod_list_ng_ran_l                      drb_to_setup_mod_list_ng_ran;
  pdu_session_res_to_setup_mod_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CipheringAlgorithm ::= ENUMERATED
struct ciphering_algorithm_opts {
  enum options { nea0, c_neg128_nea1, c_neg128_nea2, c_neg128_nea3, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ciphering_algorithm_e = enumerated<ciphering_algorithm_opts, true>;

// DRB-To-Modify-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-To-Modify-Item-EUTRAN
using drb_to_modify_list_eutran_l = dyn_array<drb_to_modify_item_eutran_s>;

// DRB-To-Remove-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-To-Remove-Item-EUTRAN
using drb_to_rem_list_eutran_l = dyn_array<drb_to_rem_item_eutran_s>;

// DRB-To-Setup-Mod-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-To-Setup-Mod-Item-EUTRAN
using drb_to_setup_mod_list_eutran_l = dyn_array<drb_to_setup_mod_item_eutran_s>;

// IntegrityProtectionAlgorithm ::= ENUMERATED
struct integrity_protection_algorithm_opts {
  enum options { nia0, i_neg128_nia1, i_neg128_nia2, i_neg128_nia3, /*...*/ nulltype } value;

  const char* to_string() const;
};
using integrity_protection_algorithm_e = enumerated<integrity_protection_algorithm_opts, true>;

// PDU-Session-Resource-To-Modify-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-To-Modify-Item
using pdu_session_res_to_modify_list_l = dyn_array<pdu_session_res_to_modify_item_s>;

// PDU-Session-Resource-To-Remove-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-To-Remove-Item
using pdu_session_res_to_rem_list_l = dyn_array<pdu_session_res_to_rem_item_s>;

// PDU-Session-Resource-To-Setup-Mod-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-To-Setup-Mod-Item
using pdu_session_res_to_setup_mod_list_l = dyn_array<pdu_session_res_to_setup_mod_item_s>;

// SecurityAlgorithm-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using security_algorithm_ext_ies_o = protocol_ext_empty_o;

// UPSecuritykey-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using up_securitykey_ext_ies_o = protocol_ext_empty_o;

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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types type_;
    choice_buffer_t<drb_to_modify_list_eutran_l,
                    drb_to_rem_list_eutran_l,
                    drb_to_setup_mod_list_eutran_l,
                    fixed_bitstring<32, false, true>>
        c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types type_;
    choice_buffer_t<pdu_session_res_to_modify_list_l,
                    pdu_session_res_to_rem_list_l,
                    pdu_session_res_to_setup_mod_list_l>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using security_algorithm_ext_ies_container = protocol_ext_container_empty_l;

// SecurityAlgorithm ::= SEQUENCE
struct security_algorithm_s {
  bool                                 ext                                    = false;
  bool                                 integrity_protection_algorithm_present = false;
  bool                                 ie_exts_present                        = false;
  ciphering_algorithm_e                ciphering_algorithm;
  integrity_protection_algorithm_e     integrity_protection_algorithm;
  security_algorithm_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using security_info_ext_ies_o = protocol_ext_empty_o;

// System-BearerContextModificationRequest-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using sys_bearer_context_mod_request_ext_ies_o = protocol_ies_empty_o;

using up_securitykey_ext_ies_container = protocol_ext_container_empty_l;

// UPSecuritykey ::= SEQUENCE
struct up_securitykey_s {
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  unbounded_octstring<true>        encryption_key;
  unbounded_octstring<true>        integrity_protection_key;
  up_securitykey_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ActivityNotificationLevel ::= ENUMERATED
struct activity_notif_level_opts {
  enum options { drb, pdu_session, ue, /*...*/ nulltype } value;

  const char* to_string() const;
};
using activity_notif_level_e = enumerated<activity_notif_level_opts, true>;

// BearerContextStatusChange ::= ENUMERATED
struct bearer_context_status_change_opts {
  enum options { suspend, resume, /*...*/ nulltype } value;

  const char* to_string() const;
};
using bearer_context_status_change_e = enumerated<bearer_context_status_change_opts, true>;

// DataDiscardRequired ::= ENUMERATED
struct data_discard_required_opts {
  enum options { required, /*...*/ nulltype } value;

  const char* to_string() const;
};
using data_discard_required_e = enumerated<data_discard_required_opts, true>;

// New-UL-TNL-Information-Required ::= ENUMERATED
struct new_ul_tnl_info_required_opts {
  enum options { required, /*...*/ nulltype } value;

  const char* to_string() const;
};
using new_ul_tnl_info_required_e = enumerated<new_ul_tnl_info_required_opts, true>;

using security_info_ext_ies_container = protocol_ext_container_empty_l;

// SecurityInformation ::= SEQUENCE
struct security_info_s {
  bool                            ext             = false;
  bool                            ie_exts_present = false;
  security_algorithm_s            security_algorithm;
  up_securitykey_s                up_securitykey;
  security_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                                                                        type_;
    choice_buffer_t<fixed_octstring<8, true>, security_info_s, sys_bearer_context_mod_request_c> c;

    void destroy_();
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

// GNB-CU-UP-CellGroupRelatedConfiguration-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using gnb_cu_up_cell_group_related_cfg_item_ext_ies_o = protocol_ext_empty_o;

using gnb_cu_up_cell_group_related_cfg_item_ext_ies_container = protocol_ext_container_empty_l;

// GNB-CU-UP-CellGroupRelatedConfiguration-Item ::= SEQUENCE
struct gnb_cu_up_cell_group_related_cfg_item_s {
  bool                                                    ul_cfg_present  = false;
  bool                                                    ie_exts_present = false;
  uint8_t                                                 cell_group_id   = 0;
  up_tnl_info_c                                           up_tnl_info;
  ul_cfg_e                                                ul_cfg;
  gnb_cu_up_cell_group_related_cfg_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Required-To-Modify-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_required_to_modify_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

// DRB-Required-To-Remove-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_required_to_rem_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

// GNB-CU-UP-CellGroupRelatedConfiguration ::= SEQUENCE (SIZE (1..8)) OF GNB-CU-UP-CellGroupRelatedConfiguration-Item
using gnb_cu_up_cell_group_related_cfg_l = dyn_array<gnb_cu_up_cell_group_related_cfg_item_s>;

using drb_required_to_modify_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Required-To-Modify-Item-NG-RAN ::= SEQUENCE
struct drb_required_to_modify_item_ng_ran_s {
  bool                                                 ext             = false;
  bool                                                 cause_present   = false;
  bool                                                 ie_exts_present = false;
  uint8_t                                              drb_id          = 1;
  gnb_cu_up_cell_group_related_cfg_l                   gnb_cu_up_cell_group_related_cfg;
  qos_flow_list_l                                      flow_to_rem;
  cause_c                                              cause;
  drb_required_to_modify_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_required_to_rem_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Required-To-Remove-Item-NG-RAN ::= SEQUENCE
struct drb_required_to_rem_item_ng_ran_s {
  bool                                              ext             = false;
  bool                                              ie_exts_present = false;
  uint8_t                                           drb_id          = 1;
  cause_c                                           cause;
  drb_required_to_rem_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Required-To-Modify-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_required_to_modify_item_eutran_ext_ies_o = protocol_ext_empty_o;

// DRB-Required-To-Modify-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Required-To-Modify-Item-NG-RAN
using drb_required_to_modify_list_ng_ran_l = dyn_array<drb_required_to_modify_item_ng_ran_s>;

// DRB-Required-To-Remove-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_required_to_rem_item_eutran_ext_ies_o = protocol_ext_empty_o;

// DRB-Required-To-Remove-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Required-To-Remove-Item-NG-RAN
using drb_required_to_rem_list_ng_ran_l = dyn_array<drb_required_to_rem_item_ng_ran_s>;

// PDU-Session-Resource-Required-To-Modify-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct pdu_session_res_required_to_modify_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { redundant_n_g_dl_up_tnl_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::redundant_n_g_dl_up_tnl_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    up_tnl_info_c&       redundant_n_g_dl_up_tnl_info() { return c; }
    const up_tnl_info_c& redundant_n_g_dl_up_tnl_info() const { return c; }

  private:
    up_tnl_info_c c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using drb_required_to_modify_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Required-To-Modify-Item-EUTRAN ::= SEQUENCE
struct drb_required_to_modify_item_eutran_s {
  bool                                                 ext                       = false;
  bool                                                 s1_dl_up_tnl_info_present = false;
  bool                                                 cause_present             = false;
  bool                                                 ie_exts_present           = false;
  uint8_t                                              drb_id                    = 1;
  up_tnl_info_c                                        s1_dl_up_tnl_info;
  gnb_cu_up_cell_group_related_cfg_l                   gnb_cu_up_cell_group_related_cfg;
  cause_c                                              cause;
  drb_required_to_modify_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_required_to_rem_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Required-To-Remove-Item-EUTRAN ::= SEQUENCE
struct drb_required_to_rem_item_eutran_s {
  bool                                              ext             = false;
  bool                                              ie_exts_present = false;
  uint8_t                                           drb_id          = 1;
  cause_c                                           cause;
  drb_required_to_rem_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-Resource-Required-To-Modify-Item ::= SEQUENCE
struct pdu_session_res_required_to_modify_item_s {
  bool                                                                        ext                       = false;
  bool                                                                        ng_dl_up_tnl_info_present = false;
  uint16_t                                                                    pdu_session_id            = 0;
  up_tnl_info_c                                                               ng_dl_up_tnl_info;
  drb_required_to_modify_list_ng_ran_l                                        drb_required_to_modify_list_ng_ran;
  drb_required_to_rem_list_ng_ran_l                                           drb_required_to_rem_list_ng_ran;
  protocol_ext_container_l<pdu_session_res_required_to_modify_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Required-To-Modify-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Required-To-Modify-Item-EUTRAN
using drb_required_to_modify_list_eutran_l = dyn_array<drb_required_to_modify_item_eutran_s>;

// DRB-Required-To-Remove-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Required-To-Remove-Item-EUTRAN
using drb_required_to_rem_list_eutran_l = dyn_array<drb_required_to_rem_item_eutran_s>;

// PDU-Session-Resource-Required-To-Modify-List ::= SEQUENCE (SIZE (1..256)) OF
// PDU-Session-Resource-Required-To-Modify-Item
using pdu_session_res_required_to_modify_list_l = dyn_array<pdu_session_res_required_to_modify_item_s>;

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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                                                                    type_;
    choice_buffer_t<drb_required_to_modify_list_eutran_l, drb_required_to_rem_list_eutran_l> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                                                                     type_;
    choice_buffer_t<pdu_session_res_required_to_modify_list_l, pdu_session_res_to_rem_list_l> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                              type_;
    choice_buffer_t<sys_bearer_context_mod_required_c> c;

    void destroy_();
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

// QoS-Flow-Failed-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using qos_flow_failed_item_ext_ies_o = protocol_ext_empty_o;

using qos_flow_failed_item_ext_ies_container = protocol_ext_container_empty_l;

// QoS-Flow-Failed-Item ::= SEQUENCE
struct qos_flow_failed_item_s {
  bool                                   ext             = false;
  bool                                   ie_exts_present = false;
  uint8_t                                qos_flow_id     = 0;
  cause_c                                cause;
  qos_flow_failed_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Failed-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_failed_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

// DRB-Failed-Mod-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_failed_mod_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

// DRB-Failed-To-Modify-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_failed_to_modify_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

// DRB-Modified-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct drb_modified_item_ng_ran_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { early_forwarding_count_info, old_qos_flow_map_ul_endmarkerexpected, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    early_forwarding_count_info_c&       early_forwarding_count_info();
    qos_flow_list_l&                     old_qos_flow_map_ul_endmarkerexpected();
    const early_forwarding_count_info_c& early_forwarding_count_info() const;
    const qos_flow_list_l&               old_qos_flow_map_ul_endmarkerexpected() const;

  private:
    types                                                           type_;
    choice_buffer_t<early_forwarding_count_info_c, qos_flow_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRB-Setup-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_setup_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

// DRB-Setup-Mod-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_setup_mod_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

// QoS-Flow-Failed-List ::= SEQUENCE (SIZE (1..64)) OF QoS-Flow-Failed-Item
using qos_flow_failed_list_l = dyn_array<qos_flow_failed_item_s>;

// ConfidentialityProtectionResult ::= ENUMERATED
struct confidentiality_protection_result_opts {
  enum options { performed, not_performed, /*...*/ nulltype } value;

  const char* to_string() const;
};
using confidentiality_protection_result_e = enumerated<confidentiality_protection_result_opts, true>;

using drb_failed_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Failed-Item-NG-RAN ::= SEQUENCE
struct drb_failed_item_ng_ran_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  uint8_t                                  drb_id          = 1;
  cause_c                                  cause;
  drb_failed_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_failed_mod_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Failed-Mod-Item-NG-RAN ::= SEQUENCE
struct drb_failed_mod_item_ng_ran_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  uint8_t                                      drb_id          = 1;
  cause_c                                      cause;
  drb_failed_mod_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_failed_to_modify_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Failed-To-Modify-Item-NG-RAN ::= SEQUENCE
struct drb_failed_to_modify_item_ng_ran_s {
  bool                                               ext             = false;
  bool                                               ie_exts_present = false;
  uint8_t                                            drb_id          = 1;
  cause_c                                            cause;
  drb_failed_to_modify_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct drb_modified_item_ng_ran_ext_ies_container {
  bool                          early_forwarding_count_info_present           = false;
  bool                          old_qos_flow_map_ul_endmarkerexpected_present = false;
  early_forwarding_count_info_c early_forwarding_count_info;
  qos_flow_list_l               old_qos_flow_map_ul_endmarkerexpected;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Modified-Item-NG-RAN ::= SEQUENCE
struct drb_modified_item_ng_ran_s {
  bool                                       ext                         = false;
  bool                                       pdcp_sn_status_info_present = false;
  bool                                       ie_exts_present             = false;
  uint8_t                                    drb_id                      = 1;
  up_params_l                                ul_up_transport_params;
  pdcp_sn_status_info_s                      pdcp_sn_status_info;
  qos_flow_list_l                            flow_setup_list;
  qos_flow_failed_list_l                     flow_failed_list;
  drb_modified_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_setup_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Setup-Item-NG-RAN ::= SEQUENCE
struct drb_setup_item_ng_ran_s {
  bool                                    ext                                   = false;
  bool                                    drb_data_forwarding_info_resp_present = false;
  bool                                    ie_exts_present                       = false;
  uint8_t                                 drb_id                                = 1;
  data_forwarding_info_s                  drb_data_forwarding_info_resp;
  up_params_l                             ul_up_transport_params;
  qos_flow_list_l                         flow_setup_list;
  qos_flow_failed_list_l                  flow_failed_list;
  drb_setup_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_setup_mod_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Setup-Mod-Item-NG-RAN ::= SEQUENCE
struct drb_setup_mod_item_ng_ran_s {
  bool                                        ext                                   = false;
  bool                                        drb_data_forwarding_info_resp_present = false;
  bool                                        ie_exts_present                       = false;
  uint8_t                                     drb_id                                = 1;
  data_forwarding_info_s                      drb_data_forwarding_info_resp;
  up_params_l                                 ul_up_transport_params;
  qos_flow_list_l                             flow_setup_list;
  qos_flow_failed_list_l                      flow_failed_list;
  drb_setup_mod_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntegrityProtectionResult ::= ENUMERATED
struct integrity_protection_result_opts {
  enum options { performed, not_performed, /*...*/ nulltype } value;

  const char* to_string() const;
};
using integrity_protection_result_e = enumerated<integrity_protection_result_opts, true>;

// QoS-Flow-Removed-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using qos_flow_remd_item_ext_ies_o = protocol_ext_empty_o;

// SecurityResult-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using security_result_ext_ies_o = protocol_ext_empty_o;

// DRB-Failed-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Failed-Item-NG-RAN
using drb_failed_list_ng_ran_l = dyn_array<drb_failed_item_ng_ran_s>;

// DRB-Failed-Mod-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_failed_mod_item_eutran_ext_ies_o = protocol_ext_empty_o;

// DRB-Failed-Mod-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Failed-Mod-Item-NG-RAN
using drb_failed_mod_list_ng_ran_l = dyn_array<drb_failed_mod_item_ng_ran_s>;

// DRB-Failed-To-Modify-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_failed_to_modify_item_eutran_ext_ies_o = protocol_ext_empty_o;

// DRB-Failed-To-Modify-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Failed-To-Modify-Item-NG-RAN
using drb_failed_to_modify_list_ng_ran_l = dyn_array<drb_failed_to_modify_item_ng_ran_s>;

// DRB-Modified-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_modified_item_eutran_ext_ies_o = protocol_ext_empty_o;

// DRB-Modified-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Modified-Item-NG-RAN
using drb_modified_list_ng_ran_l = dyn_array<drb_modified_item_ng_ran_s>;

// DRB-Removed-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_remd_item_ext_ies_o = protocol_ext_empty_o;

// DRB-Setup-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Setup-Item-NG-RAN
using drb_setup_list_ng_ran_l = dyn_array<drb_setup_item_ng_ran_s>;

// DRB-Setup-Mod-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct drb_setup_mod_item_eutran_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { data_forwarding_source_ip_address, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::data_forwarding_source_ip_address; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bounded_bitstring<1, 160, true, true>&       data_forwarding_source_ip_address() { return c; }
    const bounded_bitstring<1, 160, true, true>& data_forwarding_source_ip_address() const { return c; }

  private:
    bounded_bitstring<1, 160, true, true> c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRB-Setup-Mod-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Setup-Mod-Item-NG-RAN
using drb_setup_mod_list_ng_ran_l = dyn_array<drb_setup_mod_item_ng_ran_s>;

// PDU-Session-Resource-Failed-Mod-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdu_session_res_failed_mod_item_ext_ies_o = protocol_ext_empty_o;

// PDU-Session-Resource-Failed-To-Modify-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdu_session_res_failed_to_modify_item_ext_ies_o = protocol_ext_empty_o;

// PDU-Session-Resource-Modified-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct pdu_session_res_modified_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { redundant_n_g_dl_up_tnl_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::redundant_n_g_dl_up_tnl_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    up_tnl_info_c&       redundant_n_g_dl_up_tnl_info() { return c; }
    const up_tnl_info_c& redundant_n_g_dl_up_tnl_info() const { return c; }

  private:
    up_tnl_info_c c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDU-Session-Resource-Setup-Mod-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct pdu_session_res_setup_mod_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { redundant_n_g_dl_up_tnl_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::redundant_n_g_dl_up_tnl_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    up_tnl_info_c&       redundant_n_g_dl_up_tnl_info() { return c; }
    const up_tnl_info_c& redundant_n_g_dl_up_tnl_info() const { return c; }

  private:
    up_tnl_info_c c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using qos_flow_remd_item_ext_ies_container = protocol_ext_container_empty_l;

// QoS-Flow-Removed-Item ::= SEQUENCE
struct qos_flow_remd_item_s {
  struct qos_flow_released_in_session_opts {
    enum options { released_in_session, not_released_in_session, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using qos_flow_released_in_session_e_ = enumerated<qos_flow_released_in_session_opts, true>;

  // member variables
  bool                                 ext                                       = false;
  bool                                 qos_flow_released_in_session_present      = false;
  bool                                 qos_flow_accumulated_session_time_present = false;
  bool                                 ie_exts_present                           = false;
  uint8_t                              qos_flow_id                               = 0;
  qos_flow_released_in_session_e_      qos_flow_released_in_session;
  fixed_octstring<5, true>             qos_flow_accumulated_session_time;
  qos_flow_remd_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using security_result_ext_ies_container = protocol_ext_container_empty_l;

// SecurityResult ::= SEQUENCE
struct security_result_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  integrity_protection_result_e       integrity_protection_result;
  confidentiality_protection_result_e confidentiality_protection_result;
  security_result_ext_ies_container   ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_failed_mod_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Failed-Mod-Item-EUTRAN ::= SEQUENCE
struct drb_failed_mod_item_eutran_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  uint8_t                                      drb_id          = 1;
  cause_c                                      cause;
  drb_failed_mod_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_failed_to_modify_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Failed-To-Modify-Item-EUTRAN ::= SEQUENCE
struct drb_failed_to_modify_item_eutran_s {
  bool                                               ext             = false;
  bool                                               ie_exts_present = false;
  uint8_t                                            drb_id          = 1;
  cause_c                                            cause;
  drb_failed_to_modify_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_modified_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Modified-Item-EUTRAN ::= SEQUENCE
struct drb_modified_item_eutran_s {
  bool                                       ext                         = false;
  bool                                       s1_dl_up_tnl_info_present   = false;
  bool                                       pdcp_sn_status_info_present = false;
  bool                                       ie_exts_present             = false;
  uint8_t                                    drb_id                      = 1;
  up_tnl_info_c                              s1_dl_up_tnl_info;
  pdcp_sn_status_info_s                      pdcp_sn_status_info;
  up_params_l                                ul_up_transport_params;
  drb_modified_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_remd_item_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Removed-Item ::= SEQUENCE
struct drb_remd_item_s {
  struct drb_released_in_session_opts {
    enum options { released_in_session, not_released_in_session, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using drb_released_in_session_e_ = enumerated<drb_released_in_session_opts, true>;
  using qos_flow_remd_list_l_      = dyn_array<qos_flow_remd_item_s>;

  // member variables
  bool                            ext                                  = false;
  bool                            drb_released_in_session_present      = false;
  bool                            drb_accumulated_session_time_present = false;
  bool                            ie_exts_present                      = false;
  uint8_t                         drb_id                               = 1;
  drb_released_in_session_e_      drb_released_in_session;
  fixed_octstring<5, true>        drb_accumulated_session_time;
  qos_flow_remd_list_l_           qos_flow_remd_list;
  drb_remd_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Setup-Mod-Item-EUTRAN ::= SEQUENCE
struct drb_setup_mod_item_eutran_s {
  bool                                                          ext                               = false;
  bool                                                          data_forwarding_info_resp_present = false;
  uint8_t                                                       drb_id                            = 1;
  up_tnl_info_c                                                 s1_dl_up_tnl_info;
  data_forwarding_info_s                                        data_forwarding_info_resp;
  up_params_l                                                   ul_up_transport_params;
  protocol_ext_container_l<drb_setup_mod_item_eutran_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_failed_mod_item_ext_ies_container = protocol_ext_container_empty_l;

// PDU-Session-Resource-Failed-Mod-Item ::= SEQUENCE
struct pdu_session_res_failed_mod_item_s {
  bool                                              ext             = false;
  bool                                              ie_exts_present = false;
  uint16_t                                          pdu_session_id  = 0;
  cause_c                                           cause;
  pdu_session_res_failed_mod_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_failed_to_modify_item_ext_ies_container = protocol_ext_container_empty_l;

// PDU-Session-Resource-Failed-To-Modify-Item ::= SEQUENCE
struct pdu_session_res_failed_to_modify_item_s {
  bool                                                    ext             = false;
  bool                                                    ie_exts_present = false;
  uint16_t                                                pdu_session_id  = 0;
  cause_c                                                 cause;
  pdu_session_res_failed_to_modify_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-Resource-Modified-Item ::= SEQUENCE
struct pdu_session_res_modified_item_s {
  bool                               ext                                           = false;
  bool                               ng_dl_up_tnl_info_present                     = false;
  bool                               security_result_present                       = false;
  bool                               pdu_session_data_forwarding_info_resp_present = false;
  uint16_t                           pdu_session_id                                = 0;
  up_tnl_info_c                      ng_dl_up_tnl_info;
  security_result_s                  security_result;
  data_forwarding_info_s             pdu_session_data_forwarding_info_resp;
  drb_setup_list_ng_ran_l            drb_setup_list_ng_ran;
  drb_failed_list_ng_ran_l           drb_failed_list_ng_ran;
  drb_modified_list_ng_ran_l         drb_modified_list_ng_ran;
  drb_failed_to_modify_list_ng_ran_l drb_failed_to_modify_list_ng_ran;
  protocol_ext_container_l<pdu_session_res_modified_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-Resource-Setup-Mod-Item ::= SEQUENCE
struct pdu_session_res_setup_mod_item_s {
  bool                         ext                                           = false;
  bool                         security_result_present                       = false;
  bool                         pdu_session_data_forwarding_info_resp_present = false;
  uint16_t                     pdu_session_id                                = 0;
  security_result_s            security_result;
  up_tnl_info_c                ng_dl_up_tnl_info;
  data_forwarding_info_s       pdu_session_data_forwarding_info_resp;
  drb_setup_mod_list_ng_ran_l  drb_setup_mod_list_ng_ran;
  drb_failed_mod_list_ng_ran_l drb_failed_mod_list_ng_ran;
  protocol_ext_container_l<pdu_session_res_setup_mod_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Failed-Mod-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Failed-Mod-Item-EUTRAN
using drb_failed_mod_list_eutran_l = dyn_array<drb_failed_mod_item_eutran_s>;

// DRB-Failed-To-Modify-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Failed-To-Modify-Item-EUTRAN
using drb_failed_to_modify_list_eutran_l = dyn_array<drb_failed_to_modify_item_eutran_s>;

// DRB-Modified-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Modified-Item-EUTRAN
using drb_modified_list_eutran_l = dyn_array<drb_modified_item_eutran_s>;

// DRB-Setup-Mod-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Setup-Mod-Item-EUTRAN
using drb_setup_mod_list_eutran_l = dyn_array<drb_setup_mod_item_eutran_s>;

// PDU-Session-Resource-Failed-Mod-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-Failed-Mod-Item
using pdu_session_res_failed_mod_list_l = dyn_array<pdu_session_res_failed_mod_item_s>;

// PDU-Session-Resource-Failed-To-Modify-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-Failed-To-Modify-Item
using pdu_session_res_failed_to_modify_list_l = dyn_array<pdu_session_res_failed_to_modify_item_s>;

// PDU-Session-Resource-Modified-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-Modified-Item
using pdu_session_res_modified_list_l = dyn_array<pdu_session_res_modified_item_s>;

// PDU-Session-Resource-Setup-Mod-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-Setup-Mod-Item
using pdu_session_res_setup_mod_list_l = dyn_array<pdu_session_res_setup_mod_item_s>;

// RetainabilityMeasurementsInfo ::= SEQUENCE (SIZE (1..32)) OF DRB-Removed-Item
using retainability_meass_info_l = dyn_array<drb_remd_item_s>;

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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types type_;
    choice_buffer_t<drb_failed_mod_list_eutran_l,
                    drb_failed_to_modify_list_eutran_l,
                    drb_modified_list_eutran_l,
                    drb_setup_mod_list_eutran_l,
                    retainability_meass_info_l>
        c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types type_;
    choice_buffer_t<pdu_session_res_failed_mod_list_l,
                    pdu_session_res_failed_to_modify_list_l,
                    pdu_session_res_modified_list_l,
                    pdu_session_res_setup_mod_list_l,
                    retainability_meass_info_l>
        c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                          type_;
    choice_buffer_t<sys_bearer_context_mod_resp_c> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                    type_;
    choice_buffer_t<cause_c> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                                           type_;
    choice_buffer_t<crit_diagnostics_s, retainability_meass_info_l> c;

    void destroy_();
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

// DRB-Status-ItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_status_item_ext_ies_o = protocol_ext_empty_o;

using drb_status_item_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Status-Item ::= SEQUENCE
struct drb_status_item_s {
  bool                              ext                   = false;
  bool                              pdcp_dl_count_present = false;
  bool                              pdcp_ul_count_present = false;
  bool                              ie_exts_present       = false;
  uint8_t                           drb_id                = 1;
  pdcp_count_s                      pdcp_dl_count;
  pdcp_count_s                      pdcp_ul_count;
  drb_status_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                       type_;
    choice_buffer_t<cause_c, drb_status_list_l> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
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

// Links-to-log ::= ENUMERATED
struct links_to_log_opts {
  enum options { ul, dl, both_ul_and_dl, /*...*/ nulltype } value;

  const char* to_string() const;
};
using links_to_log_e = enumerated<links_to_log_opts, true>;

// M4Configuration-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using m4_cfg_ext_ies_o = protocol_ext_empty_o;

// M4period ::= ENUMERATED
struct m4period_opts {
  enum options { ms1024, ms2048, ms5120, ms10240, min1, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using m4period_e = enumerated<m4period_opts, true>;

// M6Configuration-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using m6_cfg_ext_ies_o = protocol_ext_empty_o;

// M6report-Interval ::= ENUMERATED
struct m6report_interv_opts {
  enum options {
    ms120,
    ms240,
    ms480,
    ms640,
    ms1024,
    ms2048,
    ms5120,
    ms10240,
    ms20480,
    ms40960,
    min1,
    min6,
    min12,
    min30,
    // ...
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using m6report_interv_e = enumerated<m6report_interv_opts, true>;

// M7Configuration-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using m7_cfg_ext_ies_o = protocol_ext_empty_o;

// RSN ::= ENUMERATED
struct r_sn_opts {
  enum options { v1, v2, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using r_sn_e = enumerated<r_sn_opts, true>;

// RedundantPDUSessionInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using redundant_pdu_session_info_ext_ies_o = protocol_ext_empty_o;

// ImmediateMDT-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using immediate_mdt_ext_ies_o = protocol_ext_empty_o;

using m4_cfg_ext_ies_container = protocol_ext_container_empty_l;

// M4Configuration ::= SEQUENCE
struct m4_cfg_s {
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  m4period_e               m4period;
  links_to_log_e           m4_links_to_log;
  m4_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using m6_cfg_ext_ies_container = protocol_ext_container_empty_l;

// M6Configuration ::= SEQUENCE
struct m6_cfg_s {
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  m6report_interv_e        m6report_interv;
  links_to_log_e           m6_links_to_log;
  m6_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using m7_cfg_ext_ies_container = protocol_ext_container_empty_l;

// M7Configuration ::= SEQUENCE
struct m7_cfg_s {
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  uint8_t                  m7period        = 1;
  links_to_log_e           m7_links_to_log;
  m7_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using redundant_pdu_session_info_ext_ies_container = protocol_ext_container_empty_l;

// RedundantPDUSessionInformation ::= SEQUENCE
struct redundant_pdu_session_info_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  r_sn_e                                       rsn;
  redundant_pdu_session_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-To-Setup-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct drb_to_setup_item_eutran_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { data_forwarding_source_ip_address, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::data_forwarding_source_ip_address; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bounded_bitstring<1, 160, true, true>&       data_forwarding_source_ip_address() { return c; }
    const bounded_bitstring<1, 160, true, true>& data_forwarding_source_ip_address() const { return c; }

  private:
    bounded_bitstring<1, 160, true, true> c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using immediate_mdt_ext_ies_container = protocol_ext_container_empty_l;

// ImmediateMDT ::= SEQUENCE
struct immediate_mdt_s {
  bool                            ext                = false;
  bool                            meas_four_present  = false;
  bool                            meas_six_present   = false;
  bool                            meas_seven_present = false;
  bool                            ie_exts_present    = false;
  fixed_bitstring<8, false, true> meass_to_activ;
  m4_cfg_s                        meas_four;
  m6_cfg_s                        meas_six;
  m7_cfg_s                        meas_seven;
  immediate_mdt_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MDTMode-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using mdt_mode_ext_ies_o = protocol_ies_empty_o;

// PDU-Session-Resource-To-Setup-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct pdu_session_res_to_setup_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        common_network_instance,
        redundant_n_g_ul_up_tnl_info,
        redundant_common_network_instance,
        redundant_pdu_session_info,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    unbounded_octstring<true>&          common_network_instance();
    up_tnl_info_c&                      redundant_n_g_ul_up_tnl_info();
    unbounded_octstring<true>&          redundant_common_network_instance();
    redundant_pdu_session_info_s&       redundant_pdu_session_info();
    const unbounded_octstring<true>&    common_network_instance() const;
    const up_tnl_info_c&                redundant_n_g_ul_up_tnl_info() const;
    const unbounded_octstring<true>&    redundant_common_network_instance() const;
    const redundant_pdu_session_info_s& redundant_pdu_session_info() const;

  private:
    types                                                                                   type_;
    choice_buffer_t<redundant_pdu_session_info_s, unbounded_octstring<true>, up_tnl_info_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRB-To-Setup-Item-EUTRAN ::= SEQUENCE
struct drb_to_setup_item_eutran_s {
  bool                                                         ext                                          = false;
  bool                                                         data_forwarding_info_request_present         = false;
  bool                                                         drb_inactivity_timer_present                 = false;
  bool                                                         existing_allocated_s1_dl_up_tnl_info_present = false;
  uint8_t                                                      drb_id                                       = 1;
  pdcp_cfg_s                                                   pdcp_cfg;
  eutran_qos_s                                                 eutran_qos;
  up_tnl_info_c                                                s1_ul_up_tnl_info;
  data_forwarding_info_request_s                               data_forwarding_info_request;
  cell_group_info_l                                            cell_group_info;
  up_params_l                                                  dl_up_params;
  uint16_t                                                     drb_inactivity_timer = 1;
  up_tnl_info_c                                                existing_allocated_s1_dl_up_tnl_info;
  protocol_ext_container_l<drb_to_setup_item_eutran_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MDT-Activation ::= ENUMERATED
struct mdt_activation_opts {
  enum options { immediate_mdt_only, immediate_mdt_and_trace, /*...*/ nulltype } value;

  const char* to_string() const;
};
using mdt_activation_e = enumerated<mdt_activation_opts, true>;

// MDT-Configuration-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mdt_cfg_ext_ies_o = protocol_ext_empty_o;

// MDTMode ::= CHOICE
struct mdt_mode_c {
  struct types_opts {
    enum options { immediate_mdt, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  mdt_mode_c() = default;
  mdt_mode_c(const mdt_mode_c& other);
  mdt_mode_c& operator=(const mdt_mode_c& other);
  ~mdt_mode_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  immediate_mdt_s& immediate_mdt()
  {
    assert_choice_type(types::immediate_mdt, type_, "MDTMode");
    return c.get<immediate_mdt_s>();
  }
  protocol_ie_single_container_s<mdt_mode_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "MDTMode");
    return c.get<protocol_ie_single_container_s<mdt_mode_ext_ies_o>>();
  }
  const immediate_mdt_s& immediate_mdt() const
  {
    assert_choice_type(types::immediate_mdt, type_, "MDTMode");
    return c.get<immediate_mdt_s>();
  }
  const protocol_ie_single_container_s<mdt_mode_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "MDTMode");
    return c.get<protocol_ie_single_container_s<mdt_mode_ext_ies_o>>();
  }
  immediate_mdt_s&                                    set_immediate_mdt();
  protocol_ie_single_container_s<mdt_mode_ext_ies_o>& set_choice_ext();

private:
  types                                                                                type_;
  choice_buffer_t<immediate_mdt_s, protocol_ie_single_container_s<mdt_mode_ext_ies_o>> c;

  void destroy_();
};

struct pdu_session_res_to_setup_item_ext_ies_container {
  bool                         common_network_instance_present           = false;
  bool                         redundant_n_g_ul_up_tnl_info_present      = false;
  bool                         redundant_common_network_instance_present = false;
  bool                         redundant_pdu_session_info_present        = false;
  unbounded_octstring<true>    common_network_instance;
  up_tnl_info_c                redundant_n_g_ul_up_tnl_info;
  unbounded_octstring<true>    redundant_common_network_instance;
  redundant_pdu_session_info_s redundant_pdu_session_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-Resource-To-Setup-Item ::= SEQUENCE
struct pdu_session_res_to_setup_item_s {
  bool                                            ext                                              = false;
  bool                                            pdu_session_res_dl_ambr_present                  = false;
  bool                                            pdu_session_data_forwarding_info_request_present = false;
  bool                                            pdu_session_inactivity_timer_present             = false;
  bool                                            existing_allocated_ng_dl_up_tnl_info_present     = false;
  bool                                            network_instance_present                         = false;
  bool                                            ie_exts_present                                  = false;
  uint16_t                                        pdu_session_id                                   = 0;
  pdu_session_type_e                              pdu_session_type;
  snssai_s                                        snssai;
  security_ind_s                                  security_ind;
  uint64_t                                        pdu_session_res_dl_ambr = 0;
  up_tnl_info_c                                   ng_ul_up_tnl_info;
  data_forwarding_info_request_s                  pdu_session_data_forwarding_info_request;
  uint16_t                                        pdu_session_inactivity_timer = 1;
  up_tnl_info_c                                   existing_allocated_ng_dl_up_tnl_info;
  uint16_t                                        network_instance = 1;
  drb_to_setup_list_ng_ran_l                      drb_to_setup_list_ng_ran;
  pdu_session_res_to_setup_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-To-Setup-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-To-Setup-Item-EUTRAN
using drb_to_setup_list_eutran_l = dyn_array<drb_to_setup_item_eutran_s>;

using mdt_cfg_ext_ies_container = protocol_ext_container_empty_l;

// MDT-Configuration ::= SEQUENCE
struct mdt_cfg_s {
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  mdt_activation_e          mdt_activation;
  mdt_mode_c                mdt_mode;
  mdt_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPNContextInfo-SNPN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using npn_context_info_sn_pn_ext_ies_o = protocol_ext_empty_o;

// PDU-Session-Resource-To-Setup-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-To-Setup-Item
using pdu_session_res_to_setup_list_l = dyn_array<pdu_session_res_to_setup_item_s>;

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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                                                         type_;
    choice_buffer_t<drb_to_setup_list_eutran_l, fixed_bitstring<32, false, true>> c;

    void destroy_();
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

// NPNContextInfo-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using npn_context_info_ext_ies_o = protocol_ies_empty_o;

using npn_context_info_sn_pn_ext_ies_container = protocol_ext_container_empty_l;

// NPNContextInfo-SNPN ::= SEQUENCE
struct npn_context_info_sn_pn_s {
  bool                                     ie_exts_present = false;
  fixed_bitstring<44, false, true>         nid;
  npn_context_info_sn_pn_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// System-BearerContextSetupRequest-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using sys_bearer_context_setup_request_ext_ies_o = protocol_ies_empty_o;

// TraceActivation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct trace_activation_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { mdt_cfg, trace_collection_entity_uri, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mdt_cfg_s&                                                   mdt_cfg();
    visible_string<0, MAX_ASN_STRING_LENGTH, false, true>&       trace_collection_entity_uri();
    const mdt_cfg_s&                                             mdt_cfg() const;
    const visible_string<0, MAX_ASN_STRING_LENGTH, false, true>& trace_collection_entity_uri() const;

  private:
    types                                                                             type_;
    choice_buffer_t<mdt_cfg_s, visible_string<0, MAX_ASN_STRING_LENGTH, false, true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TraceDepth ::= ENUMERATED
struct trace_depth_opts {
  enum options {
    minimum,
    medium,
    max,
    minimum_without_vendor_specific_ext,
    medium_without_vendor_specific_ext,
    max_without_vendor_specific_ext,
    // ...
    nulltype
  } value;

  const char* to_string() const;
};
using trace_depth_e = enumerated<trace_depth_opts, true>;

// AdditionalHandoverInfo ::= ENUMERATED
struct add_ho_info_opts {
  enum options { discard_pdpc_sn, /*...*/ nulltype } value;

  const char* to_string() const;
};
using add_ho_info_e = enumerated<add_ho_info_opts, true>;

// CHOInitiation ::= ENUMERATED
struct cho_initiation_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using cho_initiation_e = enumerated<cho_initiation_opts, true>;

// DirectForwardingPathAvailability ::= ENUMERATED
struct direct_forwarding_path_availability_opts {
  enum options { inter_sys_direct_path_available, /*...*/ intra_sys_direct_path_available, nulltype } value;

  const char* to_string() const;
};
using direct_forwarding_path_availability_e = enumerated<direct_forwarding_path_availability_opts, true, 1>;

// MDTPLMNList ::= SEQUENCE (SIZE (1..16)) OF OCTET STRING (SIZE (3))
using mdt_plmn_list_l = bounded_array<fixed_octstring<3, true>, 16>;

// NPNContextInfo ::= CHOICE
struct npn_context_info_c {
  struct types_opts {
    enum options { sn_pn, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  npn_context_info_c() = default;
  npn_context_info_c(const npn_context_info_c& other);
  npn_context_info_c& operator=(const npn_context_info_c& other);
  ~npn_context_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  npn_context_info_sn_pn_s& sn_pn()
  {
    assert_choice_type(types::sn_pn, type_, "NPNContextInfo");
    return c.get<npn_context_info_sn_pn_s>();
  }
  protocol_ie_single_container_s<npn_context_info_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "NPNContextInfo");
    return c.get<protocol_ie_single_container_s<npn_context_info_ext_ies_o>>();
  }
  const npn_context_info_sn_pn_s& sn_pn() const
  {
    assert_choice_type(types::sn_pn, type_, "NPNContextInfo");
    return c.get<npn_context_info_sn_pn_s>();
  }
  const protocol_ie_single_container_s<npn_context_info_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "NPNContextInfo");
    return c.get<protocol_ie_single_container_s<npn_context_info_ext_ies_o>>();
  }
  npn_context_info_sn_pn_s&                                   set_sn_pn();
  protocol_ie_single_container_s<npn_context_info_ext_ies_o>& set_choice_ext();

private:
  types                                                                                                 type_;
  choice_buffer_t<npn_context_info_sn_pn_s, protocol_ie_single_container_s<npn_context_info_ext_ies_o>> c;

  void destroy_();
};

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

struct trace_activation_ext_ies_container {
  bool                                                  mdt_cfg_present                     = false;
  bool                                                  trace_collection_entity_uri_present = false;
  mdt_cfg_s                                             mdt_cfg;
  visible_string<0, MAX_ASN_STRING_LENGTH, false, true> trace_collection_entity_uri;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TraceActivation ::= SEQUENCE
struct trace_activation_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  fixed_octstring<8, true>              trace_id;
  fixed_bitstring<8, false, true>       interfaces_to_trace;
  trace_depth_e                         trace_depth;
  bounded_bitstring<1, 160, true, true> trace_collection_entity_ip_address;
  trace_activation_ext_ies_container    ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types type_;
    choice_buffer_t<fixed_octstring<3, true>,
                    fixed_octstring<8, true>,
                    mdt_plmn_list_l,
                    npn_context_info_c,
                    security_info_s,
                    sys_bearer_context_setup_request_c,
                    trace_activation_s>
        c;

    void destroy_();
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

// DRB-Failed-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_failed_item_eutran_ext_ies_o = protocol_ext_empty_o;

// DRB-Setup-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct drb_setup_item_eutran_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { data_forwarding_source_ip_address, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::data_forwarding_source_ip_address; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bounded_bitstring<1, 160, true, true>&       data_forwarding_source_ip_address() { return c; }
    const bounded_bitstring<1, 160, true, true>& data_forwarding_source_ip_address() const { return c; }

  private:
    bounded_bitstring<1, 160, true, true> c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDU-Session-Resource-Failed-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdu_session_res_failed_item_ext_ies_o = protocol_ext_empty_o;

// PDU-Session-Resource-Setup-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct pdu_session_res_setup_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { redundant_n_g_dl_up_tnl_info, redundant_pdu_session_info_used, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    up_tnl_info_c&                      redundant_n_g_dl_up_tnl_info();
    redundant_pdu_session_info_s&       redundant_pdu_session_info_used();
    const up_tnl_info_c&                redundant_n_g_dl_up_tnl_info() const;
    const redundant_pdu_session_info_s& redundant_pdu_session_info_used() const;

  private:
    types                                                        type_;
    choice_buffer_t<redundant_pdu_session_info_s, up_tnl_info_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using drb_failed_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Failed-Item-EUTRAN ::= SEQUENCE
struct drb_failed_item_eutran_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  uint8_t                                  drb_id          = 1;
  cause_c                                  cause;
  drb_failed_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Setup-Item-EUTRAN ::= SEQUENCE
struct drb_setup_item_eutran_s {
  struct s1_dl_up_unchanged_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using s1_dl_up_unchanged_e_ = enumerated<s1_dl_up_unchanged_opts, true>;

  // member variables
  bool                                                      ext                               = false;
  bool                                                      data_forwarding_info_resp_present = false;
  bool                                                      s1_dl_up_unchanged_present        = false;
  uint8_t                                                   drb_id                            = 1;
  up_tnl_info_c                                             s1_dl_up_tnl_info;
  data_forwarding_info_s                                    data_forwarding_info_resp;
  up_params_l                                               ul_up_transport_params;
  s1_dl_up_unchanged_e_                                     s1_dl_up_unchanged;
  protocol_ext_container_l<drb_setup_item_eutran_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_failed_item_ext_ies_container = protocol_ext_container_empty_l;

// PDU-Session-Resource-Failed-Item ::= SEQUENCE
struct pdu_session_res_failed_item_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  uint16_t                                      pdu_session_id  = 0;
  cause_c                                       cause;
  pdu_session_res_failed_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct pdu_session_res_setup_item_ext_ies_container {
  bool                         redundant_n_g_dl_up_tnl_info_present    = false;
  bool                         redundant_pdu_session_info_used_present = false;
  up_tnl_info_c                redundant_n_g_dl_up_tnl_info;
  redundant_pdu_session_info_s redundant_pdu_session_info_used;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-Resource-Setup-Item ::= SEQUENCE
struct pdu_session_res_setup_item_s {
  struct ng_dl_up_unchanged_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using ng_dl_up_unchanged_e_ = enumerated<ng_dl_up_unchanged_opts, true>;

  // member variables
  bool                                         ext                                           = false;
  bool                                         security_result_present                       = false;
  bool                                         pdu_session_data_forwarding_info_resp_present = false;
  bool                                         ng_dl_up_unchanged_present                    = false;
  bool                                         ie_exts_present                               = false;
  uint16_t                                     pdu_session_id                                = 0;
  security_result_s                            security_result;
  up_tnl_info_c                                ng_dl_up_tnl_info;
  data_forwarding_info_s                       pdu_session_data_forwarding_info_resp;
  ng_dl_up_unchanged_e_                        ng_dl_up_unchanged;
  drb_setup_list_ng_ran_l                      drb_setup_list_ng_ran;
  drb_failed_list_ng_ran_l                     drb_failed_list_ng_ran;
  pdu_session_res_setup_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Failed-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Failed-Item-EUTRAN
using drb_failed_list_eutran_l = dyn_array<drb_failed_item_eutran_s>;

// DRB-Setup-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Setup-Item-EUTRAN
using drb_setup_list_eutran_l = dyn_array<drb_setup_item_eutran_s>;

// PDU-Session-Resource-Failed-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-Failed-Item
using pdu_session_res_failed_list_l = dyn_array<pdu_session_res_failed_item_s>;

// PDU-Session-Resource-Setup-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-Setup-Item
using pdu_session_res_setup_list_l = dyn_array<pdu_session_res_setup_item_s>;

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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                                              type_;
    choice_buffer_t<drb_failed_list_eutran_l, drb_setup_list_eutran_l> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                                                        type_;
    choice_buffer_t<pdu_session_res_failed_list_l, pdu_session_res_setup_list_l> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                            type_;
    choice_buffer_t<sys_bearer_context_setup_resp_c> c;

    void destroy_();
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

// Endpoint-IP-address-and-port-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using endpoint_ip_address_and_port_ext_ies_o = protocol_ext_empty_o;

using endpoint_ip_address_and_port_ext_ies_container = protocol_ext_container_empty_l;

// Endpoint-IP-address-and-port ::= SEQUENCE
struct endpoint_ip_address_and_port_s {
  bool                                           ie_exts_present = false;
  bounded_bitstring<1, 160, true, true>          endpoint_ip_address;
  fixed_bitstring<16, false, true>               port_num;
  endpoint_ip_address_and_port_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CP-TNL-Information-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct cp_tnl_info_ext_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { endpoint_ip_address_and_port, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::endpoint_ip_address_and_port; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    endpoint_ip_address_and_port_s&       endpoint_ip_address_and_port() { return c; }
    const endpoint_ip_address_and_port_s& endpoint_ip_address_and_port() const { return c; }

  private:
    endpoint_ip_address_and_port_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// CP-TNL-Information ::= CHOICE
struct cp_tnl_info_c {
  struct types_opts {
    enum options { endpoint_ip_address, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  cp_tnl_info_c() = default;
  cp_tnl_info_c(const cp_tnl_info_c& other);
  cp_tnl_info_c& operator=(const cp_tnl_info_c& other);
  ~cp_tnl_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  bounded_bitstring<1, 160, true, true>& endpoint_ip_address()
  {
    assert_choice_type(types::endpoint_ip_address, type_, "CP-TNL-Information");
    return c.get<bounded_bitstring<1, 160, true, true>>();
  }
  protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "CP-TNL-Information");
    return c.get<protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>>();
  }
  const bounded_bitstring<1, 160, true, true>& endpoint_ip_address() const
  {
    assert_choice_type(types::endpoint_ip_address, type_, "CP-TNL-Information");
    return c.get<bounded_bitstring<1, 160, true, true>>();
  }
  const protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "CP-TNL-Information");
    return c.get<protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>>();
  }
  bounded_bitstring<1, 160, true, true>&                 set_endpoint_ip_address();
  protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>& set_choice_ext();

private:
  types                                                                                                         type_;
  choice_buffer_t<bounded_bitstring<1, 160, true, true>, protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>> c;

  void destroy_();
};

// PrivacyIndicator ::= ENUMERATED
struct privacy_ind_opts {
  enum options { immediate_mdt, logged_mdt, /*...*/ nulltype } value;

  const char* to_string() const;
};
using privacy_ind_e = enumerated<privacy_ind_opts, true>;

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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types type_;
    choice_buffer_t<bounded_bitstring<1, 160, true, true>,
                    fixed_octstring<8, true>,
                    visible_string<0, MAX_ASN_STRING_LENGTH, false, true>>
        c;

    void destroy_();
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

// PDU-Session-To-Notify-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdu_session_to_notify_item_ext_ies_o = protocol_ext_empty_o;

using pdu_session_to_notify_item_ext_ies_container = protocol_ext_container_empty_l;

// PDU-Session-To-Notify-Item ::= SEQUENCE
struct pdu_session_to_notify_item_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  uint16_t                                     pdu_session_id  = 0;
  qos_flow_list_l                              qos_flow_list;
  pdu_session_to_notify_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-To-Notify-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-To-Notify-Item
using pdu_session_to_notify_list_l = dyn_array<pdu_session_to_notify_item_s>;

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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                         type_;
    choice_buffer_t<pdu_session_to_notify_list_l> c;

    void destroy_();
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

// DLUPTNLAddressToUpdateItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using dl_up_tnl_address_to_upd_item_ext_ies_o = protocol_ext_empty_o;

using dl_up_tnl_address_to_upd_item_ext_ies_container = protocol_ext_container_empty_l;

// DLUPTNLAddressToUpdateItem ::= SEQUENCE
struct dl_up_tnl_address_to_upd_item_s {
  bool                                            ext             = false;
  bool                                            ie_exts_present = false;
  bounded_bitstring<1, 160, true, true>           old_tnl_adress;
  bounded_bitstring<1, 160, true, true>           new_tnl_adress;
  dl_up_tnl_address_to_upd_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLUPTNLAddressToUpdateList ::= SEQUENCE (SIZE (1..8)) OF DLUPTNLAddressToUpdateItem
using dl_up_tnl_address_to_upd_list_l = dyn_array<dl_up_tnl_address_to_upd_item_s>;

// DRB-Measurement-Results-Information-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_meas_results_info_item_ext_ies_o = protocol_ext_empty_o;

using drb_meas_results_info_item_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Measurement-Results-Information-Item ::= SEQUENCE
struct drb_meas_results_info_item_s {
  bool                                         ext                  = false;
  bool                                         ul_d1_result_present = false;
  bool                                         ie_exts_present      = false;
  uint8_t                                      drb_id               = 1;
  uint16_t                                     ul_d1_result         = 0;
  drb_meas_results_info_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Measurement-Results-Information-List ::= SEQUENCE (SIZE (1..32)) OF DRB-Measurement-Results-Information-Item
using drb_meas_results_info_list_l = dyn_array<drb_meas_results_info_item_s>;

// DRB-Usage-Report-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_usage_report_item_ext_ies_o = protocol_ext_empty_o;

using drb_usage_report_item_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Usage-Report-Item ::= SEQUENCE
struct drb_usage_report_item_s {
  bool                                    ext             = false;
  bool                                    ie_exts_present = false;
  fixed_octstring<4, true>                start_time_stamp;
  fixed_octstring<4, true>                end_time_stamp;
  uint64_t                                usage_count_ul = 0;
  uint64_t                                usage_count_dl = 0;
  drb_usage_report_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Usage-Report-List ::= SEQUENCE (SIZE (1..2)) OF DRB-Usage-Report-Item
using drb_usage_report_list_l = dyn_array<drb_usage_report_item_s>;

// DRBs-Subject-To-Counter-Check-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drbs_subject_to_counter_check_item_eutran_ext_ies_o = protocol_ext_empty_o;

using drbs_subject_to_counter_check_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRBs-Subject-To-Counter-Check-Item-EUTRAN ::= SEQUENCE
struct drbs_subject_to_counter_check_item_eutran_s {
  bool                                                        ext             = false;
  bool                                                        ie_exts_present = false;
  uint8_t                                                     drb_id          = 1;
  pdcp_count_s                                                pdcp_ul_count;
  pdcp_count_s                                                pdcp_dl_count;
  drbs_subject_to_counter_check_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-Subject-To-Counter-Check-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drbs_subject_to_counter_check_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

using drbs_subject_to_counter_check_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRBs-Subject-To-Counter-Check-Item-NG-RAN ::= SEQUENCE
struct drbs_subject_to_counter_check_item_ng_ran_s {
  bool                                                        ext             = false;
  bool                                                        ie_exts_present = false;
  uint16_t                                                    pdu_session_id  = 0;
  uint8_t                                                     drb_id          = 1;
  pdcp_count_s                                                pdcp_ul_count;
  pdcp_count_s                                                pdcp_dl_count;
  drbs_subject_to_counter_check_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-Subject-To-Counter-Check-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRBs-Subject-To-Counter-Check-Item-EUTRAN
using drbs_subject_to_counter_check_list_eutran_l = dyn_array<drbs_subject_to_counter_check_item_eutran_s>;

// DRBs-Subject-To-Counter-Check-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRBs-Subject-To-Counter-Check-Item-NG-RAN
using drbs_subject_to_counter_check_list_ng_ran_l = dyn_array<drbs_subject_to_counter_check_item_ng_ran_s>;

// DRBs-Subject-To-Early-Forwarding-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drbs_subject_to_early_forwarding_item_ext_ies_o = protocol_ext_empty_o;

using drbs_subject_to_early_forwarding_item_ext_ies_container = protocol_ext_container_empty_l;

// DRBs-Subject-To-Early-Forwarding-Item ::= SEQUENCE
struct drbs_subject_to_early_forwarding_item_s {
  bool                                                    ext             = false;
  bool                                                    ie_exts_present = false;
  uint8_t                                                 drb_id          = 1;
  pdcp_count_s                                            dl_count_value;
  drbs_subject_to_early_forwarding_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-Subject-To-Early-Forwarding-List ::= SEQUENCE (SIZE (1..32)) OF DRBs-Subject-To-Early-Forwarding-Item
using drbs_subject_to_early_forwarding_list_l = dyn_array<drbs_subject_to_early_forwarding_item_s>;

// Data-Usage-Report-ItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using data_usage_report_item_ext_ies_o = protocol_ext_empty_o;

using data_usage_report_item_ext_ies_container = protocol_ext_container_empty_l;

// Data-Usage-Report-Item ::= SEQUENCE
struct data_usage_report_item_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  uint8_t                                  drb_id          = 1;
  rat_type_e                               rat_type;
  drb_usage_report_list_l                  drb_usage_report_list;
  data_usage_report_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Data-Usage-Report-List ::= SEQUENCE (SIZE (1..32)) OF Data-Usage-Report-Item
using data_usage_report_list_l = dyn_array<data_usage_report_item_s>;

// MRDC-Data-Usage-Report-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mrdc_data_usage_report_item_ext_ies_o = protocol_ext_empty_o;

// Data-Usage-per-PDU-Session-Report-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using data_usage_per_pdu_session_report_ext_ies_o = protocol_ext_empty_o;

using mrdc_data_usage_report_item_ext_ies_container = protocol_ext_container_empty_l;

// MRDC-Data-Usage-Report-Item ::= SEQUENCE
struct mrdc_data_usage_report_item_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  fixed_octstring<4, true>                      start_time_stamp;
  fixed_octstring<4, true>                      end_time_stamp;
  uint64_t                                      usage_count_ul = 0;
  uint64_t                                      usage_count_dl = 0;
  mrdc_data_usage_report_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using data_usage_per_pdu_session_report_ext_ies_container = protocol_ext_container_empty_l;

// Data-Usage-per-PDU-Session-Report ::= SEQUENCE
struct data_usage_per_pdu_session_report_s {
  struct secondary_rat_type_opts {
    enum options { nr, e_utra, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using secondary_rat_type_e_            = enumerated<secondary_rat_type_opts, true>;
  using pdu_session_timed_report_list_l_ = dyn_array<mrdc_data_usage_report_item_s>;

  // member variables
  bool                                                ext             = false;
  bool                                                ie_exts_present = false;
  secondary_rat_type_e_                               secondary_rat_type;
  pdu_session_timed_report_list_l_                    pdu_session_timed_report_list;
  data_usage_per_pdu_session_report_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Data-Usage-per-QoS-Flow-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using data_usage_per_qos_flow_item_ext_ies_o = protocol_ext_empty_o;

using data_usage_per_qos_flow_item_ext_ies_container = protocol_ext_container_empty_l;

// Data-Usage-per-QoS-Flow-Item ::= SEQUENCE
struct data_usage_per_qos_flow_item_s {
  struct secondary_rat_type_opts {
    enum options { nr, e_utra, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using secondary_rat_type_e_         = enumerated<secondary_rat_type_opts, true>;
  using qos_flow_timed_report_list_l_ = dyn_array<mrdc_data_usage_report_item_s>;

  // member variables
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  uint8_t                                        qos_flow_id     = 0;
  secondary_rat_type_e_                          secondary_rat_type;
  qos_flow_timed_report_list_l_                  qos_flow_timed_report_list;
  data_usage_per_qos_flow_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Data-Usage-per-QoS-Flow-List ::= SEQUENCE (SIZE (1..64)) OF Data-Usage-per-QoS-Flow-Item
using data_usage_per_qos_flow_list_l = dyn_array<data_usage_per_qos_flow_item_s>;

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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                     type_;
    choice_buffer_t<data_usage_report_list_l> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                     type_;
    choice_buffer_t<fixed_octstring<8, true>> c;

    void destroy_();
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

// NR-CGI-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using nr_cgi_ext_ies_o = protocol_ext_empty_o;

// EUTRAN-QoS-Support-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using eutran_qos_support_item_ext_ies_o = protocol_ext_empty_o;

// Extended-NR-CGI-Support-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using extended_nr_cgi_support_item_ext_ies_o = protocol_ext_empty_o;

// GTPTLA-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using gtp_tla_item_ext_ies_o = protocol_ext_empty_o;

// NG-RAN-QoS-Support-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using ng_ran_qos_support_item_ext_ies_o = protocol_ext_empty_o;

// NPNSupportInfo-SNPN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using npn_support_info_sn_pn_ext_ies_o = protocol_ext_empty_o;

using nr_cgi_ext_ies_container = protocol_ext_container_empty_l;

// NR-CGI ::= SEQUENCE
struct nr_cgi_s {
  bool                             ie_exts_present = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<36, false, true> nr_cell_id;
  nr_cgi_ext_ies_container         ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Slice-Support-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using slice_support_item_ext_ies_o = protocol_ext_empty_o;

using eutran_qos_support_item_ext_ies_container = protocol_ext_container_empty_l;

// EUTRAN-QoS-Support-Item ::= SEQUENCE
struct eutran_qos_support_item_s {
  bool                                      ie_exts_present = false;
  eutran_qos_s                              eutran_qos;
  eutran_qos_support_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using extended_nr_cgi_support_item_ext_ies_container = protocol_ext_container_empty_l;

// Extended-NR-CGI-Support-Item ::= SEQUENCE
struct extended_nr_cgi_support_item_s {
  bool                                           ie_exts_present = false;
  nr_cgi_s                                       nr_cgi;
  extended_nr_cgi_support_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using gtp_tla_item_ext_ies_container = protocol_ext_container_empty_l;

// GTPTLA-Item ::= SEQUENCE
struct gtp_tla_item_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  bounded_bitstring<1, 160, true, true> gtp_transport_layer_addresses;
  gtp_tla_item_ext_ies_container        ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using ng_ran_qos_support_item_ext_ies_container = protocol_ext_container_empty_l;

// NG-RAN-QoS-Support-Item ::= SEQUENCE
struct ng_ran_qos_support_item_s {
  bool                                      ie_exts_present = false;
  non_dyn_5qi_descriptor_s                  non_dyn_5qi_descriptor;
  ng_ran_qos_support_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPNSupportInfo-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using npn_support_info_ext_ies_o = protocol_ies_empty_o;

using npn_support_info_sn_pn_ext_ies_container = protocol_ext_container_empty_l;

// NPNSupportInfo-SNPN ::= SEQUENCE
struct npn_support_info_sn_pn_s {
  bool                                     ie_exts_present = false;
  fixed_bitstring<44, false, true>         nid;
  npn_support_info_sn_pn_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-CGI-Support-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using nr_cgi_support_item_ext_ies_o = protocol_ext_empty_o;

using slice_support_item_ext_ies_container = protocol_ext_container_empty_l;

// Slice-Support-Item ::= SEQUENCE
struct slice_support_item_s {
  bool                                 ie_exts_present = false;
  snssai_s                             snssai;
  slice_support_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-associatedLogicalE1-ConnectionItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using ue_associated_lc_e1_conn_item_ext_ies_o = protocol_ext_empty_o;

// EUTRAN-QoS-Support-List ::= SEQUENCE (SIZE (1..256)) OF EUTRAN-QoS-Support-Item
using eutran_qos_support_list_l = dyn_array<eutran_qos_support_item_s>;

// Extended-NR-CGI-Support-List ::= SEQUENCE (SIZE (1..16384)) OF Extended-NR-CGI-Support-Item
using extended_nr_cgi_support_list_l = dyn_array<extended_nr_cgi_support_item_s>;

// ExtendedSliceSupportList ::= SEQUENCE (SIZE (1..65535)) OF Slice-Support-Item
using extended_slice_support_list_l = dyn_array<slice_support_item_s>;

// GTPTLAs ::= SEQUENCE (SIZE (1..16)) OF GTPTLA-Item
using gtp_tlas_l = dyn_array<gtp_tla_item_s>;

// MRDC-Usage-Information-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mrdc_usage_info_ext_ies_o = protocol_ext_empty_o;

// NG-RAN-QoS-Support-List ::= SEQUENCE (SIZE (1..256)) OF NG-RAN-QoS-Support-Item
using ng_ran_qos_support_list_l = dyn_array<ng_ran_qos_support_item_s>;

// NPNSupportInfo ::= CHOICE
struct npn_support_info_c {
  struct types_opts {
    enum options { sn_pn, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  npn_support_info_c() = default;
  npn_support_info_c(const npn_support_info_c& other);
  npn_support_info_c& operator=(const npn_support_info_c& other);
  ~npn_support_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  npn_support_info_sn_pn_s& sn_pn()
  {
    assert_choice_type(types::sn_pn, type_, "NPNSupportInfo");
    return c.get<npn_support_info_sn_pn_s>();
  }
  protocol_ie_single_container_s<npn_support_info_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "NPNSupportInfo");
    return c.get<protocol_ie_single_container_s<npn_support_info_ext_ies_o>>();
  }
  const npn_support_info_sn_pn_s& sn_pn() const
  {
    assert_choice_type(types::sn_pn, type_, "NPNSupportInfo");
    return c.get<npn_support_info_sn_pn_s>();
  }
  const protocol_ie_single_container_s<npn_support_info_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "NPNSupportInfo");
    return c.get<protocol_ie_single_container_s<npn_support_info_ext_ies_o>>();
  }
  npn_support_info_sn_pn_s&                                   set_sn_pn();
  protocol_ie_single_container_s<npn_support_info_ext_ies_o>& set_choice_ext();

private:
  types                                                                                                 type_;
  choice_buffer_t<npn_support_info_sn_pn_s, protocol_ie_single_container_s<npn_support_info_ext_ies_o>> c;

  void destroy_();
};

using nr_cgi_support_item_ext_ies_container = protocol_ext_container_empty_l;

// NR-CGI-Support-Item ::= SEQUENCE
struct nr_cgi_support_item_s {
  bool                                  ie_exts_present = false;
  nr_cgi_s                              nr_cgi;
  nr_cgi_support_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoS-Parameters-Support-List-ItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using qos_params_support_list_item_ext_ies_o = protocol_ext_empty_o;

// Transport-UP-Layer-Addresses-Info-To-Add-ItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using transport_up_layer_addresses_info_to_add_item_ext_ies_o = protocol_ext_empty_o;

// Transport-UP-Layer-Addresses-Info-To-Remove-ItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using transport_up_layer_addresses_info_to_rem_item_ext_ies_o = protocol_ext_empty_o;

using ue_associated_lc_e1_conn_item_ext_ies_container = protocol_ext_container_empty_l;

// UE-associatedLogicalE1-ConnectionItem ::= SEQUENCE
struct ue_associated_lc_e1_conn_item_s {
  bool                                            ext                          = false;
  bool                                            gnb_cu_cp_ue_e1ap_id_present = false;
  bool                                            gnb_cu_up_ue_e1ap_id_present = false;
  bool                                            ie_exts_present              = false;
  uint64_t                                        gnb_cu_cp_ue_e1ap_id         = 0;
  uint64_t                                        gnb_cu_up_ue_e1ap_id         = 0;
  ue_associated_lc_e1_conn_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-CP-TNLA-Failed-To-Setup-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using gnb_cu_cp_tnl_a_failed_to_setup_item_ext_ies_o = protocol_ext_empty_o;

// GNB-CU-CP-TNLA-Setup-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using gnb_cu_cp_tnl_a_setup_item_ext_ies_o = protocol_ext_empty_o;

// GNB-CU-CP-TNLA-To-Add-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using gnb_cu_cp_tnl_a_to_add_item_ext_ies_o = protocol_ext_empty_o;

// GNB-CU-CP-TNLA-To-Remove-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct gnb_cu_cp_tnl_a_to_rem_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { tnl_assoc_transport_layer_address_gnb_cu_up, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::tnl_assoc_transport_layer_address_gnb_cu_up; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cp_tnl_info_c&       tnl_assoc_transport_layer_address_gnb_cu_up() { return c; }
    const cp_tnl_info_c& tnl_assoc_transport_layer_address_gnb_cu_up() const { return c; }

  private:
    cp_tnl_info_c c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CU-CP-TNLA-To-Update-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using gnb_cu_cp_tnl_a_to_upd_item_ext_ies_o = protocol_ext_empty_o;

// GNB-CU-UP-TNLA-To-Remove-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using gnb_cu_up_tnl_a_to_rem_item_ext_ies_o = protocol_ext_empty_o;

// IAB-donor-CU-UPPSKInfoItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using iab_donor_cu_up_psk_info_item_ext_ies_o = protocol_ext_empty_o;

using mrdc_usage_info_ext_ies_container = protocol_ext_container_empty_l;

// MRDC-Usage-Information ::= SEQUENCE
struct mrdc_usage_info_s {
  bool                                ext                                       = false;
  bool                                data_usage_per_pdu_session_report_present = false;
  bool                                ie_exts_present                           = false;
  data_usage_per_pdu_session_report_s data_usage_per_pdu_session_report;
  data_usage_per_qos_flow_list_l      data_usage_per_qos_flow_list;
  mrdc_usage_info_ext_ies_container   ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-CGI-Support-List ::= SEQUENCE (SIZE (1..512)) OF NR-CGI-Support-Item
using nr_cgi_support_list_l = dyn_array<nr_cgi_support_item_s>;

// PDU-Session-Resource-Data-Usage-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdu_session_res_data_usage_item_ext_ies_o = protocol_ext_empty_o;

// PrivateIE-ID ::= CHOICE
struct private_ie_id_c {
  struct types_opts {
    enum options { local, global, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  private_ie_id_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint32_t& local()
  {
    assert_choice_type(types::local, type_, "PrivateIE-ID");
    return c;
  }
  const uint32_t& local() const
  {
    assert_choice_type(types::local, type_, "PrivateIE-ID");
    return c;
  }
  uint32_t& set_local();
  void      set_global();

private:
  types    type_;
  uint32_t c;
};

using qos_params_support_list_item_ext_ies_container = protocol_ext_container_empty_l;

// QoS-Parameters-Support-List ::= SEQUENCE
struct qos_params_support_list_s {
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  eutran_qos_support_list_l                      eutran_qos_support_list;
  ng_ran_qos_support_list_l                      ng_ran_qos_support_list;
  qos_params_support_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Slice-Support-List ::= SEQUENCE (SIZE (1..1024)) OF Slice-Support-Item
using slice_support_list_l = dyn_array<slice_support_item_s>;

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
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
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
    types                                                                                              type_;
    choice_buffer_t<extended_nr_cgi_support_list_l, extended_slice_support_list_l, npn_support_info_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TNLAssociationUsage ::= ENUMERATED
struct tnl_assoc_usage_opts {
  enum options { ue, non_ue, both, /*...*/ nulltype } value;

  const char* to_string() const;
};
using tnl_assoc_usage_e = enumerated<tnl_assoc_usage_opts, true>;

using transport_up_layer_addresses_info_to_add_item_ext_ies_container = protocol_ext_container_empty_l;

// Transport-UP-Layer-Addresses-Info-To-Add-Item ::= SEQUENCE
struct transport_up_layer_addresses_info_to_add_item_s {
  bool                                                            ext             = false;
  bool                                                            ie_exts_present = false;
  bounded_bitstring<1, 160, true, true>                           ip_sec_transport_layer_address;
  gtp_tlas_l                                                      gtp_transport_layer_addresses_to_add;
  transport_up_layer_addresses_info_to_add_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using transport_up_layer_addresses_info_to_rem_item_ext_ies_container = protocol_ext_container_empty_l;

// Transport-UP-Layer-Addresses-Info-To-Remove-Item ::= SEQUENCE
struct transport_up_layer_addresses_info_to_rem_item_s {
  bool                                                            ext             = false;
  bool                                                            ie_exts_present = false;
  bounded_bitstring<1, 160, true, true>                           ip_sec_transport_layer_address;
  gtp_tlas_l                                                      gtp_transport_layer_addresses_to_rem;
  transport_up_layer_addresses_info_to_rem_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
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

// ULUPTNLAddressToUpdateItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using ul_up_tnl_address_to_upd_item_ext_ies_o = protocol_ext_empty_o;

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

// Extended-GNB-CU-CP-Name-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using extended_gnb_cu_cp_name_ext_ies_o = protocol_ext_empty_o;

// Extended-GNB-CU-UP-Name-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using extended_gnb_cu_up_name_ext_ies_o = protocol_ext_empty_o;

using gnb_cu_cp_tnl_a_failed_to_setup_item_ext_ies_container = protocol_ext_container_empty_l;

// GNB-CU-CP-TNLA-Failed-To-Setup-Item ::= SEQUENCE
struct gnb_cu_cp_tnl_a_failed_to_setup_item_s {
  bool                                                   ie_exts_present = false;
  cp_tnl_info_c                                          tnl_assoc_transport_layer_address;
  cause_c                                                cause;
  gnb_cu_cp_tnl_a_failed_to_setup_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using gnb_cu_cp_tnl_a_setup_item_ext_ies_container = protocol_ext_container_empty_l;

// GNB-CU-CP-TNLA-Setup-Item ::= SEQUENCE
struct gnb_cu_cp_tnl_a_setup_item_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  cp_tnl_info_c                                tnl_assoc_transport_layer_address;
  gnb_cu_cp_tnl_a_setup_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using gnb_cu_cp_tnl_a_to_add_item_ext_ies_container = protocol_ext_container_empty_l;

// GNB-CU-CP-TNLA-To-Add-Item ::= SEQUENCE
struct gnb_cu_cp_tnl_a_to_add_item_s {
  bool                                          ie_exts_present = false;
  cp_tnl_info_c                                 tnl_assoc_transport_layer_address;
  tnl_assoc_usage_e                             tnl_assoc_usage;
  gnb_cu_cp_tnl_a_to_add_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-CP-TNLA-To-Remove-Item ::= SEQUENCE
struct gnb_cu_cp_tnl_a_to_rem_item_s {
  cp_tnl_info_c                                                   tnl_assoc_transport_layer_address;
  protocol_ext_container_l<gnb_cu_cp_tnl_a_to_rem_item_ext_ies_o> ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using gnb_cu_cp_tnl_a_to_upd_item_ext_ies_container = protocol_ext_container_empty_l;

// GNB-CU-CP-TNLA-To-Update-Item ::= SEQUENCE
struct gnb_cu_cp_tnl_a_to_upd_item_s {
  bool                                          tnl_assoc_usage_present = false;
  bool                                          ie_exts_present         = false;
  cp_tnl_info_c                                 tnl_assoc_transport_layer_address;
  tnl_assoc_usage_e                             tnl_assoc_usage;
  gnb_cu_cp_tnl_a_to_upd_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using gnb_cu_up_tnl_a_to_rem_item_ext_ies_container = protocol_ext_container_empty_l;

// GNB-CU-UP-TNLA-To-Remove-Item ::= SEQUENCE
struct gnb_cu_up_tnl_a_to_rem_item_s {
  bool                                          tnl_assoc_transport_layer_address_gnb_cu_cp_present = false;
  bool                                          ie_exts_present                                     = false;
  cp_tnl_info_c                                 tnl_assoc_transport_layer_address;
  cp_tnl_info_c                                 tnl_assoc_transport_layer_address_gnb_cu_cp;
  gnb_cu_up_tnl_a_to_rem_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HW-CapacityIndicator-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using hw_capacity_ind_ext_ies_o = protocol_ext_empty_o;

using iab_donor_cu_up_psk_info_item_ext_ies_container = protocol_ext_container_empty_l;

// IAB-Donor-CU-UPPSKInfo-Item ::= SEQUENCE
struct iab_donor_cu_up_psk_info_item_s {
  bool                                            ext             = false;
  bool                                            ie_exts_present = false;
  unbounded_octstring<true>                       iab_donor_cu_up_psk;
  bounded_bitstring<1, 160, true, true>           iab_donor_cu_up_ip_address;
  bounded_bitstring<1, 160, true, true>           iab_du_ip_address;
  iab_donor_cu_up_psk_info_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
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

using pdu_session_res_data_usage_item_ext_ies_container = protocol_ext_container_empty_l;

// PDU-Session-Resource-Data-Usage-Item ::= SEQUENCE
struct pdu_session_res_data_usage_item_s {
  bool                                              ext             = false;
  bool                                              ie_exts_present = false;
  uint16_t                                          pdu_session_id  = 0;
  mrdc_usage_info_s                                 mrdc_usage_info;
  pdu_session_res_data_usage_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
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

// TNL-AvailableCapacityIndicator-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using tnl_available_capacity_ind_ext_ies_o = protocol_ext_empty_o;

// Transport-Layer-Address-Info-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using transport_layer_address_info_ext_ies_o = protocol_ext_empty_o;

// Transport-UP-Layer-Addresses-Info-To-Add-List ::= SEQUENCE (SIZE (1..16)) OF
// Transport-UP-Layer-Addresses-Info-To-Add-Item
using transport_up_layer_addresses_info_to_add_list_l = dyn_array<transport_up_layer_addresses_info_to_add_item_s>;

// Transport-UP-Layer-Addresses-Info-To-Remove-List ::= SEQUENCE (SIZE (1..16)) OF
// Transport-UP-Layer-Addresses-Info-To-Remove-Item
using transport_up_layer_addresses_info_to_rem_list_l = dyn_array<transport_up_layer_addresses_info_to_rem_item_s>;

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

using ul_up_tnl_address_to_upd_item_ext_ies_container = protocol_ext_container_empty_l;

// ULUPTNLAddressToUpdateItem ::= SEQUENCE
struct ul_up_tnl_address_to_upd_item_s {
  bool                                            ext             = false;
  bool                                            ie_exts_present = false;
  bounded_bitstring<1, 160, true, true>           old_tnl_adress;
  bounded_bitstring<1, 160, true, true>           new_tnl_adress;
  ul_up_tnl_address_to_upd_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CNSupport ::= ENUMERATED
struct cn_support_opts {
  enum options { c_epc, c_5gc, both, /*...*/ nulltype } value;
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
using cn_support_e = enumerated<cn_support_opts, true>;

using extended_gnb_cu_cp_name_ext_ies_container = protocol_ext_container_empty_l;

// Extended-GNB-CU-CP-Name ::= SEQUENCE
struct extended_gnb_cu_cp_name_s {
  bool                                      ext                                   = false;
  bool                                      gnb_cu_cp_name_visible_string_present = false;
  bool                                      gnb_cu_cp_name_utf8_string_present    = false;
  bool                                      ie_exts_present                       = false;
  visible_string<1, 150, true, true>        gnb_cu_cp_name_visible_string;
  utf8_string<1, 150, true, true>           gnb_cu_cp_name_utf8_string;
  extended_gnb_cu_cp_name_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using extended_gnb_cu_up_name_ext_ies_container = protocol_ext_container_empty_l;

// Extended-GNB-CU-UP-Name ::= SEQUENCE
struct extended_gnb_cu_up_name_s {
  bool                                      ext                                   = false;
  bool                                      gnb_cu_up_name_visible_string_present = false;
  bool                                      gnb_cu_up_name_utf8_string_present    = false;
  bool                                      ie_exts_present                       = false;
  visible_string<1, 150, true, true>        gnb_cu_up_name_visible_string;
  utf8_string<1, 150, true, true>           gnb_cu_up_name_utf8_string;
  extended_gnb_cu_up_name_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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

// GNB-CU-UP-OverloadInformation ::= ENUMERATED
struct gnb_cu_up_overload_info_opts {
  enum options { overloaded, not_overloaded, nulltype } value;

  const char* to_string() const;
};
using gnb_cu_up_overload_info_e = enumerated<gnb_cu_up_overload_info_opts>;

// GNB-CU-UP-TNLA-To-Remove-List ::= SEQUENCE (SIZE (1..32)) OF GNB-CU-UP-TNLA-To-Remove-Item
using gnb_cu_up_tnl_a_to_rem_list_l = dyn_array<gnb_cu_up_tnl_a_to_rem_item_s>;

using hw_capacity_ind_ext_ies_container = protocol_ext_container_empty_l;

// HW-CapacityIndicator ::= SEQUENCE
struct hw_capacity_ind_s {
  bool                              ext                  = false;
  uint32_t                          offered_throughput   = 1;
  uint8_t                           available_throughput = 0;
  hw_capacity_ind_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IAB-Donor-CU-UPPSKInfo ::= SEQUENCE (SIZE (1..256)) OF IAB-Donor-CU-UPPSKInfo-Item
using iab_donor_cu_up_psk_info_l = dyn_array<iab_donor_cu_up_psk_info_item_s>;

// PDU-Session-Resource-Data-Usage-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-Data-Usage-Item
using pdu_session_res_data_usage_list_l = dyn_array<pdu_session_res_data_usage_item_s>;

// PrivateIE-Field{E1AP-PRIVATE-IES : IEsSetParam} ::= SEQUENCE{{E1AP-PRIVATE-IES}}
template <class ies_set_paramT_>
struct private_ie_field_s {
  private_ie_id_c                   id;
  crit_e                            crit;
  typename ies_set_paramT_::value_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RegistrationRequest ::= ENUMERATED
struct regist_request_opts {
  enum options { start, stop, /*...*/ nulltype } value;

  const char* to_string() const;
};
using regist_request_e = enumerated<regist_request_opts, true>;

// ReportingPeriodicity ::= ENUMERATED
struct report_periodicity_opts {
  enum options {
    ms500,
    ms1000,
    ms2000,
    ms5000,
    ms10000,
    ms20000,
    ms30000,
    ms40000,
    ms50000,
    ms60000,
    ms70000,
    ms80000,
    ms90000,
    ms100000,
    ms110000,
    ms120000,
    // ...
    nulltype
  } value;
  typedef uint32_t number_type;

  const char* to_string() const;
  uint32_t    to_number() const;
};
using report_periodicity_e = enumerated<report_periodicity_opts, true>;

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

using tnl_available_capacity_ind_ext_ies_container = protocol_ext_container_empty_l;

// TNL-AvailableCapacityIndicator ::= SEQUENCE
struct tnl_available_capacity_ind_s {
  bool                                         ext                       = false;
  uint32_t                                     dl_tnl_offered_capacity   = 0;
  uint8_t                                      dl_tnl_available_capacity = 0;
  uint32_t                                     ul_tnl_offered_capacity   = 0;
  uint8_t                                      ul_tnl_available_capacity = 0;
  tnl_available_capacity_ind_ext_ies_container ie_exts;
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

using transport_layer_address_info_ext_ies_container = protocol_ext_container_empty_l;

// Transport-Layer-Address-Info ::= SEQUENCE
struct transport_layer_address_info_s {
  bool                                            ext             = false;
  bool                                            ie_exts_present = false;
  transport_up_layer_addresses_info_to_add_list_l transport_up_layer_addresses_info_to_add_list;
  transport_up_layer_addresses_info_to_rem_list_l transport_up_layer_addresses_info_to_rem_list;
  transport_layer_address_info_ext_ies_container  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                    type_;
    choice_buffer_t<cause_c> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                                    type_;
    choice_buffer_t<drbs_subject_to_early_forwarding_list_l> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types type_;
    choice_buffer_t<crit_diagnostics_s,
                    gnb_cu_cp_tnl_a_failed_to_setup_list_l,
                    gnb_cu_cp_tnl_a_setup_list_l,
                    transport_layer_address_info_s>
        c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types type_;
    choice_buffer_t<extended_gnb_cu_cp_name_s,
                    gnb_cu_cp_tnl_a_to_add_list_l,
                    gnb_cu_cp_tnl_a_to_rem_list_l,
                    gnb_cu_cp_tnl_a_to_upd_list_l,
                    printable_string<1, 150, true, true>,
                    transport_layer_address_info_s>
        c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types type_;
    choice_buffer_t<extended_gnb_cu_cp_name_s, printable_string<1, 150, true, true>, transport_layer_address_info_s> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types type_;
    choice_buffer_t<extended_gnb_cu_up_name_s,
                    printable_string<1, 150, true, true>,
                    supported_plmns_list_l,
                    transport_layer_address_info_s>
        c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                         type_;
    choice_buffer_t<drb_meas_results_info_list_l> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                                               type_;
    choice_buffer_t<crit_diagnostics_s, transport_layer_address_info_s> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types type_;
    choice_buffer_t<extended_gnb_cu_up_name_s,
                    gnb_cu_up_tnl_a_to_rem_list_l,
                    printable_string<1, 150, true, true>,
                    supported_plmns_list_l,
                    transport_layer_address_info_s>
        c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                                  type_;
    choice_buffer_t<sys_gnb_cu_up_counter_check_request_c> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types type_;
    choice_buffer_t<extended_gnb_cu_up_name_s,
                    printable_string<1, 150, true, true>,
                    supported_plmns_list_l,
                    transport_layer_address_info_s>
        c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types type_;
    choice_buffer_t<extended_gnb_cu_cp_name_s, printable_string<1, 150, true, true>, transport_layer_address_info_s> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                                                type_;
    choice_buffer_t<crit_diagnostics_s, ul_up_tnl_address_to_upd_list_l> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                            type_;
    choice_buffer_t<dl_up_tnl_address_to_upd_list_l> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                       type_;
    choice_buffer_t<iab_donor_cu_up_psk_info_l> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                              type_;
    choice_buffer_t<pdu_session_res_data_usage_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PrivateIE-Container{E1AP-PRIVATE-IES : IEsSetParam} ::= SEQUENCE (SIZE (1..65535)) OF PrivateIE-Field
template <class ies_set_paramT_>
using private_ie_container_l = dyn_seq_of<private_ie_field_s<ies_set_paramT_>, 1, 65535, true>;

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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                                                        type_;
    choice_buffer_t<crit_diagnostics_s, ue_associated_lc_e1_conn_list_res_ack_l> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                  type_;
    choice_buffer_t<cause_c, reset_type_c> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                             type_;
    choice_buffer_t<fixed_bitstring<36, false, true>> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                               type_;
    choice_buffer_t<crit_diagnostics_s> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                                            type_;
    choice_buffer_t<hw_capacity_ind_s, tnl_available_capacity_ind_s> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                               type_;
    choice_buffer_t<trace_activation_s> c;

    void destroy_();
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
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
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
    types                                         type_;
    choice_buffer_t<pdu_session_to_notify_list_l> c;

    void destroy_();
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

template <class valueT_>
struct private_ie_container_item_s {
  private_ie_id_c id;
  crit_e          crit;
  valueT_         value;

  // sequence methods
  private_ie_container_item_s(private_ie_id_c id_, crit_e crit_);
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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

// E1AP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF E1AP-ELEMENTARY-PROCEDURE
struct e1ap_elem_procs_o {
  // InitiatingMessage ::= OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options {
        reset,
        gnb_cu_up_e1_setup_request,
        gnb_cu_cp_e1_setup_request,
        gnb_cu_up_cfg_upd,
        gnb_cu_cp_cfg_upd,
        e1_release_request,
        bearer_context_setup_request,
        bearer_context_mod_request,
        bearer_context_mod_required,
        bearer_context_release_cmd,
        res_status_request,
        iab_up_tnl_address_upd,
        error_ind,
        bearer_context_release_request,
        bearer_context_inactivity_notif,
        dl_data_notif,
        ul_data_notif,
        data_usage_report,
        gnb_cu_up_counter_check_request,
        gnb_cu_up_status_ind,
        mrdc_data_usage_report,
        deactiv_trace,
        trace_start,
        private_msg,
        cell_traffic_trace,
        res_status_upd,
        early_forwarding_sn_transfer,
        gnb_cu_cp_meas_results_info,
        iabpsk_notif,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    init_msg_c() = default;
    init_msg_c(const init_msg_c& other);
    init_msg_c& operator=(const init_msg_c& other);
    ~init_msg_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    reset_s&                                 reset();
    gnb_cu_up_e1_setup_request_s&            gnb_cu_up_e1_setup_request();
    gnb_cu_cp_e1_setup_request_s&            gnb_cu_cp_e1_setup_request();
    gnb_cu_up_cfg_upd_s&                     gnb_cu_up_cfg_upd();
    gnb_cu_cp_cfg_upd_s&                     gnb_cu_cp_cfg_upd();
    e1_release_request_s&                    e1_release_request();
    bearer_context_setup_request_s&          bearer_context_setup_request();
    bearer_context_mod_request_s&            bearer_context_mod_request();
    bearer_context_mod_required_s&           bearer_context_mod_required();
    bearer_context_release_cmd_s&            bearer_context_release_cmd();
    res_status_request_s&                    res_status_request();
    iab_up_tnl_address_upd_s&                iab_up_tnl_address_upd();
    error_ind_s&                             error_ind();
    bearer_context_release_request_s&        bearer_context_release_request();
    bearer_context_inactivity_notif_s&       bearer_context_inactivity_notif();
    dl_data_notif_s&                         dl_data_notif();
    ul_data_notif_s&                         ul_data_notif();
    data_usage_report_s&                     data_usage_report();
    gnb_cu_up_counter_check_request_s&       gnb_cu_up_counter_check_request();
    gnb_cu_up_status_ind_s&                  gnb_cu_up_status_ind();
    mrdc_data_usage_report_s&                mrdc_data_usage_report();
    deactiv_trace_s&                         deactiv_trace();
    trace_start_s&                           trace_start();
    private_msg_s&                           private_msg();
    cell_traffic_trace_s&                    cell_traffic_trace();
    res_status_upd_s&                        res_status_upd();
    early_forwarding_sn_transfer_s&          early_forwarding_sn_transfer();
    gnb_cu_cp_meas_results_info_s&           gnb_cu_cp_meas_results_info();
    iabpsk_notif_s&                          iabpsk_notif();
    const reset_s&                           reset() const;
    const gnb_cu_up_e1_setup_request_s&      gnb_cu_up_e1_setup_request() const;
    const gnb_cu_cp_e1_setup_request_s&      gnb_cu_cp_e1_setup_request() const;
    const gnb_cu_up_cfg_upd_s&               gnb_cu_up_cfg_upd() const;
    const gnb_cu_cp_cfg_upd_s&               gnb_cu_cp_cfg_upd() const;
    const e1_release_request_s&              e1_release_request() const;
    const bearer_context_setup_request_s&    bearer_context_setup_request() const;
    const bearer_context_mod_request_s&      bearer_context_mod_request() const;
    const bearer_context_mod_required_s&     bearer_context_mod_required() const;
    const bearer_context_release_cmd_s&      bearer_context_release_cmd() const;
    const res_status_request_s&              res_status_request() const;
    const iab_up_tnl_address_upd_s&          iab_up_tnl_address_upd() const;
    const error_ind_s&                       error_ind() const;
    const bearer_context_release_request_s&  bearer_context_release_request() const;
    const bearer_context_inactivity_notif_s& bearer_context_inactivity_notif() const;
    const dl_data_notif_s&                   dl_data_notif() const;
    const ul_data_notif_s&                   ul_data_notif() const;
    const data_usage_report_s&               data_usage_report() const;
    const gnb_cu_up_counter_check_request_s& gnb_cu_up_counter_check_request() const;
    const gnb_cu_up_status_ind_s&            gnb_cu_up_status_ind() const;
    const mrdc_data_usage_report_s&          mrdc_data_usage_report() const;
    const deactiv_trace_s&                   deactiv_trace() const;
    const trace_start_s&                     trace_start() const;
    const private_msg_s&                     private_msg() const;
    const cell_traffic_trace_s&              cell_traffic_trace() const;
    const res_status_upd_s&                  res_status_upd() const;
    const early_forwarding_sn_transfer_s&    early_forwarding_sn_transfer() const;
    const gnb_cu_cp_meas_results_info_s&     gnb_cu_cp_meas_results_info() const;
    const iabpsk_notif_s&                    iabpsk_notif() const;

  private:
    types type_;
    choice_buffer_t<bearer_context_inactivity_notif_s,
                    bearer_context_mod_request_s,
                    bearer_context_mod_required_s,
                    bearer_context_release_cmd_s,
                    bearer_context_release_request_s,
                    bearer_context_setup_request_s,
                    cell_traffic_trace_s,
                    data_usage_report_s,
                    deactiv_trace_s,
                    dl_data_notif_s,
                    e1_release_request_s,
                    early_forwarding_sn_transfer_s,
                    error_ind_s,
                    gnb_cu_cp_cfg_upd_s,
                    gnb_cu_cp_e1_setup_request_s,
                    gnb_cu_cp_meas_results_info_s,
                    gnb_cu_up_cfg_upd_s,
                    gnb_cu_up_counter_check_request_s,
                    gnb_cu_up_e1_setup_request_s,
                    gnb_cu_up_status_ind_s,
                    iab_up_tnl_address_upd_s,
                    iabpsk_notif_s,
                    mrdc_data_usage_report_s,
                    private_msg_s,
                    res_status_request_s,
                    res_status_upd_s,
                    reset_s,
                    trace_start_s,
                    ul_data_notif_s>
        c;

    void destroy_();
  };
  // SuccessfulOutcome ::= OPEN TYPE
  struct successful_outcome_c {
    struct types_opts {
      enum options {
        reset_ack,
        gnb_cu_up_e1_setup_resp,
        gnb_cu_cp_e1_setup_resp,
        gnb_cu_up_cfg_upd_ack,
        gnb_cu_cp_cfg_upd_ack,
        e1_release_resp,
        bearer_context_setup_resp,
        bearer_context_mod_resp,
        bearer_context_mod_confirm,
        bearer_context_release_complete,
        res_status_resp,
        iab_up_tnl_address_upd_ack,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    successful_outcome_c() = default;
    successful_outcome_c(const successful_outcome_c& other);
    successful_outcome_c& operator=(const successful_outcome_c& other);
    ~successful_outcome_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    reset_ack_s&                             reset_ack();
    gnb_cu_up_e1_setup_resp_s&               gnb_cu_up_e1_setup_resp();
    gnb_cu_cp_e1_setup_resp_s&               gnb_cu_cp_e1_setup_resp();
    gnb_cu_up_cfg_upd_ack_s&                 gnb_cu_up_cfg_upd_ack();
    gnb_cu_cp_cfg_upd_ack_s&                 gnb_cu_cp_cfg_upd_ack();
    e1_release_resp_s&                       e1_release_resp();
    bearer_context_setup_resp_s&             bearer_context_setup_resp();
    bearer_context_mod_resp_s&               bearer_context_mod_resp();
    bearer_context_mod_confirm_s&            bearer_context_mod_confirm();
    bearer_context_release_complete_s&       bearer_context_release_complete();
    res_status_resp_s&                       res_status_resp();
    iab_up_tnl_address_upd_ack_s&            iab_up_tnl_address_upd_ack();
    const reset_ack_s&                       reset_ack() const;
    const gnb_cu_up_e1_setup_resp_s&         gnb_cu_up_e1_setup_resp() const;
    const gnb_cu_cp_e1_setup_resp_s&         gnb_cu_cp_e1_setup_resp() const;
    const gnb_cu_up_cfg_upd_ack_s&           gnb_cu_up_cfg_upd_ack() const;
    const gnb_cu_cp_cfg_upd_ack_s&           gnb_cu_cp_cfg_upd_ack() const;
    const e1_release_resp_s&                 e1_release_resp() const;
    const bearer_context_setup_resp_s&       bearer_context_setup_resp() const;
    const bearer_context_mod_resp_s&         bearer_context_mod_resp() const;
    const bearer_context_mod_confirm_s&      bearer_context_mod_confirm() const;
    const bearer_context_release_complete_s& bearer_context_release_complete() const;
    const res_status_resp_s&                 res_status_resp() const;
    const iab_up_tnl_address_upd_ack_s&      iab_up_tnl_address_upd_ack() const;

  private:
    types type_;
    choice_buffer_t<bearer_context_mod_confirm_s,
                    bearer_context_mod_resp_s,
                    bearer_context_release_complete_s,
                    bearer_context_setup_resp_s,
                    e1_release_resp_s,
                    gnb_cu_cp_cfg_upd_ack_s,
                    gnb_cu_cp_e1_setup_resp_s,
                    gnb_cu_up_cfg_upd_ack_s,
                    gnb_cu_up_e1_setup_resp_s,
                    iab_up_tnl_address_upd_ack_s,
                    res_status_resp_s,
                    reset_ack_s>
        c;

    void destroy_();
  };
  // UnsuccessfulOutcome ::= OPEN TYPE
  struct unsuccessful_outcome_c {
    struct types_opts {
      enum options {
        gnb_cu_up_e1_setup_fail,
        gnb_cu_cp_e1_setup_fail,
        gnb_cu_up_cfg_upd_fail,
        gnb_cu_cp_cfg_upd_fail,
        bearer_context_setup_fail,
        bearer_context_mod_fail,
        res_status_fail,
        iab_up_tnl_address_upd_fail,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    unsuccessful_outcome_c() = default;
    unsuccessful_outcome_c(const unsuccessful_outcome_c& other);
    unsuccessful_outcome_c& operator=(const unsuccessful_outcome_c& other);
    ~unsuccessful_outcome_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_cu_up_e1_setup_fail_s&           gnb_cu_up_e1_setup_fail();
    gnb_cu_cp_e1_setup_fail_s&           gnb_cu_cp_e1_setup_fail();
    gnb_cu_up_cfg_upd_fail_s&            gnb_cu_up_cfg_upd_fail();
    gnb_cu_cp_cfg_upd_fail_s&            gnb_cu_cp_cfg_upd_fail();
    bearer_context_setup_fail_s&         bearer_context_setup_fail();
    bearer_context_mod_fail_s&           bearer_context_mod_fail();
    res_status_fail_s&                   res_status_fail();
    iab_up_tnl_address_upd_fail_s&       iab_up_tnl_address_upd_fail();
    const gnb_cu_up_e1_setup_fail_s&     gnb_cu_up_e1_setup_fail() const;
    const gnb_cu_cp_e1_setup_fail_s&     gnb_cu_cp_e1_setup_fail() const;
    const gnb_cu_up_cfg_upd_fail_s&      gnb_cu_up_cfg_upd_fail() const;
    const gnb_cu_cp_cfg_upd_fail_s&      gnb_cu_cp_cfg_upd_fail() const;
    const bearer_context_setup_fail_s&   bearer_context_setup_fail() const;
    const bearer_context_mod_fail_s&     bearer_context_mod_fail() const;
    const res_status_fail_s&             res_status_fail() const;
    const iab_up_tnl_address_upd_fail_s& iab_up_tnl_address_upd_fail() const;

  private:
    types type_;
    choice_buffer_t<bearer_context_mod_fail_s,
                    bearer_context_setup_fail_s,
                    gnb_cu_cp_cfg_upd_fail_s,
                    gnb_cu_cp_e1_setup_fail_s,
                    gnb_cu_up_cfg_upd_fail_s,
                    gnb_cu_up_e1_setup_fail_s,
                    iab_up_tnl_address_upd_fail_s,
                    res_status_fail_s>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint16_t               idx_to_proc_code(uint32_t idx);
  static bool                   is_proc_code_valid(const uint16_t& proc_code);
  static init_msg_c             get_init_msg(const uint16_t& proc_code);
  static successful_outcome_c   get_successful_outcome(const uint16_t& proc_code);
  static unsuccessful_outcome_c get_unsuccessful_outcome(const uint16_t& proc_code);
  static crit_e                 get_crit(const uint16_t& proc_code);
};

// InitiatingMessage ::= SEQUENCE{{E1AP-ELEMENTARY-PROCEDURE}}
struct init_msg_s {
  uint16_t                      proc_code = 0;
  crit_e                        crit;
  e1ap_elem_procs_o::init_msg_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// SuccessfulOutcome ::= SEQUENCE{{E1AP-ELEMENTARY-PROCEDURE}}
struct successful_outcome_s {
  uint16_t                                proc_code = 0;
  crit_e                                  crit;
  e1ap_elem_procs_o::successful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// UnsuccessfulOutcome ::= SEQUENCE{{E1AP-ELEMENTARY-PROCEDURE}}
struct unsuccessful_outcome_s {
  uint16_t                                  proc_code = 0;
  crit_e                                    crit;
  e1ap_elem_procs_o::unsuccessful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// E1AP-PDU ::= CHOICE
struct e1ap_pdu_c {
  struct types_opts {
    enum options { init_msg, successful_outcome, unsuccessful_outcome, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  e1ap_pdu_c() = default;
  e1ap_pdu_c(const e1ap_pdu_c& other);
  e1ap_pdu_c& operator=(const e1ap_pdu_c& other);
  ~e1ap_pdu_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  init_msg_s& init_msg()
  {
    assert_choice_type(types::init_msg, type_, "E1AP-PDU");
    return c.get<init_msg_s>();
  }
  successful_outcome_s& successful_outcome()
  {
    assert_choice_type(types::successful_outcome, type_, "E1AP-PDU");
    return c.get<successful_outcome_s>();
  }
  unsuccessful_outcome_s& unsuccessful_outcome()
  {
    assert_choice_type(types::unsuccessful_outcome, type_, "E1AP-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  const init_msg_s& init_msg() const
  {
    assert_choice_type(types::init_msg, type_, "E1AP-PDU");
    return c.get<init_msg_s>();
  }
  const successful_outcome_s& successful_outcome() const
  {
    assert_choice_type(types::successful_outcome, type_, "E1AP-PDU");
    return c.get<successful_outcome_s>();
  }
  const unsuccessful_outcome_s& unsuccessful_outcome() const
  {
    assert_choice_type(types::unsuccessful_outcome, type_, "E1AP-PDU");
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

} // namespace e1ap
} // namespace asn1

extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_inactivity_notif_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_mod_confirm_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_mod_fail_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::dyn_5qi_descriptor_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::non_dyn_5qi_descriptor_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::qos_flow_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::qos_flow_level_qos_params_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::qos_flow_qos_param_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::pdcp_cfg_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::drb_to_modify_item_ng_ran_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::drb_to_setup_item_ng_ran_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::drb_to_setup_mod_item_ng_ran_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::pdu_session_res_to_modify_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::pdu_session_res_to_setup_mod_item_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::eutran_bearer_context_mod_request_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::ng_ran_bearer_context_mod_request_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_mod_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::eutran_bearer_context_mod_required_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::ng_ran_bearer_context_mod_required_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_mod_required_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::drb_modified_item_ng_ran_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::eutran_bearer_context_mod_resp_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::ng_ran_bearer_context_mod_resp_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_mod_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_release_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_release_complete_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_release_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_setup_fail_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::pdu_session_res_to_setup_item_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::eutran_bearer_context_setup_request_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::trace_activation_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_setup_request_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::pdu_session_res_setup_item_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::eutran_bearer_context_setup_resp_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::ng_ran_bearer_context_setup_resp_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e1ap::bearer_context_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e1ap::cp_tnl_info_ext_ies_o>;
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
