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

#include "srsran/phy/upper/uplink_request_processor.h"

namespace srsran {

class prach_buffer_pool;
class upper_phy_rx_symbol_request_notifier;

/// \brief Implementation of the uplink request processor interface.
///
/// This implementation processes uplink data requests according to the following steps.
///     1. It resolves all the dependencies.
///     2. It calls a method from the internal upper_phy_rx_symbol_request_notifier object. This will produce and notify
///        an event that requests the capture of slot symbol data.
class uplink_request_processor_impl : public uplink_request_processor
{
public:
  uplink_request_processor_impl(upper_phy_rx_symbol_request_notifier& rx_symbol_request_notifier_,
                                prach_buffer_pool&                    prach_pool_);

  // See interface for documentation.
  void process_prach_request(const prach_buffer_context& context) override;

  // See interface for documentation.
  void process_uplink_slot_request(const resource_grid_context& context, resource_grid& grid) override;

private:
  /// Symbol request notifier.
  upper_phy_rx_symbol_request_notifier& rx_symbol_request_notifier;
  /// PRACH buffer pool.
  prach_buffer_pool& prach_pool;
};

} // namespace srsran
