/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/f1u/cu_up/f1u_bearer_impl.h"
#include "srsgnb/srslog/srslog.h"
#include <gtest/gtest.h>
#include <list>

using namespace srsgnb;
using namespace srs_cu_up;

/// Mocking class of the surrounding layers invoked by the F1-U bearer
class f1u_cu_up_test_frame : public f1u_tx_pdu_notifier, public f1u_rx_delivery_notifier, public f1u_rx_sdu_notifier
{
public:
  std::list<nru_dl_message>          tx_msg_list;
  std::list<uint32_t>                rx_delivered_sdu_list;
  std::list<byte_buffer_slice_chain> rx_sdu_list;

  // f1u_tx_pdu_notifier interface
  void on_new_pdu(nru_dl_message msg) override { tx_msg_list.push_back(std::move(msg)); }

  // f1u_rx_delivery_notifier interface
  void on_delivered_sdu(uint32_t count) override { rx_delivered_sdu_list.push_back(count); }

  // f1u_rx_sdu_notifier interface
  void on_new_sdu(byte_buffer_slice_chain sdu) override { rx_sdu_list.push_back(std::move(sdu)); }
};

class f1u_trx_test
{
public:
  byte_buffer create_sdu_byte_buffer(uint32_t sdu_size, uint8_t first_byte = 0) const
  {
    byte_buffer sdu_buf;
    for (uint32_t k = 0; k < sdu_size; ++k) {
      sdu_buf.append(first_byte + k);
    }
    return sdu_buf;
  }
};

/// Fixture class for F1-U CU-UP tests
class f1u_cu_up_test : public ::testing::Test, public f1u_trx_test
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init F1-U logger
    srslog::fetch_basic_logger("F1-U", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("F1-U", false).set_hex_dump_max_size(100);

    // create tester and testee
    logger.info("Creating F1-U bearer");
    tester          = std::make_unique<f1u_cu_up_test_frame>();
    drb_id_t drb_id = drb_id_t::drb1;
    f1u             = std::make_unique<f1u_bearer_impl>(drb_id, *tester, *tester, *tester);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger&                 logger = srslog::fetch_basic_logger("TEST", false);
  std::unique_ptr<f1u_cu_up_test_frame> tester;
  std::unique_ptr<f1u_bearer_impl>      f1u;
};

TEST_F(f1u_cu_up_test, create_new_entity)
{
  EXPECT_TRUE(tester->tx_msg_list.empty());
  EXPECT_TRUE(tester->rx_delivered_sdu_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());
}

TEST_F(f1u_cu_up_test, tx_discard)
{
  constexpr uint32_t pdcp_count = 123;

  f1u->discard_sdu(pdcp_count);
  f1u->discard_sdu(pdcp_count + 7);

  EXPECT_TRUE(tester->rx_delivered_sdu_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_TRUE(tester->tx_msg_list.front().t_pdu.empty());
  EXPECT_TRUE(tester->tx_msg_list.front().dl_user_data.dl_discard_blocks);
  ASSERT_EQ(tester->tx_msg_list.front().dl_user_data.dl_discard_nof_blocks, 1);
  EXPECT_EQ(tester->tx_msg_list.front().dl_user_data.discard_blocks[0].pdcp_sn_start, pdcp_count);
  EXPECT_EQ(tester->tx_msg_list.front().dl_user_data.discard_blocks[0].block_size, 1);

  tester->tx_msg_list.pop_front();

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_TRUE(tester->tx_msg_list.front().t_pdu.empty());
  EXPECT_TRUE(tester->tx_msg_list.front().dl_user_data.dl_discard_blocks);
  ASSERT_EQ(tester->tx_msg_list.front().dl_user_data.dl_discard_nof_blocks, 1);
  EXPECT_EQ(tester->tx_msg_list.front().dl_user_data.discard_blocks[0].pdcp_sn_start, pdcp_count + 7);
  EXPECT_EQ(tester->tx_msg_list.front().dl_user_data.discard_blocks[0].block_size, 1);

  tester->tx_msg_list.pop_front();

  EXPECT_TRUE(tester->tx_msg_list.empty());
}

TEST_F(f1u_cu_up_test, tx_pdcp_pdus)
{
  constexpr uint32_t pdu_size   = 10;
  constexpr uint32_t pdcp_count = 123;

  byte_buffer tx_pdcp_pdu1 = create_sdu_byte_buffer(pdu_size, pdcp_count);
  f1u->handle_sdu(tx_pdcp_pdu1.deep_copy(), pdcp_count);

  byte_buffer tx_pdcp_pdu2 = create_sdu_byte_buffer(pdu_size, pdcp_count + 1);
  f1u->handle_sdu(tx_pdcp_pdu2.deep_copy(), pdcp_count + 1);

  EXPECT_TRUE(tester->rx_delivered_sdu_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_EQ(tester->tx_msg_list.front().t_pdu, tx_pdcp_pdu1);
  EXPECT_EQ(tester->tx_msg_list.front().pdcp_count, pdcp_count);
  EXPECT_FALSE(tester->tx_msg_list.front().dl_user_data.dl_discard_blocks);

  tester->tx_msg_list.pop_front();

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_EQ(tester->tx_msg_list.front().t_pdu, tx_pdcp_pdu2);
  EXPECT_EQ(tester->tx_msg_list.front().pdcp_count, pdcp_count + 1);
  EXPECT_FALSE(tester->tx_msg_list.front().dl_user_data.dl_discard_blocks);

  tester->tx_msg_list.pop_front();

  EXPECT_TRUE(tester->tx_msg_list.empty());
}

TEST_F(f1u_cu_up_test, rx_pdcp_pdus)
{
  constexpr uint32_t pdu_size   = 10;
  constexpr uint32_t pdcp_count = 123;

  byte_buffer    rx_pdcp_pdu1 = create_sdu_byte_buffer(pdu_size, pdcp_count);
  nru_ul_message msg1;
  msg1.t_pdu = byte_buffer_slice_chain{rx_pdcp_pdu1.deep_copy()};
  f1u->handle_pdu(std::move(msg1));

  byte_buffer    rx_pdcp_pdu2 = create_sdu_byte_buffer(pdu_size, pdcp_count + 1);
  nru_ul_message msg2;
  msg2.t_pdu = byte_buffer_slice_chain{rx_pdcp_pdu2.deep_copy()};
  f1u->handle_pdu(std::move(msg2));

  EXPECT_TRUE(tester->tx_msg_list.empty());
  EXPECT_TRUE(tester->rx_delivered_sdu_list.empty());

  ASSERT_FALSE(tester->rx_sdu_list.empty());
  EXPECT_EQ(tester->rx_sdu_list.front(), rx_pdcp_pdu1);

  tester->rx_sdu_list.pop_front();

  ASSERT_FALSE(tester->rx_sdu_list.empty());
  EXPECT_EQ(tester->rx_sdu_list.front(), rx_pdcp_pdu2);

  tester->rx_sdu_list.pop_front();

  EXPECT_TRUE(tester->rx_sdu_list.empty());
}
