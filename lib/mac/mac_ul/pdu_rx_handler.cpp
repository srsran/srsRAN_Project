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

#include "pdu_rx_handler.h"
#include "srsran/instrumentation/traces/up_traces.h"

using namespace srsran;

namespace {

/// \brief Structure used to provide a logging prefix for the UE PDU/subPDU.
struct pdu_log_prefix {
  const char*             type;
  rnti_t                  rnti;
  du_ue_index_t           ue_index;
  optional<lcid_ul_sch_t> lcid;
};

pdu_log_prefix create_prefix(const decoded_mac_rx_pdu& pdu)
{
  return pdu_log_prefix{.type = "UL", .rnti = pdu.pdu_rx.rnti, .ue_index = pdu.ue_index};
}

pdu_log_prefix create_prefix(const decoded_mac_rx_pdu& pdu, const mac_ul_sch_subpdu& subpdu)
{
  return pdu_log_prefix{.type = "UL", .rnti = pdu.pdu_rx.rnti, .ue_index = pdu.ue_index, .lcid = subpdu.lcid()};
}

} // namespace

template <>
struct fmt::formatter<pdu_log_prefix> : public basic_fmt_parser {
  template <typename FormatContext>
  auto format(const pdu_log_prefix& p, FormatContext& ctx)
  {
    fmt::format_to(ctx.out(), "{} rnti={}", p.type, p.rnti);
    if (p.ue_index != srsran::INVALID_DU_UE_INDEX) {
      fmt::format_to(ctx.out(), " ue={}", p.ue_index);
    }
    if (p.lcid.has_value()) {
      const char* event = p.lcid->is_sdu() ? p.lcid->is_ccch() ? "UL-CCCH" : "UL-DCCH" : "CE";
      fmt::format_to(ctx.out(), " lcid={:#x} {}", *p.lcid, event);
    }
    return ctx.out();
  }
};

pdu_rx_handler::pdu_rx_handler(mac_ul_ccch_notifier&          ccch_notifier_,
                               du_high_ue_executor_mapper&    ue_exec_mapper_,
                               mac_scheduler_ce_info_handler& sched_,
                               mac_ul_ue_manager&             ue_manager_,
                               du_rnti_table&                 rnti_table_,
                               mac_pcap&                      pcap_) :
  ccch_notifier(ccch_notifier_),
  ue_exec_mapper(ue_exec_mapper_),
  logger(srslog::fetch_basic_logger("MAC")),
  sched(sched_),
  ue_manager(ue_manager_),
  rnti_table(rnti_table_),
  pcap(pcap_)
{
}

bool pdu_rx_handler::handle_rx_pdu(slot_point sl_rx, du_cell_index_t cell_index, mac_rx_pdu pdu)
{
  trace_point rx_tp = up_tracer.now();
  // > Store PCAP
  write_pcap_rx_pdu(sl_rx, pdu);

  // > Fetch UE index based on PDU RNTI.
  du_ue_index_t ue_index = rnti_table[pdu.rnti];

  // > Decode MAC UL PDU.
  decoded_mac_rx_pdu      ctx{sl_rx, cell_index, std::move(pdu), ue_index};
  error_type<std::string> ret = ctx.decoded_subpdus.unpack(ctx.pdu_rx.pdu);
  if (ret.is_error()) {
    logger.info("{}: Failed to decode MAC UL PDU. Cause: {}", create_prefix(ctx), ret.error());
    return false;
  }

  // > Log MAC UL PDU.
  if (logger.info.enabled()) {
    // Note: Since subPDUs are just views, they should not be passed by value to the logging backend.
    fmt::memory_buffer fmtbuf;
    fmt::format_to(fmtbuf, "{}", ctx.decoded_subpdus);
    logger.info("{} subPDUs: [{}]", create_prefix(ctx), to_c_str(fmtbuf));
  }

  // > If Msg3 (UE index is still not assigned) is received, check if MAC CRNTI CE or UL-CCCH CE are present.
  if (not is_du_ue_index_valid(ctx.ue_index)) {
    for (unsigned n = ctx.decoded_subpdus.nof_subpdus(); n > 0; --n) {
      const mac_ul_sch_subpdu& subpdu = ctx.decoded_subpdus.subpdu(n - 1);

      if (subpdu.lcid() == lcid_ul_sch_t::CRNTI) {
        // >> Dispatch continuation of subPDU handling to execution context of previous C-RNTI.
        return handle_crnti_ce(ctx, subpdu);
      }
      if (subpdu.lcid() == lcid_ul_sch_t::CCCH_SIZE_48 or subpdu.lcid() == lcid_ul_sch_t::CCCH_SIZE_64) {
        return handle_ccch_msg(ctx, subpdu);
      }
    }
  }

  // > Handle remaining MAC UL subPDUs.
  bool pdu_ret = handle_rx_subpdus(ctx);
  up_tracer << trace_event{"mac_rx_pdu", rx_tp};
  return pdu_ret;
}

bool pdu_rx_handler::push_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer ul_ccch_msg)
{
  mac_ul_ue_context* ue = ue_manager.find_ue(ue_index);
  if (ue == nullptr) {
    logger.warning("UL subPDU ue={}, lcid={} UL-CCCH: Received UL-CCCH for non-existent UE", ue_index, LCID_SRB0);
    return false;
  }

  if (not ue->ul_bearers.contains(LCID_SRB0)) {
    logger.warning("{}: Received UL PDU for inexistent bearer.",
                   pdu_log_prefix{.type = "UL subPDU", .rnti = ue->rnti, .lcid = LCID_SRB0});
    return false;
  }

  logger.debug("{}: Forwarding UL SDU of {} bytes",
               pdu_log_prefix{.type = "UL subPDU", .rnti = ue->rnti, .ue_index = ue->ue_index, .lcid = LCID_SRB0},
               ul_ccch_msg.length());

  // Push CCCH message to upper layers.
  ue->ul_bearers[LCID_SRB0]->on_new_sdu(byte_buffer_slice{std::move(ul_ccch_msg)});

  // Notify the scheduler that a Contention Resolution CE needs to be scheduled.
  sched.handle_dl_mac_ce_indication(mac_ce_scheduling_command{ue_index, lcid_dl_sch_t::UE_CON_RES_ID});

  return true;
}

bool pdu_rx_handler::handle_rx_subpdus(const decoded_mac_rx_pdu& ctx)
{
  mac_ul_ue_context* ue = ue_manager.find_ue(ctx.ue_index);

  // Process SDUs and MAC CEs that are not C-RNTI MAC CE
  for (const mac_ul_sch_subpdu& subpdu : ctx.decoded_subpdus) {
    const bool ret = subpdu.lcid().is_sdu() ? handle_sdu(ctx, subpdu, ue) : handle_mac_ce(ctx, subpdu);
    if (not ret) {
      return false;
    }
  }

  return true;
}

bool pdu_rx_handler::handle_sdu(const decoded_mac_rx_pdu& ctx, const mac_ul_sch_subpdu& sdu, mac_ul_ue_context* ue)
{
  if (ue == nullptr) {
    logger.warning("{}: Discarding SDU. Cause: Non-existent C-RNTI", create_prefix(ctx, sdu));
    return false;
  }

  lcid_t lcid = (lcid_t)sdu.lcid().value();
  if (not ue->ul_bearers.contains(lcid)) {
    logger.warning("{}: Discarding SDU. Cause: Non-existent LCID", create_prefix(ctx, sdu));
    return false;
  }

  // Log MAC UL SDU
  logger.debug("{}: Forwarding SDU of {} bytes", create_prefix(ctx, sdu), sdu.sdu_length());

  // Push PDU to upper layers
  ue->ul_bearers[lcid]->on_new_sdu(byte_buffer_slice{ctx.pdu_rx.pdu, sdu.payload()});

  return true;
}

bool pdu_rx_handler::handle_mac_ce(const decoded_mac_rx_pdu& ctx, const mac_ul_sch_subpdu& subpdu)
{
  // Handle MAC CEs
  switch (subpdu.lcid().value()) {
    case lcid_ul_sch_t::CCCH_SIZE_48:
    case lcid_ul_sch_t::CCCH_SIZE_64: {
      // The CCCH CE is handled separately in case of Msg3, before all other CEs. See handle_rx_pdu().
      // At the point we enter this function, the CCCH should have been processed, and the UE should have been created.
      // Here, we only ensure that we are not receiving CCCH for a non-Msg3 PUSCH.
      if (is_du_ue_index_valid(ctx.ue_index)) {
        // The UE already existed, so it should not be receiving an MAC UL-CCCH CE. However, there is a change that we
        // received a PDU filled with zeros. In such case, we provide a clearer log message.
        bool all_zeros = true;
        for (span<const uint8_t> s : ctx.pdu_rx.pdu.segments()) {
          if (std::any_of(s.begin(), s.end(), [](uint8_t v) { return v != 0; })) {
            all_zeros = false;
            break;
          }
        }
        if (all_zeros) {
          logger.warning("{}: Discarding PDU. Cause: Rx PDU is filled with zeros, meaning that it was likely corrupted",
                         create_prefix(ctx, subpdu));
        } else {
          logger.warning("{}: Discarding PDU. Cause: UL-CCCH should be only for Msg3", create_prefix(ctx, subpdu));
        }
        return false;
      }
    } break;
    case lcid_ul_sch_t::SHORT_BSR:
    case lcid_ul_sch_t::SHORT_TRUNC_BSR:
    case lcid_ul_sch_t::LONG_BSR:
    case lcid_ul_sch_t::LONG_TRUNC_BSR: {
      if (not is_du_ue_index_valid(ctx.ue_index)) {
        logger.warning("{}: Discarding MAC CE. Cause: C-RNTI is not associated with any existing UE",
                       create_prefix(ctx, subpdu));
        return false;
      }
      // Forward decoded BSR to scheduler.
      mac_bsr_ce_info bsr_ind{};
      bsr_ind.cell_index = ctx.cell_index_rx;
      bsr_ind.ue_index   = ctx.ue_index;
      bsr_ind.rnti       = ctx.pdu_rx.rnti;
      if (subpdu.lcid() == lcid_ul_sch_t::SHORT_BSR or subpdu.lcid() == lcid_ul_sch_t::SHORT_TRUNC_BSR) {
        bsr_ind.bsr_fmt =
            subpdu.lcid() == lcid_ul_sch_t::SHORT_BSR ? bsr_format::SHORT_BSR : bsr_format::SHORT_TRUNC_BSR;
        bsr_ind.lcg_reports.push_back(decode_sbsr(subpdu.payload()));
      } else {
        bsr_ind.bsr_fmt = subpdu.lcid() == lcid_ul_sch_t::LONG_BSR ? bsr_format::LONG_BSR : bsr_format::LONG_TRUNC_BSR;
        expected<long_bsr_report> lbsr_report = decode_lbsr(bsr_ind.bsr_fmt, subpdu.payload());
        if (lbsr_report.is_error()) {
          logger.warning("{}: Discarding BSR MAC CE. Cause: BSR is invalid", create_prefix(ctx, subpdu));
          return false;
        }
        bsr_ind.lcg_reports = lbsr_report.value().list;
      }
      sched.handle_ul_bsr_indication(bsr_ind);
    } break;
    case lcid_ul_sch_t::CRNTI: {
      // The MAC C-RNTI CE is handled separately in case of Msg3, and before all other CEs. See handle_rx_pdu().
      // At the point we enter this function, the MAC C-RNTI CE should have already been processed, and the
      // old UE context should have been retrieved. Here, we only ensure that the C-RNTI CE is not being received
      // for a PUSCH that is not Msg3.
      const bool crnti_ce_was_not_yet_processed = decode_crnti_ce(subpdu.payload()) != ctx.pdu_rx.rnti;
      if (crnti_ce_was_not_yet_processed) {
        logger.warning("{}: C-RNTI CE received in PUSCH that is not Msg3", create_prefix(ctx, subpdu));
        return false;
      }
    } break;
    case lcid_ul_sch_t::SE_PHR: {
      if (not is_du_ue_index_valid(ctx.ue_index)) {
        logger.warning("{}: Discarding MAC CE. Cause: C-RNTI is not associated with any existing UE",
                       create_prefix(ctx, subpdu));
        return false;
      }
      mac_phr_ce_info phr_ind{};
      phr_ind.cell_index = ctx.cell_index_rx;
      phr_ind.ue_index   = ctx.ue_index;
      phr_ind.rnti       = ctx.pdu_rx.rnti;
      phr_ind.phr        = decode_se_phr(subpdu.payload());
      sched.handle_ul_phr_indication(phr_ind);
    } break;
    case lcid_ul_sch_t::PADDING:
      break;
    default:
      logger.warning("{}: Discarding MAC CE. Cause: Unhandled LCID", create_prefix(ctx, subpdu));
  }

  return true;
}

bool pdu_rx_handler::handle_ccch_msg(const decoded_mac_rx_pdu& ctx, const mac_ul_sch_subpdu& sdu)
{
  srsran_assert(ctx.ue_index == INVALID_DU_UE_INDEX,
                "This function should only be called for Msg3, when UE context has not been created yet");

  // Notify DU manager of received CCCH message.
  ul_ccch_indication_message msg{};
  msg.tc_rnti    = ctx.pdu_rx.rnti;
  msg.cell_index = ctx.cell_index_rx;
  msg.slot_rx    = ctx.slot_rx;

  if (!msg.subpdu.append(sdu.payload())) {
    logger.warning("{}: Unable to append SDU into sub-PDU", create_prefix(ctx, sdu));
    return false;
  }
  ccch_notifier.on_ul_ccch_msg_received(msg);

  // TODO: Do not discard remaining CEs.
  if (ctx.decoded_subpdus.nof_subpdus() > 1) {
    logger.debug("{}: Discarding remaining subPDUs", create_prefix(ctx, sdu));
  }

  return true;
}

/// \brief Finds if there a BSR with positive buffer size in the list of decoded subPDUs.
static bool contains_positive_bsr(const mac_ul_sch_pdu& decoded_subpdus)
{
  for (const auto& subpdu : decoded_subpdus) {
    if (subpdu.lcid() == lcid_ul_sch_t::SHORT_TRUNC_BSR || subpdu.lcid() == lcid_ul_sch_t::SHORT_BSR) {
      lcg_bsr_report rep = decode_sbsr(subpdu.payload());
      if (rep.buffer_size > 0) {
        return true;
      }
    }
    if (subpdu.lcid() == lcid_ul_sch_t::LONG_TRUNC_BSR || subpdu.lcid() == lcid_ul_sch_t::LONG_BSR) {
      auto bsr_fmt = subpdu.lcid() == lcid_ul_sch_t::LONG_BSR ? bsr_format::LONG_BSR : bsr_format::LONG_TRUNC_BSR;
      expected<long_bsr_report> lbsr_report = decode_lbsr(bsr_fmt, subpdu.payload());
      if (lbsr_report.has_value()) {
        for (const auto& l : lbsr_report.value().list) {
          if (l.buffer_size > 0) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

bool pdu_rx_handler::handle_crnti_ce(const decoded_mac_rx_pdu& ctx, const mac_ul_sch_subpdu& subpdu)
{
  // > Decode CRNTI CE and create new pdu context with the old C-RNTI.
  decoded_mac_rx_pdu new_ctx = ctx;
  new_ctx.pdu_rx.rnti        = decode_crnti_ce(subpdu.payload());
  if (not is_crnti(new_ctx.pdu_rx.rnti)) {
    logger.warning("{}: Discarding PDU. Cause: Invalid Payload length={} for C-RNTI MAC CE type",
                   create_prefix(new_ctx, subpdu),
                   subpdu.payload().length());
    return false;
  }

  // > Fetch an existing UE with the same old C-RNTI.
  new_ctx.ue_index = rnti_table[new_ctx.pdu_rx.rnti];
  if (new_ctx.ue_index == INVALID_DU_UE_INDEX) {
    logger.warning("{}: Discarding PDU. Cause: C-RNTI in C-RNTI CE is not associated with any existing UE. "
                   "It is likely that the old UE context has already been discarded",
                   create_prefix(new_ctx, subpdu));
    return false;
  }

  // > Dispatch continuation of subPDU handling to execution context of previous C-RNTI.
  task_executor& ue_exec = ue_exec_mapper.mac_ul_pdu_executor(new_ctx.ue_index);
  if (not ue_exec.execute([this, new_ctx = std::move(new_ctx)]() {
        if (ue_manager.find_ue(new_ctx.ue_index) == nullptr) {
          logger.warning(
              "{}: Discarding PDU. Cause: UE with C-RNTI in C-RNTI CE has been deleted while the CE was being handled",
              create_prefix(new_ctx));
          return;
        }

        // >> Handle remaining subPDUs using old C-RNTI.
        if (not handle_rx_subpdus(new_ctx)) {
          return;
        }

        // >> Notify scheduler of received C-RNTI CE.
        sched.handle_crnti_ce_indication(new_ctx.ue_index, new_ctx.cell_index_rx);

        // >> In case no positive BSR was provided, we force a positive BSR in the scheduler to complete the RA
        // procedure, as per TS 38.321, Section 5.1.5.
        if (not contains_positive_bsr(new_ctx.decoded_subpdus)) {
          sched.handle_ul_sched_command(
              mac_ul_scheduling_command{new_ctx.cell_index_rx, new_ctx.slot_rx, new_ctx.ue_index, new_ctx.pdu_rx.rnti});
        }
      })) {
    logger.warning("{}: Discarding PDU. Cause: Task queue is full.", create_prefix(ctx, subpdu));
  }

  return true;
}

void pdu_rx_handler::write_pcap_rx_pdu(const slot_point& sl_rx, const mac_rx_pdu& pdu)
{
  if (not pcap.is_write_enabled()) {
    return;
  }

  srsran::mac_nr_context_info context = {};
  context.radioType                   = PCAP_FDD_RADIO;
  context.direction                   = PCAP_DIRECTION_UPLINK;
  context.rntiType                    = PCAP_C_RNTI;
  context.rnti                        = to_value(pdu.rnti);
  context.ueid   = rnti_table[pdu.rnti] == du_ue_index_t::INVALID_DU_UE_INDEX ? du_ue_index_t::INVALID_DU_UE_INDEX
                                                                              : rnti_table[pdu.rnti] + 1;
  context.harqid = pdu.harq_id;
  context.system_frame_number = sl_rx.sfn();
  context.sub_frame_number    = sl_rx.subframe_index();
  context.length              = pdu.pdu.length();
  pcap.push_pdu(context, pdu.pdu.copy());
}
