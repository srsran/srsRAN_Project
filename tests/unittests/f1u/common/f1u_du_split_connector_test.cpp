/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../..//gateways/test_helpers.h"
#include "srsran/f1u/du/split_connector/f1u_split_connector.h"
#include "srsran/gateways/udp_network_gateway_factory.h"
#include "srsran/gtpu/gtpu_config.h"
#include "srsran/gtpu/gtpu_demux_factory.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/io/io_broker_factory.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

namespace {

/// Fixture class for F1-U connector tests.
/// It requires TEST_F() to create/spawn tests
class f1u_du_split_connector_test : public ::testing::Test
{
public:
  f1u_du_split_connector_test() { epoll_broker = create_io_broker(io_broker_type::epoll); }

protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init logger
    f1u_logger_du.set_level(srslog::basic_levels::debug);
    f1u_logger_du.set_hex_dump_max_size(100);
    gtpu_logger_du.set_level(srslog::basic_levels::debug);
    gtpu_logger_du.set_hex_dump_max_size(100);
    udp_logger_du.set_level(srslog::basic_levels::debug);
    udp_logger_du.set_hex_dump_max_size(100);

    logger.info("Creating F1-U connector");
    // create GTP-U dmux
    //
    gtpu_demux_creation_request msg = {};
    msg.cfg.warn_on_drop            = true;
    msg.gtpu_pcap                   = &dummy_pcap;
    demux                           = create_gtpu_demux(msg);

    // create f1-u connector
    udp_network_gateway_config ngu_gw_config = {};
    ngu_gw_config.bind_address               = "127.0.0.1";
    ngu_gw_config.bind_port                  = GTPU_PORT;
    ngu_gw_config.reuse_addr                 = true;
    udp_gw = srs_cu_up::create_udp_ngu_gateway(ngu_gw_config, *epoll_broker, io_tx_executor);
    du_gw  = std::make_unique<f1u_split_connector>(udp_gw.get(), demux.get(), dummy_pcap);

    timers = timer_factory{timer_mng, ue_worker};

    ue_inactivity_timer = timers.create_timer();

    // prepare F1-U CU-UP bearer config
    f1u_du_cfg.warn_on_drop = false;
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();

    stop_token.store(true, std::memory_order_relaxed);
    if (rx_thread.joinable()) {
      rx_thread.join();
    }
  }

  timer_manager                           timer_mng;
  manual_task_worker                      ue_worker{128};
  timer_factory                           timers;
  unique_timer                            ue_inactivity_timer;
  std::unique_ptr<io_broker>              epoll_broker;
  manual_task_worker                      io_tx_executor{128};
  std::unique_ptr<gtpu_demux>             demux;
  std::unique_ptr<srs_cu_up::ngu_gateway> udp_gw;
  null_dlt_pcap                           dummy_pcap = {};

  // Tester UDP gw to TX/RX PDUs to F1-U CU GW
  std::unique_ptr<udp_network_gateway>              udp_tester;
  std::thread                                       rx_thread; // thread for test RX
  std::atomic<bool>                                 stop_token = {false};
  dummy_network_gateway_data_notifier_with_src_addr server_data_notifier;

  f1u_config                           f1u_du_cfg;
  std::unique_ptr<f1u_split_connector> du_gw;

  srslog::basic_logger& logger         = srslog::fetch_basic_logger("TEST", false);
  srslog::basic_logger& f1u_logger_du  = srslog::fetch_basic_logger("CU-F1-U", false);
  srslog::basic_logger& gtpu_logger_du = srslog::fetch_basic_logger("GTPU", false);
  srslog::basic_logger& udp_logger_du  = srslog::fetch_basic_logger("UDP-GW", false);
};
} // namespace

/// Test the instantiation of a new entity
TEST_F(f1u_du_split_connector_test, create_new_connector)
{
  EXPECT_NE(du_gw, nullptr);
}
/*
TEST_F(f1u_cu_split_connector_test, send_sdu)
{
  // Setup GTP-U tunnel
  up_transport_layer_info ul_tnl{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{1}};
  up_transport_layer_info dl_tnl{transport_layer_address::create_from_string("127.0.0.2"), gtpu_teid_t{2}};

  dummy_f1u_cu_up_rx_notifier cu_rx;

  std::unique_ptr<srs_cu_up::f1u_tx_pdu_notifier> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, f1u_cu_up_cfg, ul_tnl, cu_rx, ue_worker, timers, ue_inactivity_timer);
  cu_gw->attach_dl_teid(ul_tnl, dl_tnl);

  // Create UDP tester and spawn RX thread
  udp_network_gateway_config server_config;
  server_config.bind_address      = "127.0.0.2";
  server_config.bind_port         = GTPU_PORT;
  server_config.non_blocking_mode = true;

  // create and bind gateways
  udp_tester = create_udp_network_gateway({server_config, server_data_notifier, io_tx_executor});
  ASSERT_NE(udp_tester, nullptr);
  ASSERT_TRUE(udp_tester->create_and_bind());
  start_receive_thread();

  byte_buffer cu_buf = make_byte_buffer("ABCD");

  nru_dl_message sdu = {};
  sdu.pdcp_sn        = 0;
  sdu.t_pdu          = cu_buf.deep_copy().value();

  cu_bearer->on_new_pdu(std::move(sdu));
  io_tx_executor.run_pending_tasks();
}

TEST_F(f1u_cu_split_connector_test, recv_sdu)
{
  // Create UDP tester for sending PDU
  udp_network_gateway_config server_config;
  server_config.bind_address      = "127.0.0.2";
  server_config.bind_port         = GTPU_PORT;
  server_config.non_blocking_mode = true;

  udp_tester = create_udp_network_gateway({server_config, server_data_notifier, io_tx_executor});
  ASSERT_NE(udp_tester, nullptr);
  ASSERT_TRUE(udp_tester->create_and_bind());

  // Setup GTP-U tunnel
  up_transport_layer_info     ul_tnl{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{1}};
  up_transport_layer_info     dl_tnl{transport_layer_address::create_from_string("127.0.0.2"), gtpu_teid_t{2}};
  dummy_f1u_cu_up_rx_notifier cu_rx;

  std::unique_ptr<srs_cu_up::f1u_tx_pdu_notifier> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, f1u_cu_up_cfg, ul_tnl, cu_rx, ue_worker, timers, ue_inactivity_timer);
  cu_gw->attach_dl_teid(ul_tnl, dl_tnl);

  // Send SDU
  byte_buffer du_buf = make_byte_buffer("34ff000e00000001000000840210000000000000abcd");
  send_to_server(std::move(du_buf), "127.0.0.1", GTPU_PORT);

  usleep(10000);
  ue_worker.run_pending_tasks();

  // Blocking waiting for RX
  expected<nru_ul_message> rx_sdu = cu_rx.get_rx_pdu_blocking();
}
*/

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
