/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief In this file, we test the correct behaviour of the scheduler when handling UE contention resolution, and
/// TC-RNTI to C-RNTI transitions.

#include "test_utils/config_generators.h"
#include "test_utils/result_test_helpers.h"
#include "test_utils/scheduler_test_bench.h"
#include <gtest/gtest.h>

using namespace srsran;

class scheduler_ue_conres_test : public scheduler_test_bench, public ::testing::Test
{
protected:
  scheduler_ue_conres_test()
  {
    add_cell(test_helpers::make_default_sched_cell_configuration_request());

    // Create a UE with a DRB active.
    auto ue_cfg     = test_helpers::create_default_sched_ue_creation_request({}, {});
    ue_cfg.ue_index = ue_index;
    ue_cfg.crnti    = rnti;
    scheduler_test_bench::add_ue(ue_cfg, true);
  }

  du_ue_index_t ue_index = to_du_ue_index(0);
  rnti_t        rnti     = to_rnti(0x4601);
};

TEST_F(scheduler_ue_conres_test, when_conres_ce_is_enqueued_and_no_msg4_is_enqueued_then_pdsch_is_not_scheduled)
{
  // Enqueue ConRes CE.
  this->sched->handle_dl_mac_ce_indication(dl_mac_ce_indication{ue_index, lcid_dl_sch_t::UE_CON_RES_ID});

  // Ensure the ConRes CEU is not scheduled without a Msg4 SDU.
  ASSERT_FALSE(this->run_slot_until([this]() { return find_ue_pdsch(rnti, *this->last_sched_res) != nullptr; }));
}

TEST_F(scheduler_ue_conres_test, when_conres_ce_and_srb0_pdu_are_enqueued_then_tcrnti_is_used)
{
  const static unsigned msg4_size = 128;

  // Enqueue ConRes CE.
  this->sched->handle_dl_mac_ce_indication(dl_mac_ce_indication{ue_index, lcid_dl_sch_t::UE_CON_RES_ID});

  // Enqueue Msg4 in SRB0.
  this->push_dl_buffer_state(dl_buffer_state_indication_message{this->ue_index, LCID_SRB0, msg4_size});

  // Ensure the Msg4 is scheduled together with the ConRes CE.
  ASSERT_TRUE(this->run_slot_until([this]() { return find_ue_pdsch(rnti, *this->last_sched_res) != nullptr; }));
  const dl_msg_alloc* msg4_alloc = find_ue_pdsch(rnti, *this->last_sched_res);
  ASSERT_EQ(msg4_alloc->tb_list.size(), 1);
  ASSERT_EQ(msg4_alloc->tb_list[0].lc_chs_to_sched.size(), 2);
  ASSERT_EQ(msg4_alloc->tb_list[0].lc_chs_to_sched[0].lcid, lcid_dl_sch_t::UE_CON_RES_ID);
  ASSERT_EQ(msg4_alloc->tb_list[0].lc_chs_to_sched[1].lcid, LCID_SRB0);

  // Ensure enough bytes are scheduled to fit the full Msg4, as RLC-TM doesn't support segmentation.
  ASSERT_GE(msg4_alloc->tb_list[0].lc_chs_to_sched[1].sched_bytes, msg4_size);

  // Ensure TC-RNTI is used for Msg4.
  ASSERT_EQ(find_ue_dl_pdcch(rnti)->dci.type, dci_dl_rnti_config_type::tc_rnti_f1_0);

  // Ensure no PDSCH multiplexing with CSI-RS.
  ASSERT_TRUE(this->last_sched_res->dl.csi_rs.empty());
}

TEST_F(scheduler_ue_conres_test, when_conres_ce_and_srb1_pdu_are_enqueued_then_tcrnti_is_used)
{
  // Enqueue ConRes CE.
  this->sched->handle_dl_mac_ce_indication(dl_mac_ce_indication{ue_index, lcid_dl_sch_t::UE_CON_RES_ID});

  // Enqueue Msg4 in SRB1.
  this->push_dl_buffer_state(dl_buffer_state_indication_message{this->ue_index, LCID_SRB1, 128});

  // Ensure the Msg4 is scheduled with a ConRes CE.
  ASSERT_TRUE(this->run_slot_until([this]() { return find_ue_pdsch(rnti, *this->last_sched_res) != nullptr; }));
  const dl_msg_alloc* msg4_alloc = find_ue_pdsch(rnti, *this->last_sched_res);
  ASSERT_EQ(msg4_alloc->tb_list.size(), 1);
  ASSERT_EQ(msg4_alloc->tb_list[0].lc_chs_to_sched.size(), 2);
  ASSERT_EQ(msg4_alloc->tb_list[0].lc_chs_to_sched[0].lcid, lcid_dl_sch_t::UE_CON_RES_ID);
  ASSERT_EQ(msg4_alloc->tb_list[0].lc_chs_to_sched[1].lcid, LCID_SRB1);

  // Ensure TC-RNTI is used.
  ASSERT_EQ(find_ue_dl_pdcch(rnti)->dci.type, dci_dl_rnti_config_type::tc_rnti_f1_0);
}
