/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/mac/mac_cell_result.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/support/async/execute_on.h"

using namespace srsran;

/// Maximum PDSH K0 value as per TS38.331 "PDSCH-TimeDomainResourceAllocation".
constexpr size_t MAX_K0_DELAY = 32;

mac_cell_processor::mac_cell_processor(const mac_cell_creation_request& cell_cfg_req_,
                                       mac_scheduler_cell_info_handler& sched_,
                                       mac_dl_ue_manager&               ue_mng_,
                                       mac_cell_result_notifier&        phy_notifier_,
                                       task_executor&                   cell_exec_,
                                       task_executor&                   slot_exec_,
                                       task_executor&                   err_ind_exec_,
                                       task_executor&                   ctrl_exec_,
                                       mac_pcap&                        pcap_) :
  logger(srslog::fetch_basic_logger("MAC")),
  cell_cfg(cell_cfg_req_),
  cell_exec(cell_exec_),
  slot_exec(slot_exec_),
  err_ind_exec(err_ind_exec_),
  ctrl_exec(ctrl_exec_),
  phy_cell(phy_notifier_),
  // The PDU pool has to be large enough to fit the maximum number of RARs and Paging PDUs per slot for all possible K0
  // values.
  pdu_pool(MAX_DL_PDU_LENGTH,
           MAX_K0_DELAY,
           get_nof_slots_per_subframe(cell_cfg.scs_common) * NOF_SFNS * NOF_SUBFRAMES_PER_FRAME),
  ssb_helper(cell_cfg_req_),
  sib_assembler(cell_cfg_req_.bcch_dl_sch_payloads),
  rar_assembler(pdu_pool),
  dlsch_assembler(ue_mng_),
  paging_assembler(pdu_pool),
  sched(sched_),
  ue_mng(ue_mng_),
  pcap(pcap_)
{
}

async_task<void> mac_cell_processor::start()
{
  return dispatch_and_resume_on(cell_exec, ctrl_exec, [this]() { state = cell_state::active; });
}

async_task<void> mac_cell_processor::stop()
{
  return dispatch_and_resume_on(cell_exec, ctrl_exec, [this]() { state = cell_state::inactive; });
}

void mac_cell_processor::handle_slot_indication(slot_point sl_tx)
{
  trace_point slot_ind_enqueue_tp = l2_tracer.now();
  // Change execution context to slot indication executor.
  if (not slot_exec.execute([this, sl_tx, slot_ind_enqueue_tp]() {
        l2_tracer << trace_event{"mac_slot_ind_enqueue", slot_ind_enqueue_tp};
        handle_slot_indication_impl(sl_tx);
      })) {
    logger.warning("Skipped slot indication={}. Cause: DL task queue is full.", sl_tx);
  }
}

void mac_cell_processor::handle_error_indication(slot_point sl_tx, error_event event)
{
  // Change execution context to slot indication executor.
  if (not err_ind_exec.execute(
          [this, sl_tx, event]() { sched.handle_error_indication(sl_tx, cell_cfg.cell_index, event); })) {
    logger.warning("slot={}: Skipped error indication. Cause: DL task queue is full.", sl_tx);
  }
}

void mac_cell_processor::handle_slot_indication_impl(slot_point sl_tx)
{
  // * Start of Critical Path * //

  trace_point sched_tp = l2_tracer.now();

  logger.set_context(sl_tx.sfn(), sl_tx.slot_index());

  // Cleans old MAC DL PDU buffers.
  pdu_pool.tick(sl_tx.to_uint());

  if (state != cell_state::active) {
    phy_cell.on_cell_results_completion(sl_tx);
    return;
  }

  // Generate DL scheduling result for provided slot and cell.
  const sched_result& sl_res = sched.slot_indication(sl_tx, cell_cfg.cell_index);
  if (not sl_res.success) {
    logger.warning("Unable to compute scheduling result for slot={}, cell={}", sl_tx, cell_cfg.cell_index);
    if (sl_res.dl.nof_dl_symbols > 0) {
      mac_dl_sched_result mac_dl_res{};
      mac_dl_res.slot = sl_tx;
      phy_cell.on_new_downlink_scheduler_results(mac_dl_res);
    }
    if (sl_res.ul.nof_ul_symbols > 0) {
      mac_ul_sched_result mac_ul_res{};
      mac_ul_res.slot = sl_tx;
      phy_cell.on_new_uplink_scheduler_results(mac_ul_res);
    }
    phy_cell.on_cell_results_completion(sl_tx);
    return;
  }

  mac_dl_sched_result mac_dl_res{};
  mac_dl_data_result  data_res{};

  l2_tracer << trace_event{"mac_sched", sched_tp};

  // If it is a DL slot, process results.
  if (sl_res.dl.nof_dl_symbols > 0) {
    trace_point dl_tti_req_tp = l2_tracer.now();

    // Assemble MAC DL scheduling request that is going to be passed to the PHY.
    assemble_dl_sched_request(mac_dl_res, sl_tx, cell_cfg.cell_index, sl_res.dl);

    // Send DL sched result to PHY.
    phy_cell.on_new_downlink_scheduler_results(mac_dl_res);

    l2_tracer << trace_event{"mac_dl_tti_req", dl_tti_req_tp};

    // Start assembling Slot Data Result.
    if (not sl_res.dl.ue_grants.empty() or not sl_res.dl.rar_grants.empty() or not sl_res.dl.bc.sibs.empty() or
        not sl_res.dl.paging_grants.empty()) {
      trace_point tx_data_req_tp = l2_tracer.now();

      assemble_dl_data_request(data_res, sl_tx, cell_cfg.cell_index, sl_res.dl);

      // Send DL Data to PHY.
      phy_cell.on_new_downlink_data(data_res);

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

    l2_tracer << trace_event{"mac_ul_tti_req", ul_tti_req_tp};
  }

  // All results have been notified at this point.
  phy_cell.on_cell_results_completion(sl_tx);

  // * End of Critical Path * //

  trace_point cleanup_tp = l2_tracer.now();

  // Update DL buffer state for the allocated logical channels.
  update_logical_channel_dl_buffer_states(sl_res.dl);

  // Write PCAP
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
    const units::bytes  tbs(sib_info.pdsch_cfg.codewords[0].tb_size_bytes);
    span<const uint8_t> payload;
    if (sib_info.si_indicator == sib_information::sib1) {
      payload = sib_assembler.encode_sib1_pdu(tbs);
    } else {
      payload = sib_assembler.encode_si_message_pdu(sib_info.si_msg_index.value(), tbs);
    }
    data_res.si_pdus.emplace_back(0, payload);
  }

  // Assemble scheduled RARs' subheaders and payloads.
  for (const rar_information& rar : dl_res.rar_grants) {
    data_res.rar_pdus.emplace_back(0, rar_assembler.encode_rar_pdu(rar));
  }

  // Assemble data grants.
  for (const dl_msg_alloc& grant : dl_res.ue_grants) {
    for (unsigned cw_idx = 0; cw_idx != grant.pdsch_cfg.codewords.size(); ++cw_idx) {
      const pdsch_codeword& cw = grant.pdsch_cfg.codewords[cw_idx];
      span<const uint8_t>   pdu;
      if (cw.new_data) {
        pdu = dlsch_assembler.assemble_newtx_pdu(
            grant.pdsch_cfg.rnti, grant.pdsch_cfg.harq_id, cw_idx, grant.tb_list[cw_idx], cw.tb_size_bytes);
      } else {
        pdu =
            dlsch_assembler.assemble_retx_pdu(grant.pdsch_cfg.rnti, grant.pdsch_cfg.harq_id, cw_idx, cw.tb_size_bytes);
      }
      data_res.ue_pdus.emplace_back(cw_idx, pdu);
    }
  }

  // Assemble scheduled Paging payloads.
  for (const dl_paging_allocation& pg : dl_res.paging_grants) {
    for (unsigned cw_idx = 0; cw_idx != pg.pdsch_cfg.codewords.size(); ++cw_idx) {
      data_res.paging_pdus.emplace_back(cw_idx, paging_assembler.encode_paging_pdu(pg));
    }
  }
}

void mac_cell_processor::update_logical_channel_dl_buffer_states(const dl_sched_result& dl_res)
{
  if (dl_res.nof_dl_symbols > 0) {
    for (const dl_msg_alloc& grant : dl_res.ue_grants) {
      for (const dl_msg_tb_info& tb_info : grant.tb_list) {
        for (const dl_msg_lc_info& lc_info : tb_info.lc_chs_to_sched) {
          if (not lc_info.lcid.is_sdu()) {
            continue;
          }

          // Fetch RLC Bearer.
          mac_sdu_tx_builder* bearer = ue_mng.get_bearer(grant.pdsch_cfg.rnti, lc_info.lcid.to_lcid());
          srsran_sanity_check(bearer != nullptr, "Scheduler is allocating inexistent bearers");

          // Update DL buffer state for the allocated logical channel.
          mac_dl_buffer_state_indication_message bs{
              ue_mng.get_ue_index(grant.pdsch_cfg.rnti), lc_info.lcid.to_lcid(), bearer->on_buffer_state_update()};
          sched.handle_dl_buffer_state_update(bs);
        }
      }
    }
  }
}

void mac_cell_processor::write_tx_pdu_pcap(const slot_point&         sl_tx,
                                           const sched_result&       sl_res,
                                           const mac_dl_data_result& dl_res)
{
  if (not pcap.is_write_enabled() or sl_res.dl.nof_dl_symbols == 0) {
    return;
  }

  for (unsigned i = 0; i < dl_res.si_pdus.size(); ++i) {
    const sib_information& dl_alloc = sl_res.dl.bc.sibs[i];
    // At the moment, we allocate max 1 SIB (SIB1) message per slot. Eventually, this will be extended to other SIBs.
    // TODO: replace sib1_pcap_dumped flag with a vector or booleans that includes other SIBs.
    if (dl_alloc.si_indicator == sib_information::sib1 and not sib1_pcap_dumped) {
      const mac_dl_data_result::dl_pdu& sib1_pdu = dl_res.si_pdus[i];
      srsran::mac_nr_context_info       context  = {};
      context.radioType = cell_cfg.sched_req.tdd_ul_dl_cfg_common.has_value() ? PCAP_TDD_RADIO : PCAP_FDD_RADIO;
      context.direction = PCAP_DIRECTION_DOWNLINK;
      context.rntiType  = PCAP_SI_RNTI;
      context.rnti      = to_value(dl_alloc.pdsch_cfg.rnti);
      context.system_frame_number = sl_tx.sfn();
      context.sub_frame_number    = sl_tx.subframe_index();
      context.length              = sib1_pdu.pdu.size();
      pcap.push_pdu(context, sib1_pdu.pdu);
      sib1_pcap_dumped = true;
    }
  }

  for (unsigned i = 0; i < dl_res.rar_pdus.size(); ++i) {
    const mac_dl_data_result::dl_pdu& rar_pdu  = dl_res.rar_pdus[i];
    const rar_information&            dl_alloc = sl_res.dl.rar_grants[i];
    srsran::mac_nr_context_info       context  = {};
    context.radioType           = cell_cfg.sched_req.tdd_ul_dl_cfg_common.has_value() ? PCAP_TDD_RADIO : PCAP_FDD_RADIO;
    context.direction           = PCAP_DIRECTION_DOWNLINK;
    context.rntiType            = PCAP_RA_RNTI;
    context.rnti                = to_value(dl_alloc.pdsch_cfg.rnti);
    context.system_frame_number = sl_tx.sfn();
    context.sub_frame_number    = sl_tx.subframe_index();
    context.length              = rar_pdu.pdu.size();
    pcap.push_pdu(context, rar_pdu.pdu);
  }
  for (unsigned i = 0; i < dl_res.paging_pdus.size(); ++i) {
    const mac_dl_data_result::dl_pdu& pg_pdu   = dl_res.paging_pdus[i];
    const dl_paging_allocation&       dl_alloc = sl_res.dl.paging_grants[i];
    srsran::mac_nr_context_info       context  = {};
    context.radioType           = cell_cfg.sched_req.tdd_ul_dl_cfg_common.has_value() ? PCAP_TDD_RADIO : PCAP_FDD_RADIO;
    context.direction           = PCAP_DIRECTION_DOWNLINK;
    context.rntiType            = PCAP_P_RNTI;
    context.rnti                = to_value(dl_alloc.pdsch_cfg.rnti);
    context.system_frame_number = sl_tx.sfn();
    context.sub_frame_number    = sl_tx.subframe_index();
    context.length              = pg_pdu.pdu.size();
    pcap.push_pdu(context, pg_pdu.pdu);
  }
  for (unsigned i = 0; i < dl_res.ue_pdus.size(); ++i) {
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
      context.length              = ue_pdu.pdu.size();
      pcap.push_pdu(context, ue_pdu.pdu);
    }
  }
}
