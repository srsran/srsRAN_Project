/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "cu_cp_routine_manager.h"
#include "../routines/initial_cu_cp_setup_routine.h"

using namespace srsran;
using namespace srs_cu_cp;

cu_cp_routine_manager::cu_cp_routine_manager(
    cu_cp_ngap_control_notifier&                                                   ngap_ctrl_notifier_,
    ngap_cu_cp_connection_notifier&                                                cu_cp_ngap_ev_notifier_,
    std::unordered_map<cu_up_index_t, std::unique_ptr<cu_up_processor_interface>>& cu_up_db_) :
  ngap_ctrl_notifier(ngap_ctrl_notifier_),
  cu_cp_ngap_ev_notifier(cu_cp_ngap_ev_notifier_),
  cu_up_db(cu_up_db_),
  main_ctrl_loop(128)
{
}

void cu_cp_routine_manager::start_initial_cu_cp_setup_routine(const ngap_configuration& ngap_cfg)
{
  main_ctrl_loop.schedule(
      launch_async<initial_cu_cp_setup_routine>(ngap_cfg, ngap_ctrl_notifier, cu_cp_ngap_ev_notifier));

  // start E1 setup procedure(s)
  for (auto& cu_up : cu_up_db) {
    cu_up.second->start();
  }
}
