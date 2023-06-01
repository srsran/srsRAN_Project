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
#include "srsran/gateways/baseband/baseband_gateway_base.h"
#include "srsran/gateways/baseband/baseband_gateway_timestamp.h"

namespace srsran {

class baseband_gateway_buffer_writer;

/// Baseband gateway - reception interface.
class baseband_gateway_receiver : public baseband_gateway_base
{
public:
  /// Receiver metadata.
  struct metadata {
    /// Timestamp of the received baseband signal.
    baseband_gateway_timestamp ts;
  };

  /// \brief Receives a number of baseband samples.
  /// \param[out,in] data Buffer of baseband samples.
  /// \return Receiver metadata.
  /// \note The \c data buffer provides the number of samples to receive through \ref
  ///       baseband_gateway_buffer::get_nof_samples.
  /// \note The \c data buffer must have the same number of channels as the stream.
  virtual metadata receive(baseband_gateway_buffer_writer& data) = 0;
};

} // namespace srsran