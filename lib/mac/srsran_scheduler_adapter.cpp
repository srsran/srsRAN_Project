/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran_scheduler_adapter.h"
#include "mac_ul/ul_bsr.h"

using namespace srsran;

void srsran_scheduler_adapter::add_cell(const mac_cell_creation_request& msg)
{
  srs_sched->handle_cell_configuration_request(msg.sched_req);
}

async_task<bool> srsran_scheduler_adapter::handle_ue_creation_request(const mac_ue_create_request& msg)
{
  return launch_async([this, msg](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);

    // Create UE in the Scheduler.
    srs_sched->handle_ue_creation_request(make_scheduler_ue_creation_request(msg));

    // Await Scheduler notification that UE was added.
    CORO_AWAIT(sched_cfg_notif_map[msg.ue_index].ue_config_ready);
    sched_cfg_notif_map[msg.ue_index].ue_config_ready.reset();

    CORO_RETURN(true);
  });
}

async_task<bool> srsran_scheduler_adapter::handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request& msg)
{
  return launch_async([this, msg](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);

    // Reconfigure UE in the scheduler.
    srs_sched->handle_ue_reconfiguration_request(make_scheduler_ue_reconfiguration_request(msg));

    // Await Scheduler notification.
    CORO_AWAIT(sched_cfg_notif_map[msg.ue_index].ue_config_ready);
    sched_cfg_notif_map[msg.ue_index].ue_config_ready.reset();

    CORO_RETURN(true);
  });
}

async_task<bool> srsran_scheduler_adapter::handle_ue_removal_request(const mac_ue_delete_request& msg)
{
  return launch_async([this, msg](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);

    // Remove UE from the scheduler.
    srs_sched->handle_ue_removal_request(msg.ue_index);

    // Await Scheduler notification.
    CORO_AWAIT(sched_cfg_notif_map[msg.ue_index].ue_config_ready);
    sched_cfg_notif_map[msg.ue_index].ue_config_ready.reset();
    CORO_RETURN(true);
  });
}

/// Converts BSR MAC CE (See 38.321, 6.1.3.1) to Scheduler UL BSR event type.
static ul_bsr_lcg_report make_sched_lcg_report(const lcg_bsr_report& ul_mac_bsr, bsr_format bsr_format)
{
  return ul_bsr_lcg_report{ul_mac_bsr.lcg_id, buff_size_field_to_bytes(ul_mac_bsr.buffer_size, bsr_format)};
}

void srsran_scheduler_adapter::handle_ul_bsr_indication(const mac_bsr_ce_info& bsr)
{
  // Prepare UL BSR indication for Scheduler
  ul_bsr_indication_message ul_bsr_ind{};
  ul_bsr_ind.cell_index = bsr.cell_index;
  ul_bsr_ind.ue_index   = bsr.ue_index;
  ul_bsr_ind.crnti      = bsr.rnti;
  ul_bsr_ind.type       = bsr.bsr_fmt;
  if (bsr.bsr_fmt == bsr_format::SHORT_BSR or bsr.bsr_fmt == bsr_format::SHORT_TRUNC_BSR) {
    const ul_bsr_lcg_report sched_bsr = make_sched_lcg_report(bsr.lcg_reports[0], bsr.bsr_fmt);

    if (sched_bsr.nof_bytes == 0) {
      // In case SBSR=0, assume all LCGs are 0.
      for (unsigned j = 0; j <= MAX_LCG_ID; ++j) {
        ul_bsr_ind.reported_lcgs.push_back(ul_bsr_lcg_report{uint_to_lcg_id(j), 0U});
      }
    } else {
      ul_bsr_ind.reported_lcgs.push_back(sched_bsr);
    }
  } else {
    // Long BSR or Long truncated BSR.
    for (const lcg_bsr_report& lb : bsr.lcg_reports) {
      ul_bsr_ind.reported_lcgs.push_back(make_sched_lcg_report(lb, bsr.bsr_fmt));
    }
  }

  // Send UL BSR indication to Scheduler.
  srs_sched->handle_ul_bsr_indication(ul_bsr_ind);
}

void srsran_scheduler_adapter::handle_ul_sched_command(const mac_ul_scheduling_command& cmd)
{
  // Convert UL scheduling command to Scheduler UCI indication with a SR included.
  uci_indication uci{};
  uci.cell_index = cmd.cell_index;
  uci.slot_rx    = cmd.sl_rx;
  uci.ucis.resize(1);
  uci.ucis[0].ue_index = cmd.ue_index;
  uci.ucis[0].crnti    = cmd.rnti;
  uci.ucis[0].pdu      = uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu{.sr_detected = true};

  // Send SR indication to Scheduler.
  srs_sched->handle_uci_indication(uci);
}

void srsran_scheduler_adapter::sched_config_notif_adapter::on_ue_config_complete(du_ue_index_t ue_index)
{
  srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid ue index={}", ue_index);

  // Remove continuation of task in ctrl executor.
  if (not parent.cfg.ctrl_exec.defer(
          [this, ue_index]() { parent.sched_cfg_notif_map[ue_index].ue_config_ready.set(true); })) {
    parent.cfg.logger.error("Unable to finish configuration of ue={}. Cause: DU task queue is full.", ue_index);
  }
}

void srsran_scheduler_adapter::sched_config_notif_adapter::on_ue_delete_response(du_ue_index_t ue_index)
{
  srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid ue index={}", ue_index);

  // Continuation of ue remove task dispatched to the ctrl executor.
  if (not parent.cfg.ctrl_exec.defer(
          [this, ue_index]() { parent.sched_cfg_notif_map[ue_index].ue_config_ready.set(true); })) {
    parent.cfg.logger.error("Unable to remove ue={}. Cause: DU task queue is full.", ue_index);
  }
}
