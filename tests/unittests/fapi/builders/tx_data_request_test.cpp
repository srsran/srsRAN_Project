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

  builder.add_pdu_custom_payload(pdu_index, cw_index, {payload});

  ASSERT_EQ(sfn, msg.sfn);
  ASSERT_EQ(slot, msg.slot);

  const auto& pdu = msg.pdus[0];
  ASSERT_EQ(pdu_index, pdu.pdu_index);
  ASSERT_EQ(cw_index, pdu.cw_index);

  const auto& tlv = pdu.tlv_custom;
  ASSERT_EQ(payload.size(), tlv.length.value());
  ASSERT_EQ(payload.data(), tlv.payload);
}
