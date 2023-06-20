/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/sdap/sdap_entity_impl.h"
#include "srsran/sdap/sdap.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsran;
using namespace srs_cu_up;

/// Mocking class of the upper layers invoked by the SDAP entity.
class sdap_upper_dummy : public sdap_rx_sdu_notifier
{
public:
  std::queue<byte_buffer> sdu_queue;

  // sdap_rx_sdu_notifier interface
  void on_new_sdu(byte_buffer pdu) override { sdu_queue.push(std::move(pdu)); };
};

/// Mocking class of the lower layers invoked by the SDAP entity.
class sdap_lower_dummy : public sdap_tx_pdu_notifier
{
public:
  std::queue<byte_buffer> pdu_queue;

  // sdap_tx_pdu_notifier interface
  void on_new_pdu(byte_buffer pdu) override { pdu_queue.push(std::move(pdu)); };
};

/// Fixture class for SDAP tests
class sdap_test : public ::testing::Test
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

    logger.info("Creating SDAP entity.");

    // Create test frames
    ul_sink  = std::make_unique<sdap_upper_dummy>();
    dl_sink1 = std::make_unique<sdap_lower_dummy>();
    dl_sink2 = std::make_unique<sdap_lower_dummy>();

    // Create SDAP TX entity
    ue_inactivity_timer = timers.create_timer();
    ue_inactivity_timer.set(std::chrono::milliseconds(10000), [](timer_id_t) {});
    sdap = std::make_unique<sdap_entity_impl>(7, pdu_session_id_t::min, ue_inactivity_timer, *ul_sink);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger&             logger = srslog::fetch_basic_logger("TEST", false);
  manual_task_worker                worker{64};
  timer_manager                     timers_manager;
  timer_factory                     timers{timers_manager, worker};
  unique_timer                      ue_inactivity_timer;
  std::unique_ptr<sdap_upper_dummy> ul_sink;
  std::unique_ptr<sdap_lower_dummy> dl_sink1;
  std::unique_ptr<sdap_lower_dummy> dl_sink2;
  std::unique_ptr<sdap_entity_impl> sdap;
};

/// \brief Test correct creation of SDAP entity
TEST_F(sdap_test, create_new_entity)
{
  ASSERT_NE(sdap, nullptr);
}

/// \brief Test TX without any mapping
TEST_F(sdap_test, test_tx_no_mapping)
{
  const std::array<uint8_t, 4> sdu_buf = {0x00, 0x01, 0x02, 0x03};
  byte_buffer                  sdu{sdu_buf};

  sdap->handle_sdu(sdu.deep_copy(), qos_flow_id_t::min);

  EXPECT_TRUE(dl_sink1->pdu_queue.empty());
  EXPECT_TRUE(dl_sink2->pdu_queue.empty());
  EXPECT_TRUE(ul_sink->sdu_queue.empty());
}

/// \brief Test TX with unknown mapping
TEST_F(sdap_test, test_tx_unknown_mapping)
{
  const std::array<uint8_t, 4> sdu_buf = {0x00, 0x01, 0x02, 0x03};
  byte_buffer                  sdu{sdu_buf};

  sdap_config sdap_cfg = {};

  // add different mapping
  sdap->add_mapping(qos_flow_id_t::min, drb_id_t::drb1, sdap_cfg, *dl_sink1);

  sdap->handle_sdu(sdu.deep_copy(), qos_flow_id_t::max); // different QFI

  EXPECT_TRUE(dl_sink1->pdu_queue.empty());
  EXPECT_TRUE(dl_sink2->pdu_queue.empty());
}

/// \brief Test TX with known mapping
TEST_F(sdap_test, test_tx_known_mapping)
{
  const std::array<uint8_t, 4> sdu_buf = {0x00, 0x01, 0x02, 0x03};
  byte_buffer                  sdu{sdu_buf};

  sdap_config sdap_cfg = {};

  // add mapping
  sdap->add_mapping(qos_flow_id_t::min, drb_id_t::drb1, sdap_cfg, *dl_sink1);

  sdap->handle_sdu(sdu.deep_copy(), qos_flow_id_t::min);

  ASSERT_FALSE(dl_sink1->pdu_queue.empty());
  EXPECT_TRUE(dl_sink2->pdu_queue.empty());
  EXPECT_EQ(dl_sink1->pdu_queue.front(), sdu);
  dl_sink1->pdu_queue.pop();
  EXPECT_TRUE(dl_sink1->pdu_queue.empty());
}

/// \brief Test TX with removed mapping
TEST_F(sdap_test, test_tx_removed_mapping)
{
  const std::array<uint8_t, 4> sdu_buf1 = {0x00, 0x01, 0x02, 0x03};
  byte_buffer                  sdu1{sdu_buf1};
  const std::array<uint8_t, 4> sdu_buf2 = {0x01, 0x02, 0x03, 0x04};
  byte_buffer                  sdu2{sdu_buf2};
  const std::array<uint8_t, 4> sdu_buf3 = {0x02, 0x03, 0x04, 0x05};
  byte_buffer                  sdu3{sdu_buf3};
  const std::array<uint8_t, 4> sdu_buf4 = {0x03, 0x04, 0x05, 0x06};
  byte_buffer                  sdu4{sdu_buf4};

  sdap_config sdap_cfg = {};

  // add mapping
  sdap->add_mapping(qos_flow_id_t::min, drb_id_t::drb1, sdap_cfg, *dl_sink1);
  sdap->add_mapping(qos_flow_id_t::max, drb_id_t::drb2, sdap_cfg, *dl_sink2);

  sdap->handle_sdu(sdu1.deep_copy(), qos_flow_id_t::min);
  sdap->handle_sdu(sdu2.deep_copy(), qos_flow_id_t::max);

  ASSERT_FALSE(dl_sink1->pdu_queue.empty());
  EXPECT_EQ(dl_sink1->pdu_queue.front(), sdu1);
  dl_sink1->pdu_queue.pop();
  EXPECT_TRUE(dl_sink1->pdu_queue.empty());

  ASSERT_FALSE(dl_sink2->pdu_queue.empty());
  EXPECT_EQ(dl_sink2->pdu_queue.front(), sdu2);
  dl_sink2->pdu_queue.pop();
  EXPECT_TRUE(dl_sink2->pdu_queue.empty());

  sdap->remove_mapping(drb_id_t::drb1);

  sdap->handle_sdu(sdu3.deep_copy(), qos_flow_id_t::min);
  sdap->handle_sdu(sdu4.deep_copy(), qos_flow_id_t::max);

  EXPECT_TRUE(dl_sink1->pdu_queue.empty());

  ASSERT_FALSE(dl_sink2->pdu_queue.empty());
  EXPECT_EQ(dl_sink2->pdu_queue.front(), sdu4);
  dl_sink2->pdu_queue.pop();
  EXPECT_TRUE(dl_sink2->pdu_queue.empty());
}

#ifdef ASSERTS_ENABLED
/// \brief Test RX without any mapping
TEST_F(sdap_test, test_rx_no_mapping)
{
  (void)(::testing::GTEST_FLAG(death_test_style) = "threadsafe");
  // no prior mapping, should trigger an assertion
  ASSERT_DEATH({ sdap->get_sdap_rx_pdu_handler(drb_id_t::drb1); }, "");
}
#endif

#ifdef ASSERTS_ENABLED
/// \brief Test RX with unknown mapping
TEST_F(sdap_test, test_rx_unknown_mapping)
{
  (void)(::testing::GTEST_FLAG(death_test_style) = "threadsafe");

  sdap_config sdap_cfg = {};

  // add different mapping
  sdap->add_mapping(qos_flow_id_t::min, drb_id_t::drb1, sdap_cfg, *dl_sink1);

  // no matching mapping, should trigger an assertion
  ASSERT_DEATH({ sdap->get_sdap_rx_pdu_handler(drb_id_t::drb2); }, "");
}
#endif

/// \brief Test RX with known mapping
TEST_F(sdap_test, test_rx_known_mapping)
{
  const std::array<uint8_t, 4> pdu_buf = {0x00, 0x01, 0x02, 0x03};
  byte_buffer                  pdu{pdu_buf};

  sdap_config sdap_cfg = {};

  // add mapping
  sdap->add_mapping(qos_flow_id_t::min, drb_id_t::drb1, sdap_cfg, *dl_sink1);

  sdap->get_sdap_rx_pdu_handler(drb_id_t::drb1).handle_pdu(pdu.deep_copy());

  ASSERT_FALSE(ul_sink->sdu_queue.empty());
  EXPECT_EQ(ul_sink->sdu_queue.front(), pdu);
  ul_sink->sdu_queue.pop();
  EXPECT_TRUE(ul_sink->sdu_queue.empty());
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
