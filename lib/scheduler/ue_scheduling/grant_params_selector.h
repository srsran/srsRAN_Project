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
class slice_ue;

namespace sched_helper {

/// Estimation of the number of PRBs and MCS to use for a given number of pending bytes and channel state.
struct mcs_prbs_selection {
  /// Recommended MCS to use.
  sch_mcs_index mcs;
  /// Number of recommended PRBs for the PDSCH grant given the number of pending bytes and chosen MCS.
  unsigned nof_prbs;
};

/// Derive recommended MCS and number of PRBs for a newTx PDSCH grant.
std::optional<mcs_prbs_selection> compute_newtx_required_mcs_and_prbs(const pdsch_config_params& pdsch_cfg,
                                                                      const ue_cell&             ue_cc,
                                                                      unsigned                   pending_bytes,
                                                                      unsigned max_nof_rbs = MAX_NOF_PRBS);

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
mcs_prbs_selection compute_newtx_required_mcs_and_prbs(const pusch_config_params& pusch_cfg,
                                                       const ue_cell&             ue_cc,
                                                       unsigned                   pending_bytes,
                                                       unsigned                   max_nof_rbs = MAX_NOF_PRBS);

/// Parameters recommended for a DL grant.
struct dl_grant_sched_params {
  /// SearchSpace to use.
  search_space_id ss_id;
  /// PDSCH time-domain resource index.
  uint8_t pdsch_td_res_index;
  /// Recommended MCS.
  sch_mcs_index mcs;
  /// Recommended number of layers.
  unsigned nof_layers;
  /// Recommended CRBs to allocate (allocation type 1).
  crb_interval alloc_crbs;
  /// Recommended number of RBs.
  unsigned nof_rbs;
  /// CRB boundaries where the DL grant allocation can take place.
  crb_interval crb_lims;
};

/// Derive recommended parameters for a DL newTx grant.
std::optional<dl_grant_sched_params> compute_newtx_dl_grant_sched_params(const slice_ue&                u,
                                                                         slot_point                     pdcch_slot,
                                                                         slot_point                     pdsch_slot,
                                                                         const crb_bitmap&              used_crbs,
                                                                         unsigned                       pending_bytes,
                                                                         const std::optional<unsigned>& max_rbs);

/// Derive recommended parameters for a DL reTx grant.
std::optional<dl_grant_sched_params> compute_retx_dl_grant_sched_params(const slice_ue&               u,
                                                                        slot_point                    pdcch_slot,
                                                                        slot_point                    pdsch_slot,
                                                                        const dl_harq_process_handle& h_dl,
                                                                        const crb_bitmap&             used_crbs);

/// Parameters recommended for a UL grant.
struct ul_grant_sched_params {
  /// SearchSpace to use.
  search_space_id ss_id;
  /// PUSCH time-domain resource index.
  uint8_t pusch_td_res_index;
  /// Recommended MCS.
  sch_mcs_index mcs;
  /// Recommended number of RBs.
  unsigned nof_rbs;
  /// Limits in CRBs for UL grant allocation.
  crb_interval crb_lims;
  /// PUSCH config params.
  pusch_config_params pusch_cfg;
};

/// Derive recommended parameters for a UL newTx grant.
std::optional<ul_grant_sched_params> compute_newtx_ul_grant_sched_params(const slice_ue& u,
                                                                         slot_point      pdcch_slot,
                                                                         slot_point      pusch_slot,
                                                                         unsigned        pending_bytes,
                                                                         unsigned        uci_nof_harq_bits,
                                                                         const std::optional<unsigned>& max_rbs);

/// Derive recommended parameters for a DL reTx grant.
std::optional<ul_grant_sched_params> compute_retx_ul_grant_sched_params(const slice_ue&               u,
                                                                        slot_point                    pdcch_slot,
                                                                        slot_point                    pusch_slot,
                                                                        const ul_harq_process_handle& h_ul,
                                                                        unsigned uci_nof_harq_bits);

} // namespace sched_helper
} // namespace srsran
