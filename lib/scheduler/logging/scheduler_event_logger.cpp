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

#include "scheduler_event_logger.h"
#include "srsran/ran/csi_report/csi_report_formatters.h"
#include "srsran/ran/pusch/pusch_tpmi_formatter.h"
#include "srsran/support/format/fmt_to_c_str.h"
#include "fmt/std.h"

using namespace srsran;

scheduler_event_logger::scheduler_event_logger(du_cell_index_t cell_index_, pci_t pci_) :
  cell_index(cell_index_),
  pci(pci_),
  logger(srslog::fetch_basic_logger("SCHED")),
  mode(logger.debug.enabled() ? mode_t::debug : (logger.info.enabled() ? mode_t::info : mode_t::none))
{
  if (mode == debug) {
    fmt::format_to(std::back_inserter(fmtbuf), "\n- Cell creation: idx={} pci={}", fmt::underlying(cell_index), pci);
  } else if (mode == info) {
    fmt::format_to(
        std::back_inserter(fmtbuf), "{}Cell creation idx={} pci={}", separator(), fmt::underlying(cell_index), pci);
  }
}

void scheduler_event_logger::log_impl()
{
  if (mode == debug) {
    logger.debug("Processed slot events pci={}:{}", pci, to_c_str(fmtbuf));
    fmtbuf.clear();
  } else if (mode == info) {
    logger.info("Processed slot events pci={}:{}", pci, to_c_str(fmtbuf));
    fmtbuf.clear();
  }
}

void scheduler_event_logger::enqueue_impl(const prach_event& rach_ev)
{
  if (mode == debug) {
    fmt::format_to(std::back_inserter(fmtbuf),
                   "\n- PRACH: slot={} pci={} preamble={} ra-rnti={} temp_crnti={} ta_cmd={}",
                   rach_ev.slot_rx,
                   pci,
                   rach_ev.preamble_id,
                   rach_ev.ra_rnti,
                   rach_ev.tc_rnti,
                   rach_ev.ta);
  } else {
    fmt::format_to(std::back_inserter(fmtbuf),
                   "{}prach(ra-rnti={} preamble={} tc-rnti={})",
                   separator(),
                   rach_ev.ra_rnti,
                   rach_ev.preamble_id,
                   rach_ev.tc_rnti);
  }
}

void scheduler_event_logger::enqueue_impl(const rach_indication_message& rach_ind)
{
  if (mode == debug) {
    fmt::format_to(std::back_inserter(fmtbuf), "\n- RACH ind: slot_rx={} pci={} PRACHs: ", rach_ind.slot_rx, pci);
    unsigned count = 0;
    for (unsigned i = 0; i != rach_ind.occasions.size(); ++i) {
      for (unsigned j = 0; j != rach_ind.occasions[i].preambles.size(); ++j) {
        const auto& preamble = rach_ind.occasions[i].preambles[j];
        fmt::format_to(std::back_inserter(fmtbuf),
                       "{}{}: preamble={} tc-rnti={} freq_idx={} start_symbol={} TA={}s",
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
    fmt::format_to(std::back_inserter(fmtbuf), "{}RACH Ind slot_rx={}", separator(), rach_ind.slot_rx);
  }
}

void scheduler_event_logger::enqueue_impl(const ue_creation_event& ue_request)
{
  if (mode == debug) {
    fmt::format_to(std::back_inserter(fmtbuf),
                   "\n- UE creation: ue={} rnti={} pci={}",
                   fmt::underlying(ue_request.ue_index),
                   ue_request.rnti,
                   pci);
  }
}

void scheduler_event_logger::enqueue_impl(const ue_reconf_event& ue_request)
{
  if (mode == debug) {
    fmt::format_to(std::back_inserter(fmtbuf),
                   "\n- UE reconfiguration: ue={} rnti={}",
                   fmt::underlying(ue_request.ue_index),
                   ue_request.rnti);
  }
}

void scheduler_event_logger::enqueue_impl(const sched_ue_delete_message& ue_request)
{
  if (mode == debug) {
    fmt::format_to(std::back_inserter(fmtbuf),
                   "\n- UE removal: ue={} rnti={}",
                   fmt::underlying(ue_request.ue_index),
                   ue_request.crnti);
  }
}

void scheduler_event_logger::enqueue_impl(const ue_cfg_applied_event& ev)
{
  if (mode == debug) {
    fmt::format_to(std::back_inserter(fmtbuf),
                   "\n- UE dedicated config applied: ue={} rnti={}",
                   fmt::underlying(ev.ue_index),
                   ev.rnti);
  }
}

void scheduler_event_logger::enqueue_impl(const error_indication_event& err_ind)
{
  if (mode == debug) {
    fmt::format_to(std::back_inserter(fmtbuf),
                   "\n- ErrorIndication: slot={}{}{}{}",
                   err_ind.sl_tx,
                   err_ind.outcome.pdcch_discarded ? ", PDCCH discarded" : "",
                   err_ind.outcome.pdsch_discarded ? ", PDSCH discarded" : "",
                   err_ind.outcome.pusch_and_pucch_discarded ? ", PUSCH and PUCCH discarded" : "");
  } else if (mode == info) {
    fmt::format_to(std::back_inserter(fmtbuf), "{}ErrorIndication slot={}", separator(), err_ind.sl_tx);
  }
}

void scheduler_event_logger::enqueue_impl(const sr_event& sr)
{
  if (mode == debug) {
    fmt::format_to(std::back_inserter(fmtbuf), "\n- SR: ue={} rnti={}", fmt::underlying(sr.ue_index), sr.rnti);
  }
}

void scheduler_event_logger::enqueue_impl(const csi_report_event& csi)
{
  if (mode == debug) {
    fmt::format_to(std::back_inserter(fmtbuf), "\n- CSI: ue={} rnti={}:", fmt::underlying(csi.ue_index), csi.rnti);
    if (csi.csi.first_tb_wideband_cqi.has_value()) {
      fmt::format_to(std::back_inserter(fmtbuf), " cqi={}", *csi.csi.first_tb_wideband_cqi);
    }
    if (csi.csi.ri.has_value()) {
      fmt::format_to(std::back_inserter(fmtbuf), " ri={}", csi.csi.ri.value());
    }
    if (csi.csi.pmi.has_value()) {
      fmt::format_to(std::back_inserter(fmtbuf), " {}", *csi.csi.pmi);
    }
  }
}

void scheduler_event_logger::enqueue_impl(const bsr_event& bsr)
{
  if (mode == debug) {
    fmt::format_to(std::back_inserter(fmtbuf),
                   "\n- BSR: ue={} rnti={} type=\"{}\" report={{",
                   fmt::underlying(bsr.ue_index),
                   bsr.rnti,
                   to_string(bsr.type));

    if (bsr.type == bsr_format::LONG_BSR or bsr.type == bsr_format::LONG_TRUNC_BSR or bsr.reported_lcgs.full()) {
      std::array<int, MAX_NOF_LCGS> report;
      std::fill(report.begin(), report.end(), -1);
      for (unsigned i = 0; i != bsr.reported_lcgs.size(); ++i) {
        report[bsr.reported_lcgs[i].lcg_id] = bsr.reported_lcgs[i].nof_bytes;
      }
      for (unsigned i = 0; i != MAX_NOF_LCGS; ++i) {
        fmt::format_to(std::back_inserter(fmtbuf), "{}", i == 0 ? "" : ", ");
        if (report[i] < 0) {
          fmt::format_to(std::back_inserter(fmtbuf), "-");
        } else {
          fmt::format_to(std::back_inserter(fmtbuf), "{}", report[i]);
        }
      }
    } else {
      for (unsigned i = 0; i != bsr.reported_lcgs.size(); ++i) {
        fmt::format_to(std::back_inserter(fmtbuf),
                       "{}{}: {}",
                       i == 0 ? "" : " ",
                       fmt::underlying(bsr.reported_lcgs[i].lcg_id),
                       bsr.reported_lcgs[i].nof_bytes);
      }
    }
    fmt::format_to(std::back_inserter(fmtbuf), "}} pending_bytes={}", bsr.tot_ul_pending_bytes);
  }
}

void scheduler_event_logger::enqueue_impl(const harq_ack_event& harq_ev)
{
  if (mode == debug) {
    fmt::format_to(std::back_inserter(fmtbuf),
                   "\n- HARQ-ACK: ue={} rnti={} pci={} slot_rx={} h_id={} ack={}",
                   fmt::underlying(harq_ev.ue_index),
                   harq_ev.rnti,
                   pci,
                   harq_ev.sl_ack_rx,
                   fmt::underlying(harq_ev.h_id),
                   (unsigned)harq_ev.ack);
    if (harq_ev.ack == mac_harq_ack_report_status::ack) {
      fmt::format_to(std::back_inserter(fmtbuf), " tbs={}", harq_ev.tbs);
    }
  }
}

void scheduler_event_logger::enqueue_impl(const crc_event& crc_ev)
{
  if (mode == debug) {
    if (crc_ev.ul_sinr_db.has_value()) {
      fmt::format_to(std::back_inserter(fmtbuf),
                     "\n- CRC: ue={} rnti={} pci={} rx_slot={} h_id={} crc={} sinr={:.2}dB",
                     fmt::underlying(crc_ev.ue_index),
                     crc_ev.rnti,
                     pci,
                     crc_ev.sl_rx,
                     fmt::underlying(crc_ev.h_id),
                     crc_ev.crc,
                     crc_ev.ul_sinr_db.value());
    } else {
      fmt::format_to(std::back_inserter(fmtbuf),
                     "\n- CRC: ue={} rnti={} pci={} rx_slot={} h_id={} crc={} sinr=N/A",
                     fmt::underlying(crc_ev.ue_index),
                     crc_ev.rnti,
                     pci,
                     crc_ev.sl_rx,
                     fmt::underlying(crc_ev.h_id),
                     crc_ev.crc);
    }
  }
}

void scheduler_event_logger::enqueue_impl(const dl_mac_ce_indication& mac_ce)
{
  if (mode == debug) {
    fmt::format_to(std::back_inserter(fmtbuf),
                   "\n- MAC CE: ue={} lcid={}",
                   fmt::underlying(mac_ce.ue_index),
                   mac_ce.ce_lcid.value());
  }
}

void scheduler_event_logger::enqueue_impl(const dl_buffer_state_indication_message& bs)
{
  if (mode == debug) {
    fmt::format_to(std::back_inserter(fmtbuf),
                   "\n- RLC Buffer State: ue={} lcid={} pending_bytes={}",
                   fmt::underlying(bs.ue_index),
                   fmt::underlying(bs.lcid),
                   bs.bs);
  }
}

void scheduler_event_logger::enqueue_impl(const phr_event& phr_ev)
{
  if (mode == debug) {
    fmt::format_to(std::back_inserter(fmtbuf),
                   "\n- PHR: ue={} rnti={} pci={} ph={}dB",
                   fmt::underlying(phr_ev.ue_index),
                   phr_ev.rnti,
                   pci,
                   phr_ev.ph);
    if (phr_ev.p_cmax.has_value()) {
      fmt::format_to(std::back_inserter(fmtbuf), " p_cmax={}dBm", phr_ev.p_cmax.value());
    }
  }
}

void scheduler_event_logger::enqueue_impl(const srs_indication_event& srs_ev)
{
  if (mode == debug) {
    fmt::format_to(std::back_inserter(fmtbuf), "\n- SRS: ue={} rnti={}", fmt::underlying(srs_ev.ue_index), srs_ev.rnti);
    if (srs_ev.tpmi_info.has_value()) {
      fmt::format_to(std::back_inserter(fmtbuf), " tpmi_info=[{:;}]", srs_ev.tpmi_info.value());
    }
  }
}

void scheduler_event_logger::enqueue_impl(const slice_reconfiguration_event& slice_reconf_ev)
{
  if (mode == debug) {
    fmt::format_to(
        std::back_inserter(fmtbuf), "\n- SLICE RECONF: cell={}", fmt::underlying(slice_reconf_ev.cell_index));
  }
}

const char* scheduler_event_logger::separator() const
{
  return fmtbuf.size() > 0 ? ", " : " ";
}
