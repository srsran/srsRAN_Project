/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/rlc/rlc_config.h"
#include <cstdint>

namespace srsran {

/// QoS Configuration, i.e. 5QI and the associated RLC
/// configuration for DRBs
struct du_qos_config {
  rlc_config rlc;
};

} // namespace srsran
