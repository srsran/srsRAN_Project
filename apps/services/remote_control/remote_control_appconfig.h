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

#include <cstdint>

namespace srsran {

/// Remote control application configuration.
struct remote_control_appconfig {
  bool     enabled = false;
  uint16_t port    = 8001;
};

} // namespace srsran
