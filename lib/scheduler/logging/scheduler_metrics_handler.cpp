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
#include "srsran/ran/resource_allocation/rb_bitmap.h"
#include "srsran/ran/slot_point.h"
#include "srsran/scheduler/result/sched_result.h"
#include "srsran/scheduler/scheduler_configurator.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

namespace {

class null_metrics_notifier final : public scheduler_cell_metrics_notifier
{
private:
  scheduler_cell_metrics& get_next() override { return null_report; }

  void commit(scheduler_cell_metrics& report) override
  {
    // do nothing
    null_report.ue_metrics.clear();
    null_report.events.clear();
  }
  bool is_sched_report_required(slot_point sl_tx) const override { return false; }

  scheduler_cell_metrics null_report{};
};

} // namespace

static null_metrics_notifier null_notifier;

cell_metrics_handler::cell_metrics_handler(
    const cell_configuration&                                                      cell_cfg_,
    const std::optional<sched_cell_configuration_request_message::metrics_config>& metrics_cfg) :
  notifier(metrics_cfg.has_value() and metrics_cfg->notifier != nullptr ? *metrics_cfg->notifier : null_notifier),
  cell_cfg(cell_cfg_),
  nof_slots_per_sf(get_nof_slots_per_subframe(cell_cfg.scs_common))
{
  if (not enabled()) {
    return;
  }

  // Pre-reserve space.
  ues.reserve(MAX_NOF_DU_UES);
  rnti_to_ue_index_lookup.reserve(MAX_NOF_DU_UES);
  const unsigned pre_reserved_event_capacity = std::min(3U * MAX_NOF_DU_UES, metrics_cfg->max_ue_events_per_report);
  pending_events.reserve(pre_reserved_event_capacity);
  unsigned tdd_period_slots =
      cell_cfg.tdd_cfg_common.has_value() ? nof_slots_per_tdd_period(*cell_cfg.tdd_cfg_common) : 0U;
  ul_prbs_used_per_tdd_slot_idx.resize(tdd_period_slots);
  dl_prbs_used_per_tdd_slot_idx.resize(tdd_period_slots);
}

cell_metrics_handler::~cell_metrics_handler() {}

bool cell_metrics_handler::enabled() const
{
  return &notifier != &null_notifier;
}

void cell_metrics_handler::handle_ue_creation(du_ue_index_t ue_index, rnti_t rnti, pci_t pcell_pci)
{
  if (not enabled()) {
    return;
  }

  ues.emplace(ue_index);
  ues[ue_index].rnti     = rnti;
  ues[ue_index].ue_index = ue_index;
  ues[ue_index].pci      = pcell_pci;
  rnti_to_ue_index_lookup.emplace(rnti, ue_index);

  if (pending_events.size() < pending_events.capacity()) {
    pending_events.push_back(scheduler_cell_event{last_slot_tx, rnti, scheduler_cell_event::event_type::ue_add});
  } else {
    data.filtered_events_counter++;
  }
}

void cell_metrics_handler::handle_ue_reconfiguration(du_ue_index_t ue_index)
{
  if (not enabled()) {
    return;
  }
  if (pending_events.size() < pending_events.capacity()) {
    pending_events.push_back(
        scheduler_cell_event{last_slot_tx, ues[ue_index].rnti, scheduler_cell_event::event_type::ue_reconf});
  } else {
    data.filtered_events_counter++;
  }
}

void cell_metrics_handler::handle_ue_deletion(du_ue_index_t ue_index)
{
  if (not enabled()) {
    return;
  }
  if (ues.contains(ue_index)) {
    rnti_t rnti = ues[ue_index].rnti;

    if (pending_events.size() < pending_events.capacity()) {
      pending_events.push_back(scheduler_cell_event{last_slot_tx, rnti, scheduler_cell_event::event_type::ue_rem});
    } else {
      data.filtered_events_counter++;
    }

    rnti_to_ue_index_lookup.erase(rnti);
    ues.erase(ue_index);
  }
}

void cell_metrics_handler::handle_rach_indication(const rach_indication_message& msg, slot_point sl_tx)
{
  if (not enabled()) {
    return;
  }
  unsigned slot_diff = sl_tx - msg.slot_rx;
  for (const auto& occ : msg.occasions) {
    data.nof_prach_preambles += occ.preambles.size();
    data.sum_prach_delay_slots += slot_diff * occ.preambles.size();
  }
}

void cell_metrics_handler::handle_msg3_crc_indication(const ul_crc_pdu_indication& crc_pdu)
{
  if (not enabled()) {
    return;
  }

  if (crc_pdu.tb_crc_success) {
    data.nof_msg3_ok++;
  } else {
    data.nof_msg3_nok++;
  }
}

void cell_metrics_handler::handle_crc_indication(slot_point                   sl_rx,
                                                 const ul_crc_pdu_indication& crc_pdu,
                                                 units::bytes                 tbs)
{
  if (not enabled()) {
    return;
  }
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
  if (not enabled()) {
    return;
  }
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
  if (not enabled()) {
    return;
  }
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

void cell_metrics_handler::handle_sr_indication(du_ue_index_t ue_index, slot_point sr_slot)
{
  if (ues.contains(ue_index)) {
    auto& u = ues[ue_index];
    if (not u.data.last_sr_slot.valid()) {
      u.data.last_sr_slot = sr_slot;
    }
    ++u.data.count_sr;
  }
}

void cell_metrics_handler::handle_ul_bsr_indication(const ul_bsr_indication_message& bsr)
{
  if (not enabled()) {
    return;
  }
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
  if (not enabled()) {
    return;
  }
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
  if (not enabled()) {
    return;
  }
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

void cell_metrics_handler::handle_late_dl_harqs()
{
  ++data.nof_failed_pdsch_allocs_late_harqs;
}

void cell_metrics_handler::handle_late_ul_harqs()
{
  ++data.nof_failed_pusch_allocs_late_harqs;
}

void cell_metrics_handler::report_metrics()
{
  auto next_report = notifier.get_builder();

  const unsigned                  report_period_slots = last_slot_tx - start_report_slot_tx;
  const std::chrono::milliseconds report_period{report_period_slots / last_slot_tx.nof_slots_per_subframe()};
  for (ue_metric_context& ue : ues) {
    // Compute statistics of the UE metrics and push the result to the report.
    next_report->ue_metrics.push_back(ue.compute_report(report_period, nof_slots_per_sf));
  }
  next_report->events.swap(pending_events);

  next_report->pci                       = cell_cfg.pci;
  next_report->slot                      = start_report_slot_tx;
  next_report->nof_slots                 = report_period_slots;
  next_report->nof_error_indications     = data.error_indication_counter;
  next_report->average_decision_latency  = data.decision_latency_sum / next_report->nof_slots;
  next_report->max_decision_latency      = data.max_decision_latency;
  next_report->max_decision_latency_slot = data.max_decision_latency_slot;
  next_report->latency_histogram         = data.decision_latency_hist;
  next_report->nof_prbs                  = cell_cfg.nof_dl_prbs; // TODO: to be removed from the report.
  next_report->nof_dl_slots              = data.nof_dl_slots;
  next_report->nof_ul_slots              = data.nof_ul_slots;
  next_report->nof_prach_preambles       = data.nof_prach_preambles;
  next_report->dl_grants_count           = data.nof_ue_pdsch_grants;
  next_report->ul_grants_count           = data.nof_ue_pusch_grants;
  next_report->nof_failed_pdcch_allocs   = data.nof_failed_pdcch_allocs;
  next_report->nof_failed_uci_allocs     = data.nof_failed_uci_allocs;
  next_report->nof_msg3_ok               = data.nof_msg3_ok;
  next_report->nof_msg3_nok              = data.nof_msg3_nok;
  next_report->avg_prach_delay_slots =
      data.nof_prach_preambles
          ? std::optional{static_cast<float>(data.sum_prach_delay_slots) / static_cast<float>(data.nof_prach_preambles)}
          : std::nullopt;
  next_report->nof_failed_pdsch_allocs_late_harqs = data.nof_failed_pdsch_allocs_late_harqs;
  next_report->nof_failed_pusch_allocs_late_harqs = data.nof_failed_pusch_allocs_late_harqs;
  next_report->nof_filtered_events                = data.filtered_events_counter;
  // Note: PUCCH is only allocated on full UL slots.
  next_report->pucch_tot_rb_usage_avg = static_cast<float>(data.pucch_rbs_used) / data.nof_ul_slots;
  for (unsigned rb_count : ul_prbs_used_per_tdd_slot_idx) {
    next_report->pusch_prbs_used_per_tdd_slot_idx.push_back(rb_count);
  }
  for (unsigned rb_count : dl_prbs_used_per_tdd_slot_idx) {
    next_report->pdsch_prbs_used_per_tdd_slot_idx.push_back(rb_count);
  }

  // Reset cell-wide metric counters.
  data                 = {};
  start_report_slot_tx = last_slot_tx;

  // Clear the PRB vectors for the next report.
  for (unsigned& rb_count : ul_prbs_used_per_tdd_slot_idx) {
    rb_count = 0;
  }
  for (unsigned& rb_count : dl_prbs_used_per_tdd_slot_idx) {
    rb_count = 0;
  }

  // Report all UE metrics in a batch.
  // Note: next_report will be reset afterwards. However, we prefer to first commit before fetching a new report.
  next_report.reset();
}

void cell_metrics_handler::handle_slot_result(const sched_result&       slot_result,
                                              std::chrono::microseconds slot_decision_latency)
{
  if (not enabled()) {
    return;
  }

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

    unsigned grant_prbs;
    if (dl_grant.pdsch_cfg.rbs.is_type0()) {
      grant_prbs = convert_rbgs_to_prbs(dl_grant.pdsch_cfg.rbs.type0(),
                                        {0, cell_cfg.nof_dl_prbs},
                                        get_nominal_rbg_size(cell_cfg.nof_dl_prbs, true))
                       .count();
    } else {
      grant_prbs = (dl_grant.pdsch_cfg.rbs.type1().length());
    }
    u.data.tot_dl_prbs_used += grant_prbs;
    if (dl_prbs_used_per_tdd_slot_idx.size()) {
      dl_prbs_used_per_tdd_slot_idx[last_slot_tx.count() % dl_prbs_used_per_tdd_slot_idx.size()] += grant_prbs;
    }
    u.last_dl_olla = dl_grant.context.olla_offset;
    if (u.data.last_pdsch_slot.valid()) {
      u.data.max_pdsch_distance_slots =
          std::max(static_cast<unsigned>(last_slot_tx - u.data.last_pdsch_slot), u.data.max_pdsch_distance_slots);
    }
    u.data.last_pdsch_slot = last_slot_tx;
  }

  data.nof_ue_pusch_grants += slot_result.ul.puschs.size();
  for (const ul_sched_info& ul_grant : slot_result.ul.puschs) {
    auto it = rnti_to_ue_index_lookup.find(ul_grant.pusch_cfg.rnti);
    if (it == rnti_to_ue_index_lookup.end()) {
      // UE not found.
      continue;
    }
    unsigned grant_prbs;
    if (ul_grant.pusch_cfg.rbs.is_type0()) {
      grant_prbs = convert_rbgs_to_prbs(ul_grant.pusch_cfg.rbs.type0(),
                                        {0, cell_cfg.nof_dl_prbs},
                                        get_nominal_rbg_size(cell_cfg.nof_dl_prbs, true))
                       .count();
    } else {
      grant_prbs = (ul_grant.pusch_cfg.rbs.type1().length());
    }
    ues[it->second].data.tot_ul_prbs_used += grant_prbs;
    if (ul_prbs_used_per_tdd_slot_idx.size()) {
      ul_prbs_used_per_tdd_slot_idx[last_slot_tx.count() % ul_prbs_used_per_tdd_slot_idx.size()] += grant_prbs;
    }
    ue_metric_context& u = ues[it->second];
    u.data.ul_mcs += ul_grant.pusch_cfg.mcs_index.to_uint();
    u.last_ul_olla = ul_grant.context.olla_offset;
    if (u.data.last_sr_slot.valid()) {
      unsigned sr_to_pusch_delay = last_slot_tx - u.data.last_sr_slot;
      u.data.sum_sr_to_pusch_delay_slots += sr_to_pusch_delay;
      u.data.max_sr_to_pusch_delay_slots = std::max(sr_to_pusch_delay, u.data.max_sr_to_pusch_delay_slots);
      u.data.last_sr_slot.clear();
      u.data.count_handled_sr++;
    }
    ++u.data.nof_puschs;
    if (u.data.last_pusch_slot.valid()) {
      u.data.max_pusch_distance_slots =
          std::max(static_cast<unsigned>(last_slot_tx - u.data.last_pusch_slot), u.data.max_pusch_distance_slots);
    }
    u.data.last_pusch_slot = last_slot_tx;
  }

  // PUCCH resource usage.
  prb_bitmap pucch_prbs(cell_cfg.nof_ul_prbs);
  for (const auto& pucch : slot_result.ul.pucchs) {
    // Mark the PRBs used by this PUCCH.
    pucch_prbs.fill(pucch.resources.prbs.start(), pucch.resources.prbs.stop());
    pucch_prbs.fill(pucch.resources.second_hop_prbs.start(), pucch.resources.second_hop_prbs.stop());
  }
  data.pucch_rbs_used += pucch_prbs.count();

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

  // Failed allocation attempts.
  data.nof_failed_pdcch_allocs += slot_result.failed_attempts.pdcch;
  data.nof_failed_uci_allocs += slot_result.failed_attempts.uci;
}

void cell_metrics_handler::push_result(slot_point                sl_tx,
                                       const sched_result&       slot_result,
                                       std::chrono::microseconds slot_decision_latency)
{
  if (not enabled()) {
    return;
  }
  if (SRSRAN_UNLIKELY(not last_slot_tx.valid())) {
    start_report_slot_tx = sl_tx - 1;
  }
  last_slot_tx = sl_tx;

  handle_slot_result(slot_result, slot_decision_latency);

  if (notifier.is_sched_report_required(sl_tx)) {
    // Prepare report and forward it to the notifier.
    report_metrics();
  }
}

void cell_metrics_handler::handle_cell_deactivation()
{
  // Commit whatever is pending for the report.
  report_metrics();
}

scheduler_ue_metrics
cell_metrics_handler::ue_metric_context::compute_report(std::chrono::milliseconds metric_report_period,
                                                        unsigned                  slots_per_sf)
{
  auto convert_slots_to_ms = [slots_per_sf](unsigned slots) {
    return static_cast<float>(slots) / static_cast<float>(slots_per_sf);
  };
  scheduler_ue_metrics ret{};
  ret.ue_index            = ue_index;
  ret.pci                 = pci;
  ret.rnti                = rnti;
  ret.cqi_stats           = data.cqi;
  ret.dl_ri_stats         = data.dl_ri;
  uint8_t mcs             = data.nof_dl_cws > 0 ? std::round(static_cast<float>(data.dl_mcs) / data.nof_dl_cws) : 0;
  ret.dl_mcs              = sch_mcs_index{mcs};
  mcs                     = data.nof_puschs > 0 ? std::round(static_cast<float>(data.ul_mcs) / data.nof_puschs) : 0;
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
  ret.max_pdsch_distance_ms          = convert_slots_to_ms(data.max_pdsch_distance_slots);
  ret.max_pusch_distance_ms          = convert_slots_to_ms(data.max_pusch_distance_slots);
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
  if (data.count_pusch_harq_pdus > 0) {
    ret.avg_pusch_harq_delay_ms =
        convert_slots_to_ms(data.sum_pusch_harq_delay_slots) / static_cast<float>(data.count_pusch_harq_pdus);
    ret.max_pusch_harq_delay_ms = convert_slots_to_ms(data.max_pusch_harq_delay_slots);
  }
  if (data.count_pucch_harq_pdus > 0) {
    ret.avg_pucch_harq_delay_ms =
        convert_slots_to_ms(data.sum_pucch_harq_delay_slots) / static_cast<float>(data.count_pucch_harq_pdus);
    ret.max_pucch_harq_delay_ms = convert_slots_to_ms(data.max_pucch_harq_delay_slots);
  }
  if (data.count_handled_sr > 0) {
    ret.avg_sr_to_pusch_delay_ms =
        convert_slots_to_ms(data.sum_sr_to_pusch_delay_slots) / static_cast<float>(data.count_handled_sr);
    ret.max_sr_to_pusch_delay_ms = convert_slots_to_ms(data.max_sr_to_pusch_delay_slots);
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

cell_metrics_handler* scheduler_metrics_handler::add_cell(
    const cell_configuration&                                                      cell_cfg,
    const std::optional<sched_cell_configuration_request_message::metrics_config>& metrics_cfg)
{
  if (cells.contains(cell_cfg.cell_index)) {
    srslog::fetch_basic_logger("SCHED").warning("Cell={} already exists", fmt::underlying(cell_cfg.cell_index));
    return nullptr;
  }

  cells.emplace(cell_cfg.cell_index, std::make_unique<cell_metrics_handler>(cell_cfg, metrics_cfg));

  return cells[cell_cfg.cell_index].get();
}

void scheduler_metrics_handler::rem_cell(du_cell_index_t cell_index)
{
  cells.erase(cell_index);
}
