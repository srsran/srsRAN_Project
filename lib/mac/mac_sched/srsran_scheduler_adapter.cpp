/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran_scheduler_adapter.h"
#include "../mac_ul/ul_bsr.h"
#include "srsran/scheduler/scheduler_factory.h"

using namespace srsran;

/// Convert a UE creation request for the MAC into a UE creation request of the scheduler.
static sched_ue_creation_request_message make_scheduler_ue_creation_request(const mac_ue_create_request& request)
{
  sched_ue_creation_request_message ret{};
  ret.ue_index           = request.ue_index;
  ret.crnti              = request.crnti;
  ret.starts_in_fallback = true;
  ret.cfg                = request.sched_cfg;
  return ret;
}

/// Convert a UE reconfiguration request for the MAC into a UE reconfiguration request of the scheduler.
static sched_ue_reconfiguration_message
make_scheduler_ue_reconfiguration_request(const mac_ue_reconfiguration_request& request)
{
  sched_ue_reconfiguration_message ret{};
  ret.ue_index = request.ue_index;
  ret.crnti    = request.crnti;
  ret.cfg      = request.sched_cfg;
  return ret;
}

srsran_scheduler_adapter::srsran_scheduler_adapter(const mac_config& params,
                                                   rnti_manager&     rnti_mng_,
                                                   rlf_detector&     rlf_handler_) :
  rnti_mng(rnti_mng_),
  rlf_handler(rlf_handler_),
  ctrl_exec(params.ctrl_exec),
  logger(srslog::fetch_basic_logger("MAC")),
  notifier(*this),
  sched_impl(create_scheduler(scheduler_config{params.sched_cfg, notifier, params.metric_notifier}))
{
}

void srsran_scheduler_adapter::add_cell(const mac_cell_creation_request& msg)
{
  // Setup UCI decoder for new cell.
  cell_handlers.emplace(msg.cell_index, msg.cell_index, *this, msg.sched_req);

  // Forward cell configuration to scheduler.
  sched_impl->handle_cell_configuration_request(msg.sched_req);
}

async_task<bool> srsran_scheduler_adapter::handle_ue_creation_request(const mac_ue_create_request& msg)
{
  return launch_async([this, msg](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);

    // Create UE in the Scheduler.
    sched_impl->handle_ue_creation_request(make_scheduler_ue_creation_request(msg));

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
    sched_impl->handle_ue_reconfiguration_request(make_scheduler_ue_reconfiguration_request(msg));

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
    sched_impl->handle_ue_removal_request(msg.ue_index);

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
  sched_impl->handle_ul_bsr_indication(ul_bsr_ind);
}

void srsran_scheduler_adapter::handle_ul_sched_command(const mac_ul_scheduling_command& cmd)
{
  // Convert UL scheduling command into a small BSR indication. The SR indication is not used for this purpose
  // because the Scheduler relies on UCI signalling to know the current UE state.
  ul_bsr_indication_message bsr{};
  bsr.cell_index    = cmd.cell_index;
  bsr.ue_index      = cmd.ue_index;
  bsr.crnti         = cmd.rnti;
  bsr.type          = bsr_format::SHORT_BSR;
  bsr.reported_lcgs = {ul_bsr_lcg_report{uint_to_lcg_id(0), 8U}};

  // Send BSR indication to Scheduler.
  sched_impl->handle_ul_bsr_indication(bsr);
}

void srsran_scheduler_adapter::handle_dl_mac_ce_indication(const mac_ce_scheduling_command& mac_ce)
{
  sched_impl->handle_dl_mac_ce_indication(dl_mac_ce_indication{mac_ce.ue_index, mac_ce.ce_lcid});
}

void srsran_scheduler_adapter::handle_dl_buffer_state_update(
    const mac_dl_buffer_state_indication_message& mac_dl_bs_ind)
{
  // Forward DL buffer state indication to the scheduler.
  dl_buffer_state_indication_message bs{};
  bs.ue_index = mac_dl_bs_ind.ue_index;
  bs.lcid     = mac_dl_bs_ind.lcid;
  bs.bs       = mac_dl_bs_ind.bs;
  sched_impl->handle_dl_buffer_state_indication(bs);
}

void srsran_scheduler_adapter::handle_ul_phr_indication(const mac_phr_ce_info& phr)
{
  // Forward UL PHR indication to the scheduler.
  ul_phr_indication_message ind{};
  ind.cell_index = phr.cell_index;
  ind.ue_index   = phr.ue_index;
  ind.rnti       = phr.rnti;
  ind.phr        = phr.phr;
  sched_impl->handle_ul_phr_indication(ind);
}

const sched_result& srsran_scheduler_adapter::slot_indication(slot_point slot_tx, du_cell_index_t cell_idx)
{
  const sched_result& res = sched_impl->slot_indication(slot_tx, cell_idx);

  if (res.success) {
    // Store UCI PDUs for later decoding.
    cell_handlers[cell_idx].uci_decoder.store_uci(slot_tx, res.ul.pucchs);
  }

  return res;
}

void srsran_scheduler_adapter::sched_config_notif_adapter::on_ue_config_complete(du_ue_index_t ue_index,
                                                                                 bool          ue_creation_result)
{
  srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid ue index={}", ue_index);

  // Remove continuation of task in ctrl executor.
  if (not parent.ctrl_exec.defer([this, ue_index, ue_creation_result]() {
        parent.sched_cfg_notif_map[ue_index].ue_config_ready.set(ue_creation_result);
      })) {
    parent.logger.error("ue={}: Unable to finish UE configuration. Cause: DU task queue is full.", ue_index);
  }
}

void srsran_scheduler_adapter::sched_config_notif_adapter::on_ue_delete_response(du_ue_index_t ue_index)
{
  srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid ue index={}", ue_index);

  // Continuation of ue remove task dispatched to the ctrl executor.
  if (not parent.ctrl_exec.defer(
          [this, ue_index]() { parent.sched_cfg_notif_map[ue_index].ue_config_ready.set(true); })) {
    parent.logger.error("ue={}: Unable to remove UE. Cause: DU task queue is full.", ue_index);
  }
}

void srsran_scheduler_adapter::handle_paging_information(const paging_information& msg)
{
  // Convert MAC paging information to scheduler notification.
  sched_paging_information pg_info{};
  pg_info.paging_drx              = msg.paging_drx;
  pg_info.paging_identity         = msg.paging_identity;
  pg_info.paging_type_indicator   = msg.paging_type_indicator;
  pg_info.ue_identity_index_value = msg.ue_identity_index_value;
  pg_info.paging_cells.resize(msg.paging_cells.size());
  pg_info.paging_cells.assign(msg.paging_cells.begin(), msg.paging_cells.end());

  sched_impl->handle_paging_information(pg_info);
}

void srsran_scheduler_adapter::cell_handler::handle_rach_indication(const mac_rach_indication& rach_ind)
{
  // Create Scheduler RACH indication message. Allocate TC-RNTIs in the process.
  rach_indication_message sched_rach{};
  sched_rach.cell_index = cell_idx;
  sched_rach.slot_rx    = rach_ind.slot_rx;
  for (const auto& occasion : rach_ind.occasions) {
    auto& sched_occasion           = sched_rach.occasions.emplace_back();
    sched_occasion.start_symbol    = occasion.start_symbol;
    sched_occasion.frequency_index = occasion.frequency_index;
    for (const auto& preamble : occasion.preambles) {
      rnti_t alloc_tc_rnti = parent->rnti_alloc.allocate();
      if (alloc_tc_rnti == rnti_t::INVALID_RNTI) {
        parent->logger.warning(
            "cell={} preamble id={}: Ignoring PRACH. Cause: Failed to allocate TC-RNTI.", cell_idx, preamble.index);
        continue;
      }
      auto& sched_preamble        = sched_occasion.preambles.emplace_back();
      sched_preamble.preamble_id  = preamble.index;
      sched_preamble.tc_rnti      = alloc_tc_rnti;
      sched_preamble.time_advance = preamble.time_advance;
    }
    if (sched_occasion.preambles.empty()) {
      // No preamble was added. Remove occasion.
      sched_rach.occasions.pop_back();
    }
  }

  // Forward RACH indication to scheduler.
  parent->sched_impl->handle_rach_indication(sched_rach);
}

void srsran_scheduler_adapter::cell_handler::handle_crc(const mac_crc_indication_message& msg)
{
  // Convert MAC CRC indication to srsRAN scheduler UL CRC indication.
  ul_crc_indication ind{};
  ind.cell_index = cell_idx;
  ind.sl_rx      = msg.sl_rx;
  ind.crcs.resize(msg.crcs.size());
  for (unsigned i = 0; i != msg.crcs.size(); ++i) {
    const mac_crc_pdu&     mac_pdu = msg.crcs[i];
    ul_crc_pdu_indication& pdu     = ind.crcs[i];
    pdu.rnti                       = mac_pdu.rnti;
    pdu.ue_index                   = parent->rnti_mng[mac_pdu.rnti];
    pdu.harq_id                    = to_harq_id(mac_pdu.harq_id);
    pdu.tb_crc_success             = mac_pdu.tb_crc_success;
    pdu.ul_sinr_metric             = mac_pdu.ul_sinr_metric;
  }

  // Forward CRC indication to the scheduler.
  parent->sched_impl->handle_crc_indication(ind);

  // Report to RLF handler the CRC result.
  for (unsigned i = 0; i != ind.crcs.size(); ++i) {
    // If Msg3, ignore the CRC result.
    // Note: UE index is invalid for Msg3 CRCs because no UE has been allocated yet.
    if (ind.crcs[i].ue_index != INVALID_DU_UE_INDEX) {
      parent->rlf_handler.handle_crc(ind.crcs[i].ue_index, ind.crcs[i].tb_crc_success);
    }
  }
}

void srsran_scheduler_adapter::cell_handler::handle_uci(const mac_uci_indication_message& msg)
{
  // Forward UCI indication to the scheduler.
  parent->sched_impl->handle_uci_indication(uci_decoder.decode_uci(msg));
}
