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

#include "srsran/phy/constants.h"
#include "srsran/phy/generic_functions/precoding/precoding_factories.h"
#include "srsran/phy/support/re_pattern.h"
#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/precoding/precoding_codebooks.h"
#include "srsran/srsvec/zero.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

// Pseudo-random number generator.
static std::mt19937 rgen;

using MultiplePRGParams = std::tuple<
    // Number of RB.
    unsigned,
    // Number of antenna ports.
    unsigned,
    // PRG size in number of RB.
    unsigned>;

namespace srsran {

static float ASSERT_MAX_ERROR = 1e-5;

static std::ostream& operator<<(std::ostream& os, span<const cf_t> data)
{
  fmt::print(os, "{}", data);
  return os;
}

static bool operator==(span<const cf_t> lhs, span<const cf_t> rhs)
{
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](cf_t lhs_val, cf_t rhs_val) {
    return (std::abs(lhs_val - rhs_val) < ASSERT_MAX_ERROR);
  });
}

} // namespace srsran

// Asserts that the contents of the resource grid match the golden symbols for the give allocation pattern.
static void assert_grid(const re_buffer_reader&     golden,
                        const re_pattern_list&      allocation,
                        const re_pattern_list&      reserved,
                        const resource_grid_reader& grid,
                        unsigned                    nof_grid_rb)
{
  unsigned nof_ports = golden.get_nof_slices();
  for (unsigned i_symbol = 0, re_offset = 0; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
    // RE mask for an OFDM symbol.
    bounded_bitset<MAX_RB * NRE> re_mask(nof_grid_rb * NRE);
    allocation.get_inclusion_mask(re_mask, i_symbol);
    reserved.get_exclusion_mask(re_mask, i_symbol);

    // View of the golden data for an OFDM symbol.
    unsigned              re_count = re_mask.count();
    re_buffer_reader_view golden_symbol(golden, re_offset, re_count);

    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      // Get the precoded and mapped symbols from the grid.
      std::vector<cf_t> grid_symbols(re_count);
      grid.get(grid_symbols, i_port, i_symbol, 0, re_mask);

      // Assert symbols.
      ASSERT_EQ(span<const cf_t>(golden_symbol.get_slice(i_port)), span<const cf_t>(grid_symbols));

      // Get the rest of the symbols from the grid.
      std::vector<cf_t> grid_zeros(re_mask.size() - re_count);

      // Golden sequence with all its elements set to zero.
      std::vector<cf_t> golden_zeros(re_mask.size() - re_count);
      srsvec::zero(golden_zeros);

      grid.get(grid_zeros, i_port, i_symbol, 0, ~re_mask);

      // Assert that all the grid locations not included in the grid allocation are set to zero.
      ASSERT_EQ(span<const cf_t>(golden_zeros), span<const cf_t>(grid_zeros));
    }

    // Advance views.
    re_offset += re_count;
  }
}

static void assert_grid(const re_buffer_reader&     golden,
                        const re_pattern_list&      allocation,
                        const resource_grid_reader& grid,
                        unsigned                    nof_grid_rb)
{
  assert_grid(golden, allocation, re_pattern_list(), grid, nof_grid_rb);
}

namespace {
class ResourceGridMapperFixture : public ::testing::TestWithParam<MultiplePRGParams>
{
protected:
  static std::shared_ptr<channel_precoder_factory> precoder_factory;
  static std::shared_ptr<resource_grid_factory>    rg_factory;

  static void SetUpTestSuite()
  {
    // Create channel precoder factory.
    if (!precoder_factory) {
      precoder_factory = create_channel_precoder_factory("auto");
      ASSERT_NE(precoder_factory, nullptr) << "Cannot create channel precoder factory";
    }

    // Create resource grid factory.
    if (!rg_factory) {
      rg_factory = create_resource_grid_factory(precoder_factory);
      ASSERT_NE(rg_factory, nullptr) << "Cannot create channel precoder factory";
    }
  }

  void SetUp() override
  {
    ASSERT_NE(precoder_factory, nullptr) << "Cannot create channel precoder factory";
    ASSERT_NE(rg_factory, nullptr) << "Cannot create resource grid factory";
  }

  // Generates and returns random RE values, as many as the specified number of layers and RE.
  const re_buffer_reader& generate_random_data(unsigned nof_layers, unsigned nof_re)
  {
    // Resize buffer.
    random_data.resize(nof_layers, nof_re);

    // Fill buffer with random values.
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      for (cf_t& val : random_data.get_slice(i_layer)) {
        val = {data_dist(rgen), data_dist(rgen)};
      }
    }

    return random_data;
  }

  // Generates and returns a random grid allocation, for a number of grid RB and the maximum number of OFDM symbols.
  static re_pattern_list generate_random_allocation(unsigned nof_rb_grid)
  {
    interval<unsigned, true> rb_range(1, MAX_RB);

    srsran_assert(
        rb_range.contains(nof_rb_grid), "The number of RBs (i.e., {}) is out of range {}.", nof_rb_grid, rb_range);

    std::uniform_int_distribution<unsigned> bool_dist(0, 1);
    std::uniform_int_distribution<unsigned> nof_alloc_rb_dist(1, nof_rb_grid);

    // Some RE.
    re_prb_mask re_mask;
    re_mask.set(0);
    for (unsigned i_re = 1; i_re != NRE; ++i_re) {
      if (static_cast<bool>(bool_dist(rgen))) {
        re_mask.set(i_re);
      }
    }

    // Some symbols.
    symbol_slot_mask symbol_mask = symbol_slot_mask(MAX_NSYMB_PER_SLOT);
    symbol_mask.set(0);
    for (unsigned i_symb = 1; i_symb != MAX_NSYMB_PER_SLOT; ++i_symb) {
      if (static_cast<bool>(bool_dist(rgen))) {
        symbol_mask.set(i_symb);
      }
    }

    // Number of contiguous RB allocated for data.
    unsigned nof_alloc_rb = nof_alloc_rb_dist(rgen);

    // Starting RB.
    std::uniform_int_distribution<unsigned> rb_begin_dist(0, nof_rb_grid - nof_alloc_rb);
    unsigned                                rb_begin = rb_begin_dist(rgen);

    re_pattern      allocation(rb_begin, rb_begin + nof_alloc_rb, 1, re_mask, symbol_mask);
    re_pattern_list allocation_list(allocation);
    return allocation_list;
  }

  // Generates the golden RE sequence parting from the input symbols and applying precoding based on the provided
  // precoding configuration and allocation and reserved RE patterns.
  const re_buffer_reader& generate_golden(const re_buffer_reader&        input,
                                          const re_pattern_list&         allocation,
                                          const precoding_configuration& configuration,
                                          const re_pattern_list&         reserved)
  {
    // Get dimensions.
    unsigned nof_layers = configuration.get_nof_layers();
    unsigned nof_ports  = configuration.get_nof_ports();

    // RE pattern containing both the allocation and the reserved RE patterns.
    re_pattern_list both;
    both.merge(allocation);
    both.merge(reserved);

    // Get the number of RE in the allocation pattern.
    unsigned nof_data_re = allocation.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, ~bounded_bitset<MAX_RB>(MAX_RB));

    // Get the number of RE in the reserved pattern.
    unsigned nof_reserved_re = reserved.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, ~bounded_bitset<MAX_RB>(MAX_RB));

    // Number of REs in the data pattern which are also reserved.
    unsigned nof_overlapping_re = nof_data_re + nof_reserved_re -
                                  both.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, ~bounded_bitset<MAX_RB>(MAX_RB));

    // Subtract the reserved REs overlapping with data from the total number of data RE.
    nof_data_re -= nof_overlapping_re;

    EXPECT_EQ(nof_data_re, input.get_nof_re())
        << "Input number of RE does not match the number of RE allocations in the allocation pattern";

    EXPECT_EQ(nof_layers, input.get_nof_slices()) << "Input and precoding matrix number of layers don't match.";

    // Resize buffer.
    golden_data.resize(nof_ports, nof_data_re);

    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      srsvec::zero(golden_data.get_slice(i_port));
    }

    unsigned i_re_buffer = 0;
    for (unsigned i_symbol = 0; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
      bounded_bitset<MAX_RB * NRE> re_pattern_symbol(MAX_RB * NRE);
      allocation.get_inclusion_mask(re_pattern_symbol, i_symbol);
      reserved.get_exclusion_mask(re_pattern_symbol, i_symbol);

      re_pattern_symbol.for_each(0, re_pattern_symbol.size(), [&](unsigned i_re) {
        // PRG of each RE.
        unsigned prg_id = i_re / (configuration.get_prg_size() * NRE);

        for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
          for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
            // Precode one RE from the input for each RE in the allocation pattern.
            golden_data.get_slice(i_port)[i_re_buffer] +=
                configuration.get_coefficient(i_layer, i_port, prg_id) * input.get_slice(i_layer)[i_re_buffer];
          }
        }

        // Advance buffers.
        ++i_re_buffer;
      });
    }

    EXPECT_EQ(nof_data_re, i_re_buffer)
        << "The number of RE of the golden sequence does not match the number of input RE.";

    return golden_data;
  }

  // Generates the golden RE sequence parting from the input symbols and applying precoding based on the provided
  // precoding configuration and allocation patterns.
  const re_buffer_reader& generate_golden(const re_buffer_reader&        input,
                                          const re_pattern_list&         allocation,
                                          const precoding_configuration& configuration)
  {
    return generate_golden(input, allocation, configuration, re_pattern_list());
  }

  // Generates a precoding configuration with the specified dimensions and random precoding coefficients.
  precoding_configuration
  generate_random_precoding(unsigned nof_layers, unsigned nof_ports, unsigned nof_prg, unsigned prg_size)
  {
    // Create configuration.
    precoding_configuration config(nof_layers, nof_ports, nof_prg, prg_size);

    // Fill the precoding matrices with random values.
    for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
      precoding_weight_matrix prg_precoding_matrix(nof_layers, nof_ports);

      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
          cf_t weight = {weight_dist(rgen), weight_dist(rgen)};
          prg_precoding_matrix.set_coefficient(weight, i_layer, i_port);
        }
      }

      config.set_prg_coefficients(prg_precoding_matrix, i_prg);
    }

    return config;
  }

private:
  // Buffer for holding randomly generated data.
  static_re_buffer<precoding_constants::MAX_NOF_LAYERS, NRE * MAX_RB * MAX_NSYMB_PER_SLOT> random_data;

  // Buffer for holding the golden sequence.
  static_re_buffer<precoding_constants::MAX_NOF_PORTS, NRE * MAX_RB * MAX_NSYMB_PER_SLOT> golden_data;

  // Uniform distribution to generate data samples.
  std::uniform_real_distribution<float> data_dist{-10.0, +10.0};

  // Uniform distribution to generate precoding coefficients.
  std::uniform_real_distribution<float> weight_dist{-1, +1};
};

std::shared_ptr<channel_precoder_factory> ResourceGridMapperFixture::precoder_factory = nullptr;
std::shared_ptr<resource_grid_factory>    ResourceGridMapperFixture::rg_factory       = nullptr;

// Tests mapping for a single port precoding configuration, generated by the function make_single_port.
TEST_F(ResourceGridMapperFixture, SinglePort)
{
  // Create resource grid.
  std::unique_ptr<resource_grid> grid = rg_factory->create(1, MAX_NSYMB_PER_SLOT, MAX_RB * NRE);
  grid->set_all_zero();

  constexpr unsigned nof_layers = 1;

  // Create a single port precoding configuration.
  precoding_configuration precoding_config = precoding_configuration::make_wideband(make_single_port());

  // Generate resource grid allocation.
  re_pattern_list allocation = generate_random_allocation(MAX_RB);

  // Get the total number of RE in the allocation pattern.
  unsigned nof_data_re = allocation.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, ~bounded_bitset<MAX_RB>(MAX_RB));

  // Generate random RE arranged by layers.
  const re_buffer_reader& input_data = generate_random_data(nof_layers, nof_data_re);

  // Get the resource grid mapper.
  resource_grid_mapper& mapper = grid->get_mapper();

  // Map into the resource grid.
  mapper.map(input_data, allocation.get_re_patterns().front(), precoding_config);

  // Generate the golden precoded data.
  const re_buffer_reader& golden = generate_golden(input_data, allocation, precoding_config);

  // Assert resource grid contents.
  assert_grid(golden, allocation, grid->get_reader(), MAX_RB);
}

// Tests mapping for a one layer to one antenna port configuration, generated by the function
// make_one_layer_one_port.
TEST_F(ResourceGridMapperFixture, OneLayerToOnePort)
{
  constexpr unsigned nof_layers = 1;

  for (unsigned nof_ports = 1; nof_ports <= precoding_constants::MAX_NOF_PORTS; ++nof_ports) {
    // Create resource grid.
    std::unique_ptr<resource_grid> grid = rg_factory->create(nof_ports, MAX_NSYMB_PER_SLOT, MAX_RB * NRE);
    grid->set_all_zero();

    // The precoding configuration maps a single layer to one of the antenna ports, selected randomly.
    std::uniform_int_distribution<unsigned> selected_port_dist{0, nof_ports - 1};

    // Create a single port precoding configuration.
    precoding_configuration precoding_config =
        precoding_configuration::make_wideband(make_one_layer_one_port(nof_ports, selected_port_dist(rgen)));

    // Generate resource grid allocation.
    re_pattern_list allocation = generate_random_allocation(MAX_RB);

    // Get the total number of RE in the allocation pattern.
    unsigned nof_data_re = allocation.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, ~bounded_bitset<MAX_RB>(MAX_RB));

    // Generate random RE arranged by layers.
    const re_buffer_reader& input_data = generate_random_data(nof_layers, nof_data_re);

    // Get the resource grid mapper.
    resource_grid_mapper& mapper = grid->get_mapper();

    // Map into the resource grid.
    mapper.map(input_data, allocation.get_re_patterns().front(), precoding_config);

    // Generate the golden precoded data.
    const re_buffer_reader& golden = generate_golden(input_data, allocation, precoding_config);

    // Assert resource grid contents.
    assert_grid(golden, allocation, grid->get_reader(), MAX_RB);
  }
}

// Tests mapping for a one layer to multiple antenna ports configuration, generated by the function
// make_one_layer_all_ports.
TEST_F(ResourceGridMapperFixture, OneLayerAllPorts)
{
  constexpr unsigned nof_layers = 1;

  for (unsigned nof_ports = 1; nof_ports <= precoding_constants::MAX_NOF_PORTS; ++nof_ports) {
    // Create resource grid.
    std::unique_ptr<resource_grid> grid = rg_factory->create(nof_ports, MAX_NSYMB_PER_SLOT, MAX_RB * NRE);
    grid->set_all_zero();

    // Create a transmit diversity precoding configuration.
    precoding_configuration precoding_config =
        precoding_configuration::make_wideband(make_one_layer_all_ports(nof_ports));

    // Generate resource grid allocation.
    re_pattern_list allocation = generate_random_allocation(MAX_RB);

    // Get the total number of RE in the allocation pattern.
    unsigned nof_data_re = allocation.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, ~bounded_bitset<MAX_RB>(MAX_RB));

    // Generate random RE arranged by layers.
    const re_buffer_reader& input_data = generate_random_data(nof_layers, nof_data_re);

    // Get the resource grid mapper.
    resource_grid_mapper& mapper = grid->get_mapper();

    // Map into the resource grid.
    mapper.map(input_data, allocation.get_re_patterns().front(), precoding_config);

    // Generate the golden precoded data.
    const re_buffer_reader& golden = generate_golden(input_data, allocation, precoding_config);

    // Assert resource grid contents.
    assert_grid(golden, allocation, grid->get_reader(), MAX_RB);
  }
}

// Tests mapping for an identity matrix precoding configuration, generated by the function make_identity.
TEST_F(ResourceGridMapperFixture, Identity)
{
  for (unsigned nof_streams = 1; nof_streams <= precoding_constants::MAX_NOF_LAYERS; ++nof_streams) {
    // Create resource grid.
    std::unique_ptr<resource_grid> grid = rg_factory->create(nof_streams, MAX_NSYMB_PER_SLOT, MAX_RB * NRE);
    grid->set_all_zero();

    // Create a precoding configuration with precoding coefficients forming an identity matrix.
    precoding_configuration precoding_config = precoding_configuration::make_wideband(make_identity(nof_streams));

    // Generate resource grid allocation.
    re_pattern_list allocation = generate_random_allocation(MAX_RB);

    // Get the total number of RE in the allocation pattern.
    unsigned nof_data_re = allocation.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, ~bounded_bitset<MAX_RB>(MAX_RB));

    // Generate random RE arranged by layers.
    const re_buffer_reader& input_data = generate_random_data(nof_streams, nof_data_re);

    // Get the resource grid mapper.
    resource_grid_mapper& mapper = grid->get_mapper();

    // Map into the resource grid.
    mapper.map(input_data, allocation.get_re_patterns().front(), precoding_config);

    // Generate the golden precoded data.
    const re_buffer_reader& golden = generate_golden(input_data, allocation, precoding_config);

    // Assert resource grid contents.
    assert_grid(golden, allocation, grid->get_reader(), MAX_RB);
  }
}

// Tests mapping for a one layer to two antenna ports configuration, generated by the function
// make_one_layer_two_ports.
TEST_F(ResourceGridMapperFixture, OneLayerTwoPorts)
{
  constexpr unsigned nof_layers = 1;

  // Create resource grid.
  std::unique_ptr<resource_grid> grid = rg_factory->create(2, MAX_NSYMB_PER_SLOT, MAX_RB * NRE);

  for (unsigned codebook_index = 0, nof_cb_index = 4; codebook_index != nof_cb_index; ++codebook_index) {
    // Initialize grid to zero.
    grid->set_all_zero();

    // Create a single layer to two ports precoding configuration.
    precoding_configuration precoding_config =
        precoding_configuration::make_wideband(make_one_layer_two_ports(codebook_index));

    // Generate resource grid allocation.
    re_pattern_list allocation = generate_random_allocation(MAX_RB);

    // Get the total number of RE in the allocation pattern.
    unsigned nof_data_re = allocation.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, ~bounded_bitset<MAX_RB>(MAX_RB));

    // Generate random RE arranged by layers.
    const re_buffer_reader& input_data = generate_random_data(nof_layers, nof_data_re);

    // Get the resource grid mapper.
    resource_grid_mapper& mapper = grid->get_mapper();

    // Map into the resource grid.
    mapper.map(input_data, allocation.get_re_patterns().front(), precoding_config);

    // Generate the golden precoded data.
    const re_buffer_reader& golden = generate_golden(input_data, allocation, precoding_config);

    // Assert resource grid contents.
    assert_grid(golden, allocation, grid->get_reader(), MAX_RB);
  }
}

// Tests mapping for a two layers to two antenna ports configuration, generated by the function
// make_two_layer_two_ports.
TEST_F(ResourceGridMapperFixture, TwoLayerTwoPorts)
{
  constexpr unsigned nof_layers = 2;

  // Create resource grid.
  std::unique_ptr<resource_grid> grid = rg_factory->create(2, MAX_NSYMB_PER_SLOT, MAX_RB * NRE);

  for (unsigned codebook_index = 0, nof_cb_index = 2; codebook_index != nof_cb_index; ++codebook_index) {
    // Initialize grid to zero.
    grid->set_all_zero();

    // Create a single layer to two ports precoding configuration.
    precoding_configuration precoding_config =
        precoding_configuration::make_wideband(make_two_layer_two_ports(codebook_index));

    // Generate resource grid allocation.
    re_pattern_list allocation = generate_random_allocation(MAX_RB);

    // Get the total number of RE in the allocation pattern.
    unsigned nof_data_re = allocation.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, ~bounded_bitset<MAX_RB>(MAX_RB));

    // Generate random RE arranged by layers.
    const re_buffer_reader& input_data = generate_random_data(nof_layers, nof_data_re);

    // Get the resource grid mapper.
    resource_grid_mapper& mapper = grid->get_mapper();

    // Map into the resource grid.
    mapper.map(input_data, allocation.get_re_patterns().front(), precoding_config);

    // Generate the golden precoded data.
    const re_buffer_reader& golden = generate_golden(input_data, allocation, precoding_config);

    // Assert resource grid contents.
    assert_grid(golden, allocation, grid->get_reader(), MAX_RB);
  }
}

// Tests mapping for a two layers to four antenna ports configuration, generated by the function
// make_one_layer_four_ports.
TEST_F(ResourceGridMapperFixture, OneLayerFourPorts)
{
  constexpr unsigned nof_layers = 1;
  constexpr unsigned nof_ports  = 4;

  // Create resource grid.
  std::unique_ptr<resource_grid> grid = rg_factory->create(nof_ports, MAX_NSYMB_PER_SLOT, MAX_RB * NRE);

  for (unsigned i_beam = 0; i_beam != 8; ++i_beam) {
    for (unsigned i_pol_phase = 0; i_pol_phase != 4; ++i_pol_phase) {
      // Initialize grid to zero.
      grid->set_all_zero();

      // Create a single layer to four ports precoding configuration.
      precoding_configuration precoding_config =
          precoding_configuration::make_wideband(make_one_layer_four_ports_type1_sp_mode1(i_beam, i_pol_phase));

      // Generate resource grid allocation.
      re_pattern_list allocation = generate_random_allocation(MAX_RB);

      // Get the total number of RE in the allocation pattern.
      unsigned nof_data_re = allocation.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, ~bounded_bitset<MAX_RB>(MAX_RB));

      // Generate random RE arranged by layers.
      const re_buffer_reader& input_data = generate_random_data(nof_layers, nof_data_re);

      // Get the resource grid mapper.
      resource_grid_mapper& mapper = grid->get_mapper();

      // Map into the resource grid.
      mapper.map(input_data, allocation.get_re_patterns().front(), precoding_config);

      // Generate the golden precoded data.
      const re_buffer_reader& golden = generate_golden(input_data, allocation, precoding_config);

      // Assert resource grid contents.
      assert_grid(golden, allocation, grid->get_reader(), MAX_RB);
    }
  }
}

// Tests mapping for a two layers to four antenna ports configuration, generated by the function
// make_two_layer_four_ports.
TEST_F(ResourceGridMapperFixture, TwoLayerFourPorts)
{
  constexpr unsigned nof_layers = 2;
  constexpr unsigned nof_ports  = 4;

  // Create resource grid.
  std::unique_ptr<resource_grid> grid = rg_factory->create(nof_ports, MAX_NSYMB_PER_SLOT, MAX_RB * NRE);

  for (unsigned i_beam = 0; i_beam != 8; ++i_beam) {
    for (unsigned i_beam_offset = 0; i_beam_offset != 1; ++i_beam_offset) {
      for (unsigned i_pol_phase = 0; i_pol_phase != 2; ++i_pol_phase) {
        // Initialize grid to zero.
        grid->set_all_zero();

        // Create a two layer to four ports precoding configuration.
        precoding_configuration precoding_config = precoding_configuration::make_wideband(
            make_two_layer_four_ports_type1_sp_mode1(i_beam, i_beam_offset, i_pol_phase));

        // Generate resource grid allocation.
        re_pattern_list allocation = generate_random_allocation(MAX_RB);

        // Get the total number of RE in the allocation pattern.
        unsigned nof_data_re = allocation.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, ~bounded_bitset<MAX_RB>(MAX_RB));

        // Generate random RE arranged by layers.
        const re_buffer_reader& input_data = generate_random_data(nof_layers, nof_data_re);

        // Get the resource grid mapper.
        resource_grid_mapper& mapper = grid->get_mapper();

        // Map into the resource grid.
        mapper.map(input_data, allocation.get_re_patterns().front(), precoding_config);

        // Generate the golden precoded data.
        const re_buffer_reader& golden = generate_golden(input_data, allocation, precoding_config);

        // Assert resource grid contents.
        assert_grid(golden, allocation, grid->get_reader(), MAX_RB);
      }
    }
  }
}

// Tests mapping for a three layers to four antenna ports configuration, generated by the function
// make_three_layer_four_ports.
TEST_F(ResourceGridMapperFixture, ThreeLayerFourPorts)
{
  constexpr unsigned nof_layers = 3;
  constexpr unsigned nof_ports  = 4;

  // Create resource grid.
  std::unique_ptr<resource_grid> grid = rg_factory->create(nof_ports, MAX_NSYMB_PER_SLOT, MAX_RB * NRE);

  for (unsigned i_beam = 0; i_beam != 8; ++i_beam) {
    for (unsigned i_pol_phase = 0; i_pol_phase != 2; ++i_pol_phase) {
      // Initialize grid to zero.
      grid->set_all_zero();

      // Create a three layer to four ports precoding configuration.
      precoding_configuration precoding_config =
          precoding_configuration::make_wideband(make_three_layer_four_ports_type1_sp(i_beam, i_pol_phase));

      // Generate resource grid allocation.
      re_pattern_list allocation = generate_random_allocation(MAX_RB);

      // Get the total number of RE in the allocation pattern.
      unsigned nof_data_re = allocation.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, ~bounded_bitset<MAX_RB>(MAX_RB));

      // Generate random RE arranged by layers.
      const re_buffer_reader& input_data = generate_random_data(nof_layers, nof_data_re);

      // Get the resource grid mapper.
      resource_grid_mapper& mapper = grid->get_mapper();

      // Map into the resource grid.
      mapper.map(input_data, allocation.get_re_patterns().front(), precoding_config);

      // Generate the golden precoded data.
      const re_buffer_reader& golden = generate_golden(input_data, allocation, precoding_config);

      // Assert resource grid contents.
      assert_grid(golden, allocation, grid->get_reader(), MAX_RB);
    }
  }
}

// Tests mapping for a four layers to four antenna ports configuration, generated by the function
// make_four_layer_four_ports.
TEST_F(ResourceGridMapperFixture, FourLayerFourPorts)
{
  constexpr unsigned nof_layers = 4;
  constexpr unsigned nof_ports  = 4;

  // Create resource grid.
  std::unique_ptr<resource_grid> grid = rg_factory->create(nof_ports, MAX_NSYMB_PER_SLOT, MAX_RB * NRE);

  for (unsigned i_beam = 0; i_beam != 8; ++i_beam) {
    for (unsigned i_pol_phase = 0; i_pol_phase != 2; ++i_pol_phase) {
      // Initialize grid to zero.
      grid->set_all_zero();

      // Create a single layer to two ports precoding configuration.
      precoding_configuration precoding_config =
          precoding_configuration::make_wideband(make_four_layer_four_ports_type1_sp(i_beam, i_pol_phase));

      // Generate resource grid allocation.
      re_pattern_list allocation = generate_random_allocation(MAX_RB);

      // Get the total number of RE in the allocation pattern.
      unsigned nof_data_re = allocation.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, ~bounded_bitset<MAX_RB>(MAX_RB));

      // Generate random RE arranged by layers.
      const re_buffer_reader& input_data = generate_random_data(nof_layers, nof_data_re);

      // Get the resource grid mapper.
      resource_grid_mapper& mapper = grid->get_mapper();

      // Map into the resource grid.
      mapper.map(input_data, allocation.get_re_patterns().front(), precoding_config);

      // Generate the golden precoded data.
      const re_buffer_reader& golden = generate_golden(input_data, allocation, precoding_config);

      // Assert resource grid contents.
      assert_grid(golden, allocation, grid->get_reader(), MAX_RB);
    }
  }
}

// Tests mapping with a precoding configuration with multiple PRG.
TEST_P(ResourceGridMapperFixture, MultiplePrg)
{
  const MultiplePRGParams& test_case = GetParam();

  unsigned nof_rb    = std::get<0>(test_case);
  unsigned nof_ports = std::get<1>(test_case);
  unsigned prg_size  = std::get<2>(test_case);

  // Create resource grid.
  std::unique_ptr<resource_grid> grid = rg_factory->create(nof_ports, MAX_NSYMB_PER_SLOT, nof_rb * NRE);

  // Number of subcarriers in an OFDM symbol.
  unsigned nof_subc = nof_rb * NRE;

  // Number of RE that fit in a PRG for an OFDM symbol.
  unsigned nof_re_prg = prg_size * NRE;

  for (unsigned nof_layers = 1; nof_layers <= nof_ports; ++nof_layers) {
    // Initialize grid to zero.
    grid->set_all_zero();

    // Number of PRG that the data spans.
    unsigned nof_prg = divide_ceil(nof_subc, nof_re_prg);

    // Create a random precoding configuration
    precoding_configuration precoding_config = generate_random_precoding(nof_layers, nof_ports, nof_prg, prg_size);

    // Generate resource grid allocation.
    re_pattern_list allocation = generate_random_allocation(nof_rb);

    // Get the total number of RE in the allocation pattern.
    unsigned nof_data_re = allocation.get_inclusion_count(0, MAX_NSYMB_PER_SLOT, ~bounded_bitset<MAX_RB>(MAX_RB));

    // Generate random RE arranged by layers.
    const re_buffer_reader& input_data = generate_random_data(nof_layers, nof_data_re);

    // Get the resource grid mapper.
    resource_grid_mapper& mapper = grid->get_mapper();

    // Map into the resource grid.
    mapper.map(input_data, allocation.get_re_patterns().front(), precoding_config);

    // Generate the golden precoded data.
    const re_buffer_reader& golden = generate_golden(input_data, allocation, precoding_config);

    // Assert resource grid contents.
    assert_grid(golden, allocation, grid->get_reader(), nof_rb);
  }
}

INSTANTIATE_TEST_SUITE_P(MultiplePrg,
                         ResourceGridMapperFixture,
                         ::testing::Combine(
                             // Number of RB.
                             ::testing::Values(13, 51, MAX_RB),
                             // Number of antenna ports.
                             ::testing::Values(1, 2, 4),
                             // PRG size in number of RB.
                             ::testing::Values(4, 13, 52, MAX_RB)));
} // namespace
