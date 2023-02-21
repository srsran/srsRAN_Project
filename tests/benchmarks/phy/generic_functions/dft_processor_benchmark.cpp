/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

static std::string dft_factory_str       = "generic";
static std::string fftw_optimization_str = "estimate";
static unsigned    nof_repetitions       = 1000;
static bool        silent                = false;

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-F DFT factory] [-R repetitions]\n", prog);
  fmt::print("\t-F Select DFT factory [Default {}]\n", dft_factory_str);
  fmt::print("\t-O Select FFTW optimization flag (estimate, measure) [Default {}]\n", dft_factory_str);
  fmt::print("\t-R Repetitions [Default {}]\n", nof_repetitions);
  fmt::print("\t-s Toggle silent operation [Default {}]\n", silent);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "F:R:O:sh")) != -1) {
    switch (opt) {
      case 'F':
        dft_factory_str = std::string(optarg);
        break;
      case 'R':
        nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'O':
        fftw_optimization_str = std::string(optarg);
        break;
      case 's':
        silent = (!silent);
        break;
      case 'h':
      default:
        usage(argv[0]);
        exit(0);
    }
  }
}

int main(int argc, char** argv)
{
  parse_args(argc, argv);

  std::uniform_real_distribution<float> dist(-1.0, +1.0);

  std::shared_ptr<dft_processor_factory> dft_factory = nullptr;
  if (dft_factory_str == "generic") {
    dft_factory = create_dft_processor_factory_generic();
  } else if (dft_factory_str == "fftw") {
    dft_factory = create_dft_processor_factory_fftw(fftw_optimization_str);
  } else {
    fmt::print("Invalid DFT factory.");
    return -1;
  }
  srsran_assert(dft_factory, "DFT factory of type {} is not available.", dft_factory_str);

  benchmarker perf_meas("DFT " + dft_factory_str, nof_repetitions);

  // Test for the most common DFT sizes
  for (unsigned size :
       {128, 256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096, 4608, 6144, 9216, 12288, 18432, 24576, 36864, 49152}) {
    for (dft_processor::direction direction : {dft_processor::direction::DIRECT, dft_processor::direction::INVERSE}) {
      // Performance measurements.
      std::vector<unsigned> perf_results;
      perf_results.reserve(nof_repetitions);

      // Create FFTW configuration;
      dft_processor::configuration config;
      config.size = size;
      config.dir  = direction;

      // Measurement description.
      std::string meas_descr =
          std::to_string(size) + " " + (direction == dft_processor::direction::DIRECT ? "direct" : "inverse");

      // Create processor
      std::unique_ptr<dft_processor> dft = dft_factory->create(config);

      // Skip processor silently if it is not available.
      if (dft == nullptr) {
        continue;
      }

      // Get DFT input buffer
      span<cf_t> input = dft->get_input();

      // Generate input random data.
      for (cf_t& value : input) {
        value = {dist(rgen), dist(rgen)};
      }

      // Measure performance.
      perf_meas.new_measure(meas_descr, size, [&]() { dft->run(); });
    }
  }

  if (!silent) {
    perf_meas.print_percentiles_throughput("samples");
  }

  return 0;
}
