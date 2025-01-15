/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/fapi/message_builders.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi;

TEST(tx_data_request_builder, valid_basic_parameters_passes)
{
  tx_data_request_message msg;
  tx_data_request_builder builder(msg);

  unsigned sfn  = 100;
  unsigned slot = 20;

  builder.set_basic_parameters(sfn, slot);
  unsigned                  pdu_index = 3;
  unsigned                  cw_index  = 0;
  static_vector<uint8_t, 5> payload   = {5, 3, 4, 5};

  shared_transport_block buffer(payload);
  builder.add_pdu(pdu_index, cw_index, buffer);

  ASSERT_EQ(sfn, msg.sfn);
  ASSERT_EQ(slot, msg.slot);

  const auto& pdu = msg.pdus[0];
  ASSERT_EQ(pdu_index, pdu.pdu_index);
  ASSERT_EQ(cw_index, pdu.cw_index);
  ASSERT_EQ(payload.size(), buffer.get_buffer().size());
}
