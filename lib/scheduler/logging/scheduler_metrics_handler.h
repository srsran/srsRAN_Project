/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/scheduler/scheduler_feedback_handler.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include "srsran/scheduler/scheduler_slot_handler.h"
#include <unordered_map>

namespace srsran {

///\brief Handler of scheduler slot metrics.
class scheduler_metrics_handler
{
  using msecs = std::chrono::milliseconds;
  using usecs = std::chrono::microseconds;

public:
  /// \brief Creates a scheduler UE metrics handler. In case the metrics_report_period is zero, no metrics are reported.
  explicit scheduler_metrics_handler(msecs metrics_report_period, scheduler_ue_metrics_notifier& notifier);

  /// \brief Register creation of a UE.
  void handle_ue_creation(du_ue_index_t ue_index, rnti_t rnti, pci_t pcell_pci);

  /// \brief Register removal of a UE.
  void handle_ue_deletion(du_ue_index_t ue_index);

  void handle_crc_indication(const ul_crc_pdu_indication& crc_pdu);

  /// \brief Register CSI report (CQI) metric.
  void handle_csi_report(du_ue_index_t                                                         ue_index,
                         const bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS>& csi);

  void handle_dl_harq_ack(du_ue_index_t ue_index, bool ack);

  /// \brief Register PUCCH SINR.
  void handle_pucch_sinr(du_ue_index_t ue_index, optional<float> pucch_sinr);

  /// \brief Handle UL BSR indication.
  void handle_ul_bsr_indication(const ul_bsr_indication_message& bsr);

  /// \brief Handle results stored in the scheduler result and push new entry.
  void push_result(slot_point sl_tx, const sched_result& slot_result);

  /// \brief Checks whether the metrics reporting is active.
  bool connected() const { return report_period != std::chrono::nanoseconds{0}; }

private:
  struct ue_metric_context {
    /// \brief In this struct we store all the metadata that is reset at every report.
    struct non_persistent_data {
      unsigned count_uci_harq_acks   = 0;
      unsigned count_uci_harqs       = 0;
      unsigned count_crc_acks        = 0;
      unsigned count_crc_pdus        = 0;
      unsigned dl_mcs                = 0;
      unsigned nof_dl_cws            = 0;
      unsigned ul_mcs                = 0;
      unsigned nof_puschs            = 0;
      uint64_t sum_dl_tb_bytes       = 0;
      uint64_t sum_ul_tb_bytes       = 0;
      unsigned sum_pusch_snrs        = 0;
      unsigned sum_pucch_snrs        = 0;
      unsigned nof_pucch_snr_reports = 0;
      unsigned nof_pusch_snr_reports = 0;
    };
    pci_t               pci;
    du_ue_index_t       ue_index;
    rnti_t              rnti;
    unsigned            last_cqi = 0;
    unsigned            last_bsr = 0;
    non_persistent_data data;

    scheduler_ue_metrics compute_report(std::chrono::milliseconds metric_report_period);

    void reset();
  };

  void report_metrics();
  void handle_slot_result(const sched_result& slot_result);

  scheduler_ue_metrics_notifier&  notifier;
  const std::chrono::milliseconds report_period;

  // derived.
  unsigned report_period_slots = 0;

  /// \brief This type is used so that multiple threads can access different positions concurrently.
  slotted_id_table<du_ue_index_t, ue_metric_context, MAX_NOF_DU_UES> ues;
  std::unordered_map<rnti_t, du_ue_index_t>                          rnti_to_ue_index_lookup;

  // \brief Counter of number of slots elapsed since the last report.
  unsigned slot_counter = 0;
};

} // namespace srsran