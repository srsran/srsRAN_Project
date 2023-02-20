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

#include "srsgnb/adt/bounded_integer.h"
#include "srsgnb/adt/complex.h"
#include "srsgnb/phy/support/prach_buffer.h"
#include "srsgnb/ran/prach/prach_preamble_format.h"
#include "srsgnb/ran/prach/restricted_set_config.h"
#include "srsgnb/ran/subcarrier_spacing.h"

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
    preamble_format format;
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
