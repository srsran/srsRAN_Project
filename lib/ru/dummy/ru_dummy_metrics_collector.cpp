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
