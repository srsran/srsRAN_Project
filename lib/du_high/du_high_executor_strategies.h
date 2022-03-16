
#ifndef SRSGNB_DU_HIGH_EXECUTOR_STRATEGIES_H
#define SRSGNB_DU_HIGH_EXECUTOR_STRATEGIES_H

#include "srsgnb/adt/circular_array.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/ran/du_l2_ul_executor_mapper.h"

namespace srsgnb {

/// L2 UL executor mapper that maps UEs based on their RNTI.
class rnti_ul_executor_mapper final : public du_l2_ul_executor_mapper
{
public:
  rnti_ul_executor_mapper(span<task_executor*> execs_) : execs(execs_.begin(), execs_.end()) {}

  virtual task_executor& rebind_executor(rnti_t rnti, du_cell_index_t pcell_index) override
  {
    // Static lookup
    return executor(rnti);
  }

  virtual task_executor& executor(rnti_t rnti) override { return *execs[rnti % execs.size()]; }

private:
  std::vector<task_executor*> execs;
};

/// L2 UL executor mapper that maps UEs based on their PCell.
class pcell_ul_executor_mapper final : public du_l2_ul_executor_mapper
{
public:
  explicit pcell_ul_executor_mapper(std::vector<std::unique_ptr<task_executor> > execs_) : execs(std::move(execs_))
  {
    for (auto& rnti_exec : rnti_to_exec) {
      rnti_exec = execs[0].get();
    }
  }

  virtual task_executor& rebind_executor(rnti_t rnti, du_cell_index_t pcell_index) override
  {
    rnti_to_exec[rnti] = execs[pcell_index % execs.size()].get();
    return *rnti_to_exec[rnti];
  }

  virtual task_executor& executor(rnti_t rnti) override { return *rnti_to_exec[rnti]; }

private:
  std::vector<std::unique_ptr<task_executor> > execs;
  circular_array<task_executor*, MAX_NOF_UES>  rnti_to_exec;
};

} // namespace srsgnb

#endif // SRSGNB_DU_HIGH_EXECUTOR_STRATEGIES_H
