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

#include <string>

namespace srsran {

/// FAPI configuration.
struct fapi_unit_config {
  /// Number of slots the L2 is running ahead of the L1.
  unsigned l2_nof_slots_ahead = 0;
  /// FAPI log level.
  std::string fapi_level = "warning";
};

} // namespace srsran
