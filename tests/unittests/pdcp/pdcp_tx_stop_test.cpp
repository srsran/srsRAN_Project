/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  // Check await rx crypto flag is not set.
  manual_event_flag& awaitable = pdcp_tx->crypto_awaitable();

  ASSERT_FALSE(awaitable.is_set());

  pdcp_tx->stop();

  // Awaitable is now set.
  ASSERT_TRUE(awaitable.is_set());
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

  ASSERT_FALSE(awaitable.is_set());

  pdcp_tx->stop();

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

/// \brief Test correct functioning when crypto engine drops PDUs
TEST_P(pdcp_tx_stop_test, full_crypto_engine_does_not_stall)
{
  init(GetParam());
  srsran::test_delimit_logger delimiter("Normal stop test. SN_SIZE={} ", sn_size);

  pdcp_tx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

  // Push SDUs to entity, but don't run crypto executor.
  for (uint32_t count = 0; count < crypto_worker_qsize; count++) {
    byte_buffer test_sdu1 = byte_buffer::create(sdu1).value();
    pdcp_tx->handle_sdu(std::move(test_sdu1));
  }
  byte_buffer test_sdu1 = byte_buffer::create(sdu1).value();
  pdcp_tx->handle_sdu(std::move(test_sdu1));
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
