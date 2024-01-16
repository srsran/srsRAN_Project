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

class validate_ul_tti_request_field
  : public validate_fapi_message<ul_tti_request_message>,
    public testing::TestWithParam<std::tuple<pdu_field_data<ul_tti_request_message>, test_case_data>>
{};

TEST_P(validate_ul_tti_request_field, with_value)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_ul_tti_request,
               validate_ul_tti_request,
               srsran::fapi::message_type_id::ul_tti_request);
};

INSTANTIATE_TEST_SUITE_P(sfn,
                         validate_ul_tti_request_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_tti_request_message>{
                                              "sfn",
                                              [](ul_tti_request_message& msg, int value) { msg.sfn = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{512, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(slot,
                         validate_ul_tti_request_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_tti_request_message>{
                                              "slot",
                                              [](ul_tti_request_message& msg, int value) { msg.slot = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{80, true},
                                                          test_case_data{159, true},
                                                          test_case_data{160, false})));

INSTANTIATE_TEST_SUITE_P(nof_pdu_grps,
                         validate_ul_tti_request_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_tti_request_message>{
                                              "Number of PDU groups",
                                              [](ul_tti_request_message& msg, int value) { msg.num_groups = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1902, true},
                                                          test_case_data{3823, false})));

/// Tests that a valid UL_TTI.request message validates correctly.
TEST(validate_ul_tti_request, valid_request_passes)
{
  const auto& msg = build_valid_ul_tti_request();

  const auto& result = validate_ul_tti_request(msg);

  ASSERT_TRUE(result);
}

/// Tests that a UL_TTI.request message which contains errors in some properties fails to validate.
TEST(validate_ul_tti_request, invalid_request_fails)
{
  auto msg = build_valid_ul_tti_request();

  // Set some errors.
  msg.pdus[0].prach_pdu.phys_cell_id   = 2000;
  msg.pdus[1].pucch_pdu.bwp_start      = 2000;
  msg.pdus[2].pucch_pdu.bwp_size       = 2000;
  msg.pdus[3].pusch_pdu.resource_alloc = static_cast<resource_allocation_type>(8);

  const auto& result = validate_ul_tti_request(msg);

  ASSERT_FALSE(result);

  const auto& report = result.error();
  // Check that the 4 errors are reported.
  ASSERT_EQ(report.reports.size(), 4u);
}
