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

#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/phy/upper/uplink_slot_processor.h"
#include "srsran/phy/upper/upper_phy_rx_symbol_handler.h"

namespace srsran {

/// \brief Implementation of the upper PHY handler of receive symbols events.
///
/// In this implementation, handling the newly received symbol event takes the following steps.
///     1. Get an uplink processor from the pool.
///     2. Call the corresponding method from the uplink processor based on the type of the received symbol (PRACH, SRS,
///        PUSCH).
class upper_phy_rx_symbol_handler_impl : public upper_phy_rx_symbol_handler
{
public:
  upper_phy_rx_symbol_handler_impl(uplink_slot_processor_pool& ul_processor_pool_);

  // See interface for documentation.
  void handle_rx_symbol(const upper_phy_rx_symbol_context& context, const shared_resource_grid& grid) override;

  // See interface for documentation.
  void handle_rx_prach_window(const prach_buffer_context& context, const prach_buffer& buffer) override;

private:
  /// Uplink processor pool.
  uplink_slot_processor_pool& ul_processor_pool;
};

} // namespace srsran
