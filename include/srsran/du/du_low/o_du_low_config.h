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

#include "srsran/du/du_low/du_low_config.h"

namespace srsran {
namespace srs_du {

using cell_prach_ports_entry = std::vector<uint8_t>;

/// O-RAN DU low configuration.
struct o_du_low_config {
  du_low_config du_low_cfg;
  /// PRACH ports.
  std::vector<cell_prach_ports_entry> prach_ports;
};

/// O-RAN DU low dependencies.
struct o_du_low_dependencies {
  /// DU low dependencies.
  du_low_dependencies du_low_deps;
};

} // namespace srs_du
} // namespace srsran
