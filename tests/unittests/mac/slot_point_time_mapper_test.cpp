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

#include "lib/mac/mac_dl/mac_cell_time_mapper_impl.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

class slot_time_point_mapper_test : public ::testing::TestWithParam<unsigned>
{
protected:
  using slot_time_point = std::chrono::time_point<std::chrono::system_clock>;

  const std::chrono::milliseconds frame_dur{10};
  unsigned                        numerology{GetParam()}; // Get numerology value from test parameters
  mac_cell_time_mapper_impl       manager{numerology};
  slot_point                      next_point{numerology, 0, 0};
};

TEST_P(slot_time_point_mapper_test, slot_time_point_mapping_not_available)
{
  const slot_point      report_slot       = {numerology, 0, 0};
  const slot_time_point report_time_point = std::chrono::system_clock::now();

  ASSERT_FALSE(manager.get_last_mapping().has_value());
  auto mapping = manager.get_last_mapping();
  ASSERT_FALSE(mapping.has_value());
  std::optional<slot_time_point> ret_time_point = manager.get_time_point(report_slot);
  ASSERT_FALSE(ret_time_point.has_value());
  std::optional<slot_point> ret_slot = manager.get_slot_point(report_time_point);
  ASSERT_FALSE(ret_slot.has_value());
}

TEST_P(slot_time_point_mapper_test, when_called_with_unitialized_value)
{
  const slot_point      report_slot       = {numerology, 0, 0};
  const slot_time_point report_time_point = std::chrono::system_clock::now();
  manager.handle_slot_indication({report_slot, report_time_point});

  // Mapping is available.
  ASSERT_TRUE(manager.get_last_mapping().has_value());
  slot_point      slot;
  slot_time_point time_point;
  // But cannot map from an uninitialized slot/time point.
  std::optional<slot_time_point> ret_time_point = manager.get_time_point(slot);
  ASSERT_FALSE(ret_time_point.has_value());
  std::optional<slot_point> ret_slot = manager.get_slot_point(time_point);
  ASSERT_FALSE(ret_slot.has_value());
}

TEST_P(slot_time_point_mapper_test, when_called_with_different_numerology)
{
  const slot_point      report_slot       = {numerology, 0, 0};
  const slot_time_point report_time_point = std::chrono::system_clock::now();
  manager.handle_slot_indication({report_slot, report_time_point});

  ASSERT_TRUE(manager.get_last_mapping().has_value());
  slot_point                     slot       = {(numerology + 1) % 5, 1};
  std::optional<slot_time_point> time_point = manager.get_time_point(slot);
  ASSERT_FALSE(time_point.has_value());
}

TEST_P(slot_time_point_mapper_test, slot_time_point_mapping_available)
{
  const slot_point      report_slot       = {numerology, 0, 0};
  const slot_time_point report_time_point = std::chrono::system_clock::now();
  manager.handle_slot_indication({report_slot, report_time_point});

  ASSERT_TRUE(manager.get_last_mapping().has_value());

  auto mapping = manager.get_last_mapping();
  ASSERT_TRUE(mapping.has_value());
  slot_point      slot       = mapping.value().sl_tx;
  slot_time_point time_point = mapping.value().time_point;
  ASSERT_EQ(slot, report_slot);
  ASSERT_EQ(time_point, report_time_point);

  std::optional<slot_time_point> ret_time_point = manager.get_time_point(report_slot);
  ASSERT_TRUE(ret_time_point.has_value());
  ASSERT_EQ(ret_time_point.value(), report_time_point);

  std::optional<slot_point> ret_slot = manager.get_slot_point(time_point);
  ASSERT_TRUE(ret_slot.has_value());
  ASSERT_EQ(ret_slot.value(), report_slot);
}

TEST_P(slot_time_point_mapper_test, get_last_mapping)
{
  slot_point      report_slot        = {numerology, 0, 0};
  slot_time_point report_time_point  = std::chrono::system_clock::now();
  unsigned        slots_per_subframe = report_slot.nof_slots_per_subframe();

  slot_point      slot;
  slot_time_point time_point;

  ASSERT_FALSE(manager.get_last_mapping().has_value());
  // Note: only SFN is stored, slot_idx = 0.
  for (unsigned i = 0; i < 10; i++) {
    report_slot += slots_per_subframe;
    report_time_point += frame_dur;
    manager.handle_slot_indication({report_slot, report_time_point});

    auto mapping = manager.get_last_mapping();
    ASSERT_TRUE(mapping.has_value());
    slot       = mapping.value().sl_tx;
    time_point = mapping.value().time_point;
    ASSERT_EQ(slot, report_slot);
    ASSERT_EQ(time_point, report_time_point);
  }
}

TEST_P(slot_time_point_mapper_test, get_last_mapping_only_sub_frame_stored)
{
  slot_point      full_subframe_slot       = {numerology, 0, 0};
  slot_time_point full_subframe_time_point = std::chrono::system_clock::now();
  const auto      slot_dur                 = std::chrono::microseconds{1000U >> numerology};

  slot_point      slot;
  slot_time_point time_point;

  ASSERT_FALSE(manager.get_last_mapping().has_value());
  manager.handle_slot_indication({full_subframe_slot, full_subframe_time_point});
  ASSERT_TRUE(manager.get_last_mapping().has_value());

  slot_point      report_slot       = full_subframe_slot;
  slot_time_point report_time_point = full_subframe_time_point;
  // Note: only subframe is stored, slot_idx = 0.
  // Note: The following updates will not be stored.
  for (unsigned i = 0; i < full_subframe_slot.nof_slots_per_frame(); i++) {
    report_slot++;
    report_time_point += slot_dur;
    if (report_slot.subframe_slot_index() == 0) {
      full_subframe_slot       = report_slot;
      full_subframe_time_point = report_time_point;
    }

    manager.handle_slot_indication({report_slot, report_time_point});
    ASSERT_TRUE(manager.get_last_mapping().has_value());

    auto mapping = manager.get_last_mapping();
    ASSERT_TRUE(mapping.has_value());
    slot       = mapping.value().sl_tx;
    time_point = mapping.value().time_point;
    ASSERT_EQ(slot, full_subframe_slot);
    ASSERT_EQ(time_point, full_subframe_time_point);
  }
}

TEST_P(slot_time_point_mapper_test, get_slot_time_point_from_slot_point)
{
  srslog::basic_logger& logger            = srslog::fetch_basic_logger("TEST");
  const slot_point      report_slot       = {numerology, 0, 0};
  const slot_time_point report_time_point = std::chrono::system_clock::now();
  const auto            slot_dur          = std::chrono::microseconds{1000U >> numerology};

  ASSERT_FALSE(manager.get_last_mapping().has_value());
  logger.info(
      "N={} Report slot={} time_point={}", numerology, report_slot, report_time_point.time_since_epoch().count());
  manager.handle_slot_indication({report_slot, report_time_point});
  ASSERT_TRUE(manager.get_last_mapping().has_value());

  slot_point                     slot;
  std::optional<slot_time_point> time_point;
  slot_time_point                expected_time_point;
  auto time_difference = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::microseconds(0));

  for (signed i = -10; i < 10; i++) {
    slot                = report_slot + i;
    expected_time_point = report_time_point + slot_dur * i;
    time_point          = manager.get_time_point(slot);
    ASSERT_TRUE(time_point.has_value());
    time_difference = std::chrono::duration_cast<std::chrono::microseconds>(time_point.value() - report_time_point);
    logger.info("N={} Returned time_point={} for slot={} (time_diff={})",
                numerology,
                time_point.value().time_since_epoch().count(),
                slot,
                time_difference.count());
    ASSERT_EQ(time_point, expected_time_point);
  }
}

TEST_P(slot_time_point_mapper_test, get_slot_point_from_time_point)
{
  srslog::basic_logger& logger            = srslog::fetch_basic_logger("TEST");
  const slot_point      report_slot       = {numerology, 0, 0};
  const slot_time_point report_time_point = std::chrono::system_clock::now();
  const auto            slot_dur          = std::chrono::microseconds{1000U >> numerology};

  ASSERT_FALSE(manager.get_last_mapping().has_value());
  logger.info(
      "N={} Report slot={} time_point={}", numerology, report_slot, report_time_point.time_since_epoch().count());
  manager.handle_slot_indication({report_slot, report_time_point});
  ASSERT_TRUE(manager.get_last_mapping().has_value());

  std::optional<slot_point> slot;
  slot_time_point           time_point;
  slot_point                expected_slot_point;
  auto time_difference = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::microseconds(0));
  for (signed i = -10; i < 10; i++) {
    time_point          = report_time_point + slot_dur * i;
    expected_slot_point = report_slot + i;

    slot = manager.get_slot_point(time_point);
    ASSERT_TRUE(slot.has_value());
    time_difference = std::chrono::duration_cast<std::chrono::microseconds>(time_point - report_time_point);
    logger.info("N={} Returned slot={} for time_point={} (time_diff={})",
                numerology,
                slot.value(),
                time_point.time_since_epoch().count(),
                time_difference.count());
    ASSERT_EQ(slot.value(), expected_slot_point);
  }
}

class atomic_sfn_time_mapper_test : public ::testing::TestWithParam<unsigned>
{
protected:
  using time_point = std::chrono::time_point<std::chrono::system_clock>;

  const std::chrono::milliseconds frame_dur{10};
  unsigned                        numerology{GetParam()}; // Get numerology value from test parameters
  atomic_sfn_time_mapper          atomic_mapper{numerology};
  slot_point                      next_point{numerology, 0, 0};
};

TEST_P(atomic_sfn_time_mapper_test, store_and_load)
{
  const slot_point report_slot       = {numerology, 11, 0};
  const time_point report_time_point = std::chrono::system_clock::now();

  const auto no_mapping = atomic_mapper.load();
  ASSERT_EQ(no_mapping.sl_tx.valid(), false);

  atomic_mapper.store(report_slot, report_time_point);
  const auto mapping = atomic_mapper.load();
  ASSERT_EQ(mapping.sl_tx, report_slot);
  ASSERT_EQ(mapping.time_point, report_time_point);
}

TEST_P(atomic_sfn_time_mapper_test, when_truncated_timestamp_wraps_around_load_computes_correct_report_time_point)
{
  srslog::basic_logger& logger      = srslog::fetch_basic_logger("TEST");
  const slot_point      report_slot = {numerology, 11, 0};

  uint64_t period                 = 1ULL << 50;
  uint64_t before_period_start_ns = period - (1ULL << 20);     // roughly 1ms before wrap-around
  uint64_t after_period_start_ns  = period + (1ULL << 32) - 1; // roughly 4s before wrap-around

  logger.info("N={} Timestamps: Before Period Start={}, Period Start: {},  After Period Start={}",
              numerology,
              before_period_start_ns,
              period,
              after_period_start_ns);

  time_point before_wraparound_tp =
      std::chrono::system_clock::time_point(std::chrono::nanoseconds(before_period_start_ns));

  time_point after_wraparound_tp =
      std::chrono::system_clock::time_point(std::chrono::nanoseconds(after_period_start_ns));

  atomic_mapper.store(report_slot, before_wraparound_tp);
  const auto mapping = atomic_mapper.load(after_wraparound_tp);
  ASSERT_EQ(mapping.sl_tx, report_slot);
  ASSERT_EQ(mapping.time_point, before_wraparound_tp);
}

// Instantiate the test suite with different numerology values
INSTANTIATE_TEST_SUITE_P(NumerologyTests, slot_time_point_mapper_test, ::testing::Values(0, 1, 2, 3, 4));
INSTANTIATE_TEST_SUITE_P(NumerologyTests, atomic_sfn_time_mapper_test, ::testing::Values(0, 1, 2, 3, 4));
