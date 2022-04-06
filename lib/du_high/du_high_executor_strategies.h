
#ifndef SRSGNB_DU_HIGH_EXECUTOR_STRATEGIES_H
#define SRSGNB_DU_HIGH_EXECUTOR_STRATEGIES_H

#include "srsgnb/adt/circular_array.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/ran/du_l2_ul_executor_mapper.h"

namespace srsgnb {

/// L2 UL executor mapper that maps UEs based on their RNTI.
class ue_index_based_ul_executor_mapper final : public du_l2_ul_executor_mapper
{
public:
  ue_index_based_ul_executor_mapper(span<task_executor*> execs_) : execs(execs_.begin(), execs_.end()) {}

  task_executor& rebind_executor(du_ue_index_t ue_index, du_cell_index_t pcell_index) override
  {
    // Static lookup
    srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid ueIdx={}", ue_index);
    return executor(ue_index);
  }

  task_executor& executor(du_ue_index_t ue_index) override
  {
    ue_index = ue_index < MAX_NOF_UES ? ue_index : 0;
    return *execs[ue_index % execs.size()];
  }

private:
  std::vector<task_executor*> execs;
};

/// L2 UL executor mapper that maps UEs based on their PCell.
class pcell_ul_executor_mapper final : public du_l2_ul_executor_mapper
{
public:
  explicit pcell_ul_executor_mapper(std::vector<std::unique_ptr<task_executor> > execs_) : execs(std::move(execs_))
  {
    // Initialize executors in a round-robin fashion.
    unsigned count = 0;
    for (auto& rnti_exec : ue_idx_to_exec) {
      rnti_exec = execs[count % execs.size()].get();
      count++;
    }
  }

  task_executor& rebind_executor(du_ue_index_t ue_index, du_cell_index_t pcell_index) override
  {
    srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid ueIdx={}", ue_index);
    ue_idx_to_exec[ue_index] = execs[pcell_index % execs.size()].get();
    return *ue_idx_to_exec[ue_index];
  }

  task_executor& executor(du_ue_index_t ue_index) override
  {
    ue_index = ue_index < MAX_NOF_UES ? ue_index : 0;
    return *ue_idx_to_exec[ue_index];
  }

private:
  std::vector<std::unique_ptr<task_executor> > execs;
  std::array<task_executor*, MAX_NOF_UES>      ue_idx_to_exec;
};

} // namespace srsgnb

#endif // SRSGNB_DU_HIGH_EXECUTOR_STRATEGIES_H
