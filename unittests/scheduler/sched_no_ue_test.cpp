/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/scheduler_impl.h"
#include "scheduler_test_suite.h"
#include "unittests/scheduler/utils/config_generators.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

class sched_cfg_dummy_notifier : public sched_configuration_notifier
{
public:
  void on_ue_config_complete(du_ue_index_t ue_index) override {}
  void on_ue_delete_response(du_ue_index_t ue_index) override {}
};

void test_no_ues()
{
  scheduler_config         sched_cfg = config_helpers::make_default_scheduler_config();
  sched_cfg_dummy_notifier cfg_notif;
  scheduler_impl           sch{sched_cfg, cfg_notif};

  // Action 1: Add Cell.
  sched_cell_configuration_request_message cell_cfg_msg = make_default_sched_cell_configuration_request();
  cell_configuration                       cell_cfg{cell_cfg_msg};
  sch.handle_cell_configuration_request(cell_cfg_msg);

  slot_point sl_tx{0, 0};

  // Action 2: Run slot 0.
  const sched_result* res = sch.slot_indication(sl_tx, to_du_cell_index(0));
  TESTASSERT(res != nullptr);
  test_scheduler_result_consistency(cell_cfg, *res);
  TESTASSERT(res->dl.ue_grants.empty());
  TESTASSERT(res->ul.puschs.empty());
}

void test_rach_indication()
{
  scheduler_config         sched_cfg = config_helpers::make_default_scheduler_config();
  sched_cfg_dummy_notifier cfg_notif;
  scheduler_impl           sch{sched_cfg, cfg_notif};

  // Action 1: Add Cell.
  sched_cell_configuration_request_message cell_cfg_msg = make_default_sched_cell_configuration_request();
  cell_configuration                       cell_cfg{cell_cfg_msg};
  sch.handle_cell_configuration_request(make_default_sched_cell_configuration_request());

  // Action 2: Add RACH indication.
  // Note: RACH is added in a slot different than the SIB1 to avoid PDCCH conflicts.
  const unsigned tx_delay = 2;
  slot_point     sl_rx{0, 1}, sl_tx = sl_rx + tx_delay;
  sch.handle_rach_indication(generate_rach_ind_msg(sl_rx, to_rnti(0x4601)));

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
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::info);

  srslog::init();

  test_no_ues();
  test_rach_indication();
}
