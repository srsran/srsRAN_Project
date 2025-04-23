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

#include "o_cu_cp_unit_impl.h"

using namespace srsran;
using namespace srs_cu_cp;

o_cu_cp_unit_impl::o_cu_cp_unit_impl(std::vector<std::unique_ptr<srs_cu_cp::n2_connection_client>> n2_clients_,
                                     std::unique_ptr<e2_cu_metrics_connector_manager>              e2_metric_connector_,
                                     std::unique_ptr<srs_cu_cp::o_cu_cp>                           cu_cp_) :
  n2_clients(std::move(n2_clients_)), e2_metric_connector(std::move(e2_metric_connector_)), o_cu(std::move(cu_cp_))
{
  srsran_assert(o_cu, "Invalid O-CU-CP object");
}

srs_cu_cp::cu_cp& o_cu_cp_unit_impl::get_cu_cp()
{
  return o_cu->get_cu_cp();
}

srs_cu_cp::cu_cp_operation_controller& o_cu_cp_unit_impl::get_operation_controller()
{
  return o_cu->get_operation_controller();
}
