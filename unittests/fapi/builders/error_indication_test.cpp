/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi/message_builders.h"
#include "srsgnb/support/srsgnb_test.h"
#include <random>

using namespace srsgnb;
using namespace fapi;

static std::mt19937 gen(0);

static void test_error_indication_ok()
{
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);
  std::uniform_int_distribution<unsigned> msg_dist(0, 7);
  std::uniform_int_distribution<unsigned> error_dist(4, 12);

  unsigned        sfn      = sfn_dist(gen);
  unsigned        slot     = slot_dist(gen);
  message_type_id msg_id   = static_cast<message_type_id>(msg_dist(gen));
  error_code_id   error_id = static_cast<error_code_id>(error_dist(gen));

  const auto& msg = build_error_indication(sfn, slot, msg_id, error_id);

  TESTASSERT_EQ(sfn, msg.sfn);
  TESTASSERT_EQ(slot, msg.slot);
  TESTASSERT_EQ(msg_id, msg.message_id);
  TESTASSERT_EQ(error_id, msg.error_code);
}

static void test_out_of_sync_error_indication_ok()
{
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);
  std::uniform_int_distribution<unsigned> msg_dist(0, 7);

  unsigned        sfn           = sfn_dist(gen);
  unsigned        slot          = slot_dist(gen);
  message_type_id msg_id        = static_cast<message_type_id>(msg_dist(gen));
  unsigned        expected_sfn  = sfn_dist(gen);
  unsigned        expected_slot = slot_dist(gen);

  const auto& msg = build_out_of_sync_error_indication(sfn, slot, msg_id, expected_sfn, expected_slot);

  TESTASSERT_EQ(sfn, msg.sfn);
  TESTASSERT_EQ(slot, msg.slot);
  TESTASSERT_EQ(msg_id, msg.message_id);
  TESTASSERT_EQ(error_code_id::out_of_sync, msg.error_code);
  TESTASSERT_EQ(expected_sfn, msg.expected_sfn);
  TESTASSERT_EQ(expected_slot, msg.expected_slot);
}

int main()
{
  test_error_indication_ok();
  test_out_of_sync_error_indication_ok();

  fmt::print("ERROR.indication builder -> OK\n");
}
