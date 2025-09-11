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

#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_pool.h"
#include "srsran/phy/lower/lower_phy_baseband_metrics.h"
#include "srsran/ran/slot_point.h"
#include <optional>

namespace srsran {

class baseband_gateway_buffer_writer;
struct lower_phy_rx_symbol_context;

/// \brief Lower physical layer PDxCH processor - Baseband interface.
///
/// Processes baseband samples with slot granularity.
class pdxch_processor_baseband
{
public:
  /// Default destructor.
  virtual ~pdxch_processor_baseband() = default;

  /// Describes the context of a newly generated slot.
  struct slot_context {
    /// Slot context.
    slot_point slot;
    /// Radio sector identifier.
    unsigned sector;
  };

  /// Groups baseband metrics and buffer for a slot.
  struct slot_result {
    /// Collected baseband buffer metrics.
    lower_phy_baseband_metrics metrics = {};
    /// Actual baseband buffer. Set to nullptr if there was no transmit request in the given slot.
    baseband_gateway_buffer_ptr buffer = nullptr;
  };

  /// \brief Processes a baseband OFDM slot.
  ///
  /// \param[in] context OFDM Symbol context.
  /// \return Slot downlink baseband results.
  virtual slot_result process_slot(slot_context context) = 0;
};

} // namespace srsran
