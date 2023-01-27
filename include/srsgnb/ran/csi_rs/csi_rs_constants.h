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

/// Maximum number of resource sets per resource configuration. See TS 38.331, \c
/// maxNrofNZP-CSI-RS-ResourceSetsPerConfig.
static constexpr unsigned MAX_NOF_NZP_CSI_RS_RESOURCE_SETS_PER_CSI_RESOURCE_CONFIG = 16;

/// Maximum number of CSI SSB resource sets per resource configuration. See TS 38.331, \c
/// maxNrofCSI-SSB-ResourceSetsPerConfig.
static constexpr unsigned MAX_NOF_CSI_SSB_RESOURCE_SETS_PER_CSI_RESOURCE_CONFIG = 1;

/// Maximum number of CSI IM resource sets per resource configuration. See TS 38.331, \c
/// maxNrofCSI-IM-ResourceSetsPerConfig.
static constexpr unsigned MAX_NOF_CSI_IM_RESOURCE_SETS_PER_CSI_RESOURCE_CONFIG = 16;

} // namespace srsgnb