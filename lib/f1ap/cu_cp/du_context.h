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

#include "srsran/ran/gnb_du_id.h"
#include <string>

namespace srsran {
namespace srs_cu_cp {

/// Context of the DU that has setup an F1 connection to the CU-CP.
struct du_context {
  gnb_du_id_t du_id = gnb_du_id_t::invalid;
  std::string du_name;
};

} // namespace srs_cu_cp
} // namespace srsran