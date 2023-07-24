/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

using namespace srsran;

scheduler_metrics_handler::scheduler_metrics_handler(msecs                          metrics_report_period,
                                                     scheduler_ue_metrics_notifier& notifier_) :
  notifier(notifier_), report_period(metrics_report_period)
{
}

void scheduler_metrics_handler::handle_ue_creation(du_ue_index_t ue_index, rnti_t rnti, pci_t pcell_pci)
{
  ues.emplace(ue_index);
  ues[ue_index].rnti     = rnti;
  ues[ue_index].ue_index = ue_index;
  ues[ue_index].pci      = pcell_pci;
  rnti_to_ue_index_lookup.emplace(rnti, ue_index);
}

void scheduler_metrics_handler::handle_ue_deletion(du_ue_index_t ue_index)
{
  if (ues.contains(ue_index)) {
    rnti_to_ue_index_lookup.erase(ues[ue_index].rnti);
    ues.erase(ue_index);
  }
}

void scheduler_metrics_handler::handle_crc_indication(const ul_crc_pdu_indication& crc_pdu, units::bytes tbs)
{
  if (ues.contains(crc_pdu.ue_index)) {
    auto& u = ues[crc_pdu.ue_index];
    u.data.count_crc_acks += crc_pdu.tb_crc_success ? 1 : 0;
    u.data.count_crc_pdus++;
    if (crc_pdu.ul_sinr_metric.has_value()) {
      u.data.nof_pusch_snr_reports++;
      u.data.sum_pusch_snrs += crc_pdu.ul_sinr_metric.value();
    }
    if (crc_pdu.tb_crc_success) {
      u.data.sum_ul_tb_bytes += tbs.value();
    }
  }
}

void scheduler_metrics_handler::handle_csi_report(du_ue_index_t ue_index, const csi_report_data& csi)
{
  if (ues.contains(ue_index)) {
    auto& u = ues[ue_index];
    if (csi.first_tb_wideband_cqi.has_value()) {
      u.last_cqi = csi.first_tb_wideband_cqi->to_uint();
    }
    if (csi.ri.has_value()) {
      u.last_ri = csi.ri->to_uint();
    }
  }
}

void scheduler_metrics_handler::handle_dl_harq_ack(du_ue_index_t ue_index, bool ack, units::bytes tbs)
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

void scheduler_metrics_handler::handle_harq_timeout(du_ue_index_t ue_index, bool is_dl)
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

void scheduler_metrics_handler::handle_pucch_sinr(du_ue_index_t ue_index, optional<float> pucch_sinr)
{
  if (ues.contains(ue_index)) {
    auto& u = ues[ue_index];
    if (pucch_sinr.has_value()) {
      u.data.nof_pucch_snr_reports++;
      u.data.sum_pucch_snrs += pucch_sinr.value();
    }
  }
}

void scheduler_metrics_handler::handle_ul_bsr_indication(const ul_bsr_indication_message& bsr)
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

void scheduler_metrics_handler::handle_ul_phr_indication(const ul_phr_indication_message& phr_ind)
{
  if (ues.contains(phr_ind.ue_index)) {
    auto& u = ues[phr_ind.ue_index];

    // Store last PHR.
    u.last_phr = phr_ind.phr;
  }
}

void scheduler_metrics_handler::report_metrics()
{
  static_vector<scheduler_ue_metrics, MAX_NOF_DU_UES> metrics_report;

  for (ue_metric_context& ue : ues) {
    // Compute statistics of the UE metrics and push the result to the report.
    metrics_report.push_back(ue.compute_report(report_period));
  }

  // Report all UE metrics in a batch.
  notifier.report_metrics(metrics_report);
}

void scheduler_metrics_handler::handle_slot_result(const sched_result& slot_result)
{
  for (const dl_msg_alloc& dl_grant : slot_result.dl.ue_grants) {
    auto it = rnti_to_ue_index_lookup.find(dl_grant.pdsch_cfg.rnti);
    if (it == rnti_to_ue_index_lookup.end()) {
      // UE not found.
      continue;
    }
    ue_metric_context& u = ues[it->second];
    for (auto& cw : dl_grant.pdsch_cfg.codewords) {
      u.data.dl_mcs += cw.mcs_index.to_uint();
      u.data.nof_dl_cws++;
    }
  }

  for (const ul_sched_info& ul_grant : slot_result.ul.puschs) {
    auto it = rnti_to_ue_index_lookup.find(ul_grant.pusch_cfg.rnti);
    if (it == rnti_to_ue_index_lookup.end()) {
      // UE not found.
      continue;
    }
    ue_metric_context& u = ues[it->second];
    u.data.ul_mcs += ul_grant.pusch_cfg.mcs_index.to_uint();
    u.data.nof_puschs++;
  }
}

void scheduler_metrics_handler::push_result(slot_point sl_tx, const sched_result& slot_result)
{
  if (report_period_slots == 0) {
    // The SCS common is now known.
    usecs slot_dur      = usecs{1000U >> sl_tx.numerology()};
    report_period_slots = usecs{report_period} / slot_dur;
  }

  handle_slot_result(slot_result);

  ++slot_counter;
  if (slot_counter >= report_period_slots) {
    report_metrics();
    slot_counter = 0;
  }
}

scheduler_ue_metrics
scheduler_metrics_handler::ue_metric_context::compute_report(std::chrono::milliseconds metric_report_period)
{
  scheduler_ue_metrics ret{};
  ret.pci           = pci;
  ret.rnti          = rnti;
  ret.cqi           = last_cqi;
  ret.ri            = last_ri;
  uint8_t mcs       = data.nof_dl_cws > 0 ? std::roundf(static_cast<float>(data.dl_mcs) / data.nof_dl_cws) : 0;
  ret.dl_mcs        = sch_mcs_index{mcs};
  mcs               = data.nof_puschs > 0 ? std::roundf(static_cast<float>(data.ul_mcs) / data.nof_puschs) : 0;
  ret.ul_mcs        = sch_mcs_index{mcs};
  ret.dl_brate_kbps = static_cast<double>(data.sum_dl_tb_bytes * 8U) / metric_report_period.count();
  ret.ul_brate_kbps = static_cast<double>(data.sum_ul_tb_bytes * 8U) / metric_report_period.count();
  ret.dl_nof_ok     = data.count_uci_harq_acks;
  ret.dl_nof_nok    = data.count_uci_harqs - data.count_uci_harq_acks;
  ret.ul_nof_ok     = data.count_crc_acks;
  ret.ul_nof_nok    = data.count_crc_pdus - data.count_crc_acks;
  ret.pusch_snr_db  = data.nof_pusch_snr_reports > 0 ? data.sum_pusch_snrs / data.nof_pusch_snr_reports : 0;
  ret.pucch_snr_db  = data.nof_pucch_snr_reports > 0 ? data.sum_pucch_snrs / data.nof_pucch_snr_reports : 0;
  ret.bsr           = last_bsr;
  // TODO: update PUSCH and PUCCH SNR metrics based on indications.

  // Reset UE stats metrics on every report.
  reset();

  return ret;
}

void scheduler_metrics_handler::ue_metric_context::reset()
{
  // Note: for BSR and CQI we just keep the last without resetting the value at every slot.
  data = {};
}
