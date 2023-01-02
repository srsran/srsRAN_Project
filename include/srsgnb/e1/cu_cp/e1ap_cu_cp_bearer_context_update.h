/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/optional.h"
#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/rlc/rlc_config.h"
#include "srsgnb/security/security.h"

namespace srsgnb {
namespace srs_cu_cp {

struct e1ap_security_ind {
  std::string           integrity_protection_ind;
  std::string           confidentiality_protection_ind;
  optional<std::string> maximum_ipdatarate;
};

struct e1ap_cell_group_info_item {
  uint8_t               cell_group_id;
  optional<std::string> ul_cfg;
  optional<std::string> dl_tx_stop;
  optional<std::string> rat_type;
};

struct e1ap_packet_error_rate {
  uint8_t per_scalar;
  uint8_t per_exponent;
};

struct e1ap_dynamic_5qi_descriptor {
  uint8_t                qos_prio_level;
  uint16_t               packet_delay_budget;
  e1ap_packet_error_rate packet_error_rate;
  optional<uint16_t>     five_qi;
  optional<std::string>  delay_crit;
  optional<uint16_t>     averaging_win;
  optional<uint16_t>     max_data_burst_volume;
};

struct e1ap_non_dynamic_5qi_descriptor {
  uint16_t           five_qi;
  optional<uint8_t>  qos_prio_level;
  optional<uint16_t> averaging_win;
  optional<uint16_t> max_data_burst_volume;
};

struct e1ap_qos_characteristics {
  optional<e1ap_dynamic_5qi_descriptor>     dyn_5qi;
  optional<e1ap_non_dynamic_5qi_descriptor> non_dyn_5qi;
};

struct e1ap_ng_ran_alloc_and_retention_prio {
  uint8_t     prio_level;
  std::string pre_emption_cap;
  std::string pre_emption_vulnerability;
};

struct e1ap_gbr_qos_flow_info {
  uint64_t           max_flow_bit_rate_dl;
  uint64_t           max_flow_bit_rate_ul;
  uint64_t           guaranteed_flow_bit_rate_dl;
  uint64_t           guaranteed_flow_bit_rate_ul;
  optional<uint16_t> max_packet_loss_rate_dl;
  optional<uint16_t> max_packet_loss_rate_ul;
};

struct e1ap_qos_flow_level_qos_params {
  e1ap_qos_characteristics             qos_characteristics;
  e1ap_ng_ran_alloc_and_retention_prio ng_ran_alloc_retention_prio;
  optional<e1ap_gbr_qos_flow_info>     gbr_qos_flow_info;
  optional<std::string>                reflective_qos_attribute;
  optional<std::string>                add_qos_info;
  optional<uint8_t>                    paging_policy_ind;
  optional<std::string>                reflective_qos_ind;
};

struct e1ap_qos_flow_qos_param_item {
  uint8_t                        qos_flow_id;
  e1ap_qos_flow_level_qos_params qos_flow_level_qos_params;
  optional<std::string>          qos_flow_map_ind;
};

struct e1ap_qos_flow_map_item {
  uint8_t               qos_flow_id;
  optional<std::string> qos_flow_map_ind;
};

struct e1ap_data_forwarding_info_request {
  std::string                         data_forwarding_request;
  std::vector<e1ap_qos_flow_map_item> qos_flows_forwarded_on_fwd_tunnels;
};

struct e1ap_pdcp_count {
  uint32_t pdcp_sn;
  uint64_t hfn;
};

struct e1ap_drb_status_transfer {
  e1ap_pdcp_count    count_value;
  optional<uint64_t> receive_statusof_pdcpsdu;
};

struct e1ap_pdcp_sn_status_info {
  e1ap_drb_status_transfer pdcp_status_transfer_ul;
  e1ap_pdcp_count          pdcp_status_transfer_dl;
};

struct e1ap_rohc {
  uint16_t       max_cid;
  uint16_t       rohc_profiles;
  optional<bool> continue_rohc;
};

struct e1ap_rohc_params {
  optional<e1ap_rohc> rohc;
  optional<e1ap_rohc> ul_only_rohc;
};

struct e1ap_pdcp_config {
  uint8_t                    pdcp_sn_size_ul;
  uint8_t                    pdcp_sn_size_dl;
  srsgnb::rlc_mode           rlc_mod;
  optional<e1ap_rohc_params> rohc_params;
  optional<uint16_t>         t_reordering_timer;
  optional<int16_t>          discard_timer;
  optional<int32_t>          ul_data_split_thres;
  optional<bool>             pdcp_dupl;
  optional<bool>             pdcp_reest;
  optional<bool>             pdcp_data_recovery;
  optional<std::string>      dupl_activation;
  optional<bool>             out_of_order_delivery;
};

struct e1ap_drb_to_setup_item_ng_ran {
  drb_id_t                                    drb_id;
  cu_cp_sdap_config                           sdap_cfg;
  e1ap_pdcp_config                            pdcp_cfg;
  std::vector<e1ap_cell_group_info_item>      cell_group_info;
  std::vector<e1ap_qos_flow_qos_param_item>   qos_flow_info_to_be_setup;
  optional<e1ap_data_forwarding_info_request> drb_data_forwarding_info_request;
  optional<uint16_t>                          drb_inactivity_timer;
  optional<e1ap_pdcp_sn_status_info>          pdcp_sn_status_info;
};

struct e1ap_pdu_session_res_to_setup_item {
  uint16_t                                   pdu_session_id;
  std::string                                pdu_session_type;
  cu_cp_s_nssai                              snssai;
  up_transport_layer_info                    ng_ul_up_tnl_info;
  e1ap_security_ind                          security_ind;
  std::vector<e1ap_drb_to_setup_item_ng_ran> drb_to_setup_list_ng_ran;

  optional<uint64_t>                          pdu_session_res_dl_ambr;
  optional<e1ap_data_forwarding_info_request> pdu_session_data_forwarding_info_request;
  optional<uint16_t>                          pdu_session_inactivity_timer;
  optional<up_transport_layer_info>           existing_allocated_ng_dl_up_tnl_info;
  optional<uint16_t>                          network_instance;
};

struct e1ap_security_algorithm {
  srsgnb::security::ciphering_algorithm           ciphering_algo;
  optional<srsgnb::security::integrity_algorithm> integrity_protection_algorithm;
};

struct e1ap_upsecuritykey {
  byte_buffer encryption_key;
  byte_buffer integrity_protection_key;
};

struct e1ap_security_info {
  e1ap_security_algorithm security_algorithm;
  e1ap_upsecuritykey      upsecuritykey;
};

struct e1ap_bearer_context_setup_request {
  cu_cp_ue_id_t                                   cu_cp_ue_id;
  e1ap_security_info                              security_info;
  uint64_t                                        uedl_aggregate_maximum_bit_rate;
  std::string                                     serving_plmn;
  std::string                                     activity_notif_level;
  std::vector<e1ap_pdu_session_res_to_setup_item> pdu_session_res_to_setup_list;
  optional<uint64_t>                              uedl_maximum_integrity_protected_data_rate;
  optional<uint16_t>                              ue_inactivity_timer;
  optional<std::string>                           bearer_context_status_change;
  optional<ran_ue_id_t>                           ranueid;
  optional<uint64_t>                              gnb_du_id;
};

struct e1ap_up_params_item {
  up_transport_layer_info up_tnl_info;
  uint8_t                 cell_group_id;
};

struct e1ap_qos_flow_item {
  uint8_t qos_flow_id;
};

struct e1ap_qos_flow_failed_item {
  uint8_t       qos_flow_id;
  cu_cp_cause_t cause;
};

struct e1ap_data_forwarding_info {
  optional<up_transport_layer_info> ul_data_forwarding;
  optional<up_transport_layer_info> dl_data_forwarding;
};

struct e1ap_drb_setup_item_ng_ran {
  drb_id_t                               drb_id;
  std::vector<e1ap_up_params_item>       ul_up_transport_params;
  std::vector<e1ap_qos_flow_item>        flow_setup_list;
  std::vector<e1ap_qos_flow_failed_item> flow_failed_list;
  optional<e1ap_data_forwarding_info>    drb_data_forwarding_info_resp;
};

struct e1ap_drb_failed_item_ng_ran {
  drb_id_t      drb_id;
  cu_cp_cause_t cause;
};

struct e1ap_pdu_session_resource_setup_modification_item {
  uint16_t                                 pdu_session_id;
  up_transport_layer_info                  ng_dl_up_tnl_info;
  std::vector<e1ap_drb_setup_item_ng_ran>  drb_setup_list_ng_ran;
  std::vector<e1ap_drb_failed_item_ng_ran> drb_failed_list_ng_ran;
  optional<cu_cp_security_result>          security_result;
  optional<e1ap_data_forwarding_info>      pdu_session_data_forwarding_info_resp;
  optional<bool>                           ng_dl_up_unchanged;
};

struct e1ap_pdu_session_resource_failed_item {
  uint16_t      pdu_session_id;
  cu_cp_cause_t cause;
};

struct e1ap_crit_diagnostics_item {
  std::string iecrit;
  uint32_t    ie_id;
  std::string type_of_error;
};

struct e1ap_crit_diagnostics {
  std::vector<e1ap_crit_diagnostics_item> ies_crit_diagnostics;
  optional<uint16_t>                      proc_code;
  optional<std::string>                   trigger_msg;
  optional<std::string>                   proc_crit;
  optional<uint16_t>                      transaction_id;
};

struct e1ap_bearer_context_setup_response {
  bool success;
  // Bearer Context Setup Response
  std::vector<e1ap_pdu_session_resource_setup_modification_item> pdu_session_resource_setup_list;
  std::vector<e1ap_pdu_session_resource_failed_item>             pdu_session_resource_failed_list;

  // Bearer Context Setup Failure
  optional<cu_cp_cause_t> cause;

  // Common
  optional<e1ap_crit_diagnostics> crit_diagnostics;
};

struct e1ap_bearer_context_modification_request {
  cu_cp_ue_id_t                            cu_cp_ue_id;
  asn1::e1ap::bearer_context_mod_request_s msg;
};

struct e1ap_drb_modified_item_ng_ran {
  drb_id_t                               drb_id;
  std::vector<e1ap_up_params_item>       ul_up_transport_params;
  std::vector<e1ap_qos_flow_item>        flow_setup_list;
  std::vector<e1ap_qos_flow_failed_item> flow_failed_list;
  optional<e1ap_pdcp_sn_status_info>     pdcp_sn_status_info;
};

struct e1ap_pdu_session_resource_modified_item {
  uint16_t                                   pdu_session_id;
  up_transport_layer_info                    ng_dl_up_tnl_info;
  std::vector<e1ap_drb_setup_item_ng_ran>    drb_setup_list_ng_ran;
  std::vector<e1ap_drb_failed_item_ng_ran>   drb_failed_list_ng_ran;
  std::vector<e1ap_drb_modified_item_ng_ran> drb_modified_list_ng_ran;
  std::vector<e1ap_drb_failed_item_ng_ran>   drb_failed_to_modify_list_ng_ran;
  optional<cu_cp_security_result>            security_result;
  optional<e1ap_data_forwarding_info>        pdu_session_data_forwarding_info_resp;
};

struct e1ap_bearer_context_modification_response_message {
  bool success;
  // Bearer Context Modification Response
  std::vector<e1ap_pdu_session_resource_setup_modification_item> pdu_session_resource_setup_list;
  std::vector<e1ap_pdu_session_resource_failed_item>             pdu_session_resource_failed_list;
  std::vector<e1ap_pdu_session_resource_modified_item>           pdu_session_resource_modified_list;
  std::vector<e1ap_pdu_session_resource_failed_item>             pdu_session_resource_failed_to_modify_list;

  // Bearer Context Modification Failure
  optional<cu_cp_cause_t> cause;

  // Common
  optional<e1ap_crit_diagnostics> crit_diagnostics;
};
struct e1ap_bearer_context_modification_response {
  asn1::e1ap::bearer_context_mod_resp_s response;
  asn1::e1ap::bearer_context_mod_fail_s failure;
  bool                                  success;
};

struct e1ap_bearer_context_release_command {
  asn1::e1ap::bearer_context_release_cmd_s msg;
};

struct e1ap_bearer_context_release_complete {
  asn1::e1ap::bearer_context_release_complete_s msg;
};

} // namespace srs_cu_cp
} // namespace srsgnb
