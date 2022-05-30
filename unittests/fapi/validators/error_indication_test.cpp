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

static void test_validate_error_indication_ok()
{
  const auto& msg = build_valid_error_indication();

  const auto& result = validate_error_indication(msg);

  TESTASSERT(result);
}

static void test_validate_error_indication_error()
{
  auto msg = build_valid_error_indication();

  msg.expected_sfn  = 3;
  msg.expected_slot = 2;

  const auto& result = validate_error_indication(msg);

  TESTASSERT(!result);

  const auto& report = result.error();
  // Check that the 2 errors are reported.
  TESTASSERT_EQ(report.reports.size(), 2U);
}

static void test_validate_out_of_sync_error_indication_ok()
{
  const auto& msg = build_valid_out_of_sync_error_indication();

  const auto& result = validate_error_indication(msg);

  TESTASSERT(result);
}

static void test_validate_out_of_sync_error_indication_error()
{
  auto msg = build_valid_out_of_sync_error_indication();

  msg.error_code = error_code_id::msg_tx_err;

  const auto& result = validate_error_indication(msg);

  TESTASSERT(!result);

  const auto& report = result.error();
  // Check that the 2 errors are reported.
  TESTASSERT_EQ(report.reports.size(), 2U);
}

int main()
{
  test_validate_error_indication_ok();
  test_validate_error_indication_error();

  test_validate_out_of_sync_error_indication_ok();
  test_validate_out_of_sync_error_indication_error();
}
