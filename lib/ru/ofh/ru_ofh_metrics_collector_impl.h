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

#pragma once

#include "srsran/ofh/ofh_sector_metrics_collector.h"
#include "srsran/ofh/timing/ofh_timing_metrics_collector.h"
#include "srsran/ru/ru_metrics_collector.h"
#include <vector>

namespace srsran {

/// Open Fronthaul Radio Unit metrics collector implementation.
class ru_ofh_metrics_collector_impl : public ru_metrics_collector
{
  ofh::timing_metrics_collector*       timing_collector;
  std::vector<ofh::metrics_collector*> sector_metrics_collectors;

public:
  explicit ru_ofh_metrics_collector_impl(ofh::timing_metrics_collector& timing_collector_) :
    timing_collector(&timing_collector_)
  {
  }

  ru_ofh_metrics_collector_impl(ofh::timing_metrics_collector&       timing_collector_,
                                std::vector<ofh::metrics_collector*> sector_metrics_collectors_) :
    timing_collector(&timing_collector_), sector_metrics_collectors(std::move(sector_metrics_collectors_))
  {
  }

  // See interface for documentation.
  void collect_metrics(ru_metrics& metrics) override;

  /// Returns true if the metrics collector is disabled, otherwise true.
  bool disabled() const { return sector_metrics_collectors.empty(); }
};

} // namespace srsran
