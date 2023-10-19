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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/du_processor.h"
#include "srsran/e1ap/common/e1ap_types.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Generate a valid dummy F1 Setup Request.
void generate_valid_f1_setup_request(f1ap_f1_setup_request& f1_setup_request,
                                     unsigned               gnb_du_id = 0x11,
                                     unsigned               nrcell_id = 6576,
                                     pci_t                  pci       = 0);

/// \brief Generate a dummy F1 Setup Request base to extend.
void generate_f1_setup_request_base(f1ap_f1_setup_request& f1_setup_request);

/// \brief Generate a dummy F1 Setup Request with unsupported number of cells.
void generate_f1_setup_request_with_too_many_cells(f1ap_f1_setup_request& f1_setup_request);

/// \brief Generate a dummy UE Creation Message.
/// \param[in] ue_index The UE index to use.
/// \param[in] c_rnti The C-RNTI to use.
/// \param[in] nrcell_id The NR Cell Id to use.
/// \return The dummy UE Creation Message.
cu_cp_ue_creation_message generate_ue_creation_message(ue_index_t ue_index, rnti_t c_rnti, unsigned nrcell_id);

/// \brief Generate a dummy UE Context Release Command.
/// \param[in] ue_index The UE Index to use.
/// \return The dummy UE Context Release Command.
cu_cp_ue_context_release_command generate_ue_context_release_command(ue_index_t ue_index);

/// \brief Generate a dummy PDU Session Resource Setup request.
cu_cp_pdu_session_resource_setup_request generate_pdu_session_resource_setup(unsigned num_pdu_sessions = 1,
                                                                             unsigned num_qos_flows    = 1);

/// \brief Generate a dummy PDU Session Resource Release Command.
cu_cp_pdu_session_resource_release_command generate_pdu_session_resource_release();

/// \brief Generate a dummy PDU Session Resource Modification Request.
cu_cp_pdu_session_resource_modify_request generate_pdu_session_resource_modification(unsigned psi = 1,
                                                                                     unsigned qfi = 2);

/// \brief Generate a dummy PDU Session Resource Modification Request with QoS flow to remove item.
cu_cp_pdu_session_resource_modify_request
generate_pdu_session_resource_modification_with_qos_flow_removal(qos_flow_id_t flow_id);

/// \brief Generate a dummy E1AP Bearer Context Setup Response.
e1ap_bearer_context_setup_response generate_e1ap_bearer_context_setup_response(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                                               gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id);

/// \brief Generate a dummy E1AP Bearer Context Modification Response.
e1ap_bearer_context_modification_response
generate_e1ap_bearer_context_modification_response(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                   gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id);

/// \brief Generate a dummy CU-CP UE Context Modification Response.
f1ap_ue_context_modification_response generate_f1ap_ue_context_modification_response(gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id,
                                                                                     gnb_du_ue_f1ap_id_t du_ue_f1ap_id);

} // namespace srs_cu_cp
} // namespace srsran
