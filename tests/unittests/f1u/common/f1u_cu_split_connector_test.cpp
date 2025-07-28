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

#include "tests/unittests/gateways/test_helpers.h"
#include "srsran/f1u/cu_up/split_connector/f1u_split_connector_factory.h"
#include "srsran/gateways/udp_network_gateway_factory.h"
#include "srsran/gtpu/gtpu_demux_factory.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/inline_task_executor.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/io/io_broker_factory.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_up;

namespace {

// dummy CU-UP RX bearer interface
class dummy_f1u_cu_up_rx_notifier : public f1u_cu_up_gateway_bearer_rx_notifier
{
public:
  dummy_f1u_cu_up_rx_notifier() = default;

  void on_new_pdu(nru_ul_message msg) override
  {
    std::lock_guard<std::mutex> lock(rx_mutex);
    if (msg.t_pdu.has_value()) {
      rx_bytes += msg.t_pdu->length();
    }
    msg_queue.push(std::move(msg));
    rx_cvar.notify_one();
  }

  unsigned get_rx_bytes()
  {
    std::lock_guard<std::mutex> lock(rx_mutex);
    return rx_bytes;
  }

  expected<nru_ul_message> get_rx_pdu_blocking(manual_task_worker&       ue_worker,
                                               std::chrono::milliseconds timeout_ms = std::chrono::milliseconds(5000))
  {
    const int                 nof_attempts       = 100;
    std::chrono::milliseconds attempt_timeout_ms = timeout_ms / nof_attempts;

    // wait until at least one PDU is received
    std::unique_lock<std::mutex> lock(rx_mutex);
    for (int i = 0; i < nof_attempts; i++) {
      if (!rx_cvar.wait_for(lock, attempt_timeout_ms, [this]() { return !msg_queue.empty(); })) {
        if (not msg_queue.empty()) {
          break;
        }
        rx_mutex.unlock();
        ue_worker.run_pending_tasks(); // unlock so we can push into the message queue
        rx_mutex.lock();
      }
    }

    if (msg_queue.empty()) {
      return make_unexpected(default_error_t{});
    }

    nru_ul_message pdu = std::move(msg_queue.front());
    msg_queue.pop();
    return pdu;
  }

  bool empty()
  {
    std::lock_guard<std::mutex> lock(rx_mutex);
    return msg_queue.empty();
  }

private:
  std::queue<nru_ul_message> msg_queue;
  std::mutex                 rx_mutex;
  std::condition_variable    rx_cvar;

  unsigned rx_bytes = 0;
};

/// Fixture class for F1-U connector tests.
/// It requires TEST_F() to create/spawn tests
class f1u_cu_split_connector_test : public ::testing::Test
{
public:
  f1u_cu_split_connector_test() { epoll_broker = create_io_broker(io_broker_type::epoll); }

protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init logger
    f1u_logger_cu.set_level(srslog::basic_levels::debug);
    f1u_logger_cu.set_hex_dump_max_size(100);
    gtpu_logger_cu.set_level(srslog::basic_levels::debug);
    gtpu_logger_cu.set_hex_dump_max_size(100);
    udp_logger_cu.set_level(srslog::basic_levels::debug);
    udp_logger_cu.set_hex_dump_max_size(100);

    logger.info("Creating F1-U connector");

    // Create UDP tester
    udp_network_gateway_config tester_config;
    tester_config.bind_address      = "127.0.0.2";
    tester_config.bind_port         = 0;
    tester_config.non_blocking_mode = true;
    udp_tester = create_udp_network_gateway({tester_config, server_data_notifier, io_tx_executor, rx_executor});
    ASSERT_TRUE(udp_tester->create_and_bind());
    std::optional<uint16_t> tester_bind_port = udp_tester->get_bind_port();
    ASSERT_TRUE(tester_bind_port.has_value());

    // create GTP-U dmux
    gtpu_demux_creation_request msg = {};
    msg.cfg.warn_on_drop            = true;
    msg.gtpu_pcap                   = &dummy_pcap;
    demux                           = create_gtpu_demux(msg);

    // create f1-u connector
    udp_network_gateway_config nru_gw_config = {};
    nru_gw_config.bind_address               = cu_gw_bind_address;
    nru_gw_config.bind_port                  = 0;
    nru_gw_config.reuse_addr                 = true;
    udp_gw = create_udp_gtpu_gateway(nru_gw_config, *epoll_broker, io_tx_executor, rx_executor);

    f1u_gw_maps.default_gws.push_back(std::move(udp_gw));
    f1u_cu_up_split_gateway_creation_msg cu_create_msg{f1u_gw_maps, *demux, dummy_pcap, tester_bind_port.value()};
    cu_gw           = create_split_f1u_gw(cu_create_msg);
    cu_gw_bind_port = cu_gw->get_bind_port();
    ASSERT_TRUE(cu_gw_bind_port.has_value());

    // prepare F1-U CU-UP bearer config
    f1u_cu_up_cfg.warn_on_drop = false;
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();

    stop_token.store(true, std::memory_order_relaxed);
    if (rx_thread.joinable()) {
      rx_thread.join();
    }
    cu_gw.reset();
    udp_gw.reset();
    udp_tester.reset();
    f1u_gw_maps.default_gws.clear();
  }

  // spawn a thread to receive data
  void start_receive_thread()
  {
    rx_thread = std::thread([this]() {
      stop_token.store(false);
      while (not stop_token.load(std::memory_order_relaxed)) {
        // call receive() on socket
        udp_tester->receive();

        std::this_thread::sleep_for(std::chrono::microseconds(100));
      }
    });
  }

  void send_to_server(byte_buffer pdu, const std::string& dest_addr, uint16_t port)
  {
    in_addr          inaddr_v4    = {};
    in6_addr         inaddr_v6    = {};
    sockaddr_storage addr_storage = {};

    if (inet_pton(AF_INET, dest_addr.c_str(), &inaddr_v4) == 1) {
      sockaddr_in* tmp = (sockaddr_in*)&addr_storage;
      tmp->sin_family  = AF_INET;
      tmp->sin_addr    = inaddr_v4;
      tmp->sin_port    = htons(port);
    } else if (inet_pton(AF_INET6, dest_addr.c_str(), &inaddr_v6) == 1) {
      sockaddr_in6* tmp = (sockaddr_in6*)&addr_storage;
      tmp->sin6_family  = AF_INET6;
      tmp->sin6_addr    = inaddr_v6;
      tmp->sin6_port    = htons(port);
    } else {
      FAIL();
    }
    udp_tester->handle_pdu(std::move(pdu), addr_storage);
    io_tx_executor.run_pending_tasks();
  }

  inline_task_executor          rx_executor;
  manual_task_worker            ue_worker{128};
  std::unique_ptr<io_broker>    epoll_broker;
  manual_task_worker            io_tx_executor{128};
  std::unique_ptr<gtpu_demux>   demux;
  std::unique_ptr<gtpu_gateway> udp_gw;
  null_dlt_pcap                 dummy_pcap  = {};
  gtpu_gateway_maps             f1u_gw_maps = {};

  // Tester UDP gw to TX/RX PDUs to F1-U CU GW
  std::unique_ptr<udp_network_gateway>              udp_tester;
  std::thread                                       rx_thread; // thread for test RX
  std::atomic<bool>                                 stop_token = {false};
  dummy_network_gateway_data_notifier_with_src_addr server_data_notifier;

  srs_cu_up::f1u_config                  f1u_cu_up_cfg;
  std::unique_ptr<f1u_cu_up_udp_gateway> cu_gw;
  std::optional<uint16_t>                cu_gw_bind_port    = 0;
  std::string                            cu_gw_bind_address = "127.0.0.1";

  srslog::basic_logger& logger         = srslog::fetch_basic_logger("TEST", false);
  srslog::basic_logger& f1u_logger_cu  = srslog::fetch_basic_logger("CU-F1-U", false);
  srslog::basic_logger& gtpu_logger_cu = srslog::fetch_basic_logger("GTPU", false);
  srslog::basic_logger& udp_logger_cu  = srslog::fetch_basic_logger("UDP-GW", false);
};

} // namespace

/// Test the instantiation of a new entity
TEST_F(f1u_cu_split_connector_test, create_new_connector)
{
  EXPECT_NE(cu_gw, nullptr);
}

TEST_F(f1u_cu_split_connector_test, send_sdu_with_dl_teid_attached)
{
  // Setup GTP-U tunnel
  up_transport_layer_info ul_tnl{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{1}};
  up_transport_layer_info dl_tnl{transport_layer_address::create_from_string("127.0.0.2"), gtpu_teid_t{2}};

  dummy_f1u_cu_up_rx_notifier cu_rx;

  std::unique_ptr<srs_cu_up::f1u_tx_pdu_notifier> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, five_qi_t{9}, f1u_cu_up_cfg, ul_tnl.gtp_teid, cu_rx, ue_worker);
  cu_gw->attach_dl_teid(ul_tnl, dl_tnl);

  ASSERT_NE(udp_tester, nullptr);
  start_receive_thread();

  expected<byte_buffer> cu_buf = make_byte_buffer("abcd");
  ASSERT_TRUE(cu_buf.has_value());

  nru_dl_message sdu = {};
  sdu.t_pdu          = cu_buf.value().deep_copy().value();

  cu_bearer->on_new_pdu(std::move(sdu));
  io_tx_executor.run_pending_tasks();

  expected<byte_buffer> exp_buf = make_byte_buffer("34ff000e00000002000000840200000000000000abcd");
  ASSERT_TRUE(exp_buf.has_value());

  expected<byte_buffer> du_rx_pdu = server_data_notifier.get_rx_pdu_blocking();
  ASSERT_TRUE(du_rx_pdu.has_value());
  ASSERT_EQ(du_rx_pdu.value().length(), exp_buf.value().length());
  ASSERT_EQ(du_rx_pdu.value(), exp_buf.value());
}

TEST_F(f1u_cu_split_connector_test, send_sdu_without_dl_teid_attached)
{
  // Setup GTP-U tunnel
  up_transport_layer_info ul_tnl{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{1}};
  up_transport_layer_info dl_tnl{transport_layer_address::create_from_string("127.0.0.2"), gtpu_teid_t{2}};

  dummy_f1u_cu_up_rx_notifier cu_rx;

  std::unique_ptr<srs_cu_up::f1u_tx_pdu_notifier> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, five_qi_t{9}, f1u_cu_up_cfg, ul_tnl.gtp_teid, cu_rx, ue_worker);
  // Not attaching DL TEID

  ASSERT_NE(udp_tester, nullptr);
  start_receive_thread();

  expected<byte_buffer> cu_buf = make_byte_buffer("abcd");
  ASSERT_TRUE(cu_buf.has_value());

  nru_dl_message sdu = {};
  sdu.t_pdu          = cu_buf.value().deep_copy().value();

  cu_bearer->on_new_pdu(std::move(sdu));
  io_tx_executor.run_pending_tasks();

  // No PDU expected
  expected<byte_buffer> du_rx_pdu = server_data_notifier.get_rx_pdu_blocking(std::chrono::milliseconds(200));
  ASSERT_FALSE(du_rx_pdu.has_value());
}

TEST_F(f1u_cu_split_connector_test, recv_sdu_with_dl_teid_attached)
{
  ASSERT_NE(udp_tester, nullptr);
  ASSERT_TRUE(cu_gw_bind_port.has_value());

  // Setup GTP-U tunnel
  up_transport_layer_info     ul_tnl{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{1}};
  up_transport_layer_info     dl_tnl{transport_layer_address::create_from_string("127.0.0.2"), gtpu_teid_t{2}};
  dummy_f1u_cu_up_rx_notifier cu_rx;

  std::unique_ptr<srs_cu_up::f1u_tx_pdu_notifier> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, five_qi_t{9}, f1u_cu_up_cfg, ul_tnl.gtp_teid, cu_rx, ue_worker);
  cu_gw->attach_dl_teid(ul_tnl, dl_tnl);

  // Send SDU
  expected<byte_buffer> du_buf = make_byte_buffer("34ff000e00000001000000840210000000000000abcd");
  ASSERT_TRUE(du_buf.has_value());
  send_to_server(std::move(du_buf.value()), "127.0.0.1", cu_gw_bind_port.value());

  // Blocking waiting for RX
  expected<nru_ul_message> rx_sdu = cu_rx.get_rx_pdu_blocking(ue_worker);
  ASSERT_TRUE(rx_sdu.has_value());
  ASSERT_TRUE(rx_sdu.value().t_pdu.has_value());

  expected<byte_buffer> exp_buf = make_byte_buffer("abcd");
  ASSERT_TRUE(exp_buf.has_value());
  ASSERT_EQ(rx_sdu.value().t_pdu->length(), exp_buf.value().length());
  ASSERT_EQ(rx_sdu.value().t_pdu.value(), exp_buf.value());
}

TEST_F(f1u_cu_split_connector_test, recv_sdu_without_dl_teid_attached)
{
  ASSERT_NE(udp_tester, nullptr);
  ASSERT_TRUE(cu_gw_bind_port.has_value());

  // Setup GTP-U tunnel
  up_transport_layer_info     ul_tnl{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{1}};
  up_transport_layer_info     dl_tnl{transport_layer_address::create_from_string("127.0.0.2"), gtpu_teid_t{2}};
  dummy_f1u_cu_up_rx_notifier cu_rx;

  std::unique_ptr<srs_cu_up::f1u_tx_pdu_notifier> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, five_qi_t{9}, f1u_cu_up_cfg, ul_tnl.gtp_teid, cu_rx, ue_worker);
  // Not attaching DL TEID

  // Send SDU
  expected<byte_buffer> du_buf = make_byte_buffer("34ff000e00000001000000840210000000000000abcd");
  ASSERT_TRUE(du_buf.has_value());
  send_to_server(std::move(du_buf.value()), "127.0.0.1", cu_gw_bind_port.value());

  // Blocking waiting for RX
  expected<nru_ul_message> rx_sdu = cu_rx.get_rx_pdu_blocking(ue_worker);
  ASSERT_TRUE(rx_sdu.has_value());
  ASSERT_TRUE(rx_sdu.value().t_pdu.has_value());

  expected<byte_buffer> exp_buf = make_byte_buffer("abcd");
  ASSERT_TRUE(exp_buf.has_value());
  ASSERT_EQ(rx_sdu.value().t_pdu->length(), exp_buf.value().length());
  ASSERT_EQ(rx_sdu.value().t_pdu.value(), exp_buf.value());
}

TEST_F(f1u_cu_split_connector_test, disconnect_stops_tx)
{
  // Setup GTP-U tunnel
  up_transport_layer_info ul_tnl{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{1}};
  up_transport_layer_info dl_tnl{transport_layer_address::create_from_string("127.0.0.2"), gtpu_teid_t{2}};

  dummy_f1u_cu_up_rx_notifier cu_rx;

  std::unique_ptr<srs_cu_up::f1u_tx_pdu_notifier> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, five_qi_t{9}, f1u_cu_up_cfg, ul_tnl.gtp_teid, cu_rx, ue_worker);
  cu_gw->attach_dl_teid(ul_tnl, dl_tnl);

  ASSERT_NE(udp_tester, nullptr);
  start_receive_thread();

  // Disconnect incorrect tunnel (no effect expected)
  cu_gw->disconnect_cu_bearer(
      up_transport_layer_info{transport_layer_address::create_from_string("127.0.0.2"), gtpu_teid_t{7}});

  expected<byte_buffer> cu_buf1 = make_byte_buffer("abcd");
  ASSERT_TRUE(cu_buf1.has_value());

  nru_dl_message sdu1 = {};
  sdu1.t_pdu          = cu_buf1.value().deep_copy().value();

  cu_bearer->on_new_pdu(std::move(sdu1));
  io_tx_executor.run_pending_tasks();

  expected<byte_buffer> exp_buf = make_byte_buffer("34ff000e00000002000000840200000000000000abcd");
  ASSERT_TRUE(exp_buf.has_value());

  expected<byte_buffer> du_rx_pdu = server_data_notifier.get_rx_pdu_blocking();
  ASSERT_TRUE(du_rx_pdu.has_value());
  ASSERT_EQ(du_rx_pdu.value().length(), exp_buf.value().length());
  ASSERT_EQ(du_rx_pdu.value(), exp_buf.value());

  // Disconnect correct tunnel explicitly (not via deleting cu_bearer because we need it to handle sdu2)
  cu_gw->disconnect_cu_bearer(ul_tnl);

  // Send another SDU
  expected<byte_buffer> cu_buf2 = make_byte_buffer("bcde");
  ASSERT_TRUE(cu_buf2.has_value());

  nru_dl_message sdu2 = {};
  sdu2.t_pdu          = cu_buf2.value().deep_copy().value();

  cu_bearer->on_new_pdu(std::move(sdu2));
  io_tx_executor.run_pending_tasks();

  // No PDU expected
  expected<byte_buffer> du_rx_pdu2 = server_data_notifier.get_rx_pdu_blocking(std::chrono::milliseconds(200));
  ASSERT_FALSE(du_rx_pdu2.has_value());

  // Destructor of cu_bearer tries to disconnect tunnel again, hence we see a warning.
}

TEST_F(f1u_cu_split_connector_test, destroy_bearer_disconnects_and_stops_rx)
{
  ASSERT_NE(udp_tester, nullptr);
  ASSERT_TRUE(cu_gw_bind_port.has_value());

  // Setup GTP-U tunnel
  up_transport_layer_info     ul_tnl{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{1}};
  up_transport_layer_info     dl_tnl{transport_layer_address::create_from_string("127.0.0.2"), gtpu_teid_t{2}};
  dummy_f1u_cu_up_rx_notifier cu_rx;

  std::unique_ptr<srs_cu_up::f1u_tx_pdu_notifier> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, five_qi_t{9}, f1u_cu_up_cfg, ul_tnl.gtp_teid, cu_rx, ue_worker);
  cu_gw->attach_dl_teid(ul_tnl, dl_tnl);

  // Disconnect incorrect tunnel (no effect expected)
  cu_gw->disconnect_cu_bearer(
      up_transport_layer_info{transport_layer_address::create_from_string("127.0.0.2"), gtpu_teid_t{7}});

  // Send SDU
  expected<byte_buffer> du_buf1 = make_byte_buffer("34ff000e00000001000000840210000000000000abcd");
  ASSERT_TRUE(du_buf1.has_value());
  send_to_server(std::move(du_buf1.value()), "127.0.0.1", cu_gw_bind_port.value());

  // Blocking waiting for RX
  expected<nru_ul_message> rx_sdu1 = cu_rx.get_rx_pdu_blocking(ue_worker);
  ASSERT_TRUE(rx_sdu1.has_value());
  ASSERT_TRUE(rx_sdu1.value().t_pdu.has_value());

  expected<byte_buffer> exp_buf1 = make_byte_buffer("abcd");
  ASSERT_TRUE(exp_buf1.has_value());
  ASSERT_EQ(rx_sdu1.value().t_pdu->length(), exp_buf1.value().length());
  ASSERT_EQ(rx_sdu1.value().t_pdu.value(), exp_buf1.value());

  // Disconnect correct tunnel by deleting the cu_bearer (which should deregister from connector upon destruction)
  cu_bearer.reset();

  // Send another SDU
  expected<byte_buffer> du_buf2 = make_byte_buffer("34ff000e00000001000000840210000000000000bcde");
  ASSERT_TRUE(du_buf2.has_value());
  send_to_server(std::move(du_buf2.value()), "127.0.0.1", cu_gw_bind_port.value());

  // Blocking waiting for RX
  expected<nru_ul_message> rx_sdu2 = cu_rx.get_rx_pdu_blocking(ue_worker, std::chrono::milliseconds(200));
  ASSERT_FALSE(rx_sdu2.has_value());
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
