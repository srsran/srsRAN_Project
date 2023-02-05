/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../support/bwp_helpers.h"
#include "harq_process.h"
#include "ue_configuration.h"
#include "srsgnb/scheduler/config/scheduler_expert_config.h"

namespace srsgnb {

struct ul_crc_pdu_indication;

/// \brief Context respective to a UE serving cell.
class ue_cell
{
public:
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

  double get_pusch_snr() const { return sched_metrics.pusch_snr_db; }

  /// \brief Estimate the number of required DL PRBs to allocate the given number of bytes.
  unsigned required_dl_prbs(unsigned time_resource, unsigned pending_bytes) const;

  /// \brief Estimate the number of required UL PRBs to allocate the given number of bytes.
  unsigned required_ul_prbs(unsigned time_resource, unsigned pending_bytes) const;

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

private:
  struct ue_cell_metrics {
    /// Save the latest PUSCH SNR reported from PHY, in dB.
    double pusch_snr_db;
    // TODO: Add other metrics of interest for the scheduler.
  };

  /// Update PUSCH SNR metric of the UE.
  void update_pusch_snr(double snr) { sched_metrics.pusch_snr_db = snr; }

  rnti_t                            crnti_;
  const scheduler_ue_expert_config& expert_cfg;
  ue_cell_configuration             ue_cfg;
  ue_cell_metrics                   sched_metrics;
};

} // namespace srsgnb
