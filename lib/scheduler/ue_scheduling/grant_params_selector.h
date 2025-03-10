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

/// Retrieve recommended PDCCH and PDSCH parameters for a reTx DL grant.
std::optional<dl_sched_context> get_retx_dl_sched_context(const slice_ue&               u,
                                                          slot_point                    pdcch_slot,
                                                          slot_point                    pdsch_slot,
                                                          const dl_harq_process_handle& h_dl);

/// Select DL CRBs to allocate for a newTx.
crb_interval compute_newtx_dl_crbs(const dl_sched_context& decision_ctxt,
                                   const crb_bitmap&       used_crbs,
                                   unsigned                max_nof_rbs = MAX_NOF_PRBS);

/// Select DL CRBs to allocate for a reTx.
crb_interval compute_retx_dl_crbs(const dl_sched_context& decision_ctxt, const crb_bitmap& used_crbs);

/// PDCCH and PUSCH parameters recommended for a UL grant.
struct ul_sched_context {
  /// SearchSpace to use.
  search_space_id ss_id;
  /// PUSCH time-domain resource index.
  uint8_t pusch_td_res_index;
  /// Limits on CRBs for UL grant allocation.
  crb_interval crb_lims;
  /// Limits for grant size in RBs.
  interval<unsigned> nof_rb_lims;
  /// Recommended MCS, considering channel state or, in case of reTx, last HARQ MCS.
  sch_mcs_index recommended_mcs;
  /// Expected number of RBs to allocate.
  unsigned expected_nof_rbs;
  /// PUSCH config params.
  pusch_config_params pusch_cfg;
};

/// Retrieve recommended PDCCH and PUSCH parameters for a newTx UL grant.
std::optional<ul_sched_context> get_newtx_ul_sched_context(const slice_ue& u,
                                                           slot_point      pdcch_slot,
                                                           slot_point      pusch_slot,
                                                           unsigned        uci_nof_harq_bits,
                                                           unsigned        pending_bytes);

/// Retrieve recommended PDCCH and PUSCH parameters for a reTx UL grant.
std::optional<ul_sched_context> get_retx_ul_sched_context(const slice_ue&               u,
                                                          slot_point                    pdcch_slot,
                                                          slot_point                    pusch_slot,
                                                          unsigned                      uci_nof_harq_bits,
                                                          const ul_harq_process_handle& h_ul);

/// Select UL CRBs to allocate for a newTx.
crb_interval compute_newtx_ul_crbs(const ul_sched_context& decision_ctxt,
                                   const crb_bitmap&       used_crbs,
                                   unsigned                max_nof_rbs = MAX_NOF_PRBS);

/// Select UL CRBs to allocate for a reTx.
crb_interval compute_retx_ul_crbs(const ul_sched_context& decision_ctxt, const crb_bitmap& used_crbs);

} // namespace sched_helper
} // namespace srsran
