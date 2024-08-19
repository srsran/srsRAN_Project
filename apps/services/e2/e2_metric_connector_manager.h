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

#include <cassert>
#include <memory>
#include <vector>

namespace srsran {

/// Manages the E2 metric connectors of the app.
template <typename ConnectorType, typename NotifierType, typename InterfaceType>
class e2_metric_connector_manager
{
public:
  e2_metric_connector_manager(unsigned nof_cells = 1)
  {
    for (unsigned i = 0, e = nof_cells; i != e; ++i) {
      e2_metric_connectors.push_back(std::make_shared<ConnectorType>());
    }
  }

  e2_metric_connector_manager(e2_metric_connector_manager&& other) noexcept :
    e2_metric_connectors(std::move(other.e2_metric_connectors))
  {
  }

  NotifierType& get_e2_metric_notifier(unsigned index)
  {
    assert(index < e2_metric_connectors.size() && "Invalid index");
    return *(e2_metric_connectors[index]);
  }
  InterfaceType& get_e2_metrics_interface(unsigned index)
  {
    assert(index < e2_metric_connectors.size() && "Invalid index");
    return *(e2_metric_connectors[index]);
  }

private:
  std::vector<std::shared_ptr<ConnectorType>> e2_metric_connectors;
};

} // namespace srsran
