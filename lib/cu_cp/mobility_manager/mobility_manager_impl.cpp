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

#include "mobility_manager_impl.h"
#include "../du_processor/du_processor_impl_interface.h"

using namespace srsran;
using namespace srs_cu_cp;

mobility_manager::mobility_manager(const mobility_manager_cfg& cfg_,
                                   cu_cp_f1c_handler&          du_db_,
                                   du_processor_ue_manager&    ue_mng_) :
  cfg(cfg_), du_db(du_db_), ue_mng(ue_mng_), logger(srslog::fetch_basic_logger("CU-CP"))
{
}

void mobility_manager::handle_neighbor_better_than_spcell(ue_index_t ue_index, pci_t neighbor_pci)
{
  if (!cfg.trigger_handover_from_measurements) {
    logger.debug("ue={}: Ignoring better neighbor pci={}", ue_index, neighbor_pci);
    return;
  }

  // Try to find target DU.
  du_index_t target_du = du_db.find_du(neighbor_pci);
  if (target_du == du_index_t::invalid) {
    logger.debug("ue={}: Requesting inter CU handover. No local DU/cell with pci={} found", ue_index, neighbor_pci);
    handle_inter_cu_handover(ue_index, neighbor_pci);
    return;
  }

  du_index_t source_du = ue_mng.find_du_ue(ue_index)->get_du_index();
  if (target_du == source_du) {
    logger.info("Trigger intra DU handover");
    // TODO: Prepare request and call notifier.
    return;
  } else {
    logger.info("ue={}: Trigger inter DU handover from source_du={} to target_du={}", ue_index, source_du, target_du);
    handle_inter_du_handover(ue_index, neighbor_pci, source_du, target_du);
  }
}

void mobility_manager::handle_inter_du_handover(ue_index_t source_ue_index,
                                                pci_t      neighbor_pci,
                                                du_index_t source_du_index,
                                                du_index_t target_du_index)
{
  // Lookup CGI at target DU.
  optional<nr_cell_global_id_t> cgi = du_db.get_du(target_du_index).get_mobility_handler().get_cgi(neighbor_pci);
  if (!cgi.has_value()) {
    logger.warning(
        "ue={}: Couldn't retrieve CGI for pci={} at du_index={}", source_ue_index, neighbor_pci, target_du_index);
    return;
  }

  cu_cp_inter_du_handover_request request = {};
  request.source_ue_index                 = source_ue_index;
  request.target_pci                      = neighbor_pci;
  request.cgi                             = cgi.value();
  request.target_du_index                 = target_du_index;

  // Lookup F1AP notifier of target DU.
  du_processor_f1ap_ue_context_notifier& target_du_f1ap_notifier =
      du_db.get_du(target_du_index).get_f1ap_ue_context_notifier();
  du_processor_ue_context_notifier& target_du_processor_notifier =
      du_db.get_du(target_du_index).get_du_processor_ue_context_notifier();

  du_processor_ue_task_handler&  ue_task = du_db.get_du(source_du_index).get_du_processor_ue_task_handler();
  du_processor_mobility_handler& mob     = du_db.get_du(source_du_index).get_mobility_handler();

  // Trigger Inter DU handover routine on the DU processor of the source DU.
  cu_cp_inter_du_handover_response response;
  auto ho_trigger = [request, response, &mob, &target_du_f1ap_notifier, &target_du_processor_notifier](
                        coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);
    CORO_AWAIT_VALUE(
        response, mob.handle_inter_du_handover_request(request, target_du_f1ap_notifier, target_du_processor_notifier));
    CORO_RETURN();
  };
  ue_task.handle_ue_async_task(request.source_ue_index, launch_async(std::move(ho_trigger)));
}

void mobility_manager::handle_intra_du_handover(ue_index_t source_ue_index, pci_t neighbor_pci)
{
  // TODO: prepare call
}

void mobility_manager::handle_inter_cu_handover(ue_index_t source_ue_index, pci_t neighbor_pci)
{
  // TODO: prepare NGAP call
}
