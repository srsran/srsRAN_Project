
#include "pdu_rx_handler.h"

using namespace srsgnb;

bool pdu_rx_handler::handle_rx_pdu(slot_point sl_rx, du_cell_index_t cell_index, mac_rx_pdu pdu)
{
  decoded_mac_rx_pdu ctx{sl_rx, cell_index, std::move(pdu)};

  // 1. Decode MAC UL PDU.
  if (ctx.decoded_subpdus.unpack(ctx.pdu_rx.pdu) < 0) {
    logger.warning("Failed to decode PDU");
    return false;
  }

  // 2. Log MAC UL PDU.
  if (logger.info.enabled()) {
    log_ul_pdu(logger, MAX_NOF_UES, ctx.pdu_rx.rnti, ctx.cell_index_rx, "PUSCH", "Content: [{}]", ctx.decoded_subpdus);
  }

  // 3. Check if MAC CRNTI CE is present.
  for (unsigned n = ctx.decoded_subpdus.nof_subpdus(); n > 0; --n) {
    const mac_ul_sch_subpdu& subpdu = ctx.decoded_subpdus.subpdu(n - 1);

    if (subpdu.lcid() == lcid_ul_sch_t::CRNTI) {
      // 4. Dispatch continuation of subPDU handling to execution context of previous C-RNTI.
      return handle_mac_ce(ctx, subpdu);
    }
  }

  // 4. Handle remaining MAC UL subPDUs.
  return handle_rx_subpdus(ctx);
}

bool pdu_rx_handler::push_ul_ccch_msg(rnti_t rnti)
{
  if (msg3_buffer[rnti].decoded_subpdus.nof_subpdus() == 0) {
    logger.warning("Received signal to push UL CCCH message for inexistent RNTI=0x{:x}", rnti);
    return false;
  }

  mac_ul_ue* ue = ue_manager.find_rnti(rnti);
  if (ue == nullptr) {
    logger.warning("Received UL CCCH for inexistent RNTI=0x{:x}", rnti);
    return false;
  }

  lcid_t lcid = 0;
  if (not ue->ul_bearers.contains(lcid)) {
    logger.warning("{}: Received UL PDU for inexistent bearer.", ue_event_prefix{}.set_rnti(ue->rnti).set_lcid(lcid));
    return false;
  }

  mac_ul_sch_subpdu& sdu = msg3_buffer[rnti].decoded_subpdus.subpdu(0);
  log_ul_pdu(
      logger, MAX_NOF_UES, ue->rnti, msg3_buffer[rnti].cell_index_rx, "CCCH", "Pushing {} bytes", sdu.sdu_length());

  // Push CCCH message to upper layers
  ue->ul_bearers[lcid]->on_new_sdu(mac_rx_sdu{ue->rnti, lcid, byte_buffer{sdu.payload().begin(), sdu.payload().end()}});
  return true;
}

bool pdu_rx_handler::handle_rx_subpdus(decoded_mac_rx_pdu& ctx)
{
  mac_ul_ue* ue = ue_manager.find_rnti(ctx.pdu_rx.rnti);

  // Process SDUs and MAC CEs that are not C-RNTI MAC CE
  for (const mac_ul_sch_subpdu& subpdu : ctx.decoded_subpdus) {
    bool ret = subpdu.lcid().is_sdu() ? handle_sdu(ctx, subpdu, ue) : handle_mac_ce(ctx, subpdu);
    if (not ret) {
      return false;
    }
  }

  return true;
}

bool pdu_rx_handler::handle_sdu(const decoded_mac_rx_pdu& ctx, const mac_ul_sch_subpdu& sdu, mac_ul_ue* ue)
{
  if (ue == nullptr) {
    logger.warning("{}: Received MAC SDU for inexistent RNTI.", ue_event_prefix{}.set_rnti(ue->rnti));
    return false;
  }

  lcid_t lcid = (lcid_t)sdu.lcid();
  if (not ue->ul_bearers.contains(lcid)) {
    logger.warning("{}: Received UL PDU for inexistent bearer.", ue_event_prefix{}.set_rnti(ue->rnti).set_lcid(lcid));
    return false;
  }

  // Log MAC UL SDU
  if (lcid == 0) {
    log_ul_pdu(logger, MAX_NOF_UES, ue->rnti, ctx.cell_index_rx, "CCCH", "Pushing {} bytes", sdu.sdu_length());
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
  ue->ul_bearers[lcid]->on_new_sdu(mac_rx_sdu{ue->rnti, lcid, byte_buffer{sdu.payload().begin(), sdu.payload().end()}});
  return true;
}

bool pdu_rx_handler::handle_mac_ce(decoded_mac_rx_pdu& ctx, const mac_ul_sch_subpdu& subpdu)
{
  // Handle MAC CEs
  switch ((lcid_t)subpdu.lcid()) {
    case lcid_ul_sch_t::CCCH_SIZE_48:
    case lcid_ul_sch_t::CCCH_SIZE_64:
      return handle_ccch_msg(ctx, subpdu);
    case lcid_ul_sch_t::CRNTI:
      return handle_crnti_ce(ctx, subpdu);
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
      ul_bsr_ind.rnti       = ctx.pdu_rx.rnti;
      ul_bsr_ind.type       = bsr_fmt;
      if (sched_bsr.nof_bytes == 0) {
        // Assume all LCGs are 0 if reported SBSR is 0
        for (lcg_id_t j = 0; j <= MAX_LOGICAL_CHANNEL_GROUP; j++) {
          ul_bsr_ind.reported_lcgs.push_back(ul_bsr_lcg_report{j, 0U});
        }
      } else {
        ul_bsr_ind.reported_lcgs.push_back(sched_bsr);
      }
      sched.ul_bsr(ul_bsr_ind);
    } break;
    case lcid_ul_sch_t::LONG_BSR:
    case lcid_ul_sch_t::LONG_TRUNC_BSR: {
      // Decode Long BSR
      bsr_format bsr_fmt = subpdu.lcid() == lcid_ul_sch_t::LONG_BSR ? bsr_format::LONG_BSR : bsr_format::LONG_TRUNC_BSR;
      long_bsr_report lbsr_ce = decode_lbsr(bsr_fmt, subpdu.payload());

      // Send UL BSR indication to Scheduler
      ul_bsr_indication_message ul_bsr_ind{};
      ul_bsr_ind.cell_index = ctx.cell_index_rx;
      ul_bsr_ind.rnti       = ctx.pdu_rx.rnti;
      ul_bsr_ind.type       = bsr_fmt;
      for (const lcg_bsr_report& lb : lbsr_ce.list) {
        ul_bsr_ind.reported_lcgs.push_back(make_sched_lcg_report(lb, bsr_fmt));
      }
      sched.ul_bsr(ul_bsr_ind);
    } break;
    case lcid_ul_sch_t::PADDING:
      break;
    default:
      logger.warning("Unhandled subPDU with LCID={}", subpdu.lcid());
  }

  return true;
}

bool pdu_rx_handler::handle_ccch_msg(decoded_mac_rx_pdu& ctx, const mac_ul_sch_subpdu& sdu)
{
  // Store Msg3 content for ConRes CE.
  msg3_buffer[ctx.pdu_rx.rnti] = std::move(ctx);

  // Notify DU manager of received CCCH message.
  ul_ccch_indication_message msg{};
  msg.crnti      = ctx.pdu_rx.rnti;
  msg.cell_index = ctx.cell_index_rx;
  cfg.event_notifier.on_ul_ccch_msg_received(msg);

  return true;
}

bool pdu_rx_handler::handle_crnti_ce(decoded_mac_rx_pdu& ctx, const mac_ul_sch_subpdu& subpdu)
{
  // 1. Decode CRNTI CE and update UE RNTI output parameter.
  ctx.pdu_rx.rnti = decode_crnti_ce(subpdu.payload());
  if (ctx.pdu_rx.rnti == INVALID_RNTI) {
    logger.error("Invalid Payload length={} for C-RNTI MAC CE", subpdu.sdu_length());
    return false;
  }

  // 2. Dispatch continuation of subPDU handling to execution context of previous C-RNTI.
  cfg.ul_exec_mapper.executor(ctx.pdu_rx.rnti).execute([this, ctx = std::move(ctx)]() mutable {
    rnti_t rnti = ctx.pdu_rx.rnti;

    // 3. Handle remaining subPDUs using old C-RNTI.
    handle_rx_subpdus(ctx);

    // 4. Scheduler should provide UL grant regardless of other BSR content for UE to complete RA.
    sched.ul_sr_info(rnti);
  });

  return true;
}
