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

#include "srsgnb/e1/cu_up/e1_cu_up.h"

namespace srsgnb {
namespace srs_cu_up {

/// \brief Generate a dummy CU-CP E1 Setup Request.
e1_message generate_cu_cp_e1_setup_request();

/// \brief Generate a dummy bearer context setup request.
e1_message generate_bearer_context_setup_request_msg(unsigned int cu_cp_ue_e1_id);

} // namespace srs_cu_up
} // namespace srsgnb