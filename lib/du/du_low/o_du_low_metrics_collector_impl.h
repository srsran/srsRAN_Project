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

#include "srsran/du/du_low/o_du_low_metrics_collector.h"

namespace srsran {
namespace srs_du {

class du_low_metrics_collector;

/// O-DU low metrics collector implementation.
class o_du_low_metrics_collector_impl : public o_du_low_metrics_collector
{
  du_low_metrics_collector* collector = nullptr;

public:
  explicit o_du_low_metrics_collector_impl(du_low_metrics_collector* collector_) : collector(collector_) {}

  // See interface for documentation.
  void collect_metrics(o_du_low_metrics& metrics) override;

  /// Returns true if the metrics are enabled, otherwise false.
  bool enabled() const { return collector != nullptr; }
};

} // namespace srs_du
} // namespace srsran
