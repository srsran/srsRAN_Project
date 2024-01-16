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

#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Generate dummy Supported PLMNs Item.
/// \param[in] nrcell_id The NR Cell Id to use.
/// \return The dummy Supported PLMNs Item.
asn1::e1ap::supported_plmns_item_s generate_supported_plmns_item(unsigned nrcell_id);

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

/// \brief Generate a dummy Bearer Context Setup Request.
/// \param[in] ue_index The UE Index.
/// \return The Bearer Context Setup Request.
e1ap_bearer_context_setup_request generate_bearer_context_setup_request(ue_index_t ue_index);

/// \brief Generate a dummy Bearer Context Setup Response.
/// \param[in] cu_cp_ue_e1ap_id The CU-CP UE E1AP ID.
e1ap_message generate_bearer_context_setup_response(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                    gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id);

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
e1ap_message generate_bearer_context_modification_response(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                           gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id);

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

/// \brief Generate a dummy Bearer Context Inactivity Notification with UE activity level.
/// \param[in] cu_cp_ue_e1ap_id The CU-CP UE E1AP ID.
/// \param[in] cu_up_ue_e1ap_id The CU-UP UE E1AP ID.
/// \return The Bearer Context Inactivity Notification.
e1ap_message generate_bearer_context_inactivity_notification_with_ue_level(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                                           gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id);

/// \brief Generate an invalid dummy Bearer Context Inactivity Notification with UE activity level.
/// \param[in] cu_cp_ue_e1ap_id The CU-CP UE E1AP ID.
/// \param[in] cu_up_ue_e1ap_id The CU-UP UE E1AP ID.
/// \return The invalid Bearer Context Inactivity Notification.
e1ap_message generate_invalid_bearer_context_inactivity_notification(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                                     gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id);

} // namespace srs_cu_cp
} // namespace srsran
