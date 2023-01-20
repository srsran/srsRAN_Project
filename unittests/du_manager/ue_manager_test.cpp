/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_manager_test_helpers.h"
#include "lib/du_manager/du_ue/du_ue_manager.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include "srsgnb/support/executors/manual_task_worker.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

/// \file
/// \brief In this file, we unit test the interaction between DU UE procedures in the DU UE Manager. For unit tests
/// addressing the specific details of each DU UE manager procedure, please check procedures/ directory.

using namespace srsgnb;
using namespace srs_du;

class du_ue_manager_tester : public ::testing::Test
{
protected:
  du_ue_manager_tester()
  {
    srslog::fetch_basic_logger("DU-MNG").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    // By default F1AP creates two F1-C bearers.
    f1ap_dummy.next_ue_create_response.result = true;
    f1ap_dummy.next_ue_create_response.f1c_bearers_added.resize(2);
  }
  ~du_ue_manager_tester() { srslog::flush(); }

  ul_ccch_indication_message create_ul_ccch_message(rnti_t rnti)
  {
    ul_ccch_indication_message ccch_ind{};
    ccch_ind.cell_index = to_du_cell_index(0);
    ccch_ind.crnti      = rnti;
    ccch_ind.subpdu     = {0, 1, 2, 3, 4, 5};
    return ccch_ind;
  }

  void push_ul_ccch_message(ul_ccch_indication_message ccch_ind)
  {
    test_logger.info("TEST: Pushing UL CCCH indication for RNTI={:#x}...", ccch_ind.crnti);
    ue_mng.handle_ue_create_request(ccch_ind);
  }

  void push_f1ap_ue_delete_request(du_ue_index_t ue_index)
  {
    f1ap_ue_delete_request ue_del_req{};
    ue_del_req.ue_index = ue_index;
    test_logger.info("TEST: Starting UE deletion with UE index={}...", ue_del_req.ue_index);
    ue_mng.schedule_async_task(ue_del_req.ue_index, ue_mng.handle_ue_delete_request(ue_del_req));
  }

  void mac_completes_ue_creation(bool result)
  {
    mac_dummy.wait_ue_create.result.ue_index   = get_last_ue_index();
    mac_dummy.wait_ue_create.result.cell_index = to_du_cell_index(0);
    mac_dummy.wait_ue_create.result.result     = result;
    mac_dummy.wait_ue_create.ready_ev.set();
  }

  void mac_completes_ue_deletion()
  {
    mac_dummy.wait_ue_delete.result.result = true;
    mac_dummy.wait_ue_delete.ready_ev.set();
  }

  du_ue_index_t get_last_ue_index() const
  {
    srsgnb_assert(f1ap_dummy.last_ue_create.has_value(), "No UE creation request was provided");
    return f1ap_dummy.last_ue_create.value().ue_index;
  }

  srslog::basic_logger&      test_logger = srslog::fetch_basic_logger("TEST");
  timer_manager              timers;
  manual_task_worker         worker{128};
  dummy_ue_executor_mapper   ue_execs{worker};
  dummy_cell_executor_mapper cell_execs{worker};

  std::vector<du_cell_config>            cells = {config_helpers::make_default_du_cell_config()};
  f1ap_test_dummy                        f1ap_dummy;
  f1u_gateway_dummy                      f1u_dummy;
  mac_test_dummy                         mac_dummy;
  dummy_ue_resource_configurator_factory cell_res_alloc;

  du_manager_params params{{"srsgnb", 1, 1, cells},
                           {timers, worker, ue_execs, cell_execs},
                           {f1ap_dummy, f1ap_dummy},
                           {f1u_dummy},
                           {mac_dummy, f1ap_dummy, f1ap_dummy},
                           {mac_dummy, mac_dummy}};

  du_ue_manager ue_mng{params, cell_res_alloc};
};

TEST_F(du_ue_manager_tester, when_ue_create_request_is_received_du_manager_requests_f1ap_and_mac_to_create_ue)
{
  // Action: UL CCCH Message received.
  ul_ccch_indication_message ccch_ind = create_ul_ccch_message(to_rnti(0x4601));
  push_ul_ccch_message(ccch_ind);

  // TEST: F1AP received request to create UE.
  TESTASSERT(f1ap_dummy.last_ue_create.has_value());
  du_ue_index_t ue_index = f1ap_dummy.last_ue_create.value().ue_index;
  TESTASSERT(ue_index < MAX_NOF_DU_UES);

  // TEST: MAC received UE creation request.
  TESTASSERT(mac_dummy.last_ue_create_msg.has_value());
  TESTASSERT_EQ(ccch_ind.crnti, mac_dummy.last_ue_create_msg->crnti);
}

TEST_F(du_ue_manager_tester,
       when_ue_create_request_is_received_du_manager_requests_mac_to_create_ue_and_awaits_response)
{
  // Action 1: UL CCCH Message received.
  ul_ccch_indication_message ccch_ind = create_ul_ccch_message(to_rnti(0x4601));
  push_ul_ccch_message(ccch_ind);
  du_ue_index_t ue_index = get_last_ue_index();

  // TEST: While MAC does not respond, no UE is created.
  TESTASSERT(ue_mng.get_ues().empty());

  // Action 2: MAC UE creation completed.
  mac_completes_ue_creation(true);

  // TEST: DU manager completes DU UE creation procedure with success.
  TESTASSERT(not ue_mng.get_ues().empty());
  TESTASSERT(ue_mng.get_ues().contains(ue_index));
  TESTASSERT(ue_mng.get_ues()[ue_index].rnti == 0x4601);
  TESTASSERT(not mac_dummy.last_pushed_ul_ccch_msg.empty());
}

TEST_F(du_ue_manager_tester, when_mac_fails_to_create_ue_then_no_ue_is_created_in_du)
{
  // Action: UL CCCH Message received and MAC UE creation fails.
  ul_ccch_indication_message ccch_ind = create_ul_ccch_message(to_rnti(0x4601));
  push_ul_ccch_message(ccch_ind);
  mac_completes_ue_creation(false);

  // TEST: DU manager completes DU UE creation procedure with failure.
  TESTASSERT(mac_dummy.last_pushed_ul_ccch_msg.empty());
  TESTASSERT(ue_mng.get_ues().empty());
}

TEST_F(du_ue_manager_tester, inexistent_ue_index_removal_is_handled)
{
  // Action: Request UE deletion for inexistent UE Index.
  push_f1ap_ue_delete_request(to_du_ue_index(test_rgen::uniform_int<unsigned>(0, MAX_NOF_DU_UES - 1)));

  // There should not be any reply from MAC and F1AP should receive failure signal
  TESTASSERT(ue_mng.get_ues().empty());
  TESTASSERT(not mac_dummy.last_ue_delete_msg.has_value());
  // TODO: F1AP check
}

TEST_F(du_ue_manager_tester,
       when_request_for_ue_creation_and_removal_are_received_concurrently_then_the_procedures_run_in_sequence)
{
  // Action 1: UL CCCH Message and UE deletion request received concurrently.
  push_ul_ccch_message(create_ul_ccch_message(to_rnti(0x4601)));
  push_f1ap_ue_delete_request(get_last_ue_index());

  // MAC and F1AP receive request to create UE.
  ASSERT_TRUE(mac_dummy.last_ue_create_msg.has_value());
  ASSERT_TRUE(f1ap_dummy.last_ue_create.has_value());

  // Until MAC completes UE creation, F1AP and MAC should not receive request to delete UE.
  ASSERT_FALSE(mac_dummy.last_ue_delete_msg.has_value());
  ASSERT_FALSE(f1ap_dummy.last_ue_release.has_value());
  mac_completes_ue_creation(true);
  ASSERT_TRUE(mac_dummy.last_ue_delete_msg.has_value());
  ASSERT_EQ(get_last_ue_index(), mac_dummy.last_ue_delete_msg->ue_index);

  // Action 2: MAC finishes UE deletion.
  ASSERT_FALSE(ue_mng.get_ues().empty());
  mac_completes_ue_deletion();

  // UE deleted from the DU.
  TESTASSERT(ue_mng.get_ues().empty());
}

TEST_F(du_ue_manager_tester,
       when_requests_for_ue_creation_are_received_concurrently_then_the_created_ues_have_different_indexes)
{
  // Action: UL CCCH Messages received concurrently.
  push_ul_ccch_message(create_ul_ccch_message(to_rnti(0x4601)));
  du_ue_index_t ue_index1 = get_last_ue_index();
  push_ul_ccch_message(create_ul_ccch_message(to_rnti(0x4602)));
  du_ue_index_t ue_index2 = get_last_ue_index();

  ASSERT_NE(ue_index1, ue_index2);
}

///// Different scenarios for testing duplicate UE creation
// enum class test_duplicate_ue_creation_mode { mac_ue_create_auto, mac_ue_create_manual };
//
///// Test the scenario where two UEs with the same RNTI are added. The following checks take place:
///// - only one UE is created in the DU manager.
///// - The MAC and F1 and correctly triggered to create a UE object.
///// - The MAC is notified that the buffered UL CCCH can be forwarded to upper layers.
// void test_duplicate_ue_creation(test_bench& bench, test_duplicate_ue_creation_mode mode)
//{
//   test_delimit_logger delimiter{"Test duplicate UE creation. Mode: {}",
//                                 mode == test_duplicate_ue_creation_mode::mac_ue_create_auto ? "auto" : "manual"};
//
//   du_ue_index_t first_ue_index = MAX_NOF_DU_UES;
//
//   bench.f1ap_dummy.next_ue_create_response.result = true;
//   bench.f1ap_dummy.next_ue_create_response.f1c_bearers_added.resize(2);
//
//   bench.mac_dummy.wait_ue_create.result.ue_index   = first_ue_index;
//   bench.mac_dummy.wait_ue_create.result.cell_index = to_du_cell_index(0);
//   bench.mac_dummy.wait_ue_create.result.result     = true;
//   if (mode == test_duplicate_ue_creation_mode::mac_ue_create_auto) {
//     bench.mac_dummy.wait_ue_create.ready_ev.set();
//   }
//
//   du_ue_manager ue_mng{bench.params, bench.cell_res_alloc};
//   TESTASSERT(ue_mng.get_ues().empty());
//
//   // Action 1: Start creation of UE by notifying UL CCCH decoding.
//   ul_ccch_indication_message ul_ccch_ind{};
//   ul_ccch_ind.cell_index = to_du_cell_index(0);
//   ul_ccch_ind.crnti      = to_rnti(0x4601);
//   ul_ccch_ind.subpdu     = {1, 2, 3, 4, 5};
//   ue_mng.handle_ue_create_request(ul_ccch_ind);
//
//   // TEST: F1 started creating first UE.
//   TESTASSERT(bench.f1ap_dummy.last_ue_create.has_value());
//   first_ue_index = bench.f1ap_dummy.last_ue_create.value().ue_index;
//   TESTASSERT(bench.f1ap_dummy.last_ue_create.value().ue_index < MAX_NOF_DU_UES);
//
//   // TEST: MAC started creating first UE.
//   TESTASSERT(bench.mac_dummy.last_ue_create_msg.has_value());
//   TESTASSERT_EQ(first_ue_index, bench.mac_dummy.last_ue_create_msg->ue_index);
//   TESTASSERT_EQ(ul_ccch_ind.crnti, bench.mac_dummy.last_ue_create_msg->crnti);
//   if (mode == test_duplicate_ue_creation_mode::mac_ue_create_auto) {
//     // TEST: First UE create procedure is complete
//     TESTASSERT(not bench.mac_dummy.last_pushed_ul_ccch_msg.empty());
//     TESTASSERT_EQ(1, ue_mng.get_ues().size());
//   }
//
//   // Action 2: Start creation of second UE.
//   bench.mac_dummy.last_ue_create_msg.reset();
//   bench.f1ap_dummy.last_ue_create.reset();
//   bench.mac_dummy.last_pushed_ul_ccch_msg.clear();
//   ul_ccch_indication_message ul_ccch_ind2 = ul_ccch_ind;
//   ue_mng.handle_ue_create_request(ul_ccch_ind2);
//
//   if (mode == test_duplicate_ue_creation_mode::mac_ue_create_auto) {
//     // TEST: UE manager does not initiate second UE creation in MAC or F1 due to repeated RNTI.
//     TESTASSERT(not bench.f1ap_dummy.last_ue_create.has_value());
//     TESTASSERT(not bench.mac_dummy.last_ue_create_msg.has_value());
//     TESTASSERT(bench.mac_dummy.last_pushed_ul_ccch_msg.empty());
//   } else {
//     // TEST: MAC didn't start creating second UE because first UE is still being created.
//     TESTASSERT(not bench.f1ap_dummy.last_ue_create.has_value());
//     TESTASSERT(not bench.mac_dummy.last_ue_create_msg.has_value());
//   }
//
//   if (mode == test_duplicate_ue_creation_mode::mac_ue_create_manual) {
//     // Action 4: Finish all the MAC UE creations.
//     bench.mac_dummy.wait_ue_create.ready_ev.set();
//
//     // TEST: First UE creation is complete. Second was aborted due to repeated RNTI.
//     TESTASSERT(not bench.f1ap_dummy.last_ue_create.has_value());
//     TESTASSERT(not bench.mac_dummy.last_ue_create_msg.has_value());
//
//     // TEST: First UE creation once finishes, notifies MAC that UL CCCH can be pushed to upper layers.
//     TESTASSERT(not bench.mac_dummy.last_pushed_ul_ccch_msg.empty());
//   }
//
//   // TEST: Only one UE should have been created
//   TESTASSERT_EQ(1, ue_mng.get_ues().size());
//   TESTASSERT(ue_mng.get_ues().contains(first_ue_index));
//   TESTASSERT_EQ(first_ue_index, ue_mng.get_ues()[first_ue_index].ue_index);
//   TESTASSERT_EQ(ul_ccch_ind.crnti, ue_mng.get_ues()[first_ue_index].rnti);
//   TESTASSERT_EQ(ul_ccch_ind.cell_index, ue_mng.get_ues()[first_ue_index].pcell_index);
// }
//
// TEST_F(du_ue_manager_tester, duplicate_ue_creation)
//{
//   test_duplicate_ue_creation(this->bench, test_duplicate_ue_creation_mode::mac_ue_create_auto);
//   test_duplicate_ue_creation(this->bench, test_duplicate_ue_creation_mode::mac_ue_create_manual);
// }
