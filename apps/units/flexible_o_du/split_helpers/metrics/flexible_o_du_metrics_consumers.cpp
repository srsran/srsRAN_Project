/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "flexible_o_du_metrics_consumers.h"
#include "flexible_o_du_app_service_metrics.h"

using namespace srsran;

void flexible_o_du_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  const flexible_o_du_metrics& odu_metrics =
      static_cast<const flexible_o_du_app_service_metrics_impl&>(metric).get_metrics();

  odu_low_metrics_handler.handle_metric(odu_metrics.du.low);
  ru_metrics_handler.handle_metric(odu_metrics.ru);
}

void flexible_o_du_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  const flexible_o_du_metrics& odu_metrics =
      static_cast<const flexible_o_du_app_service_metrics_impl&>(metric).get_metrics();

  odu_low_metrics_handler.handle_metric(odu_metrics.du.low);
  ru_metrics_handler.handle_metric(odu_metrics.ru);
}
