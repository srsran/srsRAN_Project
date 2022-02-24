
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
  manual_event_flag                       ue_created_ev;
  optional<mac_ue_create_request_message> last_ue_create_request;

  async_task<void> add_ue(const mac_ue_create_request_message& msg) override
  {
    return launch_async([this, msg](coro_context<async_task<void> >& ctx) {
      CORO_BEGIN(ctx);
      last_ue_create_request = msg;
      CORO_AWAIT(ue_created_ev);
      CORO_RETURN();
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
  manual_event_flag                       ue_created_ev;
  optional<mac_ue_create_request_message> last_ue_create_request;

  async_task<void> add_ue(const mac_ue_create_request_message& msg) override
  {
    return launch_async([this, msg](coro_context<async_task<void> >& ctx) {
      CORO_BEGIN(ctx);
      last_ue_create_request = msg;
      CORO_AWAIT(ue_created_ev);
      CORO_RETURN();
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

struct mac_test_bench {
  inline_executor               exec;
  std::array<task_executor*, 1> exec_lst;
  mac_config_test_adapter       mac_config_notifier;
  mac_common_config_t           mac_cfg;
  mac_ul_dummy_configurer       mac_ul;
  mac_dl_dummy_configurer       mac_dl;

  mac_test_bench() : exec_lst({&exec}), mac_cfg(mac_config_notifier, exec, exec_lst, exec) {}
};

enum class test_mode { success_manual_response, success_auto_response };
std::string to_string(test_mode m)
{
  switch (m) {
    case test_mode::success_auto_response:
      return "success expected, auto response";
    case test_mode::success_manual_response:
      return "success expected, manual response";
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
  }

  // STAGE 1: Procedure is launched
  async_task<void> proc = launch_async<mac_ue_create_request_procedure>(msg, mac_cfg, mac_ul, mac_dl);

  if (tmode == test_mode::success_manual_response) {
    // In this test mode, we have to manually set when the MAC UL/DL UE create response is complete

    // STAGE 2: MAC UL received the request to create UE, but hasn't signalled the task completion.
    TESTASSERT(not proc.ready());
    TESTASSERT(mac_ul.last_ue_create_request.has_value());
    TESTASSERT(mac_ul.last_ue_create_request->ue_index == msg.ue_index);
    TESTASSERT(not mac_dl.last_ue_create_request.has_value());
    TESTASSERT(not mac_config_notifier.last_ue_created.has_value());

    // STAGE 3: MAC UL signals the UE creation task has finished. MAC DL UE addition starts
    mac_ul.ue_created_ev.set();
    TESTASSERT(not proc.ready());
    TESTASSERT(mac_dl.last_ue_create_request.has_value());
    TESTASSERT(mac_dl.last_ue_create_request->ue_index == msg.ue_index);
    TESTASSERT(not mac_config_notifier.last_ue_created.has_value());

    // STAGE 4: MAC DL signals the UE creation task has finished.
    mac_dl.ue_created_ev.set();
  } else {
    // In this test mode, the MAC UL/DL UE create response is set automatically. The procedure should finish during
    // the launch call.

    TESTASSERT(mac_ul.last_ue_create_request.has_value());
    TESTASSERT(mac_ul.last_ue_create_request->ue_index == msg.ue_index);
    TESTASSERT(mac_dl.last_ue_create_request.has_value());
    TESTASSERT(mac_dl.last_ue_create_request->ue_index == msg.ue_index);
  }

  // STAGE 5: Procedure ends and UE creation request complete is sent back through config notifier
  TESTASSERT(proc.ready());
  TESTASSERT(mac_config_notifier.last_ue_created.has_value());
  TESTASSERT_EQ(1, mac_config_notifier.last_ue_created->ue_index);
  TESTASSERT(mac_config_notifier.last_ue_created->result);
}

int main()
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::info);

  srslog::init();

  test_ue_create_procedure_single_thread(test_mode::success_manual_response);
  test_ue_create_procedure_single_thread(test_mode::success_auto_response);
}
