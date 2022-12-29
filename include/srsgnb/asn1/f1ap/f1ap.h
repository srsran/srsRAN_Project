/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/*******************************************************************************
 *
 *                     3GPP TS ASN1 F1AP v15.3.0 (2019-03)
 *
 ******************************************************************************/

#pragma once

#include "srsgnb/asn1/asn1_utils.h"
#include <cstdio>
#include <stdarg.h>

namespace asn1 {
namespace f1ap {

/*******************************************************************************
 *                             Constant Definitions
 ******************************************************************************/

#define ASN1_F1AP_ID_RESET 0
#define ASN1_F1AP_ID_F1_SETUP 1
#define ASN1_F1AP_ID_ERROR_IND 2
#define ASN1_F1AP_ID_GNB_DU_CFG_UPD 3
#define ASN1_F1AP_ID_GNB_CU_CFG_UPD 4
#define ASN1_F1AP_ID_UE_CONTEXT_SETUP 5
#define ASN1_F1AP_ID_UE_CONTEXT_RELEASE 6
#define ASN1_F1AP_ID_UE_CONTEXT_MOD 7
#define ASN1_F1AP_ID_UE_CONTEXT_MOD_REQUIRED 8
#define ASN1_F1AP_ID_UE_MOB_CMD 9
#define ASN1_F1AP_ID_UE_CONTEXT_RELEASE_REQUEST 10
#define ASN1_F1AP_ID_INIT_UL_RRC_MSG_TRANSFER 11
#define ASN1_F1AP_ID_DL_RRC_MSG_TRANSFER 12
#define ASN1_F1AP_ID_UL_RRC_MSG_TRANSFER 13
#define ASN1_F1AP_ID_PRIVATE_MSG 14
#define ASN1_F1AP_ID_UE_INACTIVITY_NOTIF 15
#define ASN1_F1AP_ID_GNB_DU_RES_COORDINATION 16
#define ASN1_F1AP_ID_SYS_INFO_DELIVERY_CMD 17
#define ASN1_F1AP_ID_PAGING 18
#define ASN1_F1AP_ID_NOTIFY 19
#define ASN1_F1AP_ID_WRITE_REPLACE_WARNING 20
#define ASN1_F1AP_ID_PWS_CANCEL 21
#define ASN1_F1AP_ID_PWS_RESTART_IND 22
#define ASN1_F1AP_ID_PWS_FAIL_IND 23
#define ASN1_F1AP_ID_GNB_DU_STATUS_IND 24
#define ASN1_F1AP_ID_RRC_DELIVERY_REPORT 25
#define ASN1_F1AP_ID_F1_REMOVAL 26
#define ASN1_F1AP_ID_NETWORK_ACCESS_RATE_REDUCTION 27
#define ASN1_F1AP_MAX_PRIVATE_IES 65535
#define ASN1_F1AP_MAX_PROTOCOL_EXTS 65535
#define ASN1_F1AP_MAX_PROTOCOL_IES 65535
#define ASN1_F1AP_MAX_NR_ARFCN 3279165
#define ASN1_F1AP_MAXNOOF_ERRORS 256
#define ASN1_F1AP_MAXNOOF_INDIVIDUAL_F1_CONNS_TO_RESET 65536
#define ASN1_F1AP_MAX_CELLIN_GNB_DU 512
#define ASN1_F1AP_MAXNOOF_SCELLS 32
#define ASN1_F1AP_MAXNOOF_SRBS 8
#define ASN1_F1AP_MAXNOOF_DRBS 64
#define ASN1_F1AP_MAXNOOF_UL_UP_TNL_INFO 2
#define ASN1_F1AP_MAXNOOF_DL_UP_TNL_INFO 2
#define ASN1_F1AP_MAXNOOF_BPLMNS 6
#define ASN1_F1AP_MAXNOOF_CANDIDATE_SP_CELLS 64
#define ASN1_F1AP_MAXNOOF_POTENTIAL_SP_CELLS 64
#define ASN1_F1AP_MAXNOOF_NR_CELL_BANDS 32
#define ASN1_F1AP_MAXNOOF_SIB_TYPES 32
#define ASN1_F1AP_MAXNOOF_SI_TYPES 32
#define ASN1_F1AP_MAXNOOF_PAGING_CELLS 512
#define ASN1_F1AP_MAXNOOF_TNL_ASSOCS 32
#define ASN1_F1AP_MAXNOOF_QOS_FLOWS 64
#define ASN1_F1AP_MAXNOOF_SLICE_ITEMS 1024
#define ASN1_F1AP_MAX_CELLIN_ENB 256
#define ASN1_F1AP_MAXNOOF_EXTENDED_BPLMNS 6
#define ASN1_F1AP_MAXNOOF_UE_IDS 65536
#define ASN1_F1AP_MAXNOOF_BPLMNS_NR_1 11
#define ASN1_F1AP_MAXNOOF_UAC_PLMNS 12
#define ASN1_F1AP_MAXNOOF_UAC_PER_PLMN 64
#define ASN1_F1AP_MAXNOOF_ADD_SIBS 63
#define ASN1_F1AP_ID_CAUSE 0
#define ASN1_F1AP_ID_CELLS_FAILED_TO_BE_ACTIV_LIST 1
#define ASN1_F1AP_ID_CELLS_FAILED_TO_BE_ACTIV_LIST_ITEM 2
#define ASN1_F1AP_ID_CELLS_TO_BE_ACTIV_LIST 3
#define ASN1_F1AP_ID_CELLS_TO_BE_ACTIV_LIST_ITEM 4
#define ASN1_F1AP_ID_CELLS_TO_BE_DEACTIV_LIST 5
#define ASN1_F1AP_ID_CELLS_TO_BE_DEACTIV_LIST_ITEM 6
#define ASN1_F1AP_ID_CRIT_DIAGNOSTICS 7
#define ASN1_F1AP_ID_CU_TO_DU_RRC_INFO 9
#define ASN1_F1AP_ID_DRBS_FAILED_TO_BE_MODIFIED_ITEM 12
#define ASN1_F1AP_ID_DRBS_FAILED_TO_BE_MODIFIED_LIST 13
#define ASN1_F1AP_ID_DRBS_FAILED_TO_BE_SETUP_ITEM 14
#define ASN1_F1AP_ID_DRBS_FAILED_TO_BE_SETUP_LIST 15
#define ASN1_F1AP_ID_DRBS_FAILED_TO_BE_SETUP_MOD_ITEM 16
#define ASN1_F1AP_ID_DRBS_FAILED_TO_BE_SETUP_MOD_LIST 17
#define ASN1_F1AP_ID_DRBS_MODIFIED_CONF_ITEM 18
#define ASN1_F1AP_ID_DRBS_MODIFIED_CONF_LIST 19
#define ASN1_F1AP_ID_DRBS_MODIFIED_ITEM 20
#define ASN1_F1AP_ID_DRBS_MODIFIED_LIST 21
#define ASN1_F1AP_ID_DRBS_REQUIRED_TO_BE_MODIFIED_ITEM 22
#define ASN1_F1AP_ID_DRBS_REQUIRED_TO_BE_MODIFIED_LIST 23
#define ASN1_F1AP_ID_DRBS_REQUIRED_TO_BE_RELEASED_ITEM 24
#define ASN1_F1AP_ID_DRBS_REQUIRED_TO_BE_RELEASED_LIST 25
#define ASN1_F1AP_ID_DRBS_SETUP_ITEM 26
#define ASN1_F1AP_ID_DRBS_SETUP_LIST 27
#define ASN1_F1AP_ID_DRBS_SETUP_MOD_ITEM 28
#define ASN1_F1AP_ID_DRBS_SETUP_MOD_LIST 29
#define ASN1_F1AP_ID_DRBS_TO_BE_MODIFIED_ITEM 30
#define ASN1_F1AP_ID_DRBS_TO_BE_MODIFIED_LIST 31
#define ASN1_F1AP_ID_DRBS_TO_BE_RELEASED_ITEM 32
#define ASN1_F1AP_ID_DRBS_TO_BE_RELEASED_LIST 33
#define ASN1_F1AP_ID_DRBS_TO_BE_SETUP_ITEM 34
#define ASN1_F1AP_ID_DRBS_TO_BE_SETUP_LIST 35
#define ASN1_F1AP_ID_DRBS_TO_BE_SETUP_MOD_ITEM 36
#define ASN1_F1AP_ID_DRBS_TO_BE_SETUP_MOD_LIST 37
#define ASN1_F1AP_ID_DRX_CYCLE 38
#define ASN1_F1AP_ID_DU_TO_CU_RRC_INFO 39
#define ASN1_F1AP_ID_GNB_CU_UE_F1AP_ID 40
#define ASN1_F1AP_ID_GNB_DU_UE_F1AP_ID 41
#define ASN1_F1AP_ID_GNB_DU_ID 42
#define ASN1_F1AP_ID_GNB_DU_SERVED_CELLS_ITEM 43
#define ASN1_F1AP_ID_GNB_DU_SERVED_CELLS_LIST 44
#define ASN1_F1AP_ID_GNB_DU_NAME 45
#define ASN1_F1AP_ID_NR_CELL_ID 46
#define ASN1_F1AP_ID_OLD_GNB_DU_UE_F1AP_ID 47
#define ASN1_F1AP_ID_RESET_TYPE 48
#define ASN1_F1AP_ID_RES_COORDINATION_TRANSFER_CONTAINER 49
#define ASN1_F1AP_ID_RRC_CONTAINER 50
#define ASN1_F1AP_ID_SCELL_TO_BE_REMD_ITEM 51
#define ASN1_F1AP_ID_SCELL_TO_BE_REMD_LIST 52
#define ASN1_F1AP_ID_SCELL_TO_BE_SETUP_ITEM 53
#define ASN1_F1AP_ID_SCELL_TO_BE_SETUP_LIST 54
#define ASN1_F1AP_ID_SCELL_TO_BE_SETUP_MOD_ITEM 55
#define ASN1_F1AP_ID_SCELL_TO_BE_SETUP_MOD_LIST 56
#define ASN1_F1AP_ID_SERVED_CELLS_TO_ADD_ITEM 57
#define ASN1_F1AP_ID_SERVED_CELLS_TO_ADD_LIST 58
#define ASN1_F1AP_ID_SERVED_CELLS_TO_DELETE_ITEM 59
#define ASN1_F1AP_ID_SERVED_CELLS_TO_DELETE_LIST 60
#define ASN1_F1AP_ID_SERVED_CELLS_TO_MODIFY_ITEM 61
#define ASN1_F1AP_ID_SERVED_CELLS_TO_MODIFY_LIST 62
#define ASN1_F1AP_ID_SP_CELL_ID 63
#define ASN1_F1AP_ID_SRB_ID 64
#define ASN1_F1AP_ID_SRBS_FAILED_TO_BE_SETUP_ITEM 65
#define ASN1_F1AP_ID_SRBS_FAILED_TO_BE_SETUP_LIST 66
#define ASN1_F1AP_ID_SRBS_FAILED_TO_BE_SETUP_MOD_ITEM 67
#define ASN1_F1AP_ID_SRBS_FAILED_TO_BE_SETUP_MOD_LIST 68
#define ASN1_F1AP_ID_SRBS_REQUIRED_TO_BE_RELEASED_ITEM 69
#define ASN1_F1AP_ID_SRBS_REQUIRED_TO_BE_RELEASED_LIST 70
#define ASN1_F1AP_ID_SRBS_TO_BE_RELEASED_ITEM 71
#define ASN1_F1AP_ID_SRBS_TO_BE_RELEASED_LIST 72
#define ASN1_F1AP_ID_SRBS_TO_BE_SETUP_ITEM 73
#define ASN1_F1AP_ID_SRBS_TO_BE_SETUP_LIST 74
#define ASN1_F1AP_ID_SRBS_TO_BE_SETUP_MOD_ITEM 75
#define ASN1_F1AP_ID_SRBS_TO_BE_SETUP_MOD_LIST 76
#define ASN1_F1AP_ID_TIME_TO_WAIT 77
#define ASN1_F1AP_ID_TRANSACTION_ID 78
#define ASN1_F1AP_ID_TX_ACTION_IND 79
#define ASN1_F1AP_ID_UE_ASSOCIATED_LC_F1_CONN_ITEM 80
#define ASN1_F1AP_ID_UE_ASSOCIATED_LC_F1_CONN_LIST_RES_ACK 81
#define ASN1_F1AP_ID_GNB_CU_NAME 82
#define ASN1_F1AP_ID_SCELL_FAILEDTO_SETUP_LIST 83
#define ASN1_F1AP_ID_SCELL_FAILEDTO_SETUP_ITEM 84
#define ASN1_F1AP_ID_SCELL_FAILEDTO_SETUP_MOD_LIST 85
#define ASN1_F1AP_ID_SCELL_FAILEDTO_SETUP_MOD_ITEM 86
#define ASN1_F1AP_ID_RRC_RECFG_COMPLETE_IND 87
#define ASN1_F1AP_ID_CELLS_STATUS_ITEM 88
#define ASN1_F1AP_ID_CELLS_STATUS_LIST 89
#define ASN1_F1AP_ID_CANDIDATE_SP_CELL_LIST 90
#define ASN1_F1AP_ID_CANDIDATE_SP_CELL_ITEM 91
#define ASN1_F1AP_ID_POTENTIAL_SP_CELL_LIST 92
#define ASN1_F1AP_ID_POTENTIAL_SP_CELL_ITEM 93
#define ASN1_F1AP_ID_FULL_CFG 94
#define ASN1_F1AP_ID_C_RNTI 95
#define ASN1_F1AP_ID_SP_CELL_UL_CFGURED 96
#define ASN1_F1AP_ID_INACTIVITY_MONITORING_REQUEST 97
#define ASN1_F1AP_ID_INACTIVITY_MONITORING_RESP 98
#define ASN1_F1AP_ID_DRB_ACTIVITY_ITEM 99
#define ASN1_F1AP_ID_DRB_ACTIVITY_LIST 100
#define ASN1_F1AP_ID_EUTRA_NR_CELL_RES_COORDINATION_REQ_CONTAINER 101
#define ASN1_F1AP_ID_EUTRA_NR_CELL_RES_COORDINATION_REQ_ACK_CONTAINER 102
#define ASN1_F1AP_ID_PROTECTED_EUTRA_RES_LIST 105
#define ASN1_F1AP_ID_REQUEST_TYPE 106
#define ASN1_F1AP_ID_SERV_CELL_IDX 107
#define ASN1_F1AP_ID_RAT_FREQ_PRIO_INFO 108
#define ASN1_F1AP_ID_EXECUTE_DUPL 109
#define ASN1_F1AP_ID_NR_CGI 111
#define ASN1_F1AP_ID_PAGING_CELL_ITEM 112
#define ASN1_F1AP_ID_PAGING_CELL_LIST 113
#define ASN1_F1AP_ID_PAGING_DRX 114
#define ASN1_F1AP_ID_PAGING_PRIO 115
#define ASN1_F1AP_ID_SITYPE_LIST 116
#define ASN1_F1AP_ID_UE_ID_IDX_VALUE 117
#define ASN1_F1AP_ID_GNB_CU_SYS_INFO 118
#define ASN1_F1AP_ID_HO_PREP_INFO 119
#define ASN1_F1AP_ID_GNB_CU_TNL_ASSOC_TO_ADD_ITEM 120
#define ASN1_F1AP_ID_GNB_CU_TNL_ASSOC_TO_ADD_LIST 121
#define ASN1_F1AP_ID_GNB_CU_TNL_ASSOC_TO_REM_ITEM 122
#define ASN1_F1AP_ID_GNB_CU_TNL_ASSOC_TO_REM_LIST 123
#define ASN1_F1AP_ID_GNB_CU_TNL_ASSOC_TO_UPD_ITEM 124
#define ASN1_F1AP_ID_GNB_CU_TNL_ASSOC_TO_UPD_LIST 125
#define ASN1_F1AP_ID_MASKED_IMEISV 126
#define ASN1_F1AP_ID_PAGING_ID 127
#define ASN1_F1AP_ID_DU_TO_CU_RRC_CONTAINER 128
#define ASN1_F1AP_ID_CELLS_TO_BE_BARRED_LIST 129
#define ASN1_F1AP_ID_CELLS_TO_BE_BARRED_ITEM 130
#define ASN1_F1AP_ID_TAI_SLICE_SUPPORT_LIST 131
#define ASN1_F1AP_ID_GNB_CU_TNL_ASSOC_SETUP_LIST 132
#define ASN1_F1AP_ID_GNB_CU_TNL_ASSOC_SETUP_ITEM 133
#define ASN1_F1AP_ID_GNB_CU_TNL_ASSOC_FAILED_TO_SETUP_LIST 134
#define ASN1_F1AP_ID_GNB_CU_TNL_ASSOC_FAILED_TO_SETUP_ITEM 135
#define ASN1_F1AP_ID_DRB_NOTIFY_ITEM 136
#define ASN1_F1AP_ID_DRB_NOTIFY_LIST 137
#define ASN1_F1AP_ID_NOTFICATION_CTRL 138
#define ASN1_F1AP_ID_RANAC 139
#define ASN1_F1AP_ID_PWS_SYS_INFO 140
#define ASN1_F1AP_ID_REPEAT_PERIOD 141
#define ASN1_F1AP_ID_NUMOF_BROADCAST_REQUEST 142
#define ASN1_F1AP_ID_CELLS_TO_BE_BROADCAST_LIST 144
#define ASN1_F1AP_ID_CELLS_TO_BE_BROADCAST_ITEM 145
#define ASN1_F1AP_ID_CELLS_BROADCAST_COMPLETED_LIST 146
#define ASN1_F1AP_ID_CELLS_BROADCAST_COMPLETED_ITEM 147
#define ASN1_F1AP_ID_BROADCAST_TO_BE_CANCELLED_LIST 148
#define ASN1_F1AP_ID_BROADCAST_TO_BE_CANCELLED_ITEM 149
#define ASN1_F1AP_ID_CELLS_BROADCAST_CANCELLED_LIST 150
#define ASN1_F1AP_ID_CELLS_BROADCAST_CANCELLED_ITEM 151
#define ASN1_F1AP_ID_NR_CGI_LIST_FOR_RESTART_LIST 152
#define ASN1_F1AP_ID_NR_CGI_LIST_FOR_RESTART_ITEM 153
#define ASN1_F1AP_ID_PWS_FAILED_NR_CGI_LIST 154
#define ASN1_F1AP_ID_PWS_FAILED_NR_CGI_ITEM 155
#define ASN1_F1AP_ID_CONFIRMED_UE_ID 156
#define ASN1_F1AP_ID_CANCEL_ALL_WARNING_MSGS_IND 157
#define ASN1_F1AP_ID_GNB_DU_UE_AMBR_UL 158
#define ASN1_F1AP_ID_DRX_CFG_IND 159
#define ASN1_F1AP_ID_RLC_STATUS 160
#define ASN1_F1AP_ID_DL_PDCP_SN_LEN 161
#define ASN1_F1AP_ID_GNB_DU_CFG_QUERY 162
#define ASN1_F1AP_ID_MEAS_TIMING_CFG 163
#define ASN1_F1AP_ID_DRB_INFO 164
#define ASN1_F1AP_ID_SERVING_PLMN 165
#define ASN1_F1AP_ID_PROTECTED_EUTRA_RES_ITEM 168
#define ASN1_F1AP_ID_GNB_CU_RRC_VERSION 170
#define ASN1_F1AP_ID_GNB_DU_RRC_VERSION 171
#define ASN1_F1AP_ID_GNB_DU_OVERLOAD_INFO 172
#define ASN1_F1AP_ID_CELL_GROUP_CFG 173
#define ASN1_F1AP_ID_RLC_FAIL_IND 174
#define ASN1_F1AP_ID_UL_TX_DIRECT_CURRENT_LIST_INFO 175
#define ASN1_F1AP_ID_DC_BASED_DUPL_CFGURED 176
#define ASN1_F1AP_ID_DC_BASED_DUPL_ACTIVATION 177
#define ASN1_F1AP_ID_SUL_ACCESS_IND 178
#define ASN1_F1AP_ID_AVAILABLE_PLMN_LIST 179
#define ASN1_F1AP_ID_PDU_SESSION_ID 180
#define ASN1_F1AP_ID_UL_PDU_SESSION_AGGR_MAX_BIT_RATE 181
#define ASN1_F1AP_ID_SERVING_CELL_MO 182
#define ASN1_F1AP_ID_QOS_FLOW_MAP_IND 183
#define ASN1_F1AP_ID_RRC_DELIVERY_STATUS_REQUEST 184
#define ASN1_F1AP_ID_RRC_DELIVERY_STATUS 185
#define ASN1_F1AP_ID_BEARER_TYPE_CHANGE 186
#define ASN1_F1AP_ID_RLC_MODE 187
#define ASN1_F1AP_ID_DUPL_ACTIVATION 188
#define ASN1_F1AP_ID_DED_SI_DELIVERY_NEEDED_UE_LIST 189
#define ASN1_F1AP_ID_DED_SI_DELIVERY_NEEDED_UE_ITEM 190
#define ASN1_F1AP_ID_DRX_LONG_CYCLE_START_OFFSET 191
#define ASN1_F1AP_ID_UL_PDCP_SN_LEN 192
#define ASN1_F1AP_ID_SEL_BAND_COMBINATION_IDX 193
#define ASN1_F1AP_ID_SEL_FEATURE_SET_ENTRY_IDX 194
#define ASN1_F1AP_ID_RES_COORDINATION_TRANSFER_INFO 195
#define ASN1_F1AP_ID_EXTENDED_SERVED_PLMNS_LIST 196
#define ASN1_F1AP_ID_EXTENDED_AVAILABLE_PLMN_LIST 197
#define ASN1_F1AP_ID_ASSOCIATED_SCELL_LIST 198
#define ASN1_F1AP_ID_LATEST_RRC_VERSION_ENHANCED 199
#define ASN1_F1AP_ID_ASSOCIATED_SCELL_ITEM 200
#define ASN1_F1AP_ID_CELL_DIRECTION 201
#define ASN1_F1AP_ID_SRBS_SETUP_LIST 202
#define ASN1_F1AP_ID_SRBS_SETUP_ITEM 203
#define ASN1_F1AP_ID_SRBS_SETUP_MOD_LIST 204
#define ASN1_F1AP_ID_SRBS_SETUP_MOD_ITEM 205
#define ASN1_F1AP_ID_SRBS_MODIFIED_LIST 206
#define ASN1_F1AP_ID_SRBS_MODIFIED_ITEM 207
#define ASN1_F1AP_ID_PH_INFO_SCG 208
#define ASN1_F1AP_ID_REQUESTED_BAND_COMBINATION_IDX 209
#define ASN1_F1AP_ID_REQUESTED_FEATURE_SET_ENTRY_IDX 210
#define ASN1_F1AP_ID_REQUESTED_P_MAX_FR2 211
#define ASN1_F1AP_ID_DRX_CFG 212
#define ASN1_F1AP_ID_IGNORE_RES_COORDINATION_CONTAINER 213
#define ASN1_F1AP_ID_UE_ASSIST_INFO 214
#define ASN1_F1AP_ID_NEEDFOR_GAP 215
#define ASN1_F1AP_ID_PAGING_ORIGIN 216
#define ASN1_F1AP_ID_NEW_GNB_CU_UE_F1AP_ID 217
#define ASN1_F1AP_ID_REDIRECTED_RRCMSG 218
#define ASN1_F1AP_ID_NEW_GNB_DU_UE_F1AP_ID 219
#define ASN1_F1AP_ID_NOTIF_INFO 220
#define ASN1_F1AP_ID_PLMN_ASSIST_INFO_FOR_NET_SHAR 221
#define ASN1_F1AP_ID_UE_CONTEXT_NOT_RETRIEVABLE 222
#define ASN1_F1AP_ID_BPLMN_ID_INFO_LIST 223
#define ASN1_F1AP_ID_SEL_PLMN_ID 224
#define ASN1_F1AP_ID_UAC_ASSIST_INFO 225
#define ASN1_F1AP_ID_RAN_UE_ID 226
#define ASN1_F1AP_ID_GNB_DU_TNL_ASSOC_TO_REM_ITEM 227
#define ASN1_F1AP_ID_GNB_DU_TNL_ASSOC_TO_REM_LIST 228
#define ASN1_F1AP_ID_TNL_ASSOC_TRANSPORT_LAYER_ADDRESS_GNB_DU 229
#define ASN1_F1AP_ID_PORT_NUM 230
#define ASN1_F1AP_ID_ADD_SIB_MSG_LIST 231
#define ASN1_F1AP_ID_CELL_TYPE 232
#define ASN1_F1AP_ID_IGNORE_PRACH_CFG 233
#define ASN1_F1AP_ID_CG_CFG 234
#define ASN1_F1AP_ID_PDCCH_BLIND_DETECTION_SCG 235
#define ASN1_F1AP_ID_REQUESTED_PDCCH_BLIND_DETECTION_SCG 236
#define ASN1_F1AP_ID_PH_INFO_MCG 237
#define ASN1_F1AP_ID_MEAS_GAP_SHARING_CFG 238
#define ASN1_F1AP_ID_SYS_INFO_AREA_ID 239
#define ASN1_F1AP_ID_AREA_SCOPE 240
#define ASN1_F1AP_ID_RRC_CONTAINER_RRC_SETUP_COMPLETE 241

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// AdditionalSIBMessageList-Item-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using add_sib_msg_list_item_ext_ies_o = protocol_ext_empty_o;

using add_sib_msg_list_item_ext_ies_container = protocol_ext_container_empty_l;

// AdditionalSIBMessageList-Item ::= SEQUENCE
struct add_sib_msg_list_item_s {
  bool                                    ie_exts_present = false;
  unbounded_octstring<true>               add_sib;
  add_sib_msg_list_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AdditionalSIBMessageList ::= SEQUENCE (SIZE (1..63)) OF AdditionalSIBMessageList-Item
using add_sib_msg_list_l = dyn_array<add_sib_msg_list_item_s>;

// AllocationAndRetentionPriority-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using alloc_and_retention_prio_ext_ies_o = protocol_ext_empty_o;

// Pre-emptionCapability ::= ENUMERATED
struct pre_emption_cap_opts {
  enum options { shall_not_trigger_pre_emption, may_trigger_pre_emption, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<pre_emption_cap_opts> pre_emption_cap_e;

// Pre-emptionVulnerability ::= ENUMERATED
struct pre_emption_vulnerability_opts {
  enum options { not_pre_emptable, pre_emptable, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<pre_emption_vulnerability_opts> pre_emption_vulnerability_e;

using alloc_and_retention_prio_ext_ies_container = protocol_ext_container_empty_l;

// AllocationAndRetentionPriority ::= SEQUENCE
struct alloc_and_retention_prio_s {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  uint8_t                                    prio_level      = 0;
  pre_emption_cap_e                          pre_emption_cap;
  pre_emption_vulnerability_e                pre_emption_vulnerability;
  alloc_and_retention_prio_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NRCGI-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using nr_cgi_ext_ies_o = protocol_ext_empty_o;

// Associated-SCell-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using associated_scell_item_ext_ies_o = protocol_ext_empty_o;

using nr_cgi_ext_ies_container = protocol_ext_container_empty_l;

// NRCGI ::= SEQUENCE
struct nrcgi_s {
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<36, false, true> nr_cell_id;
  nr_cgi_ext_ies_container         ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using associated_scell_item_ext_ies_container = protocol_ext_container_empty_l;

// Associated-SCell-Item ::= SEQUENCE
struct associated_scell_item_s {
  bool                                    ie_exts_present = false;
  nrcgi_s                                 scell_id;
  associated_scell_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Associated-SCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct associated_scell_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { associated_scell_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::associated_scell_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    associated_scell_item_s&       associated_scell_item() { return c; }
    const associated_scell_item_s& associated_scell_item() const { return c; }

  private:
    associated_scell_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Associated-SCell-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using associated_scell_list_l = bounded_array<protocol_ie_single_container_s<associated_scell_item_ies_o>, 32>;

// AvailablePLMNList-Item-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using available_plmn_list_item_ext_ies_o = protocol_ext_empty_o;

using available_plmn_list_item_ext_ies_container = protocol_ext_container_empty_l;

// AvailablePLMNList-Item ::= SEQUENCE
struct available_plmn_list_item_s {
  bool                                       ie_exts_present = false;
  fixed_octstring<3, true>                   plmn_id;
  available_plmn_list_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AvailablePLMNList ::= SEQUENCE (SIZE (1..6)) OF AvailablePLMNList-Item
using available_plmn_list_l = dyn_array<available_plmn_list_item_s>;

// ExtendedAvailablePLMN-Item-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using extended_available_plmn_item_ext_ies_o = protocol_ext_empty_o;

using extended_available_plmn_item_ext_ies_container = protocol_ext_container_empty_l;

// ExtendedAvailablePLMN-Item ::= SEQUENCE
struct extended_available_plmn_item_s {
  bool                                           ie_exts_present = false;
  fixed_octstring<3, true>                       plmn_id;
  extended_available_plmn_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BPLMN-ID-Info-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using bplmn_id_info_item_ext_ies_o = protocol_ext_empty_o;

// ExtendedAvailablePLMN-List ::= SEQUENCE (SIZE (1..6)) OF ExtendedAvailablePLMN-Item
using extended_available_plmn_list_l = dyn_array<extended_available_plmn_item_s>;

using bplmn_id_info_item_ext_ies_container = protocol_ext_container_empty_l;

// BPLMN-ID-Info-Item ::= SEQUENCE
struct bplmn_id_info_item_s {
  bool                                 ext                 = false;
  bool                                 five_gs_tac_present = false;
  bool                                 ranac_present       = false;
  bool                                 ie_exts_present     = false;
  available_plmn_list_l                plmn_id_list;
  extended_available_plmn_list_l       extended_plmn_id_list;
  fixed_octstring<3, true>             five_gs_tac;
  fixed_bitstring<36, false, true>     nr_cell_id;
  uint16_t                             ranac = 0;
  bplmn_id_info_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BPLMN-ID-Info-List ::= SEQUENCE (SIZE (1..11)) OF BPLMN-ID-Info-Item
using bplmn_id_info_list_l = dyn_array<bplmn_id_info_item_s>;

// Broadcast-To-Be-Cancelled-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using broadcast_to_be_cancelled_item_ext_ies_o = protocol_ext_empty_o;

using broadcast_to_be_cancelled_item_ext_ies_container = protocol_ext_container_empty_l;

// Broadcast-To-Be-Cancelled-Item ::= SEQUENCE
struct broadcast_to_be_cancelled_item_s {
  bool                                             ext             = false;
  bool                                             ie_exts_present = false;
  nrcgi_s                                          nrcgi;
  broadcast_to_be_cancelled_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Broadcast-To-Be-Cancelled-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_to_be_cancelled_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_to_be_cancelled_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::broadcast_to_be_cancelled_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_to_be_cancelled_item_s&       broadcast_to_be_cancelled_item() { return c; }
    const broadcast_to_be_cancelled_item_s& broadcast_to_be_cancelled_item() const { return c; }

  private:
    broadcast_to_be_cancelled_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Broadcast-To-Be-Cancelled-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_to_be_cancelled_list_l =
    dyn_array<protocol_ie_single_container_s<broadcast_to_be_cancelled_list_item_ies_o>>;

// CNUEPagingIdentity-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
using cn_ue_paging_id_ext_ies_o = protocol_ies_empty_o;

// CNUEPagingIdentity ::= CHOICE
struct cn_ue_paging_id_c {
  struct types_opts {
    enum options { five_g_s_tmsi, choice_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  cn_ue_paging_id_c() = default;
  cn_ue_paging_id_c(const cn_ue_paging_id_c& other);
  cn_ue_paging_id_c& operator=(const cn_ue_paging_id_c& other);
  ~cn_ue_paging_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<48, false, true>& five_g_s_tmsi()
  {
    assert_choice_type(types::five_g_s_tmsi, type_, "CNUEPagingIdentity");
    return c.get<fixed_bitstring<48, false, true>>();
  }
  protocol_ie_single_container_s<cn_ue_paging_id_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "CNUEPagingIdentity");
    return c.get<protocol_ie_single_container_s<cn_ue_paging_id_ext_ies_o>>();
  }
  const fixed_bitstring<48, false, true>& five_g_s_tmsi() const
  {
    assert_choice_type(types::five_g_s_tmsi, type_, "CNUEPagingIdentity");
    return c.get<fixed_bitstring<48, false, true>>();
  }
  const protocol_ie_single_container_s<cn_ue_paging_id_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "CNUEPagingIdentity");
    return c.get<protocol_ie_single_container_s<cn_ue_paging_id_ext_ies_o>>();
  }
  fixed_bitstring<48, false, true>&                          set_five_g_s_tmsi();
  protocol_ie_single_container_s<cn_ue_paging_id_ext_ies_o>& set_choice_ext();

private:
  types                                                                                                        type_;
  choice_buffer_t<fixed_bitstring<48, false, true>, protocol_ie_single_container_s<cn_ue_paging_id_ext_ies_o>> c;

  void destroy_();
};

// Endpoint-IP-address-and-port-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct endpoint_ip_address_and_port_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { port_num, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::port_num; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<16, false, true>&       port_num() { return c; }
    const fixed_bitstring<16, false, true>& port_num() const { return c; }

  private:
    fixed_bitstring<16, false, true> c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// CP-TransportLayerAddress-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
using cp_transport_layer_address_ext_ies_o = protocol_ies_empty_o;

// Endpoint-IP-address-and-port ::= SEQUENCE
struct endpoint_ip_address_and_port_s {
  bounded_bitstring<1, 160, true, true>                            endpoint_ip_address;
  protocol_ext_container_l<endpoint_ip_address_and_port_ext_ies_o> ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CP-TransportLayerAddress ::= CHOICE
struct cp_transport_layer_address_c {
  struct types_opts {
    enum options { endpoint_ip_address, endpoint_ip_address_and_port, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  cp_transport_layer_address_c() = default;
  cp_transport_layer_address_c(const cp_transport_layer_address_c& other);
  cp_transport_layer_address_c& operator=(const cp_transport_layer_address_c& other);
  ~cp_transport_layer_address_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  bounded_bitstring<1, 160, true, true>& endpoint_ip_address()
  {
    assert_choice_type(types::endpoint_ip_address, type_, "CP-TransportLayerAddress");
    return c.get<bounded_bitstring<1, 160, true, true>>();
  }
  endpoint_ip_address_and_port_s& endpoint_ip_address_and_port()
  {
    assert_choice_type(types::endpoint_ip_address_and_port, type_, "CP-TransportLayerAddress");
    return c.get<endpoint_ip_address_and_port_s>();
  }
  protocol_ie_single_container_s<cp_transport_layer_address_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "CP-TransportLayerAddress");
    return c.get<protocol_ie_single_container_s<cp_transport_layer_address_ext_ies_o>>();
  }
  const bounded_bitstring<1, 160, true, true>& endpoint_ip_address() const
  {
    assert_choice_type(types::endpoint_ip_address, type_, "CP-TransportLayerAddress");
    return c.get<bounded_bitstring<1, 160, true, true>>();
  }
  const endpoint_ip_address_and_port_s& endpoint_ip_address_and_port() const
  {
    assert_choice_type(types::endpoint_ip_address_and_port, type_, "CP-TransportLayerAddress");
    return c.get<endpoint_ip_address_and_port_s>();
  }
  const protocol_ie_single_container_s<cp_transport_layer_address_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "CP-TransportLayerAddress");
    return c.get<protocol_ie_single_container_s<cp_transport_layer_address_ext_ies_o>>();
  }
  bounded_bitstring<1, 160, true, true>&                                set_endpoint_ip_address();
  endpoint_ip_address_and_port_s&                                       set_endpoint_ip_address_and_port();
  protocol_ie_single_container_s<cp_transport_layer_address_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<bounded_bitstring<1, 160, true, true>,
                  endpoint_ip_address_and_port_s,
                  protocol_ie_single_container_s<cp_transport_layer_address_ext_ies_o>>
      c;

  void destroy_();
};

// CUtoDURRCInformation-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct cu_to_du_rrc_info_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { ho_prep_info, cell_group_cfg, meas_timing_cfg, ue_assist_info, cg_cfg, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    unbounded_octstring<true>&       ho_prep_info();
    unbounded_octstring<true>&       cell_group_cfg();
    unbounded_octstring<true>&       meas_timing_cfg();
    unbounded_octstring<true>&       ue_assist_info();
    unbounded_octstring<true>&       cg_cfg();
    const unbounded_octstring<true>& ho_prep_info() const;
    const unbounded_octstring<true>& cell_group_cfg() const;
    const unbounded_octstring<true>& meas_timing_cfg() const;
    const unbounded_octstring<true>& ue_assist_info() const;
    const unbounded_octstring<true>& cg_cfg() const;

  private:
    types                                      type_;
    choice_buffer_t<unbounded_octstring<true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct cu_to_du_rrc_info_ext_ies_container {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // member variables
  bool                                  ho_prep_info_present    = false;
  bool                                  cell_group_cfg_present  = false;
  bool                                  meas_timing_cfg_present = false;
  bool                                  ue_assist_info_present  = false;
  bool                                  cg_cfg_present          = false;
  ie_field_s<unbounded_octstring<true>> ho_prep_info;
  ie_field_s<unbounded_octstring<true>> cell_group_cfg;
  ie_field_s<unbounded_octstring<true>> meas_timing_cfg;
  ie_field_s<unbounded_octstring<true>> ue_assist_info;
  ie_field_s<unbounded_octstring<true>> cg_cfg;

  // sequence methods
  cu_to_du_rrc_info_ext_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CUtoDURRCInformation ::= SEQUENCE
struct cu_to_du_rrc_info_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  unbounded_octstring<true>           cg_cfg_info;
  unbounded_octstring<true>           ue_cap_rat_container_list;
  unbounded_octstring<true>           meas_cfg;
  cu_to_du_rrc_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Candidate-SpCell-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using candidate_sp_cell_item_ext_ies_o = protocol_ext_empty_o;

using candidate_sp_cell_item_ext_ies_container = protocol_ext_container_empty_l;

// Candidate-SpCell-Item ::= SEQUENCE
struct candidate_sp_cell_item_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  nrcgi_s                                  candidate_sp_cell_id;
  candidate_sp_cell_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Candidate-SpCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct candidate_sp_cell_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { candidate_sp_cell_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::candidate_sp_cell_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    candidate_sp_cell_item_s&       candidate_sp_cell_item() { return c; }
    const candidate_sp_cell_item_s& candidate_sp_cell_item() const { return c; }

  private:
    candidate_sp_cell_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Candidate-SpCell-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using candidate_sp_cell_list_l = dyn_array<protocol_ie_single_container_s<candidate_sp_cell_item_ies_o>>;

// Cause-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
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
typedef enumerated<cause_misc_opts, true> cause_misc_e;

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
typedef enumerated<cause_protocol_opts, true> cause_protocol_e;

// CauseRadioNetwork ::= ENUMERATED
struct cause_radio_network_opts {
  enum options {
    unspecified,
    rl_fail_rlc,
    unknown_or_already_allocated_gnb_cu_ue_f1ap_id,
    unknown_or_already_allocated_gnb_du_ue_f1ap_id,
    unknown_or_inconsistent_pair_of_ue_f1ap_id,
    interaction_with_other_proc,
    not_supported_qci_value,
    action_desirable_for_radio_reasons,
    no_radio_res_available,
    proc_cancelled,
    normal_release,
    // ...
    cell_not_available,
    rl_fail_others,
    ue_rejection,
    res_not_available_for_the_slice,
    amf_initiated_abnormal_release,
    release_due_to_pre_emption,
    plmn_not_served_by_the_gnb_cu,
    multiple_drb_id_instances,
    unknown_drb_id,
    nulltype
  } value;

  const char* to_string() const;
};
typedef enumerated<cause_radio_network_opts, true, 9> cause_radio_network_e;

// CauseTransport ::= ENUMERATED
struct cause_transport_opts {
  enum options { unspecified, transport_res_unavailable, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<cause_transport_opts, true> cause_transport_e;

// Cause ::= CHOICE
struct cause_c {
  struct types_opts {
    enum options { radio_network, transport, protocol, misc, choice_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

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

// CellSize ::= ENUMERATED
struct cell_size_opts {
  enum options { verysmall, small, medium, large, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<cell_size_opts, true> cell_size_e;

// CellType-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using cell_type_ext_ies_o = protocol_ext_empty_o;

using cell_type_ext_ies_container = protocol_ext_container_empty_l;

// CellType ::= SEQUENCE
struct cell_type_s {
  bool                        ext             = false;
  bool                        ie_exts_present = false;
  cell_size_e                 cell_size;
  cell_type_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Cells-Broadcast-Cancelled-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using cells_broadcast_cancelled_item_ext_ies_o = protocol_ext_empty_o;

using cells_broadcast_cancelled_item_ext_ies_container = protocol_ext_container_empty_l;

// Cells-Broadcast-Cancelled-Item ::= SEQUENCE
struct cells_broadcast_cancelled_item_s {
  bool                                             ext             = false;
  bool                                             ie_exts_present = false;
  nrcgi_s                                          nrcgi;
  uint32_t                                         nof_broadcasts = 0;
  cells_broadcast_cancelled_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Cells-Broadcast-Cancelled-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_broadcast_cancelled_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_broadcast_cancelled_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::cells_broadcast_cancelled_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_broadcast_cancelled_item_s&       cells_broadcast_cancelled_item() { return c; }
    const cells_broadcast_cancelled_item_s& cells_broadcast_cancelled_item() const { return c; }

  private:
    cells_broadcast_cancelled_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-Broadcast-Cancelled-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using cells_broadcast_cancelled_list_l =
    dyn_array<protocol_ie_single_container_s<cells_broadcast_cancelled_list_item_ies_o>>;

// Cells-Broadcast-Completed-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using cells_broadcast_completed_item_ext_ies_o = protocol_ext_empty_o;

using cells_broadcast_completed_item_ext_ies_container = protocol_ext_container_empty_l;

// Cells-Broadcast-Completed-Item ::= SEQUENCE
struct cells_broadcast_completed_item_s {
  bool                                             ext             = false;
  bool                                             ie_exts_present = false;
  nrcgi_s                                          nrcgi;
  cells_broadcast_completed_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Cells-Broadcast-Completed-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_broadcast_completed_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_broadcast_completed_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::cells_broadcast_completed_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_broadcast_completed_item_s&       cells_broadcast_completed_item() { return c; }
    const cells_broadcast_completed_item_s& cells_broadcast_completed_item() const { return c; }

  private:
    cells_broadcast_completed_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-Broadcast-Completed-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using cells_broadcast_completed_list_l =
    dyn_array<protocol_ie_single_container_s<cells_broadcast_completed_list_item_ies_o>>;

// Cells-Failed-to-be-Activated-List-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using cells_failed_to_be_activ_list_item_ext_ies_o = protocol_ext_empty_o;

using cells_failed_to_be_activ_list_item_ext_ies_container = protocol_ext_container_empty_l;

// Cells-Failed-to-be-Activated-List-Item ::= SEQUENCE
struct cells_failed_to_be_activ_list_item_s {
  bool                                                 ext             = false;
  bool                                                 ie_exts_present = false;
  nrcgi_s                                              nrcgi;
  cause_c                                              cause;
  cells_failed_to_be_activ_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Cells-Failed-to-be-Activated-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_failed_to_be_activ_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_failed_to_be_activ_list_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::cells_failed_to_be_activ_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_failed_to_be_activ_list_item_s&       cells_failed_to_be_activ_list_item() { return c; }
    const cells_failed_to_be_activ_list_item_s& cells_failed_to_be_activ_list_item() const { return c; }

  private:
    cells_failed_to_be_activ_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-Failed-to-be-Activated-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using cells_failed_to_be_activ_list_l =
    dyn_array<protocol_ie_single_container_s<cells_failed_to_be_activ_list_item_ies_o>>;

// Service-State ::= ENUMERATED
struct service_state_opts {
  enum options { in_service, out_of_service, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<service_state_opts, true> service_state_e;

// Service-Status-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using service_status_ext_ies_o = protocol_ext_empty_o;

// Cells-Status-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using cells_status_item_ext_ies_o = protocol_ext_empty_o;

using service_status_ext_ies_container = protocol_ext_container_empty_l;

// Service-Status ::= SEQUENCE
struct service_status_s {
  struct switching_off_ongoing_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<switching_off_ongoing_opts, true> switching_off_ongoing_e_;

  // member variables
  bool                             ext                           = false;
  bool                             switching_off_ongoing_present = false;
  bool                             ie_exts_present               = false;
  service_state_e                  service_state;
  switching_off_ongoing_e_         switching_off_ongoing;
  service_status_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using cells_status_item_ext_ies_container = protocol_ext_container_empty_l;

// Cells-Status-Item ::= SEQUENCE
struct cells_status_item_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  nrcgi_s                             nrcgi;
  service_status_s                    service_status;
  cells_status_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Cells-Status-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_status_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_status_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::cells_status_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_status_item_s&       cells_status_item() { return c; }
    const cells_status_item_s& cells_status_item() const { return c; }

  private:
    cells_status_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-Status-List ::= SEQUENCE (SIZE (0..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using cells_status_list_l = dyn_array<protocol_ie_single_container_s<cells_status_item_ies_o>>;

// Cells-To-Be-Broadcast-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using cells_to_be_broadcast_item_ext_ies_o = protocol_ext_empty_o;

using cells_to_be_broadcast_item_ext_ies_container = protocol_ext_container_empty_l;

// Cells-To-Be-Broadcast-Item ::= SEQUENCE
struct cells_to_be_broadcast_item_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  nrcgi_s                                      nrcgi;
  cells_to_be_broadcast_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Cells-To-Be-Broadcast-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_to_be_broadcast_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_to_be_broadcast_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::cells_to_be_broadcast_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_to_be_broadcast_item_s&       cells_to_be_broadcast_item() { return c; }
    const cells_to_be_broadcast_item_s& cells_to_be_broadcast_item() const { return c; }

  private:
    cells_to_be_broadcast_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-To-Be-Broadcast-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using cells_to_be_broadcast_list_l = dyn_array<protocol_ie_single_container_s<cells_to_be_broadcast_list_item_ies_o>>;

// AreaScope ::= ENUMERATED
struct area_scope_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<area_scope_opts, true> area_scope_e;

// SibtypetobeupdatedListItem-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct sibtypetobeupd_list_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { area_scope, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::area_scope; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    area_scope_e&       area_scope() { return c; }
    const area_scope_e& area_scope() const { return c; }

  private:
    area_scope_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CUSystemInformation-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct gnb_cu_sys_info_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { sys_info_area_id, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::sys_info_area_id; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<24, false, true>&       sys_info_area_id() { return c; }
    const fixed_bitstring<24, false, true>& sys_info_area_id() const { return c; }

  private:
    fixed_bitstring<24, false, true> c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SibtypetobeupdatedListItem ::= SEQUENCE
struct sibtypetobeupd_list_item_s {
  bool                                                         ext     = false;
  uint8_t                                                      sibtype = 2;
  unbounded_octstring<true>                                    sibmsg;
  uint8_t                                                      value_tag = 0;
  protocol_ext_container_l<sibtypetobeupd_list_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CUSystemInformation ::= SEQUENCE
struct gnb_cu_sys_info_s {
  using sibtypetobeupdlist_l_ = dyn_array<sibtypetobeupd_list_item_s>;

  // member variables
  bool                                                ext = false;
  sibtypetobeupdlist_l_                               sibtypetobeupdlist;
  protocol_ext_container_l<gnb_cu_sys_info_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Cells-to-be-Activated-List-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct cells_to_be_activ_list_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { gnb_cu_sys_info, available_plmn_list, extended_available_plmn_list, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    gnb_cu_sys_info_s&                    gnb_cu_sys_info();
    available_plmn_list_l&                available_plmn_list();
    extended_available_plmn_list_l&       extended_available_plmn_list();
    const gnb_cu_sys_info_s&              gnb_cu_sys_info() const;
    const available_plmn_list_l&          available_plmn_list() const;
    const extended_available_plmn_list_l& extended_available_plmn_list() const;

  private:
    types                                                                                     type_;
    choice_buffer_t<available_plmn_list_l, extended_available_plmn_list_l, gnb_cu_sys_info_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct cells_to_be_activ_list_item_ext_ies_container {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // member variables
  bool                                                               gnb_cu_sys_info_present              = false;
  bool                                                               available_plmn_list_present          = false;
  bool                                                               extended_available_plmn_list_present = false;
  ie_field_s<gnb_cu_sys_info_s>                                      gnb_cu_sys_info;
  ie_field_s<dyn_seq_of<available_plmn_list_item_s, 1, 6, true>>     available_plmn_list;
  ie_field_s<dyn_seq_of<extended_available_plmn_item_s, 1, 6, true>> extended_available_plmn_list;

  // sequence methods
  cells_to_be_activ_list_item_ext_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Cells-to-be-Activated-List-Item ::= SEQUENCE
struct cells_to_be_activ_list_item_s {
  bool                                          ext             = false;
  bool                                          nr_pci_present  = false;
  bool                                          ie_exts_present = false;
  nrcgi_s                                       nrcgi;
  uint16_t                                      nr_pci = 0;
  cells_to_be_activ_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Cells-to-be-Activated-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_to_be_activ_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_to_be_activ_list_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::cells_to_be_activ_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_to_be_activ_list_item_s&       cells_to_be_activ_list_item() { return c; }
    const cells_to_be_activ_list_item_s& cells_to_be_activ_list_item() const { return c; }

  private:
    cells_to_be_activ_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-to-be-Activated-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using cells_to_be_activ_list_l = dyn_array<protocol_ie_single_container_s<cells_to_be_activ_list_item_ies_o>>;

// CellBarred ::= ENUMERATED
struct cell_barred_opts {
  enum options { barred, not_barred, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<cell_barred_opts, true> cell_barred_e;

// Cells-to-be-Barred-Item-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using cells_to_be_barred_item_ext_ies_o = protocol_ext_empty_o;

using cells_to_be_barred_item_ext_ies_container = protocol_ext_container_empty_l;

// Cells-to-be-Barred-Item ::= SEQUENCE
struct cells_to_be_barred_item_s {
  bool                                      ie_exts_present = false;
  nrcgi_s                                   nrcgi;
  cell_barred_e                             cell_barred;
  cells_to_be_barred_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Cells-to-be-Barred-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_to_be_barred_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_to_be_barred_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::cells_to_be_barred_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_to_be_barred_item_s&       cells_to_be_barred_item() { return c; }
    const cells_to_be_barred_item_s& cells_to_be_barred_item() const { return c; }

  private:
    cells_to_be_barred_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-to-be-Barred-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using cells_to_be_barred_list_l = dyn_array<protocol_ie_single_container_s<cells_to_be_barred_item_ies_o>>;

// Cells-to-be-Deactivated-List-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using cells_to_be_deactiv_list_item_ext_ies_o = protocol_ext_empty_o;

using cells_to_be_deactiv_list_item_ext_ies_container = protocol_ext_container_empty_l;

// Cells-to-be-Deactivated-List-Item ::= SEQUENCE
struct cells_to_be_deactiv_list_item_s {
  bool                                            ext             = false;
  bool                                            ie_exts_present = false;
  nrcgi_s                                         nrcgi;
  cells_to_be_deactiv_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Cells-to-be-Deactivated-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_to_be_deactiv_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_to_be_deactiv_list_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::cells_to_be_deactiv_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_to_be_deactiv_list_item_s&       cells_to_be_deactiv_list_item() { return c; }
    const cells_to_be_deactiv_list_item_s& cells_to_be_deactiv_list_item() const { return c; }

  private:
    cells_to_be_deactiv_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-to-be-Deactivated-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using cells_to_be_deactiv_list_l = dyn_array<protocol_ie_single_container_s<cells_to_be_deactiv_list_item_ies_o>>;

// CriticalityDiagnostics-IE-Item-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using crit_diagnostics_ie_item_ext_ies_o = protocol_ext_empty_o;

// TypeOfError ::= ENUMERATED
struct type_of_error_opts {
  enum options { not_understood, missing, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<type_of_error_opts, true> type_of_error_e;

using crit_diagnostics_ie_item_ext_ies_container = protocol_ext_container_empty_l;

// CriticalityDiagnostics-IE-Item ::= SEQUENCE
struct crit_diagnostics_ie_item_s {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  crit_e                                     ie_crit;
  uint32_t                                   ie_id = 0;
  type_of_error_e                            type_of_error;
  crit_diagnostics_ie_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CriticalityDiagnostics-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using crit_diagnostics_ext_ies_o = protocol_ext_empty_o;

// CriticalityDiagnostics-IE-List ::= SEQUENCE (SIZE (1..256)) OF CriticalityDiagnostics-IE-Item
using crit_diagnostics_ie_list_l = dyn_array<crit_diagnostics_ie_item_s>;

// TriggeringMessage ::= ENUMERATED
struct trigger_msg_opts {
  enum options { init_msg, successful_outcome, unsuccessful_outcome, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<trigger_msg_opts> trigger_msg_e;

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

// RAT-FrequencyPriorityInformation-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
using rat_freq_prio_info_ext_ies_o = protocol_ies_empty_o;

// ExecuteDuplication ::= ENUMERATED
struct execute_dupl_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<execute_dupl_opts, true> execute_dupl_e;

// RAT-FrequencyPriorityInformation ::= CHOICE
struct rat_freq_prio_info_c {
  struct types_opts {
    enum options { endc, ngran, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  rat_freq_prio_info_c() = default;
  rat_freq_prio_info_c(const rat_freq_prio_info_c& other);
  rat_freq_prio_info_c& operator=(const rat_freq_prio_info_c& other);
  ~rat_freq_prio_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint16_t& endc()
  {
    assert_choice_type(types::endc, type_, "RAT-FrequencyPriorityInformation");
    return c.get<uint16_t>();
  }
  uint16_t& ngran()
  {
    assert_choice_type(types::ngran, type_, "RAT-FrequencyPriorityInformation");
    return c.get<uint16_t>();
  }
  protocol_ie_single_container_s<rat_freq_prio_info_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "RAT-FrequencyPriorityInformation");
    return c.get<protocol_ie_single_container_s<rat_freq_prio_info_ext_ies_o>>();
  }
  const uint16_t& endc() const
  {
    assert_choice_type(types::endc, type_, "RAT-FrequencyPriorityInformation");
    return c.get<uint16_t>();
  }
  const uint16_t& ngran() const
  {
    assert_choice_type(types::ngran, type_, "RAT-FrequencyPriorityInformation");
    return c.get<uint16_t>();
  }
  const protocol_ie_single_container_s<rat_freq_prio_info_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "RAT-FrequencyPriorityInformation");
    return c.get<protocol_ie_single_container_s<rat_freq_prio_info_ext_ies_o>>();
  }
  uint16_t&                                                     set_endc();
  uint16_t&                                                     set_ngran();
  protocol_ie_single_container_s<rat_freq_prio_info_ext_ies_o>& set_choice_ext();

private:
  types                                                                         type_;
  choice_buffer_t<protocol_ie_single_container_s<rat_freq_prio_info_ext_ies_o>> c;

  void destroy_();
};

// RRCDeliveryStatusRequest ::= ENUMERATED
struct rrc_delivery_status_request_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<rrc_delivery_status_request_opts, true> rrc_delivery_status_request_e;

// UEContextNotRetrievable ::= ENUMERATED
struct ue_context_not_retrievable_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<ue_context_not_retrievable_opts, true> ue_context_not_retrievable_e;

// DLRRCMessageTransferIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct dl_rrc_msg_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        old_gnb_du_ue_f1ap_id,
        srbid,
        execute_dupl,
        rrc_container,
        rat_freq_prio_info,
        rrc_delivery_status_request,
        ue_context_not_retrievable,
        redirected_rrcmsg,
        plmn_assist_info_for_net_shar,
        new_gnb_cu_ue_f1ap_id,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                            gnb_cu_ue_f1ap_id();
    uint64_t&                            gnb_du_ue_f1ap_id();
    uint64_t&                            old_gnb_du_ue_f1ap_id();
    uint8_t&                             srbid();
    execute_dupl_e&                      execute_dupl();
    unbounded_octstring<true>&           rrc_container();
    rat_freq_prio_info_c&                rat_freq_prio_info();
    rrc_delivery_status_request_e&       rrc_delivery_status_request();
    ue_context_not_retrievable_e&        ue_context_not_retrievable();
    unbounded_octstring<true>&           redirected_rrcmsg();
    fixed_octstring<3, true>&            plmn_assist_info_for_net_shar();
    uint64_t&                            new_gnb_cu_ue_f1ap_id();
    const uint64_t&                      gnb_cu_ue_f1ap_id() const;
    const uint64_t&                      gnb_du_ue_f1ap_id() const;
    const uint64_t&                      old_gnb_du_ue_f1ap_id() const;
    const uint8_t&                       srbid() const;
    const execute_dupl_e&                execute_dupl() const;
    const unbounded_octstring<true>&     rrc_container() const;
    const rat_freq_prio_info_c&          rat_freq_prio_info() const;
    const rrc_delivery_status_request_e& rrc_delivery_status_request() const;
    const ue_context_not_retrievable_e&  ue_context_not_retrievable() const;
    const unbounded_octstring<true>&     redirected_rrcmsg() const;
    const fixed_octstring<3, true>&      plmn_assist_info_for_net_shar() const;
    const uint64_t&                      new_gnb_cu_ue_f1ap_id() const;

  private:
    types                                                                                      type_;
    choice_buffer_t<fixed_octstring<3, true>, rat_freq_prio_info_c, unbounded_octstring<true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct dl_rrc_msg_transfer_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      old_gnb_du_ue_f1ap_id_present         = false;
  bool                                                      execute_dupl_present                  = false;
  bool                                                      rat_freq_prio_info_present            = false;
  bool                                                      rrc_delivery_status_request_present   = false;
  bool                                                      ue_context_not_retrievable_present    = false;
  bool                                                      redirected_rrcmsg_present             = false;
  bool                                                      plmn_assist_info_for_net_shar_present = false;
  bool                                                      new_gnb_cu_ue_f1ap_id_present         = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> old_gnb_du_ue_f1ap_id;
  ie_field_s<integer<uint8_t, 0, 3, true, true>>            srbid;
  ie_field_s<execute_dupl_e>                                execute_dupl;
  ie_field_s<unbounded_octstring<true>>                     rrc_container;
  ie_field_s<rat_freq_prio_info_c>                          rat_freq_prio_info;
  ie_field_s<rrc_delivery_status_request_e>                 rrc_delivery_status_request;
  ie_field_s<ue_context_not_retrievable_e>                  ue_context_not_retrievable;
  ie_field_s<unbounded_octstring<true>>                     redirected_rrcmsg;
  ie_field_s<fixed_octstring<3, true>>                      plmn_assist_info_for_net_shar;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> new_gnb_cu_ue_f1ap_id;

  // sequence methods
  dl_rrc_msg_transfer_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLRRCMessageTransfer ::= SEQUENCE
using dl_rrc_msg_transfer_s = elementary_procedure_option<dl_rrc_msg_transfer_ies_container>;

// GTPTunnel-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using gtp_tunnel_ext_ies_o = protocol_ext_empty_o;

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

// UPTransportLayerInformation-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
using up_transport_layer_info_ext_ies_o = protocol_ies_empty_o;

// DLUPTNLInformation-ToBeSetup-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using dl_up_tnl_info_to_be_setup_item_ext_ies_o = protocol_ext_empty_o;

// UPTransportLayerInformation ::= CHOICE
struct up_transport_layer_info_c {
  struct types_opts {
    enum options { gtp_tunnel, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  up_transport_layer_info_c() = default;
  up_transport_layer_info_c(const up_transport_layer_info_c& other);
  up_transport_layer_info_c& operator=(const up_transport_layer_info_c& other);
  ~up_transport_layer_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  gtp_tunnel_s& gtp_tunnel()
  {
    assert_choice_type(types::gtp_tunnel, type_, "UPTransportLayerInformation");
    return c.get<gtp_tunnel_s>();
  }
  protocol_ie_single_container_s<up_transport_layer_info_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "UPTransportLayerInformation");
    return c.get<protocol_ie_single_container_s<up_transport_layer_info_ext_ies_o>>();
  }
  const gtp_tunnel_s& gtp_tunnel() const
  {
    assert_choice_type(types::gtp_tunnel, type_, "UPTransportLayerInformation");
    return c.get<gtp_tunnel_s>();
  }
  const protocol_ie_single_container_s<up_transport_layer_info_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "UPTransportLayerInformation");
    return c.get<protocol_ie_single_container_s<up_transport_layer_info_ext_ies_o>>();
  }
  gtp_tunnel_s&                                                      set_gtp_tunnel();
  protocol_ie_single_container_s<up_transport_layer_info_ext_ies_o>& set_choice_ext();

private:
  types                                                                                            type_;
  choice_buffer_t<gtp_tunnel_s, protocol_ie_single_container_s<up_transport_layer_info_ext_ies_o>> c;

  void destroy_();
};

using dl_up_tnl_info_to_be_setup_item_ext_ies_container = protocol_ext_container_empty_l;

// DLUPTNLInformation-ToBeSetup-Item ::= SEQUENCE
struct dl_up_tnl_info_to_be_setup_item_s {
  bool                                              ext             = false;
  bool                                              ie_exts_present = false;
  up_transport_layer_info_c                         dl_up_tnl_info;
  dl_up_tnl_info_to_be_setup_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLUPTNLInformation-ToBeSetup-List ::= SEQUENCE (SIZE (1..2)) OF DLUPTNLInformation-ToBeSetup-Item
using dl_up_tnl_info_to_be_setup_list_l = dyn_array<dl_up_tnl_info_to_be_setup_item_s>;

// DRB-Activity ::= ENUMERATED
struct drb_activity_opts {
  enum options { active, not_active, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<drb_activity_opts> drb_activity_e;

// DRB-Activity-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using drb_activity_item_ext_ies_o = protocol_ext_empty_o;

using drb_activity_item_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Activity-Item ::= SEQUENCE
struct drb_activity_item_s {
  bool                                ext                  = false;
  bool                                drb_activity_present = false;
  bool                                ie_exts_present      = false;
  uint8_t                             drb_id               = 1;
  drb_activity_e                      drb_activity;
  drb_activity_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Activity-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drb_activity_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drb_activity_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drb_activity_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drb_activity_item_s&       drb_activity_item() { return c; }
    const drb_activity_item_s& drb_activity_item() const { return c; }

  private:
    drb_activity_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRB-Activity-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drb_activity_list_l = dyn_array<protocol_ie_single_container_s<drb_activity_item_ies_o>>;

// PacketErrorRate-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using packet_error_rate_ext_ies_o = protocol_ext_empty_o;

// Dynamic5QIDescriptor-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using dyn_5qi_descriptor_ext_ies_o = protocol_ext_empty_o;

// NonDynamic5QIDescriptor-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using non_dyn_5qi_descriptor_ext_ies_o = protocol_ext_empty_o;

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

using dyn_5qi_descriptor_ext_ies_container = protocol_ext_container_empty_l;

// Dynamic5QIDescriptor ::= SEQUENCE
struct dyn_5qi_descriptor_s {
  struct delay_crit_opts {
    enum options { delay_crit, non_delay_crit, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<delay_crit_opts> delay_crit_e_;

  // member variables
  bool                                 five_qi_present               = false;
  bool                                 delay_crit_present            = false;
  bool                                 averaging_win_present         = false;
  bool                                 max_data_burst_volume_present = false;
  bool                                 ie_exts_present               = false;
  uint8_t                              qos_prio_level                = 1;
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

// GBR-QosFlowInformation-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using gbr_qos_flow_info_ext_ies_o = protocol_ext_empty_o;

// NGRANAllocationAndRetentionPriority-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using ngran_alloc_and_retention_prio_ext_ies_o = protocol_ext_empty_o;

using non_dyn_5qi_descriptor_ext_ies_container = protocol_ext_container_empty_l;

// NonDynamic5QIDescriptor ::= SEQUENCE
struct non_dyn_5qi_descriptor_s {
  bool                                     qos_prio_level_present        = false;
  bool                                     averaging_win_present         = false;
  bool                                     max_data_burst_volume_present = false;
  bool                                     ie_exts_present               = false;
  uint16_t                                 five_qi                       = 0;
  uint8_t                                  qos_prio_level                = 1;
  uint16_t                                 averaging_win                 = 0;
  uint16_t                                 max_data_burst_volume         = 0;
  non_dyn_5qi_descriptor_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoS-Characteristics-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
using qos_characteristics_ext_ies_o = protocol_ies_empty_o;

using gbr_qos_flow_info_ext_ies_container = protocol_ext_container_empty_l;

// GBR-QoSFlowInformation ::= SEQUENCE
struct gbr_qos_flow_info_s {
  bool                                ext                             = false;
  bool                                max_packet_loss_rate_dl_present = false;
  bool                                max_packet_loss_rate_ul_present = false;
  bool                                ie_exts_present                 = false;
  uint64_t                            max_flow_bit_rate_dl            = 0;
  uint64_t                            max_flow_bit_rate_ul            = 0;
  uint64_t                            guaranteed_flow_bit_rate_dl     = 0;
  uint64_t                            guaranteed_flow_bit_rate_ul     = 0;
  uint16_t                            max_packet_loss_rate_dl         = 0;
  uint16_t                            max_packet_loss_rate_ul         = 0;
  gbr_qos_flow_info_ext_ies_container ie_exts;
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

// QoS-Characteristics ::= CHOICE
struct qos_characteristics_c {
  struct types_opts {
    enum options { non_dyn_5qi, dyn_5qi, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

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

// QoSFlowLevelQoSParameters-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct qos_flow_level_qos_params_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { pdu_session_id, ul_pdu_session_aggr_max_bit_rate, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&       pdu_session_id();
    uint64_t&       ul_pdu_session_aggr_max_bit_rate();
    const uint16_t& pdu_session_id() const;
    const uint64_t& ul_pdu_session_aggr_max_bit_rate() const;

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

// QoSFlowMappingIndication ::= ENUMERATED
struct qos_flow_map_ind_opts {
  enum options { ul, dl, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<qos_flow_map_ind_opts, true> qos_flow_map_ind_e;

// Flows-Mapped-To-DRB-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct flows_mapped_to_drb_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { qos_flow_map_ind, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::qos_flow_map_ind; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    qos_flow_map_ind_e&       qos_flow_map_ind() { return c; }
    const qos_flow_map_ind_e& qos_flow_map_ind() const { return c; }

  private:
    qos_flow_map_ind_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct qos_flow_level_qos_params_ext_ies_container {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // member variables
  bool                                                        pdu_session_id_present                   = false;
  bool                                                        ul_pdu_session_aggr_max_bit_rate_present = false;
  ie_field_s<integer<uint16_t, 0, 255, false, true>>          pdu_session_id;
  ie_field_s<integer<uint64_t, 0, 4000000000000, true, true>> ul_pdu_session_aggr_max_bit_rate;

  // sequence methods
  qos_flow_level_qos_params_ext_ies_container();
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
  typedef enumerated<reflective_qos_attribute_opts, true> reflective_qos_attribute_e_;

  // member variables
  bool                                        gbr_qos_flow_info_present        = false;
  bool                                        reflective_qos_attribute_present = false;
  bool                                        ie_exts_present                  = false;
  qos_characteristics_c                       qos_characteristics;
  ngran_alloc_and_retention_prio_s            ngra_nalloc_retention_prio;
  gbr_qos_flow_info_s                         gbr_qos_flow_info;
  reflective_qos_attribute_e_                 reflective_qos_attribute;
  qos_flow_level_qos_params_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Flows-Mapped-To-DRB-Item ::= SEQUENCE
struct flows_mapped_to_drb_item_s {
  uint8_t                                                      qos_flow_id = 0;
  qos_flow_level_qos_params_s                                  qos_flow_level_qos_params;
  protocol_ext_container_l<flows_mapped_to_drb_item_ext_ies_o> ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SNSSAI-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using snssai_ext_ies_o = protocol_ext_empty_o;

// DRB-Information-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using drb_info_item_ext_ies_o = protocol_ext_empty_o;

// Flows-Mapped-To-DRB-List ::= SEQUENCE (SIZE (1..64)) OF Flows-Mapped-To-DRB-Item
using flows_mapped_to_drb_list_l = dyn_array<flows_mapped_to_drb_item_s>;

// NotificationControl ::= ENUMERATED
struct notif_ctrl_opts {
  enum options { active, not_active, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<notif_ctrl_opts, true> notif_ctrl_e;

using snssai_ext_ies_container = protocol_ext_container_empty_l;

// SNSSAI ::= SEQUENCE
struct snssai_s {
  bool                     sd_present      = false;
  bool                     ie_exts_present = false;
  fixed_octstring<1, true> sst;
  fixed_octstring<3, true> sd;
  snssai_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_info_item_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Information ::= SEQUENCE
struct drb_info_s {
  bool                            notif_ctrl_present = false;
  bool                            ie_exts_present    = false;
  qos_flow_level_qos_params_s     drb_qos;
  snssai_s                        snssai;
  notif_ctrl_e                    notif_ctrl;
  flows_mapped_to_drb_list_l      flows_mapped_to_drb_list;
  drb_info_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Notify-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using drb_notify_item_ext_ies_o = protocol_ext_empty_o;

// Notification-Cause ::= ENUMERATED
struct notif_cause_opts {
  enum options { fulfilled, not_fulfilled, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<notif_cause_opts, true> notif_cause_e;

using drb_notify_item_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Notify-Item ::= SEQUENCE
struct drb_notify_item_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  uint8_t                           drb_id          = 1;
  notif_cause_e                     notif_cause;
  drb_notify_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Notify-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drb_notify_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drb_notify_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drb_notify_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drb_notify_item_s&       drb_notify_item() { return c; }
    const drb_notify_item_s& drb_notify_item() const { return c; }

  private:
    drb_notify_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRB-Notify-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drb_notify_list_l = dyn_array<protocol_ie_single_container_s<drb_notify_item_ies_o>>;

// DRBs-FailedToBeModified-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using drbs_failed_to_be_modified_item_ext_ies_o = protocol_ext_empty_o;

using drbs_failed_to_be_modified_item_ext_ies_container = protocol_ext_container_empty_l;

// DRBs-FailedToBeModified-Item ::= SEQUENCE
struct drbs_failed_to_be_modified_item_s {
  bool                                              ext             = false;
  bool                                              cause_present   = false;
  bool                                              ie_exts_present = false;
  uint8_t                                           drb_id          = 1;
  cause_c                                           cause;
  drbs_failed_to_be_modified_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_failed_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_failed_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_failed_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_failed_to_be_modified_item_s&       drbs_failed_to_be_modified_item() { return c; }
    const drbs_failed_to_be_modified_item_s& drbs_failed_to_be_modified_item() const { return c; }

  private:
    drbs_failed_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-FailedToBeModified-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using drbs_failed_to_be_modified_list_l =
    dyn_array<protocol_ie_single_container_s<drbs_failed_to_be_modified_item_ies_o>>;

// DRBs-FailedToBeSetup-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using drbs_failed_to_be_setup_item_ext_ies_o = protocol_ext_empty_o;

using drbs_failed_to_be_setup_item_ext_ies_container = protocol_ext_container_empty_l;

// DRBs-FailedToBeSetup-Item ::= SEQUENCE
struct drbs_failed_to_be_setup_item_s {
  bool                                           ext             = false;
  bool                                           cause_present   = false;
  bool                                           ie_exts_present = false;
  uint8_t                                        drb_id          = 1;
  cause_c                                        cause;
  drbs_failed_to_be_setup_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_failed_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_failed_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_failed_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_failed_to_be_setup_item_s&       drbs_failed_to_be_setup_item() { return c; }
    const drbs_failed_to_be_setup_item_s& drbs_failed_to_be_setup_item() const { return c; }

  private:
    drbs_failed_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-FailedToBeSetup-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_failed_to_be_setup_list_l = dyn_array<protocol_ie_single_container_s<drbs_failed_to_be_setup_item_ies_o>>;

// DRBs-FailedToBeSetupMod-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using drbs_failed_to_be_setup_mod_item_ext_ies_o = protocol_ext_empty_o;

using drbs_failed_to_be_setup_mod_item_ext_ies_container = protocol_ext_container_empty_l;

// DRBs-FailedToBeSetupMod-Item ::= SEQUENCE
struct drbs_failed_to_be_setup_mod_item_s {
  bool                                               ext             = false;
  bool                                               cause_present   = false;
  bool                                               ie_exts_present = false;
  uint8_t                                            drb_id          = 1;
  cause_c                                            cause;
  drbs_failed_to_be_setup_mod_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_failed_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_failed_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_failed_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_failed_to_be_setup_mod_item_s&       drbs_failed_to_be_setup_mod_item() { return c; }
    const drbs_failed_to_be_setup_mod_item_s& drbs_failed_to_be_setup_mod_item() const { return c; }

  private:
    drbs_failed_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-FailedToBeSetupMod-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using drbs_failed_to_be_setup_mod_list_l =
    dyn_array<protocol_ie_single_container_s<drbs_failed_to_be_setup_mod_item_ies_o>>;

// RLC-Status-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using rlc_status_ext_ies_o = protocol_ext_empty_o;

// Reestablishment-Indication ::= ENUMERATED
struct reest_ind_opts {
  enum options { reestablished, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<reest_ind_opts, true> reest_ind_e;

using rlc_status_ext_ies_container = protocol_ext_container_empty_l;

// RLC-Status ::= SEQUENCE
struct rlc_status_s {
  bool                         ext             = false;
  bool                         ie_exts_present = false;
  reest_ind_e                  reest_ind;
  rlc_status_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-Modified-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct drbs_modified_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { rlc_status, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::rlc_status; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rlc_status_s&       rlc_status() { return c; }
    const rlc_status_s& rlc_status() const { return c; }

  private:
    rlc_status_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-Modified-Item ::= SEQUENCE
struct drbs_modified_item_s {
  bool                                                   ext          = false;
  bool                                                   lcid_present = false;
  uint8_t                                                drb_id       = 1;
  uint8_t                                                lcid         = 1;
  dl_up_tnl_info_to_be_setup_list_l                      dl_up_tnl_info_to_be_setup_list;
  protocol_ext_container_l<drbs_modified_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_modified_item_s&       drbs_modified_item() { return c; }
    const drbs_modified_item_s& drbs_modified_item() const { return c; }

  private:
    drbs_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-Modified-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_modified_list_l = dyn_array<protocol_ie_single_container_s<drbs_modified_item_ies_o>>;

// ULUPTNLInformation-ToBeSetup-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using ul_up_tnl_info_to_be_setup_item_ext_ies_o = protocol_ext_empty_o;

using ul_up_tnl_info_to_be_setup_item_ext_ies_container = protocol_ext_container_empty_l;

// ULUPTNLInformation-ToBeSetup-Item ::= SEQUENCE
struct ul_up_tnl_info_to_be_setup_item_s {
  bool                                              ext             = false;
  bool                                              ie_exts_present = false;
  up_transport_layer_info_c                         ul_up_tnl_info;
  ul_up_tnl_info_to_be_setup_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-ModifiedConf-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using drbs_modified_conf_item_ext_ies_o = protocol_ext_empty_o;

// ULUPTNLInformation-ToBeSetup-List ::= SEQUENCE (SIZE (1..2)) OF ULUPTNLInformation-ToBeSetup-Item
using ul_up_tnl_info_to_be_setup_list_l = dyn_array<ul_up_tnl_info_to_be_setup_item_s>;

using drbs_modified_conf_item_ext_ies_container = protocol_ext_container_empty_l;

// DRBs-ModifiedConf-Item ::= SEQUENCE
struct drbs_modified_conf_item_s {
  bool                                      ext             = false;
  bool                                      ie_exts_present = false;
  uint8_t                                   drb_id          = 1;
  ul_up_tnl_info_to_be_setup_list_l         ul_up_tnl_info_to_be_setup_list;
  drbs_modified_conf_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-ModifiedConf-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_modified_conf_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_modified_conf_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_modified_conf_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_modified_conf_item_s&       drbs_modified_conf_item() { return c; }
    const drbs_modified_conf_item_s& drbs_modified_conf_item() const { return c; }

  private:
    drbs_modified_conf_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-ModifiedConf-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_modified_conf_list_l = dyn_array<protocol_ie_single_container_s<drbs_modified_conf_item_ies_o>>;

// DRBs-Required-ToBeModified-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct drbs_required_to_be_modified_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { rlc_status, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::rlc_status; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rlc_status_s&       rlc_status() { return c; }
    const rlc_status_s& rlc_status() const { return c; }

  private:
    rlc_status_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-Required-ToBeModified-Item ::= SEQUENCE
struct drbs_required_to_be_modified_item_s {
  bool                                                                  ext    = false;
  uint8_t                                                               drb_id = 1;
  dl_up_tnl_info_to_be_setup_list_l                                     dl_up_tnl_info_to_be_setup_list;
  protocol_ext_container_l<drbs_required_to_be_modified_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-Required-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_required_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_required_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_required_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_required_to_be_modified_item_s&       drbs_required_to_be_modified_item() { return c; }
    const drbs_required_to_be_modified_item_s& drbs_required_to_be_modified_item() const { return c; }

  private:
    drbs_required_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-Required-ToBeModified-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using drbs_required_to_be_modified_list_l =
    dyn_array<protocol_ie_single_container_s<drbs_required_to_be_modified_item_ies_o>>;

// DRBs-Required-ToBeReleased-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using drbs_required_to_be_released_item_ext_ies_o = protocol_ext_empty_o;

using drbs_required_to_be_released_item_ext_ies_container = protocol_ext_container_empty_l;

// DRBs-Required-ToBeReleased-Item ::= SEQUENCE
struct drbs_required_to_be_released_item_s {
  bool                                                ext             = false;
  bool                                                ie_exts_present = false;
  uint8_t                                             drb_id          = 1;
  drbs_required_to_be_released_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-Required-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_required_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_required_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_required_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_required_to_be_released_item_s&       drbs_required_to_be_released_item() { return c; }
    const drbs_required_to_be_released_item_s& drbs_required_to_be_released_item() const { return c; }

  private:
    drbs_required_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-Required-ToBeReleased-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using drbs_required_to_be_released_list_l =
    dyn_array<protocol_ie_single_container_s<drbs_required_to_be_released_item_ies_o>>;

// DRBs-Setup-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using drbs_setup_item_ext_ies_o = protocol_ext_empty_o;

using drbs_setup_item_ext_ies_container = protocol_ext_container_empty_l;

// DRBs-Setup-Item ::= SEQUENCE
struct drbs_setup_item_s {
  bool                              ext             = false;
  bool                              lcid_present    = false;
  bool                              ie_exts_present = false;
  uint8_t                           drb_id          = 1;
  uint8_t                           lcid            = 1;
  dl_up_tnl_info_to_be_setup_list_l dl_up_tnl_info_to_be_setup_list;
  drbs_setup_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_setup_item_s&       drbs_setup_item() { return c; }
    const drbs_setup_item_s& drbs_setup_item() const { return c; }

  private:
    drbs_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-Setup-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_setup_list_l = dyn_array<protocol_ie_single_container_s<drbs_setup_item_ies_o>>;

// DRBs-SetupMod-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using drbs_setup_mod_item_ext_ies_o = protocol_ext_empty_o;

using drbs_setup_mod_item_ext_ies_container = protocol_ext_container_empty_l;

// DRBs-SetupMod-Item ::= SEQUENCE
struct drbs_setup_mod_item_s {
  bool                                  ext             = false;
  bool                                  lcid_present    = false;
  bool                                  ie_exts_present = false;
  uint8_t                               drb_id          = 1;
  uint8_t                               lcid            = 1;
  dl_up_tnl_info_to_be_setup_list_l     dl_up_tnl_info_to_be_setup_list;
  drbs_setup_mod_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_setup_mod_item_s&       drbs_setup_mod_item() { return c; }
    const drbs_setup_mod_item_s& drbs_setup_mod_item() const { return c; }

  private:
    drbs_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-SetupMod-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_setup_mod_list_l = dyn_array<protocol_ie_single_container_s<drbs_setup_mod_item_ies_o>>;

// GBR-QosInformation-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using gbr_qos_info_ext_ies_o = protocol_ext_empty_o;

// EUTRANQoS-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using eutran_qos_ext_ies_o = protocol_ext_empty_o;

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

// BearerTypeChange ::= ENUMERATED
struct bearer_type_change_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<bearer_type_change_opts, true> bearer_type_change_e;

// DCBasedDuplicationConfigured ::= ENUMERATED
struct dc_based_dupl_cfgured_opts {
  enum options { true_value, /*...*/ false_value, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<dc_based_dupl_cfgured_opts, true, 1> dc_based_dupl_cfgured_e;

// DuplicationActivation ::= ENUMERATED
struct dupl_activation_opts {
  enum options { active, inactive, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<dupl_activation_opts, true> dupl_activation_e;

using eutran_qos_ext_ies_container = protocol_ext_container_empty_l;

// EUTRANQoS ::= SEQUENCE
struct eutran_qos_s {
  bool                         ext                  = false;
  bool                         gbr_qos_info_present = false;
  bool                         ie_exts_present      = false;
  uint16_t                     qci                  = 0;
  alloc_and_retention_prio_s   alloc_and_retention_prio;
  gbr_qos_info_s               gbr_qos_info;
  eutran_qos_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCPSNLength ::= ENUMERATED
struct pdcp_sn_len_opts {
  enum options { twelve_bits, eighteen_bits, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<pdcp_sn_len_opts, true> pdcp_sn_len_e;

// QoSInformation-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct qos_info_ext_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drb_info, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drb_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drb_info_s&       drb_info() { return c; }
    const drb_info_s& drb_info() const { return c; }

  private:
    drb_info_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RLCMode ::= ENUMERATED
struct rlc_mode_opts {
  enum options {
    rlc_am,
    rlc_um_bidirectional,
    rlc_um_unidirectional_ul,
    rlc_um_unidirectional_dl,
    /*...*/ nulltype
  } value;

  const char* to_string() const;
};
typedef enumerated<rlc_mode_opts, true> rlc_mode_e;

// ULConfigurationExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using ul_cfg_ext_ies_o = protocol_ext_empty_o;

// ULUEConfiguration ::= ENUMERATED
struct ul_ue_cfg_opts {
  enum options { no_data, shared, only, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<ul_ue_cfg_opts, true> ul_ue_cfg_e;

// DRBs-ToBeModified-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct drbs_to_be_modified_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        dl_pdcp_sn_len,
        ul_pdcp_sn_len,
        bearer_type_change,
        rlc_mode,
        dupl_activation,
        dc_based_dupl_cfgured,
        dc_based_dupl_activation,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    pdcp_sn_len_e&                 dl_pdcp_sn_len();
    pdcp_sn_len_e&                 ul_pdcp_sn_len();
    bearer_type_change_e&          bearer_type_change();
    rlc_mode_e&                    rlc_mode();
    dupl_activation_e&             dupl_activation();
    dc_based_dupl_cfgured_e&       dc_based_dupl_cfgured();
    dupl_activation_e&             dc_based_dupl_activation();
    const pdcp_sn_len_e&           dl_pdcp_sn_len() const;
    const pdcp_sn_len_e&           ul_pdcp_sn_len() const;
    const bearer_type_change_e&    bearer_type_change() const;
    const rlc_mode_e&              rlc_mode() const;
    const dupl_activation_e&       dupl_activation() const;
    const dc_based_dupl_cfgured_e& dc_based_dupl_cfgured() const;
    const dupl_activation_e&       dc_based_dupl_activation() const;

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

// QoSInformation ::= CHOICE
struct qos_info_c {
  struct types_opts {
    enum options { eutran_qos, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  qos_info_c() = default;
  qos_info_c(const qos_info_c& other);
  qos_info_c& operator=(const qos_info_c& other);
  ~qos_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutran_qos_s& eutran_qos()
  {
    assert_choice_type(types::eutran_qos, type_, "QoSInformation");
    return c.get<eutran_qos_s>();
  }
  protocol_ie_single_container_s<qos_info_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "QoSInformation");
    return c.get<protocol_ie_single_container_s<qos_info_ext_ies_o>>();
  }
  const eutran_qos_s& eutran_qos() const
  {
    assert_choice_type(types::eutran_qos, type_, "QoSInformation");
    return c.get<eutran_qos_s>();
  }
  const protocol_ie_single_container_s<qos_info_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "QoSInformation");
    return c.get<protocol_ie_single_container_s<qos_info_ext_ies_o>>();
  }
  eutran_qos_s&                                       set_eutran_qos();
  protocol_ie_single_container_s<qos_info_ext_ies_o>& set_choice_ext();

private:
  types                                                                             type_;
  choice_buffer_t<eutran_qos_s, protocol_ie_single_container_s<qos_info_ext_ies_o>> c;

  void destroy_();
};

using ul_cfg_ext_ies_container = protocol_ext_container_empty_l;

// ULConfiguration ::= SEQUENCE
struct ul_cfg_s {
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  ul_ue_cfg_e              ul_ue_cfg;
  ul_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct drbs_to_be_modified_item_ext_ies_container {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // member variables
  bool                                dl_pdcp_sn_len_present           = false;
  bool                                ul_pdcp_sn_len_present           = false;
  bool                                bearer_type_change_present       = false;
  bool                                rlc_mode_present                 = false;
  bool                                dupl_activation_present          = false;
  bool                                dc_based_dupl_cfgured_present    = false;
  bool                                dc_based_dupl_activation_present = false;
  ie_field_s<pdcp_sn_len_e>           dl_pdcp_sn_len;
  ie_field_s<pdcp_sn_len_e>           ul_pdcp_sn_len;
  ie_field_s<bearer_type_change_e>    bearer_type_change;
  ie_field_s<rlc_mode_e>              rlc_mode;
  ie_field_s<dupl_activation_e>       dupl_activation;
  ie_field_s<dc_based_dupl_cfgured_e> dc_based_dupl_cfgured;
  ie_field_s<dupl_activation_e>       dc_based_dupl_activation;

  // sequence methods
  drbs_to_be_modified_item_ext_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-ToBeModified-Item ::= SEQUENCE
struct drbs_to_be_modified_item_s {
  bool                                       ext              = false;
  bool                                       qos_info_present = false;
  bool                                       ul_cfg_present   = false;
  bool                                       ie_exts_present  = false;
  uint8_t                                    drb_id           = 1;
  qos_info_c                                 qos_info;
  ul_up_tnl_info_to_be_setup_list_l          ul_up_tnl_info_to_be_setup_list;
  ul_cfg_s                                   ul_cfg;
  drbs_to_be_modified_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_to_be_modified_item_s&       drbs_to_be_modified_item() { return c; }
    const drbs_to_be_modified_item_s& drbs_to_be_modified_item() const { return c; }

  private:
    drbs_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-ToBeModified-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_to_be_modified_list_l = dyn_array<protocol_ie_single_container_s<drbs_to_be_modified_item_ies_o>>;

// DRBs-ToBeReleased-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using drbs_to_be_released_item_ext_ies_o = protocol_ext_empty_o;

using drbs_to_be_released_item_ext_ies_container = protocol_ext_container_empty_l;

// DRBs-ToBeReleased-Item ::= SEQUENCE
struct drbs_to_be_released_item_s {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  uint8_t                                    drb_id          = 1;
  drbs_to_be_released_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_to_be_released_item_s&       drbs_to_be_released_item() { return c; }
    const drbs_to_be_released_item_s& drbs_to_be_released_item() const { return c; }

  private:
    drbs_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-ToBeReleased-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_to_be_released_list_l = dyn_array<protocol_ie_single_container_s<drbs_to_be_released_item_ies_o>>;

// DRBs-ToBeSetup-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct drbs_to_be_setup_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { dc_based_dupl_cfgured, dc_based_dupl_activation, dl_pdcp_sn_len, ul_pdcp_sn_len, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    dc_based_dupl_cfgured_e&       dc_based_dupl_cfgured();
    dupl_activation_e&             dc_based_dupl_activation();
    pdcp_sn_len_e&                 dl_pdcp_sn_len();
    pdcp_sn_len_e&                 ul_pdcp_sn_len();
    const dc_based_dupl_cfgured_e& dc_based_dupl_cfgured() const;
    const dupl_activation_e&       dc_based_dupl_activation() const;
    const pdcp_sn_len_e&           dl_pdcp_sn_len() const;
    const pdcp_sn_len_e&           ul_pdcp_sn_len() const;

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

struct drbs_to_be_setup_item_ext_ies_container {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // member variables
  bool                                dc_based_dupl_cfgured_present    = false;
  bool                                dc_based_dupl_activation_present = false;
  bool                                ul_pdcp_sn_len_present           = false;
  ie_field_s<dc_based_dupl_cfgured_e> dc_based_dupl_cfgured;
  ie_field_s<dupl_activation_e>       dc_based_dupl_activation;
  ie_field_s<pdcp_sn_len_e>           dl_pdcp_sn_len;
  ie_field_s<pdcp_sn_len_e>           ul_pdcp_sn_len;

  // sequence methods
  drbs_to_be_setup_item_ext_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-ToBeSetup-Item ::= SEQUENCE
struct drbs_to_be_setup_item_s {
  bool                                    ext                     = false;
  bool                                    ul_cfg_present          = false;
  bool                                    dupl_activation_present = false;
  bool                                    ie_exts_present         = false;
  uint8_t                                 drb_id                  = 1;
  qos_info_c                              qos_info;
  ul_up_tnl_info_to_be_setup_list_l       ul_up_tnl_info_to_be_setup_list;
  rlc_mode_e                              rlc_mode;
  ul_cfg_s                                ul_cfg;
  dupl_activation_e                       dupl_activation;
  drbs_to_be_setup_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_to_be_setup_item_s&       drbs_to_be_setup_item() { return c; }
    const drbs_to_be_setup_item_s& drbs_to_be_setup_item() const { return c; }

  private:
    drbs_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-ToBeSetup-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_to_be_setup_list_l = dyn_array<protocol_ie_single_container_s<drbs_to_be_setup_item_ies_o>>;

// DRBs-ToBeSetupMod-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct drbs_to_be_setup_mod_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { dc_based_dupl_cfgured, dc_based_dupl_activation, dl_pdcp_sn_len, ul_pdcp_sn_len, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    dc_based_dupl_cfgured_e&       dc_based_dupl_cfgured();
    dupl_activation_e&             dc_based_dupl_activation();
    pdcp_sn_len_e&                 dl_pdcp_sn_len();
    pdcp_sn_len_e&                 ul_pdcp_sn_len();
    const dc_based_dupl_cfgured_e& dc_based_dupl_cfgured() const;
    const dupl_activation_e&       dc_based_dupl_activation() const;
    const pdcp_sn_len_e&           dl_pdcp_sn_len() const;
    const pdcp_sn_len_e&           ul_pdcp_sn_len() const;

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

struct drbs_to_be_setup_mod_item_ext_ies_container {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // member variables
  bool                                dc_based_dupl_cfgured_present    = false;
  bool                                dc_based_dupl_activation_present = false;
  bool                                dl_pdcp_sn_len_present           = false;
  bool                                ul_pdcp_sn_len_present           = false;
  ie_field_s<dc_based_dupl_cfgured_e> dc_based_dupl_cfgured;
  ie_field_s<dupl_activation_e>       dc_based_dupl_activation;
  ie_field_s<pdcp_sn_len_e>           dl_pdcp_sn_len;
  ie_field_s<pdcp_sn_len_e>           ul_pdcp_sn_len;

  // sequence methods
  drbs_to_be_setup_mod_item_ext_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-ToBeSetupMod-Item ::= SEQUENCE
struct drbs_to_be_setup_mod_item_s {
  bool                                        ext                     = false;
  bool                                        ul_cfg_present          = false;
  bool                                        dupl_activation_present = false;
  bool                                        ie_exts_present         = false;
  uint8_t                                     drb_id                  = 1;
  qos_info_c                                  qos_info;
  ul_up_tnl_info_to_be_setup_list_l           ul_up_tnl_info_to_be_setup_list;
  rlc_mode_e                                  rlc_mode;
  ul_cfg_s                                    ul_cfg;
  dupl_activation_e                           dupl_activation;
  drbs_to_be_setup_mod_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_to_be_setup_mod_item_s&       drbs_to_be_setup_mod_item() { return c; }
    const drbs_to_be_setup_mod_item_s& drbs_to_be_setup_mod_item() const { return c; }

  private:
    drbs_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-ToBeSetupMod-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_to_be_setup_mod_list_l = dyn_array<protocol_ie_single_container_s<drbs_to_be_setup_mod_item_ies_o>>;

// DRXCycle-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using drx_cycle_ext_ies_o = protocol_ext_empty_o;

// LongDRXCycleLength ::= ENUMERATED
struct long_drx_cycle_len_opts {
  enum options {
    ms10,
    ms20,
    ms32,
    ms40,
    ms60,
    ms64,
    ms70,
    ms80,
    ms128,
    ms160,
    ms256,
    ms320,
    ms512,
    ms640,
    ms1024,
    ms1280,
    ms2048,
    ms2560,
    ms5120,
    ms10240,
    // ...
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<long_drx_cycle_len_opts, true> long_drx_cycle_len_e;

// ShortDRXCycleLength ::= ENUMERATED
struct short_drx_cycle_len_opts {
  enum options {
    ms2,
    ms3,
    ms4,
    ms5,
    ms6,
    ms7,
    ms8,
    ms10,
    ms14,
    ms16,
    ms20,
    ms30,
    ms32,
    ms35,
    ms40,
    ms64,
    ms80,
    ms128,
    ms160,
    ms256,
    ms320,
    ms512,
    ms640,
    // ...
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<short_drx_cycle_len_opts, true> short_drx_cycle_len_e;

using drx_cycle_ext_ies_container = protocol_ext_container_empty_l;

// DRXCycle ::= SEQUENCE
struct drx_cycle_s {
  bool                        ext                           = false;
  bool                        short_drx_cycle_len_present   = false;
  bool                        short_drx_cycle_timer_present = false;
  bool                        ie_exts_present               = false;
  long_drx_cycle_len_e        long_drx_cycle_len;
  short_drx_cycle_len_e       short_drx_cycle_len;
  uint8_t                     short_drx_cycle_timer = 1;
  drx_cycle_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DUtoCURRCInformation-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct du_to_cu_rrc_info_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        drx_long_cycle_start_offset,
        sel_band_combination_idx,
        sel_feature_set_entry_idx,
        ph_info_scg,
        requested_band_combination_idx,
        requested_feature_set_entry_idx,
        requested_p_max_fr2,
        drx_cfg,
        pdcch_blind_detection_scg,
        requested_pdcch_blind_detection_scg,
        ph_info_mcg,
        meas_gap_sharing_cfg,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                        drx_long_cycle_start_offset();
    unbounded_octstring<true>&       sel_band_combination_idx();
    unbounded_octstring<true>&       sel_feature_set_entry_idx();
    unbounded_octstring<true>&       ph_info_scg();
    unbounded_octstring<true>&       requested_band_combination_idx();
    unbounded_octstring<true>&       requested_feature_set_entry_idx();
    unbounded_octstring<true>&       requested_p_max_fr2();
    unbounded_octstring<true>&       drx_cfg();
    unbounded_octstring<true>&       pdcch_blind_detection_scg();
    unbounded_octstring<true>&       requested_pdcch_blind_detection_scg();
    unbounded_octstring<true>&       ph_info_mcg();
    unbounded_octstring<true>&       meas_gap_sharing_cfg();
    const uint16_t&                  drx_long_cycle_start_offset() const;
    const unbounded_octstring<true>& sel_band_combination_idx() const;
    const unbounded_octstring<true>& sel_feature_set_entry_idx() const;
    const unbounded_octstring<true>& ph_info_scg() const;
    const unbounded_octstring<true>& requested_band_combination_idx() const;
    const unbounded_octstring<true>& requested_feature_set_entry_idx() const;
    const unbounded_octstring<true>& requested_p_max_fr2() const;
    const unbounded_octstring<true>& drx_cfg() const;
    const unbounded_octstring<true>& pdcch_blind_detection_scg() const;
    const unbounded_octstring<true>& requested_pdcch_blind_detection_scg() const;
    const unbounded_octstring<true>& ph_info_mcg() const;
    const unbounded_octstring<true>& meas_gap_sharing_cfg() const;

  private:
    types                                      type_;
    choice_buffer_t<unbounded_octstring<true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct du_to_cu_rrc_info_ext_ies_container {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // member variables
  bool                                                 drx_long_cycle_start_offset_present         = false;
  bool                                                 sel_band_combination_idx_present            = false;
  bool                                                 sel_feature_set_entry_idx_present           = false;
  bool                                                 ph_info_scg_present                         = false;
  bool                                                 requested_band_combination_idx_present      = false;
  bool                                                 requested_feature_set_entry_idx_present     = false;
  bool                                                 requested_p_max_fr2_present                 = false;
  bool                                                 drx_cfg_present                             = false;
  bool                                                 pdcch_blind_detection_scg_present           = false;
  bool                                                 requested_pdcch_blind_detection_scg_present = false;
  bool                                                 ph_info_mcg_present                         = false;
  bool                                                 meas_gap_sharing_cfg_present                = false;
  ie_field_s<integer<uint16_t, 0, 10239, false, true>> drx_long_cycle_start_offset;
  ie_field_s<unbounded_octstring<true>>                sel_band_combination_idx;
  ie_field_s<unbounded_octstring<true>>                sel_feature_set_entry_idx;
  ie_field_s<unbounded_octstring<true>>                ph_info_scg;
  ie_field_s<unbounded_octstring<true>>                requested_band_combination_idx;
  ie_field_s<unbounded_octstring<true>>                requested_feature_set_entry_idx;
  ie_field_s<unbounded_octstring<true>>                requested_p_max_fr2;
  ie_field_s<unbounded_octstring<true>>                drx_cfg;
  ie_field_s<unbounded_octstring<true>>                pdcch_blind_detection_scg;
  ie_field_s<unbounded_octstring<true>>                requested_pdcch_blind_detection_scg;
  ie_field_s<unbounded_octstring<true>>                ph_info_mcg;
  ie_field_s<unbounded_octstring<true>>                meas_gap_sharing_cfg;

  // sequence methods
  du_to_cu_rrc_info_ext_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DUtoCURRCInformation ::= SEQUENCE
struct du_to_cu_rrc_info_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  unbounded_octstring<true>           cell_group_cfg;
  unbounded_octstring<true>           meas_gap_cfg;
  unbounded_octstring<true>           requested_p_max_fr1;
  du_to_cu_rrc_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DedicatedSIDeliveryNeededUE-Item-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using ded_si_delivery_needed_ue_item_ext_ies_o = protocol_ext_empty_o;

using ded_si_delivery_needed_ue_item_ext_ies_container = protocol_ext_container_empty_l;

// Dedicated-SIDelivery-NeededUE-Item ::= SEQUENCE
struct ded_si_delivery_needed_ue_item_s {
  bool                                             ext               = false;
  bool                                             ie_exts_present   = false;
  uint64_t                                         gnb_cu_ue_f1ap_id = 0;
  nrcgi_s                                          nrcgi;
  ded_si_delivery_needed_ue_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Dedicated-SIDelivery-NeededUE-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ded_si_delivery_needed_ue_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ded_si_delivery_needed_ue_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ded_si_delivery_needed_ue_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ded_si_delivery_needed_ue_item_s&       ded_si_delivery_needed_ue_item() { return c; }
    const ded_si_delivery_needed_ue_item_s& ded_si_delivery_needed_ue_item() const { return c; }

  private:
    ded_si_delivery_needed_ue_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Dedicated-SIDelivery-NeededUE-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using ded_si_delivery_needed_ue_list_l =
    dyn_array<protocol_ie_single_container_s<ded_si_delivery_needed_ue_item_ies_o>>;

// EUTRA-Coex-FDD-Info-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using eutra_coex_fdd_info_ext_ies_o = protocol_ext_empty_o;

// EUTRA-Transmission-Bandwidth ::= ENUMERATED
struct eutra_tx_bw_opts {
  enum options { bw6, bw15, bw25, bw50, bw75, bw100, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<eutra_tx_bw_opts, true> eutra_tx_bw_e;

using eutra_coex_fdd_info_ext_ies_container = protocol_ext_container_empty_l;

// EUTRA-Coex-FDD-Info ::= SEQUENCE
struct eutra_coex_fdd_info_s {
  bool                                  ext               = false;
  bool                                  ul_earfcn_present = false;
  bool                                  ul_tx_bw_present  = false;
  bool                                  ie_exts_present   = false;
  uint32_t                              ul_earfcn         = 0;
  uint32_t                              dl_earfcn         = 0;
  eutra_tx_bw_e                         ul_tx_bw;
  eutra_tx_bw_e                         dl_tx_bw;
  eutra_coex_fdd_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-CyclicPrefixDL ::= ENUMERATED
struct eutra_cp_dl_opts {
  enum options { normal, extended, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<eutra_cp_dl_opts, true> eutra_cp_dl_e;

// EUTRA-CyclicPrefixUL ::= ENUMERATED
struct eutra_cp_ul_opts {
  enum options { normal, extended, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<eutra_cp_ul_opts, true> eutra_cp_ul_e;

// EUTRA-SpecialSubframe-Info-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using eutra_special_sf_info_ext_ies_o = protocol_ext_empty_o;

// EUTRA-SpecialSubframePatterns ::= ENUMERATED
struct eutra_special_sf_patterns_opts {
  enum options { ssp0, ssp1, ssp2, ssp3, ssp4, ssp5, ssp6, ssp7, ssp8, ssp9, ssp10, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<eutra_special_sf_patterns_opts, true> eutra_special_sf_patterns_e;

// EUTRA-Coex-TDD-Info-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using eutra_coex_tdd_info_ext_ies_o = protocol_ext_empty_o;

using eutra_special_sf_info_ext_ies_container = protocol_ext_container_empty_l;

// EUTRA-SpecialSubframe-Info ::= SEQUENCE
struct eutra_special_sf_info_s {
  bool                                    ext             = false;
  bool                                    ie_exts_present = false;
  eutra_special_sf_patterns_e             special_sf_patterns;
  eutra_cp_dl_e                           cp_dl;
  eutra_cp_ul_e                           cp_ul;
  eutra_special_sf_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-SubframeAssignment ::= ENUMERATED
struct eutra_sf_assign_opts {
  enum options { sa0, sa1, sa2, sa3, sa4, sa5, sa6, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<eutra_sf_assign_opts, true> eutra_sf_assign_e;

using eutra_coex_tdd_info_ext_ies_container = protocol_ext_container_empty_l;

// EUTRA-Coex-TDD-Info ::= SEQUENCE
struct eutra_coex_tdd_info_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  uint32_t                              earfcn          = 0;
  eutra_tx_bw_e                         tx_bw;
  eutra_sf_assign_e                     sf_assign;
  eutra_special_sf_info_s               special_sf_info;
  eutra_coex_tdd_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-Coex-Mode-Info ::= CHOICE
struct eutra_coex_mode_info_c {
  struct types_opts {
    enum options { fdd, tdd, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  eutra_coex_mode_info_c() = default;
  eutra_coex_mode_info_c(const eutra_coex_mode_info_c& other);
  eutra_coex_mode_info_c& operator=(const eutra_coex_mode_info_c& other);
  ~eutra_coex_mode_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutra_coex_fdd_info_s& fdd()
  {
    assert_choice_type(types::fdd, type_, "EUTRA-Coex-Mode-Info");
    return c.get<eutra_coex_fdd_info_s>();
  }
  eutra_coex_tdd_info_s& tdd()
  {
    assert_choice_type(types::tdd, type_, "EUTRA-Coex-Mode-Info");
    return c.get<eutra_coex_tdd_info_s>();
  }
  const eutra_coex_fdd_info_s& fdd() const
  {
    assert_choice_type(types::fdd, type_, "EUTRA-Coex-Mode-Info");
    return c.get<eutra_coex_fdd_info_s>();
  }
  const eutra_coex_tdd_info_s& tdd() const
  {
    assert_choice_type(types::tdd, type_, "EUTRA-Coex-Mode-Info");
    return c.get<eutra_coex_tdd_info_s>();
  }
  eutra_coex_fdd_info_s& set_fdd();
  eutra_coex_tdd_info_s& set_tdd();

private:
  types                                                         type_;
  choice_buffer_t<eutra_coex_fdd_info_s, eutra_coex_tdd_info_s> c;

  void destroy_();
};

// EUTRA-FDD-Info-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using eutra_fdd_info_ext_ies_o = protocol_ext_empty_o;

using eutra_fdd_info_ext_ies_container = protocol_ext_container_empty_l;

// EUTRA-FDD-Info ::= SEQUENCE
struct eutra_fdd_info_s {
  bool                             ext                  = false;
  bool                             ie_exts_present      = false;
  uint16_t                         ul_offset_to_point_a = 0;
  uint16_t                         dl_offset_to_point_a = 0;
  eutra_fdd_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-TDD-Info-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using eutra_tdd_info_ext_ies_o = protocol_ext_empty_o;

// EUTRA-Mode-Info-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
using eutra_mode_info_ext_ies_o = protocol_ies_empty_o;

using eutra_tdd_info_ext_ies_container = protocol_ext_container_empty_l;

// EUTRA-TDD-Info ::= SEQUENCE
struct eutra_tdd_info_s {
  bool                             ext               = false;
  bool                             ie_exts_present   = false;
  uint16_t                         offset_to_point_a = 0;
  eutra_tdd_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-Mode-Info ::= CHOICE
struct eutra_mode_info_c {
  struct types_opts {
    enum options { eutra_fdd, eutra_tdd, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  eutra_mode_info_c() = default;
  eutra_mode_info_c(const eutra_mode_info_c& other);
  eutra_mode_info_c& operator=(const eutra_mode_info_c& other);
  ~eutra_mode_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutra_fdd_info_s& eutra_fdd()
  {
    assert_choice_type(types::eutra_fdd, type_, "EUTRA-Mode-Info");
    return c.get<eutra_fdd_info_s>();
  }
  eutra_tdd_info_s& eutra_tdd()
  {
    assert_choice_type(types::eutra_tdd, type_, "EUTRA-Mode-Info");
    return c.get<eutra_tdd_info_s>();
  }
  protocol_ie_single_container_s<eutra_mode_info_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "EUTRA-Mode-Info");
    return c.get<protocol_ie_single_container_s<eutra_mode_info_ext_ies_o>>();
  }
  const eutra_fdd_info_s& eutra_fdd() const
  {
    assert_choice_type(types::eutra_fdd, type_, "EUTRA-Mode-Info");
    return c.get<eutra_fdd_info_s>();
  }
  const eutra_tdd_info_s& eutra_tdd() const
  {
    assert_choice_type(types::eutra_tdd, type_, "EUTRA-Mode-Info");
    return c.get<eutra_tdd_info_s>();
  }
  const protocol_ie_single_container_s<eutra_mode_info_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "EUTRA-Mode-Info");
    return c.get<protocol_ie_single_container_s<eutra_mode_info_ext_ies_o>>();
  }
  eutra_fdd_info_s&                                          set_eutra_fdd();
  eutra_tdd_info_s&                                          set_eutra_tdd();
  protocol_ie_single_container_s<eutra_mode_info_ext_ies_o>& set_choice_ext();

private:
  types                                                                                                          type_;
  choice_buffer_t<eutra_fdd_info_s, eutra_tdd_info_s, protocol_ie_single_container_s<eutra_mode_info_ext_ies_o>> c;

  void destroy_();
};

// EUTRA-PRACH-Configuration-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using eutra_prach_cfg_ext_ies_o = protocol_ext_empty_o;

using eutra_prach_cfg_ext_ies_container = protocol_ext_container_empty_l;

// EUTRA-PRACH-Configuration ::= SEQUENCE
struct eutra_prach_cfg_s {
  bool                              ext                   = false;
  bool                              prach_cfg_idx_present = false;
  bool                              ie_exts_present       = false;
  uint16_t                          root_seq_idx          = 0;
  uint8_t                           zero_correlation_idx  = 0;
  bool                              high_speed_flag       = false;
  uint8_t                           prach_freq_offset     = 0;
  uint8_t                           prach_cfg_idx         = 0;
  eutra_prach_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Served-EUTRA-Cell-Information-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using served_eutra_cell_info_ext_ies_o = protocol_ext_empty_o;

// EUTRACells-List-itemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using eutra_cells_list_item_ext_ies_o = protocol_ext_empty_o;

using served_eutra_cell_info_ext_ies_container = protocol_ext_container_empty_l;

// Served-EUTRA-Cells-Information ::= SEQUENCE
struct served_eutra_cells_info_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  eutra_mode_info_c                        eutra_mode_info;
  unbounded_octstring<true>                protected_eutra_res_ind;
  served_eutra_cell_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using eutra_cells_list_item_ext_ies_container = protocol_ext_container_empty_l;

// EUTRACells-List-item ::= SEQUENCE
struct eutra_cells_list_item_s {
  bool                                    ie_exts_present = false;
  fixed_bitstring<28, false, true>        eutra_cell_id;
  served_eutra_cells_info_s               served_eutra_cells_info;
  eutra_cells_list_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRACells-List ::= SEQUENCE (SIZE (1..256)) OF EUTRACells-List-item
using eutra_cells_list_l = dyn_array<eutra_cells_list_item_s>;

// ErrorIndicationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct error_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_ue_f1ap_id();
    uint64_t&                 gnb_du_ue_f1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const uint64_t&           gnb_cu_ue_f1ap_id() const;
    const uint64_t&           gnb_du_ue_f1ap_id() const;
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

struct error_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      gnb_cu_ue_f1ap_id_present = false;
  bool                                                      gnb_du_ue_f1ap_id_present = false;
  bool                                                      cause_present             = false;
  bool                                                      crit_diagnostics_present  = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>         transaction_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<cause_c>                                       cause;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  error_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ErrorIndication ::= SEQUENCE
using error_ind_s = elementary_procedure_option<error_ind_ies_container>;

// SliceSupportItem-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using slice_support_item_ext_ies_o = protocol_ext_empty_o;

using slice_support_item_ext_ies_container = protocol_ext_container_empty_l;

// SliceSupportItem ::= SEQUENCE
struct slice_support_item_s {
  bool                                 ie_exts_present = false;
  snssai_s                             snssai;
  slice_support_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ExtendedServedPLMNs-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using extended_served_plmns_item_ext_ies_o = protocol_ext_empty_o;

// SliceSupportList ::= SEQUENCE (SIZE (1..1024)) OF SliceSupportItem
using slice_support_list_l = dyn_array<slice_support_item_s>;

using extended_served_plmns_item_ext_ies_container = protocol_ext_container_empty_l;

// ExtendedServedPLMNs-Item ::= SEQUENCE
struct extended_served_plmns_item_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  fixed_octstring<3, true>                     plmn_id;
  slice_support_list_l                         tai_slice_support_list;
  extended_served_plmns_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ExtendedServedPLMNs-List ::= SEQUENCE (SIZE (1..6)) OF ExtendedServedPLMNs-Item
using extended_served_plmns_list_l = dyn_array<extended_served_plmns_item_s>;

// NRNRB ::= ENUMERATED
struct nrnrb_opts {
  enum options {
    nrb11,
    nrb18,
    nrb24,
    nrb25,
    nrb31,
    nrb32,
    nrb38,
    nrb51,
    nrb52,
    nrb65,
    nrb66,
    nrb78,
    nrb79,
    nrb93,
    nrb106,
    nrb107,
    nrb121,
    nrb132,
    nrb133,
    nrb135,
    nrb160,
    nrb162,
    nrb189,
    nrb216,
    nrb217,
    nrb245,
    nrb264,
    nrb270,
    nrb273,
    // ...
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<nrnrb_opts, true> nrnrb_e;

// NRSCS ::= ENUMERATED
struct nrscs_opts {
  enum options { scs15, scs30, scs60, scs120, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<nrscs_opts, true> nrscs_e;

// SupportedSULFreqBandItem-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using supported_sul_freq_band_item_ext_ies_o = protocol_ext_empty_o;

// Transmission-Bandwidth-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using tx_bw_ext_ies_o = protocol_ext_empty_o;

// FreqBandNrItem-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using freq_band_nr_item_ext_ies_o = protocol_ext_empty_o;

// SUL-InformationExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using sul_info_ext_ies_o = protocol_ext_empty_o;

using supported_sul_freq_band_item_ext_ies_container = protocol_ext_container_empty_l;

// SupportedSULFreqBandItem ::= SEQUENCE
struct supported_sul_freq_band_item_s {
  bool                                           ext              = false;
  bool                                           ie_exts_present  = false;
  uint16_t                                       freq_band_ind_nr = 1;
  supported_sul_freq_band_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using tx_bw_ext_ies_container = protocol_ext_container_empty_l;

// Transmission-Bandwidth ::= SEQUENCE
struct tx_bw_s {
  bool                    ext             = false;
  bool                    ie_exts_present = false;
  nrscs_e                 nrscs;
  nrnrb_e                 nr_nrb;
  tx_bw_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using freq_band_nr_item_ext_ies_container = protocol_ext_container_empty_l;

// FreqBandNrItem ::= SEQUENCE
struct freq_band_nr_item_s {
  using supported_sul_band_list_l_ = dyn_array<supported_sul_freq_band_item_s>;

  // member variables
  bool                                ext              = false;
  bool                                ie_exts_present  = false;
  uint16_t                            freq_band_ind_nr = 1;
  supported_sul_band_list_l_          supported_sul_band_list;
  freq_band_nr_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NRFreqInfoExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using nr_freq_info_ext_ies_o = protocol_ext_empty_o;

using sul_info_ext_ies_container = protocol_ext_container_empty_l;

// SUL-Information ::= SEQUENCE
struct sul_info_s {
  bool                       ext             = false;
  bool                       ie_exts_present = false;
  uint32_t                   sul_nr_arfcn    = 0;
  tx_bw_s                    sul_tx_bw;
  sul_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UACOperatorDefined-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using uac_operator_defined_ext_ies_o = protocol_ext_empty_o;

// FDD-Info-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using fdd_info_ext_ies_o = protocol_ext_empty_o;

using nr_freq_info_ext_ies_container = protocol_ext_container_empty_l;

// NRFreqInfo ::= SEQUENCE
struct nr_freq_info_s {
  using freq_band_list_nr_l_ = dyn_array<freq_band_nr_item_s>;

  // member variables
  bool                           ext              = false;
  bool                           sul_info_present = false;
  bool                           ie_exts_present  = false;
  uint32_t                       nr_arfcn         = 0;
  sul_info_s                     sul_info;
  freq_band_list_nr_l_           freq_band_list_nr;
  nr_freq_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServedPLMNs-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct served_plmns_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { tai_slice_support_list, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::tai_slice_support_list; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    slice_support_list_l&       tai_slice_support_list() { return c; }
    const slice_support_list_l& tai_slice_support_list() const { return c; }

  private:
    slice_support_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TDD-Info-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using tdd_info_ext_ies_o = protocol_ext_empty_o;

// UACAction ::= ENUMERATED
struct uac_action_opts {
  enum options {
    reject_non_emergency_mo_dt,
    reject_rrc_cr_sig,
    permit_emergency_sessions_and_mobile_terminated_services_only,
    permit_high_prio_sessions_and_mobile_terminated_services_only,
    // ...
    nulltype
  } value;

  const char* to_string() const;
};
typedef enumerated<uac_action_opts, true> uac_action_e;

// UACCategoryType-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
using uac_category_type_ext_ies_o = protocol_ies_empty_o;

using uac_operator_defined_ext_ies_container = protocol_ext_container_empty_l;

// UACOperatorDefined ::= SEQUENCE
struct uac_operator_defined_s {
  bool                                   ie_exts_present = false;
  uint8_t                                access_category = 32;
  fixed_bitstring<7, false, true>        access_id;
  uac_operator_defined_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Cell-Direction ::= ENUMERATED
struct cell_direction_opts {
  enum options { dl_only, ul_only, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<cell_direction_opts> cell_direction_e;

using fdd_info_ext_ies_container = protocol_ext_container_empty_l;

// FDD-Info ::= SEQUENCE
struct fdd_info_s {
  bool                       ext             = false;
  bool                       ie_exts_present = false;
  nr_freq_info_s             ul_nr_freq_info;
  nr_freq_info_s             dl_nr_freq_info;
  tx_bw_s                    ul_tx_bw;
  tx_bw_s                    dl_tx_bw;
  fdd_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-Mode-Info-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
using nr_mode_info_ext_ies_o = protocol_ies_empty_o;

// ServedPLMNs-Item ::= SEQUENCE
struct served_plmns_item_s {
  bool                                                  ext = false;
  fixed_octstring<3, true>                              plmn_id;
  protocol_ext_container_l<served_plmns_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using tdd_info_ext_ies_container = protocol_ext_container_empty_l;

// TDD-Info ::= SEQUENCE
struct tdd_info_s {
  bool                       ext             = false;
  bool                       ie_exts_present = false;
  nr_freq_info_s             nr_freq_info;
  tx_bw_s                    tx_bw;
  tdd_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UACCategoryType ::= CHOICE
struct uac_category_type_c {
  struct types_opts {
    enum options { uacstandardized, uac_operator_defined, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  uac_category_type_c() = default;
  uac_category_type_c(const uac_category_type_c& other);
  uac_category_type_c& operator=(const uac_category_type_c& other);
  ~uac_category_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uac_action_e& uacstandardized()
  {
    assert_choice_type(types::uacstandardized, type_, "UACCategoryType");
    return c.get<uac_action_e>();
  }
  uac_operator_defined_s& uac_operator_defined()
  {
    assert_choice_type(types::uac_operator_defined, type_, "UACCategoryType");
    return c.get<uac_operator_defined_s>();
  }
  protocol_ie_single_container_s<uac_category_type_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "UACCategoryType");
    return c.get<protocol_ie_single_container_s<uac_category_type_ext_ies_o>>();
  }
  const uac_action_e& uacstandardized() const
  {
    assert_choice_type(types::uacstandardized, type_, "UACCategoryType");
    return c.get<uac_action_e>();
  }
  const uac_operator_defined_s& uac_operator_defined() const
  {
    assert_choice_type(types::uac_operator_defined, type_, "UACCategoryType");
    return c.get<uac_operator_defined_s>();
  }
  const protocol_ie_single_container_s<uac_category_type_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "UACCategoryType");
    return c.get<protocol_ie_single_container_s<uac_category_type_ext_ies_o>>();
  }
  uac_action_e&                                                set_uacstandardized();
  uac_operator_defined_s&                                      set_uac_operator_defined();
  protocol_ie_single_container_s<uac_category_type_ext_ies_o>& set_choice_ext();

private:
  types                                                                                                type_;
  choice_buffer_t<protocol_ie_single_container_s<uac_category_type_ext_ies_o>, uac_operator_defined_s> c;

  void destroy_();
};

// UACType-Item-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using uac_type_item_ext_ies_o = protocol_ext_empty_o;

// GNB-DU-System-Information-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using gnb_du_sys_info_ext_ies_o = protocol_ext_empty_o;

// NR-Mode-Info ::= CHOICE
struct nr_mode_info_c {
  struct types_opts {
    enum options { fdd, tdd, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  nr_mode_info_c() = default;
  nr_mode_info_c(const nr_mode_info_c& other);
  nr_mode_info_c& operator=(const nr_mode_info_c& other);
  ~nr_mode_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fdd_info_s& fdd()
  {
    assert_choice_type(types::fdd, type_, "NR-Mode-Info");
    return c.get<fdd_info_s>();
  }
  tdd_info_s& tdd()
  {
    assert_choice_type(types::tdd, type_, "NR-Mode-Info");
    return c.get<tdd_info_s>();
  }
  protocol_ie_single_container_s<nr_mode_info_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "NR-Mode-Info");
    return c.get<protocol_ie_single_container_s<nr_mode_info_ext_ies_o>>();
  }
  const fdd_info_s& fdd() const
  {
    assert_choice_type(types::fdd, type_, "NR-Mode-Info");
    return c.get<fdd_info_s>();
  }
  const tdd_info_s& tdd() const
  {
    assert_choice_type(types::tdd, type_, "NR-Mode-Info");
    return c.get<tdd_info_s>();
  }
  const protocol_ie_single_container_s<nr_mode_info_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "NR-Mode-Info");
    return c.get<protocol_ie_single_container_s<nr_mode_info_ext_ies_o>>();
  }
  fdd_info_s&                                             set_fdd();
  tdd_info_s&                                             set_tdd();
  protocol_ie_single_container_s<nr_mode_info_ext_ies_o>& set_choice_ext();

private:
  types                                                                                           type_;
  choice_buffer_t<fdd_info_s, protocol_ie_single_container_s<nr_mode_info_ext_ies_o>, tdd_info_s> c;

  void destroy_();
};

// Served-Cell-Information-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct served_cell_info_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { ranac, extended_served_plmns_list, cell_direction, bplmn_id_info_list, cell_type, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                           ranac();
    extended_served_plmns_list_l&       extended_served_plmns_list();
    cell_direction_e&                   cell_direction();
    bplmn_id_info_list_l&               bplmn_id_info_list();
    cell_type_s&                        cell_type();
    const uint16_t&                     ranac() const;
    const extended_served_plmns_list_l& extended_served_plmns_list() const;
    const cell_direction_e&             cell_direction() const;
    const bplmn_id_info_list_l&         bplmn_id_info_list() const;
    const cell_type_s&                  cell_type() const;

  private:
    types                                                                            type_;
    choice_buffer_t<bplmn_id_info_list_l, cell_type_s, extended_served_plmns_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ServedPLMNs-List ::= SEQUENCE (SIZE (1..6)) OF ServedPLMNs-Item
using served_plmns_list_l = dyn_array<served_plmns_item_s>;

using uac_type_item_ext_ies_container = protocol_ext_container_empty_l;

// UACType-Item ::= SEQUENCE
struct uac_type_item_s {
  bool                            ie_exts_present   = false;
  uint8_t                         uac_reduction_ind = 0;
  uac_category_type_c             uac_category_type;
  uac_type_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-associatedLogicalF1-ConnectionItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using ue_associated_lc_f1_conn_item_ext_ies_o = protocol_ext_empty_o;

// CellULConfigured ::= ENUMERATED
struct cell_ul_cfgured_opts {
  enum options { none, ul, sul, ul_and_sul, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<cell_ul_cfgured_opts, true> cell_ul_cfgured_e;

// DuplicationIndication ::= ENUMERATED
struct dupl_ind_opts {
  enum options { true_value, /*...*/ false_value, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<dupl_ind_opts, true, 1> dupl_ind_e;

// GNB-CU-TNL-Association-Failed-To-Setup-Item-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using gnb_cu_tnl_assoc_failed_to_setup_item_ext_ies_o = protocol_ext_empty_o;

// GNB-CU-TNL-Association-Setup-Item-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using gnb_cu_tnl_assoc_setup_item_ext_ies_o = protocol_ext_empty_o;

// GNB-CU-TNL-Association-To-Add-Item-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using gnb_cu_tnl_assoc_to_add_item_ext_ies_o = protocol_ext_empty_o;

// GNB-CU-TNL-Association-To-Remove-Item-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct gnb_cu_tnl_assoc_to_rem_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { tnl_assoc_transport_layer_address_gnb_du, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::tnl_assoc_transport_layer_address_gnb_du; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cp_transport_layer_address_c&       tnl_assoc_transport_layer_address_gnb_du() { return c; }
    const cp_transport_layer_address_c& tnl_assoc_transport_layer_address_gnb_du() const { return c; }

  private:
    cp_transport_layer_address_c c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CU-TNL-Association-To-Update-Item-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using gnb_cu_tnl_assoc_to_upd_item_ext_ies_o = protocol_ext_empty_o;

// GNB-DU-Served-Cells-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using gnb_du_served_cells_item_ext_ies_o = protocol_ext_empty_o;

using gnb_du_sys_info_ext_ies_container = protocol_ext_container_empty_l;

// GNB-DU-System-Information ::= SEQUENCE
struct gnb_du_sys_info_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  unbounded_octstring<true>         mib_msg;
  unbounded_octstring<true>         sib1_msg;
  gnb_du_sys_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-DU-TNL-Association-To-Remove-Item-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using gnb_du_tnl_assoc_to_rem_item_ext_ies_o = protocol_ext_empty_o;

// IgnorePRACHConfiguration ::= ENUMERATED
struct ignore_prach_cfg_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<ignore_prach_cfg_opts, true> ignore_prach_cfg_e;

// NR-CGI-List-For-Restart-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using nr_cgi_list_for_restart_item_ext_ies_o = protocol_ext_empty_o;

// NotificationInformationExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using notif_info_ext_ies_o = protocol_ext_empty_o;

// PWS-Failed-NR-CGI-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using pws_failed_nr_cgi_item_ext_ies_o = protocol_ext_empty_o;

// PagingCell-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using paging_cell_item_ext_ies_o = protocol_ext_empty_o;

// Potential-SpCell-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using potential_sp_cell_item_ext_ies_o = protocol_ext_empty_o;

// Protected-EUTRA-Resources-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using protected_eutra_res_item_ext_ies_o = protocol_ext_empty_o;

// SCell-FailedtoSetup-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using scell_failedto_setup_item_ext_ies_o = protocol_ext_empty_o;

// SCell-FailedtoSetupMod-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using scell_failedto_setup_mod_item_ext_ies_o = protocol_ext_empty_o;

// SCell-ToBeRemoved-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using scell_to_be_remd_item_ext_ies_o = protocol_ext_empty_o;

// SCell-ToBeSetup-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct scell_to_be_setup_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { serving_cell_mo, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::serving_cell_mo; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t&       serving_cell_mo() { return c; }
    const uint8_t& serving_cell_mo() const { return c; }

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

// SCell-ToBeSetupMod-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct scell_to_be_setup_mod_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { serving_cell_mo, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::serving_cell_mo; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t&       serving_cell_mo() { return c; }
    const uint8_t& serving_cell_mo() const { return c; }

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

// SRBs-FailedToBeSetup-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using srbs_failed_to_be_setup_item_ext_ies_o = protocol_ext_empty_o;

// SRBs-FailedToBeSetupMod-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using srbs_failed_to_be_setup_mod_item_ext_ies_o = protocol_ext_empty_o;

// SRBs-Modified-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using srbs_modified_item_ext_ies_o = protocol_ext_empty_o;

// SRBs-Required-ToBeReleased-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using srbs_required_to_be_released_item_ext_ies_o = protocol_ext_empty_o;

// SRBs-Setup-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using srbs_setup_item_ext_ies_o = protocol_ext_empty_o;

// SRBs-SetupMod-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using srbs_setup_mod_item_ext_ies_o = protocol_ext_empty_o;

// SRBs-ToBeReleased-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using srbs_to_be_released_item_ext_ies_o = protocol_ext_empty_o;

// SRBs-ToBeSetup-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using srbs_to_be_setup_item_ext_ies_o = protocol_ext_empty_o;

// SRBs-ToBeSetupMod-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using srbs_to_be_setup_mod_item_ext_ies_o = protocol_ext_empty_o;

struct served_cell_info_ext_ies_container {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // member variables
  bool                                                             ranac_present                      = false;
  bool                                                             extended_served_plmns_list_present = false;
  bool                                                             cell_direction_present             = false;
  bool                                                             bplmn_id_info_list_present         = false;
  bool                                                             cell_type_present                  = false;
  ie_field_s<integer<uint16_t, 0, 255, false, true>>               ranac;
  ie_field_s<dyn_seq_of<extended_served_plmns_item_s, 1, 6, true>> extended_served_plmns_list;
  ie_field_s<cell_direction_e>                                     cell_direction;
  ie_field_s<dyn_seq_of<bplmn_id_info_item_s, 1, 11, true>>        bplmn_id_info_list;
  ie_field_s<cell_type_s>                                          cell_type;

  // sequence methods
  served_cell_info_ext_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Served-Cell-Information ::= SEQUENCE
struct served_cell_info_s {
  bool                               ext                     = false;
  bool                               five_gs_tac_present     = false;
  bool                               cfgured_eps_tac_present = false;
  bool                               ie_exts_present         = false;
  nrcgi_s                            nrcgi;
  uint16_t                           nr_pci = 0;
  fixed_octstring<3, true>           five_gs_tac;
  fixed_octstring<2, true>           cfgured_eps_tac;
  served_plmns_list_l                served_plmns;
  nr_mode_info_c                     nr_mode_info;
  unbounded_octstring<true>          meas_timing_cfg;
  served_cell_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Served-Cells-To-Add-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using served_cells_to_add_item_ext_ies_o = protocol_ext_empty_o;

// Served-Cells-To-Delete-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using served_cells_to_delete_item_ext_ies_o = protocol_ext_empty_o;

// Served-Cells-To-Modify-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using served_cells_to_modify_item_ext_ies_o = protocol_ext_empty_o;

// TNLAssociationUsage ::= ENUMERATED
struct tnl_assoc_usage_opts {
  enum options { ue, non_ue, both, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<tnl_assoc_usage_opts, true> tnl_assoc_usage_e;

// UACPLMN-Item-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using uac_plmn_item_ext_ies_o = protocol_ext_empty_o;

// UACType-List ::= SEQUENCE (SIZE (1..64)) OF UACType-Item
using uac_type_list_l = dyn_array<uac_type_item_s>;

using ue_associated_lc_f1_conn_item_ext_ies_container = protocol_ext_container_empty_l;

// UE-associatedLogicalF1-ConnectionItem ::= SEQUENCE
struct ue_associated_lc_f1_conn_item_s {
  bool                                            ext                       = false;
  bool                                            gnb_cu_ue_f1ap_id_present = false;
  bool                                            gnb_du_ue_f1ap_id_present = false;
  bool                                            ie_exts_present           = false;
  uint64_t                                        gnb_cu_ue_f1ap_id         = 0;
  uint64_t                                        gnb_du_ue_f1ap_id         = 0;
  ue_associated_lc_f1_conn_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using gnb_cu_tnl_assoc_failed_to_setup_item_ext_ies_container = protocol_ext_container_empty_l;

// GNB-CU-TNL-Association-Failed-To-Setup-Item ::= SEQUENCE
struct gnb_cu_tnl_assoc_failed_to_setup_item_s {
  bool                                                    ie_exts_present = false;
  cp_transport_layer_address_c                            tnl_assoc_transport_layer_address;
  cause_c                                                 cause;
  gnb_cu_tnl_assoc_failed_to_setup_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using gnb_cu_tnl_assoc_setup_item_ext_ies_container = protocol_ext_container_empty_l;

// GNB-CU-TNL-Association-Setup-Item ::= SEQUENCE
struct gnb_cu_tnl_assoc_setup_item_s {
  bool                                          ie_exts_present = false;
  cp_transport_layer_address_c                  tnl_assoc_transport_layer_address;
  gnb_cu_tnl_assoc_setup_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using gnb_cu_tnl_assoc_to_add_item_ext_ies_container = protocol_ext_container_empty_l;

// GNB-CU-TNL-Association-To-Add-Item ::= SEQUENCE
struct gnb_cu_tnl_assoc_to_add_item_s {
  bool                                           ie_exts_present = false;
  cp_transport_layer_address_c                   tnl_assoc_transport_layer_address;
  tnl_assoc_usage_e                              tnl_assoc_usage;
  gnb_cu_tnl_assoc_to_add_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-TNL-Association-To-Remove-Item ::= SEQUENCE
struct gnb_cu_tnl_assoc_to_rem_item_s {
  cp_transport_layer_address_c                                     tnl_assoc_transport_layer_address;
  protocol_ext_container_l<gnb_cu_tnl_assoc_to_rem_item_ext_ies_o> ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using gnb_cu_tnl_assoc_to_upd_item_ext_ies_container = protocol_ext_container_empty_l;

// GNB-CU-TNL-Association-To-Update-Item ::= SEQUENCE
struct gnb_cu_tnl_assoc_to_upd_item_s {
  bool                                           tnl_assoc_usage_present = false;
  bool                                           ie_exts_present         = false;
  cp_transport_layer_address_c                   tnl_assoc_transport_layer_address;
  tnl_assoc_usage_e                              tnl_assoc_usage;
  gnb_cu_tnl_assoc_to_upd_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using gnb_du_served_cells_item_ext_ies_container = protocol_ext_container_empty_l;

// GNB-DU-Served-Cells-Item ::= SEQUENCE
struct gnb_du_served_cells_item_s {
  bool                                       ext                     = false;
  bool                                       gnb_du_sys_info_present = false;
  bool                                       ie_exts_present         = false;
  served_cell_info_s                         served_cell_info;
  gnb_du_sys_info_s                          gnb_du_sys_info;
  gnb_du_served_cells_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using gnb_du_tnl_assoc_to_rem_item_ext_ies_container = protocol_ext_container_empty_l;

// GNB-DU-TNL-Association-To-Remove-Item ::= SEQUENCE
struct gnb_du_tnl_assoc_to_rem_item_s {
  bool                                           tnl_assoc_transport_layer_address_gnb_cu_present = false;
  bool                                           ie_exts_present                                  = false;
  cp_transport_layer_address_c                   tnl_assoc_transport_layer_address;
  cp_transport_layer_address_c                   tnl_assoc_transport_layer_address_gnb_cu;
  gnb_du_tnl_assoc_to_rem_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using nr_cgi_list_for_restart_item_ext_ies_container = protocol_ext_container_empty_l;

// NR-CGI-List-For-Restart-Item ::= SEQUENCE
struct nr_cgi_list_for_restart_item_s {
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  nrcgi_s                                        nrcgi;
  nr_cgi_list_for_restart_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using notif_info_ext_ies_container = protocol_ext_container_empty_l;

// NotificationInformation ::= SEQUENCE
struct notif_info_s {
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  fixed_bitstring<16, false, true> msg_id;
  fixed_bitstring<16, false, true> serial_num;
  notif_info_ext_ies_container     ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pws_failed_nr_cgi_item_ext_ies_container = protocol_ext_container_empty_l;

// PWS-Failed-NR-CGI-Item ::= SEQUENCE
struct pws_failed_nr_cgi_item_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  nrcgi_s                                  nrcgi;
  uint32_t                                 nof_broadcasts = 0;
  pws_failed_nr_cgi_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using paging_cell_item_ext_ies_container = protocol_ext_container_empty_l;

// PagingCell-Item ::= SEQUENCE
struct paging_cell_item_s {
  bool                               ie_exts_present = false;
  nrcgi_s                            nrcgi;
  paging_cell_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using potential_sp_cell_item_ext_ies_container = protocol_ext_container_empty_l;

// Potential-SpCell-Item ::= SEQUENCE
struct potential_sp_cell_item_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  nrcgi_s                                  potential_sp_cell_id;
  potential_sp_cell_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PrivateIE-ID ::= CHOICE
struct private_ie_id_c {
  struct types_opts {
    enum options { local, global, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

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

using protected_eutra_res_item_ext_ies_container = protocol_ext_container_empty_l;

// Protected-EUTRA-Resources-Item ::= SEQUENCE
struct protected_eutra_res_item_s {
  bool                                       ie_exts_present       = false;
  uint16_t                                   spec_sharing_group_id = 1;
  eutra_cells_list_l                         eutra_cells_list;
  protected_eutra_res_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANUEPagingIdentity-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using ran_ue_paging_id_ext_ies_o = protocol_ext_empty_o;

// ResourceCoordinationEUTRACellInfo-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct res_coordination_eutra_cell_info_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { ignore_prach_cfg, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ignore_prach_cfg; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ignore_prach_cfg_e&       ignore_prach_cfg() { return c; }
    const ignore_prach_cfg_e& ignore_prach_cfg() const { return c; }

  private:
    ignore_prach_cfg_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using scell_failedto_setup_item_ext_ies_container = protocol_ext_container_empty_l;

// SCell-FailedtoSetup-Item ::= SEQUENCE
struct scell_failedto_setup_item_s {
  bool                                        ext             = false;
  bool                                        cause_present   = false;
  bool                                        ie_exts_present = false;
  nrcgi_s                                     scell_id;
  cause_c                                     cause;
  scell_failedto_setup_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using scell_failedto_setup_mod_item_ext_ies_container = protocol_ext_container_empty_l;

// SCell-FailedtoSetupMod-Item ::= SEQUENCE
struct scell_failedto_setup_mod_item_s {
  bool                                            ext             = false;
  bool                                            cause_present   = false;
  bool                                            ie_exts_present = false;
  nrcgi_s                                         scell_id;
  cause_c                                         cause;
  scell_failedto_setup_mod_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using scell_to_be_remd_item_ext_ies_container = protocol_ext_container_empty_l;

// SCell-ToBeRemoved-Item ::= SEQUENCE
struct scell_to_be_remd_item_s {
  bool                                    ext             = false;
  bool                                    ie_exts_present = false;
  nrcgi_s                                 scell_id;
  scell_to_be_remd_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCell-ToBeSetup-Item ::= SEQUENCE
struct scell_to_be_setup_item_s {
  bool                                                       ext                      = false;
  bool                                                       scell_ul_cfgured_present = false;
  nrcgi_s                                                    scell_id;
  uint8_t                                                    scell_idx = 1;
  cell_ul_cfgured_e                                          scell_ul_cfgured;
  protocol_ext_container_l<scell_to_be_setup_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCell-ToBeSetupMod-Item ::= SEQUENCE
struct scell_to_be_setup_mod_item_s {
  bool                                                           ext                      = false;
  bool                                                           scell_ul_cfgured_present = false;
  nrcgi_s                                                        scell_id;
  uint8_t                                                        scell_idx = 1;
  cell_ul_cfgured_e                                              scell_ul_cfgured;
  protocol_ext_container_l<scell_to_be_setup_mod_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SItype-ItemExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using sitype_item_ext_ies_o = protocol_ext_empty_o;

using srbs_failed_to_be_setup_item_ext_ies_container = protocol_ext_container_empty_l;

// SRBs-FailedToBeSetup-Item ::= SEQUENCE
struct srbs_failed_to_be_setup_item_s {
  bool                                           ext             = false;
  bool                                           cause_present   = false;
  bool                                           ie_exts_present = false;
  uint8_t                                        srb_id          = 0;
  cause_c                                        cause;
  srbs_failed_to_be_setup_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using srbs_failed_to_be_setup_mod_item_ext_ies_container = protocol_ext_container_empty_l;

// SRBs-FailedToBeSetupMod-Item ::= SEQUENCE
struct srbs_failed_to_be_setup_mod_item_s {
  bool                                               ext             = false;
  bool                                               cause_present   = false;
  bool                                               ie_exts_present = false;
  uint8_t                                            srb_id          = 0;
  cause_c                                            cause;
  srbs_failed_to_be_setup_mod_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using srbs_modified_item_ext_ies_container = protocol_ext_container_empty_l;

// SRBs-Modified-Item ::= SEQUENCE
struct srbs_modified_item_s {
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  uint8_t                              srb_id          = 0;
  uint8_t                              lcid            = 1;
  srbs_modified_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using srbs_required_to_be_released_item_ext_ies_container = protocol_ext_container_empty_l;

// SRBs-Required-ToBeReleased-Item ::= SEQUENCE
struct srbs_required_to_be_released_item_s {
  bool                                                ext             = false;
  bool                                                ie_exts_present = false;
  uint8_t                                             srb_id          = 0;
  srbs_required_to_be_released_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using srbs_setup_item_ext_ies_container = protocol_ext_container_empty_l;

// SRBs-Setup-Item ::= SEQUENCE
struct srbs_setup_item_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  uint8_t                           srb_id          = 0;
  uint8_t                           lcid            = 1;
  srbs_setup_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using srbs_setup_mod_item_ext_ies_container = protocol_ext_container_empty_l;

// SRBs-SetupMod-Item ::= SEQUENCE
struct srbs_setup_mod_item_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  uint8_t                               srb_id          = 0;
  uint8_t                               lcid            = 1;
  srbs_setup_mod_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using srbs_to_be_released_item_ext_ies_container = protocol_ext_container_empty_l;

// SRBs-ToBeReleased-Item ::= SEQUENCE
struct srbs_to_be_released_item_s {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  uint8_t                                    srb_id          = 0;
  srbs_to_be_released_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using srbs_to_be_setup_item_ext_ies_container = protocol_ext_container_empty_l;

// SRBs-ToBeSetup-Item ::= SEQUENCE
struct srbs_to_be_setup_item_s {
  bool                                    ext              = false;
  bool                                    dupl_ind_present = false;
  bool                                    ie_exts_present  = false;
  uint8_t                                 srb_id           = 0;
  dupl_ind_e                              dupl_ind;
  srbs_to_be_setup_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using srbs_to_be_setup_mod_item_ext_ies_container = protocol_ext_container_empty_l;

// SRBs-ToBeSetupMod-Item ::= SEQUENCE
struct srbs_to_be_setup_mod_item_s {
  bool                                        ext              = false;
  bool                                        dupl_ind_present = false;
  bool                                        ie_exts_present  = false;
  uint8_t                                     srb_id           = 0;
  dupl_ind_e                                  dupl_ind;
  srbs_to_be_setup_mod_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using served_cells_to_add_item_ext_ies_container = protocol_ext_container_empty_l;

// Served-Cells-To-Add-Item ::= SEQUENCE
struct served_cells_to_add_item_s {
  bool                                       ext                     = false;
  bool                                       gnb_du_sys_info_present = false;
  bool                                       ie_exts_present         = false;
  served_cell_info_s                         served_cell_info;
  gnb_du_sys_info_s                          gnb_du_sys_info;
  served_cells_to_add_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using served_cells_to_delete_item_ext_ies_container = protocol_ext_container_empty_l;

// Served-Cells-To-Delete-Item ::= SEQUENCE
struct served_cells_to_delete_item_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  nrcgi_s                                       old_nr_cgi;
  served_cells_to_delete_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using served_cells_to_modify_item_ext_ies_container = protocol_ext_container_empty_l;

// Served-Cells-To-Modify-Item ::= SEQUENCE
struct served_cells_to_modify_item_s {
  bool                                          ext                     = false;
  bool                                          gnb_du_sys_info_present = false;
  bool                                          ie_exts_present         = false;
  nrcgi_s                                       old_nr_cgi;
  served_cell_info_s                            served_cell_info;
  gnb_du_sys_info_s                             gnb_du_sys_info;
  served_cells_to_modify_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using uac_plmn_item_ext_ies_container = protocol_ext_container_empty_l;

// UACPLMN-Item ::= SEQUENCE
struct uac_plmn_item_s {
  bool                            ie_exts_present = false;
  fixed_octstring<3, true>        plmn_id;
  uac_type_list_l                 uac_type_list;
  uac_plmn_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-associatedLogicalF1-ConnectionItemRes ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_associated_lc_f1_conn_item_res_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_associated_lc_f1_conn_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ue_associated_lc_f1_conn_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_associated_lc_f1_conn_item_s&       ue_associated_lc_f1_conn_item() { return c; }
    const ue_associated_lc_f1_conn_item_s& ue_associated_lc_f1_conn_item() const { return c; }

  private:
    ue_associated_lc_f1_conn_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CU-TNL-Association-Failed-To-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_tnl_assoc_failed_to_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_tnl_assoc_failed_to_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::gnb_cu_tnl_assoc_failed_to_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_cu_tnl_assoc_failed_to_setup_item_s&       gnb_cu_tnl_assoc_failed_to_setup_item() { return c; }
    const gnb_cu_tnl_assoc_failed_to_setup_item_s& gnb_cu_tnl_assoc_failed_to_setup_item() const { return c; }

  private:
    gnb_cu_tnl_assoc_failed_to_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CU-TNL-Association-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_tnl_assoc_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_tnl_assoc_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::gnb_cu_tnl_assoc_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_cu_tnl_assoc_setup_item_s&       gnb_cu_tnl_assoc_setup_item() { return c; }
    const gnb_cu_tnl_assoc_setup_item_s& gnb_cu_tnl_assoc_setup_item() const { return c; }

  private:
    gnb_cu_tnl_assoc_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CU-TNL-Association-To-Add-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_tnl_assoc_to_add_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_tnl_assoc_to_add_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::gnb_cu_tnl_assoc_to_add_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_cu_tnl_assoc_to_add_item_s&       gnb_cu_tnl_assoc_to_add_item() { return c; }
    const gnb_cu_tnl_assoc_to_add_item_s& gnb_cu_tnl_assoc_to_add_item() const { return c; }

  private:
    gnb_cu_tnl_assoc_to_add_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CU-TNL-Association-To-Remove-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_tnl_assoc_to_rem_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_tnl_assoc_to_rem_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::gnb_cu_tnl_assoc_to_rem_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_cu_tnl_assoc_to_rem_item_s&       gnb_cu_tnl_assoc_to_rem_item() { return c; }
    const gnb_cu_tnl_assoc_to_rem_item_s& gnb_cu_tnl_assoc_to_rem_item() const { return c; }

  private:
    gnb_cu_tnl_assoc_to_rem_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CU-TNL-Association-To-Update-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_tnl_assoc_to_upd_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_tnl_assoc_to_upd_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::gnb_cu_tnl_assoc_to_upd_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_cu_tnl_assoc_to_upd_item_s&       gnb_cu_tnl_assoc_to_upd_item() { return c; }
    const gnb_cu_tnl_assoc_to_upd_item_s& gnb_cu_tnl_assoc_to_upd_item() const { return c; }

  private:
    gnb_cu_tnl_assoc_to_upd_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-DU-Served-Cells-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_served_cells_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_du_served_cells_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::gnb_du_served_cells_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_du_served_cells_item_s&       gnb_du_served_cells_item() { return c; }
    const gnb_du_served_cells_item_s& gnb_du_served_cells_item() const { return c; }

  private:
    gnb_du_served_cells_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-DU-TNL-Association-To-Remove-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_tnl_assoc_to_rem_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_du_tnl_assoc_to_rem_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::gnb_du_tnl_assoc_to_rem_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_du_tnl_assoc_to_rem_item_s&       gnb_du_tnl_assoc_to_rem_item() { return c; }
    const gnb_du_tnl_assoc_to_rem_item_s& gnb_du_tnl_assoc_to_rem_item() const { return c; }

  private:
    gnb_du_tnl_assoc_to_rem_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NR-CGI-List-For-Restart-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct nr_cgi_list_for_restart_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { nr_cgi_list_for_restart_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::nr_cgi_list_for_restart_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    nr_cgi_list_for_restart_item_s&       nr_cgi_list_for_restart_item() { return c; }
    const nr_cgi_list_for_restart_item_s& nr_cgi_list_for_restart_item() const { return c; }

  private:
    nr_cgi_list_for_restart_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWS-Failed-NR-CGI-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pws_failed_nr_cgi_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { pws_failed_nr_cgi_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::pws_failed_nr_cgi_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pws_failed_nr_cgi_item_s&       pws_failed_nr_cgi_item() { return c; }
    const pws_failed_nr_cgi_item_s& pws_failed_nr_cgi_item() const { return c; }

  private:
    pws_failed_nr_cgi_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWSSystemInformationExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct pws_sys_info_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { notif_info, add_sib_msg_list, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    notif_info_s&             notif_info();
    add_sib_msg_list_l&       add_sib_msg_list();
    const notif_info_s&       notif_info() const;
    const add_sib_msg_list_l& add_sib_msg_list() const;

  private:
    types                                             type_;
    choice_buffer_t<add_sib_msg_list_l, notif_info_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PagingCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct paging_cell_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { paging_cell_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::paging_cell_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    paging_cell_item_s&       paging_cell_item() { return c; }
    const paging_cell_item_s& paging_cell_item() const { return c; }

  private:
    paging_cell_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PagingIdentity-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
using paging_id_ext_ies_o = protocol_ies_empty_o;

// Potential-SpCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct potential_sp_cell_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { potential_sp_cell_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::potential_sp_cell_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    potential_sp_cell_item_s&       potential_sp_cell_item() { return c; }
    const potential_sp_cell_item_s& potential_sp_cell_item() const { return c; }

  private:
    potential_sp_cell_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Protected-EUTRA-Resources-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct protected_eutra_res_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { protected_eutra_res_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::protected_eutra_res_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    protected_eutra_res_item_s&       protected_eutra_res_item() { return c; }
    const protected_eutra_res_item_s& protected_eutra_res_item() const { return c; }

  private:
    protected_eutra_res_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using ran_ue_paging_id_ext_ies_container = protocol_ext_container_empty_l;

// RANUEPagingIdentity ::= SEQUENCE
struct ran_ue_paging_id_s {
  bool                               ie_exts_present = false;
  fixed_bitstring<40, false, true>   irnti;
  ran_ue_paging_id_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLCFailureIndication-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using rlc_fail_ind_ext_ies_o = protocol_ext_empty_o;

// RRC-Version-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct rrc_version_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { latest_rrc_version_enhanced, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::latest_rrc_version_enhanced; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_octstring<3, true>&       latest_rrc_version_enhanced() { return c; }
    const fixed_octstring<3, true>& latest_rrc_version_enhanced() const { return c; }

  private:
    fixed_octstring<3, true> c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RRCDeliveryStatus-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using rrc_delivery_status_ext_ies_o = protocol_ext_empty_o;

// ResetAll ::= ENUMERATED
struct reset_all_opts {
  enum options { reset_all, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<reset_all_opts, true> reset_all_e;

// ResetType-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
using reset_type_ext_ies_o = protocol_ies_empty_o;

// ResourceCoordinationEUTRACellInfo ::= SEQUENCE
struct res_coordination_eutra_cell_info_s {
  bool                                                                 ext = false;
  eutra_coex_mode_info_c                                               eutra_mode_info;
  eutra_prach_cfg_s                                                    eutra_prach_cfg;
  protocol_ext_container_l<res_coordination_eutra_cell_info_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceCoordinationTransferInformation-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using res_coordination_transfer_info_ext_ies_o = protocol_ext_empty_o;

// SCell-FailedtoSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct scell_failedto_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { scell_failedto_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::scell_failedto_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scell_failedto_setup_item_s&       scell_failedto_setup_item() { return c; }
    const scell_failedto_setup_item_s& scell_failedto_setup_item() const { return c; }

  private:
    scell_failedto_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SCell-FailedtoSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct scell_failedto_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { scell_failedto_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::scell_failedto_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scell_failedto_setup_mod_item_s&       scell_failedto_setup_mod_item() { return c; }
    const scell_failedto_setup_mod_item_s& scell_failedto_setup_mod_item() const { return c; }

  private:
    scell_failedto_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SCell-ToBeRemoved-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct scell_to_be_remd_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { scell_to_be_remd_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::scell_to_be_remd_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scell_to_be_remd_item_s&       scell_to_be_remd_item() { return c; }
    const scell_to_be_remd_item_s& scell_to_be_remd_item() const { return c; }

  private:
    scell_to_be_remd_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SCell-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct scell_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { scell_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::scell_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scell_to_be_setup_item_s&       scell_to_be_setup_item() { return c; }
    const scell_to_be_setup_item_s& scell_to_be_setup_item() const { return c; }

  private:
    scell_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SCell-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct scell_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { scell_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::scell_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scell_to_be_setup_mod_item_s&       scell_to_be_setup_mod_item() { return c; }
    const scell_to_be_setup_mod_item_s& scell_to_be_setup_mod_item() const { return c; }

  private:
    scell_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using sitype_item_ext_ies_container = protocol_ext_container_empty_l;

// SItype-Item ::= SEQUENCE
struct sitype_item_s {
  bool                          ie_exts_present = false;
  uint8_t                       sitype          = 1;
  sitype_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_failed_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_failed_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::srbs_failed_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_failed_to_be_setup_item_s&       srbs_failed_to_be_setup_item() { return c; }
    const srbs_failed_to_be_setup_item_s& srbs_failed_to_be_setup_item() const { return c; }

  private:
    srbs_failed_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_failed_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_failed_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::srbs_failed_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_failed_to_be_setup_mod_item_s&       srbs_failed_to_be_setup_mod_item() { return c; }
    const srbs_failed_to_be_setup_mod_item_s& srbs_failed_to_be_setup_mod_item() const { return c; }

  private:
    srbs_failed_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::srbs_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_modified_item_s&       srbs_modified_item() { return c; }
    const srbs_modified_item_s& srbs_modified_item() const { return c; }

  private:
    srbs_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-Required-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_required_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_required_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::srbs_required_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_required_to_be_released_item_s&       srbs_required_to_be_released_item() { return c; }
    const srbs_required_to_be_released_item_s& srbs_required_to_be_released_item() const { return c; }

  private:
    srbs_required_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::srbs_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_setup_item_s&       srbs_setup_item() { return c; }
    const srbs_setup_item_s& srbs_setup_item() const { return c; }

  private:
    srbs_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::srbs_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_setup_mod_item_s&       srbs_setup_mod_item() { return c; }
    const srbs_setup_mod_item_s& srbs_setup_mod_item() const { return c; }

  private:
    srbs_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::srbs_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_to_be_released_item_s&       srbs_to_be_released_item() { return c; }
    const srbs_to_be_released_item_s& srbs_to_be_released_item() const { return c; }

  private:
    srbs_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::srbs_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_to_be_setup_item_s&       srbs_to_be_setup_item() { return c; }
    const srbs_to_be_setup_item_s& srbs_to_be_setup_item() const { return c; }

  private:
    srbs_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::srbs_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_to_be_setup_mod_item_s&       srbs_to_be_setup_mod_item() { return c; }
    const srbs_to_be_setup_mod_item_s& srbs_to_be_setup_mod_item() const { return c; }

  private:
    srbs_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Served-Cells-To-Add-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct served_cells_to_add_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { served_cells_to_add_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::served_cells_to_add_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    served_cells_to_add_item_s&       served_cells_to_add_item() { return c; }
    const served_cells_to_add_item_s& served_cells_to_add_item() const { return c; }

  private:
    served_cells_to_add_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Served-Cells-To-Delete-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct served_cells_to_delete_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { served_cells_to_delete_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::served_cells_to_delete_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    served_cells_to_delete_item_s&       served_cells_to_delete_item() { return c; }
    const served_cells_to_delete_item_s& served_cells_to_delete_item() const { return c; }

  private:
    served_cells_to_delete_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Served-Cells-To-Modify-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct served_cells_to_modify_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { served_cells_to_modify_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::served_cells_to_modify_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    served_cells_to_modify_item_s&       served_cells_to_modify_item() { return c; }
    const served_cells_to_modify_item_s& served_cells_to_modify_item() const { return c; }

  private:
    served_cells_to_modify_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UAC-Assistance-InfoExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using uac_assist_info_ext_ies_o = protocol_ext_empty_o;

// UACPLMN-List ::= SEQUENCE (SIZE (1..12)) OF UACPLMN-Item
using uac_plmn_list_l = dyn_array<uac_plmn_item_s>;

// UE-associatedLogicalF1-ConnectionItemResAck ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_associated_lc_f1_conn_item_res_ack_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_associated_lc_f1_conn_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ue_associated_lc_f1_conn_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_associated_lc_f1_conn_item_s&       ue_associated_lc_f1_conn_item() { return c; }
    const ue_associated_lc_f1_conn_item_s& ue_associated_lc_f1_conn_item() const { return c; }

  private:
    ue_associated_lc_f1_conn_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-associatedLogicalF1-ConnectionListRes ::= SEQUENCE (SIZE (1..65536)) OF
// ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using ue_associated_lc_f1_conn_list_res_l =
    dyn_array<protocol_ie_single_container_s<ue_associated_lc_f1_conn_item_res_o>>;

// UEIdentityIndexValueChoice-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
using ue_id_idx_value_choice_ext_ies_o = protocol_ies_empty_o;

// Cancel-all-Warning-Messages-Indicator ::= ENUMERATED
struct cancel_all_warning_msgs_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<cancel_all_warning_msgs_ind_opts, true> cancel_all_warning_msgs_ind_e;

// DRXConfigurationIndicator ::= ENUMERATED
struct drx_cfg_ind_opts {
  enum options { release, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<drx_cfg_ind_opts, true> drx_cfg_ind_e;

// FullConfiguration ::= ENUMERATED
struct full_cfg_opts {
  enum options { full, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<full_cfg_opts, true> full_cfg_e;

// GNB-CU-TNL-Association-Failed-To-Setup-List ::= SEQUENCE (SIZE (1..32)) OF
// ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using gnb_cu_tnl_assoc_failed_to_setup_list_l =
    bounded_array<protocol_ie_single_container_s<gnb_cu_tnl_assoc_failed_to_setup_item_ies_o>, 32>;

// GNB-CU-TNL-Association-Setup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using gnb_cu_tnl_assoc_setup_list_l =
    bounded_array<protocol_ie_single_container_s<gnb_cu_tnl_assoc_setup_item_ies_o>, 32>;

// GNB-CU-TNL-Association-To-Add-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using gnb_cu_tnl_assoc_to_add_list_l =
    bounded_array<protocol_ie_single_container_s<gnb_cu_tnl_assoc_to_add_item_ies_o>, 32>;

// GNB-CU-TNL-Association-To-Remove-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using gnb_cu_tnl_assoc_to_rem_list_l =
    bounded_array<protocol_ie_single_container_s<gnb_cu_tnl_assoc_to_rem_item_ies_o>, 32>;

// GNB-CU-TNL-Association-To-Update-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using gnb_cu_tnl_assoc_to_upd_list_l =
    bounded_array<protocol_ie_single_container_s<gnb_cu_tnl_assoc_to_upd_item_ies_o>, 32>;

// GNB-DU-Served-Cells-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using gnb_du_served_cells_list_l = dyn_array<protocol_ie_single_container_s<gnb_du_served_cells_item_ies_o>>;

// GNB-DU-TNL-Association-To-Remove-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using gnb_du_tnl_assoc_to_rem_list_l =
    bounded_array<protocol_ie_single_container_s<gnb_du_tnl_assoc_to_rem_item_ies_o>, 32>;

// GNB-DUConfigurationQuery ::= ENUMERATED
struct gnb_du_cfg_query_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<gnb_du_cfg_query_opts, true> gnb_du_cfg_query_e;

// GNBDUOverloadInformation ::= ENUMERATED
struct gnb_du_overload_info_opts {
  enum options { overloaded, not_overloaded, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<gnb_du_overload_info_opts> gnb_du_overload_info_e;

// IgnoreResourceCoordinationContainer ::= ENUMERATED
struct ignore_res_coordination_container_opts {
  enum options { yes, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<ignore_res_coordination_container_opts, true> ignore_res_coordination_container_e;

// InactivityMonitoringRequest ::= ENUMERATED
struct inactivity_monitoring_request_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<inactivity_monitoring_request_opts, true> inactivity_monitoring_request_e;

// InactivityMonitoringResponse ::= ENUMERATED
struct inactivity_monitoring_resp_opts {
  enum options { not_supported, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<inactivity_monitoring_resp_opts, true> inactivity_monitoring_resp_e;

// NR-CGI-List-For-Restart-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using nr_cgi_list_for_restart_list_l =
    dyn_array<protocol_ie_single_container_s<nr_cgi_list_for_restart_list_item_ies_o>>;

// NeedforGap ::= ENUMERATED
struct needfor_gap_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<needfor_gap_opts, true> needfor_gap_e;

// PWS-Failed-NR-CGI-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using pws_failed_nr_cgi_list_l = dyn_array<protocol_ie_single_container_s<pws_failed_nr_cgi_list_item_ies_o>>;

struct pws_sys_info_ext_ies_container {
  template <class extT_>
  using ie_field_s = protocol_ext_container_item_s<extT_>;

  // member variables
  bool                                                         notif_info_present       = false;
  bool                                                         add_sib_msg_list_present = false;
  ie_field_s<notif_info_s>                                     notif_info;
  ie_field_s<dyn_seq_of<add_sib_msg_list_item_s, 1, 63, true>> add_sib_msg_list;

  // sequence methods
  pws_sys_info_ext_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSSystemInformation ::= SEQUENCE
struct pws_sys_info_s {
  bool                           ext             = false;
  bool                           ie_exts_present = false;
  uint8_t                        sibtype         = 6;
  unbounded_octstring<true>      sibmsg;
  pws_sys_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingCell-list ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using paging_cell_list_l = dyn_array<protocol_ie_single_container_s<paging_cell_item_ies_o>>;

// PagingDRX ::= ENUMERATED
struct paging_drx_opts {
  enum options { v32, v64, v128, v256, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<paging_drx_opts, true> paging_drx_e;

// PagingIdentity ::= CHOICE
struct paging_id_c {
  struct types_opts {
    enum options { ran_ue_paging_id, cn_ue_paging_id, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  paging_id_c() = default;
  paging_id_c(const paging_id_c& other);
  paging_id_c& operator=(const paging_id_c& other);
  ~paging_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  ran_ue_paging_id_s& ran_ue_paging_id()
  {
    assert_choice_type(types::ran_ue_paging_id, type_, "PagingIdentity");
    return c.get<ran_ue_paging_id_s>();
  }
  cn_ue_paging_id_c& cn_ue_paging_id()
  {
    assert_choice_type(types::cn_ue_paging_id, type_, "PagingIdentity");
    return c.get<cn_ue_paging_id_c>();
  }
  protocol_ie_single_container_s<paging_id_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "PagingIdentity");
    return c.get<protocol_ie_single_container_s<paging_id_ext_ies_o>>();
  }
  const ran_ue_paging_id_s& ran_ue_paging_id() const
  {
    assert_choice_type(types::ran_ue_paging_id, type_, "PagingIdentity");
    return c.get<ran_ue_paging_id_s>();
  }
  const cn_ue_paging_id_c& cn_ue_paging_id() const
  {
    assert_choice_type(types::cn_ue_paging_id, type_, "PagingIdentity");
    return c.get<cn_ue_paging_id_c>();
  }
  const protocol_ie_single_container_s<paging_id_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "PagingIdentity");
    return c.get<protocol_ie_single_container_s<paging_id_ext_ies_o>>();
  }
  ran_ue_paging_id_s&                                  set_ran_ue_paging_id();
  cn_ue_paging_id_c&                                   set_cn_ue_paging_id();
  protocol_ie_single_container_s<paging_id_ext_ies_o>& set_choice_ext();

private:
  types                                                                                                       type_;
  choice_buffer_t<cn_ue_paging_id_c, protocol_ie_single_container_s<paging_id_ext_ies_o>, ran_ue_paging_id_s> c;

  void destroy_();
};

// PagingOrigin ::= ENUMERATED
struct paging_origin_opts {
  enum options { non_neg3gpp, /*...*/ nulltype } value;
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
typedef enumerated<paging_origin_opts, true> paging_origin_e;

// PagingPriority ::= ENUMERATED
struct paging_prio_opts {
  enum options {
    priolevel1,
    priolevel2,
    priolevel3,
    priolevel4,
    priolevel5,
    priolevel6,
    priolevel7,
    priolevel8,
    // ...
    nulltype
  } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<paging_prio_opts, true> paging_prio_e;

// Potential-SpCell-List ::= SEQUENCE (SIZE (0..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using potential_sp_cell_list_l = dyn_array<protocol_ie_single_container_s<potential_sp_cell_item_ies_o>>;

// PrivateIE-Field{F1AP-PRIVATE-IES : IEsSetParam} ::= SEQUENCE{{F1AP-PRIVATE-IES}}
template <class ies_set_paramT_>
struct private_ie_field_s {
  private_ie_id_c                   id;
  crit_e                            crit;
  typename ies_set_paramT_::value_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Protected-EUTRA-Resources-List ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using protected_eutra_res_list_l = dyn_array<protocol_ie_single_container_s<protected_eutra_res_item_ies_o>>;

using rlc_fail_ind_ext_ies_container = protocol_ext_container_empty_l;

// RLCFailureIndication ::= SEQUENCE
struct rlc_fail_ind_s {
  bool                           ie_exts_present = false;
  uint8_t                        assocated_lcid  = 1;
  rlc_fail_ind_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRC-Version ::= SEQUENCE
struct rrc_version_s {
  fixed_bitstring<3, false, true>                 latest_rrc_version;
  protocol_ext_container_l<rrc_version_ext_ies_o> ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using rrc_delivery_status_ext_ies_container = protocol_ext_container_empty_l;

// RRCDeliveryStatus ::= SEQUENCE
struct rrc_delivery_status_s {
  bool                                  ie_exts_present = false;
  uint16_t                              delivery_status = 0;
  uint16_t                              trigger_msg     = 0;
  rrc_delivery_status_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfigurationCompleteIndicator ::= ENUMERATED
struct rrc_recfg_complete_ind_opts {
  enum options { true_value, /*...*/ fail, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<rrc_recfg_complete_ind_opts, true, 1> rrc_recfg_complete_ind_e;

// RequestType ::= ENUMERATED
struct request_type_opts {
  enum options { offer, execution, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<request_type_opts, true> request_type_e;

// ResetType ::= CHOICE
struct reset_type_c {
  struct types_opts {
    enum options { f1_interface, part_of_f1_interface, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

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
  reset_all_e& f1_interface()
  {
    assert_choice_type(types::f1_interface, type_, "ResetType");
    return c.get<reset_all_e>();
  }
  ue_associated_lc_f1_conn_list_res_l& part_of_f1_interface()
  {
    assert_choice_type(types::part_of_f1_interface, type_, "ResetType");
    return c.get<ue_associated_lc_f1_conn_list_res_l>();
  }
  protocol_ie_single_container_s<reset_type_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "ResetType");
    return c.get<protocol_ie_single_container_s<reset_type_ext_ies_o>>();
  }
  const reset_all_e& f1_interface() const
  {
    assert_choice_type(types::f1_interface, type_, "ResetType");
    return c.get<reset_all_e>();
  }
  const ue_associated_lc_f1_conn_list_res_l& part_of_f1_interface() const
  {
    assert_choice_type(types::part_of_f1_interface, type_, "ResetType");
    return c.get<ue_associated_lc_f1_conn_list_res_l>();
  }
  const protocol_ie_single_container_s<reset_type_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "ResetType");
    return c.get<protocol_ie_single_container_s<reset_type_ext_ies_o>>();
  }
  reset_all_e&                                          set_f1_interface();
  ue_associated_lc_f1_conn_list_res_l&                  set_part_of_f1_interface();
  protocol_ie_single_container_s<reset_type_ext_ies_o>& set_choice_ext();

private:
  types                                                                                                      type_;
  choice_buffer_t<protocol_ie_single_container_s<reset_type_ext_ies_o>, ue_associated_lc_f1_conn_list_res_l> c;

  void destroy_();
};

using res_coordination_transfer_info_ext_ies_container = protocol_ext_container_empty_l;

// ResourceCoordinationTransferInformation ::= SEQUENCE
struct res_coordination_transfer_info_s {
  bool                                             ext                                      = false;
  bool                                             res_coordination_eutra_cell_info_present = false;
  bool                                             ie_exts_present                          = false;
  fixed_bitstring<28, false, true>                 m_enb_cell_id;
  res_coordination_eutra_cell_info_s               res_coordination_eutra_cell_info;
  res_coordination_transfer_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCell-FailedtoSetup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using scell_failedto_setup_list_l = bounded_array<protocol_ie_single_container_s<scell_failedto_setup_item_ies_o>, 32>;

// SCell-FailedtoSetupMod-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using scell_failedto_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<scell_failedto_setup_mod_item_ies_o>, 32>;

// SCell-ToBeRemoved-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using scell_to_be_remd_list_l = bounded_array<protocol_ie_single_container_s<scell_to_be_remd_item_ies_o>, 32>;

// SCell-ToBeSetup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using scell_to_be_setup_list_l = bounded_array<protocol_ie_single_container_s<scell_to_be_setup_item_ies_o>, 32>;

// SCell-ToBeSetupMod-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using scell_to_be_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<scell_to_be_setup_mod_item_ies_o>, 32>;

// SItype-List ::= SEQUENCE (SIZE (1..32)) OF SItype-Item
using sitype_list_l = dyn_array<sitype_item_s>;

// SRBs-FailedToBeSetup-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_failed_to_be_setup_list_l =
    bounded_array<protocol_ie_single_container_s<srbs_failed_to_be_setup_item_ies_o>, 8>;

// SRBs-FailedToBeSetupMod-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using srbs_failed_to_be_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<srbs_failed_to_be_setup_mod_item_ies_o>, 8>;

// SRBs-Modified-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_modified_list_l = bounded_array<protocol_ie_single_container_s<srbs_modified_item_ies_o>, 8>;

// SRBs-Required-ToBeReleased-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using srbs_required_to_be_released_list_l =
    bounded_array<protocol_ie_single_container_s<srbs_required_to_be_released_item_ies_o>, 8>;

// SRBs-Setup-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_setup_list_l = bounded_array<protocol_ie_single_container_s<srbs_setup_item_ies_o>, 8>;

// SRBs-SetupMod-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_setup_mod_list_l = bounded_array<protocol_ie_single_container_s<srbs_setup_mod_item_ies_o>, 8>;

// SRBs-ToBeReleased-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_to_be_released_list_l = bounded_array<protocol_ie_single_container_s<srbs_to_be_released_item_ies_o>, 8>;

// SRBs-ToBeSetup-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_to_be_setup_list_l = bounded_array<protocol_ie_single_container_s<srbs_to_be_setup_item_ies_o>, 8>;

// SRBs-ToBeSetupMod-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_to_be_setup_mod_list_l = bounded_array<protocol_ie_single_container_s<srbs_to_be_setup_mod_item_ies_o>, 8>;

// SULAccessIndication ::= ENUMERATED
struct sul_access_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<sul_access_ind_opts, true> sul_access_ind_e;

// Served-Cells-To-Add-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using served_cells_to_add_list_l = dyn_array<protocol_ie_single_container_s<served_cells_to_add_item_ies_o>>;

// Served-Cells-To-Delete-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using served_cells_to_delete_list_l = dyn_array<protocol_ie_single_container_s<served_cells_to_delete_item_ies_o>>;

// Served-Cells-To-Modify-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using served_cells_to_modify_list_l = dyn_array<protocol_ie_single_container_s<served_cells_to_modify_item_ies_o>>;

// TimeToWait ::= ENUMERATED
struct time_to_wait_opts {
  enum options { v1s, v2s, v5s, v10s, v20s, v60s, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<time_to_wait_opts, true> time_to_wait_e;

// TransmissionActionIndicator ::= ENUMERATED
struct tx_action_ind_opts {
  enum options { stop, /*...*/ restart, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<tx_action_ind_opts, true, 1> tx_action_ind_e;

using uac_assist_info_ext_ies_container = protocol_ext_container_empty_l;

// UAC-Assistance-Info ::= SEQUENCE
struct uac_assist_info_s {
  bool                              ie_exts_present = false;
  uac_plmn_list_l                   uac_plmn_list;
  uac_assist_info_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-associatedLogicalF1-ConnectionListResAck ::= SEQUENCE (SIZE (1..65536)) OF
// ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using ue_associated_lc_f1_conn_list_res_ack_l =
    dyn_array<protocol_ie_single_container_s<ue_associated_lc_f1_conn_item_res_ack_o>>;

// UEIdentityIndexValue ::= CHOICE
struct ue_id_idx_value_c {
  struct types_opts {
    enum options { idx_len10, choice_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  ue_id_idx_value_c() = default;
  ue_id_idx_value_c(const ue_id_idx_value_c& other);
  ue_id_idx_value_c& operator=(const ue_id_idx_value_c& other);
  ~ue_id_idx_value_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<10, false, true>& idx_len10()
  {
    assert_choice_type(types::idx_len10, type_, "UEIdentityIndexValue");
    return c.get<fixed_bitstring<10, false, true>>();
  }
  protocol_ie_single_container_s<ue_id_idx_value_choice_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "UEIdentityIndexValue");
    return c.get<protocol_ie_single_container_s<ue_id_idx_value_choice_ext_ies_o>>();
  }
  const fixed_bitstring<10, false, true>& idx_len10() const
  {
    assert_choice_type(types::idx_len10, type_, "UEIdentityIndexValue");
    return c.get<fixed_bitstring<10, false, true>>();
  }
  const protocol_ie_single_container_s<ue_id_idx_value_choice_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "UEIdentityIndexValue");
    return c.get<protocol_ie_single_container_s<ue_id_idx_value_choice_ext_ies_o>>();
  }
  fixed_bitstring<10, false, true>&                                 set_idx_len10();
  protocol_ie_single_container_s<ue_id_idx_value_choice_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<fixed_bitstring<10, false, true>, protocol_ie_single_container_s<ue_id_idx_value_choice_ext_ies_o>> c;

  void destroy_();
};

// F1RemovalFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct f1_removal_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
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

// F1RemovalRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct f1_removal_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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

// F1RemovalResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct f1_removal_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
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

// F1SetupFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct f1_setup_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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

// F1SetupRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct f1_setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        gnb_du_id,
        gnb_du_name,
        gnb_du_served_cells_list,
        gnb_du_rrc_version,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                   gnb_du_id();
    printable_string<1, 150, true, true>&       gnb_du_name();
    gnb_du_served_cells_list_l&                 gnb_du_served_cells_list();
    rrc_version_s&                              gnb_du_rrc_version();
    const uint16_t&                             transaction_id() const;
    const uint64_t&                             gnb_du_id() const;
    const printable_string<1, 150, true, true>& gnb_du_name() const;
    const gnb_du_served_cells_list_l&           gnb_du_served_cells_list() const;
    const rrc_version_s&                        gnb_du_rrc_version() const;

  private:
    types                                                                                            type_;
    choice_buffer_t<gnb_du_served_cells_list_l, printable_string<1, 150, true, true>, rrc_version_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// F1SetupResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct f1_setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, gnb_cu_name, cells_to_be_activ_list, gnb_cu_rrc_version, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    printable_string<1, 150, true, true>&       gnb_cu_name();
    cells_to_be_activ_list_l&                   cells_to_be_activ_list();
    rrc_version_s&                              gnb_cu_rrc_version();
    const uint16_t&                             transaction_id() const;
    const printable_string<1, 150, true, true>& gnb_cu_name() const;
    const cells_to_be_activ_list_l&             cells_to_be_activ_list() const;
    const rrc_version_s&                        gnb_cu_rrc_version() const;

  private:
    types                                                                                          type_;
    choice_buffer_t<cells_to_be_activ_list_l, printable_string<1, 150, true, true>, rrc_version_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNBCUConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_cfg_upd_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        cells_failed_to_be_activ_list,
        crit_diagnostics,
        gnb_cu_tnl_assoc_setup_list,
        gnb_cu_tnl_assoc_failed_to_setup_list,
        ded_si_delivery_needed_ue_list,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    cells_failed_to_be_activ_list_l&               cells_failed_to_be_activ_list();
    crit_diagnostics_s&                            crit_diagnostics();
    gnb_cu_tnl_assoc_setup_list_l&                 gnb_cu_tnl_assoc_setup_list();
    gnb_cu_tnl_assoc_failed_to_setup_list_l&       gnb_cu_tnl_assoc_failed_to_setup_list();
    ded_si_delivery_needed_ue_list_l&              ded_si_delivery_needed_ue_list();
    const uint16_t&                                transaction_id() const;
    const cells_failed_to_be_activ_list_l&         cells_failed_to_be_activ_list() const;
    const crit_diagnostics_s&                      crit_diagnostics() const;
    const gnb_cu_tnl_assoc_setup_list_l&           gnb_cu_tnl_assoc_setup_list() const;
    const gnb_cu_tnl_assoc_failed_to_setup_list_l& gnb_cu_tnl_assoc_failed_to_setup_list() const;
    const ded_si_delivery_needed_ue_list_l&        ded_si_delivery_needed_ue_list() const;

  private:
    types type_;
    choice_buffer_t<cells_failed_to_be_activ_list_l,
                    crit_diagnostics_s,
                    ded_si_delivery_needed_ue_list_l,
                    gnb_cu_tnl_assoc_failed_to_setup_list_l,
                    gnb_cu_tnl_assoc_setup_list_l>
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

// GNBCUConfigurationUpdateFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_cfg_upd_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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

// GNBCUConfigurationUpdateIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_cfg_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        cells_to_be_activ_list,
        cells_to_be_deactiv_list,
        gnb_cu_tnl_assoc_to_add_list,
        gnb_cu_tnl_assoc_to_rem_list,
        gnb_cu_tnl_assoc_to_upd_list,
        cells_to_be_barred_list,
        protected_eutra_res_list,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    cells_to_be_activ_list_l&             cells_to_be_activ_list();
    cells_to_be_deactiv_list_l&           cells_to_be_deactiv_list();
    gnb_cu_tnl_assoc_to_add_list_l&       gnb_cu_tnl_assoc_to_add_list();
    gnb_cu_tnl_assoc_to_rem_list_l&       gnb_cu_tnl_assoc_to_rem_list();
    gnb_cu_tnl_assoc_to_upd_list_l&       gnb_cu_tnl_assoc_to_upd_list();
    cells_to_be_barred_list_l&            cells_to_be_barred_list();
    protected_eutra_res_list_l&           protected_eutra_res_list();
    const uint16_t&                       transaction_id() const;
    const cells_to_be_activ_list_l&       cells_to_be_activ_list() const;
    const cells_to_be_deactiv_list_l&     cells_to_be_deactiv_list() const;
    const gnb_cu_tnl_assoc_to_add_list_l& gnb_cu_tnl_assoc_to_add_list() const;
    const gnb_cu_tnl_assoc_to_rem_list_l& gnb_cu_tnl_assoc_to_rem_list() const;
    const gnb_cu_tnl_assoc_to_upd_list_l& gnb_cu_tnl_assoc_to_upd_list() const;
    const cells_to_be_barred_list_l&      cells_to_be_barred_list() const;
    const protected_eutra_res_list_l&     protected_eutra_res_list() const;

  private:
    types type_;
    choice_buffer_t<cells_to_be_activ_list_l,
                    cells_to_be_barred_list_l,
                    cells_to_be_deactiv_list_l,
                    gnb_cu_tnl_assoc_to_add_list_l,
                    gnb_cu_tnl_assoc_to_rem_list_l,
                    gnb_cu_tnl_assoc_to_upd_list_l,
                    protected_eutra_res_list_l>
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

// GNBDUConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_cfg_upd_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        cells_to_be_activ_list,
        crit_diagnostics,
        cells_to_be_deactiv_list,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    cells_to_be_activ_list_l&         cells_to_be_activ_list();
    crit_diagnostics_s&               crit_diagnostics();
    cells_to_be_deactiv_list_l&       cells_to_be_deactiv_list();
    const uint16_t&                   transaction_id() const;
    const cells_to_be_activ_list_l&   cells_to_be_activ_list() const;
    const crit_diagnostics_s&         crit_diagnostics() const;
    const cells_to_be_deactiv_list_l& cells_to_be_deactiv_list() const;

  private:
    types                                                                                     type_;
    choice_buffer_t<cells_to_be_activ_list_l, cells_to_be_deactiv_list_l, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNBDUConfigurationUpdateFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_cfg_upd_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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

// GNBDUConfigurationUpdateIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_cfg_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        served_cells_to_add_list,
        served_cells_to_modify_list,
        served_cells_to_delete_list,
        cells_status_list,
        ded_si_delivery_needed_ue_list,
        gnb_du_id,
        gnb_du_tnl_assoc_to_rem_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    served_cells_to_add_list_l&             served_cells_to_add_list();
    served_cells_to_modify_list_l&          served_cells_to_modify_list();
    served_cells_to_delete_list_l&          served_cells_to_delete_list();
    cells_status_list_l&                    cells_status_list();
    ded_si_delivery_needed_ue_list_l&       ded_si_delivery_needed_ue_list();
    uint64_t&                               gnb_du_id();
    gnb_du_tnl_assoc_to_rem_list_l&         gnb_du_tnl_assoc_to_rem_list();
    const uint16_t&                         transaction_id() const;
    const served_cells_to_add_list_l&       served_cells_to_add_list() const;
    const served_cells_to_modify_list_l&    served_cells_to_modify_list() const;
    const served_cells_to_delete_list_l&    served_cells_to_delete_list() const;
    const cells_status_list_l&              cells_status_list() const;
    const ded_si_delivery_needed_ue_list_l& ded_si_delivery_needed_ue_list() const;
    const uint64_t&                         gnb_du_id() const;
    const gnb_du_tnl_assoc_to_rem_list_l&   gnb_du_tnl_assoc_to_rem_list() const;

  private:
    types type_;
    choice_buffer_t<cells_status_list_l,
                    ded_si_delivery_needed_ue_list_l,
                    gnb_du_tnl_assoc_to_rem_list_l,
                    served_cells_to_add_list_l,
                    served_cells_to_delete_list_l,
                    served_cells_to_modify_list_l>
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

// GNBDUResourceCoordinationRequest-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_res_coordination_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        request_type,
        eutra_nr_cell_res_coordination_req_container,
        ignore_res_coordination_container,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                                  transaction_id();
    request_type_e&                            request_type();
    unbounded_octstring<true>&                 eutra_nr_cell_res_coordination_req_container();
    ignore_res_coordination_container_e&       ignore_res_coordination_container();
    const uint16_t&                            transaction_id() const;
    const request_type_e&                      request_type() const;
    const unbounded_octstring<true>&           eutra_nr_cell_res_coordination_req_container() const;
    const ignore_res_coordination_container_e& ignore_res_coordination_container() const;

  private:
    types                                      type_;
    choice_buffer_t<unbounded_octstring<true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNBDUResourceCoordinationResponse-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_res_coordination_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, eutra_nr_cell_res_coordination_req_ack_container, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    unbounded_octstring<true>&       eutra_nr_cell_res_coordination_req_ack_container();
    const uint16_t&                  transaction_id() const;
    const unbounded_octstring<true>& eutra_nr_cell_res_coordination_req_ack_container() const;

  private:
    types                                      type_;
    choice_buffer_t<unbounded_octstring<true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNBDUStatusIndicationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_status_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, gnb_du_overload_info, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                     transaction_id();
    gnb_du_overload_info_e&       gnb_du_overload_info();
    const uint16_t&               transaction_id() const;
    const gnb_du_overload_info_e& gnb_du_overload_info() const;

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

// InitialULRRCMessageTransferIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct init_ul_rrc_msg_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_du_ue_f1ap_id,
        nrcgi,
        c_rnti,
        rrc_container,
        du_to_cu_rrc_container,
        sul_access_ind,
        transaction_id,
        ranueid,
        rrc_container_rrc_setup_complete,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                        gnb_du_ue_f1ap_id();
    nrcgi_s&                         nrcgi();
    uint32_t&                        c_rnti();
    unbounded_octstring<true>&       rrc_container();
    unbounded_octstring<true>&       du_to_cu_rrc_container();
    sul_access_ind_e&                sul_access_ind();
    uint16_t&                        transaction_id();
    fixed_octstring<8, true>&        ranueid();
    unbounded_octstring<true>&       rrc_container_rrc_setup_complete();
    const uint64_t&                  gnb_du_ue_f1ap_id() const;
    const nrcgi_s&                   nrcgi() const;
    const uint32_t&                  c_rnti() const;
    const unbounded_octstring<true>& rrc_container() const;
    const unbounded_octstring<true>& du_to_cu_rrc_container() const;
    const sul_access_ind_e&          sul_access_ind() const;
    const uint16_t&                  transaction_id() const;
    const fixed_octstring<8, true>&  ranueid() const;
    const unbounded_octstring<true>& rrc_container_rrc_setup_complete() const;

  private:
    types                                                                         type_;
    choice_buffer_t<fixed_octstring<8, true>, nrcgi_s, unbounded_octstring<true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NetworkAccessRateReductionIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct network_access_rate_reduction_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, uac_assist_info, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                transaction_id();
    uac_assist_info_s&       uac_assist_info();
    const uint16_t&          transaction_id() const;
    const uac_assist_info_s& uac_assist_info() const;

  private:
    types                              type_;
    choice_buffer_t<uac_assist_info_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NotifyIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct notify_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, drb_notify_list, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                gnb_cu_ue_f1ap_id();
    uint64_t&                gnb_du_ue_f1ap_id();
    drb_notify_list_l&       drb_notify_list();
    const uint64_t&          gnb_cu_ue_f1ap_id() const;
    const uint64_t&          gnb_du_ue_f1ap_id() const;
    const drb_notify_list_l& drb_notify_list() const;

  private:
    types                              type_;
    choice_buffer_t<drb_notify_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWSCancelRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pws_cancel_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        numof_broadcast_request,
        broadcast_to_be_cancelled_list,
        cancel_all_warning_msgs_ind,
        notif_info,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint32_t&                               numof_broadcast_request();
    broadcast_to_be_cancelled_list_l&       broadcast_to_be_cancelled_list();
    cancel_all_warning_msgs_ind_e&          cancel_all_warning_msgs_ind();
    notif_info_s&                           notif_info();
    const uint16_t&                         transaction_id() const;
    const uint32_t&                         numof_broadcast_request() const;
    const broadcast_to_be_cancelled_list_l& broadcast_to_be_cancelled_list() const;
    const cancel_all_warning_msgs_ind_e&    cancel_all_warning_msgs_ind() const;
    const notif_info_s&                     notif_info() const;

  private:
    types                                                           type_;
    choice_buffer_t<broadcast_to_be_cancelled_list_l, notif_info_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWSCancelResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pws_cancel_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cells_broadcast_cancelled_list, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    cells_broadcast_cancelled_list_l&       cells_broadcast_cancelled_list();
    crit_diagnostics_s&                     crit_diagnostics();
    const uint16_t&                         transaction_id() const;
    const cells_broadcast_cancelled_list_l& cells_broadcast_cancelled_list() const;
    const crit_diagnostics_s&               crit_diagnostics() const;

  private:
    types                                                                 type_;
    choice_buffer_t<cells_broadcast_cancelled_list_l, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWSFailureIndicationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pws_fail_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, pws_failed_nr_cgi_list, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                       transaction_id();
    pws_failed_nr_cgi_list_l&       pws_failed_nr_cgi_list();
    const uint16_t&                 transaction_id() const;
    const pws_failed_nr_cgi_list_l& pws_failed_nr_cgi_list() const;

  private:
    types                                     type_;
    choice_buffer_t<pws_failed_nr_cgi_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWSRestartIndicationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pws_restart_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, nr_cgi_list_for_restart_list, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    nr_cgi_list_for_restart_list_l&       nr_cgi_list_for_restart_list();
    const uint16_t&                       transaction_id() const;
    const nr_cgi_list_for_restart_list_l& nr_cgi_list_for_restart_list() const;

  private:
    types                                           type_;
    choice_buffer_t<nr_cgi_list_for_restart_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PagingIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct paging_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ue_id_idx_value,
        paging_id,
        paging_drx,
        paging_prio,
        paging_cell_list,
        paging_origin,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    ue_id_idx_value_c&        ue_id_idx_value();
    paging_id_c&              paging_id();
    paging_drx_e&             paging_drx();
    paging_prio_e&            paging_prio();
    paging_cell_list_l&       paging_cell_list();
    paging_origin_e&          paging_origin();
    const ue_id_idx_value_c&  ue_id_idx_value() const;
    const paging_id_c&        paging_id() const;
    const paging_drx_e&       paging_drx() const;
    const paging_prio_e&      paging_prio() const;
    const paging_cell_list_l& paging_cell_list() const;
    const paging_origin_e&    paging_origin() const;

  private:
    types                                                               type_;
    choice_buffer_t<paging_cell_list_l, paging_id_c, ue_id_idx_value_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PrivateIE-Container{F1AP-PRIVATE-IES : IEsSetParam} ::= SEQUENCE (SIZE (1..65535)) OF PrivateIE-Field
template <class ies_set_paramT_>
using private_ie_container_l = dyn_seq_of<private_ie_field_s<ies_set_paramT_>, 1, 65535, true>;

struct f1ap_private_ies_empty_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::nulltype; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };
};
// PrivateMessage-IEs ::= OBJECT SET OF F1AP-PRIVATE-IES
using private_msg_ies_o = f1ap_private_ies_empty_o;

// RRCDeliveryReportIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct rrc_delivery_report_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, rrc_delivery_status, srbid, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                    gnb_cu_ue_f1ap_id();
    uint64_t&                    gnb_du_ue_f1ap_id();
    rrc_delivery_status_s&       rrc_delivery_status();
    uint8_t&                     srbid();
    const uint64_t&              gnb_cu_ue_f1ap_id() const;
    const uint64_t&              gnb_du_ue_f1ap_id() const;
    const rrc_delivery_status_s& rrc_delivery_status() const;
    const uint8_t&               srbid() const;

  private:
    types                                  type_;
    choice_buffer_t<rrc_delivery_status_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ResetAcknowledgeIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct reset_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, ue_associated_lc_f1_conn_list_res_ack, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    ue_associated_lc_f1_conn_list_res_ack_l&       ue_associated_lc_f1_conn_list_res_ack();
    crit_diagnostics_s&                            crit_diagnostics();
    const uint16_t&                                transaction_id() const;
    const ue_associated_lc_f1_conn_list_res_ack_l& ue_associated_lc_f1_conn_list_res_ack() const;
    const crit_diagnostics_s&                      crit_diagnostics() const;

  private:
    types                                                                        type_;
    choice_buffer_t<crit_diagnostics_s, ue_associated_lc_f1_conn_list_res_ack_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ResetIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct reset_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, reset_type, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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

// SystemInformationDeliveryCommandIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sys_info_delivery_cmd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, nrcgi, sitype_list, confirmed_ue_id, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&            transaction_id();
    nrcgi_s&             nrcgi();
    sitype_list_l&       sitype_list();
    uint64_t&            confirmed_ue_id();
    const uint16_t&      transaction_id() const;
    const nrcgi_s&       nrcgi() const;
    const sitype_list_l& sitype_list() const;
    const uint64_t&      confirmed_ue_id() const;

  private:
    types                                   type_;
    choice_buffer_t<nrcgi_s, sitype_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextModificationConfirmIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_mod_confirm_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        res_coordination_transfer_container,
        drbs_modified_conf_list,
        rrc_container,
        crit_diagnostics,
        execute_dupl,
        res_coordination_transfer_info,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                               gnb_cu_ue_f1ap_id();
    uint64_t&                               gnb_du_ue_f1ap_id();
    unbounded_octstring<true>&              res_coordination_transfer_container();
    drbs_modified_conf_list_l&              drbs_modified_conf_list();
    unbounded_octstring<true>&              rrc_container();
    crit_diagnostics_s&                     crit_diagnostics();
    execute_dupl_e&                         execute_dupl();
    res_coordination_transfer_info_s&       res_coordination_transfer_info();
    const uint64_t&                         gnb_cu_ue_f1ap_id() const;
    const uint64_t&                         gnb_du_ue_f1ap_id() const;
    const unbounded_octstring<true>&        res_coordination_transfer_container() const;
    const drbs_modified_conf_list_l&        drbs_modified_conf_list() const;
    const unbounded_octstring<true>&        rrc_container() const;
    const crit_diagnostics_s&               crit_diagnostics() const;
    const execute_dupl_e&                   execute_dupl() const;
    const res_coordination_transfer_info_s& res_coordination_transfer_info() const;

  private:
    types type_;
    choice_buffer_t<crit_diagnostics_s,
                    drbs_modified_conf_list_l,
                    res_coordination_transfer_info_s,
                    unbounded_octstring<true>>
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

// UEContextModificationFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_mod_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_ue_f1ap_id();
    uint64_t&                 gnb_du_ue_f1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_ue_f1ap_id() const;
    const uint64_t&           gnb_du_ue_f1ap_id() const;
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

// UEContextModificationRefuseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_mod_refuse_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_ue_f1ap_id();
    uint64_t&                 gnb_du_ue_f1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_ue_f1ap_id() const;
    const uint64_t&           gnb_du_ue_f1ap_id() const;
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

// UEContextModificationRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_mod_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        sp_cell_id,
        serv_cell_idx,
        sp_cell_ul_cfgured,
        drx_cycle,
        cu_to_du_rrc_info,
        tx_action_ind,
        res_coordination_transfer_container,
        rrc_recfg_complete_ind,
        rrc_container,
        scell_to_be_setup_mod_list,
        scell_to_be_remd_list,
        srbs_to_be_setup_mod_list,
        drbs_to_be_setup_mod_list,
        drbs_to_be_modified_list,
        srbs_to_be_released_list,
        drbs_to_be_released_list,
        inactivity_monitoring_request,
        rat_freq_prio_info,
        drx_cfg_ind,
        rlc_fail_ind,
        ul_tx_direct_current_list_info,
        gnb_du_cfg_query,
        gnb_du_ue_ambr_ul,
        execute_dupl,
        rrc_delivery_status_request,
        res_coordination_transfer_info,
        serving_cell_mo,
        needfor_gap,
        full_cfg,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                               gnb_cu_ue_f1ap_id();
    uint64_t&                               gnb_du_ue_f1ap_id();
    nrcgi_s&                                sp_cell_id();
    uint8_t&                                serv_cell_idx();
    cell_ul_cfgured_e&                      sp_cell_ul_cfgured();
    drx_cycle_s&                            drx_cycle();
    cu_to_du_rrc_info_s&                    cu_to_du_rrc_info();
    tx_action_ind_e&                        tx_action_ind();
    unbounded_octstring<true>&              res_coordination_transfer_container();
    rrc_recfg_complete_ind_e&               rrc_recfg_complete_ind();
    unbounded_octstring<true>&              rrc_container();
    scell_to_be_setup_mod_list_l&           scell_to_be_setup_mod_list();
    scell_to_be_remd_list_l&                scell_to_be_remd_list();
    srbs_to_be_setup_mod_list_l&            srbs_to_be_setup_mod_list();
    drbs_to_be_setup_mod_list_l&            drbs_to_be_setup_mod_list();
    drbs_to_be_modified_list_l&             drbs_to_be_modified_list();
    srbs_to_be_released_list_l&             srbs_to_be_released_list();
    drbs_to_be_released_list_l&             drbs_to_be_released_list();
    inactivity_monitoring_request_e&        inactivity_monitoring_request();
    rat_freq_prio_info_c&                   rat_freq_prio_info();
    drx_cfg_ind_e&                          drx_cfg_ind();
    rlc_fail_ind_s&                         rlc_fail_ind();
    unbounded_octstring<true>&              ul_tx_direct_current_list_info();
    gnb_du_cfg_query_e&                     gnb_du_cfg_query();
    uint64_t&                               gnb_du_ue_ambr_ul();
    execute_dupl_e&                         execute_dupl();
    rrc_delivery_status_request_e&          rrc_delivery_status_request();
    res_coordination_transfer_info_s&       res_coordination_transfer_info();
    uint8_t&                                serving_cell_mo();
    needfor_gap_e&                          needfor_gap();
    full_cfg_e&                             full_cfg();
    const uint64_t&                         gnb_cu_ue_f1ap_id() const;
    const uint64_t&                         gnb_du_ue_f1ap_id() const;
    const nrcgi_s&                          sp_cell_id() const;
    const uint8_t&                          serv_cell_idx() const;
    const cell_ul_cfgured_e&                sp_cell_ul_cfgured() const;
    const drx_cycle_s&                      drx_cycle() const;
    const cu_to_du_rrc_info_s&              cu_to_du_rrc_info() const;
    const tx_action_ind_e&                  tx_action_ind() const;
    const unbounded_octstring<true>&        res_coordination_transfer_container() const;
    const rrc_recfg_complete_ind_e&         rrc_recfg_complete_ind() const;
    const unbounded_octstring<true>&        rrc_container() const;
    const scell_to_be_setup_mod_list_l&     scell_to_be_setup_mod_list() const;
    const scell_to_be_remd_list_l&          scell_to_be_remd_list() const;
    const srbs_to_be_setup_mod_list_l&      srbs_to_be_setup_mod_list() const;
    const drbs_to_be_setup_mod_list_l&      drbs_to_be_setup_mod_list() const;
    const drbs_to_be_modified_list_l&       drbs_to_be_modified_list() const;
    const srbs_to_be_released_list_l&       srbs_to_be_released_list() const;
    const drbs_to_be_released_list_l&       drbs_to_be_released_list() const;
    const inactivity_monitoring_request_e&  inactivity_monitoring_request() const;
    const rat_freq_prio_info_c&             rat_freq_prio_info() const;
    const drx_cfg_ind_e&                    drx_cfg_ind() const;
    const rlc_fail_ind_s&                   rlc_fail_ind() const;
    const unbounded_octstring<true>&        ul_tx_direct_current_list_info() const;
    const gnb_du_cfg_query_e&               gnb_du_cfg_query() const;
    const uint64_t&                         gnb_du_ue_ambr_ul() const;
    const execute_dupl_e&                   execute_dupl() const;
    const rrc_delivery_status_request_e&    rrc_delivery_status_request() const;
    const res_coordination_transfer_info_s& res_coordination_transfer_info() const;
    const uint8_t&                          serving_cell_mo() const;
    const needfor_gap_e&                    needfor_gap() const;
    const full_cfg_e&                       full_cfg() const;

  private:
    types type_;
    choice_buffer_t<cu_to_du_rrc_info_s,
                    drbs_to_be_modified_list_l,
                    drbs_to_be_released_list_l,
                    drbs_to_be_setup_mod_list_l,
                    drx_cycle_s,
                    nrcgi_s,
                    rat_freq_prio_info_c,
                    res_coordination_transfer_info_s,
                    rlc_fail_ind_s,
                    scell_to_be_remd_list_l,
                    scell_to_be_setup_mod_list_l,
                    srbs_to_be_released_list_l,
                    srbs_to_be_setup_mod_list_l,
                    unbounded_octstring<true>>
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

// UEContextModificationRequiredIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_mod_required_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        res_coordination_transfer_container,
        du_to_cu_rrc_info,
        drbs_required_to_be_modified_list,
        srbs_required_to_be_released_list,
        drbs_required_to_be_released_list,
        cause,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                  gnb_cu_ue_f1ap_id();
    uint64_t&                                  gnb_du_ue_f1ap_id();
    unbounded_octstring<true>&                 res_coordination_transfer_container();
    du_to_cu_rrc_info_s&                       du_to_cu_rrc_info();
    drbs_required_to_be_modified_list_l&       drbs_required_to_be_modified_list();
    srbs_required_to_be_released_list_l&       srbs_required_to_be_released_list();
    drbs_required_to_be_released_list_l&       drbs_required_to_be_released_list();
    cause_c&                                   cause();
    const uint64_t&                            gnb_cu_ue_f1ap_id() const;
    const uint64_t&                            gnb_du_ue_f1ap_id() const;
    const unbounded_octstring<true>&           res_coordination_transfer_container() const;
    const du_to_cu_rrc_info_s&                 du_to_cu_rrc_info() const;
    const drbs_required_to_be_modified_list_l& drbs_required_to_be_modified_list() const;
    const srbs_required_to_be_released_list_l& srbs_required_to_be_released_list() const;
    const drbs_required_to_be_released_list_l& drbs_required_to_be_released_list() const;
    const cause_c&                             cause() const;

  private:
    types type_;
    choice_buffer_t<cause_c,
                    drbs_required_to_be_modified_list_l,
                    drbs_required_to_be_released_list_l,
                    du_to_cu_rrc_info_s,
                    srbs_required_to_be_released_list_l,
                    unbounded_octstring<true>>
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

// UEContextModificationResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_mod_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        res_coordination_transfer_container,
        du_to_cu_rrc_info,
        drbs_setup_mod_list,
        drbs_modified_list,
        srbs_failed_to_be_setup_mod_list,
        drbs_failed_to_be_setup_mod_list,
        scell_failedto_setup_mod_list,
        drbs_failed_to_be_modified_list,
        inactivity_monitoring_resp,
        crit_diagnostics,
        c_rnti,
        associated_scell_list,
        srbs_setup_mod_list,
        srbs_modified_list,
        full_cfg,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                 gnb_cu_ue_f1ap_id();
    uint64_t&                                 gnb_du_ue_f1ap_id();
    unbounded_octstring<true>&                res_coordination_transfer_container();
    du_to_cu_rrc_info_s&                      du_to_cu_rrc_info();
    drbs_setup_mod_list_l&                    drbs_setup_mod_list();
    drbs_modified_list_l&                     drbs_modified_list();
    srbs_failed_to_be_setup_mod_list_l&       srbs_failed_to_be_setup_mod_list();
    drbs_failed_to_be_setup_mod_list_l&       drbs_failed_to_be_setup_mod_list();
    scell_failedto_setup_mod_list_l&          scell_failedto_setup_mod_list();
    drbs_failed_to_be_modified_list_l&        drbs_failed_to_be_modified_list();
    inactivity_monitoring_resp_e&             inactivity_monitoring_resp();
    crit_diagnostics_s&                       crit_diagnostics();
    uint32_t&                                 c_rnti();
    associated_scell_list_l&                  associated_scell_list();
    srbs_setup_mod_list_l&                    srbs_setup_mod_list();
    srbs_modified_list_l&                     srbs_modified_list();
    full_cfg_e&                               full_cfg();
    const uint64_t&                           gnb_cu_ue_f1ap_id() const;
    const uint64_t&                           gnb_du_ue_f1ap_id() const;
    const unbounded_octstring<true>&          res_coordination_transfer_container() const;
    const du_to_cu_rrc_info_s&                du_to_cu_rrc_info() const;
    const drbs_setup_mod_list_l&              drbs_setup_mod_list() const;
    const drbs_modified_list_l&               drbs_modified_list() const;
    const srbs_failed_to_be_setup_mod_list_l& srbs_failed_to_be_setup_mod_list() const;
    const drbs_failed_to_be_setup_mod_list_l& drbs_failed_to_be_setup_mod_list() const;
    const scell_failedto_setup_mod_list_l&    scell_failedto_setup_mod_list() const;
    const drbs_failed_to_be_modified_list_l&  drbs_failed_to_be_modified_list() const;
    const inactivity_monitoring_resp_e&       inactivity_monitoring_resp() const;
    const crit_diagnostics_s&                 crit_diagnostics() const;
    const uint32_t&                           c_rnti() const;
    const associated_scell_list_l&            associated_scell_list() const;
    const srbs_setup_mod_list_l&              srbs_setup_mod_list() const;
    const srbs_modified_list_l&               srbs_modified_list() const;
    const full_cfg_e&                         full_cfg() const;

  private:
    types type_;
    choice_buffer_t<associated_scell_list_l,
                    crit_diagnostics_s,
                    drbs_failed_to_be_modified_list_l,
                    drbs_failed_to_be_setup_mod_list_l,
                    drbs_modified_list_l,
                    drbs_setup_mod_list_l,
                    du_to_cu_rrc_info_s,
                    scell_failedto_setup_mod_list_l,
                    srbs_failed_to_be_setup_mod_list_l,
                    srbs_modified_list_l,
                    srbs_setup_mod_list_l,
                    unbounded_octstring<true>>
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

// UEContextReleaseCommandIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_release_cmd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        cause,
        rrc_container,
        srbid,
        old_gnb_du_ue_f1ap_id,
        execute_dupl,
        rrc_delivery_status_request,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                            gnb_cu_ue_f1ap_id();
    uint64_t&                            gnb_du_ue_f1ap_id();
    cause_c&                             cause();
    unbounded_octstring<true>&           rrc_container();
    uint8_t&                             srbid();
    uint64_t&                            old_gnb_du_ue_f1ap_id();
    execute_dupl_e&                      execute_dupl();
    rrc_delivery_status_request_e&       rrc_delivery_status_request();
    const uint64_t&                      gnb_cu_ue_f1ap_id() const;
    const uint64_t&                      gnb_du_ue_f1ap_id() const;
    const cause_c&                       cause() const;
    const unbounded_octstring<true>&     rrc_container() const;
    const uint8_t&                       srbid() const;
    const uint64_t&                      old_gnb_du_ue_f1ap_id() const;
    const execute_dupl_e&                execute_dupl() const;
    const rrc_delivery_status_request_e& rrc_delivery_status_request() const;

  private:
    types                                               type_;
    choice_buffer_t<cause_c, unbounded_octstring<true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextReleaseCompleteIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_release_complete_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_ue_f1ap_id();
    uint64_t&                 gnb_du_ue_f1ap_id();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_ue_f1ap_id() const;
    const uint64_t&           gnb_du_ue_f1ap_id() const;
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

// UEContextReleaseRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_release_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, cause, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&       gnb_cu_ue_f1ap_id();
    uint64_t&       gnb_du_ue_f1ap_id();
    cause_c&        cause();
    const uint64_t& gnb_cu_ue_f1ap_id() const;
    const uint64_t& gnb_du_ue_f1ap_id() const;
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

// UEContextSetupFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_setup_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        cause,
        crit_diagnostics,
        potential_sp_cell_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                       gnb_cu_ue_f1ap_id();
    uint64_t&                       gnb_du_ue_f1ap_id();
    cause_c&                        cause();
    crit_diagnostics_s&             crit_diagnostics();
    potential_sp_cell_list_l&       potential_sp_cell_list();
    const uint64_t&                 gnb_cu_ue_f1ap_id() const;
    const uint64_t&                 gnb_du_ue_f1ap_id() const;
    const cause_c&                  cause() const;
    const crit_diagnostics_s&       crit_diagnostics() const;
    const potential_sp_cell_list_l& potential_sp_cell_list() const;

  private:
    types                                                                  type_;
    choice_buffer_t<cause_c, crit_diagnostics_s, potential_sp_cell_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextSetupRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        sp_cell_id,
        serv_cell_idx,
        sp_cell_ul_cfgured,
        cu_to_du_rrc_info,
        candidate_sp_cell_list,
        drx_cycle,
        res_coordination_transfer_container,
        scell_to_be_setup_list,
        srbs_to_be_setup_list,
        drbs_to_be_setup_list,
        inactivity_monitoring_request,
        rat_freq_prio_info,
        rrc_container,
        masked_imeisv,
        serving_plmn,
        gnb_du_ue_ambr_ul,
        rrc_delivery_status_request,
        res_coordination_transfer_info,
        serving_cell_mo,
        new_gnb_cu_ue_f1ap_id,
        ranueid,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                               gnb_cu_ue_f1ap_id();
    uint64_t&                               gnb_du_ue_f1ap_id();
    nrcgi_s&                                sp_cell_id();
    uint8_t&                                serv_cell_idx();
    cell_ul_cfgured_e&                      sp_cell_ul_cfgured();
    cu_to_du_rrc_info_s&                    cu_to_du_rrc_info();
    candidate_sp_cell_list_l&               candidate_sp_cell_list();
    drx_cycle_s&                            drx_cycle();
    unbounded_octstring<true>&              res_coordination_transfer_container();
    scell_to_be_setup_list_l&               scell_to_be_setup_list();
    srbs_to_be_setup_list_l&                srbs_to_be_setup_list();
    drbs_to_be_setup_list_l&                drbs_to_be_setup_list();
    inactivity_monitoring_request_e&        inactivity_monitoring_request();
    rat_freq_prio_info_c&                   rat_freq_prio_info();
    unbounded_octstring<true>&              rrc_container();
    fixed_bitstring<64, false, true>&       masked_imeisv();
    fixed_octstring<3, true>&               serving_plmn();
    uint64_t&                               gnb_du_ue_ambr_ul();
    rrc_delivery_status_request_e&          rrc_delivery_status_request();
    res_coordination_transfer_info_s&       res_coordination_transfer_info();
    uint8_t&                                serving_cell_mo();
    uint64_t&                               new_gnb_cu_ue_f1ap_id();
    fixed_octstring<8, true>&               ranueid();
    const uint64_t&                         gnb_cu_ue_f1ap_id() const;
    const uint64_t&                         gnb_du_ue_f1ap_id() const;
    const nrcgi_s&                          sp_cell_id() const;
    const uint8_t&                          serv_cell_idx() const;
    const cell_ul_cfgured_e&                sp_cell_ul_cfgured() const;
    const cu_to_du_rrc_info_s&              cu_to_du_rrc_info() const;
    const candidate_sp_cell_list_l&         candidate_sp_cell_list() const;
    const drx_cycle_s&                      drx_cycle() const;
    const unbounded_octstring<true>&        res_coordination_transfer_container() const;
    const scell_to_be_setup_list_l&         scell_to_be_setup_list() const;
    const srbs_to_be_setup_list_l&          srbs_to_be_setup_list() const;
    const drbs_to_be_setup_list_l&          drbs_to_be_setup_list() const;
    const inactivity_monitoring_request_e&  inactivity_monitoring_request() const;
    const rat_freq_prio_info_c&             rat_freq_prio_info() const;
    const unbounded_octstring<true>&        rrc_container() const;
    const fixed_bitstring<64, false, true>& masked_imeisv() const;
    const fixed_octstring<3, true>&         serving_plmn() const;
    const uint64_t&                         gnb_du_ue_ambr_ul() const;
    const rrc_delivery_status_request_e&    rrc_delivery_status_request() const;
    const res_coordination_transfer_info_s& res_coordination_transfer_info() const;
    const uint8_t&                          serving_cell_mo() const;
    const uint64_t&                         new_gnb_cu_ue_f1ap_id() const;
    const fixed_octstring<8, true>&         ranueid() const;

  private:
    types type_;
    choice_buffer_t<candidate_sp_cell_list_l,
                    cu_to_du_rrc_info_s,
                    drbs_to_be_setup_list_l,
                    drx_cycle_s,
                    fixed_bitstring<64, false, true>,
                    fixed_octstring<3, true>,
                    fixed_octstring<8, true>,
                    nrcgi_s,
                    rat_freq_prio_info_c,
                    res_coordination_transfer_info_s,
                    scell_to_be_setup_list_l,
                    srbs_to_be_setup_list_l,
                    unbounded_octstring<true>>
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

// UEContextSetupResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        du_to_cu_rrc_info,
        c_rnti,
        res_coordination_transfer_container,
        full_cfg,
        drbs_setup_list,
        srbs_failed_to_be_setup_list,
        drbs_failed_to_be_setup_list,
        scell_failedto_setup_list,
        inactivity_monitoring_resp,
        crit_diagnostics,
        srbs_setup_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                             gnb_cu_ue_f1ap_id();
    uint64_t&                             gnb_du_ue_f1ap_id();
    du_to_cu_rrc_info_s&                  du_to_cu_rrc_info();
    uint32_t&                             c_rnti();
    unbounded_octstring<true>&            res_coordination_transfer_container();
    full_cfg_e&                           full_cfg();
    drbs_setup_list_l&                    drbs_setup_list();
    srbs_failed_to_be_setup_list_l&       srbs_failed_to_be_setup_list();
    drbs_failed_to_be_setup_list_l&       drbs_failed_to_be_setup_list();
    scell_failedto_setup_list_l&          scell_failedto_setup_list();
    inactivity_monitoring_resp_e&         inactivity_monitoring_resp();
    crit_diagnostics_s&                   crit_diagnostics();
    srbs_setup_list_l&                    srbs_setup_list();
    const uint64_t&                       gnb_cu_ue_f1ap_id() const;
    const uint64_t&                       gnb_du_ue_f1ap_id() const;
    const du_to_cu_rrc_info_s&            du_to_cu_rrc_info() const;
    const uint32_t&                       c_rnti() const;
    const unbounded_octstring<true>&      res_coordination_transfer_container() const;
    const full_cfg_e&                     full_cfg() const;
    const drbs_setup_list_l&              drbs_setup_list() const;
    const srbs_failed_to_be_setup_list_l& srbs_failed_to_be_setup_list() const;
    const drbs_failed_to_be_setup_list_l& drbs_failed_to_be_setup_list() const;
    const scell_failedto_setup_list_l&    scell_failedto_setup_list() const;
    const inactivity_monitoring_resp_e&   inactivity_monitoring_resp() const;
    const crit_diagnostics_s&             crit_diagnostics() const;
    const srbs_setup_list_l&              srbs_setup_list() const;

  private:
    types type_;
    choice_buffer_t<crit_diagnostics_s,
                    drbs_failed_to_be_setup_list_l,
                    drbs_setup_list_l,
                    du_to_cu_rrc_info_s,
                    scell_failedto_setup_list_l,
                    srbs_failed_to_be_setup_list_l,
                    srbs_setup_list_l,
                    unbounded_octstring<true>>
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

// UEInactivityNotificationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_inactivity_notif_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, drb_activity_list, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                  gnb_cu_ue_f1ap_id();
    uint64_t&                  gnb_du_ue_f1ap_id();
    drb_activity_list_l&       drb_activity_list();
    const uint64_t&            gnb_cu_ue_f1ap_id() const;
    const uint64_t&            gnb_du_ue_f1ap_id() const;
    const drb_activity_list_l& drb_activity_list() const;

  private:
    types                                type_;
    choice_buffer_t<drb_activity_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ULRRCMessageTransferIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ul_rrc_msg_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        srbid,
        rrc_container,
        sel_plmn_id,
        new_gnb_du_ue_f1ap_id,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                        gnb_cu_ue_f1ap_id();
    uint64_t&                        gnb_du_ue_f1ap_id();
    uint8_t&                         srbid();
    unbounded_octstring<true>&       rrc_container();
    fixed_octstring<3, true>&        sel_plmn_id();
    uint64_t&                        new_gnb_du_ue_f1ap_id();
    const uint64_t&                  gnb_cu_ue_f1ap_id() const;
    const uint64_t&                  gnb_du_ue_f1ap_id() const;
    const uint8_t&                   srbid() const;
    const unbounded_octstring<true>& rrc_container() const;
    const fixed_octstring<3, true>&  sel_plmn_id() const;
    const uint64_t&                  new_gnb_du_ue_f1ap_id() const;

  private:
    types                                                                type_;
    choice_buffer_t<fixed_octstring<3, true>, unbounded_octstring<true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// WriteReplaceWarningRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct write_replace_warning_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        pws_sys_info,
        repeat_period,
        numof_broadcast_request,
        cells_to_be_broadcast_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    pws_sys_info_s&                     pws_sys_info();
    uint32_t&                           repeat_period();
    uint32_t&                           numof_broadcast_request();
    cells_to_be_broadcast_list_l&       cells_to_be_broadcast_list();
    const uint16_t&                     transaction_id() const;
    const pws_sys_info_s&               pws_sys_info() const;
    const uint32_t&                     repeat_period() const;
    const uint32_t&                     numof_broadcast_request() const;
    const cells_to_be_broadcast_list_l& cells_to_be_broadcast_list() const;

  private:
    types                                                         type_;
    choice_buffer_t<cells_to_be_broadcast_list_l, pws_sys_info_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// WriteReplaceWarningResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct write_replace_warning_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        cells_broadcast_completed_list,
        crit_diagnostics,
        ded_si_delivery_needed_ue_list,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    cells_broadcast_completed_list_l&       cells_broadcast_completed_list();
    crit_diagnostics_s&                     crit_diagnostics();
    ded_si_delivery_needed_ue_list_l&       ded_si_delivery_needed_ue_list();
    const uint16_t&                         transaction_id() const;
    const cells_broadcast_completed_list_l& cells_broadcast_completed_list() const;
    const crit_diagnostics_s&               crit_diagnostics() const;
    const ded_si_delivery_needed_ue_list_l& ded_si_delivery_needed_ue_list() const;

  private:
    types                                                                                                   type_;
    choice_buffer_t<cells_broadcast_completed_list_l, crit_diagnostics_s, ded_si_delivery_needed_ue_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct f1_removal_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<cause_c>                               cause;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;

  // sequence methods
  f1_removal_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// F1RemovalFailure ::= SEQUENCE
using f1_removal_fail_s = elementary_procedure_option<f1_removal_fail_ies_container>;

// F1RemovalRequest ::= SEQUENCE
using f1_removal_request_s = elementary_procedure_option<protocol_ie_container_l<f1_removal_request_ies_o>>;

struct f1_removal_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;

  // sequence methods
  f1_removal_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// F1RemovalResponse ::= SEQUENCE
using f1_removal_resp_s = elementary_procedure_option<f1_removal_resp_ies_container>;

struct f1_setup_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              time_to_wait_present     = false;
  bool                                              crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<cause_c>                               cause;
  ie_field_s<time_to_wait_e>                        time_to_wait;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;

  // sequence methods
  f1_setup_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// F1SetupFailure ::= SEQUENCE
using f1_setup_fail_s = elementary_procedure_option<f1_setup_fail_ies_container>;

struct f1_setup_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       gnb_du_name_present              = false;
  bool                                                       gnb_du_served_cells_list_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>          transaction_id;
  ie_field_s<integer<uint64_t, 0, 68719476735, false, true>> gnb_du_id;
  ie_field_s<printable_string<1, 150, true, true>>           gnb_du_name;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<gnb_du_served_cells_item_ies_o>, 1, 512, true>>
                            gnb_du_served_cells_list;
  ie_field_s<rrc_version_s> gnb_du_rrc_version;

  // sequence methods
  f1_setup_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// F1SetupRequest ::= SEQUENCE
using f1_setup_request_s = elementary_procedure_option<f1_setup_request_ies_container>;

struct f1_setup_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              gnb_cu_name_present            = false;
  bool                                              cells_to_be_activ_list_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<printable_string<1, 150, true, true>>  gnb_cu_name;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_to_be_activ_list_item_ies_o>, 1, 512, true>>
                            cells_to_be_activ_list;
  ie_field_s<rrc_version_s> gnb_cu_rrc_version;

  // sequence methods
  f1_setup_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// F1SetupResponse ::= SEQUENCE
using f1_setup_resp_s = elementary_procedure_option<f1_setup_resp_ies_container>;

struct gnb_cu_cfg_upd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              cells_to_be_activ_list_present       = false;
  bool                                              cells_to_be_deactiv_list_present     = false;
  bool                                              gnb_cu_tnl_assoc_to_add_list_present = false;
  bool                                              gnb_cu_tnl_assoc_to_rem_list_present = false;
  bool                                              gnb_cu_tnl_assoc_to_upd_list_present = false;
  bool                                              cells_to_be_barred_list_present      = false;
  bool                                              protected_eutra_res_list_present     = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_to_be_activ_list_item_ies_o>, 1, 512, true>>
      cells_to_be_activ_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_to_be_deactiv_list_item_ies_o>, 1, 512, true>>
      cells_to_be_deactiv_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<gnb_cu_tnl_assoc_to_add_item_ies_o>, 1, 32, true>>
      gnb_cu_tnl_assoc_to_add_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<gnb_cu_tnl_assoc_to_rem_item_ies_o>, 1, 32, true>>
      gnb_cu_tnl_assoc_to_rem_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<gnb_cu_tnl_assoc_to_upd_item_ies_o>, 1, 32, true>>
      gnb_cu_tnl_assoc_to_upd_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_to_be_barred_item_ies_o>, 1, 512, true>>
      cells_to_be_barred_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<protected_eutra_res_item_ies_o>, 1, 256, true>>
      protected_eutra_res_list;

  // sequence methods
  gnb_cu_cfg_upd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBCUConfigurationUpdate ::= SEQUENCE
using gnb_cu_cfg_upd_s = elementary_procedure_option<gnb_cu_cfg_upd_ies_container>;

struct gnb_cu_cfg_upd_ack_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              cells_failed_to_be_activ_list_present         = false;
  bool                                              crit_diagnostics_present                      = false;
  bool                                              gnb_cu_tnl_assoc_setup_list_present           = false;
  bool                                              gnb_cu_tnl_assoc_failed_to_setup_list_present = false;
  bool                                              ded_si_delivery_needed_ue_list_present        = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_failed_to_be_activ_list_item_ies_o>, 1, 512, true>>
                                 cells_failed_to_be_activ_list;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<gnb_cu_tnl_assoc_setup_item_ies_o>, 1, 32, true>>
      gnb_cu_tnl_assoc_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<gnb_cu_tnl_assoc_failed_to_setup_item_ies_o>, 1, 32, true>>
      gnb_cu_tnl_assoc_failed_to_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ded_si_delivery_needed_ue_item_ies_o>, 1, 65536, true>>
      ded_si_delivery_needed_ue_list;

  // sequence methods
  gnb_cu_cfg_upd_ack_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBCUConfigurationUpdateAcknowledge ::= SEQUENCE
using gnb_cu_cfg_upd_ack_s = elementary_procedure_option<gnb_cu_cfg_upd_ack_ies_container>;

struct gnb_cu_cfg_upd_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              time_to_wait_present     = false;
  bool                                              crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<cause_c>                               cause;
  ie_field_s<time_to_wait_e>                        time_to_wait;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;

  // sequence methods
  gnb_cu_cfg_upd_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBCUConfigurationUpdateFailure ::= SEQUENCE
using gnb_cu_cfg_upd_fail_s = elementary_procedure_option<gnb_cu_cfg_upd_fail_ies_container>;

struct gnb_du_cfg_upd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              served_cells_to_add_list_present       = false;
  bool                                              served_cells_to_modify_list_present    = false;
  bool                                              served_cells_to_delete_list_present    = false;
  bool                                              cells_status_list_present              = false;
  bool                                              ded_si_delivery_needed_ue_list_present = false;
  bool                                              gnb_du_id_present                      = false;
  bool                                              gnb_du_tnl_assoc_to_rem_list_present   = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<served_cells_to_add_item_ies_o>, 1, 512, true>>
      served_cells_to_add_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<served_cells_to_modify_item_ies_o>, 1, 512, true>>
      served_cells_to_modify_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<served_cells_to_delete_item_ies_o>, 1, 512, true>>
      served_cells_to_delete_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_status_item_ies_o>, 0, 512, true>> cells_status_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ded_si_delivery_needed_ue_item_ies_o>, 1, 65536, true>>
                                                             ded_si_delivery_needed_ue_list;
  ie_field_s<integer<uint64_t, 0, 68719476735, false, true>> gnb_du_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<gnb_du_tnl_assoc_to_rem_item_ies_o>, 1, 32, true>>
      gnb_du_tnl_assoc_to_rem_list;

  // sequence methods
  gnb_du_cfg_upd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUConfigurationUpdate ::= SEQUENCE
using gnb_du_cfg_upd_s = elementary_procedure_option<gnb_du_cfg_upd_ies_container>;

struct gnb_du_cfg_upd_ack_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              cells_to_be_activ_list_present   = false;
  bool                                              crit_diagnostics_present         = false;
  bool                                              cells_to_be_deactiv_list_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_to_be_activ_list_item_ies_o>, 1, 512, true>>
                                 cells_to_be_activ_list;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_to_be_deactiv_list_item_ies_o>, 1, 512, true>>
      cells_to_be_deactiv_list;

  // sequence methods
  gnb_du_cfg_upd_ack_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUConfigurationUpdateAcknowledge ::= SEQUENCE
using gnb_du_cfg_upd_ack_s = elementary_procedure_option<gnb_du_cfg_upd_ack_ies_container>;

struct gnb_du_cfg_upd_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              time_to_wait_present     = false;
  bool                                              crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<cause_c>                               cause;
  ie_field_s<time_to_wait_e>                        time_to_wait;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;

  // sequence methods
  gnb_du_cfg_upd_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUConfigurationUpdateFailure ::= SEQUENCE
using gnb_du_cfg_upd_fail_s = elementary_procedure_option<gnb_du_cfg_upd_fail_ies_container>;

struct gnb_du_res_coordination_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              ignore_res_coordination_container_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<request_type_e>                        request_type;
  ie_field_s<unbounded_octstring<true>>             eutra_nr_cell_res_coordination_req_container;
  ie_field_s<ignore_res_coordination_container_e>   ignore_res_coordination_container;

  // sequence methods
  gnb_du_res_coordination_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUResourceCoordinationRequest ::= SEQUENCE
using gnb_du_res_coordination_request_s = elementary_procedure_option<gnb_du_res_coordination_request_ies_container>;

struct gnb_du_res_coordination_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<unbounded_octstring<true>>             eutra_nr_cell_res_coordination_req_ack_container;

  // sequence methods
  gnb_du_res_coordination_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUResourceCoordinationResponse ::= SEQUENCE
using gnb_du_res_coordination_resp_s = elementary_procedure_option<gnb_du_res_coordination_resp_ies_container>;

struct gnb_du_status_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<gnb_du_overload_info_e>                gnb_du_overload_info;

  // sequence methods
  gnb_du_status_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUStatusIndication ::= SEQUENCE
using gnb_du_status_ind_s = elementary_procedure_option<gnb_du_status_ind_ies_container>;

struct init_ul_rrc_msg_transfer_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      du_to_cu_rrc_container_present           = false;
  bool                                                      sul_access_ind_present                   = false;
  bool                                                      ranueid_present                          = false;
  bool                                                      rrc_container_rrc_setup_complete_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<nrcgi_s>                                       nrcgi;
  ie_field_s<integer<uint32_t, 0, 65535, true, true>>       c_rnti;
  ie_field_s<unbounded_octstring<true>>                     rrc_container;
  ie_field_s<unbounded_octstring<true>>                     du_to_cu_rrc_container;
  ie_field_s<sul_access_ind_e>                              sul_access_ind;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>         transaction_id;
  ie_field_s<fixed_octstring<8, true>>                      ranueid;
  ie_field_s<unbounded_octstring<true>>                     rrc_container_rrc_setup_complete;

  // sequence methods
  init_ul_rrc_msg_transfer_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InitialULRRCMessageTransfer ::= SEQUENCE
using init_ul_rrc_msg_transfer_s = elementary_procedure_option<init_ul_rrc_msg_transfer_ies_container>;

struct network_access_rate_reduction_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<uac_assist_info_s>                     uac_assist_info;

  // sequence methods
  network_access_rate_reduction_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NetworkAccessRateReduction ::= SEQUENCE
using network_access_rate_reduction_s = elementary_procedure_option<network_access_rate_reduction_ies_container>;

struct notify_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>>                                  gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>>                                  gnb_du_ue_f1ap_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drb_notify_item_ies_o>, 1, 64, true>> drb_notify_list;

  // sequence methods
  notify_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Notify ::= SEQUENCE
using notify_s = elementary_procedure_option<notify_ies_container>;

struct pws_cancel_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                 broadcast_to_be_cancelled_list_present = false;
  bool                                                 cancel_all_warning_msgs_ind_present    = false;
  bool                                                 notif_info_present                     = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>    transaction_id;
  ie_field_s<integer<uint32_t, 0, 65535, false, true>> numof_broadcast_request;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<broadcast_to_be_cancelled_list_item_ies_o>, 1, 512, true>>
                                            broadcast_to_be_cancelled_list;
  ie_field_s<cancel_all_warning_msgs_ind_e> cancel_all_warning_msgs_ind;
  ie_field_s<notif_info_s>                  notif_info;

  // sequence methods
  pws_cancel_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSCancelRequest ::= SEQUENCE
using pws_cancel_request_s = elementary_procedure_option<pws_cancel_request_ies_container>;

struct pws_cancel_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              cells_broadcast_cancelled_list_present = false;
  bool                                              crit_diagnostics_present               = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_broadcast_cancelled_list_item_ies_o>, 1, 512, true>>
                                 cells_broadcast_cancelled_list;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;

  // sequence methods
  pws_cancel_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSCancelResponse ::= SEQUENCE
using pws_cancel_resp_s = elementary_procedure_option<pws_cancel_resp_ies_container>;

struct pws_fail_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              pws_failed_nr_cgi_list_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<pws_failed_nr_cgi_list_item_ies_o>, 1, 512, true>>
      pws_failed_nr_cgi_list;

  // sequence methods
  pws_fail_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSFailureIndication ::= SEQUENCE
using pws_fail_ind_s = elementary_procedure_option<pws_fail_ind_ies_container>;

struct pws_restart_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<nr_cgi_list_for_restart_list_item_ies_o>, 1, 512, true>>
      nr_cgi_list_for_restart_list;

  // sequence methods
  pws_restart_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSRestartIndication ::= SEQUENCE
using pws_restart_ind_s = elementary_procedure_option<pws_restart_ind_ies_container>;

struct paging_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                          paging_drx_present    = false;
  bool                          paging_prio_present   = false;
  bool                          paging_origin_present = false;
  ie_field_s<ue_id_idx_value_c> ue_id_idx_value;
  ie_field_s<paging_id_c>       paging_id;
  ie_field_s<paging_drx_e>      paging_drx;
  ie_field_s<paging_prio_e>     paging_prio;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<paging_cell_item_ies_o>, 1, 512, true>> paging_cell_list;
  ie_field_s<paging_origin_e>                                                                  paging_origin;

  // sequence methods
  paging_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Paging ::= SEQUENCE
using paging_s = elementary_procedure_option<paging_ies_container>;

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
  template <class valueT_>
  using ie_field_s = private_ie_container_item_s<valueT_>;

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

struct rrc_delivery_report_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<rrc_delivery_status_s>                         rrc_delivery_status;
  ie_field_s<integer<uint8_t, 0, 3, true, true>>            srbid;

  // sequence methods
  rrc_delivery_report_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCDeliveryReport ::= SEQUENCE
using rrc_delivery_report_s = elementary_procedure_option<rrc_delivery_report_ies_container>;

struct reset_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<cause_c>                               cause;
  ie_field_s<reset_type_c>                          reset_type;

  // sequence methods
  reset_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Reset ::= SEQUENCE
using reset_s = elementary_procedure_option<reset_ies_container>;

struct reset_ack_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              ue_associated_lc_f1_conn_list_res_ack_present = false;
  bool                                              crit_diagnostics_present                      = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ue_associated_lc_f1_conn_item_res_ack_o>, 1, 65536, true>>
                                 ue_associated_lc_f1_conn_list_res_ack;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;

  // sequence methods
  reset_ack_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResetAcknowledge ::= SEQUENCE
using reset_ack_s = elementary_procedure_option<reset_ack_ies_container>;

struct sys_info_delivery_cmd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>>         transaction_id;
  ie_field_s<nrcgi_s>                                       nrcgi;
  ie_field_s<dyn_seq_of<sitype_item_s, 1, 32, true>>        sitype_list;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> confirmed_ue_id;

  // sequence methods
  sys_info_delivery_cmd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationDeliveryCommand ::= SEQUENCE
using sys_info_delivery_cmd_s = elementary_procedure_option<sys_info_delivery_cmd_ies_container>;

struct ue_context_mod_confirm_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      res_coordination_transfer_container_present = false;
  bool                                                      drbs_modified_conf_list_present             = false;
  bool                                                      rrc_container_present                       = false;
  bool                                                      crit_diagnostics_present                    = false;
  bool                                                      execute_dupl_present                        = false;
  bool                                                      res_coordination_transfer_info_present      = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<unbounded_octstring<true>>                     res_coordination_transfer_container;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_modified_conf_item_ies_o>, 1, 64, true>>
                                               drbs_modified_conf_list;
  ie_field_s<unbounded_octstring<true>>        rrc_container;
  ie_field_s<crit_diagnostics_s>               crit_diagnostics;
  ie_field_s<execute_dupl_e>                   execute_dupl;
  ie_field_s<res_coordination_transfer_info_s> res_coordination_transfer_info;

  // sequence methods
  ue_context_mod_confirm_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationConfirm ::= SEQUENCE
using ue_context_mod_confirm_s = elementary_procedure_option<ue_context_mod_confirm_ies_container>;

struct ue_context_mod_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<cause_c>                                       cause;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  ue_context_mod_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationFailure ::= SEQUENCE
using ue_context_mod_fail_s = elementary_procedure_option<ue_context_mod_fail_ies_container>;

struct ue_context_mod_refuse_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<cause_c>                                       cause;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  ue_context_mod_refuse_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationRefuse ::= SEQUENCE
using ue_context_mod_refuse_s = elementary_procedure_option<ue_context_mod_refuse_ies_container>;

struct ue_context_mod_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      sp_cell_id_present                          = false;
  bool                                                      serv_cell_idx_present                       = false;
  bool                                                      sp_cell_ul_cfgured_present                  = false;
  bool                                                      drx_cycle_present                           = false;
  bool                                                      cu_to_du_rrc_info_present                   = false;
  bool                                                      tx_action_ind_present                       = false;
  bool                                                      res_coordination_transfer_container_present = false;
  bool                                                      rrc_recfg_complete_ind_present              = false;
  bool                                                      rrc_container_present                       = false;
  bool                                                      scell_to_be_setup_mod_list_present          = false;
  bool                                                      scell_to_be_remd_list_present               = false;
  bool                                                      srbs_to_be_setup_mod_list_present           = false;
  bool                                                      drbs_to_be_setup_mod_list_present           = false;
  bool                                                      drbs_to_be_modified_list_present            = false;
  bool                                                      srbs_to_be_released_list_present            = false;
  bool                                                      drbs_to_be_released_list_present            = false;
  bool                                                      inactivity_monitoring_request_present       = false;
  bool                                                      rat_freq_prio_info_present                  = false;
  bool                                                      drx_cfg_ind_present                         = false;
  bool                                                      rlc_fail_ind_present                        = false;
  bool                                                      ul_tx_direct_current_list_info_present      = false;
  bool                                                      gnb_du_cfg_query_present                    = false;
  bool                                                      gnb_du_ue_ambr_ul_present                   = false;
  bool                                                      execute_dupl_present                        = false;
  bool                                                      rrc_delivery_status_request_present         = false;
  bool                                                      res_coordination_transfer_info_present      = false;
  bool                                                      serving_cell_mo_present                     = false;
  bool                                                      needfor_gap_present                         = false;
  bool                                                      full_cfg_present                            = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<nrcgi_s>                                       sp_cell_id;
  ie_field_s<integer<uint8_t, 0, 31, true, true>>           serv_cell_idx;
  ie_field_s<cell_ul_cfgured_e>                             sp_cell_ul_cfgured;
  ie_field_s<drx_cycle_s>                                   drx_cycle;
  ie_field_s<cu_to_du_rrc_info_s>                           cu_to_du_rrc_info;
  ie_field_s<tx_action_ind_e>                               tx_action_ind;
  ie_field_s<unbounded_octstring<true>>                     res_coordination_transfer_container;
  ie_field_s<rrc_recfg_complete_ind_e>                      rrc_recfg_complete_ind;
  ie_field_s<unbounded_octstring<true>>                     rrc_container;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<scell_to_be_setup_mod_item_ies_o>, 1, 32, true>>
      scell_to_be_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<scell_to_be_remd_item_ies_o>, 1, 32, true>>
      scell_to_be_remd_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<srbs_to_be_setup_mod_item_ies_o>, 1, 8, true>>
      srbs_to_be_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_to_be_setup_mod_item_ies_o>, 1, 64, true>>
      drbs_to_be_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_to_be_modified_item_ies_o>, 1, 64, true>>
      drbs_to_be_modified_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<srbs_to_be_released_item_ies_o>, 1, 8, true>>
      srbs_to_be_released_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_to_be_released_item_ies_o>, 1, 64, true>>
                                                              drbs_to_be_released_list;
  ie_field_s<inactivity_monitoring_request_e>                 inactivity_monitoring_request;
  ie_field_s<rat_freq_prio_info_c>                            rat_freq_prio_info;
  ie_field_s<drx_cfg_ind_e>                                   drx_cfg_ind;
  ie_field_s<rlc_fail_ind_s>                                  rlc_fail_ind;
  ie_field_s<unbounded_octstring<true>>                       ul_tx_direct_current_list_info;
  ie_field_s<gnb_du_cfg_query_e>                              gnb_du_cfg_query;
  ie_field_s<integer<uint64_t, 0, 4000000000000, true, true>> gnb_du_ue_ambr_ul;
  ie_field_s<execute_dupl_e>                                  execute_dupl;
  ie_field_s<rrc_delivery_status_request_e>                   rrc_delivery_status_request;
  ie_field_s<res_coordination_transfer_info_s>                res_coordination_transfer_info;
  ie_field_s<integer<uint8_t, 1, 64, true, true>>             serving_cell_mo;
  ie_field_s<needfor_gap_e>                                   needfor_gap;
  ie_field_s<full_cfg_e>                                      full_cfg;

  // sequence methods
  ue_context_mod_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationRequest ::= SEQUENCE
using ue_context_mod_request_s = elementary_procedure_option<ue_context_mod_request_ies_container>;

struct ue_context_mod_required_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      res_coordination_transfer_container_present = false;
  bool                                                      du_to_cu_rrc_info_present                   = false;
  bool                                                      drbs_required_to_be_modified_list_present   = false;
  bool                                                      srbs_required_to_be_released_list_present   = false;
  bool                                                      drbs_required_to_be_released_list_present   = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<unbounded_octstring<true>>                     res_coordination_transfer_container;
  ie_field_s<du_to_cu_rrc_info_s>                           du_to_cu_rrc_info;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_required_to_be_modified_item_ies_o>, 1, 64, true>>
      drbs_required_to_be_modified_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<srbs_required_to_be_released_item_ies_o>, 1, 8, true>>
      srbs_required_to_be_released_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_required_to_be_released_item_ies_o>, 1, 64, true>>
                      drbs_required_to_be_released_list;
  ie_field_s<cause_c> cause;

  // sequence methods
  ue_context_mod_required_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationRequired ::= SEQUENCE
using ue_context_mod_required_s = elementary_procedure_option<ue_context_mod_required_ies_container>;

struct ue_context_mod_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      res_coordination_transfer_container_present = false;
  bool                                                      du_to_cu_rrc_info_present                   = false;
  bool                                                      drbs_setup_mod_list_present                 = false;
  bool                                                      drbs_modified_list_present                  = false;
  bool                                                      srbs_failed_to_be_setup_mod_list_present    = false;
  bool                                                      drbs_failed_to_be_setup_mod_list_present    = false;
  bool                                                      scell_failedto_setup_mod_list_present       = false;
  bool                                                      drbs_failed_to_be_modified_list_present     = false;
  bool                                                      inactivity_monitoring_resp_present          = false;
  bool                                                      crit_diagnostics_present                    = false;
  bool                                                      c_rnti_present                              = false;
  bool                                                      associated_scell_list_present               = false;
  bool                                                      srbs_setup_mod_list_present                 = false;
  bool                                                      srbs_modified_list_present                  = false;
  bool                                                      full_cfg_present                            = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<unbounded_octstring<true>>                     res_coordination_transfer_container;
  ie_field_s<du_to_cu_rrc_info_s>                           du_to_cu_rrc_info;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_setup_mod_item_ies_o>, 1, 64, true>> drbs_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_modified_item_ies_o>, 1, 64, true>>  drbs_modified_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<srbs_failed_to_be_setup_mod_item_ies_o>, 1, 8, true>>
      srbs_failed_to_be_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_failed_to_be_setup_mod_item_ies_o>, 1, 64, true>>
      drbs_failed_to_be_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<scell_failedto_setup_mod_item_ies_o>, 1, 32, true>>
      scell_failedto_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_failed_to_be_modified_item_ies_o>, 1, 64, true>>
                                                      drbs_failed_to_be_modified_list;
  ie_field_s<inactivity_monitoring_resp_e>            inactivity_monitoring_resp;
  ie_field_s<crit_diagnostics_s>                      crit_diagnostics;
  ie_field_s<integer<uint32_t, 0, 65535, true, true>> c_rnti;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<associated_scell_item_ies_o>, 1, 32, true>>
                                                                                                associated_scell_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<srbs_setup_mod_item_ies_o>, 1, 8, true>> srbs_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<srbs_modified_item_ies_o>, 1, 8, true>>  srbs_modified_list;
  ie_field_s<full_cfg_e>                                                                        full_cfg;

  // sequence methods
  ue_context_mod_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationResponse ::= SEQUENCE
using ue_context_mod_resp_s = elementary_procedure_option<ue_context_mod_resp_ies_container>;

struct ue_context_release_cmd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      rrc_container_present               = false;
  bool                                                      srbid_present                       = false;
  bool                                                      old_gnb_du_ue_f1ap_id_present       = false;
  bool                                                      execute_dupl_present                = false;
  bool                                                      rrc_delivery_status_request_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<cause_c>                                       cause;
  ie_field_s<unbounded_octstring<true>>                     rrc_container;
  ie_field_s<integer<uint8_t, 0, 3, true, true>>            srbid;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> old_gnb_du_ue_f1ap_id;
  ie_field_s<execute_dupl_e>                                execute_dupl;
  ie_field_s<rrc_delivery_status_request_e>                 rrc_delivery_status_request;

  // sequence methods
  ue_context_release_cmd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextReleaseCommand ::= SEQUENCE
using ue_context_release_cmd_s = elementary_procedure_option<ue_context_release_cmd_ies_container>;

struct ue_context_release_complete_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  ue_context_release_complete_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextReleaseComplete ::= SEQUENCE
using ue_context_release_complete_s = elementary_procedure_option<ue_context_release_complete_ies_container>;

struct ue_context_release_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<cause_c>                                       cause;

  // sequence methods
  ue_context_release_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextReleaseRequest ::= SEQUENCE
using ue_context_release_request_s = elementary_procedure_option<ue_context_release_request_ies_container>;

struct ue_context_setup_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      gnb_du_ue_f1ap_id_present      = false;
  bool                                                      crit_diagnostics_present       = false;
  bool                                                      potential_sp_cell_list_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<cause_c>                                       cause;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<potential_sp_cell_item_ies_o>, 0, 64, true>>
      potential_sp_cell_list;

  // sequence methods
  ue_context_setup_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextSetupFailure ::= SEQUENCE
using ue_context_setup_fail_s = elementary_procedure_option<ue_context_setup_fail_ies_container>;

struct ue_context_setup_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      gnb_du_ue_f1ap_id_present                   = false;
  bool                                                      sp_cell_ul_cfgured_present                  = false;
  bool                                                      candidate_sp_cell_list_present              = false;
  bool                                                      drx_cycle_present                           = false;
  bool                                                      res_coordination_transfer_container_present = false;
  bool                                                      scell_to_be_setup_list_present              = false;
  bool                                                      srbs_to_be_setup_list_present               = false;
  bool                                                      drbs_to_be_setup_list_present               = false;
  bool                                                      inactivity_monitoring_request_present       = false;
  bool                                                      rat_freq_prio_info_present                  = false;
  bool                                                      rrc_container_present                       = false;
  bool                                                      masked_imeisv_present                       = false;
  bool                                                      serving_plmn_present                        = false;
  bool                                                      gnb_du_ue_ambr_ul_present                   = false;
  bool                                                      rrc_delivery_status_request_present         = false;
  bool                                                      res_coordination_transfer_info_present      = false;
  bool                                                      serving_cell_mo_present                     = false;
  bool                                                      new_gnb_cu_ue_f1ap_id_present               = false;
  bool                                                      ranueid_present                             = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<nrcgi_s>                                       sp_cell_id;
  ie_field_s<integer<uint8_t, 0, 31, true, true>>           serv_cell_idx;
  ie_field_s<cell_ul_cfgured_e>                             sp_cell_ul_cfgured;
  ie_field_s<cu_to_du_rrc_info_s>                           cu_to_du_rrc_info;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<candidate_sp_cell_item_ies_o>, 1, 64, true>>
                                        candidate_sp_cell_list;
  ie_field_s<drx_cycle_s>               drx_cycle;
  ie_field_s<unbounded_octstring<true>> res_coordination_transfer_container;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<scell_to_be_setup_item_ies_o>, 1, 32, true>>
      scell_to_be_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<srbs_to_be_setup_item_ies_o>, 1, 8, true>> srbs_to_be_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_to_be_setup_item_ies_o>, 1, 64, true>>
                                                              drbs_to_be_setup_list;
  ie_field_s<inactivity_monitoring_request_e>                 inactivity_monitoring_request;
  ie_field_s<rat_freq_prio_info_c>                            rat_freq_prio_info;
  ie_field_s<unbounded_octstring<true>>                       rrc_container;
  ie_field_s<fixed_bitstring<64, false, true>>                masked_imeisv;
  ie_field_s<fixed_octstring<3, true>>                        serving_plmn;
  ie_field_s<integer<uint64_t, 0, 4000000000000, true, true>> gnb_du_ue_ambr_ul;
  ie_field_s<rrc_delivery_status_request_e>                   rrc_delivery_status_request;
  ie_field_s<res_coordination_transfer_info_s>                res_coordination_transfer_info;
  ie_field_s<integer<uint8_t, 1, 64, true, true>>             serving_cell_mo;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>>   new_gnb_cu_ue_f1ap_id;
  ie_field_s<fixed_octstring<8, true>>                        ranueid;

  // sequence methods
  ue_context_setup_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextSetupRequest ::= SEQUENCE
using ue_context_setup_request_s = elementary_procedure_option<ue_context_setup_request_ies_container>;

struct ue_context_setup_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      c_rnti_present                              = false;
  bool                                                      res_coordination_transfer_container_present = false;
  bool                                                      full_cfg_present                            = false;
  bool                                                      drbs_setup_list_present                     = false;
  bool                                                      srbs_failed_to_be_setup_list_present        = false;
  bool                                                      drbs_failed_to_be_setup_list_present        = false;
  bool                                                      scell_failedto_setup_list_present           = false;
  bool                                                      inactivity_monitoring_resp_present          = false;
  bool                                                      crit_diagnostics_present                    = false;
  bool                                                      srbs_setup_list_present                     = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<du_to_cu_rrc_info_s>                           du_to_cu_rrc_info;
  ie_field_s<integer<uint32_t, 0, 65535, true, true>>       c_rnti;
  ie_field_s<unbounded_octstring<true>>                     res_coordination_transfer_container;
  ie_field_s<full_cfg_e>                                    full_cfg;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_setup_item_ies_o>, 1, 64, true>> drbs_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<srbs_failed_to_be_setup_item_ies_o>, 1, 8, true>>
      srbs_failed_to_be_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_failed_to_be_setup_item_ies_o>, 1, 64, true>>
      drbs_failed_to_be_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<scell_failedto_setup_item_ies_o>, 1, 32, true>>
                                                                                            scell_failedto_setup_list;
  ie_field_s<inactivity_monitoring_resp_e>                                                  inactivity_monitoring_resp;
  ie_field_s<crit_diagnostics_s>                                                            crit_diagnostics;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<srbs_setup_item_ies_o>, 1, 8, true>> srbs_setup_list;

  // sequence methods
  ue_context_setup_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextSetupResponse ::= SEQUENCE
using ue_context_setup_resp_s = elementary_procedure_option<ue_context_setup_resp_ies_container>;

struct ue_inactivity_notif_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>>                                    gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>>                                    gnb_du_ue_f1ap_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drb_activity_item_ies_o>, 1, 64, true>> drb_activity_list;

  // sequence methods
  ue_inactivity_notif_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInactivityNotification ::= SEQUENCE
using ue_inactivity_notif_s = elementary_procedure_option<ue_inactivity_notif_ies_container>;

struct ul_rrc_msg_transfer_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      sel_plmn_id_present           = false;
  bool                                                      new_gnb_du_ue_f1ap_id_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<integer<uint8_t, 0, 3, true, true>>            srbid;
  ie_field_s<unbounded_octstring<true>>                     rrc_container;
  ie_field_s<fixed_octstring<3, true>>                      sel_plmn_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> new_gnb_du_ue_f1ap_id;

  // sequence methods
  ul_rrc_msg_transfer_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULRRCMessageTransfer ::= SEQUENCE
using ul_rrc_msg_transfer_s = elementary_procedure_option<ul_rrc_msg_transfer_ies_container>;

struct write_replace_warning_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                 cells_to_be_broadcast_list_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>    transaction_id;
  ie_field_s<pws_sys_info_s>                           pws_sys_info;
  ie_field_s<integer<uint32_t, 0, 131071, true, true>> repeat_period;
  ie_field_s<integer<uint32_t, 0, 65535, false, true>> numof_broadcast_request;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_to_be_broadcast_list_item_ies_o>, 1, 512, true>>
      cells_to_be_broadcast_list;

  // sequence methods
  write_replace_warning_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WriteReplaceWarningRequest ::= SEQUENCE
using write_replace_warning_request_s = elementary_procedure_option<write_replace_warning_request_ies_container>;

struct write_replace_warning_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              cells_broadcast_completed_list_present = false;
  bool                                              crit_diagnostics_present               = false;
  bool                                              ded_si_delivery_needed_ue_list_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_broadcast_completed_list_item_ies_o>, 1, 512, true>>
                                 cells_broadcast_completed_list;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ded_si_delivery_needed_ue_item_ies_o>, 1, 65536, true>>
      ded_si_delivery_needed_ue_list;

  // sequence methods
  write_replace_warning_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WriteReplaceWarningResponse ::= SEQUENCE
using write_replace_warning_resp_s = elementary_procedure_option<write_replace_warning_resp_ies_container>;

// F1AP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF F1AP-ELEMENTARY-PROCEDURE
struct f1ap_elem_procs_o {
  // InitiatingMessage ::= OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options {
        reset,
        f1_setup_request,
        gnb_du_cfg_upd,
        gnb_cu_cfg_upd,
        ue_context_setup_request,
        ue_context_release_cmd,
        ue_context_mod_request,
        ue_context_mod_required,
        write_replace_warning_request,
        pws_cancel_request,
        gnb_du_res_coordination_request,
        f1_removal_request,
        error_ind,
        ue_context_release_request,
        dl_rrc_msg_transfer,
        ul_rrc_msg_transfer,
        ue_inactivity_notif,
        private_msg,
        init_ul_rrc_msg_transfer,
        sys_info_delivery_cmd,
        paging,
        notify,
        pws_restart_ind,
        pws_fail_ind,
        gnb_du_status_ind,
        rrc_delivery_report,
        network_access_rate_reduction,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    f1_setup_request_s&                      f1_setup_request();
    gnb_du_cfg_upd_s&                        gnb_du_cfg_upd();
    gnb_cu_cfg_upd_s&                        gnb_cu_cfg_upd();
    ue_context_setup_request_s&              ue_context_setup_request();
    ue_context_release_cmd_s&                ue_context_release_cmd();
    ue_context_mod_request_s&                ue_context_mod_request();
    ue_context_mod_required_s&               ue_context_mod_required();
    write_replace_warning_request_s&         write_replace_warning_request();
    pws_cancel_request_s&                    pws_cancel_request();
    gnb_du_res_coordination_request_s&       gnb_du_res_coordination_request();
    f1_removal_request_s&                    f1_removal_request();
    error_ind_s&                             error_ind();
    ue_context_release_request_s&            ue_context_release_request();
    dl_rrc_msg_transfer_s&                   dl_rrc_msg_transfer();
    ul_rrc_msg_transfer_s&                   ul_rrc_msg_transfer();
    ue_inactivity_notif_s&                   ue_inactivity_notif();
    private_msg_s&                           private_msg();
    init_ul_rrc_msg_transfer_s&              init_ul_rrc_msg_transfer();
    sys_info_delivery_cmd_s&                 sys_info_delivery_cmd();
    paging_s&                                paging();
    notify_s&                                notify();
    pws_restart_ind_s&                       pws_restart_ind();
    pws_fail_ind_s&                          pws_fail_ind();
    gnb_du_status_ind_s&                     gnb_du_status_ind();
    rrc_delivery_report_s&                   rrc_delivery_report();
    network_access_rate_reduction_s&         network_access_rate_reduction();
    const reset_s&                           reset() const;
    const f1_setup_request_s&                f1_setup_request() const;
    const gnb_du_cfg_upd_s&                  gnb_du_cfg_upd() const;
    const gnb_cu_cfg_upd_s&                  gnb_cu_cfg_upd() const;
    const ue_context_setup_request_s&        ue_context_setup_request() const;
    const ue_context_release_cmd_s&          ue_context_release_cmd() const;
    const ue_context_mod_request_s&          ue_context_mod_request() const;
    const ue_context_mod_required_s&         ue_context_mod_required() const;
    const write_replace_warning_request_s&   write_replace_warning_request() const;
    const pws_cancel_request_s&              pws_cancel_request() const;
    const gnb_du_res_coordination_request_s& gnb_du_res_coordination_request() const;
    const f1_removal_request_s&              f1_removal_request() const;
    const error_ind_s&                       error_ind() const;
    const ue_context_release_request_s&      ue_context_release_request() const;
    const dl_rrc_msg_transfer_s&             dl_rrc_msg_transfer() const;
    const ul_rrc_msg_transfer_s&             ul_rrc_msg_transfer() const;
    const ue_inactivity_notif_s&             ue_inactivity_notif() const;
    const private_msg_s&                     private_msg() const;
    const init_ul_rrc_msg_transfer_s&        init_ul_rrc_msg_transfer() const;
    const sys_info_delivery_cmd_s&           sys_info_delivery_cmd() const;
    const paging_s&                          paging() const;
    const notify_s&                          notify() const;
    const pws_restart_ind_s&                 pws_restart_ind() const;
    const pws_fail_ind_s&                    pws_fail_ind() const;
    const gnb_du_status_ind_s&               gnb_du_status_ind() const;
    const rrc_delivery_report_s&             rrc_delivery_report() const;
    const network_access_rate_reduction_s&   network_access_rate_reduction() const;

  private:
    types type_;
    choice_buffer_t<dl_rrc_msg_transfer_s,
                    error_ind_s,
                    f1_removal_request_s,
                    f1_setup_request_s,
                    gnb_cu_cfg_upd_s,
                    gnb_du_cfg_upd_s,
                    gnb_du_res_coordination_request_s,
                    gnb_du_status_ind_s,
                    init_ul_rrc_msg_transfer_s,
                    network_access_rate_reduction_s,
                    notify_s,
                    paging_s,
                    private_msg_s,
                    pws_cancel_request_s,
                    pws_fail_ind_s,
                    pws_restart_ind_s,
                    reset_s,
                    rrc_delivery_report_s,
                    sys_info_delivery_cmd_s,
                    ue_context_mod_request_s,
                    ue_context_mod_required_s,
                    ue_context_release_cmd_s,
                    ue_context_release_request_s,
                    ue_context_setup_request_s,
                    ue_inactivity_notif_s,
                    ul_rrc_msg_transfer_s,
                    write_replace_warning_request_s>
        c;

    void destroy_();
  };
  // SuccessfulOutcome ::= OPEN TYPE
  struct successful_outcome_c {
    struct types_opts {
      enum options {
        reset_ack,
        f1_setup_resp,
        gnb_du_cfg_upd_ack,
        gnb_cu_cfg_upd_ack,
        ue_context_setup_resp,
        ue_context_release_complete,
        ue_context_mod_resp,
        ue_context_mod_confirm,
        write_replace_warning_resp,
        pws_cancel_resp,
        gnb_du_res_coordination_resp,
        f1_removal_resp,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    reset_ack_s&                          reset_ack();
    f1_setup_resp_s&                      f1_setup_resp();
    gnb_du_cfg_upd_ack_s&                 gnb_du_cfg_upd_ack();
    gnb_cu_cfg_upd_ack_s&                 gnb_cu_cfg_upd_ack();
    ue_context_setup_resp_s&              ue_context_setup_resp();
    ue_context_release_complete_s&        ue_context_release_complete();
    ue_context_mod_resp_s&                ue_context_mod_resp();
    ue_context_mod_confirm_s&             ue_context_mod_confirm();
    write_replace_warning_resp_s&         write_replace_warning_resp();
    pws_cancel_resp_s&                    pws_cancel_resp();
    gnb_du_res_coordination_resp_s&       gnb_du_res_coordination_resp();
    f1_removal_resp_s&                    f1_removal_resp();
    const reset_ack_s&                    reset_ack() const;
    const f1_setup_resp_s&                f1_setup_resp() const;
    const gnb_du_cfg_upd_ack_s&           gnb_du_cfg_upd_ack() const;
    const gnb_cu_cfg_upd_ack_s&           gnb_cu_cfg_upd_ack() const;
    const ue_context_setup_resp_s&        ue_context_setup_resp() const;
    const ue_context_release_complete_s&  ue_context_release_complete() const;
    const ue_context_mod_resp_s&          ue_context_mod_resp() const;
    const ue_context_mod_confirm_s&       ue_context_mod_confirm() const;
    const write_replace_warning_resp_s&   write_replace_warning_resp() const;
    const pws_cancel_resp_s&              pws_cancel_resp() const;
    const gnb_du_res_coordination_resp_s& gnb_du_res_coordination_resp() const;
    const f1_removal_resp_s&              f1_removal_resp() const;

  private:
    types type_;
    choice_buffer_t<f1_removal_resp_s,
                    f1_setup_resp_s,
                    gnb_cu_cfg_upd_ack_s,
                    gnb_du_cfg_upd_ack_s,
                    gnb_du_res_coordination_resp_s,
                    pws_cancel_resp_s,
                    reset_ack_s,
                    ue_context_mod_confirm_s,
                    ue_context_mod_resp_s,
                    ue_context_release_complete_s,
                    ue_context_setup_resp_s,
                    write_replace_warning_resp_s>
        c;

    void destroy_();
  };
  // UnsuccessfulOutcome ::= OPEN TYPE
  struct unsuccessful_outcome_c {
    struct types_opts {
      enum options {
        f1_setup_fail,
        gnb_du_cfg_upd_fail,
        gnb_cu_cfg_upd_fail,
        ue_context_setup_fail,
        ue_context_mod_fail,
        ue_context_mod_refuse,
        f1_removal_fail,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    f1_setup_fail_s&               f1_setup_fail();
    gnb_du_cfg_upd_fail_s&         gnb_du_cfg_upd_fail();
    gnb_cu_cfg_upd_fail_s&         gnb_cu_cfg_upd_fail();
    ue_context_setup_fail_s&       ue_context_setup_fail();
    ue_context_mod_fail_s&         ue_context_mod_fail();
    ue_context_mod_refuse_s&       ue_context_mod_refuse();
    f1_removal_fail_s&             f1_removal_fail();
    const f1_setup_fail_s&         f1_setup_fail() const;
    const gnb_du_cfg_upd_fail_s&   gnb_du_cfg_upd_fail() const;
    const gnb_cu_cfg_upd_fail_s&   gnb_cu_cfg_upd_fail() const;
    const ue_context_setup_fail_s& ue_context_setup_fail() const;
    const ue_context_mod_fail_s&   ue_context_mod_fail() const;
    const ue_context_mod_refuse_s& ue_context_mod_refuse() const;
    const f1_removal_fail_s&       f1_removal_fail() const;

  private:
    types type_;
    choice_buffer_t<f1_removal_fail_s,
                    f1_setup_fail_s,
                    gnb_cu_cfg_upd_fail_s,
                    gnb_du_cfg_upd_fail_s,
                    ue_context_mod_fail_s,
                    ue_context_mod_refuse_s,
                    ue_context_setup_fail_s>
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

// F1AP-PDU-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
using f1ap_pdu_ext_ies_o = protocol_ies_empty_o;

// InitiatingMessage ::= SEQUENCE{{F1AP-ELEMENTARY-PROCEDURE}}
struct init_msg_s {
  uint16_t                      proc_code = 0;
  crit_e                        crit;
  f1ap_elem_procs_o::init_msg_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// SuccessfulOutcome ::= SEQUENCE{{F1AP-ELEMENTARY-PROCEDURE}}
struct successful_outcome_s {
  uint16_t                                proc_code = 0;
  crit_e                                  crit;
  f1ap_elem_procs_o::successful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// UnsuccessfulOutcome ::= SEQUENCE{{F1AP-ELEMENTARY-PROCEDURE}}
struct unsuccessful_outcome_s {
  uint16_t                                  proc_code = 0;
  crit_e                                    crit;
  f1ap_elem_procs_o::unsuccessful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// F1AP-PDU ::= CHOICE
struct f1ap_pdu_c {
  struct types_opts {
    enum options { init_msg, successful_outcome, unsuccessful_outcome, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  f1ap_pdu_c() = default;
  f1ap_pdu_c(const f1ap_pdu_c& other);
  f1ap_pdu_c& operator=(const f1ap_pdu_c& other);
  ~f1ap_pdu_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  init_msg_s& init_msg()
  {
    assert_choice_type(types::init_msg, type_, "F1AP-PDU");
    return c.get<init_msg_s>();
  }
  successful_outcome_s& successful_outcome()
  {
    assert_choice_type(types::successful_outcome, type_, "F1AP-PDU");
    return c.get<successful_outcome_s>();
  }
  unsuccessful_outcome_s& unsuccessful_outcome()
  {
    assert_choice_type(types::unsuccessful_outcome, type_, "F1AP-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "F1AP-PDU");
    return c.get<protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>>();
  }
  const init_msg_s& init_msg() const
  {
    assert_choice_type(types::init_msg, type_, "F1AP-PDU");
    return c.get<init_msg_s>();
  }
  const successful_outcome_s& successful_outcome() const
  {
    assert_choice_type(types::successful_outcome, type_, "F1AP-PDU");
    return c.get<successful_outcome_s>();
  }
  const unsuccessful_outcome_s& unsuccessful_outcome() const
  {
    assert_choice_type(types::unsuccessful_outcome, type_, "F1AP-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  const protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "F1AP-PDU");
    return c.get<protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>>();
  }
  init_msg_s&                                         set_init_msg();
  successful_outcome_s&                               set_successful_outcome();
  unsuccessful_outcome_s&                             set_unsuccessful_outcome();
  protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<init_msg_s,
                  protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>,
                  successful_outcome_s,
                  unsuccessful_outcome_s>
      c;

  void destroy_();
};

// ProtocolIE-FieldPair{F1AP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE{{F1AP-PROTOCOL-IES-PAIR}}
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

// ProtocolIE-ContainerPair{F1AP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE (SIZE (0..65535)) OF ProtocolIE-FieldPair
template <class ies_set_paramT_>
using protocol_ie_container_pair_l = dyn_seq_of<protocol_ie_field_pair_s<ies_set_paramT_>, 0, 65535, true>;

} // namespace f1ap
} // namespace asn1

extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::associated_scell_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::broadcast_to_be_cancelled_list_item_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::cu_to_du_rrc_info_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::candidate_sp_cell_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_broadcast_cancelled_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_broadcast_completed_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_failed_to_be_activ_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_status_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_broadcast_list_item_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::cells_to_be_activ_list_item_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_activ_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_barred_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_deactiv_list_item_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::dl_rrc_msg_transfer_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drb_activity_item_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::qos_flow_level_qos_params_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drb_notify_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_failed_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_failed_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_failed_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_modified_conf_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_required_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_required_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::qos_info_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::drbs_to_be_modified_item_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::drbs_to_be_setup_item_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::drbs_to_be_setup_mod_item_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::du_to_cu_rrc_info_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ded_si_delivery_needed_ue_item_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::error_ind_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::served_cell_info_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ue_associated_lc_f1_conn_item_res_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_cu_tnl_assoc_failed_to_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_cu_tnl_assoc_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_cu_tnl_assoc_to_add_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_cu_tnl_assoc_to_rem_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_cu_tnl_assoc_to_upd_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_du_served_cells_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_du_tnl_assoc_to_rem_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::nr_cgi_list_for_restart_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::pws_failed_nr_cgi_list_item_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::f1ap::pws_sys_info_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::paging_cell_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::potential_sp_cell_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::protected_eutra_res_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::scell_failedto_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::scell_failedto_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::scell_to_be_remd_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::scell_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::scell_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_failed_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_failed_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_required_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::served_cells_to_add_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::served_cells_to_delete_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::served_cells_to_modify_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ue_associated_lc_f1_conn_item_res_ack_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::f1_removal_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::f1_removal_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::f1_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::f1_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::f1_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_cu_cfg_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_cu_cfg_upd_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_cu_cfg_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_cfg_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_cfg_upd_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_cfg_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_res_coordination_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_res_coordination_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_status_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::init_ul_rrc_msg_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::network_access_rate_reduction_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::notify_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pws_cancel_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pws_cancel_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pws_fail_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pws_restart_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::paging_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::rrc_delivery_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::reset_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::reset_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::sys_info_delivery_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_mod_confirm_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_mod_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_mod_refuse_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_mod_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_mod_required_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_mod_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_release_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_release_complete_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_release_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_context_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ue_inactivity_notif_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ul_rrc_msg_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::write_replace_warning_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::write_replace_warning_resp_ies_o>;
