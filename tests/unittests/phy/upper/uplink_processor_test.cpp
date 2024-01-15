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

#include "../../../lib/phy/upper/uplink_processor_impl.h"
#include "../support/prach_buffer_test_doubles.h"
#include "channel_processors/prach_detector_test_doubles.h"
#include "channel_processors/pucch_processor_test_doubles.h"
#include "channel_processors/pusch/pusch_processor_test_doubles.h"
#include "upper_phy_rx_results_notifier_test_doubles.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace {
TEST(UplinkProcessor, calling_process_prach_enqueue_task)
{
  auto                              prach      = std::make_unique<prach_detector_spy>();
  const prach_detector_spy&         prach_spy  = *prach;
  auto                              pusch_proc = std::make_unique<pusch_processor_dummy>();
  auto                              pucch_proc = std::make_unique<pucch_processor_dummy>();
  upper_phy_rx_results_notifier_spy results_notifier;
  uplink_processor_impl             ul_processor(std::move(prach), std::move(pusch_proc), std::move(pucch_proc));

  prach_buffer_spy buffer;
  ul_processor.process_prach(results_notifier, buffer, {});

  ASSERT_TRUE(prach_spy.has_detect_method_been_called());
}

TEST(UplinkProcessor, after_detect_prach_is_executed_results_notifier_is_called)
{
  auto                              prach_spy = std::make_unique<prach_detector_spy>();
  upper_phy_rx_results_notifier_spy results_notifier;
  auto                              pusch_proc = std::make_unique<pusch_processor_dummy>();
  auto                              pucch_proc = std::make_unique<pucch_processor_dummy>();

  uplink_processor_impl ul_processor(std::move(prach_spy), std::move(pusch_proc), std::move(pucch_proc));

  prach_buffer_spy buffer;
  ul_processor.process_prach(results_notifier, buffer, {});

  ASSERT_TRUE(results_notifier.has_prach_result_been_notified());
}

} // namespace
