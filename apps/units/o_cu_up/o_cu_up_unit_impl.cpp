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

#include "o_cu_up_unit_impl.h"

using namespace srsran;

o_cu_up_unit_impl::o_cu_up_unit_impl(std::vector<std::unique_ptr<gtpu_gateway>>       gateways_,
                                     std::unique_ptr<e2_cu_metrics_connector_manager> e2_metric_connector_,
                                     std::unique_ptr<srs_cu_up::o_cu_up>              cu_up_) :
  gateways(std::move(gateways_)), e2_metric_connector(std::move(e2_metric_connector_)), cu_up(std::move(cu_up_))
{
  srsran_assert(not gateways.empty(), "Invalid NG-U gateway");
  srsran_assert(cu_up, "Invalid CU-UP");
}

srs_cu_up::cu_up_interface& o_cu_up_unit_impl::get_cu_up()
{
  return cu_up->get_cu_up();
}

srs_cu_up::cu_up_operation_controller& o_cu_up_unit_impl::get_operation_controller()
{
  return cu_up->get_operation_controller();
}
