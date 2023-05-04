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
#include "srsran/gateways/baseband/baseband_gateway_base.h"
#include "srsran/gateways/baseband/baseband_gateway_timestamp.h"

namespace srsran {

class baseband_gateway_buffer;

/// Baseband gateway - reception interface.
class baseband_gateway_receiver : public baseband_gateway_base
{
public:
  /// Receiver metadata.
  struct metadata {
    /// Timestamp of the received baseband signal.
    baseband_gateway_timestamp ts;
  };

  /// \brief Gets the buffer size.
  virtual unsigned get_buffer_size() const = 0;

  /// \brief Receives a number of baseband samples.
  /// \param[out,in] data Buffer of baseband samples.
  /// \return Receiver metadata.
  /// \note The \c data buffer provides the number of samples to receive through \ref
  ///       baseband_gateway_buffer::get_nof_samples.
  /// \note The \c data buffer must have the same number of channels as the stream.
  virtual metadata receive(baseband_gateway_buffer& data) = 0;
};

} // namespace srsran