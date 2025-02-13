/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_high_mac_metrics_producer.h"
#include "du_high_mac_metrics.h"

using namespace srsran;

void mac_metrics_producer_impl::on_new_metrics_report(const mac_metric_report& report)
{
  notifier.on_new_metric(mac_metrics_impl(report));
}
