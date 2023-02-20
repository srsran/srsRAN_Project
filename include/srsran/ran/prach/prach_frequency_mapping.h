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

#include "srsran/ran/prach/prach_subcarrier_spacing.h"
#include "srsran/ran/subcarrier_spacing.h"

namespace srsran {

/// Collects PRACH frequency mapping information as per TS38.211 Section 5.3.2 and Section 6.3.3.2.
struct prach_frequency_mapping_information {
  /// Number of resource blocks per PRACH occasion in the frequency domain, relative to the PUSCH subcarrier spacing
  /// (\f$\Delta f\f$).
  unsigned nof_rb_ra;
  /// Parameter \f$\bar{k}\f$.
  unsigned k_bar;
};

/// Reserved PRACH frequency-domain mapping information.
static constexpr prach_frequency_mapping_information PRACH_FREQUENCY_MAPPING_INFORMATION_RESERVED = {0, 0};

/// \brief Gets the PRACH frequency mapping information contained in TS38.211 Table 6.3.3.2-1.
/// \param[in] prach_scs_Hz PRACH subcarrier spacing (parameter \f$\Delta f_{RA}\f$).
/// \param[in] pusch_scs_Hz PUSCH subcarrier spacing (parameter \f$\Delta f\f$).
/// \return The requested PRACH frequency mapping information if the combination of input parameters is valid, \c
/// PRACH_FREQUENCY_MAPPING_INFORMATION_RESERVED otherwise.
prach_frequency_mapping_information prach_frequency_mapping_get(prach_subcarrier_spacing prach_scs,
                                                                subcarrier_spacing       pusch_scs);

} // namespace srsran