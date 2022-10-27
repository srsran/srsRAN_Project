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

class validate_tx_data_request_field
  : public ValidateFAPIMessage<tx_data_request_message>,
    public testing::TestWithParam<std::tuple<pdu_field_data<tx_data_request_message>, test_case_data>>
{};

TEST_P(validate_tx_data_request_field, with_value)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_tx_data_request,
               validate_tx_data_request,
               srsgnb::fapi::message_type_id::tx_data_request);
};

INSTANTIATE_TEST_SUITE_P(sfn,
                         validate_tx_data_request_field,
                         testing::Combine(testing::Values(pdu_field_data<tx_data_request_message>{
                                              "sfn",
                                              [](tx_data_request_message& msg, int value) { msg.sfn = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{512, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(slot,
                         validate_tx_data_request_field,
                         testing::Combine(testing::Values(pdu_field_data<tx_data_request_message>{
                                              "slot",
                                              [](tx_data_request_message& msg, int value) { msg.slot = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{80, true},
                                                          test_case_data{159, true},
                                                          test_case_data{160, false})));

INSTANTIATE_TEST_SUITE_P(
    cw_index,
    validate_tx_data_request_field,
    testing::Combine(testing::Values(pdu_field_data<tx_data_request_message>{
                         "CW index",
                         [](tx_data_request_message& msg, int value) { msg.pdus.back().cw_index = value; }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(tlv_payload_custom,
                         validate_tx_data_request_field,
                         testing::Combine(testing::Values(pdu_field_data<tx_data_request_message>{
                                              "TLV payload custom",
                                              [](tx_data_request_message& msg, int value) {
                                                (void)value;
                                                msg.pdus.back().tlv_custom.payload = nullptr;
                                              }}),
                                          testing::Values(test_case_data{0, false})));

TEST(validate_uci_pusch_pdu, valid_pdu_passes)
{
  auto msg = build_valid_tx_data_request();

  const auto& result = validate_tx_data_request(msg);

  ASSERT_TRUE(result);
}

TEST(validate_uci_pusch_pdu, invalid_pdu_fails)
{
  auto msg = build_valid_tx_data_request();

  msg.sfn                  = 10000;
  msg.slot                 = 2000;
  msg.pdus.back().cw_index = 3;

  const auto& result = validate_tx_data_request(msg);

  ASSERT_FALSE(result);

  const auto& report = result.error();
  // Check that the 3 errors are reported.
  ASSERT_EQ(report.reports.size(), 3u);
}
