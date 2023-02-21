/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/circular_array.h"
#include "srsran/adt/span.h"
#include "srsran/du_high/du_high_cell_executor_mapper.h"
#include "srsran/du_high/du_high_ue_executor_mapper.h"
#include "srsran/support/executors/sync_task_executor.h"

namespace srsran {

/// L2 UL executor mapper that maps UEs based on their RNTI.
class ue_index_based_ul_executor_mapper final : public du_high_ue_executor_mapper
{
public:
  ue_index_based_ul_executor_mapper(span<task_executor*> execs_) : execs(execs_.begin(), execs_.end()) {}

  task_executor& rebind_executor(du_ue_index_t ue_index, du_cell_index_t pcell_index) override
  {
    // Static lookup
    srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid ueId={}", ue_index);
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
    srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid ueId={}", ue_index);
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

/// \brief Mapper of task executors used by the MAC DL, RLC DL and MAC scheduler for low-latency tasks. The task
/// executors can be mapped based on cell index or type of task (slot indication vs others).
class cell_executor_mapper final : public du_high_cell_executor_mapper
{
public:
  /// \brief Creates a cell_executor_mapper instance.
  /// \param cell_execs_ List of task executors that will be used by the MAC DL and scheduler.
  /// \param blocking_slot_ind sets whether slot indication tasks are processed synchronously or asynchronously
  explicit cell_executor_mapper(const std::initializer_list<task_executor*>& cell_execs_, bool blocking_slot_ind) :
    cell_execs(cell_execs_.begin(), cell_execs_.end())
  {
    srsran_assert(not cell_execs.empty(), "The number of DL executors must be higher than 1");
    if (blocking_slot_ind) {
      // synchronours slot indication mode.
      srslog::fetch_basic_logger("MAC").debug("Synchronous slot indication processing selected for L2");
      for (task_executor* exec : cell_execs) {
        blocking_slot_execs.push_back(std::make_unique<sync_task_executor>(*exec));
        slot_execs.push_back(blocking_slot_execs.back().get());
      }
    } else {
      slot_execs = cell_execs;
    }
  }

  /// \brief Fetches a default task executor for a given cell.
  task_executor& executor(du_cell_index_t cell_index) override { return *cell_execs[cell_index % cell_execs.size()]; }

  /// \brief Fetches a task executor for slot indications, given a cell index.
  task_executor& slot_ind_executor(du_cell_index_t cell_index) override
  {
    return *slot_execs[cell_index % cell_execs.size()];
  }

private:
  /// Executors used to process tasks related to the MAC DL and scheduler other than the slot indications.
  std::vector<task_executor*> cell_execs;

  /// \brief Executors used to process slot indication tasks. These executors can be either the same as the
  /// \c cell_execs or point to a sync_task_executor adapter stored in \c blocking_slot_execs, depending on whether
  /// slot indication tasks are processed synchronously or asynchronously.
  std::vector<task_executor*> slot_execs;

  /// \brief Task executor adapter that forces processed tasks to run synchronously. This member is only used when
  /// the mode of blocking slot_indications tasks was selected.
  std::vector<std::unique_ptr<sync_task_executor>> blocking_slot_execs;
};

} // namespace srsran
