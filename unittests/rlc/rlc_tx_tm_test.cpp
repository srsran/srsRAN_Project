/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../lib/rlc/rlc_tx_tm_entity.h"
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

  // rlc_tx_upper_layer_data_notifier interface
  void on_delivered_sdu(uint32_t pdcp_count) override {}

  // rlc_tx_upper_layer_control_notifier interface
  void on_protocol_failure() override {}
  void on_max_retx() override {}

  // rlc_tx_buffer_state_update_notifier interface
  void on_buffer_state_update(unsigned bsr) override {}
};

/// Fixture class for RLC TM Tx tests
class rlc_tx_am_test : public ::testing::Test, public rlc_trx_test
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

    // Create RLC AM TX entity
    rlc = std::make_unique<rlc_tx_tm_entity>(
        du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, *tester, *tester, *tester);
  }

  srslog::basic_logger&                 logger = srslog::fetch_basic_logger("TEST", false);
  std::unique_ptr<rlc_tx_tm_test_frame> tester;
  std::unique_ptr<rlc_tx_tm_entity>     rlc;
};

TEST_F(rlc_tx_am_test, create_new_entity)
{
  EXPECT_EQ(rlc->get_buffer_state(), 0);
}

TEST_F(rlc_tx_am_test, test_tx)
{
  const uint32_t sdu_size = 4;
  uint32_t       count    = 0;

  EXPECT_EQ(rlc->get_buffer_state(), 0);

  byte_buffer sdu_buf = create_sdu(sdu_size, count);
  rlc_sdu sdu = {/* pdcp_count = */ count, sdu_buf.deep_copy()}; // no std::move - keep local copy for later comparison

  // write SDU into upper end
  rlc->handle_sdu(std::move(sdu));
  EXPECT_EQ(rlc->get_buffer_state(), sdu_size);

  byte_buffer_slice_chain pdu;

  // read PDU from lower end
  pdu = rlc->pull_pdu(sdu_size);
  EXPECT_EQ(pdu.length(), sdu_size);
  EXPECT_TRUE(std::equal(pdu.begin(), pdu.end(), sdu_buf.begin(), sdu_buf.end()));
  EXPECT_EQ(rlc->get_buffer_state(), 0);

  // read another PDU from lower end but there is nothing to read
  pdu = rlc->pull_pdu(sdu_size);
  EXPECT_EQ(pdu.length(), 0);
  EXPECT_EQ(rlc->get_buffer_state(), 0);

  // write another SDU into upper end
  count++;
  sdu_buf = create_sdu(sdu_size, count);
  sdu     = {/* pdcp_count = */ count, sdu_buf.deep_copy()}; // no std::move - keep local copy for later comparison
  rlc->handle_sdu(std::move(sdu));
  EXPECT_EQ(rlc->get_buffer_state(), sdu_size);

  // read PDU from lower end with insufficient space for the whole SDU
  pdu = rlc->pull_pdu(sdu_size - 1);
  EXPECT_EQ(pdu.length(), 0);
  EXPECT_EQ(rlc->get_buffer_state(), sdu_size);

  // write another SDU into upper end
  count++;
  byte_buffer sdu_buf2 = create_sdu(sdu_size, count);
  sdu = {/* pdcp_count = */ count, sdu_buf2.deep_copy()}; // no std::move - keep local copy for later comparison

  // write SDU into upper end
  rlc->handle_sdu(std::move(sdu));
  EXPECT_EQ(rlc->get_buffer_state(), 2 * sdu_size);

  // read first PDU from lower end with oversized space
  pdu = rlc->pull_pdu(3 * sdu_size);
  EXPECT_EQ(pdu.length(), sdu_size);
  EXPECT_TRUE(std::equal(pdu.begin(), pdu.end(), sdu_buf.begin(), sdu_buf.end()));
  EXPECT_EQ(rlc->get_buffer_state(), sdu_size);

  // read second PDU from lower end with oversized space
  pdu = rlc->pull_pdu(3 * sdu_size);
  EXPECT_EQ(pdu.length(), sdu_size);
  EXPECT_TRUE(std::equal(pdu.begin(), pdu.end(), sdu_buf2.begin(), sdu_buf2.end()));
  EXPECT_EQ(rlc->get_buffer_state(), 0);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
