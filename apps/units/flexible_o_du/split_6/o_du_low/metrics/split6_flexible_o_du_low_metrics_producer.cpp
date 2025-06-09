/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split6_flexible_o_du_low_metrics_producer.h"
#include "split6_flexible_o_du_low_app_service_metrics.h"
#include "split6_flexible_o_du_low_metrics.h"

using namespace srsran;

void split6_o_du_low_metrics_producer_impl::on_new_metrics(const split6_flexible_o_du_low_metrics& metrics)
{
  notifier.on_new_metric(split6_flexible_o_du_low_app_service_metrics_impl(metrics));
}
