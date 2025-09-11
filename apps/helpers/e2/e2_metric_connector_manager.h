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

#include "srsran/support/srsran_assert.h"
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

  ~e2_metric_connector_manager() = default;

  e2_metric_connector_manager(e2_metric_connector_manager&& other) noexcept :
    e2_metric_connectors(std::move(other.e2_metric_connectors))
  {
  }

  NotifierType& get_e2_metric_notifier(unsigned index)
  {
    srsran_assert(index < e2_metric_connectors.size(), "Invalid index");
    return *(e2_metric_connectors[index]);
  }
  InterfaceType& get_e2_metrics_interface(unsigned index)
  {
    srsran_assert(index < e2_metric_connectors.size(), "Invalid index");
    return *(e2_metric_connectors[index]);
  }

private:
  std::vector<std::shared_ptr<ConnectorType>> e2_metric_connectors;
};

} // namespace srsran
