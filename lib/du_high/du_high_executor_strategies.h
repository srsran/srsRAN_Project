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

#include "srsgnb/adt/circular_array.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/du_high/du_high_cell_executor_mapper.h"
#include "srsgnb/du_high/du_high_ue_executor_mapper.h"
#include "srsgnb/support/executors/sync_task_executor.h"

namespace srsgnb {

/// L2 UL executor mapper that maps UEs based on their RNTI.
class ue_index_based_ul_executor_mapper final : public du_high_ue_executor_mapper
{
public:
  ue_index_based_ul_executor_mapper(span<task_executor*> execs_) : execs(execs_.begin(), execs_.end()) {}

  task_executor& rebind_executor(du_ue_index_t ue_index, du_cell_index_t pcell_index) override
  {
    // Static lookup
    srsgnb_sanity_check(is_du_ue_index_valid(ue_index), "Invalid ueId={}", ue_index);
    return executor(ue_index);
  }

  task_executor& executor(du_ue_index_t ue_index) override
  {
    ue_index = ue_index < MAX_NOF_DU_UES ? ue_index : to_du_ue_index(0);
    return *execs[ue_index % execs.size()];
  }

private:
  std::vector<task_executor*> execs;
};

/// L2 UL executor mapper that maps UEs based on their PCell.
class pcell_ul_executor_mapper final : public du_high_ue_executor_mapper
{
public:
  explicit pcell_ul_executor_mapper(const std::initializer_list<task_executor*>& execs_) :
    execs(execs_.begin(), execs_.end())
  {
    // Initialize executors in a round-robin fashion.
    unsigned count = 0;
    for (auto& rnti_exec : ue_idx_to_exec) {
      rnti_exec = execs[count % execs.size()];
      count++;
    }
  }

  task_executor& rebind_executor(du_ue_index_t ue_index, du_cell_index_t pcell_index) override
  {
    srsgnb_sanity_check(is_du_ue_index_valid(ue_index), "Invalid ueId={}", ue_index);
    ue_idx_to_exec[ue_index] = execs[pcell_index % execs.size()];
    return *ue_idx_to_exec[ue_index];
  }

  task_executor& executor(du_ue_index_t ue_index) override
  {
    ue_index = ue_index < MAX_NOF_DU_UES ? ue_index : to_du_ue_index(0);
    return *ue_idx_to_exec[ue_index];
  }

private:
  std::vector<task_executor*>                execs;
  std::array<task_executor*, MAX_NOF_DU_UES> ue_idx_to_exec;
};

/// Dispatch cell-specific tasks based on cell index.
class cell_executor_mapper final : public du_high_cell_executor_mapper
{
public:
  explicit cell_executor_mapper(const std::initializer_list<task_executor*>& other_execs_, bool blocking_slot_ind) :
    cell_execs(other_execs_.begin(), other_execs_.end())
  {
    srsgnb_assert(not cell_execs.empty(), "The number of DL executors must be higher than 1");
    if (blocking_slot_ind) {
      for (task_executor* exec : cell_execs) {
        blocking_slot_execs.push_back(std::make_unique<sync_task_executor>(*exec));
        slot_execs.push_back(blocking_slot_execs.back().get());
      }
    } else {
      slot_execs = cell_execs;
    }
  }

  task_executor& executor(du_cell_index_t cell_index) override { return *cell_execs[cell_index % cell_execs.size()]; }

  task_executor& slot_ind_executor(du_cell_index_t cell_index) override
  {
    return *slot_execs[cell_index % cell_execs.size()];
  }

private:
  std::vector<task_executor*>                      cell_execs;
  std::vector<task_executor*>                      slot_execs;
  std::vector<std::unique_ptr<sync_task_executor>> blocking_slot_execs;
};

} // namespace srsgnb
