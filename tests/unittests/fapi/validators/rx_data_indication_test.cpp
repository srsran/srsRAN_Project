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
#include "srsran/support/test_utils.h"

using namespace srsran;
using namespace fapi;
using namespace unittest;

class validate_rx_data_indication_field
  : public validate_fapi_message<rx_data_indication_message>,
    public testing::TestWithParam<std::tuple<pdu_field_data<rx_data_indication_message>, test_case_data>>
{};

TEST_P(validate_rx_data_indication_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_rx_data_indication,
               validate_rx_data_indication,
               srsran::fapi::message_type_id::rx_data_indication);
};

INSTANTIATE_TEST_SUITE_P(sfn,
                         validate_rx_data_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<rx_data_indication_message>{
                                              "sfn",
                                              [](rx_data_indication_message& pdu, int value) { pdu.sfn = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{512, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(slot,
                         validate_rx_data_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<rx_data_indication_message>{
                                              "slot",
                                              [](rx_data_indication_message& pdu, int value) { pdu.slot = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{80, true},
                                                          test_case_data{159, true},
                                                          test_case_data{160, false})));

INSTANTIATE_TEST_SUITE_P(RNTI,
                         validate_rx_data_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<rx_data_indication_message>{
                                              "RNTI",
                                              [](rx_data_indication_message& pdu, int value) {
                                                pdu.pdus.back().rnti = to_rnti(value);
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{32767, true},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(RAPID,
                         validate_rx_data_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<rx_data_indication_message>{
                                              "RAPID",
                                              [](rx_data_indication_message& pdu, int value) {
                                                pdu.pdus.back().rapid = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{31, true},
                                                          test_case_data{63, true},
                                                          test_case_data{64, false},
                                                          test_case_data{254, false},
                                                          test_case_data{255, true})));

INSTANTIATE_TEST_SUITE_P(HarqID,
                         validate_rx_data_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<rx_data_indication_message>{
                                              "HARQ ID",
                                              [](rx_data_indication_message& pdu, int value) {
                                                pdu.pdus.back().harq_id = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{8, true},
                                                          test_case_data{15, true},
                                                          test_case_data{16, false})));

INSTANTIATE_TEST_SUITE_P(PDUTag,
                         validate_rx_data_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<rx_data_indication_message>{
                                              "PDU tag",
                                              [](rx_data_indication_message& pdu, int value) {
                                                pdu.pdus.back().pdu_tag =
                                                    static_cast<rx_data_indication_pdu::pdu_tag_type>(value);
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{99, false},
                                                          test_case_data{100, true},
                                                          test_case_data{101, false})));

/// Valid Message should pass.
TEST(validate_rx_data_indication, valid_indication_passes)
{
  auto msg = build_valid_rx_data_indication();

  const auto& result = validate_rx_data_indication(msg);

  EXPECT_TRUE(result);
}

TEST(validate_rx_data_indication, valid_indication_with_no_data_passes)
{
  auto msg = build_valid_rx_data_indication();

  msg.pdus.back().rapid      = 1U;
  msg.pdus.back().harq_id    = 1U;
  msg.pdus.back().pdu_tag    = rx_data_indication_pdu::pdu_tag_type::custom;
  msg.pdus.back().pdu_length = 0;
  msg.pdus.back().data       = nullptr;

  const auto& result = validate_rx_data_indication(msg);

  EXPECT_TRUE(result);
}

/// Add 3 errors and check that validation fails with 3 errors.
TEST(validate_rx_data_indication, invalid_indication_fails)
{
  auto msg = build_valid_rx_data_indication();

  msg.pdus.back().rapid   = 64U;
  msg.pdus.back().harq_id = 16U;
  msg.pdus.back().pdu_tag = rx_data_indication_pdu::pdu_tag_type::MAC_PDU;

  const auto& result = validate_rx_data_indication(msg);

  EXPECT_FALSE(result);
  const auto& report = result.error();
  // Check that the 3 errors are reported.
  EXPECT_EQ(report.reports.size(), 3U);
}
