/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../f1ap_du_context.h"
#include "f1ap_du_ue_manager.h"
#include "srsgnb/f1ap/du/f1ap_du_ue_config.h"

namespace srsran {
namespace srs_du {

/// \brief Creates a new UE in the F1AP, on request by DU management plane.
/// \return Result of the F1 UE creation that contains the outcome (success/failure) and list of created F1c bearers.
f1ap_ue_creation_response create_f1ap_ue(const f1ap_ue_creation_request& req,
                                         f1ap_du_ue_manager&             ues,
                                         const f1ap_du_context&          du_ctx,
                                         f1ap_event_manager&             ev_mng);

/// \brief Update existing UE configuration on request by DU management plane.
f1ap_ue_configuration_response update_f1ap_ue_config(const f1ap_ue_configuration_request& req, f1ap_du_ue_manager& ues);

} // namespace srs_du
} // namespace srsran