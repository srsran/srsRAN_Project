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

#include "srsran/support/benchmark_utils.h"
#include "srsran/support/cpu_architecture_info.h"
#include "srsran/support/executors/task_worker_pool.h"
#include <getopt.h>
#include <variant>

using namespace srsran;

struct bench_params {
  std::chrono::milliseconds duration{0};
  unsigned                  max_workers = cpu_architecture_info::get().get_host_nof_available_cpus();
  std::string               output_file{"stdout"};
};

static void usage(const char* prog, const bench_params& params)
{
  fmt::print("Usage: {} [-D duration] [-t max_workers] [-o output file]\n", prog);
  fmt::print("\t-D Duration in milliseconds [Default {}]\n", params.duration.count());
  fmt::print("\t-t Maximum number of workers [Default {}]\n", params.max_workers);
  fmt::print("\t-o Output file [Default: stdout]\n");
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv, bench_params& params)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "D:t:o:h")) != -1) {
    switch (opt) {
      case 'D':
        params.duration = std::chrono::milliseconds{std::strtol(optarg, nullptr, 10)};
        break;
      case 't':
        params.max_workers = std::strtol(optarg, nullptr, 10);
        break;
      case 'o':
        params.output_file = optarg;
        break;
      case 'h':
      default:
        usage(argv[0], params);
        std::exit(0);
    }
  }
}

struct benchmark_result {
  std::string               description;
  int64_t                   task_count{0};
  std::chrono::microseconds duration{0};
};

class benchmark_environment
{
public:
  using pool_storage =
      std::variant<std::unique_ptr<task_worker_pool<concurrent_queue_policy::lockfree_mpmc>>,
                   std::unique_ptr<task_worker_pool<concurrent_queue_policy::moodycamel_lockfree_mpmc>>>;

  benchmark_environment(concurrent_queue_policy   policy,
                        unsigned                  nof_workers_,
                        unsigned                  qsize_,
                        std::chrono::microseconds task_dur_) :
    nof_workers(nof_workers_), qsize(qsize_), task_dur(task_dur_)
  {
    if (policy == concurrent_queue_policy::lockfree_mpmc) {
      auto pool =
          std::make_unique<task_worker_pool<concurrent_queue_policy::lockfree_mpmc>>("mpmc", nof_workers, qsize);
      task_exec   = make_task_worker_pool_executor_ptr(*pool);
      storage     = std::move(pool);
      description = "lockfree_mpmc";
    } else if (policy == concurrent_queue_policy::moodycamel_lockfree_mpmc) {
      auto pool = std::make_unique<task_worker_pool<concurrent_queue_policy::moodycamel_lockfree_mpmc>>(
          "moodyl_mpmc", nof_workers, qsize);
      task_exec   = make_task_worker_pool_executor_ptr(*pool);
      storage     = std::move(pool);
      description = "moodycamel_mpmc";
    } else {
      throw std::invalid_argument("Unsupported queue policy");
    }
    description += ",workers=" + std::to_string(nof_workers) + ",task_dur=" + std::to_string(task_dur.count()) + "us";
  }

  task_executor& executor() { return *task_exec; }

  const std::string& get_description() const { return description; }

  void start()
  {
    fmt::print("STATUS: Starting {}...", description);
    unsigned nof_initial_tasks = nof_workers;
    for (unsigned i = 0; i != nof_initial_tasks; ++i) {
      while (not task_exec->defer([this]() { run_task(); })) {
        fmt::print("Unexpected failure to defer initial task, retrying...\n");
        std::this_thread::sleep_for(std::chrono::microseconds{1});
      }
    }
    reset_counters();
  }

  void reset_counters()
  {
    task_counter.store(0, std::memory_order_relaxed);
    start_time = std::chrono::high_resolution_clock::now();
  }

  void stop()
  {
    auto stop_time     = std::chrono::high_resolution_clock::now();
    result.task_count  = task_counter.load(std::memory_order_relaxed);
    result.duration    = std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time);
    result.description = description;
    running.store(false, std::memory_order_relaxed);

    fmt::print(" Done.\n");

    if (auto* pool = std::get_if<std::unique_ptr<task_worker_pool<concurrent_queue_policy::lockfree_mpmc>>>(&storage)) {
      (*pool)->wait_pending_tasks();
      (*pool)->stop();
    } else if (auto* moody_pool =
                   std::get_if<std::unique_ptr<task_worker_pool<concurrent_queue_policy::moodycamel_lockfree_mpmc>>>(
                       &storage)) {
      (*moody_pool)->wait_pending_tasks();
      (*moody_pool)->stop();
    } else {
      report_fatal_error("Unexpected storage type in benchmark_environment");
    }
  }

  benchmark_result get_result() const { return result; }

private:
  void run_task()
  {
    if (running.load(std::memory_order_relaxed)) {
      auto task_start = std::chrono::high_resolution_clock::now();
      while (std::chrono::high_resolution_clock::now() - task_start < task_dur) {
        // Simulate some work.
        do_not_optimize(running.load(std::memory_order_relaxed));
      }
      task_counter.fetch_add(1, std::memory_order_relaxed);
      bool success = false;
      for (unsigned nof_attempts = 100000; nof_attempts > 0; --nof_attempts) {
        success = task_exec->defer([this]() { run_task(); });
        if (success) {
          break;
        }
        // Yield to avoid busy waiting.
        std::this_thread::sleep_for(std::chrono::microseconds{1});
      }
      report_fatal_error_if_not(success, "Unexpected failure to defer task");
    }
  }

  const unsigned                  nof_workers;
  const unsigned                  qsize;
  pool_storage                    storage;
  std::unique_ptr<task_executor>  task_exec;
  std::string                     description;
  const std::chrono::microseconds task_dur;

  std::atomic<bool>                              running{true};
  std::chrono::high_resolution_clock::time_point start_time;
  std::atomic<uint64_t>                          task_counter{0};

  benchmark_result result;
};

benchmark_result run_throughput_benchmark(benchmark_environment& env, const bench_params& params)
{
  env.start();

  // Let it warm up.
  auto warmup_duration = std::min(std::chrono::milliseconds{100}, params.duration);
  std::this_thread::sleep_for(warmup_duration);
  env.reset_counters();

  // Collect results...
  std::this_thread::sleep_for(params.duration);
  env.stop();

  return env.get_result();
}

void run_benchmarks(const bench_params& params)
{
  std::vector<unsigned> nof_workers_list{1};
  while (true) {
    unsigned next_nof_workers = std::min(nof_workers_list.back() * 2, nof_workers_list.back() + 4);
    if (next_nof_workers > params.max_workers) {
      break;
    }
    nof_workers_list.push_back(next_nof_workers);
  }
  std::vector<concurrent_queue_policy> policies = {
      concurrent_queue_policy::lockfree_mpmc,
      concurrent_queue_policy::moodycamel_lockfree_mpmc,
  };
  std::vector<std::chrono::microseconds> task_durations{std::chrono::microseconds{0}, std::chrono::microseconds{10}};
  const unsigned                         qsize = 8192;

  fmt::print("Running benchmark with parameters: duration={}msec\n", params.duration.count());

  std::vector<benchmark_result> results;
  for (concurrent_queue_policy policy : policies) {
    for (unsigned nof_workers : nof_workers_list) {
      for (std::chrono::microseconds task_dur : task_durations) {
        benchmark_environment env{policy, nof_workers, qsize, task_dur};
        results.push_back(run_throughput_benchmark(env, params));
      }
    }
  }

  FILE* out_file = nullptr;
  if (params.output_file == "stdout") {
    out_file = stdout;
  } else {
    out_file = fopen(params.output_file.c_str(), "w");
    report_fatal_error_if_not(out_file, "Failed to open output file: {}", params.output_file);
  }

  fmt::print(out_file, "\nBenchmark results:\n");
  for (const benchmark_result& result : results) {
    fmt::print(out_file,
               "{}: task_count={}, duration={} usec, tasks-per-sec={}\n",
               result.description,
               result.task_count,
               result.duration.count(),
               result.duration.count() > 0 ? result.task_count * 1000000 / result.duration.count() : 0);
  }

  if (out_file != stdout) {
    fclose(out_file);
    fmt::print("Results written to {}\n", params.output_file);
  }
}

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("ALL").set_level(srslog::basic_levels::warning);
  srslog::init();

  bench_params params{};
  parse_args(argc, argv, params);

  // Run benchmarks for different queue policies.
  run_benchmarks(params);
}
