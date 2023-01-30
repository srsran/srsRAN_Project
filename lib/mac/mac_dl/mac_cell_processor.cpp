/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_cell_processor.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/support/async/execute_on.h"

using namespace srsgnb;

/// Maximum PDSH K0 value as per TS38.331 "PDSCH-TimeDomainResourceAllocation".
constexpr size_t MAX_K0_DELAY = 32;

mac_cell_processor::mac_cell_processor(const mac_cell_creation_request& cell_cfg_req_,
                                       mac_scheduler&                   sched_,
                                       mac_dl_ue_manager&               ue_mng_,
                                       mac_cell_result_notifier&        phy_notifier_,
                                       task_executor&                   cell_exec_,
                                       task_executor&                   slot_exec_,
                                       task_executor&                   ctrl_exec_) :
  logger(srslog::fetch_basic_logger("MAC")),
  cell_cfg(cell_cfg_req_),
  cell_exec(cell_exec_),
  slot_exec(slot_exec_),
  ctrl_exec(ctrl_exec_),
  phy_cell(phy_notifier_),
  // The PDU pool has to be large enough to fit the maximum number of PDUs per slot for all possible K0 values.
  pdu_pool(dl_sch_pdu::MAX_PDU_LENGTH * MAX_DL_PDUS_PER_SLOT,
           MAX_K0_DELAY,
           get_nof_slots_per_subframe(cell_cfg.scs_common) * NOF_SFNS * NOF_SUBFRAMES_PER_FRAME),
  ssb_helper(cell_cfg_req_),
  sib_assembler(cell_cfg_req_.bcch_dl_sch_payload),
  rar_assembler(pdu_pool),
  dlsch_assembler(ue_mng_, pdu_pool),
  sched_obj(sched_),
  ue_mng(ue_mng_)
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
  // Change execution context to slot indication executor.
  slot_exec.execute([this, sl_tx]() { handle_slot_indication_impl(sl_tx); });
}

void mac_cell_processor::handle_crc(const mac_crc_indication_message& msg)
{
  ul_crc_indication ind{};
  ind.cell_index = cell_cfg.cell_index;
  ind.crcs.resize(msg.crcs.size());
  for (unsigned i = 0; i != ind.crcs.size(); ++i) {
    // Note: UE index is invalid for Msg3 CRCs because no UE has been allocated yet.
    ind.crcs[i].rnti           = msg.crcs[i].rnti;
    ind.crcs[i].ue_index       = ue_mng.get_ue_index(msg.crcs[i].rnti);
    ind.crcs[i].harq_id        = to_harq_id(msg.crcs[i].harq_id);
    ind.crcs[i].tb_crc_success = msg.crcs[i].tb_crc_success;
    ind.crcs[i].ul_sinr_metric = msg.crcs[i].ul_sinr_metric;
  }
  sched_obj.handle_crc_indication(ind);
}

void mac_cell_processor::handle_uci(const mac_uci_indication_message& msg)
{
  uci_indication ind{};
  ind.slot_rx    = msg.sl_rx;
  ind.cell_index = cell_cfg.cell_index;
  ind.ucis.resize(msg.ucis.size());
  for (unsigned i = 0; i != msg.ucis.size(); ++i) {
    ind.ucis[i].ue_index = ue_mng.get_ue_index(msg.ucis[i].rnti);
    ind.ucis[i].crnti    = msg.ucis[i].rnti;

    switch (msg.ucis[i].type) {
      case mac_uci_pdu::pdu_type::pucch_f0_or_f1: {
        const auto& pucch = msg.ucis[i].pucch_f0_or_f1;

        uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu pdu{};
        pdu.sr_detected = false;
        if (pucch.sr_info.has_value()) {
          pdu.sr_detected = pucch.sr_info.value().sr_detected;
        }
        if (pucch.harq_info.has_value()) {
          // NOTES:
          // - We report to the scheduler only the UCI HARQ-ACKs that contain either an ACK or NACK; we ignore the
          // UCIc with DTX. In that case, the scheduler will not receive the notification and the HARQ will eventually
          // retransmit the packet.
          // - This is to handle the case of simultaneous SR + HARQ UCI, for which we receive 2 UCI PDUs from the PHY,
          // 1 for SR + HARQ, 1 for HARQ only; note that only the SR + HARQ UCI is filled by the UE, meaning that we
          // expect the received HARQ-only UCI to be DTX. If reported to the scheduler, the UCI with HARQ-ACK only would
          // be erroneously treated as a NACK (as the scheduler only accepts ACK or NACK).

          // NOTE: There is a potential error that need to be handled below, which occurs when there's the 2-bit report
          // {DTX, (N)ACK}; if this were reported, we would skip the first bit (i.e. DTX) and report the second (i.e.
          // (N)ACK). Since in the scheduler the HARQ-ACK bits for a given UCI are processed in sequence, the
          // notification of the second bit of {DTX, (N)ACK} would be seen by the scheduler as the first bit of the
          // expected 2-bit reporting. To prevent this, we assume that PUCCH Format 0 or 1 UCI is valid if none of the 1
          // or 2 bits report is DTX (not detected).

          const auto& harq_pdus = pucch.harq_info.value().harqs;
          bool        is_valid_harq_ack =
              std::find_if(harq_pdus.begin(), harq_pdus.end(), [](uci_pucch_f0_or_f1_harq_values harq_ack_value) {
                return harq_ack_value == uci_pucch_f0_or_f1_harq_values::dtx;
              }) == harq_pdus.end();

          if (is_valid_harq_ack) {
            pdu.harqs.resize(harq_pdus.size());
            for (unsigned j = 0; j != pdu.harqs.size(); ++j) {
              pdu.harqs[j] = harq_pdus[j] == uci_pucch_f0_or_f1_harq_values::ack;
            }
          }
        }
        ind.ucis[i].pdu.emplace<uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu>(pdu);
      } break;
      case mac_uci_pdu::pdu_type::pusch: {
        const auto&                            pusch = msg.ucis[i].pusch;
        uci_indication::uci_pdu::uci_pusch_pdu pdu{};
        if (pusch.harq_info.value().harq_status == uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass) {
          pdu.harqs = pusch.harq_info->payload;
        }
        ind.ucis[i].pdu.emplace<uci_indication::uci_pdu::uci_pusch_pdu>(pdu);
      } break;
      default:
        report_fatal_error("Unsupported PUCCH format");
    }
  }

  sched_obj.handle_uci_indication(ind);
}

void mac_cell_processor::handle_slot_indication_impl(slot_point sl_tx)
{
  logger.set_context(sl_tx.sfn(), sl_tx.slot_index());

  // * Start of Critical Path * //

  mac_dl_sched_result mac_dl_res{};

  // Cleans old MAC DL PDU buffers.
  pdu_pool.tick(sl_tx.to_uint());

  if (state != cell_state::active) {
    phy_cell.on_new_downlink_scheduler_results(mac_dl_res);
    return;
  }

  // Generate DL scheduling result for provided slot and cell.
  const sched_result* sl_res = sched_obj.slot_indication(sl_tx, cell_cfg.cell_index);
  if (sl_res == nullptr) {
    logger.warning("Unable to compute scheduling result for slot={}, cell={}", sl_tx, cell_cfg.cell_index);
    phy_cell.on_new_downlink_scheduler_results(mac_dl_res);
    return;
  }

  // Assemble MAC DL scheduling request that is going to be passed to the PHY.
  assemble_dl_sched_request(mac_dl_res, sl_tx, cell_cfg.cell_index, sl_res->dl);

  // Send DL sched result to PHY.
  phy_cell.on_new_downlink_scheduler_results(mac_dl_res);

  // Start assembling Slot Data Result.
  if (not sl_res->dl.ue_grants.empty() or not sl_res->dl.rar_grants.empty() or not sl_res->dl.bc.sibs.empty()) {
    mac_dl_data_result data_res;
    assemble_dl_data_request(data_res, sl_tx, cell_cfg.cell_index, sl_res->dl);

    // Send DL Data to PHY.
    phy_cell.on_new_downlink_data(data_res);
  }

  // Send UL sched result to PHY.
  mac_ul_sched_result mac_ul_res{};
  mac_ul_res.slot   = sl_tx;
  mac_ul_res.ul_res = &sl_res->ul;
  phy_cell.on_new_uplink_scheduler_results(mac_ul_res);

  // All results have been notified at this point.
  phy_cell.on_cell_results_completion(sl_tx);

  // * End of Critical Path * //

  // Update DL buffer state for the allocated logical channels.
  update_logical_channel_dl_buffer_states(sl_res->dl);
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
    default:
      srsgnb_terminate("Invalid DCI format");
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
    default:
      srsgnb_terminate("Invalid DCI format");
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
    srsgnb_assert(not data_res.sib1_pdus.full(), "No SIB1 added as SIB1 list in MAC DL data results is already full");
    span<const uint8_t> payload = sib_assembler.encode_sib_pdu(sib_info.pdsch_cfg.codewords[0].tb_size_bytes);
    data_res.sib1_pdus.emplace_back(0, payload);
  }

  // Assemble scheduled RARs' subheaders and payloads.
  for (const rar_information& rar : dl_res.rar_grants) {
    data_res.rar_pdus.emplace_back(0, rar_assembler.encode_rar_pdu(rar));
  }

  // TODO: Assemble paging payload for paging grants.

  // Assemble data grants.
  for (const dl_msg_alloc& grant : dl_res.ue_grants) {
    for (unsigned tb_idx = 0; tb_idx != grant.tb_list.size(); ++tb_idx) {
      const dl_msg_tb_info& tb_info = grant.tb_list[tb_idx];
      const pdsch_codeword& cw      = grant.pdsch_cfg.codewords[tb_idx];
      data_res.ue_pdus.emplace_back(tb_idx,
                                    dlsch_assembler.assemble_pdu(grant.pdsch_cfg.rnti, tb_info, cw.tb_size_bytes));
    }
  }
}

void mac_cell_processor::update_logical_channel_dl_buffer_states(const dl_sched_result& dl_res)
{
  for (const dl_msg_alloc& grant : dl_res.ue_grants) {
    for (const dl_msg_tb_info& tb_info : grant.tb_list) {
      for (const dl_msg_lc_info& lc_info : tb_info.lc_chs_to_sched) {
        if (not lc_info.lcid.is_sdu()) {
          continue;
        }

        // Fetch RLC Bearer.
        mac_sdu_tx_builder* bearer = ue_mng.get_bearer(grant.pdsch_cfg.rnti, lc_info.lcid.to_lcid());
        srsgnb_sanity_check(bearer != nullptr, "Scheduler is allocating inexistent bearers");

        // Update DL buffer state for the allocated logical channel.
        dl_buffer_state_indication_message bs{};
        bs.ue_index = ue_mng.get_ue_index(grant.pdsch_cfg.rnti);
        bs.lcid     = lc_info.lcid.to_lcid();
        bs.bs       = bearer->on_buffer_state_update();
        sched_obj.handle_dl_buffer_state_indication(bs);

        // Restart UE activity timer.
        ue_mng.restart_activity_timer(bs.ue_index);
      }
    }
  }
}
