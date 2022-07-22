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

#include "srsgnb/ran/phy_time_unit.h"
#include "srsgnb/ran/prach/prach_preamble_format.h"
#include "srsgnb/ran/prach/prach_subcarrier_spacing.h"

namespace srsgnb {

/// \brief Collects PRACH Preamble information parameters.
///
/// The parameters are used and described in TS38.211 Section 6.3.3.1.
struct prach_preamble_information {
  /// Parameter \f$L_{RA}\f$.
  unsigned sequence_length;
  /// Parameter \f$\Delta f^{RA}\f$.
  prach_subcarrier_spacing scs;
  /// Parameter \f$N_u\f$. Expressed in units of the reference symbol time \f$\kappa\f$.
  phy_time_unit symbol_length;
  /// Parameter \f$N_{CP}^{RA}\f$. Expressed in units of the reference symbol time \f$\kappa\f$.
  phy_time_unit cp_length;
  /// Set to true if the preamble supports the restricted set A.
  bool support_restricted_set_A;
  /// Set to true if the preamble supports the restricted set B.
  bool support_restricted_set_B;
};

/// \brief Get long PRACH preamble information as per TS38.211 Table 6.3.3.1-1.
///
/// An assertion is triggered if the PRACH preamble format is not long.
///
/// \param[in] format PRACH preamble format.
/// \return PRACH preamble information.
prach_preamble_information prach_preamble_long_get_info(preamble_format format);

} // namespace srsgnb
