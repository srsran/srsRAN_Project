/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#include "lib/rlc/rlc_metrics_aggregator.h"
#include "srsran/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsran;

/// Fixture class for RLC metrics aggregator tests
class rlc_metrics_aggregator_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init RLC logger
    srslog::fetch_basic_logger("RLC", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("RLC", false).set_hex_dump_max_size(100);

    // Create RLC AM TX entity
    metrics_agg = std::make_unique<rlc_metrics_aggregator>();
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
  std::unique_ptr<rlc_metrics_aggregator> metrics_agg;
  srslog::basic_logger&                   logger = srslog::fetch_basic_logger("TEST", false);
};

TEST_F(rlc_metrics_aggregator_test, check_basic_aggregation) {}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
