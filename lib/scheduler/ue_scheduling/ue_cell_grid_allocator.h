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

#include "../cell/resource_grid.h"
#include "../pdcch_scheduling/pdcch_resource_allocator.h"
#include "../uci_scheduling/uci_allocator.h"
#include "../ue_context/ue_repository.h"
#include "grant_params_selector.h"
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
  /// Whether interleaving VRB-to-PRB mapping is enabled.
  bool interleaving_enabled;
};

/// Request for a reTx DL grant allocation.
struct ue_retx_dl_grant_request {
  /// UE to allocate.
  const slice_ue& user;
  /// Slot at which PDSCH PDU shall be allocated.
  slot_point pdsch_slot;
  /// DL HARQ process to reTx.
  dl_harq_process_handle h_dl;
  /// Current DL VRB occupation.
  vrb_bitmap& used_dl_vrbs;
  /// Whether interleaving VRB-to-PRB mapping is enabled.
  bool interleaving_enabled;
};

/// Request to reserve space for control channels of a UL grant.
struct ue_newtx_ul_grant_request {
  /// UE to allocate.
  const slice_ue& user;
  /// Slot at which PUSCH will take place.
  slot_point pusch_slot;
  /// Pending newTx bytes to allocate.
  unsigned pending_bytes;
};

/// Request for a reTx UL grant allocation.
struct ue_retx_ul_grant_request {
  /// UE to allocate.
  const slice_ue& user;
  /// Slot at which PUSCH PDU shall be allocated.
  slot_point pusch_slot;
  /// UL HARQ process to reTx.
  ul_harq_process_handle h_ul;
  /// Current UL VRB occupation.
  vrb_bitmap& used_ul_vrbs;
};

/// \brief Status of a UE grant allocation, and action for the scheduler policy to follow afterwards.
///
/// The current status are:
/// - success - the allocation was successful with the provided parameters.
/// - skip_slot - failure to allocate and the scheduler policy should terminate the current slot processing.
/// - skip_ue - failure to allocate and the scheduler policy should move on to the next candidate UE.
/// - invalid_params - failure to allocate and the scheduler policy should try a different set of grant parameters.
enum class alloc_status { success, skip_slot, skip_ue, invalid_params };

/// \brief Status of a UE DL grant allocation, and action for the scheduler policy to follow afterwards.
///
/// The current status are:
/// - other - failure to allocate grant for reasons other than failure to allocate PDCCH or UCI.
/// - skip_slot - failure to allocate and the scheduler policy should terminate the current slot processing.
/// - pdcch_alloc_failed - failure to allocate a PDCCH for the given UE.
/// - uci_alloc_failed - failure to allocate a UCI for the given UE.
enum class dl_alloc_failure_cause { other, skip_slot, pdcch_alloc_failed, uci_alloc_failed };

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

  // Information relative to a pending UL grant for this slot.
  struct ul_grant_info {
    const slice_ue*                user;
    sched_helper::ul_sched_context cfg;
    ul_harq_process_handle         h_ul;
    pdcch_ul_information*          pdcch;
    ul_sched_info*                 pusch;
    unsigned                       pending_bytes;
  };

public:
  /// \brief Interface for a DL grant, which allows deferred setting of PDSCH parameters.
  class dl_newtx_grant_builder
  {
  public:
    dl_newtx_grant_builder(dl_newtx_grant_builder&&) noexcept            = default;
    dl_newtx_grant_builder& operator=(dl_newtx_grant_builder&&) noexcept = default;
    ~dl_newtx_grant_builder() { srsran_assert(parent == nullptr, "PDSCH parameters were not set"); }

    /// Sets the final VRBs for the PDSCH allocation.
    void set_pdsch_params(vrb_interval                          alloc_vrbs,
                          std::pair<crb_interval, crb_interval> alloc_crbs,
                          bool                                  enable_interleaving);

    /// For a given max number of RBs and a bitmap of used VRBs, returns the recommended parameters for the PDSCH grant.
    vrb_interval recommended_vrbs(const vrb_bitmap& used_vrbs, unsigned max_nof_rbs = MAX_NOF_PRBS) const
    {
      const dl_grant_info& grant = grant_info();
      return compute_newtx_dl_vrbs(grant.cfg, used_vrbs, max_nof_rbs);
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

  /// \brief Interface for a UL grant, which allows deferred setting of PUSCH parameters.
  class ul_newtx_grant_builder
  {
  public:
    ul_newtx_grant_builder(ul_newtx_grant_builder&&) noexcept            = default;
    ul_newtx_grant_builder& operator=(ul_newtx_grant_builder&&) noexcept = default;
    ~ul_newtx_grant_builder() { srsran_assert(parent == nullptr, "PUSCH parameters were not set"); }

    /// Sets the final VRBs for the PUSCH allocation.
    void set_pusch_params(const vrb_interval& alloc_vrbs);

    /// For a given max number of RBs and a bitmap of used VRBs, returns the recommended parameters for the PUSCH grant.
    vrb_interval recommended_vrbs(const vrb_bitmap& used_vrbs, unsigned max_nof_rbs = MAX_NOF_PRBS) const
    {
      const ul_grant_info& grant = grant_info();
      return compute_newtx_ul_vrbs(grant.cfg, used_vrbs, max_nof_rbs);
    }

    /// Getters for grant immutable parameters.
    const slice_ue&                       ue() const { return *grant_info().user; }
    const sched_helper::ul_sched_context& context() const { return grant_info().cfg; }
    unsigned                              pending_bytes() const { return grant_info().pending_bytes; }

  private:
    friend class ue_cell_grid_allocator;

    ul_newtx_grant_builder(ue_cell_grid_allocator& parent_, unsigned grant_index_) :
      parent(&parent_), grant_index(grant_index_)
    {
    }

    const ul_grant_info& grant_info() const { return parent->ul_grants[grant_index]; }

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
  expected<dl_newtx_grant_builder, dl_alloc_failure_cause> allocate_dl_grant(const ue_newtx_dl_grant_request& request);

  /// Allocates DL grant for a UE HARQ reTx.
  expected<vrb_interval, dl_alloc_failure_cause> allocate_dl_grant(const ue_retx_dl_grant_request& request);

  /// Allocate PDCCH, UCI and PUSCH PDUs for a UE UL grant and return a builder to set the PUSCH parameters.
  expected<ul_newtx_grant_builder, alloc_status> allocate_ul_grant(const ue_newtx_ul_grant_request& request);

  /// Allocates UL grant for a UE HARQ reTx.
  expected<vrb_interval, alloc_status> allocate_ul_grant(const ue_retx_ul_grant_request& request);

  /// \brief Called at the end of a slot to process the allocations that took place and make some final adjustments.
  ///
  /// In particular, this function can redimension the existing grants to fill the remaining RBs if it deems necessary.
  void post_process_results();

private:
  // Setup DL grant builder.
  expected<dl_grant_info, dl_alloc_failure_cause> setup_dl_grant_builder(const slice_ue&                       user,
                                                                         const sched_helper::dl_sched_context& params,
                                                                         std::optional<dl_harq_process_handle> h_dl,
                                                                         unsigned pending_bytes);

  // Setup UL grant builder.
  expected<ul_grant_info, alloc_status> setup_ul_grant_builder(const slice_ue&                       user,
                                                               const sched_helper::ul_sched_context& params,
                                                               std::optional<ul_harq_process_handle> h_ul,
                                                               unsigned                              pending_bytes);

  // Set final PDSCH parameters and allocate remaining DL grant resources.
  void set_pdsch_params(dl_grant_info&                        grant,
                        vrb_interval                          vrbs,
                        std::pair<crb_interval, crb_interval> crbs,
                        bool                                  enable_interleaving);

  // Set final PUSCH parameters and allocate remaining UL grant resources.
  void set_pusch_params(ul_grant_info& grant, const vrb_interval& vrbs);

  std::optional<sch_mcs_tbs> calculate_dl_mcs_tbs(cell_slot_resource_allocator&         pdsch_alloc,
                                                  const search_space_info&              ss_info,
                                                  uint8_t                               pdsch_td_res_index,
                                                  std::pair<crb_interval, crb_interval> crbs,
                                                  sch_mcs_index                         mcs,
                                                  unsigned                              nof_layers);

  expected<pdcch_dl_information*, alloc_status> alloc_dl_pdcch(const ue_cell& ue_cc, const search_space_info& ss_info);

  std::optional<uci_allocation>
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
  std::vector<ul_grant_info> ul_grants;
};

} // namespace srsran
