/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/cu_up/cu_up_executor_pool.h"

using namespace srsran;

class cu_up_executor_mapper_impl : public cu_up_executor_pool
{
public:
  cu_up_executor_mapper_impl(span<task_executor*> data_executors, span<task_executor*> ctrl_executors)
  {
    data_execs.assign(data_executors.begin(), data_executors.end());
    ctrl_execs.assign(ctrl_executors.begin(), ctrl_executors.end());
  }

  ptr create_pdu_executor() override
  {
    return std::unique_ptr<task_executor, unique_function<void(task_executor*)>>(
        data_execs[round_robin_data_index.fetch_add(1, std::memory_order_relaxed) % data_execs.size()],
        [this](task_executor* p) {
          if (p != nullptr) {
            dealloc_pdu_executor(p);
          }
        });
  }

  ptr create_ctrl_executor() override
  {
    return std::unique_ptr<task_executor, unique_function<void(task_executor*)>>(
        ctrl_execs[round_robin_data_index.fetch_add(1, std::memory_order_relaxed) % data_execs.size()],
        [this](task_executor* p) {
          if (p != nullptr) {
            dealloc_ctrl_executor(p);
          }
        });
  }

private:
  void dealloc_pdu_executor(task_executor* exec)
  {
    // do nothing.
  }
  void dealloc_ctrl_executor(task_executor* exec)
  {
    // do nothing.
  }

  std::vector<task_executor*> data_execs;
  std::vector<task_executor*> ctrl_execs;

  std::atomic<uint32_t> round_robin_data_index{0};
  std::atomic<uint32_t> round_robin_ctrl_index{0};
};

std::unique_ptr<cu_up_executor_pool> srsran::make_cu_up_executor_mapper(span<task_executor*> data_executors,
                                                                        span<task_executor*> ctrl_executors)
{
  return std::make_unique<cu_up_executor_mapper_impl>(data_executors, ctrl_executors);
}
