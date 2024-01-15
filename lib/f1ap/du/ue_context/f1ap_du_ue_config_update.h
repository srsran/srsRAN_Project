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

#include "../f1ap_du_context.h"
#include "f1ap_du_ue_manager.h"
#include "srsran/f1ap/du/f1ap_du_ue_config.h"

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