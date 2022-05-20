/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi/message_builders.h"
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;
using namespace fapi;

static void test_validate_tx_data_request_ok()
{
  tx_data_request_message msg;
  tx_data_request_builder builder(msg);

  unsigned sfn  = 100;
  unsigned slot = 20;

  builder.set_basic_parameters(sfn, slot);
  unsigned                  pdu_index = 3;
  unsigned                  cw_index  = 0;
  static_vector<uint8_t, 5> data      = {5, 3, 4, 5};

  builder.add_pdu_custom_payload(pdu_index, cw_index, {data});

  const auto& result = validate_tx_data_request(msg);

  TESTASSERT(result);
}

static void test_validate_tx_data_request_error()
{
  tx_data_request_message msg;
  tx_data_request_builder builder(msg);

  unsigned sfn  = 10000;
  unsigned slot = 2000;

  builder.set_basic_parameters(sfn, slot);
  unsigned                  pdu_index = 3;
  unsigned                  cw_index  = 3;
  static_vector<uint8_t, 5> data      = {5, 3, 4, 5};

  builder.add_pdu_custom_payload(pdu_index, cw_index, {data});

  const auto& result = validate_tx_data_request(msg);

  TESTASSERT(!result);

  const auto& report = result.error();
  // Check that the 3 errors are reported.
  TESTASSERT_EQ(report.reports.size(), 3u);
}

int main()
{
  test_validate_tx_data_request_ok();
  test_validate_tx_data_request_error();
  fmt::print("Tx_Data.request validator -> OK\n");
}