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

#include "srsran/du_low/du_low_config.h"

namespace srsran {

struct du_cell_config;

using cell_prach_ports_entry = std::vector<uint8_t>;

/// DU low wrapper configuration.
struct du_low_wrapper_config {
  du_low_config du_low_cfg;
  /// PRACH ports.
  std::vector<cell_prach_ports_entry> prach_ports;
};

} // namespace srsran