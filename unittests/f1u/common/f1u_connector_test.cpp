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
#include "srsgnb/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsgnb;

// dummy CU-UP RX bearer interface
struct dummy_f1u_cu_up_rx_sdu_notifier final : public srs_cu_up::f1u_rx_sdu_notifier {
  void on_new_sdu(byte_buffer_slice_chain sdu) override
  {
    logger.info(sdu.begin(), sdu.end(), "CU-UP received SDU");
    last_sdu = std::move(sdu);
  }
  byte_buffer_slice_chain last_sdu;
  srslog::basic_logger&   logger = srslog::fetch_basic_logger("F1-U", false);
};

struct dummy_f1u_cu_up_rx_delivery_notifier final : public srs_cu_up::f1u_rx_delivery_notifier {
  void on_delivered_sdu(uint32_t count) override { logger.info("CU-UP PDU with COUNT={} delivered", count); }
  srslog::basic_logger& logger = srslog::fetch_basic_logger("F1-U", false);
};

// dummy DU RX bearer interface
struct dummy_f1u_du_rx_sdu_notifier final : public srs_du::f1u_rx_sdu_notifier {
  void on_new_sdu(byte_buffer sdu, uint32_t count) override
  {
    logger.info(sdu.begin(), sdu.end(), "DU received SDU. COUNT={}", count);
    last_sdu = std::move(sdu);
  }
  void        on_discard_sdu(uint32_t count) override {}
  byte_buffer last_sdu;

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
  (void)du_conn;

  // Create CU TX notifier adapter
  dummy_f1u_cu_up_rx_sdu_notifier      cu_rx;
  dummy_f1u_cu_up_rx_delivery_notifier cu_delivery;
  srs_cu_up::f1u_bearer*               cu_bearer = cu_conn->create_cu_dl_bearer(1, cu_delivery, cu_rx);

  // Create DU TX notifier adapter and RX handler
  dummy_f1u_du_rx_sdu_notifier du_rx;
  srs_du::f1u_bearer*          du_bearer = du_conn->create_du_ul_bearer(1, 2, du_rx);

  // Create CU RX handler and attach it to the DU TX
  cu_conn->attach_cu_ul_bearer(1, 2);

  // Check CU-UP -> DU path
  byte_buffer             cu_buf = make_byte_buffer("ABCD");
  byte_buffer_slice_chain du_exp{cu_buf.deep_copy()};
  cu_bearer->get_tx_sdu_handler().handle_sdu(std::move(cu_buf), 0);

  // Check DU-> CU-UP path
  byte_buffer             du_buf = make_byte_buffer("DCBA");
  byte_buffer             cu_exp = du_buf.deep_copy();
  byte_buffer_slice_chain du_slice{du_buf.deep_copy()};
  du_bearer->get_tx_sdu_handler().handle_sdu(std::move(du_slice));

  ASSERT_EQ(du_rx.last_sdu, du_exp);
  ASSERT_EQ(cu_rx.last_sdu, cu_exp);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
