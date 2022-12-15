/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_manager_procedure_test_helpers.h"
#include "lib/du_manager/procedures/ue_creation_procedure.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_du;
using namespace asn1::rrc_nr;

cell_group_cfg_s unpack_cell_group_config(const byte_buffer& container)
{
  cell_group_cfg_s cell_group;
  asn1::cbit_ref   bref(container);
  srsgnb_assert(cell_group.unpack(bref) == asn1::SRSASN_SUCCESS, "Failed to unpack container");
  return cell_group;
}

class ue_creation_tester : public du_manager_proc_tester, public ::testing::Test
{
protected:
  ue_creation_tester() :
    du_manager_proc_tester(std::vector<du_cell_config>{config_helpers::make_default_du_cell_config()})
  {
  }

  void start_procedure(du_ue_index_t ue_index = to_du_ue_index(0), rnti_t rnti = to_rnti(0x4601))
  {
    proc = launch_async<ue_creation_procedure>(
        ue_index, create_test_ul_ccch_message(rnti), ue_mng, params.services, params.mac, params.rlc, params.f1ap);
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

    ASSERT_EQ(cell_group.rlc_bearer_to_add_mod_list.size(), 1);
    ASSERT_TRUE(cell_group.rlc_bearer_to_add_mod_list[0].served_radio_bearer_present);
    ASSERT_EQ(cell_group.rlc_bearer_to_add_mod_list[0].served_radio_bearer.srb_id(), 1);

    ASSERT_TRUE(cell_group.sp_cell_cfg_present);
    ASSERT_FALSE(cell_group.sp_cell_cfg.recfg_with_sync_present);
  }

  async_task<void>                   proc;
  optional<lazy_task_launcher<void>> proc_launcher;
};

TEST_F(ue_creation_tester, when_du_manager_receives_ue_creation_request_then_mac_and_f1ap_get_request_to_create_ue)
{
  // Start Procedure.
  du_ue_index_t ue_index = to_du_ue_index(test_rgen::uniform_int<unsigned>(0, MAX_DU_UE_INDEX));
  rnti_t        rnti     = to_rnti(test_rgen::uniform_int<unsigned>(1, MAX_CRNTI));
  f1ap.f1_ues.emplace(ue_index);
  f1ap.f1_ues[ue_index].f1c_bearers.emplace(srb_id_t::srb0);
  f1ap.f1_ues[ue_index].f1c_bearers.emplace(srb_id_t::srb1);
  f1ap.next_ue_create_response.result = true;
  f1ap.next_ue_create_response.f1c_bearers_added.resize(2);
  f1ap.next_ue_create_response.f1c_bearers_added[0] = &f1ap.f1_ues[ue_index].f1c_bearers[srb_id_t::srb0];
  f1ap.next_ue_create_response.f1c_bearers_added[1] = &f1ap.f1_ues[ue_index].f1c_bearers[srb_id_t::srb1];
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

TEST_F(ue_creation_tester,
       when_du_manager_starts_ue_creation_then_the_procedure_does_not_complete_until_mac_answers_back)
{
  // Start Procedure.
  du_ue_index_t ue_index = to_du_ue_index(test_rgen::uniform_int<unsigned>(0, MAX_DU_UE_INDEX));
  rnti_t        rnti     = to_rnti(test_rgen::uniform_int<unsigned>(1, MAX_CRNTI));
  f1ap.f1_ues.emplace(ue_index);
  f1ap.f1_ues[ue_index].f1c_bearers.emplace(srb_id_t::srb0);
  f1ap.f1_ues[ue_index].f1c_bearers.emplace(srb_id_t::srb1);
  f1ap.next_ue_create_response.result = true;
  f1ap.next_ue_create_response.f1c_bearers_added.resize(2);
  f1ap.next_ue_create_response.f1c_bearers_added[0] = &f1ap.f1_ues[ue_index].f1c_bearers[srb_id_t::srb0];
  f1ap.next_ue_create_response.f1c_bearers_added[1] = &f1ap.f1_ues[ue_index].f1c_bearers[srb_id_t::srb1];
  start_procedure(ue_index, rnti);

  ASSERT_FALSE(proc.ready());
  mac.wait_ue_create.result.result     = true;
  mac.wait_ue_create.result.ue_index   = ue_index;
  mac.wait_ue_create.result.cell_index = to_du_cell_index(0);
  mac.wait_ue_create.ready_ev.set();

  // Check procedure has finished.
  ASSERT_TRUE(proc.ready());
}

TEST_F(ue_creation_tester, when_two_ues_get_created_then_their_sr_pucch_resources_are_orthogonal)
{
  // Create two UEs.
  create_ue(to_du_ue_index(0));
  mac_ue_create_request_message req1 = *this->mac.last_ue_create_msg;
  ASSERT_NO_FATAL_FAILURE(check_du_to_cu_rrc_container_validity());
  create_ue(to_du_ue_index(1));
  mac_ue_create_request_message req2 = *this->mac.last_ue_create_msg;
  ASSERT_NO_FATAL_FAILURE(check_du_to_cu_rrc_container_validity());

  ASSERT_FALSE(req1.mac_cell_group_cfg.scheduling_request_config.empty());
  ASSERT_FALSE(req2.mac_cell_group_cfg.scheduling_request_config.empty());
  const auto& sr_res_list1 = req1.serv_cell_cfg->ul_config->init_ul_bwp.pucch_cfg->sr_res_list;
  const auto& sr_res_list2 = req2.serv_cell_cfg->ul_config->init_ul_bwp.pucch_cfg->sr_res_list;
  ASSERT_FALSE(sr_res_list1.empty());
  ASSERT_FALSE(sr_res_list2.empty());

  std::vector<unsigned>                           sr_periods1, sr_periods2;
  std::set<unsigned>                              sr_offsets;
  sr_periodicity                                  period = sr_res_list1[0].period;
  std::vector<scheduling_request_resource_config> concat_list(sr_res_list1.begin(), sr_res_list1.end());
  concat_list.insert(concat_list.end(), sr_res_list2.begin(), sr_res_list2.end());
  for (const scheduling_request_resource_config& res : concat_list) {
    ASSERT_EQ(res.period, period) << "Only one SR period supported at a time";
    ASSERT_EQ(sr_offsets.count(res.offset), 0)
        << "SR resources must be at different time offsets to avoid SR collisions";
    sr_offsets.insert(res.offset);
  }
}
