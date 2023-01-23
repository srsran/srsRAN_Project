/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/rlc/rlc_tx_tm_entity.h"
#include "rlc_test_helpers.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsgnb;

/// Mocking class of the surrounding layers invoked by the RLC TM Tx entity.
class rlc_tx_tm_test_frame : public rlc_tx_upper_layer_data_notifier,
                             public rlc_tx_upper_layer_control_notifier,
                             public rlc_tx_lower_layer_notifier
{
public:
  std::queue<byte_buffer_slice> sdu_queue;
  uint32_t                      sdu_counter = 0;
  uint32_t                      bsr         = 0;
  uint32_t                      bsr_count   = 0;

  // rlc_tx_upper_layer_data_notifier interface
  void on_transmitted_sdu(uint32_t max_tx_pdcp_sn) override {}
  void on_delivered_sdu(uint32_t max_deliv_pdcp_sn) override {}

  // rlc_tx_upper_layer_control_notifier interface
  void on_protocol_failure() override {}
  void on_max_retx() override {}

  // rlc_tx_buffer_state_update_notifier interface
  void on_buffer_state_update(unsigned bsr_) override
  {
    this->bsr = bsr_;
    this->bsr_count++;
  }
};

/// Fixture class for RLC TM Tx tests
class rlc_tx_tm_test : public ::testing::Test, public rlc_trx_test
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

    init();
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  /// \brief Initializes fixture according to size sequence number size
  void init()
  {
    logger.info("Creating RLC Tx TM entity");

    // Create test frame
    tester = std::make_unique<rlc_tx_tm_test_frame>();

    // Create RLC TM TX entity
    rlc = std::make_unique<rlc_tx_tm_entity>(du_ue_index_t::MIN_DU_UE_INDEX, srb_id_t::srb0, *tester, *tester, *tester);
  }

  srslog::basic_logger&                 logger = srslog::fetch_basic_logger("TEST", false);
  std::unique_ptr<rlc_tx_tm_test_frame> tester;
  std::unique_ptr<rlc_tx_tm_entity>     rlc;
};

TEST_F(rlc_tx_tm_test, create_new_entity)
{
  EXPECT_EQ(rlc->get_buffer_state(), 0);
  EXPECT_EQ(tester->bsr, 0);
  EXPECT_EQ(tester->bsr_count, 0);
}

TEST_F(rlc_tx_tm_test, test_tx)
{
  const uint32_t sdu_size = 4;
  uint32_t       count    = 0;

  EXPECT_EQ(rlc->get_buffer_state(), 0);

  byte_buffer sdu_buf = create_sdu(sdu_size, count);
  rlc_sdu     sdu = {sdu_buf.deep_copy(), /* pdcp_count = */ {}}; // no std::move - keep local copy for later comparison

  // write SDU into upper end
  rlc->handle_sdu(std::move(sdu));
  EXPECT_EQ(rlc->get_buffer_state(), sdu_size);
  EXPECT_EQ(tester->bsr, sdu_size);
  EXPECT_EQ(tester->bsr_count, 1);

  byte_buffer_slice_chain pdu;

  // read PDU from lower end
  pdu = rlc->pull_pdu(sdu_size);
  EXPECT_EQ(pdu.length(), sdu_size);
  EXPECT_EQ(pdu, sdu_buf);
  EXPECT_EQ(rlc->get_buffer_state(), 0);
  EXPECT_EQ(tester->bsr, 0);
  EXPECT_EQ(tester->bsr_count, 2);

  // read another PDU from lower end but there is nothing to read
  pdu = rlc->pull_pdu(sdu_size);
  EXPECT_EQ(pdu.length(), 0);
  EXPECT_EQ(rlc->get_buffer_state(), 0);
  EXPECT_EQ(tester->bsr, 0);
  EXPECT_EQ(tester->bsr_count, 2); // unchanged

  // write another SDU into upper end
  count++;
  sdu_buf = create_sdu(sdu_size, count);
  sdu     = {sdu_buf.deep_copy(), /* pdcp_count = */ {}}; // no std::move - keep local copy for later comparison
  rlc->handle_sdu(std::move(sdu));
  EXPECT_EQ(rlc->get_buffer_state(), sdu_size);
  EXPECT_EQ(tester->bsr, sdu_size);
  EXPECT_EQ(tester->bsr_count, 3);

  // read PDU from lower end with insufficient space for the whole SDU
  pdu = rlc->pull_pdu(sdu_size - 1);
  EXPECT_EQ(pdu.length(), 0);
  EXPECT_EQ(rlc->get_buffer_state(), sdu_size);
  EXPECT_EQ(tester->bsr, sdu_size);
  EXPECT_EQ(tester->bsr_count, 3); // unchanged

  // write another SDU into upper end
  count++;
  byte_buffer sdu_buf2 = create_sdu(sdu_size, count);
  sdu = {sdu_buf2.deep_copy(), /* pdcp_count = */ {}}; // no std::move - keep local copy for later comparison

  // write SDU into upper end
  rlc->handle_sdu(std::move(sdu));
  EXPECT_EQ(rlc->get_buffer_state(), 2 * sdu_size);
  EXPECT_EQ(tester->bsr, 2 * sdu_size);
  EXPECT_EQ(tester->bsr_count, 4);

  // read first PDU from lower end with oversized space
  pdu = rlc->pull_pdu(3 * sdu_size);
  EXPECT_EQ(pdu.length(), sdu_size);
  EXPECT_EQ(pdu, sdu_buf);
  EXPECT_EQ(rlc->get_buffer_state(), sdu_size);
  EXPECT_EQ(tester->bsr, sdu_size);
  EXPECT_EQ(tester->bsr_count, 5);

  // read second PDU from lower end with oversized space
  pdu = rlc->pull_pdu(3 * sdu_size);
  EXPECT_EQ(pdu.length(), sdu_size);
  EXPECT_EQ(pdu, sdu_buf2);
  EXPECT_EQ(rlc->get_buffer_state(), 0);
  EXPECT_EQ(tester->bsr, 0);
  EXPECT_EQ(tester->bsr_count, 6);
}

TEST_F(rlc_tx_tm_test, discard_sdu_increments_discard_failure_counter)
{
  const uint32_t sdu_size = 4;
  uint32_t       count    = 0;

  EXPECT_EQ(rlc->get_buffer_state(), 0);

  byte_buffer sdu_buf = create_sdu(sdu_size, count);
  rlc_sdu sdu = {sdu_buf.deep_copy(), /* pdcp_count = */ count}; // no std::move - keep local copy for later comparison

  // write SDU into upper end
  rlc->handle_sdu(std::move(sdu));
  EXPECT_EQ(rlc->get_buffer_state(), sdu_size);
  EXPECT_EQ(tester->bsr, sdu_size);
  EXPECT_EQ(tester->bsr_count, 1);

  // Try discard of valid SDU - but TM does not support any discard, discard failures are counted
  rlc->discard_sdu(0);
  EXPECT_EQ(tester->bsr, sdu_size);
  EXPECT_EQ(tester->bsr_count, 1);
  EXPECT_EQ(rlc->get_metrics().num_discarded_sdus, 0);
  EXPECT_EQ(rlc->get_metrics().num_discard_failures, 1);

  byte_buffer_slice_chain pdu;

  // read PDU from lower end
  pdu = rlc->pull_pdu(sdu_size);
  EXPECT_EQ(pdu.length(), sdu_size);
  EXPECT_EQ(pdu, sdu_buf);
  EXPECT_EQ(rlc->get_buffer_state(), 0);
  EXPECT_EQ(tester->bsr, 0);
  EXPECT_EQ(tester->bsr_count, 2);
}

TEST_F(rlc_tx_tm_test, test_tx_metrics)
{
  const uint32_t sdu_size = 4;
  uint32_t       count    = 0;

  EXPECT_EQ(rlc->get_buffer_state(), 0);

  byte_buffer sdu_buf = create_sdu(sdu_size, count);
  rlc_sdu     sdu = {sdu_buf.deep_copy(), /* pdcp_count = */ {}}; // no std::move - keep local copy for later comparison

  // write SDU into upper end
  rlc->handle_sdu(std::move(sdu));
  EXPECT_EQ(rlc->get_buffer_state(), sdu_size);
  EXPECT_EQ(tester->bsr, sdu_size);
  EXPECT_EQ(tester->bsr_count, 1);

  // read PDU from lower end
  byte_buffer_slice_chain pdu;
  pdu = rlc->pull_pdu(sdu_size - 1);

  rlc_tx_metrics m = rlc->get_metrics();
  ASSERT_EQ(m.mode, rlc_mode::tm);
  ASSERT_EQ(m.mode_specific.tm.num_small_allocs, 1);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
