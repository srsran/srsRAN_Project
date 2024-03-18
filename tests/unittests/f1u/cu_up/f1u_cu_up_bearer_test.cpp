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

#include "lib/f1u/cu_up/f1u_bearer_impl.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <list>

using namespace srsran;
using namespace srs_cu_up;

/// Mocking class of the surrounding layers invoked by the F1-U bearer
class f1u_cu_up_test_frame : public f1u_tx_pdu_notifier,
                             public f1u_rx_delivery_notifier,
                             public f1u_rx_sdu_notifier,
                             public f1u_bearer_disconnector
{
public:
  std::list<nru_dl_message>          tx_msg_list;
  std::list<uint32_t>                highest_transmitted_pdcp_sn_list;
  std::list<uint32_t>                highest_delivered_pdcp_sn_list;
  std::list<byte_buffer_chain>       rx_sdu_list;
  std::list<up_transport_layer_info> removed_ul_tnls;

  // f1u_tx_pdu_notifier interface
  void on_new_pdu(nru_dl_message msg) override { tx_msg_list.push_back(std::move(msg)); }

  // f1u_rx_delivery_notifier interface
  void on_transmit_notification(uint32_t highest_pdcp_sn) override
  {
    highest_transmitted_pdcp_sn_list.push_back(highest_pdcp_sn);
  }
  void on_delivery_notification(uint32_t highest_pdcp_sn) override
  {
    highest_delivered_pdcp_sn_list.push_back(highest_pdcp_sn);
  }

  // f1u_rx_sdu_notifier interface
  void on_new_sdu(byte_buffer_chain sdu) override { rx_sdu_list.push_back(std::move(sdu)); }

  // f1u_bearer_disconnector interface
  void disconnect_cu_bearer(const up_transport_layer_info& ul_tnl) override { removed_ul_tnls.push_back(ul_tnl); }
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

/// Fixture class for F1-U CU-UP tests
class f1u_cu_up_test : public ::testing::Test, public f1u_trx_test
{
protected:
  const unsigned inactivity_time_ms = 100;

  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init F1-U logger
    srslog::fetch_basic_logger("CU-F1-U", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("CU-F1-U", false).set_hex_dump_max_size(100);

    // create tester and testee
    logger.info("Creating F1-U bearer");
    tester              = std::make_unique<f1u_cu_up_test_frame>();
    drb_id_t drb_id     = drb_id_t::drb1;
    ue_inactivity_timer = ue_timer_factory.create_timer();
    ue_inactivity_timer.set(std::chrono::milliseconds(inactivity_time_ms), [this](timer_id_t) {
      // Text
      ue_inactivity_triggered = true;
    });
    ue_inactivity_timer.run();
    f1u = std::make_unique<f1u_bearer_impl>(
        0,
        drb_id,
        up_transport_layer_info(transport_layer_address::create_from_string("127.0.0.1"),
                                gtpu_teid_t{ul_teid_next.value()++}),
        *tester,
        *tester,
        *tester,
        ue_timer_factory,
        ue_inactivity_timer,
        ue_worker,
        *tester);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  void tick(unsigned ms = 1)
  {
    for (unsigned t = 0; t < ms; t++) {
      ue_timer_manager.tick();
      ue_worker.run_pending_tasks();
    }
  }

  srslog::basic_logger&                 logger = srslog::fetch_basic_logger("TEST", false);
  manual_task_worker                    ue_worker{128};
  timer_manager                         ue_timer_manager;
  timer_factory                         ue_timer_factory{ue_timer_manager, ue_worker};
  unique_timer                          ue_inactivity_timer;
  std::unique_ptr<f1u_cu_up_test_frame> tester;
  std::unique_ptr<f1u_bearer_impl>      f1u;
  gtpu_teid_t                           ul_teid_next{1234};

  bool ue_inactivity_triggered = false;
};

TEST_F(f1u_cu_up_test, create_and_delete)
{
  EXPECT_FALSE(ue_inactivity_triggered);
  EXPECT_TRUE(tester->tx_msg_list.empty());
  EXPECT_TRUE(tester->highest_transmitted_pdcp_sn_list.empty());
  EXPECT_TRUE(tester->highest_delivered_pdcp_sn_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());
  EXPECT_TRUE(tester->removed_ul_tnls.empty());
  gtpu_teid_t ul_teid = f1u->get_ul_teid();
  f1u.reset();
  ASSERT_FALSE(tester->removed_ul_tnls.empty());
  EXPECT_EQ(tester->removed_ul_tnls.front().gtp_teid, ul_teid);
  tester->removed_ul_tnls.pop_front();
  EXPECT_TRUE(tester->removed_ul_tnls.empty());
}

TEST_F(f1u_cu_up_test, tx_discard)
{
  constexpr uint32_t pdu_size = 10;
  constexpr uint32_t pdcp_sn  = 123;

  // advance time by one to ensure timer is stopped and ignores this tick
  tick();

  f1u->discard_sdu(pdcp_sn);
  // advance time just before the timer-based DL notification is triggered
  for (uint32_t t = 0; t < f1u_dl_notif_time_ms - 1; t++) {
    EXPECT_TRUE(tester->tx_msg_list.empty());
    tick();
  }
  EXPECT_TRUE(tester->tx_msg_list.empty());

  f1u->discard_sdu(pdcp_sn + 1); // this should be merged into previous block
  f1u->discard_sdu(pdcp_sn + 3); // this should trigger the next block

  byte_buffer tx_pdcp_pdu1 = create_sdu_byte_buffer(pdu_size, 0xcc);
  pdcp_tx_pdu sdu1;
  sdu1.buf     = tx_pdcp_pdu1.deep_copy().value();
  sdu1.pdcp_sn = pdcp_sn + 22;

  // transmit a PDU to piggy-back previous discard
  f1u->handle_sdu(std::move(sdu1));

  EXPECT_TRUE(tester->highest_transmitted_pdcp_sn_list.empty());
  EXPECT_TRUE(tester->highest_delivered_pdcp_sn_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_FALSE(tester->tx_msg_list.front().t_pdu.empty());
  EXPECT_EQ(tester->tx_msg_list.front().t_pdu, tx_pdcp_pdu1);
  ASSERT_TRUE(tester->tx_msg_list.front().dl_user_data.discard_blocks.has_value());
  ASSERT_EQ(tester->tx_msg_list.front().dl_user_data.discard_blocks.value().size(), 2);
  EXPECT_EQ(tester->tx_msg_list.front().dl_user_data.discard_blocks.value()[0].pdcp_sn_start, pdcp_sn);
  EXPECT_EQ(tester->tx_msg_list.front().dl_user_data.discard_blocks.value()[0].block_size, 2);
  EXPECT_EQ(tester->tx_msg_list.front().dl_user_data.discard_blocks.value()[1].pdcp_sn_start, pdcp_sn + 3);
  EXPECT_EQ(tester->tx_msg_list.front().dl_user_data.discard_blocks.value()[1].block_size, 1);

  tester->tx_msg_list.pop_front();
  ASSERT_TRUE(tester->tx_msg_list.empty());

  // advance time by one to ensure timer is stopped and ignores this tick
  tick();

  f1u->discard_sdu(pdcp_sn + 7);
  // advance time just before the timer-based DL notification is triggered
  for (uint32_t t = 0; t < f1u_dl_notif_time_ms - 1; t++) {
    EXPECT_TRUE(tester->tx_msg_list.empty());
    tick();
  }
  EXPECT_TRUE(tester->tx_msg_list.empty());

  // final tick to expire timer
  tick();
  EXPECT_FALSE(tester->tx_msg_list.empty());

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_TRUE(tester->tx_msg_list.front().t_pdu.empty());
  ASSERT_TRUE(tester->tx_msg_list.front().dl_user_data.discard_blocks.has_value());
  ASSERT_EQ(tester->tx_msg_list.front().dl_user_data.discard_blocks.value().size(), 1);
  EXPECT_EQ(tester->tx_msg_list.front().dl_user_data.discard_blocks.value()[0].pdcp_sn_start, pdcp_sn + 7);
  EXPECT_EQ(tester->tx_msg_list.front().dl_user_data.discard_blocks.value()[0].block_size, 1);

  tester->tx_msg_list.pop_front();

  EXPECT_TRUE(tester->tx_msg_list.empty());
}

TEST_F(f1u_cu_up_test, tx_pdcp_pdus)
{
  tick(inactivity_time_ms - 1);
  EXPECT_FALSE(ue_inactivity_triggered);

  constexpr uint32_t pdu_size = 10;
  constexpr uint32_t pdcp_sn  = 123;

  byte_buffer tx_pdcp_pdu1 = create_sdu_byte_buffer(pdu_size, pdcp_sn);
  pdcp_tx_pdu sdu1;
  sdu1.buf     = tx_pdcp_pdu1.deep_copy().value();
  sdu1.pdcp_sn = pdcp_sn;
  f1u->handle_sdu(std::move(sdu1));

  byte_buffer tx_pdcp_pdu2 = create_sdu_byte_buffer(pdu_size, pdcp_sn + 1);
  pdcp_tx_pdu sdu2;
  sdu2.buf     = tx_pdcp_pdu2.deep_copy().value();
  sdu2.pdcp_sn = pdcp_sn + 1;
  f1u->handle_sdu(std::move(sdu2));

  EXPECT_TRUE(tester->highest_transmitted_pdcp_sn_list.empty());
  EXPECT_TRUE(tester->highest_delivered_pdcp_sn_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_EQ(tester->tx_msg_list.front().t_pdu, tx_pdcp_pdu1);
  EXPECT_EQ(tester->tx_msg_list.front().pdcp_sn, pdcp_sn);
  EXPECT_FALSE(tester->tx_msg_list.front().dl_user_data.discard_blocks.has_value());

  tester->tx_msg_list.pop_front();

  ASSERT_FALSE(tester->tx_msg_list.empty());
  EXPECT_EQ(tester->tx_msg_list.front().t_pdu, tx_pdcp_pdu2);
  EXPECT_EQ(tester->tx_msg_list.front().pdcp_sn, pdcp_sn + 1);
  EXPECT_FALSE(tester->tx_msg_list.front().dl_user_data.discard_blocks.has_value());

  tester->tx_msg_list.pop_front();

  EXPECT_TRUE(tester->tx_msg_list.empty());

  // DL PDUs do not restart inactivity timer (only their transmit notif), hence one more tick shall expire the timer
  tick(inactivity_time_ms);
  EXPECT_TRUE(ue_inactivity_triggered);
}

TEST_F(f1u_cu_up_test, rx_pdcp_pdus)
{
  tick(inactivity_time_ms - 1);
  EXPECT_FALSE(ue_inactivity_triggered);

  constexpr uint32_t pdu_size = 10;
  constexpr uint32_t pdcp_sn  = 123;

  byte_buffer    rx_pdcp_pdu1 = create_sdu_byte_buffer(pdu_size, pdcp_sn);
  nru_ul_message msg1;
  auto           chain1 = byte_buffer_chain::create(rx_pdcp_pdu1.deep_copy().value());
  EXPECT_FALSE(chain1.is_error());
  msg1.t_pdu = std::move(chain1.value());
  f1u->handle_pdu(std::move(msg1));

  // UL PDUs restart inactivity timer, hence further ticks shall not expire the timer
  tick(inactivity_time_ms - 1);
  EXPECT_FALSE(ue_inactivity_triggered);

  byte_buffer    rx_pdcp_pdu2 = create_sdu_byte_buffer(pdu_size, pdcp_sn + 1);
  nru_ul_message msg2;
  auto           chain2 = byte_buffer_chain::create(rx_pdcp_pdu2.deep_copy().value());
  EXPECT_FALSE(chain2.is_error());
  msg2.t_pdu = std::move(chain2.value());
  f1u->handle_pdu(std::move(msg2));

  // UL PDUs restart inactivity timer, hence further ticks shall not expire the timer
  tick(inactivity_time_ms - 1);
  EXPECT_FALSE(ue_inactivity_triggered);

  EXPECT_TRUE(tester->tx_msg_list.empty());
  EXPECT_TRUE(tester->highest_transmitted_pdcp_sn_list.empty());
  EXPECT_TRUE(tester->highest_delivered_pdcp_sn_list.empty());

  ASSERT_FALSE(tester->rx_sdu_list.empty());
  EXPECT_EQ(tester->rx_sdu_list.front(), rx_pdcp_pdu1);

  tester->rx_sdu_list.pop_front();

  ASSERT_FALSE(tester->rx_sdu_list.empty());
  EXPECT_EQ(tester->rx_sdu_list.front(), rx_pdcp_pdu2);

  tester->rx_sdu_list.pop_front();

  EXPECT_TRUE(tester->rx_sdu_list.empty());

  // One more tick shall finally expire the inactivity timer
  tick(inactivity_time_ms);
  EXPECT_TRUE(ue_inactivity_triggered);
}

TEST_F(f1u_cu_up_test, rx_transmit_notification)
{
  tick(inactivity_time_ms - 1);
  EXPECT_FALSE(ue_inactivity_triggered);

  constexpr uint32_t highest_pdcp_sn = 123;

  nru_dl_data_delivery_status status1 = {};
  status1.highest_transmitted_pdcp_sn = highest_pdcp_sn;
  nru_ul_message msg1                 = {};
  msg1.data_delivery_status           = std::move(status1);
  f1u->handle_pdu(std::move(msg1));

  // Transmit notifications restart inactivity timer, hence further ticks shall not expire the timer
  tick(inactivity_time_ms - 1);
  EXPECT_FALSE(ue_inactivity_triggered);

  nru_dl_data_delivery_status status2 = {};
  status2.highest_transmitted_pdcp_sn = highest_pdcp_sn + 1;
  nru_ul_message msg2                 = {};
  msg2.data_delivery_status           = std::move(status2);
  f1u->handle_pdu(std::move(msg2));

  // Transmit notifications restart inactivity timer, hence further ticks shall not expire the timer
  tick(inactivity_time_ms - 1);
  EXPECT_FALSE(ue_inactivity_triggered);

  EXPECT_TRUE(tester->tx_msg_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());
  EXPECT_TRUE(tester->highest_delivered_pdcp_sn_list.empty());
  ASSERT_FALSE(tester->highest_transmitted_pdcp_sn_list.empty());
  EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), highest_pdcp_sn);

  tester->highest_transmitted_pdcp_sn_list.pop_front();

  EXPECT_TRUE(tester->tx_msg_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());
  EXPECT_TRUE(tester->highest_delivered_pdcp_sn_list.empty());
  ASSERT_FALSE(tester->highest_transmitted_pdcp_sn_list.empty());
  EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), highest_pdcp_sn + 1);

  tester->highest_transmitted_pdcp_sn_list.pop_front();

  EXPECT_TRUE(tester->highest_transmitted_pdcp_sn_list.empty());

  // One more tick shall finally expire the inactivity timer
  tick(inactivity_time_ms);
  EXPECT_TRUE(ue_inactivity_triggered);
}

TEST_F(f1u_cu_up_test, rx_delivery_notification)
{
  tick(inactivity_time_ms - 1);
  EXPECT_FALSE(ue_inactivity_triggered);

  constexpr uint32_t highest_pdcp_sn = 123;

  nru_dl_data_delivery_status status1 = {};
  status1.highest_delivered_pdcp_sn   = highest_pdcp_sn;
  nru_ul_message msg1                 = {};
  msg1.data_delivery_status           = std::move(status1);
  f1u->handle_pdu(std::move(msg1));

  nru_dl_data_delivery_status status2 = {};
  status2.highest_delivered_pdcp_sn   = highest_pdcp_sn + 1;
  nru_ul_message msg2                 = {};
  msg2.data_delivery_status           = std::move(status2);
  f1u->handle_pdu(std::move(msg2));

  EXPECT_TRUE(tester->tx_msg_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());
  EXPECT_TRUE(tester->highest_transmitted_pdcp_sn_list.empty());
  ASSERT_FALSE(tester->highest_delivered_pdcp_sn_list.empty());
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), highest_pdcp_sn);

  tester->highest_delivered_pdcp_sn_list.pop_front();

  EXPECT_TRUE(tester->tx_msg_list.empty());
  EXPECT_TRUE(tester->rx_sdu_list.empty());
  EXPECT_TRUE(tester->highest_transmitted_pdcp_sn_list.empty());
  ASSERT_FALSE(tester->highest_delivered_pdcp_sn_list.empty());
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), highest_pdcp_sn + 1);

  tester->highest_delivered_pdcp_sn_list.pop_front();

  EXPECT_TRUE(tester->highest_delivered_pdcp_sn_list.empty());

  // Delivery notifications do not restart inactivity timer, hence one more tick shall expire the timer
  tick(inactivity_time_ms);
  EXPECT_TRUE(ue_inactivity_triggered);
}
