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
  connection_metrics.reset();
}

void rrc_du_metrics_aggregator::aggregate_successful_rrc_setup()
{
  connection_metrics.add_rrc_connection();
}

void rrc_du_metrics_aggregator::aggregate_successful_rrc_release()
{
  connection_metrics.remove_rrc_connection();
}

void rrc_du_metrics_aggregator::aggregate_attempted_connection_establishment(establishment_cause_t cause)
{
  connection_establishment_metrics.attempted_rrc_connection_establishments.increase(cause);
}

void rrc_du_metrics_aggregator::aggregate_successful_connection_establishment(establishment_cause_t cause)
{
  connection_establishment_metrics.successful_rrc_connection_establishments.increase(cause);
}

void rrc_du_metrics_aggregator::aggregate_attempted_connection_reestablishment()
{
  ++connection_reestablishment_metrics.attempted_rrc_connection_reestablishments;
}

void rrc_du_metrics_aggregator::aggregate_successful_connection_reestablishment(bool with_ue_context)
{
  if (with_ue_context) {
    ++connection_reestablishment_metrics.successful_rrc_connection_reestablishments_with_ue_context;
  } else {
    ++connection_reestablishment_metrics.successful_rrc_connection_reestablishments_without_ue_context;
  }
}

void rrc_du_metrics_aggregator::collect_metrics(rrc_du_metrics& metrics)
{
  metrics.mean_nof_rrc_connections = connection_metrics.get_mean_nof_rrc_connections();
  metrics.max_nof_rrc_connections  = connection_metrics.get_max_nof_rrc_connections();
  metrics.attempted_rrc_connection_establishments =
      connection_establishment_metrics.attempted_rrc_connection_establishments;
  metrics.successful_rrc_connection_establishments =
      connection_establishment_metrics.successful_rrc_connection_establishments;
  metrics.attempted_rrc_connection_reestablishments =
      connection_reestablishment_metrics.attempted_rrc_connection_reestablishments;
  metrics.successful_rrc_connection_reestablishments_with_ue_context =
      connection_reestablishment_metrics.successful_rrc_connection_reestablishments_with_ue_context;
  metrics.successful_rrc_connection_reestablishments_without_ue_context =
      connection_reestablishment_metrics.successful_rrc_connection_reestablishments_without_ue_context;
  connection_metrics.reset();
}
