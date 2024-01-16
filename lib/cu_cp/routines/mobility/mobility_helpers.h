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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/up_resource_manager.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_ue_context_update.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handle UE context setup response from target DU and prefills the Bearer context modification.
bool handle_context_setup_response(cu_cp_inter_du_handover_response&         response_msg,
                                   e1ap_bearer_context_modification_request& bearer_context_modification_request,
                                   const f1ap_ue_context_setup_response&     target_ue_context_setup_response,
                                   up_config_update&                         next_config,
                                   const srslog::basic_logger&               logger,
                                   bool                                      reestablish_drb);

/// \brief Handler Bearer context modification response from CU-UP and prefill UE context modification for source DU.
bool handle_bearer_context_modification_response(
    cu_cp_inter_du_handover_response&                response_msg,
    f1ap_ue_context_modification_request&            source_ue_context_mod_request,
    const e1ap_bearer_context_modification_response& bearer_context_modification_response,
    up_config_update&                                next_config,
    const srslog::basic_logger&                      logger);

} // namespace srs_cu_cp
} // namespace srsran
