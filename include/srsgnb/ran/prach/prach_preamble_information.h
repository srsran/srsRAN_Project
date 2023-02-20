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

#include "srsgnb/ran/phy_time_unit.h"
#include "srsgnb/ran/prach/prach_preamble_format.h"
#include "srsgnb/ran/prach/prach_subcarrier_spacing.h"

namespace srsgnb {

/// \brief Collects PRACH preamble information parameters.
///
/// The parameters are used and described in TS38.211 Section 6.3.3.1.
struct prach_preamble_information {
  /// Sequence length in frequency domain, parameter \f$L_{RA}\f$.
  unsigned sequence_length;
  /// Parameter \f$\Delta f^{RA}\f$.
  prach_subcarrier_spacing scs;
  /// Parameter \f$N_u\f$. Expressed in units of the reference symbol time \f$\kappa\f$.
  phy_time_unit symbol_length;
  /// Parameter \f$N_{CP}^{RA}\f$. Expressed in units of the reference symbol time \f$\kappa\f$.
  phy_time_unit cp_length;
  /// Flag: true if the preamble supports the restricted sets A and B.
  bool support_restricted_sets;
};

/// \brief Get long PRACH preamble information as per TS38.211 Table 6.3.3.1-1.
///
/// An assertion is triggered if the PRACH preamble format is not long.
///
/// \param[in] format PRACH preamble format.
/// \return PRACH preamble information.
prach_preamble_information get_prach_preamble_long_info(preamble_format format);

} // namespace srsgnb
