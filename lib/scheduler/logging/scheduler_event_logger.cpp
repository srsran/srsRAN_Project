/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "scheduler_event_logger.h"

using namespace srsgnb;

void scheduler_event_logger::log_impl()
{
  if (mode == debug) {
    logger.debug("SCHED: Processed slot events:{}", to_c_str(fmtbuf));
    fmtbuf.clear();
  } else {
    logger.info("SCHED: Processed slot events: {}", to_c_str(fmtbuf));
    fmtbuf.clear();
  }
}

void scheduler_event_logger::enqueue_impl(const prach_event& rach_ev)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf,
                   "\n- PRACH: slot={}, cell={}, preamble={}, ra-rnti=0x{:x}, temp_crnti=0x{:x}, ta_cmd={}",
                   rach_ev.slot_rx,
                   rach_ev.cell_index,
                   rach_ev.preamble_id,
                   rach_ev.ra_rnti,
                   rach_ev.tc_rnti,
                   rach_ev.ta);
  } else {
    fmt::format_to(fmtbuf,
                   "{}prach(ra-rnti={:#x}, preamble={}, tc-rnti={:#x})",
                   separator(),
                   rach_ev.ra_rnti,
                   rach_ev.preamble_id,
                   rach_ev.tc_rnti);
  }
}

void scheduler_event_logger::enqueue_impl(const rach_indication_message& rach_ind)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf, "\n- RACH ind: slot_rx={}, cell={}, PRACHs: ", rach_ind.cell_index, rach_ind.slot_rx);
    unsigned count = 0;
    for (unsigned i = 0; i != rach_ind.occasions.size(); ++i) {
      for (unsigned j = 0; j != rach_ind.occasions[i].preambles.size(); ++j) {
        const auto& preamble = rach_ind.occasions[i].preambles[j];
        fmt::format_to(fmtbuf,
                       "{}{}: preamble={}, tc-rnti={:#x}, freq_idx={}, start_symbol={}, TA={}s",
                       count == 0 ? "" : ", ",
                       count,
                       preamble.preamble_id,
                       preamble.tc_rnti,
                       rach_ind.occasions[i].frequency_index,
                       rach_ind.occasions[i].start_symbol,
                       preamble.time_advance.to_seconds());
        count++;
      }
    }
  } else {
    fmt::format_to(fmtbuf, "{}RACH Ind slot_rx={}", separator(), rach_ind.slot_rx);
  }
}

void scheduler_event_logger::enqueue_impl(const ue_creation_event& ue_request)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf,
                   "\n- UE creation: UE={}, rnti={:#x}, PCell={}",
                   ue_request.ue_index,
                   ue_request.rnti,
                   ue_request.pcell_index);
  } else if (logger.info.enabled()) {
    fmt::format_to(fmtbuf, "{}UE={} creation", separator(), ue_request.ue_index);
  }
}

void scheduler_event_logger::enqueue_impl(const sched_ue_reconfiguration_message& ue_request)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf, "\n- UE reconfiguration: UE={}, rnti={:#x}", ue_request.ue_index, ue_request.crnti);
  } else if (logger.info.enabled()) {
    fmt::format_to(fmtbuf, "{}UE={} reconf", separator(), ue_request.ue_index);
  }
}

void scheduler_event_logger::enqueue_impl(const sched_ue_delete_message& ue_request)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf, "\n- UE removal: UE={}, rnti={:#x}", ue_request.ue_index, ue_request.crnti);
  } else if (logger.info.enabled()) {
    fmt::format_to(fmtbuf, "{}UE={} removal", separator(), ue_request.ue_index);
  }
}

void scheduler_event_logger::enqueue_impl(const sr_event& sr)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf, "\n- SR: UE={}, rnti={:#x}", sr.ue_index, sr.rnti);
  } else {
    fmt::format_to(fmtbuf, "{}UE={} SR ind", separator(), sr.ue_index);
  }
}

void scheduler_event_logger::enqueue_impl(const ul_bsr_indication_message& bsr)
{
  if (mode == debug) {
    fmt::format_to(
        fmtbuf, "\n- BSR: UE={}, rnti={:#x}, type={}, payload:", bsr.ue_index, bsr.crnti, to_string(bsr.type));
    for (unsigned i = 0; i != bsr.reported_lcgs.size(); ++i) {
      fmt::format_to(
          fmtbuf, "{}{}: {}", i == 0 ? "" : ", ", bsr.reported_lcgs[i].lcg_id, bsr.reported_lcgs[i].nof_bytes);
    }
  } else if (logger.info.enabled()) {
    fmt::format_to(fmtbuf, "{}UE={} UL BSR", separator(), bsr.ue_index);
  }
}

void scheduler_event_logger::enqueue_impl(const harq_ack_event& harq_ev)
{
  fmt::format_to(fmtbuf,
                 "\n- HARQ-ACK: UE={}, rnti={:#x}, cell={}, h_id={}, ack={}",
                 harq_ev.ue_index,
                 harq_ev.rnti,
                 harq_ev.cell_index,
                 harq_ev.cell_index,
                 harq_ev.h_id,
                 harq_ev.ack ? 1 : 0);
  if (harq_ev.ack) {
    fmt::format_to(fmtbuf, ", tbs={}", harq_ev.tbs);
  }
}

void scheduler_event_logger::enqueue_impl(const crc_event& crc_ev)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf,
                   "\n- CRC: UE={}, rnti={:#x}, cell={}, h_id={}, crc={}, sinr={}dB",
                   crc_ev.ue_index,
                   crc_ev.rnti,
                   crc_ev.cell_index,
                   crc_ev.h_id,
                   crc_ev.crc,
                   crc_ev.ul_sinr_db);
  } else if (logger.info.enabled()) {
    fmt::format_to(fmtbuf, "{}CRC ind", separator());
  }
}

void scheduler_event_logger::enqueue_impl(const dl_mac_ce_indication& mac_ce)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf, "\n- MAC CE: UE={}, LCID={}", mac_ce.ue_index, mac_ce.ce_lcid.value());
  } else {
    fmt::format_to(fmtbuf, "{}mac_ce(UE={}, LCID={})", separator(), mac_ce.ue_index, mac_ce.ce_lcid);
  }
}

void scheduler_event_logger::enqueue_impl(const dl_buffer_state_indication_message& bs)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf,
                   "\n- RLC Buffer State: UE={}, rnti={:#x}, lcid={}, pending_bytes={}",
                   bs.ue_index,
                   bs.rnti,
                   bs.lcid,
                   bs.bs);
  } else {
    fmt::format_to(fmtbuf, "{}rlc_bs(UE={}, lcid={}, pending={})", separator(), bs.ue_index, bs.lcid, bs.bs);
  }
}

const char* scheduler_event_logger::separator() const
{
  return fmtbuf.size() > 0 ? ", " : " ";
}
