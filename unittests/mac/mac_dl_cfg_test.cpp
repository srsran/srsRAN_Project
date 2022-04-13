
#include "../../lib/mac/mac_dl/mac_dl_processor.h"
#include "mac_ctrl_test_dummies.h"
#include "srsgnb/support/executors/blocking_task_worker.h"
#include "srsgnb/support/executors/manual_task_worker.h"
#include "srsgnb/support/executors/task_worker.h"
#include "srsgnb/support/test_utils.h"
#include "thread"

using namespace srsgnb;

/// Enum used to track the progress of the test task
enum class test_task_event { ue_created, ue_reconfigured, ue_deleted };

/// Test coroutine that calls UE create, reconfiguration, delete in sequence.
struct add_reconf_delete_ue_test_task {
  srslog::basic_logger&                  logger = srslog::fetch_basic_logger("TEST");
  std::thread::id                        tid;
  mac_dl_processor&                      mac_dl;
  unique_function<void(test_task_event)> event_test;
  mac_ue_create_request_message          create_msg{};
  mac_ue_reconfiguration_request_message reconf_msg{};
  mac_ue_delete_request_message          delete_msg{};

  add_reconf_delete_ue_test_task(mac_dl_processor& mac_dl_, unique_function<void(test_task_event)> event_callback_) :
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

  void operator()(coro_context<eager_async_task<void> >& ctx)
  {
    CORO_BEGIN(ctx);
    // UE creation
    tid = std::this_thread::get_id();
    logger.info("Step 1: Creating UE");
    CORO_AWAIT(mac_dl.add_ue(create_msg));
    logger.info("UE created");
    event_test(test_task_event::ue_created);
    // UE reconfiguration
    logger.info("Step 2: Reconfiguring UE");
    CORO_AWAIT(mac_dl.reconfigure_ue(reconf_msg));
    logger.info("UE reconfigured");
    event_test(test_task_event::ue_reconfigured);
    // UE deletion
    logger.info("Step 3: Deleting UE");
    CORO_AWAIT(mac_dl.remove_ue(delete_msg));
    logger.info("UE deleted");
    event_test(test_task_event::ue_deleted);
    TESTASSERT(std::this_thread::get_id() == tid);
    CORO_RETURN();
  }
};

void test_dl_ue_procedure_tsan()
{
  test_delimit_logger delimiter{"Test UE procedures TSAN"};

  blocking_task_worker        ctrl_worker{128};
  task_worker                 dl_worker1{"DL", 128}, dl_worker2{"DL", 128};
  task_worker_executor        dl_exec1{dl_worker1}, dl_exec2{dl_worker2};
  std::vector<task_executor*> dl_execs;
  dl_execs.push_back(&dl_exec1);
  dl_execs.push_back(&dl_exec2);
  dummy_ul_executor_mapper  ul_exec_mapper{ctrl_worker};
  dummy_mac_event_indicator du_mng_notifier;
  dummy_mac_result_notifier phy_notifier;
  mac_common_config_t       cfg{du_mng_notifier, ul_exec_mapper, dl_execs, ctrl_worker, phy_notifier};
  du_rnti_table             rnti_table;

  sched_config_adapter sched_cfg_adapter;
  sched                sched_obj{sched_cfg_adapter.get_notifier()};
  mac_dl_processor     mac_dl(cfg, sched_cfg_adapter, sched_obj, rnti_table);

  // TEST: Thread used for resumption does not change
  std::thread::id tid        = std::this_thread::get_id();
  auto            test_event = [&ctrl_worker, &tid](test_task_event ev) {
    TESTASSERT(tid == std::this_thread::get_id()); // resumes back in CTRL thread
    if (ev == test_task_event::ue_deleted) {
      ctrl_worker.request_stop();
    }
  };
  eager_async_task<void> t = launch_async<add_reconf_delete_ue_test_task>(mac_dl, test_event);

  ctrl_worker.run();
  TESTASSERT(not t.empty() and t.ready());
}

/// In this test, we verify the correct executors are called during creation, reconfiguration and deletion of a UE.
void test_dl_ue_procedure_execution_contexts()
{
  test_delimit_logger delimiter{"Test UE procedures execution contexts"};

  auto&                       logger = srslog::fetch_basic_logger("TEST");
  manual_task_worker          ctrl_worker{128};
  manual_task_worker          dl_worker{128};
  std::vector<task_executor*> dl_execs = {&dl_worker};
  dummy_ul_executor_mapper    ul_exec_mapper{ctrl_worker};
  dummy_mac_event_indicator   du_mng_notifier;
  dummy_mac_result_notifier   phy_notifier;
  mac_common_config_t         cfg{du_mng_notifier, ul_exec_mapper, dl_execs, ctrl_worker, phy_notifier};
  du_rnti_table               rnti_table;

  sched_config_adapter sched_cfg_adapter;
  sched                sched_obj{sched_cfg_adapter.get_notifier()};
  mac_dl_processor     mac_dl(cfg, sched_cfg_adapter, sched_obj, rnti_table);

  // TEST: Thread used for resumption does not change.
  bool is_ctrl_worker = true;
  auto test_event     = [&ctrl_worker, &is_ctrl_worker](test_task_event ev) {
    TESTASSERT(is_ctrl_worker);
    if (ev == test_task_event::ue_deleted) {
      ctrl_worker.request_stop();
    }
  };
  eager_async_task<void> t = launch_async<add_reconf_delete_ue_test_task>(mac_dl, test_event);

  while (not ctrl_worker.is_stopped()) {
    if (ctrl_worker.has_pending_tasks()) {
      logger.info("Running next task in CTRL worker");
      is_ctrl_worker = true;
      ctrl_worker.try_run_next();
    }
    if (dl_worker.has_pending_tasks()) {
      logger.info("Running next task in DL worker");
      is_ctrl_worker = false;
      dl_worker.try_run_next();
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
