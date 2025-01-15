/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/gnb_id.h"
#include <chrono>
#include <map>
#include <string>

namespace srsran {

namespace srs_cu_cp {

/// \brief NGAP configuration
struct ngap_configuration {
  gnb_id_t                             gnb_id{0, 22};
  std::string                          ran_node_name;
  std::vector<supported_tracking_area> supported_tas;
  std::chrono::seconds                 request_pdu_session_timeout; // timeout for requesting a pdu session in seconds
};

} // namespace srs_cu_cp

} // namespace srsran
