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
