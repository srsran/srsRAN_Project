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

#include "srsran/phy/support/prach_buffer.h"
#include "srsran/ran/prach/prach_format_type.h"
#include "srsran/ran/subcarrier_spacing.h"

namespace srsran {

/// \brief OFDM PRACH demodulator.
///
/// Performs OFDM demodulation for all the PRACH frequency- and time-domain occasions within a PRACH slot as per
/// TS38.211 Section 5.3.2.
///
class ofdm_prach_demodulator
{
public:
  /// Collects the necessary parameters to demodulate PRACH frequency- and time-domain occasions within a slot.
  struct configuration {
    /// PRACH preamble format.
    prach_format_type format;
    /// Number of time-domain occasions.
    unsigned nof_td_occasions;
    /// Number of frequency-domain occasions.
    unsigned nof_fd_occasions;
    /// Starting symbol index for the first PRACH TD occasion within the slot {0, ..., 13} at \c pusch_scs.
    unsigned start_symbol;
    /// PRACH frequency location (see \ref prach_buffer_context::rb_offset).
    unsigned rb_offset;
    /// Uplink resource grid size (see \ref prach_buffer_context::nof_prb_ul_grid).
    unsigned nof_prb_ul_grid;
    /// Uplink resource grid subcarrier spacing. Expresses the numerology \f$\mu\f$.
    subcarrier_spacing pusch_scs;
    /// Destination port identifier.
    unsigned port;
  };

  /// Default destructor.
  virtual ~ofdm_prach_demodulator() = default;

  /// \brief Demodulates a PRACH occasion as described in TS38.211 Section 5.3.2.
  ///
  /// For long preambles, the input shall contain the cyclic prefix of length \f$N_{CP,l}^{RA}\f$) and a time-domain
  /// sequence of length \f$N_u\f$. Also, the number of time-domain occasions must be one.
  ///
  /// For short preamble formats, the input shall correspond to one slot at \ref configuration::pusch_scs.
  ///
  /// An assertion is triggered if:
  /// - the input samples are not sufficient,
  /// - the buffer dimensions cannot contain the demodulated sequences, or
  /// - the port identifier exceeds the number of ports contained in the destination buffer.
  ///
  /// \param[out] buffer    Frequency-domain signal corresponding to one PRACH receive occasion.
  /// \param[in] input      PRACH receive window baseband samples.
  /// \param[in] config     PRACH demodulator configuration parameters.
  virtual void demodulate(prach_buffer& buffer, span<const cf_t> input, const configuration& config) = 0;
};
} // namespace srsran