/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/bounded_integer.h"
#include "srsran/ran/modulation_scheme.h"

namespace srsran {

/// Physical Downlink and Uplink Shared Channel Modulation and Code Scheme Index, parameter \$fI_{MCS}\$f in TS38.214
/// Section 5.1.3.1.
using sch_mcs_index = bounded_integer<uint8_t, 0, 31>;

/// \brief Physical Downlink and Uplink Shared Channel Modulation and Coding Scheme breakdown.
///
/// Represents one row of an MCS table.
struct sch_mcs_description {
  /// Subcarrier modulation scheme.
  modulation_scheme modulation;

  /// Target code rate, expressed as \f$R\times 1024\f$, range (0, ..., 1024).
  float target_code_rate;

  /// \brief Returns the target spectral efficiency, in bits per subcarrier access.
  /// \note The spectral efficiency is given by the target code rate times the number of bits per modulation symbol.
  constexpr float get_spectral_efficiency() const
  {
    return get_bits_per_symbol(modulation) * get_normalised_target_code_rate();
  }

  /// \brief Returns the target code rate normalised between 0 and 1.
  constexpr float get_normalised_target_code_rate() const { return target_code_rate * (1.F / 1024); }
};

} // namespace srsran
