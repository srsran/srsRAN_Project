/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdu_rx_handler.h"

using namespace srsgnb;

pdu_rx_handler::pdu_rx_handler(mac_common_config_t&        cfg_,
                               scheduler_feedback_handler& sched_,
                               mac_ul_ue_manager&          ue_manager_,
                               du_rnti_table&              rnti_table_) :
  cfg(cfg_), logger(cfg.logger), sched(sched_), ue_manager(ue_manager_), rnti_table(rnti_table_)
{
}

bool pdu_rx_handler::handle_rx_pdu(slot_point sl_rx, du_cell_index_t cell_index, mac_rx_pdu pdu)
{
  // 1. Fetch UE index based on PDU RNTI.
  du_ue_index_t ue_index = rnti_table[pdu.rnti];

  // 2. Decode MAC UL PDU.
  decoded_mac_rx_pdu ctx{sl_rx, cell_index, std::move(pdu), ue_index};
  if (not ctx.decoded_subpdus.unpack(ctx.pdu_rx.pdu)) {
    logger.warning("Failed to decode PDU");
    return false;
  }

  // 3. Log MAC UL PDU.
  log_ul_pdu(logger, ctx.ue_index, ctx.pdu_rx.rnti, ctx.cell_index_rx, "PUSCH", "Content: [{}]", ctx.decoded_subpdus);

  // 4. Check if MAC CRNTI CE is present.
  for (unsigned n = ctx.decoded_subpdus.nof_subpdus(); n > 0; --n) {
    const mac_ul_sch_subpdu& subpdu = ctx.decoded_subpdus.subpdu(n - 1);

    if (subpdu.lcid() == lcid_ul_sch_t::CRNTI) {
      // 4. Dispatch continuation of subPDU handling to execution context of previous C-RNTI.
      return handle_crnti_ce(ctx, subpdu);
    }
  }

  // 5. Handle remaining MAC UL subPDUs.
  return handle_rx_subpdus(ctx);
}

bool pdu_rx_handler::push_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer ul_ccch_msg)
{
  mac_ul_ue_context* ue = ue_manager.find_ue(ue_index);
  if (ue == nullptr) {
    logger.warning("Received UL CCCH for inexistent ueId={}", ue_index);
    return false;
  }

  if (not ue->ul_bearers.contains(LCID_SRB0)) {
    logger.warning("{}: Received UL PDU for inexistent bearer.", ue_event_prefix{} | ue->rnti | LCID_SRB0);
    return false;
  }

  log_ul_pdu(logger, ue->ue_index, ue->rnti, MAX_NOF_DU_CELLS, "CCCH", "Pushing {} bytes", ul_ccch_msg.length());

  // Push CCCH message to upper layers.
  ue->ul_bearers[LCID_SRB0]->on_new_sdu(std::move(ul_ccch_msg));
  return true;
}

bool pdu_rx_handler::handle_rx_subpdus(decoded_mac_rx_pdu& ctx)
{
  mac_ul_ue_context* ue = ue_manager.find_ue(ctx.ue_index);

  // Process SDUs and MAC CEs that are not C-RNTI MAC CE
  for (const mac_ul_sch_subpdu& subpdu : ctx.decoded_subpdus) {
    bool ret = subpdu.lcid().is_sdu() ? handle_sdu(ctx, subpdu, ue) : handle_mac_ce(ctx, subpdu);
    if (not ret) {
      return false;
    }
  }

  return true;
}

bool pdu_rx_handler::handle_sdu(const decoded_mac_rx_pdu& ctx, const mac_ul_sch_subpdu& sdu, mac_ul_ue_context* ue)
{
  if (ue == nullptr) {
    logger.warning("Received MAC SDU for inexistent RNTI.");
    return false;
  }

  lcid_t lcid = (lcid_t)sdu.lcid().value();
  if (not ue->ul_bearers.contains(lcid)) {
    logger.warning("{}: Received UL PDU for inexistent bearer.", ue_event_prefix{} | ue->rnti | lcid);
    return false;
  }

  // Log MAC UL SDU
  if (lcid == 0) {
    log_ul_pdu(logger, MAX_NOF_DU_UES, ue->rnti, ctx.cell_index_rx, "CCCH", "Pushing {} bytes", sdu.sdu_length());
  } else {
    log_ul_pdu(logger,
               ue->ue_index,
               ue->rnti,
               ctx.cell_index_rx,
               "DCCH",
               "Pushing {} bytes to LCID={}",
               sdu.sdu_length(),
               lcid);
  }

  // Push PDU to upper layers
  ue->ul_bearers[lcid]->on_new_sdu(byte_buffer{sdu.payload().begin(), sdu.payload().end()});
  return true;
}

bool pdu_rx_handler::handle_mac_ce(decoded_mac_rx_pdu& ctx, const mac_ul_sch_subpdu& subpdu)
{
  // Handle MAC CEs
  switch (subpdu.lcid().value()) {
    case lcid_ul_sch_t::CCCH_SIZE_48:
    case lcid_ul_sch_t::CCCH_SIZE_64:
      return handle_ccch_msg(ctx, subpdu);
    case lcid_ul_sch_t::SHORT_BSR:
    case lcid_ul_sch_t::SHORT_TRUNC_BSR: {
      // Decode Short BSR
      bsr_format bsr_fmt =
          subpdu.lcid() == lcid_ul_sch_t::SHORT_BSR ? bsr_format::SHORT_BSR : bsr_format::SHORT_TRUNC_BSR;
      lcg_bsr_report    sbsr_ce   = decode_sbsr(subpdu.payload());
      ul_bsr_lcg_report sched_bsr = make_sched_lcg_report(sbsr_ce, bsr_fmt);

      // Send UL BSR indication to Scheduler
      ul_bsr_indication_message ul_bsr_ind{};
      ul_bsr_ind.cell_index = ctx.cell_index_rx;
      ul_bsr_ind.ue_index   = ctx.ue_index;
      ul_bsr_ind.crnti      = ctx.pdu_rx.rnti;
      ul_bsr_ind.type       = bsr_fmt;
      if (sched_bsr.nof_bytes == 0) {
        // Assume all LCGs are 0 if reported SBSR is 0
        for (lcg_id_t j = 0; j <= MAX_LOGICAL_CHANNEL_GROUP; j++) {
          ul_bsr_ind.reported_lcgs.push_back(ul_bsr_lcg_report{j, 0U});
        }
      } else {
        ul_bsr_ind.reported_lcgs.push_back(sched_bsr);
      }
      sched.handle_ul_bsr_indication(ul_bsr_ind);
    } break;
    case lcid_ul_sch_t::LONG_BSR:
    case lcid_ul_sch_t::LONG_TRUNC_BSR: {
      // Decode Long BSR
      bsr_format bsr_fmt = subpdu.lcid() == lcid_ul_sch_t::LONG_BSR ? bsr_format::LONG_BSR : bsr_format::LONG_TRUNC_BSR;
      long_bsr_report lbsr_ce = decode_lbsr(bsr_fmt, subpdu.payload());

      // Send UL BSR indication to Scheduler
      ul_bsr_indication_message ul_bsr_ind{};
      ul_bsr_ind.cell_index = ctx.cell_index_rx;
      ul_bsr_ind.ue_index   = ctx.ue_index;
      ul_bsr_ind.crnti      = ctx.pdu_rx.rnti;
      ul_bsr_ind.type       = bsr_fmt;
      for (const lcg_bsr_report& lb : lbsr_ce.list) {
        ul_bsr_ind.reported_lcgs.push_back(make_sched_lcg_report(lb, bsr_fmt));
      }
      sched.handle_ul_bsr_indication(ul_bsr_ind);
    } break;
    case lcid_ul_sch_t::CRNTI:
      // The MAC CE C-RNTI is handled separately and, among all the MAC CEs, it should be the first one being processed.
      // After the MAC C-RNTI is processed, this function is invoked for all subPDUs (including the MAC C-RNTI itself).
      // Therefore, to avoid logging a warning for MAC C-RNTI, we added the case lcid_ul_sch_t::CRNTI below.
    case lcid_ul_sch_t::PADDING:
      break;
    default:
      logger.warning("Unhandled subPDU with LCID={}", subpdu.lcid());
  }

  return true;
}

bool pdu_rx_handler::handle_ccch_msg(decoded_mac_rx_pdu& ctx, const mac_ul_sch_subpdu& sdu)
{
  // Notify DU manager of received CCCH message.
  ul_ccch_indication_message msg{};
  msg.crnti      = ctx.pdu_rx.rnti;
  msg.cell_index = ctx.cell_index_rx;
  msg.slot_rx    = ctx.slot_rx;
  msg.subpdu.append(sdu.payload());
  cfg.event_notifier.on_ul_ccch_msg_received(msg);

  return true;
}

bool pdu_rx_handler::handle_crnti_ce(decoded_mac_rx_pdu& ctx, const mac_ul_sch_subpdu& subpdu)
{
  // 1. Decode CRNTI CE and update UE RNTI output parameter.
  ctx.pdu_rx.rnti = decode_crnti_ce(subpdu.payload());
  if (ctx.pdu_rx.rnti == INVALID_RNTI) {
    logger.error("Invalid Payload length={} for C-RNTI MAC CE.", subpdu.sdu_length());
    return false;
  }
  ctx.ue_index = rnti_table[ctx.pdu_rx.rnti];

  // 2. Dispatch continuation of subPDU handling to execution context of previous C-RNTI.
  cfg.ul_exec_mapper.executor(ctx.ue_index).execute([this, ctx = std::move(ctx)]() mutable {
    // 3. Handle remaining subPDUs using old C-RNTI.
    handle_rx_subpdus(ctx);

    // 4. Scheduler should provide UL grant regardless of other BSR content for UE to complete RA.
    sr_indication_message sr{};
    sr.cell_index = ctx.cell_index_rx;
    sr.ue_index   = ctx.ue_index;
    sr.crnti      = ctx.pdu_rx.rnti;
    sr.sr_payload.resize(1);
    sr.sr_payload.set(0);
    sched.handle_sr_indication(sr);
  });

  return true;
}
