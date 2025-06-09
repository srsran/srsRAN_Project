/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split6_flexible_o_du_low_metrics_consumers.h"
#include "split6_flexible_o_du_low_app_service_metrics.h"
#include "split6_flexible_o_du_low_metrics.h"

using namespace srsran;

void split6_flexible_o_du_low_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  const split6_flexible_o_du_low_metrics& odu_metrics =
      static_cast<const split6_flexible_o_du_low_app_service_metrics_impl&>(metric).get_metrics();

  odu_low_metrics_handler.handle_metric(odu_metrics.du_low);
  ru_metrics_handler.handle_metric(odu_metrics.ru);
}

void split6_flexible_o_du_low_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  const split6_flexible_o_du_low_metrics& odu_metrics =
      static_cast<const split6_flexible_o_du_low_app_service_metrics_impl&>(metric).get_metrics();

  odu_low_metrics_handler.handle_metric(odu_metrics.du_low);
  ru_metrics_handler.handle_metric(odu_metrics.ru);
}
