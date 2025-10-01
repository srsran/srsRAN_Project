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
#include "srsran/support/executors/executor_decoration_factory.h"
#include "srsran/support/executors/strand_executor.h"

using namespace srsran;
using namespace srs_cu_cp;

namespace {

class cu_cp_executor_mapper_impl : public cu_cp_executor_mapper
{
  static constexpr uint32_t task_worker_queue_size = 2048;
  using strand_type = task_strand<task_executor*, concurrent_queue_policy::lockfree_mpmc>;

public:
  cu_cp_executor_mapper_impl(const strand_based_executor_config& config) :
    pool_exec(config.pool_executor),
    ctrl_strand(std::make_unique<strand_type>(&config.pool_executor, task_worker_queue_size))
  {
    // Decorate strand if needed.
    if (config.exec_metrics_backend) {
      execution_decoration_config cfg;
      cfg.metrics.emplace("cu_cp_exec", *config.exec_metrics_backend, false);
      ctrl_strand = decorate_executor(std::move(ctrl_strand), cfg);
    }
  }

  task_executor& ctrl_executor() override { return *ctrl_strand; }

  task_executor& e2_executor() override
  {
    // Note: For now, we reuse the same strand for E2 tasks.
    return *ctrl_strand;
  }

  task_executor& n2_rx_executor() override { return pool_exec; }

  task_executor& f1c_rx_executor() override { return pool_exec; }

  task_executor& e1_rx_executor() override { return pool_exec; }

  task_executor& e2_rx_executor() override { return pool_exec; }

private:
  task_executor& pool_exec;

  std::unique_ptr<task_executor> ctrl_strand;
};

} // namespace

std::unique_ptr<cu_cp_executor_mapper> srs_cu_cp::make_cu_cp_executor_mapper(const strand_based_executor_config& config)
{
  return std::make_unique<cu_cp_executor_mapper_impl>(config);
}
