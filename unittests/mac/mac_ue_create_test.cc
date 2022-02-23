
#include "../../lib/mac/ue_creation_procedure.h"
#include "srsgnb/support/async/manual_event.h"
#include "srsgnb/support/task_worker.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

class sched_cfg_test_adapter : public sched_cfg_notifier
{
public:
  bool ue_config_complete_called = false;
  bool ue_delete_complete_called = false;

  task_executor&          ctrl_exec;
  unique_function<void()> callback = []() {};

  sched_cfg_test_adapter(task_executor& ctrl_exec_) : ctrl_exec(ctrl_exec_) {}

  void on_ue_config_complete(rnti_t) override
  {
    ctrl_exec.execute([this]() {
      ue_config_complete_called = true;
      callback();
    });
  }
  void on_ue_delete_response(rnti_t rnti) override { ue_delete_complete_called = true; }
};

class mac_ul_sdu_test_adapter : public mac_ul_sdu_notifier
{
public:
  void on_ul_sdu(mac_ul_sdu sdu) override {}
};

class mac_config_test_adapter : public mac_config_notifier
{
public:
  void on_ue_create_request_complete(const mac_ue_create_request_response_message& resp) override {}
  void on_ue_reconfiguration_complete() override {}
  void on_ue_delete_complete(const mac_ue_delete_response_message& resp) override {}
};

class sequential_executor : public task_executor
{
public:
  void execute(unique_task task) final { task(); }
};

struct mac_test_bench {
  sched_cfg_test_adapter      sched_cfg_notif;
  mac_dl_worker               dl_worker;
  mac_ul_sdu_test_adapter     ul_notif;
  mac_ul_worker               ul_worker;
  mac_config_test_adapter     mac_notif;
  std::vector<task_executor*> dl_execs;
  mac_context                 mac_ctx;
  manual_event_flag           sched_response_ev;

  mac_test_bench(task_executor& ctrl_exec, std::initializer_list<task_executor*> dl_execs_, task_executor& ul_exec) :
    sched_cfg_notif(ctrl_exec),
    dl_worker(sched_cfg_notif),
    ul_worker(ul_notif),
    dl_execs(dl_execs_.begin(), dl_execs_.end()),
    mac_ctx(mac_notif, ul_exec, dl_execs, ctrl_exec, dl_worker, ul_worker)
  {}
};

void test_ue_create_procedure_single_thread()
{
  test_delimit_logger test_delim{"Single threaded UE creation procedure"};

  // Run tasks in same thread
  sequential_executor exec;

  // Create a MAC context object
  mac_test_bench bench{exec, {&exec}, exec};

  // Launch procedure
  mac_ue_create_request_message msg{};
  msg.ue_index   = 0;
  msg.crnti      = 0x4601;
  msg.cell_index = 0;
  msg.bearers.emplace_back();
  msg.bearers[0].lcid      = 0;
  msg.bearers[0].dl_bearer = nullptr; // procedure should not use bearers
  msg.bearers[0].ul_bearer = nullptr; // procedure should not use bearers
  async_task<void> proc    = launch_async<mac_ue_create_request_procedure>(bench.mac_ctx, bench.sched_response_ev, msg);
  TESTASSERT(not proc.ready() and not proc.empty());

  // Given that there is only one single execution context, the creation of the UE in the MAC DL and UL should be
  // completed at this point, and the scheduler config notifier should have been triggered.
  TESTASSERT(bench.ul_worker.contains_ue(msg.ue_index));
  TESTASSERT(bench.dl_worker.contains_ue(msg.ue_index));
  TESTASSERT(bench.sched_cfg_notif.ue_config_complete_called);
  TESTASSERT(not bench.sched_cfg_notif.ue_delete_complete_called);

  // Manually trigger the scheduler config response completes the procedure
  bench.sched_response_ev.set();
  TESTASSERT(proc.ready());
}

void test_ue_create_procedure_multi_thread()
{
  test_delimit_logger test_delim{"Multi-threaded UE creation procedure"};

  // Run tasks in same thread
  task_worker                    ctrl_worker{"CTRLWorker", 16};
  task_worker                    ul_worker{"ULWorker", 16};
  task_worker                    dl_worker{"DLWorker", 16};
  std::unique_ptr<task_executor> ctrl_exec = make_task_executor(ctrl_worker);
  std::unique_ptr<task_executor> ul_exec   = make_task_executor(ul_worker);
  std::unique_ptr<task_executor> dl_exec   = make_task_executor(dl_worker);

  // Create a MAC context object
  mac_test_bench bench{*ctrl_exec, {dl_exec.get()}, *ul_exec};
  bench.sched_cfg_notif.callback = [&bench]() {
    // Automatically trigger scheduler config response event when MAC DL finishes creating the UE
    bench.sched_response_ev.set();
  };

  // launch procedure
  async_task<void> proc;
  ctrl_exec->execute([&bench, &proc]() {
    // Launch procedure from CTRL execution context
    mac_ue_create_request_message msg{};
    msg.ue_index   = 0;
    msg.crnti      = 0x4601;
    msg.cell_index = 0;
    msg.bearers.emplace_back();
    msg.bearers[0].lcid      = 0;
    msg.bearers[0].dl_bearer = nullptr; // procedure should not use bearers
    msg.bearers[0].ul_bearer = nullptr; // procedure should not use bearers

    proc = launch_async<mac_ue_create_request_procedure>(bench.mac_ctx, bench.sched_response_ev, msg);
    TESTASSERT(not proc.ready() and not proc.empty());
  });

  // keep polling for procedure readiness (from inside the CTRL thread)
  std::atomic<int> finished{false}, task_complete{false};
  for (int count = 0; not finished and count < 10000; ++count) {
    task_complete = false;
    ctrl_exec->execute([&proc, &finished, &task_complete]() {
      if (proc.ready()) {
        finished = true;
      }
      task_complete = true;
    });
    while (not task_complete) {
      usleep(100);
    }
  }
  TESTASSERT(finished);

//  ul_worker.stop();
}

int main()
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::info);

  srslog::init();

  test_ue_create_procedure_single_thread();
  test_ue_create_procedure_multi_thread();
}
