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

#include "../../../../lib/ofh/receiver/ofh_uplane_prach_data_flow_notifier.h"
#include "../ofh_uplane_rx_symbol_notifier_test_doubles.h"
#include "helpers.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ofh;
using namespace ofh::testing;

TEST(ofh_uplane_prach_data_flow_notifier, empty_context_does_not_notify)
{
  auto                            notifier = std::make_shared<uplane_rx_symbol_notifier_spy>();
  auto                            repo     = std::make_shared<prach_context_repository>(1);
  uplane_prach_data_flow_notifier sender(srslog::fetch_basic_logger("TEST"), repo, notifier);
  slot_point                      slot(0, 0, 1);

  sender.notify_prach(slot);

  ASSERT_TRUE(repo->get(slot).empty());
  ASSERT_FALSE(notifier->has_new_uplink_symbol_function_been_called());
  ASSERT_FALSE(notifier->has_new_prach_function_been_called());
}

TEST(ofh_uplane_prach_data_flow_notifier, unwritten_buffer_does_not_notify)
{
  auto                            notifier = std::make_shared<uplane_rx_symbol_notifier_spy>();
  auto                            repo     = std::make_shared<prach_context_repository>(1);
  uplane_prach_data_flow_notifier sender(srslog::fetch_basic_logger("TEST"), repo, notifier);
  slot_point                      slot(0, 0, 1);
  prach_buffer_dummy              buffer(1);
  prach_buffer_context            context;
  context.slot             = slot;
  context.format           = prach_format_type::zero;
  context.ports            = {0};
  context.nof_td_occasions = 1;
  context.nof_fd_occasions = 1;
  context.pusch_scs        = srsran::subcarrier_spacing::kHz30;
  context.start_symbol     = 0;

  repo->add(context, buffer);
  sender.notify_prach(slot);

  ASSERT_FALSE(repo->get(slot).empty());
  ASSERT_FALSE(notifier->has_new_uplink_symbol_function_been_called());
  ASSERT_FALSE(notifier->has_new_prach_function_been_called());
}

TEST(ofh_uplane_prach_data_flow_notifier, completed_long_prach_buffer_triggers_notification)
{
  auto                            notifier = std::make_shared<uplane_rx_symbol_notifier_spy>();
  auto                            repo     = std::make_shared<prach_context_repository>(1);
  uplane_prach_data_flow_notifier sender(srslog::fetch_basic_logger("TEST"), repo, notifier);
  slot_point                      slot(0, 0, 1);
  unsigned                        symbol = 0;
  unsigned                        port   = 0;
  prach_buffer_dummy              buffer(1);
  prach_buffer_context            context;
  context.slot             = slot;
  context.format           = prach_format_type::zero;
  context.ports            = {0};
  context.nof_td_occasions = 1;
  context.nof_fd_occasions = 1;
  context.pusch_scs        = srsran::subcarrier_spacing::kHz30;
  context.start_symbol     = 0;

  static_vector<cf_t, 839> samples(839);
  repo->add(context, buffer);
  ASSERT_FALSE(repo->get(slot).empty());

  // Fill the grid.
  repo->write_iq(slot, port, symbol, 0, samples);

  sender.notify_prach(slot);

  // Assert that the context is cleared when it is notified.
  ASSERT_TRUE(repo->get(slot).empty());

  // Assert the notification.
  ASSERT_FALSE(notifier->has_new_uplink_symbol_function_been_called());
  ASSERT_TRUE(notifier->has_new_prach_function_been_called());
}

TEST(ofh_uplane_prach_data_flow_notifier, completed_short_prach_buffer_triggers_notification)
{
  auto                            notifier = std::make_shared<uplane_rx_symbol_notifier_spy>();
  auto                            repo     = std::make_shared<prach_context_repository>(1);
  uplane_prach_data_flow_notifier sender(srslog::fetch_basic_logger("TEST"), repo, notifier);
  slot_point                      slot(0, 0, 1);
  unsigned                        port = 0;
  prach_buffer_dummy              buffer(1, false);
  prach_buffer_context            context;
  context.slot             = slot;
  context.format           = prach_format_type::B4;
  context.ports            = {0};
  context.nof_td_occasions = 1;
  context.nof_fd_occasions = 1;
  context.pusch_scs        = srsran::subcarrier_spacing::kHz30;
  context.start_symbol     = 0;

  static_vector<cf_t, 139> samples(139);
  repo->add(context, buffer);
  ASSERT_FALSE(repo->get(slot).empty());

  // Fill the grid.
  for (unsigned i = 0; i != 12; ++i) {
    repo->write_iq(slot, port, i, 0, samples);
  }

  sender.notify_prach(slot);

  // Assert that the context is cleared when it is notified.
  ASSERT_TRUE(repo->get(slot).empty());

  // Assert the notification.
  ASSERT_FALSE(notifier->has_new_uplink_symbol_function_been_called());
  ASSERT_TRUE(notifier->has_new_prach_function_been_called());
}
