/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_CONSTANTS_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_CONSTANTS_H

#include "srsgnb/phy/constants.h"

namespace srsgnb {
namespace pdcch_constants {

/// Defines the number if resource blocks per frequency resource in a CORESET.
static constexpr unsigned NOF_RB_PER_FREQ_RESOURCE = 6;

/// Defines the maximum number of frequency resources in a CORESET.
static constexpr unsigned MAX_NOF_FREQ_RESOUCES = MAX_RB / NOF_RB_PER_FREQ_RESOURCE;

/// Defines the maximum number of DCI in a PDCCH transmission.
static constexpr unsigned MAX_NOF_DCI = 1;

/// Defines the maximum payload size in bits.
static constexpr unsigned MAX_DCI_PAYLOAD_SIZE = 128;

/// Defines the number of RE used for PDCCH in a RB.
static constexpr unsigned NOF_RE_PDCCH_PER_RB = NRE - 3;

/// Defines the number of REG per CCE.
static constexpr unsigned NOF_REG_PER_CCE = 6;

/// Define the maximum CORESET duration.
static constexpr unsigned MAX_CORESET_DURATION = 3;

/// Define the maximum aggregation level. Maximum number of CCE.
static constexpr unsigned MAX_AGGREGATION_LEVEL = 16;

/// \brief Define the maximum number of RBs that can be used for a single PDCCH transmission.
///
/// It coincides with the maximum number of REG that can be used for a single PDSHC transmission.
static constexpr unsigned MAX_NOF_RB_PDCCH = MAX_AGGREGATION_LEVEL * NOF_REG_PER_CCE;

/// Define the maximum number of RE used by PDCCH.
static constexpr unsigned MAX_NOF_RE_PDCCH = MAX_NOF_RB_PDCCH * NOF_RE_PDCCH_PER_RB;

/// Define the maximum number of encoded bits in a PDCCH transmission.
static constexpr unsigned MAX_NOF_BITS = MAX_NOF_RE_PDCCH * 2;

} // namespace pdcch_constants
} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_CONSTANTS_H
