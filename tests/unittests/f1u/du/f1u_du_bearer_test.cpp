/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "lib/f1u/du/f1u_bearer_impl.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <list>

using namespace srsran;
using namespace srs_du;

namespace {

/// Mocking class of the surrounding layers invoked by the F1-U bearer
class f1u_du_test_frame : public f1u_rx_sdu_notifier, public f1u_tx_pdu_notifier
{
public:
  std::list<std::pair<byte_buffer, bool>> rx_sdu_list; // stores <PDU, is_retx>
  std::list<uint32_t>                     rx_discard_sdu_list;
  std::list<nru_ul_message>               tx_msg_list;

  // f1u_rx_sdu_notifier interface
  void on_new_sdu(byte_buffer sdu, bool is_retx) override { rx_sdu_list.push_back({std::move(sdu), is_retx}); }
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
      if (not sdu_buf.append(first_byte + k)) {
        sdu_buf.clear();
        break;
      }
    }
    return sdu_buf;
  }
};

/// Fixture class for F1-U DU tests
class f1u_du_test_base : public f1u_trx_test
{
protected:
  void tick()
  {
    timers.tick();
    ue_worker.run_pending_tasks();
  }

  void tick_all(uint32_t ticks)
  {
    for (unsigned i = 0; i < ticks; ++i) {
      tick();
    }
  }

  void init_logger()
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init F1-U logger
    srslog::fetch_basic_logger("DU-F1-U", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("DU-F1-U", false).set_hex_dump_max_size(100);
  }

  void init_f1u(f1u_config config)
  {
    // create tester and testee
    logger.info("Creating F1-U bearer");
    tester          = std::make_unique<f1u_du_test_frame>();
    drb_id_t drb_id = drb_id_t::drb1;
    f1u             = std::make_unique<f1u_bearer_impl>(
        0,
        drb_id,
        up_transport_layer_info{transport_layer_address::create_from_string("127.0.0.1"),
                                gtpu_teid_t{dl_teid_next.value()++}},
        config,
        *tester,
        *tester,
        timer_factory{timers, ue_worker},
        ue_worker);
  }

  srslog::basic_logger&              logger = srslog::fetch_basic_logger("TEST", false);
  timer_manager                      timers;
  manual_task_worker                 ue_worker{128};
  std::unique_ptr<f1u_du_test_frame> tester;
  std::unique_ptr<f1u_bearer_impl>   f1u;

  gtpu_teid_t    dl_teid_next{1234};
  const uint32_t f1u_ul_notif_time_ms             = 10;
  const uint32_t f1u_intial_rlc_queue_bytes_limit = 8000;
};

/// Fixture class for F1-U DU tests
class f1u_du_test : public ::testing::Test, public f1u_du_test_base
{
protected:
  void SetUp() override
  {
    // init test's logger
    init_logger();
    // create tester and testee
    f1u_config config            = {};
    config.t_notify              = f1u_ul_notif_time_ms;
    config.rlc_queue_bytes_limit = f1u_intial_rlc_queue_bytes_limit;
    config.warn_on_drop          = true;
    init_f1u(config);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
};

/// Fixture class for F1-U DU tests
class f1u_du_buffer_test : public ::testing::Test, public f1u_du_test_base
{
protected:
  void SetUp() override
  {
    // init test's logger
    init_logger();
    // create tester and testee
    f1u_config config           = {};
    config.t_notify             = f1u_ul_notif_time_ms;
    config.warn_on_drop         = true;
    config.buffer_ul_on_startup = true;
    config.ul_buffer_size       = 2;
    config.ul_buffer_timeout    = std::chrono::milliseconds(5);
    init_f1u(config);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
};

} // namespace

TEST_F(f1u_du_test, create_new_entity)
{
  EXPECT_TRUE(tester->rx_sdu_list.empty());
  EXPECT_TRUE(tester->rx_discard_sdu_list.empty());
  EXPECT_TRUE(tester->tx_msg_list.empty());
}

TEST_F(f1u_du_test, rx_discard)
{
  constexpr uint32_t pdcp_sn = 123;

  nru_dl_message msg1               = {};
  msg1.dl_user_data.discard_blocks  = nru_pdcp_sn_discard_blocks{};
  nru_pdcp_sn_discard_block block1a = {};
  block1a.pdcp_sn_start             = pdcp_sn;
  block1a.block_size                = 1;
  msg1.dl_user_data.discard_blocks.value().push_back(block1a);
  nru_pdcp_sn_discard_block block1b = {};
  block1b.pdcp_sn_start             = pdcp_sn + 3;
  block1b.block_size                = 2;
  msg1.dl_user_data.discard_blocks.value().push_back(block1b);
  f1u->handle_pdu(std::move(msg1));

  nru_dl_message msg2              = {};
  msg2.dl_user_data.discard_blocks = nru_pdcp_sn_discard_blocks{};
  nru_pdcp_sn_discard_block block2 = {};
  block2.pdcp_sn_start             = pdcp_sn + 9;
  block2.block_size                = 1;
  msg2.dl_user_data.discard_blocks.value().push_back(block2);
  f1u->handle_pdu(std::move(msg2));

  EXPECT_TRUE(tester->rx_sdu_list.empty());
  EXPECT_TRUE(tester->tx_msg_list.empty());

  // from block1a
  ASSERT_FALSE(tester->rx_discard_sdu_list.empty());
  ASSERT_EQ(tester->rx_discard_sdu_list.front(), pdcp_sn);
  tester->rx_discard_sdu_list.pop_front();

  // from block1b
  ASSERT_FALSE(tester->rx_discard_sdu_list.empty());
  ASSERT_EQ(tester->rx_discard_sdu_list.front(), pdcp_sn + 3);
  tester->rx_discard_sdu_list.pop_front();
  ASSERT_FALSE(tester->rx_discard_sdu_list.empty());
  ASSERT_EQ(tester->rx_discard_sdu_list.front(), pdcp_sn + 4);
  tester->rx_discard_sdu_list.pop_front();

  // from block2
  ASSERT_FALSE(tester->rx_discard_sdu_list.empty());
  ASSERT_EQ(tester->rx_discard_sdu_list.front(), pdcp_sn + 9);

  tester->rx_discard_sdu_list.pop_front();

  EXPECT_TRUE(tester->rx_discard_sdu_list.empty());
}

TEST_F(f1u_du_test, rx_pdcp_pdus)
{
  constexpr uint32_t pdu_size = 10;

  byte_buffer    rx_pdcp_pdu1           = create_sdu_byte_buffer(pdu_size, 0);
  nru_dl_message msg1                   = {};
  msg1.t_pdu                            = rx_pdcp_pdu1.deep_copy().value();
  msg1.dl_user_data.retransmission_flag = false;
  f1u->handle_pdu(std::move(msg1));

  byte_buffer    rx_pdcp_pdu2           = create_sdu_byte_buffer(pdu_size, 1);
  nru_dl_message msg2                   = {};
  msg2.t_pdu                            = rx_pdcp_pdu2.deep_copy().value();
  msg2.dl_user_data.retransmission_flag = false;
  f1u->handle_pdu(std::move(msg2));

  nru_dl_message msg2_retx                   = {};
  msg2_retx.t_pdu                            = rx_pdcp_pdu2.deep_copy().value();
  msg2_retx.dl_user_data.retransmission_flag = true;
  f1u->handle_pdu(std::move(msg2_retx));

  EXPECT_TRUE(tester->rx_discard_sdu_list.empty());
  EXPECT_TRUE(tester->tx_msg_list.empty());

  ASSERT_FALSE(tester->rx_sdu_list.empty());
  EXPECT_EQ(tester->rx_sdu_list.front().first, rx_pdcp_pdu1);
  EXPECT_FALSE(tester->rx_sdu_list.front().second);

  tester->rx_sdu_list.pop_front();

  ASSERT_FALSE(tester->rx_sdu_list.empty());
  EXPECT_EQ(tester->rx_sdu_list.front().first, rx_pdcp_pdu2);
  EXPECT_FALSE(tester->rx_sdu_list.front().second);

  tester->rx_sdu_list.pop_front();

  ASSERT_FALSE(tester->rx_sdu_list.empty());
  EXPECT_EQ(tester->rx_sdu_list.front().first, rx_pdcp_pdu2);
  EXPECT_TRUE(tester->rx_sdu_list.front().second);

  tester->rx_sdu_list.pop_front();

  EXPECT_TRUE(tester->rx_sdu_list.empty());
}

TEST_F(f1u_du_test, tx_pdcp_pdus)
{
  constexpr uint32_t pdu_size = 10;
  constexpr uint32_t pdcp_sn  = 123;

  byte_buffer tx_pdcp_pdu1 = create_sdu_byte_buffer(pdu_size, pdcp_sn);
  auto        chain1       = byte_buffer_chain::create(tx_pdcp_pdu1.deep_copy().value());
  ASSERT_TRUE(chain1.has_value());
  f1u->handle_sdu(std::move(chain1.value()));

  byte_buffer tx_pdcp_pdu2 = create_sdu_byte_buffer(pdu_size, pdcp_sn + 1);
  auto        chain2       = byte_buffer_chain::create(tx_pdcp_pdu2.deep_copy().value());
  ASSERT_TRUE(chain2.has_value());
  f1u->handle_sdu(std::move(chain2.value()));

  EXPECT_TRUE(tester->rx_discard_sdu_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_EQ(tester->tx_msg_list.front().t_pdu.value(), tx_pdcp_pdu1);
  ASSERT_TRUE(tester->tx_msg_list.front().data_delivery_status.has_value());
  {
    nru_dl_data_delivery_status& status = tester->tx_msg_list.front().data_delivery_status.value();
    EXPECT_FALSE(status.final_frame_ind);
    EXPECT_EQ(status.desired_buffer_size_for_drb, f1u_intial_rlc_queue_bytes_limit);
    EXPECT_FALSE(status.desired_data_rate);
    EXPECT_FALSE(status.lost_nru_sn_ranges.has_value());
    EXPECT_FALSE(status.highest_delivered_pdcp_sn.has_value());
    EXPECT_FALSE(status.highest_transmitted_pdcp_sn.has_value());
    EXPECT_FALSE(status.cause_value.has_value());
    EXPECT_FALSE(status.highest_delivered_retransmitted_pdcp_sn.has_value());
    EXPECT_FALSE(status.highest_retransmitted_pdcp_sn.has_value());
  }
  EXPECT_FALSE(tester->tx_msg_list.front().assistance_information.has_value());

  tester->tx_msg_list.pop_front();

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_EQ(tester->tx_msg_list.front().t_pdu.value(), tx_pdcp_pdu2);
  ASSERT_TRUE(tester->tx_msg_list.front().data_delivery_status.has_value());
  {
    nru_dl_data_delivery_status& status = tester->tx_msg_list.front().data_delivery_status.value();
    EXPECT_FALSE(status.final_frame_ind);
    EXPECT_EQ(status.desired_buffer_size_for_drb, f1u_intial_rlc_queue_bytes_limit);
    EXPECT_FALSE(status.desired_data_rate);
    EXPECT_FALSE(status.lost_nru_sn_ranges.has_value());
    EXPECT_FALSE(status.highest_delivered_pdcp_sn.has_value());
    EXPECT_FALSE(status.highest_transmitted_pdcp_sn.has_value());
    EXPECT_FALSE(status.cause_value.has_value());
    EXPECT_FALSE(status.highest_delivered_retransmitted_pdcp_sn.has_value());
    EXPECT_FALSE(status.highest_retransmitted_pdcp_sn.has_value());
  }
  EXPECT_FALSE(tester->tx_msg_list.front().assistance_information.has_value());

  tester->tx_msg_list.pop_front();

  EXPECT_TRUE(tester->tx_msg_list.empty());
}

TEST_F(f1u_du_test, tx_pdcp_pdus_with_transmit_notification)
{
  constexpr uint32_t pdu_size        = 10;
  constexpr uint32_t pdcp_sn         = 123;
  constexpr uint32_t highest_pdcp_sn = 55;

  f1u->handle_transmit_notification(highest_pdcp_sn, 0);
  f1u->handle_transmit_notification(highest_pdcp_sn + 1, 0);

  byte_buffer tx_pdcp_pdu1 = create_sdu_byte_buffer(pdu_size, pdcp_sn);
  auto        chain1       = byte_buffer_chain::create(tx_pdcp_pdu1.deep_copy().value());
  ASSERT_TRUE(chain1.has_value());
  f1u->handle_sdu(std::move(chain1.value()));

  byte_buffer tx_pdcp_pdu2 = create_sdu_byte_buffer(pdu_size, pdcp_sn + 1);
  auto        chain2       = byte_buffer_chain::create(tx_pdcp_pdu2.deep_copy().value());
  ASSERT_TRUE(chain2.has_value());
  f1u->handle_sdu(std::move(chain2.value()));

  EXPECT_TRUE(tester->rx_discard_sdu_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_EQ(tester->tx_msg_list.front().t_pdu.value(), tx_pdcp_pdu1);
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
  EXPECT_FALSE(tester->tx_msg_list.front().assistance_information.has_value());

  tester->tx_msg_list.pop_front();

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_EQ(tester->tx_msg_list.front().t_pdu.value(), tx_pdcp_pdu2);
  EXPECT_TRUE(tester->tx_msg_list.front().data_delivery_status.has_value());
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
  EXPECT_FALSE(tester->tx_msg_list.front().assistance_information.has_value());

  tester->tx_msg_list.pop_front();

  EXPECT_TRUE(tester->tx_msg_list.empty());
}

TEST_F(f1u_du_test, tx_pdcp_pdus_with_delivery_notification)
{
  constexpr uint32_t pdu_size        = 10;
  constexpr uint32_t pdcp_sn         = 123;
  constexpr uint32_t highest_pdcp_sn = 55;

  f1u->handle_delivery_notification(highest_pdcp_sn);
  f1u->handle_delivery_notification(highest_pdcp_sn + 1);

  // advance time just before the timer-based UL notification is triggered
  for (uint32_t t = 0; t < f1u_ul_notif_time_ms - 1; t++) {
    EXPECT_TRUE(tester->tx_msg_list.empty());
    tick();
  }
  EXPECT_TRUE(tester->tx_msg_list.empty());

  byte_buffer tx_pdcp_pdu1 = create_sdu_byte_buffer(pdu_size, pdcp_sn);
  auto        chain1       = byte_buffer_chain::create(tx_pdcp_pdu1.deep_copy().value());
  ASSERT_TRUE(chain1.has_value());
  f1u->handle_sdu(std::move(chain1.value()));

  byte_buffer tx_pdcp_pdu2 = create_sdu_byte_buffer(pdu_size, pdcp_sn + 1);
  auto        chain2       = byte_buffer_chain::create(tx_pdcp_pdu2.deep_copy().value());
  ASSERT_TRUE(chain2.has_value());
  f1u->handle_sdu(std::move(chain2.value()));

  EXPECT_TRUE(tester->rx_discard_sdu_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_EQ(tester->tx_msg_list.front().t_pdu.value(), tx_pdcp_pdu1);
  ASSERT_TRUE(tester->tx_msg_list.front().data_delivery_status.has_value());
  {
    nru_dl_data_delivery_status& status = tester->tx_msg_list.front().data_delivery_status.value();
    EXPECT_FALSE(status.final_frame_ind);
    EXPECT_FALSE(status.lost_nru_sn_ranges.has_value());
    ASSERT_TRUE(status.highest_delivered_pdcp_sn.has_value());
    EXPECT_EQ(status.highest_delivered_pdcp_sn.value(), highest_pdcp_sn + 1);
    EXPECT_FALSE(status.highest_delivered_retransmitted_pdcp_sn.has_value());
    EXPECT_FALSE(status.cause_value.has_value());
    EXPECT_FALSE(status.highest_retransmitted_pdcp_sn.has_value());
    EXPECT_FALSE(status.highest_transmitted_pdcp_sn.has_value());
  }
  EXPECT_FALSE(tester->tx_msg_list.front().assistance_information.has_value());

  tester->tx_msg_list.pop_front();

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_EQ(tester->tx_msg_list.front().t_pdu.value(), tx_pdcp_pdu2);
  EXPECT_TRUE(tester->tx_msg_list.front().data_delivery_status.has_value());
  {
    nru_dl_data_delivery_status& status = tester->tx_msg_list.front().data_delivery_status.value();
    EXPECT_FALSE(status.final_frame_ind);
    EXPECT_FALSE(status.lost_nru_sn_ranges.has_value());
    ASSERT_TRUE(status.highest_delivered_pdcp_sn.has_value());
    EXPECT_EQ(status.highest_delivered_pdcp_sn.value(), highest_pdcp_sn + 1);
    EXPECT_FALSE(status.highest_delivered_retransmitted_pdcp_sn.has_value());
    EXPECT_FALSE(status.cause_value.has_value());
    EXPECT_FALSE(status.highest_retransmitted_pdcp_sn.has_value());
    EXPECT_FALSE(status.highest_transmitted_pdcp_sn.has_value());
  }
  EXPECT_FALSE(tester->tx_msg_list.front().assistance_information.has_value());

  tester->tx_msg_list.pop_front();

  EXPECT_TRUE(tester->tx_msg_list.empty());

  // handle another transmit notification; check UL notif timer has been reset to full time
  f1u->handle_transmit_notification(highest_pdcp_sn + 2, 0);

  EXPECT_TRUE(tester->rx_discard_sdu_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());

  for (uint32_t t = 0; t < f1u_ul_notif_time_ms; t++) {
    EXPECT_TRUE(tester->tx_msg_list.empty());
    tick();
  }

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_FALSE(tester->tx_msg_list.front().t_pdu.has_value());
  EXPECT_FALSE(tester->tx_msg_list.front().assistance_information.has_value());
  ASSERT_TRUE(tester->tx_msg_list.front().data_delivery_status.has_value());
  {
    nru_dl_data_delivery_status& status = tester->tx_msg_list.front().data_delivery_status.value();
    EXPECT_FALSE(status.final_frame_ind);
    EXPECT_FALSE(status.lost_nru_sn_ranges.has_value());
    ASSERT_TRUE(status.highest_delivered_pdcp_sn.has_value());
    EXPECT_EQ(status.highest_delivered_pdcp_sn.value(), highest_pdcp_sn + 1);
    EXPECT_FALSE(status.cause_value.has_value());
    EXPECT_FALSE(status.highest_delivered_retransmitted_pdcp_sn.has_value());
    EXPECT_FALSE(status.highest_retransmitted_pdcp_sn.has_value());
    ASSERT_TRUE(status.highest_transmitted_pdcp_sn.has_value());
    EXPECT_EQ(status.highest_transmitted_pdcp_sn.value(), highest_pdcp_sn + 2);
  }
  tester->tx_msg_list.pop_front();

  EXPECT_TRUE(tester->tx_msg_list.empty());
}

TEST_F(f1u_du_test, tx_transmit_notification)
{
  constexpr uint32_t highest_pdcp_sn = 123;

  f1u->handle_transmit_notification(highest_pdcp_sn, 0);
  f1u->handle_transmit_notification(highest_pdcp_sn + 1, 0);

  EXPECT_TRUE(tester->rx_discard_sdu_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());

  for (uint32_t t = 0; t < f1u_ul_notif_time_ms; t++) {
    EXPECT_TRUE(tester->tx_msg_list.empty());
    tick();
  }

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_FALSE(tester->tx_msg_list.front().t_pdu.has_value());
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

  for (uint32_t t = 0; t < f1u_ul_notif_time_ms; t++) {
    EXPECT_TRUE(tester->tx_msg_list.empty());
    tick();
  }

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_FALSE(tester->tx_msg_list.front().t_pdu.has_value());
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

TEST_F(f1u_du_test, rx_with_poll_triggers_ddds_when_transmit_was_notified)
{
  constexpr uint32_t highest_pdcp_sn = 123;

  // Notify transmit
  f1u->handle_transmit_notification(highest_pdcp_sn, 0);
  f1u->handle_transmit_notification(highest_pdcp_sn + 1, 0);

  EXPECT_TRUE(tester->rx_discard_sdu_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());

  // DL message with poll for DL Data Delivery Status Report
  constexpr uint32_t pdu_size      = 10;
  byte_buffer        rx_pdcp_pdu1  = create_sdu_byte_buffer(pdu_size, 0);
  nru_dl_message     msg1          = {};
  msg1.t_pdu                       = rx_pdcp_pdu1.deep_copy().value();
  msg1.dl_user_data.report_polling = true;
  f1u->handle_pdu(std::move(msg1));

  // This should immediately trigger a DDDS
  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_FALSE(tester->tx_msg_list.front().t_pdu.has_value());
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

  // Also check that the DL PDU was properly forwarded
  ASSERT_FALSE(tester->rx_sdu_list.empty());
  EXPECT_EQ(tester->rx_sdu_list.front().first, rx_pdcp_pdu1);
  EXPECT_FALSE(tester->rx_sdu_list.front().second);

  tester->rx_sdu_list.pop_front();

  EXPECT_TRUE(tester->rx_sdu_list.empty());
}

TEST_F(f1u_du_test, rx_with_poll_triggers_ddds_when_delivery_was_notified)
{
  constexpr uint32_t highest_pdcp_sn = 123;

  // Notify delivery
  f1u->handle_delivery_notification(highest_pdcp_sn);
  f1u->handle_delivery_notification(highest_pdcp_sn + 1);

  EXPECT_TRUE(tester->rx_discard_sdu_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());

  // DL message with poll for DL Data Delivery Status Report
  constexpr uint32_t pdu_size      = 10;
  byte_buffer        rx_pdcp_pdu1  = create_sdu_byte_buffer(pdu_size, 0);
  nru_dl_message     msg1          = {};
  msg1.t_pdu                       = rx_pdcp_pdu1.deep_copy().value();
  msg1.dl_user_data.report_polling = true;
  f1u->handle_pdu(std::move(msg1));

  // This should immediately trigger a DDDS
  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_FALSE(tester->tx_msg_list.front().t_pdu.has_value());
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

  // Also check that the DL PDU was properly forwarded
  ASSERT_FALSE(tester->rx_sdu_list.empty());
  EXPECT_EQ(tester->rx_sdu_list.front().first, rx_pdcp_pdu1);
  EXPECT_FALSE(tester->rx_sdu_list.front().second);

  tester->rx_sdu_list.pop_front();

  EXPECT_TRUE(tester->rx_sdu_list.empty());
}

TEST_F(f1u_du_test, rx_with_poll_triggers_no_ddds_when_nothing_was_notified)
{
  // Do not notify any transmit or delivery

  EXPECT_TRUE(tester->rx_discard_sdu_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());

  // DL message with poll for DL Data Delivery Status Report
  constexpr uint32_t pdu_size      = 10;
  byte_buffer        rx_pdcp_pdu1  = create_sdu_byte_buffer(pdu_size, 0);
  nru_dl_message     msg1          = {};
  msg1.t_pdu                       = rx_pdcp_pdu1.deep_copy().value();
  msg1.dl_user_data.report_polling = true;
  f1u->handle_pdu(std::move(msg1));

  // This triggers a DDDS that only includes the initial "desired_buffer_size_for_drb", no other data is available yet
  ASSERT_TRUE(tester->tx_msg_list.front().data_delivery_status.has_value());
  {
    nru_dl_data_delivery_status& status = tester->tx_msg_list.front().data_delivery_status.value();
    EXPECT_FALSE(status.final_frame_ind);
    EXPECT_EQ(status.desired_buffer_size_for_drb, f1u_intial_rlc_queue_bytes_limit);
    EXPECT_FALSE(status.desired_data_rate);
    EXPECT_FALSE(status.lost_nru_sn_ranges.has_value());
    EXPECT_FALSE(status.highest_delivered_pdcp_sn.has_value());
    EXPECT_FALSE(status.highest_transmitted_pdcp_sn.has_value());
    EXPECT_FALSE(status.cause_value.has_value());
    EXPECT_FALSE(status.highest_delivered_retransmitted_pdcp_sn.has_value());
    EXPECT_FALSE(status.highest_retransmitted_pdcp_sn.has_value());
  }
  EXPECT_FALSE(tester->tx_msg_list.front().assistance_information.has_value());

  // Also check that the DL PDU was properly forwarded
  ASSERT_FALSE(tester->rx_sdu_list.empty());
  EXPECT_EQ(tester->rx_sdu_list.front().first, rx_pdcp_pdu1);
  EXPECT_FALSE(tester->rx_sdu_list.front().second);

  tester->rx_sdu_list.pop_front();

  EXPECT_TRUE(tester->rx_sdu_list.empty());
}

TEST_F(f1u_du_buffer_test, buffer_ul_pdus)
{
  constexpr uint32_t pdu_size = 10;
  constexpr uint32_t pdcp_sn  = 123;

  byte_buffer tx_pdcp_pdu1 = create_sdu_byte_buffer(pdu_size, pdcp_sn);
  auto        chain1       = byte_buffer_chain::create(tx_pdcp_pdu1.deep_copy().value());
  ASSERT_TRUE(chain1.has_value());
  f1u->handle_sdu(std::move(chain1.value()));

  byte_buffer tx_pdcp_pdu2 = create_sdu_byte_buffer(pdu_size, pdcp_sn + 1);
  auto        chain2       = byte_buffer_chain::create(tx_pdcp_pdu2.deep_copy().value());
  ASSERT_TRUE(chain2.has_value());
  f1u->handle_sdu(std::move(chain2.value()));

  byte_buffer tx_pdcp_pdu3 = create_sdu_byte_buffer(pdu_size, pdcp_sn + 2);
  auto        chain3       = byte_buffer_chain::create(tx_pdcp_pdu3.deep_copy().value());
  ASSERT_TRUE(chain3.has_value());
  f1u->handle_sdu(std::move(chain2.value())); // This should be dropped at the UL buffer, as the queue size is only 2.

  // Check F1-U is buffering.
  ASSERT_TRUE(tester->tx_msg_list.empty());
  f1u->flush_ul_buffer();

  // Check flushing is correct.
  ASSERT_EQ(tester->tx_msg_list.size(), 2);

  // Check flush was in order.
  byte_buffer t_pdu1 = tester->tx_msg_list.front().t_pdu.value().deep_copy().value();
  ASSERT_EQ(t_pdu1[0], pdcp_sn);
  tester->tx_msg_list.pop_front();
  byte_buffer t_pdu2 = tester->tx_msg_list.front().t_pdu.value().deep_copy().value();
  ASSERT_EQ(t_pdu2[0], pdcp_sn + 1);
  tester->tx_msg_list.pop_front();
}

TEST_F(f1u_du_buffer_test, buffer_ul_pdus_timeout)
{
  constexpr uint32_t pdu_size = 10;
  constexpr uint32_t pdcp_sn  = 123;

  byte_buffer tx_pdcp_pdu1 = create_sdu_byte_buffer(pdu_size, pdcp_sn);
  auto        chain1       = byte_buffer_chain::create(tx_pdcp_pdu1.deep_copy().value());
  ASSERT_TRUE(chain1.has_value());
  f1u->handle_sdu(std::move(chain1.value()));

  byte_buffer tx_pdcp_pdu2 = create_sdu_byte_buffer(pdu_size, pdcp_sn + 1);
  auto        chain2       = byte_buffer_chain::create(tx_pdcp_pdu2.deep_copy().value());
  ASSERT_TRUE(chain2.has_value());
  f1u->handle_sdu(std::move(chain2.value()));

  byte_buffer tx_pdcp_pdu3 = create_sdu_byte_buffer(pdu_size, pdcp_sn + 2);
  auto        chain3       = byte_buffer_chain::create(tx_pdcp_pdu3.deep_copy().value());
  ASSERT_TRUE(chain3.has_value());
  f1u->handle_sdu(std::move(chain2.value())); // This should be dropped at the UL buffer, as the queue size is only 2.

  // Check F1-U is buffering.
  ASSERT_TRUE(tester->tx_msg_list.empty());
  tick_all(5);

  // Check flushing is correct.
  ASSERT_EQ(tester->tx_msg_list.size(), 2);

  // Check flush was in order.
  byte_buffer t_pdu1 = tester->tx_msg_list.front().t_pdu.value().deep_copy().value();
  ASSERT_EQ(t_pdu1[0], pdcp_sn);
  tester->tx_msg_list.pop_front();
  byte_buffer t_pdu2 = tester->tx_msg_list.front().t_pdu.value().deep_copy().value();
  ASSERT_EQ(t_pdu2[0], pdcp_sn + 1);
  tester->tx_msg_list.pop_front();
}
