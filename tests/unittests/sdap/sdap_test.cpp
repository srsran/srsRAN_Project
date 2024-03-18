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

#include "sdap_test.h"

using namespace srsran;
using namespace srs_cu_up;

/// \brief Test correct creation of SDAP entity
TEST_F(sdap_test, create_new_entity)
{
  ASSERT_NE(sdap, nullptr);
}

/// \brief Test TX without any mapping
TEST_F(sdap_test, test_tx_no_mapping)
{
  const std::array<uint8_t, 4> sdu_buf = {0x00, 0x01, 0x02, 0x03};
  byte_buffer                  sdu     = byte_buffer::create(sdu_buf).value();

  sdap->handle_sdu(sdu.deep_copy().value(), qos_flow_id_t::min);

  EXPECT_TRUE(dl_sink1->pdu_queue.empty());
  EXPECT_TRUE(dl_sink2->pdu_queue.empty());
  EXPECT_TRUE(ul_sink->sdu_queue.empty());
}

/// \brief Test TX with unknown mapping
TEST_F(sdap_test, test_tx_unknown_mapping)
{
  const std::array<uint8_t, 4> sdu_buf = {0x00, 0x01, 0x02, 0x03};
  byte_buffer                  sdu     = byte_buffer::create(sdu_buf).value();

  sdap_config sdap_cfg = {};

  // add different mapping
  sdap->add_mapping(qos_flow_id_t::min, drb_id_t::drb1, sdap_cfg, *dl_sink1);

  sdap->handle_sdu(sdu.deep_copy().value(), qos_flow_id_t::max); // different QFI

  EXPECT_TRUE(dl_sink1->pdu_queue.empty());
  EXPECT_TRUE(dl_sink2->pdu_queue.empty());
}

/// \brief Test TX with known mapping
TEST_F(sdap_test, test_tx_known_mapping)
{
  const std::array<uint8_t, 4> sdu_buf = {0x00, 0x01, 0x02, 0x03};
  byte_buffer                  sdu     = byte_buffer::create(sdu_buf).value();

  sdap_config sdap_cfg = {};

  // add mapping
  sdap->add_mapping(qos_flow_id_t::min, drb_id_t::drb1, sdap_cfg, *dl_sink1);

  sdap->handle_sdu(sdu.deep_copy().value(), qos_flow_id_t::min);

  ASSERT_FALSE(dl_sink1->pdu_queue.empty());
  EXPECT_TRUE(dl_sink2->pdu_queue.empty());
  EXPECT_EQ(dl_sink1->pdu_queue.front(), sdu);
  dl_sink1->pdu_queue.pop();
  EXPECT_TRUE(dl_sink1->pdu_queue.empty());
}

/// \brief Test TX with removed mapping
TEST_F(sdap_test, test_tx_removed_mapping)
{
  const std::array<uint8_t, 4> sdu_buf1 = {0x00, 0x01, 0x02, 0x03};
  byte_buffer                  sdu1     = byte_buffer::create(sdu_buf1).value();
  const std::array<uint8_t, 4> sdu_buf2 = {0x01, 0x02, 0x03, 0x04};
  byte_buffer                  sdu2     = byte_buffer::create(sdu_buf2).value();
  const std::array<uint8_t, 4> sdu_buf3 = {0x02, 0x03, 0x04, 0x05};
  byte_buffer                  sdu3     = byte_buffer::create(sdu_buf3).value();
  const std::array<uint8_t, 4> sdu_buf4 = {0x03, 0x04, 0x05, 0x06};
  byte_buffer                  sdu4     = byte_buffer::create(sdu_buf4).value();

  sdap_config sdap_cfg = {};

  // add mapping
  sdap->add_mapping(qos_flow_id_t::min, drb_id_t::drb1, sdap_cfg, *dl_sink1);
  sdap->add_mapping(qos_flow_id_t::max, drb_id_t::drb2, sdap_cfg, *dl_sink2);

  sdap->handle_sdu(sdu1.deep_copy().value(), qos_flow_id_t::min);
  sdap->handle_sdu(sdu2.deep_copy().value(), qos_flow_id_t::max);

  ASSERT_FALSE(dl_sink1->pdu_queue.empty());
  EXPECT_EQ(dl_sink1->pdu_queue.front(), sdu1);
  dl_sink1->pdu_queue.pop();
  EXPECT_TRUE(dl_sink1->pdu_queue.empty());

  ASSERT_FALSE(dl_sink2->pdu_queue.empty());
  EXPECT_EQ(dl_sink2->pdu_queue.front(), sdu2);
  dl_sink2->pdu_queue.pop();
  EXPECT_TRUE(dl_sink2->pdu_queue.empty());

  sdap->remove_mapping(drb_id_t::drb1);

  sdap->handle_sdu(sdu3.deep_copy().value(), qos_flow_id_t::min);
  sdap->handle_sdu(sdu4.deep_copy().value(), qos_flow_id_t::max);

  EXPECT_TRUE(dl_sink1->pdu_queue.empty());

  ASSERT_FALSE(dl_sink2->pdu_queue.empty());
  EXPECT_EQ(dl_sink2->pdu_queue.front(), sdu4);
  dl_sink2->pdu_queue.pop();
  EXPECT_TRUE(dl_sink2->pdu_queue.empty());
}

#ifdef ASSERTS_ENABLED
/// \brief Test RX without any mapping
TEST_F(sdap_test, test_rx_no_mapping)
{
  (void)(::testing::GTEST_FLAG(death_test_style) = "threadsafe");
  // no prior mapping, should trigger an assertion
  ASSERT_DEATH({ sdap->get_sdap_rx_pdu_handler(drb_id_t::drb1); }, "");
}
#endif

#ifdef ASSERTS_ENABLED
/// \brief Test RX with unknown mapping
TEST_F(sdap_test, test_rx_unknown_mapping)
{
  (void)(::testing::GTEST_FLAG(death_test_style) = "threadsafe");

  sdap_config sdap_cfg = {};

  // add different mapping
  sdap->add_mapping(qos_flow_id_t::min, drb_id_t::drb1, sdap_cfg, *dl_sink1);

  // no matching mapping, should trigger an assertion
  ASSERT_DEATH({ sdap->get_sdap_rx_pdu_handler(drb_id_t::drb2); }, "");
}
#endif

/// \brief Test RX with known mapping
TEST_F(sdap_test, test_rx_known_mapping)
{
  const std::array<uint8_t, 4> pdu_buf = {0x00, 0x01, 0x02, 0x03};
  byte_buffer                  pdu     = byte_buffer::create(pdu_buf).value();

  sdap_config sdap_cfg = {};

  // add mapping
  sdap->add_mapping(qos_flow_id_t::min, drb_id_t::drb1, sdap_cfg, *dl_sink1);

  sdap->get_sdap_rx_pdu_handler(drb_id_t::drb1).handle_pdu(pdu.deep_copy().value());

  ASSERT_FALSE(ul_sink->sdu_queue.empty());
  EXPECT_EQ(ul_sink->sdu_queue.front(), pdu);
  ul_sink->sdu_queue.pop();
  EXPECT_TRUE(ul_sink->sdu_queue.empty());
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
