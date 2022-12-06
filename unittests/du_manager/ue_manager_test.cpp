/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dummy_test_classes.h"
#include "lib/du_manager/du_ue_manager.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include "srsgnb/support/executors/manual_task_worker.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace srs_du;

struct test_bench {
  timer_manager              timers;
  manual_task_worker         worker{128};
  dummy_ue_executor_mapper   ue_execs{worker};
  dummy_cell_executor_mapper cell_execs{worker};

  f1ap_test_dummy f1ap_dummy;
  mac_test_dummy  mac_dummy;

  du_manager_config_t cfg{srslog::fetch_basic_logger("DU-MNG"),
                          {},
                          {config_helpers::make_default_du_cell_config()},
                          {timers, worker, ue_execs, cell_execs},
                          {f1ap_dummy, f1ap_dummy},
                          {mac_dummy, f1ap_dummy, f1ap_dummy},
                          {mac_dummy, mac_dummy}};
};

enum class test_outcome { success, ue_create_failure };

/// Test the scenario where concurrent UECreateRequest and UECreateDelete messages arrive at UE manager
void test_ue_concurrent_procedures(test_outcome outcome)
{
  test_delimit_logger   delimiter{"Test UE concurrent procedures. Outcome: {}",
                                outcome == test_outcome::success ? "Success" : "UE create failure"};
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  test_bench bench;
  bench.f1ap_dummy.next_ue_create_response.result = true;
  bench.f1ap_dummy.next_ue_create_response.f1c_bearers_added.resize(2);

  du_ue_manager ue_mng{bench.cfg};
  TESTASSERT(ue_mng.get_ues().empty());
  du_ue_index_t ue_index = MAX_NOF_DU_UES;

  // Action 1: UL CCCH indication arrives.
  ul_ccch_indication_message ccch_ind{};
  ccch_ind.cell_index = to_du_cell_index(0);
  ccch_ind.crnti      = to_rnti(0x4601);
  ccch_ind.subpdu     = {0, 1, 2, 3, 4, 5};
  test_logger.info("TEST: Pushing UL CCCH indication for RNTI=0x{:x}...", ccch_ind.crnti);
  ue_mng.handle_ue_create_request(ccch_ind);

  // TEST: F1 UE Creation started and completed. MAC UE Creation started but hasn't returned yet.
  TESTASSERT(bench.f1ap_dummy.last_ue_create.has_value());
  ue_index = bench.f1ap_dummy.last_ue_create.value().ue_index;
  TESTASSERT(ue_index < MAX_NOF_DU_UES);
  TESTASSERT(bench.mac_dummy.last_pushed_ul_ccch_msg.empty());
  TESTASSERT(ue_mng.get_ues().empty());

  // TEST: MAC UE creation started but hasn't returned yet
  TESTASSERT(bench.mac_dummy.last_ue_create_msg.has_value());
  TESTASSERT_EQ(ccch_ind.crnti, bench.mac_dummy.last_ue_create_msg->crnti);
  TESTASSERT_EQ(ccch_ind.cell_index, bench.mac_dummy.last_ue_create_msg->cell_index);
  TESTASSERT_EQ(2, bench.mac_dummy.last_ue_create_msg->bearers.size());
  TESTASSERT_EQ(LCID_SRB0, bench.mac_dummy.last_ue_create_msg->bearers[0].lcid);
  TESTASSERT(bench.mac_dummy.last_ue_create_msg->bearers[0].ul_bearer != nullptr);
  TESTASSERT_EQ(LCID_SRB1, bench.mac_dummy.last_ue_create_msg->bearers[1].lcid);
  TESTASSERT(bench.mac_dummy.last_ue_create_msg->bearers[1].ul_bearer != nullptr);
  // TODO: Check DL bearer
  TESTASSERT(ue_mng.get_ues().empty());

  // Action 3: Start concurrent UE deletion
  f1ap_ue_delete_request ue_del_req{};
  ue_del_req.ue_index = ue_index;
  test_logger.info("TEST: Starting UE deletion with UE index={}...", ue_del_req.ue_index);
  ue_mng.schedule_async_task(ue_index, ue_mng.handle_ue_delete_request(ue_del_req));

  // TEST: Given that UE creation task hasn't finished, the UE deletion isn't processed yet
  TESTASSERT(not bench.mac_dummy.last_ue_delete_msg.has_value());

  // Action 4: MAC UE creation completed
  bench.mac_dummy.wait_ue_create.result.ue_index   = ue_index;
  bench.mac_dummy.wait_ue_create.result.cell_index = ccch_ind.cell_index;
  bench.mac_dummy.wait_ue_create.result.result     = outcome != test_outcome::ue_create_failure;
  bench.mac_dummy.wait_ue_create.ready_ev.set();

  // TEST: UE manager finished UE creation procedure. MAC should push UL CCCH to upper layers.
  if (outcome == test_outcome::success) {
    TESTASSERT(not bench.mac_dummy.last_pushed_ul_ccch_msg.empty());
    TESTASSERT(not ue_mng.get_ues().empty());
    TESTASSERT(ue_mng.get_ues().contains(ue_index));
    TESTASSERT(ue_mng.get_ues()[ue_index].rnti == 0x4601);
  } else {
    TESTASSERT(ue_mng.get_ues().empty());
  }

  // TEST: If MAC UE created failed, MAC UE Delete Request should not have been started
  if (outcome != test_outcome::success) {
    TESTASSERT(not bench.mac_dummy.last_ue_delete_msg.has_value());
    return;
  }
  TESTASSERT(bench.mac_dummy.last_ue_delete_msg.has_value());
  TESTASSERT_EQ(ue_index, bench.mac_dummy.last_ue_delete_msg->ue_index);

  // Action 5: MAC UE Deletion complete
  bench.mac_dummy.wait_ue_delete.result.result = true;
  bench.mac_dummy.wait_ue_delete.ready_ev.set();

  // TEST: DU UE Deletion is complete
  TESTASSERT(ue_mng.get_ues().empty());
  // TODO: Check F1AP delete
}

/// Test the scenario where a ue delete request arrives for an inexistent ue_index
void test_inexistent_ue_removal()
{
  test_delimit_logger delimiter{"Test inexistent UE removal"};

  test_bench bench;

  du_ue_manager ue_mng{bench.cfg};
  TESTASSERT(ue_mng.get_ues().empty());

  // Action 1: Start UE deletion
  f1ap_ue_delete_request ue_del_req{};
  ue_del_req.ue_index = to_du_ue_index(0);
  ue_mng.schedule_async_task(ue_del_req.ue_index, ue_mng.handle_ue_delete_request(ue_del_req));

  // There should not be any reply from MAC and F1AP should receive failure signal
  TESTASSERT(ue_mng.get_ues().empty());
  TESTASSERT(not bench.mac_dummy.last_ue_create_msg.has_value());
  // TODO: F1AP check
}

/// Different scenarios for testing duplicate UE creation
enum class test_duplicate_ue_creation_mode { mac_ue_create_auto, mac_ue_create_manual };

/// Test the scenario where two UEs with the same RNTI are added. The following checks take place:
/// - only one UE is created in the DU manager.
/// - The MAC and F1 and correctly triggered to create a UE object.
/// - The MAC is notified that the buffered UL CCCH can be forwarded to upper layers.
void test_duplicate_ue_creation(test_duplicate_ue_creation_mode mode)
{
  test_delimit_logger delimiter{"Test duplicate UE creation. Mode: {}",
                                mode == test_duplicate_ue_creation_mode::mac_ue_create_auto ? "auto" : "manual"};

  du_ue_index_t first_ue_index = MAX_NOF_DU_UES;

  test_bench bench;

  bench.f1ap_dummy.next_ue_create_response.result = true;
  bench.f1ap_dummy.next_ue_create_response.f1c_bearers_added.resize(2);

  bench.mac_dummy.wait_ue_create.result.ue_index   = first_ue_index;
  bench.mac_dummy.wait_ue_create.result.cell_index = to_du_cell_index(0);
  bench.mac_dummy.wait_ue_create.result.result     = true;
  if (mode == test_duplicate_ue_creation_mode::mac_ue_create_auto) {
    bench.mac_dummy.wait_ue_create.ready_ev.set();
  }

  du_ue_manager ue_mng{bench.cfg};
  TESTASSERT(ue_mng.get_ues().empty());

  // Action 1: Start creation of UE by notifying UL CCCH decoding.
  ul_ccch_indication_message ul_ccch_ind{};
  ul_ccch_ind.cell_index = to_du_cell_index(0);
  ul_ccch_ind.crnti      = to_rnti(0x4601);
  ul_ccch_ind.subpdu     = {1, 2, 3, 4, 5};
  ue_mng.handle_ue_create_request(ul_ccch_ind);

  // TEST: F1 started creating first UE.
  TESTASSERT(bench.f1ap_dummy.last_ue_create.has_value());
  first_ue_index = bench.f1ap_dummy.last_ue_create.value().ue_index;
  TESTASSERT(bench.f1ap_dummy.last_ue_create.value().ue_index < MAX_NOF_DU_UES);

  // TEST: MAC started creating first UE.
  TESTASSERT(bench.mac_dummy.last_ue_create_msg.has_value());
  TESTASSERT_EQ(first_ue_index, bench.mac_dummy.last_ue_create_msg->ue_index);
  TESTASSERT_EQ(ul_ccch_ind.crnti, bench.mac_dummy.last_ue_create_msg->crnti);
  if (mode == test_duplicate_ue_creation_mode::mac_ue_create_auto) {
    // TEST: First UE create procedure is complete
    TESTASSERT(not bench.mac_dummy.last_pushed_ul_ccch_msg.empty());
    TESTASSERT_EQ(1, ue_mng.get_ues().size());
  }

  // Action 2: Start creation of second UE.
  bench.mac_dummy.last_ue_create_msg.reset();
  bench.f1ap_dummy.last_ue_create.reset();
  bench.mac_dummy.last_pushed_ul_ccch_msg.clear();
  ul_ccch_indication_message ul_ccch_ind2 = ul_ccch_ind;
  ue_mng.handle_ue_create_request(ul_ccch_ind2);

  if (mode == test_duplicate_ue_creation_mode::mac_ue_create_auto) {
    // TEST: UE manager does not initiate second UE creation in MAC or F1 due to repeated RNTI.
    TESTASSERT(not bench.f1ap_dummy.last_ue_create.has_value());
    TESTASSERT(not bench.mac_dummy.last_ue_create_msg.has_value());
    TESTASSERT(bench.mac_dummy.last_pushed_ul_ccch_msg.empty());
  } else {
    // TEST: MAC didn't start creating second UE because first UE is still being created.
    TESTASSERT(not bench.f1ap_dummy.last_ue_create.has_value());
    TESTASSERT(not bench.mac_dummy.last_ue_create_msg.has_value());
  }

  if (mode == test_duplicate_ue_creation_mode::mac_ue_create_manual) {
    // Action 4: Finish all the MAC UE creations.
    bench.mac_dummy.wait_ue_create.ready_ev.set();

    // TEST: First UE creation is complete. Second was aborted due to repeated RNTI.
    TESTASSERT(not bench.f1ap_dummy.last_ue_create.has_value());
    TESTASSERT(not bench.mac_dummy.last_ue_create_msg.has_value());

    // TEST: First UE creation once finishes, notifies MAC that UL CCCH can be pushed to upper layers.
    TESTASSERT(not bench.mac_dummy.last_pushed_ul_ccch_msg.empty());
  }

  // TEST: Only one UE should have been created
  TESTASSERT_EQ(1, ue_mng.get_ues().size());
  TESTASSERT(ue_mng.get_ues().contains(first_ue_index));
  TESTASSERT_EQ(first_ue_index, ue_mng.get_ues()[first_ue_index].ue_index);
  TESTASSERT_EQ(ul_ccch_ind.crnti, ue_mng.get_ues()[first_ue_index].rnti);
  TESTASSERT_EQ(ul_ccch_ind.cell_index, ue_mng.get_ues()[first_ue_index].pcell_index);
}

int main()
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("DU-MNG").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);

  srslog::init();

  test_ue_concurrent_procedures(test_outcome::success);
  test_ue_concurrent_procedures(test_outcome::ue_create_failure);
  test_inexistent_ue_removal();
  test_duplicate_ue_creation(test_duplicate_ue_creation_mode::mac_ue_create_auto);
  test_duplicate_ue_creation(test_duplicate_ue_creation_mode::mac_ue_create_manual);
}
