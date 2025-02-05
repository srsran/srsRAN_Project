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

namespace srsran {

struct ru_metrics;

/// Radio Unit metrics collector that collects metrics from the RU.
class ru_metrics_collector
{
public:
  virtual ~ru_metrics_collector() = default;

  virtual void collect_metrics(ru_metrics& metrics) = 0;
};

} // namespace srsran
