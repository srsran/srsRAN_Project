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

#include "../../../../lib/ofh/receiver/ofh_rx_window_checker.h"
#include "srsran/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ofh;
using namespace std::chrono_literals;

TEST(ofh_rx_window_checker, on_time_packet_counts_one_packet)
{
  // Create window checker with timing parameters corresponding to Ta4_min=50us, Ta4_max=300us.
  rx_window_checker rx_window(true, {2, 9});

  // Create the OTA notification.
  slot_symbol_point ota_slot({1, 1, 1, 1}, 7, 14);
  rx_window.on_new_symbol({ota_slot, {}});

  slot_symbol_point message_slot = ota_slot - 4;
  rx_window.update_rx_window_statistics(message_slot);
  ASSERT_EQ(1, rx_window.nof_on_time_messages());
  ASSERT_EQ(0, rx_window.nof_early_messages());
  ASSERT_EQ(0, rx_window.nof_late_messages());
}

TEST(ofh_rx_window_checker, packet_on_the_window_start_count_as_valid)
{
  // Create window checker with timing parameters corresponding to Ta4_min=50us, Ta4_max=300us.
  rx_window_checker rx_window(true, {2, 9});

  // Create the OTA notification.
  slot_symbol_point ota_slot({1, 1, 1, 1}, 7, 14);
  rx_window.on_new_symbol({ota_slot, {}});

  slot_symbol_point message_slot = ota_slot - 2;
  rx_window.update_rx_window_statistics(message_slot);
  ASSERT_EQ(1, rx_window.nof_on_time_messages());
  ASSERT_EQ(0, rx_window.nof_early_messages());
  ASSERT_EQ(0, rx_window.nof_late_messages());
}

TEST(ofh_rx_window_checker, packet_on_the_window_end_count_as_valid)
{
  // Create window checker with timing parameters corresponding to Ta4_min=50us, Ta4_max=510us.
  rx_window_checker rx_window(true, {2, 15});

  // Create the OTA notification.
  slot_symbol_point ota_slot({1, 1, 1, 1}, 7, 14);
  rx_window.on_new_symbol({ota_slot, {}});

  slot_symbol_point message_slot = ota_slot - 14;
  rx_window.update_rx_window_statistics(message_slot);
  ASSERT_EQ(1, rx_window.nof_on_time_messages());
  ASSERT_EQ(0, rx_window.nof_early_messages());
  ASSERT_EQ(0, rx_window.nof_late_messages());
}

TEST(ofh_rx_window_checker, early_packet_counts_one_packet)
{
  // Create window checker with timing parameters corresponding to Ta4_min=80us, Ta4_max=300us.
  rx_window_checker rx_window(true, {3, 9});

  // Create the OTA notification.
  slot_symbol_point ota_slot({1, 1, 1, 1}, 7, 14);
  rx_window.on_new_symbol({ota_slot, {}});

  slot_symbol_point message_slot = ota_slot - 1;
  rx_window.update_rx_window_statistics(message_slot);
  ASSERT_EQ(0, rx_window.nof_on_time_messages());
  ASSERT_EQ(1, rx_window.nof_early_messages());
  ASSERT_EQ(0, rx_window.nof_late_messages());
}

TEST(ofh_rx_window_checker, late_packet_counts_one_packet)
{
  // Create window checker with timing parameters corresponding to Ta4_min=50us, Ta4_max=300us.
  rx_window_checker rx_window(true, {2, 9});

  // Create the OTA notification.
  slot_symbol_point ota_slot({1, 1, 1, 1}, 7, 14);
  rx_window.on_new_symbol({ota_slot, {}});

  slot_symbol_point message_slot = ota_slot - 28;
  rx_window.update_rx_window_statistics(message_slot);
  ASSERT_EQ(0, rx_window.nof_on_time_messages());
  ASSERT_EQ(0, rx_window.nof_early_messages());
  ASSERT_EQ(1, rx_window.nof_late_messages());
}

TEST(ofh_rx_window_checker, window_change_slot_works)
{
  // Create window checker with timing parameters corresponding to Ta4_min=50us, Ta4_max=300us.
  rx_window_checker rx_window(true, {2, 9});

  // Create the OTA notification.
  slot_symbol_point ota_slot({1, 1, 1, 0}, 1, 14);
  rx_window.on_new_symbol({ota_slot, {}});

  slot_symbol_point message_slot({1, 1, 0, 1}, 11, 14);
  rx_window.update_rx_window_statistics(message_slot);
  ASSERT_EQ(1, rx_window.nof_on_time_messages());
  ASSERT_EQ(0, rx_window.nof_early_messages());
  ASSERT_EQ(0, rx_window.nof_late_messages());
}

TEST(ofh_rx_window_checker, window_change_sfn_works)
{
  // Create window checker with timing parameters corresponding to Ta4_min=50us, Ta4_max=300us.
  rx_window_checker rx_window(true, {2, 9});

  // Create the OTA notification.
  slot_symbol_point ota_slot({1, 1, 0, 0}, 1, 14);
  rx_window.on_new_symbol({ota_slot, {}});

  slot_symbol_point message_slot({1, 0, 9, 1}, 11, 14);
  rx_window.update_rx_window_statistics(message_slot);
  ASSERT_EQ(1, rx_window.nof_on_time_messages());
  ASSERT_EQ(0, rx_window.nof_early_messages());
  ASSERT_EQ(0, rx_window.nof_late_messages());
}

TEST(ofh_rx_window_checker, window_change_sfn_byte_works)
{
  // Create window checker with timing parameters corresponding to Ta4_min=50us, Ta4_max=300us.
  rx_window_checker rx_window(true, {2, 9});

  // Create the OTA notification.
  slot_symbol_point ota_slot({1, 0, 0, 0}, 1, 14);
  rx_window.on_new_symbol({ota_slot, {}});

  slot_symbol_point message_slot({1, 255, 9, 1}, 11, 14);
  rx_window.update_rx_window_statistics(message_slot);
  ASSERT_EQ(1, rx_window.nof_on_time_messages());
  ASSERT_EQ(0, rx_window.nof_early_messages());
  ASSERT_EQ(0, rx_window.nof_late_messages());
}

TEST(ofh_rx_window_checker, window_change_sfn_byte_and_message_is_in_sfn_0)
{
  // Create window checker with timing parameters corresponding to Ta4_min=50us, Ta4_max=300us.
  rx_window_checker rx_window(true, {2, 9});

  // Create the OTA notification.
  slot_symbol_point ota_slot({1, 0, 0, 0}, 3, 14);
  rx_window.on_new_symbol({ota_slot, {}});

  slot_symbol_point message_slot({1, 0, 0, 0}, 0, 14);
  rx_window.update_rx_window_statistics(message_slot);
  ASSERT_EQ(1, rx_window.nof_on_time_messages());
  ASSERT_EQ(0, rx_window.nof_early_messages());
  ASSERT_EQ(0, rx_window.nof_late_messages());
}
