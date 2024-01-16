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

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/gateways/baseband/baseband_gateway_timestamp.h"

namespace srsran {

class baseband_gateway_buffer;

/// \brief Lower physical layer uplink processor - Baseband interface.
///
/// Processes baseband samples. It derives the symbol and slot timing from the number of processed samples.
class uplink_processor_baseband
{
public:
  /// Default destructor.
  virtual ~uplink_processor_baseband() = default;

  /// \brief Processes any number of baseband samples.
  ///
  /// \param[in] buffer    Baseband samples to process.
  /// \param[in] timestamp Time instant in which the first sample was captured.
  /// \remark The number of channels in \c buffer must be equal to the number of receive ports for the sector.
  virtual void process(const baseband_gateway_buffer_reader& buffer, baseband_gateway_timestamp timestamp) = 0;
};

} // namespace srsran
