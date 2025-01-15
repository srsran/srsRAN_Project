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

#include "srsran/mac/mac_lc_config.h"
#include "srsran/rlc/rlc_config.h"

namespace srsran {
namespace srs_du {

/// \brief SRB Configuration, i.e. associated RLC and MAC configuration for SRBs in the DU
struct du_srb_config {
  rlc_config rlc;
};

} // namespace srs_du
} // namespace srsran
