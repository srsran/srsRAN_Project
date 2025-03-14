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

#include "du_low_metrics_collector_impl.h"
#include "srsran/du/du_low/du_low_metrics.h"

using namespace srsran;
using namespace srs_du;

void du_low_metrics_collector_impl::collect_metrics(du_low_metrics& metrics)
{
  unsigned sector_id = 0;
  for (auto collector : upper_phy_collectors) {
    auto& sector_metrics     = metrics.sector_metrics.emplace_back();
    sector_metrics.sector_id = sector_id++;
    collector->collect_metrics(sector_metrics);
  }
}
