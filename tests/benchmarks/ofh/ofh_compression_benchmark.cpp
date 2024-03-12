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

#include "srsran/ofh/compression/compression_factory.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/slot_point.h"
#include "srsran/support/benchmark_utils.h"
#include <getopt.h>
#include <random>

using namespace srsran;

// Random generator.
static std::mt19937 rgen(0);

static unsigned                 nof_repetitions = 10000;
static bool                     silent          = false;
static std::string              method          = "bfp";
static std::string              impl_type       = "auto";
static unsigned                 nof_ports       = 1;
static bs_channel_bandwidth_fr1 bw              = srsran::bs_channel_bandwidth_fr1::MHz20;
static subcarrier_spacing       scs             = subcarrier_spacing::kHz30;

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-R repetitions] [-T compression type] [-F factory type] [-s silent]\n", prog);
  fmt::print("\t-R Repetitions [Default {}]\n", nof_repetitions);
  fmt::print("\t-T Type of compression [{{'none', 'bfp'}}, default is {}]\n", method);
  fmt::print("\t-F Select compression factory [Default {}]\n", impl_type);
  fmt::print("\t-B Channel bandwidth [Default {}]\n", bw);
  fmt::print("\t-C Subcarrier spacing. [Default {}]\n", to_string(scs));
  fmt::print("\t-N Number of ports [from 1 to 4, default {}]\n", nof_ports);
  fmt::print("\t-h Show this message\n");
}

static bool validate_bw(unsigned bandwidth)
{
  switch (bandwidth) {
    case bs_channel_bandwidth_to_MHz(bs_channel_bandwidth_fr1::MHz5):
      bw = bs_channel_bandwidth_fr1::MHz5;
      break;
    case bs_channel_bandwidth_to_MHz(bs_channel_bandwidth_fr1::MHz10):
      bw = bs_channel_bandwidth_fr1::MHz10;
      break;
    case bs_channel_bandwidth_to_MHz(bs_channel_bandwidth_fr1::MHz15):
      bw = bs_channel_bandwidth_fr1::MHz15;
      break;
    case bs_channel_bandwidth_to_MHz(bs_channel_bandwidth_fr1::MHz20):
      bw = bs_channel_bandwidth_fr1::MHz20;
      break;
    case bs_channel_bandwidth_to_MHz(bs_channel_bandwidth_fr1::MHz25):
      bw = bs_channel_bandwidth_fr1::MHz25;
      break;
    case bs_channel_bandwidth_to_MHz(bs_channel_bandwidth_fr1::MHz30):
      bw = bs_channel_bandwidth_fr1::MHz30;
      break;
    case bs_channel_bandwidth_to_MHz(bs_channel_bandwidth_fr1::MHz40):
      bw = bs_channel_bandwidth_fr1::MHz40;
      break;
    case bs_channel_bandwidth_to_MHz(bs_channel_bandwidth_fr1::MHz50):
      bw = bs_channel_bandwidth_fr1::MHz50;
      break;
    case bs_channel_bandwidth_to_MHz(bs_channel_bandwidth_fr1::MHz60):
      bw = bs_channel_bandwidth_fr1::MHz60;
      break;
    case bs_channel_bandwidth_to_MHz(bs_channel_bandwidth_fr1::MHz70):
      bw = bs_channel_bandwidth_fr1::MHz70;
      break;
    case bs_channel_bandwidth_to_MHz(bs_channel_bandwidth_fr1::MHz80):
      bw = bs_channel_bandwidth_fr1::MHz80;
      break;
    case bs_channel_bandwidth_to_MHz(bs_channel_bandwidth_fr1::MHz90):
      bw = bs_channel_bandwidth_fr1::MHz90;
      break;
    case bs_channel_bandwidth_to_MHz(bs_channel_bandwidth_fr1::MHz100):
      bw = bs_channel_bandwidth_fr1::MHz100;
      break;
    default:
      return false;
  }
  return true;
}

static void parse_args(int argc, char** argv)
{
  int  opt         = 0;
  bool invalid_arg = false;
  while ((opt = getopt(argc, argv, "R:T:F:B:C:N:sh")) != -1) {
    switch (opt) {
      case 'R':
        nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'T':
        method = std::string(optarg);
        break;
      case 'F':
        impl_type = std::string(optarg);
        break;
      case 'B':
        if (optarg != nullptr) {
          if (!validate_bw(std::strtol(optarg, nullptr, 10))) {
            fmt::print("Invalid bandwidth\n");
            invalid_arg = true;
          }
        }
        break;
      case 'C':
        if (optarg != nullptr) {
          scs = to_subcarrier_spacing(std::string(optarg));
          if (scs == subcarrier_spacing::invalid) {
            fmt::print("Invalid subcarrier spacing\n");
            invalid_arg = true;
          }
        }
        break;
      case 'N':
        nof_ports = std::strtol(optarg, nullptr, 10);
        if ((nof_ports < 1) || (nof_ports > 4)) {
          fmt::print("Invalid number of ports\n");
          invalid_arg = true;
        }
        break;
      case 's':
        silent = (!silent);
        break;
      case 'h':
      default:
        usage(argv[0]);
        exit(0);
    }
    if (invalid_arg) {
      usage(argv[0]);
      exit(0);
    }
  }
}

int main(int argc, char** argv)
{
  parse_args(argc, argv);

  std::uniform_real_distribution<float> dist(-1.0, +1.0);

  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST", false);
  logger.set_level(srslog::basic_levels::none);

  std::size_t nof_prbs           = get_max_Nprb(bs_channel_bandwidth_to_MHz(bw), scs, frequency_range::FR1);
  double      symbol_duration_us = 1e3 / (get_nsymb_per_slot(cyclic_prefix::NORMAL) * get_nof_slots_per_subframe(scs));

  std::unique_ptr<ofh::iq_compressor> compressor =
      create_iq_compressor(ofh::to_compression_type(method), logger, 0.27, impl_type);
  srsran_assert(compressor != nullptr, "Failed to create OFH compressor");

  std::unique_ptr<ofh::iq_decompressor> decompressor =
      create_iq_decompressor(ofh::to_compression_type(method), logger, impl_type);
  srsran_assert(decompressor != nullptr, "Failed to create OFH decompressor");

  fmt::memory_buffer meas_name;
  fmt::format_to(meas_name,
                 "OFH compression: Method={}, Symbol duration is {:>6.3f} us, Implementation is {}, PRBs = {}, Number "
                 "of ports = {}",
                 method,
                 symbol_duration_us,
                 impl_type,
                 nof_prbs,
                 nof_ports);
  benchmarker perf_meas(to_string(meas_name), nof_repetitions);

  // Test for the most common bit width.
  for (unsigned bit_width : {8, 9, 10, 12, 14, 16}) {
    ofh::ru_compression_params params;
    params.type       = ofh::to_compression_type(method);
    params.data_width = bit_width;

    // Measurement description.
    std::string common_meas_name         = to_string(params.type) + "-" + std::to_string(bit_width) + "b";
    std::string meas_descr_compression   = common_meas_name + " compression";
    std::string meas_descr_decompression = common_meas_name + " decompression";

    std::vector<std::vector<cf_t>>                test_data(nof_ports);
    std::vector<std::vector<cf_t>>                decompressed_data(nof_ports);
    std::vector<std::vector<ofh::compressed_prb>> compressed_data(nof_ports);
    for (unsigned i = 0; i != nof_ports; ++i) {
      test_data[i].resize(nof_prbs * NOF_SUBCARRIERS_PER_RB);
      decompressed_data[i].resize(nof_prbs * NOF_SUBCARRIERS_PER_RB);
      compressed_data[i].resize(nof_prbs);
    }

    // Generate input random data.
    for (unsigned i = 0; i != nof_ports; ++i) {
      std::generate(test_data[i].begin(), test_data[i].end(), [&]() { return cf_t{dist(rgen), dist(rgen)}; });
    }

    // Measure performance.
    perf_meas.new_measure(meas_descr_compression, nof_ports * nof_prbs * NOF_SUBCARRIERS_PER_RB * 2, [&]() {
      for (unsigned i = 0; i != nof_ports; ++i) {
        compressor->compress(compressed_data[i], test_data[i], params);
      }
    });
    perf_meas.new_measure(meas_descr_decompression, nof_ports * nof_prbs * NOF_SUBCARRIERS_PER_RB * 2, [&]() {
      for (unsigned i = 0; i != nof_ports; ++i) {
        decompressor->decompress(decompressed_data[i], compressed_data[i], params);
      }
    });
  }

  if (!silent) {
    perf_meas.print_percentiles_time("microseconds", 1e-3);
    perf_meas.print_percentiles_throughput("samples");
  }
}
