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

/*
 * TM metrics
 */
TEST_F(rlc_tx_tm_metrics_test, small_allocs)
{
  const uint32_t sdu_size = 4;
  uint32_t       count    = 0;

  EXPECT_EQ(rlc->get_buffer_state(), 0);

  byte_buffer sdu_buf = create_sdu(sdu_size, count);
  rlc_sdu sdu = {/* pdcp_count = */ count, sdu_buf.deep_copy()}; // no std::move - keep local copy for later comparison

  // write SDU into upper end
  rlc->handle_sdu(std::move(sdu));
  EXPECT_EQ(rlc->get_buffer_state(), sdu_size);
  EXPECT_EQ(tester->bsr, sdu_size);
  EXPECT_EQ(tester->bsr_count, 1);

  // read PDU from lower end
  byte_buffer_slice_chain pdu;
  pdu = rlc->pull_pdu(sdu_size - 1);

  rlc_bearer_tx_metrics_container m = rlc->get_metrics();
  ASSERT_EQ(m.mode, rlc_mode::tm);
  ASSERT_EQ(m.mode_specific.tm.num_small_allocs, 1);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
