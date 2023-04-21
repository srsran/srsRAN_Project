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

class baseband_gateway_buffer_reader;

/// Baseband gateway - transmission interface.
class baseband_gateway_transmitter : public baseband_gateway_base
{
public:
  /// Transmitter metadata.
  struct metadata {
    /// Baseband transmitter timestamp. Indicates the time the data needs to be transmitted at.
    baseband_gateway_timestamp ts;
  };

  /// \brief Transmits a set of baseband samples at the time instant provided in the metadata.
  /// \param[in] data     Buffer of baseband samples to transmit.
  /// \param[in] metadata Additional parameters for transmission.
  /// \remark The data buffers must have the same number of channels as the stream.
  virtual void transmit(const baseband_gateway_buffer_reader& data, const metadata& metadata) = 0;
};

} // namespace srsran