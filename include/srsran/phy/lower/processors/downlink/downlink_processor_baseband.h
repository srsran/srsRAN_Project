/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/gateways/baseband/baseband_gateway_timestamp.h"
#include "srsran/gateways/baseband/baseband_gateway_transmitter_metadata.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_pool.h"

namespace srsran {

/// \brief Lower physical layer downlink processor - Baseband interface.
///
/// Processes baseband samples, it derives the symbol and slot timing from the provided timestamp.
class downlink_processor_baseband
{
public:
  /// Default destructor.
  virtual ~downlink_processor_baseband() = default;

  /// Baseband processing result.
  struct processing_result {
    /// Baseband generation metadata.
    baseband_gateway_transmitter_metadata metadata;
    /// Smart pointer to a baseband buffer.
    baseband_gateway_buffer_ptr buffer;
  };

  /// \brief Processes baseband at the given timestamp.
  ///
  /// If the given timestamp does not match with a slot boundary or there is no transmit request, it returns a valid
  /// buffer filled with zeros. Otherwise, it returns a valid buffer with the modulated baseband signal for the given
  /// slot.
  ///
  /// \param timestamp Given timestamp as a number of samples passed since an known epoch in time.
  /// \return The processing results with a valid buffer and the metadata indicating the contents on the buffer.
  virtual processing_result process(baseband_gateway_timestamp timestamp) = 0;
};

} // namespace srsran
