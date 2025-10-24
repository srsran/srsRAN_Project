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

#include "test_du_high_worker_manager.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/support/executors/inline_task_executor.h"
#include "srsran/support/executors/priority_task_worker.h"
#include "srsran/support/executors/strand_executor.h"
#include "srsran/support/executors/task_worker_pool.h"
#include "srsran/support/timers.h"

using namespace srsran;
using namespace srs_du;

namespace {

// setup an affinity mask made of the specified core indexes.
os_sched_affinity_bitmask get_du_cell_affinity_mask(span<const unsigned> du_cell_cores)
{
  os_sched_affinity_bitmask mask;
  if (not du_cell_cores.empty()) {
    for (auto core : du_cell_cores) {
      mask.set(core);
    }
  }
  return mask;
}

// setup an affinity mask made of the cores not in the list of specified ids.
std::vector<os_sched_affinity_bitmask> get_other_affinity_mask(span<const unsigned> du_cell_cores)
{
  if (du_cell_cores.empty()) {
    return {};
  }
  os_sched_affinity_bitmask mask;
  for (unsigned i = 0; i != mask.size(); ++i) {
    if (std::find(du_cell_cores.begin(), du_cell_cores.end(), i) == du_cell_cores.end()) {
      mask.set(i);
    }
  }
  return {mask};
}

class multithreaded_du_high_executor_mapper final : public test_helpers::du_high_worker_manager
{
  static constexpr unsigned nof_low_prio_threads   = 3;
  static constexpr unsigned task_worker_queue_size = 10000;

public:
  multithreaded_du_high_executor_mapper(const test_helpers::du_high_worker_config& cfg) :
    worker_pool("low_prio_pool",
                nof_low_prio_threads,
                std::array<concurrent_queue_params, 2>{
                    concurrent_queue_params{concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size},
                    concurrent_queue_params{concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}},
                std::chrono::microseconds{100},
                os_thread_realtime_priority::no_realtime(),
                get_other_affinity_mask(cfg.du_cell_cores))
  {
    cell_workers.reserve(cfg.nof_cell_workers);
    for (unsigned i = 0; i != cfg.nof_cell_workers; ++i) {
      cell_workers.push_back(std::make_unique<priority_task_worker>(
          "du_cell",
          std::vector<concurrent_queue_params>({{concurrent_queue_policy::lockfree_spsc, 8},
                                                {concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size}}),
          std::chrono::microseconds{10},
          cfg.use_os_thread_prio ? os_thread_realtime_priority::max() - 10 : os_thread_realtime_priority::no_realtime(),
          get_du_cell_affinity_mask(cfg.du_cell_cores)));
      slot_execs.push_back(make_priority_task_worker_executor(enqueue_priority::max, *cell_workers.back()));
      cell_execs.push_back(make_priority_task_worker_executor(enqueue_priority::max - 1, *cell_workers.back()));
    }

    // Instantiate du-high executor mapper.
    srs_du::du_high_executor_config exec_cfg;
    auto& cells = exec_cfg.cell_executors.emplace<srs_du::du_high_executor_config::dedicated_cell_worker_list>();
    for (unsigned i = 0; i != slot_execs.size(); ++i) {
      cells.push_back(srs_du::du_high_executor_config::dedicated_cell_worker{&slot_execs[i], &cell_execs[i]});
    }
    exec_cfg.ue_executors   = {srs_du::du_high_executor_config::ue_executor_config::map_policy::per_cell,
                               cfg.nof_cell_workers,
                               task_worker_queue_size,
                               task_worker_queue_size,
                               &low_prio_exec,
                               &low_prio_exec};
    exec_cfg.ctrl_executors = {task_worker_queue_size, &high_prio_exec};

    exec_mapper = srs_du::create_du_high_executor_mapper(exec_cfg);

    timer_strand = make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(high_prio_exec, task_worker_queue_size);
  }

  void stop() override
  {
    for (auto& w : cell_workers) {
      w->stop();
    }
    worker_pool.stop();
  }

  du_high_executor_mapper& get_exec_mapper() override { return *exec_mapper; }

  task_executor& timer_executor() override { return *timer_strand; }

  void wait_pending_tasks() override { worker_pool.wait_pending_tasks(); }

private:
  // Instantiated workers.
  priority_task_worker_pool                          worker_pool;
  priority_task_worker_pool_executor                 high_prio_exec{enqueue_priority::max, worker_pool};
  priority_task_worker_pool_executor                 low_prio_exec{enqueue_priority::max - 1, worker_pool};
  std::vector<std::unique_ptr<priority_task_worker>> cell_workers;
  std::vector<priority_task_worker_executor>         slot_execs;
  std::vector<priority_task_worker_executor>         cell_execs;
  std::unique_ptr<task_executor>                     timer_strand;

  std::unique_ptr<du_high_executor_mapper> exec_mapper;
};

class preinitialize_tls_resources : public unique_thread::observer
{
public:
  preinitialize_tls_resources(timer_manager& timers_) : timers(timers_) {}

  void on_thread_creation() override
  {
    // Pre-initialize thread-local resources to avoid doing it in the critical path.
    init_byte_buffer_segment_pool_tls();
    // Pre-initialize timer queues to avoid doing it in the critical path.
    inline_task_executor dummy_executor;
    auto                 dummy_timer = timers.create_unique_timer(dummy_executor);
    dummy_timer.stop();
  }

  void on_thread_destruction() override {}

private:
  timer_manager& timers;
};

} // namespace

std::unique_ptr<test_helpers::du_high_worker_manager>
srsran::test_helpers::create_multi_threaded_du_high_executor_mapper(const du_high_worker_config& cfg)
{
  unique_thread::add_observer(std::make_unique<preinitialize_tls_resources>(cfg.timers));

  return std::make_unique<multithreaded_du_high_executor_mapper>(cfg);
}
