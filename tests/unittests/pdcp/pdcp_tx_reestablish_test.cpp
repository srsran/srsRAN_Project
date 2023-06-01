/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "pdcp_tx_reestablish_test.h"
#include "lib/pdcp/pdcp_entity_impl.h"
#include "pdcp_test_vectors.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/support/bit_encoding.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsran;

/// Test SRB reestablishment
TEST_P(pdcp_tx_reestablish_test, when_srb_reestablish_then_pdus_dropped)
{
  if (sn_size == pdcp_sn_size::size18bits) {
    return; // 18 bits not supported for SRBs
  }
  init(GetParam(), pdcp_rb_type::srb);

  // Set state of PDCP entiy
  pdcp_tx_state st = {0};
  pdcp_tx->set_state(st);
  pdcp_tx->enable_security(sec_cfg);

  // Write 5 SDU
  int n_pdus = 5;
  for (int i = 0; i < n_pdus; i++) {
    byte_buffer sdu = {sdu1};
    pdcp_tx->handle_sdu(std::move(sdu));
  }

  ASSERT_EQ(5, pdcp_tx->nof_discard_timers());
  pdcp_tx->reestablish({});
  ASSERT_EQ(0, pdcp_tx->nof_discard_timers());
}

/// Test DRB UM reestablishment
TEST_P(pdcp_tx_reestablish_test, when_drb_um_reestablish_then_pdus_and_discard_timers_preserved)
{
  init(GetParam(), pdcp_rb_type::drb, pdcp_rlc_mode::um);

  // Set state of PDCP entiy
  pdcp_tx_state st = {0};
  pdcp_tx->set_state(st);
  pdcp_tx->enable_security(sec_cfg);

  // Write 5 SDU
  int n_pdus = 5;
  for (int i = 0; i < n_pdus; i++) {
    byte_buffer sdu = {sdu1};
    pdcp_tx->handle_sdu(std::move(sdu));
  }
  ASSERT_EQ(5, pdcp_tx->nof_discard_timers());
  pdcp_tx->reestablish(sec_cfg);
  ASSERT_EQ(0, pdcp_tx->nof_discard_timers());
}

/// Test DRB AM reestablishment
TEST_P(pdcp_tx_reestablish_test, when_drb_am_reestablish_then_pdus_retx)
{
  init(GetParam(), pdcp_rb_type::drb, pdcp_rlc_mode::am);

  // Set state of PDCP entiy
  pdcp_tx_state st = {0};
  pdcp_tx->set_state(st);
  pdcp_tx->enable_security(sec_cfg);

  // Write 5 SDU
  int n_pdus = 5;
  for (int i = 0; i < n_pdus; i++) {
    byte_buffer sdu = {sdu1};
    pdcp_tx->handle_sdu(std::move(sdu));
  }
  tick_all(5);
  ASSERT_EQ(5, pdcp_tx->nof_discard_timers());

  // Check that non-ACKed SDUs are retransmitted.
  ASSERT_EQ(5, test_frame.pdu_queue.size());
  pdcp_tx->handle_delivery_notification(1); // ACK SN=0 and 1.
  pdcp_tx->reestablish(sec_cfg);
  ASSERT_EQ(8, test_frame.pdu_queue.size()); // SN=2, 3 and 4 RETXed

  // Check if discard timer was not reset.
  ASSERT_EQ(3, pdcp_tx->nof_discard_timers());
  tick_all(5);
  ASSERT_EQ(0, pdcp_tx->nof_discard_timers());
}

///////////////////////////////////////////////////////////////////
// Finally, instantiate all testcases for each supported SN size //
///////////////////////////////////////////////////////////////////
std::string test_param_info_to_string(const ::testing::TestParamInfo<pdcp_sn_size>& info)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "{}bit", pdcp_sn_size_to_uint(info.param));
  return fmt::to_string(buffer);
}

INSTANTIATE_TEST_SUITE_P(pdcp_rx_test_all_sn_sizes,
                         pdcp_tx_reestablish_test,
                         ::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
