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
