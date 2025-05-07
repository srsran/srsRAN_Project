/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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

/// Radio metrics are independent of the enabled flag. Print them.
static void print_radio_metrics(const ru_metrics& metrics)
{
  const auto* sdr_metrics = std::get_if<ru_generic_metrics>(&metrics.metrics);

  // Only valid for SDR metrics.
  if (!sdr_metrics) {
    return;
  }

  // Print late, underflow and overflow counts since last print.
  const radio_metrics& radio = sdr_metrics->radio;
  if ((radio.late_count != 0) || (radio.underflow_count != 0) || (radio.overflow_count != 0)) {
    fmt::println(
        "Late: {}; Underflow: {}; Overflow: {};", radio.late_count, radio.underflow_count, radio.overflow_count);
  }
}

void ru_metrics_consumer_stdout::handle_metric(const app_services::metrics_set& metric)
{
  const ru_metrics& metrics = static_cast<const flexible_o_du_app_service_metrics_impl&>(metric).get_metrics().ru;

  // Radio metrics are always printed.
  print_radio_metrics(metrics);

  if (!print_metrics.load(std::memory_order_relaxed)) {
    return;
  }

  handler.handle_metric(metrics);
}
