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

class ValidateSlotIndicationField
  : public ValidateFAPIMessage<slot_indication_message>,
    public testing::TestWithParam<std::tuple<pdu_field_data<slot_indication_message>, test_case_data>>
{};

TEST_P(ValidateSlotIndicationField, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_slot_indication,
               validate_slot_indication,
               srsgnb::fapi::message_type_id::slot_indication);
};

INSTANTIATE_TEST_SUITE_P(SFN,
                         ValidateSlotIndicationField,
                         testing::Combine(testing::Values(pdu_field_data<slot_indication_message>{
                                              "sfn",
                                              [](slot_indication_message& msg, int value) { msg.sfn = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{522, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(slot,
                         ValidateSlotIndicationField,
                         testing::Combine(testing::Values(pdu_field_data<slot_indication_message>{
                                              "slot",
                                              [](slot_indication_message& msg, int value) { msg.slot = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{80, true},
                                                          test_case_data{159, true},
                                                          test_case_data{160, false})));

TEST(ValidateSlotIndication, ValidMessagePasses)
{
  const auto& msg = build_valid_slot_indication();

  const auto& result = validate_slot_indication(msg);

  ASSERT_TRUE(result);
}

TEST(ValidateSlotIndication, InvalidMessageFails)
{
  auto msg = build_valid_slot_indication();

  msg.slot = 160U;
  msg.sfn  = 1024U;

  const auto& result = validate_slot_indication(msg);

  ASSERT_FALSE(result);

  const auto& report = result.error();
  // Check that the 2 errors are reported.
  ASSERT_EQ(report.reports.size(), 2U);
}
