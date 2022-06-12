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

using namespace unittest;

static const std::vector<test_group<slot_indication_message> > vector_test = {
    {[](slot_indication_message& msg, int value) { msg.sfn = value; },
     "sfn",
     {{0, true}, {511, true}, {1023, true}, {1024, false}}},
    {[](slot_indication_message& msg, int value) { msg.slot = value; },
     "slot",
     {{0, true}, {79, true}, {159, true}, {160, false}}}};

static void test_validate_each_field_error()
{
  for (const auto& group : vector_test) {
    for (const auto& test_case : group) {
      auto msg = build_valid_slot_indication();
      group.update_msg(msg, test_case.value);
      auto result = validate_slot_indication(msg);

      TESTASSERT_EQ(result.operator bool(), test_case.result);
      if (!result) {
        const auto& report = result.error();
        TESTASSERT_EQ(1U, report.reports.size());
        const auto& rep = report.reports.back();
        TESTASSERT_EQ(std::strcmp(group.property(), rep.property_name), 0);
        TESTASSERT_EQ(message_type_id::slot_indication, rep.message_type);
        TESTASSERT(!rep.pdu_type);
      }
    }
  }
}

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
  test_validate_each_field_error();
  fmt::print("SLOT.indication validator -> Ok\n");
}
