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

#include "du_manager_procedure_test_helpers.h"
#include "lib/du_manager/procedures/ue_configuration_procedure.h"
#include "lib/du_manager/procedures/ue_creation_procedure.h"
#include "srsran/support/test_utils.h"

using namespace srsran;
using namespace srs_du;

ul_ccch_indication_message srsran::srs_du::create_test_ul_ccch_message(rnti_t rnti)
{
  ul_ccch_indication_message ul_ccch_msg;
  ul_ccch_msg.cell_index = to_du_cell_index(0);
  ul_ccch_msg.tc_rnti    = rnti;
  ul_ccch_msg.slot_rx    = {0, test_rgen::uniform_int<unsigned>(0, 10239)};
  ul_ccch_msg.subpdu     = test_rgen::random_vector<uint8_t>(6);
  return ul_ccch_msg;
}

du_ue& du_manager_proc_tester::create_ue(du_ue_index_t ue_index)
{
  ul_ccch_indication_message ul_ccch_msg = create_test_ul_ccch_message(to_rnti(0x4601 + (unsigned)ue_index));

  f1ap.f1ap_ues.emplace(ue_index);
  f1ap.f1ap_ues[ue_index].f1c_bearers.emplace(srb_id_t::srb0);
  f1ap.f1ap_ues[ue_index].f1c_bearers.emplace(srb_id_t::srb1);

  f1ap.next_ue_create_response.result = true;
  f1ap.next_ue_create_response.f1c_bearers_added.resize(2);
  f1ap.next_ue_create_response.f1c_bearers_added[0] = &f1ap.f1ap_ues[ue_index].f1c_bearers[srb_id_t::srb0];
  f1ap.next_ue_create_response.f1c_bearers_added[1] = &f1ap.f1ap_ues[ue_index].f1c_bearers[srb_id_t::srb1];
  mac.wait_ue_create.result.allocated_crnti         = ul_ccch_msg.tc_rnti;
  mac.wait_ue_create.result.ue_index                = ue_index;
  mac.wait_ue_create.result.cell_index              = ul_ccch_msg.cell_index;
  mac.wait_ue_create.ready_ev.set();

  async_task<void> t = launch_async<ue_creation_procedure>(
      du_ue_creation_request{ue_index, ul_ccch_msg.cell_index, ul_ccch_msg.tc_rnti, ul_ccch_msg.subpdu.copy()},
      ue_mng,
      params,
      cell_res_alloc);

  lazy_task_launcher<void> launcher{t};

  srsran_assert(launcher.ready(), "The UE creation procedure should have completed by now");

  return *ue_mng.ues[ue_index];
}

f1ap_ue_context_update_response du_manager_proc_tester::configure_ue(const f1ap_ue_context_update_request& req)
{
  // Prepare F1AP response.
  for (srb_id_t srb_id : req.srbs_to_setup) {
    f1ap.f1ap_ues[req.ue_index].f1c_bearers.emplace(srb_id);
    f1c_bearer_addmodded b;
    b.srb_id = srb_id;
    b.bearer = &f1ap.f1ap_ues[req.ue_index].f1c_bearers[srb_id];
    f1ap.next_ue_config_response.f1c_bearers_added.push_back(b);
  }

  // Prepare MAC response.
  mac.wait_ue_reconf.result.ue_index = req.ue_index;
  mac.wait_ue_reconf.result.result   = true;
  mac.wait_ue_reconf.ready_ev.set();

  // Prepare DU resource allocator response.
  cell_res_alloc.next_context_update_result = cell_res_alloc.ue_resource_pool[req.ue_index];
  for (srb_id_t srb_id : req.srbs_to_setup) {
    cell_res_alloc.next_context_update_result.rlc_bearers.emplace_back();
    cell_res_alloc.next_context_update_result.rlc_bearers.back().lcid    = srb_id_to_lcid(srb_id);
    cell_res_alloc.next_context_update_result.rlc_bearers.back().rlc_cfg = make_default_srb_rlc_config();
  }
  for (const f1ap_drb_to_setup& drb : req.drbs_to_setup) {
    cell_res_alloc.next_context_update_result.rlc_bearers.emplace_back();
    cell_res_alloc.next_context_update_result.rlc_bearers.back().drb_id  = drb.drb_id;
    cell_res_alloc.next_context_update_result.rlc_bearers.back().lcid    = uint_to_lcid(3 + (unsigned)drb.drb_id);
    cell_res_alloc.next_context_update_result.rlc_bearers.back().rlc_cfg = make_default_srb_rlc_config();
  }
  for (drb_id_t drb_id : req.drbs_to_rem) {
    auto it = std::find_if(cell_res_alloc.next_context_update_result.rlc_bearers.begin(),
                           cell_res_alloc.next_context_update_result.rlc_bearers.end(),
                           [drb_id](const auto& b) { return b.drb_id == drb_id; });
    if (it != cell_res_alloc.next_context_update_result.rlc_bearers.end()) {
      cell_res_alloc.next_context_update_result.rlc_bearers.erase(it);
    }
  }

  // Run Procedure.
  async_task<f1ap_ue_context_update_response>         t = launch_async<ue_configuration_procedure>(req, ue_mng, params);
  lazy_task_launcher<f1ap_ue_context_update_response> launcher{t};
  srsran_assert(launcher.ready(), "The UE creation procedure should have completed by now");

  return t.get();
}
