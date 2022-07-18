/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_PHY_UPPER_CHANNEL_PROCESSORS_PRACH_PREAMBLE_HELPERS_H
#define SRSGNB_LIB_PHY_UPPER_CHANNEL_PROCESSORS_PRACH_PREAMBLE_HELPERS_H

#include "srsgnb/ran/prach/prach_preamble_format.h"
#include "srsgnb/ran/prach/prach_subcarrier_spacing.h"

namespace srsgnb {

/// Describes the parameters of a PRACH modulation as per TS38.211 Tables 6.3.3.1-1 and 6.3.3.1-2.
struct prach_preamble_modulation_info {
  /// Subcarrier spacing, parameter \f$\Delta f^{RA}\f$.
  prach_subcarrier_spacing scs;
  /// Symbol length, parameter \f$N_u\f$.
  unsigned N_u;
  /// Cyclic prefix length, parameter \f$N_{CP}^{RA}\f$.
  unsigned N_cp_ra;
};

/// \brief Gets the PRACH long preamble modulation information.
///
/// A short preamble format or zero DFT size for 15kHz SCS triggers an assertion.
///
/// \param[in] format         Indicates the preamble format.
/// \param[in] dft_size_15kHz DFT size for 15kHz subcarrier spacing.
/// \return A valid PRACH long preamble configuration structure.
inline prach_preamble_modulation_info prach_preamble_modulation_info_get_long(preamble_format format,
                                                                              unsigned        dft_size_15kHz)
{
  srsran_assert(format.is_long_preamble(), "Preamble format is not long.");
  srsran_assert(dft_size_15kHz, "DFT size for 15kHz must not be zero.");

  unsigned N_u     = 0;
  unsigned N_cp_ra = 0;
  switch (format) {
    case preamble_format::FORMAT0:
      N_u     = 24576;
      N_cp_ra = 3168;
      break;
    case preamble_format::FORMAT1:
      N_u     = 2 * 24576;
      N_cp_ra = 21024;
      break;
    case preamble_format::FORMAT2:
      N_u     = 4 * 24576;
      N_cp_ra = 4688;
      break;
    case preamble_format::FORMAT3:
      N_u     = 4 * 6144;
      N_cp_ra = 3168;
      break;
    case preamble_format::OTHER:
      // Not implemented.
      break;
  }

  prach_preamble_modulation_info result;
  result.N_u     = (N_u * dft_size_15kHz) / 2048;
  result.N_cp_ra = (N_cp_ra * dft_size_15kHz) / 2048;
  result.scs =
      (format == preamble_format::FORMAT3) ? prach_subcarrier_spacing::kHz5 : prach_subcarrier_spacing::kHz1_25;
  return result;
}

} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_CHANNEL_PROCESSORS_PRACH_PREAMBLE_HELPERS_H
