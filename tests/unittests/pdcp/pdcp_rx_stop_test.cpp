/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "pdcp_rx_stop_test.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

/// Test stopping PDCP RX entities when no PDUs are present.
TEST_P(pdcp_rx_stop_test, stop_when_there_are_no_pending_pdus)
{
  init(GetParam());
  srsran::test_delimit_logger delimiter("Normal stop test. SN_SIZE={} ", sn_size);

  pdcp_rx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

  // Check await rx crypto flag is not set.
  manual_event_flag& awaitable = pdcp_rx->crypto_awaitable();

  ASSERT_FALSE(awaitable.is_set());

  pdcp_rx->stop();

  // Awaitable is now set.
  ASSERT_TRUE(awaitable.is_set());
}

/// Test stopping PDCP RX entities when PDUs are present.
TEST_P(pdcp_rx_stop_test, stop_when_there_are_pending_pdus)
{
  init(GetParam());
  srsran::test_delimit_logger delimiter("Normal stop test. SN_SIZE={} ", sn_size);

  pdcp_rx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

  uint32_t count = 0;

  // Generate test PDUs.
  byte_buffer test_pdu1;
  get_test_pdu(count, test_pdu1);
  byte_buffer test_pdu2;
  get_test_pdu(count + 1, test_pdu2);

  // Set PDCP state.
  pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
  pdcp_rx->set_state(init_state);

  // Handle PDUs, but don't run the crypto worker
  pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu1)).value());
  pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu2)).value());

  // Check await rx crypto flag is not set.
  manual_event_flag& awaitable = pdcp_rx->crypto_awaitable();

  ASSERT_FALSE(awaitable.is_set());

  pdcp_rx->stop();

  // Awaitable is not set yet. Flushing the crypto and UL workers
  // tasks are required for the waitable to be set.
  ASSERT_FALSE(awaitable.is_set());

  // Process one PDU, one more remains in the queue.
  wait_one_crypto_task();
  worker.run_pending_tasks();

  // Token count should be 1 still, double check flag is not set.
  ASSERT_FALSE(awaitable.is_set());

  // Process second PDU in the crypto engine.
  wait_one_crypto_task();

  // Awaitable is not set yet. The UL worker still requires to be flushed.
  ASSERT_FALSE(awaitable.is_set());

  worker.run_pending_tasks();

  // Awaitable is now set.
  ASSERT_TRUE(awaitable.is_set());
}

/// Test stopping PDCP RX entities when PDUs are present.
TEST_P(pdcp_rx_stop_test, stop_when_integrity_failed)
{
  init(GetParam());
  srsran::test_delimit_logger delimiter("Normal stop test. SN_SIZE={} ", sn_size);

  pdcp_rx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

  uint32_t count = 0;

  // Generate test PDUs.
  byte_buffer test_pdu1;
  get_test_pdu(count, test_pdu1);
  byte_buffer test_pdu2;
  get_test_pdu(count + 1, test_pdu2);

  ASSERT_TRUE(test_pdu2.append(0)); // mess up MAC-I

  // Set PDCP state.
  pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
  pdcp_rx->set_state(init_state);

  // Handle PDUs, but don't run the crypto worker
  pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu1)).value());
  pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu2)).value());

  // Check await rx crypto flag is not set.
  manual_event_flag& awaitable = pdcp_rx->crypto_awaitable();

  ASSERT_FALSE(awaitable.is_set());

  pdcp_rx->stop();

  // Awaitable is not set yet. Flushing the crypto and UL workers
  // tasks are required for the waitable to be set.
  ASSERT_FALSE(awaitable.is_set());

  wait_pending_crypto();

  // Awaitable is not set yet. The UL worker still requires to be flushed.
  ASSERT_FALSE(awaitable.is_set());

  worker.run_pending_tasks();

  // Awaitable is now set.
  ASSERT_TRUE(awaitable.is_set());
}
///////////////////////////////////////////////////////////////////
// Finally, instantiate all testcases for each supported SN size //
///////////////////////////////////////////////////////////////////
static std::string test_param_info_to_string(const ::testing::TestParamInfo<std::tuple<pdcp_sn_size, unsigned>>& info)
{
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer),
                 "{}bit_nia{}_nea{}",
                 pdcp_sn_size_to_uint(std::get<pdcp_sn_size>(info.param)),
                 std::get<unsigned>(info.param),
                 std::get<unsigned>(info.param));
  return fmt::to_string(buffer);
}

INSTANTIATE_TEST_SUITE_P(pdcp_rx_stop_test_all_sn_sizes,
                         pdcp_rx_stop_test,
                         ::testing::Combine(::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                                            ::testing::Values(1, 2, 3)),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
