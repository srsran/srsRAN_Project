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
#include "../mac_ul/ul_bsr.h"
#include "srsran/du_high/rnti_value_table.h"
#include "srsran/scheduler/scheduler_factory.h"

using namespace srsran;

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
  for (unsigned i = 0; i != MAX_NOF_DU_CELLS; ++i) {
    cell_handlers[i] = cell_handler{to_du_cell_index(i), *this};
  }
}

void srsran_scheduler_adapter::add_cell(const mac_cell_creation_request& msg)
{
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
  // Convert UL scheduling command to Scheduler UCI indication with a SR included.
  uci_indication uci{};
  uci.cell_index = cmd.cell_index;
  uci.slot_rx    = cmd.sl_rx;
  uci.ucis.resize(1);
  uci.ucis[0].ue_index = cmd.ue_index;
  uci.ucis[0].crnti    = cmd.rnti;
  uci.ucis[0].pdu      = uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu{.sr_detected = true};

  // Send SR indication to Scheduler.
  sched_impl->handle_uci_indication(uci);
}

static auto convert_mac_harq_bits_to_sched_harq_values(uci_pusch_or_pucch_f2_3_4_detection_status harq_status,
                                                       const bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS>& payload)
{
  bool crc_pass = harq_status == srsran::uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass;
  static_vector<mac_harq_ack_report_status, uci_constants::MAX_NOF_HARQ_BITS> ret(
      payload.size(), crc_pass ? mac_harq_ack_report_status::nack : mac_harq_ack_report_status::dtx);
  if (crc_pass) {
    for (unsigned i = 0, e = ret.size(); i != e; ++i) {
      if (payload.test(i)) {
        ret[i] = srsran::mac_harq_ack_report_status::ack;
      }
    }
  }
  return ret;
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

const sched_result& srsran_scheduler_adapter::slot_indication(slot_point slot_tx, du_cell_index_t cell_idx)
{
  return sched_impl->slot_indication(slot_tx, cell_idx);
}

void srsran_scheduler_adapter::sched_config_notif_adapter::on_ue_config_complete(du_ue_index_t ue_index)
{
  srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid ue index={}", ue_index);

  // Remove continuation of task in ctrl executor.
  if (not parent.ctrl_exec.defer(
          [this, ue_index]() { parent.sched_cfg_notif_map[ue_index].ue_config_ready.set(true); })) {
    parent.logger.error("Unable to finish configuration of ue={}. Cause: DU task queue is full.", ue_index);
  }
}

void srsran_scheduler_adapter::sched_config_notif_adapter::on_ue_delete_response(du_ue_index_t ue_index)
{
  srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid ue index={}", ue_index);

  // Continuation of ue remove task dispatched to the ctrl executor.
  if (not parent.ctrl_exec.defer(
          [this, ue_index]() { parent.sched_cfg_notif_map[ue_index].ue_config_ready.set(true); })) {
    parent.logger.error("Unable to remove ue={}. Cause: DU task queue is full.", ue_index);
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
  for (unsigned i = 0; i != ind.crcs.size(); ++i) {
    // Note: UE index is invalid for Msg3 CRCs because no UE has been allocated yet.
    ul_crc_pdu_indication& pdu = ind.crcs.emplace_back();
    pdu.rnti                   = msg.crcs[i].rnti;
    pdu.ue_index               = parent->rnti_mng[msg.crcs[i].rnti];
    pdu.harq_id                = to_harq_id(msg.crcs[i].harq_id);
    pdu.tb_crc_success         = msg.crcs[i].tb_crc_success;
    pdu.ul_sinr_metric         = msg.crcs[i].ul_sinr_metric;

    // If not Msg3, report crc=OK/KO.
    if (pdu.ue_index != INVALID_DU_UE_INDEX) {
      parent->rlf_handler.handle_crc(pdu.ue_index, pdu.tb_crc_success);
    }
  }

  // Forward CRC indication to the scheduler.
  parent->sched_impl->handle_crc_indication(ind);
}

void srsran_scheduler_adapter::cell_handler::handle_uci(const mac_uci_indication_message& msg)
{
  // Convert MAC UCI indication to srsRAN scheduler UCI indication.
  uci_indication ind{};
  ind.slot_rx    = msg.sl_rx;
  ind.cell_index = cell_idx;
  for (unsigned i = 0; i != msg.ucis.size(); ++i) {
    uci_indication::uci_pdu& uci_pdu = ind.ucis.emplace_back();
    ind.ucis[i].crnti                = msg.ucis[i].rnti;
    ind.ucis[i].ue_index             = parent->rnti_mng[msg.ucis[i].rnti];
    if (ind.ucis[i].ue_index == INVALID_DU_UE_INDEX) {
      ind.ucis.pop_back();
      parent->logger.info("rnti={}: Discarding UCI PDU. Cause: The RNTI does not exist.", uci_pdu.crnti);
      continue;
    }

    if (variant_holds_alternative<mac_uci_pdu::pucch_f0_or_f1_type>(msg.ucis[i].pdu)) {
      const auto& pucch = variant_get<mac_uci_pdu::pucch_f0_or_f1_type>(msg.ucis[i].pdu);

      uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu pdu{};
      if (pucch.ul_sinr.has_value()) {
        pdu.ul_sinr.emplace(pucch.ul_sinr.value());
      }
      pdu.sr_detected = false;
      if (pucch.sr_info.has_value()) {
        pdu.sr_detected = pucch.sr_info.value().sr_detected;
      }
      if (pucch.harq_info.has_value()) {
        // NOTES:
        // - We report to the scheduler only the UCI HARQ-ACKs that contain either an ACK or NACK; we ignore the
        // UCIs with DTX. In that case, the scheduler will not receive the notification and the HARQ will eventually
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
        pdu.harqs.resize(harq_pdus.size());
        for (unsigned j = 0; j != pdu.harqs.size(); ++j) {
          switch (harq_pdus[j]) {
            case uci_pucch_f0_or_f1_harq_values::ack:
              pdu.harqs[j] = mac_harq_ack_report_status::ack;
              break;
            case uci_pucch_f0_or_f1_harq_values::nack:
              pdu.harqs[j] = mac_harq_ack_report_status::nack;
              break;
            default:
              pdu.harqs[j] = mac_harq_ack_report_status::dtx;
          }

          // Report ACK for RLF detection purposes.
          parent->rlf_handler.handle_ack(ind.ucis[i].ue_index, pdu.harqs[j] == mac_harq_ack_report_status::ack);
        }
      }
      ind.ucis[i].pdu.emplace<uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu>(pdu);
    } else if (variant_holds_alternative<mac_uci_pdu::pusch_type>(msg.ucis[i].pdu)) {
      const auto&                            pusch = variant_get<mac_uci_pdu::pusch_type>(msg.ucis[i].pdu);
      uci_indication::uci_pdu::uci_pusch_pdu pdu{};
      if (pusch.harq_info.has_value()) {
        pdu.harqs =
            convert_mac_harq_bits_to_sched_harq_values(pusch.harq_info.value().harq_status, pusch.harq_info->payload);

        // Report ACK for RLF detection purposes.
        for (unsigned j = 0; j != pdu.harqs.size(); ++j) {
          parent->rlf_handler.handle_ack(ind.ucis[i].ue_index, pdu.harqs[j] == mac_harq_ack_report_status::ack);
        }
      }
      if (pusch.csi_part1_info.has_value() and
          pusch.csi_part1_info.value().csi_status == uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass) {
        pdu.csi_part1 = pusch.csi_part1_info->payload;
      }
      if (pusch.csi_part2_info.has_value() and
          pusch.csi_part2_info.value().csi_status == uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass) {
        pdu.csi_part2 = pusch.csi_part2_info->payload;
      }
      ind.ucis[i].pdu.emplace<uci_indication::uci_pdu::uci_pusch_pdu>(pdu);
    } else if (variant_holds_alternative<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(msg.ucis[i].pdu)) {
      const auto& pucch = variant_get<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(msg.ucis[i].pdu);
      uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu pdu{};
      if (pucch.ul_sinr.has_value()) {
        pdu.ul_sinr.emplace(pucch.ul_sinr.value());
      }
      if (pucch.sr_info.has_value()) {
        pdu.sr_info = pucch.sr_info.value();
      }
      if (pucch.harq_info.has_value()) {
        pdu.harqs =
            convert_mac_harq_bits_to_sched_harq_values(pucch.harq_info.value().harq_status, pucch.harq_info->payload);

        // Report ACK for RLF detection purposes.
        for (unsigned j = 0; j != pdu.harqs.size(); ++j) {
          parent->rlf_handler.handle_ack(ind.ucis[i].ue_index, pdu.harqs[j] == mac_harq_ack_report_status::ack);
        }
      }
      if (pucch.uci_part1_or_csi_part1_info.has_value() and
          pucch.uci_part1_or_csi_part1_info.value().status == uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass) {
        pdu.csi_part1 = pucch.uci_part1_or_csi_part1_info->payload;
      }
      if (pucch.uci_part2_or_csi_part2_info.has_value() and
          pucch.uci_part2_or_csi_part2_info.value().status == uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass) {
        pdu.csi_part2 = pucch.uci_part2_or_csi_part2_info->payload;
      }
      ind.ucis[i].pdu.emplace<uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu>(pdu);
    }
  }

  // Forward UCI indication to the scheduler.
  parent->sched_impl->handle_uci_indication(ind);
}
