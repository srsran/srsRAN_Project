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

#include "srsran/cu_up/cu_up_executor_pool.h"

using namespace srsran;

class cu_up_executor_mapper_impl : public cu_up_executor_pool
{
public:
  cu_up_executor_mapper_impl(span<task_executor*> dl_executors,
                             span<task_executor*> ul_executors,
                             span<task_executor*> ctrl_executors)
  {
    dl_execs.assign(dl_executors.begin(), dl_executors.end());
    ul_execs.assign(ul_executors.begin(), ul_executors.end());
    ctrl_execs.assign(ctrl_executors.begin(), ctrl_executors.end());
  }

  ptr create_dl_pdu_executor() override
  {
    return {dl_execs[round_robin_dl_index.fetch_add(1, std::memory_order_relaxed) % dl_execs.size()],
            [this](task_executor* p) {
              if (p != nullptr) {
                dealloc_pdu_executor(p);
              }
            }};
  }

  ptr create_ul_pdu_executor() override
  {
    return {ul_execs[round_robin_ul_index.fetch_add(1, std::memory_order_relaxed) % ul_execs.size()],
            [this](task_executor* p) {
              if (p != nullptr) {
                dealloc_pdu_executor(p);
              }
            }};
  }

  ptr create_ctrl_executor() override
  {
    return {ctrl_execs[round_robin_ctrl_index.fetch_add(1, std::memory_order_relaxed) % ctrl_execs.size()],
            [this](task_executor* p) {
              if (p != nullptr) {
                dealloc_ctrl_executor(p);
              }
            }};
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

  std::vector<task_executor*> dl_execs;
  std::vector<task_executor*> ul_execs;
  std::vector<task_executor*> ctrl_execs;

  std::atomic<uint32_t> round_robin_dl_index{0};
  std::atomic<uint32_t> round_robin_ul_index{0};
  std::atomic<uint32_t> round_robin_ctrl_index{0};
};

std::unique_ptr<cu_up_executor_pool> srsran::make_cu_up_executor_mapper(span<task_executor*> dl_executors,
                                                                        span<task_executor*> ul_executors,
                                                                        span<task_executor*> ctrl_executors)
{
  return std::make_unique<cu_up_executor_mapper_impl>(dl_executors, ul_executors, ctrl_executors);
}
