/*
 *
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/f1u/cu_up/f1u_session_manager.h"

namespace srsran::srs_cu_up {

/// \brief Creates an F1-U bearer for the CU-UP.
std::unique_ptr<f1u_session_manager>
create_f1u_cu_up_session_manager(const std::vector<std::unique_ptr<gtpu_tnl_pdu_session>>& f1u_gws);

} // namespace srsran::srs_cu_up
