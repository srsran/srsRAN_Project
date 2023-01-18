/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "scheduler_metrics_handler.h"

using namespace srsgnb;

scheduler_metrics_handler::scheduler_metrics_handler(msecs metrics_report_period, scheduler_metrics_queue* q) :
  metric_queue(q), report_period(metrics_report_period)
{
  srsgnb_assert(report_period > msecs{0}, "The periodicity of metric reporting must be positive");
}

void scheduler_metrics_handler::handle_ue_creation(du_ue_index_t ue_index, rnti_t rnti, pci_t pcell_pci)
{
  ues.emplace(ue_index);
  ues[ue_index].metrics.rnti = rnti;
  ues[ue_index].metrics.pci  = pcell_pci;
  rnti_to_ue_index_lookup.emplace(rnti, ue_index);
}

void scheduler_metrics_handler::handle_ue_deletion(du_ue_index_t ue_index)
{
  if (ues.contains(ue_index)) {
    rnti_to_ue_index_lookup.erase(ues[ue_index].metrics.rnti);
    ues.erase(ue_index);
  }
}

void scheduler_metrics_handler::handle_crc_indication(const ul_crc_pdu_indication& crc_pdu)
{
  if (ues.contains(crc_pdu.ue_index)) {
    ues[crc_pdu.ue_index].metrics.ul_nof_ok += crc_pdu.tb_crc_success ? 1 : 0;
    ues[crc_pdu.ue_index].metrics.ul_nof_nok += crc_pdu.tb_crc_success ? 0 : 1;
  }
}

void scheduler_metrics_handler::handle_dl_harq_ack(du_ue_index_t ue_index, bool ack)
{
  if (ues.contains(ue_index)) {
    auto& u = ues[ue_index];
    u.metrics.dl_nof_ok += ack ? 1 : 0;
    u.metrics.dl_nof_nok += ack ? 0 : 1;
  }
}

void scheduler_metrics_handler::handle_ul_bsr_indication(const ul_bsr_indication_message& bsr)
{
  if (ues.contains(bsr.ue_index)) {
    auto& u = ues[bsr.ue_index];

    // Store last BSR.
    u.metrics.bsr = 0;
    // TODO: Handle different BSR formats.
    for (unsigned i = 0; i != bsr.reported_lcgs.size(); ++i) {
      u.metrics.bsr += bsr.reported_lcgs[i].nof_bytes;
    }
  }
}

void scheduler_metrics_handler::push_result()
{
  if (not connected()) {
    return;
  }
  for (ue_metric_context& ue : ues) {
    // Get stats of the UE metrics.
    ue.compute_report(report_period);

    // TODO: use batch push.
    metric_queue->try_push(ue.metrics);

    // Reset stats metrics on every report.
    ue.reset();
  }
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
      u.metrics.dl_mcs += cw.mcs_index.to_uint();
      u.metrics.dl_brate_kbps += static_cast<double>(cw.tb_size_bytes * 8);
      u.nof_dl_cws++;
    }
  }

  for (const ul_sched_info& ul_grant : slot_result.ul.puschs) {
    auto it = rnti_to_ue_index_lookup.find(ul_grant.pusch_cfg.rnti);
    if (it == rnti_to_ue_index_lookup.end()) {
      // UE not found.
      continue;
    }
    ue_metric_context& u = ues[it->second];
    u.metrics.ul_mcs += ul_grant.pusch_cfg.mcs_index.to_uint();
    u.metrics.ul_brate_kbps += static_cast<double>(ul_grant.pusch_cfg.tb_size_bytes * 8);
    u.nof_ul_pdschs++;
  }
}

void scheduler_metrics_handler::push_result(slot_point sl_tx, const sched_result& slot_result)
{
  if (report_period_slots == 0) {
    // The SCS common is now known.
    usecs slot_dur      = std::chrono::duration_cast<usecs>(msecs{1U << sl_tx.numerology()});
    report_period_slots = usecs{report_period} / slot_dur;
  }

  handle_slot_result(slot_result);

  ++slot_counter;
  if (slot_counter >= report_period_slots) {
    push_result();
    slot_counter = 0;
  }
}

void scheduler_metrics_handler::ue_metric_context::compute_report(std::chrono::milliseconds metric_report_period)
{
  metrics.dl_mcs = nof_dl_cws > 0 ? std::roundf(static_cast<float>(metrics.dl_mcs) / nof_dl_cws) : 0;
  metrics.ul_mcs = nof_ul_pdschs > 0 ? std::roundf(static_cast<float>(metrics.ul_mcs) / nof_ul_pdschs) : 0;
  metrics.dl_brate_kbps /= metric_report_period.count();
  metrics.ul_brate_kbps /= metric_report_period.count();
  metrics.pusch_snr =
      nof_pusch_snr_reports > 0 ? std::roundf(static_cast<float>(metrics.pusch_snr) / nof_pusch_snr_reports) : 0;
  metrics.pucch_snr =
      nof_pucch_snr_reports > 0 ? std::roundf(static_cast<float>(metrics.pucch_snr) / nof_pucch_snr_reports) : 0;
  // TODO: update PUSCH and PUCCH SNR metrics based on indications.
}

void scheduler_metrics_handler::ue_metric_context::reset()
{
  // Note: for BSR and CQI we just keep the last without resetting the value at every slot.
  metrics.dl_mcs        = 0;
  metrics.ul_mcs        = 0;
  metrics.dl_brate_kbps = 0;
  metrics.ul_brate_kbps = 0;
  metrics.dl_nof_ok     = 0;
  metrics.dl_nof_nok    = 0;
  metrics.ul_nof_ok     = 0;
  metrics.ul_nof_nok    = 0;
  metrics.pucch_snr     = 0;
  metrics.pusch_snr     = 0;
  nof_dl_cws            = 0;
  nof_ul_pdschs         = 0;
  nof_pucch_snr_reports = 0;
  nof_pusch_snr_reports = 0;
}
