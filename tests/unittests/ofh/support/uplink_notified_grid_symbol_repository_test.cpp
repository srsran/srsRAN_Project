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

#include "uplink_notified_grid_symbol_repository.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ofh;

TEST(uplink_notified_grid_symbol_repository, slot_not_added_to_repo_do_not_return_symbols_to_notify)
{
  uplink_notified_grid_symbol_repository repo(20);
  slot_point                             slot(subcarrier_spacing::kHz30, 0, 1);

  for (unsigned i = 0; i != MAX_NSYMB_PER_SLOT; ++i) {
    auto symbols_to_notify = repo.update_rx_symbol_and_compute_symbols_to_notify(slot, i, true);
    ASSERT_FALSE(symbols_to_notify);
  }
}

TEST(uplink_notified_grid_symbol_repository, symbols_arriving_in_order_return_one_symbol)
{
  uplink_notified_grid_symbol_repository repo(20);
  slot_point                             slot(subcarrier_spacing::kHz30, 0, 1);

  repo.add(slot);

  for (unsigned i = 0; i != MAX_NSYMB_PER_SLOT; ++i) {
    auto symbols_to_notify = repo.update_rx_symbol_and_compute_symbols_to_notify(slot, i, i % 2);
    ASSERT_TRUE(symbols_to_notify);
    ASSERT_EQ(symbols_to_notify->symbols, interval<unsigned>::start_and_len(i, 1));
    ASSERT_EQ(i % 2, symbols_to_notify->valid_symbols.test(i));
  }
}

TEST(uplink_notified_grid_symbol_repository,
     symbols_arriving_in_reverse_order_return_all_symbols_on_last_received_symbols)
{
  uplink_notified_grid_symbol_repository repo(20);
  slot_point                             slot(subcarrier_spacing::kHz30, 0, 1);

  repo.add(slot);

  for (unsigned i = MAX_NSYMB_PER_SLOT - 1; i; --i) {
    auto symbols_to_notify = repo.update_rx_symbol_and_compute_symbols_to_notify(slot, i, true);
    ASSERT_FALSE(symbols_to_notify);
  }

  auto symbols_to_notify = repo.update_rx_symbol_and_compute_symbols_to_notify(slot, 0, true);
  ASSERT_TRUE(symbols_to_notify);
  ASSERT_EQ(symbols_to_notify->symbols, interval<unsigned>::start_and_len(0, MAX_NSYMB_PER_SLOT));
  for (unsigned i = 0; i != MAX_NSYMB_PER_SLOT; ++i) {
    ASSERT_TRUE(symbols_to_notify->valid_symbols.test(i));
  }
}

TEST(uplink_notified_grid_symbol_repository, first_four_symbols_delayed_rest_on_time)
{
  uplink_notified_grid_symbol_repository repo(20);
  slot_point                             slot(subcarrier_spacing::kHz30, 0, 1);

  repo.add(slot);

  // Symbols 4-10
  for (unsigned i = 4, e = 11; i != e; ++i) {
    auto symbols_to_notify = repo.update_rx_symbol_and_compute_symbols_to_notify(slot, i, i % 2);
    ASSERT_FALSE(symbols_to_notify);
  }

  // Symbols 0-2
  for (unsigned i = 0, e = 3; i != e; ++i) {
    auto symbols_to_notify = repo.update_rx_symbol_and_compute_symbols_to_notify(slot, i, false);
    ASSERT_TRUE(symbols_to_notify);
    ASSERT_EQ(symbols_to_notify->symbols, interval<unsigned>::start_and_len(i, 1));
    ASSERT_FALSE(symbols_to_notify->valid_symbols.test(i));
  }

  // Symbol 3.
  {
    auto symbols_to_notify = repo.update_rx_symbol_and_compute_symbols_to_notify(slot, 3, true);
    ASSERT_TRUE(symbols_to_notify);
    ASSERT_EQ(symbols_to_notify->symbols, interval<unsigned>::start_and_len(3, 11 - 3));
    for (unsigned i = 3, e = 11; i != e; ++i) {
      ASSERT_EQ(i % 2, symbols_to_notify->valid_symbols.test(i));
    }
  }

  // Symbols 11-13
  for (unsigned i = 11; i != MAX_NSYMB_PER_SLOT; ++i) {
    auto symbols_to_notify = repo.update_rx_symbol_and_compute_symbols_to_notify(slot, i, true);
    ASSERT_TRUE(symbols_to_notify);
    ASSERT_EQ(symbols_to_notify->symbols, interval<unsigned>::start_and_len(i, 1));
    ASSERT_TRUE(symbols_to_notify->valid_symbols.test(i));
  }
}

TEST(uplink_notified_grid_symbol_repository, special_slot_notified_correctly)
{
  uplink_notified_grid_symbol_repository repo(20);
  unsigned                               start_symbol = 9;
  slot_point                             slot(subcarrier_spacing::kHz30, 0, 1);

  repo.add(slot, start_symbol);

  for (unsigned i = start_symbol; i != MAX_NSYMB_PER_SLOT; ++i) {
    auto symbols_to_notify = repo.update_rx_symbol_and_compute_symbols_to_notify(slot, i, true);
    ASSERT_TRUE(symbols_to_notify);
    ASSERT_EQ(symbols_to_notify->symbols, interval<unsigned>::start_and_len(i, 1));
  }
}

TEST(uplink_notified_grid_symbol_repository, extended_cyclic_prefix_notified)
{
  uplink_notified_grid_symbol_repository repo(20);
  slot_point                             slot(subcarrier_spacing::kHz30, 0, 1);
  unsigned                               nof_symbols = get_nsymb_per_slot(cyclic_prefix::EXTENDED);

  repo.add(slot, 0, cyclic_prefix::EXTENDED);

  for (unsigned i = 0; i != nof_symbols; ++i) {
    auto symbols_to_notify = repo.update_rx_symbol_and_compute_symbols_to_notify(slot, i, true);
    ASSERT_TRUE(symbols_to_notify);
    ASSERT_EQ(symbols_to_notify->symbols.length(), 1);
    ASSERT_EQ(symbols_to_notify->symbols, interval<unsigned>::start_and_len(i, 1));
  }
}

TEST(uplink_notified_grid_symbol_repository, extended_cyclic_prefix_symbol_in_reversed_order_notified)
{
  uplink_notified_grid_symbol_repository repo(20);
  slot_point                             slot(subcarrier_spacing::kHz30, 0, 1);
  unsigned                               nof_symbols = get_nsymb_per_slot(cyclic_prefix::EXTENDED);

  repo.add(slot, 0, cyclic_prefix::EXTENDED);

  for (unsigned i = nof_symbols - 1; i; --i) {
    auto symbols_to_notify = repo.update_rx_symbol_and_compute_symbols_to_notify(slot, i, true);
    ASSERT_FALSE(symbols_to_notify);
  }

  auto symbols_to_notify = repo.update_rx_symbol_and_compute_symbols_to_notify(slot, 0, true);
  ASSERT_TRUE(symbols_to_notify);
  ASSERT_EQ(symbols_to_notify->symbols.length(), nof_symbols);
  ASSERT_EQ(symbols_to_notify->symbols, interval<unsigned>::start_and_len(0, nof_symbols));
}

TEST(uplink_notified_grid_symbol_repository, extended_cyclic_prefix_receives_more_symbols_no_not_notify_extra_symbols)
{
  uplink_notified_grid_symbol_repository repo(20);
  slot_point                             slot(subcarrier_spacing::kHz30, 0, 1);
  unsigned                               nof_symbols = get_nsymb_per_slot(cyclic_prefix::EXTENDED);

  repo.add(slot, 0, cyclic_prefix::EXTENDED);

  // Get last 4 symbols of slot 1.
  for (unsigned i = 10; i != MAX_NSYMB_PER_SLOT; ++i) {
    auto symbols_to_notify = repo.update_rx_symbol_and_compute_symbols_to_notify(slot, i, true);
    ASSERT_FALSE(symbols_to_notify);
  }

  // Get the rest of the symbols for slot 1.
  for (unsigned i = 0, e = 10 - 1; i != e; ++i) {
    auto symbols_to_notify = repo.update_rx_symbol_and_compute_symbols_to_notify(slot, i, true);
    ASSERT_TRUE(symbols_to_notify);
    ASSERT_EQ(symbols_to_notify->symbols, interval<unsigned>::start_and_len(i, 1));
  }

  // Last symbol for slot 1 triggers the send of the stored ones.
  auto symbols_to_notify = repo.update_rx_symbol_and_compute_symbols_to_notify(slot, 9, true);
  ASSERT_TRUE(symbols_to_notify);
  ASSERT_EQ(symbols_to_notify->symbols, interval<unsigned>::start_and_len(9, nof_symbols - 9));
}

TEST(uplink_notified_grid_symbol_repository, different_slots_with_previous_slot_finishing_later)
{
  uplink_notified_grid_symbol_repository repo(20);
  slot_point                             slot(subcarrier_spacing::kHz30, 0, 1);
  slot_point                             slot2(subcarrier_spacing::kHz30, 0, 2);

  repo.add(slot);

  // Get last 4 symbols of slot 1.
  for (unsigned i = 10; i != MAX_NSYMB_PER_SLOT; ++i) {
    auto symbols_to_notify = repo.update_rx_symbol_and_compute_symbols_to_notify(slot, i, true);
    ASSERT_FALSE(symbols_to_notify);
  }

  repo.add(slot2);

  // Get all symbols for slot 2.
  for (unsigned i = 0; i != MAX_NSYMB_PER_SLOT; ++i) {
    auto symbols_to_notify = repo.update_rx_symbol_and_compute_symbols_to_notify(slot2, i, true);
    ASSERT_TRUE(symbols_to_notify);
    ASSERT_EQ(symbols_to_notify->symbols, interval<unsigned>::start_and_len(i, 1));
  }

  // Get the rest of the symbols for slot 1.
  for (unsigned i = 0, e = 10 - 1; i != e; ++i) {
    auto symbols_to_notify = repo.update_rx_symbol_and_compute_symbols_to_notify(slot, i, true);
    ASSERT_TRUE(symbols_to_notify);
    ASSERT_EQ(symbols_to_notify->symbols, interval<unsigned>::start_and_len(i, 1));
  }

  // Last symbol for slot 1 triggers the send of the stored ones.
  auto symbols_to_notify = repo.update_rx_symbol_and_compute_symbols_to_notify(slot, 9, true);
  ASSERT_TRUE(symbols_to_notify);
  ASSERT_EQ(symbols_to_notify->symbols, interval<unsigned>::start_and_len(9, MAX_NSYMB_PER_SLOT - 9));
}
