/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/f1u/du/f1u_bearer_impl.h"
#include "srsgnb/srslog/srslog.h"
#include <gtest/gtest.h>
#include <list>

using namespace srsran;
using namespace srs_du;

/// Mocking class of the surrounding layers invoked by the F1-U bearer
class f1u_du_test_frame : public f1u_rx_sdu_notifier, public f1u_tx_pdu_notifier
{
public:
  std::list<pdcp_tx_pdu>    rx_sdu_list;
  std::list<uint32_t>       rx_discard_sdu_list;
  std::list<nru_ul_message> tx_msg_list;

  // f1u_rx_sdu_notifier interface
  void on_new_sdu(pdcp_tx_pdu sdu) override { rx_sdu_list.push_back(std::move(sdu)); }
  void on_discard_sdu(uint32_t pdcp_sn) override { rx_discard_sdu_list.push_back(pdcp_sn); }

  // f1u_tx_pdu_notifier interface
  void on_new_pdu(nru_ul_message msg) override { tx_msg_list.push_back(std::move(msg)); }
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

/// Fixture class for F1-U DU tests
class f1u_du_test : public ::testing::Test, public f1u_trx_test
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
    tester          = std::make_unique<f1u_du_test_frame>();
    drb_id_t drb_id = drb_id_t::drb1;
    f1u             = std::make_unique<f1u_bearer_impl>(0, drb_id, *tester, *tester);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger&              logger = srslog::fetch_basic_logger("TEST", false);
  std::unique_ptr<f1u_du_test_frame> tester;
  std::unique_ptr<f1u_bearer_impl>   f1u;
};

TEST_F(f1u_du_test, create_new_entity)
{
  EXPECT_TRUE(tester->rx_sdu_list.empty());
  EXPECT_TRUE(tester->rx_discard_sdu_list.empty());
  EXPECT_TRUE(tester->tx_msg_list.empty());
}

TEST_F(f1u_du_test, rx_discard)
{
  constexpr uint32_t pdcp_sn = 123;

  nru_dl_message msg1              = {};
  msg1.dl_user_data.discard_blocks = nru_pdcp_sn_discard_blocks{};
  nru_pdcp_sn_discard_block block1 = {};
  block1.pdcp_sn_start             = pdcp_sn;
  block1.block_size                = 1;
  msg1.dl_user_data.discard_blocks.value().push_back(std::move(block1));
  f1u->handle_pdu(std::move(msg1));

  nru_dl_message msg2              = {};
  msg2.dl_user_data.discard_blocks = nru_pdcp_sn_discard_blocks{};
  nru_pdcp_sn_discard_block block2 = {};
  block2.pdcp_sn_start             = pdcp_sn + 9;
  block2.block_size                = 1;
  msg2.dl_user_data.discard_blocks.value().push_back(std::move(block2));
  f1u->handle_pdu(std::move(msg2));

  EXPECT_TRUE(tester->rx_sdu_list.empty());
  EXPECT_TRUE(tester->tx_msg_list.empty());

  ASSERT_FALSE(tester->rx_discard_sdu_list.empty());
  ASSERT_EQ(tester->rx_discard_sdu_list.front(), pdcp_sn);

  tester->rx_discard_sdu_list.pop_front();

  ASSERT_FALSE(tester->rx_discard_sdu_list.empty());
  ASSERT_EQ(tester->rx_discard_sdu_list.front(), pdcp_sn + 9);

  tester->rx_discard_sdu_list.pop_front();

  EXPECT_TRUE(tester->rx_discard_sdu_list.empty());
}

TEST_F(f1u_du_test, rx_pdcp_pdus)
{
  constexpr uint32_t pdu_size = 10;
  constexpr uint32_t pdcp_sn  = 123;

  byte_buffer    rx_pdcp_pdu1 = create_sdu_byte_buffer(pdu_size, pdcp_sn);
  nru_dl_message msg1         = {};
  msg1.t_pdu                  = rx_pdcp_pdu1.deep_copy();
  msg1.pdcp_sn                = pdcp_sn;
  f1u->handle_pdu(std::move(msg1));

  byte_buffer    rx_pdcp_pdu2 = create_sdu_byte_buffer(pdu_size, pdcp_sn + 1);
  nru_dl_message msg2         = {};
  msg2.t_pdu                  = rx_pdcp_pdu2.deep_copy();
  msg2.pdcp_sn                = pdcp_sn + 1;
  f1u->handle_pdu(std::move(msg2));

  EXPECT_TRUE(tester->rx_discard_sdu_list.empty());
  EXPECT_TRUE(tester->tx_msg_list.empty());

  ASSERT_FALSE(tester->rx_sdu_list.empty());
  EXPECT_EQ(tester->rx_sdu_list.front().buf, rx_pdcp_pdu1);
  EXPECT_EQ(tester->rx_sdu_list.front().pdcp_sn, pdcp_sn);

  tester->rx_sdu_list.pop_front();

  ASSERT_FALSE(tester->rx_sdu_list.empty());
  EXPECT_EQ(tester->rx_sdu_list.front().buf, rx_pdcp_pdu2);
  EXPECT_EQ(tester->rx_sdu_list.front().pdcp_sn, pdcp_sn + 1);

  tester->rx_sdu_list.pop_front();

  EXPECT_TRUE(tester->rx_sdu_list.empty());
}

TEST_F(f1u_du_test, tx_pdcp_pdus)
{
  constexpr uint32_t pdu_size = 10;
  constexpr uint32_t pdcp_sn  = 123;

  byte_buffer tx_pdcp_pdu1 = create_sdu_byte_buffer(pdu_size, pdcp_sn);
  f1u->handle_sdu(byte_buffer_slice_chain{tx_pdcp_pdu1.deep_copy()});

  byte_buffer tx_pdcp_pdu2 = create_sdu_byte_buffer(pdu_size, pdcp_sn + 1);
  f1u->handle_sdu(byte_buffer_slice_chain{tx_pdcp_pdu2.deep_copy()});

  EXPECT_TRUE(tester->rx_discard_sdu_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_EQ(tester->tx_msg_list.front().t_pdu, tx_pdcp_pdu1);
  EXPECT_FALSE(tester->tx_msg_list.front().data_delivery_status.has_value());
  EXPECT_FALSE(tester->tx_msg_list.front().assistance_information.has_value());

  tester->tx_msg_list.pop_front();

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_EQ(tester->tx_msg_list.front().t_pdu, tx_pdcp_pdu2);
  EXPECT_FALSE(tester->tx_msg_list.front().data_delivery_status.has_value());
  EXPECT_FALSE(tester->tx_msg_list.front().assistance_information.has_value());

  tester->tx_msg_list.pop_front();

  EXPECT_TRUE(tester->tx_msg_list.empty());
}

TEST_F(f1u_du_test, tx_transmit_notification)
{
  constexpr uint32_t highest_pdcp_sn = 123;

  f1u->handle_transmit_notification(highest_pdcp_sn);
  f1u->handle_transmit_notification(highest_pdcp_sn + 1);

  EXPECT_TRUE(tester->rx_discard_sdu_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_TRUE(tester->tx_msg_list.front().t_pdu.empty());
  EXPECT_FALSE(tester->tx_msg_list.front().assistance_information.has_value());
  ASSERT_TRUE(tester->tx_msg_list.front().data_delivery_status.has_value());
  {
    nru_dl_data_delivery_status& status = tester->tx_msg_list.front().data_delivery_status.value();
    EXPECT_FALSE(status.final_frame_ind);
    EXPECT_FALSE(status.lost_nru_sn_ranges.has_value());
    EXPECT_FALSE(status.highest_delivered_pdcp_sn.has_value());
    EXPECT_FALSE(status.cause_value.has_value());
    EXPECT_FALSE(status.highest_delivered_retransmitted_pdcp_sn.has_value());
    EXPECT_FALSE(status.highest_retransmitted_pdcp_sn.has_value());
    ASSERT_TRUE(status.highest_transmitted_pdcp_sn.has_value());
    EXPECT_EQ(status.highest_transmitted_pdcp_sn.value(), highest_pdcp_sn);
  }
  tester->tx_msg_list.pop_front();

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_TRUE(tester->tx_msg_list.front().t_pdu.empty());
  EXPECT_FALSE(tester->tx_msg_list.front().assistance_information.has_value());
  ASSERT_TRUE(tester->tx_msg_list.front().data_delivery_status.has_value());
  {
    nru_dl_data_delivery_status& status = tester->tx_msg_list.front().data_delivery_status.value();
    EXPECT_FALSE(status.final_frame_ind);
    EXPECT_FALSE(status.lost_nru_sn_ranges.has_value());
    EXPECT_FALSE(status.highest_delivered_pdcp_sn.has_value());
    EXPECT_FALSE(status.cause_value.has_value());
    EXPECT_FALSE(status.highest_delivered_retransmitted_pdcp_sn.has_value());
    EXPECT_FALSE(status.highest_retransmitted_pdcp_sn.has_value());
    ASSERT_TRUE(status.highest_transmitted_pdcp_sn.has_value());
    EXPECT_EQ(status.highest_transmitted_pdcp_sn.value(), highest_pdcp_sn + 1);
  }
  tester->tx_msg_list.pop_front();

  EXPECT_TRUE(tester->tx_msg_list.empty());
}

TEST_F(f1u_du_test, tx_delivery_notification)
{
  constexpr uint32_t highest_pdcp_sn = 123;

  f1u->handle_delivery_notification(highest_pdcp_sn);
  f1u->handle_delivery_notification(highest_pdcp_sn + 1);

  EXPECT_TRUE(tester->rx_discard_sdu_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_TRUE(tester->tx_msg_list.front().t_pdu.empty());
  EXPECT_FALSE(tester->tx_msg_list.front().assistance_information.has_value());
  ASSERT_TRUE(tester->tx_msg_list.front().data_delivery_status.has_value());
  {
    nru_dl_data_delivery_status& status = tester->tx_msg_list.front().data_delivery_status.value();
    EXPECT_FALSE(status.final_frame_ind);
    EXPECT_FALSE(status.lost_nru_sn_ranges.has_value());
    EXPECT_FALSE(status.highest_transmitted_pdcp_sn.has_value());
    EXPECT_FALSE(status.cause_value.has_value());
    EXPECT_FALSE(status.highest_delivered_retransmitted_pdcp_sn.has_value());
    EXPECT_FALSE(status.highest_retransmitted_pdcp_sn.has_value());
    ASSERT_TRUE(status.highest_delivered_pdcp_sn.has_value());
    EXPECT_EQ(status.highest_delivered_pdcp_sn.value(), highest_pdcp_sn);
  }
  tester->tx_msg_list.pop_front();

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_TRUE(tester->tx_msg_list.front().t_pdu.empty());
  EXPECT_FALSE(tester->tx_msg_list.front().assistance_information.has_value());
  ASSERT_TRUE(tester->tx_msg_list.front().data_delivery_status.has_value());
  {
    nru_dl_data_delivery_status& status = tester->tx_msg_list.front().data_delivery_status.value();
    EXPECT_FALSE(status.final_frame_ind);
    EXPECT_FALSE(status.lost_nru_sn_ranges.has_value());
    EXPECT_FALSE(status.highest_transmitted_pdcp_sn.has_value());
    EXPECT_FALSE(status.cause_value.has_value());
    EXPECT_FALSE(status.highest_delivered_retransmitted_pdcp_sn.has_value());
    EXPECT_FALSE(status.highest_retransmitted_pdcp_sn.has_value());
    ASSERT_TRUE(status.highest_delivered_pdcp_sn.has_value());
    EXPECT_EQ(status.highest_delivered_pdcp_sn.value(), highest_pdcp_sn + 1);
  }
  tester->tx_msg_list.pop_front();

  EXPECT_TRUE(tester->tx_msg_list.empty());
}
