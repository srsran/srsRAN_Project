/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/sdap/sdap_entity_tx_impl.h"
#include "srsran/sdap/sdap.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsran;
using namespace srs_cu_up;

/// Mocking class of the surrounding layers invoked by the SDAP TX entity.
class sdap_tx_test_frame : public sdap_tx_pdu_notifier
{
public:
  std::queue<byte_buffer> pdu_queue;

  // sdap_tx_pdu_notifier interface
  void on_new_pdu(byte_buffer pdu) override { pdu_queue.push(std::move(pdu)); };
};

/// Fixture class for SDAP TX tests
class sdap_tx_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init SDAP logger
    srslog::fetch_basic_logger("SDAP", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("SDAP", false).set_hex_dump_max_size(-1);

    logger.info("Creating SDAP TX entity.");

    // Create test frame
    tester = std::make_unique<sdap_tx_test_frame>();

    // Create SDAP TX entity
    sdap = std::make_unique<sdap_entity_tx_impl>(7, pdu_session_id_t::min, *tester);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger&                logger = srslog::fetch_basic_logger("TEST", false);
  std::unique_ptr<sdap_tx_test_frame>  tester;
  std::unique_ptr<sdap_entity_tx_impl> sdap;
};

/// \brief Test correct creation of SDAP TX entity
TEST_F(sdap_tx_test, create_new_entity)
{
  ASSERT_NE(sdap, nullptr);
}

/// \brief Test simple PDU transmission
TEST_F(sdap_tx_test, test_tx)
{
  const std::array<uint8_t, 4> sdu_buf = {0x00, 0x01, 0x02, 0x03};
  byte_buffer                  sdu{sdu_buf};

  sdap->handle_sdu(sdu.deep_copy());

  ASSERT_FALSE(tester->pdu_queue.empty());
  EXPECT_EQ(tester->pdu_queue.front(), sdu);
  tester->pdu_queue.pop();
  ASSERT_TRUE(tester->pdu_queue.empty());
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
