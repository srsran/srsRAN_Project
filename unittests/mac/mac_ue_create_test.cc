
#include "../../lib/mac/mac_ctrl/ue_creation_procedure.h"
#include "srsgnb/support/async/manual_event.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

class mac_config_test_adapter : public mac_config_notifier
{
public:
  optional<mac_ue_create_request_response_message> last_ue_created;

  void on_ue_create_request_complete(const mac_ue_create_request_response_message& resp) override
  {
    last_ue_created = resp;
  }
  void on_ue_reconfiguration_complete() override {}
  void on_ue_delete_complete(const mac_ue_delete_response_message& resp) override {}
};

class inline_executor : public task_executor
{
public:
  void execute(unique_task task) final { task(); }
};

class mac_ul_dummy_configurer final : public mac_ul_configurer
{
public:
  bool                                    expected_result = true;
  manual_event_flag                       ue_created_ev;
  optional<mac_ue_create_request_message> last_ue_create_request;

  async_task<bool> add_ue(const mac_ue_create_request_message& msg) override
  {
    return launch_async([this, msg](coro_context<async_task<bool> >& ctx) {
      CORO_BEGIN(ctx);
      last_ue_create_request = msg;
      CORO_AWAIT(ue_created_ev);
      CORO_RETURN(expected_result);
    });
  }

  async_task<void> remove_ue(const mac_ue_delete_request_message& msg) override
  {
    return launch_async([](coro_context<async_task<void> >& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }
};

class mac_dl_dummy_configurer final : public mac_dl_configurer
{
public:
  bool                                    expected_result = true;
  manual_event_flag                       ue_created_ev;
  optional<mac_ue_create_request_message> last_ue_create_request;

  async_task<bool> add_ue(const mac_ue_create_request_message& msg) override
  {
    return launch_async([this, msg](coro_context<async_task<bool> >& ctx) {
      CORO_BEGIN(ctx);
      last_ue_create_request = msg;
      CORO_AWAIT(ue_created_ev);
      CORO_RETURN(expected_result);
    });
  }
  async_task<void> remove_ue(const mac_ue_delete_request_message& msg) override
  {
    return launch_async([](coro_context<async_task<void> >& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }
};

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

void test_ue_create_procedure_single_thread(test_mode tmode)
{
  test_delimit_logger test_delim{"Single threaded UE creation procedure in mode: {}", to_string(tmode)};

  // Run tasks in same thread
  inline_executor               exec;
  std::array<task_executor*, 1> exec_lst = {&exec};

  // Create a MAC config object
  mac_config_test_adapter mac_config_notifier;
  mac_common_config_t     mac_cfg{mac_config_notifier, exec, exec_lst, exec};
  mac_ul_dummy_configurer mac_ul;
  mac_dl_dummy_configurer mac_dl;

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
  async_task<void> proc = launch_async<mac_ue_create_request_procedure>(msg, mac_cfg, mac_ul, mac_dl);

  // STATUS: The MAC UL received the UE creation request message
  TESTASSERT(mac_ul.last_ue_create_request.has_value());
  TESTASSERT(mac_ul.last_ue_create_request->ue_index == msg.ue_index);

  if (not mac_ul.ue_created_ev.is_set()) {
    // STATUS: The procedure is awaiting for the MAC UL UE creation to complete
    TESTASSERT(not proc.ready());
    TESTASSERT(not mac_dl.last_ue_create_request.has_value());
    TESTASSERT(not mac_config_notifier.last_ue_created.has_value());

    // ACTION: Signal MAC UL UE creation completion.
    mac_ul.ue_created_ev.set();
  }

  if (not mac_ul.expected_result) {
    // STATUS: The MAC UL UE creation failed. The UE create procedure finishes right away and notifies configurer
    TESTASSERT(proc.ready());
    TESTASSERT(not mac_dl.last_ue_create_request.has_value());
    TESTASSERT(mac_config_notifier.last_ue_created.has_value());
    TESTASSERT_EQ(1, mac_config_notifier.last_ue_created->ue_index);
    TESTASSERT(not mac_config_notifier.last_ue_created->result);
    return;
  }

  // STATUS: The MAC DL received the UE creation request message
  TESTASSERT(mac_dl.last_ue_create_request.has_value());

  if (not mac_dl.ue_created_ev.is_set()) {
    // STATUS: The procedure is awaiting for the MAC DL UE creation to complete
    TESTASSERT(not proc.ready());
    TESTASSERT(not mac_config_notifier.last_ue_created.has_value());

    // ACTION: Signal MAC UL UE creation completion.
    mac_dl.ue_created_ev.set();
  }

  // STATUS: Procedure ends and UE creation request complete message is sent back to configurer
  TESTASSERT(proc.ready());
  TESTASSERT(mac_config_notifier.last_ue_created.has_value());
  TESTASSERT_EQ(1, mac_config_notifier.last_ue_created->ue_index);
  TESTASSERT(mac_config_notifier.last_ue_created->result == mac_dl.expected_result);
}

int main()
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::info);

  srslog::init();

  test_ue_create_procedure_single_thread(test_mode::success_manual_response);
  test_ue_create_procedure_single_thread(test_mode::success_auto_response);
  test_ue_create_procedure_single_thread(test_mode::failure_ul_ue_create);
  test_ue_create_procedure_single_thread(test_mode::failure_dl_ue_create);
}
