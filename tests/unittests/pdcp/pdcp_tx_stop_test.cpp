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

#include "pdcp_tx_stop_test.h"
#include "pdcp_test_vectors.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsran;

/// \brief Test correct creation of PDCP TX  entity
TEST_P(pdcp_tx_stop_test, stop_when_there_are_no_pending_pdus)
{
  init(GetParam());
  srsran::test_delimit_logger delimiter("Normal stop test. SN_SIZE={} ", sn_size);

  pdcp_tx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

  // Check await tx crypto flag is not set.
  manual_event_flag& awaitable = pdcp_tx->crypto_awaitable();

  FLUSH_AND_ASSERT_FALSE(awaitable.is_set());

  pdcp_tx->stop();

  // Awaitable is now set.
  FLUSH_AND_ASSERT_TRUE(awaitable.is_set());
}

/// \brief Test correct packing of PDCP data PDU headers
TEST_P(pdcp_tx_stop_test, stop_when_there_are_pending_pdus)
{
  init(GetParam());
  srsran::test_delimit_logger delimiter("Normal stop test. SN_SIZE={} ", sn_size);

  pdcp_tx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

  // Push SDUs to entity, but don't run crypto executor.
  byte_buffer test_sdu1 = byte_buffer::create(sdu1).value();
  pdcp_tx->handle_sdu(std::move(test_sdu1));

  byte_buffer test_sdu2 = byte_buffer::create(sdu2).value();
  pdcp_tx->handle_sdu(std::move(test_sdu2));

  // Check await rx crypto flag is not set.
  manual_event_flag& awaitable = pdcp_tx->crypto_awaitable();

  FLUSH_AND_ASSERT_FALSE(awaitable.is_set());

  pdcp_tx->stop();

  // Awaitable is not set yet. Flushing the crypto and UL workers
  // tasks are required for the waitable to be set.
  FLUSH_AND_ASSERT_FALSE(awaitable.is_set());

  // Process one PDU, one more remains in the queue.
  wait_one_crypto_task();
  worker.run_pending_tasks();

  // Token count should be 1 still, double check flag is not set.
  FLUSH_AND_ASSERT_FALSE(awaitable.is_set());

  // Process second PDU in the crypto engine.
  wait_one_crypto_task();

  // Awaitable is not set yet. The UL worker still requires to be flushed.
  FLUSH_AND_ASSERT_FALSE(awaitable.is_set());

  worker.run_pending_tasks();

  // Awaitable is now set.
  FLUSH_AND_ASSERT_TRUE(awaitable.is_set());
}

/// \brief Test correct functioning when crypto engine drops PDUs
///
/// This test will:
/// 1. Push SDUs until the crypto executor queue is full and an SDU is dropped. This will cause a dropped SN.
/// 2. Run the crypto executor. This will flush the SDUs up to the lost one.
/// 3. Start pushing SDUs. These will stall awaiting for the lost SDU.
/// 4. Run the timers. The crypto reordering timer will expire and flush the stalled SDUs.
/// 5. Check SDU processing is back to normal.
TEST_P(pdcp_tx_stop_test, full_crypto_engine_does_not_stall)
{
  init(GetParam(), pdcp_rb_type::drb, pdcp_rlc_mode::am, pdcp_discard_timer::infinity);
  srsran::test_delimit_logger delimiter("Full crypto engine does not stall. SN_SIZE={} ", sn_size);

  pdcp_tx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

  // Push SDUs to entity, but don't run crypto executor.
  for (uint32_t count = 0; count < crypto_worker_qsize; count++) {
    byte_buffer test_sdu1 = byte_buffer::create(sdu1).value();
    pdcp_tx->handle_sdu(std::move(test_sdu1));
  }

  // This PDU should be dropped.
  byte_buffer dropped_sdu = byte_buffer::create(sdu1).value();
  pdcp_tx->handle_sdu(std::move(dropped_sdu));

  {
    pdcp_tx_metrics_container m = pdcp_tx->get_metrics_and_reset();
    FLUSH_AND_ASSERT_EQ(m.num_dropped_sdus, 1);
  }
  // Empty the crypto and DL worker queues.
  wait_pending_crypto();
  wait_pending_dl();

  {
    pdcp_tx_metrics_container m = pdcp_tx->get_metrics_and_reset();
    FLUSH_AND_ASSERT_EQ(m.num_pdus, crypto_worker_qsize);
  }

  // Send PDUs again. This should stalls, waiting for the lost PDU.
  for (uint32_t count = 0; count < crypto_worker_qsize; count++) {
    byte_buffer test_sdu1 = byte_buffer::create(sdu1).value();
    pdcp_tx->handle_sdu(std::move(test_sdu1));
  }
  wait_pending_crypto();
  wait_pending_dl();
  {
    pdcp_tx_metrics_container m = pdcp_tx->get_metrics_and_reset();
    logger.error("{}", m);
    FLUSH_AND_ASSERT_EQ(m.num_pdus, 0);
  }

  tick_all(10);
  {
    pdcp_tx_metrics_container m = pdcp_tx->get_metrics_and_reset();
    logger.error("{}", m);
    FLUSH_AND_ASSERT_EQ(m.num_pdus, 128);
  }

  // Check await rx crypto flag is not set.
  manual_event_flag& awaitable = pdcp_tx->crypto_awaitable();

  FLUSH_AND_ASSERT_FALSE(awaitable.is_set());

  for (uint32_t count = 0; count < crypto_worker_qsize; count++) {
    wait_one_crypto_task();
    wait_one_dl_worker_task();
  }

  pdcp_tx->stop();

  // Awaitable is now set.
  FLUSH_AND_ASSERT_TRUE(awaitable.is_set());
}

/// \brief Test correct functioning when DL worker drops PDUs
TEST_P(pdcp_tx_stop_test, full_dl_worker_does_not_stall)
{
  init(GetParam());
  srsran::test_delimit_logger delimiter("Full DL worker does not stall. SN_SIZE={} ", sn_size);

  pdcp_tx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

  // Push SDUs to entity, but don't run crypto executor.
  for (uint32_t count = 0; count < dl_worker_qsize; count++) {
    byte_buffer test_sdu1 = byte_buffer::create(sdu1).value();
    pdcp_tx->handle_sdu(std::move(test_sdu1));
    wait_one_crypto_task();
  }
  byte_buffer test_sdu1 = byte_buffer::create(sdu1).value();
  pdcp_tx->handle_sdu(std::move(test_sdu1));
  wait_one_crypto_task();

  // Check await rx crypto flag is not set.
  manual_event_flag& awaitable = pdcp_tx->crypto_awaitable();

  FLUSH_AND_ASSERT_FALSE(awaitable.is_set());

  wait_pending_dl();

  pdcp_tx->stop();

  // Awaitable is now set.
  FLUSH_AND_ASSERT_TRUE(awaitable.is_set());
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

INSTANTIATE_TEST_SUITE_P(pdcp_tx_stop_test_all_sn_sizes,
                         pdcp_tx_stop_test,
                         ::testing::Combine(::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                                            ::testing::Values(1, 2, 3)),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
