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

#include "pdcp_rx_reestablish_test.h"
#include "lib/pdcp/pdcp_entity_impl.h"
#include "pdcp_test_vectors.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/support/bit_encoding.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsran;

/// Test SRB reestablishment
TEST_P(pdcp_rx_reestablish_test, when_srb_reestablish_then_sdus_dropped)
{
  init(GetParam(), pdcp_rb_type::srb);
  uint32_t count = 0;

  security::sec_128_as_config reest_sec_cfg = sec_cfg;
  reest_sec_cfg.integ_algo                  = security::integrity_algorithm::nia3;
  reest_sec_cfg.cipher_algo                 = security::ciphering_algorithm::nea3;
  pdcp_rx->enable_security(sec_cfg);

  // Prepare 3 PDUs.
  byte_buffer test_pdu1;
  get_test_pdu(count, test_pdu1);
  byte_buffer test_pdu2;
  get_test_pdu(count + 1, test_pdu2);
  byte_buffer test_pdu3;
  get_test_pdu(count + 2, test_pdu3);

  // RX PDU 2 and 3, none should arrive.
  pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
  pdcp_rx->set_state(init_state);
  pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu2)});
  ASSERT_EQ(0, test_frame->sdu_queue.size());
  pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu3)});
  ASSERT_EQ(0, test_frame->sdu_queue.size());

  // Check PDCP state.
  {
    pdcp_rx_state st = pdcp_rx->get_state();
    ASSERT_EQ(st.rx_next, 3);
  }
  // Re-establish entity.
  pdcp_rx->reestablish(reest_sec_cfg);

  // Check no PDU was delivered
  // and that state was reset.
  {
    pdcp_rx_state st = pdcp_rx->get_state();
    ASSERT_EQ(st.rx_next, 0);
  }
  ASSERT_EQ(0, test_frame->sdu_queue.size());

  // Check security config changed
  {
    security::sec_128_as_config reest_sec_config2 = pdcp_rx->get_sec_config();
    ASSERT_EQ(reest_sec_cfg, reest_sec_config2);
  }
}

/// Test DRB UM reestablishment
TEST_P(pdcp_rx_reestablish_test, when_drb_um_reestablish_then_pdus_forwared)
{
  init(GetParam(), pdcp_rb_type::drb, pdcp_rlc_mode::um);
  uint32_t count = 0;

  pdcp_rx->enable_security(sec_cfg);

  // Prepare 3 PDUs.
  byte_buffer test_pdu1;
  get_test_pdu(count, test_pdu1);
  byte_buffer test_pdu2;
  get_test_pdu(count + 1, test_pdu2);
  byte_buffer test_pdu3;
  get_test_pdu(count + 2, test_pdu3);

  // RX PDU 2 and 3, none should arrive.
  pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
  pdcp_rx->set_state(init_state);
  pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu2)});
  ASSERT_EQ(0, test_frame->sdu_queue.size());
  pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu3)});
  ASSERT_EQ(0, test_frame->sdu_queue.size());

  // Check PDCP state.
  {
    pdcp_rx_state st = pdcp_rx->get_state();
    ASSERT_EQ(st.rx_next, 3);
    ASSERT_EQ(true, pdcp_rx->is_reordering_timer_running());
  }
  // Re-establish entity.
  pdcp_rx->reestablish(sec_cfg);

  // Check that 2 PDUs were delivered
  // and that state was reset.
  {
    pdcp_rx_state st = pdcp_rx->get_state();
    ASSERT_EQ(st.rx_next, 0);
  }
  ASSERT_EQ(2, test_frame->sdu_queue.size());

  // Check re-ordering timer was stopped
  ASSERT_EQ(false, pdcp_rx->is_reordering_timer_running());
}

/// Test DRB AM reestablishment
TEST_P(pdcp_rx_reestablish_test, when_drb_am_reestablish_then_state_preserved)
{
  init(GetParam(), pdcp_rb_type::drb, pdcp_rlc_mode::am);

  uint32_t                    count         = 0;
  security::sec_128_as_config reest_sec_cfg = sec_cfg;

  pdcp_rx->enable_security(sec_cfg);

  // Prepare 3 PDUs.
  byte_buffer test_pdu1;
  get_test_pdu(count, test_pdu1);
  byte_buffer test_pdu2;
  get_test_pdu(count + 1, test_pdu2);
  byte_buffer test_pdu3;
  get_test_pdu(count + 2, test_pdu3);

  // RX PDU 2 and 3, none should arrive.
  pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
  pdcp_rx->set_state(init_state);
  pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu2)});
  ASSERT_EQ(0, test_frame->sdu_queue.size());
  pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu3)});
  ASSERT_EQ(0, test_frame->sdu_queue.size());

  // Check PDCP state.
  {
    pdcp_rx_state st = pdcp_rx->get_state();
    ASSERT_EQ(st.rx_next, 3);
    ASSERT_EQ(true, pdcp_rx->is_reordering_timer_running());
  }
  // Re-establish entity.
  pdcp_rx->reestablish(reest_sec_cfg);

  // Check that PDUs were *not* delivered
  // and that state was *not* reset.
  {
    pdcp_rx_state st = pdcp_rx->get_state();
    ASSERT_EQ(st.rx_next, 3);
  }
  ASSERT_EQ(0, test_frame->sdu_queue.size());

  // Check re-ordering timer is still running
  ASSERT_EQ(true, pdcp_rx->is_reordering_timer_running());

  // Deliver first PDU
  pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu1)});
  ASSERT_EQ(3, test_frame->sdu_queue.size());
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
                         pdcp_rx_reestablish_test,
                         ::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
