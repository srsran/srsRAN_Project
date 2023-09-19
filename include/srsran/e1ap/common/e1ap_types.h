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

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/ran/cause.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/security/security.h"
#include <cstdint>
#include <limits>
#include <type_traits>

namespace srsran {

/// \brief GNB-CU-CP-UE-E1AP-ID used to identify the UE in the CU-CP E1AP.
/// \remark See TS 38.463 Section 9.3.1.4: GNB-CU-UE-E1AP-ID valid values: (0..2^32-1)
constexpr static uint64_t MAX_NOF_CU_CP_E1AP_UES = ((uint64_t)1 << 32);
enum class gnb_cu_cp_ue_e1ap_id_t : uint64_t { min = 0, max = MAX_NOF_CU_CP_E1AP_UES - 1, invalid = 0x1ffffffff };

constexpr inline uint64_t gnb_cu_cp_ue_e1ap_id_to_uint(gnb_cu_cp_ue_e1ap_id_t id)
{
  return static_cast<uint64_t>(id);
}

/// Convert integer to GNB-CU-CP-UE-E1AP-ID type.
constexpr inline gnb_cu_cp_ue_e1ap_id_t int_to_gnb_cu_cp_ue_e1ap_id(uint64_t idx)
{
  return static_cast<gnb_cu_cp_ue_e1ap_id_t>(idx);
}

/// \brief GNB-CU-UP-UE-E1AP-ID used to identify the UE in the CU-UP E1AP.
/// \remark See TS 38.473 Section 9.3.1.5: GNB-CU-UP-UE-E1AP-ID valid values: (0..2^32-1)
constexpr static uint64_t MAX_NOF_CU_UP_E1AP_UES = ((uint64_t)1 << 32);
enum class gnb_cu_up_ue_e1ap_id_t : uint64_t { min = 0, max = MAX_NOF_CU_CP_E1AP_UES - 1, invalid = 0x1ffffffff };

constexpr inline uint64_t gnb_cu_up_ue_e1ap_id_to_uint(gnb_cu_up_ue_e1ap_id_t id)
{
  return static_cast<uint64_t>(id);
}

/// Convert integer to GNB-DU-UE-E1AP-ID type.
constexpr inline gnb_cu_up_ue_e1ap_id_t int_to_gnb_cu_up_ue_e1ap_id(uint64_t idx)
{
  return static_cast<gnb_cu_up_ue_e1ap_id_t>(idx);
}

struct e1ap_cell_group_info_item {
  uint8_t               cell_group_id = 0;
  optional<std::string> ul_cfg;
  optional<std::string> dl_tx_stop;
  optional<std::string> rat_type;
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
  qos_characteristics_t                qos_characteristics;
  e1ap_ng_ran_alloc_and_retention_prio ng_ran_alloc_retention_prio;
  optional<e1ap_gbr_qos_flow_info>     gbr_qos_flow_info;
  optional<bool>                       reflective_qos_attribute;
  optional<bool>                       add_qos_info;
  optional<uint8_t>                    paging_policy_ind;
  optional<bool>                       reflective_qos_ind;
};

struct e1ap_qos_flow_qos_param_item {
  qos_flow_id_t                  qos_flow_id = qos_flow_id_t::invalid;
  e1ap_qos_flow_level_qos_params qos_flow_level_qos_params;
  optional<std::string>          qos_flow_map_ind;
};

struct e1ap_qos_flow_map_item {
  qos_flow_id_t         qos_flow_id = qos_flow_id_t::invalid;
  optional<std::string> qos_flow_map_ind;
};

struct e1ap_data_forwarding_info_request {
  std::string                                              data_forwarding_request;
  slotted_id_vector<qos_flow_id_t, e1ap_qos_flow_map_item> qos_flows_forwarded_on_fwd_tunnels;
};

struct e1ap_pdcp_count {
  uint32_t pdcp_sn;
  uint64_t hfn;
};

struct e1ap_drb_status_transfer {
  e1ap_pdcp_count    count_value;
  optional<uint64_t> receive_status_of_pdcp_sdu;
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
  pdcp_sn_size                 pdcp_sn_size_ul;
  pdcp_sn_size                 pdcp_sn_size_dl;
  srsran::pdcp_rlc_mode        rlc_mod;
  optional<e1ap_rohc_params>   rohc_params;
  optional<pdcp_t_reordering>  t_reordering_timer;
  optional<pdcp_discard_timer> discard_timer;
  optional<int32_t>            ul_data_split_thres;
  optional<bool>               pdcp_dupl;
  optional<bool>               pdcp_reest;
  optional<bool>               pdcp_data_recovery;
  optional<std::string>        dupl_activation;
  optional<bool>               out_of_order_delivery;
};

struct e1ap_drb_to_setup_item_ng_ran {
  drb_id_t                                                       drb_id = drb_id_t::invalid;
  sdap_config_t                                                  sdap_cfg;
  e1ap_pdcp_config                                               pdcp_cfg;
  std::vector<e1ap_cell_group_info_item>                         cell_group_info;
  slotted_id_vector<qos_flow_id_t, e1ap_qos_flow_qos_param_item> qos_flow_info_to_be_setup;
  optional<e1ap_data_forwarding_info_request>                    drb_data_forwarding_info_request;
  optional<std::chrono::seconds>                                 drb_inactivity_timer;
  optional<e1ap_pdcp_sn_status_info>                             pdcp_sn_status_info;
};

struct e1ap_pdu_session_res_to_setup_item {
  pdu_session_id_t                                           pdu_session_id = pdu_session_id_t::invalid;
  std::string                                                pdu_session_type;
  s_nssai_t                                                  snssai;
  up_transport_layer_info                                    ng_ul_up_tnl_info;
  security_indication_t                                      security_ind;
  slotted_id_vector<drb_id_t, e1ap_drb_to_setup_item_ng_ran> drb_to_setup_list_ng_ran;

  optional<uint64_t>                          pdu_session_res_dl_ambr;
  optional<e1ap_data_forwarding_info_request> pdu_session_data_forwarding_info_request;
  optional<std::chrono::seconds>              pdu_session_inactivity_timer;
  optional<up_transport_layer_info>           existing_allocated_ng_dl_up_tnl_info;
  optional<uint16_t>                          network_instance;
};

struct e1ap_security_algorithm {
  srsran::security::ciphering_algorithm           ciphering_algo;
  optional<srsran::security::integrity_algorithm> integrity_protection_algorithm; // Optional, TS 38.463 Sec. 9.4.5
};

struct e1ap_up_security_key {
  e1ap_up_security_key& operator=(const e1ap_up_security_key& other)
  {
    encryption_key           = other.encryption_key.copy();
    integrity_protection_key = other.integrity_protection_key.copy();
    return *this;
  }
  byte_buffer encryption_key;
  byte_buffer integrity_protection_key; // Optional, TS 38.463 Sec. 9.4.5
};

struct e1ap_security_info {
  e1ap_security_algorithm security_algorithm;
  e1ap_up_security_key    up_security_key;
};

struct e1ap_up_params_item {
  up_transport_layer_info up_tnl_info;
  uint8_t                 cell_group_id;
};

struct e1ap_qos_flow_item {
  qos_flow_id_t qos_flow_id = qos_flow_id_t::invalid;
};

struct e1ap_qos_flow_failed_item {
  qos_flow_id_t qos_flow_id = qos_flow_id_t::invalid;
  cause_t       cause;
};

struct e1ap_data_forwarding_info {
  optional<up_transport_layer_info> ul_data_forwarding;
  optional<up_transport_layer_info> dl_data_forwarding;
};

struct e1ap_drb_setup_item_ng_ran {
  drb_id_t                                                    drb_id = drb_id_t::invalid;
  std::vector<e1ap_up_params_item>                            ul_up_transport_params;
  slotted_id_vector<qos_flow_id_t, e1ap_qos_flow_item>        flow_setup_list;
  slotted_id_vector<qos_flow_id_t, e1ap_qos_flow_failed_item> flow_failed_list;
  optional<e1ap_data_forwarding_info>                         drb_data_forwarding_info_resp;
};

struct e1ap_drb_failed_item_ng_ran {
  drb_id_t drb_id = drb_id_t::invalid;
  cause_t  cause;
};

struct e1ap_pdu_session_resource_setup_modification_item {
  pdu_session_id_t                                         pdu_session_id = pdu_session_id_t::invalid;
  up_transport_layer_info                                  ng_dl_up_tnl_info;
  slotted_id_vector<drb_id_t, e1ap_drb_setup_item_ng_ran>  drb_setup_list_ng_ran;
  slotted_id_vector<drb_id_t, e1ap_drb_failed_item_ng_ran> drb_failed_list_ng_ran;
  optional<security_result_t>                              security_result;
  optional<e1ap_data_forwarding_info>                      pdu_session_data_forwarding_info_resp;
  optional<bool>                                           ng_dl_up_unchanged;
};

struct e1ap_pdu_session_resource_failed_item {
  pdu_session_id_t pdu_session_id = pdu_session_id_t::invalid;
  cause_t          cause;
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

struct e1ap_drb_to_setup_mod_item_ng_ran {
  drb_id_t                                                       drb_id = drb_id_t::invalid;
  sdap_config_t                                                  sdap_cfg;
  e1ap_pdcp_config                                               pdcp_cfg;
  std::vector<e1ap_cell_group_info_item>                         cell_group_info;
  slotted_id_vector<qos_flow_id_t, e1ap_qos_flow_qos_param_item> flow_map_info;
  optional<e1ap_data_forwarding_info_request>                    drb_data_forwarding_info_request;
  optional<uint16_t>                                             drb_inactivity_timer;
  optional<e1ap_pdcp_sn_status_info>                             pdcp_sn_status_info;
};

struct e1ap_drb_to_modify_item_ng_ran {
  drb_id_t                                                       drb_id = drb_id_t::invalid;
  optional<sdap_config_t>                                        sdap_cfg;
  optional<e1ap_pdcp_config>                                     pdcp_cfg;
  optional<e1ap_data_forwarding_info>                            drb_data_forwarding_info;
  optional<bool>                                                 pdcp_sn_status_request;
  std::vector<e1ap_up_params_item>                               dl_up_params;
  std::vector<e1ap_cell_group_info_item>                         cell_group_to_add;
  std::vector<e1ap_cell_group_info_item>                         cell_group_to_modify;
  std::vector<e1ap_cell_group_info_item>                         cell_group_to_rem;
  slotted_id_vector<qos_flow_id_t, e1ap_qos_flow_qos_param_item> flow_map_info;
  optional<uint16_t>                                             drb_inactivity_timer;
};

struct e1ap_pdu_session_res_to_modify_item {
  pdu_session_id_t                                            pdu_session_id = pdu_session_id_t::invalid;
  optional<security_indication_t>                             security_ind;
  optional<uint64_t>                                          pdu_session_res_dl_ambr;
  optional<up_transport_layer_info>                           ng_ul_up_tnl_info;
  optional<e1ap_data_forwarding_info_request>                 pdu_session_data_forwarding_info_request;
  optional<e1ap_data_forwarding_info>                         pdu_session_data_forwarding_info;
  optional<uint16_t>                                          pdu_session_inactivity_timer;
  optional<uint16_t>                                          network_instance;
  slotted_id_vector<drb_id_t, e1ap_drb_to_setup_item_ng_ran>  drb_to_setup_list_ng_ran;
  slotted_id_vector<drb_id_t, e1ap_drb_to_modify_item_ng_ran> drb_to_modify_list_ng_ran;
  std::vector<drb_id_t>                                       drb_to_rem_list_ng_ran;

  slotted_id_vector<drb_id_t, e1ap_drb_to_setup_mod_item_ng_ran> drb_to_setup_mod_list_ng_ran;
};

struct e1ap_ng_ran_bearer_context_mod_request {
  slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_res_to_setup_item>  pdu_session_res_to_setup_mod_list;
  slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_res_to_modify_item> pdu_session_res_to_modify_list;
  std::vector<pdu_session_id_t>                                            pdu_session_res_to_rem_list;
};

struct e1ap_drb_modified_item_ng_ran {
  drb_id_t                                                    drb_id = drb_id_t::invalid;
  std::vector<e1ap_up_params_item>                            ul_up_transport_params;
  slotted_id_vector<qos_flow_id_t, e1ap_qos_flow_item>        flow_setup_list;
  slotted_id_vector<qos_flow_id_t, e1ap_qos_flow_failed_item> flow_failed_list;
  optional<e1ap_pdcp_sn_status_info>                          pdcp_sn_status_info;
};

struct e1ap_pdu_session_resource_modified_item {
  pdu_session_id_t                                           pdu_session_id = pdu_session_id_t::invalid;
  optional<up_transport_layer_info>                          ng_dl_up_tnl_info;
  slotted_id_vector<drb_id_t, e1ap_drb_setup_item_ng_ran>    drb_setup_list_ng_ran;
  slotted_id_vector<drb_id_t, e1ap_drb_failed_item_ng_ran>   drb_failed_list_ng_ran;
  slotted_id_vector<drb_id_t, e1ap_drb_modified_item_ng_ran> drb_modified_list_ng_ran;
  slotted_id_vector<drb_id_t, e1ap_drb_failed_item_ng_ran>   drb_failed_to_modify_list_ng_ran;
  optional<security_result_t>                                security_result;
  optional<e1ap_data_forwarding_info>                        pdu_session_data_forwarding_info_resp;
};

} // namespace srsran
