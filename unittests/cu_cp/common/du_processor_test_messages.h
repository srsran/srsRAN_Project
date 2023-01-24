/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/e1/common/e1_types.h"

namespace srsgnb {
namespace srs_cu_cp {

/// \brief Generate a valid dummy F1 Setup Request.
f1_setup_request_message generate_valid_f1_setup_request();

/// \brief Generate a dummy F1 Setup Request base to extend.
f1_setup_request_message generate_f1_setup_request_base();

/// \brief Generate a dummy UE Creation Message.
/// \param[in] c_rnti The C-RNTI to use.
/// \param[in] nrcell_id The NR Cell Id to use.
/// \return The dummy UE Creation Message.
ue_creation_message generate_ue_creation_message(rnti_t c_rnti, unsigned nrcell_id);

/// \brief Generate a dummy UE Context Release Command.
/// \param[in] ue_index The UE Index to use.
/// \return The dummy UE Context Release Command.
ue_context_release_command_message generate_ue_context_release_command(ue_index_t ue_index);

/// \brief Generate a dummy PDU Session Resource Setup message.
cu_cp_pdu_session_resource_setup_request generate_pdu_session_resource_setup();

} // namespace srs_cu_cp
} // namespace srsgnb
