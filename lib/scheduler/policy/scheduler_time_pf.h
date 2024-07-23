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

#include "../slicing/slice_ue_repository.h"
#include "scheduler_policy.h"

namespace srsran {

class scheduler_time_pf : public scheduler_policy
{
public:
  scheduler_time_pf(const scheduler_ue_expert_config& expert_cfg_);

  void dl_sched(ue_pdsch_allocator&          pdsch_alloc,
                const ue_resource_grid_view& res_grid,
                dl_ran_slice_candidate&      slice_candidate) override;

  void ul_sched(ue_pusch_allocator&          pusch_alloc,
                const ue_resource_grid_view& res_grid,
                ul_ran_slice_candidate&      slice_candidate) override;

private:
  /// Fairness parameters.
  const double fairness_coeff;
  const double exp_avg_alpha = 0.01;

  /// Holds the information needed to compute priority of a UE in a priority queue.
  struct ue_ctxt {
    ue_ctxt(du_ue_index_t ue_index_, du_cell_index_t cell_index_, const scheduler_time_pf* parent_) :
      ue_index(ue_index_), cell_index(cell_index_), parent(parent_)
    {
    }

    /// Returns average DL rate expressed in bytes per slot.
    [[nodiscard]] double dl_avg_rate() const { return dl_nof_samples == 0 ? 0 : dl_avg_rate_; }
    /// Returns average UL rate expressed in bytes per slot.
    [[nodiscard]] double ul_avg_rate() const { return ul_nof_samples == 0 ? 0 : ul_avg_rate_; }

    void compute_dl_prio(const ue& u);
    void compute_ul_prio(const ue& u, const ue_resource_grid_view& res_grid);

    void save_dl_alloc(uint32_t alloc_bytes);
    void save_ul_alloc(uint32_t alloc_bytes);

    const du_ue_index_t      ue_index;
    const du_cell_index_t    cell_index;
    const scheduler_time_pf* parent;

    /// DL priority value of the UE.
    double dl_prio = 0;
    /// UL priority value of the UE.
    double ul_prio = 0;

    const dl_harq_process* dl_retx_h  = nullptr;
    const dl_harq_process* dl_newtx_h = nullptr;
    const ul_harq_process* ul_retx_h  = nullptr;
    const ul_harq_process* ul_newtx_h = nullptr;
    /// Number of pending newTx bytes in SRBs in DL to be scheduled.
    unsigned dl_newtx_srb_pending_bytes = 0;
    /// Number of pending newTx bytes in SRBs in UL to be scheduled.
    unsigned ul_newtx_srb_pending_bytes = 0;
    /// Flag indicating whether SR indication from the UE is received or not.
    bool sr_ind_received = false;

  private:
    /// Average DL rate expressed in bytes per slot experienced by UE.
    double dl_avg_rate_ = 0;
    /// Average UL rate expressed in bytes per slot experienced by UE.
    double ul_avg_rate_ = 0;
    /// Nof. DL samples over which average DL bitrate is computed.
    uint32_t dl_nof_samples = 0;
    /// Nof. UL samples over which average DL bitrate is computed.
    uint32_t ul_nof_samples = 0;
  };

  /// \brief Attempts to allocate PDSCH for a UE.
  /// \return Returns allocation status, nof. allocated bytes and nof. allocated RBs.
  alloc_result
  try_dl_alloc(ue_ctxt& ctxt, const slice_ue_repository& ues, ue_pdsch_allocator& pdsch_alloc, unsigned max_rbs);
  /// \brief Attempts to allocate PUSCH for a UE.
  /// \return Returns allocation status, nof. allocated bytes and nof. allocated RBs.
  alloc_result
  try_ul_alloc(ue_ctxt& ctxt, const slice_ue_repository& ues, ue_pusch_allocator& pusch_alloc, unsigned max_rbs);

  slotted_id_table<du_ue_index_t, ue_ctxt, MAX_NOF_DU_UES> ue_history_db;

  struct ue_dl_prio_compare {
    bool operator()(const ue_ctxt* lhs, const ue_ctxt* rhs) const;
  };
  struct ue_ul_prio_compare {
    bool operator()(const ue_ctxt* lhs, const ue_ctxt* rhs) const;
  };

  // Note: the std::priority_queue makes its underlying container protected, so it seems that they are ok with
  // inheritance.
  class ue_dl_queue_t : public std::priority_queue<ue_ctxt*, std::vector<ue_ctxt*>, ue_dl_prio_compare>
  {
    using base_type = std::priority_queue<ue_ctxt*, std::vector<ue_ctxt*>, ue_dl_prio_compare>;

  public:
    // Note: faster than while(!empty()) pop() because it avoids the O(NlogN). Faster than = {}, because it preserves
    // memory.
    void clear()
    {
      // Access to underlying vector.
      this->c.clear();
    }

    // Adapter of the priority_queue push method to avoid adding candidates with skip priority level.
    void push(ue_ctxt* elem)
    {
      if (elem->dl_retx_h == nullptr and elem->dl_newtx_h == nullptr) {
        return;
      }
      base_type::push(elem);
    }
  };

  // Note: the std::priority_queue makes its underlying container protected, so it seems that they are ok with
  // inheritance.
  class ue_ul_queue_t : public std::priority_queue<ue_ctxt*, std::vector<ue_ctxt*>, ue_ul_prio_compare>
  {
    using base_type = std::priority_queue<ue_ctxt*, std::vector<ue_ctxt*>, ue_ul_prio_compare>;

  public:
    // Note: faster than while(!empty()) pop() because it avoids the O(NlogN). Faster than = {}, because it preserves
    // memory.
    void clear()
    {
      // Access to underlying vector.
      this->c.clear();
    }

    // Adapter of the priority_queue push method to avoid adding candidates with skip priority level.
    void push(ue_ctxt* elem)
    {
      if (elem->ul_retx_h == nullptr and elem->ul_newtx_h == nullptr) {
        return;
      }
      base_type::push(elem);
    }
  };

  /// Priority queue of UEs to be scheduled in DL. The UE in front of the queue has highest priority and vice versa.
  ue_dl_queue_t dl_queue;
  /// Priority queue of UEs to be scheduled in UL. The UE in front of the queue has highest priority and vice versa.
  ue_ul_queue_t ul_queue;
};

} // namespace srsran
