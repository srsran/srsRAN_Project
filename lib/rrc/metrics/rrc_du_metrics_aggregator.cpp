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

#include "rrc_du_metrics_aggregator.h"

using namespace srsran;
using namespace srs_cu_cp;

rrc_du_metrics_aggregator::rrc_du_metrics_aggregator()
{
  rrc_connection_metrics.reset();
}

void rrc_du_metrics_aggregator::aggregate_successful_rrc_setup()
{
  rrc_connection_metrics.add_rrc_connection();
}

void rrc_du_metrics_aggregator::aggregate_successful_rrc_release()
{
  rrc_connection_metrics.remove_rrc_connection();
}

void rrc_du_metrics_aggregator::collect_metrics(rrc_du_metrics& metrics)
{
  metrics.mean_nof_rrc_connections = rrc_connection_metrics.get_mean_nof_rrc_connections();
  metrics.max_nof_rrc_connections  = rrc_connection_metrics.get_max_nof_rrc_connections();
  rrc_connection_metrics.reset();
}
