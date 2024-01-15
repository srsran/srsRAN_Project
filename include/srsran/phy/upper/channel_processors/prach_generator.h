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
#include "srsran/adt/complex.h"
#include "srsran/phy/support/prach_buffer.h"
#include "srsran/ran/prach/prach_format_type.h"
#include "srsran/ran/prach/restricted_set_config.h"
#include "srsran/ran/subcarrier_spacing.h"

namespace srsran {

/// \brief PRACH generator interface.
///
/// Generates the PRACH frequency-domain sequences as per TS38.211 Section 6.3.3.
class prach_generator
{
public:
  /// Collects the necessary parameters to generate a PRACH frequency-domain signal.
  struct configuration {
    /// Preamble format.
    prach_format_type format;
    /// \brief Root sequence index.
    ///
    /// Parameter \f$i\f$ to select the PRACH sequence number from:
    /// - TS38.211 Table 6.3.3.1-3 with range {0, ..., 837} for long preambles, and
    /// - TS38.211 Table 6.3.3.1-4 with range {0, ..., 137} for short preambles.
    unsigned root_sequence_index;
    /// Index of the preamble to generate. Possible values are {0, ..., 63}.
    unsigned preamble_index;
    /// Restricted set configuration.
    restricted_set_config restricted_set;
    /// Cyclic shift configuration index. Possible values are {0, ..., 15}.
    unsigned zero_correlation_zone;
  };

  /// Default destructor.
  virtual ~prach_generator() = default;

  /// \brief Generates the frequency-domain PRACH sequence \f$y_{u,v}(n)\f$.
  /// \param[in] config Parameters describing the generated sequence.
  /// \return A read-only view of the generated sequence.
  virtual span<const cf_t> generate(const configuration& config) = 0;
};
} // namespace srsran
