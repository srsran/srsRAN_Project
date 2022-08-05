/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/upper/channel_processors/prach_detector.h"
#include "srsgnb/phy/upper/upper_phy_rx_symbol_handler.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {

class task_executor;
class uplink_processor_pool;

/// \brief Implementation of the upper PHY handler of receive symbols events.
///
/// In this implementation, handling the newly received symbol event takes the following steps.
///     1. Get an uplink processor from the pool.
///     2. Call the corresponding method from the uplink processor based on the type of the received symbol (PRACH, SRS,
///     PUSCH).
class upper_phy_rx_symbol_handler_impl : public upper_phy_rx_symbol_handler
{
public:
  explicit upper_phy_rx_symbol_handler_impl(uplink_processor_pool& ul_processor_pool);

  // See interface for documentation.
  void handle_rx_symbol(const upper_phy_rx_symbol_context& context, const resource_grid_reader& grid) override;

  // See interface for documentation.
  void handle_rx_prach_symbol(const prach_buffer_context& context, const prach_buffer& buffer) override;

  // See interface for documentation.
  void handle_rx_srs_symbol(const upper_phy_rx_symbol_context& context) override;

private:
  /// Uplink processor pool.
  uplink_processor_pool& ul_processor_pool;
};

} // namespace srsgnb
