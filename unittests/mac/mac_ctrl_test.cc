
#include "../../lib/mac/mac_ctrl/mac_ctrl_component.h"
#include "mac_ctrl_test_dummies.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

enum class test_outcome { success, ue_create_failure };

void test_ue_concurrent_procedures(test_outcome outcome)
{
  test_delimit_logger delimiter{"Test UE concurrent procedures. Outcome: {}",
                                outcome == test_outcome::success ? "Success" : "UE create failure"};

  mac_config_notification_recorder mac_cfg_notifier;
  inline_executor                  exec;
  std::vector<task_executor*>      dl_execs({&exec});
  mac_common_config_t              cfg{mac_cfg_notifier, exec, dl_execs, exec};
  mac_ul_dummy_configurer          ul_unit;
  mac_dl_dummy_configurer          dl_unit;

  mac_ctrl_component mac_ctrl(cfg, ul_unit, dl_unit);

  TESTASSERT(mac_ctrl.find_by_rnti(0x4601) == nullptr);
  TESTASSERT(mac_ctrl.find_ue(0) == nullptr);

  // Action 1: Start UE creation
  mac_ue_create_request_message ue_create_msg{};
  ue_create_msg.ue_index   = 0;
  ue_create_msg.cell_index = 0;
  ue_create_msg.crnti      = 0x4601;
  mac_ctrl.ue_create_request(ue_create_msg);

  // TEST: MAC UL started adding UE but hasn't returned yet
  TESTASSERT(ul_unit.last_ue_create_request.has_value());
  TESTASSERT(not dl_unit.last_ue_create_request.has_value());

  // Action 2: Finish MAC UL UE creation
  ul_unit.ue_created_ev.set();

  // TEST: MAC UL finished, MAC DL UE creation started but hasn't finished yet
  TESTASSERT(dl_unit.last_ue_create_request.has_value());

  // Action 2: Start concurrent UE deletion
  mac_ue_delete_request_message ue_delete_msg{};
  ue_delete_msg.ue_index   = 0;
  ue_delete_msg.cell_index = 0;
  ue_delete_msg.rnti       = 0x4601;
  mac_ctrl.ue_delete_request(ue_delete_msg);

  // Given that UE creation task hasn't finished, the UE deletion isn't processed yet
  TESTASSERT(not ul_unit.last_ue_delete_request.has_value());
  TESTASSERT(not dl_unit.last_ue_delete_request.has_value());

  // Action 3: Finish MAC DL UE creation
  dl_unit.expected_result = outcome != test_outcome::ue_create_failure;
  dl_unit.ue_created_ev.set();
  if (outcome == test_outcome::ue_create_failure) {
    // UE creation failed. Ensure the UE object was deleted
    TESTASSERT(mac_ctrl.find_by_rnti(0x4601) == nullptr);
    TESTASSERT(mac_ctrl.find_ue(0) == nullptr);
    TESTASSERT(mac_cfg_notifier.last_ue_created.has_value());
    TESTASSERT(not mac_cfg_notifier.last_ue_created.value().result);
    return;
  }

  // UE creation completed, and UE delection procedure started
  TESTASSERT(mac_cfg_notifier.last_ue_created.has_value());
  TESTASSERT(mac_cfg_notifier.last_ue_created.value().result);
  TESTASSERT(dl_unit.last_ue_delete_request.has_value());
  TESTASSERT(ul_unit.last_ue_delete_request.has_value());
  TESTASSERT(mac_cfg_notifier.last_ue_deleted.has_value());
  TESTASSERT(mac_cfg_notifier.last_ue_deleted.value().result);

  // UE is removed from ue_db
  TESTASSERT(mac_ctrl.find_by_rnti(0x4601) == nullptr);
  TESTASSERT(mac_ctrl.find_ue(0) == nullptr);
}

void test_inexistent_ue_removal()
{
  test_delimit_logger delimiter{"Test inexistent UE removal"};

  mac_config_notification_recorder mac_cfg_notifier;
  inline_executor                  exec;
  std::vector<task_executor*>      dl_execs({&exec});
  mac_common_config_t              cfg{mac_cfg_notifier, exec, dl_execs, exec};
  mac_ul_dummy_configurer          ul_unit;
  mac_dl_dummy_configurer          dl_unit;

  mac_ctrl_component mac_ctrl(cfg, ul_unit, dl_unit);

  // Action 1: Start UE deletion
  mac_ue_delete_request_message ue_delete_msg{};
  ue_delete_msg.ue_index   = 0;
  ue_delete_msg.cell_index = 0;
  ue_delete_msg.rnti       = 0x4601;
  mac_ctrl.ue_delete_request(ue_delete_msg);

  TESTASSERT(mac_cfg_notifier.last_ue_deleted.has_value());
  TESTASSERT(not mac_cfg_notifier.last_ue_deleted.value().result);
  TESTASSERT(not dl_unit.last_ue_delete_request.has_value());
  TESTASSERT(not ul_unit.last_ue_delete_request.has_value());
}

void test_duplicate_ue_creation()
{
  test_delimit_logger delimiter{"Test duplicate UE creation"};

  mac_config_notification_recorder mac_cfg_notifier;
  inline_executor                  exec;
  std::vector<task_executor*>      dl_execs({&exec});
  mac_common_config_t              cfg{mac_cfg_notifier, exec, dl_execs, exec};
  mac_ul_dummy_configurer          ul_unit;
  mac_dl_dummy_configurer          dl_unit;

  mac_ctrl_component mac_ctrl(cfg, ul_unit, dl_unit);

  // Action 1: Start UE creation
  mac_ue_create_request_message ue_create_msg{};
  ue_create_msg.ue_index   = 1;
  ue_create_msg.cell_index = 0;
  ue_create_msg.crnti      = 0x4601;
  mac_ctrl.ue_create_request(ue_create_msg);

  // Action 2: State UE creation with repeated UE index but different RNTI
  ue_create_msg.crnti = 0x4602;
  mac_ctrl.ue_create_request(ue_create_msg);

  // The MAC should have reported the failed UE creation
  TESTASSERT(mac_cfg_notifier.last_ue_created.has_value());
  TESTASSERT_EQ(1, mac_cfg_notifier.last_ue_created.value().ue_index);
  TESTASSERT(not mac_cfg_notifier.last_ue_created.value().result);

  // Action 3: State UE creation with repeated RNTI but same UE index
  ue_create_msg.crnti = 0x4601;
  ue_create_msg.ue_index = 2;
  mac_ctrl.ue_create_request(ue_create_msg);

  // The MAC should have reported the failed UE creation
  TESTASSERT(mac_cfg_notifier.last_ue_created.has_value());
  TESTASSERT_EQ(2, mac_cfg_notifier.last_ue_created.value().ue_index);
  TESTASSERT(not mac_cfg_notifier.last_ue_created.value().result);

  // Action 3: Finish MAC UL and DL UE creation
  ul_unit.ue_created_ev.set();
  dl_unit.ue_created_ev.set();

  // The MAC should report the created UE
  TESTASSERT(mac_cfg_notifier.last_ue_created.has_value());
  TESTASSERT_EQ(1, mac_cfg_notifier.last_ue_created.value().ue_index);
  TESTASSERT(mac_cfg_notifier.last_ue_created.value().result);
}

int main()
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);

  srslog::init();

  test_ue_concurrent_procedures(test_outcome::ue_create_failure);
  test_ue_concurrent_procedures(test_outcome::success);
  test_inexistent_ue_removal();
  test_duplicate_ue_creation();
}