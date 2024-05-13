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

class validate_srs_indication_field
  : public validate_fapi_message<srs_indication_message>,
    public testing::TestWithParam<std::tuple<pdu_field_data<srs_indication_message>, test_case_data>>
{};

TEST_P(validate_srs_indication_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_srs_indication,
               validate_srs_indication,
               srsran::fapi::message_type_id::srs_indication);
};

INSTANTIATE_TEST_SUITE_P(sfn,
                         validate_srs_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<srs_indication_message>{
                                              "sfn",
                                              [](srs_indication_message& pdu, int value) { pdu.sfn = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{512, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(slot,
                         validate_srs_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<srs_indication_message>{
                                              "slot",
                                              [](srs_indication_message& pdu, int value) { pdu.slot = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{80, true},
                                                          test_case_data{159, true},
                                                          test_case_data{160, false})));

INSTANTIATE_TEST_SUITE_P(RNTI,
                         validate_srs_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<srs_indication_message>{
                                              "RNTI",
                                              [](srs_indication_message& pdu, int value) {
                                                pdu.pdus.back().rnti = to_rnti(value);
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{32767, true},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(
    ta,
    validate_srs_indication_field,
    testing::Combine(testing::Values(pdu_field_data<srs_indication_message>{"Timing advance offset",
                                                                            [](srs_indication_message& msg, int value) {
                                                                              msg.pdus.back().timing_advance_offset =
                                                                                  value;
                                                                            }}),
                     testing::Values(test_case_data{0, true},
                                     test_case_data{32, true},
                                     test_case_data{63, true},
                                     test_case_data{64, false},
                                     test_case_data{std::numeric_limits<uint16_t>::max() - 1, false},
                                     test_case_data{std::numeric_limits<uint16_t>::max(), true})));
INSTANTIATE_TEST_SUITE_P(ta_ns,
                         validate_srs_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<srs_indication_message>{
                                              "Timing advance offset in nanoseconds",
                                              [](srs_indication_message& msg, int value) {
                                                msg.pdus.back().timing_advance_offset_ns = value;
                                              }}),
                                          testing::Values(test_case_data{static_cast<unsigned>(int16_t(-10000)), true},
                                                          test_case_data{static_cast<unsigned>(int16_t(-16800)), true},
                                                          test_case_data{10000, true},
                                                          test_case_data{16801, false},
                                                          test_case_data{static_cast<unsigned>(int16_t(-16801)), false},
                                                          test_case_data{static_cast<unsigned>(int16_t(-32767)), false},
                                                          test_case_data{std::numeric_limits<uint16_t>::max(), true})));

/// Valid Message should pass.
TEST(validate_srs_indication, valid_indication_passes)
{
  auto msg = build_valid_srs_indication();

  const auto& result = validate_srs_indication(msg);

  EXPECT_TRUE(result);
}

/// Add 3 errors and check that validation fails with 3 errors.
TEST(validate_srs_indication, invalid_indication_fails)
{
  auto msg = build_valid_srs_indication();

  msg.pdus.back().timing_advance_offset    = 64U;
  msg.pdus.back().timing_advance_offset_ns = 17000U;
  msg.pdus.back().report_type              = 2U;

  const auto& result = validate_srs_indication(msg);

  EXPECT_FALSE(result);
  const auto& report = result.error();
  // Check that the 3 errors are reported.
  EXPECT_EQ(report.reports.size(), 3U);
}
