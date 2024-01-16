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

/// \file
/// \brief Baseband gateway transmit metadata structure declaration.

#pragma once

#include "baseband_gateway_timestamp.h"
#include "srsran/adt/optional.h"

namespace srsran {

/// \brief Baseband transmitter metadata parameters.
///
/// Provides additional information about the contents of a baseband buffer that is meant to be transmitted.
struct baseband_gateway_transmitter_metadata {
  /// Baseband transmitter timestamp. Indicates the time the data needs to be transmitted at.
  baseband_gateway_timestamp ts;
  /// Empty baseband buffer flag. If set to \c true, the baseband buffer does not contain any transmit signal.
  bool is_empty;
  /// \brief Transmission start in samples.
  ///
  /// If present, the number of samples at the start of the baseband buffer that do not contain any baseband samples.
  /// Otherwise, the baseband buffer starts with transmit signal from the first sample.
  optional<unsigned> tx_start;
  /// \brief Transmission end in samples.
  ///
  /// If present, it is the sample index in which there is no signal until the end of the buffer. Otherwise, the
  /// baseband buffer contains transmit signal until the last sample.
  optional<unsigned> tx_end;
};

} // namespace srsran