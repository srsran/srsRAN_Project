/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/f1u/common/f1u_connector_factory.h"
#include "srsgnb/f1u/common/f1u_local_bearer_adapter.h"
#include "srsgnb/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsgnb;

// dummy DU RX bearer interface
struct dummy_f1u_cu_up_rx_pdu_handler final : public srs_cu_up::f1u_rx_pdu_handler {
  void handle_pdu(nru_ul_message msg) override
  {
    logger.info(msg.t_pdu.begin(), msg.t_pdu.end(), "CU-UP received PDU");
    last_msg = std::move(msg);
  }
  nru_ul_message        last_msg;
  srslog::basic_logger& logger = srslog::fetch_basic_logger("F1-U", false);
};

// dummy DU RX bearer interface
struct dummy_f1u_du_rx_pdu_handler final : public srs_du::f1u_rx_pdu_handler {
  void handle_pdu(nru_dl_message msg) override
  {
    logger.info(msg.t_pdu.begin(), msg.t_pdu.end(), "DU received PDU");
    last_msg = std::move(msg);
  }
  nru_dl_message last_msg;

private:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("F1-U", false);
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
    f1u_logger.set_level(srslog::basic_levels::debug);
    f1u_logger.set_hex_dump_max_size(100);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  /// \brief Initializes fixture according to size sequence number size
  /// \param sn_size_ size of the sequence number
  void init()
  {
    logger.info("Creating F1-U connector");

    // create f1-u connector
    f1u_connector_creation_message msg = {};
    f1u_conn                           = create_f1u_connector(msg);
  }

  std::unique_ptr<f1u_connector> f1u_conn;
  srslog::basic_logger&          logger     = srslog::fetch_basic_logger("TEST", false);
  srslog::basic_logger&          f1u_logger = srslog::fetch_basic_logger("F1-U", false);
};

/// Test the instantiation of a new entity
TEST_F(f1u_connector_test, create_new_connector)
{
  init();
  EXPECT_NE(f1u_conn, nullptr);
  EXPECT_NE(f1u_conn->get_f1u_du_connector(), nullptr);
  EXPECT_NE(f1u_conn->get_f1u_cu_up_connector(), nullptr);
}

/// Test attaching F1-U bearer at CU-UP and DU
TEST_F(f1u_connector_test, attach_cu_up_f1u_to_du_f1u)
{
  init();
  f1u_cu_up_connector* cu_conn = f1u_conn->get_f1u_cu_up_connector();
  f1u_du_connector*    du_conn = f1u_conn->get_f1u_du_connector();
  // CU TX notifier adapter and RX interface
  f1u_dl_local_adapter           cu_tx;
  dummy_f1u_cu_up_rx_pdu_handler cu_rx;
  cu_conn->attach_cu_bearer(1, cu_tx, cu_rx);

  f1u_ul_local_adapter        du_tx;
  dummy_f1u_du_rx_pdu_handler du_rx;
  du_conn->attach_du_bearer(1, 2, du_tx, du_rx);

  // Check CU-UP -> DU path
  byte_buffer             cu_buf = make_byte_buffer("ABCD");
  byte_buffer_slice_chain du_exp{cu_buf.deep_copy()};
  // cu_tx.on_new_pdu(std::move(cu_buf));

  // Check DU-> CU-UP path
  byte_buffer             du_buf = make_byte_buffer("DCBA");
  byte_buffer_slice_chain cu_exp{du_buf.deep_copy()};
  // du_tx.on_new_tx_pdu(std::move(du_buf), 0);

  // ASSERT_EQ(cu_rx.last_pdu, cu_exp);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
