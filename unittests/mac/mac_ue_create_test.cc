
#include "../../lib/mac/mac_ctrl/ue_creation_procedure.h"
#include "mac_ctrl_test_dummies.h"
#include "srsgnb/support/async/manual_event.h"
#include "srsgnb/support/executors/manual_worker.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

/// This type is used as an test argument to specify which ue creation procedure scenario we want to test
enum class test_mode { success_manual_response, success_auto_response, failure_ul_ue_create, failure_dl_ue_create };
std::string to_string(test_mode m)
{
  switch (m) {
    case test_mode::success_auto_response:
      return "success expected, auto response";
    case test_mode::success_manual_response:
      return "success expected, manual response";
    case test_mode::failure_ul_ue_create:
      return "failure in MAC UL UE creation";
    case test_mode::failure_dl_ue_create:
      return "failure in MAC DL UE creation";
    default:
      srsran_terminate("Invalid test mode");
  }
}

void test_mac_ue_creation_procedure(test_mode tmode)
{
  test_delimit_logger test_delim{"Single threaded UE creation procedure in mode: {}", to_string(tmode)};

  // Run tasks in same thread
  manual_worker                 worker{128};
  std::array<task_executor*, 1> exec_lst = {&worker};
  dummy_ul_executor_mapper      ul_exec_mapper{worker};
  dummy_mac_event_indicator     du_mng_notif;

  // Create a MAC config object
  mac_common_config_t       mac_cfg{du_mng_notif, ul_exec_mapper, exec_lst, worker};
  mac_ctrl_dummy_configurer mac_ctrl;
  mac_ul_dummy_configurer   mac_ul;
  mac_dl_dummy_configurer   mac_dl;

  // Launch procedure
  mac_ue_create_request_message msg{};
  msg.ue_index   = 1;
  msg.crnti      = 0x4601;
  msg.cell_index = 0;
  msg.bearers.emplace_back();
  msg.bearers[0].lcid      = 0;
  msg.bearers[0].dl_bearer = nullptr; // procedure should not use bearers
  msg.bearers[0].ul_bearer = nullptr; // procedure should not use bearers

  if (tmode == test_mode::success_auto_response) {
    // The MAC UL and MAC DL will not wait for event to be set
    mac_dl.ue_created_ev.set();
    mac_ul.ue_created_ev.set();
  } else if (tmode == test_mode::failure_ul_ue_create) {
    mac_ul.expected_result = false;
  } else if (tmode == test_mode::failure_dl_ue_create) {
    mac_dl.expected_result = false;
  }

  // ACTION: Procedure is launched
  async_task<mac_ue_create_response_message> proc =
      launch_async<mac_ue_create_request_procedure>(msg, mac_cfg, mac_ctrl, mac_ul, mac_dl);

  // STATUS: The MAC UL received the UE creation request message
  TESTASSERT(mac_ul.last_ue_create_request.has_value());
  TESTASSERT(mac_ul.last_ue_create_request->ue_index == msg.ue_index);

  if (not mac_ul.ue_created_ev.is_set()) {
    // STATUS: The procedure is awaiting for the MAC UL UE creation to complete
    TESTASSERT(not proc.ready());
    TESTASSERT(not mac_dl.last_ue_create_request.has_value());

    // ACTION: Signal MAC UL UE creation completion.
    mac_ul.ue_created_ev.set();
  }

  if (not mac_ul.expected_result) {
    // STATUS: The MAC UL UE creation failed. The UE create procedure finishes right away and notifies configurer
    TESTASSERT(not mac_dl.last_ue_create_request.has_value());
    TESTASSERT(proc.ready());
    TESTASSERT_EQ(1, proc.get().ue_index);
    TESTASSERT(not proc.get().result);
    return;
  }

  // STATUS: The MAC DL received the UE creation request message
  TESTASSERT(mac_dl.last_ue_create_request.has_value());

  if (not mac_dl.ue_created_ev.is_set()) {
    // STATUS: The procedure is awaiting for the MAC DL UE creation to complete
    TESTASSERT(not proc.ready());

    // ACTION: Signal MAC UL UE creation completion.
    mac_dl.ue_created_ev.set();
  }

  // STATUS: Procedure ends and UE creation request complete message is sent back to configurer
  TESTASSERT(proc.ready());
  TESTASSERT_EQ(mac_dl.expected_result, proc.get().result);
  TESTASSERT_EQ(1, proc.get().ue_index);
}

int main()
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::info);

  srslog::init();

  test_mac_ue_creation_procedure(test_mode::success_manual_response);
  test_mac_ue_creation_procedure(test_mode::success_auto_response);
  test_mac_ue_creation_procedure(test_mode::failure_ul_ue_create);
  test_mac_ue_creation_procedure(test_mode::failure_dl_ue_create);
}
