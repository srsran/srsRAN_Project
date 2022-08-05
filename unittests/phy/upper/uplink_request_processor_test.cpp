/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/phy/upper/prach_circular_array.h"
#include "../../../lib/phy/upper/uplink_request_processor_impl.h"
#include "../support/prach_buffer_test_doubles.h"
#include "srsgnb/phy/support/support_factories.h"
#include "upper_phy_rx_symbol_request_notifier_test_doubles.h"
#include <gtest/gtest.h>

using namespace srsgnb;

namespace {

TEST(UplinkRequestProcessor, process_prach_request_adds_config_to_ring_buffer)
{
  upper_phy_rx_symbol_request_notifier_spy   symbol_notifier;
  std::vector<std::unique_ptr<prach_buffer>> config_prach_pool;
  config_prach_pool.push_back(std::make_unique<prach_buffer_dummy>());
  std::unique_ptr<prach_buffer_pool> prach_pool = create_prach_buffer_pool(std::move(config_prach_pool));
  prach_circular_array               ring_configs;
  uplink_request_processor_impl      ul_request_processor(symbol_notifier, *prach_pool, ring_configs);
  prach_buffer_context               context;
  context.slot   = slot_point(0, 0, 0);
  context.sector = 0;
  prach_detector::slot_configuration config;
  ASSERT_TRUE(ring_configs[context.slot].empty());

  ul_request_processor.process_prach_request(context, config);

  ASSERT_FALSE(ring_configs[context.slot].empty());
  // Check the stored data.
  const prach_detector::slot_configuration& stored_config = ring_configs[context.slot].back();
  ASSERT_EQ(config.format, stored_config.format);
  ASSERT_EQ(config.frequency_offset, stored_config.frequency_offset);
  ASSERT_EQ(config.nof_preamble_indices, stored_config.nof_preamble_indices);
  ASSERT_EQ(config.pusch_scs, stored_config.pusch_scs);
  ASSERT_EQ(config.restricted_set, stored_config.restricted_set);
  ASSERT_EQ(config.root_sequence_index, stored_config.root_sequence_index);
  ASSERT_EQ(config.start_preamble_index, stored_config.start_preamble_index);
  ASSERT_EQ(config.zero_correlation_zone, stored_config.zero_correlation_zone);
}

TEST(UplinkRequestProcessor, process_prach_request_produces_event_that_request_capture_prach)
{
  upper_phy_rx_symbol_request_notifier_spy   symbol_notifier;
  std::vector<std::unique_ptr<prach_buffer>> config_prach_pool;
  config_prach_pool.push_back(std::make_unique<prach_buffer_dummy>());
  std::unique_ptr<prach_buffer_pool> prach_pool = create_prach_buffer_pool(std::move(config_prach_pool));
  prach_circular_array               ring_configs;
  uplink_request_processor_impl      ul_request_processor(symbol_notifier, *prach_pool, ring_configs);
  prach_buffer_context               context;
  context.slot   = slot_point(0, 0, 0);
  context.sector = 0;
  prach_detector::slot_configuration config;

  ASSERT_FALSE(symbol_notifier.has_prach_result_been_notified());

  ul_request_processor.process_prach_request(context, config);

  ASSERT_TRUE(symbol_notifier.has_prach_result_been_notified());
}

} // namespace
