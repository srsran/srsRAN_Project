
#include "../../lib/mac/mac_ctrl/mac_ctrl_component.h"
#include "mac_ctrl_test_dummies.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

void test_mac_ctrl_ue_procedures()
{
  test_delimit_logger delimiter{"Test UE procedures"};

  inline_executor             exec;
  std::vector<task_executor*> dl_execs({&exec});
  mac_common_config_t         cfg{exec, dl_execs, exec};
  mac_ul_dummy_configurer     ul_unit;
  mac_dl_dummy_configurer     dl_unit;

  mac_ctrl_component mac_ctrl(cfg, ul_unit, dl_unit);

  // Action 1: Create UE
  mac_ue_create_request_message ue_create_msg{};
  ue_create_msg.ue_index                       = 1;
  ue_create_msg.cell_index                     = 0;
  ue_create_msg.crnti                          = 0x4601;
  async_task<mac_ue_create_response_message> t = mac_ctrl.ue_create_request(ue_create_msg);

  // Status: UE creation started in MAC UL but not in MAC DL
  TESTASSERT(ul_unit.last_ue_create_request.has_value());
  TESTASSERT_EQ(ue_create_msg.ue_index, ul_unit.last_ue_create_request->ue_index);
  TESTASSERT_EQ(ue_create_msg.crnti, ul_unit.last_ue_create_request->crnti);
  TESTASSERT(not dl_unit.last_ue_create_request.has_value());
  TESTASSERT(not t.ready());

  // Action 2: MAC UL UE Creation finishes
  ul_unit.expected_result = true;
  ul_unit.ue_created_ev.set();

  // Status: MAC DL UE Creation starts
  TESTASSERT(dl_unit.last_ue_create_request.has_value());
  TESTASSERT_EQ(ue_create_msg.ue_index, dl_unit.last_ue_create_request->ue_index);
  TESTASSERT_EQ(ue_create_msg.crnti, dl_unit.last_ue_create_request->crnti);
  TESTASSERT(not t.ready());

  // Action 3: MAC DL UE Creation finishes
  dl_unit.expected_result = true;
  dl_unit.ue_created_ev.set();

  // Status: MAC DL UE Creation finished. MAC CTRL UE Creation finished as well.
  TESTASSERT(dl_unit.last_ue_create_request.has_value());
  TESTASSERT(t.ready());
  TESTASSERT_EQ(ue_create_msg.ue_index, t.get().ue_index);
  TESTASSERT(t.get().result);
  TESTASSERT(mac_ctrl.find_ue(ue_create_msg.ue_index) != nullptr);
  TESTASSERT(mac_ctrl.find_by_rnti(ue_create_msg.crnti) != nullptr);
  TESTASSERT_EQ(ue_create_msg.ue_index, mac_ctrl.find_ue(ue_create_msg.ue_index)->du_ue_index);

  // Action 4: Delete UE
  mac_ue_delete_request_message ue_delete_msg{};
  ue_delete_msg.ue_index                        = 1;
  ue_delete_msg.rnti                            = 0x4601;
  ue_delete_msg.cell_index                      = 0;
  async_task<mac_ue_delete_response_message> t2 = mac_ctrl.ue_delete_request(ue_delete_msg);

  // Status: UE deleted from MAC DL, UL and CTRL
  TESTASSERT(dl_unit.last_ue_delete_request.has_value());
  TESTASSERT_EQ(ue_delete_msg.ue_index, dl_unit.last_ue_create_request->ue_index);
  TESTASSERT(ul_unit.last_ue_delete_request.has_value());
  TESTASSERT_EQ(ue_delete_msg.ue_index, ul_unit.last_ue_create_request->ue_index);
  TESTASSERT(t2.ready());
  TESTASSERT_EQ(ue_delete_msg.ue_index, t2.get().ue_index);
  TESTASSERT(mac_ctrl.find_ue(ue_create_msg.ue_index) == nullptr);
  TESTASSERT(mac_ctrl.find_by_rnti(ue_create_msg.crnti) == nullptr);
}

int main()
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);

  srslog::init();

  test_mac_ctrl_ue_procedures();
}
