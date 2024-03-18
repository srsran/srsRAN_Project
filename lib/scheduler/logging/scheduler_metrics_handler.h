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

#pragma once

#include "../ue_scheduling/harq_process.h"
#include "scheduler_metrics_ue_configurator.h"
#include "srsran/scheduler/scheduler_dl_buffer_state_indication_handler.h"
#include "srsran/scheduler/scheduler_feedback_handler.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include "srsran/scheduler/scheduler_slot_handler.h"
#include <unordered_map>

namespace srsran {

///\brief Handler of scheduler slot metrics.
class scheduler_metrics_handler final : public harq_timeout_handler, public sched_metrics_ue_configurator
{
  using msecs = std::chrono::milliseconds;
  using usecs = std::chrono::microseconds;

public:
  /// \brief Creates a scheduler UE metrics handler. In case the metrics_report_period is zero, no metrics are reported.
  explicit scheduler_metrics_handler(msecs metrics_report_period, scheduler_ue_metrics_notifier& notifier);

  /// \brief Register creation of a UE.
  void handle_ue_creation(du_ue_index_t ue_index, rnti_t rnti, pci_t pcell_pci, unsigned num_prbs) override;

  /// \brief Register removal of a UE.
  void handle_ue_deletion(du_ue_index_t ue_index) override;

  /// \brief Register CRC indication.
  void handle_crc_indication(const ul_crc_pdu_indication& crc_pdu, units::bytes tbs);

  /// \brief Register HARQ-ACK UCI indication.
  void handle_dl_harq_ack(du_ue_index_t ue_index, bool ack, units::bytes tbs);

  /// \brief Register HARQ timeout.
  void handle_harq_timeout(du_ue_index_t ue_index, bool is_dl) override;

  /// \brief Handle UCI PDU indication.
  void handle_uci_pdu_indication(const uci_indication::uci_pdu& pdu);

  /// \brief Handle UL BSR indication.
  void handle_ul_bsr_indication(const ul_bsr_indication_message& bsr);

  /// \brief Handle UL PHR indication.
  void handle_ul_phr_indication(const ul_phr_indication_message& phr_ind);

  /// \brief Handle DL Buffer Status indication.
  void handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& dl_bs);

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
      double   sum_pusch_snrs        = 0;
      double   sum_pucch_snrs        = 0;
      unsigned nof_pucch_snr_reports = 0;
      unsigned nof_pusch_snr_reports = 0;
      unsigned dl_prbs_used          = 0;
      unsigned ul_prbs_used          = 0;
    };
    pci_t                                  pci;
    unsigned                               nof_prbs;
    du_ue_index_t                          ue_index;
    rnti_t                                 rnti;
    uint8_t                                last_cqi = 0;
    uint8_t                                last_ri  = 1;
    unsigned                               last_bsr = 0;
    optional<int>                          last_phr;
    double                                 last_ta = -1;
    std::array<unsigned, MAX_NOF_RB_LCIDS> last_dl_bs{0};
    non_persistent_data                    data;

    scheduler_ue_metrics compute_report(std::chrono::milliseconds metric_report_period);

    void reset();
  };

  void handle_pucch_sinr(ue_metric_context& u, float sinr);
  void handle_csi_report(ue_metric_context& u, const csi_report_data& csi);

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
