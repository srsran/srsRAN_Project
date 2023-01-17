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

#include "srsgnb/scheduler/scheduler_feedback_handler.h"
#include "srsgnb/scheduler/scheduler_metrics.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"
#include <unordered_map>

namespace srsgnb {

///\brief Handler of scheduler slot metrics.
class scheduler_metrics_handler
{
  using msecs = std::chrono::milliseconds;
  using usecs = std::chrono::microseconds;

public:
  explicit scheduler_metrics_handler(msecs metrics_report_period, scheduler_metrics_queue* q = nullptr);

  /// \brief Register creation of a UE.
  void handle_ue_creation(du_ue_index_t ue_index, rnti_t rnti, pci_t pcell_pci);

  /// \brief Register removal of a UE.
  void handle_ue_deletion(du_ue_index_t ue_index);

  void handle_crc_indication(const ul_crc_indication& crc);

  void handle_dl_harq_ack(du_ue_index_t ue_index, bool ack);

  /// \brief Handle UL BSR indication.
  void handle_ul_bsr_indication(const ul_bsr_indication_message& bsr);

  /// \brief Handle results stored in the scheduler result and push new entry.
  void push(slot_point sl_tx, const sched_result& slot_result);

  /// \brief Checks whether a queue has been provided to send metrics.
  bool connected() const { return metric_queue != nullptr; }

private:
  struct ue_metric_context {
    scheduler_ue_metrics metrics;
    unsigned             nof_dl_cws            = 0;
    unsigned             nof_ul_pdschs         = 0;
    unsigned             nof_pucch_snr_reports = 0;
    unsigned             nof_pusch_snr_reports = 0;

    void compute_report(float time_elapsed_secs);

    void reset();
  };

  void push_result();
  void handle_slot_result(const sched_result& slot_result);

  scheduler_metrics_queue*        metric_queue = nullptr;
  const std::chrono::milliseconds report_period;

  // derived.
  unsigned report_period_slots = 0;
  float    report_period_secs  = 0;

  /// \brief This type is used so that multiple threads can access different positions concurrently.
  slotted_id_table<du_ue_index_t, ue_metric_context, MAX_NOF_DU_UES> ues;
  std::unordered_map<rnti_t, du_ue_index_t>                          rnti_to_ue_index_lookup;

  unsigned slot_counter = 0;
};

} // namespace srsgnb