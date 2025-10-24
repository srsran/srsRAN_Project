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

#pragma once

#include "srsran/ran/du_types.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/sch/sch_mcs.h"
#include "srsran/ran/slot_point.h"
#include "srsran/support/math/stats.h"
#include "srsran/support/zero_copy_notifier.h"
#include <array>
#include <chrono>
#include <optional>
#include <vector>

namespace srsran {

/// \brief Snapshot of the metrics for a UE.
struct scheduler_ue_metrics {
  /// UE index in the DU for this UE.
  du_ue_index_t ue_index;
  /// PCI of the UE's PCell.
  pci_t pci;
  /// Currently used C-RNTI for this UE.
  rnti_t rnti;
  /// Average MCS index used for DL grants.
  sch_mcs_index dl_mcs;
  /// Number of RBs used for PDSCH.
  unsigned tot_pdsch_prbs_used;
  /// \brief Experienced MAC DL bit rate in kbps, considering the size of the allocated MAC DL PDUs for which a positive
  /// HARQ-ACK was received.
  double dl_brate_kbps;
  /// Number of positive HARQ-ACKs received.
  unsigned dl_nof_ok;
  /// Number of detected HARQ NACKs or HARQ-ACK misdetections.
  unsigned dl_nof_nok;
  /// SNR in dB estimated for the PUSCH.
  float pusch_snr_db;
  /// RSRP in dB estimated for the PUSCH.
  float pusch_rsrp_db;
  /// SNR in dB estimated for the PUCCH.
  float pucch_snr_db;
  /// Average MCS index used for UL grants.
  sch_mcs_index ul_mcs;
  /// Number of RBs used for PUSCH.
  unsigned tot_pusch_prbs_used;
  /// \brief Experienced MAC UL bit rate in kbps, considering the size of the allocated MAC UL PDUs for which the
  /// respective CRC was decoded.
  double ul_brate_kbps;
  /// Number of positive CRC PDU indications received.
  unsigned ul_nof_ok;
  /// Number of negative CRC PDU indications received.
  unsigned ul_nof_nok;
  /// Sum of the last UL buffer status reports (BSRs) of all logical channel groups.
  unsigned bsr;
  /// Number of scheduling requests detected.
  unsigned sr_count;
  /// Sum of the last DL buffer occupancy reports of all logical channels.
  unsigned dl_bs;
  /// Invalid UCI reception metrics.
  /// @{
  unsigned nof_pucch_f0f1_invalid_harqs;
  unsigned nof_pucch_f2f3f4_invalid_harqs;
  unsigned nof_pucch_f2f3f4_invalid_csis;
  unsigned nof_pusch_invalid_harqs;
  unsigned nof_pusch_invalid_csis;
  /// @}
  /// Delay metrics.
  /// @{
  std::optional<float> avg_ce_delay_ms;
  std::optional<float> max_ce_delay_ms;
  std::optional<float> avg_crc_delay_ms;
  std::optional<float> max_crc_delay_ms;
  std::optional<float> avg_pusch_harq_delay_ms;
  std::optional<float> max_pusch_harq_delay_ms;
  std::optional<float> avg_pucch_harq_delay_ms;
  std::optional<float> max_pucch_harq_delay_ms;
  std::optional<float> avg_sr_to_pusch_delay_ms;
  std::optional<float> max_sr_to_pusch_delay_ms;
  /// @}
  std::optional<float>    last_dl_olla;
  std::optional<float>    last_ul_olla;
  std::optional<int>      last_phr;
  std::optional<unsigned> max_pdsch_distance_ms;
  std::optional<unsigned> max_pusch_distance_ms;
  /// Time advance statistics in seconds.
  sample_statistics<float> ta_stats;
  sample_statistics<float> pusch_ta_stats;
  sample_statistics<float> pucch_ta_stats;
  sample_statistics<float> srs_ta_stats;
  /// CQI statistics over the metrics report interval.
  sample_statistics<unsigned> cqi_stats;
  /// DL RI statistics over the metrics report interval.
  sample_statistics<unsigned> dl_ri_stats;
  /// UL RI statistics over the metrics report interval.
  sample_statistics<unsigned> ul_ri_stats;
};

/// \brief Event that occurred in the cell of the scheduler.
struct scheduler_cell_event {
  enum class event_type { ue_add, ue_reconf, ue_rem };

  slot_point slot;
  rnti_t     rnti = rnti_t::INVALID_RNTI;
  event_type type;
};

inline const char* sched_event_to_string(scheduler_cell_event::event_type ev)
{
  static constexpr std::array<const char*, 3> names = {"ue_add", "ue_reconf", "ue_rem"};
  return names[std::min(static_cast<size_t>(ev), names.size() - 1)];
}

/// \brief Snapshot of the metrics for a cell and its UEs.
struct scheduler_cell_metrics {
  /// Latency histogram number of bins.
  static constexpr unsigned latency_hist_bins = 10;
  /// Distance between histogram bins.
  static constexpr unsigned nof_usec_per_bin = 50;

  /// Cell PCI for which the metrics are reported.
  pci_t pci;
  /// Slot at which the metrics started being tracked for this report.
  slot_point slot;
  /// Number of slots accounted for in this report.
  unsigned nof_slots = 0;
  /// Number of cell PRBs.
  unsigned nof_prbs = 0;
  /// Number of downlink slots.
  unsigned nof_dl_slots = 0;
  /// Number of uplink slots (only full uplink slots counted for now).
  unsigned nof_ul_slots = 0;
  /// Number of PRACH preambles detected.
  unsigned nof_prach_preambles = 0;
  /// Counter of UE PDSCH grants (RARs, SIBs and Paging are not considered).
  unsigned dl_grants_count = 0;
  /// Counter of UE PUSCH grants.
  unsigned ul_grants_count = 0;
  /// Number of failed PDCCH allocation attempts.
  unsigned nof_failed_pdcch_allocs = 0;
  /// Number of failed UCI allocation attempts.
  unsigned nof_failed_uci_allocs = 0;
  /// Number of MSG3s.
  unsigned nof_msg3_ok = 0;
  /// Number of MSG3 KOs.
  unsigned nof_msg3_nok = 0;
  /// Average PRACH delay in slots.
  std::optional<float> avg_prach_delay_slots;
  /// Number of failed PDSCH allocations due to late HARQs.
  unsigned nof_failed_pdsch_allocs_late_harqs = 0;
  /// Number of failed PUSCH allocations due to late HARQs.
  unsigned nof_failed_pusch_allocs_late_harqs = 0;
  /// Number of UE events not reported because the maximum number of events was reached.
  unsigned nof_filtered_events = 0;
  /// Average number of RBs used for PUCCH per UL slot.
  float pucch_tot_rb_usage_avg = 0.0f;

  unsigned                                nof_error_indications = 0;
  std::chrono::microseconds               average_decision_latency{0};
  std::chrono::microseconds               max_decision_latency{0};
  slot_point                              max_decision_latency_slot;
  std::array<unsigned, latency_hist_bins> latency_histogram{0};
  /// Number of RBs used for PUSCH per slot index in the TDD pattern.
  std::vector<unsigned>             pusch_prbs_used_per_tdd_slot_idx;
  std::vector<unsigned>             pdsch_prbs_used_per_tdd_slot_idx;
  std::vector<scheduler_cell_event> events;
  std::vector<scheduler_ue_metrics> ue_metrics;
};

/// Scheduler metrics report for all active cells of the DU.
struct scheduler_metrics_report {
  std::vector<scheduler_cell_metrics> cells;
};

/// \brief Notifier interface used by scheduler to report metrics.
class scheduler_metrics_notifier
{
public:
  virtual ~scheduler_metrics_notifier() = default;

  /// \brief This method will be called periodically by the scheduler to report the latest UE metrics statistics.
  virtual void report_metrics(const scheduler_cell_metrics& report) = 0;
};

/// Interface used by the scheduler to determine whether a new metric report is required.
class scheduler_cell_metrics_notifier : public zero_copy_notifier<scheduler_cell_metrics>
{
public:
  /// Check whether a new metric report is required given the current slot.
  virtual bool is_sched_report_required(slot_point sl_tx) const = 0;
};

} // namespace srsran
