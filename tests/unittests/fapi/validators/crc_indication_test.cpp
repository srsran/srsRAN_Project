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

#include "../message_builder_helpers.h"
#include "helpers.h"
#include "srsran/fapi/message_validators.h"

using namespace srsran;
using namespace fapi;
using namespace unittest;

class validate_crc_message_field
  : public validate_fapi_message<crc_indication_message>,
    public testing::TestWithParam<std::tuple<pdu_field_data<crc_indication_message>, test_case_data>>
{};

TEST_P(validate_crc_message_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_crc_indication,
               validate_crc_indication,
               srsran::fapi::message_type_id::crc_indication);
};

INSTANTIATE_TEST_SUITE_P(SFN,
                         validate_crc_message_field,
                         testing::Combine(testing::Values(pdu_field_data<crc_indication_message>{
                                              "sfn",
                                              [](crc_indication_message& msg, int value) { msg.sfn = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{522, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(slot,
                         validate_crc_message_field,
                         testing::Combine(testing::Values(pdu_field_data<crc_indication_message>{
                                              "slot",
                                              [](crc_indication_message& msg, int value) { msg.slot = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{80, true},
                                                          test_case_data{159, true},
                                                          test_case_data{160, false})));

INSTANTIATE_TEST_SUITE_P(
    RNTI,
    validate_crc_message_field,
    testing::Combine(testing::Values(pdu_field_data<crc_indication_message>{
                         "RNTI",
                         [](crc_indication_message& msg, int value) { msg.pdus.front().rnti = to_rnti(value); }}),
                     testing::Values(test_case_data{0, false}, test_case_data{1, true}, test_case_data{159, true})));

INSTANTIATE_TEST_SUITE_P(RAPID,
                         validate_crc_message_field,
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
                         validate_crc_message_field,
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
                         validate_crc_message_field,
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
                         validate_crc_message_field,
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
                         validate_crc_message_field,
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
                         validate_crc_message_field,
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
TEST(validate_crc_indication, valid_message_passes)
{
  const crc_indication_message& msg    = build_valid_crc_indication();
  const auto&                   result = validate_crc_indication(msg);

  ASSERT_TRUE(result);
}

/// Add 3 errors and check that validation fails with 3 errors.
TEST(validate_crc_indication, invalid_message_fails)
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
