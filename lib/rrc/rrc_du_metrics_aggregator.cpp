/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_du_metrics_aggregator.h"
#include <thread>

using namespace srsran;
using namespace srs_cu_cp;

rrc_du_metrics_aggregator::rrc_du_metrics_aggregator()
{
  rrc_connection_metrics.reset();
}

void rrc_du_metrics_aggregator::handle_successful_rrc_setup()
{
  rrc_connection_metrics.add_rrc_connection();
}

void rrc_du_metrics_aggregator::handle_successful_rrc_release()
{
  rrc_connection_metrics.remove_rrc_connection();
}

rrc_du_metrics rrc_du_metrics_aggregator::request_metrics_report()
{
  rrc_du_metrics report;

  report.mean_nof_rrc_connections = rrc_connection_metrics.get_mean_nof_rrc_connections();
  report.max_nof_rrc_connections  = rrc_connection_metrics.get_max_nof_rrc_connections();
  rrc_connection_metrics.reset();

  return report;
}
