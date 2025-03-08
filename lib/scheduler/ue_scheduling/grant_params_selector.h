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
#include "../slicing/slice_ue_repository.h"
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

/// PDCCH and PDSCH parameters recommended for a DL grant.
struct dl_sched_context {
  /// SearchSpace to use.
  search_space_id ss_id;
  /// PDSCH time-domain resource index.
  uint8_t pdsch_td_res_index;
  /// Limits on CRBs for DL grant allocation.
  crb_interval crb_lims;
  /// Recommended MCS, considering channel state or, in case of reTx, last HARQ MCS.
  sch_mcs_index recommended_mcs;
  /// Recommended number of layers.
  unsigned recommended_ri;
  /// Expected number of RBs to allocate.
  unsigned expected_nof_rbs;
};

/// Retrieve recommended PDCCH and PDSCH parameters for a newTx DL grant.
std::optional<dl_sched_context>
get_newtx_dl_sched_context(const slice_ue& u, slot_point pdcch_slot, slot_point pdsch_slot, unsigned pending_bytes);

/// Select CRBs to allocate.
crb_interval compute_newtx_dl_crbs(const dl_sched_context& decision_ctxt,
                                   const crb_bitmap&       used_crbs,
                                   unsigned                max_nof_rbs = MAX_NOF_PRBS);

/// Selected parameters for a DL reTx grant allocation.
struct retx_dl_grant_config {
  /// Parameters related with the choice of searchSpace and PDSCH time-domain resource.
  dl_sched_context decision_ctxt;
  /// Recommended CRBs to allocate (allocation type 1).
  crb_interval alloc_crbs;
};

/// Derive parameters for a DL reTx grant.
std::optional<retx_dl_grant_config> select_retx_dl_grant_config(const slice_ue&               u,
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
