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
#include "lib/du_manager/procedures/ue_creation_procedure.h"
#include "srsran/asn1/rrc_nr/cell_group_config.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;
using namespace asn1::rrc_nr;

cell_group_cfg_s unpack_cell_group_config(const byte_buffer& container)
{
  cell_group_cfg_s cell_group;
  asn1::cbit_ref   bref(container);
  auto             err = cell_group.unpack(bref);
  srsran_assert(err == asn1::SRSASN_SUCCESS, "Failed to unpack container");
  return cell_group;
}

class du_manager_ue_creation_tester : public du_manager_proc_tester, public ::testing::Test
{
protected:
  du_manager_ue_creation_tester() :
    du_manager_proc_tester(std::vector<du_cell_config>{config_helpers::make_default_du_cell_config()})
  {
  }

  void start_procedure(du_ue_index_t ue_index = to_du_ue_index(0), rnti_t rnti = to_rnti(0x4601), bool success = true)
  {
    f1ap.f1ap_ues.emplace(ue_index);
    f1ap.f1ap_ues[ue_index].f1c_bearers.emplace(srb_id_t::srb0);
    f1ap.f1ap_ues[ue_index].f1c_bearers.emplace(srb_id_t::srb1);
    f1ap.next_ue_create_response.result = success;
    f1ap.next_ue_create_response.f1c_bearers_added.resize(2);
    f1ap.next_ue_create_response.f1c_bearers_added[0] = &f1ap.f1ap_ues[ue_index].f1c_bearers[srb_id_t::srb0];
    f1ap.next_ue_create_response.f1c_bearers_added[1] = &f1ap.f1ap_ues[ue_index].f1c_bearers[srb_id_t::srb1];

    ul_ccch_indication_message ul_ccch_msg = create_test_ul_ccch_message(rnti);

    proc = launch_async<ue_creation_procedure>(
        du_ue_creation_request{ue_index, ul_ccch_msg.cell_index, rnti, std::move(ul_ccch_msg.subpdu)},
        ue_mng,
        params,
        cell_res_alloc);
    proc_launcher.emplace(proc);
  }

  void check_du_to_cu_rrc_container_validity(bool verbose = true)
  {
    const byte_buffer& container = this->f1ap.last_ue_create->du_cu_rrc_container;
    ASSERT_FALSE(container.empty());

    cell_group_cfg_s cell_group = unpack_cell_group_config(container);
    if (verbose) {
      asn1::json_writer js;
      cell_group.to_json(js);
      fmt::print("UE Creation produced CellGroup: {}\n", js.to_string());
    }

    ASSERT_TRUE(cell_group.sp_cell_cfg_present);

    ASSERT_EQ(cell_group.rlc_bearer_to_add_mod_list.size(), 1);
    ASSERT_TRUE(cell_group.rlc_bearer_to_add_mod_list[0].served_radio_bearer_present);
    ASSERT_EQ(cell_group.rlc_bearer_to_add_mod_list[0].served_radio_bearer.srb_id(), 1);

    ASSERT_FALSE(cell_group.sp_cell_cfg.recfg_with_sync_present);
  }

  void set_sr_offset(du_ue_index_t ue_index, du_cell_index_t cell_idx, unsigned sr_offset)
  {
    this->cell_res_alloc.next_context_update_result.cells[0]
        .serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list[0]
        .offset = sr_offset;
  }

  async_task<void>                   proc;
  optional<lazy_task_launcher<void>> proc_launcher;
};

TEST_F(du_manager_ue_creation_tester,
       when_du_manager_receives_ue_creation_request_then_mac_and_f1ap_get_request_to_create_ue)
{
  // Start Procedure.
  du_ue_index_t ue_index = to_du_ue_index(test_rgen::uniform_int<unsigned>(0, MAX_DU_UE_INDEX));
  rnti_t        rnti     = to_rnti(test_rgen::uniform_int<unsigned>(1, MAX_CRNTI));
  start_procedure(ue_index, rnti);

  // Check MAC received request to create UE with valid params.
  ASSERT_TRUE(this->mac.last_ue_create_msg.has_value());
  ASSERT_EQ(this->mac.last_ue_create_msg->ue_index, ue_index);
  ASSERT_EQ(this->mac.last_ue_create_msg->crnti, rnti);
  ASSERT_FALSE(this->mac.last_ue_create_msg->ul_ccch_msg->empty());

  // Check if F1AP received request to create UE with valid params.
  ASSERT_TRUE(this->f1ap.last_ue_create.has_value());
  ASSERT_EQ(this->f1ap.last_ue_create->ue_index, ue_index);
  ASSERT_EQ(this->f1ap.last_ue_create->c_rnti, rnti);
  ASSERT_EQ(this->f1ap.last_ue_create->f1c_bearers_to_add.size(), 2);
  ASSERT_FALSE(this->f1ap.last_ue_create->du_cu_rrc_container.empty());
  ASSERT_NO_FATAL_FAILURE(check_du_to_cu_rrc_container_validity());
}

TEST_F(du_manager_ue_creation_tester,
       when_du_manager_starts_ue_creation_then_the_procedure_does_not_complete_until_mac_answers_back)
{
  // Start Procedure.
  du_ue_index_t ue_index = to_du_ue_index(test_rgen::uniform_int<unsigned>(0, MAX_DU_UE_INDEX));
  rnti_t        rnti     = to_rnti(test_rgen::uniform_int<unsigned>(1, MAX_CRNTI));
  start_procedure(ue_index, rnti);

  ASSERT_FALSE(proc.ready());
  mac.wait_ue_create.result.allocated_crnti = rnti;
  mac.wait_ue_create.result.ue_index        = ue_index;
  mac.wait_ue_create.result.cell_index      = to_du_cell_index(0);
  mac.wait_ue_create.ready_ev.set();

  // Check procedure has finished.
  ASSERT_TRUE(proc.ready());
}

TEST_F(du_manager_ue_creation_tester,
       when_a_ue_is_created_then_cell_resources_are_requested_from_cell_resource_allocator)
{
  // Test Preamble.
  // > Generate SR offsets for two UEs.
  du_ue_index_t ue_idx1 = to_du_ue_index(0), ue_idx2 = to_du_ue_index(1);
  unsigned      sr_period  = sr_periodicity_to_slot(this->cell_res_alloc.next_context_update_result.cells[0]
                                                  .serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list[0]
                                                  .period);
  unsigned      sr_offset1 = test_rgen::uniform_int<unsigned>(0, sr_period - 1);
  unsigned      sr_offset2 = test_rgen::uniform_int<unsigned>(0, sr_period - 1);
  // > Create first UE.
  set_sr_offset(ue_idx1, to_du_cell_index(0), sr_offset1);
  start_procedure(ue_idx1, to_rnti(0x4601));
  mac_ue_create_request req1 = *this->mac.last_ue_create_msg;
  ASSERT_NO_FATAL_FAILURE(check_du_to_cu_rrc_container_validity());
  du_ue_index_t ue_res_idx1 = *cell_res_alloc.last_ue_index;
  // > Create second UE.
  set_sr_offset(ue_idx2, to_du_cell_index(0), sr_offset2);
  start_procedure(ue_idx2, to_rnti(0x4602));
  mac_ue_create_request req2 = *this->mac.last_ue_create_msg;
  ASSERT_NO_FATAL_FAILURE(check_du_to_cu_rrc_container_validity());
  du_ue_index_t ue_res_idx2 = *cell_res_alloc.last_ue_index;

  // Test.
  // > DU Cell Resource Allocator was called.
  ASSERT_EQ(ue_res_idx1, ue_idx1);
  ASSERT_EQ(ue_res_idx2, ue_idx2);
  // > UE SR Offsets passed to scheduler match the ones generated.
  ASSERT_FALSE(req1.mac_cell_group_cfg.scheduling_request_config.empty());
  ASSERT_FALSE(req2.mac_cell_group_cfg.scheduling_request_config.empty());
  const auto& sr_res_list1 = req1.sched_cfg.cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list;
  const auto& sr_res_list2 = req2.sched_cfg.cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list;
  ASSERT_FALSE(sr_res_list1.empty());
  ASSERT_FALSE(sr_res_list2.empty());
  ASSERT_EQ(sr_res_list1[0].offset, sr_offset1);
  ASSERT_EQ(sr_res_list2[0].offset, sr_offset2);
}
