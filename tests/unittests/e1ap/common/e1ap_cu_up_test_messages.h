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

#include "srsran/e1ap/cu_up/e1ap_cu_up.h"

namespace srsran {
namespace srs_cu_up {

/// \brief Generate a dummy CU-UP E1 Setup Request.
cu_up_e1_setup_request generate_cu_up_e1_setup_request();

/// \brief Generate a dummy CU-UP E1 Setup Response.
e1ap_message generate_cu_up_e1_setup_response(unsigned transaction_id);

/// \brief Generate a dummy bearer context setup request.
e1ap_message generate_bearer_context_setup_request(unsigned cu_cp_ue_e1ap_id);

/// \brief Generate an invalid dummy bearer context setup request.
e1ap_message generate_invalid_bearer_context_setup_request(unsigned cu_cp_ue_e1ap_id);

/// \brief Generate a dummy bearer context setup request, with an invalid inactivity timer.
e1ap_message generate_invalid_bearer_context_setup_request_inactivity_timer(unsigned cu_cp_ue_e1ap_id);

/// \brief Generate a dummy bearer context modification request.
e1ap_message generate_bearer_context_modification_request(unsigned cu_cp_ue_e1ap_id, unsigned cu_up_ue_e1ap_id);

/// \brief Generate an invalid dummy bearer context modification request.
e1ap_message generate_invalid_bearer_context_modification_request(unsigned cu_cp_ue_e1ap_id, unsigned cu_up_ue_e1ap_id);

/// \brief Generate a dummy bearer context release command.
e1ap_message generate_bearer_context_release_command(unsigned cu_cp_ue_e1ap_id, unsigned cu_up_ue_e1ap_id);

} // namespace srs_cu_up
} // namespace srsran
