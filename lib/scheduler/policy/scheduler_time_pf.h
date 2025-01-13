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

#include "../slicing/slice_ue_repository.h"
#include "scheduler_policy.h"

namespace srsran {

class scheduler_time_pf : public scheduler_policy
{
public:
  scheduler_time_pf(const scheduler_ue_expert_config& expert_cfg_);

  void dl_sched(ue_pdsch_allocator&          pdsch_alloc,
                const ue_resource_grid_view& res_grid,
                dl_ran_slice_candidate&      slice_candidate,
                dl_harq_pending_retx_list    harq_pending_retx_list) override;

  void ul_sched(ue_pusch_allocator&          pusch_alloc,
                const ue_resource_grid_view& res_grid,
                ul_ran_slice_candidate&      slice_candidate,
                ul_harq_pending_retx_list    harq_pending_retx_list) override;

private:
  // Value used to flag that the UE cannot be allocated in a given slot.
  static constexpr double forbid_prio = std::numeric_limits<double>::lowest();

  /// Fairness parameters.
  /// Coefficient used to tweak decision in favor of fairness or throughput.
  const double fairness_coeff;
  /// Coefficient used to compute exponential moving average.
  const double exp_avg_alpha = 0.01;

  /// Holds the information needed to compute priority of a UE in a priority queue.
  struct ue_ctxt {
    ue_ctxt(du_ue_index_t ue_index_, du_cell_index_t cell_index_, const scheduler_time_pf* parent_) :
      ue_index(ue_index_), cell_index(cell_index_), parent(parent_), dl_avg_rate_per_lc(lcid_t::MAX_NOF_RB_LCIDS)
    {
      std::fill(dl_avg_rate_per_lc.begin(), dl_avg_rate_per_lc.end(), 0);
    }

    /// Returns average DL rate expressed in bytes per slot of the UE.
    [[nodiscard]] double total_dl_avg_rate() const { return dl_nof_samples == 0 ? 0 : total_dl_avg_rate_; }
    /// Returns average UL rate expressed in bytes per slot of the UE.
    [[nodiscard]] double total_ul_avg_rate() const { return ul_nof_samples == 0 ? 0 : total_ul_avg_rate_; }

    /// Computes the priority of the UE to be scheduled in DL based on the QoS and proportional fair metric.
    void compute_dl_prio(const slice_ue& u,
                         ran_slice_id_t  slice_id,
                         slot_point      pdcch_slot,
                         slot_point      pdsch_slot,
                         unsigned        nof_slots_elapsed);
    /// Computes the priority of the UE to be scheduled in UL based on the proportional fair metric.
    void compute_ul_prio(const slice_ue& u,
                         ran_slice_id_t  slice_id,
                         slot_point      pdcch_slot,
                         slot_point      pusch_slot,
                         unsigned        nof_slots_elapsed);

    void save_dl_alloc(uint32_t total_alloc_bytes, const dl_msg_tb_info& tb_info);
    void save_ul_alloc(const slice_ue& u, unsigned alloc_bytes);

    const du_ue_index_t      ue_index;
    const du_cell_index_t    cell_index;
    const scheduler_time_pf* parent;

    /// DL priority value of the UE.
    double dl_prio = forbid_prio;
    /// UL priority value of the UE.
    double ul_prio = forbid_prio;

    /// Flag indicating whether SR indication from the UE is received or not.
    bool sr_ind_received = false;

  private:
    void compute_dl_avg_rate(const slice_ue& u, unsigned nof_slots_elapsed);
    void compute_ul_avg_rate(const slice_ue& u, unsigned nof_slots_elapsed);

    // Sum of DL bytes allocated for a given slot, before it is taken into account in the average rate computation.
    static_vector<unsigned, MAX_NOF_RB_LCIDS> dl_alloc_bytes_per_lc;
    unsigned                                  dl_sum_alloc_bytes = 0;
    // Average DL rate expressed in bytes per slot experienced by UE in each of its logical channel.
    static_vector<double, MAX_NOF_RB_LCIDS> dl_avg_rate_per_lc;
    // Average DL rate expressed in bytes per slot experienced by UE.
    double total_dl_avg_rate_ = 0;
    // Sum of UL bytes allocated for a given slot, before it is taken into account in the average rate computation.
    static_vector<unsigned, MAX_NOF_LCGS> ul_alloc_bytes_per_lcg;
    // Average UL rate expressed in bytes per slot experienced by UE in each of its logical channel.
    static_vector<double, MAX_NOF_LCGS> ul_avg_rate_per_lcg;
    // Sum of UL bytes allocated for a given slot, before it is taken into account in the average rate computation.
    unsigned ul_sum_alloc_bytes = 0;
    // Average UL rate expressed in bytes per slot experienced by UE.
    double total_ul_avg_rate_ = 0;
    // Nof. DL samples over which average DL bitrate is computed.
    uint32_t dl_nof_samples = 0;
    // Nof. UL samples over which average DL bitrate is computed.
    uint32_t ul_nof_samples = 0;
  };

  dl_alloc_result schedule_dl_retxs(ue_pdsch_allocator&          pdsch_alloc,
                                    const ue_resource_grid_view& res_grid,
                                    dl_ran_slice_candidate&      slice_candidate,
                                    dl_harq_pending_retx_list    harq_pending_retx_list);
  ul_alloc_result schedule_ul_retxs(ue_pusch_allocator&          pusch_alloc,
                                    const ue_resource_grid_view& res_grid,
                                    ul_ran_slice_candidate&      slice_candidate,
                                    ul_harq_pending_retx_list    harq_pending_retx_list);

  /// \brief Attempts to allocate PDSCH for a UE.
  /// \return Returns allocation status, nof. allocated bytes and nof. allocated RBs.
  dl_alloc_result
  try_dl_alloc(ue_ctxt& ctxt, const slice_ue_repository& ues, ue_pdsch_allocator& pdsch_alloc, unsigned max_rbs);
  /// \brief Attempts to allocate PUSCH for a UE.
  /// \return Returns allocation status, nof. allocated bytes and nof. allocated RBs.
  ul_alloc_result
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
      if (elem->dl_prio == forbid_prio) {
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
      if (elem->ul_prio == forbid_prio) {
        return;
      }
      base_type::push(elem);
    }
  };

  slot_point last_pdsch_slot;
  slot_point last_pusch_slot;

  /// Priority queue of UEs to be scheduled in DL. The UE in front of the queue has highest priority and vice versa.
  ue_dl_queue_t dl_queue;
  /// Priority queue of UEs to be scheduled in UL. The UE in front of the queue has highest priority and vice versa.
  ue_ul_queue_t ul_queue;
};

} // namespace srsran
