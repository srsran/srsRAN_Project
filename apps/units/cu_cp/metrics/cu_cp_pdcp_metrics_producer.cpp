/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_pdcp_metrics_producer.h"
#include "cu_cp_pdcp_metrics.h"

using namespace srsran;

void pdcp_metrics_producer_impl::report_metrics(const pdcp_metrics_container& report)
{
  notifier.on_new_metric(pdcp_metrics_impl(report));
}
