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

#include "srsran/f1u/du/f1u_config.h"
#include "srsran/mac/mac_lc_config.h"
#include "srsran/rlc/rlc_config.h"
#include <cstdint>

namespace srsran {

/// \brief QoS Configuration, i.e. 5QI and the associated RLC configuration for DRBs
struct du_qos_config {
  rlc_config         rlc;
  srs_du::f1u_config f1u;
  mac_lc_config      mac;
};

} // namespace srsran
