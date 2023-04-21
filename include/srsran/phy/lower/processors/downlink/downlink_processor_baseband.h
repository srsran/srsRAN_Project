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

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/gateways/baseband/baseband_gateway_timestamp.h"

namespace srsran {

class baseband_gateway_buffer;

/// \brief Lower physical layer downlink processor - Baseband interface.
///
/// Processes baseband samples, it derives the symbol and slot timing from the number of processed samples.
class downlink_processor_baseband
{
public:
  /// Default destructor.
  virtual ~downlink_processor_baseband() = default;

  /// \brief Processes any number of baseband samples.
  ///
  /// \param[in] buffer    Baseband samples to process.
  /// \param[in] timestamp Time instant in which the first sample in the buffer is transmitted.
  /// \remark The number of channels in \c buffer must be equal to the number of transmit ports for the sector.
  virtual void process(baseband_gateway_buffer_writer& buffer, baseband_gateway_timestamp timestamp) = 0;
};

} // namespace srsran
