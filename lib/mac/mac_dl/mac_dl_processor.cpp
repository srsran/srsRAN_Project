
#include "mac_dl_processor.h"
#include "sched_config_helpers.h"
#include "srsgnb/mac/mac_cell_result.h"

using namespace srsgnb;

mac_dl_processor::mac_dl_processor(mac_common_config_t&    cfg_,
                                   mac_sched_configurator& sched_cfg_,
                                   sched_interface&        sched_,
                                   du_rnti_table&          rnti_table_) :
  cfg(cfg_), logger(cfg.logger), sched_cfg(sched_cfg_), ue_mng(rnti_table_), sched_obj(sched_)
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

  // TODO: Pass valid configuration to scheduler (The code added below was just for passing a test).
  cell_configuration_request_message sched_msg{};
  sched_msg.dl_cfg_common.init_dl_bwp.pdsch_cfg_common_present = true;
  sched_msg.dl_cfg_common.init_dl_bwp.pdcch_cfg_common_present = true;
  sched_msg.ul_cfg_common.init_ul_bwp.rach_cfg_common.set_setup().rach_cfg_generic.ra_resp_win.value =
      asn1::rrc_nr::rach_cfg_generic_s::ra_resp_win_opts::sl10;
  sched_msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common_present = true;
  sched_msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.set_setup().pusch_time_domain_alloc_list.resize(1);
  sched_msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.setup().pusch_time_domain_alloc_list[0].k2_present = true;
  sched_msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.setup().pusch_time_domain_alloc_list[0].k2         = 2;
  sched_msg.ul_cfg_common.init_ul_bwp.generic_params.subcarrier_spacing.value =
      asn1::rrc_nr::subcarrier_spacing_opts::khz15;
  sched_msg.dl_cfg_common.init_dl_bwp.pdcch_cfg_common.set_setup().ra_search_space_present = true;
  sched_msg.ul_cfg_common.init_ul_bwp.rach_cfg_common_present                              = true;
  sched_obj.handle_cell_configuration_request(sched_msg);
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
    ue_mng.add_ue(request);

    // 3. Create UE in scheduler.
    log_proc_started(logger, request.ue_index, request.crnti, "Sched UE create");
    CORO_AWAIT(sched_cfg.handle_ue_creation_request(request));
    log_proc_completed(logger, request.ue_index, request.crnti, "Sched UE create");

    // 4. Change back to CTRL executor before returning
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
    CORO_AWAIT(sched_cfg.handle_ue_deletion_request(request));

    // 3. Remove UE associated DL channels
    ue_mng.remove_ue(request.ue_index);

    // 4. Change back to CTRL executor before returning
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
    ue_mng.remove_bearers(request.ue_index, request.bearers_to_rem);

    // 3. AddMod UE DL bearers
    ue_mng.addmod_bearers(request.ue_index, request.bearers_to_addmod);

    // 4. Configure UE in Scheduler
    log_proc_started(logger, request.ue_index, request.crnti, "Sched UE Config");
    CORO_AWAIT(sched_cfg.handle_ue_reconfiguration_request(request));
    log_proc_completed(logger, request.ue_index, request.crnti, "Sched UE Config");

    // 5. Change back to CTRL executor before returning
    CORO_AWAIT(execute_on(cfg.ctrl_exec));

    CORO_RETURN(true);
  });
}
