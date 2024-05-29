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

#include "srsran/ngap/ngap_types.h"
#include "srsran/ran/gnb_id.h"
#include <string>

namespace srsran {

namespace srs_cu_cp {

/// \brief NGAP context
struct ngap_context_t {
  gnb_id_t             gnb_id = {0, 22};
  std::string          ran_node_name;
  std::string          plmn; /// Full PLMN as string (without possible filler digit) e.g. "00101"
  unsigned             tac;
  std::vector<guami_t> served_guami_list;
  std::chrono::seconds pdu_session_setup_timeout; // timeout for PDU context setup in seconds
};

} // namespace srs_cu_cp

} // namespace srsran
