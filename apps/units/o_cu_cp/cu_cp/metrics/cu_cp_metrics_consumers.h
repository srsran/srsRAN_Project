/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "apps/services/metrics/metrics_consumer.h"
#include "apps/units/o_cu_cp/cu_cp/metrics/consumers/ngap_metrics_consumers.h"
#include "apps/units/o_cu_cp/cu_cp/metrics/consumers/rrc_metrics_consumers.h"
#include "srsran/srslog/log_channel.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

/// Consumer for the json CU-CP metrics.
class cu_cp_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  explicit cu_cp_metrics_consumer_json(srslog::log_channel& log_chan_) : log_chan(log_chan_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  srslog::log_channel& log_chan;
};

/// Consumer for the log CU-CP metrics.
class cu_cp_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit cu_cp_metrics_consumer_log(srslog::log_channel& log_chan_) :
    ngap_consumer(log_chan_), rrc_consumer(log_chan_)
  {
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  ngap_metrics_consumer_log ngap_consumer;
  rrc_metrics_consumer_log  rrc_consumer;
};

} // namespace srsran
