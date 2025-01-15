/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/support/prach_buffer.h"
#include "srsran/ran/prach/prach_format_type.h"
#include "srsran/ran/slot_point.h"

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
    /// \brief OFDM slot in which the PRACH window starts.
    ///
    /// The slot contains uplink resource grid subcarrier spacing. Expresses the numerology \f$\mu\f$.
    slot_point slot;
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
