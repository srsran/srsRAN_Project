/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_metrics_collector_generic_impl.h"
#include "srsran/ru/ru_metrics.h"

using namespace srsran;

void ru_metrics_collector_generic_impl::collect_metrics(ru_metrics& metrics)
{
  auto& ru_metrics = metrics.metrics.emplace<ru_generic_metrics>();
  ru_metrics.cells.resize(sectors.size());

  unsigned sector_id = 0;
  for (auto& sector : sectors) {
    ru_generic_sector_metrics& cell_metric = ru_metrics.cells[sector_id];
    cell_metric.sector_id                  = sector_id++;
    sector->get_metrics_collector().collect_metrics(cell_metric);
  }
}
