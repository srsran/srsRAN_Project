/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "helpers.h"
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace fapi;
using namespace unittest;

static void test_validate_slot_indication_ok()
{
  const auto& msg = build_valid_slot_indication();

  const auto& result = validate_slot_indication(msg);

  TESTASSERT(result);
}

static void test_validate_slot_indication_error()
{
  auto msg = build_valid_slot_indication();

  msg.slot = 160U;
  msg.sfn  = 1024U;

  const auto& result = validate_slot_indication(msg);

  TESTASSERT(!result);

  const auto& report = result.error();
  // Check that the 2 errors are reported.
  TESTASSERT_EQ(report.reports.size(), 2U);
}

int main()
{
  test_validate_slot_indication_ok();
  test_validate_slot_indication_error();
}
