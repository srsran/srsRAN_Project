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
    logger.debug("Processed slot events:{}", to_c_str(fmtbuf));
    fmtbuf.clear();
  } else {
    logger.info("Processed slot events: {}", to_c_str(fmtbuf));
    fmtbuf.clear();
  }
}

void scheduler_event_logger::enqueue_impl(const prach_event& rach_ev)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf,
                   "\n- PRACH: slot={}, cell={} preamble={} ra-rnti=0x{:x} temp_crnti=0x{:x} ta_cmd={}",
                   rach_ev.slot_rx,
                   rach_ev.cell_index,
                   rach_ev.preamble_id,
                   rach_ev.ra_rnti,
                   rach_ev.tc_rnti,
                   rach_ev.ta);
  } else {
    fmt::format_to(fmtbuf,
                   "{}prach(ra-rnti={:#x} preamble={} tc-rnti={:#x})",
                   separator(),
                   rach_ev.ra_rnti,
                   rach_ev.preamble_id,
                   rach_ev.tc_rnti);
  }
}

void scheduler_event_logger::enqueue_impl(const rach_indication_message& rach_ind)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf, "\n- RACH ind: slot_rx={} cell={} PRACHs: ", rach_ind.cell_index, rach_ind.slot_rx);
    unsigned count = 0;
    for (unsigned i = 0; i != rach_ind.occasions.size(); ++i) {
      for (unsigned j = 0; j != rach_ind.occasions[i].preambles.size(); ++j) {
        const auto& preamble = rach_ind.occasions[i].preambles[j];
        fmt::format_to(fmtbuf,
                       "{}{}: preamble={} tc-rnti={:#x} freq_idx={} start_symbol={} TA={}s",
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
                   "\n- UE creation: UE={} rnti={:#x} PCell={}",
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
    fmt::format_to(fmtbuf, "\n- UE reconfiguration: UE={} rnti={:#x}", ue_request.ue_index, ue_request.crnti);
  } else if (logger.info.enabled()) {
    fmt::format_to(fmtbuf, "{}UE={} reconf", separator(), ue_request.ue_index);
  }
}

void scheduler_event_logger::enqueue_impl(const sched_ue_delete_message& ue_request)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf, "\n- UE removal: UE={} rnti={:#x}", ue_request.ue_index, ue_request.crnti);
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

void scheduler_event_logger::enqueue_impl(const bsr_event& bsr)
{
  if (mode == debug) {
    fmt::format_to(
        fmtbuf, "\n- BSR: UE={} rnti={:#x} type=\"{}\" report={{", bsr.ue_index, bsr.rnti, to_string(bsr.type));

    if (bsr.type == bsr_format::LONG_BSR or bsr.type == bsr_format::LONG_TRUNC_BSR or bsr.reported_lcgs.full()) {
      std::array<int, MAX_NOF_LCGS> report;
      std::fill(report.begin(), report.end(), -1);
      for (unsigned i = 0; i != bsr.reported_lcgs.size(); ++i) {
        report[bsr.reported_lcgs[i].lcg_id] = bsr.reported_lcgs[i].nof_bytes;
      }
      for (unsigned i = 0; i != MAX_NOF_LCGS; ++i) {
        fmt::format_to(fmtbuf, "{}", i == 0 ? "" : ", ");
        if (report[i] < 0) {
          fmt::format_to(fmtbuf, "-");
        } else {
          fmt::format_to(fmtbuf, "{}", report[i]);
        }
      }
    } else {
      for (unsigned i = 0; i != bsr.reported_lcgs.size(); ++i) {
        fmt::format_to(
            fmtbuf, "{}{}: {}", i == 0 ? "" : " ", bsr.reported_lcgs[i].lcg_id, bsr.reported_lcgs[i].nof_bytes);
      }
    }
    fmt::format_to(fmtbuf, "}} to_alloc={:B}", bsr.tot_ul_pending_bytes);
  } else if (logger.info.enabled()) {
    unsigned tot_bytes = 0;
    for (const auto& lcg : bsr.reported_lcgs) {
      tot_bytes += lcg.nof_bytes;
    }
    fmt::format_to(fmtbuf, "{}UE={} BSR={}", separator(), bsr.ue_index, tot_bytes);
  }
}

void scheduler_event_logger::enqueue_impl(const harq_ack_event& harq_ev)
{
  fmt::format_to(fmtbuf,
                 "\n- HARQ-ACK: UE={} rnti={:#x} cell={} slot_rx={} h_id={} ack={}",
                 harq_ev.ue_index,
                 harq_ev.rnti,
                 harq_ev.cell_index,
                 harq_ev.sl_ack_rx,
                 harq_ev.h_id,
                 harq_ev.ack ? 1 : 0);
  if (harq_ev.ack) {
    fmt::format_to(fmtbuf, ", tbs={}", harq_ev.tbs);
  }
}

void scheduler_event_logger::enqueue_impl(const crc_event& crc_ev)
{
  if (mode == debug) {
    if (crc_ev.ul_sinr_db.has_value()) {
      fmt::format_to(fmtbuf,
                     "\n- CRC: UE={} rnti={:#x} cell={} rx_slot={} h_id={} crc={} sinr={}dB",
                     crc_ev.ue_index,
                     crc_ev.rnti,
                     crc_ev.cell_index,
                     crc_ev.sl_rx,
                     crc_ev.h_id,
                     crc_ev.crc,
                     crc_ev.ul_sinr_db.value());
    } else {
      fmt::format_to(fmtbuf,
                     "\n- CRC: UE={} rnti={:#x} cell={} rx_slot={} h_id={} crc={} sinr=N/A",
                     crc_ev.ue_index,
                     crc_ev.rnti,
                     crc_ev.cell_index,
                     crc_ev.sl_rx,
                     crc_ev.h_id,
                     crc_ev.crc);
    }
  } else if (logger.info.enabled()) {
    fmt::format_to(fmtbuf, "{}CRC ind", separator());
  }
}

void scheduler_event_logger::enqueue_impl(const dl_mac_ce_indication& mac_ce)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf, "\n- MAC CE: UE={}, LCID={}", mac_ce.ue_index, mac_ce.ce_lcid.value());
  } else {
    fmt::format_to(fmtbuf, "{}mac_ce(UE={} LCID={})", separator(), mac_ce.ue_index, mac_ce.ce_lcid);
  }
}

void scheduler_event_logger::enqueue_impl(const dl_buffer_state_indication_message& bs)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf, "\n- RLC Buffer State: UE={} lcid={} pending_bytes={}", bs.ue_index, bs.lcid, bs.bs);
  } else {
    fmt::format_to(fmtbuf, "{}rlc_bs(UE={} lcid={} pending={})", separator(), bs.ue_index, bs.lcid, bs.bs);
  }
}

const char* scheduler_event_logger::separator() const
{
  return fmtbuf.size() > 0 ? ", " : " ";
}
