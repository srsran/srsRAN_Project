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

namespace srsgnb {

/// Maximum number of \f$k_i\f$ frequency allocation reference values, according to TS 38.211 Table 7.4.1.5.3-1.
static constexpr unsigned CSI_RS_MAX_NOF_K_INDEXES = 6;

/// Maximum number of ports that can be simultaneously used for CSI-RS, detailed in the TS38.211 Table 7.4.1.5.3-1
/// Row 18.
static constexpr unsigned CSI_RS_MAX_NOF_PORTS = 32;

} // namespace srsgnb