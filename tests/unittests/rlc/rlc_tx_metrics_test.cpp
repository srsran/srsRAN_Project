/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
