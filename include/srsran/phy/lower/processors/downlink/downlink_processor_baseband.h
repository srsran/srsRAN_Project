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
