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

#include "../config/ue_configuration.h"
#include "../support/bwp_helpers.h"
#include "../support/sch_pdu_builder.h"
#include "harq_process.h"
#include "ue_channel_state_manager.h"
#include "ue_link_adaptation_controller.h"
#include "srsran/ran/uci/uci_constants.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/scheduler/scheduler_feedback_handler.h"

namespace srsran {

struct ul_crc_pdu_indication;

struct grant_prbs_mcs {
  /// MCS to use for the UE's PUSCH.
  sch_mcs_index mcs;
  /// Number of PRBs to be allocated for the UE's PUSCH.
  unsigned n_prbs;
};

/// \brief Context respective to a UE serving cell.
class ue_cell
{
public:
  struct metrics {
    unsigned consecutive_pusch_kos = 0;
  };

  bool is_in_fallback_mode() const { return in_fallback_mode; }

  ue_cell(du_ue_index_t                ue_index_,
          rnti_t                       crnti_val,
          const ue_cell_configuration& ue_cell_cfg_,
          ue_harq_timeout_notifier     harq_timeout_notifier);

  const du_ue_index_t   ue_index;
  const du_cell_index_t cell_index;

  harq_entity harqs;

  // Slot at which PDSCH was allocated in the past for this UE in this cell.
  slot_point last_pdsch_allocated_slot;
  // Slot at which PUSCH was allocated in the past for this UE in this cell.
  slot_point last_pusch_allocated_slot;

  rnti_t rnti() const { return crnti_; }

  bwp_id_t active_bwp_id() const { return to_bwp_id(0); }

  /// \brief Determines whether the UE cell is currently active.
  bool is_active() const { return active; }

  const ue_cell_configuration& cfg() const { return *ue_cfg; }

  /// \brief Deactivates cell.
  void deactivate();

  void handle_reconfiguration_request(const ue_cell_configuration& ue_cell_cfg);

  void set_fallback_state(bool in_fallback);

  std::optional<dl_harq_process::dl_ack_info_result> handle_dl_ack_info(slot_point                 uci_slot,
                                                                        mac_harq_ack_report_status ack_value,
                                                                        unsigned                   harq_bit_idx,
                                                                        std::optional<float>       pucch_snr);

  /// \brief Estimate the number of required DL PRBs to allocate the given number of bytes.
  grant_prbs_mcs required_dl_prbs(const pdsch_time_domain_resource_allocation& pdsch_td_cfg,
                                  unsigned                                     pending_bytes,
                                  dci_dl_rnti_config_type                      dci_type) const;

  /// \brief Estimate the number of required UL PRBs to allocate the given number of bytes.
  grant_prbs_mcs required_ul_prbs(const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                  unsigned                                     pending_bytes,
                                  dci_ul_rnti_config_type                      dci_type) const;

  uint8_t get_pdsch_rv(const dl_harq_process& h_dl) const
  {
    return cell_cfg.expert_cfg.ue
        .pdsch_rv_sequence[h_dl.tb(0).nof_retxs % cell_cfg.expert_cfg.ue.pdsch_rv_sequence.size()];
  }
  uint8_t get_pusch_rv(const ul_harq_process& h_ul) const
  {
    return cell_cfg.expert_cfg.ue
        .pusch_rv_sequence[h_ul.tb().nof_retxs % cell_cfg.expert_cfg.ue.pusch_rv_sequence.size()];
  }

  /// \brief Handle CRC PDU indication.
  int handle_crc_pdu(slot_point pusch_slot, const ul_crc_pdu_indication& crc_pdu);

  /// Update UE with the latest CSI report for a given cell.
  void handle_csi_report(const csi_report_data& csi_report);

  /// \brief Get the current UE cell metrics.
  const metrics& get_metrics() const { return ue_metrics; }
  metrics&       get_metrics() { return ue_metrics; }

  sch_mcs_index get_ul_mcs(pusch_mcs_table mcs_table) const { return ue_mcs_calculator.calculate_ul_mcs(mcs_table); }

  /// \brief Get recommended aggregation level for PDCCH at a given CQI.
  aggregation_level get_aggregation_level(float cqi, const search_space_info& ss_info, bool is_dl) const;

  /// \brief Get list of recommended Search Spaces given the UE current state and channel quality.
  /// \param[in] required_dci_rnti_type Optional parameter to filter Search Spaces by DCI RNTI config type.
  /// \return List of SearchSpace configuration.
  static_vector<const search_space_info*, MAX_NOF_SEARCH_SPACE_PER_BWP>
  get_active_dl_search_spaces(slot_point                             pdcch_slot,
                              std::optional<dci_dl_rnti_config_type> required_dci_rnti_type = {}) const;
  static_vector<const search_space_info*, MAX_NOF_SEARCH_SPACE_PER_BWP>
  get_active_ul_search_spaces(slot_point                             pdcch_slot,
                              std::optional<dci_ul_rnti_config_type> required_dci_rnti_type = {}) const;

  /// \brief Defines the fallback state of the ue_cell.
  /// Transitions can be fallback => sr_csi_received => normal => fallback. The fallback => sr_csi_received transition
  /// is triggered by the reception of SR or CSI, the sr_csi_received => normal is triggered by the reception of 2
  /// CRC=OK after the first SR or CSI is received.
  enum class fallback_state { fallback, sr_csi_received, normal };

  /// \brief Get UE channel state handler.
  ue_channel_state_manager&       channel_state_manager() { return channel_state; }
  const ue_channel_state_manager& channel_state_manager() const { return channel_state; }

  const ue_link_adaptation_controller& link_adaptation_controller() const { return ue_mcs_calculator; }

  /// \brief Returns an estimated DL rate in bytes per slot based on the given input parameters.
  double get_estimated_dl_rate(const pdsch_config_params& pdsch_cfg, sch_mcs_index mcs, unsigned nof_prbs) const;
  /// \brief Returns an estimated UL rate in bytes per slot based on the given input parameters.
  double get_estimated_ul_rate(const pusch_config_params& pusch_cfg, sch_mcs_index mcs, unsigned nof_prbs) const;

private:
  /// \brief Performs link adaptation procedures such as cancelling HARQs etc.
  void apply_link_adaptation_procedures(const csi_report_data& csi_report);

  rnti_t                            crnti_;
  const cell_configuration&         cell_cfg;
  const ue_cell_configuration*      ue_cfg;
  const scheduler_ue_expert_config& expert_cfg;
  srslog::basic_logger&             logger;

  /// \brief Whether cell is currently active.
  bool active = true;

  /// Fallback state of the UE. When in "fallback" mode, only the search spaces and the configuration of
  /// cellConfigCommon are used.
  bool in_fallback_mode = true;

  metrics ue_metrics;

  ue_channel_state_manager channel_state;

  ue_link_adaptation_controller ue_mcs_calculator;
};

} // namespace srsran
