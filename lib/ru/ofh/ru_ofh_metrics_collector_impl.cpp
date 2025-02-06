/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  // Resize with the number of sectors.
  ofh_metrics.sectors.resize(sector_metrics_collectors.size());

  // Collect the metrics from each sector.
  for (unsigned i = 0, e = sector_metrics_collectors.size(); i != e; ++i) {
    sector_metrics_collectors[i]->collect_metrics(ofh_metrics.sectors[i]);
  }
}
