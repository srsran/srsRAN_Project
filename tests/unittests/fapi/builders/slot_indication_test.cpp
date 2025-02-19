/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/fapi/message_builders.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi;

TEST(slot_indication_builder, valid_basic_parameters_passes)
{
  unsigned                                           sfn        = 419;
  unsigned                                           slot       = 12;
  std::chrono::time_point<std::chrono::system_clock> time_point = std::chrono::system_clock::now();

  const auto& msg = build_slot_indication_message(sfn, slot, time_point);

  ASSERT_EQ(sfn, msg.sfn);
  ASSERT_EQ(slot, msg.slot);
  ASSERT_EQ(time_point, msg.time_point);
}
