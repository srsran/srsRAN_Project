/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/security/s3g.h"
#include "srsran/support/benchmark_utils.h"
#include <random>

int main(int argc, char** argv)
{
  (void)argc;
  (void)argv;

  std::random_device                      rd;
  std::mt19937                            gen(rd());
  std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

  constexpr uint64_t nof_repetitions = 5000;

  srsran::security::S3G_STATE  state;
  srsran::security::S3G_STATE* state_ptr = &state;

  // Generate a random key and initialization vector.
  uint32_t k[4];
  uint32_t iv[4];
  for (int i = 0; i < 4; i++) {
    k[i]  = dist(gen);
    iv[i] = dist(gen);
  }

  srsran::benchmarker perf_meas("SNOW3G implementation", nof_repetitions);

  // Benchmark keystream generation.
  for (uint32_t nof_bytes : {20, 256, 1024, 4096}) {
    uint32_t              nof_words = nof_bytes / sizeof(uint32_t);
    std::vector<uint32_t> ks(nof_words, 0);
    std::string           description = fmt::format("Keystream generation ({} bytes)", nof_bytes);

    // Initialize SNOW3G state.
    s3g_initialize(state_ptr, k, iv);

    perf_meas.new_measure(description, 8 * nof_bytes, [&]() {
      // Generate keystream.
      s3g_generate_keystream(state_ptr, nof_words, ks.data());
    });

    // Deinitialize SNOW3G state.
    s3g_deinitialize(state_ptr);
  }

  perf_meas.print_percentiles_throughput("bits");

  return 0;
}
