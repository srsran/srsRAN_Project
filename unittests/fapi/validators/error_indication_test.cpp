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

using namespace srsgnb;
using namespace fapi;
using namespace unittest;

class ValidateErrorIndicationField
  : public ValidateFAPIMessage<error_indication_message>,
    public testing::TestWithParam<std::tuple<pdu_field_data<error_indication_message>, test_case_data>>
{};

TEST_P(ValidateErrorIndicationField, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_error_indication,
               validate_error_indication,
               srsgnb::fapi::message_type_id::error_indication);
};

INSTANTIATE_TEST_SUITE_P(SFN,
                         ValidateErrorIndicationField,
                         testing::Combine(testing::Values(pdu_field_data<error_indication_message>{
                                              "sfn",
                                              [](error_indication_message& msg, int value) { msg.sfn = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{522, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(slot,
                         ValidateErrorIndicationField,
                         testing::Combine(testing::Values(pdu_field_data<error_indication_message>{
                                              "slot",
                                              [](error_indication_message& msg, int value) { msg.slot = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{80, true},
                                                          test_case_data{159, true},
                                                          test_case_data{160, false})));

INSTANTIATE_TEST_SUITE_P(MessageID,
                         ValidateErrorIndicationField,
                         testing::Combine(testing::Values(pdu_field_data<error_indication_message>{
                                              "Message ID",
                                              [](error_indication_message& msg, int value) {
                                                msg.message_id = static_cast<message_type_id>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{7, true},
                                                          test_case_data{8, false},
                                                          test_case_data{0x4f, false},
                                                          test_case_data{0x7f, false},
                                                          test_case_data{0x80, true},
                                                          test_case_data{0x8a, true},
                                                          test_case_data{0x8b, false},
                                                          test_case_data{0xff, false},
                                                          test_case_data{0x179, false},
                                                          test_case_data{0x180, true},
                                                          test_case_data{0x181, true},
                                                          test_case_data{0x182, true},
                                                          test_case_data{0x183, false})));

INSTANTIATE_TEST_SUITE_P(ErrorCodeID,
                         ValidateErrorIndicationField,
                         testing::Combine(testing::Values(pdu_field_data<error_indication_message>{
                                              "Error code",
                                              [](error_indication_message& msg, int value) {
                                                msg.error_code = static_cast<error_code_id>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{8, true},
                                                          test_case_data{0xc, true},
                                                          test_case_data{0xd, false})));

INSTANTIATE_TEST_SUITE_P(ExpectedSFN,
                         ValidateErrorIndicationField,
                         testing::Combine(testing::Values(pdu_field_data<error_indication_message>{
                                              "Expected SFN",
                                              [](error_indication_message& msg, int value) {
                                                msg.error_code    = error_code_id::out_of_sync;
                                                msg.expected_sfn  = value;
                                                msg.expected_slot = 0;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{522, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(ExpectedSFNNoValue,
                         ValidateErrorIndicationField,
                         testing::Combine(testing::Values(pdu_field_data<error_indication_message>{
                                              "Expected SFN",
                                              [](error_indication_message& msg, int value) {
                                                msg.error_code    = error_code_id::msg_bch_missing;
                                                msg.expected_sfn  = value;
                                                msg.expected_slot = std::numeric_limits<uint16_t>::max();
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{32768, false},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(ExpectedSlot,
                         ValidateErrorIndicationField,
                         testing::Combine(testing::Values(pdu_field_data<error_indication_message>{
                                              "Expected slot",
                                              [](error_indication_message& msg, int value) {
                                                msg.error_code    = error_code_id::out_of_sync;
                                                msg.expected_slot = value;
                                                msg.expected_sfn  = 0;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{80, true},
                                                          test_case_data{159, true},
                                                          test_case_data{160, false})));

INSTANTIATE_TEST_SUITE_P(ExpectedSlotNoValue,
                         ValidateErrorIndicationField,
                         testing::Combine(testing::Values(pdu_field_data<error_indication_message>{
                                              "Expected slot",
                                              [](error_indication_message& msg, int value) {
                                                msg.error_code    = error_code_id::msg_bch_missing;
                                                msg.expected_slot = value;
                                                msg.expected_sfn  = std::numeric_limits<uint16_t>::max();
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{32768, false},
                                                          test_case_data{65535, true})));

TEST(ValidateErrorIndication, ValidMessagePasses)
{
  const auto& msg = build_valid_error_indication();

  const auto& result = validate_error_indication(msg);

  ASSERT_TRUE(result);
}

TEST(ValidateErrorIndication, InvalidMessageFails)
{
  auto msg = build_valid_error_indication();

  msg.expected_sfn  = 3;
  msg.expected_slot = 2;

  const auto& result = validate_error_indication(msg);

  ASSERT_FALSE(result);

  const auto& report = result.error();
  // Check that the 2 errors are reported.
  ASSERT_EQ(report.reports.size(), 2U);
}

TEST(ValidateOutOfSyncErrorIndication, ValidMessagePasses)
{
  const auto& msg = build_valid_out_of_sync_error_indication();

  const auto& result = validate_error_indication(msg);

  ASSERT_TRUE(result);
}

TEST(ValidateOutOfSyncErrorIndication, InvalidMessageFails)
{
  auto msg = build_valid_out_of_sync_error_indication();

  msg.error_code = error_code_id::msg_tx_err;

  const auto& result = validate_error_indication(msg);

  ASSERT_FALSE(result);

  const auto& report = result.error();
  // Check that the 2 errors are reported.
  ASSERT_EQ(report.reports.size(), 2U);
}
