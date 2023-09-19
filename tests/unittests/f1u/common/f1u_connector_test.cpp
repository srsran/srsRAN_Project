/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/f1u/local_connector/f1u_local_connector.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;

// dummy CU-UP RX bearer interface
struct dummy_f1u_cu_up_rx_sdu_notifier final : public srs_cu_up::f1u_rx_sdu_notifier {
  void on_new_sdu(byte_buffer_chain sdu) override
  {
    logger.info(sdu.begin(), sdu.end(), "CU-UP received SDU");
    last_sdu = std::move(sdu);
  }
  byte_buffer_chain     last_sdu;
  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-F1-U", false);
};

struct dummy_f1u_cu_up_rx_delivery_notifier final : public srs_cu_up::f1u_rx_delivery_notifier {
  void on_transmit_notification(uint32_t highest_pdcp_sn) override
  {
    logger.info("CU-UP PDCP PDU transmitted up to highest_pdcp_sn={}", highest_pdcp_sn);
  }
  void on_delivery_notification(uint32_t highest_pdcp_sn) override
  {
    logger.info("CU-UP PDCP PDU delivered up to highest_pdcp_sn={}", highest_pdcp_sn);
  }
  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-F1-U", false);
};

// dummy DU RX bearer interface
struct dummy_f1u_du_rx_sdu_notifier final : public srs_du::f1u_rx_sdu_notifier {
  void on_new_sdu(pdcp_tx_pdu sdu) override
  {
    logger.info(sdu.buf.begin(), sdu.buf.end(), "DU received SDU. pdcp_sn={}", sdu.pdcp_sn);
    last_sdu = std::move(sdu.buf);
  }
  void        on_discard_sdu(uint32_t pdcp_sn) override {}
  byte_buffer last_sdu;

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

    timers = timer_factory{timer_mng, ue_worker};

    // set F1-U bearer config
    config.t_notify = 10;
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  timer_manager      timer_mng;
  manual_task_worker ue_worker{128};
  timer_factory      timers;

  srs_du::f1u_config                   config;
  std::unique_ptr<f1u_local_connector> f1u_conn;
  srslog::basic_logger&                logger        = srslog::fetch_basic_logger("TEST", false);
  srslog::basic_logger&                f1u_logger_cu = srslog::fetch_basic_logger("CU-F1-U", false);
  srslog::basic_logger&                f1u_logger_du = srslog::fetch_basic_logger("DU-F1-U", false);
};

/// Test the instantiation of a new entity
TEST_F(f1u_connector_test, create_new_connector)
{
  EXPECT_NE(f1u_conn, nullptr);
  EXPECT_NE(f1u_conn->get_f1u_du_gateway(), nullptr);
  EXPECT_NE(f1u_conn->get_f1u_cu_up_gateway(), nullptr);
}

/// Test attaching/detaching F1-U bearer at CU-UP and DU
TEST_F(f1u_connector_test, attach_detach_cu_up_f1u_to_du_f1u)
{
  f1u_cu_up_gateway*      cu_gw = f1u_conn->get_f1u_cu_up_gateway();
  srs_du::f1u_du_gateway* du_gw = f1u_conn->get_f1u_du_gateway();

  up_transport_layer_info ul_tnl{{"127.0.0.1"}, gtpu_teid_t{1}};
  up_transport_layer_info dl_tnl{{"127.0.0.2"}, gtpu_teid_t{2}};

  // Create CU TX notifier adapter
  dummy_f1u_cu_up_rx_sdu_notifier        cu_rx;
  dummy_f1u_cu_up_rx_delivery_notifier   cu_delivery;
  std::unique_ptr<srs_cu_up::f1u_bearer> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, ul_tnl, cu_delivery, cu_rx, timers);

  // Create DU TX notifier adapter and RX handler
  dummy_f1u_du_rx_sdu_notifier du_rx;
  srs_du::f1u_bearer* du_bearer = du_gw->create_du_bearer(0, drb_id_t::drb1, config, dl_tnl, ul_tnl, du_rx, timers);

  // Create CU RX handler and attach it to the DU TX
  cu_gw->attach_dl_teid(ul_tnl, dl_tnl);

  // Check CU-UP -> DU path
  byte_buffer       cu_buf = make_byte_buffer("ABCD");
  byte_buffer_chain du_exp{cu_buf.deep_copy()};
  pdcp_tx_pdu       sdu;
  sdu.buf     = std::move(cu_buf);
  sdu.pdcp_sn = 0;
  cu_bearer->get_tx_sdu_handler().handle_sdu(std::move(sdu));

  // Check DU-> CU-UP path
  byte_buffer       du_buf = make_byte_buffer("DCBA");
  byte_buffer       cu_exp = du_buf.deep_copy();
  byte_buffer_chain du_slice{du_buf.deep_copy()};
  du_bearer->get_tx_sdu_handler().handle_sdu(std::move(du_slice));

  ASSERT_EQ(du_rx.last_sdu, du_exp);
  ASSERT_EQ(cu_rx.last_sdu, cu_exp);

  // Delete CU bearer
  cu_bearer.reset();

  // Check DU-> CU-UP path is properly detached
  byte_buffer       du_buf2 = make_byte_buffer("LMNO");
  byte_buffer       cu_exp2 = du_buf2.deep_copy();
  byte_buffer_chain du_slice2{du_buf2.deep_copy()};
  du_bearer->get_tx_sdu_handler().handle_sdu(std::move(du_slice2));
  ASSERT_EQ(cu_rx.last_sdu, cu_exp); // Last SDU should not have changed
}

/// Test detaching F1-U bearer at DU first and make sure CU-UP does not crash
TEST_F(f1u_connector_test, detach_du_f1u_first)
{
  f1u_cu_up_gateway*      cu_gw = f1u_conn->get_f1u_cu_up_gateway();
  srs_du::f1u_du_gateway* du_gw = f1u_conn->get_f1u_du_gateway();

  up_transport_layer_info ul_tnl{{"127.0.0.1"}, gtpu_teid_t{1}};
  up_transport_layer_info dl_tnl{{"127.0.0.2"}, gtpu_teid_t{2}};

  // Create CU TX notifier adapter
  dummy_f1u_cu_up_rx_sdu_notifier        cu_rx;
  dummy_f1u_cu_up_rx_delivery_notifier   cu_delivery;
  std::unique_ptr<srs_cu_up::f1u_bearer> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, ul_tnl, cu_delivery, cu_rx, timers);

  // Create DU TX notifier adapter and RX handler
  dummy_f1u_du_rx_sdu_notifier du_rx;
  srs_du::f1u_bearer* du_bearer = du_gw->create_du_bearer(0, drb_id_t::drb1, config, dl_tnl, ul_tnl, du_rx, timers);

  // Create CU RX handler and attach it to the DU TX
  cu_gw->attach_dl_teid(ul_tnl, dl_tnl);

  // Check CU-UP -> DU path
  byte_buffer       cu_buf = make_byte_buffer("ABCD");
  byte_buffer_chain du_exp{cu_buf.deep_copy()};
  pdcp_tx_pdu       sdu;
  sdu.buf     = std::move(cu_buf);
  sdu.pdcp_sn = 0;
  cu_bearer->get_tx_sdu_handler().handle_sdu(std::move(sdu));

  // Check DU-> CU-UP path
  byte_buffer       du_buf = make_byte_buffer("DCBA");
  byte_buffer       cu_exp = du_buf.deep_copy();
  byte_buffer_chain du_slice{du_buf.deep_copy()};
  du_bearer->get_tx_sdu_handler().handle_sdu(std::move(du_slice));

  ASSERT_EQ(du_rx.last_sdu, du_exp);
  ASSERT_EQ(cu_rx.last_sdu, cu_exp);

  // Delete DU bearer
  du_gw->remove_du_bearer(dl_tnl);

  // Check DU-> CU-UP path is properly detached
  byte_buffer du_buf2 = make_byte_buffer("LMNO");
  pdcp_tx_pdu sdu2;
  sdu.buf = std::move(cu_buf);
  cu_bearer->get_tx_sdu_handler().handle_sdu(sdu2);
  ASSERT_EQ(cu_rx.last_sdu, cu_exp); // Last SDU should not have changed
}

/// Update F1-U DU bearer and remove old DU bearer, as done in handover.
TEST_F(f1u_connector_test, update_du_f1u)
{
  f1u_cu_up_gateway*      cu_gw = f1u_conn->get_f1u_cu_up_gateway();
  srs_du::f1u_du_gateway* du_gw = f1u_conn->get_f1u_du_gateway();

  up_transport_layer_info ul_tnl{{"127.0.0.1"}, gtpu_teid_t{1}};
  up_transport_layer_info dl_tnl1{{"127.0.0.2"}, gtpu_teid_t{2}};
  up_transport_layer_info dl_tnl2{{"127.0.0.3"}, gtpu_teid_t{2}};

  // Create CU TX notifier adapter
  dummy_f1u_cu_up_rx_sdu_notifier        cu_rx;
  dummy_f1u_cu_up_rx_delivery_notifier   cu_delivery;
  std::unique_ptr<srs_cu_up::f1u_bearer> cu_bearer =
      cu_gw->create_cu_bearer(0, drb_id_t::drb1, ul_tnl, cu_delivery, cu_rx, timers);

  // Create DU TX notifier adapter and RX handler
  dummy_f1u_du_rx_sdu_notifier du_rx1;
  srs_du::f1u_bearer* du_bearer1 = du_gw->create_du_bearer(0, drb_id_t::drb1, config, dl_tnl1, ul_tnl, du_rx1, timers);

  // Create CU RX handler and attach it to the DU TX
  cu_gw->attach_dl_teid(ul_tnl, dl_tnl1);

  {
    // Check CU-UP -> DU path
    byte_buffer       cu_buf = make_byte_buffer("ABCD");
    byte_buffer_chain du_exp{cu_buf.deep_copy()};
    pdcp_tx_pdu       sdu;
    sdu.buf     = std::move(cu_buf);
    sdu.pdcp_sn = 0;
    cu_bearer->get_tx_sdu_handler().handle_sdu(std::move(sdu));

    // Check DU-> CU-UP path
    byte_buffer       du_buf = make_byte_buffer("DCBA");
    byte_buffer       cu_exp = du_buf.deep_copy();
    byte_buffer_chain du_slice{du_buf.deep_copy()};
    du_bearer1->get_tx_sdu_handler().handle_sdu(std::move(du_slice));

    ASSERT_EQ(du_rx1.last_sdu, du_exp);
    ASSERT_EQ(cu_rx.last_sdu, cu_exp);
  }

  logger.info("Attach new DU bearer");

  // Attach new DU bearer
  dummy_f1u_du_rx_sdu_notifier du_rx2;
  srs_du::f1u_bearer* du_bearer2 = du_gw->create_du_bearer(0, drb_id_t::drb1, config, dl_tnl2, ul_tnl, du_rx2, timers);

  // Attach new DL TEID
  cu_gw->attach_dl_teid(ul_tnl, dl_tnl2);

  // Delete old DU bearer
  du_gw->remove_du_bearer(dl_tnl1);

  {
    // Check CU-UP -> DU path
    byte_buffer       cu_buf = make_byte_buffer("ABCD");
    byte_buffer_chain du_exp{cu_buf.deep_copy()};
    pdcp_tx_pdu       sdu;
    sdu.buf     = std::move(cu_buf);
    sdu.pdcp_sn = 0;
    cu_bearer->get_tx_sdu_handler().handle_sdu(std::move(sdu));

    // Check DU-> CU-UP path
    byte_buffer       du_buf = make_byte_buffer("DCBA");
    byte_buffer       cu_exp = du_buf.deep_copy();
    byte_buffer_chain du_slice{du_buf.deep_copy()};
    du_bearer2->get_tx_sdu_handler().handle_sdu(std::move(du_slice));
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
