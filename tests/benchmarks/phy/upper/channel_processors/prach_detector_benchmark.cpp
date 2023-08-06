/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "srsran/support/benchmark_utils.h"
#include "srsran/support/complex_normal_random.h"
#include "srsran/support/error_handling.h"
#include <getopt.h>
#include <random>
#include <set>

using namespace srsran;

// General test configuration parameters.
static uint64_t                    nof_repetitions  = 1000;
static std::set<unsigned>          set_nof_rx_ports = {1, 2, 4};
static std::set<prach_format_type> set_format       = {prach_format_type::zero, prach_format_type::B4};
static std::set<unsigned>          set_zcz          = {0, 1, 14};

// Global pseudo-random generator.
static std::mt19937 rgen(0);

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-R repetitions]\n", prog);
  fmt::print("\t-R Repetitions [Default {}]\n", nof_repetitions);
  fmt::print("\t-p Fix the number of ports\n");
  fmt::print("\t-f Fix the PRACH format\n");
  fmt::print("\t-z Fix the Zero Correlation Zone configuration index.\n");
  fmt::print("\t-h Show this message\n");
}

static int parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:p:f:z:h")) != -1) {
    switch (opt) {
      case 'R':
        nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'p':
        set_nof_rx_ports = {static_cast<unsigned>(std::strtol(optarg, nullptr, 10))};
        break;
      case 'f':
        set_format = {to_prach_format_type(optarg)};
        break;
      case 'z':
        set_zcz = {static_cast<unsigned>(std::strtol(optarg, nullptr, 10))};
        break;
      case 'h':
      default:
        usage(argv[0]);
        exit(0);
    }
  }

  return 0;
}

static prach_detector_factory& get_detector_factory()
{
  static std::shared_ptr<prach_detector_factory> prach_det_factory = nullptr;

  if (prach_det_factory) {
    return *prach_det_factory;
  }

  std::shared_ptr<dft_processor_factory> dft_proc_factory = create_dft_processor_factory_fftw_fast();
  report_fatal_error_if_not(dft_proc_factory, "Failed to create DFT processor factory.");

  std::shared_ptr<prach_generator_factory> prach_gen_factory = create_prach_generator_factory_sw();
  report_fatal_error_if_not(prach_gen_factory, "Failed to create PRACH generator factory.");

  prach_det_factory = create_prach_detector_factory_sw(dft_proc_factory, prach_gen_factory);
  report_fatal_error_if_not(prach_det_factory, "Failed to create PRACH detector factory.");

  return *prach_det_factory;
}

// Instantiates a PRACH validator.
static std::unique_ptr<prach_detector_validator> create_validator()
{
  return get_detector_factory().create_validator();
}

// Instantiates a PRACH detector.
static std::unique_ptr<prach_detector> create_detector()
{
  return get_detector_factory().create();
}

// Create buffer.
static std::unique_ptr<prach_buffer> create_buffer(prach_format_type format, unsigned nof_antennas)
{
  complex_normal_distribution<cf_t> complex_float_dist(cf_t(1, 1), std::sqrt(2.0F));

  std::unique_ptr<prach_buffer> buffer;

  if (is_long_preamble(format)) {
    buffer = create_prach_buffer_long(nof_antennas, 1);
  } else {
    buffer = create_prach_buffer_short(nof_antennas, 1, 1);
  }
  report_fatal_error_if_not(buffer, "Failed to create buffer.");

  for (unsigned i_antenna = 0; i_antenna != nof_antennas; ++i_antenna) {
    for (unsigned i_symbol = 0, i_symbol_end = buffer->get_max_nof_symbols(); i_symbol != i_symbol_end; ++i_symbol) {
      span<cf_t> prach_symbol = buffer->get_symbol(i_antenna, 0, 0, i_symbol);
      for (cf_t& sample : prach_symbol) {
        sample = complex_float_dist(rgen);
      }
    }
  }

  return buffer;
}

std::vector<prach_detector::configuration> generate_test_cases()
{
  std::vector<prach_detector::configuration> test_cases;
  for (unsigned nof_rx_ports : set_nof_rx_ports) {
    for (prach_format_type format : set_format) {
      std::uniform_int_distribution<unsigned> root_sequence_index_dist(0, (is_long_preamble(format) ? 837 : 138));

      for (unsigned zcz : set_zcz) {
        if (((format == prach_format_type::zero) && (zcz == 14)) || ((format == prach_format_type::B4) && (zcz == 1))) {
          continue;
        }
        // Create new test case.
        test_cases.emplace_back();

        // Select test case.
        prach_detector::configuration& config = test_cases.back();

        // Select RA subcarrier spacing. Set to 30kHz for all short preambles.
        prach_subcarrier_spacing ra_scs = prach_subcarrier_spacing::kHz30;
        if (is_long_preamble(format)) {
          if (format == prach_format_type::three) {
            ra_scs = prach_subcarrier_spacing::kHz5;
          } else {
            ra_scs = prach_subcarrier_spacing::kHz1_25;
          }
        }

        // Fill test case.
        config.root_sequence_index   = root_sequence_index_dist(rgen);
        config.format                = format;
        config.restricted_set        = restricted_set_config::UNRESTRICTED;
        config.zero_correlation_zone = zcz;
        config.start_preamble_index  = 0;
        config.nof_preamble_indices  = 64;
        config.ra_scs                = ra_scs;
        config.nof_rx_ports          = nof_rx_ports;
      }
    }
  }

  return test_cases;
}

int main(int argc, char** argv)
{
  int ret = parse_args(argc, argv);
  if (ret < 0) {
    return ret;
  }

  benchmarker perf_meas("PRACH detector", nof_repetitions);

  // Generate the test cases.
  std::vector<prach_detector::configuration> configurations = generate_test_cases();

  // Create validator.
  std::unique_ptr<prach_detector_validator> validator = create_validator();

  // Create detector.
  std::unique_ptr<prach_detector> detector = create_detector();

  for (const prach_detector::configuration& config : configurations) {
    // Make sure PRACH configuration is valid.
    report_fatal_error_if_not(validator->is_valid(config), "Invalid PRACH detector configuration {}.", config);

    // Create PRACH buffer.
    std::unique_ptr<prach_buffer> buffer = create_buffer(config.format, config.nof_rx_ports);
    report_fatal_error_if_not(buffer, "Failed to create buffer.");

    // Measurement description.
    fmt::memory_buffer meas_description;
    fmt::format_to(meas_description, "{}", config);

    // Run the benchmark.
    perf_meas.new_measure(
        to_string(meas_description), 1, [&detector, &buffer, &config]() { detector->detect(*buffer, config); });
  }

  // Print processing time.
  perf_meas.print_percentiles_time("microseconds", 1e-3);

  return 0;
}
