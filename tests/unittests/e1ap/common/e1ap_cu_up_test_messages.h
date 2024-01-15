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

#include "srsran/e1ap/cu_up/e1ap_cu_up.h"

namespace srsran {
namespace srs_cu_up {

/// \brief Generate a dummy CU-UP E1 Setup Request.
cu_up_e1_setup_request generate_cu_up_e1_setup_request();

/// \brief Generate a dummy CU-UP E1 Setup Response.
e1ap_message generate_cu_up_e1_setup_response(unsigned transaction_id);

/// \brief Generate a dummy bearer context setup request.
e1ap_message generate_bearer_context_setup_request(unsigned int cu_cp_ue_e1ap_id);

/// \brief Generate an invalid dummy bearer context setup request.
e1ap_message generate_invalid_bearer_context_setup_request(unsigned int cu_cp_ue_e1ap_id);

/// \brief Generate a dummy bearer context setup request, with an invalid inactivity timer.
e1ap_message generate_invalid_bearer_context_setup_request_inactivity_timer(unsigned int cu_cp_ue_e1ap_id);

/// \brief Generate a dummy bearer context modification request.
e1ap_message generate_bearer_context_modification_request(unsigned int cu_cp_ue_e1ap_id, unsigned int cu_up_ue_e1ap_id);

/// \brief Generate an invalid dummy bearer context modification request.
e1ap_message generate_invalid_bearer_context_modification_request(unsigned int cu_cp_ue_e1ap_id,
                                                                  unsigned int cu_up_ue_e1ap_id);

/// \brief Generate a dummy bearer context release command.
e1ap_message generate_bearer_context_release_command(unsigned int cu_cp_ue_e1ap_id, unsigned int cu_up_ue_e1ap_id);

} // namespace srs_cu_up
} // namespace srsran
