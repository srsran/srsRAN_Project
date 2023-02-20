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

#include "srsgnb/ran/prach/prach_subcarrier_spacing.h"
#include "srsgnb/ran/prach/restricted_set_config.h"
#include <cstdint>

namespace srsran {

/// Reserved number of cyclic shifts. It indicates the number of cyclic shifts is invalid.
static constexpr uint16_t PRACH_CYCLIC_SHIFTS_RESERVED = 430;

/// \brief Calculates the number of cyclic shifts \f$N_{CS}\f$ used in the sequence generation as per TS38.211
/// Section 6.3.3.1.
///
/// The number of cyclic shifts \f$N_{CS}\f$ is extracted from:
/// - TS38.211 Table 6.3.3.1-5 for preambles 0, 1 and 2
/// - TS38.211 Table 6.3.3.1-6 for preamble 3, and
/// - TS38.211 Table 6.3.3.1-7 for the remaining preambles.
///
/// \param[in] prach_scs             \brief PRACH subcarrier spacing (parameter \f$\Delta f^{RA}\f$ as defined in
///                                  TS38.211 Section 6.3.3.1).
/// \param[in] restricted_set        \brief Restricted set (see \e restrictedSetConfig in TS38.331 Section 6.3.2,
///                                  Information Element \e RACH-ConfigCommon).
/// \param[in] zero_correlation_zone \brief Zero correlation zone (see \e zeroCorrelationZoneConfig in TS38.331
///                                  Section 6.3.2, Information Element \e RACH-ConfigGeneric).
/// \return The number of cyclic shifts if the input parameters are valid. Otherwise, PRACH_CYCLIC_SHIFTS_RESERVED.
uint16_t prach_cyclic_shifts_get(prach_subcarrier_spacing prach_scs,
                                 restricted_set_config    restricted_set,
                                 unsigned                 zero_correlation_zone);

} // namespace srsran