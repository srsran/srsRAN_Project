/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/ran/prach/prach_preamble_format.h"
#include "srsran/ran/prach/prach_subcarrier_spacing.h"
#include "srsran/ran/prach/restricted_set_config.h"
#include "srsran/ran/slot_point.h"
#include "srsran/ran/subcarrier_spacing.h"

namespace srsran {

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
  /// Corresponds to parameter \f$k_1/N_{\textup{sc}}^{\textup{RB}}\f$ in TS38.211 Section 5.3.2. The number of PRBs is
  /// computed assuming a subcarrier spacing equal to \c pusch_scs.
  unsigned rb_offset;
  /// \brief Uplink resource grid size in PRBs.
  ///
  /// Corresponds to parameter \f$N_{grid}^{size,\mu}\f$ in TS38.211 Section 5.3.2. The number of PRBs is computed
  /// assuming a subcarrier spacing equal to \c pusch_scs.
  unsigned nof_prb_ul_grid;
  /// PUSCH subcarrier spacing, parameter \f$\mu\f$ in TS38.211 Section 5.3.2.
  subcarrier_spacing pusch_scs;
  /// Root sequence index {0, ..., 837}.
  unsigned root_sequence_index;
  /// Restricted set configuration.
  restricted_set_config restricted_set;
  /// Zero-correlation zone configuration index to calculate \f$N_{CS}\f$ as per TS38.211 section 6.3.3.1. Range {0,
  /// ..., 15}.
  unsigned zero_correlation_zone;
  /// Start of preamble logical index to monitor the PRACH occasions signaled in this slot. Range {0, ..., 63}.
  unsigned start_preamble_index;
  /// \brief Number of preamble indices to monitor {1, ..., 64}.
  ///
  /// The sum <tt>start_preamble_index + nof_preamble_indices</tt> should not exceed 64.
  unsigned nof_preamble_indices;
};

} // namespace srsran
