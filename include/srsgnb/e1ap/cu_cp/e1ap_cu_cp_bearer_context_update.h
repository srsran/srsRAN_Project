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

#include "../common/e1ap_types.h"
#include "srsgnb/cu_cp/cu_cp_types.h"

namespace srsgnb {
namespace srs_cu_cp {

struct e1ap_bearer_context_setup_request {
  ue_index_t                                                              ue_index = ue_index_t::invalid;
  e1ap_security_info                                                      security_info;
  uint64_t                                                                ue_dl_aggregate_maximum_bit_rate;
  std::string                                                             serving_plmn;
  std::string                                                             activity_notif_level;
  slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_res_to_setup_item> pdu_session_res_to_setup_list;
  optional<uint64_t>                                                      ue_dl_maximum_integrity_protected_data_rate;
  optional<uint16_t>                                                      ue_inactivity_timer;
  optional<std::string>                                                   bearer_context_status_change;
  optional<ran_ue_id_t>                                                   ran_ue_id;
  optional<uint64_t>                                                      gnb_du_id;
};

struct e1ap_bearer_context_setup_response {
  bool success = false;
  // Bearer Context Setup Response
  slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_setup_modification_item>
                                                                             pdu_session_resource_setup_list;
  slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_failed_item> pdu_session_resource_failed_list;

  // Bearer Context Setup Failure
  optional<cause_t> cause;

  // Common
  optional<e1ap_crit_diagnostics> crit_diagnostics;
};

struct e1ap_bearer_context_modification_request {
  ue_index_t                                       ue_index = ue_index_t::invalid;
  optional<e1ap_security_info>                     security_info;
  optional<uint64_t>                               ue_dl_aggr_max_bit_rate;
  optional<uint64_t>                               ue_dl_max_integrity_protected_data_rate;
  optional<std::string>                            bearer_context_status_change;
  optional<std::string>                            new_ul_tnl_info_required;
  optional<uint16_t>                               ue_inactivity_timer;
  optional<std::string>                            data_discard_required;
  optional<e1ap_ng_ran_bearer_context_mod_request> ng_ran_bearer_context_mod_request;
  optional<ran_ue_id_t>                            ran_ue_id;
  optional<uint64_t>                               gnb_du_id;
  optional<std::string>                            activity_notif_level;
};

struct e1ap_bearer_context_modification_response {
  bool success = false;
  // Bearer Context Modification Response
  slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_setup_modification_item>
                                                                               pdu_session_resource_setup_list;
  slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_failed_item>   pdu_session_resource_failed_list;
  slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_modified_item> pdu_session_resource_modified_list;
  slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_failed_item> pdu_session_resource_failed_to_modify_list;

  // Bearer Context Modification Failure
  optional<cause_t> cause;

  // Common
  optional<e1ap_crit_diagnostics> crit_diagnostics;
};

struct e1ap_bearer_context_release_command {
  ue_index_t ue_index = ue_index_t::invalid;
  cause_t    cause;
};

} // namespace srs_cu_cp
} // namespace srsgnb
