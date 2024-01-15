/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "pdcp_test_vectors.h"
#include "pdcp_tx_test.h"
#include "srsran/pdcp/pdcp_config.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsran;

constexpr uint32_t pool_size = 128;

/// Fixture class for PDCP TX tests
/// It requires TEST_P() and INSTANTIATE_TEST_SUITE_P() to create/spawn tests for each supported SN size
class pdcp_tx_empty_pool_test : public pdcp_tx_test_helper,
                                public ::testing::Test,
                                public ::testing::WithParamInterface<std::tuple<pdcp_sn_size, unsigned>>
{
protected:
  void SetUp() override
  {
    //  init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init RLC logger
    srslog::fetch_basic_logger("PDCP", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("PDCP", false).set_hex_dump_max_size(100);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
};

/// Test empty pool handling
TEST_P(pdcp_tx_empty_pool_test, empty_pool)
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
std::string test_param_info_to_string(const ::testing::TestParamInfo<std::tuple<pdcp_sn_size, unsigned>>& info)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "{}bit", pdcp_sn_size_to_uint(std::get<pdcp_sn_size>(info.param)));
  return fmt::to_string(buffer);
}

INSTANTIATE_TEST_SUITE_P(pdcp_tx_empty_pool_test_all_sn_sizes,
                         pdcp_tx_empty_pool_test,
                         ::testing::Combine(::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                                            ::testing::Values(1)),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  init_byte_buffer_segment_pool(pool_size);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
