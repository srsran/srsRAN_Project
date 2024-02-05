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

#include "srsran/asn1/asn1_ap_utils.h"

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
struct reset_ies_container;
using reset_s = elementary_procedure_option<reset_ies_container>;
struct gnb_cu_up_e1_setup_request_ies_container;
using gnb_cu_up_e1_setup_request_s = elementary_procedure_option<gnb_cu_up_e1_setup_request_ies_container>;
struct gnb_cu_cp_e1_setup_request_ies_container;
using gnb_cu_cp_e1_setup_request_s = elementary_procedure_option<gnb_cu_cp_e1_setup_request_ies_container>;
struct gnb_cu_up_cfg_upd_ies_container;
using gnb_cu_up_cfg_upd_s = elementary_procedure_option<gnb_cu_up_cfg_upd_ies_container>;
struct gnb_cu_cp_cfg_upd_ies_container;
using gnb_cu_cp_cfg_upd_s = elementary_procedure_option<gnb_cu_cp_cfg_upd_ies_container>;
struct e1_release_request_ies_container;
using e1_release_request_s = elementary_procedure_option<e1_release_request_ies_container>;
struct bearer_context_setup_request_ies_container;
using bearer_context_setup_request_s = elementary_procedure_option<bearer_context_setup_request_ies_container>;
struct bearer_context_mod_request_ies_container;
using bearer_context_mod_request_s = elementary_procedure_option<bearer_context_mod_request_ies_container>;
struct bearer_context_mod_required_ies_container;
using bearer_context_mod_required_s = elementary_procedure_option<bearer_context_mod_required_ies_container>;
struct bearer_context_release_cmd_ies_container;
using bearer_context_release_cmd_s = elementary_procedure_option<bearer_context_release_cmd_ies_container>;
struct res_status_request_ies_container;
using res_status_request_s = elementary_procedure_option<res_status_request_ies_container>;
struct iab_up_tnl_address_upd_ies_container;
using iab_up_tnl_address_upd_s = elementary_procedure_option<iab_up_tnl_address_upd_ies_container>;
struct error_ind_ies_container;
using error_ind_s = elementary_procedure_option<error_ind_ies_container>;
struct bearer_context_release_request_ies_container;
using bearer_context_release_request_s = elementary_procedure_option<bearer_context_release_request_ies_container>;
struct bearer_context_inactivity_notif_ies_container;
using bearer_context_inactivity_notif_s = elementary_procedure_option<bearer_context_inactivity_notif_ies_container>;
struct dl_data_notif_ies_container;
using dl_data_notif_s = elementary_procedure_option<dl_data_notif_ies_container>;
struct ul_data_notif_ies_container;
using ul_data_notif_s = elementary_procedure_option<ul_data_notif_ies_container>;
struct data_usage_report_ies_container;
using data_usage_report_s = elementary_procedure_option<data_usage_report_ies_container>;
struct gnb_cu_up_counter_check_request_ies_container;
using gnb_cu_up_counter_check_request_s = elementary_procedure_option<gnb_cu_up_counter_check_request_ies_container>;
struct gnb_cu_up_status_ind_ies_container;
using gnb_cu_up_status_ind_s = elementary_procedure_option<gnb_cu_up_status_ind_ies_container>;
struct mrdc_data_usage_report_ies_container;
using mrdc_data_usage_report_s = elementary_procedure_option<mrdc_data_usage_report_ies_container>;
struct deactiv_trace_ies_container;
using deactiv_trace_s = elementary_procedure_option<deactiv_trace_ies_container>;
struct trace_start_ies_container;
using trace_start_s = elementary_procedure_option<trace_start_ies_container>;
struct private_msg_s;
struct cell_traffic_trace_ies_container;
using cell_traffic_trace_s = elementary_procedure_option<cell_traffic_trace_ies_container>;
struct res_status_upd_ies_container;
using res_status_upd_s = elementary_procedure_option<res_status_upd_ies_container>;
struct early_forwarding_sn_transfer_ies_container;
using early_forwarding_sn_transfer_s = elementary_procedure_option<early_forwarding_sn_transfer_ies_container>;
struct gnb_cu_cp_meas_results_info_ies_container;
using gnb_cu_cp_meas_results_info_s = elementary_procedure_option<gnb_cu_cp_meas_results_info_ies_container>;
struct iabpsk_notif_ies_container;
using iabpsk_notif_s = elementary_procedure_option<iabpsk_notif_ies_container>;
struct reset_ack_ies_container;
using reset_ack_s = elementary_procedure_option<reset_ack_ies_container>;
struct gnb_cu_up_e1_setup_resp_ies_container;
using gnb_cu_up_e1_setup_resp_s = elementary_procedure_option<gnb_cu_up_e1_setup_resp_ies_container>;
struct gnb_cu_cp_e1_setup_resp_ies_container;
using gnb_cu_cp_e1_setup_resp_s = elementary_procedure_option<gnb_cu_cp_e1_setup_resp_ies_container>;
struct gnb_cu_up_cfg_upd_ack_ies_container;
using gnb_cu_up_cfg_upd_ack_s = elementary_procedure_option<gnb_cu_up_cfg_upd_ack_ies_container>;
struct gnb_cu_cp_cfg_upd_ack_ies_container;
using gnb_cu_cp_cfg_upd_ack_s = elementary_procedure_option<gnb_cu_cp_cfg_upd_ack_ies_container>;
struct e1_release_resp_ies_o;
using e1_release_resp_s = elementary_procedure_option<protocol_ie_container_l<e1_release_resp_ies_o>>;
struct bearer_context_setup_resp_ies_container;
using bearer_context_setup_resp_s = elementary_procedure_option<bearer_context_setup_resp_ies_container>;
struct bearer_context_mod_resp_ies_container;
using bearer_context_mod_resp_s = elementary_procedure_option<bearer_context_mod_resp_ies_container>;
struct bearer_context_mod_confirm_ies_container;
using bearer_context_mod_confirm_s = elementary_procedure_option<bearer_context_mod_confirm_ies_container>;
struct bearer_context_release_complete_ies_container;
using bearer_context_release_complete_s = elementary_procedure_option<bearer_context_release_complete_ies_container>;
struct res_status_resp_ies_container;
using res_status_resp_s = elementary_procedure_option<res_status_resp_ies_container>;
struct iab_up_tnl_address_upd_ack_ies_container;
using iab_up_tnl_address_upd_ack_s = elementary_procedure_option<iab_up_tnl_address_upd_ack_ies_container>;
struct gnb_cu_up_e1_setup_fail_ies_container;
using gnb_cu_up_e1_setup_fail_s = elementary_procedure_option<gnb_cu_up_e1_setup_fail_ies_container>;
struct gnb_cu_cp_e1_setup_fail_ies_container;
using gnb_cu_cp_e1_setup_fail_s = elementary_procedure_option<gnb_cu_cp_e1_setup_fail_ies_container>;
struct gnb_cu_up_cfg_upd_fail_ies_container;
using gnb_cu_up_cfg_upd_fail_s = elementary_procedure_option<gnb_cu_up_cfg_upd_fail_ies_container>;
struct gnb_cu_cp_cfg_upd_fail_ies_container;
using gnb_cu_cp_cfg_upd_fail_s = elementary_procedure_option<gnb_cu_cp_cfg_upd_fail_ies_container>;
struct bearer_context_setup_fail_ies_container;
using bearer_context_setup_fail_s = elementary_procedure_option<bearer_context_setup_fail_ies_container>;
struct bearer_context_mod_fail_ies_container;
using bearer_context_mod_fail_s = elementary_procedure_option<bearer_context_mod_fail_ies_container>;
struct res_status_fail_ies_container;
using res_status_fail_s = elementary_procedure_option<res_status_fail_ies_container>;
struct iab_up_tnl_address_upd_fail_ies_container;
using iab_up_tnl_address_upd_fail_s = elementary_procedure_option<iab_up_tnl_address_upd_fail_ies_container>;

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

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
    types             type_;
    choice_buffer_ptr c;
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
    types             type_;
    choice_buffer_ptr c;
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
