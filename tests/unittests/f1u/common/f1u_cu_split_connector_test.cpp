/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/f1u/cu_up/split_connector/f1u_split_connector.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/io/io_broker_factory.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_up;

namespace {

// dummy CU-UP RX bearer interface
struct dummy_f1u_cu_up_rx_notifier final : public f1u_cu_up_gateway_bearer_rx_notifier {
  void on_new_pdu(nru_ul_message msg) override
  {
    logger.info(msg.t_pdu->begin(), msg.t_pdu->end(), "CU-UP received SDU.");
    last_sdu = std::move(msg);
  }
  nru_ul_message        last_sdu;
  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-F1-U", false);
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
    f1u_logger_du.set_level(srslog::basic_levels::debug);
    f1u_logger_cu.set_hex_dump_max_size(100);
    f1u_logger_du.set_hex_dump_max_size(100);

    logger.info("Creating F1-U connector");

    // create f1-u connector
    udp_network_gateway_config ngu_gw_config = {};
    ngu_gw_config.bind_address               = "127.0.0.1";
    ngu_gw_config.bind_port                  = 0;
    ngu_gw_config.reuse_addr                 = true;
    std::unique_ptr<ngu_gateway> udp_gw = srs_cu_up::create_udp_ngu_gateway(ngu_gw_config, *epoll_broker, *executor);
    cu_gw                               = std::make_unique<f1u_split_connector>(udp_gw.get());

    timers = timer_factory{timer_mng, ue_worker};

    ue_inactivity_timer = timers.create_timer();

    // prepare F1-U CU-UP bearer config
    f1u_cu_up_cfg.warn_on_drop = false;
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  timer_manager                  timer_mng;
  manual_task_worker             ue_worker{128};
  timer_factory                  timers;
  unique_timer                   ue_inactivity_timer;
  std::unique_ptr<io_broker>     epoll_broker;
  std::unique_ptr<task_executor> executor;

  srs_cu_up::f1u_config                f1u_cu_up_cfg;
  std::unique_ptr<f1u_split_connector> cu_gw;
  srslog::basic_logger&                logger        = srslog::fetch_basic_logger("TEST", false);
  srslog::basic_logger&                f1u_logger_cu = srslog::fetch_basic_logger("CU-F1-U", false);
  srslog::basic_logger&                f1u_logger_du = srslog::fetch_basic_logger("DU-F1-U", false);
};
} // namespace

/// Test the instantiation of a new entity
TEST_F(f1u_cu_split_connector_test, create_new_connector)
{
  EXPECT_NE(cu_gw, nullptr);
}

TEST_F(f1u_cu_split_connector_test, send_sdu)
{
  up_transport_layer_info ul_tnl{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{1}};

  dummy_f1u_cu_up_rx_notifier cu_rx;

  std::unique_ptr<srs_cu_up::f1u_tx_pdu_notifier> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, f1u_cu_up_cfg, ul_tnl, cu_rx, ue_worker, timers, ue_inactivity_timer);

  expected<byte_buffer> cu_buf = make_byte_buffer("ABCD");
  ASSERT_TRUE(cu_buf.has_value());

  nru_dl_message sdu = {};
  sdu.pdcp_sn        = 0;
  sdu.t_pdu          = cu_buf.value().deep_copy().value();

  cu_bearer->on_new_pdu(std::move(sdu));
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
