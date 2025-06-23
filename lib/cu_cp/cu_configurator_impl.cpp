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

#include "cu_configurator_impl.h"
#include "du_processor/du_processor_repository.h"

using namespace srsran;
using namespace srs_cu_cp;

ue_index_t cu_configurator_impl::get_ue_index(const amf_ue_id_t&         amf_ue_id,
                                              const guami_t&             guami,
                                              const gnb_cu_ue_f1ap_id_t& gnb_cu_ue_f1ap_id) const
{
  auto* ngap = ngap_db.find_ngap(guami.plmn);
  if (ngap == nullptr) {
    return ue_index_t::invalid;
  }
  return ngap->get_ngap_ue_id_translator().get_ue_index(amf_ue_id);
}

du_index_t cu_configurator_impl::get_du_index(const ue_index_t& ue_index) const
{
  auto* ue = ue_mng.find_du_ue(ue_index);
  return ue->get_du_index();
}

du_index_t cu_configurator_impl::get_du_index(const nr_cell_global_id_t& nr_cgi) const
{
  return du_db.find_du(nr_cgi);
}

pci_t cu_configurator_impl::get_pci(const nr_cell_global_id_t& nr_cgi) const
{
  du_index_t du_index = du_db.find_du(nr_cgi);
  if (du_index == du_index_t::invalid) {
    return INVALID_PCI;
  }
  return du_db.get_du_processor(du_index).get_context()->find_cell(nr_cgi)->pci;
}

async_task<cu_cp_intra_cu_handover_response> cu_configurator_impl::return_handover_response(bool ack)
{
  return launch_async([ack](coro_context<async_task<cu_cp_intra_cu_handover_response>>& ctx) {
    CORO_BEGIN(ctx);
    cu_cp_intra_cu_handover_response response;
    response.success = ack;
    CORO_RETURN(response);
  });
}

async_task<cu_cp_intra_cu_handover_response>
cu_configurator_impl::trigger_handover(const du_index_t&                      source_du_index,
                                       const cu_cp_intra_cu_handover_request& handover_req)
{
  cu_cp_ue* u = ue_mng.find_du_ue(handover_req.source_ue_index);
  if (not u) {
    return return_handover_response(false);
  }

  cu_cp_ue_context& ue_ctxt = u->get_ue_context();
  if (ue_ctxt.reconfiguration_disabled) {
    return return_handover_response(false);
  }

  // Handover is going ahead.
  // Disable new reconfigurations from now on (except for the Handover Command).
  ue_ctxt.reconfiguration_disabled = true;

  auto ho_trigger = ([this, handover_req, source_du_index, response = cu_cp_intra_cu_handover_response{}](
                         coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);
    CORO_AWAIT_VALUE(
        response,
        mobility_notif.on_intra_cu_handover_required(handover_req, source_du_index, handover_req.target_du_index));
    CORO_RETURN();
  });

  if (not u->get_task_sched().schedule_async_task(launch_async(std::move(ho_trigger)))) {
    return return_handover_response(false);
  }

  // Only confirm that the HO request was accepted, no info about the HO outcome.
  return return_handover_response(true);
}
