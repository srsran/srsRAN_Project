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

void rrc_du_metrics_aggregator::collect_metrics(rrc_du_metrics& metrics)
{
  metrics.mean_nof_rrc_connections = connection_metrics.get_mean_nof_rrc_connections();
  metrics.max_nof_rrc_connections  = connection_metrics.get_max_nof_rrc_connections();
  metrics.attempted_rrc_connection_establishments =
      connection_establishment_metrics.attempted_rrc_connection_establishments;
  metrics.successful_rrc_connection_establishments =
      connection_establishment_metrics.successful_rrc_connection_establishments;
  connection_metrics.reset();
}
