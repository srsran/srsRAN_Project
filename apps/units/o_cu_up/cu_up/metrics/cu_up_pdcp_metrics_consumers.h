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

#include "cu_up_pdcp_metrics.h"
#include "srsran/srslog/log_channel.h"

namespace srsran {
class pdcp_metrics_notifier;

/// Consumer for the E2 CU-UP PDCP metrics.
class cu_up_pdcp_metrics_consumer_e2 : public app_services::metrics_consumer
{
public:
  explicit cu_up_pdcp_metrics_consumer_e2(pdcp_metrics_notifier& notifier_) : notifier(notifier_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  pdcp_metrics_notifier& notifier;
};

/// Consumer for the json CU-UP PDCP metrics.
class cu_up_pdcp_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  explicit cu_up_pdcp_metrics_consumer_json(srslog::log_channel& log_chan_) : log_chan(log_chan_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  srslog::log_channel& log_chan;
};

} // namespace srsran
