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

#include "../../../tests/unittests/phy/support/resource_grid_mapper_test_doubles.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/signal_processors/dmrs_pdsch_processor.h"
#include "srsran/phy/upper/signal_processors/signal_processor_factories.h"
#include "srsran/support/benchmark_utils.h"
#include "srsran/support/srsran_test.h"
#include <getopt.h>
#include <random>

using namespace srsran;

// Random generator.
static std::mt19937 rgen(0);

static unsigned nof_repetitions  = 1000;
static unsigned nof_rb           = 106;
static bool     silent           = false;
static bool     use_dummy_mapper = false;

struct channel_topology {
  unsigned nof_ports;
  unsigned nof_layers;
};

static std::vector<channel_topology> channel_topology_list{{1, 1}, {2, 2}, {3, 3}, {4, 4}};

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-P precoder type] [-n number of RB] [-R repetitions] [-s silent]\n", prog);
  fmt::print("\t-n Number of resource blocks [Default {}]\n", nof_rb);
  fmt::print("\t-D Use dummy resource grid mapper\n");
  fmt::print("\t-R Repetitions [Default {}]\n", nof_repetitions);
  fmt::print("\t-s Toggle silent operation [Default {}]\n", silent);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "n:DR:sh")) != -1) {
    switch (opt) {
      case 'n':
        nof_rb = std::strtol(optarg, nullptr, 10);
        break;
      case 'D':
        use_dummy_mapper = true;
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
        exit(0);
    }
  }
}

// Creates a resource grid.
static std::unique_ptr<resource_grid> create_resource_grid(unsigned nof_ports, unsigned nof_symbols, unsigned nof_subc)
{
  std::shared_ptr<channel_precoder_factory> precoding_factory = create_channel_precoder_factory("auto");
  TESTASSERT(precoding_factory != nullptr, "Invalid channel precoder factory.");
  std::shared_ptr<resource_grid_factory> rg_factory = create_resource_grid_factory(precoding_factory);
  TESTASSERT(rg_factory != nullptr, "Invalid resource grid factory.");

  return rg_factory->create(nof_ports, nof_symbols, nof_subc);
}

int main(int argc, char** argv)
{
  parse_args(argc, argv);

  // Create pseudo-random sequence generator.
  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_pdsch_proc_factory =
      create_dmrs_pdsch_processor_factory_sw(prg_factory);
  TESTASSERT(dmrs_pdsch_proc_factory);

  std::unique_ptr<dmrs_pdsch_processor> dmrs_proc = dmrs_pdsch_proc_factory->create();
  TESTASSERT(dmrs_proc);

  // Precoding weight distribution.
  std::uniform_real_distribution<float> weight_dist(-1.0F, 1.0F);

  resource_grid_mapper_dummy dummy_mapper;

  benchmarker perf_meas("DM-RS PDSCH processor", nof_repetitions);

  for (auto topology : channel_topology_list) {
    std::unique_ptr<resource_grid> grid = create_resource_grid(topology.nof_ports, MAX_NSYMB_PER_SLOT, MAX_RB * NRE);
    TESTASSERT(grid);

    resource_grid_mapper& mapper = use_dummy_mapper ? dummy_mapper : grid->get_mapper();

    // Generate precoding weights.
    precoding_weight_matrix weights(topology.nof_layers, topology.nof_ports);
    for (unsigned i_port = 0; i_port != topology.nof_ports; ++i_port) {
      span<cf_t> port_weights = weights.get_port_coefficients(i_port);
      std::generate(port_weights.begin(), port_weights.end(), [&weight_dist]() {
        return cf_t(weight_dist(rgen), weight_dist(rgen));
      });
    }

    // Generate DM-RS PDSCH configuration.
    dmrs_pdsch_processor::config_t dmrs_config;
    dmrs_config.slot                 = slot_point(subcarrier_spacing::kHz15, 0);
    dmrs_config.reference_point_k_rb = 0;
    dmrs_config.scrambling_id        = 0;
    dmrs_config.n_scid               = false;
    dmrs_config.amplitude            = 1.0f;
    dmrs_config.symbols_mask         = symbol_slot_mask(
        {false, false, true, false, false, false, false, false, false, false, false, true, false, false});
    dmrs_config.rb_mask   = ~bounded_bitset<MAX_RB>(nof_rb);
    dmrs_config.precoding = precoding_configuration::make_wideband(weights);

    // Test DM-RS Type 1 generation.
    {
      dmrs_config.type          = dmrs_type::options::TYPE1;
      unsigned nof_dmrs_symbols = dmrs_config.rb_mask.count() * dmrs_config.symbols_mask.count() *
                                  dmrs_config.type.nof_dmrs_per_rb() * topology.nof_layers;

      std::string meas_descr = fmt::to_string(topology.nof_ports) + " ports x " + fmt::to_string(topology.nof_layers) +
                               " layers, DM-RS Type 1 ";

      perf_meas.new_measure(
          meas_descr, nof_dmrs_symbols, [&dmrs_proc, &mapper, &dmrs_config]() { dmrs_proc->map(mapper, dmrs_config); });
    }

    // Test DM-RS Type 2 generation.
    {
      dmrs_config.type          = dmrs_type::options::TYPE2;
      unsigned nof_dmrs_symbols = dmrs_config.rb_mask.count() * dmrs_config.symbols_mask.count() *
                                  dmrs_config.type.nof_dmrs_per_rb() * topology.nof_layers;

      std::string meas_descr = fmt::to_string(topology.nof_ports) + " ports x " + fmt::to_string(topology.nof_layers) +
                               " layers, DM-RS Type 2 ";

      perf_meas.new_measure(
          meas_descr, nof_dmrs_symbols, [&dmrs_proc, &mapper, &dmrs_config]() { dmrs_proc->map(mapper, dmrs_config); });
    }
  }

  if (!silent) {
    perf_meas.print_percentiles_throughput("symbols");
  }

  return 0;
}
