/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/asn1/e1ap/e1ap_pdu_contents.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/rb_id.h"
#include <map>

namespace srsran {
namespace srs_cu_cp {

/// \brief Generate dummy Supported PLMNs Item.
/// \param[in] nrcell_id The NR Cell Id to use.
/// \return The dummy Supported PLMNs Item.
asn1::e1ap::supported_plmns_item_s generate_supported_plmns_item(nr_cell_identity nrcell_id);

/// \brief Generate a dummy CU-CP E1 Setup Request base to extend.
/// \return The CU-CP E1 Setup Request base.
e1ap_message generate_cu_up_e1_setup_request_base();

/// \brief Generate a valid dummy CU-UP E1 Setup Request.
/// \return The CU-UP E1 Setup Request.
e1ap_message generate_valid_cu_up_e1_setup_request();

/// \brief Generate a dummy CU-UP E1 Setup Response.
/// \param[in] transaction_id The transaction ID of the Response.
/// \return The CU-UP E1 Setup Response.
e1ap_message generate_cu_up_e1_setup_respose(unsigned transaction_id);

/// \brief Generate a valid dummy E1 Release Request.
/// \return The E1 Release Request.
e1ap_message generate_valid_e1_release_request();

/// \brief Generate a dummy Bearer Context Setup Request.
/// \param[in] ue_index The UE Index.
/// \return The Bearer Context Setup Request.
e1ap_bearer_context_setup_request generate_bearer_context_setup_request(ue_index_t ue_index);

struct drb_test_params {
  drb_id_t      drb_id;
  qos_flow_id_t qos_flow_id;
};

/// \brief Generate a dummy Bearer Context Setup Response.
/// \param[in] cu_cp_ue_e1ap_id The CU-CP UE E1AP ID.
e1ap_message
generate_bearer_context_setup_response(gnb_cu_cp_ue_e1ap_id_t                             cu_cp_ue_e1ap_id,
                                       gnb_cu_up_ue_e1ap_id_t                             cu_up_ue_e1ap_id,
                                       const std::map<pdu_session_id_t, drb_test_params>& pdu_sessions_to_add =
                                           {{uint_to_pdu_session_id(1), {drb_id_t::drb1, uint_to_qos_flow_id(1)}}},
                                       const std::vector<pdu_session_id_t>& pdu_sessions_to_fail = {});

/// \brief Generate a dummy Bearer Context Setup Failure.
/// \param[in] cu_cp_ue_e1ap_id The CU-CP UE E1AP ID.
/// \param[in] cu_up_ue_e1ap_id The CU-UP UE E1AP ID.
/// \return The Bearer Context Setup Failure.
e1ap_message generate_bearer_context_setup_failure(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                   gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id);

/// \brief Generate a dummy Bearer Context Modification Request.
/// \param[in] cu_cp_ue_e1ap_id The CU-CP UE E1AP ID.
e1ap_bearer_context_modification_request generate_bearer_context_modification_request(ue_index_t ue_index);

/// \brief Generate a dummy Bearer Context Modification Response.
/// \param[in] cu_cp_ue_e1ap_id The CU-CP UE E1AP ID.
/// \param[in] cu_up_ue_e1ap_id The CU-UP UE E1AP ID.
/// \return The Bearer Context Modification Response.
e1ap_message generate_bearer_context_modification_response(
    gnb_cu_cp_ue_e1ap_id_t                             cu_cp_ue_e1ap_id,
    gnb_cu_up_ue_e1ap_id_t                             cu_up_ue_e1ap_id,
    const std::map<pdu_session_id_t, drb_test_params>& pdu_sessions_to_add = {},
    const std::map<pdu_session_id_t, drb_id_t>& pdu_sessions_to_modify     = {{pdu_session_id_t::min, drb_id_t::drb1}},
    const std::vector<pdu_session_id_t>&        pdu_sessions_failed_to_modify = {});

/// \brief Generate a dummy Bearer Context Modification Response.
/// \param[in] cu_cp_ue_e1ap_id The CU-CP UE E1AP ID.
/// \param[in] cu_up_ue_e1ap_id The CU-UP UE E1AP ID.
/// \return The Bearer Context Modification Response.
e1ap_message generate_bearer_context_modification_response_with_pdcp_status(
    gnb_cu_cp_ue_e1ap_id_t                      cu_cp_ue_e1ap_id,
    gnb_cu_up_ue_e1ap_id_t                      cu_up_ue_e1ap_id,
    const std::map<pdu_session_id_t, drb_id_t>& pdu_sessions_to_modify = {{uint_to_pdu_session_id(1), drb_id_t::drb1}});

/// \brief Generate a dummy Bearer Context Modification Failure.
/// \param[in] cu_cp_ue_e1ap_id The CU-CP UE E1AP ID.
/// \param[in] cu_up_ue_e1ap_id The CU-UP UE E1AP ID.
/// \return The Bearer Context Modification Failure.
e1ap_message generate_bearer_context_modification_failure(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                          gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id);

/// \brief Generate a dummy Bearer Context Release Command.
/// \param[in] ue_index The UE Index.
e1ap_bearer_context_release_command generate_bearer_context_release_command(ue_index_t ue_index);

/// \brief Generate a dummy Bearer Context Release Complete.
/// \param[in] cu_cp_ue_e1ap_id The CU-CP UE E1AP ID.
/// \param[in] cu_up_ue_e1ap_id The CU-UP UE E1AP ID.
/// \return The Bearer Context Release Complete.
e1ap_message generate_bearer_context_release_complete(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                      gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id);

/// \brief Generate a dummy Bearer Context Release Request.
/// \param[in] cu_cp_ue_e1ap_id The CU-CP UE E1AP ID.
/// \param[in] cu_up_ue_e1ap_id The CU-UP UE E1AP ID.
/// \return The Bearer Context Release Request.
e1ap_message generate_bearer_context_release_request(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                     gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id);

/// \brief Generate a dummy Bearer Context Inactivity Notification with UE activity level.
/// \param[in] cu_cp_ue_e1ap_id The CU-CP UE E1AP ID.
/// \param[in] cu_up_ue_e1ap_id The CU-UP UE E1AP ID.
/// \return The Bearer Context Inactivity Notification.
e1ap_message generate_bearer_context_inactivity_notification_with_ue_level(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                                           gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id);

/// \brief Generate a dummy Bearer Context Inactivity Notification with DRB activity level.
/// \param[in] cu_cp_ue_e1ap_id The CU-CP UE E1AP ID.
/// \param[in] cu_up_ue_e1ap_id The CU-UP UE E1AP ID.
/// \param[in] active_drbs The active DRBs.
/// \param[in] inactive_drbs The inactive DRBs.
/// \return The Bearer Context Inactivity Notification.
e1ap_message generate_bearer_context_inactivity_notification_with_drb_level(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                                            gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id,
                                                                            const std::vector<drb_id_t>& active_drbs,
                                                                            const std::vector<drb_id_t>& inactive_drbs);

/// \brief Generate a dummy Bearer Context Inactivity Notification with PDU session activity level.
/// \param[in] cu_cp_ue_e1ap_id The CU-CP UE E1AP ID.
/// \param[in] cu_up_ue_e1ap_id The CU-UP UE E1AP ID.
/// \param[in] active_pdu_sessions The active PDU sessions.
/// \param[in] inactive_pdu_sessions The inactive PDU sessions.
/// \return The Bearer Context Inactivity Notification.
e1ap_message generate_bearer_context_inactivity_notification_with_pdu_session_level(
    gnb_cu_cp_ue_e1ap_id_t               cu_cp_ue_e1ap_id,
    gnb_cu_up_ue_e1ap_id_t               cu_up_ue_e1ap_id,
    const std::vector<pdu_session_id_t>& active_pdu_sessions,
    const std::vector<pdu_session_id_t>& inactive_pdu_sessions);

/// \brief Generate an invalid dummy Bearer Context Inactivity Notification with UE activity level.
/// \param[in] cu_cp_ue_e1ap_id The CU-CP UE E1AP ID.
/// \param[in] cu_up_ue_e1ap_id The CU-UP UE E1AP ID.
/// \return The invalid Bearer Context Inactivity Notification.
e1ap_message generate_invalid_bearer_context_inactivity_notification(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                                     gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id);

} // namespace srs_cu_cp
} // namespace srsran
