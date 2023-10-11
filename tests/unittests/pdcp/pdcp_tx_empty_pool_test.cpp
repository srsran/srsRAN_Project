/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcp_test_vectors.h"
#include "pdcp_tx_test.h"
#include "srsran/pdcp/pdcp_config.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsran;

constexpr uint32_t pool_size = 32;

/// Test empty pool handling
TEST_P(pdcp_tx_test, empty_pool)
{
  init(GetParam());
  uint32_t n_sdus          = pool_size + 1;
  auto     test_empty_pool = [this, n_sdus](uint32_t tx_next) {
    // Set state of PDCP entiy
    pdcp_tx_state st = {tx_next, tx_next};
    pdcp_tx->set_state(st);
    pdcp_tx->configure_security(sec_cfg);

    // Write first SDU
    for (uint32_t i = 0; i < n_sdus; i++) {
      byte_buffer sdu = {sdu1};
      pdcp_tx->handle_sdu(std::move(sdu));
    }
    // check nof max_count reached and max protocol failures.
    ASSERT_NE(test_frame.pdu_queue.size(), n_sdus);
    ASSERT_GE(test_frame.nof_protocol_failure, 1);
  };

  test_empty_pool(0);
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

INSTANTIATE_TEST_SUITE_P(pdcp_tx_test_all_sn_sizes,
                         pdcp_tx_test,
                         ::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  init_byte_buffer_segment_pool(pool_size);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
