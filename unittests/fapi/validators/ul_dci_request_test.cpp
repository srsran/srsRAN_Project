/*
 *
 * \section COPYRIGHT
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

/// Tests that a valid UL_DCI.request message validates correctly.
static void test_ul_dci_request_ok()
{
  auto msg = build_valid_ul_dci_request();

  const auto& result = validate_ul_dci_request(msg);

  TESTASSERT(result);
}

/// Tests that a UL_DCI.request message which contains errors in some properties fails to validate.
static void test_ul_dci_request_error()
{
  auto msg = build_valid_ul_dci_request();

  // Set some errors.
  msg.sfn                          = 2000U;
  msg.slot                         = 1000U;
  msg.pdus[0].pdu.coreset_bwp_size = 2000U;

  const auto& result = validate_ul_dci_request(msg);

  TESTASSERT(!result);

  const auto& report = result.error();
  // Check that the 3 errors are reported.
  TESTASSERT_EQ(report.reports.size(), 3U);
}

int main()
{
  test_ul_dci_request_ok();
  test_ul_dci_request_error();
}
