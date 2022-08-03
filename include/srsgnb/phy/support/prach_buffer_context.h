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

#include "srsgnb/ran/prach/prach_preamble_format.h"
#include "srsgnb/ran/prach/prach_subcarrier_spacing.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/subcarrier_spacing.h"

namespace srsgnb {

/// \brief Collects PRACH time and frequency mapping parameters.
///
/// It contains the necessary parameters for the lower PHY to capture and demodulate the PRACH sequences. Also, it
/// provides the upper PHY with the necessary information to perform the sequence detection.
///
/// \note In future versions of the software, this structure might be upgraded to support multiple PRACH occasions, both
/// in time and frequency domains.
struct prach_buffer_context {
  /// Sector identifier.
  unsigned sector;
  /// Port identifier within the sector.
  unsigned port;
  /// Slot context within the system frame.
  slot_point slot;
  /// \brief OFDM symbol index within the slot that marks the start of the acquisition window for the first time-domain
  /// PRACH occasion.
  ///
  /// Here, OFDM symbol indexing assumes a subcarrier spacing equal to \c pusch_scs.
  unsigned start_symbol;
  /// Preamble format.
  preamble_format format;
  /// \brief Offset, in PRBs, between Point A and the PRB overlapping with the lowest RE of the first frequency-domain
  /// PRACH occasion.
  ///
  /// The number of PRBs is computed assuming a subcarrier spacing equal to \c pusch_scs.
  unsigned rb_offset;
  /// PUSCH subcarrier spacing.
  subcarrier_spacing pusch_scs;
};

} // namespace srsgnb
