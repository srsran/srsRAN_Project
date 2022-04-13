
#include "../../lib/mac/mac_dl/mac_dl_processor.h"
#include "mac_ctrl_test_dummies.h"
#include "srsgnb/support/executors/blocking_task_worker.h"
#include "srsgnb/support/executors/manual_task_worker.h"
#include "srsgnb/support/executors/task_worker.h"
#include "srsgnb/support/test_utils.h"
#include "thread"

using namespace srsgnb;

class dummy_sched : public sched_interface
{
public:
  srslog::basic_logger&         logger = srslog::fetch_basic_logger("TEST");
  sched_configuration_notifier& notifier;

  dummy_sched(sched_configuration_notifier& notifier_) : notifier(notifier_) {}

  bool handle_cell_configuration_request(const cell_configuration_request_message& msg) override { return true; }
  void handle_rach_indication(const rach_indication_message& msg) override {}
  void handle_add_ue_request(const sched_ue_creation_request_message& ue_request) override
  {
    logger.info("SCHED: ueId={} Creation", ue_request.ue_index);
    notifier.on_ue_config_complete(ue_request.ue_index);
  }
  void handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request) override
  {
    logger.info("SCHED: ueId={} Reconfiguration", ue_request.ue_index);
    notifier.on_ue_config_complete(ue_request.ue_index);
  }
  void handle_ue_delete_request(du_ue_index_t ue_index) override
  {
    logger.info("SCHED: ueId={} Deletion", ue_index);
    notifier.on_ue_delete_response(ue_index);
  }

  const dl_sched_result* get_dl_sched(slot_point sl_tx, du_cell_index_t cell_index) override { return nullptr; }
  const ul_sched_result* get_ul_sched(slot_point sl_tx, du_cell_index_t cell_index) override { return nullptr; }
  void                   ul_sr_info(const sr_indication_message& sr) override {}
  void                   ul_bsr(const ul_bsr_indication_message& bsr) override {}
};

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
    create_msg.ue_index   = to_du_ue_index(0);
    create_msg.crnti      = to_rnti(0x4601);
    create_msg.cell_index = to_du_cell_index(0);
    create_msg.bearers.emplace_back();
    create_msg.bearers[0].lcid = LCID_SRB1;

    reconf_msg.ue_index   = to_du_ue_index(0);
    reconf_msg.crnti      = to_rnti(0x4601);
    reconf_msg.cell_index = to_du_cell_index(0);
    reconf_msg.bearers_to_addmod.emplace_back();
    reconf_msg.bearers_to_addmod[0].lcid = LCID_SRB2;
    reconf_msg.bearers_to_rem.emplace_back(LCID_SRB1);

    delete_msg.ue_index   = to_du_ue_index(0);
    delete_msg.cell_index = to_du_cell_index(0);
    delete_msg.rnti       = to_rnti(0x4601);
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

/// In this test, we verify the correct executors are called during creation, reconfiguration and deletion of a UE.
void test_dl_ue_procedure_execution_contexts()
{
  test_delimit_logger delimiter{"Test UE procedures execution contexts"};

  auto&                       logger = srslog::fetch_basic_logger("TEST");
  manual_task_worker          ctrl_worker{128};
  manual_task_worker          dl_worker{128};
  std::vector<task_executor*> dl_execs = {&dl_worker};
  dummy_ul_executor_mapper    ul_exec_mapper{ctrl_worker};
  dummy_dl_executor_mapper    dl_exec_mapper{dl_execs[0]};
  dummy_mac_event_indicator   du_mng_notifier;
  dummy_mac_result_notifier   phy_notifier;
  mac_common_config_t         cfg{du_mng_notifier, ul_exec_mapper, dl_exec_mapper, ctrl_worker, phy_notifier};
  du_rnti_table               rnti_table;

  srs_sched_config_adapter sched_cfg_adapter{cfg};
  dummy_sched              sched_obj{sched_cfg_adapter.get_sched_notifier()};
  mac_dl_processor         mac_dl(cfg, sched_cfg_adapter, sched_obj, rnti_table);
  sched_cfg_adapter.set_sched(sched_obj);

  // Action: Add Cell.
  mac_cell_configuration mac_cell_cfg{};
  // Set this to a valid ARFCN value (band 3, in this case, but it doesn't matter) - Required for SSB.
  mac_cell_cfg.dl_carrier.arfcn = 365000;
  mac_dl.add_cell(mac_cell_cfg);

  // TEST: Thread used for resumption does not change.
  auto test_event = [&ctrl_worker](test_task_event ev) {
    TESTASSERT(ctrl_worker.get_thread_id() == std::this_thread::get_id(), "Procedure must finish in CTRL thread.");
    if (ev == test_task_event::ue_deleted) {
      ctrl_worker.request_stop();
    }
  };
  eager_async_task<void> t = launch_async<add_reconf_delete_ue_test_task>(mac_dl, test_event);

  while (not ctrl_worker.is_stopped()) {
    if (ctrl_worker.has_pending_tasks()) {
      logger.info("Running next task in CTRL worker");
      ctrl_worker.try_run_next();
    }
    if (dl_worker.has_pending_tasks()) {
      logger.info("Running next task in DL worker");
      dl_worker.try_run_next();
    }
  }
  TESTASSERT(not t.empty() and t.ready());
}

void test_dl_ue_procedure_tsan()
{
  test_delimit_logger delimiter{"Test UE procedures TSAN"};

  blocking_task_worker      ctrl_worker{128};
  task_worker               dl_workers[] = {{"DL#1", 128}, {"DL#2", 128}};
  task_worker_executor      dl_execs[]   = {{dl_workers[0]}, {dl_workers[1]}};
  dummy_ul_executor_mapper  ul_exec_mapper{ctrl_worker};
  dummy_dl_executor_mapper  dl_exec_mapper{&dl_execs[0], &dl_execs[1]};
  dummy_mac_event_indicator du_mng_notifier;
  dummy_mac_result_notifier phy_notifier;
  mac_common_config_t       cfg{du_mng_notifier, ul_exec_mapper, dl_exec_mapper, ctrl_worker, phy_notifier};
  du_rnti_table             rnti_table;

  srs_sched_config_adapter sched_cfg_adapter{cfg};
  dummy_sched              sched_obj{sched_cfg_adapter.get_sched_notifier()};
  mac_dl_processor         mac_dl(cfg, sched_cfg_adapter, sched_obj, rnti_table);
  sched_cfg_adapter.set_sched(sched_obj);

  // Action: Add Cells.
  mac_cell_configuration cell_cfg1{};
  // Set this to a valid ARFCN value (band 3, in this case, but it doesn't matter) - Required for SSB.
  cell_cfg1.dl_carrier.arfcn = 365000;
  cell_cfg1.cell_index       = to_du_cell_index(0);
  mac_dl.add_cell(cell_cfg1);
  mac_cell_configuration cell_cfg2{};
  cell_cfg2.dl_carrier.arfcn = 365000;
  cell_cfg2.cell_index       = to_du_cell_index(1);
  mac_dl.add_cell(cell_cfg2);

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

int main()
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);

  srslog::init();

  test_dl_ue_procedure_execution_contexts();
  test_dl_ue_procedure_tsan();
}
