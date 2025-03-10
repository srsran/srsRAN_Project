/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ofh/ofh_sector_metrics_collector.h"
#include "srsran/ru/ru_metrics_collector.h"
#include <vector>

namespace srsran {

/// Open Fronthaul Radio Unit metrics collector implementation.
class ru_ofh_metrics_collector_impl : public ru_metrics_collector
{
  std::vector<ofh::metrics_collector*> sector_metrics_collectors;

public:
  ru_ofh_metrics_collector_impl() = default;

  explicit ru_ofh_metrics_collector_impl(std::vector<ofh::metrics_collector*> sector_metrics_collectors_) :
    sector_metrics_collectors(std::move(sector_metrics_collectors_))
  {
  }

  // See interface for documentation.
  void collect_metrics(ru_metrics& metrics) override;

  /// Returns true if the metrics collector is disabled, otherwise true.
  bool disabled() const { return sector_metrics_collectors.empty(); }
};

} // namespace srsran
