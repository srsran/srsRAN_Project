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

#include "rlc_tx_metrics_test.h"
#include "srsran/rlc/rlc_tx_metrics.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST_F(rlc_tx_metrics_test, poll_latency_empty)
{
  {
    // Check custom formatter
    rlc_am_tx_metrics_poll_latency m = {};
    fmt::memory_buffer             buffer;
    format_rlc_am_tx_t_poll_latencty_metrics(buffer, m);
    std::string out_str = to_c_str(buffer);
    std::string exp_str = " t_poll_nof_expiration=0";
    logger.info("out_str={}", out_str);
    logger.info("exp_str={}", exp_str);
    EXPECT_EQ(out_str, exp_str);
  }
}

TEST_F(rlc_tx_metrics_test, poll_latency_full)
{
  {
    // Check custom formatter
    rlc_am_tx_metrics_poll_latency m = {};

    m.num_latency_meas = 2;
    m.sum_latency_us   = 600;
    m.min_latency_us   = 200;
    m.max_latency_us   = 400;

    fmt::memory_buffer buffer;
    format_rlc_am_tx_t_poll_latencty_metrics(buffer, m);
    std::string out_str = to_c_str(buffer);
    std::string exp_str =
        " t_poll_nof_expiration=2 t_poll_latency_avg=300.0us t_poll_latency_min=200us t_poll_latency_max=400us";
    logger.info("out_str={}", out_str);
    logger.info("exp_str={}", exp_str);
    EXPECT_EQ(out_str, exp_str);
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
