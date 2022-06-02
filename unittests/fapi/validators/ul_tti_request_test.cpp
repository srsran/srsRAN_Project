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

static std::vector<test_group<ul_tti_request_message> > vector_test = {
    {[](ul_tti_request_message& msg, int value) { msg.sfn = value; },
     "sfn",
     {{0, true}, {511, true}, {1023, true}, {1024, false}}},
    {[](ul_tti_request_message& msg, int value) { msg.slot = value; },
     "slot",
     {{0, true}, {79, true}, {159, true}, {160, false}}},
    {[](ul_tti_request_message& msg, int value) { msg.num_groups = value; },
     "Number of PDU groups",
     {{0, true}, {1902, true}, {3823, false}}}};

static void test_validate_each_field_error()
{
  for (auto& group : vector_test) {
    for (const auto& test_case : group) {
      auto msg = build_valid_ul_tti_request();
      group.update_msg(msg, test_case.value);
      auto result = validate_ul_tti_request(msg);

      TESTASSERT_EQ(result.operator bool(), test_case.result);
      if (!result) {
        const auto& report = result.error();
        TESTASSERT_EQ(1U, report.reports.size());
        const auto& rep = report.reports.back();
        TESTASSERT_EQ(std::strcmp(group.property(), rep.property_name), 0);
        TESTASSERT_EQ(message_type_id::ul_tti_request, rep.message_type);
        TESTASSERT(!rep.pdu_type);
      }
    }
  }
}

/// Tests that a valid UL_TTI.request message validates correctly.
static void test_ul_tti_request_ok()
{
  const auto& msg = build_valid_ul_tti_request();

  const auto& result = validate_ul_tti_request(msg);

  TESTASSERT(result);
}

/// Tests that a UL_TTI.request message which contains errors in some properties fails to validate.
static void test_ul_tti_request_error()
{
  auto msg = build_valid_ul_tti_request();

  // Set some errors.
  msg.pdus[0].prach_pdu.phys_cell_id   = 2000;
  msg.pdus[1].pucch_pdu.bwp_start      = 2000;
  msg.pdus[2].pucch_pdu.bwp_size       = 2000;
  msg.pdus[3].pusch_pdu.resource_alloc = static_cast<resource_allocation_type>(8);

  const auto& result = validate_ul_tti_request(msg);

  TESTASSERT(!result);

  const auto& report = result.error();
  // Check that the 4 errors are reported.
  TESTASSERT_EQ(report.reports.size(), 4u);
}

int main()
{
  test_ul_tti_request_ok();
  test_ul_tti_request_error();
  test_validate_each_field_error();
  fmt::print("UL_TTI.request validator -> Ok\n");
}
