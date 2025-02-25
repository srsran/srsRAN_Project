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

#include "cu_cp_pdcp_metrics_consumers.h"
#include "cu_cp_pdcp_metrics.h"
#include "srsran/pdcp/pdcp_metrics.h"

using namespace srsran;

void cu_cp_pdcp_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  // Implement me and remove this line.
  log_chan.enabled();
}
