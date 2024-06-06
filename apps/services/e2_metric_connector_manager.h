/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/e2/e2_du_metrics_connector.h"

namespace srsran {

/// Manages the E2 metric connectors of the app.
class e2_metric_connector_manager
{
public:
  explicit e2_metric_connector_manager(unsigned nof_cells);

  std::vector<std::unique_ptr<e2_du_metrics_connector>> e2_du_metric_connectors;
  e2_du_metrics_notifier&                               get_e2_du_metric_notifier(unsigned du_index);
  e2_du_metrics_interface&                              get_e2_du_metrics_interface(unsigned du_index);
};

} // namespace srsran
