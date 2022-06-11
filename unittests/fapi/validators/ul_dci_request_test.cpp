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

static std::vector<test_group<ul_dci_request_message> > vector_test = {
    {[](ul_dci_request_message& msg, int value) { msg.sfn = value; },
     "sfn",
     {{0, true}, {511, true}, {1023, true}, {1024, false}}},
    {[](ul_dci_request_message& msg, int value) { msg.slot = value; },
     "slot",
     {{0, true}, {79, true}, {159, true}, {160, false}}}};

static void test_validate_each_field_error()
{
  for (auto& group : vector_test) {
    for (const auto& test_case : group) {
      auto msg = build_valid_ul_dci_request();
      group.update_msg(msg, test_case.value);
      auto result = validate_ul_dci_request(msg);

      TESTASSERT_EQ(result.operator bool(), test_case.result);
      if (!result) {
        const auto& report = result.error();
        TESTASSERT_EQ(1U, report.reports.size());
        const auto& rep = report.reports.back();
        TESTASSERT_EQ(std::strcmp(group.property(), rep.property_name), 0);
        TESTASSERT_EQ(message_type_id::ul_dci_request, rep.message_type);
        TESTASSERT(!rep.pdu_type);
      }
    }
  }
}

static void test_ul_dci_request_ok()
{
  auto msg = build_valid_ul_dci_request();

  const auto& result = validate_ul_dci_request(msg);

  TESTASSERT(result);
}

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
  test_validate_each_field_error();
  fmt::print("UL_DCI.request validator -> Ok\n");
}
