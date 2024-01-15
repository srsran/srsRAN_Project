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

#include "helpers.h"
#include "srsran/fapi/message_validators.h"

using namespace srsran;
using namespace fapi;
using namespace unittest;

class validate_error_indication_field
  : public validate_fapi_message<error_indication_message>,
    public testing::TestWithParam<std::tuple<pdu_field_data<error_indication_message>, test_case_data>>
{};

TEST_P(validate_error_indication_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_error_indication,
               validate_error_indication,
               srsran::fapi::message_type_id::error_indication);
};

INSTANTIATE_TEST_SUITE_P(SFN,
                         validate_error_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<error_indication_message>{
                                              "sfn",
                                              [](error_indication_message& msg, int value) { msg.sfn = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{522, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(slot,
                         validate_error_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<error_indication_message>{
                                              "slot",
                                              [](error_indication_message& msg, int value) { msg.slot = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{80, true},
                                                          test_case_data{159, true},
                                                          test_case_data{160, false})));

INSTANTIATE_TEST_SUITE_P(MessageID,
                         validate_error_indication_field,
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
                         validate_error_indication_field,
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
                         validate_error_indication_field,
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
                         validate_error_indication_field,
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
                         validate_error_indication_field,
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
                         validate_error_indication_field,
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

TEST(validate_error_indication, valid_message_passes)
{
  const auto& msg = build_valid_error_indication();

  const auto& result = validate_error_indication(msg);

  ASSERT_TRUE(result);
}

TEST(validate_error_indication, invalid_message_fails)
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

TEST(validate_out_of_sync_error_indication, valid_message_passes)
{
  const auto& msg = build_valid_out_of_sync_error_indication();

  const auto& result = validate_error_indication(msg);

  ASSERT_TRUE(result);
}

TEST(validate_invalid_sfn_error_indication, valid_message_passes)
{
  const auto& msg = build_valid_invalid_sfn_error_indication();

  const auto& result = validate_error_indication(msg);

  ASSERT_TRUE(result);
}

TEST(validate_msg_slot_error_indication, valid_message_passes)
{
  const auto& msg = build_valid_msg_slot_error_indication();

  const auto& result = validate_error_indication(msg);

  ASSERT_TRUE(result);
}

TEST(validate_tx_err_error_indication, valid_message_passes)
{
  const auto& msg = build_valid_tx_err_error_indication();

  const auto& result = validate_error_indication(msg);

  ASSERT_TRUE(result);
}

TEST(validate_ul_dci_err_error_indication, valid_message_passes)
{
  const auto& msg = build_valid_ul_dci_err_error_indication();

  const auto& result = validate_error_indication(msg);

  ASSERT_TRUE(result);
}

TEST(validate_out_of_sync_error_indication, invalid_message_fails)
{
  auto msg = build_valid_out_of_sync_error_indication();

  msg.error_code = error_code_id::msg_tx_err;

  const auto& result = validate_error_indication(msg);

  ASSERT_FALSE(result);

  const auto& report = result.error();
  // Check that the 2 errors are reported.
  ASSERT_EQ(report.reports.size(), 2U);
}
