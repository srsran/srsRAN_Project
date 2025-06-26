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

#include "cu_cp_controller.h"
#include "../cu_up_processor/cu_up_processor_repository.h"
#include "../du_processor/du_processor_repository.h"
#include <thread>

using namespace srsran;
using namespace srs_cu_cp;

cu_cp_controller::cu_cp_controller(const cu_cp_configuration&      config_,
                                   cu_cp_amf_reconnection_handler& cu_cp_notifier,
                                   common_task_scheduler&          common_task_sched_,
                                   ngap_repository&                ngaps_,
                                   cu_up_processor_repository&     cu_ups_,
                                   du_processor_repository&        dus_,
                                   task_executor&                  ctrl_exec_) :
  cfg(config_),
  ctrl_exec(ctrl_exec_),
  logger(srslog::fetch_basic_logger("CU-CP")),
  amf_mng(ngaps_, cu_cp_notifier, *cfg.services.timers, ctrl_exec_, common_task_sched_),
  du_mng(cfg.admission.max_nof_dus, dus_, ctrl_exec, common_task_sched_),
  cu_up_mng(cfg.admission.max_nof_cu_ups, cu_ups_, ctrl_exec, common_task_sched_)
{
}

void cu_cp_controller::stop()
{
  // Note: Called from separate outer thread.
  {
    std::lock_guard<std::mutex> lock(mutex);
    if (not running) {
      return;
    }
  }

  // Stop and delete DU connections.
  du_mng.stop();

  // Stop and delete CU-UP connections.
  cu_up_mng.stop();

  // Stop and delete AMF connections.
  amf_mng.stop();
}

bool cu_cp_controller::handle_du_setup_request(du_index_t du_idx, const du_setup_request& req)
{
  bool success = false;
  for (const auto& cell : req.gnb_du_served_cells_list) {
    if (amf_mng.is_amf_connected(cell.served_cell_info.nr_cgi.plmn_id)) {
      success = true;
    }
  }

  // If AMF is not connected, it either means that the CU-CP is not operational state, there is a CU-CP failure or no
  // AMF for the PLMN of the DU cells was found.
  return success;
}

bool cu_cp_controller::request_ue_setup(plmn_identity plmn) const
{
  if (not amf_mng.is_amf_connected(plmn)) {
    return false;
  }

  if (cu_up_mng.nof_cu_ups() == 0) {
    return false;
  }

  return true;
}
