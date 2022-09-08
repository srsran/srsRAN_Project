/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../lib/du_manager/du_ue_manager.h"
#include "srsgnb/support/async/async_test_utils.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace srs_du;

class mac_test_dummy : public mac_ue_configurator
{
public:
  optional<mac_ue_create_request_message>                           last_ue_create_msg{};
  optional<mac_ue_delete_request_message>                           last_ue_delete_msg{};
  byte_buffer                                                       last_pushed_ul_ccch_msg;
  wait_manual_event_tester<mac_ue_create_response_message>          wait_ue_create;
  wait_manual_event_tester<mac_ue_reconfiguration_response_message> wait_ue_reconf;
  wait_manual_event_tester<mac_ue_delete_response_message>          wait_ue_delete;

  async_task<mac_ue_create_response_message> handle_ue_create_request(const mac_ue_create_request_message& msg) override
  {
    last_ue_create_msg = msg;
    return wait_ue_create.launch();
  }
  async_task<mac_ue_reconfiguration_response_message>
  handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& msg) override
  {
    return wait_ue_reconf.launch();
  }
  async_task<mac_ue_delete_response_message> handle_ue_delete_request(const mac_ue_delete_request_message& msg) override
  {
    last_ue_delete_msg = msg;
    return wait_ue_delete.launch();
  }
  void handle_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer pdu) override
  {
    last_pushed_ul_ccch_msg = std::move(pdu);
  }
};

class f1_ue_context_manager_dummy : public f1ap_ue_context_manager
{
public:
  optional<f1ap_ue_create_request>                                        last_ue_create{};
  f1ap_ue_create_response                                                 next_ue_create_response;
  wait_manual_event_tester<f1ap_ue_context_modification_response_message> wait_ue_mod;

  /// Initiates creation of UE context in F1.
  f1ap_ue_create_response handle_ue_creation_request(const f1ap_ue_create_request& msg) override
  {
    last_ue_create = msg;
    return next_ue_create_response;
  }

  void handle_ue_context_release_request(const f1ap_ue_context_release_request_message& request) override {}

  async_task<f1ap_ue_context_modification_response_message>
  handle_ue_context_modification_required(const f1ap_ue_context_modification_required_message& msg) override
  {
    return wait_ue_mod.launch();
  }

  void handle_ue_inactivity_notification(const f1ap_ue_inactivity_notification_message& msg) override {}

  void handle_notify(const f1ap_notify_message& msg) override {}
};

enum class test_outcome { success, ue_create_failure };

/// Test the scenario where concurrent UECreateRequest and UECreateDelete messages arrive at UE manager
void test_ue_concurrent_procedures(test_outcome outcome)
{
  test_delimit_logger   delimiter{"Test UE concurrent procedures. Outcome: {}",
                                outcome == test_outcome::success ? "Success" : "UE create failure"};
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  mac_test_dummy              mac_dummy;
  f1_ue_context_manager_dummy f1_ue_ctx_mng_dummy;
  f1_ue_ctx_mng_dummy.next_ue_create_response.result = true;
  f1_ue_ctx_mng_dummy.next_ue_create_response.bearers_added.resize(1);

  du_manager_config_t cfg{};
  cfg.mac_ue_mng      = &mac_dummy;
  cfg.f1ap_ue_ctx_mng = &f1_ue_ctx_mng_dummy;

  du_ue_manager ue_mng{cfg};
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
  TESTASSERT(f1_ue_ctx_mng_dummy.last_ue_create.has_value());
  ue_index = f1_ue_ctx_mng_dummy.last_ue_create.value().ue_index;
  TESTASSERT(ue_index < MAX_NOF_DU_UES);
  TESTASSERT(mac_dummy.last_pushed_ul_ccch_msg.empty());
  TESTASSERT(ue_mng.get_ues().empty());

  // TEST: MAC UE creation started but hasn't returned yet
  TESTASSERT(mac_dummy.last_ue_create_msg.has_value());
  TESTASSERT_EQ(ccch_ind.crnti, mac_dummy.last_ue_create_msg->crnti);
  TESTASSERT_EQ(ccch_ind.cell_index, mac_dummy.last_ue_create_msg->cell_index);
  TESTASSERT_EQ(1, mac_dummy.last_ue_create_msg->bearers.size());
  TESTASSERT_EQ(0, mac_dummy.last_ue_create_msg->bearers[0].lcid);
  TESTASSERT(mac_dummy.last_ue_create_msg->bearers[0].ul_bearer != nullptr);
  // TODO: Check DL bearer
  TESTASSERT(ue_mng.get_ues().empty());

  // Action 3: Start concurrent UE deletion
  du_ue_delete_message ue_delete_msg{};
  ue_delete_msg.ue_index = ue_index;
  test_logger.info("TEST: Starting UE deletion with UE index={}...", ue_delete_msg.ue_index);
  ue_mng.handle_ue_delete_request(ue_delete_msg);

  // TEST: Given that UE creation task hasn't finished, the UE deletion isn't processed yet
  TESTASSERT(not mac_dummy.last_ue_delete_msg.has_value());

  // Action 4: MAC UE creation completed
  mac_dummy.wait_ue_create.result.ue_index   = ue_index;
  mac_dummy.wait_ue_create.result.cell_index = ccch_ind.cell_index;
  mac_dummy.wait_ue_create.result.result     = outcome != test_outcome::ue_create_failure;
  mac_dummy.wait_ue_create.ready_ev.set();

  // TEST: UE manager finished UE creation procedure. MAC should push UL CCCH to upper layers.
  if (outcome == test_outcome::success) {
    TESTASSERT(not mac_dummy.last_pushed_ul_ccch_msg.empty());
    TESTASSERT(not ue_mng.get_ues().empty());
    TESTASSERT(ue_mng.get_ues().contains(ue_index));
    TESTASSERT(ue_mng.get_ues()[ue_index].rnti == 0x4601);
  } else {
    TESTASSERT(ue_mng.get_ues().empty());
  }

  // TEST: If MAC UE created failed, MAC UE Delete Request should not have been started
  if (outcome != test_outcome::success) {
    TESTASSERT(not mac_dummy.last_ue_delete_msg.has_value());
    return;
  }
  TESTASSERT(mac_dummy.last_ue_delete_msg.has_value());
  TESTASSERT_EQ(ue_index, mac_dummy.last_ue_delete_msg->ue_index);

  // Action 5: MAC UE Deletion complete
  mac_dummy.wait_ue_delete.result.result = true;
  mac_dummy.wait_ue_delete.ready_ev.set();

  // TEST: DU UE Deletion is complete
  TESTASSERT(ue_mng.get_ues().empty());
  // TODO: Check F1AP delete
}

/// Test the scenario where a ue delete request arrives for an inexistent ue_index
void test_inexistent_ue_removal()
{
  test_delimit_logger delimiter{"Test inexistent UE removal"};

  mac_test_dummy              mac_dummy;
  f1_ue_context_manager_dummy f1_ue_ctx_mng_dummy;

  du_manager_config_t cfg{};
  cfg.mac_ue_mng      = &mac_dummy;
  cfg.f1ap_ue_ctx_mng = &f1_ue_ctx_mng_dummy;

  du_ue_manager ue_mng{cfg};
  TESTASSERT(ue_mng.get_ues().empty());

  // Action 1: Start UE deletion
  du_ue_delete_message ue_delete_msg{};
  ue_delete_msg.ue_index = to_du_ue_index(0);
  ue_mng.handle_ue_delete_request(ue_delete_msg);

  // There should not be any reply from MAC and F1AP should receive failure signal
  TESTASSERT(ue_mng.get_ues().empty());
  TESTASSERT(not mac_dummy.last_ue_create_msg.has_value());
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

  mac_test_dummy              mac_dummy;
  f1_ue_context_manager_dummy f1_ue_ctx_mng_dummy;

  du_manager_config_t cfg{};
  cfg.mac_ue_mng      = &mac_dummy;
  cfg.f1ap_ue_ctx_mng = &f1_ue_ctx_mng_dummy;

  f1_ue_ctx_mng_dummy.next_ue_create_response.result = true;
  f1_ue_ctx_mng_dummy.next_ue_create_response.bearers_added.resize(1);

  mac_dummy.wait_ue_create.result.ue_index   = first_ue_index;
  mac_dummy.wait_ue_create.result.cell_index = to_du_cell_index(0);
  mac_dummy.wait_ue_create.result.result     = true;
  if (mode == test_duplicate_ue_creation_mode::mac_ue_create_auto) {
    mac_dummy.wait_ue_create.ready_ev.set();
  }

  du_ue_manager ue_mng{cfg};
  TESTASSERT(ue_mng.get_ues().empty());

  // Action 1: Start creation of UE by notifying UL CCCH decoding.
  ul_ccch_indication_message ul_ccch_ind{};
  ul_ccch_ind.cell_index = to_du_cell_index(0);
  ul_ccch_ind.crnti      = to_rnti(0x4601);
  ul_ccch_ind.subpdu     = {1, 2, 3, 4, 5};
  ue_mng.handle_ue_create_request(ul_ccch_ind);

  // TEST: F1 started creating first UE.
  TESTASSERT(f1_ue_ctx_mng_dummy.last_ue_create.has_value());
  first_ue_index = f1_ue_ctx_mng_dummy.last_ue_create.value().ue_index;
  TESTASSERT(f1_ue_ctx_mng_dummy.last_ue_create.value().ue_index < MAX_NOF_DU_UES);

  // TEST: MAC started creating first UE.
  TESTASSERT(mac_dummy.last_ue_create_msg.has_value());
  TESTASSERT_EQ(first_ue_index, mac_dummy.last_ue_create_msg->ue_index);
  TESTASSERT_EQ(ul_ccch_ind.crnti, mac_dummy.last_ue_create_msg->crnti);
  if (mode == test_duplicate_ue_creation_mode::mac_ue_create_auto) {
    // TEST: First UE create procedure is complete
    TESTASSERT(not mac_dummy.last_pushed_ul_ccch_msg.empty());
    TESTASSERT_EQ(1, ue_mng.get_ues().size());
  }

  // Action 2: Start creation of second UE.
  mac_dummy.last_ue_create_msg.reset();
  f1_ue_ctx_mng_dummy.last_ue_create.reset();
  mac_dummy.last_pushed_ul_ccch_msg.clear();
  ul_ccch_indication_message ul_ccch_ind2 = ul_ccch_ind;
  ue_mng.handle_ue_create_request(ul_ccch_ind2);

  if (mode == test_duplicate_ue_creation_mode::mac_ue_create_auto) {
    // TEST: UE manager does not initiate second UE creation in MAC or F1 due to repeated RNTI.
    TESTASSERT(not f1_ue_ctx_mng_dummy.last_ue_create.has_value());
    TESTASSERT(not mac_dummy.last_ue_create_msg.has_value());
    TESTASSERT(mac_dummy.last_pushed_ul_ccch_msg.empty());
  } else {
    // TEST: MAC didn't start creating second UE because first UE is still being created.
    TESTASSERT(not f1_ue_ctx_mng_dummy.last_ue_create.has_value());
    TESTASSERT(not mac_dummy.last_ue_create_msg.has_value());
  }

  if (mode == test_duplicate_ue_creation_mode::mac_ue_create_manual) {
    // Action 4: Finish all the MAC UE creations.
    mac_dummy.wait_ue_create.ready_ev.set();

    // TEST: First UE creation is complete. Second was aborted due to repeated RNTI.
    TESTASSERT(not f1_ue_ctx_mng_dummy.last_ue_create.has_value());
    TESTASSERT(not mac_dummy.last_ue_create_msg.has_value());

    // TEST: First UE creation once finishes, notifies MAC that UL CCCH can be pushed to upper layers.
    TESTASSERT(not mac_dummy.last_pushed_ul_ccch_msg.empty());
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
