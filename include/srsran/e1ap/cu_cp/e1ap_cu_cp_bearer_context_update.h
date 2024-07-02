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

#pragma once

#include "../common/e1ap_types.h"
#include "srsran/cu_cp/cu_cp_types.h"

namespace srsran {
namespace srs_cu_cp {

struct e1ap_bearer_context_setup_request {
  ue_index_t                                                              ue_index = ue_index_t::invalid;
  e1ap_security_info                                                      security_info;
  uint64_t                                                                ue_dl_aggregate_maximum_bit_rate;
  plmn_identity                                                           serving_plmn = plmn_identity::test_value();
  std::string                                                             activity_notif_level;
  slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_res_to_setup_item> pdu_session_res_to_setup_list;
  std::optional<uint64_t>                                                 ue_dl_maximum_integrity_protected_data_rate;
  std::optional<std::chrono::seconds>                                     ue_inactivity_timer;
  std::optional<std::string>                                              bearer_context_status_change;
  std::optional<ran_ue_id_t>                                              ran_ue_id;
  std::optional<uint64_t>                                                 gnb_du_id;
};

struct e1ap_bearer_context_setup_response {
  bool success = false;
  // Bearer Context Setup Response
  slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_setup_modification_item>
                                                                             pdu_session_resource_setup_list;
  slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_failed_item> pdu_session_resource_failed_list;

  // Bearer Context Setup Failure
  std::optional<e1ap_cause_t> cause;

  // Common
  std::optional<e1ap_crit_diagnostics> crit_diagnostics;
};

struct e1ap_bearer_context_modification_request {
  ue_index_t                                            ue_index = ue_index_t::invalid;
  std::optional<e1ap_security_info>                     security_info;
  std::optional<uint64_t>                               ue_dl_aggr_max_bit_rate;
  std::optional<uint64_t>                               ue_dl_max_integrity_protected_data_rate;
  std::optional<std::string>                            bearer_context_status_change;
  std::optional<bool>                                   new_ul_tnl_info_required;
  std::optional<std::chrono::seconds>                   ue_inactivity_timer;
  std::optional<bool>                                   data_discard_required;
  std::optional<e1ap_ng_ran_bearer_context_mod_request> ng_ran_bearer_context_mod_request;
  std::optional<ran_ue_id_t>                            ran_ue_id;
  std::optional<uint64_t>                               gnb_du_id;
  std::optional<std::string>                            activity_notif_level;
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
  std::optional<e1ap_cause_t> cause;

  // Common
  std::optional<e1ap_crit_diagnostics> crit_diagnostics;
};

struct e1ap_bearer_context_release_command {
  ue_index_t   ue_index = ue_index_t::invalid;
  e1ap_cause_t cause;
};

} // namespace srs_cu_cp
} // namespace srsran
