/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_pdcp_metrics_consumers.h"
#include "cu_up_pdcp_metrics.h"
#include "srsran/pdcp/pdcp_metrics.h"

using namespace srsran;

void pdcp_metrics_consumer_e2::handle_metric(const app_services::metrics_set& metric)
{
  notifier.report_metrics(static_cast<const pdcp_metrics_impl&>(metric).get_metrics());
}
