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

#include "cell_deactivation_routine.h"
#include "../du_processor/du_processor_repository.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_configuration_update.h"
#include "srsran/ran/cause/ngap_cause.h"
#include "srsran/ran/plmn_identity.h"
#include "srsran/support/async/async_timer.h"
#include "srsran/support/async/coroutine.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

cell_deactivation_routine::cell_deactivation_routine(const cu_cp_configuration&        cu_cp_cfg_,
                                                     const std::vector<plmn_identity>& plmns_,
                                                     du_processor_repository&          du_db_,
                                                     cu_cp_ue_context_release_handler& ue_release_handler_,
                                                     ue_manager&                       ue_mng_,
                                                     srslog::basic_logger&             logger_) :
  cu_cp_cfg(cu_cp_cfg_),
  plmns(plmns_),
  du_db(du_db_),
  ue_release_handler(ue_release_handler_),
  ue_mng(ue_mng_),
  logger(logger_),
  ue_release_timer(timer_factory{*cu_cp_cfg.services.timers, *cu_cp_cfg.services.cu_cp_executor}.create_timer())
{
}

void cell_deactivation_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("\"{}\" started...", name());

  // Release all UEs with the PLMNs served by the disconnected AMF.
  release_ues();

  // Wait until all UEs are released.
  while (true) {
    all_ues_released = true;
    for (ue_release_status_it = ue_release_status.begin(); ue_release_status_it != ue_release_status.end();
         ++ue_release_status_it) {
      if (!ue_release_status_it->second) {
        CORO_AWAIT(async_wait_for(ue_release_timer, std::chrono::milliseconds(10)));
        all_ues_released = false;
      }
    }
    if (all_ues_released) {
      logger.info("All UEs released");
      break;
    }
  }

  // Deactivate all cells that serve this PLMN.
  du_indexes = du_db.get_du_processor_indexes();
  for (du_idx_it = du_indexes.begin(); du_idx_it != du_indexes.end(); ++du_idx_it) {
    du_proc = du_db.find_du_processor(*du_idx_it);
    if (du_proc == nullptr) {
      logger.warning("DU processor not found for index {}", *du_idx_it);
      continue;
    }

    if (!generate_gnb_cu_configuration_update()) {
      logger.warning("Failed to generate gNB-CU configuration update for DU {}", *du_idx_it);
      continue;
    }

    CORO_AWAIT_VALUE(f1ap_cu_cfg_update_response, du_proc->handle_configuration_update(f1ap_cu_cfg_update));
  }

  if (f1ap_cu_cfg_update_response.success) {
    logger.info("\"{}\" finished successfully", name());
  } else {
    logger.info("\"{}\" failed. Cause: {}", name(), f1ap_cu_cfg_update_response.cause.value());
  }

  CORO_RETURN();
}

void cell_deactivation_routine::release_ues()
{
  // Release all UEs with the PLMNs served by the disconnected AMF.
  for (const auto& plmn : plmns) {
    std::vector<cu_cp_ue*> ues = ue_mng.find_ues(plmn);

    for (const auto& ue : ues) {
      if (ue != nullptr) {
        ue_release_status[ue->get_ue_index()] = false;
        logger.info("ue={}: Releasing UE (PLMN {}) due to N2 disconnection", ue->get_ue_index(), plmn);
        ue->get_task_sched().schedule_async_task(launch_async(
            [this,
             command      = cu_cp_ue_context_release_command{ue->get_ue_index(),
                                                        ngap_cause_transport_t::transport_res_unavailable,
                                                        true,
                                                        std::chrono::seconds{5}},
             &ue_released = ue_release_status.at(ue->get_ue_index())](coro_context<async_task<void>>& ctx) {
              CORO_BEGIN(ctx);
              // The outcome of the procedure is ignored, as we don't send anything to the (lost) AMF.
              CORO_AWAIT(ue_release_handler.handle_ue_context_release_command(command));
              ue_released = true;
              logger.info("ue={}: UE released", command.ue_index);
              CORO_RETURN();
            }));
      }
    }
  }
}

void cell_deactivation_routine::get_remaining_plmns(const du_cell_configuration& cell_cfg)
{
  // Check which of the cells of this DU serve the lost PLMNs.
  std::set<plmn_identity> lost_plmns(plmns.begin(), plmns.end());

  for (const auto& served_plmn : cell_cfg.served_plmns) {
    // If the PLMN is not in the lost PLMNs, we need to keep it.
    if (!lost_plmns.count(served_plmn)) {
      remaining_plmns.insert(served_plmn);
    }
  }
}

bool cell_deactivation_routine::generate_gnb_cu_configuration_update()
{
  const du_configuration_context* du_ctxt = du_proc->get_context();
  if (du_ctxt == nullptr) {
    logger.warning("DU context not found for index {}", *du_idx_it);
    return false;
  }

  f1ap_cu_cfg_update = {};

  for (const auto& served_cell : du_ctxt->served_cells) {
    // Check which of the cells of this DU serve the lost PLMNs.
    get_remaining_plmns(served_cell);

    if (remaining_plmns.empty()) {
      // All PLMNs are lost, so we can deactivate the cell.
      f1ap_cu_cfg_update.cells_to_be_deactivated_list.push_back({served_cell.cgi});
      continue;
    }

    if (remaining_plmns.size() < served_cell.served_plmns.size()) {
      f1ap_cu_cfg_update.cells_to_be_deactivated_list.push_back({served_cell.cgi});
      // TODO: Deactivate only specific PLMNs
    }
  }

  f1ap_cu_cfg_update.gnb_cu_name = cu_cp_cfg.node.ran_node_name;

  return true;
}
