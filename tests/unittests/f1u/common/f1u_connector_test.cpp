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

#include "srsran/f1u/du/f1u_rx_sdu_notifier.h"
#include "srsran/f1u/local_connector/f1u_local_connector.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;

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

struct dummy_f1u_du_gateway_bearer_rx_notifier final : srsran::srs_du::f1u_du_gateway_bearer_rx_notifier {
  void on_new_pdu(nru_dl_message msg) override
  {
    logger.info(msg.t_pdu.begin(), msg.t_pdu.end(), "DU received SDU. sdu_len={}", msg.t_pdu.length());
    last_sdu = std::move(msg);
  }
  nru_dl_message last_sdu;

private:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-F1-U", false);
};

// dummy DU RX bearer interface
struct dummy_f1u_du_rx_sdu_notifier final : public srs_du::f1u_rx_sdu_notifier {
  void on_new_sdu(byte_buffer sdu, bool is_retx) override
  {
    logger.info(sdu.begin(), sdu.end(), "DU received SDU. sdu_len={} is_retx={}", sdu.length(), is_retx);
    last_sdu         = std::move(sdu);
    last_sdu_is_retx = is_retx;
  }
  void        on_discard_sdu(uint32_t pdcp_sn) override {}
  byte_buffer last_sdu;
  bool        last_sdu_is_retx;

private:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-F1-U", false);
};

/// Fixture class for F1-U connector tests.
/// It requires TEST_F() to create/spawn tests
class f1u_connector_test : public ::testing::Test
{
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
    f1u_conn = std::make_unique<f1u_local_connector>();

    // prepare F1-U DU bearer config
    f1u_du_config.t_notify     = 10;
    f1u_du_config.warn_on_drop = true;

    // prepare F1-U CU-UP bearer config
    f1u_cu_up_cfg.warn_on_drop = false;
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  timer_manager      timer_mng;
  manual_task_worker ue_worker{128};
  timer_factory      timers;
  unique_timer       ue_inactivity_timer;

  srs_du::f1u_config                   f1u_du_config;
  srs_cu_up::f1u_config                f1u_cu_up_cfg;
  std::unique_ptr<f1u_local_connector> f1u_conn;
  srslog::basic_logger&                logger        = srslog::fetch_basic_logger("TEST", false);
  srslog::basic_logger&                f1u_logger_cu = srslog::fetch_basic_logger("CU-F1-U", false);
  srslog::basic_logger&                f1u_logger_du = srslog::fetch_basic_logger("DU-F1-U", false);
};

} // namespace

// Helper function to check CU-UP -> DU path
void check_dl_path_connected(const byte_buffer&                       cu_buf,
                             srs_cu_up::f1u_tx_pdu_notifier*          cu_bearer,
                             dummy_f1u_du_gateway_bearer_rx_notifier& du_rx)
{
  auto du_exp_buf = byte_buffer_chain::create(cu_buf.deep_copy().value());
  ASSERT_TRUE(du_exp_buf.has_value());
  nru_dl_message du_exp = {};
  du_exp.t_pdu          = cu_buf.deep_copy().value();
  nru_dl_message sdu    = {};
  sdu.t_pdu             = cu_buf.deep_copy().value();
  cu_bearer->on_new_pdu(std::move(sdu));
  ASSERT_EQ(du_rx.last_sdu, du_exp); // should arrive immediately
}

// Helper function to check DU-> CU-UP path
void check_ul_path_connected(const byte_buffer&           du_buf,
                             srs_du::f1u_tx_pdu_notifier* du_bearer,
                             dummy_f1u_cu_up_rx_notifier& cu_rx)
{
  nru_ul_message sdu          = {};
  auto           du_chain_buf = byte_buffer_chain::create(du_buf.deep_copy().value());
  sdu.t_pdu                   = std::move(du_chain_buf.value());

  nru_ul_message cu_exp        = {};
  auto           du_chain_buf2 = byte_buffer_chain::create(du_buf.deep_copy().value());
  cu_exp.t_pdu                 = std::move(du_chain_buf2.value());

  du_bearer->on_new_pdu(std::move(sdu));

  // TODO revisit executor transfer
  // ASSERT_NE(cu_rx.last_sdu, cu_exp); // should not yet arrive (due to defer)
  // ue_worker.run_pending_tasks();

  ASSERT_EQ(cu_rx.last_sdu, cu_exp); // now it should arrive
}

// Helper function to check DU-> CU-UP path is disconnected
void check_dl_path_disconnected(const byte_buffer&                       cu_buf,
                                srs_cu_up::f1u_tx_pdu_notifier*          cu_bearer,
                                dummy_f1u_du_gateway_bearer_rx_notifier& du_rx)
{
  nru_dl_message sdu = {};
  sdu.t_pdu          = cu_buf.deep_copy().value();

  nru_dl_message cu_not_exp = {};
  cu_not_exp.t_pdu          = cu_buf.deep_copy().value();

  ASSERT_NE(du_rx.last_sdu, sdu); // Make sure last RX SDU is not equal to test SDU
                                  // Test cannot be done acuratly if it is already the same.

  cu_bearer->on_new_pdu(std::move(sdu));
  ASSERT_NE(du_rx.last_sdu, cu_not_exp); // Last SDU should not have changed
}

expected<nru_ul_message> make_nru_ul_message(const byte_buffer& du_buf)
{
  expected<byte_buffer> buf_cpy = du_buf.deep_copy();
  if (not buf_cpy.has_value()) {
    return make_unexpected(default_error_t{});
  }

  expected<byte_buffer_chain> du_chain_buf = byte_buffer_chain::create(std::move(buf_cpy.value()));
  if (not du_chain_buf.has_value()) {
    return make_unexpected(default_error_t{});
  }

  nru_ul_message msg = {};
  msg.t_pdu          = std::move(du_chain_buf.value());
  return msg;
}

// Helper function to check DU-> CU-UP path is disconnected
void check_ul_path_disconnected(const byte_buffer&           du_buf,
                                srs_du::f1u_tx_pdu_notifier* du_bearer,
                                dummy_f1u_cu_up_rx_notifier& cu_rx)
{
  srslog::basic_logger&    logger = srslog::fetch_basic_logger("TEST", false);
  expected<nru_ul_message> sdu    = make_nru_ul_message(du_buf);
  ASSERT_TRUE(sdu.has_value());

  expected<nru_ul_message> cu_not_exp = make_nru_ul_message(du_buf);
  ASSERT_TRUE(cu_not_exp.has_value());

  // logger.info("Testing equallity last_sdu T-PDU = {}", *sdu.value().t_pdu->begin());
  logger.info("Testing equallity last_sdu T-PDU = {}", cu_not_exp.value() == sdu.value());

  ASSERT_NE(cu_rx.last_sdu, sdu.value()); // Make sure last SDU is not equal to test SDU
                                          //   Test cannot be done acuratly if it is already the same.

  du_bearer->on_new_pdu(std::move(sdu.value()));
  ASSERT_NE(cu_rx.last_sdu, cu_not_exp.value()); // Last SDU should not have changed
}

/// Test the instantiation of a new entity
TEST_F(f1u_connector_test, create_new_connector)
{
  EXPECT_NE(f1u_conn, nullptr);
  EXPECT_NE(f1u_conn->get_f1u_du_gateway(), nullptr);
  EXPECT_NE(f1u_conn->get_f1u_cu_up_gateway(), nullptr);
}

/// Test attaching/detaching F1-U bearer at CU-UP and DU and UL DL flow
TEST_F(f1u_connector_test, ul_dl_flow)
{
  f1u_cu_up_gateway*      cu_gw = f1u_conn->get_f1u_cu_up_gateway();
  srs_du::f1u_du_gateway* du_gw = f1u_conn->get_f1u_du_gateway();

  up_transport_layer_info ul_tnl{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{1}};
  up_transport_layer_info dl_tnl{transport_layer_address::create_from_string("127.0.0.2"), gtpu_teid_t{2}};

  // Create CU TX notifier adapter
  dummy_f1u_cu_up_rx_notifier cu_rx;

  std::unique_ptr<srs_cu_up::f1u_tx_pdu_notifier> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, f1u_cu_up_cfg, ul_tnl, cu_rx, ue_worker);

  // Create DU TX notifier adapter and RX handler
  dummy_f1u_du_gateway_bearer_rx_notifier      du_rx;
  std::unique_ptr<srs_du::f1u_tx_pdu_notifier> du_bearer =
      du_gw->create_du_bearer(0, drb_id_t::drb1, f1u_du_config, dl_tnl, ul_tnl, du_rx, timers, ue_worker);

  // Create CU RX handler and attach it to the DU TX
  cu_gw->attach_dl_teid(ul_tnl, dl_tnl);

  // Check CU-UP -> DU path
  byte_buffer cu_buf = make_byte_buffer("dead").value();
  check_dl_path_connected(cu_buf.deep_copy().value(), cu_bearer.get(), du_rx);

  // Check DU-> CU-UP path
  byte_buffer du_buf = make_byte_buffer("dbee").value();
  check_ul_path_connected(du_buf.deep_copy().value(), du_bearer.get(), cu_rx);
}

/// Test destryoing F1-U bearer at CU-UP and pushing traffic at DU
TEST_F(f1u_connector_test, destroy_bearer_cu_up)
{
  f1u_cu_up_gateway*      cu_gw = f1u_conn->get_f1u_cu_up_gateway();
  srs_du::f1u_du_gateway* du_gw = f1u_conn->get_f1u_du_gateway();

  up_transport_layer_info ul_tnl{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{1}};
  up_transport_layer_info dl_tnl{transport_layer_address::create_from_string("127.0.0.2"), gtpu_teid_t{2}};

  // Create CU TX notifier adapter
  dummy_f1u_cu_up_rx_notifier cu_rx;

  std::unique_ptr<srs_cu_up::f1u_tx_pdu_notifier> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, f1u_cu_up_cfg, ul_tnl, cu_rx, ue_worker);

  // Create DU TX notifier adapter and RX handler
  dummy_f1u_du_gateway_bearer_rx_notifier      du_rx;
  std::unique_ptr<srs_du::f1u_tx_pdu_notifier> du_bearer =
      du_gw->create_du_bearer(0, drb_id_t::drb1, f1u_du_config, dl_tnl, ul_tnl, du_rx, timers, ue_worker);

  // Create CU RX handler and attach it to the DU TX
  cu_gw->attach_dl_teid(ul_tnl, dl_tnl);

  // Check CU-UP -> DU path
  byte_buffer cu_buf = make_byte_buffer("dead").value();
  check_dl_path_connected(cu_buf.deep_copy().value(), cu_bearer.get(), du_rx);

  // Check DU-> CU-UP path
  byte_buffer du_buf = make_byte_buffer("dbee").value();
  check_ul_path_connected(du_buf.deep_copy().value(), du_bearer.get(), cu_rx);

  // Deleting CU bearer will disconnect from connector
  cu_bearer.reset();

  // Check DU-> CU-UP path is properly detached
  byte_buffer du_buf2 = make_byte_buffer("fdea").value();
  check_ul_path_disconnected(du_buf2.deep_copy().value(), du_bearer.get(), cu_rx);
}

/// Test disconnecting bearer at CU-UP and pushing traffic UL and DL
TEST_F(f1u_connector_test, disconnect_bearer_cu_up)
{
  f1u_cu_up_gateway*      cu_gw = f1u_conn->get_f1u_cu_up_gateway();
  srs_du::f1u_du_gateway* du_gw = f1u_conn->get_f1u_du_gateway();

  up_transport_layer_info ul_tnl{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{1}};
  up_transport_layer_info dl_tnl{transport_layer_address::create_from_string("127.0.0.2"), gtpu_teid_t{2}};

  // Create CU TX notifier adapter
  dummy_f1u_cu_up_rx_notifier cu_rx;

  std::unique_ptr<srs_cu_up::f1u_tx_pdu_notifier> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, f1u_cu_up_cfg, ul_tnl, cu_rx, ue_worker);

  // Create DU TX notifier adapter and RX handler
  dummy_f1u_du_gateway_bearer_rx_notifier      du_rx;
  std::unique_ptr<srs_du::f1u_tx_pdu_notifier> du_bearer =
      du_gw->create_du_bearer(0, drb_id_t::drb1, f1u_du_config, dl_tnl, ul_tnl, du_rx, timers, ue_worker);

  // Create CU RX handler and attach it to the DU TX
  cu_gw->attach_dl_teid(ul_tnl, dl_tnl);

  // Check CU-UP -> DU path
  byte_buffer cu_buf = make_byte_buffer("dddd").value();
  check_dl_path_connected(cu_buf.deep_copy().value(), cu_bearer.get(), du_rx);

  // Check DU-> CU-UP path
  byte_buffer du_buf = make_byte_buffer("bbbb").value();
  check_ul_path_connected(du_buf.deep_copy().value(), du_bearer.get(), cu_rx);

  // Disconnect CU bearer without destryoing it from connector
  cu_gw->disconnect_cu_bearer(ul_tnl);

  // Check CU-UP -> DU path is properly detached
  byte_buffer cu_buf2 = make_byte_buffer("DEAD").value();
  check_dl_path_disconnected(cu_buf2.deep_copy().value(), cu_bearer.get(), du_rx);

  // Check DU-> CU-UP path is properly detached
  byte_buffer du_buf2 = make_byte_buffer("BEEF").value();
  check_ul_path_disconnected(du_buf2.deep_copy().value(), du_bearer.get(), cu_rx);
}

/// Test destryoing F1-U bearer at DU and pushing traffic at CU-UP
TEST_F(f1u_connector_test, destroy_bearer_du)
{
  f1u_cu_up_gateway*      cu_gw = f1u_conn->get_f1u_cu_up_gateway();
  srs_du::f1u_du_gateway* du_gw = f1u_conn->get_f1u_du_gateway();

  up_transport_layer_info ul_tnl{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{1}};
  up_transport_layer_info dl_tnl{transport_layer_address::create_from_string("127.0.0.2"), gtpu_teid_t{2}};

  // Create CU TX notifier adapter
  dummy_f1u_cu_up_rx_notifier                     cu_rx;
  std::unique_ptr<srs_cu_up::f1u_tx_pdu_notifier> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, f1u_cu_up_cfg, ul_tnl, cu_rx, ue_worker);

  // Create DU TX notifier adapter and RX handler
  dummy_f1u_du_gateway_bearer_rx_notifier      du_rx;
  std::unique_ptr<srs_du::f1u_tx_pdu_notifier> du_bearer =
      du_gw->create_du_bearer(0, drb_id_t::drb1, f1u_du_config, dl_tnl, ul_tnl, du_rx, timers, ue_worker);

  // Create CU RX handler and attach it to the DU TX
  cu_gw->attach_dl_teid(ul_tnl, dl_tnl);

  // Check CU-UP -> DU path
  byte_buffer cu_buf = make_byte_buffer("dead").value();
  check_dl_path_connected(cu_buf.deep_copy().value(), cu_bearer.get(), du_rx);

  // Check DU-> CU-UP path
  byte_buffer du_buf = make_byte_buffer("dbee").value();
  check_ul_path_connected(du_buf.deep_copy().value(), du_bearer.get(), cu_rx);

  // Delete DU bearer
  du_bearer.reset();

  // Check CU-UP -> DU path is properly detached
  byte_buffer cu_buf2 = make_byte_buffer("DEAD").value();
  check_dl_path_connected(cu_buf.deep_copy().value(), cu_bearer.get(), du_rx);
}

/// Test destryoing F1-U bearer at DU and pushing traffic at CU-UP
TEST_F(f1u_connector_test, disconnect_bearer_du)
{
  f1u_cu_up_gateway*      cu_gw = f1u_conn->get_f1u_cu_up_gateway();
  srs_du::f1u_du_gateway* du_gw = f1u_conn->get_f1u_du_gateway();

  up_transport_layer_info ul_tnl{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{1}};
  up_transport_layer_info dl_tnl{transport_layer_address::create_from_string("127.0.0.2"), gtpu_teid_t{2}};

  // Create CU TX notifier adapter
  dummy_f1u_cu_up_rx_notifier                     cu_rx;
  std::unique_ptr<srs_cu_up::f1u_tx_pdu_notifier> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, f1u_cu_up_cfg, ul_tnl, cu_rx, ue_worker);

  // Create DU TX notifier adapter and RX handler
  dummy_f1u_du_gateway_bearer_rx_notifier      du_rx;
  std::unique_ptr<srs_du::f1u_tx_pdu_notifier> du_bearer =
      du_gw->create_du_bearer(0, drb_id_t::drb1, f1u_du_config, dl_tnl, ul_tnl, du_rx, timers, ue_worker);

  // Create CU RX handler and attach it to the DU TX
  cu_gw->attach_dl_teid(ul_tnl, dl_tnl);

  // Check CU-UP -> DU path
  byte_buffer cu_buf = make_byte_buffer("DDDD").value();
  check_dl_path_connected(cu_buf.deep_copy().value(), cu_bearer.get(), du_rx);

  // Check DU-> CU-UP path
  byte_buffer du_buf = make_byte_buffer("BBBB").value();
  check_ul_path_connected(du_buf.deep_copy().value(), du_bearer.get(), cu_rx);

  // Disconnect DU bearer without destryoing it from connector
  du_gw->remove_du_bearer(dl_tnl);

  // Check CU-UP -> DU path is properly detached
  byte_buffer cu_buf2 = make_byte_buffer("DEAD").value();
  check_dl_path_disconnected(cu_buf2.deep_copy().value(), cu_bearer.get(), du_rx);

  // Check DU-> CU-UP path is properly detached
  byte_buffer du_buf2 = make_byte_buffer("BEEF").value();
  check_ul_path_disconnected(du_buf2.deep_copy().value(), du_bearer.get(), cu_rx);
}

/// Update F1-U DU bearer and remove old DU bearer, as done in handover.
TEST_F(f1u_connector_test, update_du_f1u)
{
  f1u_cu_up_gateway*      cu_gw = f1u_conn->get_f1u_cu_up_gateway();
  srs_du::f1u_du_gateway* du_gw = f1u_conn->get_f1u_du_gateway();

  up_transport_layer_info ul_tnl{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{1}};
  up_transport_layer_info dl_tnl1{transport_layer_address::create_from_string("127.0.0.2"), gtpu_teid_t{2}};
  up_transport_layer_info dl_tnl2{transport_layer_address::create_from_string("127.0.0.3"), gtpu_teid_t{2}};

  // Create CU TX notifier adapter
  dummy_f1u_cu_up_rx_notifier                     cu_rx;
  std::unique_ptr<srs_cu_up::f1u_tx_pdu_notifier> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, f1u_cu_up_cfg, ul_tnl, cu_rx, ue_worker);

  // Create DU TX notifier adapter and RX handler
  dummy_f1u_du_gateway_bearer_rx_notifier      du_rx1;
  std::unique_ptr<srs_du::f1u_tx_pdu_notifier> du_bearer1 =
      du_gw->create_du_bearer(0, drb_id_t::drb1, f1u_du_config, dl_tnl1, ul_tnl, du_rx1, timers, ue_worker);

  // Create CU RX handler and attach it to the DU TX
  cu_gw->attach_dl_teid(ul_tnl, dl_tnl1);

  // Check CU-UP -> DU path
  byte_buffer cu_buf = make_byte_buffer("dead").value();
  check_dl_path_connected(cu_buf.deep_copy().value(), cu_bearer.get(), du_rx1);

  // Check DU-> CU-UP path
  byte_buffer du_buf = make_byte_buffer("beef").value();
  check_ul_path_connected(du_buf.deep_copy().value(), du_bearer1.get(), cu_rx);

  logger.info("Attach new DU bearer");

  // Attach new DU bearer
  dummy_f1u_du_gateway_bearer_rx_notifier      du_rx2;
  std::unique_ptr<srs_du::f1u_tx_pdu_notifier> du_bearer2 =
      du_gw->create_du_bearer(0, drb_id_t::drb1, f1u_du_config, dl_tnl2, ul_tnl, du_rx2, timers, ue_worker);

  // Attach new DL TEID
  cu_gw->attach_dl_teid(ul_tnl, dl_tnl2);

  // Delete old DU bearer
  du_bearer1.reset();

  // Check CU-UP -> DU path
  byte_buffer cu_buf2 = make_byte_buffer("dead").value();
  check_dl_path_connected(cu_buf2.deep_copy().value(), cu_bearer.get(), du_rx2);

  // Check DU-> CU-UP path
  byte_buffer du_buf2 = make_byte_buffer("dbee").value();
  check_ul_path_connected(du_buf2.deep_copy().value(), du_bearer2.get(), cu_rx);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
