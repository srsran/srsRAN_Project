
#include "../../lib/du_manager/du_ue_manager.h"
#include "srsgnb/support/async/async_test_utils.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

class mac_test_dummy : public mac_configurer
{
public:
  optional<mac_ue_create_request_message>                           last_ue_create_msg{};
  optional<mac_ue_delete_request_message>                           last_ue_delete_msg{};
  wait_manual_event_tester<mac_ue_create_response_message>          wait_ue_create;
  wait_manual_event_tester<mac_ue_reconfiguration_response_message> wait_ue_reconf;
  wait_manual_event_tester<mac_ue_delete_response_message>          wait_ue_delete;

  async_task<mac_ue_create_response_message> ue_create_request(const mac_ue_create_request_message& msg) override
  {
    last_ue_create_msg = msg;
    return wait_ue_create.launch();
  }
  async_task<mac_ue_reconfiguration_response_message>
  ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& msg) override
  {
    return wait_ue_reconf.launch();
  }
  async_task<mac_ue_delete_response_message> ue_delete_request(const mac_ue_delete_request_message& msg) override
  {
    last_ue_delete_msg = msg;
    return wait_ue_delete.launch();
  }
};

enum class test_outcome { success, ue_create_failure };

/// Test the scenario where concurrent UECreateRequest and UECreateDelete messages arrive at UE manager
void test_ue_concurrent_procedures(test_outcome outcome)
{
  test_delimit_logger delimiter{"Test UE concurrent procedures. Outcome: {}",
                                outcome == test_outcome::success ? "Success" : "UE create failure"};

  mac_test_dummy   mac_dummy;
  f1ap_cfg_adapter f1ap_dummy;

  du_manager_config_t cfg{};
  cfg.mac = &mac_dummy;

  du_ue_manager ue_mng{cfg};
  TESTASSERT(ue_mng.get_ues().empty());

  // Action 1: Start UE creation
  du_ue_create_message ue_create_msg{};
  ue_create_msg.ue_index   = 1;
  ue_create_msg.cell_index = 0;
  ue_create_msg.crnti      = 0x4601;
  ue_create_msg.logical_channels_to_add.emplace_back();
  ue_create_msg.logical_channels_to_add[0].lcid = 1;
  ue_mng.handle_ue_create_request(ue_create_msg);

  // TEST: MAC UE creation started but hasn't returned yet
  TESTASSERT(mac_dummy.last_ue_create_msg.has_value());
  TESTASSERT_EQ(ue_create_msg.ue_index, mac_dummy.last_ue_create_msg->ue_index);
  TESTASSERT_EQ(ue_create_msg.crnti, mac_dummy.last_ue_create_msg->crnti);
  TESTASSERT_EQ(ue_create_msg.cell_index, mac_dummy.last_ue_create_msg->cell_index);
  TESTASSERT_EQ(1, mac_dummy.last_ue_create_msg->bearers.size());
  TESTASSERT_EQ(1, mac_dummy.last_ue_create_msg->bearers[0].lcid);
  TESTASSERT(mac_dummy.last_ue_create_msg->bearers[0].ul_bearer != nullptr);
  // TODO: Check DL bearer
  TESTASSERT(ue_mng.get_ues().empty());

  // Action 2: Start concurrent UE deletion
  du_ue_delete_message ue_delete_msg{};
  ue_delete_msg.ue_index = 1;
  ue_mng.handle_ue_delete_request(ue_delete_msg);

  // TEST: Given that UE creation task hasn't finished, the UE deletion isn't processed yet
  TESTASSERT(not mac_dummy.last_ue_delete_msg.has_value());

  // Action 3: MAC UE creation completed
  mac_dummy.wait_ue_create.result.ue_index   = ue_create_msg.ue_index;
  mac_dummy.wait_ue_create.result.cell_index = ue_create_msg.cell_index;
  mac_dummy.wait_ue_create.result.result     = outcome != test_outcome::ue_create_failure;
  mac_dummy.wait_ue_create.ready_ev.set();

  // TEST: UE manager finished UE creation procedure
  TESTASSERT(f1ap_dummy.last_ue_create_resp.has_value());
  TESTASSERT(f1ap_dummy.last_ue_create_resp->ue_index == ue_create_msg.ue_index);
  TESTASSERT(f1ap_dummy.last_ue_create_resp->result == (outcome == test_outcome::success));
  if (outcome == test_outcome::success) {
    TESTASSERT(not ue_mng.get_ues().empty());
    TESTASSERT(ue_mng.get_ues().contains(ue_create_msg.ue_index));
    TESTASSERT(ue_mng.get_ues()[ue_create_msg.ue_index].rnti == 0x4601);
  } else {
    TESTASSERT(ue_mng.get_ues().empty());
  }

  // TEST: If MAC UE created failed, MAC UE Delete Request should not have been started
  if (outcome != test_outcome::success) {
    TESTASSERT(not mac_dummy.last_ue_delete_msg.has_value());
    return;
  }
  TESTASSERT(mac_dummy.last_ue_delete_msg.has_value());
  TESTASSERT_EQ(1, mac_dummy.last_ue_delete_msg->ue_index);

  // Action 4: MAC UE Deletion complete
  mac_dummy.wait_ue_delete.result.ue_index = ue_create_msg.ue_index;
  mac_dummy.wait_ue_delete.result.result   = true;
  mac_dummy.wait_ue_delete.ready_ev.set();

  // TEST: DU UE Deletion is complete
  TESTASSERT(ue_mng.get_ues().empty());
  // TODO: Check F1AP delete
}

/// Test the scenario where a ue delete request arrives for an inexistent ue_index
void test_inexistent_ue_removal()
{
  test_delimit_logger delimiter{"Test inexistent UE removal"};

  mac_test_dummy mac_dummy;

  du_manager_config_t cfg{};
  cfg.mac = &mac_dummy;

  du_ue_manager ue_mng{cfg};
  TESTASSERT(ue_mng.get_ues().empty());

  // Action 1: Start UE deletion
  du_ue_delete_message ue_delete_msg{};
  ue_delete_msg.ue_index = 0;
  ue_mng.handle_ue_delete_request(ue_delete_msg);

  // There should not be any reply from MAC and F1AP should receive failure signal
  TESTASSERT(ue_mng.get_ues().empty());
  TESTASSERT(not mac_dummy.last_ue_create_msg.has_value());
  // TODO: F1AP check
}

/// Different scenarios for testing duplicate UE creation
enum class test_duplicate_ue_creation_mode { mac_ue_create_auto, mac_ue_create_manual };

/// Test the scenario where two UEs with the same ue_index are added
void test_duplicate_ue_creation(test_duplicate_ue_creation_mode mode)
{
  test_delimit_logger delimiter{"Test duplicate UE creation. Mode: {}",
                                mode == test_duplicate_ue_creation_mode::mac_ue_create_auto ? "auto" : "manual"};

  const du_ue_index_t ue_index = 1;

  mac_test_dummy   mac_dummy;
  f1ap_cfg_adapter f1ap_dummy;

  du_manager_config_t cfg{};
  cfg.mac               = &mac_dummy;
  cfg.f1ap_cfg_notifier = &f1ap_dummy;

  mac_dummy.wait_ue_create.result.ue_index   = ue_index;
  mac_dummy.wait_ue_create.result.cell_index = 0;
  mac_dummy.wait_ue_create.result.result     = true;
  if (mode == test_duplicate_ue_creation_mode::mac_ue_create_auto) {
    mac_dummy.wait_ue_create.ready_ev.set();
  }

  du_ue_manager ue_mng{cfg};
  TESTASSERT(ue_mng.get_ues().empty());

  // Action 1: Start creation of UE
  du_ue_create_message ue_create_msg{};
  ue_create_msg.ue_index   = ue_index;
  ue_create_msg.cell_index = 0;
  ue_create_msg.crnti      = 0x4601;
  ue_create_msg.logical_channels_to_add.emplace_back();
  ue_create_msg.logical_channels_to_add[0].lcid = 1;
  ue_mng.handle_ue_create_request(ue_create_msg);

  // TEST: MAC started creating UE 1
  TESTASSERT(mac_dummy.last_ue_create_msg.has_value());
  TESTASSERT_EQ(ue_index, mac_dummy.last_ue_create_msg->ue_index);
  TESTASSERT_EQ(0x4601, mac_dummy.last_ue_create_msg->crnti);
  if (mode == test_duplicate_ue_creation_mode::mac_ue_create_auto) {
    // TEST: UE 1 create procedure is complete
    TESTASSERT(f1ap_dummy.last_ue_create_resp.has_value());
    TESTASSERT_EQ(ue_index, f1ap_dummy.last_ue_create_resp->ue_index);
    TESTASSERT(f1ap_dummy.last_ue_create_resp->result);
  }

  // Action 2: Start creation of UE with repeated UE index
  mac_dummy.last_ue_create_msg.reset();
  f1ap_dummy.last_ue_create_resp.reset();
  du_ue_create_message ue_create_msg2 = ue_create_msg;
  ue_create_msg2.crnti                = 0x4602;
  ue_mng.handle_ue_create_request(ue_create_msg2);

  if (mode == test_duplicate_ue_creation_mode::mac_ue_create_auto) {
    // TEST: UE manager does not call MAC to create UE 2
    TESTASSERT(not mac_dummy.last_ue_create_msg.has_value());

    // TEST: UE 2 create procedure is complete
    TESTASSERT(f1ap_dummy.last_ue_create_resp.has_value());
    TESTASSERT_EQ(ue_index, f1ap_dummy.last_ue_create_resp->ue_index);
    TESTASSERT(not f1ap_dummy.last_ue_create_resp->result);
  } else {
    // TEST: MAC didn't start creating UE 2
    TESTASSERT(not mac_dummy.last_ue_create_msg.has_value());
  }

  if (mode == test_duplicate_ue_creation_mode::mac_ue_create_manual) {
    // Action 4: Finish all the MAC UE creations
    mac_dummy.wait_ue_create.ready_ev.set();

    // TEST: UE 1, 2 create procedures have run to completion
    TESTASSERT(f1ap_dummy.last_ue_create_resp.has_value());
    TESTASSERT_EQ(ue_index, f1ap_dummy.last_ue_create_resp->ue_index);
    TESTASSERT(not f1ap_dummy.last_ue_create_resp->result);
  }

  // TEST: Only one UE should have been created
  TESTASSERT_EQ(1, ue_mng.get_ues().size());
  TESTASSERT(ue_mng.get_ues().contains(ue_index));
  TESTASSERT_EQ(ue_index, ue_mng.get_ues()[ue_index].ue_index);
  TESTASSERT_EQ(0x4601, ue_mng.get_ues()[ue_index].rnti);
  TESTASSERT_EQ(0, ue_mng.get_ues()[ue_index].pcell_index);
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