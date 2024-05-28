/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "tests/unittests/gateways/test_helpers.h"
#include "srsran/f1u/cu_up/split_connector/f1u_split_connector.h"
#include "srsran/gateways/udp_network_gateway_factory.h"
#include "srsran/gtpu/gtpu_config.h"
#include "srsran/gtpu/gtpu_demux_factory.h"
#include "srsran/srslog/srslog.h"
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
                                               std::chrono::milliseconds timeout_ms = std::chrono::milliseconds(10))
  {
    // wait until at least one PDU is received
    std::unique_lock<std::mutex> lock(rx_mutex);
    for (int i = 0; i < 100; i++) {
      if (!rx_cvar.wait_for(lock, timeout_ms, [this]() { return !msg_queue.empty(); })) {
        if (not msg_queue.empty()) {
          break;
        }
        rx_mutex.unlock();
        ue_worker.run_pending_tasks(); // unlock so we can push into the message queue
        rx_mutex.lock();
      }
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
    udp_tester                      = create_udp_network_gateway({tester_config, server_data_notifier, io_tx_executor});
    ASSERT_TRUE(udp_tester->create_and_bind());
    std::optional<uint16_t> tester_bind_port = udp_tester->get_bind_port();
    ASSERT_TRUE(tester_bind_port.has_value());

    // create GTP-U dmux
    gtpu_demux_creation_request msg = {};
    msg.cfg.warn_on_drop            = true;
    msg.gtpu_pcap                   = &dummy_pcap;
    demux                           = create_gtpu_demux(msg);

    // create f1-u connector
    udp_network_gateway_config ngu_gw_config = {};
    ngu_gw_config.bind_address               = "127.0.0.1";
    ngu_gw_config.bind_port                  = 0;
    ngu_gw_config.reuse_addr                 = true;
    udp_gw = srs_cu_up::create_udp_ngu_gateway(ngu_gw_config, *epoll_broker, io_tx_executor);
    cu_gw  = std::make_unique<f1u_split_connector>(udp_gw.get(), demux.get(), dummy_pcap, tester_bind_port.value());
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
  }

  manual_task_worker           ue_worker{128};
  std::unique_ptr<io_broker>   epoll_broker;
  manual_task_worker           io_tx_executor{128};
  std::unique_ptr<gtpu_demux>  demux;
  std::unique_ptr<ngu_gateway> udp_gw;
  null_dlt_pcap                dummy_pcap = {};

  // Tester UDP gw to TX/RX PDUs to F1-U CU GW
  std::unique_ptr<udp_network_gateway>              udp_tester;
  std::thread                                       rx_thread; // thread for test RX
  std::atomic<bool>                                 stop_token = {false};
  dummy_network_gateway_data_notifier_with_src_addr server_data_notifier;

  srs_cu_up::f1u_config                f1u_cu_up_cfg;
  std::unique_ptr<f1u_split_connector> cu_gw;
  std::optional<uint16_t>              cu_gw_bind_port = 0;

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

TEST_F(f1u_cu_split_connector_test, send_sdu)
{
  // Setup GTP-U tunnel
  up_transport_layer_info ul_tnl{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{1}};
  up_transport_layer_info dl_tnl{transport_layer_address::create_from_string("127.0.0.2"), gtpu_teid_t{2}};

  dummy_f1u_cu_up_rx_notifier cu_rx;

  std::unique_ptr<srs_cu_up::f1u_tx_pdu_notifier> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, f1u_cu_up_cfg, ul_tnl, cu_rx, ue_worker);
  cu_gw->attach_dl_teid(ul_tnl, dl_tnl);

  ASSERT_NE(udp_tester, nullptr);
  start_receive_thread();

  expected<byte_buffer> cu_buf = make_byte_buffer("abcd");
  ASSERT_TRUE(cu_buf.has_value());

  nru_dl_message sdu = {};
  sdu.pdcp_sn        = 0;
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

TEST_F(f1u_cu_split_connector_test, recv_sdu)
{
  // Create UDP tester for sending PDU
  udp_network_gateway_config server_config;
  server_config.bind_address      = "127.0.0.2";
  server_config.bind_port         = GTPU_PORT;
  server_config.non_blocking_mode = true;

  ASSERT_NE(udp_tester, nullptr);
  ASSERT_TRUE(cu_gw_bind_port.has_value());

  // Setup GTP-U tunnel
  up_transport_layer_info     ul_tnl{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{1}};
  up_transport_layer_info     dl_tnl{transport_layer_address::create_from_string("127.0.0.2"), gtpu_teid_t{2}};
  dummy_f1u_cu_up_rx_notifier cu_rx;

  std::unique_ptr<srs_cu_up::f1u_tx_pdu_notifier> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, f1u_cu_up_cfg, ul_tnl, cu_rx, ue_worker);
  cu_gw->attach_dl_teid(ul_tnl, dl_tnl);

  // Send SDU
  expected<byte_buffer> du_buf = make_byte_buffer("34ff000e00000001000000840210000000000000abcd");
  ASSERT_TRUE(du_buf.has_value());
  send_to_server(std::move(du_buf.value()), "127.0.0.1", cu_gw_bind_port.value());

  // Blocking waiting for RX
  expected<nru_ul_message> rx_sdu = cu_rx.get_rx_pdu_blocking(ue_worker);
  ASSERT_FALSE(rx_sdu.is_error());
  ASSERT_TRUE(rx_sdu.value().t_pdu.has_value());

  expected<byte_buffer> exp_buf = make_byte_buffer("abcd");
  ASSERT_FALSE(exp_buf.is_error());
  ASSERT_EQ(rx_sdu.value().t_pdu->length(), exp_buf.value().length());
  ASSERT_EQ(rx_sdu.value().t_pdu.value(), exp_buf.value());
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}