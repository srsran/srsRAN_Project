/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
