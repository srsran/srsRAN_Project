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
#include "srsran/ofh/receiver/ofh_receiver_metrics_collector.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul sector metrics collector implementation.
class metrics_collector_impl : public metrics_collector
{
  const unsigned              sector_id;
  const bool                  is_enabled = false;
  receiver_metrics_collector& rx_metrics_collector;

public:
  metrics_collector_impl(receiver_metrics_collector* rx_metrics_collector_, bool is_enabled_, unsigned sector_id_);

  // See interface for documentation.
  void collect_metrics(sector_metrics& metric) override;

  /// Returns true if the metrics are disabled, otherwise true.
  bool disabled() const { return !is_enabled; }
};

} // namespace ofh
} // namespace srsran
