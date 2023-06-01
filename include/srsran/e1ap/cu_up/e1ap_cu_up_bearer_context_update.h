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

#include "../common/e1ap_types.h"
#include "srsran/cu_up/cu_up_types.h"

namespace srsran {
namespace srs_cu_up {

/// \brief Request to create a new UE and bearer context.
struct e1ap_bearer_context_setup_request {
  e1ap_security_info                                                      security_info;
  uint64_t                                                                ue_dl_aggregate_maximum_bit_rate;
  std::string                                                             serving_plmn;
  slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_res_to_setup_item> pdu_session_res_to_setup_list;
  optional<uint64_t>                                                      ue_dl_maximum_integrity_protected_data_rate;
  activity_notification_level_t                                           activity_notif_level;
  optional<std::chrono::seconds>                                          ue_inactivity_timer;
  optional<std::string>                                                   bearer_context_status_change;
  optional<ran_ue_id_t>                                                   ran_ue_id;
  optional<uint64_t>                                                      gnb_du_id;
};

/// \brief Response to a bearer context setup request including UE index for E1AP map.
struct e1ap_bearer_context_setup_response {
  bool       success  = false;
  ue_index_t ue_index = INVALID_UE_INDEX; // Valid UE index if setup was successful.
  // Bearer Context Setup Response
  slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_setup_modification_item>
                                                                             pdu_session_resource_setup_list;
  slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_failed_item> pdu_session_resource_failed_list;

  // Bearer Context Setup Failure
  optional<cause_t> cause;

  // Common
  optional<e1ap_crit_diagnostics> crit_diagnostics;
};

/// \brief Request to modify a bearer context.
struct e1ap_bearer_context_modification_request {
  ue_index_t                                       ue_index = INVALID_UE_INDEX;
  optional<e1ap_security_info>                     security_info;
  optional<uint64_t>                               ue_dl_aggr_max_bit_rate;
  optional<uint64_t>                               ue_dl_max_integrity_protected_data_rate;
  optional<std::string>                            bearer_context_status_change;
  optional<std::string>                            new_ul_tnl_info_required;
  optional<std::chrono::seconds>                   ue_inactivity_timer;
  optional<std::string>                            data_discard_required;
  optional<e1ap_ng_ran_bearer_context_mod_request> ng_ran_bearer_context_mod_request;
  optional<ran_ue_id_t>                            ran_ue_id;
  optional<uint64_t>                               gnb_du_id;
  optional<std::string>                            activity_notif_level;
};

/// \brief Response to a bearer context modification request including UE index for E1AP map.
struct e1ap_bearer_context_modification_response {
  bool       success  = false;
  ue_index_t ue_index = INVALID_UE_INDEX; // Valid UE index if modification was successful.

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

/// \brief Request to release a bearer context.
struct e1ap_bearer_context_release_command {
  ue_index_t ue_index = INVALID_UE_INDEX;
  cause_t    cause; // Cause of the release.
};

} // namespace srs_cu_up
} // namespace srsran
