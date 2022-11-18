/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "gtpu_pdu_test.h"
#include "srsgnb/gtpu/gtpu_factory.h"
#include "srsgnb/support/test_utils.h"
#include "srsgnb/support/timers.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsgnb;

/// \brief Test correct creation of GTP-U entity
TEST_F(gtpu_test, entity_creation)
{
  srsgnb::test_delimit_logger  delimiter("GTP-U entity creation test");
  gtpu_entity_creation_message msg  = {};
  std::unique_ptr<gtpu_entity> gtpu = create_gtpu_entity(msg);

  ASSERT_NE(gtpu, nullptr);
};

TEST_F(gtpu_test, rx_sdu)
{
  srsgnb::test_delimit_logger delimiter("GTP-U entity creation test");

  gtpu_test_rx_lower           gtpu_rx = {};
  gtpu_test_tx_upper           gtpu_tx = {};
  gtpu_entity_creation_message msg     = {};
  msg.cfg.rx.local_teid                = 0x1;
  msg.rx_lower                         = &gtpu_rx;
  msg.tx_upper                         = &gtpu_tx;
  std::unique_ptr<gtpu_entity>   gtpu  = create_gtpu_entity(msg);
  byte_buffer                    orig_vec{gtpu_ping_vec};
  gtpu_rx_upper_layer_interface* rx = gtpu->get_rx_upper_layer_interface();
  rx->handle_sdu(std::move(orig_vec));
};

TEST_F(gtpu_test, tx_pdu)
{
  srsgnb::test_delimit_logger delimiter("GTP-U entity creation test");

  gtpu_test_tx_upper           gtpu_tx = {};
  gtpu_test_rx_lower           gtpu_rx = {};
  gtpu_entity_creation_message msg     = {};
  msg.cfg.tx.peer_teid                 = 0x1;
  msg.rx_lower                         = &gtpu_rx;
  msg.tx_upper                         = &gtpu_tx;
  std::unique_ptr<gtpu_entity> gtpu    = create_gtpu_entity(msg);
  byte_buffer                  vec{gtpu_ping_vec};
  gtpu_header                  tmp;
  bool                         read_ok = gtpu_read_and_strip_header(tmp, vec, gtpu_logger);
  ASSERT_EQ(read_ok, true);
  gtpu_tx_lower_layer_interface* tx = gtpu->get_tx_lower_layer_interface();
  tx->handle_pdu(std::move(vec));
};

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
