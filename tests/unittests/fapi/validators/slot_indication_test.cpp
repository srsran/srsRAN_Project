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

class validate_slot_indication_field
  : public validate_fapi_message<slot_indication_message>,
    public testing::TestWithParam<std::tuple<pdu_field_data<slot_indication_message>, test_case_data>>
{};

TEST_P(validate_slot_indication_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_slot_indication,
               validate_slot_indication,
               srsran::fapi::message_type_id::slot_indication);
};

INSTANTIATE_TEST_SUITE_P(SFN,
                         validate_slot_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<slot_indication_message>{
                                              "sfn",
                                              [](slot_indication_message& msg, int value) { msg.sfn = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{522, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(slot,
                         validate_slot_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<slot_indication_message>{
                                              "slot",
                                              [](slot_indication_message& msg, int value) { msg.slot = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{80, true},
                                                          test_case_data{159, true},
                                                          test_case_data{160, false})));

TEST(validate_slot_indication, valid_message_passes)
{
  const auto& msg = build_valid_slot_indication();

  const auto& result = validate_slot_indication(msg);

  ASSERT_TRUE(result);
}

TEST(validate_slot_indication, invalid_message_fails)
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
