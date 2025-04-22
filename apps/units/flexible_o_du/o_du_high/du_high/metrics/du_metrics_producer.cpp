/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_metrics_producer.h"
#include "du_metrics.h"

using namespace srsran;

void du_metrics_producer_impl::on_new_metric_report(const srs_du::du_metrics_report& report)
{
  notifier.on_new_metric(du_metrics_impl(report));
}
