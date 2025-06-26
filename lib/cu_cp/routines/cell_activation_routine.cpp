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

#include "cell_activation_routine.h"
#include "../du_processor/du_processor_repository.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_configuration_update.h"
#include "srsran/ran/cause/ngap_cause.h"
#include "srsran/ran/plmn_identity.h"
#include "srsran/support/async/async_timer.h"
#include "srsran/support/async/coroutine.h"
#include "fmt/ranges.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

cell_activation_routine::cell_activation_routine(const cu_cp_configuration&        cu_cp_cfg_,
                                                 const std::vector<plmn_identity>& plmns_,
                                                 du_processor_repository&          du_db_,
                                                 srslog::basic_logger&             logger_) :
  cu_cp_cfg(cu_cp_cfg_), plmns(plmns_), du_db(du_db_), logger(logger_)
{
}

void cell_activation_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("\"{}\" started...", name());

  // Activate all cells that serve the PLMNs.
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

void cell_activation_routine::get_plmns_to_activate(const du_cell_configuration& cell_cfg)
{
  // Check which of the cells of this DU served the new PLMNs.
  std::set<plmn_identity> new_plmns(plmns.begin(), plmns.end());

  for (const auto& deactivated_plmn : cell_cfg.deactivated_plmns) {
    // If the PLMN is in the new PLMNs, we need to keep it.
    if (new_plmns.count(deactivated_plmn)) {
      plmns_to_activate.insert(deactivated_plmn);
    }
  }
}

bool cell_activation_routine::generate_gnb_cu_configuration_update()
{
  const du_configuration_context* du_ctxt = du_proc->get_context();
  if (du_ctxt == nullptr) {
    logger.warning("DU context not found for index {}", *du_idx_it);
    return false;
  }

  f1ap_cu_cfg_update = {};

  for (const auto& deactivated_cell : du_ctxt->deactivated_cells) {
    // Check which of the deactivated cells of this DU serve the lost PLMNs.
    get_plmns_to_activate(deactivated_cell);

    if (plmns_to_activate.empty()) {
      // No PLMNs are active, so we can skip this cell.
      continue;
    }

    // Update the cells configuration with the PLMNs to activate.
    f1ap_cu_cfg_update.cells_to_be_activated_list.push_back(
        {deactivated_cell.cgi,
         deactivated_cell.pci,
         std::vector<plmn_identity>(plmns_to_activate.begin(), plmns_to_activate.end())});
  }

  f1ap_cu_cfg_update.gnb_cu_name = cu_cp_cfg.node.ran_node_name;

  return true;
}
