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

class mock_rlc_metrics_notifier : public rlc_metrics_notifier
{
  void report_metrics(const rlc_metrics& metrics) override {}
};
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

    // Create mock metrics notifier and RLC AM TX entity
    metrics_notif = std::make_unique<mock_rlc_metrics_notifier>();
    metrics_agg   = std::make_unique<rlc_metrics_aggregator>(metrics_notif.get());
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
  std::unique_ptr<rlc_metrics_aggregator>    metrics_agg;
  std::unique_ptr<mock_rlc_metrics_notifier> metrics_notif;
  srslog::basic_logger&                      logger = srslog::fetch_basic_logger("TEST", false);
};

TEST_F(rlc_metrics_aggregator_test, check_basic_aggregation)
{
  rlc_tx_metrics_higher hi_m;
  rlc_tx_metrics_lower  low_m;

  hi_m.num_sdus                  = 10;
  low_m.num_pdus_no_segmentation = 10;

  metrics_agg->push_tx_high_metrics(hi_m);
  metrics_agg->push_tx_low_metrics(low_m);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
