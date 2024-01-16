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

#include "../../../lib/phy/upper/uplink_request_processor_impl.h"
#include "../support/prach_buffer_test_doubles.h"
#include "upper_phy_rx_symbol_request_notifier_test_doubles.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/support/support_factories.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace {

TEST(UplinkRequestProcessor, process_prach_request_produces_event_that_request_capture_prach)
{
  upper_phy_rx_symbol_request_notifier_spy   symbol_notifier;
  std::vector<std::unique_ptr<prach_buffer>> config_prach_pool;
  config_prach_pool.push_back(std::make_unique<prach_buffer_spy>());
  std::unique_ptr<prach_buffer_pool> prach_pool = create_prach_buffer_pool(std::move(config_prach_pool));
  uplink_request_processor_impl      ul_request_processor(symbol_notifier, *prach_pool);
  prach_buffer_context               context;
  context.slot   = slot_point(0, 0, 0);
  context.sector = 0;

  ASSERT_FALSE(symbol_notifier.has_prach_result_been_notified());

  ul_request_processor.process_prach_request(context);

  ASSERT_TRUE(symbol_notifier.has_prach_result_been_notified());
}

} // namespace
