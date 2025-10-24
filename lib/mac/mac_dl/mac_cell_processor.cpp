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

#include "mac_cell_processor.h"
#include "mac_dl_metric_handler.h"
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/mac/mac_cell_result.h"
#include "srsran/mac/mac_cell_timing_context.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/scheduler/result/sched_result.h"
#include "srsran/support/async/async_timer.h"
#include "srsran/support/async/execute_on_blocking.h"
#include "srsran/support/executors/execute_until_success.h"
#include "srsran/support/rtsan.h"

using namespace srsran;

/// Maximum PDSH K0 value as per TS38.331 "PDSCH-TimeDomainResourceAllocation".
static constexpr size_t MAX_K0_DELAY = 32;

mac_cell_processor::mac_cell_processor(const mac_cell_creation_request& cell_cfg_req_,
                                       mac_scheduler_cell_info_handler& sched_,
                                       du_rnti_table&                   rnti_table,
                                       mac_cell_result_notifier&        phy_notifier_,
                                       task_executor&                   cell_exec_,
                                       task_executor&                   slot_exec_,
                                       task_executor&                   ctrl_exec_,
                                       mac_pcap&                        pcap_,
                                       timer_manager&                   timers_,
                                       mac_cell_config_dependencies     dependencies) :
  logger(srslog::fetch_basic_logger("MAC")),
  cell_cfg(cell_cfg_req_),
  cell_exec(cell_exec_),
  slot_exec(slot_exec_),
  ctrl_exec(ctrl_exec_),
  phy_cell(phy_notifier_),
  timers(timers_),
  ue_mng(rnti_table),
  dl_harq_buffers(band_helper::get_n_rbs_from_bw(MHz_to_bs_channel_bandwidth(cell_cfg.dl_carrier.carrier_bw_mhz),
                                                 cell_cfg.scs_common,
                                                 band_helper::get_freq_range(cell_cfg.dl_carrier.band)),
                  cell_cfg.dl_carrier.nof_ant,
                  ctrl_exec_),
  // The PDU pool has to be large enough to fit the maximum number of RARs and Paging PDUs per slot for all possible K0
  // values.
  pdu_pool(MAX_DL_PDU_LENGTH,
           MAX_K0_DELAY,
           get_nof_slots_per_subframe(cell_cfg.scs_common) * NOF_SFNS * NOF_SUBFRAMES_PER_FRAME),
  ssb_helper(cell_cfg_req_),
  sib_assembler(cell_cfg_req_.sys_info),
  rar_assembler(pdu_pool),
  dlsch_assembler(ue_mng, dl_harq_buffers),
  paging_assembler(pdu_pool),
  sched(sched_),
  time_source(std::move(dependencies.timer_source)),
  metrics(cell_cfg.pci, cell_cfg.scs_common, dependencies.notifier),
  pcap(pcap_),
  slot_time_mapper(to_numerology_value(cell_cfg_req_.scs_common)),
  sib1_pcap_dumped_version(std::numeric_limits<unsigned>::max())
{
  std::fill(si_pcap_dumped_version.begin(), si_pcap_dumped_version.end(), std::numeric_limits<unsigned>::max());
}

async_task<void> mac_cell_processor::start()
{
  // Notify scheduler about activation.
  // Note: This is done in the control executor context to avoid concurrency with other CTRL procedures.
  sched.handle_cell_activation(cell_cfg.cell_index);

  return execute_and_continue_on_blocking(
      cell_exec,
      ctrl_exec,
      timers,
      [this]() noexcept SRSRAN_RTSAN_NONBLOCKING {
        if (state != cell_state::inactive) {
          // No-op.
          return;
        }

        state = cell_state::active;
        logger.info("cell={}: Cell was activated", fmt::underlying(cell_cfg.cell_index));
      },
      [this]() {
        logger.warning("cell={}: Postponed cell start operation. Cause: Task queue is full",
                       fmt::underlying(cell_cfg.cell_index));
      });
}

async_task<void> mac_cell_processor::stop()
{
  return launch_async([this](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    // Switch to respective cell executor context.
    CORO_AWAIT(defer_on_blocking(cell_exec, timers));

    if (state == cell_state::inactive) {
      // No state change detected.
      CORO_EARLY_RETURN();
    }
    // Set cell state as inactive.
    // Note: No more slot indications will reach the scheduler after this point. So, we will stop observing new
    // scheduled grants.
    state = cell_state::inactive;

    // Notify time source that the cell is being deactivated and no slot indications will be received anymore.
    time_source->on_cell_deactivation();

    // Notify lower layers that the cell is being stopped.
    // TODO: Rely on FAPI STOP procedure to signal the cell stop. For now, we just skip this step.

    // Notify that cell metrics stopped being collected.
    metrics.on_cell_deactivation();

    // Clear all UEs.
    ue_mng.clear();

    // Switch back to respective ctrl executor context.
    CORO_AWAIT(defer_on_blocking(ctrl_exec, timers));

    // Signal to the scheduler that the cell was successfully stopped in the lower layers.
    // Note: This is done in the control executor context to avoid concurrency with other CTRL procedures.
    sched.handle_cell_deactivation(cell_cfg.cell_index);

    // Clear DL buffers associated with this cell.
    dl_harq_buffers.clear();

    logger.info("cell={}: Cell was stopped.", fmt::underlying(cell_cfg.cell_index));

    CORO_RETURN();
  });
}

async_task<mac_cell_reconfig_response> mac_cell_processor::reconfigure(const mac_cell_reconfig_request& request)
{
  return launch_async([this, request, resp = mac_cell_reconfig_response{}](
                          coro_context<async_task<mac_cell_reconfig_response>>& ctx) mutable {
    CORO_BEGIN(ctx);

    if (request.new_sys_info.has_value()) {
      // Change to respective DL cell executor context.
      CORO_AWAIT(defer_on_blocking(cell_exec, timers));

      {
        SRSRAN_RTSAN_SCOPED_ENABLER;

        if (request.new_sys_info.has_value()) {
          // Forward new SIB1/SI message PDUs to SIB assembler and update version.
          sib_assembler.handle_si_change_request(*request.new_sys_info);

          // Notify scheduler of SIB1/SI message scheduling update.
          sched.handle_si_change_indication(request.new_sys_info->si_sched_cfg);

          resp.si_updated = true;
        }
      }

      // Change back to CTRL executor context.
      CORO_AWAIT(defer_on_blocking(ctrl_exec, timers));
    }

    if (request.new_si_pdu_info.has_value()) {
      sib_assembler.enqueue_si_message_pdu_updates(*request.new_si_pdu_info);
      resp.si_pdus_enqueued = true;
    }

    if (request.slice_reconf_req.has_value()) {
      // Change to respective DL cell executor context.
      CORO_AWAIT(execute_on_blocking(cell_exec, timers));

      {
        SRSRAN_RTSAN_SCOPED_ENABLER;
        sched.handle_slice_reconfiguration_request(request.slice_reconf_req.value());
      }

      // Change back to CTRL executor context.
      CORO_AWAIT(execute_on_blocking(ctrl_exec, timers));
    }

    CORO_RETURN(resp);
  });
}

void mac_cell_processor::handle_slot_indication(const mac_cell_timing_context& context) noexcept
    SRSRAN_RTSAN_NONBLOCKING
{
  slot_time_mapper.handle_slot_indication(context);
  trace_point slot_ind_enqueue_tp = metric_clock::now();
  // Change execution context to slot indication executor.
  if (not slot_exec.execute([this, context, slot_ind_enqueue_tp]() {
        l2_tracer << trace_event{"mac_slot_ind_enqueue", slot_ind_enqueue_tp};
        handle_slot_indication_impl(context.sl_tx, slot_ind_enqueue_tp);
      })) {
    logger.warning("Skipped slot indication={}. Cause: DL task queue is full.", context.sl_tx);
  }
}

void mac_cell_processor::handle_error_indication(slot_point sl_tx, error_event event) noexcept SRSRAN_RTSAN_NONBLOCKING
{
  // Forward error indication to the scheduler to be processed asynchronously.
  sched.handle_error_indication(sl_tx, cell_cfg.cell_index, event);
}

void mac_cell_processor::handle_stop_indication() noexcept
{
  defer_until_success(cell_exec, timers, [this]() {
    // Signal the cell stop procedure that the FAPI completed the FAPI STOP procedure.
    stop_completed.set();
  });
}

async_task<bool> mac_cell_processor::add_ue(const mac_ue_create_request& request)
{
  // > Allocate DL HARQ resources for the new UE.
  // Note: This may call a large allocation, and therefore, should be done out of the cell thread to avoid causing
  // lates.
  dl_harq_buffers.allocate_ue_buffers(request.ue_index, MAX_NOF_HARQS);

  // > Create a MAC UE DL context.
  mac_dl_ue_context ue_inst(request);

  // > Update the UE context inside the cell executor.
  return defer_and_continue_on_blocking(
      cell_exec,
      ctrl_exec,
      timers,
      [this, ue_inst = std::move(ue_inst)]() mutable noexcept SRSRAN_RTSAN_NONBLOCKING {
        // > Insert UE and DL bearers.

        // Note: Ensure we only do so if the cell is active.
        if (state != cell_state::active) {
          // Note: We do not need to call dl_harq_buffers.deallocate_ue_buffers(...) because the buffers were reset
          // on cell deactivation.
          return false;
        }

        return ue_mng.add_ue(std::move(ue_inst));
      },
      [this, ue_index = request.ue_index]() {
        logger.warning("ue={}: Postponed UE creation. Cause: Task queue is full", fmt::underlying(ue_index));
      });
}

async_task<void> mac_cell_processor::remove_ue(const mac_ue_delete_request& request)
{
  auto log_dispatch_failure = [this, ue_index = request.ue_index]() {
    logger.warning("ue={}: Postponed UE removal. Cause: task queue is full", fmt::underlying(ue_index));
  };

  return launch_async([this, request, log_dispatch_failure](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    // Change to respective DL cell executor.
    // Note: Caller (ctrl exec) blocks if the cell executor is full.
    CORO_AWAIT(defer_on_blocking(cell_exec, timers, log_dispatch_failure));

    {
      SRSRAN_RTSAN_SCOPED_ENABLER;

      // Remove UE associated DL channels
      ue_mng.remove_ue(request.ue_index);
    }

    // Change back to CTRL executor before returning
    // Note: Blocks if the executor is full (which should never happen).
    CORO_AWAIT(defer_on_blocking(ctrl_exec, timers, log_dispatch_failure));

    // Deallocate DL HARQ buffers back in the CTRL executor.
    dl_harq_buffers.deallocate_ue_buffers(request.ue_index);

    CORO_RETURN();
  });
}

async_task<bool> mac_cell_processor::addmod_bearers(du_ue_index_t                          ue_index,
                                                    span<const mac_logical_channel_config> logical_channels)
{
  // Note: logical_channels must outlive the returned async_task completion.

  return defer_and_continue_on_blocking(
      cell_exec,
      ctrl_exec,
      timers,
      [this, ue_index, logical_channels]() noexcept SRSRAN_RTSAN_NONBLOCKING {
        // Configure logical channels.
        return state == cell_state::active and ue_mng.addmod_bearers(ue_index, logical_channels);
      },
      [this, ue_index]() {
        logger.warning("ue={}: Postponed UE bearer add/mod operation. Cause: Task queue is full",
                       fmt::underlying(ue_index));
      });
}

async_task<bool> mac_cell_processor::remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids_to_rem)
{
  // Use bitset to minimize capture size.
  auto lcids_to_rem_bset = bit_positions_to_bitset<MAX_NOF_RB_LCIDS>(lcids_to_rem);

  return defer_and_continue_on_blocking(
      cell_exec,
      ctrl_exec,
      timers,
      [this, ue_index, lcids_to_rem_bset]() noexcept SRSRAN_RTSAN_NONBLOCKING -> bool {
        // Remove logical channels.
        return state == cell_state::active and ue_mng.remove_bearers(ue_index, lcids_to_rem_bset);
      },
      [this, ue_index]() {
        logger.warning("ue={}: Postponed UE bearer removal. Cause: Task queue is full", fmt::underlying(ue_index));
      });
}

void mac_cell_processor::handle_slot_indication_impl(slot_point               sl_tx,
                                                     metric_clock::time_point enqueue_slot_tp) noexcept
    SRSRAN_RTSAN_NONBLOCKING
{
  // * Start of Critical Path * //

  logger.set_context(sl_tx.sfn(), sl_tx.slot_index());

  if (SRSRAN_UNLIKELY(state == cell_state::inactive)) {
    // Ignore slot indication if cell is inactive.
    phy_cell.on_cell_results_completion(sl_tx);
    return;
  }

  // Tick DU timers.
  time_source->on_slot_indication(sl_tx);

  // Initiate metric capturing.
  auto        metrics_meas = metrics.start_slot(sl_tx, enqueue_slot_tp);
  trace_point sched_tp     = metrics_meas.start_time_point();

  // Cleans old MAC DL PDU buffers.
  pdu_pool.tick(sl_tx.to_uint());

  // Generate DL scheduling result for provided slot and cell.
  const sched_result& sl_res = sched.slot_indication(sl_tx, cell_cfg.cell_index);
  metrics_meas.on_sched();
  if (not sl_res.success) {
    logger.warning(
        "Unable to compute scheduling result for slot={}, cell={}", sl_tx, fmt::underlying(cell_cfg.cell_index));
    if (sl_res.dl.nof_dl_symbols > 0) {
      mac_dl_sched_result mac_dl_res{};
      mac_dl_res.slot = sl_tx;
      phy_cell.on_new_downlink_scheduler_results(mac_dl_res);
      metrics_meas.on_dl_tti_req();
    }
    if (sl_res.ul.nof_ul_symbols > 0) {
      mac_ul_sched_result mac_ul_res{};
      mac_ul_res.slot = sl_tx;
      phy_cell.on_new_uplink_scheduler_results(mac_ul_res);
      metrics_meas.on_ul_tti_req();
    }
    phy_cell.on_cell_results_completion(sl_tx);
    return;
  }

  l2_tracer << trace_event{"mac_sched", sched_tp};

  mac_dl_data_result data_res{};

  // If it is a DL slot, process results.
  if (sl_res.dl.nof_dl_symbols > 0) {
    trace_point dl_tti_req_tp = l2_tracer.now();

    // Assemble MAC DL scheduling request that is going to be passed to the PHY.
    mac_dl_sched_result mac_dl_res{};
    assemble_dl_sched_request(mac_dl_res, sl_tx, cell_cfg.cell_index, sl_res.dl);

    // Send DL sched result to PHY.
    phy_cell.on_new_downlink_scheduler_results(mac_dl_res);

    metrics_meas.on_dl_tti_req();
    l2_tracer << trace_event{"mac_dl_tti_req", dl_tti_req_tp};

    // Start assembling Slot Data Result.
    if (not sl_res.dl.ue_grants.empty() or not sl_res.dl.rar_grants.empty() or not sl_res.dl.bc.sibs.empty() or
        not sl_res.dl.paging_grants.empty()) {
      trace_point tx_data_req_tp = l2_tracer.now();

      assemble_dl_data_request(data_res, sl_tx, cell_cfg.cell_index, sl_res.dl);

      // Send DL Data to PHY.
      phy_cell.on_new_downlink_data(data_res);

      metrics_meas.on_tx_data_req();
      l2_tracer << trace_event{"mac_tx_data_req", tx_data_req_tp};
    }
  }

  if (sl_res.ul.nof_ul_symbols > 0) {
    trace_point ul_tti_req_tp = l2_tracer.now();

    // Send UL sched result to PHY.
    mac_ul_sched_result mac_ul_res{};
    mac_ul_res.slot   = sl_tx;
    mac_ul_res.ul_res = &sl_res.ul;
    phy_cell.on_new_uplink_scheduler_results(mac_ul_res);

    metrics_meas.on_ul_tti_req();
    l2_tracer << trace_event{"mac_ul_tti_req", ul_tti_req_tp};
  }

  // All results have been notified at this point.
  phy_cell.on_cell_results_completion(sl_tx);

  // * End of Critical Path * //

  trace_point cleanup_tp = l2_tracer.now();

  // Update DL buffer state for the allocated logical channels.
  update_logical_channel_dl_buffer_states(sl_res.dl);

  // Write PCAP.
  write_tx_pdu_pcap(sl_tx, sl_res, data_res);

  l2_tracer << trace_event{"mac_cleanup_tp", cleanup_tp};
}

/// Encodes DL DCI.
static dci_payload encode_dci(const pdcch_dl_information& pdcch)
{
  switch (pdcch.dci.type) {
    case dci_dl_rnti_config_type::si_f1_0:
      return dci_1_0_si_rnti_pack(pdcch.dci.si_f1_0);
    case dci_dl_rnti_config_type::ra_f1_0:
      return dci_1_0_ra_rnti_pack(pdcch.dci.ra_f1_0);
    case dci_dl_rnti_config_type::c_rnti_f1_0:
      return dci_1_0_c_rnti_pack(pdcch.dci.c_rnti_f1_0);
    case dci_dl_rnti_config_type::tc_rnti_f1_0:
      return dci_1_0_tc_rnti_pack(pdcch.dci.tc_rnti_f1_0);
    case dci_dl_rnti_config_type::p_rnti_f1_0:
      return dci_1_0_p_rnti_pack(pdcch.dci.p_rnti_f1_0);
    case dci_dl_rnti_config_type::c_rnti_f1_1:
      return dci_1_1_pack(pdcch.dci.c_rnti_f1_1);
    default:
      srsran_terminate("Invalid DCI format");
  }
}

/// Encodes UL DCI.
static dci_payload encode_dci(const pdcch_ul_information& pdcch)
{
  switch (pdcch.dci.type) {
    case dci_ul_rnti_config_type::c_rnti_f0_0:
      return dci_0_0_c_rnti_pack(pdcch.dci.c_rnti_f0_0);
    case dci_ul_rnti_config_type::tc_rnti_f0_0:
      return dci_0_0_tc_rnti_pack(pdcch.dci.tc_rnti_f0_0);
    case dci_ul_rnti_config_type::c_rnti_f0_1:
      return dci_0_1_pack(pdcch.dci.c_rnti_f0_1);
    default:
      srsran_terminate("Invalid DCI format");
  }
}

void mac_cell_processor::assemble_dl_sched_request(mac_dl_sched_result&   mac_res,
                                                   slot_point             sl_tx,
                                                   du_cell_index_t        cell_index,
                                                   const dl_sched_result& dl_res)
{
  // Pass scheduler output directly to PHY.
  mac_res.dl_res = &dl_res;
  mac_res.slot   = sl_tx;

  // Assemble SSB scheduling info and additional SSB/MIB parameters to pass to PHY.
  for (const ssb_information& ssb : dl_res.bc.ssb_info) {
    ssb_helper.assemble_ssb(mac_res.ssb_pdus.emplace_back(), ssb);
  }

  // Encode DL PDCCH DCI payloads.
  for (const pdcch_dl_information& pdcch : dl_res.dl_pdcchs) {
    mac_res.dl_pdcch_pdus.push_back(encode_dci(pdcch));
  }

  // Encode UL PDCCH DCI payloads.
  for (const pdcch_ul_information& pdcch : dl_res.ul_pdcchs) {
    mac_res.ul_pdcch_pdus.push_back(encode_dci(pdcch));
  }
}

void mac_cell_processor::assemble_dl_data_request(mac_dl_data_result&    data_res,
                                                  slot_point             sl_tx,
                                                  du_cell_index_t        cell_index,
                                                  const dl_sched_result& dl_res)
{
  data_res.slot = sl_tx;
  // Assemble scheduled BCCH-DL-SCH message containing SIBs' payload.
  for (const sib_information& sib_info : dl_res.bc.sibs) {
    srsran_assert(not data_res.si_pdus.full(), "No SIB1 added as SIB1 list in MAC DL data results is already full");
    data_res.si_pdus.emplace_back(0, shared_transport_block(sib_assembler.encode_si_pdu(sl_tx, sib_info)));
  }

  // Assemble scheduled RARs' subheaders and payloads.
  for (const rar_information& rar : dl_res.rar_grants) {
    data_res.rar_pdus.emplace_back(0, shared_transport_block(rar_assembler.encode_rar_pdu(rar)));
  }

  // Assemble data grants.
  for (const dl_msg_alloc& grant : dl_res.ue_grants) {
    for (unsigned cw_idx = 0, e = grant.pdsch_cfg.codewords.size(); cw_idx != e; ++cw_idx) {
      const pdsch_codeword& cw = grant.pdsch_cfg.codewords[cw_idx];
      if (cw.new_data) {
        data_res.ue_pdus.emplace_back(
            cw_idx,
            dlsch_assembler.assemble_newtx_pdu(
                grant.pdsch_cfg.rnti, grant.pdsch_cfg.harq_id, cw_idx, grant.tb_list[cw_idx], cw.tb_size_bytes));
        continue;
      }

      data_res.ue_pdus.emplace_back(
          cw_idx,
          dlsch_assembler.assemble_retx_pdu(grant.pdsch_cfg.rnti, grant.pdsch_cfg.harq_id, cw_idx, cw.tb_size_bytes));
    }
  }

  // Assemble scheduled Paging payloads.
  for (const dl_paging_allocation& pg : dl_res.paging_grants) {
    for (unsigned cw_idx = 0, e = pg.pdsch_cfg.codewords.size(); cw_idx != e; ++cw_idx) {
      data_res.paging_pdus.emplace_back(cw_idx, shared_transport_block(paging_assembler.encode_paging_pdu(pg)));
    }
  }
}

void mac_cell_processor::update_logical_channel_dl_buffer_states(const dl_sched_result& dl_res)
{
  if (dl_res.nof_dl_symbols == 0) {
    return;
  }

  for (const dl_msg_alloc& grant : dl_res.ue_grants) {
    for (const dl_msg_tb_info& tb_info : grant.tb_list) {
      for (const dl_msg_lc_info& lc_info : tb_info.lc_chs_to_sched) {
        if (not lc_info.lcid.is_sdu()) {
          continue;
        }

        // Fetch RLC Bearer.
        mac_sdu_tx_builder* bearer = ue_mng.get_lc_sdu_builder(grant.pdsch_cfg.rnti, lc_info.lcid.to_lcid());
        srsran_sanity_check(bearer != nullptr, "Scheduler is allocating inexistent bearers");

        // Update DL buffer state for the allocated logical channel.
        rlc_buffer_state                       rlc_bs = bearer->on_buffer_state_update();
        mac_dl_buffer_state_indication_message bs{
            ue_mng.get_ue_index(grant.pdsch_cfg.rnti), lc_info.lcid.to_lcid(), rlc_bs.pending_bytes};
        sched.handle_dl_buffer_state_update(bs);
      }
    }
  }
}

void mac_cell_processor::write_tx_pdu_pcap(slot_point                sl_tx,
                                           const sched_result&       sl_res,
                                           const mac_dl_data_result& dl_res)
{
  if (not pcap.is_write_enabled() or sl_res.dl.nof_dl_symbols == 0) {
    return;
  }

  for (unsigned i = 0, e = dl_res.si_pdus.size(); i != e; ++i) {
    const sib_information& dl_alloc = sl_res.dl.bc.sibs[i];
    if (dl_alloc.si_indicator != sib_information::sib1) {
      srsran_assert(dl_alloc.si_msg_index.has_value() and *dl_alloc.si_msg_index < si_pcap_dumped_version.size(),
                    "Invalid SI message index");
    }
    unsigned& dumped_version = (dl_alloc.si_indicator == sib_information::sib1)
                                   ? sib1_pcap_dumped_version
                                   : si_pcap_dumped_version[*dl_alloc.si_msg_index];
    if (dumped_version != dl_alloc.version) {
      const mac_dl_data_result::dl_pdu& sib1_pdu = dl_res.si_pdus[i];
      mac_nr_context_info               context  = {};
      context.radioType = cell_cfg.sched_req.tdd_ul_dl_cfg_common.has_value() ? PCAP_TDD_RADIO : PCAP_FDD_RADIO;
      context.direction = PCAP_DIRECTION_DOWNLINK;
      context.rntiType  = PCAP_SI_RNTI;
      context.rnti      = to_value(dl_alloc.pdsch_cfg.rnti);
      context.system_frame_number = sl_tx.sfn();
      context.sub_frame_number    = sl_tx.subframe_index();
      context.length              = sib1_pdu.pdu.get_buffer().size();
      pcap.push_pdu(context, sib1_pdu.pdu.get_buffer());
      dumped_version = dl_alloc.version;
    }
  }

  for (unsigned i = 0, e = dl_res.rar_pdus.size(); i != e; ++i) {
    const mac_dl_data_result::dl_pdu& rar_pdu  = dl_res.rar_pdus[i];
    const rar_information&            dl_alloc = sl_res.dl.rar_grants[i];
    mac_nr_context_info               context  = {};
    context.radioType           = cell_cfg.sched_req.tdd_ul_dl_cfg_common.has_value() ? PCAP_TDD_RADIO : PCAP_FDD_RADIO;
    context.direction           = PCAP_DIRECTION_DOWNLINK;
    context.rntiType            = PCAP_RA_RNTI;
    context.rnti                = to_value(dl_alloc.pdsch_cfg.rnti);
    context.system_frame_number = sl_tx.sfn();
    context.sub_frame_number    = sl_tx.subframe_index();
    context.length              = rar_pdu.pdu.get_buffer().size();
    pcap.push_pdu(context, rar_pdu.pdu.get_buffer());
  }

  for (unsigned i = 0, e = dl_res.paging_pdus.size(); i != e; ++i) {
    const mac_dl_data_result::dl_pdu& pg_pdu   = dl_res.paging_pdus[i];
    const dl_paging_allocation&       dl_alloc = sl_res.dl.paging_grants[i];
    srsran::mac_nr_context_info       context  = {};
    context.radioType           = cell_cfg.sched_req.tdd_ul_dl_cfg_common.has_value() ? PCAP_TDD_RADIO : PCAP_FDD_RADIO;
    context.direction           = PCAP_DIRECTION_DOWNLINK;
    context.rntiType            = PCAP_P_RNTI;
    context.rnti                = to_value(dl_alloc.pdsch_cfg.rnti);
    context.system_frame_number = sl_tx.sfn();
    context.sub_frame_number    = sl_tx.subframe_index();
    context.length              = pg_pdu.pdu.get_buffer().size();
    pcap.push_pdu(context, pg_pdu.pdu.get_buffer());
  }

  for (unsigned i = 0, e = dl_res.ue_pdus.size(); i != e; ++i) {
    const mac_dl_data_result::dl_pdu& ue_pdu   = dl_res.ue_pdus[i];
    const dl_msg_alloc&               dl_alloc = sl_res.dl.ue_grants[i];
    if (dl_alloc.pdsch_cfg.codewords[0].new_data) {
      srsran::mac_nr_context_info context = {};
      context.radioType = cell_cfg.sched_req.tdd_ul_dl_cfg_common.has_value() ? PCAP_TDD_RADIO : PCAP_FDD_RADIO;
      context.direction = PCAP_DIRECTION_DOWNLINK;
      context.rntiType  = PCAP_C_RNTI;
      context.rnti      = to_value(dl_alloc.pdsch_cfg.rnti);
      context.ueid      = dl_alloc.context.ue_index == du_ue_index_t::INVALID_DU_UE_INDEX
                              ? du_ue_index_t::INVALID_DU_UE_INDEX
                              : dl_alloc.context.ue_index + 1;
      context.harqid    = dl_alloc.pdsch_cfg.harq_id;
      context.system_frame_number = sl_tx.sfn();
      context.sub_frame_number    = sl_tx.subframe_index();
      context.length              = ue_pdu.pdu.get_buffer().size();
      pcap.push_pdu(context, ue_pdu.pdu.get_buffer());
    }
  }
}
