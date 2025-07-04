/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/cu_cp/cu_cp_executor_mapper.h"
#include "srsran/adt/mpmc_queue.h"
#include "srsran/support/executors/strand_executor.h"

using namespace srsran;
using namespace srs_cu_cp;

namespace {

class cu_cp_executor_mapper_impl : public cu_cp_executor_mapper
{
  static constexpr uint32_t task_worker_queue_size = 2048;

public:
  cu_cp_executor_mapper_impl(task_executor& pool_exec_) :
    pool_exec(pool_exec_), ctrl_strand(&pool_exec_, task_worker_queue_size)
  {
  }

  task_executor& ctrl_executor() override { return ctrl_strand; }

  task_executor& e2_executor() override
  {
    // Note: For now, we reuse the same strand for E2 tasks.
    return ctrl_strand;
  }

  task_executor& n2_rx_executor() override { return pool_exec; }

private:
  using strand_type = task_strand<task_executor*, concurrent_queue_policy::lockfree_mpmc>;

  task_executor& pool_exec;
  strand_type    ctrl_strand;
};

} // namespace

std::unique_ptr<cu_cp_executor_mapper> srs_cu_cp::make_cu_cp_executor_mapper(const strand_based_executor_config& config)
{
  return std::make_unique<cu_cp_executor_mapper_impl>(config.pool_executor);
}
