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

#include "scheduler_metrics_handler.h"
#include "../config/cell_configuration.h"

#include "srsran/scheduler/result/sched_result.h"
#include "srsran/scheduler/scheduler_configurator.h"
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

void cell_metrics_handler::handle_rach_indication(const rach_indication_message& msg)
{
  for (auto& occ : msg.occasions) {
    data.nof_prach_preambles += occ.preambles.size();
  }
}

void cell_metrics_handler::handle_crc_indication(slot_point                   sl_rx,
                                                 const ul_crc_pdu_indication& crc_pdu,
                                                 units::bytes                 tbs)
{
  if (ues.contains(crc_pdu.ue_index)) {
    auto& u = ues[crc_pdu.ue_index];
    u.data.count_crc_acks += crc_pdu.tb_crc_success ? 1 : 0;
    ++u.data.count_crc_pdus;
    if (crc_pdu.ul_sinr_dB.has_value()) {
      ++u.data.nof_pusch_snr_reports;
      u.data.sum_pusch_snrs += crc_pdu.ul_sinr_dB.value();
    }
    if (crc_pdu.ul_rsrp_dBFS.has_value()) {
      ++u.data.nof_pusch_rsrp_reports;
      u.data.sum_pusch_rsrp += crc_pdu.ul_rsrp_dBFS.value();
    }
    if (crc_pdu.tb_crc_success) {
      u.data.sum_ul_tb_bytes += tbs.value();
    }
    if (crc_pdu.time_advance_offset.has_value()) {
      u.data.ta.update(crc_pdu.time_advance_offset.value().to_seconds());
      u.data.pusch_ta.update(crc_pdu.time_advance_offset.value().to_seconds());
    }
    u.data.sum_crc_delay_slots += last_slot_tx - sl_rx;
    u.data.max_crc_delay_slots = std::max(static_cast<unsigned>(last_slot_tx - sl_rx), u.data.max_crc_delay_slots);
  }
}

void cell_metrics_handler::handle_srs_indication(const srs_indication::srs_indication_pdu& srs_pdu, unsigned ri)
{
  if (ues.contains(srs_pdu.ue_index)) {
    auto& u = ues[srs_pdu.ue_index];
    if (srs_pdu.time_advance_offset.has_value()) {
      u.data.ta.update(srs_pdu.time_advance_offset.value().to_seconds());
      u.data.srs_ta.update(srs_pdu.time_advance_offset.value().to_seconds());
      u.data.ul_ri.update(ri);
    }
  }
}

void cell_metrics_handler::handle_pucch_sinr(ue_metric_context& u, float sinr)
{
  ++u.data.nof_pucch_snr_reports;
  u.data.sum_pucch_snrs += sinr;
}

void cell_metrics_handler::handle_csi_report(ue_metric_context& u, const csi_report_data& csi)
{
  // Add new CQI and RI observations if they are available in the CSI report.
  if (csi.first_tb_wideband_cqi.has_value()) {
    u.data.cqi.update(csi.first_tb_wideband_cqi->to_uint());
  }
  if (csi.ri.has_value()) {
    u.data.dl_ri.update(csi.ri->to_uint());
  }
}

void cell_metrics_handler::handle_uci_with_harq_ack(du_ue_index_t ue_index, slot_point sl_rx, bool pucch)
{
  if (ues.contains(ue_index)) {
    auto& u = ues[ue_index];
    if (pucch) {
      u.data.sum_pucch_harq_delay_slots += last_slot_tx - sl_rx;
      u.data.max_pucch_harq_delay_slots =
          std::max(static_cast<unsigned>(last_slot_tx - sl_rx), u.data.max_pucch_harq_delay_slots);
      ++u.data.count_pucch_harq_pdus;
    } else {
      u.data.sum_pusch_harq_delay_slots += last_slot_tx - sl_rx;
      u.data.max_pusch_harq_delay_slots =
          std::max(static_cast<unsigned>(last_slot_tx - sl_rx), u.data.max_pusch_harq_delay_slots);
      ++u.data.count_pusch_harq_pdus;
    }
  }
}

void cell_metrics_handler::handle_dl_harq_ack(du_ue_index_t ue_index, bool ack, units::bytes tbs)
{
  if (ues.contains(ue_index)) {
    auto& u = ues[ue_index];
    u.data.count_uci_harq_acks += ack ? 1 : 0;
    ++u.data.count_uci_harqs;
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
      ++u.data.count_uci_harqs;
    } else {
      ++u.data.count_crc_pdus;
    }
  }
}

void cell_metrics_handler::handle_uci_pdu_indication(const uci_indication::uci_pdu& pdu, bool is_sr_opportunity_and_f1)
{
  if (ues.contains(pdu.ue_index)) {
    auto& u = ues[pdu.ue_index];

    if (const auto* f0f1 = std::get_if<uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu>(&pdu.pdu)) {
      if (f0f1->ul_sinr_dB.has_value()) {
        handle_pucch_sinr(u, f0f1->ul_sinr_dB.value());
      }

      if (f0f1->time_advance_offset.has_value()) {
        u.data.ta.update(f0f1->time_advance_offset->to_seconds());
        u.data.pucch_ta.update(f0f1->time_advance_offset->to_seconds());
      }

      if (not is_sr_opportunity_and_f1 and not f0f1->harqs.empty() and
          f0f1->harqs[0] == mac_harq_ack_report_status::dtx) {
        ++u.data.nof_pucch_f0f1_invalid_harqs;
      }
    } else if (const auto* f2f3f4 = std::get_if<uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu>(&pdu.pdu)) {
      if (f2f3f4->ul_sinr_dB.has_value()) {
        handle_pucch_sinr(u, f2f3f4->ul_sinr_dB.value());
      }

      if (f2f3f4->csi.has_value()) {
        if (f2f3f4->csi->valid) {
          handle_csi_report(u, f2f3f4->csi.value());
        } else {
          ++u.data.nof_pucch_f2f3f4_invalid_csis;
        }
      }

      if (f2f3f4->time_advance_offset.has_value()) {
        u.data.ta.update(f2f3f4->time_advance_offset->to_seconds());
        u.data.pucch_ta.update(f2f3f4->time_advance_offset->to_seconds());
      }

      if (not f2f3f4->harqs.empty() and f2f3f4->harqs[0] == mac_harq_ack_report_status::dtx) {
        ++u.data.nof_pucch_f2f3f4_invalid_harqs;
      }
    } else {
      // PUSCH case.
      const auto& pusch = std::get<uci_indication::uci_pdu::uci_pusch_pdu>(pdu.pdu);

      if (pusch.csi.has_value()) {
        if (pusch.csi->valid) {
          handle_csi_report(u, pusch.csi.value());
        } else {
          ++u.data.nof_pusch_invalid_csis;
        }
      }
      if (not pusch.harqs.empty() and pusch.harqs[0] == mac_harq_ack_report_status::dtx) {
        ++u.data.nof_pusch_invalid_harqs;
      }
    }
  }
}

void cell_metrics_handler::handle_sr_indication(du_ue_index_t ue_index)
{
  if (ues.contains(ue_index)) {
    auto& u = ues[ue_index];
    ++u.data.count_sr;
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
      u.data.sum_ul_ce_delay_slots += last_slot_tx - phr_ind.slot_rx;
      u.data.max_ul_ce_delay_slots =
          std::max(static_cast<unsigned>(last_slot_tx - phr_ind.slot_rx), u.data.max_ul_ce_delay_slots);
      ++u.data.nof_ul_ces;
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
  ++data.error_indication_counter;
}

void cell_metrics_handler::report_metrics()
{
  for (ue_metric_context& ue : ues) {
    // Compute statistics of the UE metrics and push the result to the report.
    next_report.ue_metrics.push_back(ue.compute_report(report_period, nof_slots_per_sf));
  }

  next_report.nof_slots                 = report_period_slots;
  next_report.nof_error_indications     = data.error_indication_counter;
  next_report.average_decision_latency  = data.decision_latency_sum / report_period_slots;
  next_report.max_decision_latency      = data.max_decision_latency;
  next_report.max_decision_latency_slot = data.max_decision_latency_slot;
  next_report.latency_histogram         = data.decision_latency_hist;
  next_report.nof_prbs                  = cell_cfg.nof_dl_prbs; // TODO: to be removed from the report.
  next_report.nof_dl_slots              = data.nof_dl_slots;
  next_report.nof_ul_slots              = data.nof_ul_slots;
  next_report.nof_prach_preambles       = data.nof_prach_preambles;
  next_report.dl_grants_count           = data.nof_ue_pdsch_grants;
  next_report.ul_grants_count           = data.nof_ue_pusch_grants;

  // Reset cell-wide metric counters.
  data = {};

  // Report all UE metrics in a batch.
  notifier.report_metrics(next_report);

  // Clear lists in preparation for the next report.
  next_report.ue_metrics.clear();
  next_report.events.clear();
}

void cell_metrics_handler::handle_slot_result(const sched_result&       slot_result,
                                              std::chrono::microseconds slot_decision_latency)
{
  data.nof_ue_pdsch_grants += slot_result.dl.ue_grants.size();
  for (const dl_msg_alloc& dl_grant : slot_result.dl.ue_grants) {
    auto it = rnti_to_ue_index_lookup.find(dl_grant.pdsch_cfg.rnti);
    if (it == rnti_to_ue_index_lookup.end()) {
      // UE not found.
      continue;
    }
    ue_metric_context& u = ues[it->second];
    for (const auto& cw : dl_grant.pdsch_cfg.codewords) {
      u.data.dl_mcs += cw.mcs_index.to_uint();
      ++u.data.nof_dl_cws;
    }
    if (dl_grant.pdsch_cfg.rbs.is_type0()) {
      u.data.tot_dl_prbs_used += convert_rbgs_to_prbs(dl_grant.pdsch_cfg.rbs.type0(),
                                                      {0, cell_cfg.nof_dl_prbs},
                                                      get_nominal_rbg_size(cell_cfg.nof_dl_prbs, true))
                                     .count();
    } else if (dl_grant.pdsch_cfg.rbs.is_type1()) {
      u.data.tot_dl_prbs_used += (dl_grant.pdsch_cfg.rbs.type1().length());
    }
    u.last_dl_olla = dl_grant.context.olla_offset;
  }

  data.nof_ue_pusch_grants += slot_result.ul.puschs.size();
  for (const ul_sched_info& ul_grant : slot_result.ul.puschs) {
    auto it = rnti_to_ue_index_lookup.find(ul_grant.pusch_cfg.rnti);
    if (it == rnti_to_ue_index_lookup.end()) {
      // UE not found.
      continue;
    }
    if (ul_grant.pusch_cfg.rbs.is_type0()) {
      ues[it->second].data.tot_ul_prbs_used += convert_rbgs_to_prbs(ul_grant.pusch_cfg.rbs.type0(),
                                                                    {0, cell_cfg.nof_dl_prbs},
                                                                    get_nominal_rbg_size(cell_cfg.nof_dl_prbs, true))
                                                   .count();
    } else if (ul_grant.pusch_cfg.rbs.is_type1()) {
      ues[it->second].data.tot_ul_prbs_used += (ul_grant.pusch_cfg.rbs.type1().length());
    }
    ue_metric_context& u = ues[it->second];
    u.data.ul_mcs += ul_grant.pusch_cfg.mcs_index.to_uint();
    u.last_ul_olla = ul_grant.context.olla_offset;
    ++u.data.nof_puschs;
  }

  // Count DL and UL slots.
  data.nof_dl_slots += (slot_result.dl.nof_dl_symbols > 0);
  data.nof_ul_slots += (slot_result.ul.nof_ul_symbols == 14); // Note: PUSCH in special slot not supported.;

  // Process latency.
  data.decision_latency_sum += slot_decision_latency;
  if (data.max_decision_latency < slot_decision_latency) {
    data.max_decision_latency      = slot_decision_latency;
    data.max_decision_latency_slot = last_slot_tx;
  }
  unsigned bin_idx = slot_decision_latency.count() / scheduler_cell_metrics::nof_usec_per_bin;
  bin_idx          = std::min(bin_idx, scheduler_cell_metrics::latency_hist_bins - 1);
  ++data.decision_latency_hist[bin_idx];
}

void cell_metrics_handler::push_result(slot_point                sl_tx,
                                       const sched_result&       slot_result,
                                       std::chrono::microseconds slot_decision_latency)
{
  if (SRSRAN_UNLIKELY(not next_report_slot.valid())) {
    // We enter here in the first call to this function.
    // We will make the \c next_report_slot aligned with the period.
    nof_slots_per_sf    = get_nof_slots_per_subframe(to_subcarrier_spacing(sl_tx.numerology()));
    usecs slot_dur      = usecs{1000U >> sl_tx.numerology()};
    report_period_slots = usecs{report_period} / slot_dur;
    unsigned mod_val    = sl_tx.to_uint() % report_period_slots;
    next_report_slot    = mod_val > 0 ? sl_tx + report_period_slots - mod_val : sl_tx;
  }
  last_slot_tx = sl_tx;

  handle_slot_result(slot_result, slot_decision_latency);

  if (sl_tx >= next_report_slot) {
    // Prepare report and forward it to the notifier.
    report_metrics();

    // Set next report slot.
    next_report_slot += report_period_slots;
  }
}

scheduler_ue_metrics
cell_metrics_handler::ue_metric_context::compute_report(std::chrono::milliseconds metric_report_period,
                                                        unsigned                  slots_per_sf)
{
  auto convert_slots_to_ms = [slots_per_sf](unsigned slots) {
    return static_cast<float>(slots) / static_cast<float>(slots_per_sf);
  };
  scheduler_ue_metrics ret{};
  ret.pci                 = pci;
  ret.rnti                = rnti;
  ret.cqi_stats           = data.cqi;
  ret.dl_ri_stats         = data.dl_ri;
  uint8_t mcs             = data.nof_dl_cws > 0 ? std::roundf(static_cast<float>(data.dl_mcs) / data.nof_dl_cws) : 0;
  ret.dl_mcs              = sch_mcs_index{mcs};
  mcs                     = data.nof_puschs > 0 ? std::roundf(static_cast<float>(data.ul_mcs) / data.nof_puschs) : 0;
  ret.ul_mcs              = sch_mcs_index{mcs};
  ret.tot_pdsch_prbs_used = data.tot_dl_prbs_used;
  ret.tot_pusch_prbs_used = data.tot_ul_prbs_used;
  ret.dl_brate_kbps       = static_cast<double>(data.sum_dl_tb_bytes * 8U) / metric_report_period.count();
  ret.ul_brate_kbps       = static_cast<double>(data.sum_ul_tb_bytes * 8U) / metric_report_period.count();
  ret.dl_nof_ok           = data.count_uci_harq_acks;
  ret.dl_nof_nok          = data.count_uci_harqs - data.count_uci_harq_acks;
  ret.ul_nof_ok           = data.count_crc_acks;
  ret.ul_nof_nok          = data.count_crc_pdus - data.count_crc_acks;
  ret.pusch_snr_db        = data.nof_pusch_snr_reports > 0 ? data.sum_pusch_snrs / data.nof_pusch_snr_reports : 0;
  ret.pusch_rsrp_db       = data.nof_pusch_rsrp_reports > 0 ? data.sum_pusch_rsrp / data.nof_pusch_rsrp_reports
                                                            : -std::numeric_limits<float>::infinity();
  ret.ul_ri_stats         = data.ul_ri;
  ret.pucch_snr_db        = data.nof_pucch_snr_reports > 0 ? data.sum_pucch_snrs / data.nof_pucch_snr_reports : 0;
  ret.last_dl_olla        = last_dl_olla;
  ret.last_ul_olla        = last_ul_olla;
  ret.bsr                 = last_bsr;
  ret.sr_count            = data.count_sr;
  ret.dl_bs               = 0;
  for (const unsigned value : last_dl_bs) {
    ret.dl_bs += value;
  }
  ret.ta_stats                       = data.ta;
  ret.pusch_ta_stats                 = data.pusch_ta;
  ret.pucch_ta_stats                 = data.pucch_ta;
  ret.srs_ta_stats                   = data.srs_ta;
  ret.last_phr                       = last_phr;
  ret.nof_pucch_f0f1_invalid_harqs   = data.nof_pucch_f0f1_invalid_harqs;
  ret.nof_pucch_f2f3f4_invalid_harqs = data.nof_pucch_f2f3f4_invalid_harqs;
  ret.nof_pucch_f2f3f4_invalid_harqs = data.nof_pucch_f2f3f4_invalid_harqs;
  ret.nof_pucch_f2f3f4_invalid_csis  = data.nof_pucch_f2f3f4_invalid_csis;
  ret.nof_pusch_invalid_harqs        = data.nof_pusch_invalid_harqs;
  ret.nof_pusch_invalid_csis         = data.nof_pusch_invalid_csis;
  if (data.nof_ul_ces > 0) {
    ret.avg_ce_delay_ms = convert_slots_to_ms(data.sum_ul_ce_delay_slots) / static_cast<float>(data.nof_ul_ces);
    ret.max_ce_delay_ms = convert_slots_to_ms(data.max_ul_ce_delay_slots);
  }
  if (data.count_crc_pdus > 0) {
    ret.avg_crc_delay_ms = convert_slots_to_ms(data.sum_crc_delay_slots) / static_cast<float>(data.count_crc_pdus);
    ret.max_crc_delay_ms = convert_slots_to_ms(data.max_crc_delay_slots);
  }
  if (data.count_pucch_harq_pdus > 0) {
    ret.avg_pucch_harq_delay_ms =
        convert_slots_to_ms(data.sum_pucch_harq_delay_slots) / static_cast<float>(data.count_pucch_harq_pdus);
    ret.max_pucch_harq_delay_ms = convert_slots_to_ms(data.max_pucch_harq_delay_slots);
  }
  if (data.count_pusch_harq_pdus > 0) {
    ret.avg_pusch_harq_delay_ms =
        convert_slots_to_ms(data.sum_pusch_harq_delay_slots) / static_cast<float>(data.count_pusch_harq_pdus);
    ret.max_pusch_harq_delay_ms = convert_slots_to_ms(data.max_pusch_harq_delay_slots);
  }

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
    srslog::fetch_basic_logger("SCHED").warning("Cell={} already exists", fmt::underlying(cell_cfg.cell_index));
    return nullptr;
  }

  cells.emplace(cell_cfg.cell_index, std::make_unique<cell_metrics_handler>(report_period, notifier, cell_cfg));

  return cells[cell_cfg.cell_index].get();
}
