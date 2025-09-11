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

#include "ru_ofh_metrics_collector_impl.h"
#include "srsran/ru/ru_metrics.h"

using namespace srsran;

void ru_ofh_metrics_collector_impl::collect_metrics(ru_metrics& metrics)
{
  auto& ofh_metrics = metrics.metrics.emplace<ofh::metrics>();

  // Add timestamp to the metric.
  ofh_metrics.timestamp = std::chrono::system_clock::now();

  // Timing metrics.
  timing_collector->collect_metrics(ofh_metrics.timing);

  // Resize with the number of sectors.
  ofh_metrics.sectors.resize(sector_metrics_collectors.size());

  // Collect the metrics from each sector.
  for (unsigned i = 0, e = sector_metrics_collectors.size(); i != e; ++i) {
    sector_metrics_collectors[i]->collect_metrics(ofh_metrics.sectors[i]);
  }
}
