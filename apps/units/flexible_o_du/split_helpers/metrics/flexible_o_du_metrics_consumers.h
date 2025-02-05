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

#include "apps/services/metrics/metrics_consumer.h"

namespace srsran {

/// Dummy consumer for the flexible O-DU metrics.
class flexible_o_du_metrics_consumer_dummy : public app_services::metrics_consumer
{
public:
  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;
};

} // namespace srsran
