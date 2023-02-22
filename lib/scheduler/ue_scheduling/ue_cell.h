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

#pragma once

#include "../support/bwp_helpers.h"
#include "harq_process.h"
#include "ue_configuration.h"
#include "srsran/ran/uci/uci_constants.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"

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
    /// Save the latest PUSCH SNR reported from PHY, in dB.
    // NOTE: the 0 is only used for initialization and will be overwritten by the first UL SNR report.
    double   pusch_snr_db          = 0.0;
    unsigned consecutive_pusch_kos = 0;
    // This gets initialized in the ue_cell constructor.
    unsigned latest_wb_cqi;
    // TODO: Add other metrics of interest for the scheduler.
  };

  ue_cell(du_ue_index_t                     ue_index_,
          rnti_t                            crnti_val,
          const scheduler_ue_expert_config& expert_cfg_,
          const cell_configuration&         cell_cfg_common_,
          const serving_cell_config&        ue_serv_cell);

  const du_ue_index_t   ue_index;
  const du_cell_index_t cell_index;

  harq_entity harqs;

  rnti_t rnti() const { return crnti_; }

  bwp_id_t active_bwp_id() const { return to_bwp_id(0); }
  bool     is_active() const { return true; }

  const ue_cell_configuration& cfg() const { return ue_cfg; }

  void handle_reconfiguration_request(const serving_cell_config& new_ue_cell_cfg);

  unsigned get_latest_wb_cqi() const { return ue_metrics.latest_wb_cqi; }

  void set_latest_wb_cqi(const bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS>& payload);

  /// \brief Estimate the number of required DL PRBs to allocate the given number of bytes.
  grant_prbs_mcs required_dl_prbs(unsigned time_resource, unsigned pending_bytes) const;

  /// \brief Estimate the number of required UL PRBs to allocate the given number of bytes.
  grant_prbs_mcs required_ul_prbs(unsigned time_resource, unsigned pending_bytes, dci_ul_rnti_config_type type) const;

  /// \brief Derive UL resource allocation type1 BWP configuration as per TS38.214, 6.1.2.2.2.
  bwp_configuration alloc_type1_bwp_limits(dci_ul_format dci_fmt, search_space_configuration::type_t ss_type) const
  {
    return get_resource_alloc_type_1_ul_bwp_size(dci_fmt,
                                                 ue_cfg.cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params,
                                                 ue_cfg.ul_bwp_common(active_bwp_id()).generic_params,
                                                 ss_type);
  }

  /// \brief Handle CRC PDU indication.
  int handle_crc_pdu(slot_point pusch_slot, const ul_crc_pdu_indication& crc_pdu);

  /// \brief Get the current UE cell metrics.
  const metrics& get_metrics() const { return ue_metrics; }
  metrics&       get_metrics() { return ue_metrics; }

private:
  /// Update PUSCH SNR metric of the UE.
  void update_pusch_snr(optional<float> snr)
  {
    if (snr.has_value()) {
      ue_metrics.pusch_snr_db = static_cast<double>(snr.value());
    }
  }

  rnti_t                            crnti_;
  const scheduler_ue_expert_config& expert_cfg;
  ue_cell_configuration             ue_cfg;

  metrics ue_metrics;
};

} // namespace srsran
