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

#include "srsran/adt/static_vector.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/frequency_range.h"
#include "srsran/ran/prach/prach_format_type.h"
#include "srsran/ran/slot_point.h"
#include <cstdint>

#pragma once

namespace srsran {

/// \brief Contains the PRACH configuration parameters.
///
/// The parameters are used in TS38.211 Section 6.3.3.2 and they are derived from TS38.211 Tables 6.3.3.2-2, 6.3.3.2-3
/// and 6.3.3.2-4.
///
/// The PRACH transmission occasions are in the system frames \f$n_{SFN}\f$ that satisfy \f$n_{SFN} \bmod x = y\f$.
struct prach_configuration {
  /// Preamble format (see [here](\ref preamble_format) for more information).
  prach_format_type format;
  /// SFN period, \f$x\f$.
  unsigned x;
  /// SFN offset \f$y\f$.
  unsigned y;
  /// Subframes within a frame that contain PRACH occasions.
  static_vector<uint8_t, NOF_SUBFRAMES_PER_FRAME> subframe;
  /// Starting OFDM symbol index.
  uint8_t starting_symbol;
  /// Number of PRACH slots within a subframe. Set zero for reserved.
  uint8_t nof_prach_slots_within_subframe;
  /// Number of time-domain PRACH occasions within a PRACH slot. Set zero for reserved.
  uint8_t nof_occasions_within_slot;
  /// PRACH duration in symbols. Only valid for short preamble formats.
  uint8_t duration;
};

/// Reserved PRACH configuration. Indicates the configuration parameters are invalid.
static const prach_configuration PRACH_CONFIG_RESERVED =
    {prach_format_type::invalid, UINT32_MAX, UINT32_MAX, {}, 0, 0, 0, 0};

/// \brief Gets a PRACH configuration.
///
/// Arguments are considered valid if:
/// - They are consistent with the TS38.211 Tables 6.3.3.2-2, 6.3.3.2-3 and 6.3.3.2-4, and
/// - the combination is implemented.
///
/// Currently, only long preamble formats for FR1 in FDD mode are implemented.
///
/// \param[in] fr                 Frequency range (see [here](\ref frequency_range) for more information).
/// \param[in] dm                 Duplex mode (see [here](\ref duplex_mode) for more information).
/// \param[in] prach_config_index PRACH configuration index with range {0, ..., 255}.
/// \return A valid PRACH configuration if the provided arguments are valid. Otherwise, \ref PRACH_CONFIG_RESERVED.
prach_configuration prach_configuration_get(frequency_range fr, duplex_mode dm, uint8_t prach_config_index);

} // namespace srsran
