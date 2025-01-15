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

#include "du_high_rlc_metrics.h"
#include "srsran/srslog/log_channel.h"
#include "srsran/srslog/logger.h"

namespace srsran {

/// Consumer for the json RLC metrics.
class rlc_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  explicit rlc_metrics_consumer_json(srslog::log_channel& log_chan_) : log_chan(log_chan_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  srslog::log_channel& log_chan;
};

/// Consumer for the log RLC metrics.
class rlc_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit rlc_metrics_consumer_log(srslog::basic_logger& logger_) : logger(logger_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  srslog::basic_logger& logger;
};

/// Consumer for the E2 RLC metrics.
class rlc_metrics_consumer_e2 : public app_services::metrics_consumer
{
public:
  explicit rlc_metrics_consumer_e2(rlc_metrics_notifier& notifier_) : notifier(notifier_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  rlc_metrics_notifier& notifier;
};

} // namespace srsran
