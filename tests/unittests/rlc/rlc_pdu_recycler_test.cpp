/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/rlc/rlc_pdu_recycler.h"
#include "srsran/ran/du_types.h"
#include <gtest/gtest.h>

using namespace srsran;

/// Fixture class for RLC PDU recycler tests
class rlc_pdu_recycler_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init RLC logger
    srslog::fetch_basic_logger("RLC", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("RLC", false).set_hex_dump_max_size(-1);

    logger.info("Creating RLC PDU recycler");

    // Create RLC PDU recycler
    pdu_recycler = std::make_unique<rlc_pdu_recycler>(4, rlc_logger);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger&             logger     = srslog::fetch_basic_logger("TEST", false);
  rlc_bearer_logger                 rlc_logger = rlc_bearer_logger("RLC", {du_ue_index_t{}, rb_id_t{}, "DL"});
  std::unique_ptr<rlc_pdu_recycler> pdu_recycler;

  const size_t retx_queue_size = 4;
};

TEST_F(rlc_pdu_recycler_test, create_new_entity)
{
  byte_buffer pdu;
  pdu_recycler->add_discarded_pdu(std::move(pdu));
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
