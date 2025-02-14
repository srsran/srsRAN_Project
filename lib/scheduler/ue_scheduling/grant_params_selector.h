/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cell/cell_harq_manager.h"
#include "../support/sch_pdu_builder.h"
#include "srsran/ran/sch/sch_mcs.h"

namespace srsran {

class ue_cell;

namespace sched_helper {

/// Estimation of the number of PRBs and MCS to use for a given number of pending bytes and channel state.
struct mcs_prbs_selection {
  /// Recommended MCS to use.
  sch_mcs_index mcs;
  /// Number of recommended PRBs for the PDSCH grant given the number of pending bytes and chosen MCS.
  unsigned nof_prbs;
};

/// Derive recommended MCS and number of PRBs for a newTx PDSCH grant.
std::optional<mcs_prbs_selection>
compute_newtx_required_mcs_and_prbs(const pdsch_config_params& pdsch_cfg, const ue_cell& ue_cc, unsigned pending_bytes);

/// Compute PUSCH grant parameters for a newTx given the UE state, DCI type and PUSCH time-domain resource.
pusch_config_params compute_newtx_pusch_config_params(const ue_cell&                               ue_cc,
                                                      dci_ul_rnti_config_type                      dci_type,
                                                      const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                                      unsigned                                     uci_bits,
                                                      bool                                         is_csi_report_slot);

/// Compute PUSCH grant parameters for a reTx given the UE state, DCI type and PUSCH time-domain resource.
pusch_config_params compute_retx_pusch_config_params(const ue_cell&                               ue_cc,
                                                     const ul_harq_process_handle&                h_ul,
                                                     const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                                     unsigned                                     uci_bits,
                                                     bool                                         is_csi_report_slot);

/// Derive recommended MCS and number of PRBs for a newTx PUSCH grant.
mcs_prbs_selection
compute_newtx_required_mcs_and_prbs(const pusch_config_params& pusch_cfg, const ue_cell& ue_cc, unsigned pending_bytes);

} // namespace sched_helper
} // namespace srsran
