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

  gtpu_test_dl                 gtpu_dl = {};
  gtpu_test_ul                 gtpu_ul = {};
  gtpu_entity_creation_message msg     = {};
  msg.cfg.dl.src_teid                  = 0x1;
  msg.dl_lower                         = &gtpu_dl;
  msg.ul_upper                         = &gtpu_ul;
  std::unique_ptr<gtpu_entity>   gtpu  = create_gtpu_entity(msg);
  byte_buffer                    orig_vec{gtpu_ping_vec};
  gtpu_dl_upper_layer_interface* tx = gtpu->get_dl_upper_layer_interface();
  tx->handle_sdu(std::move(orig_vec));

  ASSERT_NE(gtpu, nullptr);
};

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
