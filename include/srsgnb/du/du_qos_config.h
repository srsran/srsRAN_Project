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

#include <cstdint>

namespace srsgnb {

/// QoS Configuration, i.e. 5QI and the associated RLC
/// configuration for DRBs
struct du_qos_config {
  uint8_t five_qi;
};

} // namespace srsgnb
