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

#include "cu_cp_wrapper.h"

using namespace srsran;
using namespace srs_cu_cp;

cu_cp_wrapper::cu_cp_wrapper(std::vector<std::unique_ptr<srs_cu_cp::n2_connection_client>> n2_clients_,
                             std::unique_ptr<srs_cu_cp::cu_cp>                             cu_cp_) :
  n2_clients(std::move(n2_clients_)), cu_cp(std::move(cu_cp_))
{
  srsran_assert(cu_cp, "Invalid CU-CP object");
}

cu_cp_f1c_handler& cu_cp_wrapper::get_f1c_handler()
{
  return cu_cp->get_f1c_handler();
}

cu_cp_e1_handler& cu_cp_wrapper::get_e1_handler()
{
  return cu_cp->get_e1_handler();
}

cu_cp_ng_handler& cu_cp_wrapper::get_ng_handler()
{
  return cu_cp->get_ng_handler();
}

cu_cp_command_handler& cu_cp_wrapper::get_command_handler()
{
  return cu_cp->get_command_handler();
}

metrics_handler& cu_cp_wrapper::get_metrics_handler()
{
  return cu_cp->get_metrics_handler();
}

bool cu_cp_wrapper::start()
{
  return cu_cp->start();
}

void cu_cp_wrapper::stop()
{
  cu_cp->stop();
}
