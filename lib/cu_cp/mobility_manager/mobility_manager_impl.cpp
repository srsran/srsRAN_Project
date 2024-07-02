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
#include "../du_processor/du_processor_repository.h"
#include "srsran/ran/nr_cgi.h"

using namespace srsran;
using namespace srs_cu_cp;

mobility_manager::mobility_manager(const mobility_manager_cfg&      cfg_,
                                   mobility_manager_cu_cp_notifier& cu_cp_notifier_,
                                   ngap_control_message_handler&    ngap_handler_,
                                   du_processor_repository&         du_db_,
                                   ue_manager&                      ue_mng_) :
  cfg(cfg_),
  cu_cp_notifier(cu_cp_notifier_),
  ngap_handler(ngap_handler_),
  du_db(du_db_),
  ue_mng(ue_mng_),
  logger(srslog::fetch_basic_logger("CU-CP"))
{
}

void mobility_manager::trigger_handover(pci_t source_pci, rnti_t rnti, pci_t target_pci)
{
  ue_index_t ue_index = ue_mng.get_ue_index(source_pci, rnti);
  if (ue_index == ue_index_t::invalid) {
    logger.warning("Could not trigger handover, UE is invalid. rnti={} pci={}", rnti, source_pci);
    return;
  }
  handle_handover(ue_index, gnb_id_t{}, nr_cell_identity{}, target_pci); // TODO: define gNB-ID and NCI
}

void mobility_manager::handle_neighbor_better_than_spcell(ue_index_t       ue_index,
                                                          gnb_id_t         neighbor_gnb_id,
                                                          nr_cell_identity neighbor_nci,
                                                          pci_t            neighbor_pci)
{
  if (!cfg.trigger_handover_from_measurements) {
    logger.debug("ue={}: Ignoring better neighbor pci={}", ue_index, neighbor_pci);
    return;
  }
  handle_handover(ue_index, neighbor_gnb_id, neighbor_nci, neighbor_pci);
}

void mobility_manager::handle_handover(ue_index_t       ue_index,
                                       gnb_id_t         neighbor_gnb_id,
                                       nr_cell_identity neighbor_nci,
                                       pci_t            neighbor_pci)
{
  // Find the UE context.
  cu_cp_ue* u = ue_mng.find_du_ue(ue_index);
  if (u == nullptr) {
    logger.error("ue={}: Couldn't find UE", ue_index);
    return;
  }
  cu_cp_ue_context& ue_ctxt = u->get_ue_context();
  if (ue_ctxt.reconfiguration_disabled) {
    logger.debug("ue={}: MeasurementReport ignored. Cause: UE cannot be reconfigured", ue_index);
    return;
  }
  if (neighbor_pci == INVALID_PCI) {
    logger.error("ue={}: Ignoring Handover Request. Cause: Invalid target PCI {} received", ue_index, neighbor_pci);
    return;
  }

  // Handover is going ahead.

  // Disable new reconfigurations from now on (except for the Handover Command).
  ue_ctxt.reconfiguration_disabled = true;

  // Try to find target DU. If it is not found, it means that the target cell is not managed by this CU-CP and
  // a NG Handover is required.
  du_index_t target_du = du_db.find_du(neighbor_pci);
  if (target_du == du_index_t::invalid) {
    logger.debug("ue={}: Requesting inter CU handover. No local DU/cell with pci={} found", ue_index, neighbor_pci);
    handle_inter_cu_handover(ue_index, neighbor_gnb_id, neighbor_nci);
    return;
  }

  du_index_t source_du = ue_mng.find_du_ue(ue_index)->get_du_index();

  // TODO: Trigger intra DU HO when target_du == source_du. For now we perform inter DU HO to itself.

  logger.info("ue={}: Trigger inter DU handover from source_du={} to target_du={}", ue_index, source_du, target_du);
  handle_inter_du_handover(ue_index, neighbor_pci, source_du, target_du);
}

void mobility_manager::handle_inter_du_handover(ue_index_t source_ue_index,
                                                pci_t      neighbor_pci,
                                                du_index_t source_du_index,
                                                du_index_t target_du_index)
{
  // Lookup CGI at target DU.
  std::optional<nr_cell_global_id_t> cgi =
      du_db.get_du_processor(target_du_index).get_mobility_handler().get_cgi(neighbor_pci);
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

  cu_cp_ue* u = ue_mng.find_du_ue(source_ue_index);
  if (u == nullptr) {
    logger.error("ue={}: Couldn't find UE", source_ue_index);
    return;
  }

  // Trigger Inter DU handover routine on the DU processor of the source DU.
  auto ho_trigger = [this, request, response = cu_cp_inter_du_handover_response{}, &source_du_index, &target_du_index](
                        coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);
    CORO_AWAIT_VALUE(response, cu_cp_notifier.on_inter_du_handover_required(request, source_du_index, target_du_index));
    CORO_RETURN();
  };
  u->get_task_sched().schedule_async_task(launch_async(std::move(ho_trigger)));
}

void mobility_manager::handle_intra_du_handover(ue_index_t source_ue_index, pci_t neighbor_pci)
{
  // TODO: prepare call
}

void mobility_manager::handle_inter_cu_handover(ue_index_t       source_ue_index,
                                                gnb_id_t         target_gnb_id,
                                                nr_cell_identity target_nci)
{
  cu_cp_ue* u = ue_mng.find_du_ue(source_ue_index);
  if (u == nullptr) {
    logger.error("ue={}: Couldn't find UE", source_ue_index);
    return;
  }

  ngap_handover_preparation_request request = {};
  request.ue_index                          = source_ue_index;
  request.gnb_id                            = target_gnb_id;
  request.nci                               = target_nci;

  // create a map of all PDU sessions and their associated QoS flows
  const std::map<pdu_session_id_t, up_pdu_session_context>& pdu_sessions =
      ue_mng.find_ue(source_ue_index)->get_up_resource_manager().get_pdu_sessions_map();
  for (const auto& pdu_session : pdu_sessions) {
    std::vector<qos_flow_id_t> qos_flows;
    for (const auto& drb : pdu_session.second.drbs) {
      for (const auto& qos_flow : drb.second.qos_flows) {
        qos_flows.push_back(qos_flow.first);
      }
    }
    request.pdu_sessions.insert({pdu_session.first, qos_flows});
  }

  // Send handover preparation request to the NGAP handler.
  auto ho_trigger =
      [this, request, response = ngap_handover_preparation_response{}](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);
        CORO_AWAIT_VALUE(response, ngap_handler.handle_handover_preparation_request(request));
        CORO_RETURN();
      };
  u->get_task_sched().schedule_async_task(launch_async(std::move(ho_trigger)));
}
