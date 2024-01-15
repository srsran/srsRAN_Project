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

#include "srsran/phy/upper/equalization/equalization_factories.h"
#include "srsran/support/benchmark_utils.h"
#include "srsran/support/srsran_test.h"
#include <getopt.h>
#include <random>

using namespace srsran;

// Equalizer data dimensions.
using re_dims = channel_equalizer::re_list::dims;
using ch_dims = channel_equalizer::ch_est_list::dims;

// Random generator.
static std::mt19937 rgen(0);

static unsigned nof_repetitions   = 1000;
static unsigned nof_prb           = 106;
static unsigned max_simo_rx_ports = 4;
static bool     silent            = false;

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-R repetitions] [-s silent]\n", prog);
  fmt::print("\t-R Repetitions [Default {}]\n", nof_repetitions);
  fmt::print("\t-s Toggle silent operation [Default {}]\n", silent);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:sh")) != -1) {
    switch (opt) {
      case 'R':
        nof_repetitions = std::strtol(optarg, nullptr, 10);
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

  std::shared_ptr<channel_equalizer_factory> equalizer_factory = create_channel_equalizer_factory_zf();
  TESTASSERT(equalizer_factory);

  std::unique_ptr<channel_equalizer> equalizer = equalizer_factory->create();
  TESTASSERT(equalizer);

  // Channel estimate distributions.
  std::uniform_real_distribution<float> ch_mag_dist(0.1F, 1.0F);
  std::uniform_real_distribution<float> ch_phase_dist(0.0F, TWOPI);

  // Symbol distribution.
  std::uniform_real_distribution<float> symbol_dist(-1.0F, 1.0F);

  benchmarker perf_meas("Channel Equalizer", nof_repetitions);

  // Test the supported channel topologies.
  std::vector<std::pair<unsigned, unsigned>> channel_topologies;

  // 1xN channels: single transmit layer and one or multiple receive ports.
  for (unsigned i_rx_port = 1; i_rx_port <= max_simo_rx_ports; ++i_rx_port) {
    channel_topologies.emplace_back(std::make_pair(i_rx_port, 1));
  }
  // MIMO 2x2.
  channel_topologies.emplace_back(std::make_pair(2, 2));

  for (auto topology : channel_topologies) {
    // Get dimensions.
    unsigned nof_rx_ports     = topology.first;
    unsigned nof_tx_layers    = topology.second;
    unsigned nof_ofdm_symbols = MAX_NSYMB_PER_SLOT;
    unsigned nof_subcarriers  = nof_prb * NRE;

    // Create input and output data tensors.
    dynamic_tensor<std::underlying_type_t<re_dims>(re_dims::nof_dims), cf_t, re_dims> rx_symbols(
        {nof_subcarriers * nof_ofdm_symbols, nof_rx_ports});
    dynamic_tensor<std::underlying_type_t<re_dims>(re_dims::nof_dims), cf_t, re_dims> eq_symbols(
        {nof_subcarriers * nof_ofdm_symbols, nof_tx_layers});
    dynamic_tensor<std::underlying_type_t<re_dims>(re_dims::nof_dims), float, re_dims> eq_noise_vars(
        {nof_subcarriers * nof_ofdm_symbols, nof_tx_layers});

    // Create channel estimates tensor.
    dynamic_tensor<std::underlying_type_t<ch_dims>(ch_dims::nof_dims), cf_t, ch_dims> channel_ests(
        {nof_subcarriers * nof_ofdm_symbols, nof_rx_ports, nof_tx_layers});

    for (unsigned i_rx_port = 0; i_rx_port != nof_rx_ports; ++i_rx_port) {
      // Generate Rx symbols.
      span<cf_t> symbols = rx_symbols.get_view<>({i_rx_port});
      std::generate(
          symbols.begin(), symbols.end(), [&symbol_dist]() { return cf_t(symbol_dist(rgen), symbol_dist(rgen)); });

      for (unsigned i_tx_layer = 0; i_tx_layer != nof_tx_layers; ++i_tx_layer) {
        // Generate estimates.
        span<cf_t> ests = channel_ests.get_view<>({i_rx_port, i_tx_layer});
        std::generate(ests.begin(), ests.end(), [&ch_mag_dist, &ch_phase_dist]() {
          return std::polar(ch_mag_dist(rgen), ch_phase_dist(rgen));
        });
      }
    }

    // Set the port noise variances.
    std::vector<float> noise_var_ests(nof_rx_ports, 0.1F);

    // Number of equalized resource elements.
    unsigned nof_processed_re = nof_subcarriers * nof_ofdm_symbols * nof_tx_layers;

    // Measurement description.
    std::string meas_descr = "ZF " + fmt::to_string(nof_tx_layers) + "x" + fmt::to_string(nof_rx_ports);

    // Equalize.
    perf_meas.new_measure(meas_descr,
                          nof_processed_re,
                          [&eq_symbols, &eq_noise_vars, &rx_symbols, &channel_ests, &noise_var_ests, &equalizer]() {
                            equalizer->equalize(
                                eq_symbols, eq_noise_vars, rx_symbols, channel_ests, noise_var_ests, 1.0F);
                          });
  }

  if (!silent) {
    perf_meas.print_percentiles_throughput("RE");
  }

  return 0;
}
