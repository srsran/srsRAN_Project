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

#include "srsran/phy/constants.h"
#include "srsran/phy/generic_functions/precoding/precoding_factories.h"
#include "srsran/support/benchmark_utils.h"
#include "srsran/support/srsran_test.h"
#include <getopt.h>
#include <random>

using namespace srsran;

struct channel_topology {
  unsigned nof_ports;
  unsigned nof_layers;
};

// Random generator.
static std::mt19937 rgen(0);

static std::string precoder_type   = "auto";
static unsigned    nof_repetitions = 10000;
static unsigned    nof_rb          = 106;
static bool        silent          = false;

static std::vector<channel_topology> channel_topology_list{{1, 1}, {2, 1}, {2, 2}, {4, 1}, {4, 2}, {4, 4}};

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-R repetitions] [-T precoder type] [-s silent]\n", prog);
  fmt::print("\t-R Repetitions [Default {}]\n", nof_repetitions);
  fmt::print("\t-n Number of RB [Default {}]\n", nof_rb);
  fmt::print("\t-T Precoder type (generic, auto, avx2, avx512) [Default {}]\n", precoder_type);
  fmt::print("\t-s Toggle silent operation [Default {}]\n", silent);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:n:T:sh")) != -1) {
    switch (opt) {
      case 'R':
        nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'n':
        nof_rb = std::strtol(optarg, nullptr, 10);
        break;
      case 'T':
        precoder_type = std::string(optarg);
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

  std::shared_ptr<channel_precoder_factory> precoder_factory = create_channel_precoder_factory(precoder_type);
  TESTASSERT(precoder_factory);

  std::unique_ptr<channel_precoder> precoder = precoder_factory->create();
  TESTASSERT(precoder);

  // Symbol distribution.
  std::uniform_real_distribution<float> symbol_dist(-2.0F, 2.0F);

  // Precoding weight distribution.
  std::uniform_real_distribution<float> weight_dist(-1.0F, 1.0F);

  benchmarker perf_meas("Channel precoder", nof_repetitions);

  unsigned nof_re = nof_rb * NRE;

  for (auto topology : channel_topology_list) {
    // Get dimensions.
    unsigned nof_ports  = topology.nof_ports;
    unsigned nof_layers = topology.nof_layers;

    // Create input and output RE buffers.
    dynamic_re_buffer input_re(nof_layers, nof_re);
    dynamic_re_buffer precoded_re(nof_ports, nof_re);

    precoding_weight_matrix weights(nof_layers, nof_ports);

    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      // Generate precoding weights.
      span<cf_t> port_weights = weights.get_port_coefficients(i_port);
      std::generate(port_weights.begin(), port_weights.end(), [&weight_dist]() {
        return cf_t(weight_dist(rgen), weight_dist(rgen));
      });
    }

    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      // Generate input RE.
      span<cf_t> symbols = input_re.get_slice(i_layer);
      std::generate(
          symbols.begin(), symbols.end(), [&symbol_dist]() { return cf_t(symbol_dist(rgen), symbol_dist(rgen)); });
    }

    // Measurement description.
    std::string meas_descr = fmt::to_string(nof_ports) + " ports x " + fmt::to_string(nof_layers) + " layers";

    // Apply precoding.
    perf_meas.new_measure(meas_descr, nof_re, [&precoded_re, &input_re, &weights, &precoder]() {
      precoder->apply_precoding(precoded_re, input_re, weights);
    });
  }

  if (!silent) {
    perf_meas.print_percentiles_throughput("RE");
  }

  return 0;
}
