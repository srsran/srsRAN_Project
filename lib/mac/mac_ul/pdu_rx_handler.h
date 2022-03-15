
#ifndef SRSGNB_PDU_RX_HANDLER_H
#define SRSGNB_PDU_RX_HANDLER_H

#include "../../ran/gnb_format.h"
#include "mac_ul_sch_pdu.h"
#include "mac_ul_ue_manager.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/mac/sched_ue_feedback.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/slot_point.h"
#include "ul_bsr.h"

namespace srsgnb {

/// Stores any contextual or temporary information related to the decoding of a MAC RX PDU.
struct mac_rx_pdu_context {
  mac_rx_pdu_context() = default;
  mac_rx_pdu_context(slot_point slot_rx_, du_cell_index_t cell_idx_, mac_rx_pdu pdu_rx_) :
    slot_rx(slot_rx_), cell_index_rx(cell_idx_), pdu_rx(std::move(pdu_rx_))
  {
    srsran_sanity_check(not pdu_rx.pdu.empty(), "Received empty PDU");
  }

  /// Whether object is in a valid state.
  bool valid() const { return not pdu_rx.pdu.empty(); }

  /// Clear PDU context.
  void clear()
  {
    slot_rx       = {};
    cell_index_rx = MAX_NOF_CELLS;
    ce_crnti      = INVALID_RNTI;
    pdu_rx.pdu.clear();
    decoded_subpdus.clear();
  }

  /// C-RNTI for which subPDUs are directed at. It may be different from the RNTI in the PDU, depending on whether
  /// a CRNTI MAC CE is present.
  rnti_t crnti() const { return ce_crnti == INVALID_RNTI ? pdu_rx.rnti : ce_crnti; }

  /// Slot when PDU was received in the PHY.
  slot_point slot_rx;

  /// Cell where PDU was decoded by the PHY.
  du_cell_index_t cell_index_rx;

  /// C-RNTI in CRNTI MAC CE, if the CE is present.
  rnti_t ce_crnti = INVALID_RNTI;

  /// Received MAC PDU.
  mac_rx_pdu pdu_rx;

  /// View of decoded subPDUs of the MAC PDU.
  mac_ul_sch_pdu decoded_subpdus;
};

/// Decodes MAC Rx PDUs and handles the respective CEs and SDUs by dispatching them to the Scheduler or UE UL bearers.
/// This class is thread-safe. It can be concurrently called for multiple MAC UL PDUs. The intermediary decoding steps
/// are stored in the passed decoded_pdu_rx argument.
class pdu_rx_handler
{
public:
  pdu_rx_handler(srslog::basic_logger& logger_, sched_ue_feedback& sched_, mac_ul_sdu_notifier& ccch_notifier_) :
    logger(logger_), sched(sched_), ccch_notifier(ccch_notifier_)
  {}

  /// Decode MAC Rx PDU and log contents.
  /// \param [in/out] ctx Decoded PDU and other PDU-specific contextual information.
  /// \return true if decoded successfully.
  bool decode_rx_pdu(mac_rx_pdu_context& ctx)
  {
    // 1. Decode MAC UL PDU.
    if (ctx.decoded_subpdus.unpack(ctx.pdu_rx.pdu) < 0) {
      logger.warning("Failed to decode PDU");
      ctx.clear();
      return false;
    }

    // 2. Log MAC UL PDU.
    if (logger.info.enabled()) {
      log_ul_pdu(logger, MAX_NOF_UES, ctx.pdu_rx.rnti, ctx.cell_index_rx, "PUSCH", "Content: {}", ctx.decoded_subpdus);
    }

    // 3. Check if MAC CRNTI CE exists.
    for (unsigned n = ctx.decoded_subpdus.nof_subpdus(); n > 0; --n) {
      const mac_ul_sch_subpdu& subpdu = ctx.decoded_subpdus.subpdu(n - 1);
      if (subpdu.lcid() == lcid_ul_sch_t::CRNTI) {
        // Decode CRNTI CE and update UE RNTI output parameter.
        ctx.ce_crnti = decode_crnti_ce(subpdu.payload());
        break;
      }
    }
    return true;
  }

  bool handle_rx_subpdus(mac_ul_ue* ue, const mac_rx_pdu_context& ctx)
  {
    // Process SDUs and remaining MAC CEs
    for (const mac_ul_sch_subpdu& subpdu : ctx.decoded_subpdus) {
      if (subpdu.lcid().is_sdu()) {
        process_sdu(ue, ctx, subpdu);
      } else {
        process_ce_subpdu(ue, ctx, subpdu);
      }
    }

    return true;
  }

private:
  bool process_sdu(mac_ul_ue* ue, const mac_rx_pdu_context& ctx, const mac_ul_sch_subpdu& sdu)
  {
    if (ue == nullptr) {
      logger.warning("Received MAC SDU for inexistent RNTI=0x{:x}", ctx.crnti());
      return false;
    }

    lcid_t lcid = (lcid_t)sdu.lcid();
    if (not ue->ul_bearers.contains(lcid)) {
      logger.warning("Received UL PDU for inexistent bearer {{" FMT_RNTI ", {}}}", ue->rnti, lcid);
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
    ue->ul_bearers[lcid]->on_ul_sdu(mac_ul_sdu{ue->rnti, lcid, byte_buffer{}}); // TODO: Create SDU
    return true;
  }

  bool process_ce_subpdu(mac_ul_ue* ue, const mac_rx_pdu_context& ctx, const mac_ul_sch_subpdu& subpdu)
  {
    // Handle MAC CEs
    switch ((lcid_t)subpdu.lcid()) {
      case lcid_ul_sch_t::CCCH_SIZE_48:
      case lcid_ul_sch_t::CCCH_SIZE_64: {
        // Store Msg3 content for ConRes CE.
        // TODO

        // Notify upper layers with received SDU.
        ccch_notifier.on_ul_sdu(
            mac_ul_sdu{ctx.crnti(), 0, byte_buffer{subpdu.payload().begin(), subpdu.payload().end()}}); // TODO
      } break;
      case lcid_ul_sch_t::CRNTI: {
        // Note: C-RNTI MAC CE already decoded in decode_rx_pdu function.

        // provide UL grant regardless of other BSR content for UE to complete RA
        sched.ul_sr_info(ctx.crnti());
      } break;
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
        ul_bsr_ind.rnti       = ctx.crnti();
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
        bsr_format bsr_fmt =
            subpdu.lcid() == lcid_ul_sch_t::LONG_BSR ? bsr_format::LONG_BSR : bsr_format::LONG_TRUNC_BSR;
        long_bsr_report lbsr_ce = decode_lbsr(bsr_fmt, subpdu.payload());

        // Send UL BSR indication to Scheduler
        ul_bsr_indication_message ul_bsr_ind{};
        ul_bsr_ind.cell_index = ctx.cell_index_rx;
        ul_bsr_ind.rnti       = ctx.crnti();
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

  srslog::basic_logger& logger;
  sched_ue_feedback&    sched;
  mac_ul_sdu_notifier&  ccch_notifier;
};

} // namespace srsgnb

#endif // SRSGNB_PDU_RX_HANDLER_H
