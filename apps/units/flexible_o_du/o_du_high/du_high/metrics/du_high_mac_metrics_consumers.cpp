/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_high_mac_metrics_consumers.h"
#include "srsran/mac/mac_metrics.h"

using namespace srsran;

void mac_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  // Implement me!
  log_chan.enabled();
}
