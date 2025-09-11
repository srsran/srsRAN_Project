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

#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_configuration.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_factory.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_result.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/srs/srs_resource_formatter.h"
#include "srsran/support/benchmark_utils.h"
#include "srsran/support/math/complex_normal_random.h"
#include "srsran/support/srsran_test.h"
#include <getopt.h>
#include <random>

using namespace srsran;

static unsigned nof_repetitions = 1000;
static unsigned nof_rb          = MAX_NOF_PRBS;
static bool     silent          = false;

struct channel_topology {
  unsigned nof_tx_ports;
  unsigned nof_rx_ports;
};

static std::vector<channel_topology> channel_topology_list{{1, 1}, {2, 2}, {3, 3}, {4, 4}};

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-n number of RB] [-R repetitions] [-s silent]\n", prog);
  fmt::print("\t-n Number of resource blocks [Default {}]\n", nof_rb);
  fmt::print("\t-R Repetitions [Default {}]\n", nof_repetitions);
  fmt::print("\t-s Toggle silent operation [Default {}]\n", silent);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "n:R:sh")) != -1) {
    switch (opt) {
      case 'n':
        nof_rb = std::strtol(optarg, nullptr, 10);
        break;
      case 'R':
        nof_repetitions = std::strtol(optarg, nullptr, 10);
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

// Creates a resource grid.
static std::unique_ptr<resource_grid> create_resource_grid(unsigned nof_ports, unsigned nof_symbols, unsigned nof_subc)
{
  std::shared_ptr<channel_precoder_factory> precoding_factory = create_channel_precoder_factory("auto");
  TESTASSERT(precoding_factory != nullptr, "Invalid channel precoder factory.");
  std::shared_ptr<resource_grid_factory> rg_factory = create_resource_grid_factory();
  TESTASSERT(rg_factory != nullptr, "Invalid resource grid factory.");

  return rg_factory->create(nof_ports, nof_symbols, nof_subc);
}

int main(int argc, char** argv)
{
  std::mt19937 rgen(0);

  parse_args(argc, argv);

  std::shared_ptr<low_papr_sequence_generator_factory> low_papr_seq_gen_factory =
      create_low_papr_sequence_generator_sw_factory();
  TESTASSERT(low_papr_seq_gen_factory);

  std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_slow();
  if (!dft_factory) {
    dft_factory = create_dft_processor_factory_generic();
  }
  TESTASSERT(dft_factory);

  std::shared_ptr<time_alignment_estimator_factory> ta_est_factory =
      create_time_alignment_estimator_dft_factory(dft_factory);
  TESTASSERT(low_papr_seq_gen_factory);

  std::shared_ptr<srs_estimator_factory> srs_est_factory =
      create_srs_estimator_generic_factory(low_papr_seq_gen_factory, ta_est_factory, MAX_RB);

  std::unique_ptr<srs_estimator> estimator = srs_est_factory->create();
  TESTASSERT(estimator);

  // Precoding weight distribution.
  std::uniform_real_distribution<float> weight_dist(-1.0F, 1.0F);

  benchmarker perf_meas("SRS channel estimator", nof_repetitions);

  // Grid dimensions for all test cases.
  unsigned grid_nof_symbols = get_nsymb_per_slot(cyclic_prefix::NORMAL);
  unsigned grid_nof_subcs   = MAX_RB * NRE;

  unsigned           nof_rx_ports    = 4;
  unsigned           nof_tx_ports    = 4;
  unsigned           nof_symbols     = 4;
  unsigned           start_symbol    = 0;
  unsigned           bw_config_index = 63;
  unsigned           sequence_id     = 0;
  unsigned           bw_index        = 0;
  subcarrier_spacing scs             = subcarrier_spacing::kHz30;

  // Create resource grid.
  std::unique_ptr<resource_grid> grid = create_resource_grid(nof_rx_ports, grid_nof_symbols, grid_nof_subcs);
  TESTASSERT(grid);

  unsigned nof_grid_re = grid_nof_subcs * grid_nof_symbols * nof_rx_ports;

  // Create a vector to hold the randomly generated RE.
  std::vector<cf_t> random_re(nof_grid_re);

  // Standard complex normal distribution with zero mean.
  complex_normal_distribution<cf_t> c_normal_dist = {};

  // Generate random RE.
  std::generate(random_re.begin(), random_re.end(), [&rgen, &c_normal_dist]() { return c_normal_dist(rgen); });

  // Generate a RE mask and set all elements to true.
  bounded_bitset<NRE * MAX_RB> re_mask = ~bounded_bitset<NRE * MAX_RB>(grid_nof_subcs);

  // Fill the grid with the random RE.
  span<const cf_t> re_view(random_re);
  for (unsigned i_rx_port = 0; i_rx_port != nof_rx_ports; ++i_rx_port) {
    for (unsigned i_symbol = 0; i_symbol != grid_nof_symbols; ++i_symbol) {
      re_view = grid->get_writer().put(i_rx_port, i_symbol, 0, re_mask, re_view);
    }
  }

  srs_resource_configuration srs_resource;
  srs_resource.nof_antenna_ports   = static_cast<srs_resource_configuration::one_two_four_enum>(nof_tx_ports);
  srs_resource.nof_symbols         = static_cast<srs_resource_configuration::one_two_four_enum>(nof_symbols);
  srs_resource.start_symbol        = start_symbol;
  srs_resource.configuration_index = bw_config_index;
  srs_resource.sequence_id         = sequence_id;
  srs_resource.bandwidth_index     = bw_index;
  srs_resource.comb_size           = srs_resource_configuration::comb_size_enum::four;
  srs_resource.comb_offset         = 0;
  srs_resource.cyclic_shift        = 0;
  srs_resource.freq_position       = 0;
  srs_resource.freq_shift          = 0;
  srs_resource.freq_hopping        = 0;
  srs_resource.hopping             = srs_resource_configuration::group_or_sequence_hopping_enum::neither;
  srs_resource.periodicity         = srs_resource_configuration::periodicity_and_offset{0, 0};

  srs_estimator_configuration srs_config;
  srs_config.slot = slot_point(to_numerology_value(scs), 0);
  srs_config.ports.resize(nof_rx_ports);
  srs_config.resource = srs_resource;
  std::iota(srs_config.ports.begin(), srs_config.ports.end(), 0);

  fmt::memory_buffer str_buffer;
  fmt::format_to(std::back_inserter(str_buffer), "{}", srs_resource);

  std::string meas_descr = to_string(str_buffer);

  perf_meas.new_measure(
      meas_descr, 1, [&estimator, &grid, &srs_config]() { estimator->estimate(grid->get_reader(), srs_config); });

  if (!silent) {
    perf_meas.print_percentiles_time("microseconds", 1e-3);
  }

  return 0;
}
