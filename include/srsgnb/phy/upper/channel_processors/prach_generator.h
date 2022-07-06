/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PRACH_GENERATOR_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PRACH_GENERATOR_H

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/ran/prach/prach_preamble_format.h"
#include "srsgnb/ran/prach/restricted_set_config.h"
#include "srsgnb/ran/subcarrier_spacing.h"

namespace srsgnb {

/// \brief PRACH generator interface.
///
/// It generates the PRACH frequency-domain sequences as per TS38.211 Section 6.3.3 and modulates the channel
/// as per TS38.211 Section 5.3.2.
///
class prach_generator
{
public:
  /// Collects the necessary parameters to generate a PRACH time domain signal.
  struct configuration {
    /// Indicates the preamble format.
    preamble_format format;
    /// \brief Root sequence index.
    ///
    /// Parameter \f$i\f$ to select the PRACH sequence number from:
    /// - TS38.211 Table 6.3.3.1-3 with range {0, ..., 837} for long preambles, and
    /// - TS38.211 Table 6.3.3.1-4 with range {0, ..., 137} for short preambles.
    unsigned root_sequence_index;
    /// Index of the preamble to generate {0, ..., 63}.
    unsigned preamble_index;
    /// Restricted set configuration.
    restricted_set_config restricted_set;
    /// Cyclic shift configuration index {0, ..., 15}.
    unsigned zero_correlation_zone;
    /// Offset in PRBs between Point-A and a PRB overlapping with the lowest RE of the PRACH signal.
    unsigned rb_offset;
    /// Subcarrier spacing for PUSCH.
    subcarrier_spacing pusch_scs;
    /// Generate the sequence in frequency-domain.
    bool frequency_domain;
  };

  /// Default destructor.
  virtual ~prach_generator() = default;

  /// \brief Generates the PRACH sequence.
  ///
  /// If the parameter \c config.frequency_domain is false, it generates the time-domain modulated PRACH sequence.
  /// Otherwise, it generates the frequency-domain symbol.
  ///
  /// \param[in] config Provides the necessary parameters to generate the sequence.
  /// \return A read-only view of the generated sequence.
  virtual span<const cf_t> generate(const configuration& config) = 0;
};
} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PRACH_GENERATOR_H
