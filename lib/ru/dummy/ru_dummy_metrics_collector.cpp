/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_dummy_metrics_collector.h"
#include "srsran/ru/ru_metrics.h"

using namespace srsran;

void ru_dummy_metrics_collector::collect_metrics(ru_metrics& metrics)
{
  auto& dummy_metrics = metrics.metrics.emplace<ru_dummy_metrics>();
  dummy_metrics.sectors.resize(sectors.size());
  unsigned i = 0;

  // Collect metrics from each sector.
  for (auto& sector : sectors) {
    ru_dummy_sector_metrics& cell_metrics = dummy_metrics.sectors[i];
    cell_metrics.sector_id                = i++;
    sector.collect_metrics(cell_metrics);
  }
}
