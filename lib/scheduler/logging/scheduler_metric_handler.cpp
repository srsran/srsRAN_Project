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

#include "../config/cell_configuration.h"
#include "scheduler_metrics_handler.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

cell_metrics_handler::cell_metrics_handler(msecs                       metrics_report_period,
                                           scheduler_metrics_notifier& notifier_,
                                           const cell_configuration&   cell_cfg_) :
  notifier(notifier_), report_period(metrics_report_period), cell_cfg(cell_cfg_)
{
  next_report.ue_metrics.reserve(MAX_NOF_DU_UES);
  next_report.events.reserve(MAX_NOF_DU_UES);
}

void cell_metrics_handler::handle_ue_creation(du_ue_index_t ue_index, rnti_t rnti, pci_t pcell_pci)
{
  ues.emplace(ue_index);
  ues[ue_index].rnti     = rnti;
  ues[ue_index].ue_index = ue_index;
  ues[ue_index].pci      = pcell_pci;
  rnti_to_ue_index_lookup.emplace(rnti, ue_index);

  next_report.events.push_back(scheduler_cell_event{last_slot_tx, rnti, scheduler_cell_event::event_type::ue_add});
}

void cell_metrics_handler::handle_ue_reconfiguration(du_ue_index_t ue_index)
{
  next_report.events.push_back(
      scheduler_cell_event{last_slot_tx, ues[ue_index].rnti, scheduler_cell_event::event_type::ue_reconf});
}

void cell_metrics_handler::handle_ue_deletion(du_ue_index_t ue_index)
{
  if (ues.contains(ue_index)) {
    rnti_t rnti = ues[ue_index].rnti;
    next_report.events.push_back(scheduler_cell_event{last_slot_tx, rnti, scheduler_cell_event::event_type::ue_rem});

    rnti_to_ue_index_lookup.erase(rnti);
    ues.erase(ue_index);
  }
}

void cell_metrics_handler::handle_crc_indication(const ul_crc_pdu_indication& crc_pdu, units::bytes tbs)
{
  if (ues.contains(crc_pdu.ue_index)) {
    auto& u = ues[crc_pdu.ue_index];
    u.data.count_crc_acks += crc_pdu.tb_crc_success ? 1 : 0;
    u.data.count_crc_pdus++;
    if (crc_pdu.ul_sinr_dB.has_value()) {
      u.data.nof_pusch_snr_reports++;
      u.data.sum_pusch_snrs += crc_pdu.ul_sinr_dB.value();
    }
    if (crc_pdu.ul_rsrp_dBFS.has_value()) {
      u.data.nof_pusch_rsrp_reports++;
      u.data.sum_pusch_rsrp += crc_pdu.ul_rsrp_dBFS.value();
    }
    if (crc_pdu.tb_crc_success) {
      u.data.sum_ul_tb_bytes += tbs.value();
    }
    if (crc_pdu.time_advance_offset.has_value()) {
      u.last_ta = crc_pdu.time_advance_offset;
    }
  }
}

void cell_metrics_handler::handle_pucch_sinr(ue_metric_context& u, float sinr)
{
  u.data.nof_pucch_snr_reports++;
  u.data.sum_pucch_snrs += sinr;
}

void cell_metrics_handler::handle_csi_report(ue_metric_context& u, const csi_report_data& csi)
{
  // Add new CQI and RI observations if they are available in the CSI report.
  if (csi.first_tb_wideband_cqi.has_value()) {
    u.data.cqi.update(csi.first_tb_wideband_cqi->to_uint());
  }
  if (csi.ri.has_value()) {
    u.data.ri.update(csi.ri->to_uint());
  }
}

void cell_metrics_handler::handle_dl_harq_ack(du_ue_index_t ue_index, bool ack, units::bytes tbs)
{
  if (ues.contains(ue_index)) {
    auto& u = ues[ue_index];
    u.data.count_uci_harq_acks += ack ? 1 : 0;
    u.data.count_uci_harqs++;
    if (ack) {
      u.data.sum_dl_tb_bytes += tbs.value();
    }
  }
}

void cell_metrics_handler::handle_harq_timeout(du_ue_index_t ue_index, bool is_dl)
{
  if (ues.contains(ue_index)) {
    auto& u = ues[ue_index];
    if (is_dl) {
      u.data.count_uci_harqs++;
    } else {
      u.data.count_crc_pdus++;
    }
  }
}

void cell_metrics_handler::handle_uci_pdu_indication(const uci_indication::uci_pdu& pdu)
{
  if (ues.contains(pdu.ue_index)) {
    auto& u = ues[pdu.ue_index];

    if (const auto* f1 = std::get_if<uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu>(&pdu.pdu)) {
      if (f1->ul_sinr_dB.has_value()) {
        handle_pucch_sinr(u, f1->ul_sinr_dB.value());
      }

      if (f1->time_advance_offset.has_value()) {
        u.last_ta = f1->time_advance_offset;
      }
    } else if (const auto* f2 = std::get_if<uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu>(&pdu.pdu)) {
      if (f2->ul_sinr_dB.has_value()) {
        handle_pucch_sinr(u, f2->ul_sinr_dB.value());
      }

      if (f2->csi.has_value()) {
        handle_csi_report(u, f2->csi.value());
      }

      if (f2->time_advance_offset.has_value()) {
        u.last_ta = f2->time_advance_offset;
      }
    } else {
      // PUSCH case.
      const auto& pusch = std::get<uci_indication::uci_pdu::uci_pusch_pdu>(pdu.pdu);

      if (pusch.csi.has_value()) {
        handle_csi_report(u, pusch.csi.value());
      }
    }
  }
}

void cell_metrics_handler::handle_ul_bsr_indication(const ul_bsr_indication_message& bsr)
{
  if (ues.contains(bsr.ue_index)) {
    auto& u = ues[bsr.ue_index];

    // Store last BSR.
    u.last_bsr = 0;
    // TODO: Handle different BSR formats.
    for (unsigned i = 0; i != bsr.reported_lcgs.size(); ++i) {
      u.last_bsr += bsr.reported_lcgs[i].nof_bytes;
    }
  }
}

void cell_metrics_handler::handle_ul_phr_indication(const ul_phr_indication_message& phr_ind)
{
  if (ues.contains(phr_ind.ue_index)) {
    auto& u = ues[phr_ind.ue_index];

    // Store last PHR.
    if (not phr_ind.phr.get_phr().empty()) {
      // Log the floor of the average of the PH interval.
      interval<int> rg = phr_ind.phr.get_phr().front().ph;
      u.last_phr       = (rg.start() + rg.stop()) / 2;
    }
  }
}

void cell_metrics_handler::handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& dl_bs)
{
  if (ues.contains(dl_bs.ue_index)) {
    auto& u = ues[dl_bs.ue_index];

    // Store last DL buffer state.
    u.last_dl_bs[dl_bs.lcid] = dl_bs.bs;
  }
}

void cell_metrics_handler::handle_error_indication()
{
  error_indication_counter++;
}

void cell_metrics_handler::report_metrics()
{
  for (ue_metric_context& ue : ues) {
    // Compute statistics of the UE metrics and push the result to the report.
    scheduler_ue_metrics sched_ue_metrics = ue.compute_report(report_period);
    sched_ue_metrics.mean_dl_prbs_used =
        nof_dl_slots > 0 ? static_cast<double>(1.0 * sched_ue_metrics.tot_dl_prbs_used / nof_dl_slots) : 0;
    sched_ue_metrics.mean_ul_prbs_used =
        nof_ul_slots > 0 ? static_cast<double>(1.0 * sched_ue_metrics.tot_ul_prbs_used / nof_ul_slots) : 0;
    next_report.ue_metrics.push_back(sched_ue_metrics);
  }

  next_report.nof_error_indications    = error_indication_counter;
  next_report.average_decision_latency = decision_latency_sum / report_period_slots;
  next_report.latency_histogram        = decision_latency_hist;
  next_report.nof_prbs                 = cell_cfg.nof_dl_prbs; // TODO: to be removed from the report.
  next_report.nof_dl_slots             = nof_dl_slots;
  next_report.nof_ul_slots             = nof_ul_slots;

  // Reset cell-wide metric counters.
  error_indication_counter = 0;
  decision_latency_sum     = std::chrono::microseconds{0};
  decision_latency_hist    = {};
  nof_dl_slots             = 0;
  nof_ul_slots             = 0;

  // Report all UE metrics in a batch.
  notifier.report_metrics(next_report);

  // Clear lists in preparation for the next report.
  next_report.ue_metrics.clear();
  next_report.events.clear();
}

void cell_metrics_handler::handle_slot_result(const sched_result&       slot_result,
                                              std::chrono::microseconds slot_decision_latency)
{
  // Count only full DL/UL slots.
  bool full_dl_slot = (slot_result.dl.nof_dl_symbols == 14);
  bool full_ul_slot = (slot_result.ul.nof_ul_symbols == 14);

  for (const dl_msg_alloc& dl_grant : slot_result.dl.ue_grants) {
    auto it = rnti_to_ue_index_lookup.find(dl_grant.pdsch_cfg.rnti);
    if (it == rnti_to_ue_index_lookup.end()) {
      // UE not found.
      continue;
    }
    ue_metric_context& u = ues[it->second];
    for (const auto& cw : dl_grant.pdsch_cfg.codewords) {
      u.data.dl_mcs += cw.mcs_index.to_uint();
      u.data.nof_dl_cws++;
    }
    if (dl_grant.pdsch_cfg.rbs.is_type0()) {
      u.data.tot_dl_prbs_used += full_dl_slot ? convert_rbgs_to_prbs(dl_grant.pdsch_cfg.rbs.type0(),
                                                                     {0, cell_cfg.nof_dl_prbs},
                                                                     get_nominal_rbg_size(cell_cfg.nof_dl_prbs, true))
                                                    .count()
                                              : 0;
    } else if (dl_grant.pdsch_cfg.rbs.is_type1()) {
      u.data.tot_dl_prbs_used += full_dl_slot ? (dl_grant.pdsch_cfg.rbs.type1().length()) : 0;
    }
  }

  for (const ul_sched_info& ul_grant : slot_result.ul.puschs) {
    auto it = rnti_to_ue_index_lookup.find(ul_grant.pusch_cfg.rnti);
    if (it == rnti_to_ue_index_lookup.end()) {
      // UE not found.
      continue;
    }
    if (ul_grant.pusch_cfg.rbs.is_type0()) {
      ues[it->second].data.tot_ul_prbs_used +=
          full_ul_slot ? convert_rbgs_to_prbs(ul_grant.pusch_cfg.rbs.type0(),
                                              {0, cell_cfg.nof_dl_prbs},
                                              get_nominal_rbg_size(cell_cfg.nof_dl_prbs, true))
                             .count()
                       : 0;
    } else if (ul_grant.pusch_cfg.rbs.is_type1()) {
      ues[it->second].data.tot_ul_prbs_used += full_ul_slot ? (ul_grant.pusch_cfg.rbs.type1().length()) : 0;
    }
    ue_metric_context& u = ues[it->second];
    u.data.ul_mcs += ul_grant.pusch_cfg.mcs_index.to_uint();
    u.data.nof_puschs++;
  }

  nof_dl_slots += full_dl_slot;
  nof_ul_slots += full_ul_slot;

  // Process latency.
  decision_latency_sum += slot_decision_latency;
  unsigned bin_idx = slot_decision_latency.count() / scheduler_cell_metrics::nof_usec_per_bin;
  bin_idx          = std::min(bin_idx, scheduler_cell_metrics::latency_hist_bins - 1);
  decision_latency_hist[bin_idx]++;
}

void cell_metrics_handler::handle_ul_delay(du_ue_index_t ue_index, double delay)
{
  if (ues.contains(ue_index)) {
    ues[ue_index].data.sum_ul_delay_ms += delay;
  }
}

void cell_metrics_handler::push_result(slot_point                sl_tx,
                                       const sched_result&       slot_result,
                                       std::chrono::microseconds slot_decision_latency)
{
  if (report_period_slots == 0) {
    // The SCS common is now known.
    usecs slot_dur      = usecs{1000U >> sl_tx.numerology()};
    report_period_slots = usecs{report_period} / slot_dur;
  }

  last_slot_tx = sl_tx;

  handle_slot_result(slot_result, slot_decision_latency);

  ++slot_counter;
  if (slot_counter >= report_period_slots) {
    report_metrics();
    slot_counter = 0;
  }
}

scheduler_ue_metrics
cell_metrics_handler::ue_metric_context::compute_report(std::chrono::milliseconds metric_report_period)
{
  scheduler_ue_metrics ret{};
  ret.pci              = pci;
  ret.rnti             = rnti;
  ret.cqi_stats        = data.cqi;
  ret.ri_stats         = data.ri;
  uint8_t mcs          = data.nof_dl_cws > 0 ? std::roundf(static_cast<float>(data.dl_mcs) / data.nof_dl_cws) : 0;
  ret.dl_mcs           = sch_mcs_index{mcs};
  mcs                  = data.nof_puschs > 0 ? std::roundf(static_cast<float>(data.ul_mcs) / data.nof_puschs) : 0;
  ret.ul_mcs           = sch_mcs_index{mcs};
  ret.tot_dl_prbs_used = data.tot_dl_prbs_used;
  ret.tot_ul_prbs_used = data.tot_ul_prbs_used;
  ret.dl_brate_kbps    = static_cast<double>(data.sum_dl_tb_bytes * 8U) / metric_report_period.count();
  ret.ul_brate_kbps    = static_cast<double>(data.sum_ul_tb_bytes * 8U) / metric_report_period.count();
  ret.dl_nof_ok        = data.count_uci_harq_acks;
  ret.dl_nof_nok       = data.count_uci_harqs - data.count_uci_harq_acks;
  ret.ul_nof_ok        = data.count_crc_acks;
  ret.ul_nof_nok       = data.count_crc_pdus - data.count_crc_acks;
  ret.pusch_snr_db     = data.nof_pusch_snr_reports > 0 ? data.sum_pusch_snrs / data.nof_pusch_snr_reports : 0;
  ret.pusch_rsrp_db    = data.nof_pusch_rsrp_reports > 0 ? data.sum_pusch_rsrp / data.nof_pusch_rsrp_reports
                                                         : -std::numeric_limits<float>::infinity();
  ret.pucch_snr_db     = data.nof_pucch_snr_reports > 0 ? data.sum_pucch_snrs / data.nof_pucch_snr_reports : 0;
  ret.ul_delay_ms      = data.sum_ul_delay_ms / data.count_crc_pdus;
  ret.bsr              = last_bsr;
  ret.dl_bs            = 0;
  for (const unsigned value : last_dl_bs) {
    ret.dl_bs += value;
  }
  if (last_ta.has_value()) {
    ret.last_ta = last_ta;
  }
  ret.last_phr = last_phr;

  // Reset UE stats metrics on every report.
  reset();

  return ret;
}

void cell_metrics_handler::ue_metric_context::reset()
{
  // Note: for BSR and CQI we just keep the last without resetting the value at every slot.
  data = {};
}

scheduler_metrics_handler::scheduler_metrics_handler(msecs                       metrics_report_period,
                                                     scheduler_metrics_notifier& notifier_) :
  notifier(notifier_), report_period(metrics_report_period)
{
}

cell_metrics_handler* scheduler_metrics_handler::add_cell(const cell_configuration& cell_cfg)
{
  if (cells.contains(cell_cfg.cell_index)) {
    srslog::fetch_basic_logger("SCHED").warning("Cell={} already exists", cell_cfg.cell_index);
    return nullptr;
  }

  cells.emplace(cell_cfg.cell_index, report_period, notifier, cell_cfg);

  return &cells[cell_cfg.cell_index];
}
