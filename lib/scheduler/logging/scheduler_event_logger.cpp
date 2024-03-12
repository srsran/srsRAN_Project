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

#include "scheduler_event_logger.h"
#include "srsran/ran/csi_report/csi_report_formatters.h"

using namespace srsran;

void scheduler_event_logger::log_impl()
{
  if (mode == debug) {
    logger.debug("Processed slot events:{}", to_c_str(fmtbuf));
    fmtbuf.clear();
  } else if (mode == info) {
    logger.info("Processed slot events:{}", to_c_str(fmtbuf));
    fmtbuf.clear();
  }
}

void scheduler_event_logger::enqueue_impl(const cell_creation_event& cell_ev)
{
  cell_pcis[cell_ev.cell_index] = cell_ev.pci;

  if (mode == debug) {
    fmt::format_to(fmtbuf, "\n- Cell creation: idx={} pci={}", cell_ev.cell_index, cell_ev.pci);
  } else if (mode == info) {
    fmt::format_to(fmtbuf, "{}Cell creation idx={} pci={}", separator(), cell_ev.cell_index, cell_ev.pci);
  }
}

void scheduler_event_logger::enqueue_impl(const prach_event& rach_ev)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf,
                   "\n- PRACH: slot={}, pci={} preamble={} ra-rnti={} temp_crnti={} ta_cmd={}",
                   rach_ev.slot_rx,
                   cell_pcis[rach_ev.cell_index],
                   rach_ev.preamble_id,
                   rach_ev.ra_rnti,
                   rach_ev.tc_rnti,
                   rach_ev.ta);
  } else {
    fmt::format_to(fmtbuf,
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
    fmt::format_to(
        fmtbuf, "\n- RACH ind: slot_rx={} pci={} PRACHs: ", cell_pcis[rach_ind.cell_index], rach_ind.slot_rx);
    unsigned count = 0;
    for (unsigned i = 0; i != rach_ind.occasions.size(); ++i) {
      for (unsigned j = 0; j != rach_ind.occasions[i].preambles.size(); ++j) {
        const auto& preamble = rach_ind.occasions[i].preambles[j];
        fmt::format_to(fmtbuf,
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
    fmt::format_to(fmtbuf, "{}RACH Ind slot_rx={}", separator(), rach_ind.slot_rx);
  }
}

void scheduler_event_logger::enqueue_impl(const ue_creation_event& ue_request)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf,
                   "\n- UE creation: ue={} rnti={} pci={}",
                   ue_request.ue_index,
                   ue_request.rnti,
                   cell_pcis[ue_request.pcell_index]);
  }
}

void scheduler_event_logger::enqueue_impl(const ue_reconf_event& ue_request)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf, "\n- UE reconfiguration: ue={} rnti={}", ue_request.ue_index, ue_request.rnti);
  }
}

void scheduler_event_logger::enqueue_impl(const sched_ue_delete_message& ue_request)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf, "\n- UE removal: ue={} rnti={}", ue_request.ue_index, ue_request.crnti);
  }
}

void scheduler_event_logger::enqueue_impl(const error_indication_event& err_ind)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf,
                   "\n- ErrorIndication: slot={}{}{}{}",
                   err_ind.sl_tx,
                   err_ind.outcome.pdcch_discarded ? ", PDCCH discarded" : "",
                   err_ind.outcome.pdsch_discarded ? ", PDSCH discarded" : "",
                   err_ind.outcome.pusch_and_pucch_discarded ? ", PUSCH and PUCCH discarded" : "");
  } else if (mode == info) {
    fmt::format_to(fmtbuf, "{}ErrorIndication slot={}", separator(), err_ind.sl_tx);
  }
}

void scheduler_event_logger::enqueue_impl(const sr_event& sr)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf, "\n- SR: ue={} rnti={}", sr.ue_index, sr.rnti);
  }
}

void scheduler_event_logger::enqueue_impl(const csi_report_event& csi)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf, "\n- CSI: ue={} rnti={}:", csi.ue_index, csi.rnti);
    if (csi.csi.first_tb_wideband_cqi.has_value()) {
      fmt::format_to(fmtbuf, " cqi={}", *csi.csi.first_tb_wideband_cqi);
    }
    if (csi.csi.ri.has_value()) {
      fmt::format_to(fmtbuf, " ri={}", csi.csi.ri.value());
    }
    if (csi.csi.pmi.has_value()) {
      fmt::format_to(fmtbuf, " {}", *csi.csi.pmi);
    }
  }
}

void scheduler_event_logger::enqueue_impl(const bsr_event& bsr)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf, "\n- BSR: ue={} rnti={} type=\"{}\" report={{", bsr.ue_index, bsr.rnti, to_string(bsr.type));

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
    fmt::format_to(fmtbuf, "}} pending_bytes={}", bsr.tot_ul_pending_bytes);
  }
}

void scheduler_event_logger::enqueue_impl(const harq_ack_event& harq_ev)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf,
                   "\n- HARQ-ACK: ue={} rnti={} pci={} slot_rx={} h_id={} ack={}",
                   harq_ev.ue_index,
                   harq_ev.rnti,
                   cell_pcis[harq_ev.cell_index],
                   harq_ev.sl_ack_rx,
                   harq_ev.h_id,
                   (unsigned)harq_ev.ack);
    if (harq_ev.ack == mac_harq_ack_report_status::ack) {
      fmt::format_to(fmtbuf, " tbs={}", harq_ev.tbs);
    }
  }
}

void scheduler_event_logger::enqueue_impl(const crc_event& crc_ev)
{
  if (mode == debug) {
    if (crc_ev.ul_sinr_db.has_value()) {
      fmt::format_to(fmtbuf,
                     "\n- CRC: ue={} rnti={} pci={} rx_slot={} h_id={} crc={} sinr={}dB",
                     crc_ev.ue_index,
                     crc_ev.rnti,
                     cell_pcis[crc_ev.cell_index],
                     crc_ev.sl_rx,
                     crc_ev.h_id,
                     crc_ev.crc,
                     crc_ev.ul_sinr_db.value());
    } else {
      fmt::format_to(fmtbuf,
                     "\n- CRC: ue={} rnti={} pci={} rx_slot={} h_id={} crc={} sinr=N/A",
                     crc_ev.ue_index,
                     crc_ev.rnti,
                     cell_pcis[crc_ev.cell_index],
                     crc_ev.sl_rx,
                     crc_ev.h_id,
                     crc_ev.crc);
    }
  }
}

void scheduler_event_logger::enqueue_impl(const dl_mac_ce_indication& mac_ce)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf, "\n- MAC CE: ue={} lcid={}", mac_ce.ue_index, mac_ce.ce_lcid.value());
  }
}

void scheduler_event_logger::enqueue_impl(const dl_buffer_state_indication_message& bs)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf, "\n- RLC Buffer State: ue={} lcid={} pending_bytes={}", bs.ue_index, bs.lcid, bs.bs);
  }
}

void scheduler_event_logger::enqueue_impl(const phr_event& phr_ev)
{
  if (mode == debug) {
    fmt::format_to(fmtbuf,
                   "\n- PHR: ue={} rnti={} pci={} ph={}dB",
                   phr_ev.ue_index,
                   phr_ev.rnti,
                   cell_pcis[phr_ev.cell_index],
                   phr_ev.ph);
    if (phr_ev.p_cmax.has_value()) {
      fmt::format_to(fmtbuf, " p_cmax={}dBm", phr_ev.p_cmax.value());
    }
  }
}

const char* scheduler_event_logger::separator() const
{
  return fmtbuf.size() > 0 ? ", " : " ";
}
