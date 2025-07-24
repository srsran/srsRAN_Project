/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_metrics_producer.h"
#include "cu_cp_metrics.h"

using namespace srsran;

void cu_cp_metrics_producer_impl::notify_metrics_report_request(const srs_cu_cp::metrics_report& report)
{
  notifier.on_new_metric(cu_cp_metrics_impl(report));
}
