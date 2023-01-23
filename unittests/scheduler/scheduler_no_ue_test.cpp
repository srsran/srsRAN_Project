/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief In this file, we verify the correct operation of the MAC scheduler, as a whole, for basic operations, in
/// setups without any UEs. The objective is to cover and verify the integration of the scheduler building blocks.
/// The currently covered operations include:
/// - run_slot without any event
/// - handle RACH indication + RAR allocation.

#include "lib/scheduler/scheduler_impl.h"
#include "unittests/scheduler/test_utils/config_generators.h"
#include "unittests/scheduler/test_utils/dummy_test_components.h"
#include "unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

void test_no_ues()
{
  scheduler_expert_config             sched_cfg = config_helpers::make_default_scheduler_expert_config();
  sched_cfg_dummy_notifier            cfg_notif;
  scheduler_ue_metrics_dummy_notifier metrics_notif;
  scheduler_impl                      sch{scheduler_config{sched_cfg, cfg_notif, metrics_notif}};

  // Action 1: Add Cell.
  sched_cell_configuration_request_message cell_cfg_msg = test_helpers::make_default_sched_cell_configuration_request();
  cell_configuration                       cell_cfg{cell_cfg_msg};
  sch.handle_cell_configuration_request(cell_cfg_msg);

  slot_point sl_tx{0, test_rgen::uniform_int<unsigned>(0, 10240)};

  // Action 2: Run slot.
  const sched_result* res = sch.slot_indication(sl_tx, to_du_cell_index(0));
  TESTASSERT(res != nullptr);
  test_scheduler_result_consistency(cell_cfg, *res);
  TESTASSERT(res->dl.ue_grants.empty());
  TESTASSERT(res->ul.puschs.empty());
}

void test_rach_indication()
{
  scheduler_expert_config             sched_cfg = config_helpers::make_default_scheduler_expert_config();
  sched_cfg_dummy_notifier            cfg_notif;
  scheduler_ue_metrics_dummy_notifier metrics_notif;
  scheduler_impl                      sch{scheduler_config{sched_cfg, cfg_notif, metrics_notif}};

  // Action 1: Add Cell.
  sched_cell_configuration_request_message cell_cfg_msg = test_helpers::make_default_sched_cell_configuration_request();
  cell_configuration                       cell_cfg{cell_cfg_msg};
  sch.handle_cell_configuration_request(test_helpers::make_default_sched_cell_configuration_request());

  // Action 2: Add RACH indication.
  // Note: RACH is added in a slot different than the SIB1 to avoid PDCCH conflicts.
  const unsigned tx_delay = 2;
  slot_point     sl_rx{0, 1}, sl_tx = sl_rx + tx_delay;
  sch.handle_rach_indication(test_helpers::generate_rach_ind_msg(sl_rx, to_rnti(0x4601)));

  // Action 3: Run slot 0.
  const sched_result* res = sch.slot_indication(sl_tx, to_du_cell_index(0));

  // TEST: Result exists. No Data allocated. A RAR has been allocated.
  TESTASSERT(res != nullptr);
  test_scheduler_result_consistency(cell_cfg, *res);
  TESTASSERT(res->dl.ue_grants.empty());
  TESTASSERT(not res->dl.rar_grants.empty());
}

int main()
{
  srslog::fetch_basic_logger("SCHED", true).set_level(srslog::basic_levels::info);

  srslog::init();

  test_no_ues();
  test_rach_indication();
}
