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

class validate_rach_indication_field
  : public validate_fapi_message<rach_indication_message>,
    public testing::TestWithParam<std::tuple<pdu_field_data<rach_indication_message>, test_case_data>>
{};

TEST_P(validate_rach_indication_field, with_value)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_rach_indication,
               validate_rach_indication,
               srsran::fapi::message_type_id::rach_indication);
};

INSTANTIATE_TEST_SUITE_P(sfn,
                         validate_rach_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<rach_indication_message>{
                                              "sfn",
                                              [](rach_indication_message& pdu, int value) { pdu.sfn = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{512, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(slot,
                         validate_rach_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<rach_indication_message>{
                                              "slot",
                                              [](rach_indication_message& pdu, int value) { pdu.slot = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{80, true},
                                                          test_case_data{159, true},
                                                          test_case_data{160, false})));

INSTANTIATE_TEST_SUITE_P(symbol_index,
                         validate_rach_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<rach_indication_message>{
                                              "Symbol index",
                                              [](rach_indication_message& msg, int value) {
                                                msg.pdus.back().symbol_index = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{6, true},
                                                          test_case_data{13, true},
                                                          test_case_data{14, false})));

INSTANTIATE_TEST_SUITE_P(slot_index,
                         validate_rach_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<rach_indication_message>{
                                              "Slot index",
                                              [](rach_indication_message& msg, int value) {
                                                msg.pdus.back().slot_index = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{40, true},
                                                          test_case_data{79, true},
                                                          test_case_data{80, false})));

INSTANTIATE_TEST_SUITE_P(ra_index,
                         validate_rach_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<rach_indication_message>{
                                              "Index of the received PRACH frequency domain occasion",
                                              [](rach_indication_message& msg, int value) {
                                                msg.pdus.back().ra_index = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{4, true},
                                                          test_case_data{7, true},
                                                          test_case_data{8, false})));

INSTANTIATE_TEST_SUITE_P(
    rssi,
    validate_rach_indication_field,
    testing::Combine(testing::Values(pdu_field_data<rach_indication_message>{
                         "AVG RSSI",
                         [](rach_indication_message& msg, int value) { msg.pdus.back().avg_rssi = value; }}),
                     testing::Values(test_case_data{0, true},
                                     test_case_data{85000, true},
                                     test_case_data{170000, true},
                                     test_case_data{170001, false},
                                     test_case_data{std::numeric_limits<uint32_t>::max() - 1, false},
                                     test_case_data{std::numeric_limits<uint32_t>::max(), true})));

INSTANTIATE_TEST_SUITE_P(
    rsrp,
    validate_rach_indication_field,
    testing::Combine(testing::Values(pdu_field_data<rach_indication_message>{
                         "RSRP",
                         [](rach_indication_message& msg, int value) { msg.pdus.back().rsrp = value; }}),
                     testing::Values(test_case_data{0, true},
                                     test_case_data{640, true},
                                     test_case_data{1280, true},
                                     test_case_data{1281, false},
                                     test_case_data{std::numeric_limits<uint16_t>::max() - 1, false},
                                     test_case_data{std::numeric_limits<uint16_t>::max(), true})));

INSTANTIATE_TEST_SUITE_P(preamble_index,
                         validate_rach_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<rach_indication_message>{
                                              "Preamble index",
                                              [](rach_indication_message& msg, int value) {
                                                msg.pdus.back().preambles.back().preamble_index = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{32, true},
                                                          test_case_data{63, true},
                                                          test_case_data{64, false})));

INSTANTIATE_TEST_SUITE_P(ta,
                         validate_rach_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<rach_indication_message>{
                                              "Timing advance offset",
                                              [](rach_indication_message& msg, int value) {
                                                msg.pdus.back().preambles.back().timing_advance_offset = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1923, true},
                                                          test_case_data{3846, true},
                                                          test_case_data{3847, false},
                                                          test_case_data{std::numeric_limits<uint16_t>::max() - 1,
                                                                         false},
                                                          test_case_data{std::numeric_limits<uint16_t>::max(), true})));
INSTANTIATE_TEST_SUITE_P(ta_ns,
                         validate_rach_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<rach_indication_message>{
                                              "Timing advance offset in nanoseconds",
                                              [](rach_indication_message& msg, int value) {
                                                msg.pdus.back().preambles.back().timing_advance_offset_ns = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1002000, true},
                                                          test_case_data{2005000, true},
                                                          test_case_data{2005001, false},
                                                          test_case_data{std::numeric_limits<uint32_t>::max() - 1,
                                                                         false},
                                                          test_case_data{std::numeric_limits<uint32_t>::max(), true})));

INSTANTIATE_TEST_SUITE_P(preamble_pwr,
                         validate_rach_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<rach_indication_message>{
                                              "Preamble power",
                                              [](rach_indication_message& msg, int value) {
                                                msg.pdus.back().preambles.back().preamble_pwr = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{85000, true},
                                                          test_case_data{170000, true},
                                                          test_case_data{170001, false},
                                                          test_case_data{std::numeric_limits<uint32_t>::max() - 1,
                                                                         false},
                                                          test_case_data{std::numeric_limits<uint32_t>::max(), true})));

/// Valid Message should pass.
TEST(validate_rach_indication, valid_indication_passes)
{
  rach_indication_message msg = build_valid_rach_indication();

  const auto& result = validate_rach_indication(msg);

  ASSERT_TRUE(result);
}

TEST(validate_rach_indication, invalid_indication_fails)
{
  rach_indication_message msg = build_valid_rach_indication();

  msg.pdus.back().symbol_index = 14;
  msg.pdus.back().slot_index   = 80;
  msg.pdus.back().ra_index     = 9;

  const auto& result = validate_rach_indication(msg);

  ASSERT_FALSE(result);
  const auto& report = result.error();
  // Check that the 3 errors are reported.
  ASSERT_EQ(report.reports.size(), 3U);
}
