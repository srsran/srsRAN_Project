/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
                                   bool                                      reestablish_pdcp);

/// \brief Handler Bearer context modification response from CU-UP and prefill UE context modification for source DU.
bool handle_bearer_context_modification_response(
    cu_cp_inter_du_handover_response&                response_msg,
    f1ap_ue_context_modification_request&            source_ue_context_mod_request,
    const e1ap_bearer_context_modification_response& bearer_context_modification_response,
    up_config_update&                                next_config,
    const srslog::basic_logger&                      logger);

} // namespace srs_cu_cp
} // namespace srsran
