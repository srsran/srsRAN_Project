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

class ValidateCRCMessageField
  : public ValidateFAPIMessage<crc_indication_message>,
    public testing::TestWithParam<std::tuple<pdu_field_data<crc_indication_message>, test_case_data>>
{};

TEST_P(ValidateCRCMessageField, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_crc_indication,
               validate_crc_indication,
               srsgnb::fapi::message_type_id::crc_indication);
};

INSTANTIATE_TEST_SUITE_P(SFN,
                         ValidateCRCMessageField,
                         testing::Combine(testing::Values(pdu_field_data<crc_indication_message>{
                                              "sfn",
                                              [](crc_indication_message& msg, int value) { msg.sfn = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{522, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(slot,
                         ValidateCRCMessageField,
                         testing::Combine(testing::Values(pdu_field_data<crc_indication_message>{
                                              "slot",
                                              [](crc_indication_message& msg, int value) { msg.slot = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{80, true},
                                                          test_case_data{159, true},
                                                          test_case_data{160, false})));

INSTANTIATE_TEST_SUITE_P(
    RNTI,
    ValidateCRCMessageField,
    testing::Combine(testing::Values(pdu_field_data<crc_indication_message>{
                         "RNTI",
                         [](crc_indication_message& msg, int value) { msg.pdus.front().rnti = to_rnti(value); }}),
                     testing::Values(test_case_data{0, false}, test_case_data{1, true}, test_case_data{159, true})));

INSTANTIATE_TEST_SUITE_P(RAPID,
                         ValidateCRCMessageField,
                         testing::Combine(testing::Values(pdu_field_data<crc_indication_message>{
                                              "RAPID",
                                              [](crc_indication_message& msg, int value) {
                                                msg.pdus.front().rapid = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{31, true},
                                                          test_case_data{63, true},
                                                          test_case_data{64, false},
                                                          test_case_data{254, false},
                                                          test_case_data{255, true})));

INSTANTIATE_TEST_SUITE_P(HARQ,
                         ValidateCRCMessageField,
                         testing::Combine(testing::Values(pdu_field_data<crc_indication_message>{
                                              "HARQ ID",
                                              [](crc_indication_message& msg, int value) {
                                                msg.pdus.front().harq_id = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{8, true},
                                                          test_case_data{15, true},
                                                          test_case_data{16, false})));

INSTANTIATE_TEST_SUITE_P(TA,
                         ValidateCRCMessageField,
                         testing::Combine(testing::Values(pdu_field_data<crc_indication_message>{
                                              "Timing advance offset",
                                              [](crc_indication_message& msg, int value) {
                                                msg.pdus.front().timing_advance_offset = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{31, true},
                                                          test_case_data{63, true},
                                                          test_case_data{64, false},
                                                          test_case_data{65534, false},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(TA_ns,
                         ValidateCRCMessageField,
                         testing::Combine(testing::Values(pdu_field_data<crc_indication_message>{
                                              "Timing advance offset in nanoseconds",
                                              [](crc_indication_message& msg, int value) {
                                                msg.pdus.front().timing_advance_offset_ns = value;
                                              }}),
                                          testing::Values(test_case_data{static_cast<unsigned>(int16_t(-32768)), true},
                                                          test_case_data{static_cast<unsigned>(int16_t(-32767)), false},
                                                          test_case_data{static_cast<unsigned>(int16_t(-16801)), false},
                                                          test_case_data{static_cast<unsigned>(int16_t(-16800)), true},
                                                          test_case_data{0, true},
                                                          test_case_data{16800, true},
                                                          test_case_data{16801, false})));

INSTANTIATE_TEST_SUITE_P(RSSI,
                         ValidateCRCMessageField,
                         testing::Combine(testing::Values(pdu_field_data<crc_indication_message>{
                                              "RSSI",
                                              [](crc_indication_message& msg, int value) {
                                                msg.pdus.front().rssi = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{640, true},
                                                          test_case_data{1280, true},
                                                          test_case_data{1281, false},
                                                          test_case_data{65534, false},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(RSRP,
                         ValidateCRCMessageField,
                         testing::Combine(testing::Values(pdu_field_data<crc_indication_message>{
                                              "RSRP",
                                              [](crc_indication_message& msg, int value) {
                                                msg.pdus.front().rsrp = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{640, true},
                                                          test_case_data{1280, true},
                                                          test_case_data{1281, false},
                                                          test_case_data{65534, false},
                                                          test_case_data{65535, true})));

/// Valid Message should pass.
TEST(ValidateCRCIndication, ValidMessagePasses)
{
  const crc_indication_message& msg    = build_valid_crc_indication();
  const auto&                   result = validate_crc_indication(msg);

  ASSERT_TRUE(result);
}

/// Add 3 errors and check that validation fails with 3 errors.
TEST(ValidateCRCIndication, InvalidMessageFails)
{
  crc_indication_message msg = build_valid_crc_indication();

  // Force 3 errors.
  msg.slot                                  = 1025;
  msg.pdus.front().rsrp                     = 10000;
  msg.pdus.front().timing_advance_offset_ns = -30000;

  const auto& result = validate_crc_indication(msg);

  ASSERT_FALSE(result);
  // Assert 3 reports were generated.
  ASSERT_EQ(result.error().reports.size(), 3u);
}
