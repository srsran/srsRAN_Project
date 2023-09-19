/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "gnb_appconfig.h"
#include "srsran/e2/e2_du_metrics_connector.h"

namespace srsran {

/// Manages the E2 metric connectors of the app.
class e2_metric_connector_manager
{
public:
  e2_metric_connector_manager(const gnb_appconfig& appcfg);

  std::vector<std::unique_ptr<e2_du_metrics_connector>> e2_du_metric_connectors;
  e2_du_metrics_notifier&                               get_e2_du_metric_notifier(unsigned du_index);
  e2_du_metrics_interface&                              get_e2_du_metrics_interface(unsigned du_index);
};

} // namespace srsran
