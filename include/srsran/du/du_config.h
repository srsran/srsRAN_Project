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

#include "srsran/du/du_low_config.h"
#include "srsran/du_high/du_high_configuration.h"

namespace srsran {

struct fapi_configuration {
  std::string              log_level;
  unsigned                 sector;
  unsigned                 l2_nof_slots_ahead;
  std::vector<uint8_t>     prach_ports;
  optional<task_executor*> executor;
};

struct du_config {
  /// Configuration of the DU-high that comprises the MAC, RLC and F1 layers.
  srs_du::du_high_configuration du_hi;
  /// Configuration of the DU-low that comprises the upper PHY.
  du_low_configuration du_lo;
  fapi_configuration   fapi;
};

} // namespace srsran
