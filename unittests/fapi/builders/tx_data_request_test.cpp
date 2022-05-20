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
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;
using namespace fapi;

static void test_tx_data_request()
{
  tx_data_request_message msg;
  tx_data_request_builder builder(msg);

  unsigned sfn  = 100;
  unsigned slot = 20;

  builder.set_basic_parameters(sfn, slot);
  unsigned                  pdu_index = 3;
  unsigned                  cw_index  = 0;
  static_vector<uint8_t, 5> payload   = {5, 3, 4, 5};

  builder.add_pdu_custom_payload(pdu_index, cw_index, {payload});

  TESTASSERT_EQ(sfn, msg.sfn);
  TESTASSERT_EQ(slot, msg.slot);

  const auto& pdu = msg.pdus[0];
  TESTASSERT_EQ(pdu_index, pdu.pdu_index);
  TESTASSERT_EQ(cw_index, pdu.cw_index);

  const auto& tlv = pdu.tlv_custom;
  TESTASSERT_EQ(payload.size(), tlv.length);
  TESTASSERT_EQ(payload.data(), tlv.payload);
}

int main()
{
  test_tx_data_request();
  fmt::print("Tx_Data.request builder -> OK\n");
}