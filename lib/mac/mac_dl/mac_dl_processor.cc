
#include "mac_dl_processor.h"
#include "dci_encoder.h"
#include "pdu_encoder.h"
#include "srsgnb/mac/mac_dl_result.h"
#include "ssb_encoder.h"

using namespace srsgnb;

void mac_dl_processor::add_cell(const mac_cell_configuration& cell_cfg)
{
  // Add cell in cell manager.
  cells.add_cell(cell_cfg);

  // TODO: Pass configuration to scheduler.
}

void mac_dl_processor::remove_cell(du_cell_index_t cell_index)
{
  // TODO: remove cell from scheduler.

  // Remove cell from cell manager.
  cells.remove_cell(cell_index);
}

async_task<void> mac_dl_processor::start_cell(du_cell_index_t cell_index)
{
  return dispatch_and_resume_on(*cfg.dl_execs[cell_index % cfg.dl_execs.size()], cfg.ctrl_exec, [this, cell_index]() {
    cells.start_cell(cell_index);
  });
}

async_task<void> mac_dl_processor::stop_cell(du_cell_index_t cell_index)
{
  return dispatch_and_resume_on(*cfg.dl_execs[cell_index % cfg.dl_execs.size()], cfg.ctrl_exec, [this, cell_index]() {
    cells.stop_cell(cell_index);
  });
}

void mac_dl_processor::slot_indication(slot_point sl_tx, du_cell_index_t cell_index)
{
  if (not cells.is_cell_active(cell_index)) {
    // TODO: Send empty DL result to PHY.
    return;
  }

  // Generate DL scheduling result for provided slot and cell.
  const dl_sched_result* dl_res = sched_obj.get_dl_sched(sl_tx, cell_index);
  if (dl_res == nullptr) {
    logger.warning("Unable to compute scheduling result for slot={}, cell={}", sl_tx, cell_index);
    return;
  }

  // Assemble MAC DL scheduling request that is going to be passed to the PHY.
  mac_dl_sched_result mac_dl_res;
  assemble_dl_sched_request(sl_tx, cell_index, *dl_res, mac_dl_res);

  // Send DL sched result to PHY.
  // TODO: MAC DL requires FAPI handle to send the result.

  // Start assembling Slot Data Result.
  mac_dl_data_result data_res;
  assemble_dl_data_request(sl_tx, cell_index, *dl_res, data_res);

  // Send DL Data to PHY.
  // TODO: MAC DL requires FAPI handle to send the DL Data.
}

void mac_dl_processor::assemble_dl_sched_request(slot_point             sl_tx,
                                                 du_cell_index_t        cell_index,
                                                 const dl_sched_result& dl_res,
                                                 mac_dl_sched_result&   mac_res)
{
  const mac_cell_configuration& cell_cfg = cells.get_config(cell_index);

  // Pass scheduler output directly to PHY.
  mac_res.dl_res = &dl_res;

  // Encode SSB if present.
  if (dl_res.bc.ssb_info.has_value()) {
    const ssb_information& ssb = dl_res.bc.ssb_info.value();
    mac_res.ssb_pdu.emplace();
    encode_ssb(cell_cfg, ssb, *mac_res.ssb_pdu);
  }

  // Encode PDCCH DCI payloads.
  for (const auto& sib : dl_res.bc.sibs) {
    mac_res.pdcch_pdus.emplace_back();
    encode_dci(cell_cfg, sib, mac_res.pdcch_pdus.back().dci_payload);
  }
  for (const rar_information& rar : dl_res.rar_grants) {
    mac_res.pdcch_pdus.emplace_back();
    encode_dci(cell_cfg, rar, mac_res.pdcch_pdus.back().dci_payload);
  }
  for (const dl_msg_alloc& data : dl_res.ue_grants) {
    mac_res.pdcch_pdus.emplace_back();
    encode_dci(cell_cfg, data, mac_res.pdcch_pdus.back().dci_payload);
  }
}

void mac_dl_processor::assemble_dl_data_request(slot_point             sl_tx,
                                                du_cell_index_t        cell_index,
                                                const dl_sched_result& dl_res,
                                                mac_dl_data_result&    data_res)
{
  const mac_cell_configuration& cell_cfg = cells.get_config(cell_index);

  data_res.slot = sl_tx;

  // Assemble scheduled SIBs' payload.
  for (const sib_information& sib : dl_res.bc.sibs) {
    data_res.pdus.emplace_back();
    encode_sib_pdu(cell_cfg, sib, data_res.pdus.back());
  }

  // Assemble scheduled RARs' payload.
  for (const rar_information& rar : dl_res.rar_grants) {
    data_res.pdus.emplace_back();
    // call MAC encoder.
    encode_rar_pdu(cell_cfg, rar, data_res.pdus.back());
  }

  // Assemble data grants.
  for (const dl_msg_alloc& grant : dl_res.ue_grants) {
    for (const dl_msg_tb_info& tb_info : grant.tbs) {
      for (const dl_msg_lc_info& bearer_alloc : tb_info.lc_lst) {
        // Fetch RLC Bearer.
        mac_sdu_tx_builder* bearer = mux.get_bearer(grant.crnti, bearer_alloc.lcid);
        srsran_sanity_check(bearer != nullptr, "Scheduler is allocating inexistent bearers");

        // Assemble MAC SDU.
        data_res.pdus.emplace_back();
        byte_buffer& sdu = data_res.pdus.back();
        sdu.resize(bearer_alloc.sched_bytes);
        bearer->on_new_tx_sdu(sdu);
      }
    }
  }
}

async_task<bool> mac_dl_processor::add_ue(const mac_ue_create_request_message& request)
{
  return launch_async([this, request](coro_context<async_task<bool> >& ctx) {
    CORO_BEGIN(ctx);

    // 1. Change to respective DL executor
    CORO_AWAIT(execute_on(*cfg.dl_execs[request.cell_index]));

    // 2. Insert UE and DL bearers
    mux.add_ue(request.crnti, request.ue_index, request.bearers);

    // 3. Create UE in scheduler
    log_proc_started(logger, request.ue_index, request.crnti, "Sched UE Config");
    sched_obj.config_ue(request.crnti);

    // 4. Await scheduler to respond via notifier
    CORO_AWAIT(sched_cfg_notif_map[request.crnti % sched_cfg_notif_map.size()]);

    log_proc_completed(logger, request.ue_index, request.crnti, "Sched UE Config");

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
    CORO_AWAIT(sched_cfg_notif_map[request.rnti % sched_cfg_notif_map.size()]);

    // 4. Remove UE associated DL channels
    mux.remove_ue(request.rnti);

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
    mux.remove_bearers(request.crnti, request.bearers_to_rem);

    // 3. AddMod UE DL bearers
    mux.addmod_bearers(request.crnti, request.bearers_to_addmod);

    // 4. Configure UE in Scheduler
    log_proc_started(logger, request.ue_index, request.crnti, "Sched UE Config");
    sched_obj.config_ue(request.crnti);

    // 4. Await scheduler to respond via notifier
    CORO_AWAIT(sched_cfg_notif_map[request.crnti % sched_cfg_notif_map.size()]);

    log_proc_completed(logger, request.ue_index, request.crnti, "Sched UE Config");

    // 5. Change back to CTRL executor before returning
    CORO_AWAIT(execute_on(cfg.ctrl_exec));

    CORO_RETURN(true);
  });
}
