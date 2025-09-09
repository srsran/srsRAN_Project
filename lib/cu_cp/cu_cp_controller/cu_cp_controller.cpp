/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_controller.h"
#include "../cu_up_processor/cu_up_processor_repository.h"
#include "../du_processor/du_processor_repository.h"
#include "srsran/ran/plmn_identity.h"

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

bool cu_cp_controller::handle_du_setup_request(du_index_t du_idx, const std::set<plmn_identity>& plmn_ids)
{
  bool success = false;
  for (const auto& plmn : plmn_ids) {
    if (amf_mng.is_amf_connected(plmn)) {
      success = true;
    } else {
      logger.debug("No AMF for PLMN={} is connected", plmn);
    }
  }

  // If AMF is not connected, it either means that the CU-CP is not operational state, there is a CU-CP failure or no
  // AMF for the PLMN of the DU cells was found.
  return success;
}

bool cu_cp_controller::request_ue_setup() const
{
  if (amf_mng.nof_amfs() == 0) {
    return false;
  }

  if (cu_up_mng.nof_cu_ups() == 0) {
    return false;
  }

  return true;
}

bool cu_cp_controller::is_supported_plmn(const plmn_identity& plmn) const
{
  return amf_mng.is_amf_connected(plmn);
}
