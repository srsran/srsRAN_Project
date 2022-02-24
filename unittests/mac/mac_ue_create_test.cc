
#include "../../lib/mac/ue_creation_procedure.h"
#include "srsgnb/support/async/manual_event.h"
#include "srsgnb/support/task_worker.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

class mac_ul_sdu_test_adapter : public mac_ul_sdu_notifier
{
public:
  void on_ul_sdu(mac_ul_sdu sdu) override {}
};

class mac_config_test_adapter : public mac_config_notifier
{
public:
  std::mutex                                       mutex;
  std::condition_variable                          cvar;
  optional<mac_ue_create_request_response_message> last_ue_created{};

  void on_ue_create_request_complete(const mac_ue_create_request_response_message& resp) override
  {
    std::unique_lock<std::mutex> lock(mutex);
    last_ue_created = resp;
    cvar.notify_one();
  }
  void on_ue_reconfiguration_complete() override {}
  void on_ue_delete_complete(const mac_ue_delete_response_message& resp) override {}
};

class sequential_executor : public task_executor
{
public:
  void execute(unique_task task) final { task(); }
};

struct mac_test_bench {
  mac_config_test_adapter mac_notif;
  mac_common_config_t     mac_cfg;
  mac_dl                  dl_mac;
  mac_ul_sdu_test_adapter ul_notif;
  mac_ul                  ul_mac;

  mac_test_bench(task_executor& ctrl_exec, span<task_executor*> dl_execs_, task_executor& ul_exec) :
    mac_cfg(mac_notif, ul_exec, dl_execs_, ctrl_exec), dl_mac(mac_cfg), ul_mac(mac_cfg, ul_notif)
  {}
};

void test_ue_create_procedure_single_thread()
{
  test_delimit_logger test_delim{"Single threaded UE creation procedure"};

  // Run tasks in same thread
  sequential_executor exec;

  // Create a MAC context object
  std::vector<task_executor*> dl_execs = {&exec};
  mac_test_bench              bench{exec, dl_execs, exec};

  // Launch procedure
  mac_ue_create_request_message msg{};
  msg.ue_index   = 1;
  msg.crnti      = 0x4601;
  msg.cell_index = 0;
  msg.bearers.emplace_back();
  msg.bearers[0].lcid      = 0;
  msg.bearers[0].dl_bearer = nullptr; // procedure should not use bearers
  msg.bearers[0].ul_bearer = nullptr; // procedure should not use bearers
  async_task<void> proc = launch_async<mac_ue_create_request_procedure>(msg, bench.mac_cfg, bench.ul_mac, bench.dl_mac);

  // Given that there is only one single execution context, the creation of the UE in the MAC DL and UL should be
  // completed at this point, and the scheduler config notifier should have been triggered.
  TESTASSERT(proc.ready());
  TESTASSERT_EQ(1, bench.mac_notif.last_ue_created->ue_index);
  TESTASSERT(bench.mac_notif.last_ue_created->result);
}

void test_ue_create_procedure_multi_thread()
{
  test_delimit_logger test_delim{"Multi-threaded UE creation procedure"};

  // Run tasks in different threads
  task_worker                    ctrl_worker{"CTRLWorker", 16};
  task_worker                    ul_worker{"ULWorker", 16};
  task_worker                    dl_worker{"DLWorker", 16};
  std::unique_ptr<task_executor> ctrl_exec = make_task_executor(ctrl_worker);
  std::unique_ptr<task_executor> ul_exec   = make_task_executor(ul_worker);
  std::unique_ptr<task_executor> dl_exec   = make_task_executor(dl_worker);

  // Create a MAC context object
  std::vector<task_executor*> dl_execs = {dl_exec.get()};
  mac_test_bench              bench{*ctrl_exec, dl_execs, *ul_exec};

  // launch procedure
  async_task<void> proc;
  ctrl_exec->execute([&bench, &proc]() {
    // Launch procedure from CTRL execution context
    mac_ue_create_request_message msg{};
    msg.ue_index   = 3;
    msg.crnti      = 0x4601;
    msg.cell_index = 0;
    msg.bearers.emplace_back();
    msg.bearers[0].lcid      = 0;
    msg.bearers[0].dl_bearer = nullptr; // procedure should not use bearers
    msg.bearers[0].ul_bearer = nullptr; // procedure should not use bearers

    proc = launch_async<mac_ue_create_request_procedure>(msg, bench.mac_cfg, bench.ul_mac, bench.dl_mac);
  });

  {
    std::unique_lock<std::mutex> lock(bench.mac_notif.mutex);
    while (not bench.mac_notif.last_ue_created.has_value()) {
      bench.mac_notif.cvar.wait_for(lock, std::chrono::milliseconds{100});
    }
  }
  TESTASSERT(bench.mac_notif.last_ue_created.has_value());
  TESTASSERT_EQ(3, bench.mac_notif.last_ue_created->ue_index);
  TESTASSERT(bench.mac_notif.last_ue_created->result);
  ctrl_worker.stop();
}

int main()
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::info);

  srslog::init();

  test_ue_create_procedure_single_thread();
  test_ue_create_procedure_multi_thread();
}
