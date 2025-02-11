/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_pdcp_metrics_consumers.h"
#include "cu_cp_pdcp_metrics.h"
#include "srsran/pdcp/pdcp_metrics.h"

using namespace srsran;

void cu_cp_pdcp_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  // Implement me and remove this line.
  log_chan.enabled();
}
