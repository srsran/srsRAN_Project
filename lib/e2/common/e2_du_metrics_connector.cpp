/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/e2/e2_du_metrics_connector.h"

using namespace srsran;

e2_du_metrics_connector::e2_du_metrics_connector()
{
  ue_metrics_queue.resize(MAX_UE_METRICS);
}

void e2_du_metrics_connector::report_metrics(span<const scheduler_ue_metrics> ue_metrics)
{
  for (auto& ue_metric : ue_metrics) {
    if (ue_metrics_queue.size() == MAX_UE_METRICS) {
      ue_metrics_queue.pop_front();
    }
    ue_metrics_queue.push_back(ue_metric);
  }

  if (e2_meas_provider) {
    // Pass metrics to the E2 Measurement Provider.
    e2_meas_provider->report_metrics(ue_metrics);
  }
}

void e2_du_metrics_connector::report_metrics(const rlc_metrics& metrics)
{
  if (e2_meas_provider) {
    // Pass metrics to the E2 Measurement Provider.
    e2_meas_provider->report_metrics(metrics);
  }
}

void e2_du_metrics_connector::get_metrics(scheduler_ue_metrics& ue_metrics)
{
  if (ue_metrics_queue.empty()) {
    return;
  }
  ue_metrics = ue_metrics_queue.front();
}

void e2_du_metrics_connector::connect_e2_du_meas_provider(std::unique_ptr<e2_du_metrics_notifier> meas_provider)
{
  e2_meas_provider = std::move(meas_provider);
}