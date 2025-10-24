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

#include "srsran/phy/generic_functions/generic_functions_factories.h"
#include "srsran/support/benchmark_utils.h"
#include "srsran/support/srsran_assert.h"
#include <getopt.h>
#include <random>

using namespace srsran;

// Random generator.
static std::mt19937 rgen(0);

static std::string fftw_optimization_str      = "estimate";
static double      fftw_plan_creation_timeout = 1.0;
static unsigned    nof_repetitions            = 1000;
static bool        silent                     = false;

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-F DFT factory] [-R repetitions]\n", prog);
  fmt::print("\t-O Select FFTW optimization flag (estimate, measure, exhaustive) [Default {}]\n",
             fftw_optimization_str);
  fmt::print("\t-T Select FFTW plan creation maximum time in seconds, set to zero or lower for infinite [Default {}]\n",
             fftw_plan_creation_timeout);
  fmt::print("\t-R Repetitions [Default {}]\n", nof_repetitions);
  fmt::print("\t-s Toggle silent operation [Default {}]\n", silent);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:O:T:sh")) != -1) {
    switch (opt) {
      case 'R':
        nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'O':
        fftw_optimization_str = std::string(optarg);
        break;
      case 'T':
        fftw_plan_creation_timeout = std::strtod(optarg, nullptr);
        break;
      case 's':
        silent = (!silent);
        break;
      case 'h':
      default:
        usage(argv[0]);
        std::exit(0);
    }
  }
}

int main(int argc, char** argv)
{
  parse_args(argc, argv);

  std::uniform_real_distribution<float> dist(-1.0, +1.0);

  std::shared_ptr<dft_processor_factory> generic_dft_factory = create_dft_processor_factory_generic();

  std::shared_ptr<dft_processor_factory> fftw_dft_factory =
      create_dft_processor_factory_fftw(fftw_optimization_str, fftw_plan_creation_timeout);

  // Create DFT for 16-bit complex integers. It might not be available for machines other than x86.
  std::shared_ptr<dft_processor_ci16_factory> dft_ci16_factory = create_dft_processor_ci16_factory_avx2();

  benchmarker perf_meas("DFT", nof_repetitions);

  // Test for the most common DFT sizes
  for (unsigned size :
       {128, 256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096, 4608, 6144, 9216, 12288, 18432, 24576, 36864, 49152}) {
    for (dft_processor::direction direction : {dft_processor::direction::DIRECT, dft_processor::direction::INVERSE}) {
      // Create FFTW configuration;
      dft_processor::configuration config;
      config.size = size;
      config.dir  = direction;

      // Benchmark generic DFT if available.
      {
        std::unique_ptr<dft_processor> dft = generic_dft_factory->create(config);
        if (dft != nullptr) {
          // Get DFT input buffer
          span<cf_t> input = dft->get_input();

          // Generate input random data.
          for (cf_t& value : input) {
            value = {dist(rgen), dist(rgen)};
          }

          // Measure performance.
          perf_meas.new_measure(fmt::format("generic {} {}", size, dft_processor::direction_to_string(direction)),
                                size,
                                [&dft]() { dft->run(); });
        }
      }

      // Benchmark FFTW DFT if available.
      {
        std::unique_ptr<dft_processor> dft = fftw_dft_factory->create(config);
        if (dft != nullptr) {
          // Get DFT input buffer
          span<cf_t> input = dft->get_input();

          // Generate input random data.
          for (cf_t& value : input) {
            value = {dist(rgen), dist(rgen)};
          }

          // Measure performance.
          perf_meas.new_measure(fmt::format("fftw {} {}", size, dft_processor::direction_to_string(direction)),
                                size,
                                [&dft]() { dft->run(); });
        }
      }

      // Benchmark FFTW DFT if available.
      if (dft_ci16_factory) {
        dft_processor_ci16::configuration config_ci16;
        config_ci16.size = size;
        config_ci16.dir  = direction == dft_processor::direction::DIRECT ? dft_processor_ci16::direction::direct
                                                                         : dft_processor_ci16::direction::inverse;

        std::unique_ptr<dft_processor_ci16> dft = dft_ci16_factory->create(config_ci16);
        if (dft != nullptr) {
          std::vector<ci16_t> input_ci16(size);
          std::vector<ci16_t> output_ci16(size);

          for (ci16_t& value : input_ci16) {
            value = to_ci16({dist(rgen), dist(rgen)});
          }

          // Measure performance.
          perf_meas.new_measure(fmt::format("ci16 {} {}", size, dft_processor::direction_to_string(direction)),
                                size,
                                [&dft, &input_ci16, &output_ci16]() { dft->run(output_ci16, input_ci16); });
        }
      }
    }
  }

  if (!silent) {
    perf_meas.print_percentiles_throughput("samples");
  }

  return 0;
}
