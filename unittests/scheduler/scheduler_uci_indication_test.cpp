/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/cell/cell_configuration.h"
#include "scheduler_test_suite.h"
#include "unittests/scheduler/utils/config_generators.h"
#include "utils/scheduler_test_bench.h"
#include "srsgnb/scheduler/scheduler_factory.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;

class uci_sched_tester : public ::testing::Test
{
protected:
  uci_sched_tester() : sched(create_scheduler(config_helpers::make_default_scheduler_config(), notif))
  {
    add_cell();
    add_ue();

    next_slot = slot_point{0, 1};
  }

  void add_cell()
  {
    sched_cell_configuration_request_message cell_cfg_msg = make_default_sched_cell_configuration_request();
    cell_cfg.emplace(cell_cfg_msg);
    sched->handle_cell_configuration_request(cell_cfg_msg);
  }

  void add_ue()
  {
    sched_ue_creation_request_message ue_cfg_msg = make_default_sched_ue_creation_request();
    ue_cfg_msg.crnti                             = ue_rnti;
    sched->handle_add_ue_request(ue_cfg_msg);
  }

  void run_slot()
  {
    logger.set_context(next_slot.to_uint());
    last_sched_res = sched->slot_indication(next_slot, to_du_cell_index(0));
    TESTASSERT(last_sched_res != nullptr);
    test_scheduler_result_consistency(*cell_cfg, *last_sched_res);
    ++next_slot;
  }

  void notify_uci_ind(bool sr_ind, span<const bool> harq_bits)
  {
    uci_indication uci_ind{};
    uci_ind.cell_index = to_du_cell_index(0);
    uci_ind.slot_rx    = next_slot - 1;
    uci_ind.ucis.resize(1);
    uci_ind.ucis[0].ue_index    = to_du_ue_index(0);
    uci_ind.ucis[0].sr_detected = sr_ind;
    uci_ind.ucis[0].harqs.resize(harq_bits.size());
    for (unsigned i = 0; i != harq_bits.size(); ++i) {
      uci_ind.ucis[0].harqs[i] = harq_bits[i];
    }

    sched->handle_uci_indication(uci_ind);
  }

  void notify_dl_buffer_status(lcid_t lcid, unsigned pending_bytes)
  {
    dl_buffer_state_indication_message msg{};
    msg.rnti     = ue_rnti;
    msg.ue_index = ue_id;
    msg.bs       = pending_bytes;
    msg.lcid     = lcid;
    sched->handle_dl_buffer_state_indication(msg);
  }

  bool ue_pdsch_scheduled() const
  {
    for (const auto& pdsch : last_sched_res->dl.ue_grants) {
      if (pdsch.pdsch_cfg.rnti == ue_rnti) {
        return true;
      }
    }
    return false;
  }

  bool ue_pusch_scheduled() const
  {
    return std::any_of(last_sched_res->ul.puschs.begin(), last_sched_res->ul.puschs.end(), [](const auto& pusch) {
      return pusch.pusch_cfg.rnti == ue_rnti;
    });
  }

  bool ue_pucch_scheduled() const
  {
    return std::any_of(last_sched_res->ul.pucchs.begin(), last_sched_res->ul.pucchs.end(), [](const auto& pucch) {
      return pucch.crnti == ue_rnti;
    });
  }

  constexpr static du_ue_index_t ue_id   = to_du_ue_index(0);
  constexpr static rnti_t        ue_rnti = to_rnti(0x4601);

  srslog::basic_logger&          logger = srslog::fetch_basic_logger("MAC");
  sched_cfg_dummy_notifier       notif;
  optional<cell_configuration>   cell_cfg;
  std::unique_ptr<mac_scheduler> sched;

  slot_point          next_slot;
  const sched_result* last_sched_res = nullptr;
};

TEST_F(uci_sched_tester, no_retx_after_harq_ack)
{
  static constexpr unsigned MAX_COUNT = 16;
  notify_dl_buffer_status(LCID_SRB0, 100);

  bool pucch_found = false;
  for (unsigned i = 0; i != MAX_COUNT; ++i) {
    run_slot();
    if (ue_pucch_scheduled()) {
      notify_uci_ind(false, std::array<bool, 1>{true});
      pucch_found = true;
      break;
    }
  }
  ASSERT_TRUE(pucch_found);

  // No more DL grants after the ACK.
  for (unsigned i = 0; i != MAX_COUNT; ++i) {
    run_slot();
    ASSERT_FALSE(ue_pdsch_scheduled());
    ASSERT_FALSE(ue_pucch_scheduled());
  }
}

TEST_F(uci_sched_tester, pusch_scheduled_after_sr_indication)
{
  // Maximum delay between the SR indication being forwarded to the scheduler and the scheduler generating an UL grant.
  static constexpr unsigned MAX_UL_GRANT_DELAY = 8;
  notify_uci_ind(true, {});

  bool pusch_found = false;
  for (unsigned i = 0; i != MAX_UL_GRANT_DELAY; ++i) {
    run_slot();
    ASSERT_FALSE(ue_pdsch_scheduled());
    ASSERT_FALSE(ue_pucch_scheduled());
    if (ue_pusch_scheduled()) {
      pusch_found = true;
      break;
    }
  }
  ASSERT_TRUE(pusch_found);
}

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}