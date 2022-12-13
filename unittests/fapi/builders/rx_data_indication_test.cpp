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
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace fapi;

TEST(rx_data_indication_builder, valid_basic_parameters_passes)
{
  unsigned sfn            = 600;
  unsigned slot           = 40;
  unsigned control_length = 119;

  rx_data_indication_message         msg;
  rx_data_indication_message_builder builder(msg);

  builder.set_basic_parameters(sfn, slot, control_length);

  ASSERT_EQ(sfn, msg.sfn);
  ASSERT_EQ(slot, msg.slot);
  ASSERT_EQ(control_length, msg.control_length);
}

TEST(rx_data_indication_builder, add_custom_pdu_passes)
{
  static_vector<uint8_t, 18> data = {1, 2, 3, 4, 5};

  unsigned           handle = 765;
  rnti_t             rnti   = to_rnti(29);
  optional<unsigned> rapid;
  unsigned           harq = 14;

  rx_data_indication_message         msg;
  rx_data_indication_message_builder builder(msg);

  builder.add_custom_pdu(handle, rnti, rapid, harq, {data});

  const auto& pdu = msg.pdus.back();
  ASSERT_EQ(handle, pdu.handle);
  ASSERT_EQ(rnti, pdu.rnti);
  ASSERT_EQ(rapid ? static_cast<uint8_t>(rapid.value()) : 255U, pdu.rapid);
  ASSERT_EQ(harq, pdu.harq_id);
  ASSERT_EQ(rx_data_indication_pdu::pdu_tag_type::custom, pdu.pdu_tag);
  ASSERT_EQ(data.size(), pdu.pdu_length);
  ASSERT_EQ(data.data(), pdu.data);
}
