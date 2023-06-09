/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../support/bwp_helpers.h"
#include "harq_process.h"
#include "ue_channel_state_manager.h"
#include "ue_configuration.h"
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

  ue_cell(du_ue_index_t                     ue_index_,
          rnti_t                            crnti_val,
          const scheduler_ue_expert_config& expert_cfg_,
          const cell_configuration&         cell_cfg_common_,
          const serving_cell_config&        ue_serv_cell,
          ue_harq_timeout_notifier          harq_timeout_notifier);

  const du_ue_index_t   ue_index;
  const du_cell_index_t cell_index;

  harq_entity harqs;

  ue_channel_state_manager channel_state;

  rnti_t rnti() const { return crnti_; }

  bwp_id_t active_bwp_id() const { return to_bwp_id(0); }
  bool     is_active() const { return true; }

  const ue_cell_configuration& cfg() const { return ue_cfg; }

  void handle_reconfiguration_request(const serving_cell_config& new_ue_cell_cfg);

  /// \brief Estimate the number of required DL PRBs to allocate the given number of bytes.
  grant_prbs_mcs required_dl_prbs(const pdsch_time_domain_resource_allocation& pdsch_td_cfg,
                                  unsigned                                     pending_bytes) const;

  /// \brief Estimate the number of required UL PRBs to allocate the given number of bytes.
  grant_prbs_mcs required_ul_prbs(const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                  unsigned                                     pending_bytes,
                                  dci_ul_rnti_config_type                      type) const;

  uint8_t get_pdsch_rv(const dl_harq_process& h_dl) const
  {
    return expert_cfg.pdsch_rv_sequence[h_dl.tb(0).nof_retxs % expert_cfg.pdsch_rv_sequence.size()];
  }
  uint8_t get_pusch_rv(const ul_harq_process& h_ul) const
  {
    return expert_cfg.pusch_rv_sequence[h_ul.tb().nof_retxs % expert_cfg.pusch_rv_sequence.size()];
  }

  /// \brief Handle CRC PDU indication.
  int handle_crc_pdu(slot_point pusch_slot, const ul_crc_pdu_indication& crc_pdu);

  /// \brief Get the current UE cell metrics.
  const metrics& get_metrics() const { return ue_metrics; }
  metrics&       get_metrics() { return ue_metrics; }

  /// \brief Get recommended aggregation level for PDCCH given reported CQI.
  aggregation_level get_aggregation_level() const
  {
    // TODO: Use dynamic aggregation level.
    return aggregation_level::n4;
  }

  /// \brief Get list of recommended Search Spaces given the UE current state and channel quality.
  /// \param[in] required_dci_rnti_type Optional parameter to filter Search Spaces by DCI RNTI config type.
  /// \return List of SearchSpace configuration.
  static_vector<const search_space_info*, MAX_NOF_SEARCH_SPACE_PER_BWP>
  get_active_dl_search_spaces(optional<dci_dl_rnti_config_type> required_dci_rnti_type = {}) const;
  static_vector<const search_space_info*, MAX_NOF_SEARCH_SPACE_PER_BWP>
  get_active_ul_search_spaces(optional<dci_ul_rnti_config_type> required_dci_rnti_type = {}) const;

  /// \brief Set UE fallback state. When in "fallback" mode, only the search spaces of cellConfigCommon are used. The UE
  /// should automatically leave this mode, when a CRC=OK is received.
  void set_fallback_state(bool fallback_state_) { is_fallback_mode = fallback_state_; }

private:
  rnti_t                            crnti_;
  const scheduler_ue_expert_config& expert_cfg;
  ue_cell_configuration             ue_cfg;
  srslog::basic_logger&             logger;

  /// \brief Fallback state of the UE.
  bool is_fallback_mode = false;

  metrics ue_metrics;
};

} // namespace srsran
