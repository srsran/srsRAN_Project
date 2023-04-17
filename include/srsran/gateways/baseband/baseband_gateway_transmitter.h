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

/// Describes a baseband gateway interface for transmission.
class baseband_gateway_transmitter : public baseband_gateway_base
{
public:
  /// Describes transmitter metadata.
  struct metadata {
    /// Baseband transmitter timestamp. Indicates the time the data needs to be transmitted.
    baseband_gateway_timestamp ts;
  };

  /// Gets the optimal transmitter buffer size.
  virtual unsigned get_buffer_size() const = 0;

  /// \brief Transmits a given baseband buffer in a given time provided in the metadata.
  /// \param[in] data     Baseband buffer to transmit.
  /// \param[in] metadata Additional parameters for transmission.
  /// \remark The data buffers must have the same number of channels than the stream.
  virtual void transmit(baseband_gateway_buffer& data, const metadata& metadata) = 0;
};

} // namespace srsran