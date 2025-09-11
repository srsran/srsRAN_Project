/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/scheduler/result/sched_result.h"
#include "srsran/scheduler/scheduler_factory.h"

using namespace srsran;

/// Convert a UE creation request for the MAC into a UE creation request of the scheduler.
static sched_ue_creation_request_message make_scheduler_ue_creation_request(const mac_ue_create_request& request)
{
  sched_ue_creation_request_message ret{};
  ret.ue_index           = request.ue_index;
  ret.crnti              = request.crnti;
  ret.starts_in_fallback = request.initial_fallback;
  ret.ul_ccch_slot_rx    = request.ul_ccch_slot_rx;
  ret.cfg                = request.sched_cfg;
  ret.tag_config         = request.mac_cell_group_cfg.tag_config;
  return ret;
}

/// Convert a UE reconfiguration request for the MAC into a UE reconfiguration request of the scheduler.
static sched_ue_reconfiguration_message
make_scheduler_ue_reconfiguration_request(const mac_ue_reconfiguration_request& request)
{
  sched_ue_reconfiguration_message ret{};
  ret.ue_index      = request.ue_index;
  ret.crnti         = request.crnti;
  ret.cfg           = request.sched_cfg;
  ret.reestablished = request.reestablished;
  return ret;
}

srsran_scheduler_adapter::srsran_scheduler_adapter(const srsran_mac_sched_config& params, rnti_manager& rnti_mng_) :
  rnti_mng(rnti_mng_),
  rlf_handler(params.mac_cfg),
  ctrl_exec(params.ctrl_exec),
  logger(srslog::fetch_basic_logger("MAC")),
  notifier(*this),
  sched_impl(create_scheduler(scheduler_config{params.sched_cfg, notifier})),
  rach_handler(*sched_impl, rnti_mng, logger)
{
  srsran_assert(last_slot_point.is_lock_free(), "slot point is not lock free");
  srsran_assert(last_slot_tp.is_lock_free(), "slot time_point is not lock free");
}

void srsran_scheduler_adapter::add_cell(const mac_scheduler_cell_creation_request& msg)
{
  // Setup UCI decoder for new cell.
  cell_handlers.emplace(msg.cell_params.cell_index, *this, msg.cell_params.sched_req);

  // Forward cell configuration to scheduler.
  auto sched_req    = msg.cell_params.sched_req;
  sched_req.metrics = {msg.metric_report_period, msg.metric_notifier};
  sched_impl->handle_cell_configuration_request(sched_req);
}

void srsran_scheduler_adapter::remove_cell(du_cell_index_t cell_index)
{
  if (not cell_handlers.contains(cell_index)) {
    return;
  }

  // Request cell removal from scheduler.
  sched_impl->handle_cell_removal_request(cell_index);

  // Remove cell from cell handlers.
  cell_handlers.erase(cell_index);
}

async_task<bool> srsran_scheduler_adapter::handle_ue_creation_request(const mac_ue_create_request& msg)
{
  return launch_async([this, msg](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);

    // Add UE to RLF handler.
    rlf_handler.add_ue(msg.ue_index, *msg.rlf_notifier);

    // In case of CFRA, store the RA preamble.
    if (msg.cfra_preamble_index.has_value()) {
      if (not cell_handlers[msg.cell_index].get_rach_handler().handle_cfra_allocation(
              msg.cfra_preamble_index.value(), msg.ue_index, msg.crnti)) {
        CORO_EARLY_RETURN(false);
      }
    }

    // Create UE in the Scheduler.
    sched_impl->handle_ue_creation_request(make_scheduler_ue_creation_request(msg));

    // Await Scheduler notification that UE was added.
    CORO_AWAIT_VALUE(bool res, sched_cfg_notif_map[msg.ue_index].ue_config_ready);
    sched_cfg_notif_map[msg.ue_index].ue_config_ready.reset();

    CORO_RETURN(res);
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

async_task<void> srsran_scheduler_adapter::handle_ue_removal_request(const mac_ue_delete_request& msg)
{
  return launch_async([this, msg](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    // Remove UE from the scheduler.
    sched_impl->handle_ue_removal_request(msg.ue_index);

    // Await Scheduler notification.
    CORO_AWAIT(sched_cfg_notif_map[msg.ue_index].ue_config_ready);
    sched_cfg_notif_map[msg.ue_index].ue_config_ready.reset();

    // In case the UE was created via CFRA, remove the RA preamble.
    cell_handlers[msg.cell_index].get_rach_handler().handle_cfra_deallocation(msg.ue_index);

    // Remove UE from RLF handler.
    rlf_handler.rem_ue(msg.ue_index, msg.cell_index);

    CORO_RETURN();
  });
}

void srsran_scheduler_adapter::handle_ue_config_applied(du_ue_index_t ue_index)
{
  // Notify scheduler that the UE confirmed the configuration.
  sched_impl->handle_ue_config_applied(ue_index);
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

static slot_point chrono_to_slot_point(std::chrono::high_resolution_clock::time_point hol_toa,
                                       std::chrono::high_resolution_clock::time_point last_slot_tp,
                                       slot_point                                     last_slot_p)
{
  using namespace std::chrono;
  static constexpr microseconds half_system_frame_dur = milliseconds{10240 / 2};

  // Get delay between last slot indication time point and HOL ToA.
  microseconds hol_delay = duration_cast<microseconds>(last_slot_tp - hol_toa);

  // Bound delay to avoid negative values and slot wrap around ambiguity.
  hol_delay = std::min(std::max(hol_delay, microseconds{0}), half_system_frame_dur);

  // Convert usec to slots
  const int hol_delay_slots = hol_delay.count() * last_slot_p.nof_slots_per_subframe() / 1000;

  // Subtract the delay to the current slot.
  return last_slot_p - hol_delay_slots;
}

void srsran_scheduler_adapter::handle_dl_buffer_state_update(
    const mac_dl_buffer_state_indication_message& mac_dl_bs_ind)
{
  using namespace std::chrono;

  // Forward DL buffer state indication to the scheduler.
  dl_buffer_state_indication_message bs{};
  bs.ue_index = mac_dl_bs_ind.ue_index;
  bs.lcid     = mac_dl_bs_ind.lcid;
  bs.bs       = mac_dl_bs_ind.bs;
  if (mac_dl_bs_ind.hol_toa.has_value()) {
    // Check if at least one slot indication has been processed.
    const high_resolution_clock::time_point sl_tp = last_slot_tp.load(std::memory_order_relaxed);
    if (sl_tp != high_resolution_clock::time_point{}) {
      // Convert HOL TOA from chrono time point to slots.
      bs.hol_toa =
          chrono_to_slot_point(mac_dl_bs_ind.hol_toa.value(), sl_tp, last_slot_point.load(std::memory_order_relaxed));
    }
  }

  // Forward DL buffer status.
  sched_impl->handle_dl_buffer_state_indication(bs);
}

void srsran_scheduler_adapter::handle_ul_phr_indication(const mac_phr_ce_info& phr)
{
  // Forward UL PHR indication to the scheduler.
  ul_phr_indication_message ind{};
  ind.cell_index = phr.cell_index;
  ind.ue_index   = phr.ue_index;
  ind.rnti       = phr.rnti;
  ind.slot_rx    = phr.slot_rx;
  ind.phr        = phr.phr;
  sched_impl->handle_ul_phr_indication(ind);
}

void srsran_scheduler_adapter::handle_crnti_ce_indication(du_ue_index_t old_ue_index, du_cell_index_t cell_index)
{
  rlf_handler.handle_crnti_ce(old_ue_index);
}

const sched_result& srsran_scheduler_adapter::slot_indication(slot_point slot_tx, du_cell_index_t cell_idx)
{
  using namespace std::chrono;

  // Mark start of the slot in time (estimate).
  if (last_slot_point.exchange(slot_tx, std::memory_order_relaxed) != slot_tx) {
    auto slot_tp = high_resolution_clock::now();
    last_slot_tp.store(slot_tp, std::memory_order_relaxed);
  }

  const sched_result& res = sched_impl->slot_indication(slot_tx, cell_idx);

  if (res.success) {
    // Store UCI PDUs for later decoding.
    cell_handlers[cell_idx].uci_decoder.store_uci(slot_tx, res.ul.pucchs, res.ul.puschs);
  }

  return res;
}

void srsran_scheduler_adapter::handle_error_indication(slot_point                         slot_tx,
                                                       du_cell_index_t                    cell_idx,
                                                       mac_cell_slot_handler::error_event event)
{
  scheduler_slot_handler::error_outcome sched_err;
  sched_err.pdcch_discarded           = event.pdcch_discarded;
  sched_err.pdsch_discarded           = event.pdsch_discarded;
  sched_err.pusch_and_pucch_discarded = event.pusch_and_pucch_discarded;
  sched_impl->handle_error_indication(slot_tx, cell_idx, sched_err);
}

void srsran_scheduler_adapter::handle_si_change_indication(const si_scheduling_update_request& request)
{
  sched_impl->handle_si_update_request(request);
}

async_task<mac_cell_positioning_measurement_response>
srsran_scheduler_adapter::handle_positioning_measurement_request(du_cell_index_t cell_index,
                                                                 const mac_cell_positioning_measurement_request& req)
{
  return cell_handlers[cell_index].pos_handler->handle_positioning_measurement_request(req);
}

void srsran_scheduler_adapter::sched_config_notif_adapter::on_ue_config_complete(du_ue_index_t ue_index,
                                                                                 bool          ue_creation_result)
{
  srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid ue index={}", fmt::underlying(ue_index));

  // Remove continuation of task in ctrl executor.
  if (not parent.ctrl_exec.defer([this, ue_index, ue_creation_result]() {
        parent.sched_cfg_notif_map[ue_index].ue_config_ready.set(ue_creation_result);
      })) {
    parent.logger.error("ue={}: Unable to finish UE configuration. Cause: DU task queue is full.",
                        fmt::underlying(ue_index));
  }
}

void srsran_scheduler_adapter::sched_config_notif_adapter::on_ue_deletion_completed(du_ue_index_t ue_index)
{
  srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid ue index={}", fmt::underlying(ue_index));

  // Continuation of ue remove task dispatched to the ctrl executor.
  if (not parent.ctrl_exec.defer(
          [this, ue_index]() { parent.sched_cfg_notif_map[ue_index].ue_config_ready.set(true); })) {
    parent.logger.error("ue={}: Unable to remove UE. Cause: DU task queue is full.", fmt::underlying(ue_index));
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

srsran_scheduler_adapter::cell_handler::cell_handler(srsran_scheduler_adapter&                       parent_,
                                                     const sched_cell_configuration_request_message& sched_cfg) :
  uci_decoder(sched_cfg, parent_.rnti_mng, parent_.rlf_handler),
  pos_handler(create_positioning_handler(*parent_.sched_impl, sched_cfg.cell_index, parent_.ctrl_exec, parent_.logger)),
  cell_idx(sched_cfg.cell_index),
  parent(parent_),
  rach_handler(parent.rach_handler.add_cell(sched_cfg))
{
}

void srsran_scheduler_adapter::cell_handler::handle_crc(const mac_crc_indication_message& msg)
{
  // Convert MAC CRC indication to srsRAN scheduler UL CRC indication.
  ul_crc_indication ind{};
  ind.cell_index = cell_idx;
  ind.sl_rx      = msg.sl_rx;
  ind.crcs.resize(msg.crcs.size());
  for (unsigned i = 0, size = msg.crcs.size(); i != size; ++i) {
    const mac_crc_pdu&     mac_pdu = msg.crcs[i];
    ul_crc_pdu_indication& pdu     = ind.crcs[i];
    pdu.rnti                       = mac_pdu.rnti;
    pdu.ue_index                   = parent.rnti_mng[mac_pdu.rnti];
    pdu.harq_id                    = to_harq_id(mac_pdu.harq_id);
    pdu.tb_crc_success             = mac_pdu.tb_crc_success;
    pdu.ul_sinr_dB                 = mac_pdu.ul_sinr_dB;
    pdu.ul_rsrp_dBFS               = mac_pdu.ul_rsrp_dBFS;
    pdu.time_advance_offset        = mac_pdu.time_advance_offset;
  }

  // Forward CRC indication to the scheduler.
  parent.sched_impl->handle_crc_indication(ind);

  // Report to RLF handler the CRC result.
  for (const auto& crc : ind.crcs) {
    // If Msg3, ignore the CRC result.
    // Note: UE index is invalid for Msg3 CRCs because no UE has been allocated yet.
    if (crc.ue_index != INVALID_DU_UE_INDEX) {
      parent.rlf_handler.handle_crc(crc.ue_index, cell_idx, crc.tb_crc_success);
    }
  }
}

void srsran_scheduler_adapter::cell_handler::handle_uci(const mac_uci_indication_message& msg)
{
  // Forward UCI indication to the scheduler.
  parent.sched_impl->handle_uci_indication(uci_decoder.decode_uci(msg));
}

void srsran_scheduler_adapter::cell_handler::handle_srs(const mac_srs_indication_message& msg)
{
  srs_indication ind;
  ind.cell_index = cell_idx;
  ind.slot_rx    = msg.sl_rx;
  for (const auto& mac_pdu : msg.srss) {
    // Only add PDUs with normalized channel IQ matrix.
    if (const auto* matrix = std::get_if<mac_srs_pdu::normalized_channel_iq_matrix>(&mac_pdu.report))
      ind.srss.emplace_back(
          parent.rnti_mng[mac_pdu.rnti], mac_pdu.rnti, mac_pdu.time_advance_offset, matrix->channel_matrix);
  }
  // Forward SRS indication to the scheduler.
  parent.sched_impl->handle_srs_indication(ind);

  // Forward SRS into positioning handler.
  pos_handler->handle_srs_indication(msg);
}
