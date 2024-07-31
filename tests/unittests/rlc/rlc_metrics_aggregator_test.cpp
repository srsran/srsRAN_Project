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
#include "lib/rlc/rlc_metrics_aggregator.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;

class mock_rlc_metrics_notifier : public rlc_metrics_notifier
{
  void report_metrics(const rlc_metrics& metrics) override { metrics_list.push_back(metrics); }

public:
  std::vector<rlc_metrics> metrics_list;
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
    metrics_agg   = std::make_unique<rlc_metrics_aggregator>(
        gnb_du_id_t{}, du_ue_index_t{}, rb_id_t{}, timer_duration{1000}, metrics_notif.get(), ue_worker);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
  std::unique_ptr<rlc_metrics_aggregator>    metrics_agg;
  std::unique_ptr<mock_rlc_metrics_notifier> metrics_notif;
  srslog::basic_logger&                      logger = srslog::fetch_basic_logger("TEST", false);

  manual_task_worker ue_worker{128};
};

TEST_F(rlc_metrics_aggregator_test, check_basic_aggregation)
{
  // Check if pushing two metrics causes the metrics to be pushed.
  {
    rlc_tx_metrics_higher hi_m;
    rlc_tx_metrics_lower  low_m;
    rlc_rx_metrics        rx_m;

    hi_m.num_sdus                  = 10;
    low_m.num_pdus_no_segmentation = 10;
    rx_m.num_sdus                  = 20;

    metrics_agg->push_tx_high_metrics(hi_m);
    ASSERT_EQ(metrics_notif->metrics_list.size(), 0);

    metrics_agg->push_tx_low_metrics(low_m);
    ASSERT_EQ(metrics_notif->metrics_list.size(), 0);

    metrics_agg->push_rx_high_metrics(rx_m);
    ASSERT_EQ(metrics_notif->metrics_list.size(), 1);
    // TODO check report
  }

  metrics_notif->metrics_list.clear();

  // Same, but reversed order of pushing.
  {
    rlc_tx_metrics_higher hi_m;
    rlc_tx_metrics_lower  low_m;
    rlc_rx_metrics        rx_m;

    hi_m.num_sdus                  = 10;
    hi_m.counter                   = 1;
    low_m.num_pdus_no_segmentation = 10;
    low_m.counter                  = 1;
    rx_m.num_sdus                  = 20;
    rx_m.counter                   = 1;

    metrics_agg->push_rx_high_metrics(rx_m);
    ASSERT_EQ(metrics_notif->metrics_list.size(), 0);

    metrics_agg->push_tx_low_metrics(low_m);
    ASSERT_EQ(metrics_notif->metrics_list.size(), 0);

    metrics_agg->push_tx_high_metrics(hi_m);
    ASSERT_EQ(metrics_notif->metrics_list.size(), 1);

    // TODO check report
  }
}

TEST_F(rlc_metrics_aggregator_test, drop_one_report)
{
  // Check if pushing two metrics causes the metrics to be pushed.
  {
    rlc_tx_metrics_higher hi_m;
    rlc_tx_metrics_lower  low_m;
    rlc_rx_metrics        rx_m;

    hi_m.num_sdus                  = 10;
    low_m.num_pdus_no_segmentation = 10;
    rx_m.num_sdus                  = 20;

    metrics_agg->push_tx_high_metrics(hi_m);
    ASSERT_EQ(metrics_notif->metrics_list.size(), 0);

    metrics_agg->push_tx_low_metrics(low_m);
    ASSERT_EQ(metrics_notif->metrics_list.size(), 0);

    // don't push RX report
  }

  metrics_notif->metrics_list.clear();

  // Now push counter == 1
  {
    rlc_tx_metrics_higher hi_m;
    rlc_tx_metrics_lower  low_m;
    rlc_rx_metrics        rx_m;

    hi_m.num_sdus                  = 10;
    hi_m.counter                   = 1;
    low_m.num_pdus_no_segmentation = 10;
    low_m.counter                  = 1;
    rx_m.num_sdus                  = 20;
    rx_m.counter                   = 1;

    metrics_agg->push_rx_high_metrics(rx_m);
    ASSERT_EQ(metrics_notif->metrics_list.size(), 0);

    metrics_agg->push_tx_low_metrics(low_m);
    ASSERT_EQ(metrics_notif->metrics_list.size(), 0);

    metrics_agg->push_tx_high_metrics(hi_m);
    ASSERT_EQ(metrics_notif->metrics_list.size(), 1);

    // TODO check report
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
