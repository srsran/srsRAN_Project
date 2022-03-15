
#include "../../lib/mac/mac_dl/mac_dl_component.h"
#include "mac_ctrl_test_dummies.h"
#include "srsgnb/support/task_worker.h"
#include "srsgnb/support/test_utils.h"
#include "thread"

using namespace srsgnb;

class blocking_worker final : public task_executor
{
public:
  blocking_worker(size_t q_size) : pending_tasks(q_size) {}

  void execute(unique_task task) override { pending_tasks.push_blocking(std::move(task)); }

  void request_stop()
  {
    execute([this]() {
      if (not pending_tasks.is_stopped()) {
        pending_tasks.stop();
      }
    });
  }

  void run()
  {
    while (true) {
      bool        success;
      unique_task t = pending_tasks.pop_blocking(&success);
      if (not success) {
        break;
      }
      t();
    }
  }

private:
  dyn_blocking_queue<unique_task> pending_tasks;
};

class manual_worker final : public task_executor
{
public:
  manual_worker(size_t q_size) : pending_tasks(q_size) {}

  void execute(unique_task task) override { pending_tasks.push_blocking(std::move(task)); }

  bool has_pending_tasks() const { return not pending_tasks.empty(); }

  bool is_stopped() const { return pending_tasks.is_stopped(); }

  void stop()
  {
    if (not is_stopped()) {
      pending_tasks.stop();
    }
  }

  void request_stop()
  {
    execute([this]() { stop(); });
  }

  void run_next()
  {
    bool        success;
    unique_task t = pending_tasks.pop_blocking(&success);
    if (not success) {
      return;
    }
    t();
  }

private:
  dyn_blocking_queue<unique_task> pending_tasks;
};

/// Enum used to track the progress of the test task
enum class test_task_event { ue_created, ue_reconfigured, ue_deleted };

struct add_reconf_delete_ue_test_task {
  srslog::basic_logger&                  logger = srslog::fetch_basic_logger("TEST");
  std::thread::id                        tid;
  mac_dl_component&                      mac_dl;
  unique_function<void(test_task_event)> event_test;
  mac_ue_create_request_message          create_msg{};
  mac_ue_reconfiguration_request_message reconf_msg{};
  mac_ue_delete_request_message          delete_msg{};

  add_reconf_delete_ue_test_task(mac_dl_component& mac_dl_, unique_function<void(test_task_event)> event_callback_) :
    mac_dl(mac_dl_), event_test(std::move(event_callback_))
  {
    create_msg.ue_index   = 0;
    create_msg.crnti      = 0x4601;
    create_msg.cell_index = 0;
    create_msg.bearers.emplace_back();
    create_msg.bearers[0].lcid = 1;

    reconf_msg.ue_index   = 0;
    reconf_msg.crnti      = 0x4601;
    reconf_msg.cell_index = 0;
    reconf_msg.bearers_to_addmod.emplace_back();
    reconf_msg.bearers_to_addmod[0].lcid = 2;
    reconf_msg.bearers_to_rem.emplace_back(1);

    delete_msg.ue_index   = 0;
    delete_msg.cell_index = 0;
    delete_msg.rnti       = 0x4601;
  }

  void operator()(coro_context<async_task<void> >& ctx)
  {
    CORO_BEGIN(ctx);
    // UE creation
    tid = std::this_thread::get_id();
    logger.info("Creating UE");
    CORO_AWAIT(mac_dl.add_ue(create_msg));
    logger.info("UE created");
    event_test(test_task_event::ue_created);
    // UE reconfiguration
    logger.info("Reconfiguring UE");
    CORO_AWAIT(mac_dl.reconfigure_ue(reconf_msg));
    logger.info("UE reconfigured");
    event_test(test_task_event::ue_reconfigured);
    // UE deletion
    logger.info("Deleting UE");
    CORO_AWAIT(mac_dl.remove_ue(delete_msg));
    logger.info("UE deleted");
    event_test(test_task_event::ue_deleted);
    CORO_RETURN();
  }
};

void test_dl_ue_procedure_tsan()
{
  test_delimit_logger delimiter{"Test UE procedures TSAN"};

  blocking_worker             ctrl_worker{128};
  task_worker                 dl_worker1{"DL", 128}, dl_worker2{"DL", 128};
  task_worker_executor        dl_exec1{dl_worker1}, dl_exec2{dl_worker2};
  std::vector<task_executor*> dl_execs;
  dl_execs.push_back(&dl_exec1);
  dl_execs.push_back(&dl_exec2);
  dummy_ul_executor_mapper ul_exec_mapper{ctrl_worker};
  mac_common_config_t      cfg{ul_exec_mapper, dl_execs, ctrl_worker};

  mac_dl_component mac_dl(cfg);

  // TEST: Thread used for resumption does not change
  std::thread::id tid        = std::this_thread::get_id();
  auto            test_event = [&ctrl_worker, &tid](test_task_event ev) {
    TESTASSERT(tid == std::this_thread::get_id()); // resumes back in CTRL thread
    if (ev == test_task_event::ue_deleted) {
      ctrl_worker.request_stop();
    }
  };
  async_task<void> t = launch_async<add_reconf_delete_ue_test_task>(mac_dl, test_event);

  ctrl_worker.run();
  TESTASSERT(not t.empty() and t.ready());
}

void test_dl_ue_procedure_execution_contexts()
{
  test_delimit_logger delimiter{"Test UE procedures execution contexts"};

  auto&                       logger = srslog::fetch_basic_logger("TEST");
  manual_worker               ctrl_worker{128};
  manual_worker               dl_worker{128};
  std::vector<task_executor*> dl_execs;
  dl_execs.push_back(&dl_worker);
  dummy_ul_executor_mapper ul_exec_mapper{ctrl_worker};
  mac_common_config_t      cfg{ul_exec_mapper, dl_execs, ctrl_worker};

  mac_dl_component mac_dl(cfg);

  // TEST: Thread used for resumption does not change
  bool is_ctrl_worker = true;
  auto test_event     = [&ctrl_worker, &is_ctrl_worker](test_task_event ev) {
    TESTASSERT(is_ctrl_worker);
    if (ev == test_task_event::ue_deleted) {
      ctrl_worker.request_stop();
    }
  };
  async_task<void> t = launch_async<add_reconf_delete_ue_test_task>(mac_dl, test_event);

  while (not ctrl_worker.is_stopped()) {
    if (ctrl_worker.has_pending_tasks()) {
      logger.info("Running next task in CTRL worker");
      is_ctrl_worker = true;
      ctrl_worker.run_next();
    }
    if (dl_worker.has_pending_tasks()) {
      logger.info("Running next task in DL worker");
      is_ctrl_worker = false;
      dl_worker.run_next();
    }
  }
  TESTASSERT(not t.empty() and t.ready());
}

int main()
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);

  srslog::init();

  test_dl_ue_procedure_tsan();
  test_dl_ue_procedure_execution_contexts();
}
