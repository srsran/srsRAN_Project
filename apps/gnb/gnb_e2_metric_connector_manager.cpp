/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "gnb_e2_metric_connector_manager.h"

using namespace srsran;

e2_metric_connector_manager::e2_metric_connector_manager(const gnb_appconfig& appcfg)
{
  for (unsigned i = 0, e = appcfg.cells_cfg.size(); i != e; ++i) {
    e2_du_metric_connectors.push_back(std::make_unique<e2_du_metrics_connector>());
  }
}

e2_du_metrics_notifier& e2_metric_connector_manager::get_e2_du_metric_notifier(unsigned du_index)
{
  srsran_assert(du_index < e2_du_metric_connectors.size(), "Invalid DU index");
  return *e2_du_metric_connectors[du_index];
}

e2_du_metrics_interface& e2_metric_connector_manager::get_e2_du_metrics_interface(unsigned du_index)
{
  srsran_assert(du_index < e2_du_metric_connectors.size(), "Invalid DU index");
  return *e2_du_metric_connectors[du_index];
}
