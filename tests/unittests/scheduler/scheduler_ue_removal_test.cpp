/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "test_utils/config_generators.h"
#include "test_utils/result_test_helpers.h"
#include "test_utils/scheduler_test_bench.h"
#include <gtest/gtest.h>

using namespace srsran;

class sched_ue_removal_test : public scheduler_test_bench, public ::testing::Test
{
protected:
  sched_ue_removal_test() { add_cell(test_helpers::make_default_sched_cell_configuration_request()); }

  void add_ue(du_ue_index_t ue_index, rnti_t rnti)
  {
    // Create a UE with a DRB active.
    auto ue_cfg     = test_helpers::create_default_sched_ue_creation_request({}, {test_lcid_drb});
    ue_cfg.ue_index = ue_index;
    ue_cfg.crnti    = rnti;
    scheduler_test_bench::add_ue(ue_cfg, true);
  }

  static const lcid_t test_lcid_drb = LCID_MIN_DRB;
};

TEST_F(sched_ue_removal_test, when_ue_has_no_pending_txs_then_ue_removal_is_immediate)
{
  // Create UE.
  du_ue_index_t ue_index = (du_ue_index_t)test_rgen::uniform_int<unsigned>(0, MAX_DU_UE_INDEX);
  rnti_t        rnti     = to_rnti(test_rgen::uniform_int<unsigned>(0x4601, MAX_CRNTI));
  add_ue(ue_index, rnti);
  ASSERT_FALSE(notif.last_ue_index_deleted.has_value());

  // Remove UE.
  rem_ue(ue_index);

  const unsigned REM_TIMEOUT = 1;
  for (unsigned i = 0; not notif.last_ue_index_deleted.has_value() and i != REM_TIMEOUT; ++i) {
    run_slot();
  }
  ASSERT_TRUE(notif.last_ue_index_deleted == ue_index);
}

TEST_F(sched_ue_removal_test, when_ue_has_pending_harqs_then_scheduler_waits_for_harq_clear_before_deleting_ue)
{
  // Create UE.
  du_ue_index_t ue_index = (du_ue_index_t)test_rgen::uniform_int<unsigned>(0, MAX_DU_UE_INDEX);
  rnti_t        rnti     = to_rnti(test_rgen::uniform_int<unsigned>(0x4601, MAX_CRNTI));
  add_ue(ue_index, rnti);

  // Push DL buffer status update for UE DRB.
  this->push_dl_buffer_state(dl_buffer_state_indication_message{ue_index, test_lcid_drb, 10000000});

  // Wait for at least one DL HARQ to be allocated.
  const dl_msg_alloc* alloc      = nullptr;
  const unsigned      TX_TIMEOUT = 10;
  for (unsigned i = 0; i != TX_TIMEOUT; ++i) {
    this->run_slot();
    alloc = find_ue_pdsch(rnti, *last_sched_res);
    if (alloc != nullptr) {
      break;
    }
  }
  ASSERT_NE(alloc, nullptr);

  // Schedule UE removal.
  rem_ue(ue_index);

  // Wait for the right slot for ACK.
  const unsigned    ACK_TIMEOUT = 10;
  const pucch_info* pucch       = nullptr;
  for (unsigned count = 0; count != ACK_TIMEOUT and pucch == nullptr; ++count) {
    this->run_slot();
    ASSERT_EQ(find_ue_pdsch(rnti, *last_sched_res), nullptr)
        << "UE allocated despite having no SRB pending bytes and being marked for removal";

    pucch = find_ue_pucch(rnti, *last_sched_res);
  }
  ASSERT_NE(pucch, nullptr);

  // HARQ-ACK should empty the HARQ process.
  uci_indication uci;
  uci.cell_index = to_du_cell_index(0);
  uci.slot_rx    = last_result_slot();
  uci.ucis.push_back(create_uci_pdu_with_harq_ack(ue_index, *pucch));
  this->sched->handle_uci_indication(uci);

  // The UE should be removed at this point.
  const unsigned REM_TIMEOUT = 1;
  for (unsigned i = 0; not notif.last_ue_index_deleted.has_value() and i != REM_TIMEOUT; ++i) {
    run_slot();
  }
  ASSERT_TRUE(notif.last_ue_index_deleted == ue_index);
}

TEST_F(sched_ue_removal_test, when_ue_has_pending_srb_data_then_scheduler_waits_for_srb_data_to_be_flushed)
{
  // Create UE.
  du_ue_index_t ue_index = (du_ue_index_t)test_rgen::uniform_int<unsigned>(0, MAX_DU_UE_INDEX);
  rnti_t        rnti     = to_rnti(test_rgen::uniform_int<unsigned>(0x4601, MAX_CRNTI));
  add_ue(ue_index, rnti);

  // Push DL buffer status update for UE SRB.
  const unsigned bytes_to_tx = 1000;
  this->push_dl_buffer_state(dl_buffer_state_indication_message{ue_index, LCID_SRB1, bytes_to_tx});

  // Schedule UE removal.
  rem_ue(ue_index);

  // Do not delete UE until SRB data is flushed.
  const unsigned TX_TIMEOUT   = 1000;
  unsigned       nof_dl_bytes = 0;
  for (unsigned i = 0; i != TX_TIMEOUT and not notif.last_ue_index_deleted.has_value(); ++i) {
    this->run_slot();

    const dl_msg_alloc* alloc = find_ue_pdsch(rnti, *last_sched_res);
    if (alloc != nullptr) {
      nof_dl_bytes += alloc->pdsch_cfg.codewords[0].tb_size_bytes;
    }

    const pucch_info* pucch = find_ue_pucch(rnti, *last_sched_res);
    if (pucch != nullptr) {
      // HARQ-ACK and empty the HARQ process.
      uci_indication uci;
      uci.cell_index = to_du_cell_index(0);
      uci.slot_rx    = last_result_slot();
      uci.ucis.push_back(create_uci_pdu_with_harq_ack(ue_index, *pucch));
      this->sched->handle_uci_indication(uci);
    }
  }

  ASSERT_TRUE(notif.last_ue_index_deleted == ue_index);
  ASSERT_GE(nof_dl_bytes, bytes_to_tx);
}
