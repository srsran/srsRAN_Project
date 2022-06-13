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
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;
using namespace fapi;
using namespace unittest;

static std::vector<test_group<tx_data_request_message> > vector_test = {
    {[](tx_data_request_message& msg, int value) { msg.sfn = value; },
     "sfn",
     {{0, true}, {511, true}, {1023, true}, {1024, false}}},
    {[](tx_data_request_message& msg, int value) { msg.slot = value; },
     "slot",
     {{0, true}, {79, true}, {159, true}, {160, false}}},
    {[](tx_data_request_message& msg, int value) { msg.pdus.back().cw_index = value; },
     "CW index",
     {{0, true}, {1, true}, {2, false}}},
    {[](tx_data_request_message& msg, int value) {
       (void)value;
       msg.pdus.back().tlv_custom.payload = nullptr;
     },
     "TLV payload custom",
     {{0, false}}}};

static void test_validate_each_field_error()
{
  for (auto& group : vector_test) {
    for (const auto& test_case : group) {
      auto    msg = build_valid_tx_data_request();
      uint8_t payload;
      msg.pdus.back().tlv_custom.payload = &payload;
      group.update_msg(msg, test_case.value);
      auto result = validate_tx_data_request(msg);

      TESTASSERT_EQ(result.operator bool(), test_case.result);
      if (!result) {
        const auto& report = result.error();
        TESTASSERT_EQ(1U, report.reports.size());
        const auto& rep = report.reports.back();
        TESTASSERT_EQ(std::strcmp(group.property(), rep.property_name), 0);
        TESTASSERT_EQ(message_type_id::tx_data_request, rep.message_type);
        TESTASSERT(!rep.pdu_type);
      }
    }
  }
}

static void test_validate_tx_data_request_ok()
{
  auto    msg = build_valid_tx_data_request();
  uint8_t payload;
  msg.pdus.back().tlv_custom.payload = &payload;

  const auto& result = validate_tx_data_request(msg);

  TESTASSERT(result);
}

static void test_validate_tx_data_request_error()
{
  auto msg = build_valid_tx_data_request();

  msg.sfn                  = 10000;
  msg.slot                 = 2000;
  msg.pdus.back().cw_index = 3;

  const auto& result = validate_tx_data_request(msg);

  TESTASSERT(!result);

  const auto& report = result.error();
  // Check that the 4 errors are reported.
  TESTASSERT_EQ(report.reports.size(), 4u);
}

int main()
{
  test_validate_tx_data_request_ok();
  test_validate_tx_data_request_error();
  test_validate_each_field_error();
  fmt::print("Tx_Data.request validator -> OK\n");
}