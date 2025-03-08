/*
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cell/resource_grid.h"
#include "../pdcch_scheduling/pdcch_resource_allocator.h"
#include "../slicing/ran_slice_candidate.h"
#include "../uci_scheduling/uci_allocator.h"
#include "grant_params_selector.h"
#include "ue_repository.h"
#include "srsran/adt/noop_functor.h"

namespace srsran {

struct scheduler_ue_expert_config;

/// Request to reserve space for control channels of a DL grant.
struct ue_newtx_dl_grant_request {
  /// UE to allocate.
  const slice_ue& user;
  /// Slot at which PDSCH will take place.
  slot_point pdsch_slot;
  /// Pending newTx bytes to allocate.
  unsigned pending_bytes;
};

/// Request for a DL grant allocation.
struct ue_retx_dl_grant_request {
  /// Slot at which PDSCH PDU shall be allocated.
  slot_point pdsch_slot;
  /// UE to allocate.
  const slice_ue& user;
  /// DL HARQ process to reTx.
  dl_harq_process_handle h_dl;
  /// Selected allocation parameters.
  sched_helper::retx_dl_grant_config params;
};

/// Request for a UL grant allocation.
struct ue_ul_grant_request {
  /// Slot at which PUSCH PDU shall be allocated.
  slot_point pusch_slot;
  /// UE to allocate.
  const slice_ue& user;
  /// HARQ process to be retransmitted.
  std::optional<ul_harq_process_handle> h_ul;
  /// Recommended allocation parameters.
  sched_helper::ul_grant_sched_params alloc_params;
};

/// \brief Status of a UE grant allocation, and action for the scheduler policy to follow afterwards.
///
/// The current status are:
/// - success - the allocation was successful with the provided parameters.
/// - skip_slot - failure to allocate and the scheduler policy should terminate the current slot processing.
/// - skip_ue - failure to allocate and the scheduler policy should move on to the next candidate UE.
/// - invalid_params - failure to allocate and the scheduler policy should try a different set of grant parameters.
enum class alloc_status { success, skip_slot, skip_ue, invalid_params };

/// Allocation result of a UE DL grant allocation.
struct dl_alloc_result {
  alloc_status status;
  /// Nof. of bytes allocated if allocation was successful.
  unsigned alloc_bytes{0};
  /// Nof. of resource blocks allocated if allocation was successful.
  unsigned alloc_nof_rbs{0};
  /// List of logical channels scheduled in this TB if allocation was successful.
  dl_msg_tb_info tb_info;
};

/// Allocation result of a UE UL grant allocation.
struct ul_alloc_result {
  alloc_status status;
  /// Nof. of bytes allocated if allocation was successful.
  unsigned alloc_bytes{0};
  /// Nof. of resource blocks allocated if allocation was successful.
  unsigned alloc_nof_rbs{0};
};

/// \brief This class implements the methods to allocate PDCCH, UCI, PDSCH and PUSCH PDUs in the cell resource grid for
/// UE grants.
class ue_cell_grid_allocator
{
  // Information relative to a pending DL grant for this slot.
  struct dl_grant_info {
    const slice_ue*                user;
    sched_helper::dl_sched_context cfg;
    dl_harq_process_handle         h_dl;
    pdcch_dl_information*          pdcch;
    dl_msg_alloc*                  pdsch;
    uci_allocation                 uci_alloc;
    unsigned                       pending_bytes;
  };

public:
  /// Placeholder for a grant in the DL resource grid, which allows deferred setting of PDSCH parameters.
  struct dl_newtx_grant_builder {
    dl_newtx_grant_builder(dl_newtx_grant_builder&&) noexcept            = default;
    dl_newtx_grant_builder& operator=(dl_newtx_grant_builder&&) noexcept = default;
    ~dl_newtx_grant_builder() { srsran_assert(parent == nullptr, "PDSCH parameters were not set"); }

    /// Sets the final CRBs for the PDSCH allocation.
    void set_pdsch_params(const crb_interval& alloc_crbs);

    /// For a given max number of RBs and a bitmap of used CRBs, returns the recommended parameters for the PDSCH grant.
    crb_interval recommended_crbs(const crb_bitmap& used_crbs, unsigned max_nof_rbs = MAX_NOF_PRBS) const
    {
      const dl_grant_info& grant = grant_info();
      return sched_helper::compute_newtx_dl_crbs(grant.cfg, used_crbs, max_nof_rbs);
    }

    /// Getters for grant immutable parameters.
    const slice_ue&                       ue() const { return *grant_info().user; }
    const sched_helper::dl_sched_context& context() const { return grant_info().cfg; }
    unsigned                              pending_bytes() const { return grant_info().pending_bytes; }

  private:
    friend class ue_cell_grid_allocator;

    dl_newtx_grant_builder(ue_cell_grid_allocator& parent_, unsigned grant_index_) :
      parent(&parent_), grant_index(grant_index_)
    {
    }

    const dl_grant_info& grant_info() const { return parent->dl_grants[grant_index]; }

    std::unique_ptr<ue_cell_grid_allocator, noop_operation> parent;
    unsigned                                                grant_index;
  };

  ue_cell_grid_allocator(const scheduler_ue_expert_config& expert_cfg_,
                         ue_repository&                    ues_,
                         pdcch_resource_allocator&         pdcch_sched_,
                         uci_allocator&                    uci_alloc_,
                         cell_resource_allocator&          cell_alloc_,
                         srslog::basic_logger&             logger_);

  /// Allocate PDCCH, UCI and PDSCH PDUs for a UE DL grant and return a builder to set the PDSCH parameters.
  expected<dl_newtx_grant_builder, alloc_status> allocate_dl_grant(const ue_newtx_dl_grant_request& request);

  /// Allocates DL grant for a UE HARQ reTx.
  alloc_status allocate_dl_grant(const ue_retx_dl_grant_request& request);

  /// Allocates UL grant for a UE newTx.
  ul_alloc_result allocate_ul_grant(const ue_ul_grant_request& request);

  /// \brief Called at the end of a slot to process the allocations that took place and make some final adjustments.
  ///
  /// In particular, this function can redimension the existing grants to fill the remaining RBs if it deems necessary.
  void post_process_results();

private:
  // Setup DL grant builder.
  expected<dl_grant_info, alloc_status> setup_dl_grant_builder(const slice_ue&                       user,
                                                               const sched_helper::dl_sched_context& params,
                                                               std::optional<dl_harq_process_handle> h_dl,
                                                               unsigned                              pending_bytes);

  void set_pdsch_params(dl_grant_info& grant, const crb_interval& crbs);

  sch_mcs_tbs calculate_dl_mcs_tbs(cell_slot_resource_allocator& pdsch_alloc,
                                   const search_space_info&      ss_info,
                                   uint8_t                       pdsch_td_res_index,
                                   const crb_interval&           crbs,
                                   sch_mcs_index                 mcs,
                                   unsigned                      nof_layers);

  expected<pdcch_dl_information*, alloc_status> alloc_dl_pdcch(const ue_cell& ue_cc, const search_space_info& ss_info);

  expected<uci_allocation, alloc_status>
  alloc_uci(const ue_cell& ue_cc, const search_space_info& ss_info, uint8_t pdsch_td_res_index);

  // Save the PUCCH power control results for the given slot.
  void post_process_pucch_pw_ctrl_results(slot_point slot);

  const scheduler_ue_expert_config& expert_cfg;
  ue_repository&                    ues;
  pdcch_resource_allocator&         pdcch_sched;
  uci_allocator&                    uci_alloc;
  cell_resource_allocator&          cell_alloc;
  srslog::basic_logger&             logger;

  std::vector<dl_grant_info> dl_grants;
};

} // namespace srsran
