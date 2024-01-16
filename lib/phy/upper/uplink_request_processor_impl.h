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
