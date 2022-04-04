
#include "mac_dl_processor.h"
#include "srsgnb/mac/mac_cell_result.h"

using namespace srsgnb;

mac_dl_processor::mac_dl_processor(mac_common_config_t&  cfg_,
                                   sched_config_adapter& sched_cfg_notif_,
                                   sched_interface&      sched_) :
  cfg(cfg_), logger(cfg.logger), sched_cfg_notif(sched_cfg_notif_), sched_obj(sched_)
{}

bool mac_dl_processor::has_cell(du_cell_index_t cell_index) const
{
  return cell_index < MAX_NOF_CELLS and cells[cell_index] != nullptr;
}

void mac_dl_processor::add_cell(const mac_cell_configuration& cell_cfg)
{
  srsran_assert(not has_cell(cell_cfg.cell_index), "Overwriting existing cell is invalid.");
  // Create one cell.
  cells[cell_cfg.cell_index] = std::make_unique<mac_dl_cell_processor>(cfg, cell_cfg, sched_obj, ue_mng);

  // TODO: Pass configuration to scheduler.
}

void mac_dl_processor::remove_cell(du_cell_index_t cell_index)
{
  // TODO: remove cell from scheduler.

  // Remove cell from cell manager.
  cells[cell_index].reset();
}

async_task<bool> mac_dl_processor::add_ue(const mac_ue_create_request_message& request)
{
  return launch_async([this, request](coro_context<async_task<bool> >& ctx) {
    CORO_BEGIN(ctx);

    // 1. Change to respective DL executor
    CORO_AWAIT(execute_on(*cfg.dl_execs[request.cell_index]));

    // 2. Insert UE and DL bearers
    ue_mng.add_ue(request.crnti, request.ue_index, request.bearers);

    // 3. Create UE in scheduler
    sched_obj.config_ue(request.crnti);

    // 4. Await scheduler to respond via notifier
    CORO_AWAIT(sched_cfg_notif.ue_configuration_completed(request.crnti));

    // 5. Change back to CTRL executor before returning
    CORO_AWAIT(execute_on(cfg.ctrl_exec));

    CORO_RETURN(true);
  });
}

async_task<void> mac_dl_processor::remove_ue(const mac_ue_delete_request_message& request)
{
  return launch_async([this, request](coro_context<async_task<void> >& ctx) {
    CORO_BEGIN(ctx);

    // 1. Change to respective DL executor
    CORO_AWAIT(execute_on(*cfg.dl_execs[request.cell_index]));

    // 2. Remove UE from scheduler
    sched_obj.delete_ue_request(request.rnti);

    // 3. Await scheduler to respond via notifier
    CORO_AWAIT(sched_cfg_notif.ue_deletion_completed(request.rnti));

    // 4. Remove UE associated DL channels
    CORO_AWAIT(execute_on(*cfg.dl_execs[request.cell_index]));
    ue_mng.remove_ue(request.rnti);

    // 5. Change back to CTRL executor before returning
    CORO_AWAIT(execute_on(cfg.ctrl_exec));

    CORO_RETURN();
  });
}

async_task<bool> mac_dl_processor::reconfigure_ue(const mac_ue_reconfiguration_request_message& request)
{
  return launch_async([this, request](coro_context<async_task<bool> >& ctx) {
    CORO_BEGIN(ctx);

    // 1. Change to respective DL executor
    CORO_AWAIT(execute_on(*cfg.dl_execs[request.cell_index]));

    // 2. Remove UE DL bearers
    ue_mng.remove_bearers(request.crnti, request.bearers_to_rem);

    // 3. AddMod UE DL bearers
    ue_mng.addmod_bearers(request.crnti, request.bearers_to_addmod);

    // 4. Configure UE in Scheduler
    log_proc_started(logger, request.ue_index, request.crnti, "Sched UE Config");
    sched_obj.config_ue(request.crnti);

    // 4. Await scheduler to respond via notifier
    CORO_AWAIT(sched_cfg_notif.ue_configuration_completed(request.crnti));

    log_proc_completed(logger, request.ue_index, request.crnti, "Sched UE Config");

    // 5. Change back to CTRL executor before returning
    CORO_AWAIT(execute_on(cfg.ctrl_exec));

    CORO_RETURN(true);
  });
}
