/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/srslog/srslog.h"

using namespace srslog;

static constexpr unsigned num_iterations = 4000000;

int main()
{
  auto& s       = srslog::fetch_file_sink("srslog_latency_benchmark.txt");
  auto& channel = srslog::fetch_log_channel("bench", s, {});

  srslog::init(srslog::backend_priority::high);

  auto begin = std::chrono::steady_clock::now();
  for (unsigned i = 0; i != num_iterations; ++i) {
    double d = i;
    channel("SRSLOG backend benchmark: int: {}, double: {}", i, d);
  }
  srslog::flush();
  auto end = std::chrono::steady_clock::now();

  fmt::print("SRSLOG Backend throughput Benchmark:\n"
             "  benchmark run for {} ms and logged {} times\n"
             "  {:.1f} million logs/sec",
             std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count(),
             num_iterations,
             num_iterations / std::chrono::duration_cast<std::chrono::duration<double>>(end - begin).count() / 1e6);

  return 0;
}
