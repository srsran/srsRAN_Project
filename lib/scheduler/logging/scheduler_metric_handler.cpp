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

void scheduler_metrics_handler::handle_crc_indication(const ul_crc_pdu_indication& crc_pdu)
{
  if (ues.contains(crc_pdu.ue_index)) {
    ues[crc_pdu.ue_index].data.count_crc_acks += crc_pdu.tb_crc_success ? 1 : 0;
    ues[crc_pdu.ue_index].data.count_crc_pdus++;
    if (crc_pdu.ul_sinr_metric.has_value()) {
      ues[crc_pdu.ue_index].data.nof_pusch_snr_reports++;
      ues[crc_pdu.ue_index].data.sum_pusch_snrs += crc_pdu.ul_sinr_metric.value();
    }
  }
}

void scheduler_metrics_handler::handle_csi_report(
    du_ue_index_t                                                         ue_index,
    const bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS>& csi)
{
  if (ues.contains(ue_index)) {
    auto&               u                = ues[ue_index];
    static const size_t cqi_payload_size = 4;
    if (csi.size() != cqi_payload_size) {
      return;
    }
    // Refer to \ref mac_uci_pdu::pucch_f2_or_f3_or_f4_type::uci_payload_or_csi_information for the CSI payload bit
    // encoding.
    u.last_cqi = (static_cast<unsigned>(csi.test(0)) << 3) + (static_cast<unsigned>(csi.test(1)) << 2) +
                 (static_cast<unsigned>(csi.test(2)) << 1) + (static_cast<unsigned>(csi.test(3)));
  }
}

void scheduler_metrics_handler::handle_dl_harq_ack(du_ue_index_t ue_index, bool ack)
{
  if (ues.contains(ue_index)) {
    auto& u = ues[ue_index];
    u.data.count_uci_harq_acks += ack ? 1 : 0;
    u.data.count_uci_harqs++;
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
      u.data.sum_dl_tb_bytes += cw.tb_size_bytes;
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
    u.data.sum_ul_tb_bytes += ul_grant.pusch_cfg.tb_size_bytes;
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
  ret.pusch_snr_db  = data.nof_pusch_snr_reports > 0
                          ? std::roundf(static_cast<float>(data.sum_pusch_snrs) / data.nof_pusch_snr_reports)
                          : 0;
  ret.pucch_snr_db  = data.nof_pucch_snr_reports > 0
                          ? std::roundf(static_cast<float>(data.sum_pucch_snrs) / data.nof_pucch_snr_reports)
                          : 0;
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
