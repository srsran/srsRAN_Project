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

#include "scheduler_metrics_ue_configurator.h"
#include "srsran/adt/flat_map.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/adt/slotted_vector.h"
#include "srsran/scheduler/scheduler_configurator.h"
#include "srsran/scheduler/scheduler_dl_buffer_state_indication_handler.h"
#include "srsran/scheduler/scheduler_feedback_handler.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include "srsran/support/math/stats.h"
#include "srsran/support/units.h"

namespace srsran {

class cell_configuration;
struct rach_indication_message;
struct sched_result;

///\brief Handler of scheduler slot metrics for a given cell.
class cell_metrics_handler final : public sched_metrics_ue_configurator
{
  using msecs = std::chrono::milliseconds;
  using usecs = std::chrono::microseconds;

  struct ue_metric_context {
    /// \brief In this struct we store all the metadata that is reset at every report.
    struct non_persistent_data {
      unsigned count_uci_harq_acks            = 0;
      unsigned count_uci_harqs                = 0;
      unsigned count_crc_acks                 = 0;
      unsigned count_crc_pdus                 = 0;
      unsigned count_pucch_harq_pdus          = 0;
      unsigned count_pusch_harq_pdus          = 0;
      unsigned count_sr                       = 0;
      unsigned dl_mcs                         = 0;
      unsigned nof_dl_cws                     = 0;
      unsigned ul_mcs                         = 0;
      unsigned nof_puschs                     = 0;
      uint64_t sum_dl_tb_bytes                = 0;
      uint64_t sum_ul_tb_bytes                = 0;
      double   sum_pusch_snrs                 = 0;
      double   sum_pucch_snrs                 = 0;
      double   sum_pusch_rsrp                 = 0;
      unsigned sum_crc_delay_slots            = 0;
      unsigned max_crc_delay_slots            = 0;
      unsigned sum_pusch_harq_delay_slots     = 0;
      unsigned max_pusch_harq_delay_slots     = 0;
      unsigned sum_pucch_harq_delay_slots     = 0;
      unsigned max_pucch_harq_delay_slots     = 0;
      unsigned nof_pucch_snr_reports          = 0;
      unsigned nof_pucch_f0f1_invalid_harqs   = 0;
      unsigned nof_pucch_f2f3f4_invalid_harqs = 0;
      unsigned nof_pucch_f2f3f4_invalid_csis  = 0;
      unsigned nof_pusch_snr_reports          = 0;
      unsigned nof_pusch_rsrp_reports         = 0;
      unsigned nof_pusch_invalid_harqs        = 0;
      unsigned nof_pusch_invalid_csis         = 0;
      unsigned tot_dl_prbs_used               = 0;
      unsigned tot_ul_prbs_used               = 0;
      unsigned sum_ul_ce_delay_slots          = 0;
      unsigned max_ul_ce_delay_slots          = 0;
      unsigned nof_ul_ces                     = 0;
      /// TA statistics over the metrics report interval, in seconds.
      sample_statistics<float> ta;
      /// PUSCH TA statistics over the metrics report interval, in seconds.
      sample_statistics<float> pusch_ta;
      /// PUCCH TA statistics over the metrics report interval, in seconds.
      sample_statistics<float> pucch_ta;
      /// SRS TA statistics over the metrics report interval, in seconds.
      sample_statistics<float> srs_ta;
      /// CQI statistics over the metrics report interval.
      sample_statistics<unsigned> cqi;
      /// DL RI statistics over the metrics report interval.
      sample_statistics<unsigned> dl_ri;
      /// UL RI statistics over the metrics report interval.
      sample_statistics<unsigned> ul_ri;
    };

    // This user provided constructor is added here to fix a Clang compilation error related to the use of nested types
    // with std::optional.
    ue_metric_context() {}

    pci_t                                  pci;
    du_ue_index_t                          ue_index;
    rnti_t                                 rnti;
    unsigned                               last_bsr = 0;
    std::optional<int>                     last_phr;
    std::array<unsigned, MAX_NOF_RB_LCIDS> last_dl_bs{0};
    std::optional<float>                   last_dl_olla;
    std::optional<float>                   last_ul_olla;
    non_persistent_data                    data;

    scheduler_ue_metrics compute_report(std::chrono::milliseconds metric_report_period, unsigned nof_slots_per_sf);
    void                 reset();
  };

  /// Counters for scheduler cell metrics that get reset on every report.
  struct non_persistent_data {
    // Counter for detected PRACH preambles
    unsigned nof_prach_preambles = 0;
    // Counter of error indications.
    unsigned error_indication_counter = 0;
    // Tracks the sum of scheduler decision latencies.
    std::chrono::microseconds decision_latency_sum{0};
    // Tracks the maximum scheduler decision latency.
    std::chrono::microseconds max_decision_latency{0};
    // Respective slot at which the max decision latency was detected.
    slot_point max_decision_latency_slot;
    // Histogram of scheduler latencies.
    std::array<unsigned, scheduler_cell_metrics::latency_hist_bins> decision_latency_hist{};
    // Number of downlink slots.
    unsigned nof_dl_slots = 0;
    // Number of uplink slots.
    unsigned nof_ul_slots = 0;
    // Sum of UE PDSCH grants.
    unsigned nof_ue_pdsch_grants = 0;
    // Sum of UE PUSCH grants.
    unsigned nof_ue_pusch_grants = 0;
    // Number of failed PDCCH allocation attempts.
    unsigned nof_failed_pdcch_allocs = 0;
    // Number of failed UCI allocation attempts.
    unsigned nof_failed_uci_allocs = 0;
    // Number of MSG3 OKs.
    unsigned nof_msg3_ok = 0;
    // Number of MSG3 KOs.
    unsigned nof_msg3_nok = 0;
    // Total PRACH delay in slots.
    unsigned sum_prach_delay_slots = 0;
    // Number of failed PDSCH allocations due to late HARQs.
    unsigned nof_failed_pdsch_allocs_late_harqs = 0;
    // Number of failed PUSCH allocations due to late HARQs.
    unsigned nof_failed_pusch_allocs_late_harqs = 0;
    // Number of UE events not reported because the maximum number of events was reached.
    unsigned filtered_events_counter = 0;
    // Number of RBs used for PUCCH in the reporting period.
    unsigned pucch_rbs_used = 0;
  };

  scheduler_cell_metrics_notifier& notifier;
  const std::chrono::milliseconds  report_period;
  const cell_configuration&        cell_cfg;

  // Derived values.
  const unsigned nof_slots_per_sf;
  const unsigned report_period_slots;

  slot_point last_slot_tx;

  slotted_id_vector<du_ue_index_t, ue_metric_context> ues;
  flat_map<rnti_t, du_ue_index_t>                     rnti_to_ue_index_lookup;

  /// Metrics tracked that are reset on every report.
  non_persistent_data data;

public:
  /// \brief Creates a scheduler UE metrics handler for a given cell. In case the metrics_report_period is zero,
  /// no metrics are reported.
  explicit cell_metrics_handler(
      const cell_configuration&                                                      cell_cfg_,
      const std::optional<sched_cell_configuration_request_message::metrics_config>& metrics_cfg);
  ~cell_metrics_handler() override;

  /// \brief Register creation of a UE.
  void handle_ue_creation(du_ue_index_t ue_index, rnti_t rnti, pci_t pcell_pci) override;

  /// \brief Register UE reconfiguration.
  void handle_ue_reconfiguration(du_ue_index_t ue_index) override;

  /// \brief Register removal of a UE.
  void handle_ue_deletion(du_ue_index_t ue_index) override;

  /// \brief Register detected PRACH.
  void handle_rach_indication(const rach_indication_message& msg, slot_point sl_tx);

  /// \brief Register MSG3 CRC indication.
  void handle_msg3_crc_indication(const ul_crc_pdu_indication& crc_pdu);

  /// \brief Register CRC indication.
  void handle_crc_indication(slot_point sl_rx, const ul_crc_pdu_indication& crc_pdu, units::bytes tbs);

  /// \brief Handle SRS indication.
  void handle_srs_indication(const srs_indication::srs_indication_pdu& srs_pdu, unsigned ri);

  /// \brief Handle UCI that contains HARQ-ACK information.
  void handle_uci_with_harq_ack(du_ue_index_t ue_index, slot_point slot_rx, bool pucch);

  /// \brief Register HARQ-ACK UCI indication.
  void handle_dl_harq_ack(du_ue_index_t ue_index, bool ack, units::bytes tbs);

  /// \brief Register HARQ timeout.
  void handle_harq_timeout(du_ue_index_t ue_index, bool is_dl);

  /// \brief Handle UCI PDU indication.
  void handle_uci_pdu_indication(const uci_indication::uci_pdu& pdu, bool is_sr_opportunity_and_f1);

  /// \brief Handle SR indication.
  void handle_sr_indication(du_ue_index_t ue_index);

  /// \brief Handle UL BSR indication.
  void handle_ul_bsr_indication(const ul_bsr_indication_message& bsr);

  /// \brief Handle UL PHR indication.
  void handle_ul_phr_indication(const ul_phr_indication_message& phr_ind);

  /// \brief Handle DL Buffer Status indication.
  void handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& dl_bs);

  /// \brief Handle Error Indication reported to the scheduler for a given cell.
  void handle_error_indication();

  /// \brief Handle late DL HARQ indication.
  void handle_late_dl_harqs();

  /// \brief Handle late UL HARQ indication.
  void handle_late_ul_harqs();

  /// \brief Handle results stored in the scheduler result and push new entry.
  void push_result(slot_point sl_tx, const sched_result& slot_result, std::chrono::microseconds slot_decision_latency);

  /// \brief Checks whether the metrics reporting is active.
  bool enabled() const { return report_period_slots != 0; }

  /// \brief Called when the cell is stopped. This will trigger a cell stop report.
  void handle_cell_deactivation();

private:
  void handle_pucch_sinr(ue_metric_context& u, float sinr);
  void handle_csi_report(ue_metric_context& u, const csi_report_data& csi);
  void report_metrics();
  void handle_slot_result(const sched_result& slot_result, std::chrono::microseconds slot_decision_latency);

  std::vector<scheduler_cell_event> pending_events;
};

/// Handler of metrics for all the UEs and cells of the scheduler.
class scheduler_metrics_handler
{
public:
  cell_metrics_handler*
  add_cell(const cell_configuration&                                                      cell_cfg,
           const std::optional<sched_cell_configuration_request_message::metrics_config>& metrics_cfg);

  void rem_cell(du_cell_index_t cell_index);

  cell_metrics_handler& at(du_cell_index_t cell_idx) { return *cells[cell_idx]; }

private:
  slotted_array<std::unique_ptr<cell_metrics_handler>, MAX_NOF_DU_CELLS> cells;
};

} // namespace srsran
