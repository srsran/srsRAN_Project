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
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace fapi;
using namespace unittest;

static void test_validate_rx_data_indication_ok()
{
  auto msg = build_valid_rx_data_indication();

  // Append the payload here to avoid dangling pointers.
  auto& pdu = msg.pdus.back();

  static_vector<uint8_t, 18> data = {1, 2, 3, 4, 5};
  pdu.pdu_tag                     = rx_data_indication_pdu::pdu_tag_type::custom;
  pdu.pdu_length                  = data.size();
  pdu.data                        = data.data();

  const auto& result = validate_rx_data_indication(msg);

  TESTASSERT(result);
}

static void test_validate_rx_data_indication_error()
{
  auto msg = build_valid_rx_data_indication();

  msg.pdus.back().rapid   = 64U;
  msg.pdus.back().harq_id = 16U;
  msg.pdus.back().pdu_tag = rx_data_indication_pdu::pdu_tag_type::MAC_PDU;
  msg.pdus.back().data    = nullptr;

  const auto& result = validate_rx_data_indication(msg);

  TESTASSERT(!result);

  const auto& report = result.error();
  // Check that the 4 errors are reported.
  TESTASSERT_EQ(report.reports.size(), 4U);
}

int main()
{
  test_validate_rx_data_indication_ok();
  test_validate_rx_data_indication_error();
}
