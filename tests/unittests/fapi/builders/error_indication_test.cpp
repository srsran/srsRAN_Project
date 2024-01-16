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

#include "srsran/fapi/message_builders.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi;

TEST(error_indication_builder, valid_basic_parameters_passes)
{
  unsigned        sfn      = 100;
  unsigned        slot     = 128;
  message_type_id msg_id   = static_cast<message_type_id>(4);
  error_code_id   error_id = static_cast<error_code_id>(6);

  const auto& msg = build_error_indication(sfn, slot, msg_id, error_id);

  ASSERT_EQ(sfn, msg.sfn);
  ASSERT_EQ(slot, msg.slot);
  ASSERT_EQ(msg_id, msg.message_id);
  ASSERT_EQ(error_id, msg.error_code);
}

TEST(out_of_sync_error_indication_builder, valid_basic_parameters_passes)
{
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);
  std::uniform_int_distribution<unsigned> msg_dist(0, 7);

  unsigned        sfn           = 198;
  unsigned        slot          = 21;
  message_type_id msg_id        = static_cast<message_type_id>(3);
  unsigned        expected_sfn  = 178;
  unsigned        expected_slot = 19;

  const auto& msg = build_out_of_sync_error_indication(sfn, slot, msg_id, expected_sfn, expected_slot);

  ASSERT_EQ(sfn, msg.sfn);
  ASSERT_EQ(slot, msg.slot);
  ASSERT_EQ(msg_id, msg.message_id);
  ASSERT_EQ(error_code_id::out_of_sync, msg.error_code);
  ASSERT_EQ(expected_sfn, msg.expected_sfn);
  ASSERT_EQ(expected_slot, msg.expected_slot);
}
