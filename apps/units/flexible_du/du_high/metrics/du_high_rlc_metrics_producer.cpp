/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_high_rlc_metrics_producer.h"
#include "du_high_rlc_metrics.h"

using namespace srsran;

void rlc_metrics_producer_impl::report_metrics(const rlc_metrics& report)
{
  notifier.on_new_metric(rlc_metrics_impl(report));
}
